// useQTDLL_vscpp.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>
#include <fstream>
#include "VDMocapSDK_VDSuitMini_DataType.h"
#include "datatransmission.h"

#pragma comment(lib, "ws2_32.lib")


using namespace std;
using namespace VDSuitMiniDevice;

int sockfd;  // ȫ�ֱ���
struct sockaddr_in broadcastAddr;  // ȫ�ֱ���


// ����ص�����ָ�롣
// ��ȡ�������ݣ���������������ϵ�µġ�
typedef void(*mocapDataCallBack)(_MocapData_ md);
typedef void(*SetVDMocapDataCallBackFunc)(mocapDataCallBack);
SetVDMocapDataCallBackFunc setVDMocapDataCallBackFunc;

// ��ȡ�豸�������Ͽ��ź�
typedef void(*deviceBreakCallBack)();
typedef void(*SetDeviceBreakCallBackFunc)(deviceBreakCallBack BreakCallBack);
SetDeviceBreakCallBackFunc setDeviceBreakCallBackFunc;

// ��ʼ������Ҫ����ģ�͹Ǽ�Tpose���ڵ����꣨��������ϵ��
typedef void(*Initial)(_WorldSpace_ worldSpace, float InitialPosition_Body[NODES_BODY][3],
	float InitialPosition_rHand[NODES_HAND][3], float InitialPosition_lHand[NODES_HAND][3], int dllIndex);
Initial initial;

//typedef void(*InitialMocap)(char* path, _WorldSpace_ worldSpace, float InitialPosition_Body[NODES_BODY][3],
//	float InitialPosition_rHand[NODES_HAND][3], float InitialPosition_lHand[NODES_HAND][3], int dllIndex);
//InitialMocap initialMocap;

//�����豸
typedef bool(*Connect)();
Connect connectDevice;

//�Ͽ�����
typedef void(*DisConnect)();
DisConnect disConnect;

//���ӳɹ��󣬳���ͨ���ص�������ȡ���������⣬Ҳ��ͨ���ú�����ȡ�������ݡ�
//��ȡ�����ݶ�������������ϵ�µġ�
typedef void(*RecvMocapData)(_MocapData_* md);
RecvMocapData recvMocapData;

//��ȡ����״̬
typedef bool(*GetConnectState)();
GetConnectState getConnectState;

//��ȡ�����豸�����ͣ�VDSuitMini��VDSuitFull��
typedef _DeviceType_(*GetDeviceType)();
GetDeviceType getDeviceType;

//��ȡ�豸���ݴ���Ƶ��
typedef int(*GetFrequency)();
GetFrequency getFrequency;

//��ȡ�豸����
typedef float(*GetDevicePower)();
GetDevicePower getDevicePower;

//�����豸���ݴ���Ƶ��
typedef bool(*SetFrequency)(_Frequency_ frequency);
SetFrequency setFrequency;

//��ģ�͵ĳ�ʼĿ��������ÿ���ڵ������
typedef void(*SetModelNodesPositionInInitialTpose)( float InitialPosition_Body[NODES_BODY][3],
	float InitialPosition_rHand[NODES_HAND][3], float InitialPosition_lHand[NODES_HAND][3]);
SetModelNodesPositionInInitialTpose setModelNodesPositionInInitialTpose;

/*-------------------------------------�궨����		start--------------------------------------------------*/

//�궨��"Apose"�궨�Ǳ�����еģ�"Ppose"�궨�ǿ�ѡ�ġ�
//�ñ궨��ʶ�𴩴�������pose��궨pose�Ƿ�һ�£����궨�еĶ����̶ȣ�������̫���������޷��궨��ɡ�
typedef void(*StartCalibration)(_CalibrationMode_ calibrationMode, float quat_EndCalibration_root[4]);
StartCalibration startCalibration;

//���ٱ궨��"Apose"�궨�Ǳ�����еģ�"Ppose"�궨�ǿ�ѡ�ġ�
//�궨�ٶȱ� StartCalibration() ��ܶ࣬����Ҫ�����������ñ궨pose��Ȼ����ִ�иú������б궨��
typedef bool(*StartCalibrationFast)(_CalibrationMode_ calibrationMode, float quat_EndCalibration_root[4]);
StartCalibrationFast startCalibrationFast;

//ȡ���궨
typedef void(*CancelCalibration)();
CancelCalibration cancelCalibration;

