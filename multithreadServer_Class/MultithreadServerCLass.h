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

	//Server::Server() _構造函數
	Server(const char* ip, const char* port);
	
	//Server::ServerInit() _初始化
	int ServerInit();

	//Server::ControlPanel()  _占用主線程 處理用戶輸入
	int ControlPanel(Server& s);
	
	//Server::CloseServer() _關閉伺服器
	int CloseServer(Server& s);
	
	//Server::clientRevc() _給子線程回調 接收客戶端回傳
	int clientRevc(Server& s);
	
	//Server::acceptClient() _給子線程回調 接收客戶端加入
	Server acceptClient(Server& s);
	
	//Server:: Client_Instructions() _提供伺服器說明文字
	void Server_Instruction();

	//Server::PrintClientInfo() _打印客戶端信息
	void PrintClientInfo(Server& s, int i);

	//Server::search_client_by_number() _用編號查詢客戶端
	int search_client_by_Client_Number(Server& s);

	//Server::traversal_client() _遍歷客戶端
	int traversal_client(Server& s);
		
	//Server::kick() _踢除客戶端
	int kick_by_Client_Number(Server& s);

	//Server::send_to_client() _傳訊息給指定客戶端
	int send_to_client_by__Client_Number(Server& s);

	//Server::send_to_all_client() _傳訊息給所有客戶端
	int send_to_all_client(Server& s);


	//-------------客戶端處理
	
	//Server::client_personal_information() _處理客戶端要求信息
	int client_personal_information(Server& s, int i);

	//Server::client_client_close() _處理客戶端關閉
	int client_client_close(Server& s, int i);

	//Server::client_send_to_server _處理客戶端傳送給伺服器
	int client_send_to_server(Server &s, int i);

	//Server::client_send_to_client _處理客戶端傳送至客戶端訊息
	int client_send_to_client(Server &s, int i);

	//Server::client_set_name() _處理客戶端改名
	int client_set_name(Server& s, int i);

	//---------------一些工具
	bool isClientNumberAccessible(Server &s, int i);
	
	void code_information(const char* a, const char* b, int c);

public:
	const char* Server_IP;
	const char* Server_Port;
	int CurrentClientNumber = 0;
	SOCKET listenSocket = INVALID_SOCKET;
	SOCKET clientSocket[20];						//用戶Socket
	std::string clientName[20];						//用戶名稱 默認unknown
	int clientNumber[20];							//用戶編號 默認-1
	char* clientIP[20];								//用戶IP
	int clientPort[20];								//用戶port 
	char char_clientBuffer[20][1024];				//用戶char_buffer
	int int_clientBuffer[20];						//用戶int_buffer

private:
	int int_result;									//結果判斷
	int int_server_input;							//伺服器輸入
	char command_char[1024];						//伺服器輸入
	std::string message;							//指令buffer
	struct addrinfo* result = NULL;
	struct addrinfo hints;
	WSADATA wsaData;
	int optval = 1;
	
};


//global _處理switch的輸入問題
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

//global _處理switch的輸入問題
constexpr hash_t hash_compile_time(char const* str, hash_t last_value = basis)
{
	return *str ? hash_compile_time(str + 1, (*str ^ last_value) * prime) : last_value;
}

//global _處理switch的輸入問題
constexpr unsigned long long operator "" _hash(char const* p, size_t)
{
	return hash_compile_time(p);
}

#endif // Server_Class_H