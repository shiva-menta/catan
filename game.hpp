#ifndef GAME_H
#define GAME_H

#include <string>
#include <unordered_map>
#include <algorithm>
#include <random>
#include <iostream>
#include <sstream>
#include "board.hpp"
#include "defs.hpp"

enum BuildingResource {Town, City, Road};

const int WIN_THRESHOLD;
const int DISCARD_LIMIT;
const int printWidth;

class Game {
    // Game State
    bool isActive;
    int turnCount;
    unsigned seed;

    // Board State
    Board board;
    
    // Player State
    int playerCount;
    std::vector<int> playerScores;
    std::vector<std::unordered_map<BuildingResource, int>> playerBuildingResources;
    std::vector<int> playerOrder;
    
    // Resource Cards
    std::unordered_map<Resource, int> resourceCards;
    std::vector<std::unordered_map<Resource, int>> playerResourceCards;

    // Development Cards
    std::vector<DevelopmentCard> developmentCards;
    std::vector<std::unordered_map<DevelopmentCard, int>> playerDevelopmentCards;

    // Knight
    Tile* knightPos;
    std::unordered_map<int, int> knightCount;

    public:
        // Add user to the current game if active.
        int addUser();

        // Initialize resource card stack.
        void makeResourceCards(int cardsPerResource);

        // Initialize development card stack.
        void makeDevelopmentCards();

        // Start game turns. Needs to handle logic of processing special first turn as well.
        void startGame();

        // Resource Checks
        bool hasResources(int player, std::unordered_map<Resource, int> res);
        void useResources(int player, std::unordered_map<Resource, int> res);

        // Place Road
        bool placeRoad(int player, int row, int col, bool firstTurn);

        // Place Town
        bool placeSettlement(int player, int row, int col, bool firstTurn);

        // Place City
        bool upgradeSettlement(int player, int row, int col);

        // Buy Development Card
        bool buyDevelopmentCard(int player, DevelopmentCard card);

        // Move Robber
        bool moveRobber(int player, int row, int col, bool fromDev);

        // Discard Cards
        std::vector<bool> playersUnderLimit();
        bool discardCardsOverLimit(int player, std::unordered_map<Resource, int> cards);

        // Handle Roll Dice
        int rollDice();
        void updateResourceCountsFromRoll(int roll);

        // Development Card Actions
        bool useKnight(int player, int row, int col);
        bool useMonopoly(int player, Resource res);
        bool useRoadBuilding(int player, int row1, int col1, int row2, int col2);
        bool useYearOfPlenty(int player, Resource res1, Resource res2);

        // Decide Player Order
        std::vector<int> getPlayerOrder();

        // Check Win Condition
        bool isPlayerWinner(int player);

        // End Turn
        void nextTurn();

        // Player of Current Turn
        int currentTurnPlayer();

        // Current Turn
        int getTurn();

        // Helper
        std::string getPaddedInt(int valArg);

        // Print user view.
        std::string printGameState(int player);
};

#endif