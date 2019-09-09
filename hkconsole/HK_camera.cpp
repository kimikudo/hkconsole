#include "pch.h"
//#include "stdafx.h" //VS2017����Ҫ��Ӵ�Ԥ����ͷ�ļ�
#include"HK_camera.h"
#include <iostream>
#include<HCNetSDK.h>
#include<plaympeg4.h>
#include<PlayM4.h>    //��ͷ�ļ���Ҫ��������ڶ�������Bug�еķ���ȥ���
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include<string>


//ȫ�ֱ���
LONG g_nPort;
Mat g_BGRImage;
LONG lUserID;
int num = 1;

//���ݽ���ص�������
//���ܣ���YV_12��ʽ����Ƶ������ת��Ϊ�ɹ�opencv�����BGR���͵�ͼƬ���ݣ���ʵʱ��ʾ��
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

//ʵʱ��Ƶ�������ݻ�ȡ �ص�����
void CALLBACK fRealDataCallBack_V30(LONG lPlayHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, void* pUser)
{
	
	if (dwDataType == NET_DVR_STREAMDATA)//��������
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
				
				//jpeg ����
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
//���캯��
HK_camera::HK_camera(void)
{

}
//��������
HK_camera::~HK_camera(void)
{
}
//��ʼ��������������ʼ��״̬���
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


//��¼��������������ͷid�Լ����������¼
//bool HK_camera::Login(char* sDeviceAddress, char* sUserName, char* sPassword, WORD wPort)
bool HK_camera::Login(const char* sDeviceAddress,const char* sUserName,const char* sPassword, WORD wPort)        //��½��VS2017�汾��
{


	cout << "dddddddddddddddddddddddd" << endl;

	cout << NET_DVR_GetLastError() << endl;

	cout<<"hello"<<endl;

	NET_DVR_USER_LOGIN_INFO pLoginInfo = { 0 };
	//NET_DVR_DEVICEINFO_V40 lpDeviceInfo = { 0 };
	NET_DVR_DEVICEINFO_V30 lpDeviceInfo2 = { 0 };

	pLoginInfo.bUseAsynLogin = 0;     //ͬ����¼��ʽ
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

//��Ƶ����ʾ����
void HK_camera::show()
{
	cout << NET_DVR_GetLastError() << endl;

	if (PlayM4_GetPort(&g_nPort))            //��ȡ���ſ�ͨ����
	{
		if (PlayM4_SetStreamOpenMode(g_nPort, STREAME_REALTIME))      //������ģʽ
		{
			if (PlayM4_OpenStream(g_nPort, NULL, 0, 1024 * 768))         //����
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
	//����Ԥ�������ûص�������
	
	NET_DVR_PREVIEWINFO struPlayInfo = { 0 };
	struPlayInfo.hPlayWnd = NULL; //����Ϊ�գ��豸SDK������ֻȡ��
	struPlayInfo.lChannel = 33; //Channel number �豸ͨ��
	//������ȥ.������ͨ���ŸĶ���,ͨ����ͨ��SDK�Դ���ClientDemo.exe����¼����õ�!
	struPlayInfo.dwStreamType = 0;// �������ͣ�0-��������1-��������2-����3��3-����4, 4-����5,5-����6,7-����7,8-����8,9-����9,10-����10
	struPlayInfo.dwLinkMode = 0;// 0��TCP��ʽ,1��UDP��ʽ,2���ಥ��ʽ,3 - RTP��ʽ��4-RTP/RTSP,5-RSTP/HTTP 
	struPlayInfo.bBlocked = 0; //0-������ȡ��, 1-����ȡ��, �������SDK�ڲ�connectʧ�ܽ�����5s�ĳ�ʱ���ܹ�����,���ʺ�����ѯȡ������.
	struPlayInfo.byPreviewMode = 0;
	struPlayInfo.dwDisplayBufNum = 30;

	std::cout << lUserID << endl;

	int playResult = NET_DVR_RealPlay_V40(lUserID, &struPlayInfo, fRealDataCallBack_V30, NULL);

	cout << NET_DVR_GetLastError() << endl;

	if (playResult==0)//������ж�Ҳ����,NET_DVR_RealPlay_V40����ʧ�ܷ��ص���-1,�ɹ�����0,֮ǰд����if(playResult),��������ɹ���Զ!��!��!��!!!!!!!!���!
	{
		cout<<NET_DVR_GetLastError()<<endl;
		namedWindow("RGBImage2");
		
		//NET_DVR_CaptureJPEGPicture(lUserID,33,)
		
	}
}