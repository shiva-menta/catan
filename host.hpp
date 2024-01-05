#ifndef HOST_H
#define HOST_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <sstream>
#include <thread>
#include <unordered_set>
#include <mutex>
#include <condition_variable>
#include "blockingQueue.hpp"
#include "game.hpp"

const std::unordered_set<std::string> integerCommands {"road", "settlement", "city", "robber", "knight"};
const std::unordered_set<std::string> stringCommands {"buyDev", "monopoly", "yearofplenty", "discard", "trade"};
std::condition_variable cv;
std::mutex cv_m;

bool isInteger(const std::string &str);
std::vector<std::string> splitString(const std::string &str);
std::string blockingReceive(int sock);
std::pair<std::string, std::string> splitFirstTwoTerms(const std::string& str);
bool handleCommand(Game* game, int player, std::vector<std::string> args, bool firstTurn);
void untilValid(Game* game, blockingQueue<std::pair<int, std::string>>* moveQueue, int player, std::string command, bool firstTurn);
void handlePlayerThread(int sock, int playerArg, bool* waitingForPlayers, bool* isSessionActive, blockingQueue<std::pair<int, std::string>>* moveQueue);
void handleConnectionsThread(int server_fd, sockaddr_in* address, std::vector<std::pair<int, int>>* sockAddrs, Game* game, bool* waitingForPlayers, bool* isSessionActive, blockingQueue<std::pair<int, std::string>>* moveQueue);
void broadcastNewBoards(Game* game, std::vector<std::pair<int, int>>& sockPairs, int rollNum, int playerNum);
int main(int argc, char** argv);

#endif