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

class SettlementJunction;

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
    vector<shared_ptr<RoadJunction>> roads;

    public:
        SettlementJunction() : roads {vector<shared_ptr<RoadJunction>>(6, nullptr)} {}

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

        SettlementJunction* getSettlement(HexPos pos) {
            return settlements[pos].get();
        }
};

union GridPiece {
    Tile* tile;
    SettlementJunction* settlement;
    RoadJunction* road;
};

class Board {
    vector<vector<Tile>> tileGrid;
    map<int, list<Tile*>> rollTileLists;
    vector<vector<GridPiece>> pieceGrid;
    unsigned seed = 0;
    int numTiles {19};
    const vector<int> rowLengths {3,4,5,4,3};

    public:
        shared_ptr<SettlementJunction> getSettlementReference(int row, int col, HexPos pos) {
            Tile& tile = tileGrid[row][col];
            return tile.getSettlements()[pos];
        }

        HexPos getOppositePos(HexPos pos) {
            int posInt = static_cast<int>(pos);
            int oppositeInt = (posInt + 3) % 6;
            return static_cast<HexPos>(oppositeInt);
            // switch (pos) {
            //     case HexPos::Top:
            //         return HexPos::Bottom;
            //     case HexPos::TopRight:
            //         return HexPos::BottomLeft;
            //     case HexPos::TopLeft:
            //         return HexPos::BottomRight;
            //     case HexPos::Bottom:
            //         return HexPos::Top;
            //     case HexPos::BottomRight:
            //         return HexPos::TopLeft;
            //     default:
            //         return HexPos::TopRight; 
            // }
        }

        void makeRoad(shared_ptr<SettlementJunction> s1, shared_ptr<SettlementJunction> s2, HexPos pos) {
            HexPos oppositePos = getOppositePos(pos);
            vector<shared_ptr<RoadJunction>>& roads1 = s1->getRoads();
            if (roads1[pos] == nullptr) {
                shared_ptr<RoadJunction> newRoad = make_shared<RoadJunction>(s1, s2);
                vector<shared_ptr<RoadJunction>>& roads2 = s2->getRoads();
                roads1[pos] = shared_ptr<RoadJunction>(newRoad);
                roads2[oppositePos] = shared_ptr<RoadJunction>(newRoad);
            }
        }
    
        Board() : tileGrid {vector<vector<Tile>>(5)}  {
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
            // Create Default Grid
            vector<vector<GridPiece>> pieceGrid(11, vector<GridPiece>(21));
            // Create Grid Pointers
            int gridRow = 0;
            int gridCol = 0;
            for (int row = 0; row < rowLengths.size(); row++) {
                int rowWidth = rowLengths[row] * 4 + 1;
                gridCol = floor((21 - rowWidth) / 2);
                if (row <= 2) {
                    for (int col = 0; col < rowLengths[row]; col++) {
                        Tile& currTile = tileGrid[row][col];
                        SettlementJunction* tl = currTile.getSettlement(HexPos::TopLeft);
                        GridPiece tlsPiece;
                        tlsPiece.settlement = tl;
                        pieceGrid[gridRow][gridCol] = tlsPiece;
                        gridCol++;

                        GridPiece tlrPiece;
                        tlrPiece.road = tl->getRoad(HexPos::TopRight);
                        pieceGrid[gridRow][gridCol] = tlrPiece;
                        gridCol++;

                        SettlementJunction* t = currTile.getSettlement(HexPos::Top);
                        GridPiece tsPiece;
                        tsPiece.settlement = t;
                        pieceGrid[gridRow][gridCol] = tsPiece;
                        gridCol++;

                        GridPiece trrPiece;
                        trrPiece.road = t->getRoad(HexPos::BottomRight);
                        pieceGrid[gridRow][gridCol] = trrPiece;
                        gridCol++;
                    }
                    Tile& currTile = tileGrid[row][rowLengths[row] - 1];
                    SettlementJunction* tr = currTile.getSettlement(HexPos::TopRight);
                    GridPiece trsPiece;
                    trsPiece.settlement = tr;
                    pieceGrid[gridRow][gridCol] = trsPiece;
                    gridRow++;
                    gridCol = 0;
                }
                for (int col = 0; col < rowLengths[row]; col++) {
                    Tile& currTile = tileGrid[row][col];
                    SettlementJunction* bl = currTile.getSettlement(HexPos::BottomLeft);
                    GridPiece lrPiece;
                    lrPiece.road = bl->getRoad(HexPos::Top);
                    pieceGrid[gridRow][gridCol] = lrPiece;
                    gridCol++;

                    gridCol++;
                    GridPiece tPiece;
                    tPiece.tile = &currTile;
                    pieceGrid[gridRow][gridCol] = tPiece;
                    gridCol++;
                }
                gridRow++;
                gridCol = 0;
                if (row >= 2) {
                    for (int col = 0; col < rowLengths[row]; col++) {
                        Tile& currTile = tileGrid[row][col];
                        SettlementJunction* bl = currTile.getSettlement(HexPos::BottomLeft);
                        GridPiece blsPiece;
                        blsPiece.settlement = bl;
                        pieceGrid[gridRow][gridCol] = blsPiece;
                        gridCol++;

                        GridPiece blrPiece;
                        blrPiece.road = bl->getRoad(HexPos::BottomRight);
                        pieceGrid[gridRow][gridCol] = blrPiece;
                        gridCol++;

                        SettlementJunction* b = currTile.getSettlement(HexPos::Bottom);
                        GridPiece bsPiece;
                        bsPiece.settlement = b;
                        pieceGrid[gridRow][gridCol] = bsPiece;
                        gridCol++;

                        GridPiece brrPiece;
                        brrPiece.road = b->getRoad(HexPos::TopRight);
                        pieceGrid[gridRow][gridCol] = brrPiece;
                        gridCol++;
                    }
                    Tile& currTile = tileGrid[row][rowLengths[row] - 1];
                    SettlementJunction* br = currTile.getSettlement(HexPos::BottomRight);
                    GridPiece brsPiece;
                    brsPiece.settlement = br;
                    pieceGrid[gridRow][gridCol] = brsPiece;
                    gridRow++;
                }
                gridCol = 0;
            }
        }

        int getLongestRoadUser() {
            return -1;
        }

        void printBoardState() {
            // print Grid Representation with Pointers and Ownership
            const int columnWidth = 3;
            const int columnGap = 1;
            const char seaChar = '~';
            // Maybe have a board state grid composed of references that is a union type.
            // First two rows – top two sections
            // Middle row - all sections
            // Bottom two rows - bottom two sections
            for (int row = 0; row < rowLengths.size(); row++) {
                // Top Section (Settlements & Roads)
                string seaEdge = string(0, seaChar);
                if (row <= 2) {
                    cout << seaEdge;
                    for (int col = 0; col < rowLengths[row]; col++) {
                        Tile& currTile = tileGrid[row][col];
                    }
                    cout << seaEdge << endl;
                }
                // Middle Section (Roads & Tiles)
                cout << seaEdge;
                for (int col = 0; col < rowLengths[row]; col++) {
                        
                }
                cout << seaEdge << endl;
                // Bottom Section (Settlements & Roads)
                if (row >= 2) {
                    cout << seaEdge;
                    for (int col = 0; col < rowLengths[row]; col++) {
                        
                    }
                    cout << seaEdge << endl;
                }
            }
            cout << "Hello world.";
        }
};

int main() {
    Board board = Board();
    board.printBoardState();
}