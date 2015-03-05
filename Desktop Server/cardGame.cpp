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


		// Data
		int MAXOBSERVERS, MAXPLAYERS, GAMEID, GOAL;	// Maximum Observers and players, game's unique id, winning goal
		int * SCORES;
		std::string * gameName;
		client ** PLAYERS;
		client ** OBSERVERS;
		bool gameOver;
		char gType;
		char STATUS;			// Game's current status, 'W': waiting 'R': ready to play 'P':playing 'B':bored
		LogFile * gLog;			// Game's log file
*/

#include "cardGame.h"


cardGame::cardGame() {}
cardGame::cardGame(client * p1, char * ntype, int * playerNum, int * obsNum, int * gGoal, std::string * gameName) {	// Sets up new game

	// Setup Game
	setupGame(ntype, playerNum, obsNum, gGoal, gameName);

	// Add player 1 to the game
	addPlayer(p1);

	// Wait for game to be ready
	while (STATUS != 'R') {
		// Check for new players
		checkForConnections();

		// Check for player requests
		checkForRequests();
		// Update current players

		// ??

		// check if game is ready
		if (ready())
			STATUS = 'R';
	}

	// Once ready, play game:
	do {
		play(gType);
	} while (restart());		// While players still want to play, verify game-type data and restart

	// game data should be saved after each round, so no current need for a cleanup function.
}
bool cardGame::checkForConnections() {				// checks for new incoming connections
	printf("\nchecking for connections...");

	return true;
}
bool cardGame::checkForRequests() {					// checks for requests from clients
	for (int i = 0; i < MAXPLAYERS; i++) {			// for each possible player
		if (PLAYERS[i] != NULL)						// if there is a player currently connected
			executeRequest(PLAYERS[i]);				// check and execute any commands
	}
	for (int i = 0; i < MAXOBSERVERS; i++) {			// for each possible player
		if (OBSERVERS[i] != NULL)						// if there is a player currently connected
			executeRequest(OBSERVERS[i]);				// check and execute any commands
	}
}
bool cardGame::executeRequest(client * player) {		// checks for and executes any commands from player
	// check for unhandled responsed
	unsigned char request;
	while (player->hasRequests()) {
		// get request from client
		request = player->getNextRequest();

		// handle request
		if (handleRequest(player, request) == false)
			return false;
	}

	// return success
	return true;
}
bool cardGame::handleRequest(client * player, unsigned char request) {	// handles request from client and notifies client of request status, returns true if success
	// execute appropriate response to request passed in as argument
	switch (request) {
	case STAT_QUERY:	// client requests game status
		player->sendM(G_STATUS, STATUS);
		player->requestHandled(G_STATUS);
		break;
	case BEC_PLAYER:
		player->requestHandled(BEC_PLAYER, makePlayer(player));
	}
}
bool cardGame::makePlayer(client * player)	{	// adds player to current players, returns true if successful, false otherwise
	// add player to player list
}
void cardGame::play(char gType) {	// Launch the game type specified
	switch (gType) {
	case 'P':
		managePinochleGamePlay();
		break;
	case 'E':
		manageEuchreGamePlay();
		break;
	}
}
void cardGame::setupGame(char * ntype, int * playerNum, int * obsNum, int * gGoal, std::string * gameName) {	// parses player's game type request and initializes play environment
	// Initiate log file
	gLog = new LogFile("game log file_", "C:\\Users\\Pookey\\OneDrive\\Projects\\PinochleGame\\logs\\games");

	// Setup game info
	gType = *ntype;
	MAXPLAYERS = *playerNum;
	MAXOBSERVERS = *obsNum;
	GOAL = *gGoal;
	PLAYERS = new client*[MAXPLAYERS];
	gameName->assign(gameName->c_str());
	SCORES = new int[MAXPLAYERS/2];
	gameOver = false;
	totalObservers = 0;
	totalPlayers = 0;


	// *******************TESTING*******************
	printf("Setting up %s a game of: %c for %d players and %d observers. The winning score will be: %d.", *gameName, gType, MAXPLAYERS, MAXOBSERVERS, GOAL);


}
int cardGame::getGameID() {		// Returns game ID
	return 1;
}
//void cardGame::echo(client * curPlayer) {		// echoes and talks to current player
//	std::string * answer;
//	// send hello
//	curPlayer->sendM(0, "Hello client! welcome the card game situation!");
//
//	while (true) {
//		// Save and format answer
//		int result = curPlayer->getStrAnswer(answer);
//		std::string answer->insert(0, "Answer received: ");
//		printf(answer->c_str());						// Print out player response
//
//		// Send response
//		answer->insert(0, "Thank you client for: ");
//		curPlayer->sendM(0, answer);
//		curPlayer->sendM(0, "would you like to send us anything else?");
//	}
//
//}
void cardGame::addPlayer(client * newPlayer) {		//checks on and adds any new players

}
void cardGame::checkPlayerCommand() {	//check with and execute player commands
}
void cardGame::manageEuchreGamePlay() {		// plays a euchre round
	// Play Euchre
}
void cardGame::managePinochleGamePlay() {//plays a pinochle round
	while (!gameOver) {
		pinochleBiddingPhase();
		pinochlemeldingPhase();
		pinochleTricksPhase();
		publishReport();
	}
}
void cardGame::pinochleBiddingPhase() {		//manages entire bidding phase

}
void cardGame::pinochlemeldingPhase() {		//manages entire melding phase

}
void cardGame::pinochleTricksPhase() {			//manages entire trick taking phase

}
void cardGame::publishReport() {		//totals scores and publishes to player

}
void cardGame::diconnectPlayers() {	//disconnect all network connections

}
void cardGame::saveGame() {

}
bool cardGame::restart() {
	int temp;
	int ans = 0;
	for (int i = 0; i < totalPlayers; i++) {	// send query
		PLAYERS[i]->sendM(RESTART_G);
	}
	for (int i = 0; i < totalPlayers; i++) {	// tally responses
		PLAYERS[i]->getIntAnswer(&temp);
		ans += temp;
	}

	return (totalPlayers == ans);				// return group decision
}



//manage shutdown of the game and connections
void cardGame::quit(int type) {		// shutdown/save the game and close connections, if type == 0, don't save

	//Disconnect users
	for (int i = 0; i < MAXPLAYERS; ++i)
		PLAYERS[i]->closeConnection();

	//disconnect any observers
	for (int i = 0; i < MAXOBSERVERS; ++i) 		//for each occupied observer spot
		if (OBSERVERS[i])						//disconnect it if it exists
			OBSERVERS[i]->closeConnection();


	delete [] *PLAYERS;
	delete [] *OBSERVERS;

	WSACleanup();		//deallocates WSA data
}

int cardGame::ready() {				// Returns true if the game is at capacity
	return totalPlayers == MAXPLAYERS;
}
