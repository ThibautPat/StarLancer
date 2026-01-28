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
            network->MessageBuffer.emplace_back(buffer, buffer + received);
        }
    }
    return 0;
}

void ClientNetwork::ParseurMessage()
{
    for (const auto& message : MessageBuffer)
    {
        const char* buffer = message.data();

        const Header* head = reinterpret_cast<const Header*>(buffer);
        if (!head || head == nullptr)
            return;

        switch (head->type)
        {
            case MessageType::CONNECTION:
            {
                const ReturnConnexionMessage* message = reinterpret_cast<const ReturnConnexionMessage*>(buffer);
                MyIDClient = ntohl(message->ClientID);
				Connected = true; // Successfully connected to the server
                break;
            }

            case MessageType::UPDATE_POS:
            {
                const UpdatePos* message = reinterpret_cast<const UpdatePos*>(buffer);
                App& instance = App::GetInstance();

                uint32_t entityID = ntohl(message->entityID);

                EnterCriticalSection(&instance.m_cs);

                EntityClient* Entity = instance.GetEntities()[entityID];
                if (Entity != nullptr)
                {
                    cpu_entity* entity = Entity->pEntity;
                    instance.UpdateEntityPosition(entity, message->PosX, message->PosY, message->PosZ);
                } else {
                    std::cout << "Warning: Entity " << entityID << " not found for UPDATE_POS" << std::endl;
                }

                LeaveCriticalSection(&instance.m_cs);
                break;
            }
            case MessageType::UPDATE_ROT:
            {
                const UpdateRot* message = reinterpret_cast<const UpdateRot*>(buffer);
                App& instance = App::GetInstance();

                uint32_t entityID = ntohl(message->entityID);

                EnterCriticalSection(&instance.m_cs);

                EntityClient* Entity = instance.GetEntities()[entityID];
                if (Entity != nullptr)
                {
                    cpu_entity* entity = Entity->pEntity;
                    instance.UpdateEntityRotation(entity, message->Yaw, message->Pitch, message->Roll);
                }

                LeaveCriticalSection(&instance.m_cs);
                break;
            }
            case MessageType::ENTITY:
            {
                const SpawnEntity* message = reinterpret_cast<const SpawnEntity*>(buffer);

                switch (message->entity)
                {
                    case(EntityType::SPACESHIP):
                    {
                        App& instance = App::GetInstance();

                        uint32_t entityID = ntohl(message->IDEntity);

                        EnterCriticalSection(&instance.m_cs);

						EntityClient* entityClient = new EntityClient();

                        entityClient->pEntity = cpuEngine.CreateEntity();
                        cpu_mesh* m_meshShip = new cpu_mesh();

                        //m_meshShip->CreateCube();

                        
                        m_meshShip->LoadOBJ("../../res/3D_model/SpaceShip.obj",{1,1,1},false);
                        m_meshShip->FlipWinding();
                        m_meshShip->Optimize();
                        

                        entityClient->pEntity->pMesh = m_meshShip;
						entityClient->entityID = entityID;

                        instance.GetEntities().push_back(entityClient);

                        AABBUpdateMessage AabbMessage;
                        AabbMessage.head.type = MessageType::ENTITY;
                        AabbMessage.IDEntity = htonl(entityID);
                        AabbMessage.minX = m_meshShip->aabb.min.x;
                        AabbMessage.minY = m_meshShip->aabb.min.y;
                        AabbMessage.minZ = m_meshShip->aabb.min.z;

                        AabbMessage.maxX = m_meshShip->aabb.max.x;
                        AabbMessage.maxY = m_meshShip->aabb.max.y;
                        AabbMessage.maxZ = m_meshShip->aabb.max.z;

                        SendMessageToServer(reinterpret_cast<const char*>(&AabbMessage), sizeof(AABBUpdateMessage));
                        LeaveCriticalSection(&instance.m_cs);
                        break;
                    }
                    case(EntityType::BULLET):
                    {
						App& instance = App::GetInstance();
						instance.CreateBullet(ntohl(message->IDEntity), ntohl(message->IDUser));
                        break;
                    }
                }
                break;
            }
        }
    }
    MessageBuffer.clear();
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

    SendMessageToServer(reinterpret_cast<const char*>(&msg), sizeof(ConnexionMessage));
}

void ClientNetwork::ChoseTarget(const char* ip)
{
    //if (inet_pton(AF_INET, "127.0.0.1", &ServeurAddr.sin_addr) <= 0) //LOCAL
    //    return;

    //if (inet_pton(AF_INET, "217.182.207.204", &ServeurAddr.sin_addr) <= 0) //VPS
    //    return;

    //if (inet_pton(AF_INET, "10.10.137.11", &ServeurAddr.sin_addr) <= 0) //MOI192.168.1.159
    //    return;
    if (inet_pton(AF_INET,ip, &ServeurAddr.sin_addr) <= 0) //MOI
        return;
	App::GetInstance().connected = true;
    //if (inet_pton(AF_INET, "10.10.137.20", &ServeurAddr.sin_addr) <= 0) //MOI
    //        return;
    //if (inet_pton(AF_INET, "10.10.137.66", &ServeurAddr.sin_addr) <= 0) //THIB
    //    return;

    //if (inet_pton(AF_INET, "10.10.137.53", &ServeurAddr.sin_addr) <= 0) //ARNAUD
    //    return;

    //if (inet_pton(AF_INET, "10.10.137.52", &ServeurAddr.sin_addr) <= 0) //VALENTIN
    //    return;

    //if (inet_pton(AF_INET, "10.10.137.61", &ServeurAddr.sin_addr) <= 0) //AYMERIC
    //    return;

    //if (inet_pton(AF_INET, "10.10.137.12", &ServeurAddr.sin_addr) <= 0) //ALYSSA
    //    return;
}