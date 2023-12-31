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
	//Client::Client() _構造函數
	Client(const char* ip, const char* port);

	//Client::ClientInit() _初始化
	int ClientInit();

	//Client::clientControlPanel() _占用主線程 處理用戶輸入
	int clientControlPanel(Client& c);

	//Client::serverRecv() _給子線程回調 接收伺服器回傳
	int serverRecv(Client& c);

	//Client:: Client_Instructions() _提供客戶端說明文字
	void Client_Instructions();

	//Client::Client_Close() __關閉client
	int Client_Close();

	//Client::Client_Send_To_Server() _傳輸訊息給Server
	int Client_Send_To_Server();

	//Client::set_name() _設置名稱
	int set_name();

	//Client::send_to_client() _傳送訊息給其他客戶端
	int send_to_client();

	//----------

	//Client::ServerRecv_send_to_client _處理伺服器傳送他人傳送過來的訊息
	int ServerRecv_send_to_client();

public:
	SOCKET ConnectSocket = INVALID_SOCKET;
	
private:
	int input_int;
	int int_result;
	char buffer[1024];
	WSADATA wsaData;
	struct addrinfo* result = NULL, * ptr = NULL, hints;
	const char* Server_IP;
	const char* Server_Port;
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

//global hash_compile_time _處理switch的輸入問題
constexpr hash_t hash_compile_time(char const* str, hash_t last_value = basis)
{
	return *str ? hash_compile_time(str + 1, (*str ^ last_value) * prime) : last_value;
}

//global _處理switch的輸入問題
constexpr unsigned long long operator "" _hash(char const* p, size_t)
{
	return hash_compile_time(p);
}

#endif // ClientClass_H