// useQTDLL_vscpp.cpp : 定义控制台应用程序的入口点。
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

int sockfd;  // 全局变量
struct sockaddr_in broadcastAddr;  // 全局变量


// 定义回调函数指针。
// 获取动捕数据，都是在世界坐标系下的。
typedef void(*mocapDataCallBack)(_MocapData_ md);
typedef void(*SetVDMocapDataCallBackFunc)(mocapDataCallBack);
SetVDMocapDataCallBackFunc setVDMocapDataCallBackFunc;

// 获取设备非正常断开信号
typedef void(*deviceBreakCallBack)();
typedef void(*SetDeviceBreakCallBackFunc)(deviceBreakCallBack BreakCallBack);
SetDeviceBreakCallBackFunc setDeviceBreakCallBackFunc;

// 初始化，需要输入模型骨架Tpose各节点坐标（世界坐标系）
typedef void(*Initial)(_WorldSpace_ worldSpace, float InitialPosition_Body[NODES_BODY][3],
	float InitialPosition_rHand[NODES_HAND][3], float InitialPosition_lHand[NODES_HAND][3], int dllIndex);
Initial initial;

//typedef void(*InitialMocap)(char* path, _WorldSpace_ worldSpace, float InitialPosition_Body[NODES_BODY][3],
//	float InitialPosition_rHand[NODES_HAND][3], float InitialPosition_lHand[NODES_HAND][3], int dllIndex);
//InitialMocap initialMocap;

//连接设备
typedef bool(*Connect)();
Connect connectDevice;

//断开连接
typedef void(*DisConnect)();
DisConnect disConnect;

//连接成功后，除了通过回调函数获取动捕数据外，也可通过该函数获取动捕数据。
//获取的数据都是在世界坐标系下的。
typedef void(*RecvMocapData)(_MocapData_* md);
RecvMocapData recvMocapData;

//获取连接状态
typedef bool(*GetConnectState)();
GetConnectState getConnectState;

//获取连接设备的类型（VDSuitMini、VDSuitFull）
typedef _DeviceType_(*GetDeviceType)();
GetDeviceType getDeviceType;

//获取设备数据传输频率
typedef int(*GetFrequency)();
GetFrequency getFrequency;

//获取设备电量
typedef float(*GetDevicePower)();
GetDevicePower getDevicePower;

//设置设备数据传输频率
typedef bool(*SetFrequency)(_Frequency_ frequency);
SetFrequency setFrequency;

//在模型的初始目标下设置每个节点的坐标
typedef void(*SetModelNodesPositionInInitialTpose)( float InitialPosition_Body[NODES_BODY][3],
	float InitialPosition_rHand[NODES_HAND][3], float InitialPosition_lHand[NODES_HAND][3]);
SetModelNodesPositionInInitialTpose setModelNodesPositionInInitialTpose;

/*-------------------------------------标定函数		start--------------------------------------------------*/

//标定，"Apose"标定是必须进行的，"Ppose"标定是可选的。
//该标定会识别穿戴者所做pose与标定pose是否一致，及标定中的抖动程度，若抖动太厉害，则无法标定完成。
typedef void(*StartCalibration)(_CalibrationMode_ calibrationMode, float quat_EndCalibration_root[4]);
StartCalibration startCalibration;

//快速标定，"Apose"标定是必须进行的，"Ppose"标定是可选的。
//标定速度比 StartCalibration() 快很多，但需要穿戴者先做好标定pose，然后再执行该函数进行标定。
typedef bool(*StartCalibrationFast)(_CalibrationMode_ calibrationMode, float quat_EndCalibration_root[4]);
StartCalibrationFast startCalibrationFast;

//取消标定
typedef void(*CancelCalibration)();
CancelCalibration cancelCalibration;

//获取标定进度
typedef _CalibrationProgress_(*GetCalibrationProgress)();
GetCalibrationProgress getCalibrationProgress;

//设置标定过程中允许的抖动程度，范围[0, 1]，默认为0（最严格），可不设置。
typedef bool(*SetShakeDegreeForCalibration)(float degree);
SetShakeDegreeForCalibration setShakeDegreeForCalibration;

