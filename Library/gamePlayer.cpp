/*	gamePlayer.cpp
	
	Card Game Client Game Playing Class

	Lucas Weisensee 2015

	functions for managing playing the client side of a game

	see gamePlayer.h for more information

	data objects:
			char playerType;
			client * server;
			char gType;
			int MAXPLAYERS;
			int GOAL;
			char* gameName;
*/

#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\gamePlayer.h"		// Client communication class

gamePlayer::gamePlayer(gameSettings *toMake, char pType, client * nServer):gameSettings(*toMake) {		// creates a game with the given arguments
	playerType = pType;
	server = nServer;
}
bool gamePlayer::play() {			// starts playing the game, returns true if player is still connected and may want to continue connection
	// while the game is not ready to start
	bool ready = false;
	while (!ready) {
		// check if user wants to send any command to server
		checkPlayerCommand();

		// update game status from server
		updateGameStatus();

		// wait one second between loops
		Sleep(500);

		// if game is ready, start play
		if (gameStatus == 'R')
			ready = true;
	}

	switch (gType) {
	case 'E':
		return playEuchre();
	case 'P':
		return playPinochle();
	default:
		printf("\ngType not recognized, quitting...");
		return false;
	}
}
bool gamePlayer::checkPlayerCommand() {	// check for commands from player
	// if the player is human, allow to check for commands
	if (playerType == 'H')
		printf("\nChecking with human player");

	return true;
}
bool gamePlayer::updateGameStatus() {		// get updated status from server and update local variable
	// get status from server
	std::string * ans = new std::string();
	int result = server->getStrQueryAnswer(ans, STAT_QUERY, G_STATUS);

	// check that query was successful
	if (result < 1)
		return false;

	// update local status
	gameStatus = ans->at(0);
	printf("\nCurrent Game status: %c", gameStatus);

	//return success
	return true;

}
bool gamePlayer::playEuchre() {		// plays a game of Euchre
	char ans = userQuery.cQuery("Now 'Playing Euchre' in 'playEuchre', enter a P to play or a D to disconnect", "PD");

	if (ans == 'P')
		return true;
	else if (ans == 'D')
		return false;
	else {
		printf("\nPlayEuchx`re, If/Else error");
		return false;
	}
}
bool gamePlayer::playPinochle() {			// plays a game of Pinochle
	char ans = userQuery.cQuery("Now 'Playing Pinochle' in 'playPinochle', enter a P to play or a D to disconnect", "PD");

	if (ans == 'P')
		return true;
	else if (ans == 'D')
		return false;
	else {
		printf("\nplayPinochle, If/Else error");
		return false;
	}
}
gamePlayer::~gamePlayer() {
	// server 
	if(server) {
		delete server;
		server = NULL;
	}
}
