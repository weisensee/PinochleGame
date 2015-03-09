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
//****************************************************************************
// ::CONSTRUCTORS AND INITIALIZERS::
//****************************************************************************
cardGame::cardGame() {
}
cardGame::cardGame(gameSettings toMake):gameSettings(toMake) {			// constructs an active game loading settings from the game settings file passed in
	// Setup Game
	// Initiate log file
	//gLog = new LogFile("game log file_", "C:\\Users\\Pookey\\OneDrive\\Projects\\PinochleGame\\logs\\games");

	// game is setting up: 'S'
	STATUS = 'S';

	PLAYERS = new client*[MAXPLAYERS];
	OBSERVERS = new client*[MAXOBSERVERS];

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

	if (!InitializeCriticalSectionAndSpinCount(&playerCountLock, 0x00000400) ||	/* Check for any errors that  might occur*/
		!InitializeCriticalSectionAndSpinCount(&newPlayerListLock, 0x00000400) ||
		!InitializeCriticalSectionAndSpinCount(&gameIdLock, 0x00000400) ||
		!InitializeCriticalSectionAndSpinCount(&statusLock, 0x00000400) ||
		!InitializeCriticalSectionAndSpinCount(&infoStrLock, 0x00000400)) {
		perror("\nMutex Initiation error: ");		// report errors if any
		exit(0);
	}

	// *******************TESTING*******************
	std::cout << gType << std::endl;
	printf("Setting up %s a game of: %c for %i players and %i observers. The winning score will be: %i.", gameName.c_str(), gType, MAXPLAYERS, MAXOBSERVERS, GOAL);

	// Add player 1 to the game
	addPlayer(player1, 0);

}
cardGame::~cardGame() {				// destructor
	// release critical sections
	DeleteCriticalSection(&playerCountLock);
	DeleteCriticalSection(&newPlayerListLock);
	DeleteCriticalSection(&gameIdLock);
	DeleteCriticalSection(&statusLock);

	// release arrays of players and Observers
	if (PLAYERS)
		delete[] PLAYERS;

	if (OBSERVERS)
		delete[] OBSERVERS;

	//// delete log object
	//delete gLog;
}	
int cardGame::getNewId() {				// returns a new ID from the server for the current game
	srand(time(NULL));
	return rand();
}
//****************************************************************************
// ::SERVER COMMUNICATOR FUNCTIONS::
//****************************************************************************
std::string cardGame::getInfoString() {		//returns game information as string
	std::string temp;

	// Request ownership of infoStrLock critical section.
	EnterCriticalSection(&infoStrLock);


	// if the info string is out of date, rebuild it
	if (infoStringDirty) 
		buildInfoString();

	// copy the info string
	temp = infoString;
	
	// Release ownership of gameIdLock critical section.
	LeaveCriticalSection(&gameIdLock);

	// Return info string
	return temp;
}
void cardGame::buildInfoString() {		// rebuilds and sets the game info string, function must take place within critical sections to avoid race conditions
	// Request ownership of appropriate critical sections before copying them to send
	EnterCriticalSection(&statusLock);
	char tStatus = STATUS;
	LeaveCriticalSection(&statusLock);

	EnterCriticalSection(&gameIdLock);
	int tID = GAMEID;
	LeaveCriticalSection(&gameIdLock);

	EnterCriticalSection(&infoStrLock);
	std::string tName = gameName;
	LeaveCriticalSection(&infoStrLock);


	// set new info string
	infoString = tStatus;
	infoString += '^';
	infoString.append(std::to_string(tID));
	infoString += '^';
	infoString.append(tName);
	infoString += '^';
	infoString.append(PLAYERS[0]->getName());

	//append other player names if available
	if (totalPlayers > 1)
		for (int i = 1; i < totalPlayers; ++i)
			infoString.append('^' + PLAYERS[i]->getName());
}
int cardGame::getGameID() {		// Returns game ID
	// Request ownership of gameIdLock critical section.
	EnterCriticalSection(&gameIdLock);

	int ID = GAMEID;		// read value

	// Release ownership of gameIdLock critical section.
	LeaveCriticalSection(&gameIdLock);

	return ID;
}
int cardGame::playerCount() {			// returns current total players
	return totalPlayers;
}
bool cardGame::connectClient(client * toAdd) {	// adds new client to list of clients to add to game
	// Request ownership of newPlayerListLock critical section.
	EnterCriticalSection(&newPlayerListLock);

	// add new player to list
	newPlayers.push(*toAdd);

	// Release ownership of newPlayerListLock critical section.
	LeaveCriticalSection(&newPlayerListLock);

	return true;

}

