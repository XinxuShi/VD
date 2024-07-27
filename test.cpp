#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <cstdlib>

#define PORT 12345
#define BUFFER_SIZE 65536

#define NODES_BODY 23
#define NODES_HAND 20

typedef enum SENSORSTATE {
    SS_NONE = 0,                    // NONE. 不获取该传感器信息。
    SS_Well,                        // well.
    SS_NoData,                      // no data.
    SS_UnReady,                     // on initial.
    SS_BadMag,                      // the magnetic field is disturbed.
}_SensorState_;

typedef enum BODYNODES {
    BN_Hips = 0,                    ///< Hips
    BN_RightUpperLeg,               ///< Right Upper Leg
    BN_RightLowerLeg,               ///< Right Lower Leg
    BN_RightFoot,                   ///< Right Foot
    BN_RightToe,                    ///< Right Toe
    BN_LeftUpperLeg,                ///< Left Upper Leg
    BN_LeftLowerLeg,                ///< Left Lower Leg
    BN_LeftFoot,                    ///< Left Foot
    BN_LeftToe,                     ///< Left Toe
    BN_Spine,                       ///< Spine
    BN_Spine1,                      ///< Spine1
    BN_Spine2,                      ///< Spine2
    BN_Spine3,                      ///< Spine3 -- Back
    BN_Neck,                        ///< Neck
    BN_Head,                        ///< Head
    BN_RightShoulder,               ///< Right Shoulder
    BN_RightUpperArm,               ///< Right Upper Arm
    BN_RightLowerArm,               ///< Right Lower Arm
    BN_RightHand,                   ///< Right Hand
    BN_LeftShoulder,                ///< Left Shoulder
    BN_LeftUpperArm,                ///< Left Upper Arm
    BN_LeftLowerArm,                ///< Left Lower Arm
    BN_LeftHand,                    ///< Left Hand
}_BodyNodes_;

typedef enum HANDNODES {
    HN_Hand = 0,
    HN_ThumbFinger,
    HN_ThumbFinger1,
    HN_ThumbFinger2,
    HN_IndexFinger,
    HN_IndexFinger1,
    HN_IndexFinger2,
    HN_IndexFinger3,
    HN_MiddleFinger,
    HN_MiddleFinger1,
    HN_MiddleFinger2,
    HN_MiddleFinger3,
    HN_RingFinger,
    HN_RingFinger1,
    HN_RingFinger2,
    HN_RingFinger3,
    HN_PinkyFinger,
    HN_PinkyFinger1,
    HN_PinkyFinger2,
    HN_PinkyFinger3,
}_HandNodes_;

typedef struct MOCAPDATA {
    bool isUpdate;                                   // true表示设备数据已更新
    int frameIndex;                                  // 帧序号
    float devicePower;                               // 设备电量
    int frequency;                                   // 设备数据传输频率

    _SensorState_ sensorState_body[NODES_BODY];      // 设备全身（不包括手指）各传感器工作状态（按_BodyNodes_序号排列，下同）
    float position_body[NODES_BODY][3];              // 全身（不包括手指）各节点坐标，单位m
    float quaternion_body[NODES_BODY][4];            // 全身（不包括手指）各节点四元数

    _SensorState_ sensorState_rHand[NODES_HAND];     // 设备双手（手掌及手指）各传感器工作状态（按_HandNodes_序号排列，下同）
    float position_rHand[NODES_HAND][3];             // 双手（手掌及手指）各节点坐标，单位m
    float quaternion_rHand[NODES_HAND][4];           // 双手（手掌及手指）各节点四元数

    _SensorState_ sensorState_lHand[NODES_HAND];     // 设备双手（手掌及手指）各传感器工作状态（按_HandNodes_序号排列，下同）
    float position_lHand[NODES_HAND][3];             // 双手（手掌及手指）各节点坐标，单位m
    float quaternion_lHand[NODES_HAND][4];           // 双手（手掌及手指）各节点四元数

    int nsResult;                                    // 其它数据
}_MocapData_;

void deserializeMocapData(char* buffer, _MocapData_ &md) {
    size_t offset = 0;

    memcpy(&md.isUpdate, buffer + offset, sizeof(md.isUpdate));
    offset += sizeof(md.isUpdate);

    memcpy(&md.frameIndex, buffer + offset, sizeof(md.frameIndex));
    offset += sizeof(md.frameIndex);

    memcpy(&md.devicePower, buffer + offset, sizeof(md.devicePower));
    offset += sizeof(md.devicePower);

    memcpy(&md.frequency, buffer + offset, sizeof(md.frequency));
    offset += sizeof(md.frequency);

    memcpy(md.sensorState_body, buffer + offset, sizeof(md.sensorState_body));
    offset += sizeof(md.sensorState_body);

    memcpy(md.position_body, buffer + offset, sizeof(md.position_body));
    offset += sizeof(md.position_body);

    memcpy(md.quaternion_body, buffer + offset, sizeof(md.quaternion_body));
    offset += sizeof(md.quaternion_body);

    memcpy(md.sensorState_rHand, buffer + offset, sizeof(md.sensorState_rHand));
    offset += sizeof(md.sensorState_rHand);

    memcpy(md.position_rHand, buffer + offset, sizeof(md.position_rHand));
    offset += sizeof(md.position_rHand);

    memcpy(md.quaternion_rHand, buffer + offset, sizeof(md.quaternion_rHand));
    offset += sizeof(md.quaternion_rHand);

    memcpy(md.sensorState_lHand, buffer + offset, sizeof(md.sensorState_lHand));
    offset += sizeof(md.sensorState_lHand);

    memcpy(md.position_lHand, buffer + offset, sizeof(md.position_lHand));
    offset += sizeof(md.position_lHand);

    memcpy(md.quaternion_lHand, buffer + offset, sizeof(md.quaternion_lHand));
    offset += sizeof(md.quaternion_lHand);

    memcpy(&md.nsResult, buffer + offset, sizeof(md.nsResult));
}

void receiveMocapData() {
    int sockfd;
    struct sockaddr_in recvAddr;
    char buffer[BUFFER_SIZE];

    // 创建UDP套接字
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 绑定套接字到端口
    memset(&recvAddr, 0, sizeof(recvAddr));
    recvAddr.sin_family = AF_INET;
    recvAddr.sin_port = htons(PORT);
    recvAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (const struct sockaddr *)&recvAddr, sizeof(recvAddr)) < 0) {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    while (true) {
        socklen_t len = sizeof(recvAddr);
        int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&recvAddr, &len);
        if (n < 0) {
            perror("recvfrom failed");
            continue;
        }

        _MocapData_ md;
        deserializeMocapData(buffer, md);

        // 处理接收到的数据
        std::string str = "\n====== pose: position_xyz, quat_wxyz ======\n";
        for (int ii = 0; ii < NODES_BODY; ii++)
        {
            for (int i = 0; i < 3; i++) { str += std::__cxx11::to_string(md.position_body[ii][i]) + " "; }
            str += "\t";
            for (int i = 0; i < 4; i++) { str += std::__cxx11::to_string(md.quaternion_body[ii][i]) + " "; }
            str += "\n";
        }
        std::cout << str << std::endl;
    }

    // 关闭套接字
    close(sockfd);
}

int main() {
    receiveMocapData();
    return 0;
}