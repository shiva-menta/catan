#include <string>
#include <list>
#include <map>
#include <stack>
#include <algorithm>
#include <random>
#include <unordered_set>

using namespace std;

enum class Resource {Brick, Wood, Sheep, Wheat, Ore, Sand};
enum HexPos {Top, TopRight, BottomRight, Bottom, BottomLeft, TopLeft};

class Tile {
    Resource resource;
    int roll;

    vector<shared_ptr<SettlementJunction>> settlements;
    bool hasRobber;

    public:
        Tile (Resource resourceArg, int rollArg) : resource {resourceArg}, roll {rollArg}, settlements {vector<shared_ptr<SettlementJunction>>(6, nullptr)}, hasRobber {false} {}
        
        Resource getResource() {
            return resource;
        }

        int getRoll() {
            return roll;
        }

        bool removeRobber() {
            bool tempHasRobber {hasRobber};
            hasRobber = false;
            return tempHasRobber;
        }

        bool placeRobber() {
            bool tempHasRobber {hasRobber};
            hasRobber = true;
            return tempHasRobber;
        }

        vector<shared_ptr<SettlementJunction>>& getSettlements() {
            return settlements;
        }
};

class RoadJunction {
    int player = -1;
    vector<shared_ptr<SettlementJunction>> settlements;

    public:
        RoadJunction(shared_ptr<SettlementJunction> s1, shared_ptr<SettlementJunction> s2) : settlements {{s1, s2}} {}

        bool hasRoad() {
            return player != -1;
        }

        bool placeRoad(int playerArg) {
            if (player == -1) {
                player = playerArg;
            }
            return player != -1;
        }

        vector<shared_ptr<SettlementJunction>>& getSettlements() {
            return settlements;
        }
};

class SettlementJunction {
    int player = -1;
    int type = -1;
    unordered_set<shared_ptr<RoadJunction>> roads;

    public:
        bool hasSettlement() {
            return player == -1;
        }

        int settlementType() {
            return type;
        }

        bool placeSettlement(int playerArg) {
            if (player == -1) {
                player = playerArg;
                type = 0;
            }
            return player == -1;
        }

        bool upgradeSettlement(int playerArg) {
            int prevType {type};
            if (prevType == 0) {
                type = 1;
            }
            return prevType == 0;
        }

        unordered_set<shared_ptr<RoadJunction>>& getRoads() {
            return roads;
        }
};

class Board {
    vector<vector<Tile>> tileGrid;
    map<int, list<Tile*>> rollTileLists;
    unsigned seed = 0;
    int numTiles {19};
    const vector<int> rowLengths {3,4,5,4,3};

    public:
        shared_ptr<SettlementJunction> getSettlementReference(int row, int col, HexPos pos) {
            Tile& tile = tileGrid[row][col];
            return tile.getSettlements()[pos];
        }

        void makeRoad(shared_ptr<SettlementJunction> s1, shared_ptr<SettlementJunction> s2) {
            unordered_set<shared_ptr<RoadJunction>>& roads1 = s1->getRoads();
            bool hasRoad = false;
            for (const auto& road : roads1) {
                auto matchedSettlements = road->getSettlements();
                if ((matchedSettlements[0] == s1 && matchedSettlements[1] == s2) || (matchedSettlements[1] == s1 && matchedSettlements[0] == s2)) {
                    hasRoad = true;
                    break;
                }
            }

            if (!hasRoad) {
                shared_ptr<RoadJunction> newRoad = make_shared<RoadJunction>(s1, s2);
                unordered_set<shared_ptr<RoadJunction>>& roads2 = s2->getRoads();
                roads1.insert(shared_ptr<RoadJunction>(newRoad));
                roads2.insert(shared_ptr<RoadJunction>(newRoad));
            }
        }
    
