// VideoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "X200ActiveX.h"
#include "VideoDlg.h"
#include "afxdialogex.h"
#include "DisplayWindow.h"
#include <string>
#include <ctime>
#include <mutex>
#include "Tools.h"
#include "cJSON.hpp"

//ffmepg+sdl
extern "C"
{
#include "libavcodec\avcodec.h"
#include "libavformat\avformat.h"
#include "libswscale\swscale.h"
#include "libavutil\imgutils.h"
#include <SDL.h>
}

//const char* officialHost = "139.196.31.90";
#define KEY_ADMINID "adminId"
#define KEY_FLAG "flag"
#define KEY_SNID "snId"
#define KEY_SEND_TYPE "sendType"

mutex mut;
//const char* officialHost = "114.215.115.84";
DisplayWindow* mDisplayWindowsArray[9];
// CVideoDlg 对话框

IMPLEMENT_DYNAMIC(CVideoDlg, CDialogEx)
UINT Thread_sendHeart(LPVOID lpParam) {
	CVideoDlg* pThis = (CVideoDlg*)lpParam;
	pThis->SendHeart();
	return  0;
}



UINT Thread_Udp(LPVOID lpParam) {
	CVideoDlg* pThis = (CVideoDlg*)lpParam;
	pThis->ProcessUdp();

	//playVideo(lpParam);

	return 0;
}


UINT Thread_OpenOL(LPVOID lpParam) {
	CVideoDlg* pThis = (CVideoDlg*)lpParam;
	//openOL(lpParam);
	pThis->OpenOL();
	return 0;
}




CVideoDlg::CVideoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_VIDEO_DLG, pParent)
{
	//blUDPStarted = false;
	TRACE("Create dlg!!!\n");
	strcpy(officialHost,"114.215.115.84");
	pThreadUdp = AfxBeginThread(Thread_Udp,this);
}


CVideoDlg::~CVideoDlg()
{
	for (int i = 0; i < windows_num; i++)
	{
		delete mDisplayWindowsArray[i];
		mDisplayWindowsArray[i] = NULL;
	}
	closesocket(udpSocket);
	closesocket(tcpSocket);
	WSACleanup();
	TRACE(_T("destroy dlg\n"));
}
BOOL CVideoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	// TODO:  在此添加额外的初始化

	m_blkBrush = ::CreateSolidBrush(RGB(0,0,0));
//	ZeroMemory(mDisplayWindowsArray,sizeof(mDisplayWindowsArray));
	SetTimer(1, 10000, NULL);
//	_CrtSetBreakAlloc(0x11321001);
	av_register_all();
	avformat_network_init();

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER))
	{
		AfxMessageBox(_T("Could not initialize SDL\n"));

	}


	mAudioDecoder = new AudioDecoder();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CVideoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	
}


