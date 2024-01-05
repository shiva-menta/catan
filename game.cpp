#include "game.hpp"

#include <string>
#include <unordered_map>
#include <algorithm>
#include <random>
#include <iostream>
#include <sstream>
#include "board.hpp"
#include "defs.hpp"

using namespace std;

class Game {
    // Game State
    bool isActive = false;
    int turnCount = 0;
    unsigned seed = 0;

    // Board State
    Board board;
    
    // Player State
    int playerCount = 0;
    vector<int> playerScores;
    vector<unordered_map<BuildingResource, int>> playerBuildingResources;
    vector<int> playerOrder;
    
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
        Game() : board() {}

        int addUser() {
            if (isActive) {
                return -1;
            }
            playerResourceCards.push_back(unordered_map<Resource, int>());
            playerDevelopmentCards.push_back(unordered_map<DevelopmentCard, int>());
            playerBuildingResources.push_back(unordered_map<BuildingResource, int> {
                {BuildingResource::Town, 5},
                {BuildingResource::City, 4},
                {BuildingResource::Road, 15}
            });
            playerScores.push_back(0);

            return playerCount++;
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
            playerOrder = vector<int>(playerCount);
            iota(playerOrder.begin(), playerOrder.end(), 0);
            shuffle(playerOrder.begin(), playerOrder.end(), default_random_engine(seed));
        }

        bool hasResources(int player, unordered_map<Resource, int> res) {
            for (const auto& pair : res) {
                if (pair.second > playerResourceCards[player][pair.first]) {
                    return false;
                }
            }
            return true;
        }
        void useResources(int player, unordered_map<Resource, int> res) {
            for (const auto& pair : res) {
                playerResourceCards[player][pair.first]-=pair.second;
            }
        }

        bool placeRoad(int player, int row, int col, bool firstTurn) {
            unordered_map<Resource, int> roadResources {
                {Resource::Brick, 1},
                {Resource::Tree, 1}
            };
            bool resourcesValid = hasResources(player, roadResources);
            if (playerBuildingResources[player][BuildingResource::Road] > 0 && resourcesValid) {
                if (board.placeRoad(row, col, player, firstTurn)) {
                    useResources(player, roadResources);
                    return true;
                }
            }
            return false;
        }
        
        bool placeSettlement(int player, int row, int col, bool firstTurn) {
            unordered_map<Resource, int> settlementResources {
                {Resource::Brick, 1},
                {Resource::Tree, 1},
                {Resource::Wheat, 1},
                {Resource::Sheep, 1}
            };
            bool resourcesValid = hasResources(player, settlementResources);
            if (playerBuildingResources[player][BuildingResource::Town] > 0 && resourcesValid) {
                if (board.placeSettlement(row, col, player, firstTurn)) {
                    useResources(player, settlementResources);
                    playerScores[player]++;
                    return true;
                }
            }
            return false;
        }

