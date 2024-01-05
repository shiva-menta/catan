#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <sstream>
#include <thread>
#include <unordered_set>
#include <mutex>
#include <condition_variable>
#include "game.hpp"
#include "blockingQueue.hpp"

using namespace std;

const unordered_set<string> integerCommands {"road", "settlement", "city", "robber", "knight"};
const unordered_set<string> stringCommands {"buyDev", "monopoly", "yearofplenty", "discard"};

string blockingReceive(int sock) {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    int bytesReceived = recv(sock, buffer, sizeof(buffer), 0);
    if (bytesReceived == -1) {
        std::cerr << "Error in recv()." << endl;
    } else if (bytesReceived == 0) {
        std::cout << "Connection closed by the server." << endl;
    } else {
        return string(buffer, bytesReceived);
    }

    return "";
}

pair<string, string> splitFirstTwoTerms(const string& str) {
   pair<string, string> result {"", ""};

    size_t pos = str.find('x');
    if (pos != std::string::npos) {
        result.first = str.substr(0, pos);
        if (pos + 1 < str.size()) {
            result.second = str.substr(pos + 1);
        }
    }

    return result;
}

// make more efficient – can convert to hashing and O(1) switch statements, code can also look better by grouping parsing
bool handleCommand(Game* game, int player, vector<string> args, bool firstTurn) {
    string command = args[0];
    int numArgs = args.size() - 1;

    if (integerCommands.count(command) && all_of(next(args.begin()), args.end(), isInteger) && numArgs == 2) {
        int arg1 = stoi(args[1]), arg2 = stoi(args[2]);
        if (command == "road") {
            return game->placeRoad(player, arg1, arg2, firstTurn);
        } else if (command == "settlement") {
            return game->placeSettlement(player, arg1, arg2, firstTurn);
        } else if (command == "city") {
            return game->upgradeSettlement(player, arg1, arg2);
        } else if (command == "robber") {
            return game->moveRobber(player, arg1, arg2, firstTurn);
        } else if (command == "knight") {
            return game->useKnight(player, arg1, arg2);
        }
    } else if (stringCommands.count(command)) {
        if (command == "buyDev" && numArgs == 1) {
            int devInt = parseDevelopmentCard(args[1]);
            if (devInt != -1) {
                DevelopmentCard dev = static_cast<DevelopmentCard>(devInt);
                return game->buyDevelopmentCard(player, dev);
            }
        } else if (command == "monopoly" && numArgs == 1) {
            Resource res = parseResource(args[1]);
            if (res != Resource::Sand) {
                return game->useMonopoly(player, res);
            }
        } else if (command == "yearofplenty" && numArgs == 2) {
            Resource res1 = parseResource(args[1]);
            Resource res2 = parseResource(args[2]);
            if (res1 != Resource::Sand && res2 != Resource::Sand) {
                return game->useYearOfPlenty(player, res1, res2);
            }
        } else if (command == "discard") {
            unordered_map<Resource, int> discards;
            for (int i = 1; i < args.size(); i++) {
                pair<string, string> splitTerms = splitFirstTwoTerms(args[i]);
                Resource res = parseResource(splitTerms.first);
                if (res != Resource::Sand && isInteger(splitTerms.second)) {
                    discards[res] = stoi(splitTerms.second);
                }
            }
            return game->discardCardsOverLimit(player, discards);
        }
    }
     
    return false;
}

void untilValid(Game* game, queue<pair<int, string>>* moveQueue, int player, string command, bool firstTurn) {
   while (true) {
        pair<int, string> message = moveQueue->pop();
        if (message.first == player) {
            vector<string> args = splitString(message.second);
            if ((command == "" || args[0] == command) && handleCommand(game, player, args, firstTurn)) {
                break;
            }
        }
    }
}

void handlePlayerThread(int sock, int playerArg, bool* waitingForPlayers, bool* isSessionActive, queue<pair<int, string>>* moveQueue) {
    int player = playerArg;
    while (*isSessionActive) {
        string message = blockingReceive(sock);
        if (!message.empty()) {
            if (*waitingForPlayers && message == "start") {
                *waitingForPlayers = false;
            } else {
                moveQueue->push(pair<int, string> {player, message});
            }
        } else {
            return;
        }
    }
    return;
}

bool isInteger(const string &str) {
    if (str.empty() || !all_of(str.begin(), str.end(), ::isdigit)) {
        return false;
    };
    return true;
}

