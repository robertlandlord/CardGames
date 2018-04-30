// Lab 3 Combined: all source files are integrated into main cpp

#include "stdafx.h"
#include <array>
#include <algorithm>
#include <vector>
#include <string>
#include <random>
#include <shared_mutex>
#include <iostream>
#include <fstream>
#include <sstream>

#include "support.h"
#include "Card.h"
#include "Hand.h"
#include "Deck.h"
#include "Player.h"
#include "Game.h"
#include "FiveCardDraw.h"
#include "SevenCardStud.h"

using namespace std;

enum funcStatus {															// enumerates main() return types and errors
	success = 0,
	argumentError = 20,
	fileNotFound = 2,
	handSizeError = 3,
	vectorSize = 100,
	invalidCommand = 101,
	noFileGiven = 102,
	tooManyCommands = 103,
	noInputGiven = 104,
	shortDeck = 200,
	noCardsLeft = 263,
	cannotFindGame = 300,
	unknownGame = 301,
	gameAlreadyStarted = 302,
	gameNotInProgress = 303,
	playerAlreadyPlaying = 400,
	gameLogicError = 401,
	generalError = 666
};

int main(int argc, char* argv[])
{
	static int minimum_arguments = 4;
	static string program_name = argv[0];

	if (argc < minimum_arguments) {
		usage(program_name, argumentError);
		return funcStatus::argumentError;
	}
	string game_name = argv[1];

	try {
		Game::start_game(game_name);
		shared_ptr<Game> running_game = Game::instance();
		for (int i = 2; i < argc; ++i) {
			running_game->add_player(argv[i]);
		}
		if ((*running_game).before_round() != 0 || (*running_game).round() != 0 || (*running_game).after_round() != 0) {
			(*running_game).stop_game();
			return gameLogicError;
		}
		running_game->stop_game();
		while (true) {
			cout << "Play new game?  Enter name of game and players to continue" << endl;
			string gameplayers;
			cin.ignore();
			getline(cin, gameplayers);
			istringstream iss(gameplayers);
			string s;
			iss >> s;
			cout << s << endl;
			Game::start_game(s);
			shared_ptr<Game> running_game = Game::instance();
			while (iss >> s) {
				running_game->add_player(s);
			}
			if ((*running_game).before_round() != 0 || (*running_game).round() != 0 || (*running_game).after_round() != 0) {
				(*running_game).stop_game();
				return gameLogicError;
			}
			running_game->stop_game();
		}
		cout << "Invalid game." << endl;
	}
	catch (exception &e) {
		cerr << e.what() << endl;	
		if ((string)e.what() == (string)"instance_not_available") {
			usage(program_name, cannotFindGame);
			return cannotFindGame;
		} 
		if ((string)e.what() == (string)"game_already_started") {
			usage(program_name, gameAlreadyStarted);
			return gameAlreadyStarted;
		}
		if ((string)e.what() == (string)"unknown_game") {
			usage(program_name, unknownGame);
			return unknownGame;
		}
		if ((string)e.what() == (string)"already_playing") {
			usage(program_name, playerAlreadyPlaying);
			return playerAlreadyPlaying;
		}
		return generalError;
	}

}

#pragma region Card

Card::Card(Rank r, Suit s) {
	rank = r;
	suit = s;
	empty = false;
	id = 100 * s + r;
}
Card::Card(int id) {
	if (id / 100 > 4 || id < 100) return;
	if (id % 100 < 2 || id % 100 > 14) return;
	suit = static_cast<Suit>(id / 100);
	rank = static_cast<Rank>(id % 100);
	this->id = id;
	empty = false;
}
Card::Card() {
	empty = true;
	id = 0;
}

const bool Card::isEmpty() const {
	return empty;
}

const string Card::toString() const {
	if (empty) return "";

	string name;
	switch (rank) {
	case Rank::two:
		name += "two";
		break;
	case 3:
		name += "three";
		break;
	case four:
		name += "four";
		break;
	case five:
		name += "five";
		break;
	case six:
		name += "six";
		break;
	case seven:
		name += "seven";
		break;
	case eight:
		name += "eight";
		break;
	case nine:
		name += "nine";
		break;
	case ten:
		name += "ten";
		break;
	case jack:
		name += "jack";
		break;
	case queen:
		name += "queen";
		break;
	case king:
		name += "king";
		break;
	case ace:
		name += "ace";
		break;
	}
	name += " of ";
	switch (suit) {
	case clubs:
		name += "clubs";
		break;
	case diamonds:
		name += "diamonds";
		break;
	case spades:
		name += "spades";
		break;
	case hearts:
		name += "hearts";
		break;
	}
	if (hidden) name += " (hidden)";
	return name;
}

const bool Card::operator<(const Card& c) const {						//taken from c++ forum "Overloading operators"
	if (rank < c.rank) return true;
	if (rank == c.rank && suit < c.suit) return true;
	return false;
}
const bool Card::operator==(const Card& c) const {
	return id == c.id;
}
#pragma endregion Contains Card member methods and insertion operators

#pragma region Deck
Deck::Deck(string& filename) {
	if (parsefile(filename) == funcStatus::fileNotFound) {
		throw std::domain_error("Cannot open file.");
	}
}
// The default destructor is used since no object was created within the struct
Deck::Deck() {

}
Deck::~Deck() {

}

