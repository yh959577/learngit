#pragma once
#include "afxwin.h"
#include <thread>
#include "AudioDecoder.h"
// CVideoDlg 对话框

class CVideoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CVideoDlg)

public:
	CVideoDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CVideoDlg();
	CWinThread* pThreadHeart;
	CWinThread* pThreadUdp;
	CWinThread* pThreadTcp;
	LONG requestId=0;
	
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VIDEO_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void StartMonitor();
	afx_msg void StartMapMonitor(CHAR *ip, LONG id);
	afx_msg void ResetDlg();
	afx_msg void SetWindows(int line_num, int column_num);
	afx_msg void Set1X2Window();
	afx_msg void Set2X2Window();
	afx_msg void Set3X3Window();
	afx_msg void CloseVideo();
	afx_msg void OpenOL();
	afx_msg void SendHeart();
	afx_msg void ProcessUdp();

	int packageUDPHeartBeat(char *pUDP, long time, boolean blFirst);
	int packageTCPopenOL(char*pTCP, char* openCommand);
	int packageTCPclose(char*pTCP,char* closeCommand);
	void getSysTime();
	LRESULT OnWorkThreadMsg(WPARAM wParam, LPARAM lParam);

//	afx_msg void OnTimer(UINT_PTR nIDEvent);
    

private:
	
	CRect rt;
	//char timeBuf[100];
	long sysTime;
	double scale = 9.0 / 16.0;
	int windows_num=0;
	HBRUSH m_blkBrush;
	WSADATA wsd;
	bool isReconnect=true;
	int timeNum = 0;
	SOCKET udpSocket;
	SOCKET tcpSocket;
	SOCKADDR_IN addrServUdp;
	SOCKADDR_IN addrServTcp;
	bool isFindId=false;
	AudioDecoder* mAudioDecoder;
	int mSelectedId;
	char *pJSonStr;

	CHAR officialHost[100];

	bool blUDPStarted;

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
//	afx_msg void OnDestroy();
//	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	virtual BOOL IsInvokeAllowed(DISPID /*dispid*/);
};
