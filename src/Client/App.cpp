#include "pch.h"

#include <iostream>
#include <sstream>

#include "Utils.h"


App::App()
{
	s_pApp = this;
	CPU_CALLBACK_START(OnStart);
	CPU_CALLBACK_UPDATE(OnUpdate);
	CPU_CALLBACK_EXIT(OnExit);
	CPU_CALLBACK_RENDER(OnRender);

    InitializeCriticalSection(&m_cs);
    InitializeCriticalSection(&m_cs2);
}

App::~App()
{
	

}

void App::UpdateEntityPosition(cpu_entity* entity, float x, float y, float z)
{
    entity->transform.SetPosition(x, y, z);
}

void App::UpdateEntityRotation(cpu_entity* entity, float rx, float ry, float rz)
{
    entity->transform.SetYPR(rx, ry, rz);
}

void App::UpdateEntityScale(cpu_entity* entity, float scale)
{
    entity->transform.SetScaling(scale);
}

void App::SendMessageToServer(const char* message , size_t size)
{ 


    if (sendto(*network->GetSocket(), message, size, 0, (SOCKADDR*)&ServeurAddr, sizeof(ServeurAddr)) == SOCKET_ERROR)
        std::cout << "PROUT";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ChoseTarget(sockaddr_in& ServeurAddr)
{
    //if (inet_pton(AF_INET, "127.0.0.1", &ServeurAddr.sin_addr) <= 0) //LOCAL
    //    return;

    //if (inet_pton(AF_INET, "217.182.207.204", &ServeurAddr.sin_addr) <= 0) //VPS
    //    return;

    if (inet_pton(AF_INET, "10.10.137.11", &ServeurAddr.sin_addr) <= 0) //MOI
        return;

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void App::OnStart()
{
    menuManager = new MenuManager();
    menuManager->Start();

    m_meshSphere.CreateSphere(2.0f, 12, 12, cpu::ToColor(224, 224, 224));

    m_font.Create(12);

    cpuEngine.GetParticleData()->Create(1000000);
    cpuEngine.GetParticlePhysics()->gy = -0.5f;
    m_pEmitter = cpuEngine.CreateParticleEmitter();
    m_pEmitter->density = 3000.0f;
    m_pEmitter->colorMin = cpu::ToColor(255, 0, 0);
    m_pEmitter->colorMax = cpu::ToColor(255, 128, 0);

    // ------------- CONNEXION ------------------
    ServeurAddr;
    ChoseTarget(ServeurAddr);
    ServeurAddr.sin_family = AF_INET;
    ServeurAddr.sin_port = htons(1888);

    network = new ClientNetwork();
    network->InitNetwork();
    network->Thread_StartListening();

    ConnexionMessage msg{};
    msg.head.type = MessageType::CONNEXION;
    msg.magicnumber = htonl(8542);

    SendMessageToServer(reinterpret_cast<const char*>(&msg), sizeof(ConnexionMessage));
}

void App::OnUpdate()
{
    // ----- PARSE -----
    for (const auto& message : network->MessageBuffer)
    {
        network->ParseurMessage(message.data());
    }
    network->MessageBuffer.clear();

    // ----- UI -----
    menuManager->Update(cpuTime.delta);

    // ----- INPUT -----
    if (cpuInput.IsKey(VK_DOWN))
    {
        InputMessage msg;
        msg.head.type = MessageType::BACKWARD;
        msg.ClientID = network->MyIDClient;

        SendMessageToServer(reinterpret_cast<const char*>(&msg), sizeof(InputMessage));
    }
    if (cpuInput.IsKey(VK_UP))
    {
        InputMessage msg;
        msg.head.type = MessageType::FORWARD;
        msg.ClientID = network->MyIDClient;

        SendMessageToServer(reinterpret_cast<const char*>(&msg), sizeof(InputMessage));
    }
    if (cpuInput.IsKey(VK_LEFT)) 
    {
        InputMessage msg;
        msg.head.type = MessageType::LEFT;
        msg.ClientID = network->MyIDClient;

        SendMessageToServer(reinterpret_cast<const char*>(&msg), sizeof(InputMessage));
    }
    if (cpuInput.IsKey(VK_RIGHT))
    {
        InputMessage msg;
        msg.head.type = MessageType::RIGHT;
        msg.ClientID = network->MyIDClient;

        SendMessageToServer(reinterpret_cast<const char*>(&msg), sizeof(InputMessage));
    }

    // ----- CAMERA -----
    cpu_transform& cam = cpuEngine.GetCamera()->transform;
	EnterCriticalSection(&m_cs);
    if (m_entities[0])
    {
        cpu_transform t = m_entities[0]->transform;

        cam.SetPosition(t.pos.x, t.pos.y + camHeight, t.pos.z + camDistance);
        cam.ResetFlags();
        cam.LookAt(t.pos.x, t.pos.y, t.pos.z);
        m_pEmitter->pos = { t.pos.x , t.pos.y , t.pos.z};
        m_pEmitter->dir = t.dir;
        m_pEmitter->dir.x = -m_pEmitter->dir.x;
        m_pEmitter->dir.y = -m_pEmitter->dir.y;
        m_pEmitter->dir.z = -m_pEmitter->dir.z;
    }
    LeaveCriticalSection(&m_cs);
}

void App::OnExit()
{
}

void App::OnRender(int pass)
{
    switch (pass)
    {
        case CPU_PASS_PARTICLE_BEGIN:
        {
            // Blur particles
            //cpuEngine.SetRT(m_rts[0]);
            //cpuEngine.ClearColor();
            break;
        }
        case CPU_PASS_PARTICLE_END:
        {
            // Blur particles
            //cpuEngine.Blur(10);
            //cpuEngine.SetMainRT();
            //cpuEngine.AlphaBlend(m_rts[0]);
            break;
        }
        case CPU_PASS_UI_END:
        {
            // Debug
            cpu_stats& stats = *cpuEngine.GetStats();

            menuManager->Draw(&cpuDevice);
            break;
        }
    }
}


