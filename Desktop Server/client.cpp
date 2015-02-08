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

#include "client.h"

/*
client::client() : client((SOCKET)NULL) {}

client::client(SOCKET pSocket) : client("NONE", pSocket)	{}		// Constructs client with given socket and no name
*/


client::client(std::string nName, SOCKET pSocket) {	//constructs client with arguments passed in
	pLog = new LogFile("client log file_", "C:\\Users\\Pookey\\OneDrive\\Projects\\PinochleGame\\logs\\clients");
	name = nName;
	clientSocket = pSocket;
	std::string temp = "Constructor, name: " + name;
	pLog->writetolog(&temp);
}

client::~client() {
	// Should we close the socket connection here? yes.
}

int client::sendM(int n) {		// Sends char array to client, includes description in case error log must be written
	if (n > 256)
		pLog->writetolog("Send error(char overflow):");
	return sendM((unsigned char)n)	;
	
}

int client::sendM(unsigned char * code) {	// Sends message corresponding to code to client
	char empty = '\0';
	return sendM(code, &empty);				// send code with empty string as message body
}
int client::sendM(unsigned char code) {		// Sends message corresponding to code to client
	char empty = '\0';
	return sendM(&code, &empty);			// send code with empty string as message body
}
int client::sendM(unsigned char * code, const char * toSend) {	// Sends char array to client, includes description in case error log must be written
	std::string temp = toSend;
	return sendM(code, &temp);
}
int client::sendM(unsigned char * code, std::string* toSend) {	// Sends char array to client, of message code: type
	return sendMFinal(code, toSend);
}
int client::sendMFinal(unsigned char * code, std::string* toSend) {	// Sends char array to client, of message code: type
	// Generate message string:
	std::string * message = generateMessage(*code, toSend);

	// Send message
	int result = send(clientSocket, message->c_str(), message->length(), 0);		//send list

	if (result == SOCKET_ERROR || result <= 0) {		// if send failed
		message->insert(0, ": ");						// insert error info into string and:
		message->insert(0, 1, result);
		message->insert(0, "Send Error: ");
		pLog->writetolog(message, WSAGetLastError());	// write to log
	}
	return result;
}

int client::sendM(unsigned char * code, char * toSend) {		// Sends char array to client, includes description in case error log must be written
	std::string temp = toSend;
	return sendM(code, &temp);
}
int client::sendM(int code, std::string* toSend) {	// Sends char array to client, of message code: type
	if (code > 256)
		pLog->writetolog("Send error(char overflow):");
	unsigned char temp = (unsigned char)code;
	return sendMFinal(&temp, toSend);
}

int client::getConnectionType() {		// Returns what type of connection the client wants to initiate: database access or game play
	// send connection type request
	int result = sendM((unsigned char) 1);

	return getIntAnswer();
}

int client::getAnswerType() {		// Returns the message code from the most recent packet received from the client, -1 if null
	if (buffer)
		return buffer[0];
	else
		return -1;
}


std::string *client::getStrAnswer(int n) {			// Gets string answer from client
	client::sendM(n);
	return getStrAnswer();

}
std::string * client::getStrAnswer() {			// Gets string answer from client
		// Get result from client, store in buffer
	int result = recv(clientSocket, buffer, DEFAULT_BUFLEN, 0);

	// Check receive success
	if (result < 1) {
		pLog->writetolog("Receive Error: ", WSAGetLastError());	//write to log if receive failed
		return NULL;
	}
	
	else {				// if receive was successful, return answer
		std::string *temp = new std::string(buffer);
		return temp;
	}


}
int client::getIntAnswer() {				// Gets integer answer from client
	// Get answer
	std::string * temp = getStrAnswer();

	//Convert to Int and return answer
	return std::stoi(temp->c_str());
}

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
		return false;
	}
	return (name.length() > 0);		// return true if name's length is greater than 0
}

bool client::setName() {				// queries user for name then sets their new name, returns true if successful
	// get new name from client
	std::string *temp = getStrAnswer(3);		// request client name

	// update name
	return setName(*temp);
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

std::string* client::generateMessage(unsigned char code, std::string *toSend) {
	// Insert code appropriately
	toSend->insert(0, (const char*)&code);
	
	// Return complete message
	return toSend;
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
