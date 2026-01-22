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


void ClientNetwork::ParseurMessage(const char* buffer)
{
    const Header* head = reinterpret_cast<const Header*>(buffer);
    if (!head || head == nullptr)
    {
        return;
    }

    switch (head->type)
    {
    case MessageType::CONNECTION:
    {
        const ReturnConnexionMessage* message = reinterpret_cast<const ReturnConnexionMessage*>(buffer);
        MyIDClient = ntohl(message->ClientID);
        std::cout << "Received ClientID: " << MyIDClient << std::endl;
        break;
    }

    case MessageType::UPDATE_POS:
    {
        const UpdatePos* message = reinterpret_cast<const UpdatePos*>(buffer);
        App& instance = App::GetInstance();  

        uint32_t entityID = ntohl(message->entityID); 

        EnterCriticalSection(&instance.m_cs);

        auto it = instance.GetEntities().find(entityID);
        if (it != instance.GetEntities().end() && it->second != nullptr)
        {
            cpu_entity* entity = it->second;
            instance.UpdateEntityPosition(entity, message->PosX, message->PosY, message->PosZ);
        }
        else
        {
            std::cout << "Warning: Entity " << entityID << " not found for UPDATE_POS" << std::endl;
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

            if (instance.GetEntities().find(entityID) != instance.GetEntities().end())
            {
                std::cout << "Warning: Entity " << entityID << " already exists!" << std::endl;
                LeaveCriticalSection(&instance.m_cs);
                break;
            }

            cpu_entity* SpaceShip = cpuEngine.CreateEntity();
            cpu_mesh* m_meshShip = new cpu_mesh();
            m_meshShip->CreateCube();
            SpaceShip->pMesh = m_meshShip;

            instance.GetEntities()[entityID] = SpaceShip;

            std::cout << "Spawned entity " << entityID << std::endl;



			AABBUpdateMessage AabbMessage;
			AabbMessage.head.type = MessageType::ENTITY;
            AabbMessage.IDEntity = htonl(entityID);
            AabbMessage.minX =m_meshShip->aabb.min.x;
            AabbMessage.minY =m_meshShip->aabb.min.y;
            AabbMessage.minZ =m_meshShip->aabb.min.z;

            AabbMessage.maxX =m_meshShip->aabb.max.x;
            AabbMessage.maxY =m_meshShip->aabb.max.y;
            AabbMessage.maxZ =m_meshShip->aabb.max.z;

			instance.SendMessageToServer(reinterpret_cast<const char*>(&AabbMessage), sizeof(uint32_t));
            LeaveCriticalSection(&instance.m_cs);
            break;
        }
        }
        break;
    }
    }
}