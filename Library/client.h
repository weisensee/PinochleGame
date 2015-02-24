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
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\message.h"		//message type library
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\serverException.h"		//exception library


class client: public message {
public:
	client::client() : client((SOCKET)NULL){}
	client::client(int nPort);						// Setup Client connection on given port
	client::client(SOCKET pSocket) : client("NONE", pSocket){}	// Constructs client with given socket and no name
	client::client(std::string nName, SOCKET pSocket);				//constructs client with arguments passed in


	~client();

	// SEND functions:
	int sendMFinal(unsigned char code, std::string* toSend);	// Sends char array to client, of message code: type
	int sendM(unsigned char code, char * toSend);				// Sends char array to client, of message code: type
	int sendM(unsigned char code, const char * toSend);			// Sends char array to client, of message code: type
	int sendM(unsigned char code, std::string* toSend);			// Sends char array to client, of message code: type
	int sendM(int code, std::string* toSend);	// Sends char array to client, of message code: type
	int sendM(unsigned char code);		// Sends char array to client, of message code: type
	int sendM(int n);					// Sends char array to client, includes description in case error log must be written

	// GET ANSWER functions:
	/* All "getAnswer" functions return an error/success status of the function.
		0 if receive error or if answer code doesn't match 'aCode'
		-1 on connection closure
		1 on success and if answer code matches 'aCode' */
	unsigned char getAnswerType();					// Returns the message code from the most recent packet received from the client, -1 if null
	int getStrAnswer(std::string * ans);			// Sets ans to string answer from client, returns 0 if receive error, and -1 on connection closure, 1 on success 
	int getStrAnswer(std::string * ans, unsigned char aCode);						// Sets ans to client's string response and checks ans type, returns 0 if receive error or if answer code doesn't match 'aCode', and -1 on connection closure, 1 on success and if answer code matches 'aCode'
	int getStrQueryAnswer(std::string * ans, unsigned char qCode);						// Sets ans to client's string response to message "qcode" and checks ans type, returns 0 if receive error, and -1 on connection closure, 1 on success 
	int getStrQueryAnswer(std::string * ans, unsigned char qCode, unsigned char aCode);	// Sets ans to client's string response to message "qcode" and checks ans type, returns 0 if receive error or if answer code doesn't match 'aCode', and -1 on connection closure, 1 on success and if answer code matches 'aCode'
	int getIntAnswer(int * ans);					// Gets integer answer from client, returns receive success code
	int getConnectionType();						// Returns what type of connection the client wants to initiate: database access or game play, 0 if answer not answered correctly
	bool closeConnection();

	// Getters and Setters
	std::string* generateMessage(unsigned char code, std::string *toSend);
	bool setName(std::string nName);	// sets passed in name to client's name, returns true if successful
	bool setName();						// queries user for name then sets their new name, returns true if successful
	std::string getName();				// returns client's name
	void setSocket(SOCKET nSocket);		// updates the clients socket
	SOCKET getSocket();					// returns the clients socket
private:
	int checkClientResponse(int result, unsigned char aCode);	// checks that correct response was received, considers receive success, then checks code of received message. if receive was successful, returns 1 if message type matched, 0 otherwise. if receive failed, returns failure code
	int setupClient(std::string nName, SOCKET pSocket);	//constructs client with arguments passed in
	static const int DEFAULT_BUFLEN = 1024;
	char buffer[DEFAULT_BUFLEN];
	std::string name;					// client's name
	SOCKET clientSocket;				// client's socket
	LogFile * pLog;
	unsigned char recent;				// Most recent message type
};