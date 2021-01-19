#include "LX_AVRecord.h"
#include <windows.h>
#pragma comment(lib, "Winmm.lib")
#include <iostream>
#include <assert.h>
using namespace std;

#include <opencv2/highgui.hpp>
using namespace cv;


LX_AVRecord::LX_AVRecord()
{
	
	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
	assert(frequency != 0);

}

bool LX_AVRecord::init(char * outUrl, bool isScreen, int camIndex)
{
	this->isScreen = isScreen;
	this->camIndex = camIndex;
	if (outUrl == 0)
	{
		this->outUrl = "test.mp4";
	}
	else
	{
		this->outUrl = outUrl;
	}




	if (isScreen == true)//选择屏幕录像机
	{
		//..................
		inWidth = cam_scr.width;
		inHeight = cam_scr.height;
		fps = 25;
	}
	else
	{
		if (cam.open(camIndex) == false)
		{
			cout << "cam open failed!" << endl;
			return false;
		}
		cout << " cam open success" << endl;
		inWidth = cam.width;
		inHeight = cam.height;
		fps = cam.fps;
	}
	
	
	

	//初始化格式转换上下文
	if (video_scale.InitScaleFromBGR24ToYUV420P(inWidth, inHeight, inWidth, inHeight) == false)
	{
		cout << "video_scale.InitScaleFromBGR24ToYUV420P failed!" << endl;
		return false;
	}
	//初始化编码上下文
	if (video_encoder.InitVideoCodecH264(inWidth, inHeight, fps) == false)
	{
		cout << "video_encoder.InitVideoCodecH264 failed!" << endl;
		return false;
	}


	bool ret = resample.Init_FromS16ToFLTP(channels, sampleRate);
	if (ret == false)
	{
		cout << "音频重采样 上下文初始化 failed!" << endl;
		return false;
	}
	cout << "音频重采样 上下文初始化成功!" << endl;

	ret = audio_encoder.InitAudioCodecAAC(channels, sampleRate);
	if (ret == false)
	{
		cout << "audio_encoder.InitAudioCodecAAC failed!" << endl;
		return 0;
	}
	cout << "audio_encoder.InitAudioCodecAAC成功!" << endl;

	ret = make_mp4.Init(outUrl);
	if (ret == false)
	{
		cout << "make_mp4.Init(outUrl) failed" << endl;
		return false;
	}
	ret = make_mp4.AddStream(audio_encoder.ac);
	if (ret == false)
	{
		cout << "make_mp4.AddStream(audio_encoder.ac) failed" << endl;
		return false;
	}
	ret = make_mp4.AddStream(video_encoder.vc);
	if (ret == false)
	{
		cout << "make_mp4.AddStream(video_encoder.vc) failed" << endl;
		return false;
	}
	ret = make_mp4.open();
	if (ret == false)
	{
		cout << "make_mp4.open() failed" << endl;
		return false;
	}
	ret = make_mp4.SendHead();
	if (ret == false)
	{
		cout << "make_mp4.SendHead() failed" << endl;
		return false;
	}

	QAudioFormat fmt;
	fmt.setSampleRate(sampleRate);
	fmt.setChannelCount(channels);
	fmt.setSampleSize(sampleByte * 8);
	fmt.setCodec("audio/pcm");
	fmt.setByteOrder(QAudioFormat::LittleEndian);
	fmt.setSampleType(QAudioFormat::UnSignedInt);
	QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
	if (!info.isFormatSupported(fmt))
	{
		cout << "Audio format not support!" << endl;
		fmt = info.nearestFormat(fmt);
	}
	input = new QAudioInput(fmt);
		
	return true;
}

void LX_AVRecord::startRecord()
{
	isExit = false;
	//开始录制视频
	QueryPerformanceCounter((LARGE_INTEGER*)&start_time);
	if (isScreen == true)
	{
		cam_scr.Start(start_time);
	}
	else
	{
		cam.Start(start_time);
	}
	
	
	//开始录制音频
	io = input->start();
	this->start();//开启线程
}

void LX_AVRecord::stop()
{
	if (isScreen == true)
	{
		cam_scr.Stop();
	}
	else
	{
		cam.Stop();
	}
	
	isExit = true;
	wait();
}
void LX_AVRecord::clear()
{
	cam_scr.close();
	cam_scr.Clear();
	cam.close();
	cam.Clear();
	video_scale.Close();
	video_encoder.Close();
	resample.close();
	audio_encoder.Close();
	make_mp4.Close();
	input->stop();
	delete input;
}

void LX_AVRecord::run()
{
	//视频帧
	Mat frame;
	//一次读取一帧音频的字节数
	int readSize = 1024 * channels*sampleByte;
	char *buf = new char[readSize];
	for (;isExit!=true;)
	{
		Image_Data img;
		if (isScreen == true)
		{
			img = cam_scr.Pop();
		}
		else
		{
			img = cam.Pop();
		}
		
		//读视频帧
		if (img.data != 0)
		{
			AVFrame * yuvframe = allocYUV420pFrame(inWidth, inHeight);
			int h = video_scale.ScaleFromBGR24ToYUV420P((unsigned char*)img.data, yuvframe);
			if (h > 0)
			{
				AVPacket *pkt = video_encoder.EncodeVideo(yuvframe, make_mp4.indexOfVideo, img.pts, frequency);
				freeFrame(yuvframe);
				if (pkt != 0)
				{
					make_mp4.SendPacket(pkt);
				}
			}
			else
			{
				freeFrame(yuvframe);
			}
			img.Drop();
		}


		//一次读取一帧音频
		if (input->bytesReady() < readSize)
		{
			QThread::msleep(1);
			continue;
		}
		int size = 0;
		while (size != readSize)
		{
			int len = io->read(buf + size, readSize - size);
			if (len < 0)break;
			size += len;
		}
		if (size != readSize)continue;
		LARGE_INTEGER  large_interger;
		QueryPerformanceCounter(&large_interger);
		AVFrame *pcm = allocFLTP_PCMFrame(channels, 1024);
		resample.Resample_FromS16ToFLTP(buf, pcm);
		AVPacket*pkt = audio_encoder.EncodeAudio(pcm, make_mp4.indexOfAudio, large_interger.QuadPart - start_time, frequency);
		freeFrame(pcm);
		if (pkt == 0)
			continue;
		make_mp4.SendPacket(pkt);
	}
	make_mp4.SendEnd();
	delete buf;
}

LX_AVRecord::~LX_AVRecord()
{
}
