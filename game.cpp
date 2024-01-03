#include <string>
#include <unordered_map>
#include <algorithm>
#include <random>
#include <iostream>
#include "defs.hpp"
#include "board.hpp"


// To-Do
// Consider how to implement terminal prompting
// Implement Roll 7
// Longest road check logic (only on road placement)

using namespace std;

enum BuildingResource {Town, City, Road};
enum DevelopmentCard {Knight, Monopoly, RoadBuilding, YearOfPlenty, VictoryPoint};

const int WIN_THRESHOLD = 10;
const int printWidth = 6;

class Game {
    // Game State
    bool isActive;
    int turnCount;
    unsigned seed = 0;

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
        Game();

        int addUser() {
            // Add a new entry to playerResourceCards.
            playerResourceCards.push_back(unordered_map<Resource, int>());
            // Add a new entry to playerDevelopmentCards.
            playerDevelopmentCards.push_back(unordered_map<DevelopmentCard, int>());
            // Add a new entry to playerBuildingMaterials.
            playerBuildingResources.push_back(unordered_map<BuildingResource, int> {
                {BuildingResource::Town, 5},
                {BuildingResource::City, 4},
                {BuildingResource::Road, 15}
            });
            // Add a new player score.
            playerScores.push_back(0);

            // Modifies player counter.
            playerCount++;
            return playerCount;
        };

        void makeResourceCards(int cardsPerResource) {
            resourceCards[Resource::Brick] = cardsPerResource;
            resourceCards[Resource::Ore] = cardsPerResource;
            resourceCards[Resource::Sheep] = cardsPerResource;
            resourceCards[Resource::Wheat] = cardsPerResource;
            resourceCards[Resource::Tree] = cardsPerResource;
        }

        void makeDevelopmentCards(){
            for (int i = 0; i < 14; i++) {
                developmentCards.push_back(DevelopmentCard::Knight);
            }
            for (int i = 0; i < 2; i++) {
                developmentCards.push_back(DevelopmentCard::Monopoly);
                developmentCards.push_back(DevelopmentCard::RoadBuilding);
                developmentCards.push_back(DevelopmentCard::YearOfPlenty);
            }
            for (int i = 0; i < 5; i++) {
                developmentCards.push_back(DevelopmentCard::VictoryPoint);
            }

            // Randomize
            shuffle(developmentCards.begin(), developmentCards.end(), default_random_engine(seed));
        }

        void startGame() {
            isActive = true;
        }

        bool hasResources(int player, unordered_map<Resource, int> res) {
            return true;
        }

        void useResources(int player, unordered_map<Resource, int> res) {
            return;
        }

        bool placeRoad(int row, int col, int player) {
            unordered_map<Resource, int> roadResources {
                {Resource::Brick, 1},
                {Resource::Tree, 1}
            };
            bool resourcesValid = hasResources(player, roadResources);
            if (playerBuildingResources[player][BuildingResource::Road] > 0 && resourcesValid) {
                if (board.placeRoad(row, col, player)) {
                    useResources(player, roadResources);
                    return true;
                }
            }
            return false;
        }
        
        bool placeSettlement(int row, int col, int player) {
            unordered_map<Resource, int> settlementResources {
                {Resource::Brick, 1},
                {Resource::Tree, 1},
                {Resource::Wheat, 1},
                {Resource::Sheep, 1}
            };
            bool resourcesValid = hasResources(player, settlementResources);
            if (playerBuildingResources[player][BuildingResource::Town] > 0 && resourcesValid) {
                if (board.placeRoad(row, col, player)) {
                    useResources(player, settlementResources);
                    playerScores[player]++;
                    return true;
                }
            }
            return false;
        }

        bool upgradeSettlement(int row, int col, int player) {
            unordered_map<Resource, int> cityResources {
                {Resource::Ore, 3},
                {Resource::Wheat, 2}
            };
            bool resourcesValid = hasResources(player, cityResources);
            if (playerBuildingResources[player][BuildingResource::City] > 0 && resourcesValid) {
                if (board.placeRoad(row, col, player)) {
                    useResources(player, cityResources);
                    playerScores[player]++;
                    playerBuildingResources[player][BuildingResource::Town]++;
                    return true;
                }
            }
            return false;
        }