//****************************************************************************
// ::GAME SETUP::
//****************************************************************************
bool cardGame::addPlayer(client * newPlayer, int n) {		//checks on and adds any new players
	// check that no player occupies desired spot n
	if (PLAYERS[n] != NULL)
		return makePlayer(newPlayer);

	// if the spot is unoccupied
	PLAYERS[n] = newPlayer;
	incrementPlayerCount();
	return true;
}
void cardGame::decrementPlayerCount() {				// decreases the player count by one
	// Request ownership of playerCountLock critical section.
	EnterCriticalSection(&playerCountLock);

	totalPlayers--;

	// Release ownership of playerCountLock critical section.
	LeaveCriticalSection(&playerCountLock);
}
void cardGame::incrementPlayerCount() {				// increases the player count by one
	// Request ownership of playerCountLock critical section.
	EnterCriticalSection(&playerCountLock);

	totalPlayers++;

	// Release ownership of playerCountLock critical section.
	LeaveCriticalSection(&playerCountLock);
}
void cardGame::decrementObserverCount() {				// decreases the player count by one
	// Request ownership of playerCountLock critical section.
	EnterCriticalSection(&playerCountLock);

	totalObservers--;

	// Release ownership of playerCountLock critical section.
	LeaveCriticalSection(&playerCountLock);
}
void cardGame::incrementObserverCount() {				// increases the player count by one
	// Request ownership of playerCountLock critical section.
	EnterCriticalSection(&playerCountLock);

	totalObservers++;

	// Release ownership of playerCountLock critical section.
	LeaveCriticalSection(&playerCountLock);

}
bool cardGame::makePlayer(client * player)	{	// adds player to current players, returns true if successful, false otherwise
	// add player to player list
	// get player position preference from player
	int newPos = player->getPlayerPref();
	int oldPos = -1;

	// if preference is valid, add them to position
	if (newPos > 0 && newPos < MAXPLAYERS) {
		// remove user currently occupying position
		client * otherPlayer = PLAYERS[newPos];
		if (otherPlayer != NULL)
			decrementPlayerCount();
		PLAYERS[newPos] = NULL;			// set new position to NULL

		// remove player from position in game
		oldPos = removeClient(player);		// removes client from game, decrementing appropriate counters and returning previous position

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
void cardGame::run() {			// starts the current game
	// Wait for game to be ready
	while (STATUS != 'R') {
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
int cardGame::removeClient(client * player) { 	// removes client from game, decrementing appropriate counters, returns previous position, 0 if not in game, 1-maxplayers if player, maxplayers-maxobservers if observer
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
void cardGame::sendToAll(unsigned char code) {		// send message of type code to all players and observers
	sendToAll(code, NULL);
}
void cardGame::sendToAll(unsigned char code, char msg) {	// send message of type code, with body of type msg to all players and observers
	// send message to all players
	for (int i = 0; i < MAXPLAYERS; ++i) 
		if (PLAYERS[i])
			PLAYERS[i]->sendM(code, msg);

	// send message to all observers
	for (int i = 0; i < MAXOBSERVERS; ++i)
		if (OBSERVERS[i])
			OBSERVERS[i]->sendM(code, msg);
}
bool cardGame::checkForConnections() {				// checks for new incoming connections
	printf("\nchecking for connections...");

	// Request ownership of newPlayerList critical section.
	EnterCriticalSection(&newPlayerListLock);

	// if new players have connected, add them to the game
	while (newPlayers.size() > 0) {
		// get next client
		client * toAdd = new client(newPlayers.front());	// copy over new client data

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

		newPlayers.pop();		// remove client from list
	}

	// Release ownership of newPlayerListLock critical section.
	LeaveCriticalSection(&newPlayerListLock);

	return true;
}
bool cardGame::checkCurrentConnections() {					// checks for requests from clients
	for (int i = 0; i < MAXPLAYERS; i++) {				// for each possible player
		if (PLAYERS[i] != NULL && PLAYERS[i]->getStatus() > 0)	// if there is a player currently connected
			checkForRequests(PLAYERS[i]);					// check and execute any commands
	}
	for (int i = 0; i < MAXOBSERVERS; i++) {			// for each possible player
		if (OBSERVERS[i] != NULL && OBSERVERS[i]->getStatus() > 0)	// if there is a player currently connected
			checkForRequests(OBSERVERS[i]);						// check and execute any commands
	}

	// return success
	return true;
}
bool cardGame::checkForRequests(client * player) {		// checks for and executes any commands from player
	// check for unhandled responses
	unsigned char request;
	bool hasReq = false;
	int result = player->hasRequests(&hasReq);

	// while the player is still connected and has requests to execute
	while(result > 0 && hasReq) {
		// get request from client
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
bool cardGame::handleRequest(client * player, unsigned char request) {	// handles request from client and notifies client of request status, returns true if success
	// execute appropriate response to request passed in as argument
	switch (request) {
	case R_PNAME:	// client requests player name
		player->sendM(G_STATUS, STATUS);
		player->requestHandled(G_STATUS);
		break;
	case G_LIST:	// client requests game status
		printf("\nUnhandled request:%u from player: %s", request, player->getName());
		break;
	case RESTART_G:	// client requests game status
		printf("\nUnhandled request:%u from player: %s", request, player->getName());
		break;
	case RESTART_ANS:	// client requests game status
		printf("\nUnhandled request:%u from player: %s", request, player->getName());
		break;
	case N_GQUERY:	// client requests game status
		printf("\nUnhandled request:%u from player: %s", request, player->getName());
		break;
	case N_GINFO:
		buildInfoString();
		player->sendM(G_STATUS, &infoString);
		player->requestHandled(N_GINFO);
		break;
	case PING_QRY:	// client requests connection status
		player->sendM(PING_ANS);			// send connected signal
		player->requestHandled(PING_QRY);
		break;
	case STAT_QUERY:	// client requests game status
		player->sendM(G_STATUS, STATUS);
		player->requestHandled(G_STATUS);
		break;
	case BEC_PLAYER:
		player->requestHandled(BEC_PLAYER, makePlayer(player));	// send the result of the makePlayer command
		break;
	case BEC_OBS:
		player->requestHandled(BEC_OBS, makePlayer(player));	// send the result of the makePlayer command
		break;
	case REQ_PLAY:
		printf("\nUnhandled request:%u from player: %s", request, player->getName());
		break;
	case PLY_CARD:
		printf("\nUnhandled request:%u from player: %s", request, player->getName());
		break;
	default:
		printf("\nUnhandled request:%u from player: %s", request, player->getName());
		player->requestHandled(request);
		break;
	}

	return true;
}
/*void cardGame::echo(client * curPlayer) {		// echoes and talks to current player
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
//}	*/

//****************************************************************************
// ::GAMEPLAY::
//****************************************************************************
void cardGame::manageEuchreGamePlay() {		// plays a euchre round
	// check player count
	if (MAXPLAYERS != 4) {
		printf("\nMax Players Error, MAXPLAYERS: %d", MAXPLAYERS);
		return;
	}		

	// Play Euchre
	while (!gameOver) {
		euchreDealCards();
		euchreBiddingPhase();
		euchreTricksPhase();
		publishReport();
	}
}
void cardGame::euchreDealCards() {		// deal out euchre hand to each player
	// shuffle and deal out cards
	eRound = new euchreRound();

	// send dealt hand to each player
	for (int i = 0; i < MAXPLAYERS; i++)					// for each player
		PLAYERS[i]->sendM(HAND_DEALT, eRound->handDealt(i));	// send hand dealt
}
void cardGame::euchreBiddingPhase() {	// manages Euchre bidding phase
	bool trumpSet = false;
	while (!trumpSet) {
		// flip over the trump card so each player can see
		card top = eRound->flipKitty();

		// If the stack was empty, re-deal 
		if (top.value == 0) {
			euchreDealCards();	// re-deal
			continue;			// restart while loop
		}

		// give each player the chance to call trump, in clockwise order starting with the dealer
		sendToAll(KITTY_FLP, top.chr());		// notify all players of trump card being flipped
		int ans;
		for (int i = 0; i < MAXPLAYERS; i++) {
			int result = PLAYERS[(i + dealer) % MAXPLAYERS]->getIntAnswer(&ans, ORDER_UP);	// get answer from player and ensure correct message was received
			if (result > 0) {
				if (ans > 0) {
					orderUp(top, (i + dealer) % MAXPLAYERS);	// sets trump, makers and notifies all players that player n ordered up card top
					i += MAXPLAYERS;							// stop asking players if they want to order up
					trumpSet = true;
				}
				else
					sendToAll(TRMP_ORD, 0);		// notify other players the current player passed, order up = pass or 'false' or 0
			}
			else
				printf("\nPlayer connection error with player: %d, result: %d", (i + dealer) % MAXPLAYERS, result);
		}
		// If no one bids flip over the next card on next loop through while()		
	}
}
void cardGame::euchreTricksPhase() {	// manages Euchre trick playing phase
	int hasLead = dealer;
	card* recent;
	bool legal;
	// When all tricks have been played, quit
	while (eRound->inPlay()) {
		// Get first card, The dealer goes first
		// gather one trick
		for (int i = 0; i < MAXPLAYERS; i++) {						// for each player, starting with the trick leader
			// get/check current players play
			legal = false;
			while (!legal) {										// while they haven't played a legal card
				recent = PLAYERS[hasLead + i]->getNextPlay();		// request play and verify legality
				legal = eRound->playCard(*recent);					// if play was legal, move to next player				
			}
			// update other players of card recently played
			sendToAll(PLY_CARD, recent->chr());
		}
		// Whoever wins the trick, goes first next trick
		hasLead = eRound->wonLastTrick();
	}
}
void cardGame::orderUp(card top, int n) {	// sets trump, makers and notifies all players that player n ordered up card top
	// set trump makers
	eRound->orderUp(n);

	// notify all players and observers
	sendToAll(TRMP_ORD, 1);

	// check if anyone wants to go it alone?? ***********************************
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
		PLAYERS[i]->getIntAnswer(&temp, RESTART_ANS);
		ans += temp;
	}

	return (totalPlayers == ans);				// return group decision
}
void cardGame::quit(int type) {		// shutdown/save the game and close connections, if type == 0, don't save

	//Disconnect users
	for (int i = 0; i < MAXPLAYERS; ++i)
		PLAYERS[i]->closeConnection();

	//disconnect any observers
	for (int i = 0; i < MAXOBSERVERS; ++i) 		//for each occupied observer spot
		if (OBSERVERS[i])						//disconnect it if it exists
			OBSERVERS[i]->closeConnection();

	WSACleanup();		//deallocates WSA data
}
int cardGame::ready() {				// Returns true if the game is at capacity
	// check if there are still users in the game
	if (totalPlayers < 1)
		STATUS = 'Q';

	// return true if the game is full
	return totalPlayers == MAXPLAYERS;
}
