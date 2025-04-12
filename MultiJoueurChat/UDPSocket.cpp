#include "UDPSocket.h"
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

UDPSocket::UDPSocket() {
    WSADATA data;
    if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
        std::cerr << "WSAStartup failed. Error: " << WSAGetLastError() << "\n";
        throw std::runtime_error("WSAStartup failed");
    }
    m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_socket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed. Error: " << WSAGetLastError() << "\n";
        throw std::runtime_error("Socket creation failed");
    }
}

UDPSocket::~UDPSocket() {
    closesocket(m_socket);
    WSACleanup();
}

void UDPSocket::Bind(const std::string& ip, unsigned short port) {
    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr);

    if (::bind(m_socket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed. Error: " << WSAGetLastError() << "\n";
        throw std::runtime_error("Bind failed");
    }
}

// SEND DATA
void UDPSocket::SendData(SOCKET sckt, const sockaddr_in& dst, const char* data, size_t dataSize) {
    int result = sendto(sckt, data, dataSize, 0, (struct sockaddr*)&dst, sizeof(dst));
    if (result == SOCKET_ERROR) {
        std::cerr << "sendto failed. Error: " << WSAGetLastError() << "\n";
    }
}

// RECEIVE DATA
int UDPSocket::ReceiveData(char* buffer, size_t bufferSize, sockaddr_in& senderAddr) {
    int addrLen = sizeof(senderAddr);
    int result = recvfrom(m_socket, buffer, bufferSize, 0, (struct sockaddr*)&senderAddr, &addrLen);
    if (result == SOCKET_ERROR) {
        std::cerr << "recvfrom failed. Error: " << WSAGetLastError() << "\n";
        return -1;
    }
    return result;
}

SOCKET UDPSocket::GetSocket() const {
    return m_socket;
}

sockaddr_in UDPSocket::getLastSender() const {
    return m_lastSender;
}