/*	pinochleRound.h -- Online Pinochle Game -- Lucas Weisensee November 2014

		Complete Pinochle Round/Hand Record

		This object holds a complete round or hand of a PLAYED pinochle game

		SEE pinochleHand.h for active game record
	
		-winning bid
		-bid winner
		-cards played
*/
		
class pinochleRound
{
public:
	pinochleRound();
	~pinochleRound();
	int playcard(char card);
private:
	char bid;
	char bidwinner;
	char cardsPassed[4];
	char cardsplayed[48];
};