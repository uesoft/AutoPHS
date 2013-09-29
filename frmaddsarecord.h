//{{AFX_INCLUDES()
#include "datagrid.h"
#include "adodc.h"
//}}AFX_INCLUDES
#if !defined(AFX_FRMADDSARECORD1_H__02364749_81AB_11D5_8752_00D009619765__INCLUDED_)
#define AFX_FRMADDSARECORD1_H__02364749_81AB_11D5_8752_00D009619765__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FrmAddSArecord1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFrmAddSArecord dialog
#include "DlgListBox.h"
class CFrmAddSArecord : public CDialog
{
// Construction
public:
	CFrmAddSArecord(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFrmAddSArecord)
	enum { IDD = IDD_ADDSARECORD };
	CDlgListBox	m_List1;
	CDataGrid	m_DBGrid1;
	CAdodc	m_Data1;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFrmAddSArecord)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
public:
	void LoadList();
	CRect m_oldRect;
	void RefWndRect();
//	_RecordsetPtr m_Data1;
// Implementation
protected:
	CString m_strCustomID;

	// Generated message map functions
	//{{AFX_MSG(CFrmAddSArecord)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelchangeList1();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClose();
	afx_msg void OnBtnEdit();
	afx_msg void OnBtnAdd();
	afx_msg void OnBtnRemove();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRMADDSARECORD1_H__02364749_81AB_11D5_8752_00D009619765__INCLUDED_)
