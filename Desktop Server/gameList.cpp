/*	gameList.cpp -- Online Pinochle Game -- Lucas Weisensee November 2014
		
		Functions for gameList.h : 
			Manages and stores list of active games
			Responds to querries about active games


*/

NOTES::
nodes no longer delete all following nodes



#include "gameList.h"


gameList(unsigned short MAX_GAMES)		//constructs a game list of linked nodes with max length MAX_LENGTH
{
	// Initiate variables
	max = MAX_GAMES;
	total = 0;
	next = NULL;
	head = NULL;
	currentList = NULL;

}

gameList()		//default: constructs a game list with MAX_LENGTH = 10
{
	gameList(10);
}

~gameList()			//destructor
{
	if(head)					//delete the following items in the list if they exist
		delete head;
	if(currentList)
		delete [] currentList;
}

int add(char * gamename, char * playerName, HANDLE threadHandle)		//add game with specified components to list if it's not already there
{
	// If the list is full: 
	if(total >= max)	//return an error
		return 0;

	// If the list is empty:
	if(head == NULL) {		//create head node
		head = new gameListNode(gamename, playerName, threadHandle);
		tail = head;
	}

	// If the list is not empty
	else {
		tail->next = new gameListNode(gamename, playerName, threadHandle);
		tail = tail -> next;		//update tail pointer
}

bool getCurrent(char * sendList)		//copies formatted list of current active games to sendList, returns true if list existed
{
	// Initiate pointers
	std::string toSend; 
	char * toCopy;

	// If the list exists:
	if(head) {
		gameListNode current = head;					//start at first node
		while(current) {								//while at a valid node
			toSend += current->getInfoString() + "/";	//add its info to send plus game delimiter
			current = current->next;					//move to next node
		}
		toSend += "*";			//append closing symbol
		sendList = new char[toSend.length()]
		strcpy(sendList, toSend);			//save to buffer string
	}

	// If the list doesnt exist return 0
	else 
		return 0;
	return true;			// returns sucess if list existed
}

int total_games()						//returns total active games in list
{
	return total;
}

int remove(int gameID)	//remove matching game from list
{
	// Retreive the pointer to the node preceeding gameID
	gameListNode found = find_preceeding(gameID);

	// Check for errors
	if (found == -1)		//if game not found
		return found;

	// If the head of the list needs to be removed
	if (found == 0) {
		gameListNode temp = head->next;
		delete head;
		head = temp;
	}

	// If the node to remove is not the head 
	else {
			gameListNode nextNode = found->next.next;		//remove it
			delete found->next;
			found->next = nextNode;							//re-connect list
		}

	//return success
	return 1;
}

bool updateStatus(unsigned short gameID, char status)		//updates the list's current status for a specific game
{
	return find_preceeding(gameID).updateStatus(status);		//find game and send update
}

//returns a pointer to the node preceeding gameID. 
	//returns -1 if failed, 0 if head, * to node otherwise
gameListNode* find_preceeding(unsigned short gameID) {		

	//check head Node return 1 if match
	if (*head.getID() == gameID) {
		return 0;

	//iterate over list
	else {
		gameListNode current = head;
		while (current->next && *current->next.getID() != gameID)		//traverse to node before matching node
			current = current->next;	
		if (current->next) 							//if the node was found
			return current;							// return it

		else if (current->next == NULL)						//if the node was not found
			return -1;										//return error
	}

}
