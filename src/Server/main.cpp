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
            msg.entityID = entity->s_userID;
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
        // PHYSIQUE

        // ----- PARSE -----
        for (const auto& message : network->MessageBufferRecev)
        {
            network->ParseurMessage(message.first.data(), message.second);
        }
        network->MessageBufferRecev.clear();

        // ----- MERGE USER -----
        EnterCriticalSection(&network->csMovedUsers);
        network->ListUser_MainTread = network->ListUser_Tread;
        LeaveCriticalSection(&network->csMovedUsers);

        // ----- SEND NUKE -----
        //for (auto* currentUser : network->ListUser_MainTread)
        //    SendAllPositions(network);
        
        Sleep(10);
    }
    network->CloseSocket(*network->GetSocket());
    return 0;
}
