/*	Live Game Manager -- Pinochle Game
		Desktop Application
		Lucas Weisensee
		November 2014

	This program will function as a pinochle game manager. It will:
		-manage connections to all current playersa and observers
		-keep track of the current game
		-keep track of the current round
		-enforce turn rules/turn order
		-quit and save game when finished
*/

#include "gameRecord.h"

void prepforstart(game, playerSockets, observerSockets);		//manages the waiting/setup process
void addnewplayer(game, playerSockets);			//checks on and adds any new players
void checkplayercommand(game, playerSockets);	//check with and execute player commands
void manageGamePlay(game, playerSockets, observerSockets);
void biddingPhase(game, playerSockets);			//manages entire bidding phase
void meldingPhase(game, playerSockets);			//manages entire melding phase
void tricksPhase(game, playerSockets);			//manages entire trick taking phase
void publishReport(game, playerSockets);		//totals scores and publishes to player
void diconnectPlayers(playerSockets);			//disconnect all network connections
void shutdownGame(SOCKETS playerSockets, SOCKETS observerSockets, int players, int observers);	//manage shutdown of the game and connections


//starts new game thread with arguments [int: number of players] [char player 0 name] [player 0 socket] [observer limit]
int main(int argc, char const *argv[])
{
	//initialize WSA

	//Initiate new game setup

	OBSERVER_LIMIT = argv[4];
	SOCKET observerSockets[OBSERVER_LIMIT];				//create observer socket array
	gameRecord * game = new game(argv[1], argv[2]);		//create new game
	SOCKET observerSockets[OBSERVER_LIMIT];				//create observer socket array
	SOCKET playerSockets[argv[1]];						//create player socket array
	playerSockets[0] = argv[3];							//save player 0's socket

	//Connect players/wait for start
	//wait until the game is ready and players are ready to start
	//game.ready() returns -1 if all players have left.
	prepforstart(game, playerSockets, observerSockets);

	//play games until users want to quit
	do {
		//Manage play
		manageGamePlay(game, playerSockets, observerSockets)

		//Save game to archives
		game.save();

	} while(!game.restart());	//Play another game if users want

	shutdownGame(playerSockets, observerSockets, argv[1], observers);

	return 0;
}

void prepforstart(game, playerSockets, observerSockets)		//manages the waiting/setup process
{
	while(game.ready() > 0) {						//while the game is not ready or empty
		addnewplayer(game, playerSockets);			//check for new players to add to game
		checkplayercommand(game, playerSockets);	//check for player requests
	}

	if (game.ready() < 0)					//if all players have left
	{
		game.quit(0)						//quit without saving
		delete game;						//deallocate memory
		diconnectPlayers(playerSockets);	//disconnect all network connections
		delete [] playerSockets;
		return 0;							//exit process
	}
}


void manageGamePlay(game, playerSockets, observerSockets)
{
	while(game.continue()) {
		biddingPhase(game, playerSockets);
		meldingPhase(game, playerSockets);
		tricksPhase(game, playerSockets);
		publishReport(game, playerSockets);
	}
}

void manageGamePlay(game, playerSockets, observerSockets)
{

}

void biddingPhase(game, playerSockets)			//manages entire bidding phase
{

}

void meldingPhase(game, playerSockets)			//manages entire melding phase
{

}

void tricksPhase(game, playerSockets)			//manages entire trick taking phase
{

}

void publishReport(game, playerSockets)		//totals scores and publishes to player
{

}

//manage shutdown of the game and connections
void shutdownGame(SOCKETS playerSockets, SOCKETS observerSockets, int players, int observers)
{
	//Disconnect users
	for (int i = 0; i < players; ++i)
		disconnect(playerSockets[i]);

	//disconnect any observers
	if(observers) {
		for (int i = 0; i < observers; ++i) 			//for each occupied observer spot
			if (observerSockets[i])					//disconnect
				disconnect(observerSockets[i]);
	}

	WSACleanup();		//deallocates WSA data
}


void disconnect(SOCKET * current, n) 
{
	for (int i = 0; i < n; ++i)
		disconnectSocket(current[n]);
}

int disconnect(SOCKET * current) 
{
	//disconnect client "current"
	int result = shutdown(current, SD_SEND);
	if (result == SOCKET_ERROR) {						//quit on shutdown error
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(current);
		WSACleanup();
		return 1;										//*************// SHOULD BE THROWING AN ERROR \\*********

	closesocket(playerSockets[i])
}

