#include "pch.h"
//#include "stdafx.h" //VS2017����Ҫ��Ӵ�Ԥ����ͷ�ļ�
#include"HK_camera.h"
#include <iostream>
#include <Windows.h>

using namespace std;

int main()
{
	HK_camera camera;
	if (camera.Init())
	{
		cout << "init success" << endl;

		bool loginResult = camera.Login("192.168.0.53", "admin", "admin123", 8000);

		cout << loginResult << endl;

		if (loginResult)//�û����Լ����룬���ݴ�ϵ�в���һ�еķ����鿴������
		{
			cout << "login successfully" << endl;
			camera.show();
		}
		else
		{
			cout << "login fail" << endl;
		}
	}
	else
	{
		cout << "init fail" << endl;
	}

	while (1)
	{

	}
	return 0;
}