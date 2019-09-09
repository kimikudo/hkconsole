#include "pch.h"
//#include "stdafx.h" //VS2017中需要添加此预编译头文件
#include"HK_camera.h"
#include <iostream>
#include<HCNetSDK.h>
#include<plaympeg4.h>
#include<PlayM4.h>    //此头文件需要按照下面第二步调试Bug中的方法去添加
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include<string>


//全局变量
LONG g_nPort;
Mat g_BGRImage;
LONG lUserID;
int num = 1;

//数据解码回调函数，
//功能：将YV_12格式的视频数据流转码为可供opencv处理的BGR类型的图片数据，并实时显示。
void CALLBACK DecCBFun(long nPort, char* pBuf, long nSize, FRAME_INFO* pFrameInfo, long nUser, long nReserved2)
{
	if (pFrameInfo->nType == T_YV12)
	{
		std::cout << "the frame infomation is T_YV12" << std::endl;
		if (g_BGRImage.empty())
		{
			g_BGRImage.create(pFrameInfo->nHeight, pFrameInfo->nWidth, CV_8UC3);
		}
		Mat YUVImage(pFrameInfo->nHeight + pFrameInfo->nHeight / 2, pFrameInfo->nWidth, CV_8UC1, (unsigned char*)pBuf);

		cvtColor(YUVImage, g_BGRImage, COLOR_YUV2BGR_YV12);
		imshow("RGBImage1", g_BGRImage);
		waitKey(15);

		YUVImage.~Mat();
	}
}

//实时视频码流数据获取 回调函数
void CALLBACK fRealDataCallBack_V30(LONG lPlayHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, void* pUser)
{
	
	if (dwDataType == NET_DVR_STREAMDATA)//码流数据
	{
		if (dwBufSize > 0 && g_nPort != -1)
		{
			if (!PlayM4_InputData(g_nPort, pBuffer, dwBufSize))
			{
				std::cout << "fail input data" << std::endl;
			}
			else
			{
				std::cout << "success input data" << std::endl;
				//bool setResult=NET_DVR_SetCapturePictureMode(1);
				//cout << "setResult:"<<setResult << endl;
				//cout  << NET_DVR_GetLastError() << endl;
				char PicName[256] = { 0 };
				//strcpy_s(PicName, num+".jpg");
				sprintf_s(PicName, "%d.jpg", num++);
				string fileName = num + ".jpg";

				//num++;
				//bool result=NET_DVR_CapturePicture(lUserID, PicName);
				//cout << "esult:" << result << endl;
				//cout << NET_DVR_GetLastError() << endl;
				
				//jpeg 参数
				NET_DVR_JPEGPARA jpegPara = { 0 };
				jpegPara.wPicQuality = 0;
				jpegPara.wPicSize = 0xff;
				//bool catchResult = NET_DVR_CaptureJPEGPicture(lUserID, 33, &jpegPara , PicName);
				bool catchResult = NET_DVR_CaptureJPEGPicture(lUserID, 33, &jpegPara, PicName);
				cout << "catchResult:" << catchResult << endl;
				cout << NET_DVR_GetLastError() << endl;
			}

		}
	}
}
//构造函数
HK_camera::HK_camera(void)
{

}
//析构函数
HK_camera::~HK_camera(void)
{
}
//初始化函数，用作初始化状态检测
bool HK_camera::Init()
{
	if (NET_DVR_Init())
	{
		NET_DVR_SetLogToFile(3, (char*)"C:\\SdkLog\\",true);

		NET_DVR_SetConnectTime(200000, 1);

		NET_DVR_SetReconnect(20000, true);
		return true;
	}
	else
	{
		return false;
	}
}