int Deck::parsefile(string& filename) {
	fstream reader;
	reader.open(filename, fstream::in);

	if (!reader) {
		reader.close();
		return funcStatus::fileNotFound;
	}

	string fileString;
	while (!reader.eof()) {
		reader >> fileString;
		Card fileCard = stringToCard(fileString);
		if (!fileCard.empty) {
			list.push_back(fileCard);
		}
	}

	reader.close();
	return funcStatus::success;
}
vector<int> Deck::draw(int len) {
	vector<int> cards;
	for (int i = 0; i < len; ++i) {
		cards.push_back(list[list.size() - 1].id);
		list.pop_back();
	}
	return cards;
}
vector<Card> Deck::clear() {
	vector<Card> release(list);
	list.clear();
	return release;
}

void Deck::append(vector<Card> v) {
	for (Card c : v) list.push_back(c);
}
void Deck::addCard(Card c) {
	list.push_back(c);
}
void Deck::shuffle() {
	srand(unsigned(time(0)));
	std::random_shuffle(list.begin(), list.end());
}

const int Deck::size() const {
	return list.size();
}
const string Deck::toString() const {
	string s;
	for (Card c : list) { s += c.toString(); s += '\n'; }
	return s;
}
#pragma endregion Contains member methods of the struct Deck
ostream& operator<< (ostream& out, const Deck& d) {
	cout << "This Deck contains the following cards:" << endl;
	printCardVector(d.list);
	return out;
}
#pragma region Hand
Hand::Hand() {
}
Hand::Hand(Hand& other) {
	//for (int i : other.getCards()) { list.push_back(Card(i)); }				// saved in case I can't just say "list = other.list"
	list = other.list;
	std::sort(list.begin(), list.end());
}
Hand::~Hand() {}												// no new object was created within the struct's constructors
void Hand::print_hidden() {
	decltype(list.size()) it = 0;

	while (it < list.size()) {
		string cardString = list[it].toString();
		if (list[it].hidden) cardString = "* *";
		cout << cardString << '\n';
		++it;
	}
	cout << endl;
}
vector<int> Hand::getCards() {												// returns a sorted vector of the ids of the cards in this hand
	vector<int> cards;
	if (list.size() == 0) return cards;

	for (Card c : list) { cards.push_back(c.id); }
	std::sort(cards.begin(), cards.end());

	return cards;
}
void Hand::make_hidden(size_t loc) {
	if (!(loc > list.size())) {
		list[loc].hidden = true;
	}
}
const int Hand::size() const {
	return list.size();
}
const string Hand::toString() const {
	string s;
	for (Card c : list) { s += c.toString(); s += '\n'; }
	return s;
}
void Hand::append(vector<int> cards) {
	for (int i : cards) { list.push_back(Card(i)); }
	std::sort(list.begin(), list.end());
}
void Hand::remove(Card c) {
	list.erase(find(list.begin(), list.end(), c));
}
void Hand::remove_card(size_t loc) {
	if (loc > list.size()) {
		throw noCard;
	}
	list.erase(list.begin() + loc);
}
vector<Card> Hand::clear() {
	vector<Card> release(list);
	list.clear();
	return release;
}

void Hand::operator= (const Hand& other) {
	if (&other == this) { return; }
	list = other.list;
	std::sort(list.begin(), list.end());
}
const bool Hand::operator== (const Hand& other) const {
	return other.list == list;
}
const bool Hand::operator< (const Hand& other) const {
	for (int i = 0; i < min(list.size(), other.list.size()); ++i) {
		if (list[i].rank == other.list[i].rank) continue;
		return list[i].rank < other.list[i].rank;
	}

	return(list.size() < other.list.size());
}
Card Hand::operator[](size_t loc) {
	if (loc > list.size() - 1) {
		throw outOfHand;
	}
	return list[loc];

}
#pragma endregion Contains member methods of the struct Hand
ostream& operator<< (ostream& out, const Hand& h) {
	cout << " has the following cards: " << endl;
	printCardVector(h.list);
	return out;
}

Hand& operator<< (Hand& h, Deck& d) {
	if (d.list.size() == 0) throw length_error("Cannot draw card");						// will throw length_error if not enough cards in deck

	Card c = d.list[d.list.size() - 1];
	h.list.push_back(c);
	std::sort(h.list.begin(), h.list.end());
	d.list.pop_back();

	return h;
}

#pragma region Player
ostream & operator<<(ostream &output, const Player &p) {
	output << p.playerName << " " << p.handsWon << " " << p.handsLost << " "<<p.chipCounter << " stayed: "<< p.isPlaying << endl;
	return output;
}

void Player::readPlayerInfo(string &s) {
	ifstream inFile(s);
	int counter = 0;
	if (inFile.is_open()) {  //if the instream is open,
		string line_1, word_1;
		while (getline(inFile, line_1)) {     //takes in first line from the file
			istringstream iss(line_1);    //new stream for strings
			while (iss >> word_1) {
				if (counter == 1) {
					handsWon = stringToInt(word_1);
				}
				if (counter == 2) {
					handsLost = stringToInt(word_1);
				}
				if (counter == 3) {
					chipCounter = stringToInt(word_1);
				}
				counter++;
			}
			if (counter < 3) {
				handsWon = 0;
				handsLost = 0;
			}
		}
	}
	else {
		handsWon = 0;
		handsLost = 0;
	}
};

int Player::stringToInt(string &s) {
	stringstream stream(s);
	int end = 0;
	stream >> end;
	return end;
}
#pragma endregion Contains Player member methods plus insertion operator

