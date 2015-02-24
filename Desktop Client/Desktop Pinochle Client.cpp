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

#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\LogFile.h"		// Log writing class
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\client.h"		// Client communication class
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\query.h"		// Client communication class
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\serverException.h"		// Client communication class


// link with required libraries
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512

LogFile LogF;
query userQuery;


bool setupConnection(int argc, char **argv, char * nPORT);	// Setup connection to server
SOCKET setupListenSocket(char * nPORT, char **argv);		// Setup listen socket with server
void communicateWithServer(client * server);				// Run tests/communicate with server
bool launchEchoRoom(client * server);						// Echo testing room with server
bool playGames(client * server);			// start playing games
bool liveDataEntry(client * server);		// start live entry mode
bool accessDatabase(client * server);		// connect to database

int main(int argc, char **argv)	
{
	// Setup connection
	char nPORT[7];
	SOCKET serverSocket = INVALID_SOCKET;
	bool success = setupConnection(argc, argv, nPORT);

	// if connection setup was successful, continue with socket setup
	if (success) {
		//initialize socket 
		SOCKET listenSocket = setupListenSocket(nPORT, argv);
		if (listenSocket == INVALID_SOCKET) {
			fprintf(stderr, "error: INVALID_SOCKET, quitting.");
			LogF.writetolog("error: INVALID_SOCKET, quitting.");
			exit(0);
		}

		// if the socket setup was successful, continue with testing interface
		else {
			// Create server object
			client * server = new client(listenSocket);

			// testing interface with server
			try {
				communicateWithServer(server);
			} catch (serverException& e) {
				printf(e.sendEr());
			}
		}
	}

	// if connection setup failed, exit
	else {
		printf("connection setup failed, exiting");
		LogF.writetolog("connection setup failed.");
		exit(0);
	}
	return 0;	// exit
}
void communicateWithServer(client * server) {				// Run tests/communicate with server
	/*     parameter testing::
	// value testing
	printf(received->c_str());
	std::cout << "\nreceived->length: " <<  received->length();
	printf("\nserver->getAnswerType(): %u", server->getAnswerType());
	std::cout << "\nreceived->length(): " << received->length();
	std::cout << "\nreceived->at(0): " << received->at(0);


	if (result == 1)
	const char servStatus = received->at(0);			// server status compare value, only necessary if

	if (result == 1)
		if (server->getAnswerType() == S_STATUS)
			if (received->length() > 0)
				if (atoi(&servStatus) == 1)
					printf("\nserver ready message received successfully");
					*/


	// Check that server is synced/ready for connection
	std::string * received = new std::string();

	bool continu = true;

	while (continu) {
		int result = server->getStrAnswer(received);

		// if server message is as expected: check that server is setup, synced and ready, continue
		if (result == 1										//check that message was successfully received
			&& server->getAnswerType() == S_STATUS			// check that appropriate message type was received
			&& received->length() > 0						// check that received message exists
			&& atoi((const char*)&received->at(0)) == 1) {	// check that message received from server was a 'ready' signal (='1') 

			// get user direction
			int ans = -1;
			while (ans < 0 || ans > 4)
				ans = userQuery.iQuery("\nServer Ready\nWhat would you like to do?\n[0] - Quit\n[1] - Request list of active games\n[2] - Live Game Data Entry Mode\n[3] - Access Database\n[4] - Echo Testing\n\n");
			char temp[32];
			itoa(ans, temp, 30);	// convert int to string
			result = server->sendM(CON_TYPE, temp);		// and send to server

			switch (ans) {					// execute user command
			case 0:
				break;						// quit connection
			case 1:
				continu = playGames(server);			// start playing games
				break;
			case 2:
				continu = liveDataEntry(server);		// start live entry mode
				break;
			case 3:
				continu = accessDatabase(server);		// connect to database
				break;
			case 4:
				continu = launchEchoRoom
					(server);		// start echo testing with server
				break;
			default:
				printf("switch error in comminicateWithServer");	// error state
				break;
			}
		}
		// if server is not ready
		else {
			std::string rchar;				// print out failure information
			if (result == 1)
				rchar = received->at(0);
			else
				rchar = "non-valid result";
			printf("\nserver not connected, synced or ready, msg result = %d, message type received: %c, received->at(1): %s", result, server->getAnswerType(), rchar);
			LogF.writetolog("connection setup failed.");
		}
	}
}
bool setupConnection(int argc, char **argv, char * nPORT) {	// Initiates connection 

	// Setup Log File
	LogF.setLogFileName("Client_log_file_", "C:\\Users\\Pookey\\OneDrive\\Projects\\PinochleGame\\logs\\TestingClient");


	//Check for proper command line usage
	if (argc < 3) {
		fprintf(stderr, "\n%s usage: %s hostname portnumber\n", argv[0], argv[0]);
		LogF.writetolog("usage: hostname portnumber", (int)stderr);
		return false;
	}

	// Set Port Number
	strncpy(nPORT, argv[2], strlen(argv[2]));
	nPORT[strlen(argv[2])] = '\0';

	printf("\nSetting up connection to Server\n");
	return true;
}
SOCKET setupListenSocket(char * nPORT, char **argv) {
	//initialize windows socket
	WSADATA wsaData;
	SOCKET serverSocket;

	// Initiate Windows Socket
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0){
		LogF.writetolog("WSAStartup failed! Returned:%d\n", result);
		return INVALID_SOCKET;
	}

	// Setup socket and connection objects
	struct addrinfo *socketInfo = NULL, *tempInfo = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	//Get's address info from system for port and socket connection
	// check for failure
	//result = getaddrinfo(argv[1], nPORT, &hints, &socketInfo);
	result = getaddrinfo("localhost", nPORT, &hints, &socketInfo);
	if (result != 0){
			// if it failed, write log and print error messages
		fprintf(stderr, "getaddrinfo failed with error: %s\n", gai_strerror(result));
		printf("Calling getaddrinfo with following parameters:\n");
		printf("\tnodename = %s\n", argv[1]);
		printf("\tservname (or port) = %s\n\n", nPORT);
		LogF.writetolog("getaddrinfo failed with error: %d\n", gai_strerror(result));
		WSACleanup();	//deallocate memory and quit
		return  INVALID_SOCKET;;
	}

	//Attempt to connect to a server address until one succeeds
	for (tempInfo = socketInfo; tempInfo != NULL; tempInfo = tempInfo->ai_next) {

		//Create a SOCKET for connecting to server
		serverSocket = socket(tempInfo->ai_family, tempInfo->ai_socktype,
			tempInfo->ai_protocol);
		if (serverSocket == INVALID_SOCKET) {
			printf("socket() creation failed with error: %ld\n", WSAGetLastError());
			LogF.writetolog("socket() creation failed with error: %ld\n", WSAGetLastError());
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

	return serverSocket;
}
bool playGames(client * server) {			// start playing games
	printf("playing games!");
	return true;
}
bool liveDataEntry(client * server) {		// start live entry mode
	printf("beginning live data entry mode");
	return true;
}
bool accessDatabase(client * server) {		// connect to database
	printf("accessing database...");
	return true;
}
bool launchEchoRoom(client * server) {					// Echo testing room with server
	// make "quit" compare string:
	std::string quitString = "quit";
	int result;
	quitString.insert(quitString.begin(), (char)MSG);	// insert message code to synthesize quitString
	std::string * toSend;								// message to send to server
	std::string * received = new std::string();			// message received

	// echo with server until user types quit to stop
	while (true) {
		result = server->getStrAnswer(received);	// get message from server
		printf(received->c_str());		// print message received

		// If message was sent successfully
		if (result == 1) {
			// get desired message from user
			toSend = userQuery.sQuery("\nWhat would you like to send to the server? enter 'quit' to quit\n");

			// send message to server
			server->sendM(MSG, toSend);

			// quit if the user requested to
			if (strcmp(toSend->c_str(), quitString.c_str()) == 0)
				break;
		}
		// If message was not sent successfully
		else {
			printf("message receive error: %i", result);	// print error
			return false;									// quit connection with server
		}
	}

	// notify user and exit echo session to menu
	printf("exiting echo session");
	return true;
}
