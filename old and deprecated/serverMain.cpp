/*	Pinochle Game Server
		Desktop Application
		Lucas Weisensee
		November 2014

	to START THE PROGRAM in command prompt::
	executableName PortNumber

	This program will function as a desktop game server for a network based pinochle game. It will:
	-Listen for new connections.
	-Manage incomming player connections.
		-Spinn off processes to handle new games.
		-Allow new players to connect to games in progress.
	-Keep track of games in progress.
	-Allow games to be added to database
	-Allow games to be requested from the database.

*/
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <string.h>
#include <sys/types.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#pragma comment(lib, "Ws2_32.lib")

//Error report management function
void error(char *er_msg)
{
  perror(er_msg);
  exit(1);
}

int main(int argc, char const *argv[])
{
	std::cout << "\nSetting up connection";

	//check argument length for port number
	if (argc < 2)
	{
		fprintf(stderr,"ERROR, no port provided");
		exit(1);
	}

	//int portnumber = *argv[1];		//unnecessary?

	#define DEFAULT_PORT argv[1];

	//initialize windows socket
	WSADATA wsaData;

	int result;
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		printf("WSAStartup failed! Returned:%d\n", result);
		return 1;
	}

	struct addrinfo *socketInfo = NULL, *ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	//Get's address info from system for port and socket connection
	result = getaddrinfo(NULL, DEFAULT_PORT, &hints, &socketInfo);
	if (result != 0)
	{
		printf("getaddrinfo failed with error: %d\n", result);
		WSACleanup();	//deallocate memory and quit
		return 1;
	}

	// Create a SOCKET for the server to listen to
	SOCKET ListenSocket = INVALID_SOCKET;
	ListenSocket = socket(socketInfo->ai_family,socketInfo->ai_socktype,socketInfo->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(socketInfo);
		WSACleanup();
		return 1;
	}

	//Setup the TCP listening socket
	result = bind(ListenSocket,socketInfo->ai_addr, (int)socketInfo->ai_addrlen);
	if (result == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(socketInfo);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	//deallocate socket address memory
	freeaddrinfo(socketInfo);

	//listen for client connection on sockey created
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR){				//if listen fails
		printf("Listen() failed with error: %ld\n", WSAGetLastError());	//print error
		closesocket(ListenSocket);										//and close program
		WSACleanup();
		return 1;
	}

	//initialize client connection socket
	SOCKET ClientSocket;
	ClientSocket = INVALID_SOCKET;

	//Accept the client's socket
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	//spin off thread and restart!******************************************

	//SEND AND RECEIVE INFORMATION WITH THE CLIENT::
	
	#define DEFAULT_BUFLEN 512

	char buffer[DEFAULT_BUFLEN];
	int result, iSendResult;
	int bufflen = DEFAULT_BUFLEN;

	// Receive until the client shuts down the connection
	do {

		result = recv(ClientSocket, buffer, bufflen, 0);
		if (result > 0) {
			printf("Bytes received: %d\n", result);
			printf("\nMessage received: %d", buffer);

			//send test message back to client
			strcpy(buffer, "Bytes received : " + result);
			iSendResult = send(ClientSocket, buffer, result, 0);
			if (iSendResult == SOCKET_ERROR) {		//close if errors
				printf("send failed: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
			printf("\nBytes sent: %d\n", iSendResult);
		}
		else if (result == 0)		//close connection if shut down by client
			printf("Connection closing...\n");
		else {						//quit on receive error
			printf("recv failed: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}

	} while (result > 0);		//repeat while client sends

	//shutdown the sending half of the client connection
	result = shutdown(ClientSocket, SD_SEND);
	if (result == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

	//close remaining socket components
	closesocket(ClientSocket);		//closes socket
	WSACleanup();					//deallocates data

	return 0;
}