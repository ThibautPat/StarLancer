#include "pch.h"

#include <iostream>
#include <sstream>

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void App::MoveBall(const char* buffer)
{
    float x = 0.f, y = 0.f, z = 0.f;

    std::string msg(buffer);

    if (msg.front() == '{' && msg.back() == '}')
    {
        msg = msg.substr(1, msg.size() - 2);
    }
    else
    {
        return;
    }

    std::stringstream ss(msg);
    char separator;

    ss >> x >> separator >> y >> separator >> z;

    if (ss.fail())
        return;
    
    s_pApp->m_pBall->transform.SetPosition(x, y, z);
    std::cout << "NEW LOCATION";
}

DWORD WINAPI ThreadFonction(LPVOID lpParam)
{
    while (true)
    {
        sockaddr_in SenderAddr{};
        int SenderAddrSize = sizeof(SenderAddr);
        char buffer[1024];
        SOCKET sock = (SOCKET)lpParam;

        int received = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (sockaddr*)&SenderAddr, &SenderAddrSize);

        if (received > 0)
        {
            buffer[received] = '\0';
            App::MoveBall(buffer);
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

void Send(sockaddr_in& ServeurAddr)
{
    //if (inet_pton(AF_INET, "127.0.0.1", &ServeurAddr.sin_addr) <= 0) //LOCAL
    //    return;

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
    SpaceShip = cpuEngine.CreateEntity();

    //Mesh
    m_meshShip = new cpu_mesh();
    m_meshShip->LoadOBJ("../../res/3D_model/SpaceShip.obj",{1,1,1},false);
    m_meshShip->FlipWinding();
    m_meshShip->Optimize();
    SpaceShip->pMesh = m_meshShip;


    m_ShipTexture = new cpu_texture();
    m_ShipTexture->Load(".. /../res/Texture/ORANGE_DEBUG_TEXTURE.png");

    m_pEmitter = cpuEngine.CreateParticleEmitter();
    m_pEmitter->density = 3000.0f;
    m_pEmitter->colorMin = cpu::ToColor(255, 0, 0);
    m_pEmitter->colorMax = cpu::ToColor(255, 128, 0);

    //Transform
    SpaceShip->transform.SetPosition(0, 0, 0);
    SpaceShip->transform.SetYPR(0,0,0);
}

void App::OnUpdate()
{
    cpu_transform& t = SpaceShip->transform;

    // ----- Souris -----
    POINT currentMouse;
    GetCursorPos(&currentMouse);
    ScreenToClient(cpu_engine::GetInstance().GetWindow()->GetHWND(), &currentMouse);

    static POINT lastMousePos = currentMouse;
    float deltaX = (currentMouse.x - lastMousePos.x) * 0.01f;
    lastMousePos = currentMouse;

    float centerX = cpu_engine::GetInstance().GetWindow()->GetWidth() / 2.0f;
    float centerZ = cpu_engine::GetInstance().GetWindow()->GetHeight() / 2.0f;

    float mouseX = (currentMouse.x - centerX) / centerX;
    float mouseZ = (currentMouse.y - centerZ) / centerZ;

    // Calcul de la position cible relative à la souris
    XMFLOAT3 target;
    target.x = t.pos.x + mouseX * 2.0f;
    target.y = t.pos.y + mouseZ * 2.0f;
    target.z = t.pos.z + 10.0f;

    // Oriente le vaisseau vers la cible
    t.LookAt(target.x, target.y, target.z);

    // ----- Déplacement vers la souris -----
    // Vecteur direction normalisé
    XMFLOAT3 direction;
    direction.x = target.x - t.pos.x;
    direction.y = target.y - t.pos.y;
    direction.z = target.z - t.pos.z;

    float length = sqrt(direction.x * direction.x +
        direction.y * direction.y +
        direction.z * direction.z);

    if (length > 0.0001f)
    {
        direction.x /= length;
        direction.y /= length;
        direction.z /= length;
    }

    float speed = 10.0f; // vitesse
    if (cpuInput.IsKey(VK_UP)) // avancer vers la souris
    {
        t.pos.x += direction.x * cpuTime.delta * speed;
        t.pos.y += direction.y * cpuTime.delta * speed;
        t.pos.z += direction.z * cpuTime.delta * speed;
    }
    if (cpuInput.IsKey(VK_DOWN)) // reculer
    {
        t.pos.x -= direction.x * cpuTime.delta * speed;
        t.pos.y -= direction.y * cpuTime.delta * speed;
        t.pos.z -= direction.z * cpuTime.delta * speed;
    }

    // ----- CAMÉRA -----
    cpu_transform& cam = cpuEngine.GetCamera()->transform;
    cam.SetPosition(t.pos.x, t.pos.y + camHeight, t.pos.z + camDistance);
    cam.ResetFlags();
    cam.LookAt(t.pos.x, t.pos.y, t.pos.z);

    // ----- Particule -----
    m_pEmitter->pos = t.pos;
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
}

void App::MyPixelShader(cpu_ps_io& io)
{
	io.color = io.p.color;
}
