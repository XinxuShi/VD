#pragma once
#ifndef VDMOCAPSDK_VDSUITMINI_DATATYPE_H
#define VDMOCAPSDK_VDSUITMINI_DATATYPE_H

#define NODES_BODY 23
#define NODES_HAND 20
#define PC_NODES_VIRTUAL 18

namespace VDSuitMiniDevice
{
	/**
	* @brief
	*   Body nodes name and its index.
	*/
	typedef enum BODYNODES
	{
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


	/**
	* @brief
	*   Hand nodes name and its index.
	*/
	typedef enum HANDNODES
	{
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



	// �豸����
	typedef enum DEVICETYPE
	{
		DT_NONE = -1,
		DT_Auto = 0,
		//DT_VDSuit = 1,
		DT_VDSuitMini = 2,  //����
		DT_VDSuitFull = 3,   //������
	}_DeviceType_;



	typedef enum SENSORSTATE {
		SS_NONE = 0,                    //NONE. ����ȡ�ô�������Ϣ��
		SS_Well,                        //well.
		SS_NoData,                      //no data.
		SS_UnReady,                     //on initial.
		SS_BadMag,                      //the magnetic field is disturbed.
	}_SensorState_;



	typedef enum WORLDSPACE {
		WS_Geo = 0,                        //��ʾ��������ϵΪ��������ϵ
		WS_Unity,                          //��ʾ��������ϵΪUnity��������ϵ
		WS_UE4,                            //��ʾ��������ϵΪUE4��������ϵ
	}_WorldSpace_;



	typedef enum CALIBRATIONMODE {
		CM_Apose = 0, //Apose�궨ģʽ������Apose��վ����˫����ֱƽ�У�˫����ָ����ǰ����˫����Ȼ����ƽ����������ԣ���ָ��ֱ����Ĵָ��ʳָ��45�ȼнǹ�ϵ�������ڱ궨�����в�����ǰ��������б��Ҳ���������ձ���
		CM_Ppose,   //Ppose�궨������Ppose����Aposeվ���Ļ����ϣ�������ͬʱˮƽ����ǰ��ƽ����ƽ�У��������£���ָ��ֱ����Ĵָ��ʳָ��45�ȼнǹ�ϵ������Apose�궨���pose�궨�������ֲ��ı궨��
	}_CalibrationMode_;



	typedef enum CALIBRATIONSTATE {
		CS_UnStart = 0,                    //δ��ʼ�궨״̬
		CS_InPose,                         //����pose�궨״̬
		CS_Successed,                      //�궨�ɹ�
		CS_Failed,                         //�궨ʧ��
	}_CalibrationState_;



	typedef enum FREQUENCY {
		HZ_60 = 60,
		HZ_72 = 72,
		HZ_80 = 80,
		HZ_96 = 96,
		//HZ_120 = 120,
	}_Frequency_;

	typedef struct MAGCORRECTRESULT
	{
		bool isFinished = false;  //�Ƿ����
		bool isHaveSucceed = false;  //�Ƿ���У׼�ɹ��Ĵ���������Ϊfalse���������������Ч
		float progress = 0;
		int bLength = 0;
		_BodyNodes_ bFailedNodes[NODES_BODY] = { BN_Hips };
		int rLength = 0;
		_HandNodes_ rFailedNodes[NODES_HAND] = { HN_Hand };
		int lLength = 0;
		_HandNodes_ lFailedNodes[NODES_HAND] = { HN_Hand };
	}_MagCorrectResult_;



	typedef struct CALIBRATIONPROGRESS {
		_CalibrationState_ state;
		float progress;                         //�� state Ϊ CS_Apose ʱ��Ч
	}_CalibrationProgress_;



	typedef struct MOCAPDATA {
		bool isUpdate;                                   //true��ʾ�豸�����Ѹ���
		int frameIndex;                                  //֡���
		float devicePower;                               //�豸����
		int frequency;                                   //�豸���ݴ���Ƶ��

		_SensorState_ sensorState_body[NODES_BODY];      //�豸ȫ����������ָ��������������״̬����_BodyNodes_������У���ͬ��	
		float position_body[NODES_BODY][3]/*xyz-m*/;     //ȫ����������ָ�����ڵ����꣬��λm
		float quaternion_body[NODES_BODY][4]/*wxyz*/;    //ȫ����������ָ�����ڵ���Ԫ��

		_SensorState_ sensorState_rHand[NODES_HAND];     //�豸˫�֣����Ƽ���ָ��������������״̬����_HandNodes_������У���ͬ��	
		float position_rHand[NODES_HAND][3]/*xyz-m*/;    //˫�֣����Ƽ���ָ�����ڵ����꣬��λm
		float quaternion_rHand[NODES_HAND][4]/*wxyz*/;   //˫�֣����Ƽ���ָ�����ڵ���Ԫ��

		_SensorState_ sensorState_lHand[NODES_HAND];     //�豸˫�֣����Ƽ���ָ��������������״̬����_HandNodes_������У���ͬ��	
		float position_lHand[NODES_HAND][3]/*xyz-m*/;    //˫�֣����Ƽ���ָ�����ڵ����꣬��λm
		float quaternion_lHand[NODES_HAND][4]/*wxyz*/;   //˫�֣����Ƽ���ָ�����ڵ���Ԫ��

		int nsResult;                                  //��������
	}_MocapData_;

	
	typedef enum GESTURE {
		GESTURE_NONE = 0,  //δ֪����
		GESTURE_1,         //������
		GESTURE_2,         //ȭͷ
		GESTURE_3,         //��
		GESTURE_4,         //OK
		GESTURE_5,         //����
		GESTURE_6,         //��ǹ״
		GESTURE_7,         //��Ĵָ��ʳָ��Сָ��ֱ��������ָ��£
		GESTURE_8,         //�ã������ץ�����յĸ�£
		GESTURE_9,         //ץ
		GESTURE_10,        //��Ĵָ��ʳָ��ֱ��£
		GESTURE_11,        //ʳָ��ֱ��������ָ��£
	}_Gesture_;




	//
	typedef struct VERSION
	{
		unsigned char Project_Name[26] = { 0 };
		unsigned char Author_Organization[128] = { 0 };
		unsigned char Author_Domain[26] = { 0 };
		unsigned char Author_Maintainer[26] = { 0 };
		unsigned char Version[26] = { 0 };
		unsigned char Version_Major;
		unsigned char Version_Minor;
		unsigned char Version_Patch;
	}_Version_;


	typedef enum NoticeMode
	{
		NOPRITF = 0,
		NOTICE_PRINTF,
		DEBUG_PRINTF,
	}_NoticeMode_;



	

	typedef void(*VDMOCAPDATA_CALLBACK)(_MocapData_ mocapData); //���庯���ص�ָ��
	
	typedef void(*DEVICEBREAK_CALLBACK)();

	//���˻ص��ӿ�
	typedef void(*VDMOCAPDATA_CALLBACK_Mult)(_MocapData_ mocapData, int index); //���庯���ص�ָ��

	typedef void(*DEVICEBREAK_CALLBACK_Mult)(int index);



}//end namespace

#endif
