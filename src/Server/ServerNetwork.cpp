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
    ListEntity[newUser->s_userID]->transform.Identity();

    ListEntity[newUser->s_userID]->currentPitch = 0.f;
    ListEntity[newUser->s_userID]->currentYaw = 0.f;
    ListEntity[newUser->s_userID]->currentRoll= 0.f;

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
            SpawnEntity replicationMessage{};
            replicationMessage.head.type = MessageType::ENTITY;
            replicationMessage.entity = EntityType::BULLET;
            replicationMessage.IDEntity = htonl(ListEntity.size()); 
            replicationMessage.IDUser = htonl(user->s_userID);      

            ReplicationMessage<SpawnEntity>(reinterpret_cast<char*>(&replicationMessage));

            //CREATION STRUCT DONNEE
            EntityBulletServer* bullet = new EntityBulletServer();
            bullet->entityType = EntityType::BULLET;
            bullet->Owner = ListEntity[user->s_userID];
			bullet->ownerBULLET_FORWARD = ListEntity[user->s_userID]->transform.dir;
            ListEntity[ListEntity.size()] = bullet;

            bullet->transform.pos = ListEntity[user->s_userID]->transform.pos;
            
            break;
        }
        case MessageType::MOUSE:
        {
            const MouseMessage* message = reinterpret_cast<const MouseMessage*>(buffer);
            uint32_t MyID = message->ClientID;
            EntityServer* entity = ListEntity[MyID];

            float dirX = message->X;
            float dirY = message->Y;

            // Paramètres
            float maxPitchAngle = XM_PIDIV4;     
            float yawSpeed = 0.05f;              
            float pitchSmoothFactor = 0.10f;     

            entity->currentYaw += dirX * yawSpeed;

            float targetPitch = -dirY * maxPitchAngle;
            entity->currentPitch += (targetPitch - entity->currentPitch) * pitchSmoothFactor;

            entity->currentRoll = 0.0f;

            UpdateRot rotMsg{};
            rotMsg.head.type = MessageType::UPDATE_ROT;
            rotMsg.entityID = htonl(MyID);
            rotMsg.Yaw = entity->currentYaw;     
            rotMsg.Pitch = entity->currentPitch; 
            rotMsg.Roll = entity->currentRoll;

            entity->transform.SetYPR(entity->currentYaw, entity->currentPitch, entity->currentRoll);
            entity->transform.UpdateWorld();

            ReplicationMessage<UpdateRot>(reinterpret_cast<char*>(&rotMsg));

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
        senderAddrSize = sizeof(senderAddr);
        int received = recvfrom(*network->GetSocket(), buffer, sizeof(buffer) - 1, 0, (sockaddr*)&senderAddr, &senderAddrSize);

        if (received == SOCKET_ERROR)
        {
            int err = WSAGetLastError();
            if (err == WSAEWOULDBLOCK || err == WSAETIMEDOUT)
                continue;

            std::cerr << "recvfrom error: " << err << std::endl;
            continue;
        }

        if (received <= 0)
            continue;

        char ip_current[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &senderAddr.sin_addr, ip_current, INET_ADDRSTRLEN);

        User* user = nullptr;

        for (auto c : network->ListUser_Tread)
        {
            if (strcmp(ip_current, c->s_networkInfo->IP) == 0)
            {
                user = c;
                c->s_networkInfo->Addr_User = senderAddr;
                c->s_networkInfo->port = senderAddr.sin_port;
                break;
            }
        }

        if (!user)
        {
            user = network->NewUser(senderAddr);

            network->ListUser_Tread.push_back(user);
        }

        EnterCriticalSection(&network->csNewUser);
        network->MessageBufferRecev.emplace(std::vector<char>(buffer, buffer + received), user);
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
        oldMsg.IDEntity = htonl(u->s_userID);
        sendto(*GetSocket(), reinterpret_cast<const char*>(&oldMsg), sizeof(oldMsg), 0, (sockaddr*)&Recever->s_networkInfo->Addr_User, sizeof(Recever->s_networkInfo->Addr_User));

        SpawnPlayer newMsg{};
        newMsg.head.type = MessageType::ENTITY;
        newMsg.entity = EntityType::SPACESHIP;
        newMsg.IDEntity = htonl(Recever->s_userID);
        sendto(*GetSocket(), reinterpret_cast<const char*>(&newMsg), sizeof(newMsg), 0, (sockaddr*)&u->s_networkInfo->Addr_User, sizeof(u->s_networkInfo->Addr_User));
    }
}