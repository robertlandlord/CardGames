//
// Created by rober on 3/4/2019.
//
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

#pragma region Game
shared_ptr<Game> Game::ptr = nullptr;
shared_ptr<Game> Game::instance() {
    if (ptr) return ptr;
    throw std::exception("instance_not_available");							// Must catch
}
// instance will throw a general exception
void Game::start_game(const string& s) {
    if (ptr != nullptr) throw std::exception("game_already_started");
    if (s != "FiveCardDraw"&&s != "SevenCardStud") throw std::exception("unknown_game");
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
