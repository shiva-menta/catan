#ifndef GAME_H
#define GAME_H

#include <string>
#include <unordered_map>
#include "defs.hpp"
#include "board.hpp"

using namespace std;

enum BuildingResource {Town, City, Road};
enum DevelopmentCard {Knight, Monopoly, RoadBuilding, YearOfPlenty, VictoryPoint};

const int WIN_THRESHOLD;
const int DISCARD_LIMIT;

class Game {
    // Game State
    bool isActive;
    int turnCount;
    unsigned seed;

    // Board State
    Board board;
    
    // Player State
    int playerCount;
    vector<int> playerScores;
    vector<unordered_map<BuildingResource, int>> playerBuildingResources;
    
    // Resource Cards
    unordered_map<Resource, int> resourceCards;
    vector<unordered_map<Resource, int>> playerResourceCards;

    // Development Cards
    vector<DevelopmentCard> developmentCards;
    vector<unordered_map<DevelopmentCard, int>> playerDevelopmentCards;

    // Knight
    Tile* knightPos;
    unordered_map<int, int> knightCount;

    public:
        // Initialize board resources.
        Game();

        // Add user to the current game if active.
        int addUser();

        // Initialize resource card stack.
        void makeResourceCards(int cardsPerResource);

        // Initialize development card stack.
        void makeDevelopmentCards();

        // Start game turns. Needs to handle logic of processing special first turn as well.
        void startGame();

        // Place Road
        bool placeRoad(int player, int row, int col, bool firstTurn);

        // Place Town
        bool placeSettlement(int player, int row, int col, bool firstTurn);

        // Place City
        bool placeCity(int player, int row, int col);

        // Buy Development Card
        bool buyDevelopmentCard(int player, DevelopmentCard card);

        // Move Robber
        bool moveRobber(int player, int row, int col, bool fromDev);

        // Discard Cards
        vector<bool> playersUnderLimit();
        bool discardCardsOverLimit(int player, unordered_map<Resource, int> cards);

        // Handle Roll Dice
        int rollDice();
        void updateResourceCountsFromRoll(int roll);

        // Development Card Actions
        bool useKnight(int player, int row, int col);
        bool useMonopoly(int player, Resource res);
        bool useRoadBuilding(int player, int row1, int col1, int row2, int col2);
        bool useYearOfPlenty(int player, Resource res1, Resource res2);

        // Decide Player Order
        vector<int> getPlayerOrder();

        // Check Win Condition
        bool isPlayerWinner(int player);

        // End Turn
        void nextTurn();

        // Player of Current Turn
        int currentTurnPlayer();

        // Current Turn
        int getTurn();

        // Print user view.
        string printGameState(int player);
};

int main();

#endif