/*	Desktop Server -- Pinochle Game Server
Desktop Application
Lucas Weisensee
January 2015

to START THE PROGRAM in command prompt::
executableName PortNumber

This program will function as a desktop game server for a network based pinochle game. It will:
-Listen for new connections.
-Manage incoming Client connections.
-Spin off processes to handle new games.
-Allow new Clients to connect to games in progress.
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
#pragma once
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <process.h>
#include <string.h>
#include <sys/types.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")	//Winsock server needs to be linked with libraries

//**************************************************
// ::PERSONAL LIBRARIES::
//**************************************************
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\LogFile.h"		//Log writing library
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Desktop Server\CardGame.h"
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Desktop Server\GameList.h"
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\Client.h"


#define DEBUG true

#ifdef DEBUG
#define DEBUG_IF(cond) if(cond)
#else
#define DEBUG_IF(cond) if(false)
#endif

//**************************************************
// ::DEFAULTS::
//**************************************************
#define DEFAULT_BUFLEN 512
#define DEBUG_MODE 0		// debugging mode 1 = on, 0 = off
#define MAX_THREADS 100		// default maximum current threads
#define MAX_GAMES 20		// default maximum active games

//**************************************************
// ::GLOBAL RESOURCES::
//**************************************************
// Server Log File and Mutex Lock
//LogFile LogF;
CRITICAL_SECTION logLock;						// log file access lock

// Active Game List and Mutex Lock
GameList ACTIVE_GAMES(MAX_GAMES);		// List of general info on all active games
CRITICAL_SECTION GameListLock;			// Active game list critical section lock: for accessing/changing the active games list

// Current Connections and Mutex Lock
int connections;						// number of connections accepted
CRITICAL_SECTION connectionsLock;

//**************************************************
// ::SERVER FUNCTIONS::
//**************************************************

// Setup and Client Handling:
void setupServer(int argc, char const *argv[], char * nPORT);		//Sets up the new server, checks inputs
SOCKET setupListenSocket(char * nPORT);				// Setup Listen socket for accepting new connections
void handleNewClient(void* newSocket);// Handles a new Client when connection is formed
int singleThreadedHandler(SOCKET ListenSocket);	// debugging mode: alternative Client handler, creates no additional threads
void incrementClientCount();					// obtains mutex lock and increment/print Client counter

// Executing Client Requests:
bool playGames(Client * curClient);				// sends active game list to Client, returns true if Client may wish to continue server connection, false otherwise
bool liveGameDataEntry(Client * curClient);		// allows Client to enter live game data entry mode, returns true if Client may wish to continue server connection, false otherwise
bool consoleEchoTest(Client * curClient);		// launches console echo testing, returns true if Client may wish to continue server connection, false otherwise
bool connectToDatabase(Client * curClient);		// connects Client to the database, returns true if Client may wish to continue server connection, false otherwise
bool createGame(Client * curClient);			// initiates the creation of a new game
bool addNewGame(CardGame * newGame);			// adds new game to active games list, using appropriate mutex locks
bool joinGame(int choice, Client * curClient);	// sends the Client to the game specified

// Handling Errors and Closing Down Server:
void serverCleanUp();							// Cleans up allocated memory, threads and Socket overhead
void receiveError(SOCKET ClientSocket);			// handles a receive error, prints to log


int main(int argc, char const *argv[]) {
	DEBUG_IF(true) {
		printf("\nServer is starting up, press enter to continue... ");
		char temp[4];
		std::cin.getline(temp, 3);

		printf("\nArguments passed in: \n");
		for (int i = 0; i < argc; ++i) {
			if (i == 3)
				printf("\n%s", argv[i]);
			else
				std::cout << argv[i] << std::endl;
			std::cin.getline(temp, 3);
		}
	}

	
	char nPORT[7];			// The port number to be used for listening
	setupServer(argc, argv, nPORT);	// Check inputs and initiate sever settings 


	DEBUG_IF(true)
		printf("\nIn Debugging Mode");


	// Create a SOCKET for the server to listen to
	SOCKET ListenSocket = setupListenSocket(nPORT);
	printf("\nServer initialized and listen socket setup, preparing to listen on port %s", nPORT);

	// Temp Socket for new Clients
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
			//initialize Client connection socket
			ClientSocket = new SOCKET;
			*ClientSocket = INVALID_SOCKET;

			//Accept the Client's socket
			*ClientSocket = accept(ListenSocket, NULL, NULL);
			if (*ClientSocket == INVALID_SOCKET) {
				//printf("accept failed: %d\n", WSAGetLastError());
				closesocket(ListenSocket);
				WSACleanup();
				return 1;
			}

			//spin off thread to handle current user return to listening
			//ThreadArray[*i%10] = 
			_beginthread(
				handleNewClient,		// thread function name
				0,          				// use default stack size  
				(void*)*ClientSocket		// argument to thread function 
				);		// returns the thread identifier 

			connections++;
		}

	// Thread and memory cleanup
	serverCleanUp();
	return 0;
}
void setupServer(int argc, char const *argv[], char * nPORT) {		//Sets up the new server, prepares to listen for connections
	// Use defaults if proper command line usage was not present
	if (argc < 2) {
		fprintf(stderr, "\n%s usage: %s port-number\n", argv[0], argv[0]);
		printf("\nUsing defaults: port: %s", DEFAULT_PORT);
	}
	// If proper command line usage:
	else {
		// Set port number
		strncpy(nPORT, argv[1], strlen(argv[1]));
		nPORT[strlen(argv[1])] = '\0';
		printf("\nPort initiated: %s", nPORT);
	}

	// Initiate server type log file in location specified
	//LogF.setLogFileName("server log file_", "C:\\Users\\Pookey\\OneDrive\\Projects\\PinochleGame\\logs\\server");

	// INITIATE CRITICAL SECTIONS:
	if (!InitializeCriticalSectionAndSpinCount(&connectionsLock, 0x00000400) ||	/* Check for any errors that  might occur*/
		!InitializeCriticalSectionAndSpinCount(&GameListLock, 0x00000400) ||
		!InitializeCriticalSectionAndSpinCount(&logLock, 0x00000400)) {
			perror("\nMutex Initiation error: ");		// report errors if any
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
		printf("WSAStartup failed! Returned:%d\n", result);
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
		printf("getaddrinfo failed with error: %d\n", result);
		WSACleanup();	//deallocate memory and quit
		return  INVALID_SOCKET;;
	}


	ListenSocket = socket(socketInfo->ai_family, socketInfo->ai_socktype, socketInfo->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(socketInfo);
		WSACleanup();
		return  INVALID_SOCKET;;
	}

	//Setup the TCP listening socket
	result = bind(ListenSocket, socketInfo->ai_addr, (int)socketInfo->ai_addrlen);
	if (result == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(socketInfo);
		closesocket(ListenSocket);
		WSACleanup();
		return  INVALID_SOCKET;;
	}

	//deallocate socket address memory
	freeaddrinfo(socketInfo);

	//listen for Client connection on socket created
	//SOMAXCONN allows for max reasonable connections as determined by the system
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR){				//if listen fails
		printf("Listen() failed with error: %ld\n", WSAGetLastError());	//print error
		closesocket(ListenSocket);										//and close program
		WSACleanup();
		return  INVALID_SOCKET;;
	}

	return ListenSocket;
}
void handleNewClient(void* newSocket) {	// Handles an individual Client::
	// notify user/terminal
	incrementClientCount();

	//initiate variables
	Client curClient((SOCKET)newSocket);	// Create new Client object
	//curClient.setName();
	bool continu = true;		// does the Client want to quit
	int answer, result = 0;			// result of Client's interactions

	//Direct the Client to the correct path and close the connection when finished
		/* execute Clients request, see "packet specifications.txt" for more info
				1: Request the list of active games
				2: Game Data entry mode
				3: Initiates database access mode
				4: console echo testing */
	while (continu) {
		// get Client connection preference
		std::string temp = "1";
		curClient.sendM(S_STATUS, &temp);
		//**************************NEEDS NULL POINTER CHECK*********************************
		result = curClient.getIntAnswer(&answer, CON_TYPE);	// save Client's answer

		// if the answer was received successfully
		if (result == 1) {
			// initiate the connection the Client wants
			switch (answer) {
			case 0:						// if the Client wants quit						
				continu = false;		// quit
				break;
			case 1:						// if the Client wants to play games							
				continu = playGames(&curClient);
				break;
			case 2:						// allows Client to enter live game data entry mode
				continu = liveGameDataEntry(&curClient);
				break;
			case 3:						// If the Client wants to access the game database
				continu = connectToDatabase(&curClient);
				break;
			case 4:
				continu = consoleEchoTest(&curClient);	// launches console echo testing
				break;
			default:		// if Client didn't enter a valid command
				continu = 1;	// allow them to try again
				break;
			}
		}
		// if Client connection is closed
		else if (result == -1)
			break;		// quit
		// if message receiving error occurred
		else {
			printf("messaging error occurred, result: %d", result);
			continu = false;
		}
	}
	// End current connection and operations
	//_endthread();;		//close thread
}
int singleThreadedHandler(SOCKET ListenSocket) {		// debugging mode: alternative Client handler, creates no additional threads
	//******  ::DEBUGGING SERVER LOOP::  ***********
	// Wait for incoming connections, accept and spin off new thread to handle them, only once! 
	// to maintain single threaded server for testing purposes

	printf("\nEntering debugging mode: single threaded server");

	// Temp Socket for new Client
	SOCKET * ClientSocket;

	//initialize Client connection socket
	ClientSocket = new SOCKET;
	*ClientSocket = INVALID_SOCKET;

	//Accept the Client's socket
	*ClientSocket = accept(ListenSocket, NULL, NULL);
	if (*ClientSocket == INVALID_SOCKET) {
		printf("accept failed: %d\n", WSAGetLastError());
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
bool playGames(Client * curClient)		// Queries and sends Client to desired game-path
{
	// Send list of current games to Client
	std::string currentGameList = ACTIVE_GAMES.getCurrent();	//gets list of active games for Client to connect to
	curClient->sendM(G_LIST, &currentGameList);		// Send to current list Client

	// Parse user game choice: start new game or join an existing one
	int choice;
	int result = curClient->getIntAnswer(&choice, G_CHOICE);	// Get game choice from Client

	// if receive was successful
	if (result == 1) {
		if (choice == 0)						// if Client chose to setup a new game
			return createGame(curClient);				// set it up
		else if (choice > 0)					// If Client chose to connect to an existing game
			return joinGame(choice, curClient);		// connect to it
	}
	// if receive failed
	else {
		printf("Receive Error: playGames: getIntAnswer: ", result);		// write failure to log and exit
		// if connection was closed
		if (result == -1)
			return 0;		// quit connection
	}

	// return to main menu
	return true;

}
void receiveError(Client * curClient)		//handles a receive error, prints to log, returns true if Client may wish to continue server connection, false otherwise
{
		printf("receive failed: ", WSAGetLastError());
		closesocket(curClient->getSocket());
		WSACleanup();
}
bool connectToDatabase(Client * curClient)	// connects Client to the database, returns true if Client may wish to continue server connection, false otherwise
{
	std::string buffer;
	buffer += "Hi, game database coming soon...";
	int result = curClient->sendM(0, buffer.c_str());			// send descriptive error message to Client

	if (result <= 0)
		return false;
	
	// return to main menu
	return true;
}
bool createGame(Client * curClient) {			//initiates the creation of a new game
	// request new game creation info
	GameSettings SETTINGS(curClient);
	std::string gameInfo;
	int result = curClient->getStrQueryAnswer(&gameInfo, N_GQUERY, N_GINFO);	// send request for and receive new game creation info
	
	// if new game info message was received in response
	if (result == 1) {
		// Parse new game info
		SETTINGS.setFromInfoString(gameInfo.c_str());

		// create, launch and manage new game
		CardGame newGame(SETTINGS);
		addNewGame(&newGame);
		newGame.run();
		return true;
	}


	// ::ERROR HANDLING::
	// if Client closed connection
	else if (result == -1) {
		printf("Client connection unexpectedly closed, result: %d", result);
		return false;
	}
	// if new game info was not received
	else {
		printf("new game info not received");
		return true;
	}
}
bool addNewGame(CardGame * newGame) {			// adds new game to active games list, using appropriate mutex locks
	// Request ownership of the critical section.
	EnterCriticalSection(&GameListLock);

	bool result = ACTIVE_GAMES.add(newGame);

	// Release ownership of the critical section.
	LeaveCriticalSection(&GameListLock);

	// return success
	return result;
}
bool consoleEchoTest(Client * curClient) {		// launches console echo testing, returns true if Client may wish to continue server connection, false otherwise
	// send welcome message to Client
	int result = curClient->sendM(MESSG, "Welcome to the console echo test! what would you like me to echo?");

	// echo messages with Client until they want to quit
	std::string answer;
	bool quit = false;
	while (!quit)	
	{
		result = curClient->getStrAnswer(&answer, MESSG);

		if (strcmp(answer.c_str(), "quit") == 0) {
			printf("\nClient message matches 'quit', Client message: %s", answer.c_str());
			quit = true;
		}

		// if the Client shuts down the connection or sends message "quit"
		else if (result <= 0)
			return false;	// quit on the server end as well

		// if the message was received successfully
		else if (result >= 1) {
			answer.insert(0, "received: ");	// modify
			curClient->sendM(MESSG, &answer);
		}

	}

	return true;
}
bool liveGameDataEntry(Client * curClient) {				// allows Client to enter live game data entry mode, returns true if Client may wish to continue server connection, false otherwise
	printf("executing \"liveGameDataEntry\" function");
	return true;
}
bool joinGame(int choice, Client * curClient)				// sends the Client to the game specified
{
	int successful = ACTIVE_GAMES.addClient(choice, curClient);		// adds Client to game, returns result
	if(!successful)
		printf("ACTIVE_GAMES.addClient() failure: ", successful);	// write to log if add Client failed
	
	//debug
	printf("\nClient: %s added to game: %d", curClient->getName().c_str(), choice);

	ExitThread(0);
}
void serverCleanUp() { // Cleans up allocated memory, threads and Socket overhead
	// release critical sections
	DeleteCriticalSection(&connectionsLock);
	DeleteCriticalSection(&GameListLock);
	DeleteCriticalSection(&logLock);

	WSACleanup();			//deallocates data after execution's complete

	printf("Exiting server");

	// Do other stuff
}
void incrementClientCount() {	// obtains mutex lock and increment/print Client counter
	// Request ownership of the critical section.
	EnterCriticalSection(&connectionsLock);

	printf("\nNew Client connected, total: %i", connections);
	connections++;

	// Release ownership of the critical section.
	LeaveCriticalSection(&connectionsLock);
}
/*
void createGameProcess(Client * curClient)	{		// Creates a new thread for a new game manager by the given Client
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
printf("StdoutRd CreatePipe");

// Ensure the read handle to the pipe for STDOUT is not inherited
if (!SetHandleInformation(child_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
printf("Stdout SetHandleInformation");

// Create a pipe for the child process's STDIN.
if (!CreatePipe(&child_IN_Rd, &child_IN_Wr, &secAttr, 0))
printf("Stdin CreatePipe");

// Ensure the write handle to the pipe for STDIN is not inherited
if (!SetHandleInformation(child_IN_Wr, HANDLE_FLAG_INHERIT, 0))
printf("Stdin SetHandleInformation");

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
printf("game list full");
else
printf("Process initiation failure: ", ready);		// Otherwise write failure to log file
}
*/
