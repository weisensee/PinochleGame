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
-Allow games to be added to database
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

//#include <iphlpapi.h>

//Winsock server needs to be linked with:
#pragma comment (lib, "Ws2_32.lib")

/*Error report management function
void error(char *er_msg)
{
	perror(er_msg);
	exit(1);
}
*/


#define DEFAULT_BUFLEN 512
#define MAX_THREADS 20
#define MAX_GAMES 10

gameList * ACTIVE_GAMES(MAX_GAMES);		//array of general info on all active games
char * LOG_FILE_NAME;
int connections;


DWORD WINAPI handleNewClient(LPVOID* newSocket);
void receiveError(SOCKET ClientSocket);		//handles a receive error, prints to log
void accessDatabase(playerName, ClientSocket);	//handles client access to database
void createGame(SOCKET ClientSocket, char * gamename, char * playerName);	//initiates the creation of a new game
void joinGame(SOCKET ClientSocket, int n);						//sends the client to the game specified
void writetolog(char * report);					//writes report to log file
void writetolog(char * report, int error);		//writes errror report to log file
void setLogFileName();		//sets log file name with time, date and PID info

int main(int argc, char const *argv[])
{
	/*
	char testing[4];
	printf("testing location 2, argv[1]: %s\n", argv[1]);
	std::cin.getline(testing, '\n');
	*/



		//Check for proper command line usage
	if (argc < 2) {
		fprintf(stderr, "\n%s usage: %s portnumber\n", argv[0], argv[0]);
		exit(0);
	}

	setLogFileName();

	std::cout << "\n\nSetting up connection";

	char nPORT[6];
	strcpy(nPORT, argv[1]);
	nPORT[strlen(argv[1])] = '\0';

	// DWORD   ThreadIdArray[MAX_THREADS];		//Threads the server has created
	//HANDLE  ThreadArray[MAX_THREADS]; 		//stores thread handles

	//initialize windows socket
	WSADATA wsaData;

	int result;

	// Initiate Windows Socket
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		printf("WSAStartup failed! Returned:%d\n", result);
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
	if (result != 0)
	{
		printf("getaddrinfo failed with error: %d\n", result);
		WSACleanup();	//deallocate memory and quit
		return 1;
	}


	// Create a SOCKET for the server to listen to
	SOCKET ListenSocket = INVALID_SOCKET;
	ListenSocket = socket(socketInfo->ai_family, socketInfo->ai_socktype, socketInfo->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(socketInfo);
		WSACleanup();
		return 1;
	}

	//Setup the TCP listening socket
	result = bind(ListenSocket, socketInfo->ai_addr, (int)socketInfo->ai_addrlen);
	if (result == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
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
		printf("Listen() failed with error: %ld\n", WSAGetLastError());	//print error
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
			printf("accept failed: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}

		//spin off thread to handle current user return to listening
		//ThreadArray[*i%10] = 
		CreateThread( 
            NULL,             			// uses default security attributes
            0,          				// uses default stack size  
            (LPTHREAD_START_ROUTINE)handleNewClient,		// thread function name
            (void*)*ClientSocket,		// argument to thread function 
            0,                      	// use default creation flags 
            0);		// returns the thread identifier 

		connections++;
	}
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
	char playerName[DEFAULT_BUFLEN];
	int * answer = NULL;
	int result;
	SOCKET ClientSocket = (SOCKET)newSocket;

	//Direct the client to the correct path and close the connection when finished
			//get message from client
	result = recv(ClientSocket, playerName, DEFAULT_BUFLEN, 0);

	if(result >= 0)	{				//if the message was received successfully							
		if (playerName[0] == 1)							//play the game if client requests it
			sendGameList(ClientSocket, answer);			//send game list to player
		interpretClientGameChoice(playerName[0], ClientSocket, answer)
	}

	else 							//close if errors on packet receipt
		receiveError(ClientSocket);

	WSACleanup();					//deallocates data

	return 0;						//close
}

void sendGameList(SOCKET ClientSocket, int * answer)
{
	int result;					//send/receive result
	char * currentGameList;		//string to pass to client
	gameList.getCurrent(currentGameList)	//gets list of active games for client to connect to
	result = send(ClientSocket, currentGameList, strlen(currentGameList), 0);		//send list
	if(result > 0)								//if the sending was successful
	{												//get answer
		answer = new char[DEFAULT_BUFLEN];
		result = recv(ClientSocket, answer, DEFAULT_BUFLEN, 0);	//get response back
		if(result < 1)
			writetolog("error, receive game choice result < 1 ", WSAGetLastError());	//write to log if receive failed
	}
	else						//write error to log if sending failed
		writetolog("error, sendGameList result < 1 ", WSAGetLastError());
	return 0;
}

void interpretClientGameChoice(int mainChoice, ClientSocket, char gameChoice)	//interprets client's commands/choice
{
	if(mainChoice == 1)												//if client chose to play pinochle
		if(gameChoice == 1)										//if the player requested a new game
			createGame(ClientSocket, answer[2], (int)answer[1]);	//create the new game
		else if (gameChoice > 1)								//otherwise join the game requested
			joinGame(ClientSocket, answer[0]);
		else if (gameChoice < 0)								//write to log if errors result
			writetolog("join game response failure: ", WSAGetLastError())	
		}
	if(mainChoice[0] == 0)	//initiate database connection if the client requests it
		accessDatabase(playerName, ClientSocket);
}

