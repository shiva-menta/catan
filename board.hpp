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
};

class RoadJunction {
    int player;
    vector<shared_ptr<SettlementJunction>> settlements;

    public:
        RoadJunction();
        bool hasRoad();
        bool placeRoad(int playerArg);
        vector<shared_ptr<SettlementJunction>>& getSettlements();
};

class SettlementJunction {
    int player;
    int type;
    unordered_set<shared_ptr<RoadJunction>> roads;

    public:
        bool hasSettlement();
        int settlementType();
        bool placeSettlement(int playerArg);
        bool upgradeSettlement(int playerArg);
        unordered_set<shared_ptr<RoadJunction>>& getRoads();
};

class Board {
    vector<vector<Tile>> tileGrid;
    map<int, list<Tile*>> rollTileLists;
    unsigned seed;
    int numTiles;
    const vector<int> rowLengths;

    public:
        shared_ptr<SettlementJunction> getSettlementReference(int row, int col, HexPos pos);
        void makeRoad(shared_ptr<SettlementJunction> s1, shared_ptr<SettlementJunction> s2);
        Board();
        int getLongestRoadUser();
};

#endif