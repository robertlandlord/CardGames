#pragma once
#ifndef deck_h
#define deck_h

struct Deck {
	friend ostream& operator<< (ostream&, const Deck&);
	friend Hand& operator<< (Hand& h, Deck& d);
private:
	vector<Card> list;

public:
	Deck();
	Deck(string& filename);
	~Deck();

	int parsefile(string& filename);
	vector<int> draw(int len);
	vector<Card> clear();

	void append(vector<Card> v);
	void addCard(Card c);

	void shuffle();
	const int size() const;
	const string toString() const;
};
ostream& operator<< (ostream& out, const Deck& d);

#endif