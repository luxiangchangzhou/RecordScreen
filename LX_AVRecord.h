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
	//Ҫ��׺
	LX_AVRecord();
	~LX_AVRecord();


	bool init(char * outUrl, bool isScreen, int camIndex = 0);
	//������̨�߳�
	void startRecord();
	void stop();
	void clear();
	void run();

	int camIndex;
	bool isScreen;
	//����ļ���
	char *outUrl;
	//����ʱ��
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
	//���ظ�ʽת��������
	LX_VideoScale video_scale;
	//������������
	LX_VideoEncoder video_encoder;

	LX_Resample resample;

	LX_AudioEncoder audio_encoder;
	LX_Make_MP4 make_mp4;

	QAudioInput *input;
	QIODevice *io;
};