#pragma region Game
shared_ptr<Game> Game::ptr = nullptr;
shared_ptr<Game> Game::instance() {
	if (ptr) return ptr;
	throw std::exception("instance_not_available");							// Must catch
}
// instance will throw a general exception
void Game::start_game(const string& s) {
	if (ptr != nullptr) throw std::exception("game_already_started");
	if (s != "FiveCardDraw"&&s!="SevenCardStud") throw std::exception("unknown_game");
	if (s == "FiveCardDraw") {
		ptr = make_shared<FiveCardDraw>();
	}
	if (s == "SevenCardStud") {
		ptr = make_shared<SevenCardStud>();
	}
	
}
// start_game will throw multiple general exceptions
void Game::stop_game() {
	if (!ptr) throw exception("no_game_in_progress");
	ptr = nullptr;
}
// stop_game will throw a general exception
void Game::add_player(const string s) {
	if (find_player(s) != nullptr) throw exception("already_playing");
	Player* ptr = new Player(s);
	playerList.push_back(ptr);

}
// add_player will throw a general exception
Player* Game::find_player(const string s) {

	for (size_t i = 0; i < playerList.size(); ++i) {
		if (playerList[i]->playerName == s) return playerList[i];
	}

	return nullptr;
}
// will possibly return null pointer!
void Game::bet_start() {
	int betPlayer = 0;
	for (Player *p : playerList) {
		cout << p->playerName << endl;
		p->isPlaying = true;
		p->chipCounter--;
		pot++;
		string s;
		if (bet > 0) {
			int chips = 0;
			while (p->isPlaying) {
				if (bet > p->chipCounter) {
					break;
				}
				cout << "The bet is now " << bet << endl;
				cout << "Would you like to call or raise? If call, enter 0. If raise, enter chips to raise." << endl;
				cout << "If you would like to fold, enter 'fold'." << endl;
				cin >> s;
				if (s == "fold") {
					p->isPlaying = false;
					cout << "You have folded" << endl;
					continue;
				}
				if (s.size() != 1) {
					cout << "Invalid input" << endl;
					continue;
				}
				chips = s[0] - '0';
				if (chips == 0)break;
				if (chips == 1 || chips == 2) {
					if (bet + chips > p->chipCounter) {
						chips = p->chipCounter - p->bet;
						p->bet = p->chipCounter;
						if (p->bet > bet) {
							bet = p->bet;
							betPlayer = find(playerList.begin(), playerList.end(), p) - playerList.begin();;
						}
					}
					else {
						betPlayer = find(playerList.begin(), playerList.end(), p) - playerList.begin();;
						bet += chips;
						p->bet = bet;
					}
					break;
				}
				cerr << "Can only enter 0 or 1 or 2 chips";
			}
		}
		s = "";
		if (bet == 0) {
			int chips = 0;
			while (true) {
				if (bet > p->chipCounter) {
					break;
				}
				cout << "Bet is currently " << bet << endl;
				cout << "Would you like to check or bet? If betting, how many chips would you like to bet? If Check, enter '0'. " << endl;
				cin >> s;
				if (s.size() != 1) {
					cout << "Invalid input" << endl;
					continue;
				}
				chips = s[0] - '0';
				if (chips == 0) { break; }
				if (chips == 1 || chips == 2) {
					if (bet + chips > p->chipCounter) {
						chips = p->chipCounter - p->bet;
						p->bet = p->chipCounter;
						if (p->bet > bet) {
							bet = p->bet;
							betPlayer = find(playerList.begin(), playerList.end(), p) - playerList.begin();;
						}
					}
					else {
						betPlayer = find(playerList.begin(), playerList.end(), p) - playerList.begin();;
						bet += chips;
						p->bet = bet;
					}

					break;
				}
				cerr << "Can only enter 1 or 2 chips" << endl;
			}
			cout << "The bet is now " << bet << endl;
		}
		p->bet = bet;
	}
	int turnPlayer = -1;
	while (true) {
		cout << "raising stage1" << endl;
		turnPlayer = (turnPlayer + 1) % playerList.size();
		if (turnPlayer == betPlayer) break;
		Player* p = playerList[turnPlayer];
		if (!(p->isPlaying))continue;
		cout << p->playerName << endl;

		int chips = 0;
		while (true) {
			if (bet > p->chipCounter) {
				break;
			}
			string s;
			cout << "The bet is now " << bet << endl;
			cout << "Would you like to call or raise? If call, enter 0. If raise, enter chips to raise." << endl;
			cout << "If you would like to fold, enter 'fold'." << endl;
			cin >> s;
			if (s == "fold") {
				playerList[turnPlayer]->isPlaying = false;
				cout << "You have folded" << endl;
				break;
			}
			if (s.size() != 1) {
				cout << "Invalid input" << endl;
				continue;
			}
			chips = s[0] - '0';
			if (chips == 0)break;
			if (chips == 1 || chips == 2) {
				if (bet + chips > p->chipCounter) {
					chips = p->chipCounter - p->bet;
					p->bet = p->chipCounter;
					if (p->bet > bet) {
						bet = p->bet;
						betPlayer = turnPlayer;
					}
				}
				else {
					betPlayer = turnPlayer;
					bet += chips;
					p->bet = bet;
				}

				break;
			}
			cerr << "Can only enter 0 or 1 or 2 chips";
		}
	}
}
void Game::bet_round(int round) {
	int betPlayer = 0;
	int turnPlayer = 0;
	bool breaker = false;
	for (int i = 0; i < playerList.size(); ++i) {
		Player* p = playerList[i];
		int chips = 0;
		cout << p->playerName << endl;
		while (true) {
			if (bet > p->chipCounter) {
				break;
			}
			string s;
			cout << "The bet is now " << bet << endl;
			cout << "Would you like to call or raise? If call, enter 0. If raise, enter chips to raise." << endl;
			cout << "If you would like to fold, enter 'fold'." << endl;
			cin >> s;
			if (s == "fold") {
				playerList[turnPlayer]->isPlaying = false;
				cout << "You have folded" << endl;
				break;
			}
			if (s.size() != 1) {
				cout << "Invalid input" << endl;
				continue;
			}
			chips = s[0] - '0';
			if (chips == 0) {
				p->bet = min(bet, p->chipCounter);
				break;
			}
			if (chips == 1 || chips == 2) {
				if (bet + chips > p->chipCounter) {
					chips = p->chipCounter - p->bet;
					p->bet = p->chipCounter;
					if (p->bet > bet) {
						bet = p->bet;
						betPlayer = turnPlayer;
					}
				}
				else {
					betPlayer = turnPlayer;
					bet += chips;
					p->bet = bet;
				}
				breaker = true;
				betPlayer = i;
				break;
			}
			cerr << "Can only enter 0 or 1 or 2 chips";
		}
		if (breaker) break;
	}

	if (!breaker) return;

	turnPlayer = betPlayer;
	while (true) {
		cout << "raising stage1" << endl;
		turnPlayer = (turnPlayer + 1) % playerList.size();
		if (turnPlayer == betPlayer) break;
		Player* p = playerList[turnPlayer];
		if (!(p->isPlaying))continue;
		cout << p->playerName << endl;

		int chips = 0;
		while (true) {
			if (bet > p->chipCounter) {
				break;
			}
			string s;
			cout << "The bet is now " << bet << endl;
			cout << "Would you like to call or raise? If call, enter 0. If raise, enter chips to raise." << endl;
			cout << "If you would like to fold, enter 'fold'." << endl;
			cin >> s;
			if (s == "fold") {
				playerList[turnPlayer]->isPlaying = false;
				cout << "You have folded" << endl;
				break;
			}
			if (s.size() != 1) {
				cout << "Invalid input" << endl;
				continue;
			}
			chips = s[0] - '0';
			if (chips == 0) { 
				p->bet = min(bet, p->chipCounter);
				break; 
			}
			if (chips == 1 || chips == 2) {
				if (bet + chips > p->chipCounter) {
					chips = p->chipCounter - p->bet;
					p->bet = p->chipCounter;
					if (p->bet > bet) {
						bet = p->bet;
						betPlayer = turnPlayer;
					}
				}
				else {
					betPlayer = turnPlayer;
					bet += chips;
					p->bet = bet;
				}

				break;
			}
			cerr << "Can only enter 0 or 1 or 2 chips";
		}
	}

}
Game::~Game() {
	for (Player* p : this->playerList) {
		delete p;
	}
}
const int Game::count_player() const {
	return playerList.size();
}
#pragma endregion Contains Game member methods

