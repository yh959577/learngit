#pragma once
#include "VideoDecoder.h"
#include "AudioDecoder.h"
#include <queue>
#include <concurrent_queue.h>
#include <list>
#include "UdpDataStruct.h"
using namespace std;
struct frameData
{
	int time;
	int frameDataLen;
	char* data;
	~frameData() {
		delete[] data;
		data = NULL;
	}
};

class DisplayWindow
{
public:
	DisplayWindow();
	DisplayWindow(int l, int t, int r, int b, CWnd* dlg, int nIndex);
	~DisplayWindow();
	void StartDisplayVideo(char* UdpData);
	void CreateDisplayWindow(int l, int t, int r, int b, CWnd* dlg, int nIndex);
	void MoveWindow(int l, int t, int r, int b);
	void setID(long id);
	void StartDecode();
	long getID();
	void setSelect(bool isSelect);
	bool getSelect();
	int OrderVideoUdp();	
	VideoDecoder* mVideoDecoder;
	int frame_buf_size = 1000 * 500;
	char* frame_buf;
	int frameShift;
	int timeStamp;
	bool isDecodeThreadExit = false;
	CWinThread* pDecodeThread;
	UdpDataStruct* mVideoUdpStruct;
	list<UdpDataStruct*> orderedList;
	std::list<UdpDataStruct*>::reverse_iterator mIterator;
private:
	queue<frameData*> cache;
	long mID=0;
	bool mIsSelect=false;
	bool mIsExit = true;
 };

