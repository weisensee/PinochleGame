/* gameSettings.h 	card game AI/Server

	Lucas Weisensee 	February 2015

	Stores Game settings and setup data for a card game
		-specifically pinochle and euchre

	Allows different defaults to be set in one location across the server

	Simplifies the passing of game settings between classes

	Manages defaults and updates game settings
*/
#pragma once

#include <string>

#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\message.h"
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\client.h"		// Client communication class


class gameSettings: public message {
public:
	gameSettings();				// default constructor
	gameSettings(client * p1);	// constructor
	gameSettings(char TYPE);	// default constructor for game type: 'TYPE'
	~gameSettings();
	std::string* settingsString();	// returns the formatted settings string for sending to a server/client
	void setup(char TYPE);		// sets up the gameSettings with the default for the passed in type
	void setEuchre();	// sets up game with default euchre settings:
	void setPinochle();	// sets up game with default Pinochle settings:

	client* player1;				//
	std::string gameName;			// current game's name
	int MAXOBSERVERS, MAXPLAYERS;	// maximum players and observers in game
	int GOAL;					// winning score
	char gType;				// Game type: 'P': pinochle, 'E': euchre
};