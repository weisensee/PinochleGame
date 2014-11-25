/*	gameListNode.cpp -- Online Pinochle Game -- Lucas Weisensee November 2014

	functions for game list node items

*/
	//Creates a game list node with the passed in attributes
gameListNode(unsigned short ngameID, char * ngameName, char * nplayerName, HANDLE * ngameHandle, char nstatus, unsigned char nplayers)
{
	if(ngameID && ngameHandle && ngameName && nplayerName) {	//if the values were passed correctly
		gameHandle = *ngameHandle;		//thread handles for active games array
		status = nstatus;
		gameID = ngameID;
		players = 1;

		//allocate array space
		playerNames = new char*[nplayers]							//player name array
		playerNames[0] = new char[strlen(nplayerName) + 1];			//first player/creating player name copy
		gameName = new char[strlen(ngameName) + 1];					//game name array allocation

		//copy info
		strcpy(playerName[0], nplayerName);
		strcpy(gameName, ngameName);
	}
}

~gameListNode()		//deallocate reserved memory
{
	if(gameName)				//if memory was used
		delete gameName;			//delete it
	for (int i = 0; i < 10; ++i)	//check each player name *
	{
		if(playerNames[i])
			delete playerNames[i];	//delete it
		else
			i + 10;
	}
}

std::string getInfoString()				//returns game information as string see "server packet specifications.txt"
{

	std::string * temp = new std::string;
	temp += status + "^" + gameID + "^" + gameName + "^" + gamecreator + "^" +  playerNames[0];

	//append other player names if available
	if (players > 1)
		for (int i = 1; i < players; ++i)
			temp += "^" + playerNames[i];

	//close game string
	temp += "\\";
}

bool updateStatus(char newStatus)	//updates current game with new status returns true if success
{
	if(newStatus)
		status = newStatus
	else
		return false;
	return true;
}

