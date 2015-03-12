/*	Message.cppMessage class -- Pinochle Game Server
Desktop Application
Lucas Weisensee
January 2015

function definitions for Message class

Message class stores Message data as parent to other packet/Message sending classes
*/

#include "Message.h"

Message::Message() {
	// initiate Message type library
	// set all Messages to not requests
	// library value is true if Message type is request
	rLibrary.fill(false);

	// ::REQUEST LIBRARY::
	// add any Messages that are requests to library
	// A request is a Message that requires a specific answer
	// All requests will be stored in the request queue until answered

	// ::SERVER CONNECTION::
	rLibrary[S_STATUS] = true;		// Send Server Status, requesting type of connection
	rLibrary[R_PNAME] = true;		// Request Player Name
	rLibrary[G_LIST] = true; 		// Active Game List
	rLibrary[RESTART_G] = true;	 	// Restart Query
	rLibrary[RESTART_ANS] = true; 	// Restart answer
	rLibrary[N_GQuery] = true;	 	// Query For new Game Creation Info
	rLibrary[N_GINFO] = true; 		// New Game Creation Info
	rLibrary[PING_QRY] = true; 		// Ping for recent activity

	// ::GAME SETUP::
	rLibrary[STAT_Query] = true; 	// Status Query of current game
	rLibrary[BEC_PLAYER] = true;	// Become Player
	rLibrary[BEC_OBS] = true;  		// Become Observer

	// ::GAMEPLAY::
	rLibrary[ORDER_UP] = true; 	 	// Order up answer
	rLibrary[REQ_PLAY] = true;	 	// Request Card play from Client
	rLibrary[PLY_Card] = true;	 	// Play Card on trick

}
Message::~Message(){}
