#ifndef BOARD_H
#define BOARD_H

#include <string>
#include <map>
#include <algorithm>
#include <random>
#include <unordered_set>
#include <iostream>
#include <sstream>
#include "defs.hpp"

enum HexPos {Top, TopRight, BottomRight, Bottom, BottomLeft, TopLeft};
const std::vector<std::string> roadChars {"|", "/", "\\"};

class SettlementJunction;

class RoadJunction {
    int player = -1;
    std::vector<std::shared_ptr<SettlementJunction>> settlements;
    int display;

    public:
        RoadJunction(std::shared_ptr<SettlementJunction> s1, std::shared_ptr<SettlementJunction> s2, int hex);
        int getPlayer();
        int getDisplay();
        bool hasRoad();
        bool placeRoad(int playerArg);
        bool removeRoad();
        std::vector<std::shared_ptr<SettlementJunction>>& getSettlements();
};

class SettlementJunction {
    int player = -1;
    int type = -1;
    std::vector<std::shared_ptr<RoadJunction>> roads;

    public:
        SettlementJunction();
        bool hasSettlement();
        int getSettlementType();
        int getPlayer();
        bool placeSettlement(int playerArg);
        bool upgradeSettlement(int playerArg);
        bool hasEmptyRoads();
        std::vector<std::shared_ptr<RoadJunction>>& getRoads();
        RoadJunction* getRoad(HexPos pos);
};

class Tile {
    Resource resource;
    int roll;
    std::vector<std::shared_ptr<SettlementJunction>> settlements;
    bool hasRobberVal;

    public:
        Tile(Resource resourceArg, int rollArg);
        Resource getResource();
        int getRoll();
        bool hasRobber();
        bool removeRobber();
        bool placeRobber();
        std::vector<std::shared_ptr<SettlementJunction>>& getSettlements();
        SettlementJunction* getSettlement(HexPos pos);
};

std::ostream& operator<<(std::ostream& ostrm, Tile &tile);
std::ostream& operator<<(std::ostream& ostrm, SettlementJunction &settlement);
std::ostream& operator<<(std::ostream& ostrm, RoadJunction &road);

class Board {
    std::vector<std::vector<Tile>> tileGrid;
    std::unordered_map<int, std::vector<Tile*>> rollTileLists;
    std::vector<std::vector<std::variant<std::monostate, Tile*, SettlementJunction*, RoadJunction*>>> pieceGrid;
    unsigned seed = 0;
    int numTiles {19};
    const std::vector<int> rowLengths {3,4,5,4,3};
    Tile* robberTile;

    public:
        Board();
        std::string formatNumber(int num);
        std::shared_ptr<SettlementJunction> getSettlementReference(int row, int col, HexPos pos);
        HexPos getOppositePos(HexPos pos);
        void makeRoad(std::shared_ptr<SettlementJunction> s1, std::shared_ptr<SettlementJunction> s2, HexPos pos);
        int getLongestRoadUser();
        std::string printBoardState();
        bool moveRobber(int row, int col);
        bool placeRoad(int row, int col, int player, bool firstTurn);
        bool removeRoad(int row, int col);
        bool placeSettlement(int row, int col, int player, bool firstTurn);
        bool upgradeSettlement(int row, int col, int player);
        std::unordered_map<int, std::unordered_map<Resource, int>> rollToResourceCounts(int roll);
};

#endif