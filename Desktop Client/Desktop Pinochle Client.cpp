/*	Testing Client -- Pinochle Game Server
Desktop Application
Lucas Weisensee
November 2014

This program will function as a testing Client for the pinochle game server.
It will allow a developer to run a series of testing functions locally.
*/

#define WIN32_LEAN_AND_MEAN

#pragma once
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <deque>

//#include "..\Library\LogFile.h"		// Log writing class
#include "..\Library\Message.h"		//Message type library
#include "..\Library\Client.h"		// Client communication class
#include "..\Library\Query.h"		// User communication class
#include "..\Library\ServerException.h"		// Handles server exceptions
#include "..\Library\GameSettings.h"
#include "..\Library\GamePlayer.h"

// link with required libraries
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

// ::DEBUGGING::
#define DEBUG false
#ifdef DEBUG
#define DEBUG_IF(cond) if(cond)
#else
#define DEBUG_IF(cond) if(false)
#endif

#define DEFAULT_BUFLEN 512

//LogFile LogF;
Query userQuery;			// User Querying object
Client * server;			// server communication object
char playerType;

// Default Settings
//char * DEFAULT_HOST = "localhost";

GameSettings SETTINGS;
int pNUM = -1;				// player's preferred number, for AI implementation

bool setupConnection(int argc, char **argv, char* nPORT, char* HOST);	// Setup connection to server
SOCKET setupListenSocket(char * nPORT, char **argv);		// Setup listen socket with server
void connectToServerSubsystem();			// Run tests/communicate with server
bool launchEchoRoom();						// Echo testing room with server
bool playGames();			// start playing games
void sendCreationInfo();	// creates and sends game creation info string
bool QueryForGameInfo(char type); // gets and sends the new game creation info from user
bool liveDataEntry();		// start live entry mode
bool accessDatabase();		// connect to database
bool printGameList(std::string * currentGameList);	// print arg as current list, return true if list exists, false if no list
bool createGame();									// create a new game
bool joinGame(int toPlay);							// join the existing game (GAMEID = toPlay);
bool startGame();				// creates a GamePlayer and proceeds with play
void argcheck(int argc, char **argv);

