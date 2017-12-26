#pragma once
#include "afxwin.h"
class VideoStatic :
	public CStatic
{
public:
	VideoStatic(CWnd* dlg, int nIndex);
	~VideoStatic();
	
//	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	void setBackColor();
	DECLARE_MESSAGE_MAP()
protected:
	COLORREF m_crBackColor;
	CBrush   m_Brush;

	HWND m_hParent;
	int m_nIndex;
	BOOL m_blSelect;

public:
	afx_msg void OnStnClicked();

//	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
//	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL IsInvokeAllowed(DISPID /*dispid*/);
};

