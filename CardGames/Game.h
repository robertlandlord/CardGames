#pragma once									// include after Player header
#ifndef game_h
#define game_h

class FiveCardDraw;

class Game {
protected:
	static shared_ptr<Game> ptr;
	Deck d;
	vector<Player*> playerList;
	size_t dealer;
	Deck discardedPile;

public:
	unsigned int pot;
	unsigned int bet;
	void bet_round(int round);
	void bet_start();
	static shared_ptr<Game> instance();
	static void start_game(const string& s);
	static void stop_game();
	void add_player(const string s);
	Player* find_player(const string s);
	~Game();

	const int count_player() const;

	virtual int before_turn(Player &p) = 0;
	virtual int turn(Player &p) = 0;
	virtual int after_turn(Player &p) = 0;
	virtual int before_round() = 0;
	virtual int round() = 0;
	virtual int after_round() = 0;
};

#endif