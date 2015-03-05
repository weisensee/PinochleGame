/*	Testing Client -- Pinochle Game Server
Desktop Application
Lucas Weisensee
November 2014

This program will function as a testing client for the pinochle game server.
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

#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\LogFile.h"		// Log writing class
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\client.h"		// Client communication class
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\query.h"		// Client communication class
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\serverException.h"		// Client communication class
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\gamePlayer.h"		// client game player managing class


// link with required libraries
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512

LogFile LogF;
query userQuery;
client * server;
char TYPE = 'E';
int PLAYERS = 4;
int OBSERVERS = 2;
int GOAL = 1500;
int pNUM;
std::string * GAMENAME = new std::string("def_game_name");

bool setupConnection(int argc, char **argv, char * nPORT);	// Setup connection to server
SOCKET setupListenSocket(char * nPORT, char **argv);		// Setup listen socket with server
void communicateWithServer();				// Run tests/communicate with server
bool launchEchoRoom();						// Echo testing room with server
bool playGames();			// start playing games
void sendCreationInfo();	// creates and sends game creation info string
bool queryForGameInfo(char type); // gets and sends the new game creation info from user
bool liveDataEntry();		// start live entry mode
bool accessDatabase();		// connect to database
bool printGameList(std::string * currentGameList);	// print arg as current list, return true if list exists, false if no list
bool createGame();									// create a new game
bool joinGame(int toPlay);							// join the existing game (gameID = toPlay);

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
			server = new client(listenSocket);

			// testing interface with server
			try {
				communicateWithServer();
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
void communicateWithServer() {				// Run tests/communicate with server
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
			std::string rchar;				// print out failure information
			if (result == 1)
				rchar = received->at(0);
			else
				rchar = "non-valid result";
			printf("\nserver not connected, synced or ready, msg result = %d, message type received: %c, received->at(1): %s", result, server->getAnswerType(), rchar);
			LogF.writetolog("connection setup failed.");
			
			continu = false;		// quit trying to connect
		}
	}
}
bool setupConnection(int argc, char **argv, char * nPORT) {	// Initiates connection 

	// Setup Log File
	LogF.setLogFileName("Client_log_file_", "C:\\Users\\Pookey\\OneDrive\\Projects\\PinochleGame\\logs\\TestingClient");


	//Check for proper command line usage
	if (argc < 3) {
		fprintf(stderr, "\n%s usage: %s hostname portnumber\nUsing default port: %s", argv[0], argv[0], DEFAULT_PORT);
		
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
bool printGameList(std::string * currentGameList) {	// print arg as current list*********return values should be cleaned up
	char * game;				// stores each game's info string
	std::deque<char*> games;	// list of game info strings


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
		// get next game string
		game = games.front();

		// Split game list up into info tokens
		char * info = strtok(game, "^");	// split it into info tokens
		int tknNum = 0;

		while (info != NULL) {				// while there are more info tokens
			switch (tknNum) {				// print out the appropriate info
			case (0) :	// Status
				printf("\nStatus: %s ", info);
				break;
			case (1) :	// Game ID
				printf("game ID: %s ", info);
				break;
			case (2) :	// Game Creator
				printf("game ID: %s ", info);
				break;
			case (3) :	// Player 1
				printf("Player 1: %s ", info);
				break;
			case (4) :	// Player 2
				printf("Player 2: %s ", info);
				break;
			case (5) :	// Player 3
				printf("Player 3: %s ", info);
				break;
			case (6) :	// Player 4
				printf("Player 4: %s ", info);
				break;
			case (7) :	// Player 5
				printf("Player 5: %s ", info);
				break;
			case (8) :	// Player 6
				printf("Player 6: %s ", info);
				break;
			default:
				printf("Switch error: Player Overload!");
				break;
			}

			// finished with current info token, iterate to next info item
			info = strtok(NULL, "^");
			tknNum++;
		}

		// finished with current game, iterate to next game string
		games.pop_front();	// delete current item
	}
	
	// complete
	return true;
}
bool createGame() {					// create a new game
	// send game creation info
	std::string * temp = new std::string;
	if (server->getStrAnswer(temp, N_GQUERY) == 1) {	// If new game query was received
		// ask user if they'd like to create default or custom game
		char ans = userQuery.cQuery("What type of new game would you like to create?\n[D]efault\n[P]inochle (custom)\n[E]uchre (custom)", "DPE");

		// execute user game type creation choice
		switch (toupper(ans)) {
		case 'D':					// default game
			sendCreationInfo();
			break;
		case 'P':					// pinochle game
			queryForGameInfo('P');
			break;
		case 'E':					// euchre game
			queryForGameInfo('E');
			break;
		default:
			printf("switch error, ans = %c", ans);
			return false;
		}

		// play game
		gamePlayer newGame('H', server, TYPE, PLAYERS, GOAL, GAMENAME);
		return newGame.play();

	}
	return true;
}
bool queryForGameInfo(char type) { // gets and sends the new game creation info from user
	// max players
	PLAYERS = userQuery.iQuery("How many players will this game be for?");

	// max observers
	OBSERVERS = userQuery.iQuery("How many observers will this game allow?");

	// winning score
	GOAL = userQuery.iQuery("What will be the winning score?");

	// game name
	GAMENAME = userQuery.sQuery("What will the game name be?");

	sendCreationInfo();
	return true;
}
void sendCreationInfo() {	// creates and sends game creation info string
	std::string gameInfo;
	char temp[24];
	
	// type
	gameInfo.assign(1, TYPE);
	gameInfo.append(1, '^');	// Delimiter 

	// number of players
	gameInfo.append(itoa(PLAYERS, temp, 10));
	gameInfo.append(1, '^');	// Delimiter 

	// number of observers
	gameInfo.append(itoa(OBSERVERS, temp, 10));
	gameInfo.append(1, '^');	// Delimiter 

	// winning score
	gameInfo.append(itoa(GOAL, temp, 10));
	gameInfo.append(1, '^');	// Delimiter 

	// game name
	gameInfo.append(*GAMENAME);

	// send info to server
	server->sendM(N_GINFO, &gameInfo);
}
bool joinGame(int toPlay) {		// join the existing game (gameID = toPlay);
	return true;
}