//登录函数，用作摄像头id以及密码输入登录
//bool HK_camera::Login(char* sDeviceAddress, char* sUserName, char* sPassword, WORD wPort)
bool HK_camera::Login(const char* sDeviceAddress,const char* sUserName,const char* sPassword, WORD wPort)        //登陆（VS2017版本）
{


	cout << "dddddddddddddddddddddddd" << endl;

	cout << NET_DVR_GetLastError() << endl;

	cout<<"hello"<<endl;

	NET_DVR_USER_LOGIN_INFO pLoginInfo = { 0 };
	//NET_DVR_DEVICEINFO_V40 lpDeviceInfo = { 0 };
	NET_DVR_DEVICEINFO_V30 lpDeviceInfo2 = { 0 };

	pLoginInfo.bUseAsynLogin = 0;     //同步登录方式
	strcpy_s(pLoginInfo.sDeviceAddress, sDeviceAddress);
	strcpy_s(pLoginInfo.sUserName, sUserName);
	strcpy_s(pLoginInfo.sPassword, sPassword);
	pLoginInfo.wPort = wPort;

	//lUserID = NET_DVR_Login_V40(&pLoginInfo, &lpDeviceInfo);
	lUserID = NET_DVR_Login_V30((char*)sDeviceAddress, wPort, (char*)sUserName, (char*)sPassword, &lpDeviceInfo2);

	if (lUserID < 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

//视频流显示函数
void HK_camera::show()
{
	cout << NET_DVR_GetLastError() << endl;

	if (PlayM4_GetPort(&g_nPort))            //获取播放库通道号
	{
		if (PlayM4_SetStreamOpenMode(g_nPort, STREAME_REALTIME))      //设置流模式
		{
			if (PlayM4_OpenStream(g_nPort, NULL, 0, 1024 * 768))         //打开流
			{
				if (PlayM4_SetDecCallBackExMend(g_nPort, DecCBFun, NULL, 0, NULL))
				{
					if (PlayM4_Play(g_nPort, NULL))
					{
						std::cout << "success to set play mode" << std::endl;
					}
					else
					{
						std::cout << "fail to set play mode" << std::endl;
					}
				}
				else
				{
					std::cout << "fail to set dec callback " << std::endl;
				}
			}
			else
			{
				std::cout << "fail to open stream" << std::endl;
			}
		}
		else
		{
			std::cout << "fail to set stream open mode" << std::endl;
		}
	}
	else
	{
		std::cout << "fail to get port" << std::endl;
	}

	cout << NET_DVR_GetLastError() << endl;
	//启动预览并设置回调数据流
	
	NET_DVR_PREVIEWINFO struPlayInfo = { 0 };
	struPlayInfo.hPlayWnd = NULL; //窗口为空，设备SDK不解码只取流
	struPlayInfo.lChannel = 33; //Channel number 设备通道
	//改来改去.改这里通道号改对了,通道号通过SDK自带的ClientDemo.exe连接录像机得到!
	struPlayInfo.dwStreamType = 0;// 码流类型，0-主码流，1-子码流，2-码流3，3-码流4, 4-码流5,5-码流6,7-码流7,8-码流8,9-码流9,10-码流10
	struPlayInfo.dwLinkMode = 0;// 0：TCP方式,1：UDP方式,2：多播方式,3 - RTP方式，4-RTP/RTSP,5-RSTP/HTTP 
	struPlayInfo.bBlocked = 0; //0-非阻塞取流, 1-阻塞取流, 如果阻塞SDK内部connect失败将会有5s的超时才能够返回,不适合于轮询取流操作.
	struPlayInfo.byPreviewMode = 0;
	struPlayInfo.dwDisplayBufNum = 30;

	std::cout << lUserID << endl;

	int playResult = NET_DVR_RealPlay_V40(lUserID, &struPlayInfo, fRealDataCallBack_V30, NULL);

	cout << NET_DVR_GetLastError() << endl;

	if (playResult==0)//这里的判断也不对,NET_DVR_RealPlay_V40函数失败返回的是-1,成功返回0,之前写的是if(playResult),所以如果成功永远!进!不!来!!!!!!!!深坑!
	{
		cout<<NET_DVR_GetLastError()<<endl;
		namedWindow("RGBImage2");
		
		//NET_DVR_CaptureJPEGPicture(lUserID,33,)
		
	}
}