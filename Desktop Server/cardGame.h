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
#include <winsock2.h>
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\pinochleRound.h"
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\client.h"
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\message.h"
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\LogFile.h"		//Log writing library

class cardGame: public message {
public:
	cardGame();
	cardGame(client * p1);
	void run();

private:
	// Setup
	void cardGame::setupGame(std::string * gameInfo);	// parses player's game type request and initializes play environment
	void addPlayer(client * newPlayer);					//checks on and adds any new players
	void echo(client * curPlayer);						// echoes and talks to current player, for network testing purposes
	int getGameID();									// Returns game ID

	void prepForStart();		//manages the waiting/setup process
	int ready();				// Returns true if the game is ready
	void checkPlayerCommand();	//check with and execute player commands

	// Game Control Flow
	void play(char gType);	// Launch the game type specified
	void diconnectPlayers();	//disconnect all network connections


	// Gameplay
	void managePinochleGamePlay();		//plays a pinochle round
	void manageEuchreGamePlay();		// plays a euchre round
	void pinochleBiddingPhase();		//manages entire bidding phase
	void pinochlemeldingPhase();		//manages entire melding phase
	void pinochleTricksPhase();			//manages entire trick taking phase
	void publishReport();		//totals scores and publishes to player
	void saveGame();
	void quit(int type);		// shutdown/save the game and close connections, if type == 0, don't save
	bool restart();

	// Data
	int MAXOBSERVERS, MAXPLAYERS, GAMEID, GOAL;	// Maximum Observers and players, game's unique id, winning goal
	int totalPlayers, totalObservers;
	int * SCORES;
	std::string * gameName;
	client ** PLAYERS;
	client ** OBSERVERS;
	bool gameOver;
	char gType;				// Game type: 'P': pinochle, 'E': euchre
	char STATUS;			// Game's current status, 'W': waiting 'R': ready to play 'P':playing 'B':bored
	LogFile * gLog;			// Game's log file
};