#pragma once
extern "C"
{
#include "libavcodec\avcodec.h"
#include "libavformat\avformat.h"
#include "libswscale\swscale.h"
#include "libavutil\imgutils.h"
#include "libswresample\swresample.h"
}
#include <mutex>
#include <SDL.h>
#include <queue>
#include"UdpDataStruct.h"

//using namespace std;

#define MAX_AUDIO_FRAME_SIZE 192000

struct AudioData
{
	int frameDataLen;
	char* data;
};

class AudioDecoder
{
public:
	AudioDecoder();
	~AudioDecoder();
	void startDecode(char* frame,int frame_len);
	void setAudioUdpData(char* udp_data, int udp_len);
    void PlayAudio(uint8_t* out_buf, int outLen);
	void StartAudioProcess();
	void sdl_init();

	static void  fill_audio(void * udata, Uint8 * stream, int len);
	
	Uint32  audio_len;
	Uint8  *audio_pos;
private:
	AVCodec* avCodec;
//	AVPacket avPackt;
	AVCodecContext* avCodecCtx;
	AVFrame* avCodcFrame;
	UdpDataStruct* mAudioUdpStruct;
	bool isAudioThreadExit = false;
	std::queue<AudioData*> audioCache;
	CWinThread* pAudioThread;
	//存储参数避免反复设置
	int m_sample_rate = 0;
	uint64_t m_channel_layout = 0;
	int m_audio_len;
	int m_channels;
	int m_frame_size;
	//uint8_t *out_buffer = (uint8_t *)av_malloc(MAX_AUDIO_FRAME_SIZE * 2);
	//
	std::mutex  AudioMut;
	SDL_AudioDeviceID deviceId;
};

