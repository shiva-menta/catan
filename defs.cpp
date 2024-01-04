#include <string>

using namespace std;

enum class Resource {Brick, Tree, Sheep, Wheat, Ore, Sand};
enum DevelopmentCard {Knight, Monopoly, RoadBuilding, YearOfPlenty, VictoryPoint};

string getAbbrev(Resource res) {
    switch (res) {
        case Resource::Brick:
            return "B";
        case Resource::Tree:
            return "T";
        case Resource::Sheep:
            return "S";
        case Resource::Wheat:
            return "W";
        case Resource::Ore:
            return "O";
        default:
            return "D";
    }
}

Resource parseResource(string str) {
    if (str == "brick") {
        return Resource::Brick;
    } else if (str == "tree") {
        return Resource::Tree;
    } else if (str == "sheep") {
        return Resource::Sheep;
    } else if (str == "wheat") {
        return Resource::Wheat;
    } else if (str == "ore") {
        return Resource::Ore;
    }
    return Resource::Sand;
}

int parseDevelopmentCard(string str) {
    DevelopmentCard dev;
    if (str == "knight") {
        dev = DevelopmentCard::Knight;
    } else if (str == "monopoly") {
        dev = DevelopmentCard::Monopoly;
    } else if (str == "roadbuilding") {
        dev = DevelopmentCard::RoadBuilding;
    } else if (str == "yearofplenty") {
        dev = DevelopmentCard::YearOfPlenty;
    } else if (str == "victorypoint") {
        dev = DevelopmentCard::VictoryPoint;
    } else {
        return -1;
    }
    return static_cast<int>(dev);
}
