#include <iostream>
#include <stdlib.h>
#include <WS2tcpip.h>		// how windows access sockets
#include <string>
#include <sstream>

#pragma comment (lib, "ws2_32.lib")  // specifies how compiler process input

// network big endian, pc little endian
using namespace std;

string decypher(string a);
string CalcResponse(size_t op, char operand, string a);

string decypher(string a) 
{
	string response;

	if (!a.compare("Hi"))
	{
		response = "Hi, there!";
	}
	else if(!a.compare("help"))
	{
		response = "The program can be used as a calculator";
	}
	else{
		size_t oper;		

		if (a.find("+") != string::npos) {
			oper = a.find("+");
			response = CalcResponse(oper, '+', a);
		
		}
		else if (a.find("*") != string::npos) {
			oper = a.find("*");
			response = CalcResponse(oper, '*', a);
		}
		else if (a.find("/") != string::npos) {
			oper = a.find("/");
			response = CalcResponse(oper, '/', a);
		}
		else if (a.find("-") != string::npos) {
			oper = a.find("-");
			response = CalcResponse(oper, '-', a);
		}
		else {
			response = "invalid input";
		}
	}
	return response;
	
}

string CalcResponse(size_t op, char operand, string a) {
	string response;
	char first[4096 / 2];
	char second[4096 / 2];
	char arrchar[4096];
	int firstValue, secondValue, returnValue;

	strcpy_s(arrchar, a.c_str());

	for (int i = 0; i < op; i++)
	{
		first[i] = arrchar[i];
	}

	for (int i = op + 1; i < 2048; i++)
	{
		if (arrchar[i] == '\n' || arrchar[i] == NULL)
		{
			break;
		}
		second[i - op - 1] = arrchar[i];
	}
	firstValue = atoi(first);
	secondValue = atoi(second);

	if (operand == '+'){
		returnValue = firstValue + secondValue;

		response = to_string(returnValue);
	}
	else if (operand == '-') {
		returnValue = firstValue - secondValue;

		response = to_string(returnValue);
	}
	else if (operand == '*') {
		returnValue = firstValue * secondValue;

		response = to_string(returnValue);
	}
	else {
		returnValue = firstValue / secondValue;

		response = to_string(returnValue);
	}
	return response;
}

void main()
{

	int wsOK;  // opening window socket check

	// initialize winsock
	WSADATA wsData;					// filled with call to win socket startup
	WORD ver = MAKEWORD(2, 2);		// request version 2.2

	if((wsOK = WSAStartup(ver, &wsData)) != 0) 
	{
		cerr << "Can't Initialize winsock!" << endl;  // bailing hard
		return;
	}

	// Create Socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);  // DGRAM another one check online for function
	if (listening == INVALID_SOCKET)
	{
		cerr << "Couldn't create socket" << endl;
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
				char buf[4096];
				ZeroMemory(buf, 4096);

				// accept new message
				int bytesIn = recv(sock, buf, 4096, 0);
				if (bytesIn <= 0)
				{
					// drop client
					closesocket(sock);
					FD_CLR(sock, &master);
				}
				else 
				{
					// send message to other 
					for (int i = 0; i < master.fd_count; i++)
					{
						SOCKET outSock = master.fd_array[i];
						if (outSock != listening && outSock == sock)
						{
							ostringstream ss;
							ss << "Socket #" << sock << ":" << decypher(string(buf)) << "\r\n";
							string strOut = ss.str();
							send(outSock, strOut.c_str(), strOut.size()+1, 0);


							ss.clear();
							ss << ">";
							strOut = ss.str();
							send(outSock, strOut.c_str(), strOut.size() + 1, 0);
						}
					}
				}
				// send message to other clients and definitely not listening socket
			}
		}
	}

	// cleanup winsock
	WSACleanup();
}
