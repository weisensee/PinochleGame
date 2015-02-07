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

player(SOCKET pSocket)	{		// Constructs player with given socket and no name
	name = "NONE";
	playerSocket = pSocket;

player::player(string nName, SOCKET pSocket) {	//constructs player with arguments passed in
	name = nName;
	playerSocket = pSocket;
}

player::~player() {
	// Should we close the socket connection here?
}

int player::send(char * toSend) {	// Sends the char string to client {
	return send(toSend, NULL);
}

int player::send(char * toSend, char * actionDescription) {		// Sends char array to player, includes description in case error log must be written

	int result = send(ClientSocket, toSend, strlen(toSend), 0);		//send list

	if (result < 0)
		writetolog("Send Error: " + actionDescription, WSAGetLastError());	//write to log if receive failed

	if (result > 0)	{							//if the sending was successful
		
	}
	else						//write error to log if sending failed
		writetolog("error, sendGameList result < 1 ", WSAGetLastError());

}

int player::getConnectionType() {		// Returns what type of connection the client wants to initiate: database access or game play

}

char * player::getStrAnswer() {			// Gets string answer from client
	// Get result from player, store in buffer
	int result = recv(ClientSocket, buffer, DEFAULT_BUFLEN, 0);	

	// Check receive success
	if (result < 1)
		writetolog("Send Error: " + actionDescription, WSAGetLastError());	//write to log if receive failed


}
int player::getIntAnswer();				// Gets integer answer from client

void player::setName(string nName) { // Sets player's name to argument passed in
	name = nName;
}

string player::getName() { 	// Returns the player's name
	return name;
}

void player::setSocket(SOCKET nSocket) {	// Sets the players socket to argument passed in
	SOCKET = nSocket;
}

SOCKET player::getSocket() {	// Returns the players socket
	return playerSocket;
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
