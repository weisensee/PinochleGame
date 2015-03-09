/*	Card Game Mesage ID class 
	Lucas Weisensee 2015

	Stores message IDs for client/server communication
	Message IDs allow use of global constants within messaging classes to represent messages with unique identifiers
	
	see packet specifications.txt for detailed information

	message constants in message.cpp
*/

#pragma once
#include <array>

//******************************
// ::GLOBAL DEFAULTS::
//******************************
#define DEFAULT_HOST "localhost"
#define DEFAULT_PORT 444444

// ::SERVER CONNECTION::
//		CODE		ID 		DESCRIPTION
#define UNKNOWN		99	 //		Unknown message type
#define MESSAGE		100	 // Error/Message to client
#define S_STATUS	1	 // Server Status
#define CON_TYPE	2	 // Client Connection Type
#define R_PNAME 	3	 // Request Player Name
#define PNAME 		4	 // Player Name
#define G_LIST	 	5	 //	Active Game List
#define G_CHOICE 	6	 // Game Choice (connect to)
#define G_CON_STAT	7	 // Game Connection Status and player number
#define RESTART_G 	11	 // Restart query
#define RESTART_ANS 12	 // Restart answer
#define N_GQUERY 	13	 // Query For new Game Creation Info
#define N_GINFO 	14	 // New Game Creation Info
#define PING_QRY 	15	 // Ping for recent activity
#define PING_ANS 	16	 // Answer Ping for recent activity


	// ::GAME SETUP::
#define STAT_QUERY	20	 // Status query of current game
#define G_STATUS	21	 // Status of current game
#define BEC_PLAYER	22	 // Become Player
#define BEC_OBS		24	 // Become Observer
#define PL_POS		25	 // Current Players position in current game

	// ::GAMEPLAY::
#define GAM_START	39 	 // Game Starting
#define HAND_DEALT	41 	 // Hand of cards dealt to player
#define KITTY_FLP 	43 	 // Card Played on the table
#define ORDER_UP	44 	 // Order up answer
#define TRMP_ORD 	45 	 // Trump was ordered notification
#define REQ_PLAY	47	 // Request card play from client
#define PLY_CARD	48	 // Play card on trick
#define ACC_CARD	49	 // Accept card that was played on trick


class message {
public:
	message();
	~message();

	bool isRequest(char n);	// returns true if n is contained in the request library
	bool isRequest(int n);

	std::array<bool, 256>rLibrary;	// true if message code is a request

};