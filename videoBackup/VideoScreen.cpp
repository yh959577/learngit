#include "stdafx.h"
#include "VideoScreen.h"
#include <mutex>

using namespace std;
mutex screenMut;

VideoScreen::VideoScreen(int l, int t, int r, int b, CWnd* dlg, int nIndex)
{
	mScreenStatic = new VideoStatic(dlg, nIndex);
	bool create = mScreenStatic->Create(NULL, WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE|SS_NOTIFY, CRect(l, t, r, b), dlg);
	  mScreenStatic->setBackColor();
	 
	if (!create)
	{
		AfxMessageBox(_T("create static failed\n"));
	}
	sdlWindow = SDL_CreateWindowFrom(mScreenStatic->GetSafeHwnd());

	if (!sdlWindow)
	{
		AfxMessageBox(_T("Could not create windows -exiting\n"));

	}

	sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_SOFTWARE);

}


VideoScreen::~VideoScreen()
{
	delete mScreenStatic;
	mScreenStatic = NULL;
}

void VideoScreen::StartDisplay(int codec_width, int codec_height, const void * pixel, int pitch)
{
	lock_guard<mutex> lock(screenMut);
	if (isMove)
	{
		sdlWindow = SDL_CreateWindowFrom(mScreenStatic->GetSafeHwnd());

		if (!sdlWindow)
		{
			AfxMessageBox(_T("Could not create windows -exiting\n"));

		}

		sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_SOFTWARE);
		sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, codec_width, codec_height);
		isMove = false;
    }


	if (m_codec_width != codec_width || m_codec_height != m_codec_height)
	{
		sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, codec_width, codec_height);
		m_codec_width = codec_width;
		m_codec_height = codec_height;
	}
	SDL_UpdateTexture(sdlTexture, NULL, pixel, pitch);
	SDL_RenderClear(sdlRenderer);
	//SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, &rect_one);
	SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
	SDL_RenderPresent(sdlRenderer);
}

void VideoScreen::MoveWindow(LPCRECT lpRect)
{
	lock_guard<mutex>lock(screenMut);
	mScreenStatic->MoveWindow(lpRect,true);
	isMove = true;
}

