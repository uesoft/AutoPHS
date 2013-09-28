#if !defined(AFX_NEWSTYLEDLG_H__3AFD6A98_D33E_48A7_90D7_2CB8B567613B__INCLUDED_)
#define AFX_NEWSTYLEDLG_H__3AFD6A98_D33E_48A7_90D7_2CB8B567613B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewStyleDlg.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CNewStyleDlg dialog

class CNewStyleDlg : public CDialog
{
// Construction
public:
	CNewStyleDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewStyleDlg)
	enum { IDD = IDD_DIALOG_NEWSTYLE };
	CString	m_strStyleName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewStyleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewStyleDlg)
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWSTYLEDLG_H__3AFD6A98_D33E_48A7_90D7_2CB8B567613B__INCLUDED_)