//��ȡ�궨����
typedef _CalibrationProgress_(*GetCalibrationProgress)();
GetCalibrationProgress getCalibrationProgress;

//���ñ궨����������Ķ����̶ȣ���Χ[0, 1]��Ĭ��Ϊ0�����ϸ񣩣��ɲ����á�
typedef bool(*SetShakeDegreeForCalibration)(float degree);
SetShakeDegreeForCalibration setShakeDegreeForCalibration;

/*-------------------------------------�궨����		end--------------------------------------------------*/

/*-------------------------------------�����ƺ���		start--------------------------------------------------*/


//��ʼ������У׼
typedef bool(*StartMagCorrect)();
StartMagCorrect startMagCorrect;

//ȡ��������У׼
typedef void(*CancelMagCorrect)();
CancelMagCorrect cancelMagCorrect;

//����������У׼
typedef void(*EndMagCorrect)();
EndMagCorrect endMagCorrect;

typedef bool(*GetMagCorrectResult)(_MagCorrectResult_* magCorrectResult);
GetMagCorrectResult getMagCorrectResult;

/*-------------------------------------�����ƺ���		end--------------------------------------------------*/


/*-------------------------------------��������		end--------------------------------------------------*/
//������
typedef void(*MakeBuzz)();
MakeBuzz makeBuzz;

//����
typedef void(*GetGesture)(_Gesture_* gesture_R, _Gesture_* gesture_L);
GetGesture getGesture;

/*-------------------------------------��������		end--------------------------------------------------*/



/*-------------------------------------��̬���ں���		start--------------------------------------------------*/
//����������ȡ���ݺ󼴿ɵ��ڣ�����չʾ
//����ģ�ͳ��� [-1,1],It's minus 180 to 180 degrees.
typedef void(*SetPositionDirection)(float AdjustYaw);
SetPositionDirection setPositionDirection;

//ģ��λ�Ƹ�λ
typedef void(*PositionBackToOrigin)();
PositionBackToOrigin positionBackToOrigin;

//�����ٶ����̶ȣ����ڵ�����Ծ��ˮƽƯ��
//[0 or 1], default value 00: default mode, without restriction.   1 : in situ mode, Limit it to a small area
typedef void(*SetVelocityMode)(int m_VelocityMode);
SetVelocityMode setVelocityMode;

//���ø߲����̶ȣ�Ӧ���ڵ������ߵĶٴ�,[0,10], default value 2
typedef void(*SetWalkingFallValue)(float m_WalkingFallValue);
SetWalkingFallValue setWalkingFallValue;

//�����ֱ��ź϶ȣ�Ӧ���ڵ��ں��Ƶȶ���Ч��, [-0.5, 0.5], default value 0
typedef void(*SetArmHorizontal)(float m_ArmHorizontal);
SetArmHorizontal setArmHorizontal;

//�����ֱ۳�ʼ�ǣ�Ӧ�õ�ʵʱ��ģ����,[-1.5,1.5], default value 0
typedef void(*SetArmVerticality)(float m_ArmVerticality);
SetArmVerticality setArmVerticality;

//�������ܵȼ��� ���ܵȼ�����[0 ���������, ��ʱ���; 1 �������ȶ�, ��ʱ����;,default value1
typedef void(*SetPerformanceLevel)(int m_PerformanceLeve);
SetPerformanceLevel setPerformanceLevel;

/*-------------------------------------��̬���ں���		end--------------------------------------------------*/





//
_CalibrationMode_ cm = CM_Apose;
_CalibrationProgress_ cp;


bool MagCorrectState = false;    //�Ƿ�������У׼״̬       ��У׼��ɺ���Ҫ�����豸������Ч��Ҳ���Բ��sign out������������
int MagCorrectTime = 30;		//����		����У׼��30s			���У׼��75s

// ����ص��������� 
void GetMocapData(_MocapData_ md)
{
	//���ݴ�ӡ��ʱ�ϳ������ܵ��¶�֡�������������ġ�
	if (!MagCorrectState)       //��У׼ʱ����ӡ���ݣ���ֹ������У׼���
	{
		
		string str = "\n====== pose: position_xyz, quat_wxyz ======\n";
		for (int ii = 0; ii < NODES_BODY; ii++)
		{
			for (int i = 0; i < 3; i++) { str += to_string(md.position_body[ii][i]) + " "; }
			str += "\t";
			for (int i = 0; i < 4; i++) { str += to_string(md.quaternion_body[ii][i]) + " "; }
			str += "\n";
		}
		cout << str << endl;
		

		broadcastMocapData(md);

	}
}

