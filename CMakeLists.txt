cmake_minimum_required(VERSION 3.13)
project(CardGames)

set(CMAKE_CXX_STANDARD 14)

include_directories(CardGames)

add_executable(CardGames
        CardGames/Card.h
        CardGames/CardGames.cpp
        CardGames/Deck.h
        CardGames/FiveCardDraw.h
        CardGames/Game.h
        CardGames/Hand.h
        CardGames/Player.h
        CardGames/SevenCardStud.h
        CardGames/stdafx.cpp
        CardGames/stdafx.h
        CardGames/support.h
        CardGames/targetver.h CardGames/Game.cpp CardGames/Card.cpp)
