/*	GamePlayer.cpp
	
	Card Game Client Game Playing Class

	Lucas Weisensee 2015

	functions for managing playing the Client side of a game

	see GamePlayer.h for more information

	data objects:
			char playerType;
			Client * server;
			char gType;
			int MAXPLAYERS;
			int GOAL;
			char* gameName;
*/

#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\GamePlayer.h"		// Client communication class


GamePlayer::GamePlayer(){		// creates a GamePlayer, copying the given arguments
	playerType = 'U';
	server = NULL;

	// initialize critical section
	if (!InitializeCriticalSectionAndSpinCount(&StaticDataLock, 0x00000400)){	/* Check for any errors that  might occur*/
		perror("\nMutex Initiation error: ");		// report errors if any
		exit(0);
	}


	// Initialize "update signal" event object
	// (uninheritable by child processes, auto reset, initialized to signaled, no name)
	dataUpdate = CreateEvent(NULL, false, true, NULL);
	if (dataUpdate)
		server->setUpdateEvent(dataUpdate);
	else
		printf("\ncreate event error in GamePlayer");

	roundsToPlay = 0;	// amount of times to play current game
	roundsPlayed = 0;	// current game number
	restartMsg = false;	// restart after the current round?

}
GamePlayer::GamePlayer(GameSettings toCopy) :GameSettings(toCopy){		// creates a GamePlayer, copying the given arguments
	GamePlayer();
}
GamePlayer::GamePlayer(GameSettings *toMake, char pType, Client * nServer):GameSettings(*toMake) {		// creates a game with the given arguments
	GamePlayer();

	playerType = pType;
	server = nServer;

	// send event handle to Client
	server->setUpdateEvent(dataUpdate);
}
GamePlayer::~GamePlayer() {
	// server 
	if (server) {
		delete server;
		server = NULL;
	}
}
bool GamePlayer::run() {			// starts playing the game, returns true if player is still connected and may want to continue connection
	// get game string and status
	std::string infostring;
	Sleep(1500);	//sleep for a sec to let the server setup
	server->getStrAnswer(&infostring, G_STATUS);
	setFromInfoString(infostring.c_str());
	printf("\nIn Game: %s", GameSettings::print().c_str());

	// while the game is not ready to start
	bool ready = false;
	while (!ready) {
		// Wait (x) microseconds for information update
		int result = WaitForSingleObject(dataUpdate, 10000);

		printf("\nQuit Waiting, result: %d", result);

		// check if user wants to send any command to server
		if (playerType == 'H')
			checkPlayerCommand();

		// check/handle Messages from server
		// specifically game status updates
		checkForRequests(server);

		// update game status from server
		if (!checkGameStatus())// get updated status from server and update local variable, returns false if game is  closed/over
			break;

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
// ::PLAYER COMMUNICATION::
bool GamePlayer::checkPlayerCommand() {	// check for commands from player
	// if the player is human, allow to check for commands
	if (playerType == 'H')
		if (userQuery.bQuery("Would you like to send a command to the server?"))
			talkToServer();

	return true;
}
bool GamePlayer::checkGameStatus() {		// get updated status from server and update local variable, returns false if game is  closed/over
	// get status from server

	// check that game hasnt quit
	if (STATUS == 'Q')
		return false;

	//return success
	return true;

}
int GamePlayer::playCardFromMessage() {	// plays a card on the current game from a Message from Client
	if (gType == 'E')
		return eRound.playCard(*server->getNextPlay());
	else if (gType == 'E')
		return pRound.playCard(*server->getNextPlay());
	else
		printf("ERROR: invalid game type!");

}

// ::GAME MANAGEMENT::
bool GamePlayer::playEuchre() {		// plays a game of Euchre
	do {
		eRound.reset();	// reset the current round's status

		setupNewRound();		// deals cards and handles trump bidding
		while (eRound.inPlay()) {			// while the round is not over
			// if this player isn't up to play, get the next card from server/other players
			if (!eRound.upNext()) {
				// Wait (x) microseconds for information update
				if (WaitForSingleObject(dataUpdate, 4000) == WAIT_FAILED) {// check if wait failed
					printf("\nWait failed, server disconnected, error: %d", GetLastError());
				}

				// check and process any Messages
				// specifically: update eRound with any new cards
				checkForRequests(server);
			}
			// if this player is up to play, get their play
			else {
				// pick a card to play
				if (playerType != 'H')
					getCardPlay();	// human
				else
					AIpickCard();	// AI
			}
		}
	} while (restart());
}
card GamePlayer::AIpickCard() {			// allows the AI to pick a card
	do {
		nextPlay = eRound.getAIPlay();
	} while (!eRound.playCard(nextPlay));
}
card GamePlayer::getCardPlay() {			// allows the user to pick a card
	do {
		// print possible choices
		eRound.printChoices();

		// Query user for their choice
		nextPlay = userQuery.iQuery("Enter the number for the card you'd like to play ");
	} while (!eRound.playCard(nextPlay));
}
bool GamePlayer::restart() {						// returns true if the game will be restarting
	// if the # of rounds played is less than to play
	// and if the server sent a restart Message
	if (roundsToPlay > roundsPlayed && restartMsg) 
		return true;

	// otherwise dont restart
	return false;
}
void GamePlayer::setupNewRound() {	// initiate the current round
	bool trumpPicked = false;

	// dont start the round until the trump has been picked for this current round
	while (eRound.pickingTrump()) {
		// get hand and player number from dealer/server
		std::string ans;
		server->getStrAnswer(&ans, HAND_DEALT);

		// set player number, hand and reset round data
		eRound.parseDealtHand(&ans);

		// participate in trump setting
		while (eRound.pickingTrump()) {
			server->getStrAnswer(&ans, KITTY_FLP);	// send flipped card to eRound

			// redeal if stack was empty
			if (eRound.flipKitty() == 0)
				break;			// prepare for redeal
			
			// if the current player is up to pick, ask them if they'd like to pass or call
			if (eRound.upNext())
				orderUp(QueryForOrderUp());

			// if the current player is not up to pick
			// Wait for information update
			int result = WaitForSingleObject(dataUpdate, 10000);

			// check for new card Message
			checkForRequests(server);
		}
	}
}
bool GamePlayer::QueryForOrderUp() {	// Query player whether or not to call trump
	if (playerType == 'H') {
		eRound.print(eRound.flipKitty());	// show card to user
		char ans = userQuery.cQuery("Do you want to:\n[O]rder up \nor\n[P]ass?", "OP");
		return ans = 'O';		// return true if player 'orders up'
	}
	else
		return eRound.getAIPlay().value;
}
void GamePlayer::orderUp(bool ans) {		// decide as a table whether or not to call trump
	// send appropriate Message to server and local card round
	if (ans) {			// if player ordered up
		eRound.orderUp(eRound.playerNum);	//update server and local game file
		server->sendM(ORDER_UP, 1);
	}
	else {
		eRound.pass();
		server->sendM(ORDER_UP,(int)0);
	}
}
bool GamePlayer::playPinochle() {			// plays a game of Pinochle
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

// :: THREAD COMMUNICATION::
bool GamePlayer::checkForRequests(Client * player) {		// checks for and executes any commands from player
	// check for unhandled responses
	unsigned char request;
	bool hasReq = false;
	int result = player->hasRequests(&hasReq);

	DEBUG_IF(true) {
		//player->printinbox();
		if (hasReq)
			printf("\nplayer has requests");
		else
			printf("\nplayer doesnt have requests");
	}

	// while the player is still connected and has requests to execute

	//printf("\nbefore:");
	//player->printRequests();

	while (result > 0 && hasReq) {
		// get request from Client
		request = player->getNextRequest();

		// handle request
		if (handleRequest(player, request) == false)
			return false;


		// check again for connection and requests
		hasReq = false;
		result = player->hasRequests(&hasReq);
	}

	// return success
	return true;
}
bool GamePlayer::handleRequest(Client * player, unsigned char request) {	// handles request from Client and notifies Client of request status, returns true if success
	// execute appropriate response to request passed in as argument
	switch (request) {
	case R_PNAME:	// Client requests player name
		player->sendM(G_STATUS, STATUS);
		player->requestHandled(G_STATUS);
		break;
	case G_LIST:	// Client requests game list
		printf("\nUnhandled request:%u from player: %s", request, player->getName());
		break;
	case RESTART_G:	// Requesting restart verification
		int msg;
		if (restart())
			msg = 1;
		else
			msg = 0;
		player->sendM(RESTART_ANS, msg);
		player->requestHandled(RESTART_G);
		break;
	case N_GQuery:	// Query For new Game Creation Info
		// wait for player to respond
		break;
	case N_GINFO: // New Game Creation Info
		// if the game data can be updated, update it
		// either way, return current game data
		if (eRound.inPlay() == false && pRound.inPlay() == false){
			std::string ans;
			player->getStrAnswer(&ans, N_GINFO);
			setFromInfoString(ans.c_str());
		}
		player->sendM(G_STATUS, settingsString());
		player->requestHandled(N_GINFO);
		break;
	case PING_QRY:	// Client requests connection status
		player->sendM(PING_ANS);			// send connected signal
		player->requestHandled(PING_QRY);
		break;
	case STAT_Query:	// Client requests game status
		player->sendM(G_STATUS, settingsString());	// send it
		player->requestHandled(G_STATUS);
		break;
	case REQ_PLAY:	 // Request for card play from Client
		// request handled by game playing functions
		// and should be deleted by them also
		break;
	case PLY_CARD:	 // Play card on trick
		player->sendM(ACC_CARD, playCardFromMessage());	// return result
		player->requestHandled(ACC_CARD);
		break;
	default:
		return false;
	}
	return true;
}
void GamePlayer::signalDataUpdate() {	// signals to game manager that game data has been updated and should be rechecked
	//signal update object
	if (!SetEvent(dataUpdate))
		printf("\nSetEvent failed on signaling 'dataUpdate'");	// print error if it fails
}
void GamePlayer::talkToServer() {		// allows the user to send/receive requests from the server "interactive mode"
	printf("\ntalking to server...");
}
