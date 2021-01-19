#include "LX_Camera.h"
#include <windows.h>
#pragma comment(lib, "Winmm.lib")
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
using namespace cv;
#pragma comment(lib,"opencv_world412.lib")
#include <iostream>
using namespace std;

LX_Camera::LX_Camera()
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
	assert(frequency != 0);
}


LX_Camera::~LX_Camera()
{
}

bool LX_Camera::open(int camIndex)
{
	cam.open(camIndex);
	if (!cam.isOpened())
	{
		cout << "cam open failed!" << endl;
		return false;
	}
	cout << camIndex << " cam open success" << endl;
	width = cam.get(CAP_PROP_FRAME_WIDTH);
	height = cam.get(CAP_PROP_FRAME_HEIGHT);
	fps = cam.get(CAP_PROP_FPS);
	if (fps == 0) fps = 25;
	return true;
}
bool LX_Camera::open(const char *url)
{
	cam.open(url);
	if (!cam.isOpened())
	{
		cout << "cam open failed!" << endl;
		return false;
	}
	cout << url << " cam open success" << endl;
	width = cam.get(CAP_PROP_FRAME_WIDTH);
	height = cam.get(CAP_PROP_FRAME_HEIGHT);
	fps = cam.get(CAP_PROP_FPS);
	if (fps == 0) fps = 25;
	return true;

}
void LX_Camera::close()
{
	Stop();
	if (cam.isOpened())
	{
		cam.release();
	}
}


//在列表结尾插入
void LX_Camera::Push(Image_Data d)
{
	mutex.lock();
	if (datas.size() > maxList)
	{
		datas.front().Drop();
		datas.pop_front();
	}
	datas.push_back(d);
	mutex.unlock();
}

//读取列表中最早的数据,返回数据需要调用XData.Drop清理
Image_Data LX_Camera::Pop()
{
	mutex.lock();
	if (datas.empty())
	{
		mutex.unlock();
		return Image_Data();
	}
	Image_Data d = datas.front();
	datas.pop_front();
	mutex.unlock();
	return d;
}

//启动线程
bool LX_Camera::Start(long long start_time)
{
	this->start_time = start_time;
	isExit = false;
	QThread::start();
	return true;
}

//退出线程，并等待线程退出（阻塞）
void LX_Camera::Stop()
{
	isExit = true;
	wait();
}

void LX_Camera::Clear()
{
	mutex.lock();
	while (!datas.empty())
	{
		datas.front().Drop();
		datas.pop_front();
	}
	mutex.unlock();
}

void LX_Camera::run()
{/*
 namedWindow("v");*/
	Mat frame1;
	Mat frame;
	while (!isExit)
	{
		if (!cam.read(frame))
		{
			msleep(1);
			continue;
		}
		if (frame.empty())
		{
			msleep(1);
			continue;
		}
		int d = 15;
		//bilateralFilter(frame1, frame, d, d * 2, d / 2);
		 //确保数据是连续的 
		Image_Data data((char*)frame.data, frame.cols*frame.rows*frame.elemSize(), GetCurTime());
		Push(data);
	}
}
long long LX_Camera::GetCurTime()
{
	LARGE_INTEGER  large_interger;
	QueryPerformanceCounter(&large_interger);
	return large_interger.QuadPart - start_time;
}





