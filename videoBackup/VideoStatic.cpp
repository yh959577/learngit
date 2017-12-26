#include "stdafx.h"
#include "VideoStatic.h"

VideoStatic::VideoStatic(CWnd* dlg, int nIndex)
{
	m_hParent = dlg->GetSafeHwnd();
	m_nIndex = nIndex;
	m_blSelect = FALSE;
}


VideoStatic::~VideoStatic()
{
}
BEGIN_MESSAGE_MAP(VideoStatic, CStatic)
//	ON_WM_CTLCOLOR_REFLECT()
	
	ON_CONTROL_REFLECT(STN_CLICKED, &VideoStatic::OnStnClicked)
	ON_WM_PAINT()
ON_WM_CTLCOLOR_REFLECT()
ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


//HBRUSH VideoStatic::CtlColor(CDC* pDC, UINT nCtlColor)
//{
//	// TODO:  在此更改 DC 的任何特性
//
//	// TODO:  如果不应调用父级的处理程序，则返回非 null 画笔
//
//	m_Brush.DeleteObject();
//	m_Brush.CreateSolidBrush(RGB(0,0,0));
//	pDC->SetBkColor(RGB(0, 0, 0));
//	pDC->SetTextColor(RGB(0, 0, 0));
//	TRACE(_T("coclorctr\n"));
//	
//	return (HBRUSH)m_Brush;
//}

void VideoStatic::setBackColor()
{
	//m_crBackColor = crBackColor;
	RedrawWindow();
	TRACE(_T("redrawWin\n"));
}


void VideoStatic::OnStnClicked()
{
	// TODO: 在此添加控件通知处理程序代码
	//AfxMessageBox(_T("this is xxx"));
	::SendMessage(m_hParent, WM_ON_CONTROL_CMD_MSG, STATIC_SELECT_MSG, (LPARAM)m_nIndex);
}


HBRUSH VideoStatic::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	// TODO:  在此更改 DC 的任何特性

	// TODO:  如果不应调用父级的处理程序，则返回非 null 画笔

	m_Brush.DeleteObject();
		m_Brush.CreateSolidBrush(RGB(0,0,0));
		pDC->SetBkColor(RGB(0, 0, 0));
		pDC->SetTextColor(RGB(0, 0, 0));

		TRACE(_T("coclorctr\n"));
	return m_Brush;
}


BOOL VideoStatic::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	TRACE(_T("eraseBk\n"));
	//return CStatic::OnEraseBkgnd(pDC);
	return true;
}


BOOL VideoStatic::IsInvokeAllowed(DISPID /*dispid*/)
{
	// TODO: 在此添加专用代码和/或调用基类

	return TRUE;
}
