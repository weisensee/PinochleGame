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


	see DEFAULT_BUFLEN for default send/receive buffer size

*/
#define WIN32_LEAN_AND_MEAN
#pragma once
#include <winsock2.h>
#include <stdlib.h>
#include <string>

class player
{
public:
	player();
	player(SOCKET pSocket);
	player(std::string nName, SOCKET pSocket);
	~player();
	int send(const char * toSend);								// Sends char array to player
	int send(char * toSend, char * actionDescription);		// Sends char array to player, includes description in case error log must be written
	char * getStrAnswer();			// Gets string answer from client
	int getIntAnswer();				// Gets integer answer from client
	int getConnectionType();		// Returns what type of connection the client wants to initiate: database access or game play
	int getGameChoice();			// Returns which game the player wants to join or 0 if they wish to start a new one
	bool closeConnection();

	// Getters and Setters
	bool setName(std::string nName);	// sets passed in name to player's name, returns true if successful
	bool setName();						// queries user for name then sets their new name, returns true if successful
	std::string getName();				// returns player's name
	void setSocket(SOCKET nSocket);		// updates the players socket
	SOCKET getSocket();					// returns the players socket
private:
	static const int DEFAULT_BUFLEN = 512;
	static const char buffer[DEFAULT_BUFLEN];
	std::string name;					// Player's name
	SOCKET playerSocket;				// Player's socket
};