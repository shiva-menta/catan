#ifndef DEVCARDS_H
#define DEVCARDS_H

#include "game.hpp"

// (14 Knight/Soldier Cards, 6 Progress Cards, 5 Victory Point Cards).
class DevelopmentCard {
    public:
        virtual void use(Game& game);
};

class KnightCard : public DevelopmentCard {
    public:
        void use(Game& game);
};

class MonopolyCard : public DevelopmentCard {
    public:
        void use(Game& game);
};

class RoadBuildingCard : public DevelopmentCard {
    public:
        void use(Game& game);
};

class YearOfPlentyCard : public DevelopmentCard {
    public:
        void use(Game& game);
};

class VictoryPointCard : public DevelopmentCard {
    public:
        void use(Game& game);
};

#endif