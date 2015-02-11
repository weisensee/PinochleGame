/*	Card Game Mesage ID class 
	Lucas Weisensee 2015

	Stores message IDs for client/server communication
	Message IDs allow use of global constants within messaging classes to represent messages with unique identifiers
	
	see packet specifications.txt for detailed information

	message constants in message.cpp
*/

//		CODE		ID 		DESCRIPTION
#define MSG 		0	 // Error/Message to client
#define S_STATUS	1	 // Server Status
#define CON_TYPE	2	 // Client Connection Type
#define R_PNAME 	3	 // Request Player Name
#define PNAME 		4	 // Player Name
#define G_LIST	 	5	 //	Active Game List
#define G_CHOICE 	6	 // Game Choice (connect to)
#define G_CON_STAT	7	 // Game Connection Status
#define BEC_PLAYER	8	 // Become Player
#define BEC_OBS		9	 // Become Observer
#define CREATE_G	10	 // Create Game
#define RESTART_G 	11	 // Restart query
#define RESTART_ANS 12	 // Restart answer
#define N_GQUERY 	13	 // Query For new Game Creation Info
#define N_GINFO 	14	 // New Game Creation Info
#define RESTART_ANS 12	 // Restart answer
#define RESTART_ANS 12	 // Restart answer



class message {
public:
	message();
	~message();
};