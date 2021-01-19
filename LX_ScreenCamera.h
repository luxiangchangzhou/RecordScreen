#pragma once

#include <QThread>
#include <list>
#include <QMutex>
#include <QScreen>
#include "Image_Data.h"


class LX_ScreenCamera:protected QThread
{
public:
	LX_ScreenCamera();
	~LX_ScreenCamera();

	void close();
	int width = 0;
	int height = 0;

	//���б��β����
	void Push(Image_Data d);

	//��ȡ�б������������,����������Ҫ����XData.Drop����
	Image_Data Pop();

	//�����߳�
	bool Start(long long start_time);

	//�˳��̣߳����ȴ��߳��˳���������
	void Stop();

	void Clear();

	void run();

	long long GetCurTime();

	//�������б��С���б����ֵ������ɾ����ɵ�����
	int maxList = 5;
	//��Ž������� ������� �Ƚ��ȳ�
	std::list<Image_Data> datas;
	//���������б��С
	int dataCount = 0;
	//�����߳��˳�
	bool isExit = false;
	//������� datas;
	QMutex mutex;
	long long frequency = 0;
	long long start_time = 0;
};

