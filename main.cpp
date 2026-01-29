#include "pch.h"
#include "main.h"
#include "Utils.h"

#include <map>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>

#ifdef _WIN32
    #include <windows.h>
    #define SLEEP_MS(x) Sleep(x)
#else
    #include <unistd.h>
    #define SLEEP_MS(x) usleep((x) * 1000)
#endif

#include "Network.h"
#include "EntityBulletServer.h"
#include "EntityShipServer.h"

void SendAllPositions(ServerNetwork* network)
{
    for (auto* client : network->ListUser_MainTread)
    {
        for (auto& entity : network->ListEntity)
        {
            if (entity.second->IsDead == true || entity.second->entityType == EntityType::BULLET)
                continue;

            UpdatePos msg;
            msg.head.type = MessageType::UPDATE_POS;
            msg.entityID = htonl(entity.first);
            msg.PosX = network->ListEntity[entity.first]->transform.pos.x;
            msg.PosY = network->ListEntity[entity.first]->transform.pos.y;
            msg.PosZ = network->ListEntity[entity.first]->transform.pos.z;
            sockaddr_in addr = client->s_networkInfo->Addr_User;
            socklen_t sizeAddr = sizeof(addr);
            int result = sendto(*network->GetSocket(), reinterpret_cast<const char*>(&msg), sizeof(UpdatePos), 0, (sockaddr*)&addr, sizeAddr);
            if (result == SOCKET_ERROR)
            {
                #ifdef _WIN32
                int err = WSAGetLastError();
                #else
                int err = errno;
                #endif
                std::cerr << "[SendAllPositions] sendto failed for user " << client->s_userID << " : " << err << std::endl;
            }
        }
    }
}

void CollisionCheck(ServerNetwork* network)
{
    static std::vector<std::pair<uint32_t, EntityServer*>> aliveEntities;
    aliveEntities.clear();
    aliveEntities.reserve(network->ListEntity.size());
    
    for (auto& entity : network->ListEntity)
    {
        if (!entity.second->IsDead)
            aliveEntities.push_back({entity.first, entity.second});
    }

    size_t count = aliveEntities.size();
    
    for (size_t i = 0; i < count; ++i)
    {
        EntityServer* e1 = aliveEntities[i].second;
        uint32_t id1 = aliveEntities[i].first;

        cpu_aabb aabb1;
        aabb1.min.x = e1->minAABB.x + e1->transform.pos.x;
        aabb1.min.y = e1->minAABB.y + e1->transform.pos.y;
        aabb1.min.z = e1->minAABB.z + e1->transform.pos.z;
        aabb1.max.x = e1->maxAABB.x + e1->transform.pos.x;
        aabb1.max.y = e1->maxAABB.y + e1->transform.pos.y;
        aabb1.max.z = e1->maxAABB.z + e1->transform.pos.z;

        for (size_t j = i + 1; j < count; ++j)
        {
            EntityServer* e2 = aliveEntities[j].second;
            uint32_t id2 = aliveEntities[j].first;

            bool skipCollision = false;
            if (e1->entityType == EntityType::BULLET)
            {
                EntityBulletServer* bullet = static_cast<EntityBulletServer*>(e1);
                if (bullet->Owner && bullet->Owner->entityID == id2)
                    skipCollision = true;
            }
            if (e2->entityType == EntityType::BULLET)
            {
                EntityBulletServer* bullet = static_cast<EntityBulletServer*>(e2);
                if (bullet->Owner && bullet->Owner->entityID == id1)
                    skipCollision = true;
            }

            if(e1->entityType == EntityType::BULLET && e2->entityType == EntityType::BULLET)
                skipCollision = true;

            if (skipCollision)
                continue;

            cpu_aabb aabb2;
            aabb2.min.x = e2->minAABB.x + e2->transform.pos.x;
            aabb2.min.y = e2->minAABB.y + e2->transform.pos.y;
            aabb2.min.z = e2->minAABB.z + e2->transform.pos.z;
            aabb2.max.x = e2->maxAABB.x + e2->transform.pos.x;
            aabb2.max.y = e2->maxAABB.y + e2->transform.pos.y;
            aabb2.max.z = e2->maxAABB.z + e2->transform.pos.z;
            
            if (cpu::AabbAabb(aabb1, aabb2))
            {
                e1->OnCollide(e2);
                e2->OnCollide(e1);

                if (e1->entityType == EntityType::BULLET && e2->entityType == EntityType::SPACESHIP)
                {
                    BulletHitMessage msg{};
                    msg.head.type = MessageType::HIT;
                    msg.bulletID = htonl(id1);
                    msg.targetID = htonl(id2);
                    msg.targetLife = dynamic_cast<EntityShipServer*>(e2)->life;
                    network->ReplicationMessage<BulletHitMessage>(reinterpret_cast<char*>(&msg));
                }
                if (e2->entityType == EntityType::BULLET && e1->entityType == EntityType::SPACESHIP)
                {
                    BulletHitMessage msg{};
                    msg.head.type = MessageType::HIT;
                    msg.bulletID = htonl(id2);
                    msg.targetID = htonl(id1);
                    msg.targetLife = dynamic_cast<EntityShipServer*>(e1)->life;
                    network->ReplicationMessage<BulletHitMessage>(reinterpret_cast<char*>(&msg));
                }
            }
        }
    }
}

