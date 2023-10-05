#pragma once

#include "MultithreadServerCLass.h"

//----------------

//Server::ControlPanel() __占用主線程 處理用戶輸入
int Server::ControlPanel(Server& s)
{
	Server_Instruction();
	while (std::cin >> s.command_char)
	{
		//所有send去client的, client也會有一套處理系統
		switch (hash_(s.command_char))
		{
		case "功能介紹"_hash:
			Server_Instruction();
			break;
		case "server_close"_hash:
			CloseServer(s);
			return 0;

			break;
		case "search_client_by_number"_hash:
			search_client_by_Client_Number(s);
			break;
		case "traversal_client"_hash:
			traversal_client(s);

			break;
		case "kick"_hash:
			kick_by_Client_Number(s);

			break;
		case "send"_hash:
			send_to_client_by__Client_Number(s);
			
			break;
		case "send_all"_hash:
			send_to_all_client(s);

			break;
		default:
			std::cout << "[控制面板] : 輸入錯誤或無此指令...\n";
			std::cout << "請輸入 : ";
		}
	}

	return 0;
}

//Server::clientRevc() _給子線程回調 接收客戶端回傳
int Server::clientRevc(Server& s)
{
	//char clientRecvBuffer[1024];
	int i = s.CurrentClientNumber;
	while (true) {
		memset(char_clientBuffer[i], 0, sizeof(char_clientBuffer[i]));
		int_result = recv(s.clientSocket[i], char_clientBuffer[i], sizeof(char_clientBuffer[i]), 0);
		if (int_result == -1)
		{
			std::cout << "\n[系統][第" << i << "號][" << s.clientName[i] << "] : 異常斷開\n\n";
			std::cout << "\n請輸入: " << std::endl;
			closesocket(s.clientSocket[i]);
			return 1;
		}
		
		char_clientBuffer[i][int_result] = '\0';
		switch (hash_(char_clientBuffer[i]))
		{
		case "personal_information"_hash:
			client_personal_information(s, i);

			break;
		case "client_close"_hash:
			client_client_close(s, i);

			break;
		case "send_to_server"_hash:
			client_send_to_server(s, i);

			break;
		case "send_to_client"_hash:
			client_send_to_client(s, i);

			break;
		case "set_name"_hash:
			client_set_name(s, i);

			break;
		default:
			std::cout << "\n[第" << i << "號client回傳] : 輸入錯誤或無此指令...\n\n";
			std::cout << "\n請輸入 : ";
		}
	}
}

//Server:: Client_Instructions() _提供伺服器說明文字
void Server::Server_Instruction()
{
	std::cout << "[伺服器]\n";																		//
	std::cout << "------功能介紹------\n";															//
	std::cout << "伺服器功能介紹			輸入: 功能介紹\n";										//
	std::cout << "以編號查詢客戶端信息		輸入: search_client_by_number [0-20之間的數字]\n";		//
	std::cout << "遍歷客戶端信息			輸入: traversal_client\n";								//
	std::cout << "移除客戶端			輸入: kick [編號0~20]\n";									//
	std::cout << "關掉伺服器			輸入: server_close \n";										//
	std::cout << "--------------------\n";															//
	std::cout << "以編號和指定客戶端說話		輸入: send [編號0~20] [句子]\n";					//
	std::cout << "發句子給所有客戶端		輸入: send_all [句子]\n";								//
	std::cout << "--------------------\n";
	std::cout << "請輸入 : ";
}

//----------------

//Server::acceptClient() _給子線程回調 接收客戶端加入
Server Server::acceptClient(Server& s)
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
		std::thread clientRecvThread([&s]() {
			s.clientRevc(s);
			});
		clientRecvThread.detach();
		Sleep(1);
		s.CurrentClientNumber += 1;
	}
}

//Server::Server() _構造函數
Server::Server(const char* ip, const char* port)
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

