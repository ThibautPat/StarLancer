#include "pch.h"
#include "main.h"

#include <map>

#include <iostream>
#include <sstream>
#include <iomanip>

#include "Network.h"

void SendAllPositions(ServerNetwork* network) // DOOOM
{
    for (auto* client : network->ListUser_MainTread)
    {
        for (auto& entity : network->ListEntity)
        {
            UpdatePos msg;
            msg.head.type = MessageType::UPDATE_POS;
            msg.entityID = htonl(entity.first);
            msg.PosX = network->ListEntity[entity.first]->PosX;
            msg.PosY = network->ListEntity[entity.first]->PosY;
            msg.PosZ = network->ListEntity[entity.first]->PosZ;

            sockaddr_in addr = client->s_networkInfo->Addr_User;
            int sizeAddr = sizeof(addr);

            int result = sendto(*network->GetSocket(),reinterpret_cast<const char*>(&msg), sizeof(UpdatePos), 0, (sockaddr*)&addr, sizeAddr);

            if (result == SOCKET_ERROR)
            {
                int err = WSAGetLastError();
                std::cerr << "[SendAllPositions] sendto failed for user "<< client->s_userID << " : " << err << std::endl;
            }
        }
    }


}


void CollisionCheck(ServerNetwork* network)
{
    for (auto& entity : network->ListEntity)
    {
        for (auto& entity1 : network->ListEntity)
        {
            if (entity == entity1)
                continue;

            cpu_aabb aabb1;
            aabb1.min.x = entity.second->minAABB.x +entity.second->PosX;
            aabb1.min.y = entity.second->minAABB.y +entity.second->PosY;
            aabb1.min.z = entity.second->minAABB.z +entity.second->PosZ;

            aabb1.max.x = entity.second->maxAABB.x + entity.second->PosX;
            aabb1.max.y = entity.second->maxAABB.y + entity.second->PosY;
            aabb1.max.z = entity.second->maxAABB.z + entity.second->PosZ;

            cpu_aabb aabb2;
            aabb2.min.x = entity1.second->minAABB.x + entity1.second->PosX;
            aabb2.min.y = entity1.second->minAABB.y + entity1.second->PosY;
            aabb2.min.z = entity1.second->minAABB.z + entity1.second->PosZ;

            aabb2.max.x =entity1.second->maxAABB.x + entity1.second->PosX;
            aabb2.max.y =entity1.second->maxAABB.y + entity1.second->PosY;
            aabb2.max.z =entity1.second->maxAABB.z + entity1.second->PosZ;

            if (cpu::AabbAabb(aabb1, aabb2))
            {
                entity.second->OnCollide(entity1.second);
                entity1.second->OnCollide(entity.second);

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

    while (true)
    {
        for (auto& entity : network->ListBullet)
        {
            
            entity.second->PosZ -= 0.5f; 
			
        }


        // PARSE
        for (const auto& message : network->MessageBufferRecev)
            network->ParseurMessage(message.first.data(), message.second);
        network->MessageBufferRecev.clear();

        // MERGE USER
        EnterCriticalSection(&network->csMovedUsers);
        network->ListUser_MainTread = network->ListUser_Tread;
        LeaveCriticalSection(&network->csMovedUsers);
        // SEND NUKE 
        SendAllPositions(network);

        CollisionCheck(network);



        Sleep(32);
    }
    network->CloseSocket(*network->GetSocket());
    return 0;
}
