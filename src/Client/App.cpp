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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


DWORD WINAPI ThreadFonction(LPVOID lpParam)
{
    char buffer[1024];
    SOCKET sock = (SOCKET)lpParam;
    while (true)
    {
        sockaddr_in SenderAddr{};
        int SenderAddrSize = sizeof(SenderAddr);

        int received = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (sockaddr*)&SenderAddr, &SenderAddrSize);
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
    if (inet_pton(AF_INET, ip, &Addr.sin_addr) <= 0)
        return false;

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
    m_meshSphere.CreateSphere(2.0f, 12, 12, cpu::ToColor(224, 224, 224));
    m_font.Create(12);

    InitWinSock();

    struct sockaddr_in ServeurAddr;

    Send(ServeurAddr);

    ServeurAddr.sin_family = AF_INET;
    ServeurAddr.sin_port = htons(1888);

    // SOCKET
    CreateSocket(ClientSock);

    //THREAD
    thread1 = CreateThread(NULL, 0, ThreadFonction, (LPVOID)ClientSock, 0, NULL);
    CloseHandle(thread1);

    // ENVOIE
    char buffer[100] = "Hello";
    if (sendto(ClientSock, buffer, sizeof(buffer), 0, (SOCKADDR*)&ServeurAddr, sizeof(ServeurAddr)) == SOCKET_ERROR)
    {
        std::cout << "ERREUR D'ENVOIE\n";
    }
    std::cout << "SENDED\n";

    m_pBall = cpuEngine.CreateEntity();
    m_pBall->pMesh = &m_meshSphere;

    m_pBall->transform.pos.x = 0.0f;
    m_pBall->transform.pos.y = 0.0f;
    m_pBall->transform.pos.z = 0.0f;
    m_entities[0]= m_pBall;

}

void App::OnUpdate()
{
    // Move ship
    if (cpuInput.IsKey(VK_UP))
    {
        cpuEngine.GetCamera()->transform.Move(cpuTime.delta * 1.0f);
    }
    if (cpuInput.IsKey(VK_DOWN))
    {
        cpuEngine.GetCamera()->transform.Move(-cpuTime.delta * 1.0f);
    }
}

void App::OnExit()
{
	// YOUR CODE HERE
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
        std::string info = "error: " + WSAGetLastError();



        cpuDevice.DrawText(&m_font, info.c_str(), (int)(cpuDevice.GetWidth() * 0.5f), 10, CPU_TEXT_CENTER);
        break;
    }
    }
}

void App::MyPixelShader(cpu_ps_io& io)
{
	// YOUR CODE HERE
	io.color = io.p.color;
}
