#include "board.hpp"

using namespace std;

// Road Junction Functions
RoadJunction::RoadJunction(shared_ptr<SettlementJunction> s1, shared_ptr<SettlementJunction> s2, int hex) : settlements {{s1, s2}}, display {hex} {}
int RoadJunction::getPlayer() {
    return player;
}
int RoadJunction::getDisplay() {
    return display;
}
bool RoadJunction::hasRoad() {
    return player != -1;
}
bool RoadJunction::placeRoad(int playerArg) {
    if (player == -1) {
        player = playerArg;
        return true;
    }
    return false;
}
bool RoadJunction::removeRoad() {
    if (player != -1) {
        player = -1;
        return true;
    }
    return false;
}
vector<shared_ptr<SettlementJunction>>& RoadJunction::getSettlements() {
    return settlements;
}

// SettlementJunction Functions
SettlementJunction::SettlementJunction() : roads {vector<shared_ptr<RoadJunction>>(6, nullptr)} {}
bool SettlementJunction::hasSettlement() {
    return player != -1;
}
int SettlementJunction::getSettlementType() {
    return type;
}
int SettlementJunction::getPlayer() {
    return player;
}
bool SettlementJunction::placeSettlement(int playerArg) {
    if (player == -1) {
        player = playerArg;
        type = 0;
        return true;
    }
    return false;
}
bool SettlementJunction::upgradeSettlement(int playerArg) {
    if (player == playerArg && type == 0) {
        type = 1;
        return true;
    }
    return false;
}
bool SettlementJunction::hasEmptyRoads() {
    for (shared_ptr<RoadJunction> roadSharedPtr : roads) {
        RoadJunction* roadPtr = roadSharedPtr.get();
        if (roadPtr != nullptr && roadPtr->hasRoad()) {
            return false;
        }
    }
    return true;
}
vector<shared_ptr<RoadJunction>>& SettlementJunction::getRoads() {
    return roads;
}
RoadJunction* SettlementJunction::getRoad(HexPos pos) {
    return roads[pos].get();
}

// Tile Functions
Tile::Tile (Resource resourceArg, int rollArg) : resource {resourceArg}, roll {rollArg}, settlements {vector<shared_ptr<SettlementJunction>>(6, nullptr)}, hasRobberVal {false} {}
Resource Tile::getResource() {
    return resource;
}
int Tile::getRoll() {
    return roll;
}
bool Tile::hasRobber() {
    return hasRobberVal;
}
bool Tile::removeRobber() {
    bool tempHasRobber {hasRobberVal};
    hasRobberVal = false;
    return tempHasRobber;
}
bool Tile::placeRobber() {
    bool tempHasRobber {hasRobberVal};
    hasRobberVal = true;
    return tempHasRobber;
}
vector<shared_ptr<SettlementJunction>>& Tile::getSettlements() {
    return settlements;
}
SettlementJunction* Tile::getSettlement(HexPos pos) {
    return settlements[pos].get();
}

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

// Board Functions
Board::Board() : tileGrid {vector<vector<Tile>>(5)}, pieceGrid {vector<vector<variant<monostate, Tile*, SettlementJunction*, RoadJunction*>>>(11, vector<variant<monostate, Tile*, SettlementJunction*, RoadJunction*>>(21, monostate{}))} {
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

string Board::formatNumber(int num) {
    string numStr = to_string(num);
    if (numStr.size() == 2) {
        return " " + numStr;
    } else {
        return " " + numStr + " ";
    }
}

shared_ptr<SettlementJunction> Board::getSettlementReference(int row, int col, HexPos pos) {
    Tile& tile = tileGrid[row][col];
    return tile.getSettlements()[pos];
}

HexPos Board::getOppositePos(HexPos pos) {
    int posInt = static_cast<int>(pos);
    int oppositeInt = (posInt + 3) % 6;
    return static_cast<HexPos>(oppositeInt);
}

void Board::makeRoad(shared_ptr<SettlementJunction> s1, shared_ptr<SettlementJunction> s2, HexPos pos) {
    HexPos oppositePos = getOppositePos(pos);
    vector<shared_ptr<RoadJunction>>& roads1 = s1->getRoads();
    if (roads1[pos] == nullptr) {
        shared_ptr<RoadJunction> newRoad = make_shared<RoadJunction>(s1, s2, static_cast<int>(pos) % 3);
        vector<shared_ptr<RoadJunction>>& roads2 = s2->getRoads();
        roads1[pos] = shared_ptr<RoadJunction>(newRoad);
        roads2[oppositePos] = shared_ptr<RoadJunction>(newRoad);
    }
}

int Board::getLongestRoadUser() {
    return -1;
}

string Board::printBoardState() {
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

bool Board::moveRobber(int row, int col) {
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

bool Board::placeRoad(int row, int col, int player, bool firstTurn) {
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

bool Board::removeRoad(int row, int col) {
    if (holds_alternative<RoadJunction*>(pieceGrid[row - 1][col - 1])) {
        RoadJunction& road = *get<RoadJunction*>(pieceGrid[row - 1][col - 1]);
        road.removeRoad();
    }
    return false;
}

bool Board::placeSettlement(int row, int col, int player, bool firstTurn) {
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

vector<Resource> Board::getStartingResources(int row, int col) {
    vector<Resource> resources;
    if (holds_alternative<SettlementJunction*>(pieceGrid[row - 1][col - 1])) {
        SettlementJunction& settlement = *get<SettlementJunction*>(pieceGrid[row - 1][col - 1]);

        // (bad logic) – generate placements based on row / col positions
        bool hasUpRoad = (row - 2 >= 0) && holds_alternative<RoadJunction*>(pieceGrid[row - 2][col - 1]);
        bool hasUpSettlement = (row - 2 >= 0) && holds_alternative<SettlementJunction*>(pieceGrid[row - 2][col - 1]);
        bool hasDownRoad = (row < 11) && holds_alternative<RoadJunction*>(pieceGrid[row][col - 1]);
        bool hasDownSettlement = (row < 11) && holds_alternative<SettlementJunction*>(pieceGrid[row][col - 1]);

        vector<pair<int, int>> settlementCoordinates {};
        if (hasUpRoad || hasDownSettlement) {
            // upside down triangle formation
            settlementCoordinates.push_back(pair<int, int> {row, col - 1});
            settlementCoordinates.push_back(pair<int, int> {row - 2, col - 3});
            settlementCoordinates.push_back(pair<int, int> {row - 2, col + 1});
        } else {
            // normal triangle formation
            settlementCoordinates.push_back(pair<int, int> {row - 2, col - 1});
            settlementCoordinates.push_back(pair<int, int> {row, col - 3});
            settlementCoordinates.push_back(pair<int, int> {row, col + 1});
        }

        for (auto const& pair : settlementCoordinates) {
            int nRow = pair.first, nCol = pair.second;
            if (0 <= nRow && nRow < 11 && 0 <= nCol && nCol < 21 && holds_alternative<Tile*>(pieceGrid[nRow][nCol])) {
                resources.push_back(get<Tile*>(pieceGrid[nRow][nCol])->getResource());
            }
        }
    }
    return resources;
}

bool Board::upgradeSettlement(int row, int col, int player) {
    if (holds_alternative<SettlementJunction*>(pieceGrid[row - 1][col - 1])) {
        SettlementJunction& settlement = *get<SettlementJunction*>(pieceGrid[row - 1][col - 1]);
        return settlement.upgradeSettlement(player);
    }
    return false;
}

unordered_map<int, unordered_map<Resource, int>> Board::rollToResourceCounts(int roll) {
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