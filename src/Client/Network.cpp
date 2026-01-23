#include "pch.h"
#include "Network.h"

bool Network::InitWindSock()
{
	#ifdef _WIN32
		WSADATA wsa;
		return WSAStartup(MAKEWORD(2, 2), &wsa) == 0;
	#else
		return true;
	#endif
}

void Network::CloseSocket(socket_t& sock)
{
	#ifdef _WIN32
		closesocket(sock);
		WSACleanup();
	#else
		close(sock);
	#endif
}

bool Network::CreateSocket(socket_t& sock)
{
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	return sock != INVALID_SOCKET;
}

bool Network::BindSocketToPort(socket_t& sock, int port)
{
	sockaddr_in Addr{};
	Addr.sin_family = AF_INET;
	Addr.sin_port = htons(port);
	Addr.sin_addr.s_addr = INADDR_ANY;

	return bind(sock, (sockaddr*)&Addr, sizeof(Addr)) != SOCKET_ERROR;
}

// PUBLIC ---------------------------------------------------

bool Network::InitNetwork()
{
	if (!InitWindSock())
	{
		std::cerr << "Network init failed\n";
		return 1;
	}

	if (!CreateSocket(m_NetworkSocket))
	{
		std::cerr << "Socket creation failed\n";
		return 1;
	}
}
