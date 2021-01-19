#pragma once

#include <QThread>
#include <list>
#include <QMutex>
#include <opencv2/highgui.hpp>
#include "Image_Data.h"



class LX_Camera :protected QThread
{
public:
	LX_Camera();
	~LX_Camera();
	bool open(int camIndex);
	bool open(const char *url);
	void close();
	cv::VideoCapture cam;

	int width = 0;
	int height = 0;
	int fps = 0;

	//在列表结尾插入
	void Push(Image_Data d);

	//读取列表中最早的数据,返回数据需要调用XData.Drop清理
	Image_Data Pop();

	//启动线程
	bool Start(long long start_time);

	//退出线程，并等待线程退出（阻塞）
	void Stop();

	void Clear();

	void run();

	long long GetCurTime();


	//（缓冲列表大小）列表最大值，超出删除最旧的数据
	int maxList = 5;
	//存放交互数据 插入策略 先进先出
	std::list<Image_Data> datas;
	//交互数据列表大小
	int dataCount = 0;
	//处理线程退出
	bool isExit = false;
	//互斥访问 datas;
	QMutex mutex;

	long long frequency = 0;
	long long start_time = 0;

};

