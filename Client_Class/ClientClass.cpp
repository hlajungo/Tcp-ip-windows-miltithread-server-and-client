#pragma once

#include "ClientClass.h"

//----------------


//Client::clientControlPanel() _�e�ΥD�u�{ �B�z�Τ��J
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
		case"personal_information"_hash://�ФŧR��, ���Jpersonal_information, �z��client������

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
			std::cout << " ��J���~�εL�����O\n";
			std::cout << "\n�п�J : ";
			break;
		}
	}
}

//Client::serverRecv() _���l�u�{�^�� �������A���^��
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
			std::cout << "\n[�t��][Server]�� :\n" << buffer << "\n";

		}
		std::cout << "\n�п�J:";

	}
}

//Client:: Client_Instructions() _���ѫȤ�ݻ�����r
void Client::Client_Instructions()
{
	std::cout << "[�Ȥ��]\n";
	std::cout << "------�\�श��------\n";
	std::cout << "��ܭӤH��T		��J : personal_information \n";
	std::cout << "���W��		��J : set_name [�W��]\n";
	std::cout << "��Server����		��J : send_to_server [�y�l]\n";
	std::cout << "���LClient����	��J : send_to_client {�s��1~20] [�y�l]\n";
	std::cout << "�����Ȥ��		��J : client_close \n";
	std::cout << "--------------------\n";
	std::cout << "�п�J : ";

}


//----------------

//Client::Client() _�c�y���
Client::Client(const char* ip, const char* port)
{
	this->Server_IP = ip;
	this->Server_Port = port;
}

//Client::ClientInit() _��l��
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

//Client::Client_Close() _����client
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

//Client::Client_Send_To_Server() _�ǿ�T����Server
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
	std::cout << "\n�п�J : ";
	return 0;
}

//Client::set_name() _�]�m�W��
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
	std::cout << "�п�J : ";

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
	std::cout << "�п�J : ";

	return 0;
}


//------------

//Client::ServerRecv_send_to_client _�B�z���A���ǰe�L�H�ǰe�L�Ӫ��T��
int Client::ServerRecv_send_to_client()
{
	memset(buffer, 0, sizeof(buffer));
	int_result = recv(ConnectSocket, buffer, sizeof(buffer), 0);
	std::cout << "\n�t��][��" << buffer << "��]��A�� :";
	memset(buffer, 0, sizeof(buffer));
	int_result = recv(ConnectSocket, buffer, sizeof(buffer), 0);
	std::cout << buffer << "\n\n";

	return 0;
}

