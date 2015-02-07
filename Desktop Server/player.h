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
	bool closeConnection();

	// Getters and Setters
	void setName(std::string nName);
	std::string getName();
	void setSocket(SOCKET nSocket);
	SOCKET getSocket();
private:
	static const int DEFAULT_BUFLEN = 512;
	static const char buffer[DEFAULT_BUFLEN];
	std::string name;
	SOCKET playerSocket;
};