/*	Desktop Server -- Pinochle Game Server
Desktop Application
Lucas Weisensee
January 2015

to START THE PROGRAM in command prompt::
executableName PortNumber

This program will function as a desktop game server for a network based pinochle game. It will:
-Listen for new connections.
-Manage incoming client connections.
-Spin off processes to handle new games.
-Allow new clients to connect to games in progress.
-Keep track of games in progress.
-Allow games to be added to the database
-Allow games to be requested from the database.


	******************************
	TO DO:
		-accept updates to the active game list from child processes
			-implement mutex lock on active game list
		-error check/parse user answer for different answers/menu navigation than expected
		-keep track of threads created and make sure they're all cleaned up
		-To avoid resource leaks in a larger application, close handles explicitly. 

*/

#define WIN32_LEAN_AND_MEAN


// ********************WINDOWS LIBRARIES::
//#include "stdafx.h"
#pragma once
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
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\LogFile.h"		//Log writing library
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Desktop Server\cardGame.h"
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Desktop Server\gameList.h"
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\client.h"

#define DEFAULT_BUFLEN 512
#define GAME_MANAGER_APPLICATION "pinochleGameManager.exe"			// The individual game manager executable
#define DEBUG_MODE 1		// debugging mode 1 = on, 0 = off
LogFile LogF;
int MAX_THREADS = 20;
int MAX_GAMES = 10;
gameList ACTIVE_GAMES(MAX_GAMES);		// List of general info on all active games
int connections;						// number of connections accepted
HANDLE connectionsMutex;

void setupServer(int argc, char const *argv[], char * nPORT);		//Sets up the new server, checks inputs
SOCKET setupListenSocket(char * nPORT);				// Setup Listen socket for accepting new connections
DWORD WINAPI handleNewClient(LPVOID* newSocket);// Handles a new client when connection is formed
void incrementClientCount();					// obtains mutex lock and increment/print client counter
bool playGames(client * curClient);				// sends active game list to client, returns true if client may wish to continue server connection, false otherwise
void receiveError(SOCKET ClientSocket);			// handles a receive error, prints to log
bool liveGameDataEntry(client * curClient);		// allows client to enter live game data entry mode, returns true if client may wish to continue server connection, false otherwise
bool consoleEchoTest(client * curClient);		// launches console echo testing, returns true if client may wish to continue server connection, false otherwise
bool connectToDatabase(client * curClient);		// connects client to the database, returns true if client may wish to continue server connection, false otherwise
bool createGame(client * curClient);			// initiates the creation of a new game
bool joinGame(int choice, client * curClient);	// sends the client to the game specified
void serverCleanUp();							// Cleans up allocated memory, threads and Socket overhead
void parseNewGameInfo(char * gameInfo, char * type, int * playerNum, int * obsNum, int * goal, std::string * gameName);	// parses the new game info string and copies the info into the appropriate data location
int singleThreadedHandler(SOCKET ListenSocket);	// debugging mode: alternative client handler, creates no additional threads


