#ifndef DEVCARDS_H
#define DEVCARDS_H

#include "game.hpp"

// (14 Knight/Soldier Cards, 6 Progress Cards, 5 Victory Point Cards).
class DevelopmentCard {
    public:
        virtual void use(Game& game);
};

class KnightCard : private DevelopmentCard {
    public:
        void use(Game& game);
};

class MonopolyCard : private DevelopmentCard {
    public:
        void use(Game& game);
};

class RoadBuildingCard : private DevelopmentCard {
    public:
        void use(Game& game);
};

class YearOfPlentyCard : private DevelopmentCard {
    public:
        void use(Game& game);
};

class VictoryPointCard : private DevelopmentCard {
    public:
        void use(Game& game);
};

#endif