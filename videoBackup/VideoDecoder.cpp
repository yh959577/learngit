#include "stdafx.h"
#include "VideoDecoder.h"

VideoDecoder::VideoDecoder(int l, int t, int r, int b, CWnd* dlg, int nIndex)
{
	mVideoScreen = new VideoScreen(l, t, r, b, dlg, nIndex);
	
	m_frameRate.num = 1;
	m_frameRate.den = 15;
	
	pVideoCodec = avcodec_find_decoder(AV_CODEC_ID_H264);	


	if (!pVideoCodec)
	{
		AfxMessageBox(_T("VideoCodec not found"));
    }
	pVideoCodecCtx = avcodec_alloc_context3(pVideoCodec);

	if (avcodec_open2(pVideoCodecCtx, pVideoCodec, NULL) >= 0)
	{
		pFrame = av_frame_alloc();
		pFrameYUV = av_frame_alloc();

	}
	else
	{
		AfxMessageBox(_T("open codec failed"));
	}
}


VideoDecoder::~VideoDecoder()
{
	delete mVideoScreen;
	mVideoScreen = NULL;
	avcodec_free_context(&pVideoCodecCtx);
	av_frame_free(&pFrame);
	av_frame_free(&pFrameYUV);
	
}

void VideoDecoder::StartDecode(char * frame_buf, int len)
{


	AVPacket packet = { 0 };

	int gotpicture = 0;
	packet.data = (uint8_t*)frame_buf;
	packet.size = len;
	avcodec_send_packet(pVideoCodecCtx, &packet);
	int ret = avcodec_receive_frame(pVideoCodecCtx, pFrame);	
	//int len = avcodec_decode_video2();
	
	if (ret == 0)
	{
		//动态获取编码信息
		if ((m_decode_width != pVideoCodecCtx->width) || (m_decode_height != pVideoCodecCtx->height))
		{
			out_buffer = (uint8_t*)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pVideoCodecCtx->width, pVideoCodecCtx->height, 1));
			av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer, AV_PIX_FMT_YUV420P, pVideoCodecCtx->width, pVideoCodecCtx->height, 1);
			m_decode_width = pVideoCodecCtx->width;
			m_decode_height = pVideoCodecCtx->height;
		}
		TRACE("decode success\n");
		img_convert_ctx = sws_getContext(pVideoCodecCtx->width, pVideoCodecCtx->height, pVideoCodecCtx->pix_fmt, pVideoCodecCtx->width, pVideoCodecCtx->height,
			AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
		if (img_convert_ctx != NULL) //smetimes null
		{
			sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pVideoCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);
			mVideoScreen->StartDisplay(m_decode_width, m_decode_height, pFrameYUV->data[0], pFrameYUV->linesize[0]);
		}
		}
	else if (ret == EAGAIN)
	{
		TRACE("EAGAIN\n");

	}
	else if (ret == EINVAL)
	{
		TRACE("EINVAL\n");
	}
	else if (ret == AVERROR_EOF)
	{

		TRACE("AVERROR_EOF\n");
	}
	else
	{
		/*	pFrame = av_frame_alloc();*/

		TRACE("lose frame\n", frame_buf[0]);

	}
	av_packet_unref(&packet);


}
void VideoDecoder::MoveWindow(LPRECT lpRect)
{
		mVideoScreen->MoveWindow(lpRect);
}

int VideoDecoder::getFrameRateDen()
{
	return m_frameRate.den;
}

int VideoDecoder::getFrameRateNum()
{
	return m_frameRate.num;
}

