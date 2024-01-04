#include <string>
#include <map>
#include <algorithm>
#include <random>
#include <unordered_set>
#include <iostream>
#include <sstream>
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

        int getPlayer() {
            return player;
        }

        int getDisplay() {
            return display;
        }

        bool hasRoad() {
            return player != -1;
        }

        bool placeRoad(int playerArg) {
            if (player == -1) {
                player = playerArg;
                return true;
            }
            return false;
        }

        bool removeRoad() {
            if (player != -1) {
                player = -1;
                return true;
            }
            return false;
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
            return player != -1;
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
                return true;
            }
            return false;
        }

        bool upgradeSettlement(int playerArg) {
            if (player == playerArg && type == 0) {
                type = 1;
                return true;
            }
            return false;
        }

        bool hasEmptyRoads() {
            for (shared_ptr<RoadJunction> roadSharedPtr : roads) {
                RoadJunction* roadPtr = roadSharedPtr.get();
                if (roadPtr != nullptr && roadPtr->hasRoad()) {
                    return false;
                }
            }
            return true;
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
    bool hasRobberVal;

    public:
        Tile (Resource resourceArg, int rollArg) : resource {resourceArg}, roll {rollArg}, settlements {vector<shared_ptr<SettlementJunction>>(6, nullptr)}, hasRobberVal {false} {}
        
        const Resource getResource() {
            return resource;
        }

        const int getRoll() {
            return roll;
        }

        bool hasRobber() {
            return hasRobberVal;
        }

        bool removeRobber() {
            bool tempHasRobber {hasRobberVal};
            hasRobberVal = false;
            return tempHasRobber;
        }

        bool placeRobber() {
            bool tempHasRobber {hasRobberVal};
            hasRobberVal = true;
            return tempHasRobber;
        }

        vector<shared_ptr<SettlementJunction>>& getSettlements() {
            return settlements;
        }

        SettlementJunction* getSettlement(HexPos pos) {
            return settlements[pos].get();
        }
};

