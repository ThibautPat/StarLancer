#include "pch.h"
#include "Network.h"

void ClientNetwork::Thread_StartListening()
{
	HANDLE thread1;

	thread1 = CreateThread(NULL, 0, ClientNetwork::ThreadFonction, (LPVOID)this, 0, NULL);
	CloseHandle(thread1);
}

DWORD WINAPI ClientNetwork::ThreadFonction(LPVOID lpParam)
{
    char buffer[1024];
    ClientNetwork* network = static_cast<ClientNetwork*>(lpParam);

    while (true)
    {
        sockaddr_in SenderAddr;
        int SenderAddrSize = sizeof(SenderAddr);

        int received = recvfrom(*network->GetSocket(),buffer, sizeof(buffer), 0,(sockaddr*)&SenderAddr, &SenderAddrSize);

        if (received == SOCKET_ERROR)
            continue;

        if (received > 0)
        {
            EnterCriticalSection(&network->csMessageBuffer);
            network->MessageBuffer.emplace_back(buffer, buffer + received);
            LeaveCriticalSection(&network->csMessageBuffer);
        }
    }
    return 0;
}

void ClientNetwork::ParseurMessage()
{
    std::vector<std::vector<char>> messagesToProcess;

    // swap thread-safe avec le buffer principal
    EnterCriticalSection(&csMessageBuffer);
    messagesToProcess.swap(MessageBuffer);
    LeaveCriticalSection(&csMessageBuffer);

    // traitement sans verrou
    for (auto& msg : messagesToProcess)
    {
        if (msg.size() < sizeof(Header))
        {
            std::cerr << "[Warning] Message trop petit" << std::endl;
            continue;
        }

        const Header* head = reinterpret_cast<const Header*>(msg.data());

        switch (head->type)
        {
        case MessageType::CONNECTION:
        {
            if (msg.size() < sizeof(ReturnConnexionMessage)) break;
            const ReturnConnexionMessage* message = reinterpret_cast<const ReturnConnexionMessage*>(msg.data());
            MyIDClient = ntohl(message->ClientID);
            Connected = true;

            break;
        }

        case MessageType::UPDATE_POS:
        {
            if (msg.size() < sizeof(UpdatePos)) break;
            const UpdatePos* message = reinterpret_cast<const UpdatePos*>(msg.data());
            App& instance = App::GetInstance();

            uint32_t entityID = ntohl(message->entityID);

            EnterCriticalSection(&instance.m_cs);

            EntityClient* Entity = instance.GetEntitie(entityID);
            if (Entity != nullptr)
            {
                cpu_entity* entity = Entity->pEntity;
                instance.UpdateEntityPosition(entity, message->PosX, message->PosY, message->PosZ);
            }
            else
            {
                std::cout << "Warning: Entity " << entityID << " not found for UPDATE_POS" << std::endl;
            }

            LeaveCriticalSection(&instance.m_cs);
            break;
        }

        case MessageType::UPDATE_ROT:
        {
            if (msg.size() < sizeof(UpdateRot)) break;
            const UpdateRot* message = reinterpret_cast<const UpdateRot*>(msg.data());
            App& instance = App::GetInstance();

            uint32_t entityID = ntohl(message->entityID);

            EnterCriticalSection(&instance.m_cs);

            EntityClient* Entity = instance.GetEntitie(entityID);
            if (Entity != nullptr)
            {
                cpu_entity* entity = Entity->pEntity;
                instance.UpdateEntityRotation(entity, message->Yaw, message->Pitch, message->Roll);
            }

            LeaveCriticalSection(&instance.m_cs);
            break;
        }

        case MessageType::HIT:
        {
            if (msg.size() < sizeof(BulletHitMessage)) break;
            const BulletHitMessage* message = reinterpret_cast<const BulletHitMessage*>(msg.data());
            App& instance = App::GetInstance();

            EnterCriticalSection(&instance.m_cs);

            EntityClient* bullet = instance.GetEntitie(ntohl(message->bulletID));
            if(bullet)
                bullet->ToDestroy = true;

            EntityClient* target = instance.GetEntitie(ntohl(message->targetID));
            if (target) 
                target->life = message->targetLife;

            LeaveCriticalSection(&instance.m_cs);
            break;
        }

        case MessageType::RESPAWN:
        {
            if (msg.size() < sizeof(RespawnEntity)) break;
            const RespawnEntity* message = reinterpret_cast<const RespawnEntity*>(msg.data());
            App& instance = App::GetInstance();

            EnterCriticalSection(&instance.m_cs);

            EntityClient* target = instance.GetEntitie(message->targetID);
            if (target) 
                target->Respawn(message->targetLife);

            LeaveCriticalSection(&instance.m_cs);
            break;
        }

        case MessageType::ENTITY:
        {
            if (msg.size() < sizeof(SpawnEntity)) 
                break;
            const SpawnEntity* message = reinterpret_cast<const SpawnEntity*>(msg.data());

            App& instance = App::GetInstance();

            switch (message->entity)
            {
            case EntityType::SPACESHIP:
            {

                const SpawnPlayer* PlayerMessage = reinterpret_cast<const SpawnPlayer*>(msg.data());


                uint32_t entityID = ntohl(message->IDEntity);
                EnterCriticalSection(&instance.m_cs);

                strncpy_s(m_pseudos[entityID], 32, PlayerMessage->pseudo, _TRUNCATE);


                EntityClient* entityClient = new EntityClient();
                entityClient->pEntity = cpuEngine.CreateEntity();
                cpu_mesh* m_meshShip = new cpu_mesh();
                m_meshShip->LoadOBJ("../../res/3D_model/SpaceShip.obj", { 1,1,1 }, false);

                entityClient->pEntity->pMaterial = new cpu_material();
                entityClient->pEntity->pMaterial->color = { 1.0f,1.0f,1.0f };
                entityClient->pEntity->pMesh = m_meshShip;
                entityClient->entityID = entityID;

                m_meshShip->FlipWinding();
                m_meshShip->Optimize();

                instance.GetEntitiesList()[entityClient->entityID] = entityClient;

                AABBUpdateMessage AabbMessage{};
                AabbMessage.head.type = MessageType::ENTITY;
                AabbMessage.IDEntity = htonl(entityID);
                AabbMessage.minX = m_meshShip->aabb.min.x;
                AabbMessage.minY = m_meshShip->aabb.min.y;
                AabbMessage.minZ = m_meshShip->aabb.min.z;
                AabbMessage.maxX = m_meshShip->aabb.max.x;
                AabbMessage.maxY = m_meshShip->aabb.max.y;
                AabbMessage.maxZ = m_meshShip->aabb.max.z;

                SendMessageToServer(reinterpret_cast<const char*>(&AabbMessage), sizeof(AABBUpdateMessage));

                // A BOUGER 

                DataPlayer* info = new DataPlayer();

                PlayerInfoList.push_back(info);

                // -----------------------------------

                LeaveCriticalSection(&instance.m_cs);
                break;
            }

            case EntityType::BULLET:
            {
                uint32_t bulletID = ntohl(message->IDEntity);
                uint32_t userID = ntohl(message->IDUser);
                instance.CreateBullet(bulletID, userID);
                break;
            }
            }
            break;
        }
        } // switch
    } // for
}


void ClientNetwork::SendMessageToServer(const char* message, size_t size)
{
    if (sendto(*GetSocket(), message, size, 0, (SOCKADDR*)&ServeurAddr, sizeof(ServeurAddr)) == SOCKET_ERROR)
        std::cout << "Error";
}

void ClientNetwork::ConnexionProtcol()
{
    ConnexionMessage msg{};
    msg.head.type = MessageType::CONNECTION;
    msg.magicnumber = htonl(8542);
    strncpy_s(msg.pseudo, 32, MyPseudo, _TRUNCATE);

    SendMessageToServer(reinterpret_cast<const char*>(&msg), sizeof(ConnexionMessage));
}

void ClientNetwork::ChoseTarget(const char* ip)
{
    if (inet_pton(AF_INET, ip, &ServeurAddr.sin_addr) <= 0) //MOI
        return;
    App::GetInstance().connected = true;
}

// 169.254.158.79