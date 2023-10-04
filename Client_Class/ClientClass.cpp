#pragma once

#include "ClientClass.h"

//Client::Client() _構造函數
Client::Client(const char* ip, const char* port)
{
	this->Server_IP = ip;
	this->Server_Port = port;
}

//Client::ClientInit() _初始化
int Client::ClientInit()
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

//Client::clientControlPanel() _占用主線程 處理用戶輸入
int Client::clientControlPanel(Client& c)
{
	Client_Instructions();
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
		case"personal_information"_hash://請勿刪掉, 當輸入personal_information, 理應client不做事

			break;
		case "client_close"_hash:
			Client_Close();

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

//Client::serverRecv() _給子線程回調 接收伺服器回傳
int Client::serverRecv(Client& c)
{
	while (true)
	{
		c.bytesRead = recv(c.ConnectSocket, c.recvbuf, sizeof(c.recvbuf), 0);
		if (c.bytesRead == -1)
		{
			std::cout << "recv failed with error: " << WSAGetLastError() << "\n";
			closesocket(c.ConnectSocket);
			WSACleanup();
			exit(0);
		}
		c.recvbuf[c.bytesRead] = '\0';
		switch (hash_(c.recvbuf))
		{
		case "kick"_hash:
			Client_Close();

			break;
		case "123"_hash:
			break;
		default:
			std::cout << "\n[系統][Server傳輸] \n" << c.recvbuf;

		}
		std::cout << "\n請輸入:";

	}
}

//Client:: Client_Instructions() _提供客戶端說明文字
void Client::Client_Instructions()
{
	std::cout << "[客戶端]\n";
	std::cout << "------功能介紹------\n";
	std::cout << "顯示個人資訊		輸入 : personal_information \n";
	std::cout << "更改名稱		輸入 : set_name [名稱]\n";
	std::cout << "關掉客戶端		輸入 : client_close \n";
	std::cout << "--------------------\n";
	std::cout << "請輸入 : ";

}

//Client::Client_Close() _關閉client
int Client::Client_Close()
{
	iResult = shutdown(ConnectSocket, SD_BOTH);
	if (iResult == SOCKET_ERROR) {
		std::cout << "shutdown failed with error: " << WSAGetLastError() << "\n";
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}
	closesocket(ConnectSocket);
	WSACleanup();

	return 0;
}
