#pragma once

#ifndef Client_Class
#define Client_Class

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <thread>

//link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

typedef std::uint64_t hash_t;
constexpr static hash_t prime = 0x100000001B3ull;
constexpr static hash_t basis = 0xCBF29CE484222325ull;

class Client
{
public:
	//Client::Client() _�c�y���
	Client(const char* ip, const char* port);

	//Client::ClientInit() _��l��
	int ClientInit();

	//Client::clientControlPanel() _�e�ΥD�u�{ �B�z�Τ��J
	int clientControlPanel(Client& c);

	//Client::serverRecv() _���l�u�{�^�� �������A���^��
	int serverRecv(Client& c);

	//Client:: Client_Instructions() _���ѫȤ�ݻ�����r
	void Client_Instructions();

	//Client::Client_Close() __����client
	int Client_Close();

	//Client::Client_Send_To_Server() _�ǿ鵹Server
	int Client_Send_To_Server();

public:
	int iResult;
	int bytesRead;
	SOCKET ConnectSocket = INVALID_SOCKET;
	char recvbuf[1024];
	
private:
	WSADATA wsaData;
	struct addrinfo* result = NULL, * ptr = NULL, hints;
	char sendbuf[1024];
	const char* Server_IP;
	const char* Server_Port;
};


//global _�B�zswitch����J���D
inline hash_t hash_(char const* str)
{
	hash_t ret{ basis };
	while (*str) {
		ret ^= *str;
		ret *= prime;
		str++;
	}

	return ret;
}

//global _�B�zswitch����J���D
constexpr hash_t hash_compile_time(char const* str, hash_t last_value = basis)
{
	return *str ? hash_compile_time(str + 1, (*str ^ last_value) * prime) : last_value;
}

//global _�B�zswitch����J���D
constexpr unsigned long long operator "" _hash(char const* p, size_t)
{
	return hash_compile_time(p);
}

#endif // ClientClass_H