BEGIN_MESSAGE_MAP(CVideoDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_MESSAGE(WM_ON_CONTROL_CMD_MSG, OnWorkThreadMsg)
//	ON_WM_DESTROY()
//ON_WM_CLOSE()
ON_WM_DESTROY()
ON_WM_CLOSE()
END_MESSAGE_MAP()

int CVideoDlg::packageUDPHeartBeat(char *pUDP, long time, boolean blFirst)
{
	//UINT16 nOffset = 0;
	/*
	nOffset = 2;

	pUDP[nOffset++] = 'T';
	pUDP[nOffset++] = 'H';

	strcpy(pUDP + nOffset, pTime);
	nOffset += strlen(pTime);

	*(pUDP + nOffset) = blFirst ? '1' : '0';
	nOffset++;

	pUDP[0] = ((nOffset - 2) >> 8) & 0xff;
	pUDP[1] = (nOffset - 2) & 0xff;*/	
   
	cJSON *pJsonRoot;
	pJsonRoot = NULL;
	pJSonStr = NULL;
	pJsonRoot = cJSON_CreateObject();
	cJSON_AddNumberToObject(pJsonRoot,KEY_ADMINID,time);
	cJSON_AddStringToObject(pJsonRoot,KEY_FLAG, blFirst ? "1" : "0");
	pJSonStr = cJSON_Print(pJsonRoot);

	UINT16 nOffset = 2;
	pUDP[nOffset++] = 'A';
	pUDP[nOffset++] = 'A';
	pUDP[nOffset++] = 'H';
	cJSON_Minify(pJSonStr);
	strcpy(pUDP + nOffset, pJSonStr);	
	nOffset += strlen(pJSonStr);
	pUDP[0] = ((nOffset - 2) >> 8) & 0xff;
	pUDP[1] = (nOffset - 2) & 0xff;
	cJSON_Delete(pJsonRoot);
	free(pJSonStr);
	return nOffset;
}



int CVideoDlg::packageTCPopenOL(char*pTCP, char* openCommand) {
	/*UINT16 nOffset = 0;
	char idChar[4] = {(id>>24)&0xff,(id>>16)&0xff,(id>>8)&0xff,id&0xff};
	pTCP[nOffset++] = 0;
	pTCP[nOffset++] = 21;
	memcpy(pTCP+nOffset,sendType,strlen(sendType));
	nOffset += strlen(sendType);
	memcpy(pTCP + nOffset, idChar, sizeof(idChar));
	nOffset += sizeof(idChar);
	strcpy(pTCP+nOffset,pTime);
	nOffset += strlen(pTime);*/
	UINT16 nOffset = 2;
	memcpy(pTCP+nOffset,openCommand,strlen(openCommand));
	nOffset += strlen(openCommand);
	
	cJSON *pJsonRoot;
	pJsonRoot = NULL;
	pJSonStr = NULL;
	pJsonRoot = cJSON_CreateObject();
	cJSON_AddNumberToObject(pJsonRoot, KEY_ADMINID, sysTime);
	cJSON_AddNumberToObject(pJsonRoot, KEY_SNID,requestId);
	cJSON_AddStringToObject(pJsonRoot,KEY_SEND_TYPE,"admin");
	pJSonStr = cJSON_Print(pJsonRoot);
	cJSON_Minify(pJSonStr);
	strcpy(pTCP + nOffset, pJSonStr);
	nOffset += strlen(pJSonStr);
	pTCP[0] = ((nOffset -2 ) >> 8) & 0xff;
	pTCP[1] = (nOffset-2 ) & 0xff;

	return nOffset;
}

int CVideoDlg::packageTCPclose(char * pTCP, char * closeCommand)
{




	UINT16 nOffset = 2;
	memcpy(pTCP + nOffset, closeCommand, strlen(closeCommand));
	nOffset += strlen(closeCommand);

	cJSON *pJsonRoot;
	pJsonRoot = NULL;
	pJSonStr = NULL;
	pJsonRoot = cJSON_CreateObject();
	cJSON_AddStringToObject(pJsonRoot, KEY_SEND_TYPE, "admin");
	cJSON_AddNumberToObject(pJsonRoot, KEY_SNID, requestId);
	
	pJSonStr = cJSON_Print(pJsonRoot);
	cJSON_Minify(pJSonStr);
	strcpy(pTCP + nOffset, pJSonStr);
	nOffset += strlen(pJSonStr);
	pTCP[0] = ((nOffset - 2) >> 8) & 0xff;
	pTCP[1] = (nOffset - 2) & 0xff;
	return nOffset;
}

void getTime(char* strTime) {
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	sprintf(strTime, "%02d%02d%03d", sys.wHour, 
		sys.wMinute, sys.wSecond);
}



void CVideoDlg::getSysTime()
{
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	char strTime[10];
	sprintf(strTime, "%02d%02d%03d", sys.wHour,
		sys.wMinute, sys.wSecond);
	sysTime = atol(strTime);

}
//使窗口动态变化
void CVideoDlg::SetWindows(int line_num, int column_num)
{
	int set_window_num = line_num*column_num;

	double r_height = rt.Height();
	double r_width = rt.Width();
	double h_shif = r_height*0.025;

	double area_width = r_width / column_num;
	double area_height = r_height / line_num;
	double s_height = area_height - h_shif;
	double s_width = s_height*scale;
	double w_shift = (area_width - s_width) / 2; //让视频居中
	int arrayIndex = 0;

	lock_guard<mutex> lock(mut);
	if (set_window_num>windows_num) //需要新建
	{
		for (int i = 0; i < line_num; i++)
		{
			for (int j = 0; j < column_num; j++)
			{
				if (mDisplayWindowsArray[arrayIndex]!=NULL)
				{
					mDisplayWindowsArray[arrayIndex]->MoveWindow(area_width*j + w_shift, area_height*i, area_width*j + w_shift + s_width, area_height*i + s_height);
				}
				else
				{
					//mDisplayWindowsArray[arrayIndex] = new DisplayWindow(area_width*j + w_shift, area_height*i, area_width*j + w_shift + s_width, area_height*i + s_height, this, arrayIndex);
				
					mDisplayWindowsArray[arrayIndex] = new DisplayWindow();
					mDisplayWindowsArray[arrayIndex]->CreateDisplayWindow(area_width*j + w_shift, area_height*i, area_width*j + w_shift + s_width, area_height*i + s_height, this, arrayIndex);
				}
				arrayIndex++;
			}
		}
   }
	else if (set_window_num<windows_num) //需要销毁
	{
		for (int i = 0; i < line_num; i++)
		{
			for (int j = 0; j < column_num; j++)
			{
			mDisplayWindowsArray[arrayIndex]->MoveWindow(area_width*j + w_shift, area_height*i, area_width*j + w_shift + s_width, area_height*i + s_height);
			arrayIndex++;
			}
		}
		for (int index = arrayIndex; index<windows_num ; index++)
		{
			
             	delete mDisplayWindowsArray[index];
	        	mDisplayWindowsArray[index] = NULL;
		}

	}
	else if (set_window_num==windows_num) //仅调整位置
	{
		for (int i = 0; i < line_num; i++)
		{
			for (int j = 0; j < column_num; j++)
			{
				mDisplayWindowsArray[arrayIndex]->MoveWindow(area_width*j + w_shift, area_height*i, area_width*j + w_shift + s_width, area_height*i + s_height);
				arrayIndex++;
			}
		}
	}
	windows_num = set_window_num;

}


void CVideoDlg::StartMonitor() {
	/*for each (DisplayWindow* window  in mDisPlayWindowsArray)
	{
	
	}*/	
	pThreadTcp = AfxBeginThread(Thread_OpenOL,this);
}

void CVideoDlg::StartMapMonitor(CHAR *ip, LONG id)
{
	//ZeroMemory(officialHost, sizeof(officialHost));
	//strcpy(officialHost, ip);
	for (int i = 0; i < windows_num; i++)
	{
		if (mDisplayWindowsArray[i]->getSelect()) mDisplayWindowsArray[i]->setID(id);
	}

	this->requestId = id;
	
	/*if (!blUDPStarted)
	{
		blUDPStarted = true;
		pThreadUdp = AfxBeginThread(Thread_Udp, this);
	}*/
    
	pThreadTcp = AfxBeginThread(Thread_OpenOL,this);
}


void CVideoDlg::CloseVideo()
{
	addrServTcp.sin_family = AF_INET;
	addrServTcp.sin_port = htons(9082);
	addrServTcp.sin_addr.S_un.S_addr = inet_addr(officialHost);
	tcpSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (tcpSocket == SOCKET_ERROR)
	{
		AfxMessageBox(_T("Tcp socket failed !/n"));
		//WSACleanup();
		return;
	}
	if (connect(tcpSocket, (struct  sockaddr*)&addrServTcp, sizeof(addrServTcp)) == INVALID_SOCKET)
	{
		AfxMessageBox(_T("connect failed !/n"));
		//WSACleanup();
		return;
	}

	char tcpBuffer[100] = { 0 };
	char closeCommand[5] = { 0 };
    sprintf(closeCommand, "%c%c%c", 'A', 'C','V');
	
//	int tcpLen = packageTCPopenOL(tcpBuffer, cvCommand, (int)requestId, timeBuf);
	int tcpLen = packageTCPclose(tcpBuffer,closeCommand);
	send(tcpSocket, tcpBuffer, tcpLen, 0);
	
	closesocket(tcpSocket);
	AfxMessageBox(_T("关闭直播已发出！"));
	SDL_PauseAudio(1);
	WSACleanup();
	return;
}




//开启tcp请求
void CVideoDlg::OpenOL()
{  
	/*if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		AfxMessageBox(_T("WSAStartUp Failed!/n"));
		return;
	}*/
	addrServTcp.sin_family = AF_INET;
	addrServTcp.sin_port = htons(9082);
	addrServTcp.sin_addr.S_un.S_addr = inet_addr(officialHost);
	tcpSocket = socket(AF_INET,SOCK_STREAM,0);

	if (tcpSocket == SOCKET_ERROR)
	{
		AfxMessageBox(_T("Open Tcp socket failed !/n"));
	//	WSACleanup();
		return;
	}
	if (connect(tcpSocket, (struct  sockaddr*)&addrServTcp, sizeof(addrServTcp)) == INVALID_SOCKET)
	{
		AfxMessageBox(_T("Open connect failed !/n"));
	//	WSACleanup();
		return;
	}
	char tcpBuffer[100] = { 0 };
	char olCommand[5] = { 0 };
	char recBuf[1024] = { 0 };
	sprintf(olCommand,"%c%c%c",'A','O','V');
	int tcpLen = packageTCPopenOL(tcpBuffer, olCommand);
	send(tcpSocket, tcpBuffer, tcpLen, 0);
	ZeroMemory(tcpBuffer, sizeof(tcpBuffer));
	int ret=recv(tcpSocket, recBuf, 1024, 0);
	if (ret<0)
	{
		AfxMessageBox(_T("Tcp 接受失败!! "));
	}
	char returnData = recBuf[14];
	switch (returnData)
	{
	case '0':
		AfxMessageBox(_T("连接成功!!!"));
		break;
	case '1':
		AfxMessageBox(_T("连接失败!!!"));
		break;
	case '2':
		AfxMessageBox(_T("未找到设备!!!"));
		break;
	default:
		break;
	}
	
	//if (recBuf[2] == 'U'&&recBuf[3] == 'V')
	//{
	//	AfxMessageBox(_T("准备接受数据！"));
 //   }
	//else
	//{
	//	AfxMessageBox(_T("请求直播失败"));
	//}
	closesocket(tcpSocket);
	//WSACleanup();
	return;
}


//发送心跳
void CVideoDlg::SendHeart()
{
	char udpBuffer[100] = { 0 };
	int nUdpLen = 0;
	int nServAddLen = sizeof(addrServUdp);
		nUdpLen = packageUDPHeartBeat(udpBuffer,sysTime, isReconnect);
		TRACE(_T("isReconnect=%d\n"),isReconnect);
		int ret = sendto(udpSocket, udpBuffer, nUdpLen, 0, (sockaddr*)&addrServUdp, nServAddLen);
		if (ret == SOCKET_ERROR)
		{
			AfxMessageBox(_T("发送心跳失败==%d"), ret);
			//closesocket(udpSocket);
			//WSACleanup();
			return;
		}	
		isReconnect = true;  //if not get SH
}


//udp线程，处理心跳和视频数据
void CVideoDlg::ProcessUdp()
{

	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		AfxMessageBox(_T("WSAStartUp udp Failed!"));
		return;
	}

	udpSocket = socket(AF_INET, SOCK_DGRAM, 0);

	if (INVALID_SOCKET == udpSocket)
	{
		AfxMessageBox(_T("create socket udp failed !"));
		WSACleanup();
		return;
	}

	addrServUdp.sin_family = AF_INET;   //address family internet
	addrServUdp.sin_port = htons(9081); //host to network short
	addrServUdp.sin_addr.S_un.S_addr = inet_addr(officialHost);
	int udpServLen = sizeof(addrServUdp);
	const int s_whole_buf = 1024;

	char *whole_buf = new char[s_whole_buf];
	char *video_buf = new char[s_whole_buf];

	ZeroMemory(video_buf,s_whole_buf);
	ZeroMemory(whole_buf, s_whole_buf);
//	getTime(timeBuf);
	getSysTime();
	SendHeart();


	while (1)
	{
		
	int	retVal = recvfrom(udpSocket, whole_buf, s_whole_buf, 0, (SOCKADDR*)&addrServUdp, &udpServLen);
		//TRACE("retVal==%d\n", retVal);
		if (SOCKET_ERROR == retVal)
		{
			CString temp;
			temp.Format(_T("recv failed===%d!!!"), retVal);
			AfxMessageBox(temp);
		//	closesocket(udpSocket);
			//closesocket(sClient);
			//WSACleanup();
			return ;
		}
		

		if ((whole_buf[2]=='A')&&(whole_buf[3]=='R')&&(whole_buf[4]='H'))
		{
			TRACE(_T("收到心跳\n"));
			isReconnect = false;
		}
		else
		{
			int len = ((whole_buf[0] & 0xff) << 8) | (whole_buf[1] & 0xff);
			//处理视频数据		
				int recId = Tools::char_to_int(whole_buf[5], whole_buf[6], whole_buf[7], whole_buf[8]);
				memcpy(video_buf, whole_buf + 9, len - 7);
				if (video_buf[0] == 1)
				{
					TRACE(_T("收到视频数据\n"));

					lock_guard<mutex> lock(mut);
					for (int i = 0; i < windows_num; i++)
					{
						if (mDisplayWindowsArray[i]->getID() == recId)
						{
							TRACE("find ID\n");

							mDisplayWindowsArray[i]->StartDisplayVideo(video_buf);
						}
					}

					//mDisPlayWindowsArray[0]->Start(video_buf);
				}
				else if (video_buf[0] == 0)
				{
					//音频处理
					TRACE(_T("收到音频数据====%d\n"), retVal);
					if (recId == requestId)
				    {
                     	mAudioDecoder->setAudioUdpData(video_buf, len - 7);
					}
				
				}
			}
		ZeroMemory(whole_buf, s_whole_buf);
		ZeroMemory(video_buf, s_whole_buf);
	}
		ZeroMemory(whole_buf, s_whole_buf);
		ZeroMemory(video_buf, s_whole_buf);
		delete[]video_buf;
		delete[]whole_buf;
}


   


