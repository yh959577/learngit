#include "stdafx.h"
#include "DisplayWindow.h"
#include <queue>
#include <iostream>
#include "Tools.h"
#include <Windows.h>
#include <mutex>

mutex mutt;
int find_head(char* buf, int len) {
	int i;
	bool isMatch = FALSE;
	for (i = 0; i < len; i++)
	{
		if ((buf[i] == 0 && buf[i + 1] == 0 && buf[i + 2] == 0 && buf[i + 3] == 1 && buf[i + 4] == 0x41
			) || (buf[i] == 0 && buf[i + 1] == 0 && buf[i + 2] == 0 && buf[i + 3] == 1 && buf[i + 4] == 0x67))

		{
			isMatch = TRUE;
			break;
		}
	}
	return isMatch ? i : -1;
}

int find_head_H265(char* buf, int len) {

	int i;
	bool isMatch = FALSE;
	for (i = 0; i < len; i++)
	{
		if ((buf[i] == 0 && buf[i + 1] == 0 && buf[i + 2] == 0 && buf[i + 3] == 1 && buf[i + 4] == 2
			) || (buf[i] == 0 && buf[i + 1] == 0 && buf[i + 2] == 0 && buf[i + 3] == 1 && buf[i + 4] == 0x40))

		{
			isMatch = TRUE;
			break;
		}
	}
	return isMatch ? i : -1;
}

UINT thread_play(LPVOID pParam) {
	TRACE("open decode thread\n");
	DisplayWindow *pThis = (DisplayWindow*)pParam;
	pThis->StartDecode();
    return 0;
}


DisplayWindow::DisplayWindow()
{
	//初始化拼帧变量
	frameShift = 0;
	frame_buf = new char[frame_buf_size];
	ZeroMemory(frame_buf, frame_buf_size);
	timeStamp = 0;
}

DisplayWindow::DisplayWindow(int l, int t, int r, int b, CWnd* dlg, int nIndex)
{
	mVideoDecoder = new VideoDecoder(l, t, r, b, dlg, nIndex);
	frameShift = 0;
	frame_buf = new char[frame_buf_size];
	ZeroMemory(frame_buf, frame_buf_size);
	timeStamp = 0;
	//pThread = AfxBeginThread(thread_play,this);
    
}


DisplayWindow::~DisplayWindow()
{
	TRACE(_T("destroy DisplayWindow\n"));
	delete mVideoDecoder;
	delete[] frame_buf;
	mVideoDecoder = NULL;
	frame_buf = NULL;
	//lock_guard<mutex> lock(mutt);
	mIsExit = 0;
}


