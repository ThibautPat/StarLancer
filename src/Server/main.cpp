#include "pch.h"
#include "main.h"

#include <map>

#include <iostream>
#include <sstream>
#include <iomanip>

#include "Network.h"

void MoveLoop(socket_t sock, sockaddr* Addr_User, int sizeAddr_User, EntityData* ball)
{
    //std::ostringstream oss;
    //oss << std::fixed << std::setprecision(2)<< '{' << ball->PosX << ';' << ball->PosY << ';' << ball->PosZ << '}';

    //std::string message = oss.str();

    //sendto(sock, message.c_str(), message.size(), 0, Addr_User, sizeAddr_User);

    //ball->PosZ += 0.02;

    //if (ball->PosZ >= 100)
    //    ball->PosZ = 0;
}

/* ======================= MAIN ======================= */

int main()
{
    ServerNetwork* network = new ServerNetwork();

    network->InitNetwork();

    std::cerr << "Server UP --------------------\n";

    while (true)
    {
        for (auto* currentUser : network->ListUser)
        {
            if (currentUser == nullptr)
                continue;
            MoveLoop(*network->GetSocket(), (sockaddr*)&currentUser->s_networkInfo->Addr_User, sizeof(currentUser->s_networkInfo->Addr_User), currentUser->s_EntityData);
        }
        network->ListUser = network->newListUser;
    }
    network->CloseSocket(*network->GetSocket());
    return 0;
}

