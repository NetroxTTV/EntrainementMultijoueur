#include "server.h"
#include "UDPSocket.h"

void SendDataToClients(UDPSocket& udpSocket, const char* data, size_t dataSize) {
    for (const auto& client : clientAddresses) {
        const auto& addr = client.second;
        udpSocket.SendData(udpSocket.GetSocket(), addr, data, dataSize);
    }
}

void ReceiveDataThread(UDPSocket& udpSocket) {
    char buffer[1024];
    sockaddr_in senderAddr;

    while (true) {
        int bytesReceived = udpSocket.ReceiveData(buffer, sizeof(buffer), senderAddr);
        if (bytesReceived > 0) {
            if (bytesReceived >= sizeof(buffer))
                bytesReceived = sizeof(buffer) - 1;
            buffer[bytesReceived] = '\0';

            std::istringstream iss(buffer);
            std::string name, chatMsg;

            iss >> name;
            std::getline(iss, chatMsg);
            if (!chatMsg.empty() && chatMsg[0] == ' ') chatMsg.erase(0, 1);

            std::cout << name << " " << chatMsg << std::endl;

            std::ostringstream oss;
            oss << name << " : " << chatMsg;
            std::string modifiedData = oss.str();

            clientAddresses[ntohs(senderAddr.sin_port)] = senderAddr;

            SendDataToClients(udpSocket, modifiedData.c_str(), modifiedData.size());
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