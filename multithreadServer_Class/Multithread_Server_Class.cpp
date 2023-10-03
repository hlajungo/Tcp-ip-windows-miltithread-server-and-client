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
			clientName[i] = "unknown";
			clientSocket[i] = INVALID_SOCKET;
			clientNumber[i] = -1;
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
	int ControlPanel(Server &s)
	{
		std::cout << "[伺服器]\n";
		std::cout << "------功能介紹------\n";
		std::cout << "以編號查詢客戶端信息		輸入: search_client_by_number [0-20之間的數字]\n";
		std::cout << "遍歷客戶端信息			輸入: traversal_client\n";
		std::cout << "移除客戶端			輸入: kick [[0-20之間的數字]\n";
		std::cout << "關掉伺服器			輸入: server_close \n";
		std::cout << "--------------------\n";
		std::cout << "請輸入 : ";

		while (std::cin >> s.command_char)
		{
			//所有send去client的, client也會有一套處理系統
			switch (hash_(s.command_char))
			{
			case "server_close"_hash:
				std::cout << "[控制面板] : 結束所有通訊\n\n";
				CloseServer(s);
				return 0;

				break;
			case "search_client_by_number"_hash:
				std::cin >> s.command_int;
				if (s.clientNumber[s.command_int] == -1)
				{
					std::cout << "你要求的客戶端編號尚未使用...... \n";
					std::cout << "請輸入 : ";
					break;
				}
				std::cout << "[系統]編號"<<s.command_int << "的編號		: " << s.clientNumber[command_int] << "\n";
				std::cout << "[系統]編號"<<s.command_int << "的名稱		: " << s.clientName[command_int] << "\n";
				std::cout << "[系統]編號"<<s.command_int << "的IP			: " << s.clientIP[command_int] << "\n";
				std::cout << "[系統]編號"<<s.command_int << "的Port		: " << s.clientPort[command_int] << "\n\n";
				std::cout << "請輸入 : ";

				break;
			case "traversal_client"_hash:
				if (s.CurrentClientNumber == 0)
				{
					std::cout << "目前沒有客戶端......";
					break;
				}
				std::cout << "目前有" << s.CurrentClientNumber << " / 20個客戶端使用中......\n";
				for (int i = 0; i < s.CurrentClientNumber; i++)
				{
					std::cout << "[系統]編號" << i << "的編號		: " << s.clientNumber[i] << "\n";
					std::cout << "[系統]編號" << i << "的名稱		: " << s.clientName[i] << "\n";
					std::cout << "[系統]編號" << i << "的IP			: " << s.clientIP[i] << "\n";
					std::cout << "[系統]編號" << i << "的Port		: " << s.clientPort[i] << "\n\n";
				}
				std::cout << "請輸入 : ";

				break;
			case "kick"_hash:
				std::cin >> s.command_int;
				if (s.clientNumber[s.command_int] == -1)
				{
					std::cout << "你要求的客戶端編號尚未使用...... \n";
					std::cout << "請輸入 : ";
					break;
				}
				s.Send_Buffer[s.command_int] = "kick";
				bytesSend = send(s.clientSocket[command_int], (s.Send_Buffer[command_int].c_str()), (int)strlen(s.Send_Buffer[command_int].c_str()), 0);
				if (bytesSend == -1)
				{
					std::cout << "send failed with error: " << WSAGetLastError() << "\n";
					closesocket(s.clientSocket[command_int]);
					WSACleanup();
					return 1;
				}
				iResult = shutdown(s.clientSocket[command_int], SD_RECEIVE);
				if (iResult == SOCKET_ERROR) {
					std::cout << "shutdown failed with error: " << WSAGetLastError() << "\n";
					closesocket(s.clientSocket[command_int]);
					WSACleanup();
					return 1;
				}
				closesocket(clientSocket[command_int]);


				break;
			default:
				std::cout << "[控制面板] : 輸入錯誤或無此指令...\n";
				std::cout << "請輸入 : ";
			}
		}
	}

	//Server::CloseServer()
	int CloseServer(Server &s)
	{
		closesocket(this->listenSocket);
		for (int i = 0; i < 10; i++)
		{
			iResult = shutdown(s.clientSocket[i], SD_BOTH);
			if (iResult == SOCKET_ERROR) {
				std::cout << "shutdown failed with error: " << WSAGetLastError() << "\n";
				closesocket(s.clientSocket[command_int]);
				WSACleanup();
				return 1;
			}
			closesocket(clientSocket[i]);
		}
		WSACleanup();

		return 0;
	}

	//Server::Search_Client_By_Number
	int Search_Client_By_Number(Server s)
	{

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
	char* clientIP[20];								//用戶IP
	int clientPort[20];								//用戶port 
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

//clientIO (thread callback)
int clientRevc(Server &s)
{
	char Recv_Buffer[1024];
	int i = s.CurrentClientNumber;
	while (true) {
		memset(Recv_Buffer, 0, sizeof(Recv_Buffer));
		int bytesRead = recv(s.clientSocket[i], Recv_Buffer, sizeof(Recv_Buffer), 0);
		if (bytesRead == -1)
		{
			std::cout << "\n[系統][第" << i << "號][" << s.clientName[i] << "] : 異常斷開\n\n";
			std::cout << "\n請輸入: " << std::endl;
			closesocket(s.clientSocket[i]);
			return 1;
		}

		Recv_Buffer[bytesRead] = '\0';
		switch (hash_(Recv_Buffer))
		{
		case "personal_information"_hash:
			std::cout << "\n[系統][第" << i << "號][" << s.clientName[i] << "] : 請求個人資訊\n\n";
			std::cout << "請輸入 : ";

			 s.Send_Buffer[i] = "[系統]你的編號	 : " + std::to_string(s.clientNumber[i]) + "\n[系統]你的名稱	 : " + s.clientName[i] + "\n[系統]你的IP	: " + s.clientIP[i] + "\n[系統]你的Port	 : " + std::to_string(s.clientPort[i]) + "\n\n";
			//傳輸 編號  名稱 IP Port
			s.bytesSend = send(s.clientSocket[i], (s.Send_Buffer[i].c_str()), (int)strlen(s.Send_Buffer[i].c_str()), 0);
			if(s.bytesSend == -1)
			{
				std::cout << "send failed with error: " << WSAGetLastError() << "\n";
				closesocket(s.clientSocket[i]);
				WSACleanup();
				return 1;
			}
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
Server acceptClient(Server &s)
{
	while (1)
	{
		//開始等待客戶端, 等到後傳入
		struct sockaddr_in  client_addr;
		int clientAddrSize = sizeof(client_addr);
		s.clientSocket[s.CurrentClientNumber] = accept(s.listenSocket, (SOCKADDR*)&client_addr, &clientAddrSize);
		if (s.clientSocket[s.CurrentClientNumber] == INVALID_SOCKET)
		{
			std::cout << "\nsocket accept failed with error: " << WSAGetLastError() << "\n\n";
			exit(0);
		}
		s.clientNumber[s.CurrentClientNumber] = s.CurrentClientNumber;
		s.clientIP[s.CurrentClientNumber] = inet_ntoa(client_addr.sin_addr);
		s.clientPort[s.CurrentClientNumber] = ntohs(client_addr.sin_port);
		
		std::cout << "\n[系統][客戶接收] : 第" << s.CurrentClientNumber << "個客戶端加入了\n\n";
		std::cout << "請輸入 : ";
		std::thread clientIOa(clientRevc, std::ref(s));
		clientIOa.detach();
		Sleep(1);
		s.CurrentClientNumber += 1;
	}
}

int main()
{
	Server s("192.168.1.200", "8080");
	s.ServerInit();
	std::thread clientThread(acceptClient, std::ref(s));
	clientThread.detach();
	s.ControlPanel(s);

	return 0;
}

