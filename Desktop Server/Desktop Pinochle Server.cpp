/*	Desktop Server -- Pinochle Game Server
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
-Allow games to be added to the database
-Allow games to be requested from the database.

*/

#define WIN32_LEAN_AND_MEAN

//#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")	//Winsock server needs to be linked with libraries

#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\log.h"		//Log writing library
#include "gameList.h"

#define DEFAULT_BUFLEN 512
int MAX_THREADS = 20;
int MAX_GAMES = 10;

gameList ACTIVE_GAMES(MAX_GAMES);		// List of general info on all active games
int connections;						// number of connections accepted
int gameCounter;						// number of active games

void serverSetupPrep(int argc, char const *argv[], char * nPORT);		//Sets up the new server, checks inputs
DWORD WINAPI handleNewClient(LPVOID* newSocket);
void sendGameList(SOCKET ClientSocket, char * answer);			//sends active game list to client
void interpretClientGameChoice(char * cResponse1, SOCKET ClientSocket, char * gameChoice);	//interprets client's commands/choice
void receiveError(SOCKET ClientSocket);							//handles a receive error, prints to log
void accessDatabase(char * playerName, SOCKET ClientSocket);	//handles client access to database
void createGame(SOCKET ClientSocket, char * gamename, char * playerName, unsigned char maxplayers);	//initiates the creation of a new game
void joinGame(int gameNumber, char * playerName, SOCKET ClientSocket, unsigned char playernum);	//sends the client to the game specified
void setLogFileName();											//sets log file name with time, date and PID info

bool checkPlayPinochle(char * cResponse1);				// Check player's response1:: returns true if player name string included

