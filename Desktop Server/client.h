/*	client.h -- Card Game client
		Desktop Application
		Lucas Weisensee
		January 2015

	Stores info for current client

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
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\LogFile.h"		//Log writing library


class client {
public:
	client::client() : client((SOCKET)NULL){}
	client::client(SOCKET pSocket) : client("NONE", pSocket){}	// Constructs client with given socket and no name
	client::client(std::string nName, SOCKET pSocket);				//constructs client with arguments passed in


	~client();

	// SEND functions:
	int sendMFinal(unsigned char * code, std::string* toSend);	// Sends char array to client, of message code: type
	int sendM(unsigned char * code, char * toSend);			// Sends char array to client, of message code: type
	int sendM(unsigned char * code, const char * toSend);	// Sends char array to client, of message code: type
	int sendM(unsigned char * code, std::string* toSend);	// Sends char array to client, of message code: type
	int sendM(int code, std::string* toSend);	// Sends char array to client, of message code: type
	int sendM(unsigned char * code);	// Sends char array to client, of message code: type
	int sendM(unsigned char code);	// Sends char array to client, of message code: type
	int sendM(int n);					// Sends char array to client, includes description in case error log must be written

	// GET ANSWER functions:
	int getAnswerType();					// Returns the message code from the most recent packet received from the client, -1 if null
	std::string * getStrAnswer();			// Gets string answer from client
	std::string * getStrAnswer(int n);		// Gets string answer from client
	int getIntAnswer();				// Gets integer answer from client
	int getConnectionType();		// Returns what type of connection the client wants to initiate: database access or game play
	bool closeConnection();

	// Getters and Setters
	std::string* generateMessage(unsigned char code, std::string *toSend);
	bool setName(std::string nName);	// sets passed in name to client's name, returns true if successful
	bool setName();						// queries user for name then sets their new name, returns true if successful
	std::string getName();				// returns client's name
	void setSocket(SOCKET nSocket);		// updates the clients socket
	SOCKET getSocket();					// returns the clients socket
private:
	static const int DEFAULT_BUFLEN = 512;
	char buffer[DEFAULT_BUFLEN];
	std::string name;					// client's name
	SOCKET clientSocket;				// client's socket
	LogFile * pLog;
};