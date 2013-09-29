//{{AFX_INCLUDES()
#include "datagrid.h"
#include "adodc.h"
//}}AFX_INCLUDES
#if !defined(AFX_DLGEDITDB_H__380142C1_0CBC_11D6_BBA7_00D009619765__INCLUDED_)
#define AFX_DLGEDITDB_H__380142C1_0CBC_11D6_BBA7_00D009619765__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgEditDB.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgEditDB dialog

class CDlgEditDB : public CDialog
{
// Construction
public:
	CString m_strPassword;
	BOOL ListTableName();
	void OpenTable();
	CString m_strSQL;
	CString m_strDBName;
	CDlgEditDB(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgEditDB)
	enum { IDD = IDD_DLGEDITDB_DIALOG };
	CListBox	m_lstTableName;
	CDataGrid	m_dbGrid;
	CAdodc	m_Adodc;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgEditDB)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgEditDB)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeList1();
	afx_msg void OnMoveCompleteAdodc1(long adReason, LPDISPATCH pError, long FAR* adStatus, LPDISPATCH pRecordset);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEDITDB_H__380142C1_0CBC_11D6_BBA7_00D009619765__INCLUDED_)
