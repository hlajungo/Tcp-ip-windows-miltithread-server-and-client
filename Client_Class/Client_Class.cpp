#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

//使switch可使用string
typedef std::uint64_t hash_t;
constexpr hash_t prime = 0x100000001B3ull;
constexpr hash_t basis = 0xCBF29CE484222325ull;
hash_t hash_(char const* str)
{
	hash_t ret{ basis };

	while (*str) {
		ret ^= *str;
		ret *= prime;
		str++;
	}

	return ret;
}
constexpr hash_t hash_compile_time(char const* str, hash_t last_value = basis)
{
	return *str ? hash_compile_time(str + 1, (*str ^ last_value) * prime) : last_value;
}
constexpr unsigned long long operator "" _hash(char const* p, size_t)
{
	return hash_compile_time(p);
}




class Client
{
public:
	Client(const char* ip, const char* port)
	{
		this->Server_IP = ip;
		this->Server_Port = port;
	}

	int ClientInit()
	{
		iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0) {
			std::cout << "WSAStartup failed with error: " << iResult << "\n";
			return 1;
		}
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		iResult = getaddrinfo(Server_IP, Server_Port, &hints, &result);
		if (iResult != 0) {
			std::cout << "getaddrinfo failed with error: " << iResult << "\n";
			WSACleanup();
			return 1;
		}
		for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
			ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
				ptr->ai_protocol);
			if (ConnectSocket == INVALID_SOCKET) {
				std::cout << "socket failed with error: " << WSAGetLastError() << "\n";
				WSACleanup();
				return 1;
			}
			iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
			if (iResult == SOCKET_ERROR) {
				closesocket(ConnectSocket);
				ConnectSocket = INVALID_SOCKET;
				continue;
			}
			break;
		}
		freeaddrinfo(result);
		if (ConnectSocket == INVALID_SOCKET) {
			std::cout << "Unable to connect to server!\n";
			WSACleanup();
			return 1;
		}
	}

	int clientControlPanel()
	{
		// 開始傳輸
		std::cout << "[客戶端]\n";
		std::cout << "------功能介紹------\n";
		std::cout << "關掉客戶端 輸入: client_close \n";
		std::cout << "--------------------\n";
		std::cout << "請輸入 : ";
		//data會傳送後由server處理, 並且client也會有一套處理系統
		while (std::cin >> sendbuf)
		{
			iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
			if (iResult == SOCKET_ERROR)
			{
				std::cout << "send failed with error: " << WSAGetLastError() << "\n";
				closesocket(ConnectSocket);
				WSACleanup();
				return 1;
			}

			switch (hash_(sendbuf))
			{
			case "client_close"_hash:
				iResult = shutdown(ConnectSocket, SD_BOTH);
				if (iResult == SOCKET_ERROR) {
					std::cout << "shutdown failed with error: " << WSAGetLastError() << "\n";
					closesocket(ConnectSocket);
					WSACleanup();
					return 1;
				}
				closesocket(ConnectSocket);
				WSACleanup();
				std::cout << "\n[控制面板] : 結束通訊...\n";

				return 0;
				break;
			case "send"_hash:
				std::cout << "請輸入要傳輸的內容 : ";
				std::cin >> sendbuf;
				iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
				if (iResult == SOCKET_ERROR)
				{
					std::cout << "send failed with error: " << WSAGetLastError() << "\n";
					closesocket(ConnectSocket);
					WSACleanup();
					return 1;
				}
				break;
			case "third"_hash:
				std::cout << "3rd one\n";
				break;
			default:
				std::cout << "\n請輸入 : ";
				break;
			}

		}

	}

public:

private:
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo* result = NULL, * ptr = NULL, hints;
	int iResult;
	char sendbuf[1024];
	const char* Server_IP;
	const char* Server_Port;


};

int main()
{
	Client c("192.168.1.200", "8080");
	c.ClientInit();
	c.clientControlPanel();

	return 0;
}

