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

	Data:
	string name;
	SOCKET ClientSocket;

*/

#include "..\Library\Client.h"

// ::SETTINGS::
double DEFAULT_TIMEOUT = 4000;	// default amount of time between Messages before server rechecks Client's status
int MAX_ATTEMPTS = 5;			// number of times to try and receive the correct Message type


// **************************************************************
// codeMatcher predicate helper class for list search
//***************************************************************
unsigned char predicateCode;
struct codeMatcher {		// returns true if toMatch matches the global code: 'predicateCode' which must be before hand, helper function for list removal
public:
	bool operator() (std::string toMatch) {
		printf("\n comparing two chars, 1: %c and 2: %c", (unsigned char)toMatch[0], predicateCode);
		if ((unsigned char)toMatch[0] == predicateCode)
			printf("\nAnd it was true!");
		return ((unsigned char)toMatch.at(0) == predicateCode);
	}
	void operator= (unsigned char code) {
		predicateCode = code;
	}
};
// **************************************************************
//			Client CONSTRUCTORS AND SETUP
//***************************************************************
Client::Client(const Client &toCopy){
	// copy all info from toCopy
	name = toCopy.name;			// Client's name
	ClientSocket = toCopy.ClientSocket;		// Client's socket
	unsigned char recent = toCopy.recent;		// Most recent Message type
	playerPref = toCopy.playerPref;			// players preferred position # (1-max)

	// ::INBOX AND REQUEST HISTORY::
	requests = toCopy.requests;	// lists of active requests
	inbox = toCopy.inbox;		// lists of unreceived Messages

	// ::TIMEOUT AND ACTIVITY MONITORING::
	int localStatus = toCopy.localStatus;			// the status to return when queried for local status
	double timeoutInterval = toCopy.timeoutInterval;		// how long between contact with Client before server re-checks status
	time_t recentActivity = toCopy.recentActivity;		// most recent time of contact with Client

	// ::MULTI_THREADING::
	CRITICAL_SECTION inboxLock = toCopy.inboxLock;	// Message inbox access lock
	CRITICAL_SECTION EventLock = toCopy.EventLock;	// Message inbox access lock

	// copy out atomic value (...atomically)
	ClientStatus = toCopy.ClientStatus.load();
	hasUpdateEvent = toCopy.hasUpdateEvent;			// true if the Client has an update event they should be signaling
	newMessage = toCopy.newMessage;			// server's new Message event

}
Client::Client(std::string nName, SOCKET pSocket, int tInterval) : Client(nName, pSocket) {	//constructs Client with arguments passed in
	// perform normal construction
	
	// set default timeout interval
	DEFAULT_TIMEOUT = tInterval;
}
Client::Client(std::string nName, SOCKET pSocket) {	//constructs Client with arguments passed in
	setupClient(nName, pSocket);

	// initialize most recent Message type
	recent = (unsigned char) 111;
}
Client::~Client() {
	// Should we close the socket connection here? yes.
	DeleteCriticalSection(&inboxLock);

}
int Client::setupClient(std::string nName, SOCKET pSocket) {	//constructs Client with arguments passed in
	// initialize log file
	//pLog = new LogFile("Client log file_", "C:\\Users\\Pookey\\OneDrive\\Projects\\PinochleGame\\logs\\Clients");
	
	// set Client socket
	ClientSocket = pSocket;
	
	// set Client name
	setName(nName);

	// starts without an hasUpdateEvent
	hasUpdateEvent = false;

	// initialize predicate code
	predicateCode = (unsigned char)0;

	// zero out player position preference
	playerPref = 0;

	// set status to active
	ClientStatus = 1;
	localStatus = 1;

	// amount of time between requests before server rechecks Client's status
	timeoutInterval = DEFAULT_TIMEOUT; 

	// Launch listening thread to listen on the incoming connection and critical sections for communicating with it
	InitializeCriticalSectionAndSpinCount(&inboxLock, 0x00000200);
	InitializeCriticalSectionAndSpinCount(&EventLock, 0x00000200);
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
// ::SEND Message FUNCTIONS::
//***************************************************************
int Client::sendMFinal(unsigned char code, std::string* toSend) {	// Sends char array to Client, of Message code: type, returns false if send not successful
	// Verify Client status was functional on last connection
	if (ClientStatus < 1) {
		printf("\nERROR: sending Message when Client status is already: %i", ClientStatus);
		return ClientStatus;
	}
	// if the Client had functional status before
	else {
		// Generate Message string:
		std::string * Message = generateMessage(code, toSend);

		DEBUG_IF(true)
			printf("\nSending: %u, Message: %s", (unsigned char)Message->at(0), Message->c_str());

		// Send Message and update connection ClientStatus
		int result = send(ClientSocket, Message->c_str(), Message->length(), 0);		//send list

		// Convert to string
		if (result == SOCKET_ERROR) {		// if send failed
			printf("Send Error: %i, Message type: %u, Message: %s", result, code, toSend->c_str());
			ClientStatus = result;		// update Client status
		}
		return result;
	}
}
int Client::sendM(unsigned char code, std::string toSend) {	// Sends char array to Client, of Message type: code
	return sendMFinal(code, &toSend);
}
int Client::sendM(int n) {		// Sends char array to Client, includes description in case error log must be written
	if (n > 256)
		printf("Send error(char overflow):");
	return sendM((unsigned char)n);

}
int Client::sendM(unsigned char code) {	// Sends Message corresponding to code to Client
	std::string* temp = new std::string("");
	return sendM(code, temp);				// send code with empty string as Message body
}
int Client::sendM(unsigned char code, int toSend) {					// Sends int  Message to Client, of Message type: code
	// convert int to string
	std::string temp;
	char buf[32];
	itoa(toSend, buf, 10);
	temp.assign(buf);

	// send string to Client
	return sendMFinal(code, &temp);	// call main send Message function
}
int Client::sendM(unsigned char code, char toSend) {		// Sends char Message to Client, of Message type: code
	std::string temp;
	temp.assign(1, toSend);		//copy to send into string
	//temp.append("\0"); 

	printf("\nsending char: %c",temp.c_str());


	return sendMFinal(code, &temp);	// call main send Message function
}
int Client::sendM(unsigned char code, const char * toSend) {	// Sends char array to Client, includes description in case error log must be written
	std::string temp = toSend;
	return sendM(code, &temp);
}
int Client::sendM(unsigned char code, std::string* toSend) {	// Sends char array to Client, of Message code: type
	return sendMFinal(code, toSend);
}
int Client::sendM(unsigned char code, char * toSend) {		// Sends char array to Client, includes description in case error log must be written
	std::string temp = toSend;
	return sendM(code, &temp);
}
int Client::sendM(int code, std::string* toSend) {	// Sends char array to Client, of Message code: type
	if (code > 256)
		printf("Send error(char overflow):");
	unsigned char temp = (unsigned char)code;
	return sendMFinal(temp, toSend);
}
std::string* Client::generateMessage(unsigned char code, std::string *toSend) {
	// Insert Message code appropriately
	toSend->insert(0, (const char*)&code);

	// append null terminating character
	toSend->append(1, '\0');

	// Return complete Message
	return toSend;
}
int Client::getConnectionType() {		// Returns what type of connection the Client wants to initiate: database access or game play
	// send connection type request
	int result = sendM(S_STATUS);

	// if send failed, return error
	if (!result)
		return result;

	// otherwise return answer
	else {
		std::string ans;
		result = getStrAnswer(&ans, CON_TYPE);		// get answer from Client
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
bool Client::setUpdateEvent(WSAEVENT newEvent) {	// sends in the update event to be signaled by the Client when updates are received from user
	// seems like we can just use the event copy constructor here
	//newMessage = OpenEvent(desiredAccess, false, newEvent)

	// copy specified event 
	newMessage = newEvent;

	// set update event ownership to true
	hasUpdateEvent = true;

	// if Messages have been received, signal game manager
	if (inbox.size() > 0)
		signalNewMessage();

	return true;
}
void Client::signalNewMessage() {				// updates the server of new Message arrival
	if (!SetEvent(newMessage))
		printf("\nSetEvent failed on signaling 'newMessage'");	// print error if it fails

}
static DWORD WINAPI launchListenThread(LPVOID* callingObj) {		// creates a thread that listens on the socket for incoming packets
	// cast *this from parameter
	Client* self = (Client*)callingObj;

	// launch listening function
	self->listenForPacket();

	// return some value
	return 1;
}
void Client::listenForPacket() {			// listens for packet on the Client's socket
	std::string * temp = new std::string();

	// Setup event to listen for
	WSAEVENT readEvent[1];
	readEvent[0] = WSACreateEvent();
	int result = WSAEventSelect(ClientSocket, readEvent[0], FD_READ);

	// check that event was created successfully
	if (result == SOCKET_ERROR) {
		printf("\nERROR: socket error in listenforpacket: WSAEventSelect: %i", WSAGetLastError());
		exit(0);
	}
	char buf[256];

	// listen indefinitely
	while (true) {
		// Listen for Message on socket connection
		result = WSAWaitForMultipleEvents(
			1,			// wait for one event
			readEvent,	// the event to wait for
			false,		// don't wait for all the events
			WSA_INFINITE,	// wait forever	WSA_INFINITE
			false);		// unnecessary - some kind of I/O completion stat
			
		printf("\nWSAWaitForMultipleEvents result: %d", result);

		// if the wait failed
		if (result == WSA_WAIT_FAILED) {
			printf("\nWSA_WAIT_FAILED, ClientStatus: %i, error: %i", ClientStatus.load(), WSAGetLastError());
			exit(0);
		}

		// if wait succeeded
		if (result >= 0) {
			// if the event was successfully waited for, get packet
			result = recv(ClientSocket, buf, DEFAULT_BUFLEN, 0);

			// if the recv succeeded
			if (result > 0) {
				/// add to list
				// Request ownership of inboxLock critical section.
				EnterCriticalSection(&inboxLock);

				DEBUG_IF(true) {
					printf("\nreceived: %u, Message: ", buf[0], buf);
					//printinbox();
				}

				temp->assign(buf);
				inbox.push_back(*temp);		// add to list

				DEBUG_IF(true) {
					printf("\nreceived: %u, Message: ", buf[0], buf);
					//printinbox();
				}

				// Release ownership of inboxLock critical section.
				LeaveCriticalSection(&inboxLock);


				// if game event is provided, signal game of Message receipt
				if (hasUpdateEvent)
					signalNewMessage();
			}
			// if recv did not succeed
			else if (result < 0) {
				printf("\nRecv failure, buffer %s, ClientStatus: %i, error: %i", buf, ClientStatus.load(), WSAGetLastError());
			}
		}

		if (!WSAResetEvent(readEvent[0]))
			printf("\nWSAResetEvent failure, error: %i", WSAGetLastError());

	}

	delete temp;
}
unsigned char Client::getAnswerType() {		// Returns the Message code from the most recent packet received from the Client, -1 if null
	if (buffer.length() > 0)
		return recent;
	else
		return -1;
}
int Client::getStrAnswer(std::string * ans, unsigned char aCode) {	// Sets ans to Client's string response and checks ans type, returns 0 if receive error or if answer code doesn't match 'aCode', and -1 on connection closure, 1 on success and if answer code matches 'aCode'
	// If specific answer is requested, check if it has already been received	
	// check that connection is functioning and the Message desired is in the inbox
	//*************TESTING::*************
	std::cout << "\n";
	while (ClientStatus > 0 && !inboxHas(aCode)) {
		std::cout << ". .";

		// rest for a little
		int result = WaitForSingleObject(newMessage, 5000);
	}

	//DEBUG_IF(true)
	//	std::cout << "\nClientStatus: " << ClientStatus << " ClientStatus.load(): " << ClientStatus.load();

	// update local timestamp of recent activity
	time(&recentActivity);

	return getInboxMessage(ans, aCode);	// return received Message if it's in the inbox
}
int Client::getStrQueryAnswer(std::string * ans, unsigned char qCode, unsigned char aCode){	// Sets ans to Client's string response to Message "qcode" and checks ans type, returns 0 if receive error or if answer code doesn't match 'aCode', and -1 on connection closure, 1 on success and if answer code matches 'aCode'
	// send Query
	int result = sendM(qCode);

	// if Query was received successfully, get answer
	if (result > 0)
		return getStrAnswer(ans,aCode);

	// if Query was not received successfully
	return result;	
}
int Client::getIntAnswer(int * ans, unsigned char aCode) {	// Gets integer answer from Client, returns receive success code if answer type matches aCode type
	std::string *temp = new std::string();
	int result = getStrAnswer(temp, aCode);

	// check receive success
	if (result > 0) {
		//Convert to Int and save answer to arg passed in

		//*************************INVALID ARGUMENT ERROR:: (when Client quit)**********************
		printf("\nTemp: %s", temp->c_str());
		*ans = std::stoi(*temp, NULL, NULL);

		// return success
		return 1;
	}
	// if receive failed
	else
		return result;
}
int Client::checkClientResponse(unsigned char aCode) {	// checks that correct response was received, considers receive success, then checks code of received Message. if receive was successful, returns 1 if Message type matched, 0 otherwise. if receive failed, returns failure code
	// Check that connection is still functioning
	if (ClientStatus < 1){
		printf("\nClient connection error and in checking response: %u", aCode);
		return false;
	}
	
	// if aCode matches received code return true
	printf("\nChecking Client response: %u against code: %u", (unsigned char)buffer.at(0), aCode);
	if (aCode == (unsigned char)buffer.at(0)) {
		printf("\nThe response matched");
		return true;
	}
	// if the response didn't match the required response type
	else {
		printf("\nThe response didn't Match!");

		// add response to unReported Message list (inbox)
		inbox.push_back(buffer);
		return false;
	}

}
Card* Client::getNextPlay() {		// return the Clients next Card play on the current trick
	std::string ans;
	// if the play Message is already in the inbox
	if (inboxHas(PLY_Card)) {
		getStrAnswer(&ans, PLY_Card);
		return new Card(ans.at(0));		// return the played Card
	}
	// if the Card hasn't been played: Query for Card play and get Card answer
	else {
		int result = getStrQueryAnswer(&ans, REQ_PLAY, PLY_Card);

		// parse to Card
		return new Card(ans.at(0));
	}
}
// **************************************************************
//			Client REQUEST HANDLING
//***************************************************************
int Client::getStatus() {	// returns true if the Client has been active within the set time interval
	// if server hasn't heard from Client for a while, check if still active
	if (!recentlyActive())
		// check if player is still active
		updateStatus();

	return ClientStatus;
}
bool Client::isRequest(unsigned char code) {	// returns true if code is a request type that must be answered
	//bool temp = rLibrary[code];
	//if (temp)
	//	printf("\ncode: %u is TRUE in library", code);
	//else 
	//	printf("\ncode: %u is FALSE in library", code);
	return rLibrary[code];
}
void Client::requestHandled(unsigned char code, bool success) {	// notify Client of request handled and success
	if (success)					// if the request was handled successfully
		requestHandled(code);		// remove all that match from list
	else
		handleRequestFailure(code);
}
void Client::requestHandled(unsigned char code) {				// notify Client of request handled	
	predicateCode = code;	// set global predicate value for searching function
	requests.remove_if(codeMatcher());	// remove all requests that match value
}
unsigned char Client::getNextRequest() {	// returns code of next request to process 
	std::string temp = requests.front();		// pop the front element
	requests.pop_front();
	requests.push_back(temp);			// add it to the back of the list
	return (unsigned char)temp.at(0);	// return it's Message type
}
void Client::checkInbox() {	// checks the inbox for any requests and processes any pings
	
	// if there are Messages in the inbox check them for request status
	if (inbox.size() > 0) {

		// check each Message in the inbox
		for (auto current = inbox.begin(); current != inbox.end();) {
			// if it is a request, add it to the requests list
			if ((current->at(0))) {
				requests.push_back(*current);	// add to requests
				current = inbox.erase(current);			// remove from inbox
				//dont iterate to next element
			}
			// if element is a ping, respond
			else if (current->at(0) == PING_QRY) {
				sendM(PING_ANS, localStatus);	// respond to ping request
				current = inbox.erase(current);			// remove from inbox
			}
			else
				++current;		// if no element was removed, iterate to next element
		}
	}
}
bool Client::hasRequests(bool* hasReq) {						// returns true if there are requests to be handled, false otherwise
	// check inbox for requests and pings
	checkInbox();

	// set has requests value to true if there are requests in the list
	if (requests.size() > 0)
		*hasReq = true; 
	else 
		*hasReq = false;

	// returns the players ClientStatus
	return ClientStatus > 0;	
}
void Client::handleRequestFailure(unsigned char code) {	// handles the server's failure to fulfill a specified request
	// do something to handle failed requests
	printf("\nhandling request failure: %c", code);
	requestHandled(code);
}
// **************************************************************
//			GETTERS AND SETTERS
//***************************************************************
bool Client::setName(std::string nName) { // Sets Client's name to argument passed in, returns true if successful
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
bool Client::setName() {				// queries user for name then sets their new name, returns true if successful
	// get new name from Client
	std::string *temp = new std::string();
	int result = getStrAnswer(temp, R_PNAME);		// request Client name

	// update name if correct Message was received
	if (result == 1)
		return setName(*temp);
	else return false;
}
std::string Client::getName() { 	// Returns the Client's name
	return name;
}
void Client::setSocket(SOCKET nSocket) {	// Sets the Clients socket to argument passed in
	ClientSocket = nSocket;
}
SOCKET Client::getSocket() {	// Returns the Clients socket
	return ClientSocket;
}
bool Client::closeConnection() {		// Close out current connection with Client
	//disconnect Client
	int result = shutdown(ClientSocket, SD_SEND);
	if (result == SOCKET_ERROR) {						//quit on shutdown error
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}
	//else {
	//	do {
	//		result = recv(ClientSocket, buffer, DEFAULT_BUFLEN, 0);		// Check for further packages sent by Client
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
			result = closesocket(ClientSocket);		// attempt to close socket until it throws error indicating closure
		}
		catch (int e) {
			socketClosed = true;
			printf("Socket closed? with error:", e);
		}
	} while (!socketClosed);
	return true;
}
int Client::getPlayerPref() {					// returns player # preference, or 0 if none
	return (int)playerPref;
}
void Client::setPlayerPref(int n) {					// updates player # preference
	playerPref = n;
}
// **************************************************************
// ::TIMEOUT AND ACTIVITY MONITORING::
//***************************************************************
bool Client::recentlyActive() {		// returns true if the Client was active within a set time interval
	// returns true if Client has been active within the set time interval
	time_t currentTime = time(NULL);			// get current time

	// return true if the time since recent activity is less than the specified time interval
	return difftime(currentTime, recentActivity) < timeoutInterval;	//
}
void Client::updateStatus() {		// checks in with and updates the current Client's ClientStatus
	int result, status;

	// ping server for ClientStatus
	result = sendM(PING_QRY);

	// get new ClientStatus
	if (result > 0) 	// if the ping Query was sent successfully
		result = getIntAnswer(&status, PING_ANS);	// set the server's status

	// if Client connection had errors
	// put the lowest value into ClientStatus
	if (result < 1 || status < 1){
		if (result >= status)
			ClientStatus = result;
		else
			ClientStatus = status;
	}
}
void Client::updateStatus(int n) {	// checks in with and updates the current Client's status
	ClientStatus = n;
}
// **************************************************************
// ::INBOX AND REQUEST HISTORY::
//***************************************************************
bool Client::inboxHas(unsigned char aCode) {	// returns true if a Message of type 'aCode' is in the inbox
	// check each element for aCode
	for (auto it = inbox.begin(); it != inbox.end(); ) {
		if ((unsigned char)it->at(0) == aCode)
			return true;
		else if ((unsigned char)it->at(0) == PING_QRY) {
			sendM(PING_ANS, localStatus);	// respond to ping request
			it = inbox.erase(it);			// remove from inbox
		}
		else
			++it;
	}

	// if no Message was found
	return false;
}
int Client::getInboxMessage(std::string * ans, unsigned char aCode) {	// get's specified Message from inbox as if it were just received
	// retrieve relevant Message from inbox
	for (auto it = inbox.begin(); it != inbox.end();) {
		// navigate to the correct Message
		if ((unsigned char)it->at(0) == aCode) {
			// copy Message out of list
			buffer.assign(*it);

			// set recent Message value
			recent = (unsigned char)buffer.at(0);

			// remove Message from inbox
			it = inbox.erase(it);

			// if Message was request, add it to requests list
			if (isRequest(recent)) {
				DEBUG_IF(true)
					printf("\nAdding Message: %u to requests list, Message: %s", buffer.at(0), buffer.c_str());
				requests.push_back(buffer);
			}

			// quit search
			break;
		}
		else	// if an item wasnt removed from the list, iterate to next item
			++it;
	}

	// if the Message had a body, copy it out
	if (buffer.length() > 0) 			// copy to argument for asking function
		ans->assign(buffer.substr(1, std::string::npos));	// start at buffer[1], skipping Message type signifier

	// if there was no body to the Message, assign the empty string
	else
		ans->assign("");
	
	// return status
	return ClientStatus.load();
}
void Client::printinbox() {		// print out the Messages in the inbox
	printf("\n\n**************************************************\nPrinting inbox of size: %d", inbox.size());
	int count = 0;
	for (auto it = inbox.begin(); it != inbox.end(); ++it) {
		printf("\nItem: %i, type: %u -- %s", count, it->at(0), it->c_str());
		++count;
	}
	printf("\ninbox printed, count: %d\n**************************************************\n\n", count);
}
void Client::printRequests() {	// print out the requests list for debugging
	printf("\n\n**************************************************\nPrinting requests list of size: %d", requests.size());
	int count = 0;
	for (auto it = requests.begin(); it != requests.end(); ++it) {
		printf("\nItem: %i, type: %u -- %s", count, it->at(0), it->c_str());
		++count;
	}
	printf("\nRequests printed, count: %d\n**************************************************\n\n", count);

}