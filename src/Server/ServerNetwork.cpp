#include "pch.h"
#include "Network.h"

#include "SpaceShipMove_Calculator.h"

User* ServerNetwork::NewUser(sockaddr_in addr)
{
	User* newUser = new User();
    newUser->s_EntityData = new EntityData();
    newUser->s_networkInfo = new ServerNetworkInfo();

    newUser->s_userID = ListUser_Tread.size();
    ListUser_Tread.push_back(newUser);

    newUser->s_networkInfo->Addr_User = addr;
    newUser->s_networkInfo->port = ntohs(addr.sin_port);
    inet_ntop(AF_INET, &addr.sin_addr, newUser->s_networkInfo->IP, INET_ADDRSTRLEN);

	std::cout << "New User: " << newUser->s_networkInfo->IP << ":" << newUser->s_networkInfo->port << "\n";
	return newUser;
}

void ServerNetwork::ParseurMessage(const char* buffer, User* user)
{
    const Header* head = reinterpret_cast<const Header*>(buffer);

    switch(head->type) {
        case MessageType::CONNEXION:
        {
            ConnexionMessage message;

            //if (strlen(buffer) < sizeof(ConnexionMessage))
            //    return;

            memcpy(&message, buffer, sizeof(ConnexionMessage));

            message.magicnumber = ntohl(message.magicnumber);
            if (message.magicnumber != 8542)
                return;
            
            ReturnConnexionMessage msg;
            msg.ClientID = htonl(static_cast<uint32_t>(ListUser_MainTread.size()));
            msg.head.type = MessageType::CONNEXION;

            sockaddr_in addr = user->s_networkInfo->Addr_User;
            int sizeAddr = sizeof(addr);

            int result = sendto(*GetSocket(), reinterpret_cast<const char*>(&msg), sizeof(ReturnConnexionMessage), 0, (sockaddr*)&addr, sizeAddr);

            BacklogSend(user);

            if (result == SOCKET_ERROR)
                int err = WSAGetLastError();

            break;
        }
        case MessageType::FORWARD:
        {
            const InputMessage* message = reinterpret_cast<const InputMessage*>(buffer);

            for(int i = 0; i < ListUser_MainTread.size(); i++)
            {
                if(message->ClientID == ListUser_MainTread[i]->s_userID)
                     SpaceShipMove_Calculator::Calcul_Forward(ListUser_MainTread[i]);
            }
            break;
        }
        case MessageType::BACKWARD:
        {
            const InputMessage* message = reinterpret_cast<const InputMessage*>(buffer);

            for (int i = 0; i < ListUser_MainTread.size(); i++)
            {
                if (message->ClientID == ListUser_MainTread[i]->s_userID)
                    SpaceShipMove_Calculator::Calcul_Backward(ListUser_MainTread[i]);
            }
            break;
        }
        case MessageType::LEFT:
        {
            const InputMessage* message = reinterpret_cast<const InputMessage*>(buffer);

            for (int i = 0; i < ListUser_MainTread.size(); i++)
            {
                if (message->ClientID == ListUser_MainTread[i]->s_userID)
                    SpaceShipMove_Calculator::Calcul_Left(ListUser_MainTread[i]);
            }
            break;
        }
        case MessageType::RIGHT:
        {
            const InputMessage* message = reinterpret_cast<const InputMessage*>(buffer);

            for (int i = 0; i < ListUser_MainTread.size(); i++)
            {
                if (message->ClientID == ListUser_MainTread[i]->s_userID)
                    SpaceShipMove_Calculator::Calcul_Right(ListUser_MainTread[i]);
            }
            break;
        }
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
        int received = recvfrom(*network->GetSocket(), buffer, sizeof(buffer) - 1, 0, (sockaddr*)&senderAddr, &senderAddrSize);

        if (received == SOCKET_ERROR)
        {
            int err = WSAGetLastError();
            if (err == WSAEWOULDBLOCK)
                continue;
            break;
        }

        if (received <= 0)
        {
            continue;
        }

        char ip_current[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &senderAddr.sin_addr, ip_current, INET_ADDRSTRLEN);

        User* user = nullptr;

        EnterCriticalSection(&network->csNewUser);

        for (auto c : network->ListUser_MainTread)
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
        }

        network->MessageBufferRecev.emplace( std::vector<char>(buffer, buffer + received), user); // ADD MESSAGE TO BUFFER

        LeaveCriticalSection(&network->csNewUser);
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
    SpawnEntity msg;
    msg.head.type = MessageType::ENTITY;
    char name[32] = "SPACESHIP";
    msg.entity = EntityType::SPACESHIP;

    sendto(*GetSocket(), reinterpret_cast<const char*>(&msg), sizeof(SpawnEntity), 0, (sockaddr*)&Recever->s_networkInfo->Addr_User, sizeof(Recever->s_networkInfo->Addr_User));

    //for (int i = 0; i < ListUser.size(); ++i)
    //{
    //    if (ListUser[i] == Recever)
    //        continue;

    //    //Create SpaceShip for each client -> the new client
    //    sendto(*GetSocket(), buffer.c_str(), buffer.size(), 0, (sockaddr*)&Recever->s_networkInfo->Addr_User, sizeof(Recever->s_networkInfo->Addr_User));
    //    
    //    //Create one SpaceShip for client already connect
    //    sendto(*GetSocket(), buffer.c_str(), buffer.size(), 0, (sockaddr*)&ListUser[i]->s_networkInfo->Addr_User, sizeof(ListUser[i]->s_networkInfo->Addr_User));
    //}
}

