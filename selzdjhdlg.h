//{{AFX_INCLUDES()
#include "datagrid.h"
//}}AFX_INCLUDES
#if !defined(AFX_SELZDJHDLG_H__58F4B646_B1BF_11D5_AE95_00D00961973B__INCLUDED_)
#define AFX_SELZDJHDLG_H__58F4B646_B1BF_11D5_AE95_00D00961973B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelZdjhDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelZdjhDlg dialog

class CSelZdjhDlg : public CDialog
{
// Construction
public:
	_RecordsetPtr m_rs;
	long m_JSJDH;
	long m_zdjh;
	CSelZdjhDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelZdjhDlg)
	enum { IDD = IDD_SEL_ZDJH };
	CDataGrid	m_dbGrid;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelZdjhDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelZdjhDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELZDJHDLG_H__58F4B646_B1BF_11D5_AE95_00D00961973B__INCLUDED_)