#pragma region FiveCardDraw
int FiveCardDraw::before_turn(Player &p) {
	if (!p.isPlaying) return funcStatus::success;
	string discard;
	while (true) {
		bool bigBreaker = false;
		while (true) {
			cin.clear();
			cout << "What card would you like to discard? Press enter if you don't want to discard" << endl;
			cin.ignore();
			getline(cin, discard);
			if (discard.size() == 0) { bigBreaker = true; }
			bool breaker = true;
			for (char c : discard) {
				if (c != 32 && !(c >= 48 && c <= 57)) { breaker = false; };
			}
			if (breaker) { break; }
			cout << "Invalid card position" << endl;
		}
		if (bigBreaker)break;
		string checker;
		stringstream iss1(discard);
		vector<int> vec;
		while (iss1 >> checker) {
			int converted = stoi(checker);
			vec.push_back(converted);
		}
		if ((*std::max_element(vec.begin(), vec.end()) < 6) && (*min_element(vec.begin(), vec.end()) > 0)) break;
		cout << "Invalid input" << endl;
	}

	string word_1;
	stringstream iss(discard);
	vector<Card> tracker;
	while (iss >> word_1) {
		size_t converted = stoi(word_1);
		tracker.push_back(p.playerHand[converted - 1]);
	}
	for (size_t i = 0; i < tracker.size(); ++i) {
		discardedPile.addCard(tracker[i]);
		p.playerHand.remove(tracker[i]);
	}
	for (size_t i = 0; i < tracker.size(); ++i) {
		cout << "You discarded the " << tracker[i].toString() << endl;
	}
	//change value to be removed to some other value
	//run an iterator through it to remove all cards of that value
	return funcStatus::success;
}
int FiveCardDraw::turn(Player&p) {
	if (!p.isPlaying) return funcStatus::success;
	while (p.playerHand.size() < 5) {
		if (d.size() != 0) {
			p.playerHand << d;
		}
		else if (d.size() == 0 && discardedPile.size() != 0) {
			discardedPile.shuffle();
			p.playerHand << discardedPile;
		}
		else {
			return noCardsLeft;
		}
	}
	return funcStatus::success;
}
int FiveCardDraw::after_turn(Player &p) {
	if (!p.isPlaying) return funcStatus::success;
	cout << p.playerName << p.playerHand << endl;
	return funcStatus::success;
}
int FiveCardDraw::before_round() {
	d.shuffle();
	bet = 0;
	pot = 0;
	for (Player *p : playerList) {
		p->chipCounter--;
		pot++;
	}

	bet_round(0);

	
	for (int i = 0; i < 5; ++i) {
		for (size_t j = 0; j < playerList.size(); ++j) {
			if ((*playerList[(j + dealer) % playerList.size()]).isPlaying) {
				(*playerList[(j + dealer) % playerList.size()]).playerHand << d;
			}
		}
	}

	for (size_t j = 0; j < playerList.size(); ++j) {
		Player *p = playerList[(j + dealer) % playerList.size()];
		if (!p->isPlaying) continue;
		cout << p->playerName << p->playerHand << endl;
		before_turn(*playerList[(j + dealer) % playerList.size()]);
	}
	bet_round(0);

	for (Player *p : playerList) {
		pot += p->bet;
		p->chipCounter -= p->bet;
		p->bet = 0;
	}

	return funcStatus::success;
}
int FiveCardDraw::round() {
	for (size_t i = 0; i < playerList.size(); ++i) {
		int z = turn(*playerList[i]);
		if (z != 0) {
			return z;
		}
		after_turn(*playerList[i]);
	}
	return funcStatus::success;
}
int FiveCardDraw::after_round() {
	vector<Player*> copy = playerList;
	sort(copy.begin(), copy.end(), [](Player*a, Player* b) {
		return poker_hand(a->playerHand, b->playerHand);
	});
	(*copy[0]).handsWon++;
	(*copy[0]).chipCounter += pot;
	pot = 0;
	for (size_t i = 1; i < copy.size(); ++i) {
		(*copy[i]).handsLost++;
	}
	for (size_t k = 0; k < copy.size(); ++k) {
		cout << (*copy[k]) << endl;
		cout << (*copy[k]).playerName << (*copy[k]).playerHand << endl;
	}

	for (size_t j = 0; j < copy.size(); ++j) {
		d.append((*playerList[j]).playerHand.clear());
	}
	d.append(discardedPile.clear());
	string s;
	for (Player* p : playerList) {
		string answer;
		if (p->chipCounter < 1) {
			cout << p->playerName<< ", you have no chips left, do you want to restart? If not you will leave" << endl;
			cin >> answer;
			if (answer == "yes" || answer == "Yes") {
				p->chipCounter = 20;
				cout << "Your chips have been reset to 20" << endl;
			}
			else {
				cout << "You have been kicked from the game" << endl;
				ofstream writer;
				writer.open(p->playerName);
				writer << (*find_player(p->playerName));
				writer.close();
				playerList.erase(find(playerList.begin(), playerList.end(), find_player(p->playerName)));
				delete find_player(p->playerName);
			}
		}
	}
	while (playerList.size()>0) {
		cout << "What player wants to leave? enter 'no' if no one wants to leave" << endl;
		cin >> s;
		if (s == "no" || s == "No") break;
		if (find_player(s) == nullptr) {
			cout << "This player is not in the game." << endl;
		}
		if (find_player(s) != nullptr) {
			ofstream writer;
			writer.open(s);
			writer << (*find_player(s));
			writer.close();
			playerList.erase(find(playerList.begin(), playerList.end(), find_player(s)));
			delete find_player(s);
		}

	}
	while (true) {
		cout << "Does anyone want to join? If yes, please enter name of player" << endl;
		cin >> s;
		if (s == "no" || s == "No") break;
		try {
			add_player(s);
		}
		catch (exception& e) { cerr << e.what(); }
		if (playerList[playerList.size() - 1]->chipCounter < 1) {
			Player * p = playerList[playerList.size() - 1];
			string answer;
			cout << p->playerName << ", you have no chips left, do you want to restart? If not you will leave" << endl;
			cin >> answer;
			if (answer == "yes" || answer == "Yes") {
				p->chipCounter = 20;
				cout << "Your chips have been reset to 20" << endl;
			}
			else {
				cout << "You cannot join the game" << endl;
				ofstream writer;
				writer.open(p->playerName);
				writer << (*find_player(p->playerName));
				writer.close();
				playerList.erase(find(playerList.begin(), playerList.end(), find_player(p->playerName)));
				delete find_player(p->playerName);
			}
		}
		if (dealer != playerList.size()) {
			dealer++;
		}
		else {
			dealer = 0;
		}
	}
	return funcStatus::success;
}
#pragma endregion Contains FiveCardDraw member methods

