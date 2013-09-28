#if !defined(AFX_DLGOUTDESCOPT_H__3562F5F9_4F8E_4B4E_B2B8_A9433703151E__INCLUDED_)
#define AFX_DLGOUTDESCOPT_H__3562F5F9_4F8E_4B4E_B2B8_A9433703151E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgOutDescOpt.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgOutDescOption dialog

class CDlgOutDescOption : public CDialog
{
// Construction
public:
	CDlgOutDescOption(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgOutDescOption)
	enum { IDD = IDD_USER_OUTOPTION };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgOutDescOption)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgOutDescOption)
	afx_msg void OnCheckPhsmanual();
	afx_msg void OnCheckPhssetnum();
	afx_msg void OnCheckUsercustom();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckPhspicture();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGOUTDESCOPT_H__3562F5F9_4F8E_4B4E_B2B8_A9433703151E__INCLUDED_)
