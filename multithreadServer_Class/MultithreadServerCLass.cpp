#pragma once

#include "MultithreadServerCLass.h"

//----------------

//Server::ControlPanel() __�e�ΥD�u�{ �B�z�Τ��J
int Server::ControlPanel(Server& s)
{
	Server_Instruction();
	while (std::cin >> s.command_char)
	{
		//�Ҧ�send�hclient��, client�]�|���@�M�B�z�t��
		switch (hash_(s.command_char))
		{
		case "�\�श��"_hash:
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
			std::cout << "[����O] : ��J���~�εL�����O...\n";
			std::cout << "�п�J : ";
		}
	}

	return 0;
}

//Server::clientRevc() _���l�u�{�^�� �����Ȥ�ݦ^��
int Server::clientRevc(Server& s)
{
	char Recv_Buffer[1024];
	int i = s.CurrentClientNumber;
	while (true) {
		memset(Recv_Buffer, 0, sizeof(Recv_Buffer));
		int bytesRead = recv(s.clientSocket[i], Recv_Buffer, sizeof(Recv_Buffer), 0);
		if (bytesRead == -1)
		{
			std::cout << "\n[�t��][��" << i << "��][" << s.clientName[i] << "] : ���`�_�}\n\n";
			std::cout << "\n�п�J: " << std::endl;
			closesocket(s.clientSocket[i]);
			return 1;
		}
		
		Recv_Buffer[bytesRead] = '\0';
		switch (hash_(Recv_Buffer))
		{
		case "personal_information"_hash:
			client_personal_information(s, i);

			break;
		case "client_close"_hash:
			client_client_close(s, i);

			break;
		case "send"_hash:
			client_send_to_server(s, Recv_Buffer, i, bytesRead);
			break;
		case "set_name"_hash:
			bytesRead = recv(s.clientSocket[i], Recv_Buffer, sizeof(Recv_Buffer), 0);
			if (bytesRead == -1)
			{
				std::cout << "\n[�t��][��" << i << "��][" << s.clientName[i] << "] : ���`�_�}\n\n";
				std::cout << "\n�п�J: " << std::endl;
				closesocket(s.clientSocket[i]);
				return 1;
			}
			s.clientName[i] = Recv_Buffer;
			std::cout << "\n[�t��][��" << i << "��][" << s.clientName[i] << "]�]�m�W�٬�\"" << Recv_Buffer << "\"\n\n";
			std::cout << "�п�J : ";

			break;
		default:
			std::cout << "\n[��" << i << "��client�^��] : ��J���~�εL�����O...\n\n";
			std::cout << "\n�п�J : ";
		}
	}
}

//Server:: Client_Instructions() _���Ѧ��A��������r
void Server::Server_Instruction()
{
	std::cout << "[���A��]\n";																		//
	std::cout << "------�\�श��------\n";															//
	std::cout << "���A���\�श��			��J: �\�श��\n";										//
	std::cout << "�H�s���d�߫Ȥ�ݫH��		��J: search_client_by_number [0-20�������Ʀr]\n";		//
	std::cout << "�M���Ȥ�ݫH��			��J: traversal_client\n";								//
	std::cout << "�����Ȥ��			��J: kick [�s��0~20]\n";									//
	std::cout << "�������A��			��J: server_close \n";										//
	std::cout << "--------------------\n";															//
	std::cout << "�H�s���M���w�Ȥ�ݻ���		��J: send [�s��0~20] [�y�l]\n";					//
	std::cout << "�o�y�l���Ҧ��Ȥ��		��J: send_all [�y�l]\n";								//
	std::cout << "--------------------\n";
	std::cout << "�п�J : ";
}

//----------------

//Server::acceptClient() _���l�u�{�^�� �����Ȥ�ݥ[�J
Server Server::acceptClient(Server& s)
{
	while (1)
	{
		//�}�l���ݫȤ��, �����ǤJ
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

		std::cout << "\n[�t��][�Ȥᱵ��] : ��" << s.CurrentClientNumber << "�ӫȤ�ݥ[�J�F\n\n";
		std::cout << "�п�J : ";
		std::thread clientRecvThread([&s]() {
			s.clientRevc(s);
			});
		clientRecvThread.detach();
		Sleep(1);
		s.CurrentClientNumber += 1;
	}
}

//Server::Server() _�c�y���
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

//Server::ServerInit() _��l��
int Server::ServerInit()
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

	return 0;
}

//Server::CloseServer() _�������A��
int Server::CloseServer(Server& s)
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

//Server::PrintClientInfo() _���L�Ȥ�ݫH��
void Server::PrintClientInfo(Server &s, int i)
{
	std::cout << "[�t��]�s��" << i << "���s��		: " << s.clientNumber[i] << "\n";
	std::cout << "[�t��]�s��" << i << "���W��		: " << s.clientName[i] << "\n";
	std::cout << "[�t��]�s��" << i << "��IP			: " << s.clientIP[i] << "\n";
	std::cout << "[�t��]�s��" << i << "��Port		: " << s.clientPort[i] << "\n\n";
}