#pragma region SevenCardStud
int SevenCardStud::before_turn(Player &p) {
	if (!p.isPlaying) return funcStatus::success;

	//run an iterator through it to remove all cards of that value
	return funcStatus::success;
}
int SevenCardStud::turn(Player &p) {
	return funcStatus::success;
}
int SevenCardStud::turn1(Player& p) {
	if (!p.isPlaying) return funcStatus::success;
	cout << p.playerName << "'s turn..." << endl;
	while (p.playerHand.size() < 2) {
		if (d.size() != 0) {
			p.playerHand << d;
		}
		else if (d.size() == 0 && discardedPile.size() != 0) {
			discardedPile.shuffle();
			p.playerHand << discardedPile;
		}
		else {
			return noCardsLeft;
		}
	}

	while (p.playerHand.size() < 3) {
		if (d.size() != 0) {
			p.playerHand << d;
			p.playerHand.make_hidden(2);
		}
		else if (d.size() == 0 && discardedPile.size() != 0) {
			discardedPile.shuffle();
			p.playerHand << discardedPile;
			p.playerHand.make_hidden(2);
		}
		else {
			return noCardsLeft;
		}
	}
	return funcStatus::success;
}
int SevenCardStud::turn2(Player& p) {
	if (!p.isPlaying) return funcStatus::success;
	while (p.playerHand.size() < 4) {
		if (d.size() != 0) {
			p.playerHand << d;
		}
		else if (d.size() == 0 && discardedPile.size() != 0) {
			discardedPile.shuffle();
			p.playerHand << discardedPile;
		}
		else {
			return noCardsLeft;
		}
	}
	return funcStatus::success;
}
int SevenCardStud::turn3(Player& p) {
	if (!p.isPlaying) return funcStatus::success;
	while (p.playerHand.size() < 5) {
		if (d.size() != 0) {
			p.playerHand << d;
		}
		else if (d.size() == 0 && discardedPile.size() != 0) {
			discardedPile.shuffle();
			p.playerHand << discardedPile;
		}
		else {
			return noCardsLeft;
		}
	}
	cout << "Opponent cards: " << endl;
	for (Player *others : playerList) {
		if (others->playerName != p.playerName) {
			others->playerHand.print_hidden();
			cout << '\n' << endl;
		}
	}
	cout << p.playerName << ", your hand is: " << p.playerHand << endl;
	return funcStatus::success;
}
int SevenCardStud::turn4(Player& p) {
	if (!p.isPlaying) return funcStatus::success;
	while (p.playerHand.size() < 6) {
		if (d.size() != 0) {
			p.playerHand << d;
		}
		else if (d.size() == 0 && discardedPile.size() != 0) {
			discardedPile.shuffle();
			p.playerHand << discardedPile;
		}
		else {
			return noCardsLeft;
		}
	}
	cout << "Opponent cards: " << endl;
	for (Player *others : playerList) {
		if (others->playerName != p.playerName) {
			others->playerHand.print_hidden();
			cout << '\n' << endl;
		}
	}
	cout << p.playerName << ", your hand is: " << p.playerHand << endl;
	return funcStatus::success;
}
int SevenCardStud::turn5(Player& p) {
	if (!p.isPlaying) return funcStatus::success;
	while (p.playerHand.size() < 7) {
		if (d.size() != 0) {
			p.playerHand << d;
			p.playerHand.make_hidden(6);
		}
		else if (d.size() == 0 && discardedPile.size() != 0) {
			discardedPile.shuffle();
			p.playerHand << discardedPile;
			p.playerHand.make_hidden(6);
		}
		else {
			return noCardsLeft;
		}
	}
	return funcStatus::success;
}
int SevenCardStud::after_turn(Player &p) {
	if (!p.isPlaying) return funcStatus::success;
	cout << p.playerName << p.playerHand << endl;
	return funcStatus::success;
}
int SevenCardStud::before_round() {
	d.shuffle();
	bet = 0;
	pot = 0;
	for (Player *p : playerList) {
		p->chipCounter--;
		pot++;
	}

	bet_round(0);
	

	return funcStatus::success;
}
int SevenCardStud::round() {
	for (Player* p : playerList) {
		int z;
		if (p->isPlaying) z=turn1(*p);
		if (z) return z;
	}
	for (Player *p : playerList) {
		cin.clear();
		displayCards(*p);
		cout << "Press enter to continue" << endl;
		string s;
		cin.ignore();
		getline(cin, s);
	}
	bet_round(1);
	for (Player* p : playerList) {
		int z;
		if (p->isPlaying) z = turn2(*p);
		if (z) return z;
	}
	for (Player *p : playerList) {
		cin.clear();
		displayCards(*p);
		cout << "Press enter to continue" << endl;
		string s;
		cin.ignore();
		getline(cin, s);
	}
	bet_round(2);
	for (Player* p : playerList) {
		int z;
		if (p->isPlaying) z = turn3(*p);
		if (z) return z;
	}
	for (Player *p : playerList) {
		cin.clear();
		displayCards(*p);
		cout << "Press enter to continue" << endl;
		string s;
		cin.ignore();
		getline(cin, s);
	}
	bet_round(3);
	for (Player* p : playerList) {
		int z;
		if (p->isPlaying) z = turn4(*p);
		if (z) return z;
	}
	for (Player *p : playerList) {
		cin.clear();
		displayCards(*p);
		cout << "Press enter to continue" << endl;
		string s;
		cin.ignore();
		getline(cin, s);
	}
	bet_round(4);
	for (Player* p : playerList) {
		int z;
		if (p->isPlaying) z = turn5(*p);
		if (z) return z;
	}
	for (Player *p : playerList) {
		pot += p->bet;
		p->chipCounter -= p->bet;
		p->bet = 0;
	}
	for (Player* p : playerList) {
		int z;
		if (p->isPlaying) z = after_turn(*p);
		if (z) return z;
	}
	return funcStatus::success;
}
int SevenCardStud::after_round() {
	vector<Player*> copy = playerList;
	sort(copy.begin(), copy.end(), [](Player*a, Player* b) {
		return compare_rank(a->playerHand, b->playerHand);
	});
	(*copy[0]).handsWon++;
	(*copy[0]).chipCounter += pot;
	pot = 0;
	for (size_t i = 1; i < copy.size(); ++i) {
		(*copy[i]).handsLost++;
	}
	for (size_t k = 0; k < copy.size(); ++k) {
		cout << (*copy[k]) << endl;
		cout << (*copy[k]).playerName << (*copy[k]).playerHand << endl;
	}

	for (size_t j = 0; j < copy.size(); ++j) {
		d.append((*playerList[j]).playerHand.clear());
	}
	d.append(discardedPile.clear());
	string s;
	for (Player* p : playerList) {
		string answer;
		if (p->chipCounter < 1) {
			cout << p->playerName << ", you have no chips left, do you want to restart? If not you will leave" << endl;
			cin >> answer;
			if (answer == "yes" || answer == "Yes") {
				p->chipCounter = 20;
				cout << "Your chips have been reset to 20" << endl;
			}
			else {
				cout << "You have been kicked from the game" << endl;
				ofstream writer;
				writer.open(p->playerName);
				writer << (*find_player(p->playerName));
				writer.close();
				playerList.erase(find(playerList.begin(), playerList.end(), find_player(p->playerName)));
				delete find_player(p->playerName);
			}
		}
	}
	while (playerList.size()>0) {
		cout << "What player wants to leave? enter 'no' if no one wants to leave" << endl;
		cin >> s;
		if (s == "no" || s == "No") break;
		if (find_player(s) == nullptr) {
			cout << "This player is not in the game." << endl;
		}
		if (find_player(s) != nullptr) {
			ofstream writer;
			writer.open(s);
			writer << (*find_player(s));
			writer.close();
			playerList.erase(find(playerList.begin(), playerList.end(), find_player(s)));
			delete find_player(s);
		}

	}
	while (true) {
		cout << "Does anyone want to join? If yes, please enter name of player" << endl;
		cin >> s;
		if (s == "no" || s == "No") break;
		try {
			add_player(s);
		}
		catch (exception& e) { cerr << e.what(); }
		if (playerList[playerList.size() - 1]->chipCounter < 1) {
			Player * p = playerList[playerList.size() - 1];
			string answer;
			cout << p->playerName << ", you have no chips left, do you want to restart? If not you will leave" << endl;
			cin >> answer;
			if (answer == "yes" || answer == "Yes") {
				p->chipCounter = 20;
				cout << "Your chips have been reset to 20" << endl;
			}
			else {
				cout << "You cannot join the game" << endl;
				ofstream writer;
				writer.open(p->playerName);
				writer << (*find_player(p->playerName));
				writer.close();
				playerList.erase(find(playerList.begin(), playerList.end(), find_player(p->playerName)));
				delete find_player(p->playerName);
			}
		}
		if (dealer != playerList.size()) {
			dealer++;
		}
		else {
			dealer = 0;
		}
	}
	return funcStatus::success;
}
void SevenCardStud::displayCards(Player &p) {
	cout << "Current player: " << p.playerName << endl;
	cout << '\n' << "Opposing player hands:" << endl;
	for (Player *others : playerList) {
		if (others->playerName != p.playerName) {
			others->playerHand.print_hidden();
			cout << '\n' << endl;
		}
	}
	cout << '\n' << "Current hand:" << endl;
	cout << p.playerHand << endl;
}
#pragma endregion Contains SevenCardStud member methods

