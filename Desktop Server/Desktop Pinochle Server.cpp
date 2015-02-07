/*	Desktop Server -- Pinochle Game Server
Desktop Application
Lucas Weisensee
November 2014

to START THE PROGRAM in command prompt::
executableName PortNumber

This program will function as a desktop game server for a network based pinochle game. It will:
-Listen for new connections.
-Manage incoming player connections.
-Spin off processes to handle new games.
-Allow new players to connect to games in progress.
-Keep track of games in progress.
-Allow games to be added to the database
-Allow games to be requested from the database.


	******************************
	TO DO:
		-accept updates to the active game list from child processes
			-implement mutex lock on active game list
		-error check/parse user answer for different answers/menu navigation than expected
		-keep track of threads created and make sure they're all cleaned up

*/

#define WIN32_LEAN_AND_MEAN


// ********************WINDOWS LIBRARIES::
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

// *******************PRIVATE LIBRARIES::
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\log.h"		//Log writing library
#include "gameList.h"
#include "player.h"

#define DEFAULT_BUFLEN 512
#define GAME_MANAGER_APPLICATION "pinochleGameManager.exe"			// The individual game manager executable
int MAX_THREADS = 20;
int MAX_GAMES = 10;

gameList ACTIVE_GAMES(MAX_GAMES);		// List of general info on all active games
int connections;						// number of connections accepted
int gameCounter;						// number of active games

void serverSetupPrep(int argc, char const *argv[], char * nPORT);		//Sets up the new server, checks inputs
SOCKET setupListenSocket(char * nPORT);				// Setup Listen socket for accepting new connections
DWORD WINAPI handleNewClient(LPVOID* newSocket);
void sendGameList(player * curClient, char * answer);			//sends active game list to client
void interpretClientGameChoice(char * cResponse1, SOCKET ClientSocket, char * gameChoice);	//interprets client's commands/choice
void receiveError(SOCKET ClientSocket);							//handles a receive error, prints to log
void connectToDatabase(player * curClient);						// connects client to the database
void createGame(SOCKET ClientSocket, char * gamename, char * playerName, unsigned char maxplayers);	//initiates the creation of a new game
void joinGame(int gameNumber, char * playerName, SOCKET ClientSocket, unsigned char playernum);	//sends the client to the game specified
void setLogFileName();											//sets log file name with time, date and PID info
void serverCleanUp();					 // Cleans up allocated memory, threads and Socket overhead


bool checkPlayPinochle(char * cResponse1);				// Check player's response1:: returns true if player name string included

int main(int argc, char const *argv[]) {

	int result;				// setup result variable, used for error checking
	char nPORT[6];			// The port number to be used for listening
	serverSetupPrep(argc, argv, nPORT);	// Check inputs and initiate sever settings 


	// Create a SOCKET for the server to listen to
	SOCKET ListenSocket = setupListenSocket(nPORT);

	// Temp Socket for new clients
	SOCKET * ClientSocket;	

	//****** ::MAIN SERVER LOOP::  ***********
	//Wait for incoming connections, accept and spin off new thread to handle them... while the ListenSocket is valid
	while(ListenSocket != INVALID_SOCKET)
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

	// Thread and memory cleanup
	serverCleanUp();
	return 0;
}
void setupServer(int argc, char const *argv[], char * nPORT)		//Sets up the new server, prepares to listen for connections
{
	//Check for proper command line usage
	if (argc < 2) {
		fprintf(stderr, "\n%s usage: %s port-number\n", argv[0], argv[0]);
		exit(0);
	}

	// Set port number
	strncpy(nPORT, argv[1], strlen(argv[1]));
	nPORT[strlen(argv[1])] = '\0';

	// Initiate global variables
	setLogFileName();
	gameCounter = 0;

	printf("\nSetting up connection\n");




}

