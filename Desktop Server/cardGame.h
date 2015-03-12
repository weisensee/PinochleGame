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
#include <string>
#include <queue>
#include <winsock2.h>
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\Client.h"
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\Message.h"
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\LogFile.h"		//Log writing library
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\GameP	layer.h"		//Log writing library
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\GameSettings.h"

class CardGame: public GamePlayer {
public:
	// ::CONSTRUCTORS AND INITIALIZERS::
	CardGame();
	CardGame(GameSettings toMake);			// constructs an active game loading settings from the game settings file passed in
	~CardGame();
	void run();			// starts the current game

	// ::SERVER COMMUNICATOR FUNCTIONS::
	int playerCount();			// returns current total players
	bool connectClient(Client * toAdd);	// adds new Client to list of Clients to add to game
	int getGameID();					// Returns game ID
	void setGameID(int newId);			// sets/updates the current game's gameID
	std::string getInfoString();		// returns game information as string
	//void buildInfoString();			// rebuilds and sets the game info string, function must take place within critical sections to avoid race conditions

private:
	// ::GAME SETUP::
	void setupGame(char * ntype, int * playerNum, int * obsNum, int * gGoal, std::string * gameName);	// parses player's game type request and initializes play environment
	bool checkForConnections();					// checks for new incoming connections
	bool addPlayer(Client * newPlayer, int n);	// adds specified new player to position n
	void echo(Client * curPlayer);				// echoes and talks to current player, for network testing purposes
	int ready();				// Returns true if the game is ready
	bool makePlayer(Client * player);			// adds player to current players, returns true if successful, false otherwise
	void decrementPlayerCount();				// decreases the player count by one
	void incrementPlayerCount();				// increases the player count by one
	void incrementObserverCount();				// increases the player count by one
	void decrementObserverCount(); 				// decreases the player count by one


	// ::PLAYER COMMUNICATION::
	bool handleRequest(Client * player, unsigned char request);		// handles request from Client and notifies Client of request status, returns true if success
	void sendToAll(unsigned char code);				// send message of type code to all players and observers
	void sendToAll(unsigned char code, char toSend);// send message of type code, with body of type toSend to all players and observers
	void sendToAll(unsigned char code, std::string * msg);	// send message of type code, with body of type msg to all players and observers
	bool checkCurrentConnections();					// checks for requests from Clients

	// ::GAME CONTROL FLOW::
	void play(char gType);	// Launch the game type specified
	void diconnectPlayers();	//disconnect all network connections
	int removeClient(Client * player); 	// removes Client from game, decrementing appropriate counters, returns previous position, 0 if not in game, 1-maxplayers if player, maxplayers-maxobservers if observer

	// ::GAMEPLAY::
	void managePinochleGamePlay();		//plays a pinochle round
	void manageEuchreGamePlay();		// plays a euchre round
	void euchreDealCards();				// deal out euchre hand to each player
	void euchreBiddingPhase();			// manages Euchre bidding phase
	void orderUp(Card top, int n);				// sets trump, makers and notifies all players that player n ordered up Card top
	void euchreTricksPhase();			// manages Euchre trick playing phase
	void pinochleBiddingPhase();		//manages entire bidding phase
	void pinochlemeldingPhase();		//manages entire melding phase
	void pinochleTricksPhase();			//manages entire trick taking phase
	void publishReport();		//totals scores and publishes to player
	void saveGame();	
	void quit(int type);		// shutdown/save the game and close connections, if type == 0, don't save
	bool restart();
	void handleRestartAns();	// runs the servers restart answer protocol
	// ::DATA::

	// players and connections
	Client ** PLAYERS;
	Client ** OBSERVERS;

	// Incrementers and Game variable states
	int * SCORES;						// current teams' scores
	int totalPlayers, totalObservers;	// current count of each
	// Static Game Data
	std::string infoString;	// most recent info string
	bool infoStringDirty;		// true if info string is dirty and needs to be updated
	bool gameOver;
	//LogFile * gLog;			// Game's log file

	// ::Threads and Server Connection::
	std::queue<Client*> newPlayers;		// new players to add to game
	CRITICAL_SECTION newPlayerListLock;	// new players access lock
	CRITICAL_SECTION playerCountLock;		// STATUS access lock
	
};