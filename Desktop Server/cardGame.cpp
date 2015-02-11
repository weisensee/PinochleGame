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
*/

#include "cardGame.h"

cardGame() {}
cardGame(client * p1);
void run();
// Setup

// Gameplay


//starts new game thread with arguments [(1)int: number of players] [(2)string player 0 name] [(3)player 0 socket] [(4) int observer limit] [(5)int Game ID#] [(6)string Game Name]
int  cardGame::run() {
	// Query for game type
	PLAYERS[0]->sendM(N_GQUERY);
	std::string

	//Initiate new game setup
	constructGame();


	//Connect players/wait for start
	//wait until the game is ready and players are ready to start
	//game.ready() returns -1 if all players have left.
	prepforstart();

	//play games until users want to quit
	do {
		//Manage play
		manageGamePlay();

		//Save game to archives
		saveGame();

	} while(!restart());	//Play another game if users want

	quit(1);

	return 0;
}
void cardGame::prepForStart() {		//manages the waiting/setup process
	while(ready() > 0) {					//while the game is not ready or empty
		addnewplayer();		//check for new players to add to game
		checkplayercommand();	//check for player requests
	}

	if (ready() < 0)					//if all players have left
		quit(0);						//quit without saving
}

//argv: [(1)int: number of players] [(2)string player 0 name] [(3)player 0 socket] [(4) int observer limit] [(5)int Game ID#] [(6)string Game Name]
void cardGame::constructGame(char T) {				// initialize game variables
	// Setup Player array
	MAXPLAYERS = (int) *argv[1];
	*PLAYERS = new player[MAXPLAYERS];
	PLAYERS[0] = new player(argv[2], *argv[3]);	// add player 1 to playerlist

	// Setup observer array
	MAXOBSERVERS = *argv[4];
	for (int i = 0; i < MAXOBSERVERS; ++i)
		OBSERVERS[i] = NULL;

	// Initiate other constants
	GAMEID = *argv[5];
	SCORES = new int[2];
	gameOver = false;
}
void cardGame::addPlayer(client * newPlayer) {		//checks on and adds any new players

}
void cardGame::checkPlayerCommand() {	//check with and execute player commands
}
void manageEuchreGamePlay() {		// plays a euchre round
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
	for (int i = 0; i < playerCount; i++) {
		PLAYERS[i]->sendM(RESTART_Q)
	}
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

int cardGame::ready() {				// Returns true if the game is ready
	return true;
}
