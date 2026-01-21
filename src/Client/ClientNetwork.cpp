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

    switch (head->type) 
    {
        case MessageType::CONNEXION:
        {
            const ReturnConnexionMessage* message = reinterpret_cast<const ReturnConnexionMessage*>(buffer);
            MyIDClient = message->ClientID;
            break;
        }
        case MessageType::UPDATE_POS:
        {
            const UpdatePos* message = reinterpret_cast<const UpdatePos*>(buffer);
            App instance = App::GetInstance();
            cpu_entity* entity = instance.GetEntities()[message->entityID];
            instance.UpdateEntityPosition(entity, message->PosX, message->PosY, message->PosZ);
            break;
        }
        case MessageType::ENTITY:
        {
            const SpawnEntity* message = reinterpret_cast<const SpawnEntity*>(buffer);

            switch(message->entity) 
            {
                case(EntityType::SPACESHIP):
                {
                    App* instance = &App::GetInstance();

                    EnterCriticalSection(&instance->m_cs2); //SECTION DASSAUlT

                    cpu_entity* SpaceShip = cpuEngine.CreateEntity();

                    cpu_mesh* m_meshShip = new cpu_mesh();

                    m_meshShip->CreateCube();

                    //m_meshShip->LoadOBJ("../../res/3D_model/SpaceShip.obj", { 1,1,1 }, false);
                    //m_meshShip->FlipWinding();
                    //m_meshShip->Optimize();

                    SpaceShip->pMesh = m_meshShip;

                        int i = instance->GetEntities().size();
                        instance->GetEntities()[i] = SpaceShip;

                        LeaveCriticalSection(&instance->m_cs2); //SECTION DASSAUlT
                        break;
                    }
                }
            }
        }
    }