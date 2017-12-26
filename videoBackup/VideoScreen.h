#pragma once
extern "C"
#include <SDL.h>
#include "VideoStatic.h"
class VideoScreen
{
public:
	VideoScreen(int l, int t, int r, int b, CWnd* dlg, int nIndex);
	~VideoScreen();
	void StartDisplay(int codec_width, int codec_height, const void* pixel, int pitch);
	void MoveWindow(LPCRECT lpRect);
	int m_codec_width=0;
	int m_codec_height=0;
	VideoStatic*   mScreenStatic;
	//VideoStatic* mScreenStatic;
	SDL_Window *sdlWindow;
	SDL_Renderer *sdlRenderer;
	SDL_Texture *sdlTexture;
	bool isMove=false;
	
};