/* ======================= MAIN ======================= */

int main()
{
    ServerNetwork* network = new ServerNetwork();
    network->InitNetwork();
    network->Thread_StartListening();

    std::cerr << "Server UP --------------------\n";

    const float TARGET_FPS = 60.0f;
    const float FRAME_TIME = 1.0f / TARGET_FPS;
    const std::chrono::duration<float> TARGET_FRAME_DURATION(FRAME_TIME);

    auto lastFrameTime = std::chrono::high_resolution_clock::now();
    float deltaTime = 0.0f;

    while (true)
    {
        auto frameStart = std::chrono::high_resolution_clock::now();

        std::chrono::duration<float> elapsed = frameStart - lastFrameTime;
        deltaTime = elapsed.count();
        lastFrameTime = frameStart;
        
        for (auto entity : network->ListEntity)
        {
            entity.second->Update(deltaTime);
            CollisionCheck(network);

            if (entity.second->entityType != EntityType::SPACESHIP)
                continue;

            EntityShipServer* ship = dynamic_cast<EntityShipServer*>(entity.second);

            if (ship->NeedToRespawn == true)
            {
                ship->NeedToRespawn = false;
                ship->IsDead = false;
                ship->life = 50;

                ship->transform.pos.x = randomBetweenMinus25And25();
                ship->transform.pos.y = randomBetweenMinus25And25();
                ship->transform.pos.z = randomBetweenMinus25And25();

                RespawnEntity msg{};
                msg.head.type = MessageType::RESPAWN;
                msg.targetID = ship->entityID;
                msg.targetLife = 100;
                
                MessageScore Score1{}; //LOSER
                Score1.head.type = MessageType::DATA;
                Score1.targetID = ship->entityID;
                network->GetUserMain(ship->entityID)->Death++;
                Score1.Death = network->GetUserMain(ship->entityID)->Death;
                Score1.Kill = network->GetUserMain(ship->entityID)->Kill;
                
                MessageScore Score2{}; //WINNER
                Score2.head.type = MessageType::DATA;
                if (ship->LastKiller)
                {
                    Score2.targetID = ship->LastKiller->entityID;
                    Score2.Death = network->GetUserMain(ship->LastKiller->entityID)->Death;

                    network->GetUserMain(ship->LastKiller->entityID)->Kill++;
                    Score2.Kill = network->GetUserMain(ship->LastKiller->entityID)->Kill;
                }

                network->ReplicationMessage<RespawnEntity>(reinterpret_cast<char*>(&msg));

                network->ReplicationMessage<MessageScore>(reinterpret_cast<char*>(&Score1));
                if (ship->LastKiller)
                    network->ReplicationMessage<MessageScore>(reinterpret_cast<char*>(&Score2));
            }
        }

        // PARSE
        for (const auto& message : network->MessageBufferRecev)
            network->ParseurMessage(message.first.data(), message.second);

        {
            std::lock_guard<std::mutex> lock(network->mtxNewUser);
            network->MessageBufferRecev.clear();
        }

        // MERGE USER
        {
            std::lock_guard<std::mutex> lock(network->mtxMovedUsers);
            network->ListUser_MainTread = network->ListUser_Tread;
        }

        network->ClearDeadEntity();

        // SEND NUKE 
        SendAllPositions(network);

        auto frameEnd = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> frameDuration = frameEnd - frameStart;

        if (frameDuration < TARGET_FRAME_DURATION)
        {
            auto sleepDuration = TARGET_FRAME_DURATION - frameDuration;
            SLEEP_MS(static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(sleepDuration).count()));
        }
    }

    network->CloseSocket(*network->GetSocket());
    delete network;
    return 0;
}
