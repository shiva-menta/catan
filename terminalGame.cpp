#include <string>
#include <iostream>
#include <sstream>
#include "game.hpp"

using namespace std;

// Need to include logic for handling turns and handling separate players for things like 7 rolls, placements of items.
// Maybe for 7 rolls, to simplify, treat that as a "special turn" where users have to discard if they are over, otherwise their turn is skipped.
// Right now, assume single player game mode to test.
// Socket programming wise, one person will host, one person will need to connect.

class TerminalGame {
    Game game {};

    public:
        // Clears terminal page.
        void clearTerminal();

        // Split input string into words.
        vector<string> splitString(const string &str) {
            vector<string> words;
            istringstream iss(str);
            string word;

            while (iss >> word) {
                words.push_back(word);
            }

            return words;
        }

        // Handle Single Line Command
        void handleInput(string input) {
            vector<string> args = splitString(input);
        };
};

// While loop that continuously takes in a new line of input and calls handleInput
int main() {
    TerminalGame tg {};
    string input;

    cout << "Welcome to Catan!";

    while (getline(cin, input)) {
        if (input == "exit") {
            break;
        }
        tg.handleInput(input);
    }

    return 0;
};
