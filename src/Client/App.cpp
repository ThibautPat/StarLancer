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
    ButtonListenerManager::Init();
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
    ShowCursor(FALSE);
   
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
    if (GetEntitie(network->MyIDClient)->IsDead == true)
        return;

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
    if (cpuInput.IsKey(VK_LBUTTON) && TimerShoot <=0) 
    {
        TimerShoot = coldownShoot;
        InputMessage msg;
        msg.head.type = MessageType::FIRE_BULLET;
        msg.ClientID = network->MyIDClient;

        network->SendMessageToServer(reinterpret_cast<const char*>(&msg), sizeof(InputMessage));
    }
    if (cpuInput.IsKeyDown(VK_ESCAPE))
    {
        if (m_LockCursor)
        {
            while (ShowCursor(TRUE) <= 0); // Boucler jusqu'à ce que le compteur soit négatif

            m_LockCursor = !m_LockCursor;
        }
        else {
            while (ShowCursor(FALSE) >= 0); // Boucler jusqu'à ce que le compteur soit négatif

            m_LockCursor = !m_LockCursor;
        }
       
    }
    if (m_LockCursor)
    {
        POINT pt;
        GetCursorPos(&pt);

        int centerX = cpuEngine.GetWindow()->GetWidth() / 2;
        int centerY = cpuEngine.GetWindow()->GetHeight() / 2;

        // Delta
        float deltaX = pt.x - centerX;
        float deltaY = pt.y - centerY;

        // Seuil de d�tection (la souris n'est jamais EXACTEMENT au centre)
        const float DEADZONE = 2.0f; // pixels

        if (fabs(deltaX) < DEADZONE && fabs(deltaY) < DEADZONE)
        {
            // Pas de mouvement
            CursorDir = { 0.0f, 0.0f };
        }
        else
        {
            // Il y a du mouvement, normaliser
            CursorDir = { deltaX, deltaY };
            XMVECTOR vec = XMLoadFloat2(&CursorDir);
            vec = XMVector2Normalize(vec);
            XMStoreFloat2(&CursorDir, vec);
        }

        // Recentrer
        POINT centerPoint = { centerX, centerY };
        ClientToScreen(cpuEngine.GetWindow()->GetHWND(), &centerPoint);
        SetCursorPos(centerPoint.x, centerPoint.y);

        if(CursorDir.x != 0.0f || CursorDir.y != 0.0f)
        {
            MouseMessage msg;
            msg.head.type = MessageType::MOUSE;
            msg.ClientID = network->MyIDClient;
            msg.X = CursorDir.x;
            msg.Y = CursorDir.y;

            network->SendMessageToServer(reinterpret_cast<const char*>(&msg), sizeof(MouseMessage));
		}
       
    }
}

void App::CameraUpdate()
{
    cpu_transform& cam = cpuEngine.GetCamera()->transform;

    EnterCriticalSection(&m_cs);
    EntityClient* player = m_entities[network->MyIDClient];
    if (player != nullptr)
    {
        cpu_transform t = player->pEntity->transform;

        XMFLOAT3 pos = { t.pos.x - (t.dir.x * camDistance) ,t.pos.y - (t.dir.y * camDistance)+ 4  ,t.pos.z - (t.dir.z * camDistance)};

        cam.SetPosition(pos.x, pos.y,pos.z);
        cam.ResetFlags();
        cam.LookAt(t.pos.x, t.pos.y, t.pos.z);
    }
    LeaveCriticalSection(&m_cs);
}

void App::UpdateParticul()
{
    EnterCriticalSection(&m_cs);
    EntityClient* player = m_entities[network->MyIDClient];
    if (player != nullptr)
    {
        cpu_transform t = player->pEntity->transform;

        m_pEmitter->pos = { t.pos.x , t.pos.y , t.pos.z };
        m_pEmitter->dir = t.dir;
        m_pEmitter->dir.x = -m_pEmitter->dir.x;
        m_pEmitter->dir.y = -m_pEmitter->dir.y;
        m_pEmitter->dir.z = -m_pEmitter->dir.z;
    }
    LeaveCriticalSection(&m_cs);
}

void App::ClearDeadEntity()
{
    EnterCriticalSection(&m_cs);

    for (auto it = m_entities.begin(); it != m_entities.end(); )
    {
        EntityClient* entity = it->second;

        if (entity->ToDestroy)
        {
            entity->clearEntity();
            delete entity;
            it = m_entities.erase(it);
        }
        else
        {
            ++it;
        }
    }

    LeaveCriticalSection(&m_cs);
}

void App::OnUpdate()
{
    TimerShoot -= cpuTime.delta;
    network->ParseurMessage();
    menuManager->Update(cpuTime.delta);
    if (ButtonListenerManager::s_inputActive)
    {
        ButtonListenerManager::UpdateInput();
    }
    if (network->Connected == false)
    {   
		coldownNetwork += cpuTime.delta;
        if (coldownNetwork > TimerBeforeRetry)
        {
			coldownNetwork = 0.0f;
            network->ConnexionProtcol();
        }
    }

    if(network->Connected == true)
    {
        InputManager();

        for (auto entitie : m_entities) 
            entitie.second->Update_EntityClient(cpuTime.delta);
        
        UpdateParticul();

        CameraUpdate();

        ClearDeadEntity();
    }
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
            if (network->Connected == true)
            {
                cpu_stats& stats = *cpuEngine.GetStats();

                std::string enti = " nmb Entity : " + std::to_string(GetEntitiesList().size());
                cpuDevice.DrawText(&m_font, enti.c_str(), 0, 10);

                std::string score = " Life : " + std::to_string(GetEntitie(network->MyIDClient)->life);
                cpuDevice.DrawText(&m_font, score.c_str(), 0, 50);

                std::string Kill = " K/D : " + std::to_string(GetEntitie(network->));
                cpuDevice.DrawText(&m_font, score.c_str(), 0, 100);

            }
            menuManager->Draw();
            break;
        }
    }
}

void App::CreateBullet(uint32_t IdEntity , uint32_t OwnerID)
{
    EntityBulletClient* bullet = new EntityBulletClient();
	bullet->pEntity = cpuEngine.CreateEntity();
    
    cpu_mesh* m_meshBullet = new cpu_mesh();
    m_meshBullet->radius = 0.1f;
    m_meshBullet->CreateSphere(m_meshBullet->radius);

    EnterCriticalSection(&m_cs);
	bullet->ownerBULLET_FORWARD = GetEntitie(OwnerID)->pEntity->transform.dir;
    bullet->pEntity->pMesh = m_meshBullet;
    bullet->pEntity->transform.pos = GetEntitie(OwnerID)->pEntity->transform.pos;
    bullet->OwnerID = OwnerID;
	bullet->entityID = IdEntity;
    GetEntitiesList()[IdEntity] = bullet;
    LeaveCriticalSection(&m_cs);
}