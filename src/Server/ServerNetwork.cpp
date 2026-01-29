#include "pch.h"
#include "Network.h"
#include "EntityBulletServer.h"
#include "SpaceShipMove_Calculator.h"
#include "EntityShipServer.h"

User* ServerNetwork::NewUser(sockaddr_in addr)
{
	User* newUser = new User();
    newUser->s_networkInfo = new ServerNetworkInfo();

    newUser->s_userID = IdIndex;
    IdIndex++;

    ListEntity[newUser->s_userID] = new EntityShipServer();
    ListEntity[newUser->s_userID]->entityType = EntityType::SPACESHIP;

    ListEntity[newUser->s_userID]->transform.Identity();
    ListEntity[newUser->s_userID]->entityID = newUser->s_userID;

    ListEntity[newUser->s_userID]->currentPitch = 0.f;
    ListEntity[newUser->s_userID]->currentYaw = 0.f;
    ListEntity[newUser->s_userID]->currentRoll= 0.f;

    ListUser_Tread.push_back(newUser);

    newUser->s_networkInfo->Addr_User = addr;
    newUser->s_networkInfo->port = ntohs(addr.sin_port);
    inet_ntop(AF_INET, &addr.sin_addr, newUser->s_networkInfo->IP, INET_ADDRSTRLEN);

	return newUser;
}

void ServerNetwork::ClearDeadEntity()
{
    std::vector<uint32_t> toDelete;

    for (auto& entity : ListEntity)
    {
        if (entity.second->IsDead)
        {
            toDelete.push_back(entity.first);
        }
    }

    for (uint32_t id : toDelete)
    {
        if(ListEntity[id]->entityType == EntityType::BULLET)
        {
            delete ListEntity[id];
            ListEntity.erase(id);
        }
    }
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

            strncpy_s(user->Pseudo, 32, message.pseudo, _TRUNCATE);

            ReturnConnexionMessage msg;
            msg.ClientID = htonl(user->s_userID);
            msg.head.type = MessageType::CONNECTION;

            sockaddr_in addr = user->s_networkInfo->Addr_User;
            int sizeAddr = sizeof(addr);

            int result = sendto(*GetSocket(), reinterpret_cast<const char*>(&msg), sizeof(ReturnConnexionMessage), 0, (sockaddr*)&addr, sizeAddr);

            BacklogSend(user, true);

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
            //CREATION STRUCT DONNEE
            EntityBulletServer* bullet = new EntityBulletServer();
            bullet->entityType = EntityType::BULLET;

            bullet->Owner = ListEntity[user->s_userID];
			bullet->ownerBULLET_FORWARD = ListEntity[user->s_userID]->transform.dir;

            bullet->entityID = IdIndex;
            ListEntity[IdIndex] = bullet;
            IdIndex++;

            bullet->transform.pos = ListEntity[user->s_userID]->transform.pos;

            //SEND DONNEE
            SpawnEntity replicationMessage{};
            replicationMessage.head.type = MessageType::ENTITY;
            replicationMessage.entity = EntityType::BULLET;
            replicationMessage.IDEntity = htonl(bullet->entityID);
            replicationMessage.IDUser = htonl(user->s_userID);

            ReplicationMessage<SpawnEntity>(reinterpret_cast<char*>(&replicationMessage));
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

void ServerNetwork::BacklogSend(User* Recever, bool toOld)
{
    // NEW PLAYER SHIP
    SpawnPlayer msg{};
    msg.head.type = MessageType::ENTITY;
    msg.entity = EntityType::SPACESHIP;
    msg.IDEntity = htonl(Recever->s_userID);
    strncpy_s(msg.pseudo, 32, Recever->Pseudo, _TRUNCATE);

    sendto(*GetSocket(), reinterpret_cast<const char*>(&msg), sizeof(msg), 0, (sockaddr*)&Recever->s_networkInfo->Addr_User, sizeof(Recever->s_networkInfo->Addr_User));

    // ANCIEN JOUEURS VERS LE NOUVEAU
    for (auto& u : ListUser_MainTread)
    {
        if (u->s_userID == Recever->s_userID)
            continue;

        SpawnPlayer msg{};
        msg.head.type = MessageType::ENTITY;
        msg.entity = EntityType::SPACESHIP;
        msg.IDEntity = htonl(u->s_userID);
        strncpy_s(msg.pseudo, 32, u->Pseudo, _TRUNCATE);

        sendto(*GetSocket(), reinterpret_cast<const char*>(&msg), sizeof(msg), 0, (sockaddr*)&Recever->s_networkInfo->Addr_User, sizeof(Recever->s_networkInfo->Addr_User));
    }

    if (toOld == false)
        return;

    // NOUVEAU JOUEURS VERS LES ANCIENS
    for (auto& u : ListUser_MainTread)
    {
        if (u->s_userID == Recever->s_userID) 
            continue;

        SpawnPlayer msg{};
        msg.head.type = MessageType::ENTITY;
        msg.entity = EntityType::SPACESHIP;
        msg.IDEntity = htonl(Recever->s_userID);
        strncpy_s(msg.pseudo, 32, Recever->Pseudo, _TRUNCATE);

        sendto(*GetSocket(), reinterpret_cast<const char*>(&msg), sizeof(msg), 0, (sockaddr*)&u->s_networkInfo->Addr_User, sizeof(u->s_networkInfo->Addr_User));
    }
}