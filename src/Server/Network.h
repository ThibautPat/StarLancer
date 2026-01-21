#pragma once

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

// SERVEUR LOGIQUE -----------------------------------------

struct EntityData
{
	float PosX = 0;
	float PosY = 0;
	float PosZ = 0;
};

struct ServerNetworkInfo
{
	sockaddr_in Addr_User{};
	char IP[INET_ADDRSTRLEN];
	int port;
};

struct User
{
	int s_userID;

	ServerNetworkInfo* s_networkInfo;

	std::string s_nameTag = "Anonyme";
	bool s_nameTagModifier = false;

	EntityData* s_EntityData;
};

class ServerNetwork : public Network
{
	User* NewUser(sockaddr_in addr);

	void ParseurMessage(User* user, const char* buffer);

	static DWORD WINAPI ThreadFonction(LPVOID lpParam);


	std::atomic<bool> IsRunning = true;

public:

	CRITICAL_SECTION csNewUser;
	CRITICAL_SECTION csMovedUsers;

	ServerNetwork() { InitializeCriticalSection(&csMovedUsers);     InitializeCriticalSection(&csNewUser); };

	void Thread_StartListening();

	void BacklogSend(User* Recever);

	std::vector<User*> ListUser;

	std::vector<User*> ListOfUserMoved;
};