        Board() : tileGrid {vector<vector<Tile>>(rowLengths.size())}  {
            // Initialize Tile Value Stacks.
            map<Resource, int> resourceTiles = {
                {Resource::Brick, 3},
                {Resource::Wood, 4},
                {Resource::Sheep, 4},
                {Resource::Wheat, 4},
                {Resource::Ore, 3},
                {Resource::Sand, 1}
            };
            vector<Resource> resourceStack;
            for (auto const& [key, val] : resourceTiles) {
                for (int i = 0; i < val; i++) {
                    resourceStack.push_back(key);
                }
            }
            vector<int> rollStack;
            for (int i = 1; i < 13; i++) {
                if (i != 2 && i != 12) {
                    rollStack.push_back(i);
                } else {
                    continue;
                }
                rollStack.push_back(i);
            }
            shuffle(resourceStack.begin(), resourceStack.end(), default_random_engine(seed));
            shuffle(rollStack.begin(), rollStack.end(), default_random_engine(seed));
            

            // Tile / TileGrid Generation.
            if (resourceStack.size() != numTiles || rollStack.size() != numTiles) {
                throw length_error("Incorrect Size Stacks");
            }
            size_t currentRow = 0;
            while (resourceStack.size() != 0 && rollStack.size() != 0 && currentRow < rowLengths.size()) {
                for (int i = 0; i < rowLengths[currentRow] && resourceStack.size() != 0 && rollStack.size() != 0; i++) {
                    Resource lastResource = resourceStack.back();
                    int lastRoll = rollStack.back();

                    resourceStack.pop_back();
                    rollStack.pop_back();

                    tileGrid[currentRow].push_back(Tile {lastResource, lastRoll});
                    Tile& newTile = tileGrid[currentRow].back();
                    if (rollTileLists.count(lastRoll)) {
                        rollTileLists[lastRoll].push_back(&newTile);
                    } else {
                        rollTileLists[lastRoll] = list<Tile*> {&newTile};
                    }
                }
                currentRow++;
            }


            // Junction & Road Generation
            for (int row = 0; row < rowLengths.size(); row++) {
                for (int col = 0; col < rowLengths[row]; col++) {
                    Tile& currTile = tileGrid[row][col];
                    auto currSettlements = currTile.getSettlements();

                    // Generate all Junction Settlements
                    // Top Settlement
                    if (row > 0) {
                        int shift = rowLengths[row] > rowLengths[row - 1] ? 0 : 1;
                        currSettlements[HexPos::Top] = shared_ptr<SettlementJunction> {getSettlementReference(row - 1, col + shift, HexPos::BottomLeft)};
                    } else {
                        currSettlements[HexPos::Top] = make_shared<SettlementJunction>();
                    }
                    // TopRight Settlement
                    if (row > 0) {
                        int shift = rowLengths[row] > rowLengths[row - 1] ? 0 : 1;
                        currSettlements[HexPos::TopRight] = shared_ptr<SettlementJunction> {getSettlementReference(row - 1, col + shift, HexPos::Bottom)};
                    } else {
                        currSettlements[HexPos::TopRight] = make_shared<SettlementJunction>();
                    }
                    // BottomRight Settlement
                    currSettlements[HexPos::BottomRight] = make_shared<SettlementJunction>();
                    // Bottom Settlement
                    currSettlements[HexPos::Bottom] = make_shared<SettlementJunction>();
                    // BottomLeft Settlement (only will be shared with the left tile)
                    if (col > 0) {
                        currSettlements[HexPos::BottomLeft] = shared_ptr<SettlementJunction> {getSettlementReference(row, col - 1, HexPos::BottomRight)};
                    } else {
                        currSettlements[HexPos::BottomLeft] = make_shared<SettlementJunction>();
                    }
                    // TopLeft Settlement
                    if (col > 0) {
                        currSettlements[HexPos::TopLeft] = shared_ptr<SettlementJunction> {getSettlementReference(row, col - 1, HexPos::TopRight)};
                    } else {
                        if (row == 0 || rowLengths[row] > rowLengths[row - 1]) {
                            currSettlements[HexPos::TopLeft] = make_shared<SettlementJunction>();
                        } else {
                            currSettlements[HexPos::TopLeft] = shared_ptr<SettlementJunction> {getSettlementReference(row - 1, col, HexPos::Bottom)};
                        }
                    }
                    
                    // Generate all Road Spots
                    for (int i = 0; i < 6; i++) {
                        int next = i + 1 == 6 ? 0 : i + 1;
                        makeRoad(currSettlements[i], currSettlements[next]);
                    }
                }
            }
        }

        int getLongestRoadUser() {
            return -1;
        }
};