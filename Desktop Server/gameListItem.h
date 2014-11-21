/*	gameListItem.h -- Online Pinochle Game -- Lucas Weisensee November 2014

	stores game items for active game list

*/

class gameListItem
{
public:
	gameListItem(arguments);
	~gameListItem();

	HANDLE * gameHandles;				//thread handles for active games array
	char * playerNames;					//first player/creating player name array
	char * gameNames;					//game name array
	bool currentListReady;				//true if the current list is ready to be sent
	char * currentList;					//current active list if recently generated
	int * gameID;						//ID number for specific Pinochle Game
};