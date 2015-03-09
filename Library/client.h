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


	************************TO DO**********************
	--Implement connection closed error instead of repeated 'result' checks

*/
#define WIN32_LEAN_AND_MEAN
#pragma once
#include <winsock2.h>
#include <stdlib.h>
#include <string>
#include <list>
#include <atomic>
#include <time.h>
//#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\LogFile.h"		//Log writing library
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\message.h"		//message type library
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\serverException.h"		//exception library
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\card.h"

static DWORD WINAPI launchListenThread(LPVOID* callingObj);		// creates a thread that listens on the socket for incoming packets


class client: public message {
public:
	// ::CLIENT CONSTRUCTORS AND SETUP:: 
	client() : client((SOCKET)NULL){}
	client(const client &toCopy);
	client(SOCKET pSocket) : client("NONE", pSocket){}	// Constructs client with given socket and no name
	client(std::string nName, SOCKET pSocket);				//constructs client with arguments passed in
	client(std::string nName, SOCKET pSocket, int tInterval);	//constructs client with arguments passed in, and given timeout Interval
	~client();

	// ::SEND MESSAGE FUNCTIONS::
	int sendMFinal(unsigned char code, std::string* toSend);	// Sends char array to client, of message type: code
	int sendM(unsigned char code, char * toSend);				// Sends char array to client, of message type: code
	int sendM(unsigned char code, char toSend);					// Sends char message to client, of message type: code
	int sendM(unsigned char code, int toSend);					// Sends int  message to client, of message type: code
	int sendM(unsigned char code, const char * toSend);			// Sends char array to client, of message type: code
	int sendM(unsigned char code, std::string* toSend);			// Sends char array to client, of message type: code
	int sendM(int code, std::string* toSend);	// Sends char array to client, of message type: code
	int sendM(unsigned char code);		// Sends char array to client, of message type: code
	int sendM(int n);					// Sends char array to client, includes description in case error log must be written

	// ::GET ANSWER FUNCTIONS::
	/* All "getAnswer" functions return an error/success status of the function.
			0 if receive error or if answer code doesn't match 'aCode'
			-1 on connection closure
			1 on success and if answer code matches 'aCode' 
		ALL "getAnswer" functions return only the answer from the client
		*/
	int getStatus();				// returns true if the client has been active within the set time interval
	card* getNextPlay();			// return the clients next card play on the current trick
	unsigned char getAnswerType();	// Returns the message code from the most recent packet received from the client, -1 if null
	int getConnectionType();		// Returns what type of connection the client wants to initiate: database access or game play, 0 if answer not answered correctly
	int receiveMessage();			// emulate recv function, pulling from inbox(std::list)
	int getStrAnswer(std::string * ans, unsigned char aCode);							// Sets ans to client's string response and checks ans type, returns 0 if receive error or if answer code doesn't match 'aCode', and -1 on connection closure, 1 on success and if answer code matches 'aCode'
	int getStrQueryAnswer(std::string * ans, unsigned char qCode, unsigned char aCode);	// Sets ans to client's string response to message "qcode" and checks ans type, returns 0 if receive error or if answer code doesn't match 'aCode', and -1 on connection closure, 1 on success and if answer code matches 'aCode'
	int getIntAnswer(int * ans, unsigned char aCode);// Gets integer answer from client, returns receive success code if answer type matches aCode type
	bool closeConnection();

	// ::REQUEST HANDLING::
	void requestHandled(unsigned char code, bool success);	// notify client of request handled and success
	void requestHandled(unsigned char code);				// notify client of request handled	
	unsigned char getNextRequest();					// returns code of next request to process 
	bool hasRequests(bool* hasReq);					// returns true if there are requests to be handled, false otherwise
	void handleRequestFailure(unsigned char code);	// handles the server's failure to fulfill a specified request
	bool isRequest(unsigned char code);				// returns true if code is a request type that must be answered

	//:: GETTERS AND SETTERS::
	bool setName(std::string nName);	// sets passed in name to client's name, returns true if successful
	bool setName();						// queries user for name then sets their new name, returns true if successful
	std::string getName();				// returns client's name
	int getPlayerPref();				// returns player # preference, or 0 if none
	void setPlayerPref(int n);			// updates player # preference
	void setSocket(SOCKET nSocket);		// updates the clients socket
	SOCKET getSocket();					// returns the clients socket

	std::string* generateMessage(unsigned char code, std::string *toSend);
	int checkClientResponse(unsigned char aCode);	// checks that correct response was received, considers receive success, then checks code of received message. if receive was successful, returns 1 if message type matched, 0 otherwise. if receive failed, returns failure code
	int setupClient(std::string nName, SOCKET pSocket);	//constructs client with arguments passed in

	// GLOBAL DATA VALUES
	static const int DEFAULT_BUFLEN = 1024;
	std::string buffer;			// incoming message buffer
	std::string name;			// client's name
	SOCKET clientSocket;		// client's socket
	//LogFile * pLog;			// log file
	unsigned char recent;		// Most recent message type
	char playerPref;			// players preferred position # (1-max)

	// ::INBOX AND REQUEST HISTORY::
	bool inboxHas(unsigned char aCode);				// returns true if a message of type 'aCode' is in the inbox
	int getInboxMessage(std::string * ans, unsigned char aCode);	// get's specified message from inbox as if it were just received
	std::list<std::string> requests;	// lists of active requests
	std::list<std::string> inbox;		// lists of unreceived messages

	// ::TIMEOUT AND ACTIVITY MONITORING::
	int localStatus;			// the status to return when queried for local status
	void handlePing();			// checks and handles a status request from server
	bool recentlyActive();		// returns true if the client was active within a set time interval
	void updateStatus();		// checks in with and updates the current client's status
	void updateStatus(int n);	// checks in with and updates the current client's status
	double timeoutInterval;		// how long between contact with client before server re-checks status
	time_t recentActivity;		// most recent time of contact with client

	// ::MULTI_THREADING::
	void listenForPacket();			// listens for packet on the client's socket
	CRITICAL_SECTION inboxLock;	// message inbox access lock
	std::atomic<int> clientStatus;	// status of client's connection
};