SOCKET setupListenSocket(char * nPORT) {
	SOCKET ListenSocket;

	//initialize windows socket
	WSADATA wsaData;


	// Initiate Windows Socket
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0){
		writetolog("WSAStartup failed! Returned:%d\n", result);
		return INVALID_SOCKET;
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
		return  INVALID_SOCKET;;
	}


	ListenSocket = socket(socketInfo->ai_family, socketInfo->ai_socktype, socketInfo->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) {
		writetolog("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(socketInfo);
		WSACleanup();
		return  INVALID_SOCKET;;
	}

	//Setup the TCP listening socket
	result = bind(ListenSocket, socketInfo->ai_addr, (int)socketInfo->ai_addrlen);
	if (result == SOCKET_ERROR) {
		writetolog("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(socketInfo);
		closesocket(ListenSocket);
		WSACleanup();
		return  INVALID_SOCKET;;
	}

	//deallocate socket address memory
	freeaddrinfo(socketInfo);

	//listen for client connection on socket created
	//SOMAXCONN allows for max reasonable connections as determined by the system
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR){				//if listen fails
		writetolog("Listen() failed with error: %ld\n", WSAGetLastError());	//print error
		closesocket(ListenSocket);										//and close program
		WSACleanup();
		return  INVALID_SOCKET;;
	}

	return ListenSocket;
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
	player curClient((SOCKET)newSocket);	// Create new player object
	curClient.setName();

	//Direct the client to the correct path and close the connection when finished
	int answer = curClient.getConnectionType();

	if (answer == 1)					//if the client wants to play games							
		playGames(&curClient);
	else if (answer == 2)				// If the client wants to access the game database
		connectToDatabase(&curClient);

	// End current connection and operations
	WSACleanup();				//deallocates data after execution's complete
	return 1;					//close program
}

void playGames(player * curClient)		// Queries and sends client to desired game-path
{
	// Send list of current games to client
	std::string * currentGameList;		//string to pass to client
	ACTIVE_GAMES.getCurrent(currentGameList);	//gets list of active games for client to connect to
	curClient->send(currentGameList->c_str());		// Send to current list client

	// Parse user game choice: start new game or join an existing one
	int choice = curClient->getGameChoice();	// Get game choice from player
	if (choice == 0)							// if player chose to setup a new game
		setupNewGame(&curClient);					// set it up
	else if (choice > 0)						// If player chose to connect to an existing game
		interpretClientGameChoice();				// connect to it

}

void interpretClientGameChoice()	//interprets client's commands/choice
{
	//parse answer from client (cResponse2)::
	unsigned short gameChoice = (unsigned short)strtoul(cResponse2, NULL, 0);

	// Handle pinochle gaming request:
	if (checkPlayPinochle(cResponse1)) {			//if client chose to play pinochle and formatted packet correctly
		// execute game request
		if (gameChoice == 0)					//if the player requested a new game
			//create the new game: socket, playername, gamename, maxplayers:
			createGame(ClientSocket, &cResponse1[1], &cResponse2[4], (unsigned char)cResponse1[2]);
		else if (gameChoice > 1)								//otherwise join the game requested
			// join game: gameID, playerName, PlayerSocket, PlayNumber:
			joinGame(gameChoice, &cResponse1[1], ClientSocket, cResponse2[3]);
		else if (gameChoice < 0)								//write to log if errors result
			writetolog("join game response failure: ", WSAGetLastError());
	}
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

void receiveError(player * curClient)		//handles a receive error, prints to log
{
		writetolog("receive failed: ", WSAGetLastError());
		closesocket(curClient->getSocket());
		WSACleanup();
}

void connectToDatabase(player * curClient)	// connects client to the database
{
	std::string buffer;
	buffer += "Hi, game database coming soon...";
	curClient->send(buffer.c_str());
}

// Creates a new thread for a new game manager by the given client
void createGame(player * curClient)	{
	STARTUPINFO startupInfo;
	_PROCESS_INFORMATION * processInfo;

	// Create new game process
	CreateProcess(GAME_MANAGER_APPLICATION
		NULL,			// Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&startupInfo,   // STARTUPINFO structure
		processInfo);   // PROCESS_INFORMATION structure

	// Wait for new process to be setup/idle (processHANDLE, MillisecondWait)
	DWORD ready = WaitForInputIdle(processInfo->hProcess, 5000);

	if (ready == 0)					// If the new process if ready to receive game info
		ACTIVE_GAMES.addGame(processInfo, curClient);
	else
		writetolog("Process initiation failure: ", ready);		// Otherwise write failure to log file
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

void serverCleanUp() { // Cleans up allocated memory, threads and Socket overhead
	
	writetolog("Exiting server")

	// Do other stuff
}
