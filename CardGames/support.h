#pragma once
#ifndef support_h
#define support_h

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <ctime>
#include <cstdlib>

using namespace std;

struct Card;
struct Hand;

int parseFile(vector<Card> &list, string filename);
int printCardVector(vector<Card> v);
int usage(string name, int error);
bool poker_hand(Hand& h1, Hand& h2);
bool compare_rank(Hand& h1, Hand& h2);

Card stringToCard(string s);
vector<int> handToRank(Hand& h);

enum result {
	notOpen, shuffleOnly, twoNoShuffle, noCommand, moreThanTwo, deckTooSmall, outOfHand, noCard, failure,
};

#endif#pragma once