/*-------------------------------------标定函数		end--------------------------------------------------*/

/*-------------------------------------磁力计函数		start--------------------------------------------------*/


//开始磁力计校准
typedef bool(*StartMagCorrect)();
StartMagCorrect startMagCorrect;

//取消磁力计校准
typedef void(*CancelMagCorrect)();
CancelMagCorrect cancelMagCorrect;

//结束磁力计校准
typedef void(*EndMagCorrect)();
EndMagCorrect endMagCorrect;

typedef bool(*GetMagCorrectResult)(_MagCorrectResult_* magCorrectResult);
GetMagCorrectResult getMagCorrectResult;

/*-------------------------------------磁力计函数		end--------------------------------------------------*/


/*-------------------------------------其他函数		end--------------------------------------------------*/
//蜂鸣器
typedef void(*MakeBuzz)();
MakeBuzz makeBuzz;

//手势
typedef void(*GetGesture)(_Gesture_* gesture_R, _Gesture_* gesture_L);
GetGesture getGesture;

/*-------------------------------------其他函数		end--------------------------------------------------*/



/*-------------------------------------姿态调节函数		start--------------------------------------------------*/
//正常启动获取数据后即可调节，不作展示
//设置模型朝向 [-1,1],It's minus 180 to 180 degrees.
typedef void(*SetPositionDirection)(float AdjustYaw);
SetPositionDirection setPositionDirection;

//模型位移复位
typedef void(*PositionBackToOrigin)();
PositionBackToOrigin positionBackToOrigin;

//设置速度容忍度，用于调节跳跃的水平漂移
//[0 or 1], default value 00: default mode, without restriction.   1 : in situ mode, Limit it to a small area
typedef void(*SetVelocityMode)(int m_VelocityMode);
SetVelocityMode setVelocityMode;

//设置高差容忍度，应用于调节行走的顿挫,[0,10], default value 2
typedef void(*SetWalkingFallValue)(float m_WalkingFallValue);
SetWalkingFallValue setWalkingFallValue;

//设置手臂张合度，应用于调节合掌等动作效果, [-0.5, 0.5], default value 0
typedef void(*SetArmHorizontal)(float m_ArmHorizontal);
SetArmHorizontal setArmHorizontal;

//设置手臂初始角，应用到实时穿模场景,[-1.5,1.5], default value 0
typedef void(*SetArmVerticality)(float m_ArmVerticality);
SetArmVerticality setArmVerticality;

//设置性能等级， 性能等级设置[0 ：性能最低, 延时最低; 1 ：性能稳定, 延时适中;,default value1
typedef void(*SetPerformanceLevel)(int m_PerformanceLeve);
SetPerformanceLevel setPerformanceLevel;

/*-------------------------------------姿态调节函数		end--------------------------------------------------*/





//
_CalibrationMode_ cm = CM_Apose;
_CalibrationProgress_ cp;


bool MagCorrectState = false;    //是否启动磁校准状态       磁校准完成后需要重启设备才能生效（也可以插拔sign out数据重启）；
int MagCorrectTime = 30;		//建议		盒子校准：30s			体操校准：75s

// 定义回调函数方法 
void GetMocapData(_MocapData_ md)
{
	//数据打印耗时较长，可能导致丢帧现象，这是正常的。
	if (!MagCorrectState)       //磁校准时不打印数据，防止看不到校准结果
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
	cout << "====== 设备非正常断开了 ======" << endl;
}

