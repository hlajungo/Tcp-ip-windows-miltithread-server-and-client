#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <string>
#include <thread>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <vector>
#pragma comment (lib, "Ws2_32.lib")

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

char* hexToCharIP(struct in_addr addrIP);


class Server
{
public:

	//Server::Server()
	Server(const char* ip, const char* port)
	{
		this->Server_IP = ip;
		this->Server_Port = port;
		for (int i = 0; i < 10; i++)
		{
			this->clientName[i] = "unknown";
			this->clientSocket[i] = INVALID_SOCKET;
			this->clientNumber[i] = -1;
		}
	}

	//Server::ServerInit()
	int ServerInit()
	{
		iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0) {
			std::cout << "WSAStartup failed with error: " << iResult << "\n";
			return 1;
		}
		memset((&hints), 0, (sizeof(hints)));
		memset(&result, 0, sizeof(result));

		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;
		iResult = getaddrinfo(Server_IP, Server_Port, &hints, &result);
		if (iResult != 0) {
			std::cout << "getaddrinfo failed with error: " << iResult << "\n";
			WSACleanup();
			return 1;
		}
		listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (listenSocket == INVALID_SOCKET) {
			std::cout << "socket failed with error: " << WSAGetLastError() << "\n";
			freeaddrinfo(result);
			WSACleanup();
			return 1;
		}
		iResult = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			std::cout << "bind failed with error: " << WSAGetLastError() << "\n";
			freeaddrinfo(result);
			closesocket(listenSocket);
			WSACleanup();
			return 1;
		}
		freeaddrinfo(result);
		iResult = listen(listenSocket, SOMAXCONN);
		if (iResult == SOCKET_ERROR) {
			std::cout << "listen failed with error: " << WSAGetLastError() << "\n";
			closesocket(listenSocket);
			WSACleanup();
			return 1;
		}
		iResult = setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval));
		if (iResult == SOCKET_ERROR) {
			std::cout << "setsockopt failed with error: " << WSAGetLastError() << "\n";
			closesocket(listenSocket);
			WSACleanup();
		}

	}

	//Server::ControlPanel()
	int ControlPanel()
	{
		std::cout << "[伺服器]\n";
		std::cout << "------功能介紹------\n";
		std::cout << "關掉伺服器 輸入: server_close \n";
		std::cout << "--------------------\n";
		std::cout << "請輸入 : ";
		char command[1024];
		while (std::cin >> command)
		{
			switch (hash_(command))
			{
			case "server_close"_hash:
				std::cout << "[控制面板] : 結束所有通訊\n\n";
				CloseServer();
				return 0;
				break;
			case "second"_hash:
				std::cout << "2nd one\n";
				break;
			case "third"_hash:
				std::cout << "3rd one\n";
				break;
			default:
				std::cout << "[控制面板] : 輸入錯誤或無此指令...\n";
				std::cout << "請輸入 : ";
			}
		}
	}

	//Server::CloseServer()
	int CloseServer()
	{
		closesocket(this->listenSocket);
		for (int i = 0; i < 10; i++)
		{
			closesocket(clientSocket[i]);
		}
		WSACleanup();

		return 0;
	}

public:
	const char* Server_IP;
	const char* Server_Port;
	int CurrentClientNumber = 0;
	SOCKET listenSocket = INVALID_SOCKET;
	SOCKET clientSocket[20];						//用戶Socket
	std::string clientName[20];						//用戶名稱 默認unknown
	int clientNumber[20];							//用戶編號 默認-1
	char* clientIP[20];							//用戶IP
	int clientPort[20];								//用戶port 

private:
	int iResult;
	bool isCloseServer = false;
	struct addrinfo* result = NULL;
	struct addrinfo hints;
	WSADATA wsaData;
	int optval = 1;
};



//clientIO (thread callback)
int clientIO(Server s)
{
	char buffer[1024];
	int i = s.CurrentClientNumber;
	while (true) {
		memset(buffer, 0, sizeof(buffer));
		int bytesRead = recv(s.clientSocket[i], buffer, sizeof(buffer), 0);
		if (bytesRead == -1)
		{
			std::cout << "\n[系統][第" << i << "號][" << s.clientName[i] << "] : 異常斷開\n\n";
			std::cout << "\n請輸入: " << std::endl;
			closesocket(s.clientSocket[i]);
			return 1;
		}

		buffer[bytesRead] = '\0';
		switch (hash_(buffer))
		{
		case "personal_information"_hash:
			//傳輸 編號  名稱 IP Port
			std::cout << "\n[系統][第" << i << "號][" << s.clientName[i] << "] : 請求個人資訊\n\n";
			send(s.clientSocket[i], s.clientIP[i], (int)strlen(s.clientIP[i]), 0);
			break;
		case "client_close"_hash:
			std::cout << "\n[系統][第" << i << "號][" << s.clientName[i] << "] : 斷開連接\n\n";
			std::cout << "\n請輸入: ";
			shutdown(s.clientSocket[i], SD_BOTH);
			closesocket(s.clientSocket[i]);
			return 0;
			break;
		case "send"_hash:
			std::cout << "\n[系統][第" << i << "號][" << s.clientName[i] << "]說 : \n\n";
			break;
		default:
			std::cout << "\n[第" << i << "號client回傳] : 輸入錯誤或無此指令...\n\n";
			std::cout << "\n請輸入 : ";
		}
	}
}

//acceptClient (thread callback)
int acceptClient(Server s)
{
	char client_ip[INET_ADDRSTRLEN];
	while (1)
	{
		//開始等待客戶端, 等到後傳入
		struct sockaddr_in  client_addr;
		int clientAddrSize = sizeof(client_addr);
		s.clientSocket[s.CurrentClientNumber] = accept(s.listenSocket, (SOCKADDR*)&client_addr, &clientAddrSize);
		if (s.clientSocket[s.CurrentClientNumber] == INVALID_SOCKET)
		{
			std::cout << "\nsocket accept failed with error: " << WSAGetLastError() << "\n\n";
			return 1;
		}
		s.clientNumber[s.CurrentClientNumber] = s.CurrentClientNumber;
		s.clientIP[s.CurrentClientNumber] = inet_ntoa(client_addr.sin_addr);
		s.clientPort[s.CurrentClientNumber] = ntohs(client_addr.sin_port);
		std::cout << "\n[系統][客戶接收] : 第" << s.CurrentClientNumber << "個客戶端加入了\n\n";
		std::thread clientIOa(clientIO, s);
		clientIOa.detach();
		s.CurrentClientNumber += 1;
	}
}

int main()
{
	Server s("192.168.1.200", "8080");
	s.ServerInit();
	std::thread clientThread(acceptClient, s);
	clientThread.detach();
	s.ControlPanel();

	return 0;
}


