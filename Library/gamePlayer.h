/*	gamePlayer.h

	Card Game Client Game Playing Class

	Lucas Weisensee 2015

	manages playing the client side of a game
*/

#pragma once
#include <stdio.h>
#include <iostream>


#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\LogFile.h"		// Log writing class
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\client.h"		// Client communication class
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\query.h"		// Client communication class


class gamePlayer
{
public:
	gamePlayer(char pType, client * nServer, char TYPE, int PLAYERS, int GOAL, std::string * GAMENAME);	//Starts a game with the given arguments: player type, server object, game type, max players, max observers, winning score, game name
	~gamePlayer();
	bool play();			// starts playing the game, returns true if player is still connected and may want to continue connection

private:
	bool checkPlayerCommand();		// check for commands from player
	bool updateGameStatus();		// get updated status from server and update local variable
	bool playEuchre();				// plays a game of Euchre
	bool playPinochle();			// plays a game of Pinochle


	char playerType;
	client * server;
	char gameType;
	int maxPlayers;
	int goal;
	char* gameName;
	char gameStatus;		// current status of game
	query userQuery;

};