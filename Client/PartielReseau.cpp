#include "framework.h"
#include "UDPSocket.h"
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <ws2tcpip.h>

void ReceiveMessages(UDPSocket& udpSocket) {
	char buffer[1024]; 
	sockaddr_in senderAddr;

	while (true) {
		int bytesReceived = udpSocket.ReceiveData(buffer, sizeof(buffer) - 1, senderAddr);
		if (bytesReceived > 0) {
			buffer[bytesReceived] = '\0';
			std::string rawMsg(buffer);
			std::cout << rawMsg << std::endl;
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
        
		std::cout << "Send an operation to the server : \n";

		std::thread receiver(ReceiveMessages, std::ref(udpSocket));

		std::string message;
		while (true) {
			std::cout << "";
			std::getline(std::cin, message);
			if (message.empty()) continue;

			udpSocket.SendData(udpSocket.GetSocket(), serverAddr, message.c_str(), message.size());
		}

		receiver.join();
	}
	catch (const std::exception& ex) {
		std::cerr << "Client error: " << ex.what() << "\n";
		return -1;
	}

	return 0;
}