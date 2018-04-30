#pragma once
#ifndef hand_h
#define hand_h

struct Deck;

struct Hand {
	friend ostream& operator<< (ostream& out, const Hand& h);
	friend Hand& operator<< (Hand& h, Deck& d);
private:
	vector<Card> list;
public:
	Hand();
	Hand(Hand &other);
	~Hand();

	vector<int> getCards();
	const int size() const;
	const string toString() const;
	void append(vector<int> cards);
	void remove(Card c);
	void remove_card(size_t loc);
	void make_hidden(size_t loc);
	void print_hidden();
	vector<Card> clear();

	void operator= (const Hand& other);
	const bool operator== (const Hand& other) const;
	const bool operator< (const Hand& other) const;
	Card operator[](size_t loc);
};
ostream& operator<< (ostream& out, const Hand& h);

#endif