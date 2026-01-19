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
void App::SendMessageToServer(std::string message)
{


    size_t a = sizeof(message);

    sendto(UserSock, message.c_str(), a, 0, (SOCKADDR*)&ServeurAddr, sizeof(ServeurAddr));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


DWORD WINAPI ThreadFonction(LPVOID lpParam)
{
    char buffer[1024];
    SOCKET sock = (SOCKET)lpParam;
    while (true)
    {
        sockaddr_in SenderAddr;
        int SenderAddrSize = sizeof(SenderAddr);

        int received = recvfrom(sock, buffer, 1024 - 1, 0, (sockaddr*)&SenderAddr, &SenderAddrSize);
        if (received == SOCKET_ERROR)
        {
            int error = WSAGetLastError();
            continue;
        }
        if (received > 0)
        {
            buffer[received] = '\0';
            Utils::ParseurMessage(&App::GetInstance(), buffer);
        }
    }
    return 0;
}

bool InitWinSock()
{
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cout << "WSAStartup failed\n";
        return false;
    }
    std::cout << "WSAStartup successful\n";
    return true;
}

bool CreateSocket(SOCKET& sock)
{
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) {
        std::cout << "Socket creation failed\n";
        WSACleanup();
        return false;
    }
    std::cout << "Socket created\n";
    return true;
}

bool BindSocketToPort(SOCKET& sock, int port, PCSTR ip)
{
    sockaddr_in Addr;
   /* if (inet_pton(AF_INET, ip, &Addr.sin_addr) <= 0)
        return false;*/

    Addr.sin_family = AF_INET;
    Addr.sin_port = htons(port);
    Addr.sin_addr.s_addr = ADDR_ANY;

    if (bind(sock, (sockaddr*)&Addr, sizeof(Addr)) == SOCKET_ERROR)
    {
        std::cout << "bind failed\n";
        closesocket(sock);
        WSACleanup();
        return false;
    }

    std::cout << "bind successful\n";
    return true;
}

void ChoseTarget(sockaddr_in& ServeurAddr)
{
    if (inet_pton(AF_INET, "127.0.0.1", &ServeurAddr.sin_addr) <= 0) //LOCAL
        return;

    //if (inet_pton(AF_INET, "217.182.207.204", &ServeurAddr.sin_addr) <= 0) //VPS
    //    return;

    //if (inet_pton(AF_INET, "10.10.137.11", &ServeurAddr.sin_addr) <= 0) //MOI
    //    return;

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
    InitWinSock();
    m_font.Create(12);
    SpaceShip = cpuEngine.CreateEntity();
    m_meshSphere.CreateSphere(2.0f, 12, 12, cpu::ToColor(224, 224, 224));
    //Mesh
    m_pBall = cpuEngine.CreateEntity();
    m_pBall->pMesh = &m_meshSphere;
    m_meshShip = new cpu_mesh();
    m_meshShip->LoadOBJ("../../res/3D_model/SpaceShip.obj",{1,1,1},false);
    m_meshShip->FlipWinding();
    m_meshShip->Optimize();
    SpaceShip->pMesh = m_meshShip;

    m_ShipTexture = new cpu_texture();
    m_ShipTexture->Load(".. /../res/Texture/ORANGE_DEBUG_TEXTURE.png");

    cpuEngine.GetParticleData()->Create(1000000);
    cpuEngine.GetParticlePhysics()->gy = -0.5f;
    m_pEmitter = cpuEngine.CreateParticleEmitter();
    m_pEmitter->density = 3000.0f;
    m_pEmitter->colorMin = cpu::ToColor(255, 0, 0);
    m_pEmitter->colorMax = cpu::ToColor(255, 128, 0);

    //Transform
    SpaceShip->transform.SetPosition(0, 0, 0);
    SpaceShip->transform.SetYPR(0,0,0);


    // ------------- CONNEXION ------------------
    ServeurAddr;
    ChoseTarget(ServeurAddr);
    ServeurAddr.sin_family = AF_INET;
    ServeurAddr.sin_port = htons(1888);

    // SOCKET
    CreateSocket(UserSock);

    //if (bind(UserSock, (sockaddr*)&ServeurAddr, sizeof(ServeurAddr)) == SOCKET_ERROR)
    //{
    //    std::cout << "Bind failed: " << WSAGetLastError() << "\n";
    //}

    //THREAD
    thread1 = CreateThread(NULL, 0, ThreadFonction, (LPVOID)UserSock, 0, NULL);
    CloseHandle(thread1);

    // ENVOIE
  
    SendMessageToServer("{CONNEXION}");
    std::cout << "SENDED\n";

   
    m_entities[0]= SpaceShip;



}

void App::OnUpdate()
{
    if (cpuInput.IsKey(VK_DOWN)) // avancer vers la souris
    {
        SendMessageToServer("{BACKWARD}");
    }
    if (cpuInput.IsKey(VK_UP)) // reculer
    {
        SendMessageToServer("{FORWARD}");

    }
    if (cpuInput.IsKey(VK_DOWN)) // avancer vers la souris
    {
        SendMessageToServer("{LEFT}");
    }
    if (cpuInput.IsKey(VK_UP)) // reculer
    {
        SendMessageToServer("{RIGHT}");

    }
    // ----- CAM�RA -----
    cpu_transform& cam = cpuEngine.GetCamera()->transform;
	cpu_transform t = SpaceShip->transform;

    cam.SetPosition(t.pos.x, t.pos.y + camHeight, t.pos.z + camDistance);
    cam.ResetFlags();
    cam.LookAt(t.pos.x, t.pos.y, t.pos.z);

    // ----- Particule -----
    m_pEmitter->pos = { t.pos.x , t.pos.y , t.pos.z};
    m_pEmitter->dir = t.dir;
    m_pEmitter->dir.x = -m_pEmitter->dir.x;
    m_pEmitter->dir.y = -m_pEmitter->dir.y;
    m_pEmitter->dir.z = -m_pEmitter->dir.z;
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
        break;
    }
    }
}

void App::MyPixelShader(cpu_ps_io& io)
{
	io.color = io.p.color;
}
