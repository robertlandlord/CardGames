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