void receiveError(SOCKET ClientSocket)		//handles a receive error, prints to log
{
		writetolog("receive failed", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
}

void accessDatabase(playerName, ClientSocket)	// handles client access to database
{
	char buffer[DEFAULT_BUFLEN];
	strcpy(buffer, "Hi, " + playerName + "! \ngame database coming soon...");
	send(ClientSocket, buffer, strlen(buffer), 0);
}

// Creates a new game with given arguments and adds it to list.
void createGame(SOCKET ClientSocket, char * gamename, char * playerName, HANDLE threadHandle)	
{
	// Initiate and Start game
	if(game(ClientSocket, gamename, playerName)) {		//If initiation is successful:
		ACTIVE_GAMES.add(game);							// add to global active game list
		game.run();										// start game
	}
	else
		writetolog("game initiation failure");			// Otherwise write failure to log file
}

// Adds specified client to game
int joinGame(int gameNumber, SOCKET ClientSocket, int n)				// sends the client to the game specified
{
	return ACTIVE_GAMES[i].addPlayer(gameNumber, ClientSocket, n);		// adds player to game, returns result
}

void writetolog(string * treport)					//writes report to log file
{
	//generate report
	string * report = addTimeString(treport)

	//open log file
	FILE * logFile = fopen(LOG_FILE_NAME)

	//write string to file
	fprintf(logFile, "%s\n", report);

	//close log file
	logFile.fclose();
	delete treport;		//deallocate report string
}

// Appends system time to string for log/error reporting
// report_string dayofweek SystemTime: MM/DD/YYYY HH:MM:SS
string addTimeString(string * treport)
{
	SYSTEMTIME time;
	GetSystemTime(time);
	treport += " " + time.wDayOfWeek + " SysTime: " + time.wMonth + "/" + time.wDay + "/" + time.wYear + " " + time.wHour + ":" time.wMinute + ":" + time.wSecond;
	return treport;
}

// Adds error to report and sends to be written to logfile
void writetolog(char * report, int error)		//writes errror report to log file
{
	char catReport[strlen(report) + 10];		//create new concatenaten string
	strcpy(catReport, report);
	strcat(catReport, error);
	writetolog(catReport);						//send concatenated report
}

// Sets LOG_FILE_NAME with date and PID info
// MM/DD/YYYY HH:MM:SS:mSmS pid [int processID]
void setLogFileName()
{
	SYSTEMTIME time;
	GetSystemTime(time);
	string * name = time.wMonth + "/" + time.wDay + "/" + time.wYear + " " + time.wHour + ":" time.wMinute + ":" + time.wSecond + ":" +time.wMilliseconds " pid " + GetCurrentProcessId();
	LOG_FILE_NAME = name;
	name.insert(0, "Log File Created: ");
	writetolog(name)
}