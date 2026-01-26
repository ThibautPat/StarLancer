#pragma once

#include "DataProtocol.h"

#include <cstring>
#include <iostream>
#include <vector>

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

// NETWORK BASE -----------------------------------------

class Network
{
	bool InitWindSock();

	bool CreateSocket(socket_t& sock);

	bool BindSocketToPort(socket_t& sock, int port);

	SOCKET m_NetworkSocket;

public:

	bool InitNetwork();

	socket_t* GetSocket() { return &m_NetworkSocket; };

	void CloseSocket(socket_t& sock);

};

class ClientNetwork : public Network
{
public:
	SOCKET UserSock;
	HANDLE thread1;
	sockaddr_in ServeurAddr;

	bool Connected = false;

	void SendMessageToServer(const char* message, size_t size);

	void ConnexionProtcol();

	void ChoseTarget();

	void ParseurMessage();

	static DWORD WINAPI ThreadFonction(LPVOID lpParam);

	void Thread_StartListening();

	std::vector<std::vector<char>> MessageBuffer;

	uint32_t MyIDClient;
};            