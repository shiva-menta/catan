#include <string>

enum class Resource {Brick, Wood, Sheep, Wheat, Ore, Sand};

string getAbbrev(Resource res) {
    switch (res) {
        case Resource::Brick:
            return "B";
        case Resource::Wood:
            return "W";
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
