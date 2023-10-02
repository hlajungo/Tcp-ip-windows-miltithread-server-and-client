#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <thread>

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

	//Client::Client()
	Client(const char* ip, const char* port)
	{
		this->Server_IP = ip;
		this->Server_Port = port;
	}

	//Client::ClientInit()
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

	//Client::clientControlPanel()
	int clientControlPanel(Client& c)
	{
		// 開始傳輸
		std::cout << "[客戶端]\n";
		std::cout << "------功能介紹------\n";
		std::cout << "顯示個人資訊		輸入 : personal_information \n";
		std::cout << "更改名稱		輸入 : set_name [名稱]\n";
		std::cout << "關掉客戶端		輸入: client_close \n";
		std::cout << "--------------------\n";
		std::cout << "請輸入 : ";
		//data會傳送後由server處理, 並且client也會有一套處理系統, recv有額外處理系統
		while (std::cin.getline(sendbuf, sizeof(sendbuf)))
		{
			memset(recvbuf, 0, sizeof(recvbuf));
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
			case"personal_information"_hash:


				break;
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
				std::cout << "\n[系統] : 結束通訊...\n";

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
			default:
				std::cout << " 輸入錯誤或無此指令\n";
				std::cout << "\n請輸入 : ";
				break;
			}

		}

	}

public:
	int iResult;
	SOCKET ConnectSocket = INVALID_SOCKET;
	char recvbuf[1024];
	int bytesRead;
private:
	WSADATA wsaData;
	struct addrinfo* result = NULL, * ptr = NULL, hints;
	char sendbuf[1024];
	const char* Server_IP;
	const char* Server_Port;


};

int serverRecv(Client& c)
{
	while (true)
	{
		c.bytesRead = recv(c.ConnectSocket, c.recvbuf, sizeof(c.recvbuf), 0);
		if (c.bytesRead == -1)
		{
			std::cout << "recv failed with error: " << WSAGetLastError() << "\n";
			closesocket(c.ConnectSocket);
			WSACleanup();
			return 1;
		}
		c.recvbuf[c.bytesRead] = '\0';
		switch (hash_(c.recvbuf))
		{
		case "kick"_hash:
			std::cout << "\n[系統][Server回傳] : 你被踢除了...\n\n";
			c.iResult = shutdown(c.ConnectSocket, SD_BOTH);
			if (c.iResult == SOCKET_ERROR) {
				std::cout << "shutdown failed with error: " << WSAGetLastError() << "\n";
				closesocket(c.ConnectSocket);
				WSACleanup();
				return 1;
			}
			closesocket(c.ConnectSocket);
			WSACleanup();
			system("pause");
			exit(0);
			break;
		default:
			std::cout << "\n[系統][Server回傳] : Server回傳錯誤或無此指令...\n\n";
			std::cout << "\n請輸入 : ";
		}

		std::cout << c.recvbuf;
		std::cout << "請輸入:";
	}
}

int main()
{
	Client c("192.168.1.200", "8080");
	c.ClientInit();
	std::thread serverRecva(serverRecv, std::ref(c));
	serverRecva.detach();
	c.clientControlPanel(c);
	

	return 0;
}

