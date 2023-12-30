#ifndef GAME_H
#define GAME_H

// Think about the process.
// Users connect to your localhost and are logged in as valid users.
// Addresses are mapped to player integers (position in an array).
// Should I separate connections and the actual game?
// Let's assume this is only the game representation.
#include <string>
#include <unordered_map>
#include "defs.hpp"
#include "board.hpp"
#include "developmentCards.hpp"

using namespace std;

class Game {
    // Development Cards
    vector<DevelopmentCard> developmentCards;
    // Resource Cards
    unordered_map<Resource, int> resourceCards;
    // Initialize a Board
    // Initialize Players
    int playerCount;
    unordered_map<string, int> playerMap;
    // Initialize knight cards.
    Tile* knightPos = nullptr;
    unordered_map<int, int> knightCount;

    public:
        Game();

        // When a user makes a connection with local computer, add them as a user.
        void addUser(string user);

        // Initialize resource cards.
        void makeResourceCards(int cardsPerResource);

        // Initialize development cards.
        void makeDevelopmentCards();
};

#endif