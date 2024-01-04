#ifndef DEFS_H
#define DEFS_H

#include <string>

enum class Resource {Brick, Tree, Sheep, Wheat, Ore, Sand};
enum DevelopmentCard {Knight, Monopoly, RoadBuilding, YearOfPlenty, VictoryPoint};

std::string getAbbrev(Resource res);
Resource parseResource(std::string str);
int parseDevelopmentCard(std::string str);

#endif