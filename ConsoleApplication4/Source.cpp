// ConsoleApplication12.cpp: определяет точку входа для консольного приложения.
//
#pragma comment(lib,"Ws2_32.lib")
#include <sys/types.h>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <ctime>
#include <iostream>
#include <vector>
#include <string>


class Client
{
private:
	int key;
	SOCKET Client_Sock;
public:
	Client(SOCKET * in, int C) :Client_Sock(*in) { key = C; }
	Client() {}
	SOCKET getsock() { return Client_Sock; }
	int getkey() { return key; }
};

struct SENDBUFFER
{
	SENDBUFFER()
	{
		typemessage = 0;
		key = 0;
		ZeroMemory(name, sizeof(char) * 14);
		ZeroMemory(buffer, sizeof(char) * 202);
	}
	int key;
	int typemessage;
	char name[14];
	char buffer[202];
};

//Сокет для подключения, для хранения подключенных и для "прослушка" + подсчет количества подключенных 

SOCKET Connect;
SOCKET Listen;
int Count = 0;

std::vector<Client> Connection(64);

//Функция для отправки-приема сообщений... Принимает сообщение - рассылает всем подключенным

void decompres(int n)
{
	for (int i = n; i < Count - 1; i++)
		Connection[i] = Connection[i + 1];
	Connection[Count].~Client();
}

void SendM(int ID)
{
	for (;; Sleep(75))
	{
		SENDBUFFER s;
		int iResult = recv(Connection[ID].getsock(), (char*)&s, sizeof(s), NULL);
		std::cout << s.name << ' ' << s.typemessage << ' ' << Connection[ID].getkey() << std::endl;
		if (s.typemessage == 3)
		{
			for (int i = 0; i <= Count; i++)
				send(Connection[i].getsock(), (char*)&s, sizeof(s), NULL);
			closesocket(Connection[ID].getsock());
			decompres(ID);
			Count--;
			break;
		}
		if (iResult>0)
		{
			for (int i = 0; i <= Count; i++)
				send(Connection[i].getsock(), (char*)&s, sizeof(s), NULL);
		}
	}
	std::cout << ID << " off\n";
}

std::string encode(std::string s)
{
	std::string out;
	for (unsigned int i = 0; i < s.length(); i++)
		out += (char)(s[i] + 120) % 125 + 12;
	return out;
}

int main()
{
	setlocale(LC_ALL, "russian");
	std::cout << "Enter password\n";
	std::string pass;
	std::cin >> pass;
	if (encode(pass) != "ололоеек")
	{
		std::cout << "Error\n";
		return 0;
	}
	std::cout << "Creating server:\n";
	WSAData  ws;
	WORD version = MAKEWORD(2, 2);
	int MasterSocket = WSAStartup(version, &ws);
	if (MasterSocket != 0)
	{
		return 0;
	}
	struct addrinfo hints;
	struct addrinfo * result;
	ZeroMemory(&hints, sizeof(hints));

	//Задание сокетов

	hints.ai_family = AF_INET;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	//установка ip и порта
	std::cout << "Введите ip:\n";
	std::string iport; std::cin >> iport;
	std::cout << "Введите port:\n";
	std::string port; std::cin >> port;
	getaddrinfo(iport.c_str(), port.c_str(), &hints, &result);

	//Заполнение сокета listen

	Listen = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	bind(Listen, result->ai_addr, result->ai_addrlen);
	listen(Listen, SOMAXCONN);
	freeaddrinfo(result);

	//Начало работы сервера

	std::cout << "Start" << std::endl;
	char c_connect[] = "Connect";
	while (1)
	{
		//проверка на получение сигнала от кого-нибудь
		if (Connect = accept(Listen, NULL, NULL))
		{
			std::cout << c_connect << ' ' << Count << std::endl;
			Connection[Count] = Client::Client(&Connect, Count);
			Count++;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)SendM, (LPVOID)(Count - 1), NULL, NULL);
		}
	}
	return 0;
}
