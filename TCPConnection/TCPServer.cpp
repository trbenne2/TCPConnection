#include <iostream>
#include <WS2tcpip.h>		// how windows access sockets
#include <string>

#pragma comment (lib, "ws2_32.lib")  

// network big endian, pc little endian

void main()
{
	int wsOK;  // opening window socket check

	// initialize winsock
	WSADATA wsData;					// filled with call to win socket startup
	WORD ver = MAKEWORD(2, 2);		// request version 2.2

	if((wsOK = WSAStartup(ver, &wsData)) != 0) 
	{
		std::cerr << "Can't Initialize winsock!" << std::endl;  // bailing hard
		return;
	}

	// Create Socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);  // DGRAM another one check online for function
	if (listening == INVALID_SOCKET)
	{
		std::cerr << "Couldn't create socket" << std::endl;
		return;
	}

	// bind socket to ip and port
	sockaddr_in hint;						// hint structure
	hint.sin_family = AF_INET;				// 
	hint.sin_port = htons(54000);			// host to network short
	hint.sin_addr.S_un.S_addr = INADDR_ANY; // bind to any address
	// could you inet_pton  ^
	
	bind(listening, (sockaddr*)&hint, sizeof(hint));  // bind the port to the socket


	// tell winsoc, socket is for listening
	listen(listening, SOMAXCONN); // marks as being for listening...... Max connections

	//define set
	fd_set master;

	FD_ZERO(&master);  // nothing exist in set

	FD_SET(listening, &master);	// add connection to set

	while (true)
	{
		fd_set copy = master;  // everytime call select destories it

		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		for (int i = 0; i < socketCount; i++)
		{
			SOCKET sock = copy.fd_array[i];
			if (sock == listening)
			{
				// Accept a new connection
				SOCKET client = accept(listening, nullptr, nullptr);

				// add new connection to list of connected clients
				FD_SET(client, &master);

				// send welcome message
				string welcomeMsg = ">";
				send(client, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);
				// TODO: broadcast new connection
			}
			else
			{
				// accept new message
				// send message to other clients and definitely not listening socket
			}
		}
	}

	// cleanup winsock
	WSACleanup();
}

/*
select()
FD_ckr - remove from set
FD_set -  add to set
FD_zero - clear set

fd_set - contains all sockets connected
*/

// support 1 client below

/*// wait for connection
sockaddr_in client;
int clientSize = sizeof(client);

SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);  // connection happens
if (clientSocket == INVALID_SOCKET)
{
std::cout << "socket couldn't connect" << std::endl;
}

char host[NI_MAXHOST];		// client's remote name  dns_lookup
char service[NI_MAXHOST];  // Service (port) the client is connect on

ZeroMemory(host, NI_MAXHOST);		// on linux/MAC memset(host, 0, NI_MAXHOST)
ZeroMemory(service, NI_MAXSERV);	// Zeromemory windows only

// try to look up host name other wise display ip
if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)	//if we can get name info (DNS lookup host name)
{
std::cout << host << " connect on port " << service << std::endl;
}
else
{
inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);  //	get IP back as string char
std::cout << host << " connected on port " << ntohs(client.sin_port) << std::endl;   // network to host short
}

// close listening socket... won't do if more than 1 client connects
closesocket(listening);

// while loop: accept and echo messge to client  // here down can put in thread
char buf[4096];

while (true)
{
ZeroMemory(buf, 4096);

// wait for client data
int bytesReceived = recv(clientSocket, buf, 4096, 0);
if (bytesReceived == SOCKET_ERROR)
{
std::cerr << " Error in recv()" << std::endl;
break;
}

if (bytesReceived == 0)
{
std::cout << " client disconnected " << std::endl;
break;
}

// send message
send(clientSocket, buf, bytesReceived + 1, 0);
}

// close socket
closesocket(clientSocket);
*/