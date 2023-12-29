#ifndef BOARD_H
#define BOARD_H

#include <string>
#include <map>

using namespace std;

enum class Resource {Brick, Wood, Sheep, Wheat, Ore, Sand};

class Tile {
    Resource resource;
    int roll;

    public:
        Tile(Resource resourceArg, int rollArg);
        Resource getResource();
        int getRoll();
};

class RoadJunction {
    int player;
    Tile adjacentTiles[2];

    public:
        bool hasRoad();
        bool placeRoad(int playerArg);
};

class SettlementJunction {
    int player;
    int type;
    Tile adjacentTiles[3];

    public:
        bool hasSettlement();
        int settlementType();
        bool placeSettlement(int playerArg);
        bool upgradeSettlement(int playerArg);
};

// Currently only supporting static size assignment. Board can present itself as a grid for terminal LAN play.
class Board {
    vector<Tile> tiles;

    public:
        Board();
        int getLongestRoadUser();
};

#endif