#pragma once
#include <QThread>
#include <QAudioInput>
#include <QScreen>
#include <QPainter>
#include <QImage>
#include "LX_AudioEncoder.h"
#include "LX_Resample.h"
#include "LX_Rtmp.h"
#include "LX_VideoScale.h"
#include "LX_VideoEncoder.h"
#include "LX_Camera.h"
#include "LX_Make_MP4.h"
#include "LX_ScreenCamera.h"

class LX_AVRecord:protected QThread
{
public:
	//要后缀
	LX_AVRecord();
	~LX_AVRecord();


	bool init(char * outUrl, bool isScreen, int camIndex = 0);
	//开启后台线程
	void startRecord();
	void stop();
	void clear();
	void run();

	int camIndex;
	bool isScreen;
	//输出文件名
	char *outUrl;
	//配置时钟
	long long frequency = 0;
	int inWidth;
	int inHeight;
	int fps;
	long long start_time;

	int sampleRate = 44100;
	int channels = 2;
	int sampleByte = 2;

	bool isExit = true;

	LX_ScreenCamera cam_scr;

	//VideoCapture cam;
	LX_Camera cam;
	//像素格式转换上下文
	LX_VideoScale video_scale;
	//编码器上下文
	LX_VideoEncoder video_encoder;

	LX_Resample resample;

	LX_AudioEncoder audio_encoder;
	LX_Make_MP4 make_mp4;

	QAudioInput *input;
	QIODevice *io;
};

