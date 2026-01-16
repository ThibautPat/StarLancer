#include "pch.h"
#include "main.h"

#include <map>

#include <iostream>
#include <sstream>
#include <iomanip>

#include "Network.h"

void MoveLoop(socket_t sock, sockaddr* Addr_User, int sizeAddr_User, EntityData* ball)
{
    char buffer[100] = "{UPDATE_POS;0;0.0;0.0;5.0}";

    sendto(sock, buffer, sizeof(buffer), 0, (SOCKADDR*)&Addr_User, sizeof(Addr_User)) == SOCKET_ERROR;
    std::cout << "SEND\n";

    ball->PosZ += 0.02;

    if (ball->PosZ >= 100)
        ball->PosZ = 0;

    Sleep(500);
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