int main(int argc, char const *argv[]) {
	char nPORT[7];			// The port number to be used for listening
	setupServer(argc, argv, nPORT);	// Check inputs and initiate sever settings 

	// Create a SOCKET for the server to listen to
	SOCKET ListenSocket = setupListenSocket(nPORT);
	printf("\nServer initialized and listen socket setup, preparing to listen on port %s", nPORT);

	// Temp Socket for new clients
	SOCKET * ClientSocket;	

	// if in debugging mode, run single threaded server
	if (DEBUG_MODE)
		// Single threaded testing server
		singleThreadedHandler(ListenSocket);
	else
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
				LogF.writetolog("accept failed: %d\n", WSAGetLastError());
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
int singleThreadedHandler(SOCKET ListenSocket) {		// debugging mode: alternative client handler, creates no additional threads
	//******  ::DEBUGGING SERVER LOOP::  ***********
	// Wait for incoming connections, accept and spin off new thread to handle them, only once! 
	// to maintain single threaded server for testing purposes

	printf("\nEntering debugging mode: single threaded server");

	// Temp Socket for new client
	SOCKET * ClientSocket;

	//initialize client connection socket
	ClientSocket = new SOCKET;
	*ClientSocket = INVALID_SOCKET;

	//Accept the client's socket
	*ClientSocket = accept(ListenSocket, NULL, NULL);
	if (*ClientSocket == INVALID_SOCKET) {
		LogF.writetolog("accept failed: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	else
		printf("\nAccept succeeded.");

	// handle current user in current thread with handleNewClient routine
	handleNewClient((LPVOID*)*ClientSocket);		// thread function name
	return 1;
}

void setupServer(int argc, char const *argv[], char * nPORT) {		//Sets up the new server, prepares to listen for connections
	//Check for proper command line usage
	if (argc < 2) {
		fprintf(stderr, "\n%s usage: %s port-number\n", argv[0], argv[0]);
		exit(0);
	}

	// Set port number
	strncpy(nPORT, argv[1], strlen(argv[1]));
	nPORT[strlen(argv[1])] = '\0';	

	// Initiate server type log file in location specified
	LogF.setLogFileName("server log file_", "C:\\Users\\Pookey\\OneDrive\\Projects\\PinochleGame\\logs\\server");

	// Initialize Mutex with no security attr, owner or name
	connectionsMutex = CreateMutex(NULL, false, NULL);
	if (connectionsMutex == NULL){
		perror("connections mutex error: ");
		exit(0);
	}

}
SOCKET setupListenSocket(char * nPORT) {
	SOCKET ListenSocket;

	//initialize windows socket
	WSADATA wsaData;


	// Initiate Windows Socket
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0){
		LogF.writetolog("WSAStartup failed! Returned:%d\n", result);
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
		LogF.writetolog("getaddrinfo failed with error: %d\n", result);
		WSACleanup();	//deallocate memory and quit
		return  INVALID_SOCKET;;
	}


	ListenSocket = socket(socketInfo->ai_family, socketInfo->ai_socktype, socketInfo->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) {
		LogF.writetolog("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(socketInfo);
		WSACleanup();
		return  INVALID_SOCKET;;
	}

	//Setup the TCP listening socket
	result = bind(ListenSocket, socketInfo->ai_addr, (int)socketInfo->ai_addrlen);
	if (result == SOCKET_ERROR) {
		LogF.writetolog("bind failed with error: %d\n", WSAGetLastError());
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
		LogF.writetolog("Listen() failed with error: %ld\n", WSAGetLastError());	//print error
		closesocket(ListenSocket);										//and close program
		WSACleanup();
		return  INVALID_SOCKET;;
	}

	return ListenSocket;
}
DWORD WINAPI handleNewClient(LPVOID* newSocket) {	// Handles an individual Client::
	// notify user/terminal
	incrementClientCount();

	//initiate variables
	client curClient((SOCKET)newSocket);	// Create new client object
	//curClient.setName();
	bool continu = true;		// does the client want to quit
	int answer, result = 0;			// result of client's interactions

	//Direct the client to the correct path and close the connection when finished
		/* execute clients request, see "packet specifications.txt" for more info
				1: Request the list of active games
				2: Game Data entry mode
				3: Initiates database access mode
				4: console echo testing */
	while (continu) {
		// get client connection preference
		std::string temp = "1";
		curClient.sendM(S_STATUS, &temp);
		//**************************NEEDS NULL POINTER CHECK*********************************
		result = curClient.getIntAnswer(&answer);	// save client's answer

		// if the answer was received successfully
		if (result == 1) {
			// initiate the connection the client wants
			switch (answer) {
			case 0:						// if the client wants quit						
				continu = false;		// quit
				break;
			case 1:						// if the client wants to play games							
				continu = playGames(&curClient);
				break;
			case 2:						// allows client to enter live game data entry mode
				continu = liveGameDataEntry(&curClient);
				break;
			case 3:						// If the client wants to access the game database
				continu = connectToDatabase(&curClient);
				break;
			case 4:
				continu = consoleEchoTest(&curClient);	// launches console echo testing
				break;
			default:		// if client didn't enter a valid command
				continu = 1;	// allow them to try again
				break;
			}
		}
		// if client connection is closed
		else if (result == -1)
			return false;		// quit
		// if message receiving error occurred
		else {
			printf("messaging error occurred, result: %d", result);
			continu = false;
		}
	}
	// End current connection and operations
	WSACleanup();				//deallocates data after execution's complete
	return 1;					//close thread
}
bool playGames(client * curClient)		// Queries and sends client to desired game-path
{
	// Send list of current games to client
	std::string currentGameList = ACTIVE_GAMES.getCurrent();	//gets list of active games for client to connect to
	curClient->sendM(G_LIST, &currentGameList);		// Send to current list client

	// Parse user game choice: start new game or join an existing one
	int choice;
	int result = curClient->getIntAnswer(&choice, G_CHOICE);	// Get game choice from client

	// if receive was successful
	if (result == 1) {
		if (choice == 0)						// if client chose to setup a new game
			return createGame(curClient);				// set it up
		else if (choice > 0)					// If client chose to connect to an existing game
			return joinGame(choice, curClient);		// connect to it
	}
	// if receive failed
	else {
		LogF.writetolog("Receive Error: playGames: getIntAnswer: ", result);		// write failure to log and exit
		// if connection was closed
		if (result == -1)
			return 0;		// quit connection
	}

	// return to main menu
	return true;

}
void receiveError(client * curClient)		//handles a receive error, prints to log, returns true if client may wish to continue server connection, false otherwise
{
		LogF.writetolog("receive failed: ", WSAGetLastError());
		closesocket(curClient->getSocket());
		WSACleanup();
}
bool connectToDatabase(client * curClient)	// connects client to the database, returns true if client may wish to continue server connection, false otherwise
{
	std::string buffer;
	buffer += "Hi, game database coming soon...";
	int result = curClient->sendM(0, buffer.c_str());			// send descriptive error message to client

	if (result <= 0)
		return false;
	
	// return to main menu
	return true;
}
bool createGame(client * curClient) {			//initiates the creation of a new game
	// request new game creation info
	std::string gameInfo;
	int result = curClient->getStrQueryAnswer(&gameInfo, N_GQUERY, N_GINFO);
	if (result == 1) {
		char type;
		int playerNum;
		int obsNum;
		int goal;
		std::string gameName;
		char * infostring = new char[gameName.length() + 1];
		strcpy(infostring, gameName.c_str());

		// Parse new game info
		parseNewGameInfo(infostring, &type, &playerNum, &obsNum, &goal, &gameName);

		// create, launch and manage new game
		cardGame newGame(curClient, &type, &playerNum, &obsNum, &goal, &gameName);
		return true;
	}
	// if client closed connection
	else if (result == -1) {
		printf("client connection unexpectedly closed, result: %d", result);
		return false;
	}
	// if new game info was not received
	else {
		printf("new game info not received");
		return true;
	}
}
void parseNewGameInfo(char * gameInfo, char * type, int * playerNum, int * obsNum, int * goal, std::string * gameName) { // parses the new game info string and copies the info into the appropriate data location
	// Tokenize string by '^'
	char * tkns = strtok(gameInfo, "^");
	
	// game type
	*type = gameInfo[0];

	// Number of players
	tkns = strtok(NULL, "^") + sizeof(char);
	*playerNum = std::stoi(tkns, NULL, 10);

	// Number of Observers
	tkns = strtok(NULL, "^");
	*obsNum = std::stoi(tkns, NULL, 10);

	// Winning Score
	tkns = strtok(NULL, "^");
	*goal = std::stoi(tkns, NULL, 10);

	// game Name
	tkns = strtok(NULL, "^");
	gameName = new std::string(tkns);	// copy name to new arg
}

bool consoleEchoTest(client * curClient) {		// launches console echo testing, returns true if client may wish to continue server connection, false otherwise
	// send welcome message to client
	int result = curClient->sendM(MSG, "Welcome to the console echo test! what would you like me to echo?");

	// echo messages with client until they want to quit
	std::string answer;
	bool quit = false;
	while (!quit)	
	{
		result = curClient->getStrAnswer(&answer);

		if (strcmp(answer.c_str(), "quit") == 0) {
			printf("\nclient message matches 'quit', client message: %s", answer.c_str());
			quit = true;
		}

		// if the client shuts down the connection or sends message "quit"
		else if (result == -1)
			return false;	// quit on the server end as well

		// if the message was received successfully
		else if (result == 1) {
			answer.insert(0, "received: ");	// modify
			curClient->sendM(MSG, &answer);
		}

	}

	return true;
}
bool liveGameDataEntry(client * curClient) {				// allows client to enter live game data entry mode, returns true if client may wish to continue server connection, false otherwise
	printf("executing \"liveGameDataEntry\" function");
	return true;
}
// Adds specified client to game
bool joinGame(int choice, client * curClient)				// sends the client to the game specified
{
	int successful = ACTIVE_GAMES.addclient(choice, curClient);		// adds client to game, returns result
	if(!successful)
		LogF.writetolog("ACTIVE_GAMES.addclient() failure: ", successful);	// write to log if add client failed
	exit(EXIT_SUCCESS);
}

void serverCleanUp() { // Cleans up allocated memory, threads and Socket overhead
	
	LogF.writetolog("Exiting server");

	// Do other stuff
}

void incrementClientCount() {	// obtains mutex lock and increment/print client counter
	// get mutex lock on connections count
	DWORD err = WaitForSingleObject(connectionsMutex, INFINITE);
	if (err != WAIT_ABANDONED) {
		printf("\nNew Client connected, total: %i", connections);
		connections++;
	}
	// check mutex wait success
	else {
		perror("Mutex wait error");
		exit(0);
	}
}
/*
void createGameProcess(client * curClient)	{		// Creates a new thread for a new game manager by the given client
// Initiate variables
STARTUPINFO startupInfo;
HANDLE child_IN_Rd = NULL;
HANDLE child_IN_Wr = NULL;
HANDLE child_OUT_Rd = NULL;
HANDLE child_OUT_Wr = NULL;
_PROCESS_INFORMATION * processInfo;
ZeroMemory(&processInfo, sizeof(PROCESS_INFORMATION));	// Ensure process data-structure memory is zeroed


// Create a pipe for the child process's STD OUT
if (!CreatePipe(&child_OUT_Rd, &child_OUT_Wr, &secAttr, 0))
LogF.writetolog("StdoutRd CreatePipe");

// Ensure the read handle to the pipe for STDOUT is not inherited
if (!SetHandleInformation(child_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
LogF.writetolog("Stdout SetHandleInformation");

// Create a pipe for the child process's STDIN.
if (!CreatePipe(&child_IN_Rd, &child_IN_Wr, &secAttr, 0))
LogF.writetolog("Stdin CreatePipe");

// Ensure the write handle to the pipe for STDIN is not inherited
if (!SetHandleInformation(child_IN_Wr, HANDLE_FLAG_INHERIT, 0))
LogF.writetolog("Stdin SetHandleInformation");

// Create new game process
CreateProcess(GAME_MANAGER_APPLICATION,
0,			// Command line
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
if (!ACTIVE_GAMES.addGame(processInfo, curClient))
LogF.writetolog("game list full");
else
LogF.writetolog("Process initiation failure: ", ready);		// Otherwise write failure to log file
}
*/
