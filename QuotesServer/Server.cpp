#include "Server.h"


Server::Server(string _address, unsigned int _port)
{
	ipAddress = _address;
	port = _port;
}


Server::~Server()
{

}


////////////////////////////////////////////////////////////////////////////////////////
//1
//	   Summery: Opens the socket to communicate with the and requesters
//
////////////////////////////////////////////////////////////////////////////////////////
void Server::openSocket()
{
	Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (Socket == INVALID_SOCKET)
	{
		string message = "Quotes Server: Socket error! Error Code: " + WSAGetLastError();
		throw std::exception(message.c_str());
	}
}



////////////////////////////////////////////////////////////////////////////////////////
//
//	   Summery: Closes the communication socket between the server and the client
//
////////////////////////////////////////////////////////////////////////////////////////
void Server::closeSocket()
{
	if (shutdown(Socket, SD_SEND) != 0)
	{
		string message = "Quotes Server: Shutdown error! Error Code: " + WSAGetLastError();
		throw std::exception(message.c_str());
	}
}




////////////////////////////////////////////////////////////////////////////////////////
//
//	   Summery: Binds to the socket
//
////////////////////////////////////////////////////////////////////////////////////////
void Server::bindSocket()
{
	if (bind(Socket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
	{
		closeSocket();
		string message = "Quote Server: Bind Socket Error! Error Code: " + WSAGetLastError();
		throw std::exception(message.c_str());
	}
}



////////////////////////////////////////////////////////////////////////////////////////
//
//	   Summery: Listens in on the socket for any incoming connections
//
////////////////////////////////////////////////////////////////////////////////////////
void Server::listenToSocket()
{
	if (listen(Socket, 5) == SOCKET_ERROR)
	{
		closeSocket();
		string message = "Quote Server: Bind Socket Error! Error Code: " + WSAGetLastError();
		throw std::exception(message.c_str());
	}
	else
	{
		cout << "QUOTES SERVER: Listening in at IP-Address: " + ipAddress + " on port: " << port << endl;
	}
}




////////////////////////////////////////////////////////////////////////////////////////
//
//	   Summery: accepts a connection from a client
//
////////////////////////////////////////////////////////////////////////////////////////
void Server::acceptConnection()
{
	acceptSocket = accept(Socket, NULL, NULL);
	if (acceptSocket == SOCKET_ERROR)
	{
		closeSocket();
		string message = "Quote Server: Accept Socket Error! Error Code: " + WSAGetLastError();
		throw std::exception(message.c_str());
	}
}





////////////////////////////////////////////////////////////////////////////////////////
//
//	   Summery: Receives the response
//
////////////////////////////////////////////////////////////////////////////////////////
void Server::receive()
{
	char tempRecBuff[buffsize];
	int i = 1;
	do
	{
		int bytesRec = recv(acceptSocket, tempRecBuff, sizeof(tempRecBuff), 0);
		
		if (bytesRec == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET) {
			//client has disconnected!
			return;
		}

		if (bytesRec == SOCKET_ERROR)
		{
			string message = "Quote Server: Receive error! Error Code: " + WSAGetLastError();
			throw std::exception(message.c_str());
		}

		if (bytesRec <= 0) break;
		tempRecBuff[bytesRec] = '\0';
		tempBuff = tempRecBuff;
		size_t pos = tempBuff.find("SET");
		if (pos == string::npos)
		{
			pos = tempBuff.find("GET");
			if (pos == string::npos)
			{
				string message = "Quote Server: Request error! Error Code: " + WSAGetLastError();
				throw std::exception(message.c_str());
			}
			else
			{
				// handle the get request
				handleGetRequest(tempRecBuff);
			}
		}
		else
		{
			// handle the set request
			handleSetRequest(tempRecBuff, tempBuff.size());
		}
	} while (true);
}





////////////////////////////////////////////////////////////////////////////////////////
//
//	   Summery: Receives the response
//
////////////////////////////////////////////////////////////////////////////////////////
void Server::handleSetRequest(char* _message, int _size)
{
	do
	{
		// need to get the number and test whether or not it falls under 20
		char* rn = strstr(_message, "\r\n");
		char* front = _message;
		char* temp = strchr(_message, ' ');
		temp++;

		// get the number
		quoteLastPlaced = atoi(temp);

		if (quoteLastPlaced < 0 || quoteLastPlaced > 20)
		{
			sendBuffer << "ERR BAD REQUEST\r\n";
			sendReply(sendBuffer.str());
			sendBuffer.str("");
		}


		while (*temp != ' ') // we should now be over the number
		{
			temp++;
		}

		while (*temp == ' ')
			temp++;


		int quoteLength = (int)(rn - temp);
		char quote[2048];
		strncpy_s(quote, temp, quoteLength);
		quote[quoteLength] = '\0';
		quotes[quoteLastPlaced - 1] = quote;
		sendBuffer << "OK " << quoteLastPlaced << "\r\n";
		sendReply(sendBuffer.str());
		sendBuffer.str("");

		cout << "QUOTES SERVER - Response: OK " << quoteLastPlaced << " " << quotes[quoteLastPlaced - 1] << "\n\n";

		int messageLength = (int)(rn - front);
		if (messageLength + 2 < _size)
		{
			// we have more of a message
			string message = _message;
			string firstMessage;
			firstMessage.assign(message.begin(), message.end() + 4);

			std::string::size_type i = message.find(firstMessage);
			if (i != std::string::npos)
				message.erase(i, firstMessage.length());
			_message = (char*)message.c_str();
		}
		else
		{
			break;
		}
		
	} while (true);
}




////////////////////////////////////////////////////////////////////////////////////////
//
//	   Summery: Receives the response
//
////////////////////////////////////////////////////////////////////////////////////////
void Server::handleGetRequest(char* _message)
{
	// need to get the number and test whether or not it falls under 20
	char* rn = strstr(_message, "\r\n");
	char* temp = strchr(_message, ' ');
	temp++;

	// get the number
	int quoteNumTemp = atoi(temp);

	if (quoteNumTemp < 0 || quoteNumTemp > 20)
	{
		sendBuffer.str("");
		sendBuffer << "ERR BAD REQUEST\r\n";
		sendReply(sendBuffer.str());
		sendBuffer.str("");
	}
	else
	{
		sendBuffer.str("");
		sendBuffer << "OK " << quoteNumTemp << " " << quotes[quoteNumTemp - 1] << "\r\n";
		sendReply(sendBuffer.str());
		sendBuffer.str("");
	}
}





////////////////////////////////////////////////////////////////////////////////////////
//
//	   Summery: Send the reply to the client
//
////////////////////////////////////////////////////////////////////////////////////////
void Server::sendReply(string _message)
{
	
	unsigned int bytesSent = send(acceptSocket, _message.c_str(), _message.length(), 0);
	if (bytesSent == SOCKET_ERROR)
	{
		string message = "Quote Server: Request error! Error Code: " + WSAGetLastError();
		throw std::exception(message.c_str());
	}
}




////////////////////////////////////////////////////////////////////////////////////////
//
//	   Summery: Sets up the dns server address.
//
////////////////////////////////////////////////////////////////////////////////////////
void Server::setupServer()
{
	try
	{
		serverAddress.sin_family = AF_INET; // IPv4
		serverAddress.sin_port = htons(port); // port no.
		serverAddress.sin_addr.s_addr = htonl(INADDR_ANY); //the ip address
	}
	catch (std::exception)
	{
		closeSocket();
		string message = "Quotes Server: Server setup error! Error Code: " + WSAGetLastError();
		throw std::exception(message.c_str());
	}
}