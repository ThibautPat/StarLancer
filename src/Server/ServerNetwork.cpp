#include "pch.h"
#include "Network.h"
#include "EntityBulletServer.h"
#include "SpaceShipMove_Calculator.h"

User* ServerNetwork::NewUser(sockaddr_in addr)
{
	User* newUser = new User();
    newUser->s_networkInfo = new ServerNetworkInfo();

    newUser->s_userID = ListEntity.size();

    ListEntity[newUser->s_userID] = new EntityServer();

    ListUser_Tread.push_back(newUser);

    newUser->s_networkInfo->Addr_User = addr;
    newUser->s_networkInfo->port = ntohs(addr.sin_port);
    inet_ntop(AF_INET, &addr.sin_addr, newUser->s_networkInfo->IP, INET_ADDRSTRLEN);

	return newUser;
}

void ServerNetwork::ParseurMessage(const char* buffer, User* user)
{
    const Header* head = reinterpret_cast<const Header*>(buffer);

    switch(head->type) {
        case MessageType::CONNECTION:
        {
            ConnexionMessage message;

            //if (strlen(buffer) < sizeof(ConnexionMessage))
            //    return;

            memcpy(&message, buffer, sizeof(ConnexionMessage));

            message.magicnumber = ntohl(message.magicnumber);
            if (message.magicnumber != 8542)
                return;
            
            ReturnConnexionMessage msg;
            msg.ClientID = htonl(user->s_userID);
            msg.head.type = MessageType::CONNECTION;

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

            SpaceShipMove_Calculator::Calcul_Forward(user,this);
            
            break;
        }
        case MessageType::BACKWARD:
        {
            const InputMessage* message = reinterpret_cast<const InputMessage*>(buffer);

            SpaceShipMove_Calculator::Calcul_Backward(user, this);
            
            break;
        }
        case MessageType::LEFT:
        {
            const InputMessage* message = reinterpret_cast<const InputMessage*>(buffer);

           
            SpaceShipMove_Calculator::Calcul_Left(user, this);
            
            break;
        }
        case MessageType::RIGHT:
        {
            const InputMessage* message = reinterpret_cast<const InputMessage*>(buffer);

           
            SpaceShipMove_Calculator::Calcul_Right(user, this);
            
            break;
        }
        case MessageType::ENTITY:
        {
            const AABBUpdateMessage* message = reinterpret_cast<const AABBUpdateMessage*>(buffer);
            ListEntity[user->s_userID]->maxAABB.x = message->maxX;
            ListEntity[user->s_userID]->maxAABB.y = message->maxY;
            ListEntity[user->s_userID]->maxAABB.z = message->maxZ;

            ListEntity[user->s_userID]->minAABB.x = message->minX;
            ListEntity[user->s_userID]->minAABB.y = message->minY;
            ListEntity[user->s_userID]->minAABB.z = message->minZ;

			break;
        }
        case MessageType::FIRE_BULLET:
        {
             
            SpawnEntity replicationMessage;
            replicationMessage.head.type = MessageType::ENTITY;
            replicationMessage.entity = EntityType::BULLET;
			replicationMessage.IDEntity = htonl(ListEntity.size());

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


        for (auto c : network->ListUser_Tread)
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

        EnterCriticalSection(&network->csNewUser);
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
    SpawnPlayer msg{};
    msg.head.type = MessageType::ENTITY;
    msg.entity = EntityType::SPACESHIP;
    msg.IDEntity = htonl(Recever->s_userID);

    sendto(*GetSocket(), reinterpret_cast<const char*>(&msg), sizeof(msg), 0, (sockaddr*)&Recever->s_networkInfo->Addr_User, sizeof(Recever->s_networkInfo->Addr_User));

    for (auto& u : ListUser_MainTread)
    {
        if (u == Recever)
            continue;

        SpawnPlayer oldMsg{};
        oldMsg.head.type = MessageType::ENTITY;
        oldMsg.entity = EntityType::SPACESHIP;
        oldMsg.IDEntity = htonl(u->s_userID); //  ID de l'ancien joueur
        sendto(*GetSocket(), reinterpret_cast<const char*>(&oldMsg), sizeof(oldMsg), 0, (sockaddr*)&Recever->s_networkInfo->Addr_User, sizeof(Recever->s_networkInfo->Addr_User));

        SpawnPlayer newMsg{};
        newMsg.head.type = MessageType::ENTITY;
        newMsg.entity = EntityType::SPACESHIP;
        newMsg.IDEntity = htonl(Recever->s_userID); //  ID du nouveau joueur
        sendto(*GetSocket(), reinterpret_cast<const char*>(&newMsg), sizeof(newMsg), 0, (sockaddr*)&u->s_networkInfo->Addr_User, sizeof(u->s_networkInfo->Addr_User));
    }
}


template<typename T>
void ServerNetwork::ReplicationMessage( char * test)
{
     T* msg = reinterpret_cast< T*>(test);
    msg->IDEntity = htonl(msg->IDEntity);

    for (auto& u : ListUser_MainTread)
    {

        sendto(*GetSocket(), reinterpret_cast<const char*>(msg), sizeof(msg), 0,(sockaddr*)&u->s_networkInfo->Addr_User,sizeof(u->s_networkInfo->Addr_User));
    }
}



