#ifndef PLAYER_H
#define PLAYER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <thread>

using namespace std;

void processServerMessage(string message);
string blockingReceive(int sock);
void receiveThreadFunction(int sock, bool* sessionOngoing);

int main();

#endif