#include "stdafx.h"
#include "AudioDecoder.h"

#define AUDIO_INBUF_SIZE   20480
#define AUDIO_REFILL_THRESH   4096
 // Uint8  *audio_chunk;

using namespace std;
//mutex  AudioMut;

AudioDecoder::AudioDecoder()
{
	/*av_register_all();
	avformat_network_init();*/
	avCodec = avcodec_find_decoder(AV_CODEC_ID_AAC);
	

	if (avCodec != NULL) {
		avCodecCtx = avcodec_alloc_context3(avCodec);
	
		// 初始化codecCtx
		/*avCodecCtx->codec_type = AVMEDIA_TYPE_AUDIO;
		avCodecCtx->sample_rate = 44100;
		avCodecCtx->channels =2;
		avCodecCtx->bit_rate = 320000;
		avCodecCtx->channel_layout = AV_CH_LAYOUT_STEREO;*/

		// 打开codec
		if (avcodec_open2(avCodecCtx, avCodec, NULL) >= 0) {
			avCodcFrame = av_frame_alloc();
		}
		else
		{
			AfxMessageBox(_T("Could not open codec"));

		}
	}
	sdl_init();
}


AudioDecoder::~AudioDecoder()
{
	avcodec_free_context(&avCodecCtx);
	av_frame_free(&avCodcFrame);
	SDL_ClearQueuedAudio(deviceId);
}


UINT Thread_DecodeAudio(LPVOID lpParam) {
	AudioDecoder* pThis = (AudioDecoder*)lpParam;
	pThis->StartAudioProcess();
	return 0;
}


void AudioDecoder::startDecode(char * mediaData, int frame_len)
{
	//av_init_packet(&avPackt);
	AVPacket avPackt = { 0 };
	avPackt.data = (uint8_t *)mediaData;
	avPackt.size = frame_len;

	if (&avPackt) {
		avcodec_send_packet(avCodecCtx, &avPackt);
		int result = avcodec_receive_frame(avCodecCtx, avCodcFrame);

		if (result == 0) {
			TRACE(_T("解码成功!!!\n"));
			struct SwrContext *au_convert_ctx = swr_alloc();
			au_convert_ctx = swr_alloc_set_opts(au_convert_ctx,
				AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16, avCodecCtx->sample_rate,
				avCodecCtx->channel_layout, avCodecCtx->sample_fmt, avCodecCtx->sample_rate,
				0, NULL);
			swr_init(au_convert_ctx);

			int out_linesize;
			int out_buffer_size = av_samples_get_buffer_size(&out_linesize, avCodecCtx->channels, avCodecCtx->frame_size, avCodecCtx->sample_fmt, 1);
			uint8_t *out_buffer = (uint8_t *)av_malloc(MAX_AUDIO_FRAME_SIZE * 2);
			swr_convert(au_convert_ctx, &out_buffer, out_linesize, (const uint8_t **)avCodcFrame->data, avCodcFrame->nb_samples);
			PlayAudio(out_buffer, out_linesize);
			swr_free(&au_convert_ctx);
			au_convert_ctx = NULL;
			// 释放
			av_free(out_buffer);
		}
		else
		{
			TRACE(_T("解码失败\n"));
		}
	}
	av_packet_unref(&avPackt);


}


void AudioDecoder::setAudioUdpData(char * udp_data, int udp_len)
{
	lock_guard<mutex> lock(AudioMut);
	mAudioUdpStruct = new UdpDataStruct(udp_data, udp_len);
	AudioData *pf = new AudioData;
	pf->frameDataLen = mAudioUdpStruct->getLength();
	pf->data = new char[pf->frameDataLen];
	memcpy(pf->data, mAudioUdpStruct->getAudioData(), pf->frameDataLen);
	audioCache.push(pf);
	
//	startDecode(mAudioUdpStruct->getAudioData(), mAudioUdpStruct->getLength());
	//mVideoDecoder->StartDecodeAudio(mAudioUdpStruct->getAudioData(),mAudioUdpStruct->getLength());


	for (int i = 0; i < 4; i++)
	{
		mAudioUdpStruct->nextAudioShift();
		AudioData *pf = new AudioData;
		pf->frameDataLen = mAudioUdpStruct->getLength();
		pf->data = new char[pf->frameDataLen];
		memcpy(pf->data, mAudioUdpStruct->getAudioData(), pf->frameDataLen);
		audioCache.push(pf);
	//	startDecode(mAudioUdpStruct->getAudioData(), mAudioUdpStruct->getLength());
		//	mVideoDecoder->StartDecodeAudio(mAudioUdpStruct->getAudioData(), mAudioUdpStruct->getLength());

	}
	if (!isAudioThreadExit&&audioCache.size()>5)
	{
		pAudioThread = AfxBeginThread(Thread_DecodeAudio,this);
	}
	delete mAudioUdpStruct;
	mAudioUdpStruct = NULL;

}



void  AudioDecoder::fill_audio(void * udata, Uint8 * stream, int len)
{
	AudioDecoder *pThis = (AudioDecoder*)udata;
	SDL_memset(stream, 0, len);
	if (pThis->audio_len == 0)
		return;
	len = (len>pThis->audio_len ? pThis->audio_len : len);

	SDL_MixAudio(stream, pThis->audio_pos, len, SDL_MIX_MAXVOLUME);
	pThis->audio_pos += len;
	pThis->audio_len -= len;
}

void AudioDecoder::sdl_init()
{
	/*if (SDL_Init(SDL_INIT_AUDIO)) {
		AfxMessageBox(_T("Could not initialize SDL - %s\n", SDL_GetError()));
		return;
	}*/
	//SDL_AudioSpec  
	SDL_AudioSpec wanted_spec;
	wanted_spec.freq = 44100;
	wanted_spec.format = AUDIO_S16SYS;
	wanted_spec.channels = 2;
	wanted_spec.silence = 0;
	wanted_spec.samples = 1024;
	wanted_spec.callback = NULL;// fill_audio;
	//	wanted_spec.userdata = avCodecCtx;
	deviceId = SDL_OpenAudio(&wanted_spec, NULL);
	if (deviceId<0) {
		AfxMessageBox(_T("can't open audio"));
		return;
	}
	else
	{
		deviceId = 1;
	}
	
	SDL_PauseAudio(0);
}
void AudioDecoder::PlayAudio(uint8_t * out_buf, int len)
{
	TRACE(_T("开始播放,out_buf=0x%x,lend=%d\n"), out_buf, len);

	if ((out_buf == NULL) || (len <= 0))
	{
		TRACE(_T("PlayAudio Error!!"));
		return;
	}

	//Play  
	int ret=SDL_QueueAudio(deviceId, out_buf, len);
	//av_free(out_buf);
	/*while (audio_len>0)
	{
		SDL_Delay(1);
	}
	audio_pos = (Uint8 *)out_buf;
	audio_len = len;*/
	//audio_pos = audio_chunk;

	return;
}

void AudioDecoder::StartAudioProcess()
{
	isAudioThreadExit = true;
	while (isAudioThreadExit)
	{
		lock_guard<mutex> lock(AudioMut);
		if (audioCache.size()>5)
		{
			startDecode(audioCache.front()->data,audioCache.front()->frameDataLen);
			delete audioCache.front();
			audioCache.front() = NULL;
			audioCache.pop();
		}
    }


}
