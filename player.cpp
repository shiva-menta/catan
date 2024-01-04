#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <thread>

using namespace std;

void processServerMessage(string message) {
    system("clear");
    cout << message << endl;
    return;
}

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

void receiveThreadFunction(int sock, bool* sessionOngoing) {
    while (*sessionOngoing == true) {
        string message = blockingReceive(sock);
        if (!message.empty()) {
            processServerMessage(message);
        }
    }
}

int main(int argc, char** argv) {
    // Sanity Checks
    if (argc != 3) {
        cerr << "Incorrect arguments provided." << endl;
        return -1;
    }
    string arg1 = argv[1];
    if (arg1.empty() || !all_of(arg1.begin(), arg1.end(), ::isdigit)) {
        cerr << "Invalid port number." << endl;
        return -1;
    };
    int addr = stoi(arg1);
    char* hostAddr = argv[2];

    // Create Socket
    struct sockaddr_in serv_addr;
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    // Establish Server Address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(addr);

    // Convert IP Address
    if (inet_pton(AF_INET, hostAddr, &serv_addr.sin_addr) <= 0) {
        cerr << "Invalid Address" << endl;
        return -1;
    };

    // Connect to Server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        cerr << "Connection to Server Failed" << endl;
        return -1;
    };

    // Launch Read & Update Thread
    bool sessionOngoing = true;
    thread receiveThread(receiveThreadFunction, sock, &sessionOngoing);

    // Loop to Process User Commands & Send
    string input;
    while (getline(cin, input)) {
        if (input == "exit") {
            sessionOngoing = false;
            break;
        }
        int dataSize = input.size();
        if (write(sock, input.c_str(), dataSize) == -1) {
            cerr << "Error sending message.";
        }
    }

    // Close Socket & Terminate
    receiveThread.join();
    close(sock);
    return 0;
}