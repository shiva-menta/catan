#include <string>
#include <list>
#include <map>
#include <algorithm>
#include <random>
#include <unordered_set>
#include <iostream>
#include "defs.hpp"

using namespace std;

enum HexPos {Top, TopRight, BottomRight, Bottom, BottomLeft, TopLeft};
const vector<string> roadChars {"|", "/", "\\"};

class SettlementJunction;

class RoadJunction {
    int player = -1;
    vector<shared_ptr<SettlementJunction>> settlements;
    int display;

    public:
        RoadJunction(shared_ptr<SettlementJunction> s1, shared_ptr<SettlementJunction> s2, int hex) : settlements {{s1, s2}}, display {hex} {}

        bool hasRoad() {
            return player != -1;
        }

        int getPlayer() {
            return player;
        }

        int getDisplay() {
            return display;
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
    vector<shared_ptr<RoadJunction>> roads;

    public:
        SettlementJunction() : roads {vector<shared_ptr<RoadJunction>>(6, nullptr)} {}

        bool hasSettlement() {
            return player == -1;
        }

        int getSettlementType() {
            return type;
        }

        int getPlayer() {
            return player;
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

        vector<shared_ptr<RoadJunction>>& getRoads() {
            return roads;
        }

        RoadJunction* getRoad(HexPos pos) {
            return roads[pos].get();
        }
};

class Tile {
    Resource resource;
    int roll;

    vector<shared_ptr<SettlementJunction>> settlements;
    bool hasRobber;

    public:
        Tile (Resource resourceArg, int rollArg) : resource {resourceArg}, roll {rollArg}, settlements {vector<shared_ptr<SettlementJunction>>(6, nullptr)}, hasRobber {false} {}
        
        const Resource getResource() {
            return resource;
        }

        const int getRoll() {
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

        SettlementJunction* getSettlement(HexPos pos) {
            return settlements[pos].get();
        }
};

union GridPiece {
    Tile* tile;
    SettlementJunction* settlement;
    RoadJunction* road;
};

// Custom To Strings
ostream& operator<<(ostream& ostrm, Tile &tile) {
    string roll = to_string(tile.getRoll());
    string padding = roll.size() == 2 ? "" : " ";
    return ostrm << padding << roll << getAbbrev(tile.getResource());
}
ostream& operator<<(ostream& ostrm, SettlementJunction &settlement) {
    string rep;
    int type = settlement.getSettlementType();
    int playerArg = settlement.getPlayer();
    string player = playerArg == -1 ? "" : to_string(playerArg);
    string padding = player.size() == 1 ? "" : " ";
    switch (type) {
        case 0:
            rep = "T";
            break;
        case 1:
            rep = "C";
            break;
        default:
            rep = "o";
            break;
    }
    return ostrm << padding << player << type << " ";
}
ostream& operator<<(ostream& ostrm, RoadJunction &road) {
    int playerArg = road.getPlayer();
    string player = playerArg == -1 ? "" : to_string(playerArg);
    string padding = player.size() == 1 ? "" : " ";
    int display = road.getDisplay();
    return ostrm << padding << player << roadChars[display] << " ";
}

class Board {
    vector<vector<Tile>> tileGrid;
    map<int, list<Tile*>> rollTileLists;
    vector<vector<GridPiece>> pieceGrid;
    unsigned seed = 0;
    int numTiles {19};
    const vector<int> rowLengths {3,4,5,4,3};

    public:
        void makeAssignGridPiece(int row, int col, Tile* tilePtr, SettlementJunction* settlementPtr, RoadJunction* roadPtr) {
            GridPiece piece;
            if (tilePtr != nullptr) {
                piece.tile = tilePtr;
            } else if (settlementPtr != nullptr) {
                piece.settlement = settlementPtr;
            } else {
                piece.road = roadPtr;
            }
            cout << "(" << row << ", " << col << ")" << endl;
            pieceGrid[row][col] = piece;
        }

        void makeAssignTilePiece(int row, int col, Tile* tilePtr) {
            makeAssignGridPiece(row, col, tilePtr, nullptr, nullptr);
        }

        void makeAssignSettlementPiece(int row, int col, SettlementJunction* settlementPtr) {
            makeAssignGridPiece(row, col, nullptr, settlementPtr, nullptr);
        }

        void makeAssignRoadPiece(int row, int col, RoadJunction* roadPtr) {
            makeAssignGridPiece(row, col, nullptr, nullptr, roadPtr);
        }

        shared_ptr<SettlementJunction> getSettlementReference(int row, int col, HexPos pos) {
            Tile& tile = tileGrid[row][col];
            return tile.getSettlements()[pos];
        }

        HexPos getOppositePos(HexPos pos) {
            int posInt = static_cast<int>(pos);
            int oppositeInt = (posInt + 3) % 6;
            return static_cast<HexPos>(oppositeInt);
        }

        void makeRoad(shared_ptr<SettlementJunction> s1, shared_ptr<SettlementJunction> s2, HexPos pos) {
            HexPos oppositePos = getOppositePos(pos);
            vector<shared_ptr<RoadJunction>>& roads1 = s1->getRoads();
            if (roads1[pos] == nullptr) {
                shared_ptr<RoadJunction> newRoad = make_shared<RoadJunction>(s1, s2, static_cast<int>(pos) % 3);
                vector<shared_ptr<RoadJunction>>& roads2 = s2->getRoads();
                roads1[pos] = shared_ptr<RoadJunction>(newRoad);
                roads2[oppositePos] = shared_ptr<RoadJunction>(newRoad);
            }
        }
    
        Board() : tileGrid {vector<vector<Tile>>(5)}, pieceGrid {vector<vector<GridPiece>>(11, vector<GridPiece>(21))} {
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
            for (int i = 2; i < 13; i++) {
                if (i == 7) {
                    continue;
                }
                if (i != 2 && i != 12) {
                    rollStack.push_back(i);
                }
                rollStack.push_back(i);
            }
            shuffle(resourceStack.begin(), resourceStack.end(), default_random_engine(seed));
            shuffle(rollStack.begin(), rollStack.end(), default_random_engine(seed));
            

            // Tile / TileGrid Generation.
            if (resourceStack.size() != numTiles || rollStack.size() != numTiles - 1) {
                cout << "Resource Stack Size: " << resourceStack.size() << "\n";
                cout << "Roll Stack Size: " << rollStack.size() << "\n";
                throw length_error("Incorrect Size Stacks");
            }
            int currentRow = 0;
            while (resourceStack.size() != 0 && rollStack.size() != 0 && currentRow < rowLengths.size()) {
                for (int i = 0; i < rowLengths[currentRow] && resourceStack.size() != 0 && rollStack.size() != 0; i++) {
                    Resource lastResource = resourceStack.back();
                    int lastRoll = rollStack.back();

                    resourceStack.pop_back();
                    if (lastResource == Resource::Sand) {
                        tileGrid[currentRow].push_back(Tile {lastResource, -1});
                    } else {
                        rollStack.pop_back();
                        tileGrid[currentRow].push_back(Tile {lastResource, lastRoll});
                        Tile& newTile = tileGrid[currentRow].back();
                        if (rollTileLists.count(lastRoll)) {
                            rollTileLists[lastRoll].push_back(&newTile);
                        }
                    }
                }
                currentRow++;
            }

            // Junction & Road Generation
            for (int row = 0; row < rowLengths.size(); row++) {
                for (int col = 0; col < rowLengths[row]; col++) {
                    Tile& currTile = tileGrid[row][col];
                    vector<shared_ptr<SettlementJunction>>& currSettlements = currTile.getSettlements();
                    // Debuggging Line.
                    // cout << "(" << row << "," << col << ")" << endl;
                    // cout << "has type: " << (int) currTile.getResource() << endl;
                    // for (int i = 0; i < 6; i++) {
                    //     cout << "address: " << currSettlements[i].get() << endl;
                    //     cout << "pos: " << i << " " << (currSettlements[i] == nullptr) << endl;
                    // }

                    // Generate all Junction Settlements
                    // Top Settlement
                    if (row > 0 and col == rowLengths[row - 1]) {
                        currSettlements[HexPos::Top] = getSettlementReference(row - 1, col - 1, HexPos::BottomRight);
                    } else if (row > 0) {
                        int shift = rowLengths[row] > rowLengths[row - 1] ? 0 : 1;
                        currSettlements[HexPos::Top] = getSettlementReference(row - 1, col + shift, HexPos::BottomLeft);
                    } else {
                        currSettlements[HexPos::Top] = make_shared<SettlementJunction>();
                    }
                    // TopRight Settlement
                    if (row > 0 && col != rowLengths[row - 1]) {
                        int shift = rowLengths[row] > rowLengths[row - 1] ? 0 : 1;
                        currSettlements[HexPos::TopRight] = getSettlementReference(row - 1, col + shift, HexPos::Bottom);
                    } else {
                        currSettlements[HexPos::TopRight] = make_shared<SettlementJunction>();
                    }
                    // BottomRight Settlement
                    currSettlements[HexPos::BottomRight] = make_shared<SettlementJunction>();
                    // Bottom Settlement
                    currSettlements[HexPos::Bottom] = make_shared<SettlementJunction>();
                    // BottomLeft Settlement
                    if (col > 0) {
                        currSettlements[HexPos::BottomLeft] = getSettlementReference(row, col - 1, HexPos::BottomRight);
                    } else {
                        currSettlements[HexPos::BottomLeft] = make_shared<SettlementJunction>();
                    }
                    // TopLeft Settlement
                    if (col > 0) {
                        currSettlements[HexPos::TopLeft] = getSettlementReference(row, col - 1, HexPos::TopRight);
                    } else {
                        if (row == 0 || rowLengths[row] > rowLengths[row - 1]) {
                            currSettlements[HexPos::TopLeft] = make_shared<SettlementJunction>();
                        } else {
                            currSettlements[HexPos::TopLeft] = getSettlementReference(row - 1, col, HexPos::Bottom);
                        }
                    }
                    
                    // Generate all Road Spots
                    makeRoad(currSettlements[0], currSettlements[1], HexPos::BottomRight);
                    makeRoad(currSettlements[1], currSettlements[2], HexPos::Bottom);
                    makeRoad(currSettlements[2], currSettlements[3], HexPos::BottomLeft);
                    makeRoad(currSettlements[3], currSettlements[4], HexPos::TopLeft);
                    makeRoad(currSettlements[4], currSettlements[5], HexPos::Top);
                    makeRoad(currSettlements[5], currSettlements[0], HexPos::TopRight);
                }
            }

            // Create Pointer Grid Representation (11x21)
            // Initialize Grid w/ Nullptrs
            for (int row = 0; pieceGrid.size(); row++) {
                for (int col = 0; pieceGrid[row].size(); col++) {
                    pieceGrid[row][col] = GridPiece {nullptr};
                }
            }
            // Create Grid Pointers
            int gridRow = 0;
            int gridCol = 0;
            for (int row = 0; row < rowLengths.size(); row++) {
                int rowWidth = rowLengths[row] * 4 + 1;
                int rowStart = floor((21 - rowWidth) / 2);
                int gridCol = rowStart;
                if (row <= 2) {
                    for (int col = 0; col < rowLengths[row]; col++) {
                        Tile& currTile = tileGrid[row][col];
                        SettlementJunction* tl = currTile.getSettlement(HexPos::TopLeft);
                        makeAssignSettlementPiece(gridRow, gridCol, tl);
                        gridCol++;
                        
                        makeAssignRoadPiece(gridRow, gridCol, tl->getRoad(HexPos::TopRight));
                        gridCol++;

                        SettlementJunction* t = currTile.getSettlement(HexPos::Top);
                        makeAssignSettlementPiece(gridRow, gridCol, t);
                        gridCol++;

                        makeAssignRoadPiece(gridRow, gridCol, t->getRoad(HexPos::BottomRight));
                        gridCol++;
                    }
                    Tile& currTile = tileGrid[row][rowLengths[row] - 1];
                    SettlementJunction* tr = currTile.getSettlement(HexPos::TopRight);
                    makeAssignSettlementPiece(gridRow, gridCol, tr);
                    gridRow++;
                    gridCol = rowStart;
                }
                for (int col = 0; col < rowLengths[row]; col++) {
                    Tile& currTile = tileGrid[row][col];
                    SettlementJunction* bl = currTile.getSettlement(HexPos::BottomLeft);
                    makeAssignRoadPiece(gridRow, gridCol, bl->getRoad(HexPos::Top));
                    gridCol+=2;

                    makeAssignTilePiece(gridRow, gridCol, &currTile);
                    gridCol+=2;
                }
                Tile& currTile = tileGrid[row][rowLengths[row] - 1];
                SettlementJunction* br = currTile.getSettlement(HexPos::BottomRight);
                makeAssignRoadPiece(gridRow, gridCol, br->getRoad(HexPos::Top));
                gridRow++;
                gridCol = rowStart;
                if (row >= 2) {
                    for (int col = 0; col < rowLengths[row]; col++) {
                        Tile& currTile = tileGrid[row][col];
                        SettlementJunction* bl = currTile.getSettlement(HexPos::BottomLeft);
                        makeAssignSettlementPiece(gridRow, gridCol, bl);
                        gridCol++;

                        makeAssignRoadPiece(gridRow, gridCol, bl->getRoad(HexPos::BottomRight));
                        gridCol++;

                        SettlementJunction* b = currTile.getSettlement(HexPos::Bottom);
                        makeAssignSettlementPiece(gridRow, gridCol, b);
                        gridCol++;

                        makeAssignRoadPiece(gridRow, gridCol, b->getRoad(HexPos::TopRight));
                        gridCol++;
                    }
                    Tile& currTile = tileGrid[row][rowLengths[row] - 1];
                    SettlementJunction* br = currTile.getSettlement(HexPos::BottomRight);
                    makeAssignSettlementPiece(gridRow, gridCol, br);
                    gridRow++;
                }
                gridCol = 0;
            }
        }

        int getLongestRoadUser() {
            return -1;
        }

        void printBoardState() {
            // for (int row = 0; row < pieceGrid.size(); row++) {
            //     for (int col = 0; col < pieceGrid[row].size(); col++) {
            //         if (pieceGrid[row][col].tile == nullptr) {
            //             cout << "~~~";
            //         } else {
            //             cout <<
            //         }
            //         cout << " ";
            //     }
            //     cout << endl;
            // }
            return;
        }
};

int main() {
    Board board = Board();
    board.printBoardState();
}