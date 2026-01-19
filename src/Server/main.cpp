#include "pch.h"
#include "main.h"

#include <map>

#include <iostream>
#include <sstream>
#include <iomanip>

#include "Network.h"

void SendAllPositions(ServerNetwork* network)
{
    for (auto* client : network->ListUser)
    {
        // On envoie la position de toutes les entités à ce client
        for (auto* entity : network->ListUser)
        {
            if (!entity || !entity->s_EntityData)
                continue;

            std::string buffer = "{UPDATE_POS;"
                + std::to_string(entity->s_userID) + ";"
                + std::to_string(entity->s_EntityData->PosX) + ";"
                + std::to_string(entity->s_EntityData->PosY) + ";"
                + std::to_string(entity->s_EntityData->PosZ) + ";}";

            sockaddr_in addr = client->s_networkInfo->Addr_User;
            int sizeAddr = sizeof(addr);

            int result = sendto(*network->GetSocket(),buffer.c_str(),static_cast<int>(buffer.size()),0,(sockaddr*)&addr,sizeAddr);

            if (result == SOCKET_ERROR)
            {
                int err = WSAGetLastError();
                std::cerr << "[SendAllPositions] sendto failed for user "<< client->s_userID << ": " << err << std::endl;
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
        EnterCriticalSection(&network->csMovedUsers);

        for (auto* currentUser : network->ListOfUserMoved)
        {
            SendAllPositions(network);
        }
        network->ListOfUserMoved.clear();

        LeaveCriticalSection(&network->csMovedUsers);

        Sleep(1);
    }
    network->CloseSocket(*network->GetSocket());
    return 0;
}