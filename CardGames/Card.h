#pragma once
#ifndef card_h
#define card_h

struct Card {

	enum Rank {
		two = 2,
		three = 3,
		four = 4,
		five = 5,
		six = 6,
		seven = 7,
		eight = 8,
		nine = 9,
		ten = 10,
		jack = 11,
		queen = 12,
		king = 13,
		ace = 14
	};
	enum Suit {
		clubs = 1,
		diamonds = 2,
		hearts = 3,
		spades = 4
	};

	Card::Suit suit;
	Card::Rank rank;
	bool empty;
	bool hidden = false;
	unsigned int id;

	Card(Card::Rank r, Card::Suit s);
	Card(int id);
	Card();

	const bool isEmpty() const;
	const string toString() const;
	const bool operator<(const Card& c) const;
	const bool operator==(const Card& c) const;
};

#endif