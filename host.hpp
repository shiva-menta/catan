#ifndef HOST_H
#define HOST_H

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

const std::unordered_set<std::string> integerCommands;
const std::unordered_set<std::string> stringCommands;

std::string blockingReceive(int sock);
std::pair<std::string, std::string> splitFirstTwoTerms(const std::string& str);
bool handleCommand(Game* game, int player, std::vector<std::string> args, bool firstTurn);
void untilValid(Game* game, queue<std::pair<int, std::string>>* moveQueue, int player, std::string command, bool firstTurn);
void handlePlayerThread(int sock, int playerArg, bool* waitingForPlayers, bool* isSessionActive, queue<std::pair<int, std::string>>* moveQueue);
bool isInteger(const std::string &str);
std::vector<std::string> splitString(const std::string &str);
void broadcastNewBoards(Game* game, std::vector<std::pair<int, int>>& sockPairs);
int main(int argc, char** argv);

#endif