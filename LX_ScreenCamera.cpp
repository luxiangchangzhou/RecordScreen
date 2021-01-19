#include "LX_ScreenCamera.h"
#include <QGuiApplication>
#include <QImage>
#include <QPixmap>
#include <QApplication>
#include <QDesktopWidget>
#include <windows.h>
#include <QTime>
#include <d3d9.h>
#include <iostream>
using namespace std;
#pragma comment (lib,"d3d9.lib")



//截取全屏
void CaptureScreen(char *data)
{
	//1 创建directx3d对象
	static IDirect3D9 *d3d = NULL;
	if (!d3d)
	{
		d3d = Direct3DCreate9(D3D_SDK_VERSION);
	}
	if (!d3d) return;

	//2 创建显卡的设备对象
	static IDirect3DDevice9 *device = NULL;
	if (!device)
	{
		D3DPRESENT_PARAMETERS pa;
		ZeroMemory(&pa, sizeof(pa));
		pa.Windowed = true;
		pa.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
		pa.SwapEffect = D3DSWAPEFFECT_DISCARD;
		pa.hDeviceWindow = GetDesktopWindow();
		d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 0,
			D3DCREATE_HARDWARE_VERTEXPROCESSING, &pa, &device
		);
	}
	if (!device)return;



	//3创建离屏表面
	int w = GetSystemMetrics(SM_CXSCREEN);
	int h = GetSystemMetrics(SM_CYSCREEN);
	static IDirect3DSurface9 *sur = NULL;
	if (!sur)
	{
		device->CreateOffscreenPlainSurface(w, h,
			D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &sur, 0);
	}
	if (!sur)return;
	if (!data)return;

	//4 抓屏
	device->GetFrontBufferData(0, sur);

	//5 取出数据
	D3DLOCKED_RECT rect;
	ZeroMemory(&rect, sizeof(rect));
	if (sur->LockRect(&rect, 0, 0) != S_OK)
	{
		return;
	}

	//unsigned char * dataTmp = new unsigned char[w*h * 4];
	//memcpy(dataTmp, rect.pBits, w * h * 4);

	//自己做像素格式转化
	for (int i = 0,j = 0;i < w*h*3;i+=3,j+=4)
	{//BGRA
		data[i] = ((char*)(rect.pBits))[j];
		data[i + 1] = ((char*)(rect.pBits))[j + 1];
		data[i + 2] = ((char*)(rect.pBits))[j + 2];
	}

	////使用QImage做像素格式转化
	//QImage image(dataTmp, 1920, 1080, QImage::Format_ARGB32);
	//QImage image_RGB888 = image.convertToFormat(QImage::Format_RGB888);
	//int k = image.bytesPerLine();
	//memcpy(data, image_RGB888.constBits(), image_RGB888.byteCount());

	sur->UnlockRect();
	//delete dataTmp;

}


//在列表结尾插入
void LX_ScreenCamera::Push(Image_Data d)
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
Image_Data LX_ScreenCamera::Pop()
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

LX_ScreenCamera::LX_ScreenCamera()
{
	//初始化截屏函数
	CaptureScreen(0);


	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
	width = GetSystemMetrics(SM_CXSCREEN);
	height = GetSystemMetrics(SM_CYSCREEN);
	
}


void LX_ScreenCamera::run()
{
	char*buf = new char[width*height * 3];
	for (;isExit!=true;)
	{
		CaptureScreen(buf);
		Image_Data data(buf, width*height * 3, GetCurTime());
		Push(data);
	}
	delete buf;
	
}

//启动线程
bool LX_ScreenCamera::Start(long long start_time)
{
	this->start_time = start_time;
	isExit = false;
	QThread::start();
	return true;
}

//退出线程，并等待线程退出（阻塞）
void LX_ScreenCamera::Stop()
{
	isExit = true;
	wait();
}

void LX_ScreenCamera::close()
{
	Stop();
}
void LX_ScreenCamera::Clear()
{
	mutex.lock();
	while (!datas.empty())
	{
		datas.front().Drop();
		datas.pop_front();
	}
	mutex.unlock();
}


long long LX_ScreenCamera::GetCurTime()
{
	LARGE_INTEGER  large_interger;
	QueryPerformanceCounter(&large_interger);
	return large_interger.QuadPart - start_time;
}

LX_ScreenCamera::~LX_ScreenCamera()
{
}
