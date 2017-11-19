//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//		   Program: Quotes Server
//		Programmer: Jon Sultana
//			  Date: 10/29/2017
//
//  LTU Honor Code: "I have neither given nor received unauthorized aid in completing
//                   this work nor have I claimed someone else's work as my own."
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////


#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string>
#include "Server.h"


using std::cout;
using std::cin;
using std::endl;

int main(void)
{
	WSADATA wsaData;
	string ipAddress = "10.10.10.112";
	unsigned int port = 21001;


	// Initialize Winsock version 2.2
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	printf("Winsock DLL status is %s.\n", wsaData.szSystemStatus);


	//////////////////////////////////////////
	//
	// Start a Quotes Server
	//  0) Create the server object
	//	1) Open the socket
	//  2) setup socket address structure
	//  3) bind the socket
	//  4) listen to the socket
	//  5) accept the connection with the client
	//  6) close the socket
	//
	//////////////////////////////////////////
	Server server(ipAddress, port);

	try
	{
		server.openSocket();
		server.setupServer();
		server.bindSocket();
		server.listenToSocket();

		do
		{
			server.acceptConnection();
			server.receive();

		} while (true);

		server.closeSocket();
	}
	catch (std::exception& e)
	{
		cout << e.what() << endl;
		system("pause");
		WSACleanup();
		return -1;
	}


	if (WSACleanup() != 0)
	{
		cout << "Server: WSACleanup() failed!..." << endl;
		system("pause");
		return -1;
	}

	return 0;
}
