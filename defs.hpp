#ifndef DEFS_H
#define DEFS_H

#include <string>

using namespace std;

enum class Resource {Brick, Tree, Sheep, Wheat, Ore, Sand};
enum DevelopmentCard {Knight, Monopoly, RoadBuilding, YearOfPlenty, VictoryPoint};

string getAbbrev(Resource res);
Resource parseResource(string str);
int parseDevelopmentCard(string str);

#endif