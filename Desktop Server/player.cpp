/*	player.h -- Card Game Player
		Desktop Application
		Lucas Weisensee
		January 2015

	Stores info for current player

	allows for:
	-connection info to be stored and monitored
	-packages to be sent
	-name 
	-device

	Data:
	string name;
	SOCKET playerSocket;

*/

#include "player.h"


player::player()
{}

player::player(string nName, SOCKET pSocket) {	//constructs player with arguments passed in
	name = nName;
	playerSocket = pSocket;
}

player::~player() {
	// Should we close the socket connection here?
}

bool player::send(char * toSend) {	// Sends the char string to client

}

void player::setName(string nName) { // Sets player's name to argument passed in
	name = nName;
}

string player::getName() { 	// Returns the player's name
	return name;
}

void player::setSocket(SOCKET nSocket) {	// Sets the players socket to argument passed in
	SOCKET = nSocket;
}

bool player::closeConnection() 
{
	//disconnect client "current"
	int result = shutdown(current, SD_SEND);
	if (result == SOCKET_ERROR) {						//quit on shutdown error
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(current);
		WSACleanup();
		return 1;			

		//  "" SHOULD BE THROWING AN ERROR ""
	closesocket(playerSockets[i])
}