void DisplayWindow::StartDisplayVideo(char * UdpData)
{
	  mVideoUdpStruct = new UdpDataStruct(UdpData);
       
	  OrderVideoUdp();
	
	  TRACE(_T("orderListSize======%d\n"), orderedList.size());
	  if ((orderedList.size() > 10))
	  {  
		 // lock_guard<mutex> lock(mutt);
		  int videoDataLen = orderedList.front()->getLength();
		  char* videoData = new char[videoDataLen];
		  ZeroMemory(videoData, videoDataLen);
		  memcpy(videoData, orderedList.front()->getVideoData(), videoDataLen);
		  delete  orderedList.front();
		  orderedList.front() = NULL;
		  orderedList.pop_front();
		/*  int videoTag = orderedList.front()->getFrameTag();
		 
		  switch (videoTag)
		  {
          case 0:
			  ZeroMemory(frame_buf,frame_buf_size);	  
			  frameShift = 0;
			  memcpy(frame_buf,videoData,videoDataLen);
			  frameShift += videoDataLen;
			  TRACE(_T("开始帧=====%d\n"),timeStamp);
			  break;
		  case 1:
			  memcpy(frame_buf+frameShift,videoData,videoDataLen);
			  frameShift += videoDataLen;
			  TRACE(_T("中间帧=======%d\n"),timeStamp);
			  break;
		  case 2:
			  
			  memcpy(frame_buf + frameShift, videoData, videoDataLen);
			  frameShift += videoDataLen;
			  frameData f;
			  f.frameDataLen = frameShift;
	   	      f.data = new char[f.frameDataLen];
			  f.time = orderedList.front()->getTime();
			  memcpy(f.data, frame_buf, frameShift);
			  cache.push(f);
			  TRACE(_T("结束帧=======%d\n"),timeStamp);
			  break;
		  case 3:
			  ZeroMemory(frame_buf, frame_buf_size);
			  memcpy(frame_buf, videoData, videoDataLen);
			  f.frameDataLen = videoDataLen;
			  f.data = new char[f.frameDataLen];
			  f.time = orderedList.front()->getTime();
			  memcpy(f.data, frame_buf, videoDataLen);
			  cache.push(f);
			  TRACE(_T("独立帧========%d\n"),timeStamp);
			  break;
		  default:
			  TRACE(_T("数据异常\n"));
			  break;
		  }

*/
		 
		 //  int finalTime = orderedList.front()->getTime();
		
		  int headPosition = find_head(videoData, videoDataLen);


		  if (headPosition != -1)
		  {
			  lock_guard<mutex> lock(mutt);
		  	if (headPosition == 0)
		  	{   //I frame in the packet head
		  		//send frameBuf,clear frameBuf,write frameBuf
		  		if (frameShift > 0)
		  		{
		  			//	beginToDecode(pFrame_buf, totalFrame);//total Frame: the real len of the data
		  			//	mVideoDecoder->StartDecode(frame_buf, frameShift);									  //memset(pFrame_buf, 0, FF_INPUT_BUFFER_PADDING_SIZE);
		  			frameData* f=new frameData;
		  			f->frameDataLen = frameShift;
		  			f->data = new char[f->frameDataLen];
		  			memcpy(f->data, frame_buf, frameShift);
		  			cache.push(f);
				
		  			ZeroMemory(frame_buf, frame_buf_size);
		  			frameShift = 0;
		  		}
		  		memcpy(frame_buf, videoData, videoDataLen);
		  		frameShift += videoDataLen;
			/*	delete[] videoData;
				videoData = NULL;*/
			}
		  	else
		  	{
		  		//I frame in the  middle of the packet
		  		memcpy(frame_buf + frameShift, videoData, headPosition);
		  		frameShift += headPosition;

		  		//beginToDecode(pFrame_buf, totalFrame);
		  		//	mVideoDecoder->StartDecode(frame_buf, frameShift);
		  		frameData *f=new frameData;
		  		f->frameDataLen = frameShift;
		  		f->data = new char[f->frameDataLen];
		  		memcpy(f->data, frame_buf, frameShift);
		//		lock_guard<mutex> lock(mutt);
		  		cache.push(f);
		  		//memset(frame_buf, 0, FF_INPUT_BUFFER_PADDING_SIZE);
		  		ZeroMemory(frame_buf, frame_buf_size);
		  		frameShift = 0;
		  		memcpy(frame_buf, videoData + headPosition, videoDataLen - headPosition);
		  		frameShift += (videoDataLen - headPosition);
		/*		delete[] videoData;
				videoData = NULL;*/
		  	}
		  }
		  else
		  {
		  	//not have I frame
		  	//write to frame_buf
		  	memcpy(frame_buf + frameShift, videoData, videoDataLen);
		  	frameShift += videoDataLen;
		/*	delete[] videoData;
			videoData = NULL;*/
		  }
		 delete[] videoData;
		  videoData = NULL;
}
	  TRACE(_T("cacheSize===%d\n"), cache.size());
	if ((!isDecodeThreadExit)&&cache.size()>5)
	{
		pDecodeThread = AfxBeginThread(thread_play, this);
	}
}

void DisplayWindow::CreateDisplayWindow(int l, int t, int r, int b, CWnd * dlg, int nIndex)
{
	mVideoDecoder = new VideoDecoder(l, t, r, b, dlg, nIndex);
}

void DisplayWindow::MoveWindow(int l, int t, int r, int b)
{
	RECT rect = { l,t,r,b };
	mVideoDecoder->MoveWindow(&rect);
}

void DisplayWindow::setID(long id)
{
	mID = id;
}

void DisplayWindow::StartDecode()
{
	isDecodeThreadExit = true;
	int rate = 0;
	while (mIsExit)
	{
		//TRACE(_T("decode th===%d\n"),mIsExit);
		
			if (cache.size() >0)
			{
				/*if (lastTime!=0)
				{
					deltaTime = cache.front().time - lastTime;
					Sleep(deltaTime);
				}*/

					lock_guard<mutex> lock(mutt);
					mVideoDecoder->StartDecode(cache.front()->data, cache.front()->frameDataLen);
					//				lock_guard<mutex> lock(mutt);
								//	lastTime = cache.front().time;
				 //   rate = (mVideoDecoder->getFrameRateDen())/(mVideoDecoder->getFrameRateNum());
					delete cache.front();
					cache.front() = NULL;
					cache.pop();
					
			}
			//if (rate!=0)
			//{
			//	//TRACE(_T("the decoder rate is %d\n"), rate);
				//Sleep(1000 /30);
			//}
			//Sleep(1000 /15);
	}
	return;
}

long DisplayWindow::getID()
{
	return mID;
}

void DisplayWindow::setSelect(bool isSelect)
{
	mIsSelect = isSelect;
}

bool DisplayWindow::getSelect()
{
	return mIsSelect;
}

int DisplayWindow::OrderVideoUdp()
{
	if (orderedList.size() == 0)
		{
			orderedList.push_front(mVideoUdpStruct);
		}
		else {
			for (mIterator = orderedList.rbegin(); mIterator != orderedList.rend(); mIterator++)
			{
				if ((mVideoUdpStruct->getNum()) > ((*mIterator)->getNum())) {
					std::list<UdpDataStruct*>::iterator i(mIterator.base());
					orderedList.insert(i, mVideoUdpStruct);
					return 0;
				}
			}
			orderedList.push_front(mVideoUdpStruct);
		}


	//orderedList.push_back(mUdpStruct);
		return 0;

}





