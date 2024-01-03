#ifndef GAME_H
#define GAME_H

#include <string>
#include <unordered_map>
#include "defs.hpp"
#include "board.hpp"

using namespace std;

enum BuildingResource {Town, City, Road};
// (14 Knight/Soldier Cards, 6 Progress Cards, 5 Victory Point Cards).
enum DevelopmentCard {Knight, Monopoly, RoadBuilding, YearOfPlenty, VictoryPoint};

const int WIN_THRESHOLD;

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
        // Initialize playerCount - keeps track of number of players in game.
        // Initialize turnCount - keeps track of which player's turn it is.
        // Initialize scoreboard - keeps track of score of all players.
        // Initialize full stack of resource cards.
        // Initialize full stack of development cards.
        Game();

        int addUser();

        // Initialize resource card stack.
        void makeResourceCards(int cardsPerResource);

        // Initialize development card stack.
        void makeDevelopmentCards();

        // Start game turns. Needs to handle logic of processing special first turn as well.
        void startGame();

        // Place Road
        bool placeRoad(int row, int col, int player, bool firstTurn);

        // Place Town
        bool placeSettlement(int row, int col, int player, bool firstTurn);

        // Place City
        bool placeCity(int row, int col, int player);

        // Buy Development Card
        bool buyDevelopmentCard(int player);

        // Move Robber
        void moveRobber(int row, int col, int player, bool fromDev);

        // Handle Roll Dice
        void handleDiceRoll();

        // Decide Player Order
        vector<int> getPlayerOrder();

        // Check Win Condition
        bool isPlayerWinner(int player);

        // Print user view.
        void printGameState(int player);
};

int main();

#endif