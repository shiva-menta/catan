#ifndef PLAYER_H
#define PLAYER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <thread>

void processServerMessage(std::string message);
std::string blockingReceive(int sock);
void receiveThreadFunction(int sock, bool* sessionOngoing);
int main(int argc, char** argv);

#endif