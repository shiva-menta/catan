#include <string>

using namespace std;

enum class Resource {Brick, Tree, Sheep, Wheat, Ore, Sand};

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
