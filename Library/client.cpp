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

	Data:
	string name;
	SOCKET clientSocket;

*/

#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\client.h"

// ::SETTINGS::
double DEFAULT_TIMEOUT = 4000;	// default amount of time between messages before server rechecks client's status
int MAX_ATTEMPTS = 5;			// number of times to try and receive the correct message type


// **************************************************************
// codeMatcher predicate helper class for list search
//***************************************************************
unsigned char predicateCode;
struct codeMatcher {		// returns true if toMatch matches the global code: 'predicateCode' which must be set before hand, helper function for list removal
public:
	bool operator() (std::string toMatch) {
		//printf("\n comparing two chars, 1: %c and 2: %c", (unsigned char)toMatch[0], predicateCode);
		//if ((unsigned char)toMatch[0] == predicateCode)
		//	printf("\nAnd it was true!");
		return ((unsigned char)toMatch.at(0) == predicateCode);
	}
	void operator= (unsigned char code) {
		predicateCode = code;
	}
};
// **************************************************************
//			CLIENT CONSTRUCTORS AND SETUP
//***************************************************************
client::client(const client &toCopy) : client(toCopy){
	// copy out atomic value (...atomically)
	clientStatus = toCopy.clientStatus.load();
}
client::client(std::string nName, SOCKET pSocket, int tInterval) : client(nName, pSocket) {	//constructs client with arguments passed in
	// perform normal construction
	
	// set default timeout interval
	DEFAULT_TIMEOUT = tInterval;
}
client::client(std::string nName, SOCKET pSocket) {	//constructs client with arguments passed in
	setupClient(nName, pSocket);

	// initialize most recent message type
	recent = (unsigned char) 111;
}
client::~client() {
	// Should we close the socket connection here? yes.
	DeleteCriticalSection(&inboxLock);

}
int client::setupClient(std::string nName, SOCKET pSocket) {	//constructs client with arguments passed in
	// initialize log file
	//pLog = new LogFile("client log file_", "C:\\Users\\Pookey\\OneDrive\\Projects\\PinochleGame\\logs\\clients");
	clientSocket = pSocket;
	
	// set client name
	setName(nName);

	//std::string temp = "Constructor, name: " + name;
	//pLog->writetolog(&temp);

	// initialize predicate code
	predicateCode = (unsigned char)0;

	// zero out player position preference
	playerPref = (char)0;

	// set status to active
	clientStatus = 1;
	localStatus = 1;

	// amount of time between requests before server rechecks client's status
	timeoutInterval = DEFAULT_TIMEOUT;

	// Launch listening thread to listen on the incoming connection
	InitializeCriticalSectionAndSpinCount(&inboxLock, 0x00000200);
	CreateThread(
		NULL,             			// use default security attributes
		0,          				// use default stack size  
		(LPTHREAD_START_ROUTINE)launchListenThread,	// thread function name
		(void*)this,		// argument to thread function 
		0,					// use default creation flags 
		0);		// returns the thread identifier 

	// return success if no errors
	return true;
}
// **************************************************************
// ::SEND MESSAGE FUNCTIONS::
//***************************************************************
int client::sendMFinal(unsigned char code, std::string* toSend) {	// Sends char array to client, of message code: type, returns false if send not successful
	// Verify client status was functional on last connection
	if (clientStatus < 1) {
		printf("\nERROR: sending message when client status is already: %i", clientStatus);
		return clientStatus;
	}
	// if the client had functional status before
	else {
		// Generate message string:
		std::string * message = generateMessage(code, toSend);

		// Send message and update connection clientStatus
		int result = send(clientSocket, message->c_str(), message->length(), 0);		//send list

		// Convert to string
		if (result == SOCKET_ERROR) {		// if send failed
			printf("Send Error: %i, message type: %u, message: %s", result, code, toSend->c_str());
			clientStatus = result;		// update client status
		}
		return result;
	}
}
int client::sendM(int n) {		// Sends char array to client, includes description in case error log must be written
	if (n > 256)
		printf("Send error(char overflow):");
	return sendM((unsigned char)n);

}
int client::sendM(unsigned char code) {	// Sends message corresponding to code to client
	std::string* temp = new std::string("");
	return sendM(code, temp);				// send code with empty string as message body
}
int client::sendM(unsigned char code, int toSend) {					// Sends int  message to client, of message type: code
	// convert int to string
	std::string temp;
	char buf[32];
	itoa(toSend, buf, 10);
	temp.assign(buf);

	// send string to client
	return sendMFinal(code, &temp);	// call main send message function
}
int client::sendM(unsigned char code, char toSend) {		// Sends char message to client, of message type: code
	std::string temp;
	temp.assign(1, toSend);		//copy to send into string
	//temp.append("\0"); 

	printf("\nsending char: %c",temp.c_str());


	return sendMFinal(code, &temp);	// call main send message function
}
int client::sendM(unsigned char code, const char * toSend) {	// Sends char array to client, includes description in case error log must be written
	std::string temp = toSend;
	return sendM(code, &temp);
}
int client::sendM(unsigned char code, std::string* toSend) {	// Sends char array to client, of message code: type
	return sendMFinal(code, toSend);
}
int client::sendM(unsigned char code, char * toSend) {		// Sends char array to client, includes description in case error log must be written
	std::string temp = toSend;
	return sendM(code, &temp);
}
int client::sendM(int code, std::string* toSend) {	// Sends char array to client, of message code: type
	if (code > 256)
		printf("Send error(char overflow):");
	unsigned char temp = (unsigned char)code;
	return sendMFinal(temp, toSend);
}
std::string* client::generateMessage(unsigned char code, std::string *toSend) {
	// Insert message code appropriately
	toSend->insert(0, (const char*)&code);

	// append null terminating character
	toSend->append(1, '\0');

	// Return complete message
	return toSend;
}
int client::getConnectionType() {		// Returns what type of connection the client wants to initiate: database access or game play
	// send connection type request
	int result = sendM(S_STATUS);

	// if send failed, return error
	if (!result)
		return result;

	// otherwise return answer
	else {
		std::string ans;
		result = getStrAnswer(&ans, CON_TYPE);		// get answer from client
		if (result > 0)					// check that answer was received successfully
				return buffer.at(1);	// return requested connection type
		// if answer was not received successfully
		else
			return result;	// return error type
	}
}
// **************************************************************
// ::GET ANSWER FUNCTIONS::
//***************************************************************
static DWORD WINAPI launchListenThread(LPVOID* callingObj) {		// creates a thread that listens on the socket for incoming packets
	// cast *this from parameter
	client* self = (client*)callingObj;

	// launch listening function
	self->listenForPacket();

	// return some value
	return 1;
}
void client::listenForPacket() {			// listens for packet on the client's socket
	std::string * temp = new std::string();

	// Setup event to listen for
	WSAEVENT readEvent[1];
	readEvent[0] = WSACreateEvent();
	int result = WSAEventSelect(clientSocket, readEvent[0], FD_READ);

	// check that event was created successfully
	if (result == SOCKET_ERROR) {
		printf("\nERROR: socket error in listenforpacket: WSAEventSelect: %i", WSAGetLastError());
		exit(0);
	}
	char buf[256];

	// listen indefinitely
	while (true) {
		// Listen for message on socket connection
		result = WSAWaitForMultipleEvents(
			1,			// wait for one event
			readEvent,	// the event to wait for
			false,		// don't wait for all the events
			100000,	// wait forever	WSA_INFINITE
			false);		// unnecessary - some kind of I/O completion stat
			
		printf("\nWSAWaitForMultipleEvents result; %d", result);

		// if the listening failed
		if (result == WSA_WAIT_FAILED) {
			printf("\nWSA_WAIT_FAILED, clientStatus: %i, error: %i", clientStatus.load(), WSAGetLastError());
			exit(0);
		}
		if (result >= 0) {
			// if the event was successfully waited for, get packet
			result = recv(clientSocket, buf, DEFAULT_BUFLEN, 0);

			if (result > 0) {
				/// add to list
				printf("\nreceived: %s", buf);
				temp->assign(buf);
				inbox.push_back(*temp);		// add to list
			}
			// if recv did not succeed
			else if (result < 0) {
				printf("\nRecv failure, buffer %s, clientStatus: %i, error: %i", buf, clientStatus.load(), WSAGetLastError());
			}
		}

		if (!WSAResetEvent(readEvent[0]))
			printf("\nWSAResetEvent failure, error: %i", WSAGetLastError());

	}

	delete temp;
	closesocket(clientSocket);
	ExitThread(0);
}
void client::handlePing() {	// responds to client status request 
	// remove ping messages from inbox
	predicateCode = PING_QRY;
	inbox.remove_if(codeMatcher());
	
	// respond to ping request
	sendM(PING_ANS, localStatus);
}
unsigned char client::getAnswerType() {		// Returns the message code from the most recent packet received from the client, -1 if null
	if (buffer.length() > 0)
		return recent;
	else
		return -1;
}
int client::getStrAnswer(std::string * ans, unsigned char aCode) {	// Sets ans to client's string response and checks ans type, returns 0 if receive error or if answer code doesn't match 'aCode', and -1 on connection closure, 1 on success and if answer code matches 'aCode'
	// If specific answer is requested, check if it has already been received	
	// check that connection is functioning and the message desired is in the inbox
	//*************TESTING::*************
	std::cout << "\n";
	while (clientStatus > 0 && !inboxHas(aCode)) {
		std::cout << "..0..";

		// rest for a little
		Sleep(500);
	}

	std::cout << "\nclientStatus: " << clientStatus << " clientStatus.load(): " << clientStatus.load();

	// update local timestamp of recent activity
	time(&recentActivity);

	return getInboxMessage(ans, aCode);	// return received message if it's in the inbox
}
int client::getStrQueryAnswer(std::string * ans, unsigned char qCode, unsigned char aCode){	// Sets ans to client's string response to message "qcode" and checks ans type, returns 0 if receive error or if answer code doesn't match 'aCode', and -1 on connection closure, 1 on success and if answer code matches 'aCode'
	// send query
	int result = sendM(qCode);

	// if query was received successfully, get answer
	if (result > 0)
		return getStrAnswer(ans,aCode);

	// if query was not received successfully
	return result;	
}
int client::getIntAnswer(int * ans, unsigned char aCode) {	// Gets integer answer from client, returns receive success code if answer type matches aCode type
	std::string *temp = new std::string();
	int result = getStrAnswer(temp, aCode);

	// check receive success
	if (result > 0) {
		//Convert to Int and save answer to arg passed in

		//*************************INVALID ARGUMENT ERROR:: (when client quit)**********************
		printf("\nTemp: %s", temp->c_str());
		*ans = std::stoi(*temp, NULL, NULL);

		// return success
		return 1;
	}
	// if receive failed
	else
		return result;
}
int client::checkClientResponse(unsigned char aCode) {	// checks that correct response was received, considers receive success, then checks code of received message. if receive was successful, returns 1 if message type matched, 0 otherwise. if receive failed, returns failure code
	// Check that connection is still functioning
	if (clientStatus < 1){
		printf("\nClient connection error and in checking response: %u", aCode);
		return false;
	}
	
	// if aCode matches received code return true
	printf("\nChecking client response: %u against code: %u", (unsigned char)buffer.at(0), aCode);
	if (aCode == (unsigned char)buffer.at(0)) {
		printf("\nThe response matched");
		return true;
	}
	// if the response didn't match the required response type
	else {
		printf("\nThe response didn't Match!");

		// add response to unReported message list (inbox)
		inbox.push_back(buffer);
		return false;
	}

}
card* client::getNextPlay() {			// return the clients next card play on the current trick
	// query for card play and get card answer
	std::string ans;
	int result = getStrQueryAnswer(&ans, REQ_PLAY, PLY_CARD);

	// parse to card
	card * temp = new card(ans.at(0));
	return temp;
}
// **************************************************************
//			CLIENT REQUEST HANDLING
//***************************************************************
int client::getStatus() {	// returns true if the client has been active within the set time interval
	// if server hasn't heard from client for a while, check if still active
	if (!recentlyActive())
		// check if player is still active
		updateStatus();

	return clientStatus;
}
bool client::isRequest(unsigned char code) {	// returns true if code is a request type that must be answered
	bool temp = rLibrary[code];
	if (temp)
		printf("\ncode: %u is TRUE in library", code);
	else 
		printf("\ncode: %u is FALSE in library", code);

	return rLibrary[code];
}
void client::requestHandled(unsigned char code, bool success) {	// notify client of request handled and success
	if (success)					// if the request was handled successfully
		requestHandled(code);		// remove all that match from list
	else
		handleRequestFailure(code);
}
void client::requestHandled(unsigned char code) {				// notify client of request handled	
	predicateCode = code;	// set global predicate value for searching function
	requests.remove_if(codeMatcher());	// remove all requests that match value
}
unsigned char client::getNextRequest() {	// returns code of next request to process 
	std::string temp = requests.front();		// pop the front element
	requests.pop_front();
	requests.push_back(temp);			// add it to the back of the list
	return (unsigned char)temp.at(0);	// return it's message type
}
bool client::hasRequests(bool* hasReq) {						// returns true if there are requests to be handled, false otherwise
	// set has requests value;
	*hasReq = requests.size() > 0;

	// returns the players clientStatus
	return clientStatus;	
}
void client::handleRequestFailure(unsigned char code) {	// handles the server's failure to fulfill a specified request
	// do something to handle failed requests
	printf("\nhandling request failure: %c", code);
	requestHandled(code);
}
// **************************************************************
//			GETTERS AND SETTERS
//***************************************************************
bool client::setName(std::string nName) { // Sets client's name to argument passed in, returns true if successful
	// If the new name exists, set it and print to log
	if (&nName && nName.length() > 0) {
		name = nName;
		printf("new name: %s", name.c_str());
	}
	else {
		std::string temp = "new name error: " + name;
		printf(temp.c_str());
		return false;					// returns false if name not set correctly
	}
	return (name.length() > 0);		// return true if name's length is greater than 0
}
bool client::setName() {				// queries user for name then sets their new name, returns true if successful
	// get new name from client
	std::string *temp = new std::string();
	int result = getStrAnswer(temp, R_PNAME);		// request client name

	// update name if correct message was received
	if (result == 1)
		return setName(*temp);
	else return false;
}
std::string client::getName() { 	// Returns the client's name
	return name;
}
void client::setSocket(SOCKET nSocket) {	// Sets the clients socket to argument passed in
	clientSocket = nSocket;
}
SOCKET client::getSocket() {	// Returns the clients socket
	return clientSocket;
}
bool client::closeConnection() {		// Close out current connection with client
	//disconnect client
	int result = shutdown(clientSocket, SD_SEND);
	if (result == SOCKET_ERROR) {						//quit on shutdown error
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}
	//else {
	//	do {
	//		result = recv(clientSocket, buffer, DEFAULT_BUFLEN, 0);		// Check for further packages sent by client
	//		if (result > 1) {
	//			std::string temp = "ERROR: packets received after socket closure: ";	// if packets are received, print to log
	//			temp += buffer;
	//			printf(temp.c_str());
	//		}
	//	} while (result > 0);
	//}

	bool socketClosed = false;
	do {				// ensure that socket is successfully
		try {
			result = closesocket(clientSocket);		// attempt to close socket until it throws error indicating closure
		}
		catch (int e) {
			socketClosed = true;
			printf("Socket closed? with error:", e);
		}
	} while (!socketClosed);
	return true;
}
int client::getPlayerPref() {					// returns player # preference, or 0 if none
	return (int)playerPref;
}
void client::setPlayerPref(int n) {					// updates player # preference
	playerPref = (char)n;
}
// **************************************************************
// ::TIMEOUT AND ACTIVITY MONITORING::
//***************************************************************
bool client::recentlyActive() {		// returns true if the client was active within a set time interval
	// returns true if client has been active within the set time interval
	time_t currentTime = time(NULL);			// get current time

	// return true if the time since recent activity is less than the specified time interval
	return difftime(currentTime, recentActivity) < timeoutInterval;	//
}
void client::updateStatus() {		// checks in with and updates the current client's clientStatus
	int result, status;

	// ping server for clientStatus
	result = sendM(PING_QRY);

	// get new clientStatus
	if (result > 0) 	// if the ping query was sent successfully
		result = getIntAnswer(&status, PING_ANS);	// set the server's status

	// if client connection had errors
	// put the lowest value into clientStatus
	if (result < 1 || status < 1){
		if (result >= status)
			clientStatus = result;
		else
			clientStatus = status;
	}
}
void client::updateStatus(int n) {	// checks in with and updates the current client's status
	clientStatus = n;
}
// **************************************************************
// ::INBOX AND REQUEST HISTORY::
//***************************************************************
bool client::inboxHas(unsigned char aCode) {	// returns true if a message of type 'aCode' is in the inbox
	// check each element for aCode
	for (auto it = inbox.begin(); it != inbox.end(); it++) {
		if ((unsigned char)it->at(0) == aCode)
			return true;
		else if ((unsigned char)it->at(0) == PING_QRY) {
			handlePing();
			return inboxHas(aCode);
		}
	}

	// if no message was found
	return false;
}
int client::getInboxMessage(std::string * ans, unsigned char aCode) {	// get's specified message from inbox as if it were just received
	// retrieve relevant message from inbox
	for (auto it = inbox.begin(); it != inbox.end(); it++) {
		// navigate to the correct message
		if ((unsigned char)it->at(0) == aCode) {
			// copy message out of list
			buffer.assign(*it);

			// set recent message value
			recent = (unsigned char)buffer.at(0);

			// remove message from inbox
			inbox.erase(it);

			// if message was request, add it to requests list
			if (isRequest(recent)) {
				printf("\nAdding message: %u to requests list, message: %s", buffer.at(0), buffer.c_str());
				requests.push_back(buffer);
			}

			// quit search
			break;
		}
	}

	// if the message had a body, copy it out
	if (buffer.length() > 0) 			// copy to argument for asking function
		ans->assign(buffer.substr(1, std::string::npos));	// start at buffer[1], skipping message type signifier

	// if there was no body to the message, assign the empty string
	else
		ans->assign("");
	
	// return status
	return clientStatus.load();
}
