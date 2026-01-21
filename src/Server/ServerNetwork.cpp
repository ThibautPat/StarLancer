#include "pch.h"
#include "Network.h"

#include "SpaceShipMove_Calculator.h"

User* ServerNetwork::NewUser(sockaddr_in addr)
{
	User* newUser = new User();
    newUser->s_EntityData = new EntityData();
    newUser->s_networkInfo = new ServerNetworkInfo();

    newUser->s_userID = ListUser.size();
	ListUser.push_back(newUser);

    newUser->s_networkInfo->Addr_User = addr;
    newUser->s_networkInfo->port = ntohs(addr.sin_port);
    inet_ntop(AF_INET, &addr.sin_addr, newUser->s_networkInfo->IP, INET_ADDRSTRLEN);

	std::cout << "New User: " << newUser->s_networkInfo->IP << ":" << newUser->s_networkInfo->port << "\n";
	return newUser;
}

void ServerNetwork::ParseurMessage(User* user, const char* buffer)
{
    std::string msg(buffer);

    if (msg.front() == '{' && msg.back() == '}')
    {
        msg = msg.substr(1, msg.size() - 2);
    }
    else { return;}

    if (msg == "CONNEXION")
    {

    }
    else if (msg == "FORWARD")
    {
        SpaceShipMove_Calculator::Calcul_Forward(user);
        ListOfUserMoved.push_back(user);
    }
    else if (msg == "BACKWARD")
    {
        SpaceShipMove_Calculator::Calcul_Backward(user);
        ListOfUserMoved.push_back(user);
    }
    else if (msg == "LEFT")
    {
        SpaceShipMove_Calculator::Calcul_Left(user);
        ListOfUserMoved.push_back(user);
    }
    else if (msg == "RIGHT")
    {
        SpaceShipMove_Calculator::Calcul_Right(user);
        ListOfUserMoved.push_back(user);
    }
}

DWORD WINAPI ServerNetwork::ThreadFonction(LPVOID lpParam)
{
    ServerNetwork* network = static_cast<ServerNetwork*>(lpParam);

    sockaddr_in senderAddr{};
    int senderAddrSize = sizeof(senderAddr);

    char buffer[1024];

    while (network->IsRunning)
    {
        int received = recvfrom(*network->GetSocket(),buffer,sizeof(buffer) - 1,0,(sockaddr*)&senderAddr, &senderAddrSize);

        if (received == SOCKET_ERROR)
        {
            int err = WSAGetLastError();
            if (err == WSAEWOULDBLOCK)
                continue;
            break; 
        }

        buffer[received] = '\0';

        char ip_current[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &senderAddr.sin_addr, ip_current, INET_ADDRSTRLEN);

        User* user = nullptr;

        EnterCriticalSection(&network->csNewUser);

        for (auto c : network->ListUser)
        {
            if (strcmp(ip_current, c->s_networkInfo->IP) == 0)
            {
                user = c;
                user->s_networkInfo->Addr_User = senderAddr;
                user->s_networkInfo->port = senderAddr.sin_port;
                break;
            }
        }

        if (!user)
        {
            user = network->NewUser(senderAddr);
            network->BacklogSend(user);
        }

        LeaveCriticalSection(&network->csNewUser);

        network->ParseurMessage(user, buffer);
    }

    return 0;
}

void ServerNetwork::Thread_StartListening()
{
	HANDLE thread1;

	thread1 = CreateThread(NULL, 0, ServerNetwork::ThreadFonction, (LPVOID)this, 0, NULL);
	CloseHandle(thread1);
}

void ServerNetwork::BacklogSend(User* Recever)
{
    std::string buffer = "{ENTITY;SPACESHIP}";
    sendto(*GetSocket(), buffer.c_str(), buffer.size(), 0, (sockaddr*)&Recever->s_networkInfo->Addr_User, sizeof(Recever->s_networkInfo->Addr_User));

    for (int i = 0; i < ListUser.size(); ++i)
    {
        if (ListUser[i] == Recever)
            continue;

        //Create SpaceShip for each client -> the new client
        sendto(*GetSocket(), buffer.c_str(), buffer.size(), 0, (sockaddr*)&Recever->s_networkInfo->Addr_User, sizeof(Recever->s_networkInfo->Addr_User));
        
        //Create one SpaceShip for client already connect
        sendto(*GetSocket(), buffer.c_str(), buffer.size(), 0, (sockaddr*)&ListUser[i]->s_networkInfo->Addr_User, sizeof(ListUser[i]->s_networkInfo->Addr_User));
    }
}

