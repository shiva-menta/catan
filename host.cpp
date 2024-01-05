#include "host.hpp"

using namespace std;

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

string blockingReceive(int sock) {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    int bytesReceived = recv(sock, buffer, sizeof(buffer), 0);
    if (bytesReceived == -1) {
        cerr << "Error in recv()." << endl;
    } else if (bytesReceived == 0) {
        cout << "Connection closed by the server." << endl;
    } else {
        string message = string(buffer, bytesReceived);
        cout << "Message received: " << message << endl;
        return message;
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
        if (command == "buyDev" && numArgs == 0) {
            return game->buyDevelopmentCard(player);
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

void untilValid(Game* game, blockingQueue<pair<int, string>>* moveQueue, int player, string command, bool firstTurn) {
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

void handlePlayerThread(int sock, int playerArg, bool* waitingForPlayers, bool* isSessionActive, blockingQueue<pair<int, string>>* moveQueue) {
    int player = playerArg;
    while (*isSessionActive) {
        string message = blockingReceive(sock);
        if (!message.empty()) {
            if (*waitingForPlayers && message == "start") {
                cout << "Ending loading." << endl;
                *waitingForPlayers = false;
                cv.notify_one();
            } else {
                moveQueue->push(pair<int, string> {player, message});
            }
        } else {
            return;
        }
    }
    return;
}

void handleConnectionsThread(int server_fd, sockaddr_in* address, vector<pair<int, int>>* sockAddrs, Game* game, bool* waitingForPlayers, bool* isSessionActive, blockingQueue<pair<int, string>>* moveQueue) {
    int addrlen = sizeof(*address);
    while (*isSessionActive && *waitingForPlayers) {
        int sock = accept(server_fd, (struct sockaddr *)address, (socklen_t*)&addrlen);
        if (sock < 0) {
            cerr << "Accept failed" << endl;
            close(server_fd);
            continue;
        }
        int userNum = game->addUser();
        if (userNum != -1) {
            cout << "Connection Accepted. Player: " << userNum << endl;
            sockAddrs->push_back(pair<int, int> {userNum, sock});
            thread(handlePlayerThread, sock, userNum, waitingForPlayers, isSessionActive, moveQueue).detach();
        } else {
            cout << "Connection Rejected." << endl;
        }
    }
    return;
}

void broadcastNewBoards(Game* game, vector<pair<int, int>>& sockPairs) {
    // Not sure if threads can be left detached
    for (const auto& sockPair : sockPairs) {
        int player = sockPair.first, sock = sockPair.second;
        string board = game->printGameState(player);
        int boardSize = board.size();
        thread([board,boardSize](int sock){ write(sock, board.c_str(), boardSize); return; }, sock); // this return might not be necessary
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
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        cerr << "Failed to create socket." << endl;
        close(server_fd);
        return -1;
    }
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        cerr << "Can't set sock opt." << endl;
        return -1;
    }

    // Bind to Port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    if (::bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        cerr << "Failed to bind socket." << endl;
        close(server_fd);
        return -1;
    }
    cout << "Binded" << endl;

    // Listen For Connections
    if (listen(server_fd, 4) < 0) {
        cerr << "Could not listen." << endl;
        close(server_fd);
        return -1;
    }
    cout << "Listening" << endl;

    // Initiate Game Logic
    Game game {};
    blockingQueue<pair<int, string>> moveQueue;

    // Accept New Connections Thread
    bool isSessionActive = true;
    bool waitingForPlayers = true;
    vector<pair<int, int>> sockAddrs;

    // Launch New Connections Thread & Wait Until Start Message Received
    thread connectionsThread(handleConnectionsThread, server_fd, &address, &sockAddrs, &game, &waitingForPlayers, &isSessionActive, &moveQueue);
    unique_lock<mutex> lock(cv_m);
    cv.wait(lock, [&]{ return !waitingForPlayers; });

    // Start Game
    game.startGame();
    cout << "Game starting." << endl;

    // Handle Game Turns
    // First Turn
    broadcastNewBoards(&game, sockAddrs);
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
    connectionsThread.join();
    close(server_fd);
    return 0;
}