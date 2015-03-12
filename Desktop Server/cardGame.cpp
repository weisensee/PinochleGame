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
		Client ** PLAYERS;
		Client ** OBSERVERS;
		bool gameOver;
		char gType;
		char STATUS;			// Game's current status, 'W': waiting 'R': ready to play 'P':playing 'B':bored
		LogFile * gLog;			// Game's log file
*/

#include "CardGame.h"		//exception library


//****************************************************************************
// ::CONSTRUCTORS AND INITIALIZERS::
//****************************************************************************
CardGame::CardGame() {
}
CardGame::CardGame(GameSettings toMake):gamePlayer(toMake) {			// constructs an active game loading settings from the game settings file passed in
	// Setup Game
	// Initiate log file
	//gLog = new LogFile("game log file_", "C:\\Users\\Pookey\\OneDrive\\Projects\\PinochleGame\\logs\\games");

	infoStringDirty = true;

	PLAYERS = new Client*[MAXPLAYERS];
	OBSERVERS = new Client*[MAXOBSERVERS];

	// set score board
	SCORES = new int[MAXPLAYERS / 2];
	for (int i = 0; i < MAXPLAYERS / 2; i++)
		SCORES[i] = 0;
	gameOver = false;
	totalObservers = 0;
	totalPlayers = 0;

	// set player and observer pointers to NULL
	for (int i = 0; i < MAXPLAYERS; i++)
		PLAYERS[i] = NULL;
	for (int i = 0; i < MAXOBSERVERS; i++) 
		OBSERVERS[i] = NULL;

	// Initialize Critical Section Locks 
	if (!InitializeCriticalSectionAndSpinCount(&playerCountLock, 0x00000400) ||/* Check for any errors that  might occur*/
		!InitializeCriticalSectionAndSpinCount(&newPlayerListLock, 0x00000400)) {
		perror("\nMutex Initiation error: ");		// report errors if any
		exit(0);
	}

	DEBUG_IF(true)
		printf("Setting up %s a game of: %c for %i players and %i observers. The winning score will be: %i.", gameName.c_str(), gType, MAXPLAYERS, MAXOBSERVERS, GOAL);

	// Add player 1 to the game
	addPlayer(player1, 0);

}
CardGame::~CardGame() {				// destructor
	// release critical sections
	DeleteCriticalSection(&StaticDataLock);
	DeleteCriticalSection(&newPlayerListLock);

	// release arrays of players and Observers
	if (PLAYERS)
		delete[] PLAYERS;

	if (OBSERVERS)
		delete[] OBSERVERS;

	//// delete log object
	//delete gLog;
}	
//****************************************************************************
// ::SERVER COMMUNICATOR FUNCTIONS::
//****************************************************************************
std::string CardGame::getInfoString() {		//returns game information as string
	std::string temp;

	// if the info string is out of date, rebuild it
	if (infoStringDirty) {
		// Request ownership of StaticDataLock critical section.
		EnterCriticalSection(&StaticDataLock);


		infoString = *settingsString();
		infoStringDirty = false;

		// Release ownership of StaticDataLock critical section.
		LeaveCriticalSection(&StaticDataLock);
	}

	// copy the info string
	temp = infoString;
	

	// Return info string
	return temp;
}
//void CardGame::buildInfoString() {		// rebuilds and sets the game info string, function must take place within critical sections to avoid race conditions
//	// Request ownership of appropriate critical sections before copying them to send
//	EnterCriticalSection(&statusLock);
//	char tStatus = STATUS;
//	LeaveCriticalSection(&statusLock);
//
//	EnterCriticalSection(&gameIdLock);
//	int tID = GAMEID;
//	LeaveCriticalSection(&gameIdLock);
//
//	EnterCriticalSection(&infoStrLock);
//	std::string tName = gameName;
//	LeaveCriticalSection(&infoStrLock);
//
//
//	// set new info string
//	infoString = tStatus;
//	infoString += '^';
//	infoString.append(std::to_string(tID));
//	infoString += '^';
//	infoString.append(tName);
//	infoString += '^';
//	infoString.append(PLAYERS[0]->getName());
//
//	//append other player names if available
//	if (totalPlayers > 1)
//		for (int i = 1; i < totalPlayers; ++i)
//			infoString.append('^' + PLAYERS[i]->getName());
//}
void CardGame::setGameID(int newId) {			// sets/updates the current game's gameID
	EnterCriticalSection(&StaticDataLock);
	
	infoStringDirty = true;
	GAMEID = newId;		// read value

	// Release ownership of StaticDataLock critical section.
	LeaveCriticalSection(&StaticDataLock);
}
int CardGame::getGameID() {		// Returns game ID
	//// Request ownership of StaticDataLock critical section.
	//EnterCriticalSection(&StaticDataLock);

	int ID = GAMEID;		// read value

	//// Release ownership of StaticDataLock critical section.
	//LeaveCriticalSection(&StaticDataLock);

	return ID;
}
int CardGame::playerCount() {			// returns current total players
	return totalPlayers;
}
bool CardGame::connectClient(Client * toAdd) {	// adds new Client to list of Clients to add to game
	// Request ownership of newPlayerListLock critical section.
	EnterCriticalSection(&newPlayerListLock);

	// add new player to list
	newPlayers.push(toAdd);

	// Release ownership of newPlayerListLock critical section.
	LeaveCriticalSection(&newPlayerListLock);

	// signal Client updates
	signalDataUpdate();

	return true;

}
//****************************************************************************
// ::GAME SETUP::
//****************************************************************************
bool CardGame::addPlayer(Client * newPlayer, int n) {		//checks on and adds any new players
	
	// give player the newMessage event handle
	if (dataUpdate)
		newPlayer->setUpdateEvent(dataUpdate);
	else
		printf("\ncreate event error in CardGame");

	// check that no player occupies desired spot n
	if (PLAYERS[n] != NULL)
		return makePlayer(newPlayer);

	// if the spot is unoccupied
	PLAYERS[n] = newPlayer;
	incrementPlayerCount();
	return true;
}
void CardGame::decrementPlayerCount() {				// decreases the player count by one
	// Request ownership of playerCountLock critical section.
	EnterCriticalSection(&playerCountLock);

	totalPlayers--;
	infoStringDirty = true;

	// Release ownership of playerCountLock critical section.
	LeaveCriticalSection(&playerCountLock);
}
void CardGame::incrementPlayerCount() {				// increases the player count by one
	// Request ownership of playerCountLock critical section.
	EnterCriticalSection(&playerCountLock);

	totalPlayers++;

	// Release ownership of playerCountLock critical section.
	LeaveCriticalSection(&playerCountLock);
}
void CardGame::decrementObserverCount() {				// decreases the player count by one
	// Request ownership of playerCountLock critical section.
	EnterCriticalSection(&playerCountLock);

	totalObservers--;

	// Release ownership of playerCountLock critical section.
	LeaveCriticalSection(&playerCountLock);
}
void CardGame::incrementObserverCount() {				// increases the player count by one
	// Request ownership of playerCountLock critical section.
	EnterCriticalSection(&playerCountLock);

	totalObservers++;

	// Release ownership of playerCountLock critical section.
	LeaveCriticalSection(&playerCountLock);

}
bool CardGame::makePlayer(Client * player)	{	// adds player to current players, returns true if successful, false otherwise
	// add player to player list
	// get player position preference from player
	int newPos = player->getPlayerPref();
	int oldPos = -1;

	// if preference is valid, add them to position
	if (newPos > 0 && newPos < MAXPLAYERS) {
		// remove user currently occupying position
		Client * otherPlayer = PLAYERS[newPos];
		if (otherPlayer != NULL)
			decrementPlayerCount();
		PLAYERS[newPos] = NULL;			// set new position to NULL

		// remove player from position in game
		oldPos = removeClient(player);		// removes Client from game, decrementing appropriate counters and returning previous position

		// add user to desired position
		addPlayer(player, newPos);

		// if other player occupied a position, add them to player's previous position, if they had one
		if (otherPlayer != NULL)
			addPlayer(otherPlayer, oldPos);
		// if the player didn't have a previous position
		else {
			otherPlayer->setPlayerPref(0);	// reset player preference
			connectClient(otherPlayer);		// put them back in the connection queue
		}
	}
	// if position preference is invalid
	else
		return false;	// return failure to player

	return true;		// return true if success
}
//****************************************************************************
// ::GAME CONTROL FLOW::
//****************************************************************************
void CardGame::run() {			// starts the current game
	// Wait for game to be ready
	while (STATUS != 'R') {
		// Wait (x) microseconds for information update
		int result = WaitForSingleObject(dataUpdate, 10000);
		
		printf("\nQuit Waiting, result: %d", result);

		// Check for new players
		checkForConnections();

		// Check on current connections status and requests
		checkCurrentConnections();

		// check if game is ready
		if (ready())
			STATUS = 'R';

		// If the users quit, quit
		if (STATUS == 'Q')
			quit(0);
	}

	// Once ready, play game:
	do {
		play(gType);
	} while (restart());		// While players still want to play, verify game-type data and restart

	// game data should be saved after each round, so no current need for a cleanup function.

}
void CardGame::play(char gType) {	// Launch the game type specified
	// update players
	sendToAll(G_STATUS, settingsString());

	//update status to playing
	STATUS = 'P';

	switch (gType) {
	case 'P':
		managePinochleGamePlay();
		break;
	case 'E':
		manageEuchreGamePlay();
		break;
	}
}
int CardGame::removeClient(Client * player) { 	// removes Client from game, decrementing appropriate counters, returns previous position, 0 if not in game, 1-maxplayers if player, maxplayers-maxobservers if observer
	int oldPos = 0;

	// check if player is playing
	for (int i = 0; i < MAXPLAYERS; i++)	// for each player position
		if (PLAYERS[i] == player) {			// check if that position is the player in question
			oldPos = i;						// if so, save that position
			PLAYERS[i] = NULL;
		}
	// decrement player count if player was removed from current game
	if (oldPos) {
		decrementPlayerCount();
		return oldPos;					// return their player position
	}
	// otherwise check if player is an observer
	else {
		for (int i = 0; i < MAXOBSERVERS; i++)	// for each player position
			if (OBSERVERS[i] == player) {			// check if that position is the player in question
				oldPos = i;						// if so, save that position
				OBSERVERS[i] = NULL;
			}

		// check if player was an observer
		if (oldPos) {
			decrementObserverCount();
			return oldPos + MAXPLAYERS;			// return their observer position
		}
	}

	// otherwise player is waiting to be added, return 0
	return 0;
}