int main(int argc, char **argv)	
{
	argcheck(argc, argv);

	// Setup connection
	char nPORT[7];	// port to connect to
	char * HOST = new char[DEFAULT_BUFLEN];	// host to connect to
	SOCKET serverSocket = INVALID_SOCKET;
	bool success = setupConnection(argc, argv, nPORT, HOST);

	// if connection setup was successful, continue with socket setup
	if (success) {
		//initialize socket 
		SOCKET listenSocket = setupListenSocket(nPORT, argv);
		if (listenSocket == INVALID_SOCKET) {
			fprintf(stderr, "error: INVALID_SOCKET, quitting.");
			//LogF.writetolog("error: INVALID_SOCKET, quitting.");
			exit(0);
		}

		// if the socket setup was successful, continue with testing interface
		else {
			// Create server object
			server = new Client(listenSocket);

			// testing interface with server
			try {
				connectToServerSubsystem();
			} catch (ServerException& e) {
				printf(e.sendEr());
			}
		}
	}

	// if connection setup failed, exit
	else {
		printf("connection setup failed, exiting");
		//LogF.writetolog("connection setup failed.");
		exit(0);
	}
	return 0;	// exit
}
void argcheck(int argc, char **argv) {
	DEBUG_IF(true) {
		printf("\nClient is starting up, press enter to continue... ");
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
}
bool setupConnection(int argc, char **argv, char* nPORT, char* HOST) {	// Initiates connection 
	// Check for proper command line usage
	if (argc < 3) {
		fprintf(stderr, "\n%s nusage: %s hostname portnumber\nUsing default host: %s port: %d", argv[0], argv[0], DEFAULT_HOST, DEFAULT_PORT);
		
		// setup port with defaults
		char * temp = new char[7];		// copy from DEFAULT_PORT
		itoa(DEFAULT_PORT, temp, 10);
		strncpy(nPORT, temp, 6);
		nPORT[6] = '\0';				// ensure that nPORT ends with \0, no matter the length of DEFAULT_PORT
		nPORT[strlen(nPORT)] = '\0';

		// 
	}
	else {
		if (strcmp("-A", argv[1]) == 0) {
			printf("\nMODE: AI Player.");
			printf("\n%s Usage>>>> ");


		}
		else if (argc == 3) {
			// Set Port Number
			printf("\nMODE: Human Player.");
			strncpy(nPORT, argv[2], strlen(argv[2]));
			nPORT[strlen(argv[2])] = '\0';

		}
	}


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
		printf("WSAStartup failed! Returned:%d\n", result);
		return INVALID_SOCKET;
	}

	// Setup socket and connection objects
	struct addrinfo *socketInfo = NULL, *tempInfo = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Get's address info from system for port and socket connection
	// check for failure
	// result = getaddrinfo(argv[1], nPORT, &hints, &socketInfo);
	result = getaddrinfo("localhost", nPORT, &hints, &socketInfo);
	if (result != 0){
		// if it failed, write log and print error Messages
		fprintf(stderr, "getaddrinfo failed with error: %s\n", gai_strerror(result));
		printf("Calling getaddrinfo with following parameters:\n");
		printf("\tnodename = %s\n", argv[1]);
		printf("\tservname (or port) = %s\n\n", nPORT);
		//LogF.writetolog("getaddrinfo failed with error: %d\n", gai_strerror(result));
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
			//LogF.writetolog("socket() creation failed with error: %ld\n", WSAGetLastError());
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
void connectToServerSubsystem() {				// Run tests/communicate with server
	// Check that server is synced/ready for connection
	int status = 0;
	bool continu = true;

	while (continu) {
		// get server status Message
		int result = server->getIntAnswer(&status, S_STATUS);

		// if server Message is as expected: check that server is setup, synced and ready, continue
		if (result > 0	&& status == 1) {	// check that Message received from server was a 'ready' signal (='1') 
			// get user direction
			int ans = -1;
			while (ans < 0 || ans > 4)
				ans = userQuery.iQuery("\nServer Ready\nWhat would you like to do?\n[0] - Quit\n[1] - Request list of active games\n[2] - Live Game Data Entry Mode\n[3] - Access Database\n[4] - Echo Testing\n\n");
			char temp[32];
			itoa(ans, temp, 30);	// convert int to string
			result = server->sendM(CON_TYPE, temp);	// and send to server
			server->requestHandled(S_STATUS);		// set request to handled

			switch (ans) {					// execute user command
			case 0:
				continu = false;
				break;						// quit connection
			case 1:
				continu = playGames();			// start playing games
				break;
			case 2:
				continu = liveDataEntry();		// start live entry mode
				break;
			case 3:
				continu = accessDatabase();		// connect to database
				break;
			case 4:
				continu = launchEchoRoom();		// start echo testing with server
				break;
			default:
				printf("switch error in comminicateWithServer");	// error state
				break;
			}
		}
		// if server is not ready
		else {
			printf("\nserver not connected, synced or ready, Message result = %d, Message received [server->getAnswerType()]: %u, status: %i", result, server->getAnswerType(), status);
			//LogF.writetolog("connection setup failed.");
			//
			continu = false;		// quit trying to connect
		}
	}
}
bool playGames() {			// start playing games
	// Get current game list
	std::string * currentGameList = new std::string;	// list of active games to connect to
	int result = server->getStrAnswer(currentGameList, G_LIST);		// retrieve current list

	// check that current list exists
	if (currentGameList->length() < 1) 
		printf("\nNo active games, please create a new game or check back later...");	// notify user if no current active games
	else
		printGameList(currentGameList);									// print current game list

	// Pick game to join or play new game
	int toPlay = userQuery.iQuery("\nWhich game would you like to play? Enter '0' to create a new game");

	// Send game choice to server and check for errors
	char temp[10];
	if (server->sendM(G_CHOICE, itoa(toPlay, temp, 10)) < 1)
		return false;
	
	// Execute answer
	if (toPlay == 0)
		return createGame();
	else
		return joinGame(toPlay);
}
bool liveDataEntry() {		// start live entry mode
	printf("beginning live data entry mode");
	return true;
}
bool accessDatabase() {		// connect to database
	printf("accessing database...");
	return true;
}
bool launchEchoRoom() {					// Echo testing room with server
	// make "quit" compare string:
	std::string quitString = "quit";
	int result;
	quitString.insert(quitString.begin(), (char)Message);	// insert 'Message' identifier code to synthesize quitString
	std::string * toSend;								// Message to send to server
	std::string * received = new std::string();			// Message received

	// echo with server until user types quit to stop
	while (true) {
		result = server->getStrAnswer(received, Message);	// get Message from server
		printf(received->c_str());		// print Message received

		// If Message was sent successfully
		if (result >= 1) {
			// get desired Message from user
			toSend = userQuery.sQuery("\nWhat would you like to send to the server? enter 'quit' to quit\n");

			// send Message to server
			server->sendM(Message, toSend);

			// quit if the user requested to
			if (strcmp(toSend->c_str(), quitString.c_str()) == 0)
				break;
		}
		// If Message was not sent successfully
		else {
			printf("Message receive error: %i", result);	// print error
			return false;									// quit connection with server
		}
	}

	// notify user and exit echo session to menu
	printf("exiting echo session");
	return true;
}
bool printGameList(std::string * currentGameList) {	// print arg as current list*********return values should be cleaned up
	char * game;				// stores each game's info string
	std::deque<char*> games;	// list of game info strings
	GameSettings toPrint;


	// Split game list up into strings
	char * gameList = new char[currentGameList->length() + 1];
	game =	strtok(gameList, "/");

		// build game info string list
	while (game != NULL) {
		// add current game to back of queue
		games.push_back(game);

		// iterate to next game tokent
		game = strtok(NULL, "/");
	}

	// Print each game
	while (!games.empty()) {				// while there is another game token on the queue
		// print next game string
		toPrint.setFromInfoString(games.front());

		printf("\n#1: %s", toPrint.print().c_str());


		// finished with current game, iterate to next game string
		games.pop_front();	// delete current item
	}
	
	// complete
	return true;
}
bool createGame() {					// create a new game
	// send game creation info
	std::string * temp = new std::string;
	if (server->getStrAnswer(temp, N_GQuery) >= 1) {	// If new game Query was received
		// ask user if they'd like to create default or custom game
		char ans = userQuery.cQuery("What type of new game would you like to create?\n[D]efault\n[P]inochle (custom)\n[E]uchre (custom)", "DPE");

		// execute user game type creation choice
		switch (toupper(ans)) {
		case 'D':					// default game
			sendCreationInfo();
			break;
		case 'P':					// pinochle game
			QueryForGameInfo('P');
			sendCreationInfo();
			break;
		case 'E':					// euchre game
			QueryForGameInfo('E');
			sendCreationInfo();
			break;
		default:
			printf("switch error, ans = %c", ans);
			return false;
		}

		// play game
		return startGame();
	}
	return true;
}
bool QueryForGameInfo(char type) { // gets and sends the new game creation info from user
	// Set game type
	SETTINGS.gType = type;
	
	// max players
	SETTINGS.MAXPLAYERS = userQuery.iQuery("How many players will this game be for?");

	// max observers
	SETTINGS.MAXOBSERVERS = userQuery.iQuery("How many observers will this game allow?");

	// winning score
	SETTINGS.GOAL = userQuery.iQuery("What will be the winning score?");

	// game name
	SETTINGS.gameName = *(userQuery.sQuery("What will the game name be?"));

	return true;
}
void sendCreationInfo() {	// creates and sends game creation info string
	// Get info from settings object and send to server
	std::string * toSend = SETTINGS.settingsString();
	DEBUG_IF(true)
		printf("\npreparing to send: %s", toSend->c_str());
	server->sendM(N_GINFO, toSend->c_str());
	server->requestHandled(N_GINFO);
}
bool joinGame(int toPlay) {		// join the existing game (GAMEID = toPlay);
	printf("\nJoining game %i",toPlay);

	// start play of specified game
	return startGame();
}
bool startGame() {
	// Launch game with current settings
	GamePlayer newGame(&SETTINGS, playerType, server);
	return newGame.run();

}
