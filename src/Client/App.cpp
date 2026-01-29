#include "pch.h"
#include <iostream>
struct ProjectionResult
{
    XMFLOAT2 screenPosition;
    bool isOnScreen;
    float depth; // Pour le tri si nécessaire
};

ProjectionResult ProjectWorldToScreen(
    const XMFLOAT3& worldPosition,
    const XMFLOAT4X4& viewMatrix,
    const XMFLOAT4X4& projectionMatrix,
    float screenWidth,
    float screenHeight)
{
    ProjectionResult result;
    result.isOnScreen = false;
    result.depth = 0.0f;

    XMMATRIX view = XMLoadFloat4x4(&viewMatrix);
    XMMATRIX proj = XMLoadFloat4x4(&projectionMatrix);

    XMVECTOR worldPos = XMLoadFloat3(&worldPosition);
    worldPos = XMVectorSetW(worldPos, 1.0f);

    XMVECTOR viewPos = XMVector4Transform(worldPos, view);

    XMVECTOR projPos = XMVector4Transform(viewPos, proj);

    float w = XMVectorGetW(projPos);

    if (w <= 0.0f)
    {
        return result;
    }

    XMFLOAT4 ndc;
    XMStoreFloat4(&ndc, projPos);
    ndc.x /= w;
    ndc.y /= w;
    ndc.z /= w;

    if (ndc.x < -1.0f || ndc.x > 1.0f ||
        ndc.y < -1.0f || ndc.y > 1.0f ||
        ndc.z < 0.0f || ndc.z > 1.0f)
    {
        return result;
    }

    result.screenPosition.x = (ndc.x + 1.0f) * 0.5f * screenWidth;
    result.screenPosition.y = (1.0f - ndc.y) * 0.5f * screenHeight; // Inverser Y
    result.depth = ndc.z;
    result.isOnScreen = true;

    return result;
}
void App::RenderEntityLabels(
    cpu_camera* camera,
    float screenWidth,
    float screenHeight)
{
    for (auto& entity : network->PlayerInfoList)
    {
        ProjectionResult result = ProjectWorldToScreen(
            GetEntitie(entity->ID)->pEntity->transform.pos,
            camera->matView,
            camera->matProj,
            screenWidth,
            screenHeight
        );

        if (result.isOnScreen)
        {
            if(entity->ID != network->MyIDClient)
            {
                float offsetY = -30.0f; 
                cpuDevice.DrawText(&m_font, entity->pseudo, result.screenPosition.x, result.screenPosition.y + offsetY);
            }
        }
    }
}
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

    m_meshShip = new cpu_mesh();
    m_meshShip->LoadOBJ("../../res/3D_model/SpaceShip.obj", { 1,1,1 }, false);
    m_meshShip->FlipWinding();
    m_meshShip->Optimize();

    m_meshBullet = new cpu_mesh();
    m_meshBullet->radius = 0.1f;
    m_meshBullet->CreateSphere(m_meshBullet->radius);

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
    bool tabPressedNow = cpuInput.IsKey(VK_TAB);
    if (tabPressedNow && !tabPreviouslyPressed)
    {
        LookStat = !LookStat;
    }
    tabPreviouslyPressed = tabPressedNow;

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
            while (ShowCursor(TRUE) <= 0);

            m_LockCursor = !m_LockCursor;
        }
        else {
            while (ShowCursor(FALSE) >= 0);

            m_LockCursor = !m_LockCursor;
        }
       
    }
    if (m_LockCursor)
    {
        POINT pt;
        GetCursorPos(&pt);

        int centerX = cpuEngine.GetWindow()->GetWidth() / 2;
        int centerY = cpuEngine.GetWindow()->GetHeight() / 2;

        float deltaX = pt.x - centerX;
        float deltaY = pt.y - centerY;

        const float DEADZONE = 1.0f;

        if (fabs(deltaX) < DEADZONE && fabs(deltaY) < DEADZONE)
        {
            CursorDir = { 0.0f, 0.0f };
        }
        else
        {
            CursorDir = { deltaX, deltaY };
            XMVECTOR vec = XMLoadFloat2(&CursorDir);
            vec = XMVector2Normalize(vec);
            XMStoreFloat2(&CursorDir, vec);
        }

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
    if (network->Connected == false && network->PseudoSelected)
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
                if (LookStat)
                {
                    float startX = cpuWindow.GetWidth() * 0.25f;
                    float startY = cpuWindow.GetHeight() * 0.25f;
                    float lineHeight = 20.f;

                    for (int i = 0; i < network->PlayerInfoList.size(); ++i)
                    {
                        DataPlayer* player = network->PlayerInfoList[i];

                        float kd = (player->DeathCount == 0) ? static_cast<float>(player->KillCount)
                                    : static_cast<float>(player->KillCount) / player->DeathCount;

                        std::string enti = "Player: " + std::string(player->pseudo)
                                        + " | Kill: " + std::to_string(player->KillCount)
                                        + " | Death: " + std::to_string(player->DeathCount)
                                        + " | K/D: " + std::to_string(kd);

                        cpuDevice.DrawText(&m_font, enti.c_str(), startX, startY + i * lineHeight);
                    }
                }
                else
                {
                    cpu_stats& stats = *cpuEngine.GetStats();

                    std::string score = " Life : " + std::to_string(GetEntitie(network->MyIDClient)->life);
                    cpuDevice.DrawText(&m_font, score.c_str(), 0, 50);

                    DataPlayer* info = network->GetData(network->MyIDClient);
                    std::string KD = " K/D : " + std::to_string(info->KillCount) + "/" + std::to_string(info->DeathCount);
                    cpuDevice.DrawText(&m_font, KD.c_str(), 0, 100);
                }
            }

            RenderEntityLabels( cpuEngine.GetCamera(), cpuDevice.GetWidth(), cpuDevice.GetHeight());
            menuManager->Draw();
            break;
        }
    }
}

void App::CreateBullet(uint32_t IdEntity , uint32_t OwnerID)
{
    EntityBulletClient* bullet = new EntityBulletClient();
	bullet->pEntity = cpuEngine.CreateEntity();
    
    EnterCriticalSection(&m_cs);
	bullet->ownerBULLET_FORWARD = GetEntitie(OwnerID)->pEntity->transform.dir;
    bullet->pEntity->pMesh = m_meshBullet;
    bullet->pEntity->transform.pos = GetEntitie(OwnerID)->pEntity->transform.pos;
    bullet->OwnerID = OwnerID;
	bullet->entityID = IdEntity;
    GetEntitiesList()[IdEntity] = bullet;
    LeaveCriticalSection(&m_cs);
}