        bool buyDevelopmentCard(int player) {
            unordered_map<Resource, int> devResources {
                {Resource::Ore, 1},
                {Resource::Wheat, 1},
                {Resource::Sheep, 1}
            };
            bool resourcesValid = hasResources(player, devResources);
            if (developmentCards.size() > 0 && resourcesValid) {
                DevelopmentCard dev = developmentCards.back();
                developmentCards.pop_back();
                playerDevelopmentCards[player][dev]++;
            }
            return false;
        }

        void moveRobber(int row, int col, int player, bool fromDev) {
            if (fromDev) {
                knightCount[player]++;
            }
            board.moveRobber(row, col);
        }

        void handleDiceRoll() {
            int roll = (rand() % 6) + (rand() % 6) + 2;
            if (roll != 7) {
                // Place Robber In New Location
                // Make Players Discard
            } else {
                unordered_map<int, unordered_map<Resource, int>> newResources = board.rollToResourceCounts(roll);
                for (auto const& [player, playerMap] : newResources) {
                    for (auto const& [res, resCount] : playerMap) {
                        int taken = min(resourceCards[res], resCount);
                        resourceCards[res]-=taken;
                        playerResourceCards[player][res]+=taken;
                    }
                }
            }
        }

        vector<int> getPlayerOrder() {
            vector<int> order(playerCount);
            iota(order.begin(), order.end(), default_random_engine(seed));
            shuffle(order.begin(), order.end(), default_random_engine(seed));
        }

        bool isPlayerWinner(int player) { 
            return playerScores[player] >= WIN_THRESHOLD;
        }

        string getPaddedInt(int valArg) {
            string val = to_string(valArg);
            return val + string(' ', printWidth - val.size());
        }

        void printGameState(int playerView) {
            board.printBoardState();
            cout << endl;

            // All Player Summaries
            cout << "P.   " << "Score  " << "Res.   " << "Dev.   " << endl;
            cout << string('-', 10) << endl;
            for (int i = 0; i < playerCount; i++) {
                string player = getPaddedInt(i);

                string score = getPaddedInt(playerScores[i]);

                int resSum = 0;
                for (const auto& pair : playerResourceCards[i]) {
                    resSum += pair.second;
                }
                string res = getPaddedInt(resSum);

                int devSum = 0;
                for (const auto& pair : playerDevelopmentCards[i]) {
                    devSum += pair.second;
                }
                string dev = getPaddedInt(devSum);
                cout << player << score << res << dev << endl;
            }
            cout << endl;

            // My Information Summary (score resource cards type, dev cards type, building materials)
            cout << "My Information" << endl;
            cout << string('-', 5) << endl;
            // Score
            int modScore = playerScores[playerView] + playerDevelopmentCards[playerView][DevelopmentCard::VictoryPoint];
            cout << "Score - " << modScore << endl;
            // Resource Cards
            cout << "Resources -";
            cout << " Brick: x" << playerResourceCards[playerView][Resource::Brick];
            cout << " Tree: x" << playerResourceCards[playerView][Resource::Tree];
            cout << " Wheat: x" << playerResourceCards[playerView][Resource::Wheat];
            cout << " Sheep: x" << playerResourceCards[playerView][Resource::Sheep];
            cout << " Ore: x" << playerResourceCards[playerView][Resource::Ore];
            cout << endl;
            // Dev Cards
            cout << "DevCards -";
            cout << " Knight: x" << playerDevelopmentCards[playerView][DevelopmentCard::Knight];
            cout << " Monopoly: x" << playerDevelopmentCards[playerView][DevelopmentCard::Monopoly];
            cout << " RoadBuilder: x" << playerDevelopmentCards[playerView][DevelopmentCard::RoadBuilding];
            cout << " YearOfPlenty: x" << playerDevelopmentCards[playerView][DevelopmentCard::YearOfPlenty];
            cout << " VictoryPoint: x" << playerDevelopmentCards[playerView][DevelopmentCard::VictoryPoint];
            cout << endl;
            // Building Materials
            cout << "Build -";
            cout << " Town: x" << playerBuildingResources[playerView][BuildingResource::Town];
            cout << " City: x" << playerBuildingResources[playerView][BuildingResource::City];
            cout << " Road: x" << playerBuildingResources[playerView][BuildingResource::Road];
        }
};
