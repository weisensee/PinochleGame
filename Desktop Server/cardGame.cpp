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
cardGame::cardGame(client * nPlayer) {	// Sets up new game
	// Initiate log file
	gLog = new LogFile("game log file_", "C:\\Users\\Pookey\\OneDrive\\Projects\\PinochleGame\\logs\\games");

	// **********TESTING*************
	echo(nPlayer);

	// Get new game info
	int result = nPlayer->sendM(N_GQUERY);		// new game info query
	std::string * gameInfo;
	result = nPlayer->getStrAnswer(gameInfo, N_GINFO);

	// Setup Game
	setupGame(gameInfo);

	// Add player 1 to the game
	addPlayer(nPlayer);

	// Wait for game to be ready
	while (STATUS != 'R') {
		// Check for new players
		// Check for player requests
		// Update current players
	}

	// Once ready, play game:
	do {
		play(gType);
	} while (restart());		// While players still want to play, verify game-type data and restart

	// game data should be saved after each round, so no current need for a cleanup function.
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
void cardGame::setupGame(std::string * gameInfo) {	// parses player's game type request and initializes play environment
	// Ensure correct message was received by checking code byte at front
	if (gameInfo->at(0) == N_GINFO)	{
		// Iterate over gameInfo, processing information
		int counter = 1;

		// Game Type
		
		// Number of players
		
		// Number of Observers

		// Goal/winning score

		// Game name

		// Initiate other constants
		SCORES = new int[MAXPLAYERS/2];
		gameOver = false;
		totalObservers = 0;
		totalPlayers = 0;

		// Get game ID
		GAMEID = getGameID();


	}
	else
		gLog->writetolog("gameInfo setup string not of code 'N_GINFO'");
}
int cardGame::getGameID() {		// Returns game ID
	return 1;
}
void cardGame::echo(client * curPlayer) {		// echoes and talks to current player
	std::string * answer;
	// send hello
	curPlayer->sendM(0, "Hello client! welcome the card game situation!");

	while (true) {
		// Save and format answer
		int result = curPlayer->getStrAnswer(answer);
		answer->insert(0, "Answer received: ");
		printf(answer->c_str());						// Print out player response

		// Send response
		answer->insert(0, "Thank you client for: ");
		curPlayer->sendM(0, answer);
		curPlayer->sendM(0, "would you like to send us anything else?");
	}

}
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

int cardGame::ready() {				// Returns true if the game is ready
	return true;
}
