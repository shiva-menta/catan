#include <string>
#include <unordered_map>
#include <algorithm>
#include <random>
#include "defs.hpp"
#include "board.hpp"
#include "developmentCards.hpp"

using namespace std;

class Game {
    unsigned seed = 0;
    vector<unique_ptr<DevelopmentCard>> developmentCards;
    unordered_map<Resource, int> resourceCards;
    int playerCount;
    unordered_map<string, int> playerMap;
    Tile* knightPos = nullptr;
    unordered_map<int, int> knightCount;

    public:
        Game();

        void addUser(string user);

        void makeResourceCards(int cardsPerResource) {
            resourceCards[Resource::Brick] = cardsPerResource;
            resourceCards[Resource::Ore] = cardsPerResource;
            resourceCards[Resource::Sheep] = cardsPerResource;
            resourceCards[Resource::Wheat] = cardsPerResource;
            resourceCards[Resource::Wood] = cardsPerResource;
        }

        // Initialize development cards.
        void makeDevelopmentCards(){
            // Add different types of development cards.
            for (int i = 0; i < 14; i++) {
                developmentCards.push_back(make_unique<KnightCard>());
            }
            for (int i = 0; i < 2; i++) {
                developmentCards.push_back(make_unique<MonopolyCard>());
            }
            for (int i = 0; i < 2; i++) {
                developmentCards.push_back(make_unique<RoadBuildingCard>());
            }
            for (int i = 0; i < 2; i++) {
                developmentCards.push_back(make_unique<YearOfPlentyCard>());
            }
            for (int i = 0; i < 5; i++) {
                developmentCards.push_back(make_unique<VictoryPointCard>());
            }

            // Randomize
            shuffle(developmentCards.begin(), developmentCards.end(), default_random_engine(seed));
        }
        
};
