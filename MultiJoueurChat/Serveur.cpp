#include "Serveur.h"
#include "framework.h"
#include "UDPSocket.h"

void SendDataToClients(UDPSocket& udpSocket, sockaddr_in senderAddr, const char* data, size_t dataSize) {
    udpSocket.SendData(udpSocket.GetSocket(), senderAddr, data, dataSize);
}

std::string Calculator(const std::string& calc) {
    std::istringstream iss(calc);
    float a, b;
    char operation;

    if (!(iss >> a >> operation >> b)) {
        return "ERROR | Use +, -, *, or /.";
    }

    switch (operation) {
    case '+': 
        return "Result: " + std::to_string(a + b);      
    case '-': 
        return "Result: " + std::to_string(a - b);
    case '*': 
        return "Result: " + std::to_string(a * b);
    case '/':
        if (a == 0 || b == 0)
            return "ERROR | Division by zero.";
        return "Result: " + std::to_string(a / b);
    default: 
        return "ERROR | Use +, -, *, or /.";
    }
}

void ReceiveDataThread(UDPSocket& udpSocket) {
    char buffer[1024];
    sockaddr_in senderAddr; 
    const char* end = "Send another calculation: ";

    while (true) {
        int bytesReceived = udpSocket.ReceiveData(buffer, sizeof(buffer), senderAddr);
        if (bytesReceived > 0) {
            if (bytesReceived >= sizeof(buffer))
                bytesReceived = sizeof(buffer) - 1;
            buffer[bytesReceived] = '\0';

            std::string receivedData(buffer);
            std::string operationAnswer = Calculator(receivedData);

            std::cout << "[LOG] : " << senderAddr.sin_port << " | result : " << operationAnswer.c_str() << "\n";

            SendDataToClients(udpSocket, senderAddr, operationAnswer.c_str(), operationAnswer.size());
            SendDataToClients(udpSocket, senderAddr, end, strlen(end));
        }
    }
}

int main() {
    try { 
        UDPSocket udpSocket;
        udpSocket.Bind("0.0.0.0", 8888);

        HANDLE hThread = CreateThread(
            NULL,
            0,
            (LPTHREAD_START_ROUTINE)ReceiveDataThread,
            &udpSocket,
            0,
            NULL
        );

        if (hThread == NULL) {
            std::cerr << "Error: Unable to create thread.\n";
            return 1;
        }

        std::cout << "Server is running...\nPress Enter to exit.\n";
        std::cin.get();

        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
    }
    catch (const std::exception& ex) {
        std::cerr << "Server error: " << ex.what() << "\n";
        return -1;
    }

    return 0;
}