#pragma region Functions
int parseFile(vector<Card> &list, string filename) {		// Reads file and 5 cards at a time (by line) into a vector
	fstream reader;
	reader.open(filename, fstream::in);

	if (!reader) {
		cerr << "Unable to open file." << endl;
		return funcStatus::fileNotFound;
	}

	string fileLine;
	while (getline(reader, fileLine)) {						// Reads line and checks whether there are 5 cards in line
		std::istringstream iss(fileLine);	//taken from stack overflow "Read file line by line"
		string card;
		Card fileCard; vector<Card> cardVector;

		while (iss >> card) {								// Reads line parsed from file
			if (card.find("//") == 0) {			// if new word begins with "//", break line
				break;
			}
			fileCard = stringToCard(card);
			if (fileCard.empty) {
				cerr << "Error: Invalid card expression found." << endl;
			}
			else {
				cardVector.push_back(fileCard);
			}
		}

		if (cardVector.size() != 5 && cardVector.size() != 0) {			// Checks number of cards to make sure it's 5 (or 0, comment line)
			cerr << "Error: There are a incorrect number of cards in row.  Should be 5;  Has " << cardVector.size() << endl;
		}
		else {
			for (Card c : cardVector) {
				list.push_back(c);
			}
		}
	}

	reader.close();
	return funcStatus::success;
}

