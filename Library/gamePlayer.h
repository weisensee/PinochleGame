/*	GamePlayer.h

	Card Game Client Game Playing Class

	Lucas Weisensee 2015

	manages playing the Client side of a game
*/

#pragma once
#include <stdio.h>
#include <iostream>


//#include "..\Library\LogFile.h"		// Log writing class
#include "..\Library\Query.h"		// Client communication class
#include "..\Library\GameSettings.h"
#include "..\Library\Card.h"
#include "..\Library\EuchreAI.h"
#include "..\Library\PinochleAI.h"


class GamePlayer: public GameSettings {
public:
	GamePlayer();		// creates a GamePlayer, copying the given arguments
	GamePlayer(GameSettings toCopy);		// creates a GamePlayer, copying the given arguments
	GamePlayer(GameSettings* toMake, char pType, Client * nServer);		// creates a game with the given arguments
	~GamePlayer();
	bool run();			// starts playing the game, returns true if player is still connected and may want to continue connection

	// ::GAME MANAGEMENT::
	bool restart();						// returns true if the game will be restarting
	void setupNewRound();	// initiate the current round
	bool checkGameStatus();		// get updated status from server and update local variable, returns false if game is  closed/over
	bool playEuchre();			// plays a game of Euchre
	bool playPinochle();		// plays a game of Pinochle
	card AIpickCard();			// allows the AI to pick a card
	card getCardPlay();			// allows the user to pick a card
	int playCardFromMessage();	// plays a card on the current game from a Message from Client

	// ::PLAYER COMMUNICATION::
	void setUpNext();		// flags the player as up next
	bool QueryForOrderUp();	// Query player whether or not to call trump
	void orderUp(bool ans);	// decide as a table whether or not to call trump
	bool checkForRequests(Client * player);		// checks for and executes any commands from player
	bool handleRequest(Client * player, unsigned char request);	// handles request from Client and notifies Client of request status, returns true if success
	bool checkPlayerCommand();		// check for commands from player
	void talkToServer();		// allows the user to send/receive requests from the server "interactive mode"


	// DATA MEMBERS::
	card nextPlay;			// the next card to play
	EuchreAI eRound;		// current game round
	PinochleAI pRound;		// current game round
	std::string playerName;
	char playerType;	// 'H': human, 'A': AI, 'R': random AI
	Client * server;	// game manager
	char gameStatus;	// current status of game
	Query userQuery;
	int roundsPlayed;	// current game number
	bool restartMsg;		// restart after the current round?

	// :: THREAD COMMUNICATION::
	std::atomic<bool> upNext;
	void signalDataUpdate();	// signals to game manager that game data has been updated and should be rechecked
	WSAEVENT dataUpdate;
	CRITICAL_SECTION StaticDataLock;		// STATUS access lock
};