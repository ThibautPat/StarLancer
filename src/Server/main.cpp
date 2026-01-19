#include "pch.h"
#include "main.h"

#include <map>

#include <iostream>
#include <sstream>
#include <iomanip>

#include "Network.h"

void Send(socket_t* sock, sockaddr* Addr_User, int sizeAddr_User, EntityData* entity)
{
    std::string buffer = "{UPDATE_POS;0;" + std::to_string(entity->PosX) + ";" + std::to_string(entity->PosY) + ";" + std::to_string(entity->PosZ) + ";}";

    int result = sendto(*sock,buffer.c_str(),buffer.size(),0,Addr_User,sizeAddr_User);
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

        for (auto* currentUser : network->ListUser)
        {
            Send(network->GetSocket(), (sockaddr*)&currentUser->s_networkInfo->Addr_User, sizeof(currentUser->s_networkInfo->Addr_User),currentUser->s_EntityData);
        }
        network->ListOfUserMoved.clear();

        LeaveCriticalSection(&network->csMovedUsers);

        Sleep(1);
    }
    network->CloseSocket(*network->GetSocket());
    return 0;
}