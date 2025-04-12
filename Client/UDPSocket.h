#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include <WinSock2.h>
#include <WS2tcpip.h>

#include <string>

#pragma comment(lib, "ws2_32.lib")

class UDPSocket {
public:
    UDPSocket();
    ~UDPSocket();

    void Bind(const std::string& ip, unsigned short port);
    void SendData(SOCKET sckt, const sockaddr_in& dst, const char* data, size_t dataSize);
    int ReceiveData(char* buffer, size_t bufferSize, sockaddr_in& senderAddr);

    sockaddr_in getLastSender() const;
    SOCKET GetSocket() const;

private:
    SOCKET m_socket;
    sockaddr_in m_lastSender;
};

#endif