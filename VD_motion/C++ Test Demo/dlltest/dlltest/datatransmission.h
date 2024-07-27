#pragma once


#include "VDMocapSDK_VDSuitMini_DataType.h"
#include <iostream>
#include <Winsock2.h>
#include <windows.h>
#include <Ws2tcpip.h>

void serializeMocapData(const VDSuitMiniDevice::_MocapData_& md, char* buffer, size_t bufferSize);
void broadcastMocapData(const VDSuitMiniDevice::_MocapData_& md);
int initializeSocket(struct sockaddr_in& broadcastAddr);
void cleanupSocket(int sockfd);