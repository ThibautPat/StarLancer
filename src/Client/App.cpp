#include "pch.h"
#include <iostream>

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void App::OnStart()
{    
    // ------------- CONNEXION ------------------
    network = new ClientNetwork();
    network->InitNetwork();
    network->Thread_StartListening();

    network->ChoseTarget();
    network->ServeurAddr.sin_family = AF_INET;
    network->ServeurAddr.sin_port = htons(1888);

    network->ConnexionProtcol();

    // ------------- MENU ------------------
    menuManager = new MenuManager();
    menuManager->Start();
    m_font.Create(12);

    // ------------- MESH ------------------
    m_meshSphere.CreateSphere(2.0f, 12, 12, cpu::ToColor(224, 224, 224));

    // ------------- PARTICUL ------------------
    cpuEngine.GetParticleData()->Create(1000000);
    cpuEngine.GetParticlePhysics()->gy = -0.5f;
    m_pEmitter = cpuEngine.CreateParticleEmitter();
    m_pEmitter->density = 3000.0f;
    m_pEmitter->colorMin = cpu::ToColor(255, 0, 0);
    m_pEmitter->colorMax = cpu::ToColor(255, 128, 0);
}

void App::InputManager()
{
    if (cpuInput.IsKey(VK_DOWN))
    {
        InputMessage msg;
        msg.head.type = MessageType::BACKWARD;
        msg.ClientID = network->MyIDClient;

        network->SendMessageToServer(reinterpret_cast<const char*>(&msg), sizeof(InputMessage));
    }
    if (cpuInput.IsKey(VK_UP))
    {
        InputMessage msg;
        msg.head.type = MessageType::FORWARD;
        msg.ClientID = network->MyIDClient;

        network->SendMessageToServer(reinterpret_cast<const char*>(&msg), sizeof(InputMessage));
    }
    if (cpuInput.IsKey(VK_LEFT))
    {
        InputMessage msg;
        msg.head.type = MessageType::LEFT;
        msg.ClientID = network->MyIDClient;

        network->SendMessageToServer(reinterpret_cast<const char*>(&msg), sizeof(InputMessage));
    }
    if (cpuInput.IsKey(VK_RIGHT))
    {
        InputMessage msg;
        msg.head.type = MessageType::RIGHT;
        msg.ClientID = network->MyIDClient;

        network->SendMessageToServer(reinterpret_cast<const char*>(&msg), sizeof(InputMessage));
    }
    if (cpuInput.IsKey(VK_SPACE)) 
    {
        InputMessage msg;
        msg.head.type = MessageType::FIRE_BULLET;
        msg.ClientID = network->MyIDClient;

        network->SendMessageToServer(reinterpret_cast<const char*>(&msg), sizeof(InputMessage));
    }
}

void App::CameraUpdate()
{
    cpu_transform& cam = cpuEngine.GetCamera()->transform;

    EnterCriticalSection(&m_cs);
    auto it = m_entities.find(network->MyIDClient);
    if (it != m_entities.end() && it->second != nullptr)
    {
        cpu_transform t = it->second->transform;
        cam.SetPosition(t.pos.x, t.pos.y + camHeight, t.pos.z + camDistance);
        cam.ResetFlags();
        cam.LookAt(t.pos.x, t.pos.y, t.pos.z);
    }
    LeaveCriticalSection(&m_cs);
}

void App::UpdateParticul()
{
    EnterCriticalSection(&m_cs);
    auto it = m_entities.find(network->MyIDClient);
    if (it != m_entities.end() && it->second != nullptr)
    {
        cpu_transform t = it->second->transform;
        m_pEmitter->pos = { t.pos.x , t.pos.y , t.pos.z };
        m_pEmitter->dir = t.dir;
        m_pEmitter->dir.x = -m_pEmitter->dir.x;
        m_pEmitter->dir.y = -m_pEmitter->dir.y;
        m_pEmitter->dir.z = -m_pEmitter->dir.z;
    }
    LeaveCriticalSection(&m_cs);
}

void App::OnUpdate()
{
    network->ParseurMessage();

    menuManager->Update(cpuTime.delta);

    InputManager();

    UpdateParticul();

    CameraUpdate();
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
            break;
        }
        case CPU_PASS_PARTICLE_END:
        {
            break;
        }
        case CPU_PASS_UI_END:
        {
            cpu_stats& stats = *cpuEngine.GetStats();

            menuManager->Draw(&cpuDevice);
            break;
        }
    }
}


