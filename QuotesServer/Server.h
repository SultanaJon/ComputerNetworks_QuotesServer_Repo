#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <iostream>
#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <sstream>

using std::vector;
using std::string;
using std::cout;
using std::endl;
using std::string;
using std::stringstream;

class Server
{
private:
	// PRIVATE MEMBERS
	SOCKET Socket;
	SOCKADDR_IN serverAddress;
	unsigned int port;
	string ipAddress;
	SOCKET acceptSocket;
	static const int buffsize = 10240;
	string recBuffer;
	stringstream sendBuffer;
	string tempBuff;
	string quotes[20];
	int quoteLastPlaced;

	// PRIVATE FUNCTIONS
	void handleSetRequest(char* _message, int size);
	void handleGetRequest(char* _message);
	void sendReply(string _message);

public:
	Server(string _address, unsigned int _ports);
	~Server();
	void openSocket();
	void bindSocket();
	void listenToSocket();
	void closeSocket();
	void acceptConnection();
	void receive();
	void setupServer();
};

