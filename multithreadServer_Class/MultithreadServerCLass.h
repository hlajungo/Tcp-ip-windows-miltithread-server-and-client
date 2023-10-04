#pragma once

#ifndef Server_Class
#define Server_Class

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <string>
#include <thread>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")

typedef std::uint64_t hash_t;
constexpr static hash_t prime = 0x100000001B3ull;
constexpr static hash_t basis = 0xCBF29CE484222325ull;

class Server
{
public:
	//Server::Server()
	Server(const char* ip, const char* port);
	

	//Server::ServerInit() _��l��
	int ServerInit();

	//Server::ControlPanel()  _�e�ΥD�u�{ �B�z�Τ��J
	int ControlPanel(Server& s);
	
	//Server::CloseServer() _�������A��
	int CloseServer(Server& s);
	
	//Server::clientRevc() _���l�u�{�^�� �����Ȥ�ݦ^��
	int clientRevc(Server& s);
	
	//Server::acceptClient() _���l�u�{�^�� �����Ȥ�ݥ[�J
	Server acceptClient(Server& s);
	
	//Server:: Client_Instructions() _���Ѧ��A��������r
	void Server_Instruction();

	//Server::PrintClientInfo() _���L�Ȥ�ݫH��
	void PrintClientInfo(Server& s, int i);

	//Server::search_client_by_number() _�νs���d�߫Ȥ��
	int search_client_by_number(Server& s);

	//Server::traversal_client() _�M���Ȥ��
	int traversal_client(Server& s);
		
	//Server::kick() _�𰣫Ȥ��
	int kick(Server& s);

	//Server::client_personal_information() _�B�z�Ȥ�ݫH���n�D
	int client_personal_information(Server& s, int i);

	//Server::client_client_close() _�B�z�Ȥ�������n�D
	int client_client_close(Server& s, int i);

public:
	const char* Server_IP;
	const char* Server_Port;
	int CurrentClientNumber = 0;
	SOCKET listenSocket = INVALID_SOCKET;
	SOCKET clientSocket[20];						//�Τ�Socket
	std::string clientName[20];						//�Τ�W�� �q�{unknown
	int clientNumber[20];							//�Τ�s�� �q�{-1
	char* clientIP[20];								//�Τ�IP
	int clientPort[20];								//�Τ�port 
	std::string Send_Buffer[20];
	int bytesSend;

private:
	int iResult;
	struct addrinfo* result = NULL;
	struct addrinfo hints;
	WSADATA wsaData;
	int optval = 1;
	char command_char[1024];
	int command_int;
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

#endif // Server_Class_H