//Server::ServerInit() _初始化
int Server::ServerInit()
{
	int_result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (int_result != 0) {
		std::cout << "WSAStartup failed with error: " << int_result << "\n";
		return 1;
	}
	memset((&hints), 0, (sizeof(hints)));
	memset(&result, 0, sizeof(result));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	int_result = getaddrinfo(Server_IP, Server_Port, &hints, &result);
	if (int_result != 0) {
		std::cout << "getaddrinfo failed with error: " << int_result << "\n";
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
	int_result = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (int_result == SOCKET_ERROR) {
		std::cout << "bind failed with error: " << WSAGetLastError() << "\n";
		freeaddrinfo(result);
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}
	freeaddrinfo(result);
	int_result = listen(listenSocket, SOMAXCONN);
	if (int_result == SOCKET_ERROR) {
		std::cout << "listen failed with error: " << WSAGetLastError() << "\n";
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}
	int_result = setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval));
	if (int_result == SOCKET_ERROR) {
		std::cout << "setsockopt failed with error: " << WSAGetLastError() << "\n";
		closesocket(listenSocket);
		WSACleanup();
	}

	return 0;
}

//Server::CloseServer() _關閉伺服器
int Server::CloseServer(Server& s)
{
	closesocket(this->listenSocket);
	for (int i = 0; i < 10; i++)
	{
		int_result = shutdown(s.clientSocket[i], SD_BOTH);
		if (int_result == SOCKET_ERROR) {
			std::cout << "shutdown failed with error: " << WSAGetLastError() << "\n";
			closesocket(s.clientSocket[int_server_input]);
			WSACleanup();
			return 1;
		}
		closesocket(clientSocket[i]);
	}
	WSACleanup();

	return 0;
}

//Server::PrintClientInfo() _打印客戶端信息
void Server::PrintClientInfo(Server &s, int i)
{
	std::cout << "[系統]編號" << i << "的編號		: " << s.clientNumber[i] << "\n";
	std::cout << "[系統]編號" << i << "的名稱		: " << s.clientName[i] << "\n";
	std::cout << "[系統]編號" << i << "的IP			: " << s.clientIP[i] << "\n";
	std::cout << "[系統]編號" << i << "的Port		: " << s.clientPort[i] << "\n\n";
}

//Server::search_client_by_number() _用編號查詢客戶端
int Server::search_client_by_Client_Number(Server &s)
{
	std::cin >> s.int_server_input;
	if (isClientNumberAccessible(s, int_server_input) == false)return 1;
	PrintClientInfo(s, int_server_input);
	std::cout << "請輸入 : ";

	return 0;
}

//Server::traversal_client() _遍歷客戶端
int Server::traversal_client(Server& s)
{
	if (s.CurrentClientNumber == 0)
	{
		std::cout << "目前沒有客戶端......";
		return 1;
	}
	std::cout << "目前有" << s.CurrentClientNumber << " / 20個客戶端使用中......\n";
	for (int i = 0; i < s.CurrentClientNumber; i++)
	{
		PrintClientInfo(s, i);
	}
	std::cout << "請輸入 : ";

	return 0;
}

//Server::kick() _踢除客戶端
int Server::kick_by_Client_Number(Server& s)
{
	std::cin >> s.int_server_input;
	if (isClientNumberAccessible(s, int_server_input) == false)return 1;

	std::string message = "kick";
	strcpy_s(char_clientBuffer[int_server_input], message.c_str());
	int_result = send(s.clientSocket[s.int_server_input], char_clientBuffer[s.int_server_input], (int)strlen(char_clientBuffer[s.int_server_input]), 0);
	if (int_result == -1)
	{
		std::cout << "send failed with error: " << WSAGetLastError() << "\n";
		closesocket(s.clientSocket[int_server_input]);
		WSACleanup();
		return 1;
	}
	int_result = shutdown(s.clientSocket[int_server_input], SD_RECEIVE);
	if (int_result == SOCKET_ERROR) {
		std::cout << "shutdown failed with error: " << WSAGetLastError() << "\n";
		closesocket(s.clientSocket[int_server_input]);
		WSACleanup();
		return 1;
	}
	closesocket(clientSocket[int_server_input]);


	return 0;
}

