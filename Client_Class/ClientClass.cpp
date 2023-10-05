#pragma once

#include "ClientClass.h"

//----------------


//Client::clientControlPanel() _占用主線程 處理用戶輸入
int Client::clientControlPanel(Client& c)
{
	
	Client_Instructions();
	while (std::cin >> buffer)
	{

		int_result = send(ConnectSocket, buffer, (int)strlen(buffer), 0);
		if (int_result == SOCKET_ERROR)
		{
			std::cout << "send failed with error: " << WSAGetLastError() << "\n";
			closesocket(ConnectSocket);
			WSACleanup();
			return 1;
		}

		switch (hash_(buffer))
		{
		case"personal_information"_hash://請勿刪掉, 當輸入personal_information, 理應client不做事

			break;
		case "client_close"_hash:
			Client_Close();

			break;
		case "send_to_server"_hash:
			Client_Send_To_Server();

			break;
		case "set_name"_hash:
			set_name();

			break;
		case "send_to_client"_hash:
			send_to_client();

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
		memset(buffer, 0, sizeof(buffer));
		int_result = recv(c.ConnectSocket, buffer, sizeof(buffer), 0);
		if (int_result == -1)
		{
			std::cout << "recv failed with error: " << WSAGetLastError() << "\n";
			closesocket(c.ConnectSocket);
			WSACleanup();
			exit(0);
		}
		buffer[int_result] = '\0';
		switch (hash_(c.buffer))
		{
		case "kick"_hash:
			Client_Close();

			break;
		case "send_to_client"_hash:
			ServerRecv_send_to_client();

			break;
		default:
			std::cout << "\n[系統][Server]說 :\n" << buffer << "\n";

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
	std::cout << "對Server說話		輸入 : send_to_server [句子]\n";
	std::cout << "對其他Client說話	輸入 : send_to_client {編號1~20] [句子]\n";
	std::cout << "關掉客戶端		輸入 : client_close \n";
	std::cout << "--------------------\n";
	std::cout << "請輸入 : ";

}


//----------------

//Client::Client() _構造函數
Client::Client(const char* ip, const char* port)
{
	this->Server_IP = ip;
	this->Server_Port = port;
}

//Client::ClientInit() _初始化
int Client::ClientInit()
{
	int_result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (int_result != 0) {
		std::cout << "WSAStartup failed with error: " << int_result << "\n";
		return 1;
	}
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	int_result = getaddrinfo(Server_IP, Server_Port, &hints, &result);
	if (int_result != 0) {
		std::cout << "getaddrinfo failed with error: " << int_result << "\n";
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
		int_result = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (int_result == SOCKET_ERROR) {
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

//Client::Client_Close() _關閉client
int Client::Client_Close()
{
	int_result = shutdown(ConnectSocket, SD_BOTH);
	if (int_result == SOCKET_ERROR) {
		std::cout << "shutdown failed with error: " << WSAGetLastError() << "\n";
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}
	closesocket(ConnectSocket);
	WSACleanup();

	return 0;
}

//Client::Client_Send_To_Server() _傳輸訊息給Server
int Client::Client_Send_To_Server()
{
	memset(buffer, 0, sizeof(buffer));
	std::cin.getline(buffer, sizeof(buffer));
	int_result = send(ConnectSocket, buffer, (int)strlen(buffer), 0);
	if (int_result == SOCKET_ERROR)
	{
		std::cout << "send failed with error: " << WSAGetLastError() << "\n";
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}
	std::cout << "\n請輸入 : ";
	return 0;
}

//Client::set_name() _設置名稱
int Client::set_name()
{
	memset(buffer, 0, sizeof(buffer));
	std::cin.getline(buffer, sizeof(buffer));
	int_result = send(ConnectSocket, buffer, (int)strlen(buffer), 0);
	if (int_result == SOCKET_ERROR)
	{
		std::cout << "send failed with error: " << WSAGetLastError() << "\n";
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}
	std::cout << "請輸入 : ";

	return 0;
}

int Client::send_to_client()
{
	memset(buffer, 0, sizeof(buffer));
	std::cin >> buffer;
	int_result = send(ConnectSocket, buffer, (int)strlen(buffer), 0);
	if (int_result == SOCKET_ERROR)
	{
		std::cout << "send failed with error: " << WSAGetLastError() << "\n";
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}
	std::cin.getline(buffer, sizeof(buffer));
	int_result = send(ConnectSocket, buffer, (int)strlen(buffer), 0);
	if (int_result == SOCKET_ERROR)
	{
		std::cout << "send failed with error: " << WSAGetLastError() << "\n";
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}
	std::cout << "請輸入 : ";

	return 0;
}


//------------

//Client::ServerRecv_send_to_client _處理伺服器傳送他人傳送過來的訊息
int Client::ServerRecv_send_to_client()
{
	memset(buffer, 0, sizeof(buffer));
	int_result = recv(ConnectSocket, buffer, sizeof(buffer), 0);
	std::cout << "\n系統][第" << buffer << "號]對你說 :";
	memset(buffer, 0, sizeof(buffer));
	int_result = recv(ConnectSocket, buffer, sizeof(buffer), 0);
	std::cout << buffer << "\n\n";

	return 0;
}

