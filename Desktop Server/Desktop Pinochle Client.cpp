/*	Testing Client -- Pinochle Game Server
Desktop Application
Lucas Weisensee
November 2014

This program will function as a testing client for the pinochle game server.
It will allow a developer to run a series of testing functions locally.
*/

#define WIN32_LEAN_AND_MEAN

//#include "stdafx.h"
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>


// link with required libraries
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512


int main(int argc, char **argv)
{
	char testing[4];			//*************************************************************

	
	//Check for proper command line usage
	if (argc < 3) {
		fprintf(stderr, "\n%s usage: %s hostname portnumber\n", argv[0], argv[0]);
		exit(0);
	}

	char nPORT[6];
	strcpy(nPORT, argv[2]);
	nPORT[strlen(argv[2])] = '\0';	


	//initialize socket datastore

	WSADATA wsaData;
	SOCKET serverSocket = INVALID_SOCKET;
	struct addrinfo *socketInfo = NULL,
		*tempInfo = NULL,
		hints;

	//Test message
	char sendbuf[DEFAULT_BUFLEN];
	char recvbuf[DEFAULT_BUFLEN];
	int result;
	int recvbuflen = DEFAULT_BUFLEN;

	//Initialize Winsock for socket creation
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		printf("WSAStartup w/wsaData failed! Error: %d\n", result);
		return 1;
	}

	//wipe address info to prepare for rewrite and copy over default values
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	//Resolve the server address and port
	result = getaddrinfo(argv[1], nPORT, &hints, &socketInfo);
	if (result != 0) {
		printf("getaddrinfo failed with error: %d\n", result);
		WSACleanup();
		return 1;
	}

	//Attempt to connect to a server address until one succeeds
	for (tempInfo = socketInfo; tempInfo != NULL; tempInfo = tempInfo->ai_next) {

		//Create a SOCKET for connecting to server
		serverSocket = socket(tempInfo->ai_family, tempInfo->ai_socktype,
			tempInfo->ai_protocol);
		if (serverSocket == INVALID_SOCKET) {
			printf("socket() creation failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}

		//Attempt to connect to server.
		result = connect(serverSocket, tempInfo->ai_addr, (int)tempInfo->ai_addrlen);
		if (result == SOCKET_ERROR) {	//if connection failed, continue through list of addresses
			closesocket(serverSocket);		//close current socket
			serverSocket = INVALID_SOCKET;	//reset it's status
			continue;					//
		}
		break;
	}

	freeaddrinfo(socketInfo);	//deallocate unneeded address info

	if (serverSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

	printf("Bytes Sent: %ld\n", result);

	//talk to server
	do 
	{
		printf("\nEnter a message to send to the server: ");
		std::cin.getline(sendbuf, DEFAULT_BUFLEN, '\n');		//get user input

		//send message and check for errors
		result = send(serverSocket, sendbuf, (int)strlen(sendbuf), 0);
		printf("Bytes sent: %d.\n", result);

		if (result == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(serverSocket);
			WSACleanup();
			return 1;
		}
		else if (result <= 1)		//close connection if user wants
			result = 0;
		else{
					//receive message from server
			result = recv(serverSocket, recvbuf, recvbuflen, 0);	//get data from socket
			recvbuf[result] = '\0';

			if (result > 0){										//interpret received data
				printf("\nBytes received: %d", result);
				printf("\nMessage received: %s", recvbuf);
			}
			else if (result == 0)
				printf("\nConnection closed");
			else
				printf("\nrecv failed with error: %d\n", WSAGetLastError());
		}

	} while (result > 0);			//repeat while the server sends data

	//shutdown the sending connection component
	result = shutdown(serverSocket, SD_SEND);
	if (result == SOCKET_ERROR) {			//check result of shutdown
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(serverSocket);
		WSACleanup();
		return 1;			//exit on error
	}

	//memory and socket cleanup
	closesocket(serverSocket);
	WSACleanup();

	return 0;	//exit
}
