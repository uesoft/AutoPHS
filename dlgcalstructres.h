//{{AFX_INCLUDES()
#include "datagrid.h"
//}}AFX_INCLUDES
#if !defined(AFX_DLGCALSTRUCTRES_H__D8B0823C_E2AF_4F6E_86FF_F3E52983F204__INCLUDED_)
#define AFX_DLGCALSTRUCTRES_H__D8B0823C_E2AF_4F6E_86FF_F3E52983F204__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgCalStructRes.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgCalStructRes dialog

class CDlgCalStructRes : public CDialog
{
// Construction
public:
	CSize m_size;
	int m_igridRow;
	_RecordsetPtr m_rs1;
	_RecordsetPtr m_rs;
	_ConnectionPtr m_pConnPrj;
	CDlgCalStructRes(int iComCount,CString* pstrType,UINT * pnLen,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgCalStructRes)
	enum { IDD = IDD_DIALOG_CALSTRUCT_RESULT };
	CDataGrid	m_grid;
	CDataGrid	m_grid1;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgCalStructRes)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_bCreated;
	CString m_strMaterial;
	UINT *m_pnLen;
	CString * m_pstrType;
	int m_iComCount;

	// Generated message map functions
	//{{AFX_MSG(CDlgCalStructRes)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnRowColChangeDatagrid1(VARIANT FAR* LastRow, short LastCol);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCALSTRUCTRES_H__D8B0823C_E2AF_4F6E_86FF_F3E52983F204__INCLUDED_)
