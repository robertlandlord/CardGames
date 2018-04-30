#pragma once
#ifndef fivecarddraw_h
#define fivecarddraw_h

class FiveCardDraw : public Game {

protected:
	size_t dealer;
	Deck discardedPile;
public:
	FiveCardDraw() :dealer(0) {
		for (int i = 1; i < 5; ++i) {
			for (int j = 2; j < 15; ++j) {
				Card newCard = Card(100 * i + j);
				d.addCard(newCard); 
			}
		}
	}
	
	virtual int before_turn(Player &p);
	virtual int turn(Player &p);
	virtual int after_turn(Player &p);
	virtual int before_round();
	virtual int round();
	virtual int after_round();
};

#endif