//
void DeviceBreak()
{
	cout << "====== �豸�������Ͽ��� ======" << endl;
}

//
void DebugSome()
{		
	//cp��ʼ��
	cp.state = CS_UnStart;
	cp.progress = 0;



	//
	while (true)
	{
		Sleep(20);

		cp = getCalibrationProgress();
		if (cp.state != CS_UnStart) {
			printf("�궨����: state: %d   progress: %.2f\n", cp.state, cp.progress);
			if (cp.state == CS_Successed || cp.state == CS_Failed) 
			{ 
				break;
			}
		}
	}
	return;
}


//
int main()
{

	
	sockfd = initializeSocket(broadcastAddr);
    //����dll 
    HINSTANCE hDll = LoadLibraryA("E:\\Documents\\Cyber\\VD_motion\\lib\\x64\\VDMocapSDK_VDSuitMini.dll");
	//
	if (hDll != NULL)
	{
		//
		setVDMocapDataCallBackFunc = (SetVDMocapDataCallBackFunc)GetProcAddress(hDll, "SetVDMocapDataCallBackFunc");
		setDeviceBreakCallBackFunc = (SetDeviceBreakCallBackFunc)GetProcAddress(hDll, "SetDeviceBreakCallBackFunc");
		initial = (Initial)GetProcAddress(hDll, "Initial");
		//initialMocap = (InitialMocap)GetProcAddress(hDll, "InitialMocap");
		connectDevice = (Connect)GetProcAddress(hDll, "Connect");
		disConnect = (DisConnect)GetProcAddress(hDll, "DisConnect");
		recvMocapData = (RecvMocapData)GetProcAddress(hDll, "RecvMocapData");
		getConnectState = (GetConnectState)GetProcAddress(hDll, "GetConnectState");
		getDeviceType = (GetDeviceType)GetProcAddress(hDll, "GetDeviceType");
		getFrequency = (GetFrequency)GetProcAddress(hDll, "GetFrequency");
		getDevicePower = (GetDevicePower)GetProcAddress(hDll, "GetDevicePower");
		setFrequency = (SetFrequency)GetProcAddress(hDll, "SetFrequency");
		startCalibration = (StartCalibration)GetProcAddress(hDll, "StartCalibration");
		startCalibrationFast = (StartCalibrationFast)GetProcAddress(hDll, "StartCalibrationFast");
		cancelCalibration = (CancelCalibration)GetProcAddress(hDll, "CancelCalibration");
		getCalibrationProgress = (GetCalibrationProgress)GetProcAddress(hDll, "GetCalibrationProgress");
		setShakeDegreeForCalibration = (SetShakeDegreeForCalibration)GetProcAddress(hDll, "SetShakeDegreeForCalibration");

		startMagCorrect = (StartMagCorrect)GetProcAddress(hDll, "StartMagCorrect");
		cancelMagCorrect = (CancelMagCorrect)GetProcAddress(hDll, "CancelMagCorrect");
		endMagCorrect = (EndMagCorrect)GetProcAddress(hDll, "EndMagCorrect");
		getMagCorrectResult = (GetMagCorrectResult)GetProcAddress(hDll, "GetMagCorrectResult");

		makeBuzz = (MakeBuzz)GetProcAddress(hDll, "MakeBuzz");
		getGesture = (GetGesture)GetProcAddress(hDll, "GetGesture");

		setPositionDirection = (SetPositionDirection)GetProcAddress(hDll, "SetPositionDirection");
		positionBackToOrigin = (PositionBackToOrigin)GetProcAddress(hDll, "PositionBackToOrigin");
		setVelocityMode = (SetVelocityMode)GetProcAddress(hDll, "SetVelocityMode");
		setWalkingFallValue = (SetWalkingFallValue)GetProcAddress(hDll, "SetWalkingFallValue");
		setArmHorizontal = (SetArmHorizontal)GetProcAddress(hDll, "SetArmHorizontal");
		setArmVerticality = (SetArmVerticality)GetProcAddress(hDll, "SetArmVerticality");
		setPerformanceLevel = (SetPerformanceLevel)GetProcAddress(hDll, "SetPerformanceLevel");


		//body nodes
		float InitialPosition_Body[NODES_BODY][3] = {
			{0, 1.01417, -0.01136745 },
		    {0.09608424, 0.9339905, -0.01918257},
			{0.1070137, 0.5183502, -0.03298733},
			{0.1029733, 0.09305052, -0.03527176},
			{0.1029734, 0.02005178, 0.07543653},
			{-0.09608424, 0.9339905, -0.01918257},
			{-0.1070137, 0.5183502, -0.03298733},
			{-0.1029733, 0.09305052, -0.03527176},
			{-0.1029734, 0.02005178, 0.07543653},
			{1.610145E-15, 1.101661, -0.01136744},
			{-1.967892E-16, 1.204164, -0.01136744},
			{1.722054E-16, 1.313723, -0.01136744},
			{3.313691E-17, 1.420459, 2.017152E-09},
			{1.412962E-16, 1.530355, -0.01136744},
			{-1.426205E-16, 1.62144, -0.01136744},
			{0.04465847, 1.45745, -0.01136743},
			{0.1871382, 1.45745, -0.01136743},
			{0.4227339, 1.45745, -0.01136743},
			{0.6813283, 1.45745, -0.01136743},
			{-0.04465845, 1.457453, -0.01136744},
			{-0.1871379, 1.457453, -0.01136744},
			{-0.4227338, 1.457453, -0.01136744},
			{-0.6813283, 1.457453, -0.01136744},
		};
		//rhand nodes
		float InitialPosition_rHand[NODES_HAND][3] = {
			{ 0.6813, 1.4575, -0.0114 },
			{ 0.7121, 1.4598, 0.0272 },
			{ 0.7444, 1.4597, 0.0592 },
			{ 0.7669, 1.4598, 0.0815 },
			{ 0.6813, 1.4575, -0.0114 },
			{ 0.7857, 1.4626, 0.0254 },
			{ 0.8304, 1.4604, 0.0255 },
			{ 0.8557, 1.4589, 0.0255 },
			{ 0.6813, 1.4575, -0.0114 },
			{ 0.7871, 1.4628, 0.0019 },
			{ 0.8358, 1.4595, 0.0017 },
			{ 0.8663, 1.4571, 0.0018 },
			{ 0.6813, 1.4575, -0.0114 },
			{ 0.7802, 1.4639, -0.0189 },
			{ 0.8226, 1.4599, -0.0188 },
			{ 0.8520, 1.4573, -0.0189 },
			{ 0.6813, 1.4575, -0.0114 },
			{ 0.7716, 1.4629, -0.0397 },
			{ 0.8056, 1.4611, -0.0398 },
			{ 0.8270, 1.4594, -0.0398 },
		};
		//lhand nodes
		float InitialPosition_lHand[NODES_HAND][3] = {
			{ -0.6813, 1.4575, -0.0114 },
			{ -0.7121, 1.4598, 0.0272 },
			{ -0.7444, 1.4599, 0.0593 },
			{ -0.7666, 1.4598, 0.0817 },
			{ -0.6813, 1.4575, -0.0114 },
			{ -0.7857, 1.4626, 0.0254 },
			{ -0.8304, 1.4604, 0.0253 },
			{ -0.8557, 1.4588, 0.0254 },
			{ -0.6813, 1.4575, -0.0114 },
			{ -0.7871, 1.4628, 0.0019 },
			{ -0.8358, 1.4595, 0.0017 },
			{ -0.8663, 1.4571, 0.0019 },
			{ -0.6813, 1.4575, -0.0114 },
			{ -0.7802, 1.4639, -0.0189 },
			{ -0.8226, 1.4606, -0.0189 },
			{ -0.8521, 1.4585, -0.0189 },
			{ -0.6813, 1.4575, -0.0114 },
			{ -0.7716, 1.4629, -0.0397 },
			{ -0.8056, 1.4611, -0.0396 },
			{ -0.8270, 1.4594, -0.0397 },
		};



		//��ʼ���������Tpose�Ǽܸ��ڵ���������Unity��������ϵ�µģ������� WS_Unity.
		//Unity��������ϵ(WS_Unity)����������ϵ��y��ָ�졣
		//UE4��������ϵ(WS_UE4)����������ϵ��z��ָ�졣
		//��������ϵ(WS_Geo)����������ϵ��z��ָ�졣
		//�ڲ�ͬ��������ϵ�£���ʼTposeʱ�泯��z������Unity��, y������UE4���� y������Geo��.
		initial(WS_Unity, InitialPosition_Body, InitialPosition_rHand, InitialPosition_lHand, 0); //�÷���ʹ��Ĭ�������㷨
		//initialMocap("vdposi_sdk.dll", WS_Unity, InitialPosition_Body, InitialPosition_rHand, InitialPosition_lHand, 0); //�÷���ʹ���ⲿ�����㷨


		// ���ûص�����
		if (setVDMocapDataCallBackFunc) {
			setVDMocapDataCallBackFunc(GetMocapData);
		}

		// ���ûص�����
		if (setDeviceBreakCallBackFunc) {
			setDeviceBreakCallBackFunc(DeviceBreak);
		}

		//�����豸
		bool isOk = connectDevice();
		if (isOk) { cout << "====== ���ӳɹ� ======" << endl; }
		else { cout << "====== ����ʧ�� ======" << endl; }

		//��ȡ���ӵ��豸����
		_DeviceType_ deviceType = getDeviceType();
		if (deviceType == DT_VDSuitMini) { cout << "���ӵ��� VDSuitMini �豸��" << endl; }
		else if (deviceType == DT_VDSuitFull) { cout << "���ӵ��� VDSuitFull �豸��" << endl; }

		//��ȡ�豸����
		printf("devicePower = %2f.\n", getDevicePower());

		//��ȡ�豸���ݴ���Ƶ��
		printf("deviceFrequency(before setted) = %d\n", getFrequency());

		//�����豸���ݴ���Ƶ��
		setFrequency(HZ_72);
		printf("deviceFrequency(after setted) = %d\n", getFrequency());
		setFrequency(HZ_60);


		/*****************************��У׼	start**************************************************/
		_MagCorrectResult_ magCorrectresult;
		if (MagCorrectState)
		{
			//��ʼУ׼
			cout << "3��ʼ��У׼" << endl;
			Sleep(3000);

			startMagCorrect();
			cout << "��ʼ��У׼" << endl;

			while (MagCorrectTime--)    //�ռ����ݣ�     Զ��ų����ں�����У׼�����������У׼
			{
				cout << "��У׼��ʣ " << MagCorrectTime << "s" << endl;
				Sleep(1000);
			}

			endMagCorrect();

			getMagCorrectResult(&magCorrectresult);
			while (!magCorrectresult.isFinished)
			{
				getMagCorrectResult(&magCorrectresult);
				cout << "progress	is	" << magCorrectresult.progress << endl;
			}

			//���ݷŻؽ���жϴ������Ƿ�У׼�ɹ�

			if (magCorrectresult.isHaveSucceed)    //  ������һ���ɹ�
			{
				cout << "��У׼���" << endl;
				if (magCorrectresult.bLength > 0)
				{

					for (int i = 0; i < magCorrectresult.bLength; i++)
					{
						cout << "����У׼ʧ�ܵĴ�������" << magCorrectresult.bFailedNodes[i] << endl;

					}
				}

				if (magCorrectresult.rLength > 0)
				{
					for (int i = 0; i < magCorrectresult.rLength; i++)
					{
						cout << "����У׼ʧ�ܵĴ�������" << magCorrectresult.rFailedNodes[i] << endl;

					}
				}

				if (magCorrectresult.lLength > 0)
				{
					for (int i = 0; i < magCorrectresult.lLength; i++)
					{
						cout << "����У׼ʧ�ܵĴ�������" << magCorrectresult.lFailedNodes[i] << endl;

					}
				}
			}
			else
			{
				cout << "��У׼ʧ�ܣ���Զ��ų�����" << endl;      //û��һ����У׼�ɹ�
			}
		}
		MagCorrectState = false;     //�ָ���ӡ����
		/*****************************��У׼	end**************************************************/



		//�����豸����Ҫ���б궨��Apose�Ǳ���Ҫ�궨�ģ�Ppose�ǿ�ѡ�ģ����ȱ궨Apose����
		float quat_EndCalibration[4] = { 0,0,0,0 };
		startCalibration(cm, quat_EndCalibration);
		thread thr1(DebugSome);



		thr1.join();      //���ó��������һֱ���ջص������Żص�����

		
		
		//�Ͽ�����
		//disConnect();

		//�ͷ� dll
		//FreeLibrary(hDll);
	}
	cleanupSocket(sockfd);
	cout << "���е���" << endl;
	//
	getchar();
	return   0;
}