bool compare_rank(Hand& h1, Hand&h2) {											// two hands must contain exactly 5 or 7 cards or else will throw domain error
	if (h1.size() == 5 && h2.size() == 5) return poker_hand(h1, h2);

	Hand b1;
	Hand b2;

	for (int i = 0; i < h1.size() - 1; ++i) {
		for (int j = i + 1; j < h1.size(); ++j) {
			vector<int> v = h1.getCards();
			v.erase(v.begin() + j);
			v.erase(v.begin() + i);
			Hand h;
			h.append(v);

			if (poker_hand(h, b1)) b1 = h;
		}
	}

	for (int i = 0; i < h2.size() - 1; ++i) {
		for (int j = i + 1; j < h2.size(); ++j) {
			vector<int> v = h2.getCards();
			v.erase(v.begin() + j);
			v.erase(v.begin() + i);
			Hand h;
			h.append(v);

			if (poker_hand(h, b2)) b2 = h;
		}
	}

	return poker_hand(b1, b2);
}

int printCardVector(vector<Card> v) {								// Prints a vector of Cards into console window
	decltype(v.size()) it = 0;

	while (it < v.size()) {
		string cardString = v[it].toString();
		cout << cardString << '\n';
		++it;
	}
	cout << endl;

	return funcStatus::success;
}

int printOtherVector(vector<Card> v) {								// Prints a vector of Cards of other player's hand
	decltype(v.size()) it = 0;

	while (it < v.size()) {
		string cardString = v[it].toString();
		if (v[it].hidden) cardString = "*";
		cout << cardString << '\n';
		++it;
	}
	cout << endl;

	return funcStatus::success;
}

int usage(string name, int error) {
	cout << "This is " << name << endl
		<< "How to use this program: " << endl;
	cout << "Enter name of the game, followed by name of at least two players." << endl <<
		"Valid game names: FiveCardDraw, SevenCardStud" << endl;
	return error;
}

bool poker_hand(Hand& h1, Hand& h2) {											// returns true iff rank of hand 2 is less than rank of hand 1

	if (h2.size() == 0) return true;
	if (h1.size() == 0) return false;

	vector<int> rank1 = handToRank(h1);											// possibly throws domain_error
	vector<int> rank2 = handToRank(h2);

	rank1.resize(8);
	rank2.resize(8);

	for (int i = 0; i < 8; i++) {
		if (rank2[i] < rank1[i]) return true;
		if (rank1[i] < rank2[i]) return false;
	}
	return false;
}

