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
	//char clientRecvBuffer[1024];
	int i = s.CurrentClientNumber;
	while (true) {
		memset(char_clientBuffer[i], 0, sizeof(char_clientBuffer[i]));
		int_result = recv(s.clientSocket[i], char_clientBuffer[i], sizeof(char_clientBuffer[i]), 0);
		if (int_result == -1)
		{
			std::cout << "\n[�t��][��" << i << "��][" << s.clientName[i] << "] : ���`�_�}\n\n";
			std::cout << "\n�п�J: " << std::endl;
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

//Server::CloseServer() _�������A��
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
	std::cin >> s.int_server_input;
	if (isClientNumberAccessible(s, int_server_input) == false)return 1;
	PrintClientInfo(s, int_server_input);
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

//Server::send_to_client() _�ǰT�������w�Ȥ��
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
	std::cout << "\n�п�J : ";

	return 0;
}

//Server::send_to_all_client() _�ǰT�����Ҧ��Ȥ��
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
	std::cout << "\n�п�J : ";

	return 0;
}

//-----------------

//Server::client_personal_information() _�B�z�Ȥ�ݫH��
int Server::client_personal_information(Server& s, int i)
{
	std::cout << "\n[�t��][��" << i << "��][" << s.clientName[i] << "] : �ШD�ӤH��T\n\n";
	std::cout << "�п�J : ";

	//�ǿ� �s��  �W�� IP Port
	std::string message = "[�t��]�A���s��	 : " + std::to_string(s.clientNumber[i]) + "\n[�t��]�A���W��	 : " + s.clientName[i] + "\n[�t��]�A��IP	: " + s.clientIP[i] + "\n[�t��]�A��Port	 : " + std::to_string(s.clientPort[i]) + "\n\n";
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

//Server::client_client_close() _�B�z�Ȥ������
int Server::client_client_close(Server& s, int i)
{
	std::cout << "\n[�t��][��" << i << "��][" << s.clientName[i] << "] : �_�}�s��\n\n";
	std::cout << "\n�п�J: ";
	shutdown(s.clientSocket[i], SD_BOTH);
	closesocket(s.clientSocket[i]);

	return 0;
}

//Server::client_send_to_server _�B�z�Ȥ�ݶǰe�����A��
int Server::client_send_to_server(Server &s, int i )
{
	memset(char_clientBuffer[i], 0, sizeof(char_clientBuffer[i]));
	int_result = recv(s.clientSocket[i], char_clientBuffer[i], sizeof(char_clientBuffer[i]), 0);
	if (int_result == -1)
	{
		std::cout << "\n[�t��][��" << i << "��][" << s.clientName[i] << "] : ���`�_�}\n\n";
		std::cout << "\n�п�J: " << std::endl;
		closesocket(s.clientSocket[i]);
		return 1;
	}
	std::cout << "\n[�t��][��" << i << "��][" << s.clientName[i] << "]��Server�� :" << char_clientBuffer[i] << "\n\n";
	return 0;
}

//Server::client_send_to_client _�B�z�Ȥ�ݶǰe�ܫȤ�ݰT��
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
	//�ǿ���O
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
	//�ǿ�n�D�ǿ�̪��s��
	int_result = send(clientSocket[l], std::to_string(i).c_str(), (int)strlen(std::to_string(i).c_str()), 0);
	if (int_result == -1)
	{
		std::cout << "send failed with error: " << WSAGetLastError() << "\n";
		code_information(__FILE__, __FUNCTION__, __LINE__);
		closesocket(s.clientSocket[int_server_input]);
		WSACleanup();
		return 1;
	}
	//�ǿ餺�e
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
	std::cout << "\n[�t��][��" << i << "��][" << s.clientName[i] << "]��[��" << l << "��]�� :" << char_clientBuffer[i] << "\n\n";

	return 0;
}

//Server::client_set_name() _�B�z�Ȥ�ݧ�W
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
	std::cout << "\n[�t��][��" << i << "��][" << s.clientName[i] << "]�]�m�W�٬�\"" << char_clientBuffer[i] << "\"\n\n";
	std::cout << "�п�J : ";

	return 0;
}

//----------------

bool Server::isClientNumberAccessible(Server &s, int i)
{
	if (clientNumber[i] == -1 || i < 0)
	{
		std::cout << "�A�n�D���Ȥ�ݽs���|���ϥ�...... \n";
		std::cout << "�п�J : ";
		return false;
	}
	return true;
}

void Server::code_information(const char* a, const char* b, int c)
{
	std::cout <<"��" << c << "��X��" << "\n";
	std::cout <<"�����" << a << "�����" << b << "\n";
}