//Server::search_client_by_number() _�νs���d�߫Ȥ��
int Server::search_client_by_Client_Number(Server &s)
{
	std::cin >> s.command_int;
	if (isClientNumberAccessible(s) == false)return 1;
	PrintClientInfo(s, command_int);
	std::cout << "�п�J : ";

	return 0;
}

//Server::traversal_client() _�M���Ȥ��
int Server::traversal_client(Server& s)
{
	if (s.CurrentClientNumber == 0)
	{
		std::cout << "�ثe�S���Ȥ��......";
		return 1;
	}
	std::cout << "�ثe��" << s.CurrentClientNumber << " / 20�ӫȤ�ݨϥΤ�......\n";
	for (int i = 0; i < s.CurrentClientNumber; i++)
	{
		PrintClientInfo(s, i);
	}
	std::cout << "�п�J : ";

	return 0;
}

//Server::kick() _�𰣫Ȥ��
int Server::kick_by_Client_Number(Server& s)
{
	std::cin >> s.command_int;
	if (isClientNumberAccessible(s) == false)return 1;
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


	return 0;
}

//Server::send_to_client() _�ǰT�������w�Ȥ��
int Server::send_to_client_by__Client_Number(Server& s)
{
	std::cin >> command_int;
	if (isClientNumberAccessible(s) == false)return 1;
	std::cin.getline(command_char, sizeof(command_char));
	bytesSend = send(clientSocket[command_int], command_char, (int)strlen(command_char), 0);
	if (bytesSend == -1)
	{
		std::cout << "send failed with error: " << WSAGetLastError() << "\n";
		closesocket(clientSocket[command_int]);
		WSACleanup();
		return 1;
	}
	std::cout << "\n�п�J : ";

	return 0;
}

int Server::send_to_all_client(Server& s)
{
	std::cin.getline(command_char, sizeof(command_char));
	for (int i = 0; i < s.CurrentClientNumber; i++)
	{
		bytesSend = send(clientSocket[i], command_char, (int)strlen(command_char), 0);
		if (bytesSend == -1)
		{
			std::cout << "send failed with error: " << WSAGetLastError() << "\n";
			closesocket(clientSocket[command_int]);
			WSACleanup();
			return 1;
		}
	}
	std::cout << "\n�п�J : ";

	return 0;
}

//Server::client_personal_information() _�B�z�Ȥ�ݫH���n�D
int Server::client_personal_information(Server& s, int i)
{
	std::cout << "\n[�t��][��" << i << "��][" << s.clientName[i] << "] : �ШD�ӤH��T\n\n";
	std::cout << "�п�J : ";

	//�ǿ� �s��  �W�� IP Port
	s.Send_Buffer[i] = "[�t��]�A���s��	 : " + std::to_string(s.clientNumber[i]) + "\n[�t��]�A���W��	 : " + s.clientName[i] + "\n[�t��]�A��IP	: " + s.clientIP[i] + "\n[�t��]�A��Port	 : " + std::to_string(s.clientPort[i]) + "\n\n";
	s.bytesSend = send(s.clientSocket[i], (s.Send_Buffer[i].c_str()), (int)strlen(s.Send_Buffer[i].c_str()), 0);
	if (s.bytesSend == -1)
	{
		std::cout << "send failed with error: " << WSAGetLastError() << "\n";
		closesocket(s.clientSocket[i]);
		WSACleanup();
		return 1;
	}
	return 0;
}

//Server::client_client_close() _�B�z�Ȥ�������n�D
int Server::client_client_close(Server& s, int i)
{
	std::cout << "\n[�t��][��" << i << "��][" << s.clientName[i] << "] : �_�}�s��\n\n";
	std::cout << "\n�п�J: ";
	shutdown(s.clientSocket[i], SD_BOTH);
	closesocket(s.clientSocket[i]);

	return 0;
}

int Server::client_send_to_server(Server &s, char* Recv_Buffer, int i, int bytesRead)
{
	bytesRead = recv(s.clientSocket[i], Recv_Buffer, sizeof(Recv_Buffer), 0);
	if (bytesRead == -1)
	{
		std::cout << "\n[�t��][��" << i << "��][" << s.clientName[i] << "] : ���`�_�}\n\n";
		std::cout << "\n�п�J: " << std::endl;
		closesocket(s.clientSocket[i]);
		return 1;
	}
	std::cout << "\n[�t��][��" << i << "��][" << s.clientName[i] << "]��Server�� :" << Recv_Buffer << "\n\n";
	return 0;
}




//----------------
bool Server::isClientNumberAccessible(Server &s)
{
	if (s.clientNumber[s.command_int] == -1 || command_int < 0)
	{
		std::cout << "�A�n�D���Ȥ�ݽs���|���ϥ�...... \n";
		std::cout << "�п�J : ";
		return false;
	}
	return true;
}



