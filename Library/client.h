/*	Client.h -- Card Game Client
		Desktop Application
		Lucas Weisensee
		January 2015

	Stores info for current Client

	allows for:
	-connection info to be stored and monitored
	-packages to be sent
	-name 
	-device


	************************TO DO**********************
	--Implement connection closed error instead of repeated 'result' checks

*/
// DEBUGGING:
#define DEBUG true

#ifdef DEBUG
#define DEBUG_IF(cond) if(cond)
#else
#define DEBUG_IF(cond) if(false)
#endif

#define WIN32_LEAN_AND_MEAN
#pragma once
#include <winsock2.h>
#include <stdlib.h>
#include <string>
#include <list>
#include <atomic>
#include <time.h>
//#include "..\Library\LogFile.h"		//Log writing library
#include "..\Library\Message.h"		//Message type library
#include "..\Library\ServerException.h"		//exception library
#include "..\Library\Card.h"

static DWORD WINAPI launchListenThread(LPVOID* callingObj);		// creates a thread that listens on the socket for incoming packets


class Client: public Message {
public:
	// ::Client CONSTRUCTORS AND SETUP:: 
	Client() : Client((SOCKET)NULL){}
	Client(const Client &toCopy);
	Client(SOCKET pSocket) : Client("NONE", pSocket){}	// Constructs Client with given socket and no name
	Client(std::string nName, SOCKET pSocket);				//constructs Client with arguments passed in
	Client(std::string nName, SOCKET pSocket, int tInterval);	//constructs Client with arguments passed in, and given timeout Interval
	~Client();

	// ::SEND Message FUNCTIONS::
	int sendMFinal(unsigned char code, std::string* toSend);	// Sends char array to Client, of Message type: code
	int sendM(unsigned char code, std::string toSend);	// Sends char array to Client, of Message type: code
	int sendM(unsigned char code, char * toSend);				// Sends char array to Client, of Message type: code
	int sendM(unsigned char code, char toSend);					// Sends char Message to Client, of Message type: code
	int sendM(unsigned char code, int toSend);					// Sends int  Message to Client, of Message type: code
	int sendM(unsigned char code, const char * toSend);			// Sends char array to Client, of Message type: code
	int sendM(unsigned char code, std::string* toSend);			// Sends char array to Client, of Message type: code
	int sendM(int code, std::string* toSend);	// Sends char array to Client, of Message type: code
	int sendM(unsigned char code);		// Sends char array to Client, of Message type: code
	int sendM(int n);					// Sends char array to Client, includes description in case error log must be written

	// ::GET ANSWER FUNCTIONS::
	/* All "getAnswer" functions return an error/success status of the function.
			0 if receive error or if answer code doesn't match 'aCode'
			-1 on connection closure
			1 on success and if answer code matches 'aCode' 
		ALL "getAnswer" functions return only the answer from the Client
		*/
	Card* getNextPlay();			// return the Clients next Card play on the current trick
	unsigned char getAnswerType();	// Returns the Message code from the most recent packet received from the Client, -1 if null
	int getConnectionType();		// Returns what type of connection the Client wants to initiate: database access or game play, 0 if answer not answered correctly
	int receiveMessage();			// emulate recv function, pulling from inbox(std::list)
	int getStrAnswer(std::string * ans, unsigned char aCode);							// Sets ans to Client's string response and checks ans type, returns 0 if receive error or if answer code doesn't match 'aCode', and -1 on connection closure, 1 on success and if answer code matches 'aCode'
	int getStrQueryAnswer(std::string * ans, unsigned char qCode, unsigned char aCode);	// Sets ans to Client's string response to Message "qcode" and checks ans type, returns 0 if receive error or if answer code doesn't match 'aCode', and -1 on connection closure, 1 on success and if answer code matches 'aCode'
	int getIntAnswer(int * ans, unsigned char aCode);// Gets integer answer from Client, returns receive success code if answer type matches aCode type
	bool closeConnection();

	// ::REQUEST HANDLING::
	void checkInbox();	// checks the inbox for any requests and processes any pings
	void requestHandled(unsigned char code, bool success);	// notify Client of request handled and success
	void requestHandled(unsigned char code);				// notify Client of request handled	
	unsigned char getNextRequest();					// returns code of next request to process 
	bool hasRequests(bool* hasReq);					// returns true if there are requests to be handled, false otherwise
	void handleRequestFailure(unsigned char code);	// handles the server's failure to fulfill a specified request
	bool isRequest(unsigned char code);				// returns true if code is a request type that must be answered

	//:: GETTERS AND SETTERS::
	int getStatus();				// returns true if the Client has been active within the set time interval
	bool setUpdateEvent(WSAEVENT newEvent);	// sends in the update event to be signaled by the Client when updates are received from user
	bool setName(std::string nName);	// sets passed in name to Client's name, returns true if successful
	bool setName();						// queries user for name then sets their new name, returns true if successful
	std::string getName();				// returns Client's name
	int getPlayerPref();				// returns player # preference, or 0 if none
	void setPlayerPref(int n);			// updates player # preference
	void setSocket(SOCKET nSocket);		// updates the Clients socket
	SOCKET getSocket();					// returns the Clients socket

	std::string* generateMessage(unsigned char code, std::string *toSend);
	int checkClientResponse(unsigned char aCode);	// checks that correct response was received, considers receive success, then checks code of received Message. if receive was successful, returns 1 if Message type matched, 0 otherwise. if receive failed, returns failure code
	int setupClient(std::string nName, SOCKET pSocket);	//constructs Client with arguments passed in

	// GLOBAL DATA VALUES
	static const int DEFAULT_BUFLEN = 1024;
	std::string buffer;			// incoming Message buffer
	std::string name;			// Client's name
	SOCKET ClientSocket;		// Client's socket
	//LogFile * pLog;			// log file
	unsigned char recent;		// Most recent Message type
	int playerPref;				// players preferred position # (1-max)

	// ::INBOX AND REQUEST HISTORY::
	bool inboxHas(unsigned char aCode);				// returns true if a Message of type 'aCode' is in the inbox
	void printRequests();	// print out the requests list for debugging
	void printinbox();		// print out the Messages in the inbox
	int getInboxMessage(std::string * ans, unsigned char aCode);	// get's specified Message from inbox as if it were just received
	std::list<std::string> requests;	// lists of active requests
	std::list<std::string> inbox;		// lists of unreceived Messages

	// ::TIMEOUT AND ACTIVITY MONITORING::
	int localStatus;			// the status to return when queried for local status
	void handlePing();			// checks and handles a status request from server
	bool recentlyActive();		// returns true if the Client was active within a set time interval
	void updateStatus();		// checks in with and updates the current Client's status
	void updateStatus(int n);	// checks in with and updates the current Client's status
	double timeoutInterval;		// how long between contact with Client before server re-checks status
	time_t recentActivity;		// most recent time of contact with Client

	// ::MULTI_THREADING::
	bool hasUpdateEvent;			// true if the Client has an update event they should be signaling
	WSAEVENT newMessage;			// server's new Message event
	void signalNewMessage();		// updates the server of new Message arrival
	void listenForPacket();			// listens for packet on the Client's socket
	CRITICAL_SECTION inboxLock;		// Message inbox access lock
	CRITICAL_SECTION EventLock;		// Message inbox access lock
	std::atomic<int> ClientStatus;	// status of Client's connection
};