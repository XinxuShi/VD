#include "stdafx.h"



using namespace std;
using namespace VDSuitMiniDevice;


void serializeMocapData(const _MocapData_& md, char* buffer, size_t bufferSize) {
    size_t offset = 0;

    memcpy(buffer + offset, &md.isUpdate, sizeof(md.isUpdate));
    offset += sizeof(md.isUpdate);

    memcpy(buffer + offset, &md.frameIndex, sizeof(md.frameIndex));
    offset += sizeof(md.frameIndex);

    memcpy(buffer + offset, &md.devicePower, sizeof(md.devicePower));
    offset += sizeof(md.devicePower);

    memcpy(buffer + offset, &md.frequency, sizeof(md.frequency));
    offset += sizeof(md.frequency);

    memcpy(buffer + offset, md.sensorState_body, sizeof(md.sensorState_body));
    offset += sizeof(md.sensorState_body);

    memcpy(buffer + offset, md.position_body, sizeof(md.position_body));
    offset += sizeof(md.position_body);

    memcpy(buffer + offset, md.quaternion_body, sizeof(md.quaternion_body));
    offset += sizeof(md.quaternion_body);

    memcpy(buffer + offset, md.sensorState_rHand, sizeof(md.sensorState_rHand));
    offset += sizeof(md.sensorState_rHand);

    memcpy(buffer + offset, md.position_rHand, sizeof(md.position_rHand));
    offset += sizeof(md.position_rHand);

    memcpy(buffer + offset, md.quaternion_rHand, sizeof(md.quaternion_rHand));
    offset += sizeof(md.quaternion_rHand);

    memcpy(buffer + offset, md.sensorState_lHand, sizeof(md.sensorState_lHand));
    offset += sizeof(md.sensorState_lHand);

    memcpy(buffer + offset, md.position_lHand, sizeof(md.position_lHand));
    offset += sizeof(md.position_lHand);

    memcpy(buffer + offset, md.quaternion_lHand, sizeof(md.quaternion_lHand));
    offset += sizeof(md.quaternion_lHand);

    memcpy(buffer + offset, &md.nsResult, sizeof(md.nsResult));
    offset += sizeof(md.nsResult);
}


void broadcastMocapData(const _MocapData_& md) {
    const size_t bufferSize = sizeof(_MocapData_);
    char buffer[bufferSize];
    serializeMocapData(md, buffer, bufferSize);
    extern int sockfd; // 声明全局变量
    extern struct sockaddr_in broadcastAddr;
    if (sendto(sockfd, buffer, bufferSize, 0, (struct sockaddr*)&broadcastAddr, sizeof(broadcastAddr)) < 0) {
        perror("sendto failed");
    }
}

int initializeSocket(struct sockaddr_in& broadcastAddr) {
    WSADATA wsaData;
    int sockfd;

    // 初始化Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("WSAStartup failed");
        exit(EXIT_FAILURE);
    }

    // 创建UDP套接字
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == INVALID_SOCKET) {
        perror("socket creation failed");
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // 设置套接字选项以允许广播
    int broadcastEnable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (char*)&broadcastEnable, sizeof(broadcastEnable)) < 0) {
        perror("setsockopt failed");
        closesocket(sockfd);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // 设置广播地址
    memset(&broadcastAddr, 0, sizeof(broadcastAddr));
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_port = htons(12345); // 替换为您实际使用的端口

    // 使用InetPton替代inet_addr
    if (InetPton(AF_INET, L"192.168.1.31", &broadcastAddr.sin_addr.s_addr) <= 0) {
        perror("InetPton failed");
        closesocket(sockfd);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

void cleanupSocket(int sockfd) {
    closesocket(sockfd);
    WSACleanup();
}