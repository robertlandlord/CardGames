#pragma once
#ifndef player_h
#define player_h

struct Player {
	unsigned int bet;
	bool isPlaying;
	string playerName;
	Hand playerHand;
	unsigned int handsWon;
	unsigned int handsLost;
	unsigned int chipCounter;
	Player(string name):playerName(name),handsWon(0),handsLost(0), chipCounter(20), isPlaying(true), bet(0) {
		//open file that has same name as player
		//if success, read wins, losses, assigns them
		readPlayerInfo(playerName);
	}
	void readPlayerInfo(string &s);
	int stringToInt(string &s);
	
};
ostream & operator<<(ostream &output, const Player &p);
#endif