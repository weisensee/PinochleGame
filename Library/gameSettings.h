/* GameSettings.h 	card game AI/Server

	Lucas Weisensee 	February 2015

	Stores Game settings and setup data for a card game
		-specifically pinochle and euchre

	Allows different defaults to be set in one location across the server

	Simplifies the passing of game settings between classes

	Manages defaults and updates game settings
*/
#pragma once

#include <string>

#include "..\Library\Client.h"		// Client communication class


class GameSettings: public Message {
public:
	// ::CONSTRUCTORS AND DESTRUCTORS::
	GameSettings();				// default constructor
	GameSettings(Client * p1);	// constructor
	GameSettings(char TYPE);	// default constructor for game type: 'TYPE'
	~GameSettings();

	// ::GAME STRING PROCESSING::
	void reset();					// resets the game to it's default state
	std::string* settingsString();	// returns the formatted settings string for sending to a server/Client
	void setFromInfoString(const char* newInfoStr);	// set current GameSettings data to match that in info string newData

	// ::SETUP::
	void setup(char TYPE);	// sets up the GameSettings with the default for the passed in type
	void setEuchre();		// sets up game with default euchre settings:
	void setPinochle();		// sets up game with default Pinochle settings:

	
	std::string print();		// returns the local info in printable string format

	Client* player1;				
	std::string * names;			// player names in the game	
	std::string creator;			// game creator
	std::string gameName;			// current game's name
	int MAXOBSERVERS, MAXPLAYERS;	// maximum players and observers in game
	int roundsToPlay;			// amount of times to play current game
	char gMode;					// gameplay mode
	int GOAL;					// winning score
	int GAMEID;
	char gType;				// Game type: 'P': pinochle, 'E': euchre
	char STATUS;			// game status
};