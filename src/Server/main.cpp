#include "pch.h"
#include "main.h"

#include <vector>
#include <map>
#include <cstring>
#include <iostream>
#include <sstream>
#include <iomanip>

/* ======================= PLATFORM ======================= */

#ifdef _WIN32
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#pragma comment(lib, "Ws2_32.lib")
	typedef SOCKET socket_t;
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <unistd.h>
	#include <cerrno>
	typedef int socket_t;
	#define INVALID_SOCKET (-1)
	#define SOCKET_ERROR   (-1)
#endif

/* ======================= STRUCTURES ======================= */

struct entity
{
    float PosX = 0;
    float PosY = 0;
    float PosZ = 0;
};

struct Utilisateur
{
    int ID;

    sockaddr_in Addr_utilisateur{};
    char IP[INET_ADDRSTRLEN];
    int port;

    std::string Pseudo_utilisateur = "Anonyme";
    bool pseudoPersonnaliser = false;

    entity* m_entity;
};

enum Commande
{
    Pseudo,
    MP
};

std::vector<Utilisateur*> ListClient;
std::vector<Utilisateur*> newListClient;

/* ======================= NETWORK ======================= */

bool InitNetwork()
{
    #ifdef _WIN32
	    WSADATA wsa;
	    return WSAStartup(MAKEWORD(2, 2), &wsa) == 0;
    #else
	    return true;
    #endif
}

void CloseSocket(socket_t sock)
{
    #ifdef _WIN32
	    closesocket(sock);
	    WSACleanup();
    #else
	    close(sock);
    #endif
}

bool CreateSocket(socket_t& sock)
{
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	return sock != INVALID_SOCKET;
}

bool BindSocketToPort(socket_t sock, int port)
{
	sockaddr_in Addr{};
	Addr.sin_family = AF_INET;
	Addr.sin_port = htons(port);
	Addr.sin_addr.s_addr = INADDR_ANY;

	return bind(sock, (sockaddr*)&Addr, sizeof(Addr)) != SOCKET_ERROR;
}

/* ======================= CLIENT ======================= */

Utilisateur* NewClient(sockaddr_in addr)
{
    Utilisateur* client = new Utilisateur();
    newListClient.push_back(client);

    client->ID = newListClient.size();
    client->Addr_utilisateur = addr;

    client->m_entity = new entity();

    inet_ntop(AF_INET, &addr.sin_addr, client->IP, INET_ADDRSTRLEN);
    client->port = ntohs(addr.sin_port);

    std::cout << "New client: " << client->IP << ":" << client->port << "\n";
    return client;
}

/* ======================= THEARD ======================= */

DWORD WINAPI ThreadFonction(LPVOID lpParam)
{
    while (true)
    {
        SOCKET sock = (SOCKET)lpParam;

        sockaddr_in SenderAddr{};
        #ifdef _WIN32
            int SenderAddrSize = sizeof(SenderAddr);
        #else
            socklen_t SenderAddrSize = sizeof(SenderAddr);
        #endif

        char buffer[1024]{};

        while (true)
        {
            memset(buffer, 0, sizeof(buffer));

            int received = recvfrom(
                sock,
                buffer,
                sizeof(buffer) - 1,
                0,
                (sockaddr*)&SenderAddr,
                &SenderAddrSize
            );

            if (received <= 0)
                continue;

            Utilisateur* client = nullptr;
            bool found = false;

            char ip_current[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &SenderAddr.sin_addr, ip_current, INET_ADDRSTRLEN);

            for (auto* c : ListClient)
            {
                if (strcmp(ip_current, c->IP) == 0 ) //&& SenderAddr.sin_port == c->Addr_utilisateur.sin_port
                {
                    client = c;
                    found = true;
                    break;
                }
            }

            if (!found)
                client = NewClient(SenderAddr);
        }
    }
}

/* ======================= MESSAGE ======================= */

void ReplicationMessage(socket_t sock, const std::string& message, Utilisateur* sender, bool excludeSender)
{
    for (auto* client : ListClient)
    {
        if (excludeSender && sender && client->ID == sender->ID)
            continue;

        sendto(sock, message.c_str(), message.size(), 0, (sockaddr*)&client->Addr_utilisateur, sizeof(client->Addr_utilisateur));
    }
}

std::string ConcataneMessage(Utilisateur* client, const char* buffer)
{
    return buffer;
}

void MoveLoop(socket_t sock, sockaddr* Addr_utilisateur, int sizeAddr_utilisateur, entity* ball)
{
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2)<< '{' << ball->PosX << ';' << ball->PosY << ';' << ball->PosZ << '}';

    std::string message = oss.str();

    sendto(sock, message.c_str(), message.size(), 0, Addr_utilisateur, sizeAddr_utilisateur);

    ball->PosZ += 0.02;

    if (ball->PosZ >= 100)
        ball->PosZ = 0;
}

/* ======================= MAIN ======================= */

int main()
{
    if (!InitNetwork())
    {
        std::cerr << "Network init failed\n";
        return 1;
    }

    socket_t ServeurSock;

    if (!CreateSocket(ServeurSock))
    {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    if (!BindSocketToPort(ServeurSock, 1888))
    {
        std::cerr << "Bind failed\n";
        CloseSocket(ServeurSock);
        return 1;
    }

    std::cerr << "Server UP -- \n";

    HANDLE thread1;
    thread1 = CreateThread(NULL, 0, ThreadFonction, (LPVOID)ServeurSock, 0, NULL);
    CloseHandle(thread1);

    while (true)
    {
        for (auto* client : ListClient)
        {
            if (client == nullptr)
                continue;
            MoveLoop(ServeurSock, (sockaddr*)&client->Addr_utilisateur, sizeof(client->Addr_utilisateur), client->m_entity);
        }
        ListClient = newListClient;
    }

    CloseSocket(ServeurSock);
    return 0;
}

