/* GameSettings.cpp 	Card game AI/Server

	Lucas Weisensee 	February 2015

	Functions for GameSettings class: 
		Stores Game settings and setup data for a Card game
			-specifically pinochle and euchre

		Allows different defaults to be set in one location across the server

		Simplifies the passing of game settings between classes

		Manages defaults and updates game settings
*/

#include "..\Library\GameSettings.h"

#define DEFAULT_TYPE 'E'
//***************************************************************
// ::CONSTRUCTORS AND DESTRUCTORS::
//***************************************************************
GameSettings::GameSettings(Client * p1):GameSettings() {				// Client constructor
	// copy player one Client info
	player1 = p1;
	roundsToPlay = 1;	// amount of times to play current game
}
GameSettings::GameSettings() {				// default constructor
	// default settings:
	setup(DEFAULT_TYPE);

	// set player1 to NULL
	gMode = 'A';		// mode set to auto by default
	player1 = NULL;	// default setting is null for first player
}
GameSettings::GameSettings(char TYPE):GameSettings() {	// default constructor for game type: 'TYPE'
	// setup the appropriate game settings for passed in variable
	setup(TYPE);

	// set player1 to NULL
	player1 = NULL;	// default setting is null for first player
}
GameSettings::~GameSettings() {}
//***************************************************************
// ::GAME STRING PROCESSING::
//***************************************************************
void GameSettings::reset() {		// reset to default settings
	GameSettings();
}
std::string* GameSettings::settingsString() {	// returns the formatted settings string for sending to a server/Client
	std::string* settingsInfo = new std::string();
	char temp[24];
	// String format:
	//type^status^ID^gamename^creator^maxplayers^maxobservers^rounds^goal
	// type
	settingsInfo->assign(1, gType);
	settingsInfo->append(1, '^');	// Delimiter 

	// status
	settingsInfo->append(1, STATUS);
	settingsInfo->append(1, '^');	// Delimiter 

	// Game ID
	settingsInfo->append(itoa(GAMEID, temp, 10));
	settingsInfo->append(1, '^');	// Delimiter 

	// game gMode
	settingsInfo->append(1, gMode);	// Delimiter 
	settingsInfo->append(1, '^');	// Delimiter 

	// game name
	settingsInfo->append(gameName);
	settingsInfo->append(1, '^');	// Delimiter 

	// game creator
	*settingsInfo += creator;
	settingsInfo->append(1, '^');	// Delimiter 

	// max number of players
	settingsInfo->append(itoa(MAXPLAYERS, temp, 10));
	settingsInfo->append(1, '^');	// Delimiter 

	// max number of observers	
	settingsInfo->append(itoa(MAXOBSERVERS, temp, 10));
	settingsInfo->append(1, '^');	// Delimiter 

	// game rounds to play	
	settingsInfo->append(itoa(roundsToPlay, temp, 10));
	settingsInfo->append(1, '^');	// Delimiter 

	// winning score
	settingsInfo->append(itoa(GOAL, temp, 10));


	// return settings info string
	return settingsInfo;
}
void GameSettings::setFromInfoString(const char* newInfoStr) {	// set current GameSettings data to match that in info string newData
	char * gameInfo = strdup(newInfoStr);
	// Split game list up into info tokens
	char * info = strtok(gameInfo, "^");	// split it into info tokens
	int tknNum = 0;

	// String format:
	//type^status^ID^gamename^creator^maxplayers^maxobservers^rounds^goal
	while (info != NULL) {				// while there are more info tokens
		switch (tknNum) {				// print out the appropriate info
		case (0) :	// Type
			gType = info[0];
			break;
		case (1) :	// Status
			STATUS = info[0];
			break;
		case (2) :	// Game ID
			GAMEID = atoi(info); 
			break;
		case (3) :	// Game gMode
			gMode = info[0];
			break;
		case (4) :	// Game Name
			gameName = info;
			break;
		case (5) :	// Game Creator
			creator = info;
			break;
		case (6) :	// max players
			MAXPLAYERS	 = atoi(info);
			break;
		case (7) :	// max observers
			MAXOBSERVERS = atoi(info);
			break;
		case (8) :	// rounds to play
			roundsToPlay = atoi(info);
			break;
		case (9) :	// Game Name
			GOAL = atoi(info);
			break;
		case (10) :	// Player 1
			names[1] = info;
			break;
		case (11) :	// Player 2
			names[2] = info;
			break;
		case (12) :	// Player 3
			names[3] = info;
			break;
		case (13) :	// Player 4
			names[4] = info;
			break;
		case (14) :	// Player 5
			names[5] = info;
			break;
		case (15) :	// Player 6
			names[6] = info;
			break;
		default:
			printf("Switch error: Player Overload!");
			break;
		}

		// finished with current info token, iterate to next info item
		info = strtok(NULL, "^");
		tknNum++;
	}


}
//***************************************************************
// ::SETUP::
//***************************************************************
void GameSettings::setup(char TYPE) {		// sets up the GameSettings with the default for the passed in type
	// setup the appropriate game settings for passed in variable
	switch (TYPE) {
		case 'E':
		setEuchre();		// setup euchre
		break;
		case 'P':
		setPinochle();		// setup Pinochle
		break;
		default:
		setEuchre();		// setup Default
	}
}
void GameSettings::setEuchre() {	// sets up game with default euchre settings:
	// default euchre settings:
	gType = 'E';			// game type
	MAXPLAYERS = 4;			// max players
	MAXOBSERVERS = 2;		// max observers
	GOAL = 10;				// winning score
	gameName = "Euchre Game";	// default game name
	STATUS = 'W';
	creator = "default builder";
}
void GameSettings::setPinochle() {	// sets up game with default Pinochle settings:
	// default Pinochle settings:
	gType = 'P';			// game type
	MAXPLAYERS = 4;			// max players
	MAXOBSERVERS = 2;		// max observers
	GOAL = 1500;			// winning score
	gameName = "Pinochle Game";	// default game name
}


// ::DATA MANAGEMENT::
std::string GameSettings::print() {		// returns the local info in printable string format
	std::string * temp = new std::string();
	std::string type;
	if (gType == 'E')
		type = "Euchre";
	else if (gType == 'P')
		type = "Pinochle";
	else
		type = "Unknown";

	// convert ints to strings
	char goal[32];
	char maxP[32];
	char ID[32];
	itoa(GOAL, goal, 10);
	itoa(MAXPLAYERS, maxP, 10);
	itoa(GAMEID, ID, 10);

	// add to printing string
	*temp = "ID: ";
	*temp += +ID;
	*temp += " name: " + gameName +
		" status: ";
	temp->append(1, (char)STATUS);
	*temp += " type: " + type +
		" winning score: " + goal +
		" max players: " + maxP +
		" created by: " + creator;

	// return string
	return *temp;
}