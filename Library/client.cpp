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

// **************************************************************
//			CLIENT CONSTRUCTORS AND SETUP
//***************************************************************
client::client(std::string nName, SOCKET pSocket) {	//constructs client with arguments passed in
	setupClient(nName, pSocket);

	// initialize most recent message type
	recent = (unsigned char) 111;
}
client::~client() {
	// Should we close the socket connection here? yes.
}
int client::setupClient(std::string nName, SOCKET pSocket) {	//constructs client with arguments passed in
	// initialize log file
	pLog = new LogFile("client log file_", "C:\\Users\\Pookey\\OneDrive\\Projects\\PinochleGame\\logs\\clients");
	clientSocket = pSocket;
	
	// set client name
	setName(nName);

	std::string temp = "Constructor, name: " + name;
	pLog->writetolog(&temp);

	// initialize predicate code
	predicateCode = (unsigned char)0;

	return true;
}
int client::sendM(int n) {		// Sends char array to client, includes description in case error log must be written
	if (n > 256)
		pLog->writetolog("Send error(char overflow):");
	return sendM((unsigned char)n)	;
	
}
// **************************************************************
//			CLIENT MESSAGE SENDING
//***************************************************************
int client::sendM(unsigned char code) {	// Sends message corresponding to code to client
	char empty = '\0';
	return sendM(code, &empty);				// send code with empty string as message body
}
int client::sendM(unsigned char code, char toSend) {					// Sends char message to client, of message type: code
	std::string temp;
	temp.assign(1, toSend);		//copy to send into string
	temp.append('\0'); 

	return sendMFinal(code, &temp);	// call main send message function
}
int client::sendM(unsigned char code, const char * toSend) {	// Sends char array to client, includes description in case error log must be written
	std::string temp = toSend;
	return sendM(code, &temp);
}
int client::sendM(unsigned char code, std::string* toSend) {	// Sends char array to client, of message code: type
	return sendMFinal(code, toSend);
}
int client::sendMFinal(unsigned char code, std::string* toSend) {	// Sends char array to client, of message code: type, returns false if send not successful
	// Generate message string:
	std::string * message = generateMessage(code, toSend);

	// Send message
	int result = send(clientSocket, message->c_str(), message->length(), 0);		//send list

	if (result == SOCKET_ERROR || result <= 0) {		// if send failed
		message->insert(0, ": ");						// insert error info into string and:
		char *err = new char[10];
		message->insert(0, itoa(result, err, 10));		// insert result after converting it to char from int
		message->insert(0, "Send Error, result: ");
		delete[] err;
		printf(message->c_str());
		pLog->writetolog(message, WSAGetLastError());	// write to log
		return result;
	}
	return result;
}
int client::sendM(unsigned char code, char * toSend) {		// Sends char array to client, includes description in case error log must be written
	std::string temp = toSend;
	return sendM(code, &temp);
}
int client::sendM(int code, std::string* toSend) {	// Sends char array to client, of message code: type
	if (code > 256)
		pLog->writetolog("Send error(char overflow):");
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
		result = getStrAnswer(&ans);		// get answer from client
		if (!result) {				// check that answer was received successfully
			if (getAnswerType() == CON_TYPE)	// verify that correct answer type was received
				return buffer[1];				// return requested connection type
			else							// if wrong answer type was received
				return 0;						// return 0 if wrong answer type was received
		}
		// if answer was not received successfully
		else
			return result;	// retrun error type
	}
}
// **************************************************************
//			CLIENT MESSAGE RECEIPT
//***************************************************************
int client::getStrAnswerFinal(std::string * ans) {			// final get str ans function, all others are wrapper for this function
	int result = recv(clientSocket, buffer, DEFAULT_BUFLEN, 0);

	// Check for receive error
	if (result < 1) {
		pLog->writetolog("Receive Error: ", WSAGetLastError());	//write to log if receive failed
		return NULL;		// returns 0 if receive error
	}

	// Check if connection was closed by client
	if (result == 0) {
		pLog->writetolog("\nConnection closed");
		return -1;			// returns -1 if connection was closed by client
	}

	// If receive was successful, set client response to ans (argument passed in)
	else {
		// set recent message value
		recent = buffer[0];

		// if message was request, add it to requests list
		if (isRequest(recent))
			requests.push_back(buffer);

		// set argument value for asking function
		ans->assign(buffer + 1);	// start at buffer[1], skipping message type signifier
		return 1;
	}
	return -2;
}
unsigned char client::getAnswerType() {		// Returns the message code from the most recent packet received from the client, -1 if null
	if (buffer)
		return recent;
	else
		return -1;
}
int client::getStrAnswer(std::string * ans) {			// Sets ans to string answer from client, returns 0 if receive error, and -1 on connection closure, 1 on success 
	// get message from client
	return getStrAnswerFinal(ans);
}
int client::getStrAnswer(std::string * ans, unsigned char aCode) {	// Sets ans to client's string response and checks ans type, returns 0 if receive error or if answer code doesn't match 'aCode', and -1 on connection closure, 1 on success and if answer code matches 'aCode'
	// get client response
	int result = getStrAnswerFinal(ans);

	return checkClientResponse(result, aCode);
}
int client::getStrQueryAnswer(std::string * ans, unsigned char qCode) {						// Sets ans to client's string response to message "qcode", returns 0 if receive error, and -1 on connection closure, 1 on success 
	// send query
	int result = sendM(qCode);

	// check query send status
	if (result < 1)
		return result;	// if error, return error

	// get answer to query
	return getStrAnswerFinal(ans);	// return error status
}
int client::getStrQueryAnswer(std::string * ans, unsigned char qCode, unsigned char aCode){	// Sets ans to client's string response to message "qcode" and checks ans type, returns 0 if receive error or if answer code doesn't match 'aCode', and -1 on connection closure, 1 on success and if answer code matches 'aCode'
	// get answer to query
	int result = getStrQueryAnswer(ans, qCode);

	// if answer was received successfully
	if (result > 0)
		return aCode == getAnswerType();

	// if answer was not received successfully
	return result;	
}
int client::getIntAnswer(int * ans, unsigned char aCode) {	// Gets integer answer from client, returns receive success code if answer type matches aCode type
	// get int ans from network
	int result = getIntAnswer(ans);

	// check that answer was received successfully
	if (result < 1)
		return result;

	// check that answer type matches requirement (aCode)
	if (getAnswerType() != aCode)	// if the wrong answer was returned
		return 0;
	else return result;				// if the correct answer was returned return the result of the answer fetch
}
int client::getIntAnswer(int * ans) {			// Gets integer answer from client, returns receive success code
	// get answer from client
	// get string answer from client
	std::string *temp = new std::string();
	int result = getStrAnswerFinal(temp);

	// check receive success
	if (result == 1) {
		//Convert to Int and save answer to arg passed in

		//*************************INVALID ARGUMENT ERROR:: (when client quit)**********************
		*ans = std::stoi(temp->c_str());

			// return success
		return 1;
	}
	// if receive failed
	else
		return result;
}
int client::checkClientResponse(int result, unsigned char aCode) {	// checks that correct response was received, considers receive success, then checks code of received message. if receive was successful, returns 1 if message type matched, 0 otherwise. if receive failed, returns failure code
	// check that result was correct
	if (result > 0)
		return result;

	// check that aCode matches received code
	return aCode = (unsigned char)buffer[0];
}
// **************************************************************
//			CLIENT REQUEST HANDLING
//***************************************************************
void client::addRequest(char* newRequest) {
	// copy request string
	int len = strlen(newRequest);		// get length
	char * toAdd = new char[len + 1];	// allocate string
	strncpy(toAdd, newRequest, len);	// copy string
	toAdd[len] = '\0';					// ensure null char termination

	// add to queue
	requests.push_back(toAdd);
}
void client::requestHandled(unsigned char code, bool success) {	// notify client of request handled and success
	if (success)					// if the request was handled successfully
		requestHandled(code);		// remove all that match from list
	else
		handleRequestFailure(code);
}
void client::requestHandled(unsigned char code) {				// notify client of request handled	
	predicateCode = code;				// set global predicate value for searching funcion
	requests.remove_if(matchesCode);	// remove all requests that match value
}
unsigned char client::getNextRequest() {			// returns code of next request to process 
	char * temp = requests.front();		// pop the front element
	requests.pop_front();
	requests.push_back(temp);			// add it to the back of the list
	return (unsigned char)temp[0];		// return it's message type
}
bool client::hasRequests() {						// returns true if there are requests to be handled, false otherwise
	 return requests.size() > 0;
}
void client::handleRequestFailure(unsigned char code) {	// handles the server's failure to fulfill a specified request
	// do something to handle failed requests
	printf("\nhandling request failure: %c", code);
	requestHandled(code);
}
bool client::matchesCode(char * toMatch)	{	// returns true if toMatch matches the global code: 'predicateCode' which must be set before hand, helper function for list removal
	return ((unsigned char)toMatch[0] == predicateCode);
}
// **************************************************************
//			GETTERS AND SETTERS
//***************************************************************
bool client::setName(std::string nName) { // Sets client's name to argument passed in, returns true if successful
	// If the new name exists, set it and print to log
	if (&nName && nName.length() > 0) {
		name = nName;
		std::string temp = "new name: " + name;
		pLog->writetolog(&temp);
	}
	else {
		std::string temp = "new name error: " + name;
		pLog->writetolog(&temp);
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
		pLog->writetolog("shutdown failed: %d\n", WSAGetLastError());
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}
	else {
		do {
			result = recv(clientSocket, buffer, DEFAULT_BUFLEN, 0);		// Check for further packages sent by client
			if (result > 1) {
				std::string temp = "ERROR: packets received after socket closure: ";	// if packets are received, print to log
				temp += buffer;
				pLog->writetolog(&temp);
			}
		} while (result > 0);
	}

	bool socketClosed = false;
	do {				// ensure that socket is successfully
		try {
			result = closesocket(clientSocket);		// attempt to close socket until it throws error indicating closure
		}
		catch (int e) {
			socketClosed = true;
			pLog->writetolog("Socket closed? with error:", e);
		}
	} while (!socketClosed);
	return true;
}