//****************************************************************************
// ::PLAYER COMMUNICATION::
//****************************************************************************
bool CardGame::handleRequest(Client * player, unsigned char request) {	// handles request from Client and notifies Client of request status, returns true if success
	
	// execute appropriate response to request passed in as argument
	switch (request) {
	case G_LIST:	// Client requests game list
		printf("\nUnhandled request:%u from player: %s", request, player->getName());
		break;
	case RESTART_ANS:	// Restart answer
		handleRestartAns();
		player->requestHandled(RESTART_ANS);
		break;
	case N_GINFO: // New Game Creation Info
		player->sendM(G_STATUS, settingsString());
		player->requestHandled(N_GINFO);
		break;
	case BEC_PLAYER:// Become Player
		player->requestHandled(BEC_PLAYER, makePlayer(player));	// send the result of the makePlayer command
		break;
	case BEC_OBS:// Become Observer
		player->requestHandled(BEC_OBS, makePlayer(player));	// send the result of the makePlayer command
		break;
	default:	// request not handled
		// Run in base class to accept any default responses
		if (gamePlayer::handleRequest(player, request))
			return true;
		printf("\nUnhandled request:%u from player: %s", request, player->getName());
		return false;
	}

	return true;
}
void CardGame::sendToAll(unsigned char code) {		// send message of type code to all players and observers
	sendToAll(code, '\0');
}
void CardGame::sendToAll(unsigned char code, char msg) {	// send message of type code, with body of type msg to all players and observers
	//convert to string
	std::string temp;
	temp.fill(1, msg);

	// send message to all players

}
void CardGame::sendToAll(unsigned char code, std::string * msg) {	// send message of type code, with body of type msg to all players and observers
		// send message to all players
		for (int i = 0; i < MAXPLAYERS; ++i)
		if (PLAYERS[i])
			PLAYERS[i]->sendM(code, msg);

	// send message to all observers
	for (int i = 0; i < MAXOBSERVERS; ++i)
		if (OBSERVERS[i])
			OBSERVERS[i]->sendM(code, msg);
}
bool CardGame::checkForConnections() {				// checks for new incoming connections
	printf("\nchecking for connections...");

	// Request ownership of newPlayerList critical section.
	EnterCriticalSection(&newPlayerListLock);

	// if new players have connected, add them to the game
	while (newPlayers.size() > 0) {
		// get next Client
		Client * toAdd = newPlayers.front();	// copy over new Client data

		// get preferred position
		int pos = toAdd->getPlayerPref();
		if (pos == 0) {					//if the player doesnt have a preferred position
			if (MAXPLAYERS > totalPlayers){				// if there is still room in the game
				for (int i = 0; i < MAXPLAYERS; i++)	// add them to the first available one
					if (PLAYERS[i] == NULL) {				// iterate to the first available position
						pos = i;							// save that position and quit looking
						i += MAXPLAYERS;
					}
			}
			else 		// if there is no room in the game
				pos = MAXPLAYERS + 1;
		}

		// add player to the position chosen
		addPlayer(toAdd, pos);

		newPlayers.pop();		// remove Client from list
	}

	// Release ownership of newPlayerListLock critical section.
	LeaveCriticalSection(&newPlayerListLock);

	return true;
}
bool CardGame::checkCurrentConnections() {					// checks for requests from Clients
	for (int i = 0; i < MAXPLAYERS; i++) {				// for each possible player
		if (PLAYERS[i] != NULL && PLAYERS[i]->getStatus() > 0) {	// if there is a player currently connected
			DEBUG_IF(PLAYERS[i]->getStatus() <= 0)
				printf("\nskipping player %s, status: %c", PLAYERS[i]->getName(), PLAYERS[i]->getStatus());
			checkForRequests(PLAYERS[i]);					// check and execute any commands
		}
	}
	for (int i = 0; i < MAXOBSERVERS; i++) {			// for each possible player
		if (OBSERVERS[i] != NULL && OBSERVERS[i]->getStatus() > 0)	// if there is a player currently connected
			checkForRequests(OBSERVERS[i]);						// check and execute any commands
	}

	// return success
	return true;
}
void handleRestartAns() {	// runs the servers restart answer protocol
	printf("\nhandling restart answer");
}
/*void CardGame::echo(Client * curPlayer) {		// echoes and talks to current player
//	std::string * answer;
//	// send hello
//	curPlayer->sendM(0, "Hello Client! welcome the Card game situation!");
//
//	while (true) {
//		// Save and format answer
//		int result = curPlayer->getStrAnswer(answer);
//		std::string answer->insert(0, "Answer received: ");
//		printf(answer->c_str());						// Print out player response
//
//		// Send response
//		answer->insert(0, "Thank you Client for: ");
//		curPlayer->sendM(0, answer);
//		curPlayer->sendM(0, "would you like to send us anything else?");
//	}
//
//}	*/