// Custom To Strings
ostream& operator<<(ostream& ostrm, Tile &tile) {
    string roll = to_string(tile.getRoll());
    string robber = tile.hasRobber() ? "R" : "";
    if (roll == "-1") {
        string padding = robber == "R" ? "" : " ";
        return ostrm << padding << robber << getAbbrev(tile.getResource()) << " ";
    } else {
        string padding = (roll.size() + robber.size()) >= 2 ? "" : " ";
        return ostrm << robber << padding << roll << getAbbrev(tile.getResource());
    }
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
    return ostrm << padding << player << rep << " ";
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
    unordered_map<int, vector<Tile*>> rollTileLists;
    vector<vector<variant<monostate, Tile*, SettlementJunction*, RoadJunction*>>> pieceGrid;
    unsigned seed = 0;
    int numTiles {19};
    const vector<int> rowLengths {3,4,5,4,3};
    Tile* robberTile;

    public:
        Board() : tileGrid {vector<vector<Tile>>(5)}, pieceGrid {vector<vector<variant<monostate, Tile*, SettlementJunction*, RoadJunction*>>>(11, vector<variant<monostate, Tile*, SettlementJunction*, RoadJunction*>>(21, monostate{}))} {
            // Initialize Tile Value Stacks.
            map<Resource, int> resourceTiles = {
                {Resource::Brick, 3},
                {Resource::Tree, 4},
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
            // rule of thumb - don't store pointers to lists of vectors since they can be reallocated
            while (resourceStack.size() != 0 && rollStack.size() != 0 && currentRow < rowLengths.size()) {
                for (int i = 0; i < rowLengths[currentRow] && resourceStack.size() != 0 && rollStack.size() != 0; i++) {
                    Resource lastResource = resourceStack.back();
                    int lastRoll = rollStack.back();

                    resourceStack.pop_back();
                    if (lastResource == Resource::Sand) {
                        lastRoll = -1;
                    } else {
                        rollStack.pop_back();
                    }
                    tileGrid[currentRow].push_back(Tile {lastResource, lastRoll});
                }
                currentRow++;
            }
            // Second Loop (because pointers to lists of vectors can be iffy if reallocated)
            for (int row = 0; row < rowLengths.size(); row++) {
                for (int col = 0; col < rowLengths[row]; col++) {
                    Tile& currTile = tileGrid[row][col];
                    if (currTile.getResource() != Resource::Sand) {
                        rollTileLists[currTile.getRoll()].push_back(&tileGrid[row][col]);
                    } else {
                        robberTile = &tileGrid[row][col];
                        robberTile->placeRobber();
                    }
                }
            }

            // Junction & Road Generation
            for (int row = 0; row < rowLengths.size(); row++) {
                for (int col = 0; col < rowLengths[row]; col++) {
                    Tile& currTile = tileGrid[row][col];
                    vector<shared_ptr<SettlementJunction>>& currSettlements = currTile.getSettlements();

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
                        pieceGrid[gridRow][gridCol] = tl;
                        gridCol++;
                        
                        pieceGrid[gridRow][gridCol] = tl->getRoad(HexPos::TopRight);
                        gridCol++;

                        SettlementJunction* t = currTile.getSettlement(HexPos::Top);
                        pieceGrid[gridRow][gridCol] = t;
                        gridCol++;

                        pieceGrid[gridRow][gridCol] = t->getRoad(HexPos::BottomRight);
                        gridCol++;
                    }
                    Tile& currTile = tileGrid[row][rowLengths[row] - 1];
                    SettlementJunction* tr = currTile.getSettlement(HexPos::TopRight);
                    pieceGrid[gridRow][gridCol] = tr;
                    gridRow++;
                    gridCol = rowStart;
                }
                for (int col = 0; col < rowLengths[row]; col++) {
                    Tile& currTile = tileGrid[row][col];
                    SettlementJunction* bl = currTile.getSettlement(HexPos::BottomLeft);
                    pieceGrid[gridRow][gridCol] = bl->getRoad(HexPos::Top);
                    gridCol+=2;

                    pieceGrid[gridRow][gridCol] = &currTile;
                    gridCol+=2;
                }
                Tile& currTile = tileGrid[row][rowLengths[row] - 1];
                SettlementJunction* br = currTile.getSettlement(HexPos::BottomRight);
                pieceGrid[gridRow][gridCol] = br->getRoad(HexPos::Top);
                gridRow++;
                gridCol = rowStart;
                if (row >= 2) {
                    for (int col = 0; col < rowLengths[row]; col++) {
                        Tile& currTile = tileGrid[row][col];
                        SettlementJunction* bl = currTile.getSettlement(HexPos::BottomLeft);
                        pieceGrid[gridRow][gridCol] = bl;
                        gridCol++;

                        pieceGrid[gridRow][gridCol] = bl->getRoad(HexPos::BottomRight);
                        gridCol++;

                        SettlementJunction* b = currTile.getSettlement(HexPos::Bottom);
                        pieceGrid[gridRow][gridCol] = b;
                        gridCol++;

                        pieceGrid[gridRow][gridCol] = b->getRoad(HexPos::TopRight);
                        gridCol++;
                    }
                    Tile& currTile = tileGrid[row][rowLengths[row] - 1];
                    SettlementJunction* br = currTile.getSettlement(HexPos::BottomRight);
                    pieceGrid[gridRow][gridCol] = br;
                    gridRow++;
                }
                gridCol = 0;
            }
        }

        string formatNumber(int num) {
            string numStr = to_string(num);
            if (numStr.size() == 2) {
                return " " + numStr;
            } else {
                return " " + numStr + " ";
            }
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

        int getLongestRoadUser() {
            return -1;
        }

        string printBoardState() {
            stringstream output;
            for (int col = 0; col < 22; col++) {
                output << formatNumber(col) << " ";
            }
            output << "\n";

            for (int row = 0; row < pieceGrid.size(); row++) {
                // Print Row Number
                output << formatNumber(row + 1) << " ";

                // Print Row contents
                int rowStart = 0;
                for (int col = 0; col < pieceGrid[row].size(); col++) {
                    if (!holds_alternative<monostate>(pieceGrid[row][col])) {
                        break;
                    }
                    rowStart++;
                }
                int rowEnd = 20 - rowStart;
                for (int col = 0; col < pieceGrid[row].size(); col++) {
                    if (holds_alternative<monostate>(pieceGrid[row][col])) {
                        if (rowStart <= col && col <= rowEnd) {
                            output << "   ";
                        } else {
                            output << "~~~";
                        }
                    } else if (holds_alternative<Tile*>(pieceGrid[row][col])) {
                        output << *get<Tile*>(pieceGrid[row][col]);
                    } else if (holds_alternative<SettlementJunction*>(pieceGrid[row][col])) {
                        output << *get<SettlementJunction*>(pieceGrid[row][col]);
                    } else {
                        output << *get<RoadJunction*>(pieceGrid[row][col]);
                    }
                    output << " ";
                }
                output << "\n";
            }
            return output.str();
        }

        bool moveRobber(int row, int col) {
            variant<monostate, Tile*, SettlementJunction*, RoadJunction*> currPiece = pieceGrid[row-1][col-1];
            if (holds_alternative<Tile*>(currPiece)) {
                Tile* currTile = get<Tile*>(currPiece);
                if (currTile == robberTile) {
                    return false;
                }

                robberTile->removeRobber();
                currTile->placeRobber();
                robberTile = currTile;
                return true;
            }
            return false;
        }

        bool placeRoad(int row, int col, int player, bool firstTurn) {
            if (holds_alternative<RoadJunction*>(pieceGrid[row - 1][col - 1])) {
                RoadJunction& road = *get<RoadJunction*>(pieceGrid[row - 1][col - 1]);
                vector<shared_ptr<SettlementJunction>>& settlements = road.getSettlements();
                if (firstTurn) {
                    SettlementJunction* first = settlements[0].get();
                    SettlementJunction* second = settlements[1].get();

                    bool firstValid = first->hasSettlement() && first->getPlayer() == player && first->hasEmptyRoads();
                    bool secondValid = second->hasSettlement() && second->getPlayer() == player && second->hasEmptyRoads();

                    if (firstValid || secondValid) {
                        return road.placeRoad(player);
                    }
                } else {
                    for (auto settlement : settlements) {
                        int settlementPlayer = settlement->getPlayer();
                        if (settlementPlayer == -1 || settlementPlayer == player) {
                            vector<shared_ptr<RoadJunction>> settlementRoads = settlement->getRoads();
                            for (shared_ptr<RoadJunction> settlementRoad : settlementRoads) {
                                RoadJunction* roadPtr = settlementRoad.get();
                                if (roadPtr != nullptr && settlementRoad->getPlayer() == player) {
                                    return road.placeRoad(player);
                                }
                            }
                        }
                    }
                }
            }
            return false;
        }

        bool removeRoad(int row, int col) {
            if (holds_alternative<RoadJunction*>(pieceGrid[row - 1][col - 1])) {
                RoadJunction& road = *get<RoadJunction*>(pieceGrid[row - 1][col - 1]);
                road.removeRoad();
            }
            return false;
        }

        bool placeSettlement(int row, int col, int player, bool firstTurn) {
            if (holds_alternative<SettlementJunction*>(pieceGrid[row - 1][col - 1])) {
                SettlementJunction* settlementPtr = get<SettlementJunction*>(pieceGrid[row - 1][col - 1]);
                SettlementJunction& settlement = *settlementPtr;

                bool isValidFirstTurn = true; // not within 1 distance of any other settlement
                bool isValidNormalTurn = false; // not within 1 distance & connected by a road

                vector<shared_ptr<RoadJunction>> roads = settlement.getRoads();
                for (shared_ptr<RoadJunction> roadSharedPtr : roads) {
                    RoadJunction* roadPtr = roadSharedPtr.get();
                    if (roadPtr != nullptr) {
                        int roadPlayer = roadPtr->getPlayer();

                        vector<shared_ptr<SettlementJunction>>& settlements = roadPtr->getSettlements();
                        bool firstMatches = settlements[0].get() == settlementPtr;
                        SettlementJunction* secondSettlementPtr = settlements[firstMatches ? 1 : 0].get();
                        isValidFirstTurn &= !secondSettlementPtr->hasSettlement();
                        isValidNormalTurn |= roadPlayer == player;
                    }
                }

                if (!settlement.hasSettlement() && isValidFirstTurn && (firstTurn ? isValidFirstTurn : isValidNormalTurn)) {
                    return settlement.placeSettlement(player);
                }
            }
            return false;
        }

        bool upgradeSettlement(int row, int col, int player) {
            if (holds_alternative<SettlementJunction*>(pieceGrid[row - 1][col - 1])) {
                SettlementJunction& settlement = *get<SettlementJunction*>(pieceGrid[row - 1][col - 1]);
                return settlement.upgradeSettlement(player);
            }
            return false;
        }

        unordered_map<int, unordered_map<Resource, int>> rollToResourceCounts(int roll) {
            unordered_map<int, unordered_map<Resource, int>> playerCounts;
            vector<Tile*>& rollList = rollTileLists[roll];

            for (Tile* tile : rollList) {
                Resource resource = tile->getResource();
                if (tile->hasRobber()) {
                    continue;
                }
                for (shared_ptr<SettlementJunction>& settlement : tile->getSettlements()) {
                    int player = settlement.get()->getPlayer();
                    int type = settlement.get()->getSettlementType();

                    if (player != -1 && type != -1) {
                        if (!playerCounts.count(player)) {
                            playerCounts[player] = unordered_map<Resource, int>();
                        }
                        // May throw error, unsure how it is initialized.
                        playerCounts[player][resource]++;
                    }
                }
            }

            return playerCounts;
        }
};