#include "UDPSocket.h"
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <ws2tcpip.h>

std::string _NAME;

void ReceiveMessages(UDPSocket& udpSocket) {
    char buffer[1024];
    sockaddr_in senderAddr;

    while (true) {
        int bytesReceived = udpSocket.ReceiveData(buffer, sizeof(buffer) - 1, senderAddr);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            std::string rawMsg(buffer);

            std::istringstream iss(rawMsg);
            std::string senderName;
            iss >> senderName;

            if (senderName != _NAME) {
                std::cout << rawMsg << std::endl;
            }
        }
    }
}

int main() {
    try {
        UDPSocket udpSocket;

        udpSocket.Bind("0.0.0.0", 0);

        sockaddr_in serverAddr = {};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(8888); 
        inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
        
        std::cout << "Enter your name: ";
        std::getline(std::cin, _NAME);

        std::thread receiver(ReceiveMessages, std::ref(udpSocket));

        std::string message;
        while (true) {
            std::cout << "";
            std::getline(std::cin, message);
            if (message.empty()) continue;

            std::string fullMessage = _NAME + " " + message;
            udpSocket.SendData(udpSocket.GetSocket(), serverAddr, fullMessage.c_str(), fullMessage.size());
        }

        receiver.join(); // Technically unreachable
    }
    catch (const std::exception& ex) {
        std::cerr << "Client error: " << ex.what() << "\n";
        return -1;
    }

    return 0;
}
