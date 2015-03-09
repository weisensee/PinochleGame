/*	Live Game Manager -- Pinochle Game
		Desktop Application
		Lucas Weisensee
		November 2014

	This program will function as a pinochle game manager. It will:
		-manage connections to all current players and observers
		-keep track of the current game
		-keep track of the current round
		-enforce turn rules/turn order
		-quit and save game when finished
*/
#define WIN32_LEAN_AND_MEAN

#pragma once
#include <stdlib.h>
#include <time.h>
#include <string>
#include <queue>
#include <winsock2.h>
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\euchreRound.h"
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\pinochleRound.h"
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\client.h"
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\message.h"
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\LogFile.h"		//Log writing library
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\gameSettings.h"

class cardGame: public gameSettings {
public:
	// ::CONSTRUCTORS AND INITIALIZERS::
	cardGame();
	cardGame(gameSettings toMake);			// constructs an active game loading settings from the game settings file passed in
	~cardGame();
	void run();			// starts the current game

	// ::SERVER COMMUNICATOR FUNCTIONS::
	int playerCount();			// returns current total players
	bool connectClient(client * toAdd);	// adds new client to list of clients to add to game
	int getGameID();					// Returns game ID
	std::string getInfoString();		// returns game information as string
	void buildInfoString();				// rebuilds and sets the game info string, function must take place within critical sections to avoid race conditions

private:
	// ::GAME SETUP::
	int getNewId();				// returns a new ID from the server for the current game
	void setupGame(char * ntype, int * playerNum, int * obsNum, int * gGoal, std::string * gameName);	// parses player's game type request and initializes play environment
	bool checkForConnections();					// checks for new incoming connections
	bool addPlayer(client * newPlayer, int n);	// adds specified new player to position n
	void echo(client * curPlayer);				// echoes and talks to current player, for network testing purposes
	int ready();				// Returns true if the game is ready
	bool makePlayer(client * player);			// adds player to current players, returns true if successful, false otherwise
	void decrementPlayerCount();				// decreases the player count by one
	void incrementPlayerCount();				// increases the player count by one
	void incrementObserverCount();				// increases the player count by one
	void decrementObserverCount(); 				// decreases the player count by one


	// ::PLAER COMMUNICATION::
	void sendToAll(unsigned char code);				// send message of type code to all players and observers
	void sendToAll(unsigned char code, char toSend);// send message of type code, with body of type toSend to all players and observers
	bool checkForRequests(client * player);		// checks for and executes any commands from player
	bool checkCurrentConnections();					// checks for requests from clients
	bool handleRequest(client * player, unsigned char request);	// handles request from client and notifies client of request status, returns true if success

	// ::GAME CONTROL FLOW::
	void play(char gType);	// Launch the game type specified
	void diconnectPlayers();	//disconnect all network connections
	int removeClient(client * player); 	// removes client from game, decrementing appropriate counters, returns previous position, 0 if not in game, 1-maxplayers if player, maxplayers-maxobservers if observer

	// ::GAMEPLAY::
	void managePinochleGamePlay();		//plays a pinochle round
	void manageEuchreGamePlay();		// plays a euchre round
	void euchreDealCards();				// deal out euchre hand to each player
	void euchreBiddingPhase();			// manages Euchre bidding phase
	void orderUp(card top, int n);				// sets trump, makers and notifies all players that player n ordered up card top
	void euchreTricksPhase();			// manages Euchre trick playing phase
	void pinochleBiddingPhase();		//manages entire bidding phase
	void pinochlemeldingPhase();		//manages entire melding phase
	void pinochleTricksPhase();			//manages entire trick taking phase
	void publishReport();		//totals scores and publishes to player
	void saveGame();
	void quit(int type);		// shutdown/save the game and close connections, if type == 0, don't save
	bool restart();

	// ::DATA::

	// players and connections
	client ** PLAYERS;
	client ** OBSERVERS;

	// Incrementers and Game variable states
	int * SCORES;						// current teams' scores
	int dealer;							// dealer # for current round
	char STATUS;						// Game's current status, 'W': waiting 'R': ready to play 'P':playing 'B':bored
	int totalPlayers, totalObservers;	// current count of each
	// Static Game Data
	int GAMEID;	// Game's unique ID #
	std::string infoString;	// most recent info string
	bool infoStringDirty;		// true if info string is dirty and needs to be updated
	bool gameOver;
	//LogFile * gLog;			// Game's log file

	// Stores active round data for current round
	euchreRound * eRound;		// current game round
	pinochleRound * pRound;		// current game round

	// Threads and Server Connection
	std::queue<client> newPlayers;		// new players to add to game
	CRITICAL_SECTION playerCountLock;	// total players access lock
	CRITICAL_SECTION newPlayerListLock;		// new players access lock
	CRITICAL_SECTION gameIdLock;		// GAMEID access lock
	CRITICAL_SECTION statusLock;		// STATUS access lock
	CRITICAL_SECTION infoStrLock;		// STATUS access lock

};