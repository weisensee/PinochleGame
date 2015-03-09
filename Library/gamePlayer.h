/*	gamePlayer.h

	Card Game Client Game Playing Class

	Lucas Weisensee 2015

	manages playing the client side of a game
*/

#pragma once
#include <stdio.h>
#include <iostream>


//#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\LogFile.h"		// Log writing class
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\client.h"		// Client communication class
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\query.h"		// Client communication class
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\gameSettings.h"


class gamePlayer: gameSettings {
public:
	gamePlayer(gameSettings *toMake, char pType, client * nServer);		// creates a game with the given arguments
	~gamePlayer();
	bool play();			// starts playing the game, returns true if player is still connected and may want to continue connection

private:
	bool checkPlayerCommand();		// check for commands from player
	bool updateGameStatus();		// get updated status from server and update local variable
	bool playEuchre();				// plays a game of Euchre
	bool playPinochle();			// plays a game of Pinochle


	char playerType;
	client * server;
	char gameStatus;		// current status of game
	query userQuery;

};