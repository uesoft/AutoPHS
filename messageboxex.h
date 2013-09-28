#if !defined(AFX_MESSAGEBOXEX_H__48ACC903_B9B5_11D5_AE98_00D00961973B__INCLUDED_)
#define AFX_MESSAGEBOXEX_H__48ACC903_B9B5_11D5_AE98_00D00961973B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MessageBoxEx.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMessageBoxEx dialog

class CMessageBoxEx : public CDialog
{
// Construction
public:
	CString m_strTitle;
	CMessageBoxEx(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMessageBoxEx)
	enum { IDD = IDD_MESSAGE_BOX };
	CString	m_strPrompt;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMessageBoxEx)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMessageBoxEx)
	afx_msg void OnAllYes();
	afx_msg void OnAllNo();
	virtual BOOL OnInitDialog();
	afx_msg void OnYes();
	afx_msg void OnNo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MESSAGEBOXEX_H__48ACC903_B9B5_11D5_AE98_00D00961973B__INCLUDED_)