//计时器
void CVideoDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	timeNum++;
	TRACE(_T("this is timer=====%d\n"),timeNum);
	SendHeart();
	CDialogEx::OnTimer(nIDEvent);
}


//由static返回的消息
LRESULT CVideoDlg::OnWorkThreadMsg(WPARAM wParam, LPARAM lParam)
{
	if (wParam == STATIC_SELECT_MSG)
	{
		int nSelectedIndex = (int)lParam;
		for (int i = 0; i < windows_num; i++)
		{
			if (mDisplayWindowsArray[i] != NULL)
			{
				if (nSelectedIndex != i)
				{
					mDisplayWindowsArray[i]->setSelect(FALSE);
				}
				else
				{
					mDisplayWindowsArray[i]->setSelect(TRUE);
					CString temp;
					temp.Format(_T("你选择了%d号窗口\n"), i);
					AfxMessageBox(temp);
				}
			}
		}
	
	}

	return 0;
}



void CVideoDlg::ResetDlg() {
	GetWindowRect(&rt);
	//setWindows(rt, this, 1, 1);
	SetWindows(1, 1);
	mDisplayWindowsArray[0]->setSelect(TRUE);
}
void CVideoDlg::Set1X2Window()
{
	GetWindowRect(&rt);
	SetWindows(1, 2);
	//setWindows(rt, this, 1, 2);
}

void CVideoDlg::Set2X2Window()
{
	GetWindowRect(&rt);
	SetWindows(2, 2);
	//setWindows(rt, this, 2, 2);
}

void CVideoDlg::Set3X3Window()
{
	GetWindowRect(&rt);
	SetWindows(3, 3);
	//setWindows(rt, this, 3, 3);
}



//void CVideoDlg::OnDestroy()
//{
//	CDialogEx::OnDestroy();
//	TRACE("destroy dlg\n");
//	// TODO: 在此处添加消息处理程序代码
//}


//void CVideoDlg::OnClose()
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//
//	CDialogEx::OnClose();
//}


void CVideoDlg::OnDestroy()
{
	
	CDialogEx::OnDestroy();
	
	// TODO: 在此处添加消息处理程序代码
}


void CVideoDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (MessageBox(L"要关闭窗口吗？", L"友情提示", MB_ICONINFORMATION | MB_YESNO) == IDNO)
		return;
	CloseVideo();
	CDialogEx::OnClose();
}


BOOL CVideoDlg::IsInvokeAllowed(DISPID /*dispid*/)
{
	// TODO: 在此添加专用代码和/或调用基类

	return TRUE;
}
