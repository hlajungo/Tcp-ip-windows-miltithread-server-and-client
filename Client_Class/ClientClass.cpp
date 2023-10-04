#pragma once

#include "ClientClass.h"

//Client::Client() _�c�y���
Client::Client(const char* ip, const char* port)
{
	this->Server_IP = ip;
	this->Server_Port = port;
}

//Client::ClientInit() _��l��
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

//Client::clientControlPanel() _�e�ΥD�u�{ �B�z�Τ��J
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
		case"personal_information"_hash://�ФŧR��, ���Jpersonal_information, �z��client������

			break;
		case "client_close"_hash:
			Client_Close();

			break;
		case "send"_hash:
			std::cout << "�п�J�n�ǿ骺���e : ";
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
			std::cout << "\n[�t��][Server�ǿ�] \n" << c.recvbuf;

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
	std::cout << "�����Ȥ��		��J : client_close \n";
	std::cout << "--------------------\n";
	std::cout << "�п�J : ";

}

//Client::Client_Close() _����client
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
