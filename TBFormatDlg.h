//{{AFX_INCLUDES()
#include "datagrid.h"
//}}AFX_INCLUDES
#if !defined(AFX_TBFORMATDLG_H__C8F8D38B_7EAC_42E5_B692_9F39634141C8__INCLUDED_)
#define AFX_TBFORMATDLG_H__C8F8D38B_7EAC_42E5_B692_9F39634141C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TBFormatDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTBFormatDlg dialog

class CTBFormatDlg : public CDialog
{
// Construction
public:
	CTBFormatDlg(CWnd* pParent = NULL);   // standard constructor
public:
	_ConnectionPtr m_connSort;
	_RecordsetPtr m_rs;
// Dialog Data
	//{{AFX_DATA(CTBFormatDlg)
	enum { IDD = IDD_TB_FORMAT };
	CDataGrid	m_DataGrid;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTBFormatDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTBFormatDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TBFORMATDLG_H__C8F8D38B_7EAC_42E5_B692_9F39634141C8__INCLUDED_)
