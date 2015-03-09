/* gameSettings.cpp 	card game AI/Server

	Lucas Weisensee 	February 2015

	Functions for gameSettings class: 
		Stores Game settings and setup data for a card game
			-specifically pinochle and euchre

		Allows different defaults to be set in one location across the server

		Simplifies the passing of game settings between classes

		Manages defaults and updates game settings
*/

#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\gameSettings.h"

#define DEFAULT_TYPE 'E'

gameSettings::~gameSettings() {}
gameSettings::gameSettings(client * p1):gameSettings() {				// client constructor
	// copy player one client info
	player1 = p1;
}
gameSettings::gameSettings() {				// default constructor
	// default settings:
	setup(DEFAULT_TYPE);

	// set player1 to NULL
	player1 = NULL;	// default setting is null for first player
}
gameSettings::gameSettings(char TYPE) {	// default constructor for game type: 'TYPE'
	// setup the appropriate game settings for passed in variable
	setup(TYPE);

	// set player1 to NULL
	player1 = NULL;	// default setting is null for first player
}
void gameSettings::setup(char TYPE) {		// sets up the gameSettings with the default for the passed in type
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
void gameSettings::setEuchre() {	// sets up game with default euchre settings:
	// default euchre settings:
	gType = 'E';			// game type
	MAXPLAYERS = 4;			// max players
	MAXOBSERVERS = 2;		// max observers
	GOAL = 10;				// winning score
	gameName = "Euchre Game";	// default game name
}
void gameSettings::setPinochle() {	// sets up game with default Pinochle settings:
	// default Pinochle settings:
	gType = 'P';			// game type
	MAXPLAYERS = 4;			// max players
	MAXOBSERVERS = 2;		// max observers
	GOAL = 1500;			// winning score
	gameName = "Pinochle Game";	// default game name
}
std::string* gameSettings::settingsString() {	// returns the formatted settings string for sending to a server/client
	std::string* settingsInfo = new std::string;
	char temp[24];
	
	// type
	settingsInfo->assign(1, gType);
	settingsInfo->append(1, '^');	// Delimiter 

	// number of players
	settingsInfo->append(itoa(MAXPLAYERS, temp, 10));
	settingsInfo->append(1, '^');	// Delimiter 

	// number of observers	
	settingsInfo->append(itoa(MAXOBSERVERS, temp, 10));
	settingsInfo->append(1, '^');	// Delimiter 

	// winning score
	settingsInfo->append(itoa(GOAL, temp, 10));
	settingsInfo->append(1, '^');	// Delimiter 

	// game name
	settingsInfo->append(gameName);

	// return settings info string
	return settingsInfo;
}