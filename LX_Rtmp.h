#pragma once
#include <iostream>
using namespace std;
struct AVFormatContext;
struct AVCodecContext;
struct AVPacket;
struct AVStream;
class LX_Rtmp
{
public:
	LX_Rtmp();
	~LX_Rtmp();
	bool Init(const char *url);
	void Close();
	bool AddStream(AVCodecContext *c);
	bool SendHead();
	bool SendPacket(AVPacket *pack);
	bool open();
	//rtmp flv ·â×°Æ÷
	AVFormatContext *ic = 0;
	string url = "";
	//ÊÓÆµ±àÂëÆ÷Á÷
	AVCodecContext *vc = 0;
	AVStream *vs = 0;
	AVCodecContext *ac = 0;
	AVStream *as = 0;

	int indexOfVideo = -1;
	int indexOfAudio = -1;
};