int main(int argc, char const *argv[]) {

	int result;				// setup result variable, used for error checking
	char nPORT[6];			// The port number to be used for listening
	serverSetupPrep(argc, argv, nPORT);	// Check inputs and initate sever settings 

				// DWORD   ThreadIdArray[MAX_THREADS];		//Threads the server has created
				// HANDLE  ThreadArray[MAX_THREADS]; 		//stores thread handles

	//initialize windows socket
	//initializeWinSocket()
	WSADATA wsaData;



	// Initiate Windows Socket
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0){
		writetolog("WSAStartup failed! Returned:%d\n", result);
		return 1;
	}


	// Setup socket and connection objects
	struct addrinfo *socketInfo = NULL, *ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	//Get's address info from system for port and socket connection

	result = getaddrinfo(NULL, nPORT, &hints, &socketInfo);
	if (result != 0){
		writetolog("getaddrinfo failed with error: %d\n", result);
		WSACleanup();	//deallocate memory and quit
		return 1;
	}


	// Create a SOCKET for the server to listen to
	SOCKET ListenSocket = INVALID_SOCKET;
	ListenSocket = socket(socketInfo->ai_family, socketInfo->ai_socktype, socketInfo->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) {
		writetolog("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(socketInfo);
		WSACleanup();
		return 1;
	}

	//Setup the TCP listening socket
	result = bind(ListenSocket, socketInfo->ai_addr, (int)socketInfo->ai_addrlen);
	if (result == SOCKET_ERROR) {
		writetolog("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(socketInfo);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	//deallocate socket address memory
	freeaddrinfo(socketInfo);

	//listen for client connection on socket created
	//SOMAXCONN allows for max reasonable connections as determined by the system
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR){				//if listen fails
		writetolog("Listen() failed with error: %ld\n", WSAGetLastError());	//print error
		closesocket(ListenSocket);										//and close program
		WSACleanup();
		return 1;
	}

	SOCKET * ClientSocket;	// Temp Socket for new clients


	//Wait for incoming connections, accept and spin off new thread to handle them...
	while(true)
	 {

		//initialize client connection socket
		ClientSocket = new SOCKET;
		*ClientSocket = INVALID_SOCKET;

		//Accept the client's socket
		*ClientSocket = accept(ListenSocket, NULL, NULL);
		if (*ClientSocket == INVALID_SOCKET) {
			writetolog("accept failed: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}

		//spin off thread to handle current user return to listening
		//ThreadArray[*i%10] = 
		CreateThread( 
            NULL,             			// use default security attributes
            0,          				// use default stack size  
            (LPTHREAD_START_ROUTINE)handleNewClient,		// thread function name
            (void*)*ClientSocket,		// argument to thread function 
            0,                      	// use default creation flags 
            0);		// returns the thread identifier 

		connections++;
	}
}
void setupServer(int argc, char const *argv[], char * nPORT)		//Sets up the new server, prepares to listen for connections
{
	//Check for proper command line usage
	if (argc < 2) {
		fprintf(stderr, "\n%s usage: %s portnumber\n", argv[0], argv[0]);
		exit(0);
	}

	// Set port number
	strcpy(nPORT, argv[1]);
	nPORT[strlen(argv[1])] = '\0';

	// Initiate global variables
	setLogFileName();
	gameCounter = 0;

	printf("\nSetting up connection\n");
}
	// Handles an individual Client::
/* 
	Parses and Executes Client's commands
		0: initiates database access mode
		1: sends the list of active games
		2: receives user command:
			0n%s: create new game with n users and %s name
			xn:  join game x as n user
*/
DWORD WINAPI handleNewClient(LPVOID* newSocket) {
	//initiate variables
	char cResponse1[DEFAULT_BUFLEN];	//player answer from preliminary querry
	char * cResponse2 = NULL;
	int result;
	SOCKET ClientSocket = (SOCKET)newSocket;

	//Direct the client to the correct path and close the connection when finished
			//get message from client
	result = recv(ClientSocket, cResponse1, DEFAULT_BUFLEN, 0);

	if(result >= 0)	{				//if the message was received successfully							
		if (cResponse1[0] == 1)							//play the game if client requests it
			sendGameList(ClientSocket, cResponse2);		//send game list to player
		interpretClientGameChoice(cResponse1, ClientSocket, cResponse2);
	}

	else 							//close if errors on packet receipt
		receiveError(ClientSocket);

	WSACleanup();					//deallocates data after execution's complete

	return 0;						//close program
}

void sendGameList(SOCKET ClientSocket, char * answer)
{
	int result;					//send/receive result
	char * currentGameList;		//string to pass to client
	ACTIVE_GAMES.getCurrent(currentGameList);	//gets list of active games for client to connect to
	result = send(ClientSocket, currentGameList, strlen(currentGameList), 0);		//send list
	if(result > 0)								//if the sending was successful
	{												//get answer
		result = recv(ClientSocket, answer, DEFAULT_BUFLEN, 0);	//get response back
		if(result < 1)
			writetolog("error, receive game choice result < 1 ", WSAGetLastError());	//write to log if receive failed
	}
	else						//write error to log if sending failed
		writetolog("error, sendGameList result < 1 ", WSAGetLastError());
}

void interpretClientGameChoice(char * cResponse1, SOCKET ClientSocket, char * cResponse2)	//interprets client's commands/choice
{
	//parse answer from client (cResponse2)::
	unsigned short gameChoice = (unsigned short)strtoul(cResponse2, NULL, 0);

	// Handle pinochle gaming request:
	if(checkPlayPinochle(cResponse1)) {			//if client chose to play pinochle and formatted packet correctly
		// execute game request
		if(gameChoice == 1)					//if the player requested a new game
			//create the new game: socket, playername, gamename, maxplayers:
			createGame(ClientSocket, &cResponse1[1], &cResponse2[4], (unsigned char)cResponse1[2]);	
		else if (gameChoice > 1)								//otherwise join the game requested
			// join game: gameID, playerName, PlayerSocket, PlayNumber:
			joinGame(gameChoice, &cResponse1[1], ClientSocket, cResponse2[3]);
		else if (gameChoice < 0)								//write to log if errors result
			writetolog("join game response failure: ", WSAGetLastError());	
		}
	if(cResponse1[0] == 0)	//initiate database connection if the client requests it
		// Start database access with: playerName, Socket
		accessDatabase(&cResponse1[1], ClientSocket);
}

// Check player's response1:: returns true if player name string included
bool checkPlayPinochle(char * cResponse1)
{
	// If the client wants to play and sent the packet in correct format
	if(cResponse1[0] == 1 && '^' == cResponse1[1])		//if they chose to play
		if (cResponse1[2] >0)						//and included a player name
			return 1;
		else 
			writetolog("error with appended playerName in cResponse1: ", cResponse1[2]);
	return 0;
}

void receiveError(SOCKET ClientSocket)		//handles a receive error, prints to log
{
		writetolog("receive failed", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
}

void accessDatabase(char * playerName, SOCKET ClientSocket)	// handles client access to database
{
	std::string buffer;
	buffer += "Hi, " ;
	buffer += *playerName;
	buffer += "! \ngame database coming soon...";
	send(ClientSocket, buffer.c_str(), buffer.length(), 0);
}

// Creates a new game with given arguments and adds it to list.
void createGame(SOCKET ClientSocket, char * gamename, char * playerName, unsigned char maxplayers)	
{
	// Initiate game
	game newGame(ClientSocket, gamename, playerName, maxplayers); 

	if(newGame.initiated())  {						//If initiation is successful:
		ACTIVE_GAMES.add(&newGame);							// add to global active newGame list
		newGame.run();										// start newGame
	}
	else
		writetolog("game initiation failure");			// Otherwise write failure to log file
}

// Adds specified client to game
void joinGame(int gameNumber, char * playerName, SOCKET ClientSocket, unsigned char playernum)				// sends the client to the game specified
{
	int result = ACTIVE_GAMES.addPlayer(gameNumber, playerName, ClientSocket, playernum);		// adds player to game, returns result
	if(result)
		exit(EXIT_SUCCESS);
	else {
		writetolog("ACTIVE_GAMES.addPlayer() failure: ", result);
		exit(EXIT_SUCCESS);
	}
}

