#if !defined(AFX_TXSRPAGITEM_H__C3BDD721_00FE_11D6_BBA7_00D009619765__INCLUDED_)
#define AFX_TXSRPAGITEM_H__C3BDD721_00FE_11D6_BBA7_00D009619765__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TxsrPagItem.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTxsrPagItem dialog

class CTxsrPagItem : public CDialog
{

// Construction
public:
	CTxsrPagItem(CWnd* pParentWnd = NULL );
	~CTxsrPagItem();

// Dialog Data
	//{{AFX_DATA(CTxsrPagItem)
	enum { IDD = IDD_TXSR_PAG_ITEM };
	CString	m_PSAver;
	CString	m_SPRrigid;
	CString	m_SPRWorks;
	CString	m_conSPRworks;
	CString	m_LblPrjName;
	CString	m_txtCLB1;
	CString	m_ConstantSPRNo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTxsrPagItem)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTxsrPagItem)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnChangeEDITDwgName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TXSRPAGITEM_H__C3BDD721_00FE_11D6_BBA7_00D009619765__INCLUDED_)