        bool upgradeSettlement(int player, int row, int col) {
            unordered_map<Resource, int> cityResources {
                {Resource::Ore, 3},
                {Resource::Wheat, 2}
            };
            bool resourcesValid = hasResources(player, cityResources);
            if (playerBuildingResources[player][BuildingResource::City] > 0 && resourcesValid) {
                if (board.upgradeSettlement(row, col, player)) {
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

        bool moveRobber(int player, int row, int col, bool fromDev) {
            bool res = board.moveRobber(row, col);
            if (fromDev && res) {
                knightCount[player]++;
            }
            return res;
        }

        vector<bool> playersUnderLimit() {
            vector<bool> playerStatus(playerCount, true);
            for (int i = 0; i < playerCount; i++) {
                int resSum = 0;
                for (const auto& pair : playerResourceCards[i]) {
                    resSum += pair.second;
                }
                playerStatus[i] = resSum <= DISCARD_LIMIT;
            }

            return playerStatus;
        }

        bool discardCardsOverLimit(int player, unordered_map<Resource, int> discards) {
            int removeCards = 0;
            int totalCards = 0;
            for (const auto& pair : playerResourceCards[player]) {
                totalCards += pair.second;
                if (discards.count(pair.first)) {
                    if (discards[pair.first] > playerResourceCards[player][pair.first]) {
                        return false;
                    }
                    removeCards += discards[pair.first];
                }
            }

            if (totalCards - removeCards == 7) {
                useResources(player, discards);
                return true;
            }
            return false;
        };

        int rollDice() {
            return (rand() % 6) + (rand() % 6) + 2;
        }

        void updateResourceCountsFromRoll(int roll) {
            unordered_map<int, unordered_map<Resource, int>> newResources = board.rollToResourceCounts(roll);
            for (auto const& [player, playerMap] : newResources) {
                for (auto const& [res, resCount] : playerMap) {
                    int taken = min(resourceCards[res], resCount);
                    resourceCards[res]-=taken;
                    playerResourceCards[player][res]+=taken;
                }
            }
        }

        bool useKnight(int player, int row, int col) {
            bool hasCard = playerDevelopmentCards[player][DevelopmentCard::Knight] > 0;
            if (hasCard && moveRobber(row, col, player, true)) {
                playerDevelopmentCards[player][DevelopmentCard::Knight]--;
                return true;
            }
            return false;
        };
        bool useMonopoly(int player, Resource res) {
            if (playerDevelopmentCards[player][DevelopmentCard::Monopoly] <= 0) {
                return false;
            }

            int resourceCount = 0;
            for (int i = 0; i < playerResourceCards.size(); i++) {
                resourceCount += playerResourceCards[i][res];
                playerResourceCards[i][res] = 0;
            }
            playerResourceCards[player][res] += resourceCount;
            return true;
        };
        bool useRoadBuilding(int player, int row1, int col1, int row2, int col2) {
            bool hasCard = playerDevelopmentCards[player][DevelopmentCard::RoadBuilding] > 0;
            if (!hasCard) {
                return false;
            }

            bool firstRoad = board.placeRoad(row1, col1, player, false);
            bool secondRoad = board.placeRoad(row2, col2, player, false);

            if (!(firstRoad && secondRoad)) {
                board.removeRoad(row1, col1);
                board.removeRoad(row2, col2);
                playerDevelopmentCards[player][DevelopmentCard::RoadBuilding]--;
            }
            return firstRoad && secondRoad;
        };
        bool useYearOfPlenty(int player, Resource res1, Resource res2) {
            if (playerDevelopmentCards[player][DevelopmentCard::YearOfPlenty] <= 0) {
                return false;
            }

            for (auto resource : vector<Resource> {res1, res2}) {
                if (resourceCards[resource] > 0) {
                    resourceCards[resource]--;
                    playerResourceCards[player][resource]++;
                }
            }
            return true;
        };

        vector<int> getPlayerOrder() {
            return playerOrder;
        }

        bool isPlayerWinner(int player) { 
            return playerScores[player] >= WIN_THRESHOLD;
        }

        void nextTurn() {
            turnCount++;
        }

        int currentTurnPlayer() {
            return playerOrder[turnCount % playerCount];
        }

        int getTurn() {
            return turnCount;
        }

        string getPaddedInt(int valArg) {
            string val = to_string(valArg);
            return val + string(' ', printWidth - val.size());
        }

        string printGameState(int playerView) {
            string boardStr = board.printBoardState();
            stringstream output;
            output << "\n";

            // All Player Summaries
            output << "P.   " << "Score  " << "Res.   " << "Dev.   " << "\n";
            output << string('-', 10) << "\n";
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
                output << player << score << res << dev << "\n";
            }
            output << "\n";

            // My Information Summary (score resource cards type, dev cards type, building materials)
            output << "My Information" << "\n";
            output << string('-', 5) << "\n";
            // Score
            int modScore = playerScores[playerView] + playerDevelopmentCards[playerView][DevelopmentCard::VictoryPoint];
            output << "Score - " << modScore << "\n";
            // Resource Cards
            output << "Resources -";
            output << " Brick: x" << playerResourceCards[playerView][Resource::Brick];
            output << " Tree: x" << playerResourceCards[playerView][Resource::Tree];
            output << " Wheat: x" << playerResourceCards[playerView][Resource::Wheat];
            output << " Sheep: x" << playerResourceCards[playerView][Resource::Sheep];
            output << " Ore: x" << playerResourceCards[playerView][Resource::Ore];
            output << "\n";
            // Dev Cards
            output << "DevCards -";
            output << " Knight: x" << playerDevelopmentCards[playerView][DevelopmentCard::Knight];
            output << " Monopoly: x" << playerDevelopmentCards[playerView][DevelopmentCard::Monopoly];
            output << " RoadBuilder: x" << playerDevelopmentCards[playerView][DevelopmentCard::RoadBuilding];
            output << " YearOfPlenty: x" << playerDevelopmentCards[playerView][DevelopmentCard::YearOfPlenty];
            output << " VictoryPoint: x" << playerDevelopmentCards[playerView][DevelopmentCard::VictoryPoint];
            output << "\n";
            // Building Materials
            output << "Build -";
            output << " Town: x" << playerBuildingResources[playerView][BuildingResource::Town];
            output << " City: x" << playerBuildingResources[playerView][BuildingResource::City];
            output << " Road: x" << playerBuildingResources[playerView][BuildingResource::Road];

            return boardStr + output.str();
        }
};