#pragma once
extern "C"
{
#include "libavcodec\avcodec.h"
#include "libavformat\avformat.h"
#include "libswscale\swscale.h"
#include "libavutil\imgutils.h"
}
#include "VideoScreen.h"
class VideoDecoder
{
public:
	VideoDecoder(int l, int t, int r, int b, CWnd* dlg, int nIndex);
	~VideoDecoder();
	void StartDecode(char* frame, int len);
	void MoveWindow(LPRECT lpRect);
	int getFrameRateDen();
	int getFrameRateNum();
	int m_decode_width;
	int m_decode_height;
	AVRational m_frameRate;

	AVFrame* pFrame;
	AVFrame* pFrameYUV;
	AVCodecContext* pVideoCodecCtx;
	AVCodec* pVideoCodec;
	uint8_t* out_buffer;
	AVRational mAVRational;
	struct SwsContext *img_convert_ctx;
	VideoScreen* mVideoScreen;
};