vector<string> splitString(const string &str) {
    vector<string> words;
    istringstream iss(str);
    string word;

    while (iss >> word) {
        words.push_back(word);
    }

    return words;
}

void broadcastNewBoards(Game* game, vector<pair<int, int>>& sockPairs) {
    // Not sure if threads can be left detached
    for (const auto& sockPair : sockPairs) {
        int player = sockPair.first, sock = sockPair.second;
        string board = game->printGameState(player);
        int boardSize = board.size();
        thread([board,boardSize](int sock){ write(sock, board.c_str(), boardSize);}, sock);
    }
}

int main(int argc, char** argv) {
    // Sanity Checks
    if (argc != 2) {
        cerr << "Incorrect arguments provided." << endl;
        return -1;
    }
    string arg1 = argv[1];
    if (arg1.empty() || !all_of(arg1.begin(), arg1.end(), ::isdigit)) {
        cerr << "Invalid port number." << endl;
        return -1;
    };
    int port = stoi(arg1);

    // Create a Socket
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Bind to Port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));

    // Listen For Connections
    listen(server_fd, 4);

    // Initiate Game Logic (need to consider race conditions)
    Game game {};
    queue<pair<int, string>> moveQueue;

    // Accept New Connections Thread
    vector<thread> threads;
    bool isSessionActive = true;
    bool waitingForPlayers = true;
    vector<pair<int, int>> sockAddrs;

    while (waitingForPlayers) {
        int sock = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (sock < 0) {
            cerr << "Accept failed" << std::endl;
            continue;
        }
        int userNum = game.addUser();
        sockAddrs.push_back(pair<int, int> {userNum, sock});
        threads.emplace_back(thread(handlePlayerThread, sock, userNum, &waitingForPlayers, &isSessionActive, &moveQueue));
    }
    game.startGame();

    // Handle Game Turns
    // First Turn
    vector<int> playerOrder = game.getPlayerOrder();
    vector<int> extendedPlayerOrder(playerOrder.size() * 2);
    copy(playerOrder.begin(), playerOrder.end(), extendedPlayerOrder.begin());
    reverse_copy(playerOrder.begin(), playerOrder.end(), extendedPlayerOrder.begin() + playerOrder.size());

    for (auto player : extendedPlayerOrder) {
        // Wait Until Valid Place Settlement
        untilValid(&game, &moveQueue, player, "settlement", true);
        // Wait Until Valid Place Road
        untilValid(&game, &moveQueue, player, "road", true);
    }

    // Subsequent Turns
    while (isSessionActive) {
        // Get Turn Information
        int currentTurn = game.getTurn();
        int player = game.currentTurnPlayer();
        int roll = game.rollDice();

        // Handle Dice Roll 7 Special Case
        if (roll == 7) {
            // Wait Until Valid Move Robber
            untilValid(&game, &moveQueue, player, "robber", true);
            // Wait Until Valid Remove Cards from All Players
            vector<bool> hasDiscarded = game.playersUnderLimit();

            while (!all_of(hasDiscarded.begin(), hasDiscarded.end(), [](bool discarded) {return discarded;})) {
                pair<int, string> message = moveQueue.pop();
                int messagePlayer = message.first;
                vector<string> args = splitString(message.second);
                string command = args[0];

                if (command == "discard" && handleCommand(&game, player, args, false)) {
                    hasDiscarded[player] = true;
                }
            }
        }

        // Handle Normal Commands
        while (true) {
            pair<int, string> message = moveQueue.pop();
            if (message.first == player) {
                vector<string> args = splitString(message.second);
                if (args.size() > 0 && args[0] == "done") {
                    break;
                }
                handleCommand(&game, player, args, false);
            }
        }
        
        // Check Win Condition
        if (game.isPlayerWinner(player)) {
            string message = "Player " + to_string(player) + " is the winner! Game over.";
            int messageSize = message.size();
            vector<thread> newThreads;
            for (pair<int, int> sockPair : sockAddrs) {
                newThreads.emplace_back(thread([message, messageSize](int sock){ write(sock, message.c_str(), messageSize);}, sockPair.second));
            }
            for (int i = 0; i < newThreads.size(); i++) {
                newThreads[i].join();
            }
            break;
        }

        // Increment Turn
        game.nextTurn();
    }

    // Close Socket & Terminate
    for (int i = 0; i < threads.size(); i++) {
        threads[i].join();
    }
    close(server_fd);
    return 0;
}