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
			isAccessable[i] = false;
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
		std::cout << "[���A��]\n";
		std::cout << "------�\�श��------\n";
		std::cout << "�H�s���d�߫Ȥ�ݫH��		��J: search_client_by_number [0-20�������Ʀr]\n";
		std::cout << "�������A��			��J: server_close \n";
		std::cout << "--------------------\n";
		std::cout << "�п�J : ";

		while (std::cin >> s.command_char)//(std::cin.getline(command, sizeof(command)))
		{
			switch (hash_(s.command_char))
			{
			case "server_close"_hash:
				std::cout << "[����O] : �����Ҧ��q�T\n\n";
				CloseServer();
				return 0;
				break;

			case "search_client_by_number"_hash:
				std::cin >> s.command_int;
				std::cout << "s.command_int " << s.command_int <<"\n";
				std::cout << "s.clientNumber[s.command_int] " << s.clientNumber[s.command_int] << "\n";

				if (s.clientNumber[s.command_int] == -1)
				{
					std::cout << "�A�n�D���Ȥ�ݽs���|���ϥ�...... \n";
					std::cout << "�п�J : ";
					break;
				}
				std::cout << "[�t��]�s��"<<s.command_int << "���s��		: " << s.clientNumber[command_int] << "\n";
				std::cout << "[�t��]�s��"<<s.command_int << "���W��		: " << s.clientName[command_int] << "\n";
				std::cout << "[�t��]�s��"<<s.command_int << "��IP		: " << s.clientIP[command_int] << "\n";
				std::cout << "[�t��]�s��"<<s.command_int << "��Port		: " << s.clientPort[command_int] << "\n\n";
				std::cout << "�п�J : ";
				break;
			case "third"_hash:

				break;
			default:
				std::cout << "[����O] : ��J���~�εL�����O...\n";
				std::cout << "�п�J : ";
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
	SOCKET clientSocket[20];						//�Τ�Socket
	std::string clientName[20];						//�Τ�W�� �q�{unknown
	int clientNumber[20];							//�Τ�s�� �q�{-1
	char* clientIP[20];								//�Τ�IP
	int clientPort[20];								//�Τ�port 
	std::string Send_Buffer[20];
private:
	int iResult;
	bool isAccessable[20] ;
	struct addrinfo* result = NULL;
	struct addrinfo hints;
	WSADATA wsaData;
	int optval = 1;
	char command_char[1024];
	int command_int;
};

//clientIO (thread callback)
int clientIO(Server &s)
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

			s.Send_Buffer[i] = "[�t��]�A���s��	 : " + std::to_string(s.clientNumber[i]) + "\n[�t��]�A���W��	 : " + s.clientName[i] + "\n[�t��]�A��IP	 : " + s.clientIP[i] + "\n[�t��]�A��Port	 : " + std::to_string(s.clientPort[i]) + "\n\n";

			//�ǿ� �s��  �W�� IP Port
			std::cout << "\n[�t��][��" << i << "��][" << s.clientName[i] << "] : �ШD�ӤH��T\n\n";
			send(s.clientSocket[i], (s.Send_Buffer[i].c_str()), (int)strlen(s.Send_Buffer[i].c_str()), 0);

			break;
		case "client_close"_hash:
			std::cout << "\n[�t��][��" << i << "��][" << s.clientName[i] << "] : �_�}�s��\n\n";
			std::cout << "\n�п�J: ";
			shutdown(s.clientSocket[i], SD_BOTH);
			closesocket(s.clientSocket[i]);
			return 0;
			break;
		case "send"_hash:
			std::cout << "\n[�t��][��" << i << "��][" << s.clientName[i] << "]�� : \n\n";
			break;
		default:
			std::cout << "\n[��" << i << "��client�^��] : ��J���~�εL�����O...\n\n";
			std::cout << "\n�п�J : ";
		}
	}
}

//acceptClient (thread callback)
Server acceptClient(Server &s)
{
	char client_ip[INET_ADDRSTRLEN];
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
		std::thread clientIOa(clientIO, std::ref(s));
		clientIOa.detach();
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


//���O �ƾڪ���s�� ���G�O�L�ǤJ�ɪ��� �ݭn�����B�z ���s���� ��s�ƾ�
//�s��client�i�J �ڭ̻ݭn��s.ControlPanel(s);��s�ƾ