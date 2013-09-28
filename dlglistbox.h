#if !defined(AFX_DLGLISTBOX_H__853EA1C1_7CEF_11D5_8752_00D009619765__INCLUDED_)
#define AFX_DLGLISTBOX_H__853EA1C1_7CEF_11D5_8752_00D009619765__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgListBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgListBox window

class CDlgListBox : public CListBox
{
// Construction
public:
	CDlgListBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgListBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	HBRUSH m_bkHbr;
//	CBrush m_sbkBr;
	void SetBkColor(DWORD color);
	int GetText(int ix,LPTSTR lpszBuffer);
	void GetText(int ix,CString& rString);
	virtual ~CDlgListBox();

	// Generated message map functions
protected:
	DWORD m_bkColor;
	CBrush m_bkBr;
	//{{AFX_MSG(CDlgListBox)
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGLISTBOX_H__853EA1C1_7CEF_11D5_8752_00D009619765__INCLUDED_)
