/*	message.cppMessage class -- Pinochle Game Server
Desktop Application
Lucas Weisensee
January 2015

function definitions for message class

message class stores message data as parent to other packet/message sending classes
*/

#include "message.h"

message::message() {
	// initiate message type library
	// set all messages to not requests
	// library value is true if message type is request
	rLibrary.fill(false);

	// ::REQUEST LIBRARY::
	// add any messages that are requests to library
	// A request is a message that requires a specific answer
	// All requests will be stored in the request queue until answered

	// ::SERVER CONNECTION::
	rLibrary[S_STATUS] = true;		// Send Server Status, requesting type of connection
	rLibrary[R_PNAME] = true;		// Request Player Name
	rLibrary[G_LIST] = true; 		// Active Game List
	rLibrary[RESTART_G] = true;	 	// Restart query
	rLibrary[RESTART_ANS] = true; 	// Restart answer
	rLibrary[N_GQUERY] = true;	 	// Query For new Game Creation Info
	rLibrary[N_GINFO] = true; 		// New Game Creation Info
	rLibrary[PING_QRY] = true; 		// Ping for recent activity

	// ::GAME SETUP::
	rLibrary[STAT_QUERY] = true; 	// Status query of current game
	rLibrary[BEC_PLAYER] = true;	// Become Player
	rLibrary[BEC_OBS] = true;  		// Become Observer

	// ::GAMEPLAY::
	rLibrary[ORDER_UP] = true; 	 	// Order up answer
	rLibrary[REQ_PLAY] = true;	 	// Request card play from client
	rLibrary[PLY_CARD] = true;	 	// Play card on trick

}
message::~message(){}