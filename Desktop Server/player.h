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

*/

#include <winsock2.h>
#include <stdlib.h>
#include <string>

class player
{
public:
	player();
	player(std::string nName, SOCKET pSocket);
	~player();
	bool send(char * toSend);
	bool closeConnection();

	// Getters and Setters
	void setName(std::string nName);
	std::string getName();
	void setSocket(SOCKET nSocket);
private:
	std::string name;
	SOCKET playerSocket;
};