//****************************************************************************
// ::GAMEPLAY::
//****************************************************************************
void CardGame::manageEuchreGamePlay() {		// plays a euchre round
	// check player count
	if (MAXPLAYERS != 4) {
		printf("\nMax Players Error, MAXPLAYERS: %d", MAXPLAYERS);
		return;
	}		

	// Play Euchre
	while (restart()) {
		euchreDealCards();
		euchreBiddingPhase();
		euchreTricksPhase();
		publishReport();
	}
}
void CardGame::euchreDealCards() {		// deal out euchre hand to each player
	// shuffle and deal out Cards
	eRound.reset();

	// send dealt hand to each player
	for (int i = 0; i < MAXPLAYERS; i++)					// for each player
		PLAYERS[i]->sendM(HAND_DEALT, eRound.handDealtString(i));	// send hand dealt with player number
}
void CardGame::euchreBiddingPhase() {	// manages Euchre bidding phase
	bool trumpSet = false;
	while (!trumpSet) {
		// flip over the trump Card so each player can see
		Card top = eRound.flipKitty();

		// If the stack was empty, re-deal 
		if (top.value == 0) {
			euchreDealCards();	// re-deal
			continue;			// restart while loop
		}

		// give each player the chance to call trump, in clockwise order starting with the dealer
		sendToAll(KITTY_FLP, top.chr());		// notify all players of trump Card being flipped
		int ans;
		for (int i = 0; i < MAXPLAYERS; i++) {
			int result = PLAYERS[(i + eRound.leader) % MAXPLAYERS]->getIntAnswer(&ans, ORDER_UP);	// get answer from player and ensure correct message was received
			if (result > 0) {
				if (ans > 0) {
					orderUp(top, (i + eRound.leader) % MAXPLAYERS);	// sets trump, makers and notifies all players that player n ordered up Card top
					i += MAXPLAYERS;							// stop asking players if they want to order up
					trumpSet = true;
				}
				else
					sendToAll(TRMP_ORD, '0');		// notify other players the current player passed, order up = pass or 'false' or 0
			}
			else
				printf("\nPlayer connection error with player: %d, result: %d", (i + eRound.leader) % MAXPLAYERS, result);
		}
		// If no one bids flip over the next Card on next loop through while()		
	}
}
void CardGame::euchreTricksPhase() {	// manages Euchre trick playing phase
	int hasLead = eRound.leader;
	Card* recent;
	bool legal;
	// When all tricks have been played, quit
	while (eRound.inPlay()) {
		// Get first Card, The dealer goes first
		// gather one trick
		for (int i = 0; i < MAXPLAYERS; i++) {						// for each player, starting with the trick leader
			// get/check current players play
			legal = false;
			while (!legal) {										// while they haven't played a legal Card
				recent = PLAYERS[hasLead + i]->getNextPlay();		// request play and verify legality
				legal = eRound.playCard(*recent);					// if play was legal, move to next player				
			}
			// update other players of Card recently played
			sendToAll(PLY_Card, recent->chr());
		}
		// Whoever wins the trick, goes first next trick
		hasLead = eRound.wonLastTrick();
	}
}
void CardGame::orderUp(Card top, int n) {	// sets trump, makers and notifies all players that player n ordered up Card top
	// set trump makers
	eRound.orderUp(n);

	// notify all players and observers
	sendToAll(TRMP_ORD, 1);

	// check if anyone wants to go it alone?? ***********************************
}
void CardGame::managePinochleGamePlay() {//plays a pinochle round
	while (!gameOver) {
		pinochleBiddingPhase();
		pinochlemeldingPhase();
		pinochleTricksPhase();
		publishReport();
	}
}
void CardGame::pinochleBiddingPhase() {		//manages entire bidding phase

}
void CardGame::pinochlemeldingPhase() {		//manages entire melding phase

}
void CardGame::pinochleTricksPhase() {			//manages entire trick taking phase

}
void CardGame::publishReport() {		//totals scores and publishes to player

}
void CardGame::diconnectPlayers() {	//disconnect all network connections

}
void CardGame::saveGame() {

}
bool CardGame::restart() {
	int temp;
	int ans = 0;
	if ()
	for (int i = 0; i < totalPlayers; i++) {	// send query
		PLAYERS[i]->sendM(RESTART_G);
	}
	for (int i = 0; i < totalPlayers; i++) {	// tally responses
		PLAYERS[i]->getIntAnswer(&temp, RESTART_ANS);
		ans += temp;
	}

	return (totalPlayers == ans);				// return group decision
}
void CardGame::quit(int type) {		// shutdown/save the game and close connections, if type == 0, don't save

	//Disconnect users
	for (int i = 0; i < MAXPLAYERS; ++i)
		PLAYERS[i]->closeConnection();

	//disconnect any observers
	for (int i = 0; i < MAXOBSERVERS; ++i) 		//for each occupied observer spot
		if (OBSERVERS[i])						//disconnect it if it exists
			OBSERVERS[i]->closeConnection();

	WSACleanup();		//deallocates WSA data
}
int CardGame::ready() {				// Returns true if the game is at capacity
	// check if there are still users in the game
	if (totalPlayers < 1)
		STATUS = 'Q';

	// return true if the game is full
	return totalPlayers == MAXPLAYERS;
}