// Auxiliary Functions 

Card stringToCard(string s) {									// Converts a string, eg "4c", to a card
	int suit = 0;												// returns empty card to be handled by parent function if invalid string
	int rank = 0;

	long it = 0;
	rank = s[it] - '0';
	if (rank == 1) {
		++it;
		if (s[it] == '0') rank = 10;
		else return Card();
	}
	if (rank == 26 || rank == 58) rank = 11;
	if (rank == 33 || rank == 65) rank = 12;
	if (rank == 27 || rank == 59) rank = 13;
	if (rank == 17 || rank == 49) rank = 14;

	++it;

	if (s[it] == 'c' || s[it] == 'C') suit = 1;
	if (s[it] == 'd' || s[it] == 'D') suit = 2;
	if (s[it] == 'h' || s[it] == 'H') suit = 3;
	if (s[it] == 's' || s[it] == 'S') suit = 4;
	if (suit == 0) return Card();

	++it;
	if (it == s.length()) return Card(100 * suit + rank);

	if (s[it] == '/' && s[it + 1] == '/') return Card(100 * suit + rank);
	return Card();
}

vector<int> handToRank(Hand& h) {											// Gives the rank of a hand (of 5) to a vector of ints
	vector<int> list;														// The more determining factors are earlier in the vector
	list = h.getCards();													// ie. the first element in the vector is compared first, then seconds, and etc
	if (list.size() != 5) { throw domain_error("Hand must have 5 cards"); }
	std::sort(list.begin(), list.end(), [](int a, int b) {
		return (a % 100) < (b % 100);
	}
	);

	vector<int> rankCounter(16);
	vector<int> suitCounter(6);
	vector<int> score;

	for (int i : list) {													// Sorts cards into piles
		++rankCounter[i % 100];												// Note that this separates a card's rank from its suit
		++suitCounter[i / 100];
	}																		// some positions of these vectors are always empty
																			// one empty last position is always left for ease of search
																			// for the purpose of easy-to-understand coding
	for (int i = 2; i < 11; ++i) {											// Checks straight flush and straight
		if (rankCounter[i] == 1 && rankCounter[i + 1] == 1
			&& rankCounter[i + 2] == 1 && rankCounter[i + 3] == 1 && rankCounter[i + 4] == 1) {
			if (*std::max_element(suitCounter.begin(), suitCounter.end()) == 5) {
				score.push_back(8);
				score.push_back(list[4] % 100);
				return score;
			}
			score.push_back(4);
			score.push_back(list[4] % 100);
			return score;
		}
	}

	if (*std::max_element(rankCounter.begin(), rankCounter.end()) == 4) {	// Checks for four of a kind
		score.push_back(7);
		auto it = find(rankCounter.begin(), rankCounter.end(), 4);			// from c++ reference bank
		score.push_back(it - rankCounter.begin());							// retrieve position/index of iterator
		for (int i = 0; i < 5; ++i) {
			if (list[4 - i] % 100 != score[1]) score.push_back(list[4 - i] % 100);
		}
		return score;
	}

	if (*std::max_element(rankCounter.begin(), rankCounter.end()) == 3) {	// Checks for full house and three of a kind
		for (int c : rankCounter) {
			if (c == 2) {
				score.push_back(6);
				auto it = find(rankCounter.begin(), rankCounter.end(), 3);
				score.push_back(it - rankCounter.begin());
				it = find(rankCounter.begin(), rankCounter.end(), 2);
				score.push_back(it - rankCounter.begin());
				for (int i = 0; i < 5; ++i) {
					if (list[4 - i] % 100 != score[1] && list[4 - i] % 100 != score[2]) score.push_back(list[4 - i] % 100);
				}
				return score;
			}
		}
		score.push_back(3);
		auto it = find(rankCounter.begin(), rankCounter.end(), 3);
		score.push_back(it - rankCounter.begin());
		for (int i = 0; i < 5; ++i) {
			if (list[4 - i] % 100 != score[1]) score.push_back(list[4 - i] % 100);
		}
		return score;
	}

	if (*std::max_element(suitCounter.begin(), suitCounter.end()) == 5) {	// Checks for flush
		score.push_back(5);
		for (int i = 0; i < 5; ++i) { score.push_back(list[4 - i] % 100); }
		return score;
	}

	if (count(rankCounter.begin(), rankCounter.end(), 2) == 2) {			// Checks for two pairs
		score.push_back(2);
		auto it = find(rankCounter.begin(), rankCounter.end(), 2);
		auto it2 = find(it + 1, rankCounter.end(), 2);
		score.push_back(it2 - rankCounter.begin());
		score.push_back(it - rankCounter.begin());
		it = find(rankCounter.begin(), rankCounter.end(), 1);
		score.push_back(it - rankCounter.begin());
		return score;
	}
	if (count(rankCounter.begin(), rankCounter.end(), 2) == 1) {			// Checks for single pair
		score.push_back(1);
		auto it = find(rankCounter.begin(), rankCounter.end(), 2);
		score.push_back(it - rankCounter.begin());
		for (int i = 0; i < 5; ++i) {
			if (list[4 - i] % 100 != score[1]) score.push_back(list[4 - i] % 100);
		}
		return score;
	}

	score.push_back(0);														// no applicable rank
	for (int i = 0; i < 5; ++i) { score.push_back(list[4 - i] % 100); }
	return score;
}
#pragma endregion Contains various auxiliary functions
