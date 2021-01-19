#include "LX_Rtmp.h"

#include <iostream>
#include <string>
using namespace std;
extern "C"
{
#include <libavformat/avformat.h>
#include <libavutil/time.h>
}
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avformat.lib")

LX_Rtmp::LX_Rtmp()
{
	//ע�����еı������
	avcodec_register_all();
	//ע�����еķ�װ��
	av_register_all();
	//ע����������Э��
	avformat_network_init();
}
bool LX_Rtmp::Init(const char *url)
{
	///5 �����װ������Ƶ������
	//a ���������װ��������
	int ret = avformat_alloc_output_context2(&ic, 0, "flv", url);
	this->url = url;
	if (ret != 0)
	{
		char buf[1024] = { 0 };
		av_strerror(ret, buf, sizeof(buf) - 1);
		cout << buf;
		return false;
	}
	return true;
}
bool LX_Rtmp::open()
{
	///��rtmp ���������IO
	int ret = avio_open(&ic->pb, url.c_str(), AVIO_FLAG_WRITE);
	if (ret != 0)
	{
		char buf[1024] = { 0 };
		av_strerror(ret, buf, sizeof(buf) - 1);
		cout << buf << endl;
		return false;
	}
	return true;
}
void LX_Rtmp::Close()
{
	if (ic)
	{
		avformat_close_input(&ic);
		vs = NULL;
	}
	vc = NULL;
	url = "";
}
bool LX_Rtmp::AddStream(AVCodecContext *c)
{
	if (!c)return false;

	//b ������Ƶ�� 
	AVStream *st = avformat_new_stream(ic, NULL);
	if (!st)
	{
		cout << "avformat_new_stream failed" << endl;
		return false;
	}
	st->codecpar->codec_tag = 0;
	//�ӱ��������Ʋ���
	avcodec_parameters_from_context(st->codecpar, c);
	av_dump_format(ic, 0, url.c_str(), 1);

	if (c->codec_type == AVMEDIA_TYPE_VIDEO)
	{
		vc = c;
		vs = st;
		indexOfVideo = vs->index;
	}
	else if (c->codec_type == AVMEDIA_TYPE_AUDIO)
	{
		ac = c;
		as = st;
		indexOfAudio = as->index;
	}
	return true;
}


bool LX_Rtmp::SendHead()
{
	//д���װͷ
	int ret = avformat_write_header(ic, NULL);
	if (ret != 0)
	{
		char buf[1024] = { 0 };
		av_strerror(ret, buf, sizeof(buf) - 1);
		cout << buf << endl;
		return false;
	}
	return true;
}
bool LX_Rtmp::SendPacket(AVPacket *pack)
{
	int index = pack->stream_index;
	//��������ڲ�����һ��ʱ�������ת��
	//��packet��ʱ�����ת��Ϊic��videostream��ʱ�����
	//packet����videoencoder��������,����time_base��videoencoderָ��(��ָ��Ϊvc->time_base = { 1,fps };)
	//��ֻ��һ�ֱ���,ʵ����AVPacket�ṹ���в�û��time_base
	//������������������,�ǻ���ic������,Ҳ����AVFormatContext *ic
	//��AddStream��ʱ���õ��˱������Ĳ���,�����������沢û��time_base
	//��ffmpeg���ֲ��֪,�ڵ���avformat_write_header֮��time_base�Żᱻȷ��(���ǵ����Ƕ���,��û����ȷ˵��),
	//������һ����ת���Ǳ�Ҫ��
	if (!pack||pack->size <= 0 || !pack->data)return false;

	AVRational src_time_base;
	AVRational dst_time_base;
	//�ж�����Ƶ������Ƶ
	if (vs && vc&& pack->stream_index == vs->index)
	{
		src_time_base = vc->time_base;
		dst_time_base = vs->time_base;
	}
	else if (as && ac&&pack->stream_index == as->index)
	{
		src_time_base = ac->time_base;
		dst_time_base = as->time_base;
	}

	//����
	pack->pts = av_rescale_q(pack->pts, src_time_base, dst_time_base);
	pack->dts = av_rescale_q(pack->dts, src_time_base, dst_time_base);
	pack->duration = av_rescale_q(pack->duration, src_time_base, dst_time_base);
	int ret = av_interleaved_write_frame(ic, pack);
	//int ret = av_write_frame(ic, pack);
	//av_packet_free(&pack);
	if (ret == 0)
	{
		if (index == indexOfAudio)
		{
			cout << "#" << flush;
		}
		if (index == indexOfVideo)
		{
			cout << "$" << flush;
		}
		return true;
	}
	return false;
}

LX_Rtmp::~LX_Rtmp()
{
}