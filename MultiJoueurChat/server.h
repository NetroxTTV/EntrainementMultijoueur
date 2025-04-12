#pragma once
#include "UDPSocket.h"

#include <iostream>
#include <sstream>
#include <cstring>
#include <map>
#include <thread>
#include <vector>

struct Position {
    int x = 100;
    int y = 100;
};

std::map<int, sockaddr_in> clientAddresses;
std::map<int, Position> clientPositions;

int nextProjectileID = 10;

std::map<int, std::string> clientNames;