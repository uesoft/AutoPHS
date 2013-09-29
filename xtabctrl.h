#if !defined(AFX_XTABCTRL_H__F5C734C2_1AB8_441A_B25D_393A40FF59D5__INCLUDED_)
#define AFX_XTABCTRL_H__F5C734C2_1AB8_441A_B25D_393A40FF59D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// XTabCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CXTabCtrl window

class CXTabCtrl : public CTabCtrl
{
// Construction
public:
	CXTabCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXTabCtrl)
	public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL RemoveWnd(int index,BOOL bDestroyWindow=TRUE);
	BOOL RemoveWnd(CWnd* pWnd,BOOL bDestroyWindow=TRUE);
	BOOL SetActiveWnd(CWnd* pWnd);
	CWnd* GetActiveWnd();
	BOOL AddWnd(CWnd* pWnd,LPCTSTR lpszLabel=NULL);
	virtual ~CXTabCtrl();
protected:
	HWND m_hActiveWnd;


	// Generated message map functions
protected:
	//{{AFX_MSG(CXTabCtrl)
	afx_msg void OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XTABCTRL_H__F5C734C2_1AB8_441A_B25D_393A40FF59D5__INCLUDED_)
