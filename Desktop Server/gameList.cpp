/*	gameList.cpp -- Online Pinochle Game -- Lucas Weisensee November 2014
		
		Functions for gameList.h : 
			Manages and stores list of active games
			Responds to queries about active games




NOTES::
nodes no longer delete all following nodes, git test

*/

#include "gameList.h"


gameList::gameList(int MAX_GAMES)		//constructs a game list of linked nodes with max length MAX_LENGTH
{
	// Initiate variables
	max = MAX_GAMES;
	total = 0;
	tail = NULL;
	head = NULL;
	currentList = NULL;

}

gameList::gameList()		//default: constructs a game list with MAX_LENGTH = 10
{
	gameList(10);
}

gameList::~gameList()			//destructor
{
	if(head)					//delete the following items in the list if they exist
		delete head;
	if(currentList)
		delete [] currentList;
}

bool gameList::addGame(_PROCESS_INFORMATION * processInfo, client * curClient)		//add game with specified components to list if it's not already there
{
	// If the list is full: 
	if (total >= max)	//return an error
		return 0;

	// Otherwise create and add node to list
	gameListNode * temp = new gameListNode(processInfo, curClient);
	return add(temp);
}
int gameList::addclient(int gameID, client * newClient) {	//add specified client to specified game, returns count of players in game or -1 if no list or game found
	std::string * name = new std::string(newClient->getName());
	if (head) {							// if the list exists
		gameListNode * prev = find_preceding(gameID);
		if ((int)prev == 0) {		// check if the head node is the game specified the head node
			head->addPlayer(name);
			return head->playerCount();
		}
		else if ((int)prev == -1)			// game not found
				return -1;
		else {
			prev->next->addPlayer(name);
			return head->playerCount();
		}
	}
	else
		return -1;							// list not found
}

bool gameList::add(gameListNode * ngame)							//add ngame to list of active games
{
	// If the list is full: 
	if (total >= max)	//return an error
		return 0;

	// If the list is empty:
	if(head == NULL) {		//create head node
		head = ngame;
		tail = head;
	}

	// If the list is not empty
	else {
		tail->next = ngame;
		tail = tail->next;		//update tail pointer
	}
	return true;
}

std::string gameList::getCurrent()		// copies formatted list of current active games to sendList, returns true if list existed
{
	// Initiate pointers
	std::string toSend; 

	// If the list exists:
	if(head) {
		gameListNode *current = head;					// start at first node
		while(current) {								// while at a valid node
			toSend += current->getInfoString() + "/";	// add its info to send plus game delimiter
			current = current->next;					// move to next node
		}
		toSend += "*";			// append closing symbol
	}

	// If the list doesn't exist return 0
	else 
		return 0;
	return toSend.c_str();			// returns success if list existed
}

	int gameList::total_games()						//returns total active games in list
{
	return total;
}

	bool gameList::remove(int gameID)	// remove matching game from list
{
	// Retrieve the pointer to the node preceding gameID
	gameListNode *found = find_preceding(gameID);

	// Check for errors
	if ((int)found == -1)		//if game not found
		return false;

	// If the head of the list needs to be removed
	if (found == 0) {
		gameListNode *temp = head->next;
		delete head;
		head = temp;
	}

	// If the node to remove is not the head 
	else {
		gameListNode *nextNode = found->next->next;		//remove it
		delete found->next;
		found->next = nextNode;							//re-connect list
	}

	//return success
	return true;
}

	bool gameList::updateStatus(int gameID, char status)		//updates the list's current status for a specific game
{
	return find_preceding(gameID)->updateStatus(status);		//find game and send update
}

// Returns a pointer to the node preceding gameID, -1 if failed, 0 if head, * to node otherwise
	gameListNode* gameList::find_preceding(int fGameID) {

	//check head Node return 0 if match
	if (head->getID() == fGameID)
		return NULL;

	//iterate over list
	else {
		gameListNode * current = head;
		while (current->next && current->next->getID() != fGameID)		//traverse to node before matching node
			current = current->next;	
		if (current->next) 							// if the node was found
			return current;							// return it
		else if (current->next == NULL)				// if the node was not found
			return (gameListNode*)-1;				// return error
	}

	return NULL;
}