//
void DebugSome()
{		
	//cp初始化
	cp.state = CS_UnStart;
	cp.progress = 0;



	//
	while (true)
	{
		Sleep(20);

		cp = getCalibrationProgress();
		if (cp.state != CS_UnStart) {
			printf("标定进度: state: %d   progress: %.2f\n", cp.state, cp.progress);
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
    //载入dll 
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



		//初始化，这里的Tpose骨架各节点坐标是在Unity世界坐标系下的，故输入 WS_Unity.
		//Unity世界坐标系(WS_Unity)：左手坐标系，y轴指天。
		//UE4世界坐标系(WS_UE4)：左手坐标系，z轴指天。
		//地理坐标系(WS_Geo)：右手坐标系，z轴指天。
		//在不同世界坐标系下，初始Tpose时面朝向：z轴正向（Unity）, y轴正向（UE4）， y轴正向（Geo）.
		initial(WS_Unity, InitialPosition_Body, InitialPosition_rHand, InitialPosition_lHand, 0); //该方法使用默认驱动算法
		//initialMocap("vdposi_sdk.dll", WS_Unity, InitialPosition_Body, InitialPosition_rHand, InitialPosition_lHand, 0); //该方法使用外部驱动算法


		// 设置回调函数
		if (setVDMocapDataCallBackFunc) {
			setVDMocapDataCallBackFunc(GetMocapData);
		}

		// 设置回调函数
		if (setDeviceBreakCallBackFunc) {
			setDeviceBreakCallBackFunc(DeviceBreak);
		}

		//连接设备
		bool isOk = connectDevice();
		if (isOk) { cout << "====== 连接成功 ======" << endl; }
		else { cout << "====== 连接失败 ======" << endl; }

		//获取连接的设备类型
		_DeviceType_ deviceType = getDeviceType();
		if (deviceType == DT_VDSuitMini) { cout << "连接的是 VDSuitMini 设备。" << endl; }
		else if (deviceType == DT_VDSuitFull) { cout << "连接的是 VDSuitFull 设备。" << endl; }

		//获取设备电量
		printf("devicePower = %2f.\n", getDevicePower());

		//获取设备数据传输频率
		printf("deviceFrequency(before setted) = %d\n", getFrequency());

		//设置设备数据传输频率
		setFrequency(HZ_72);
		printf("deviceFrequency(after setted) = %d\n", getFrequency());
		setFrequency(HZ_60);


		/*****************************磁校准	start**************************************************/
		_MagCorrectResult_ magCorrectresult;
		if (MagCorrectState)
		{
			//开始校准
			cout << "3后开始磁校准" << endl;
			Sleep(3000);

			startMagCorrect();
			cout << "开始磁校准" << endl;

			while (MagCorrectTime--)    //收集数据，     远离磁场，在盒子里校准或者是做体操校准
			{
				cout << "磁校准还剩 " << MagCorrectTime << "s" << endl;
				Sleep(1000);
			}

			endMagCorrect();

			getMagCorrectResult(&magCorrectresult);
			while (!magCorrectresult.isFinished)
			{
				getMagCorrectResult(&magCorrectresult);
				cout << "progress	is	" << magCorrectresult.progress << endl;
			}

			//根据放回结果判断传感器是否校准成功

			if (magCorrectresult.isHaveSucceed)    //  至少有一个成功
			{
				cout << "磁校准完成" << endl;
				if (magCorrectresult.bLength > 0)
				{

					for (int i = 0; i < magCorrectresult.bLength; i++)
					{
						cout << "身体校准失败的传感器有" << magCorrectresult.bFailedNodes[i] << endl;

					}
				}

				if (magCorrectresult.rLength > 0)
				{
					for (int i = 0; i < magCorrectresult.rLength; i++)
					{
						cout << "右手校准失败的传感器有" << magCorrectresult.rFailedNodes[i] << endl;

					}
				}

				if (magCorrectresult.lLength > 0)
				{
					for (int i = 0; i < magCorrectresult.lLength; i++)
					{
						cout << "左手校准失败的传感器有" << magCorrectresult.lFailedNodes[i] << endl;

					}
				}
			}
			else
			{
				cout << "磁校准失败，请远离磁场环境" << endl;      //没有一个磁校准成功
			}
		}
		MagCorrectState = false;     //恢复打印数据
		/*****************************磁校准	end**************************************************/



		//连接设备后，需要进行标定，Apose是必须要标定的，Ppose是可选的（需先标定Apose）。
		float quat_EndCalibration[4] = { 0,0,0,0 };
		startCalibration(cm, quat_EndCalibration);
		thread thr1(DebugSome);



		thr1.join();      //不让程序结束，一直接收回调函数放回的数据

		
		
		//断开连接
		//disConnect();

		//释放 dll
		//FreeLibrary(hDll);
	}
	cleanupSocket(sockfd);
	cout << "运行到此" << endl;
	//
	getchar();
	return   0;
}