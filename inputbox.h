#if !defined(AFX_INPUTBOX_H__FDE3E3C3_A75F_11D5_AE95_00D00961973B__INCLUDED_)
#define AFX_INPUTBOX_H__FDE3E3C3_A75F_11D5_AE95_00D00961973B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InputBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInputBox dialog
#include "resource.h"

class CInputBox : public CDialog
{
// Construction
public:
	BOOL m_bIsProtected;
	CString m_strWndText;
	CInputBox(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInputBox)
	enum { IDD = IDD_INPUT_BOX };
	CString	m_strValue;
	CString	m_strTitle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInputBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInputBox)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPUTBOX_H__FDE3E3C3_A75F_11D5_AE95_00D00961973B__INCLUDED_)
