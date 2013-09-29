#if !defined(AFX_TAB_H__4AE108D3_46ED_4728_9C3F_39797C8E6E7E__INCLUDED_)
#define AFX_TAB_H__4AE108D3_46ED_4728_9C3F_39797C8E6E7E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Tab.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTab window

class CTab : public CTabCtrl
{
// Construction
public:
	CTab();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTab)
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL SetActiveWnd(CWnd *pWnd);
	BOOL AddWnd(CWnd *pWnd, LPCTSTR lpszLabel);
	virtual ~CTab();
protected:
	HWND m_hActiveWnd;
	// Generated message map functions
protected:
	//{{AFX_MSG(CTab)
	afx_msg void OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TAB_H__4AE108D3_46ED_4728_9C3F_39797C8E6E7E__INCLUDED_)
