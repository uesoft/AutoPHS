#if !defined(AFX_XEDIT_H__1E0570A9_7214_4CFE_BD2A_EEEE95815B6C__INCLUDED_)
#define AFX_XEDIT_H__1E0570A9_7214_4CFE_BD2A_EEEE95815B6C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// XEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CXEdit window

class CXEdit : public CEdit
{
// Construction
public:
	CXEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetBkColor(COLORREF color);
	void SetTextColor(COLORREF color);
	virtual ~CXEdit();

	// Generated message map functions
protected:
	COLORREF m_crBk;
	COLORREF m_crText;
	CBrush m_brBk;
	//{{AFX_MSG(CXEdit)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XEDIT_H__1E0570A9_7214_4CFE_BD2A_EEEE95815B6C__INCLUDED_)
