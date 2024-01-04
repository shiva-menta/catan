#ifndef HOST_H
#define HOST_H

using namespace std;

// Responsibilities: host the game, take in text commands from clients, return them a game state string / bytes if valid

class GameHost {
    public:
        void initializeSocket(int portNum);
};

int main();

#endif