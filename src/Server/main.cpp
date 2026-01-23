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
        for (auto* entity : network->ListUser_MainTread)
        {
            if (!entity || !entity->s_EntityData)
                continue;

            UpdatePos msg;
            msg.head.type = MessageType::UPDATE_POS;
            msg.entityID = htonl(entity->s_userID);
            msg.PosX = entity->s_EntityData->PosX;
            msg.PosY = entity->s_EntityData->PosY;
            msg.PosZ = entity->s_EntityData->PosZ;

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

/* ======================= MAIN ======================= */

int main()
{
    ServerNetwork* network = new ServerNetwork();

    network->InitNetwork();

    network->Thread_StartListening();

    std::cerr << "Server UP --------------------\n";

    while (true)
    {

       

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

        for (User* user : network->ListUser_MainTread)
        {
            for (User* user1 : network->ListUser_MainTread)
            {
                if (user1 == user)
                    continue;

                cpu_aabb aabb1;
                aabb1.min.x = user->s_EntityData->minAABB.x + user->s_EntityData->PosX;
                aabb1.min.y = user->s_EntityData->minAABB.y + user->s_EntityData->PosY;
                aabb1.min.z = user->s_EntityData->minAABB.z + user->s_EntityData->PosZ;

                aabb1.max.x = user->s_EntityData->maxAABB.x + user->s_EntityData->PosX;
                aabb1.max.y = user->s_EntityData->maxAABB.y + user->s_EntityData->PosY;
                aabb1.max.z = user->s_EntityData->maxAABB.z + user->s_EntityData->PosZ;

                cpu_aabb aabb2;
                aabb2.min.x = user1->s_EntityData->minAABB.x + user1->s_EntityData->PosX;
                aabb2.min.y = user1->s_EntityData->minAABB.y + user1->s_EntityData->PosY;
                aabb2.min.z = user1->s_EntityData->minAABB.z + user1->s_EntityData->PosZ;
                
                aabb2.max.x = user1->s_EntityData->maxAABB.x + user1->s_EntityData->PosX;
                aabb2.max.y = user1->s_EntityData->maxAABB.y + user1->s_EntityData->PosY;
                aabb2.max.z = user1->s_EntityData->maxAABB.z + user1->s_EntityData->PosZ;

                if (cpu::AabbAabb(aabb1, aabb2))
                {
                    exit(0);
                }
            }
        }


        Sleep(10);
    }
    network->CloseSocket(*network->GetSocket());
    return 0;
}
