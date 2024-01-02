#ifndef BOARD_H
#define BOARD_H

#include <string>
#include <list>
#include <map>
#include <algorithm>
#include <random>
#include <unordered_set>
#include "defs.hpp"

using namespace std;

enum HexPos {Top, TopRight, BottomRight, Bottom, BottomLeft, TopLeft};
const vector<string> roadChars {"|", "/", "\\"};

class SettlementJunction;

class RoadJunction {
    int player;
    vector<shared_ptr<SettlementJunction>> settlements;
    int display;

    public:
        RoadJunction();
        int getPlayer();
        int getDisplay();
        bool hasRoad();
        bool placeRoad(int playerArg);
        vector<shared_ptr<SettlementJunction>>& getSettlements();
};

class SettlementJunction {
    int player;
    int type;
    unordered_set<shared_ptr<RoadJunction>> roads;

    public:
        SettlementJunction();
        bool hasSettlement();
        int getSettlementType();
        int getPlayer();
        bool placeSettlement(int playerArg);
        bool upgradeSettlement(int playerArg);
        unordered_set<shared_ptr<RoadJunction>>& getRoads();
        RoadJunction* getRoad(HexPos pos);
};

class Tile {
    Resource resource;
    int roll;
    vector<shared_ptr<SettlementJunction>> settlements;
    bool hasRobber;

    public:
        Tile(Resource resourceArg, int rollArg);
        Resource getResource();
        int getRoll();
        bool removeRobber();
        bool placeRobber();
        vector<shared_ptr<SettlementJunction>>& getSettlements();
        SettlementJunction* getSettlement(HexPos pos);
};

ostream& operator<<(ostream& ostrm, Tile &tile);
ostream& operator<<(ostream& ostrm, SettlementJunction &settlement);
ostream& operator<<(ostream& ostrm, RoadJunction &road);

class Board {
    vector<vector<Tile>> tileGrid;
    map<int, list<Tile*>> rollTileLists;
    vector<vector<variant<monostate, Tile*, SettlementJunction*, RoadJunction*>>> pieceGrid;
    unsigned seed;
    int numTiles;
    const vector<int> rowLengths;

    public:
        string formatNumber(int num);
        shared_ptr<SettlementJunction> getSettlementReference(int row, int col, HexPos pos);
        HexPos getOppositePos(HexPos pos);
        void makeRoad(shared_ptr<SettlementJunction> s1, shared_ptr<SettlementJunction> s2);
        Board();
        int getLongestRoadUser();
        void printBoardState();
        bool placeRoad(int row, int col, int player);
        // need to account for settlement proximity, road connection (assume this is non beginning turn)
        bool placeSettlement(int row, int col, int player, bool isInitialTurn);
        bool upgradeSettlement(int row, int col, int player);
};

#endif