//Server::send_to_client() _傳訊息給指定客戶端
int Server::send_to_client_by__Client_Number(Server& s)
{
	std::cin >> int_server_input;
	if (isClientNumberAccessible(s, int_server_input) == false)return 1;
	std::cin.getline(command_char, sizeof(command_char));
	int_result = send(clientSocket[int_server_input], command_char, (int)strlen(command_char), 0);
	if (int_result == -1)
	{
		std::cout << "send failed with error: " << WSAGetLastError() << "\n";
		closesocket(clientSocket[int_server_input]);
		WSACleanup();
		return 1;
	}
	std::cout << "\n請輸入 : ";

	return 0;
}

//Server::send_to_all_client() _傳訊息給所有客戶端
int Server::send_to_all_client(Server& s)
{
	std::cin.getline(command_char, sizeof(command_char));
	for (int i = 0; i < s.CurrentClientNumber; i++)
	{
		int_result = send(clientSocket[i], command_char, (int)strlen(command_char), 0);
		if (int_result == -1)
		{
			std::cout << "send failed with error: " << WSAGetLastError() << "\n";
			closesocket(clientSocket[int_server_input]);
			WSACleanup();
			return 1;
		}
	}
	std::cout << "\n請輸入 : ";

	return 0;
}

//-----------------

//Server::client_personal_information() _處理客戶端信息
int Server::client_personal_information(Server& s, int i)
{
	std::cout << "\n[系統][第" << i << "號][" << s.clientName[i] << "] : 請求個人資訊\n\n";
	std::cout << "請輸入 : ";

	//傳輸 編號  名稱 IP Port
	std::string message = "[系統]你的編號	 : " + std::to_string(s.clientNumber[i]) + "\n[系統]你的名稱	 : " + s.clientName[i] + "\n[系統]你的IP	: " + s.clientIP[i] + "\n[系統]你的Port	 : " + std::to_string(s.clientPort[i]) + "\n\n";
	strcpy_s(char_clientBuffer[i], message.c_str());

	s.int_result = send(s.clientSocket[i], char_clientBuffer[i], (int)strlen(char_clientBuffer[i]), 0);
	if (s.int_result == -1)
	{
		std::cout << "send failed with error: " << WSAGetLastError() << "\n";
		closesocket(s.clientSocket[i]);
		WSACleanup();
		return 1;
	}
	return 0;
}

//Server::client_client_close() _處理客戶端關閉
int Server::client_client_close(Server& s, int i)
{
	std::cout << "\n[系統][第" << i << "號][" << s.clientName[i] << "] : 斷開連接\n\n";
	std::cout << "\n請輸入: ";
	shutdown(s.clientSocket[i], SD_BOTH);
	closesocket(s.clientSocket[i]);

	return 0;
}

//Server::client_send_to_server _處理客戶端傳送給伺服器
int Server::client_send_to_server(Server &s, int i )
{
	memset(char_clientBuffer[i], 0, sizeof(char_clientBuffer[i]));
	int_result = recv(s.clientSocket[i], char_clientBuffer[i], sizeof(char_clientBuffer[i]), 0);
	if (int_result == -1)
	{
		std::cout << "\n[系統][第" << i << "號][" << s.clientName[i] << "] : 異常斷開\n\n";
		std::cout << "\n請輸入: " << std::endl;
		closesocket(s.clientSocket[i]);
		return 1;
	}
	std::cout << "\n[系統][第" << i << "號][" << s.clientName[i] << "]對Server說 :" << char_clientBuffer[i] << "\n\n";
	return 0;
}

