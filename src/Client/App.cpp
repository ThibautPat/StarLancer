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
    ShowCursor(TRUE);

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
    if (cpuInput.IsKeyDown(VK_ESCAPE))
    {
        if (m_LockCursor)
        {
            m_LockCursor = !m_LockCursor;
        }
        else {
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

        // Seuil de détection (la souris n'est jamais EXACTEMENT au centre)
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
        cam.SetPosition(t.pos.x, t.pos.y + camHeight, t.pos.z + camDistance);
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

void App::OnUpdate()
{
    network->ParseurMessage();

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
        menuManager->Update(cpuTime.delta);

        InputManager();

        UpdateBullets(cpuTime.delta);

        UpdateParticul();

        CameraUpdate();
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
            cpu_stats& stats = *cpuEngine.GetStats();
            std::string s = " x :" + std::to_string(CursorDir.x) + " y :" + std::to_string(CursorDir.y);
            cpuDevice.DrawText(&m_font, s.c_str(), 0, 0);

            menuManager->Draw(&cpuDevice);
            break;
        }
    }
}

void App::UpdateBullets(float deltaTime)
{
	std::vector<BulletHitMessage*> HitDetection;

    EnterCriticalSection(&m_cs);

    for (auto& bullet : m_bullets)
    {
		bullet->pEntity->transform.pos.z -= 0.5f * deltaTime; // Move bullet forward
       for (auto Entity : m_entities)
        {
           if (Entity->entityID == network->MyIDClient || bullet->OwnerID == Entity->entityID)
           {
               continue;
           }

            if (cpu::AabbAabb(bullet->pEntity->aabb, Entity->pEntity->aabb))
            {
				BulletHitMessage* msg = new BulletHitMessage();
				msg->head.type = MessageType::HIT;
				msg->bulletID = htonl(bullet->entityID);
				msg->targetID = htonl(Entity->entityID);
				HitDetection.push_back(msg);
            }
        }

    }
    LeaveCriticalSection(&m_cs);


    for (BulletHitMessage* msg: HitDetection)
    {
		network->SendMessageToServer(reinterpret_cast<const char*>(msg), sizeof(BulletHitMessage));
    }

}

void App::CreateBullet(uint32_t IdEntity , uint32_t OwnerID)
{
    EnterCriticalSection(&m_cs);

    EntityBulletClient* bullet = new EntityBulletClient();
	bullet->pEntity = cpuEngine.CreateEntity();
    
    cpu_mesh* m_meshBullet = new cpu_mesh();
    m_meshBullet->radius = 0.1f;
    m_meshBullet->CreateSphere(m_meshBullet->radius);

    bullet->pEntity->pMesh = m_meshBullet;
    bullet->pEntity->transform.pos = GetEntities()[OwnerID]->pEntity->transform.pos;
    bullet->OwnerID = OwnerID;
	bullet->entityID = IdEntity;

    GetBullets().push_back(bullet);

    LeaveCriticalSection(&m_cs);
}