//Server::client_send_to_client _處理客戶端傳送至客戶端訊息
int Server::client_send_to_client(Server &s,int i)
{
	int l;
	memset(char_clientBuffer[i], 0, sizeof(char_clientBuffer[i]));
	int_result = recv(s.clientSocket[i], char_clientBuffer[i], sizeof(char_clientBuffer[i]), 0);
	l = atoi(char_clientBuffer[i]);
	if (int_result == -1)
	{
		std::cout << "recv failed with error: " << WSAGetLastError() << "\n";
		code_information(__FILE__, __FUNCTION__, __LINE__);
		closesocket(s.clientSocket[int_server_input]);
		WSACleanup();
		return 1;
	}
	if (isClientNumberAccessible(s, l) == false)return 1;
	//傳輸指令
	message = "send_to_client";
	strcpy_s(char_clientBuffer[i], message.c_str());
	int_result = send(clientSocket[l], char_clientBuffer[i], (int)strlen(char_clientBuffer[i]), 0);
	if (int_result == -1)
	{
		std::cout << "send failed with error: " << WSAGetLastError() << "\n";
		code_information(__FILE__, __FUNCTION__, __LINE__);
		closesocket(s.clientSocket[int_server_input]);
		WSACleanup();
		return 1;
	}
	//傳輸要求傳輸者的編號
	int_result = send(clientSocket[l], std::to_string(i).c_str(), (int)strlen(std::to_string(i).c_str()), 0);
	if (int_result == -1)
	{
		std::cout << "send failed with error: " << WSAGetLastError() << "\n";
		code_information(__FILE__, __FUNCTION__, __LINE__);
		closesocket(s.clientSocket[int_server_input]);
		WSACleanup();
		return 1;
	}
	//傳輸內容
	memset(char_clientBuffer[i], 0, sizeof(char_clientBuffer[i]));
	int_result = recv(s.clientSocket[i], char_clientBuffer[i], sizeof(char_clientBuffer[i]), 0);
	if (int_result == -1)
	{
		std::cout << "recv failed with error: " << WSAGetLastError() << "\n";
		code_information(__FILE__, __FUNCTION__, __LINE__);
		closesocket(s.clientSocket[int_server_input]);
		WSACleanup();
		return 1;
	}
	int_result = send(clientSocket[l], char_clientBuffer[i], (int)strlen(char_clientBuffer[i]), 0);
	if (int_result == -1)
	{
		std::cout << "send failed with error: " << WSAGetLastError() << "\n";
		code_information(__FILE__, __FUNCTION__, __LINE__);
		closesocket(s.clientSocket[int_server_input]);
		WSACleanup();
		return 1;
	}
	std::cout << "\n[系統][第" << i << "號][" << s.clientName[i] << "]對[第" << l << "號]說 :" << char_clientBuffer[i] << "\n\n";

	return 0;
}

//Server::client_set_name() _處理客戶端改名
int Server::client_set_name(Server &s, int i)
{
	memset(char_clientBuffer[i], 0, sizeof(char_clientBuffer[i]));
	int_result = recv(s.clientSocket[i], char_clientBuffer[i], sizeof(char_clientBuffer[i]), 0);
	if (int_result == -1)
	{
		std::cout << "recv failed with error: " << WSAGetLastError() << "\n";
		closesocket(s.clientSocket[int_server_input]);
		WSACleanup();
		return 1;
	}
	s.clientName[i] = char_clientBuffer[i];
	std::cout << "\n[系統][第" << i << "號][" << s.clientName[i] << "]設置名稱為\"" << char_clientBuffer[i] << "\"\n\n";
	std::cout << "請輸入 : ";

	return 0;
}

//----------------

bool Server::isClientNumberAccessible(Server &s, int i)
{
	if (clientNumber[i] == -1 || i < 0)
	{
		std::cout << "你要求的客戶端編號尚未使用...... \n";
		std::cout << "請輸入 : ";
		return false;
	}
	return true;
}

void Server::code_information(const char* a, const char* b, int c)
{
	std::cout <<"第" << c << "行出錯" << "\n";
	std::cout <<"位於文件" << a << "的函數" << b << "\n";
}



