#if !defined(AFX_TXSRPAG1_H__4F214919_6971_11D5_8752_00D009619765__INCLUDED_)
#define AFX_TXSRPAG1_H__4F214919_6971_11D5_8752_00D009619765__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TxsrPag1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTxsrPag1 dialog
class CTxsrPag1 : public CDialog
{
// Construction
public:
	bool m_bCustomDesc;
//	bool m_bSysAndUser;

	bool m_bOnlyUser;
	bool m_bOnlySys;
	CTxsrPag1(CWnd* pParentWnd = NULL );   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTxsrPag1)
	enum { IDD = IDD_TXSR_PAG1 };
	CButton	m_btnCustomDesc;
	CButton	m_checkCustomDesc;
	CButton	m_btnOnlyUser;
	CButton	m_btnOnlySys;
	CEdit	m_editUserDesc;
	float	m_fHAZ;
	float	m_fPAZ1;
	float	m_fPAZ0;
	float	m_fPGZ1;
	float	m_fPJG1;
	CString	m_labPaz;
	CString	m_labPaz0;
	CString	m_labPgz;
	CString	m_labPjg;
	CString	m_strUserDesc;
	BOOL	m_bsys;
	BOOL	m_buser;
	BOOL	m_bcustom;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTxsrPag1)
	public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTxsrPag1)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnCheckOnlysys();
	afx_msg void OnCheckOnlyuser();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditUserdesc();
	afx_msg void OnKillfocusEditUserdesc();
	afx_msg void OnBtnUserOutoption();
	afx_msg void OnCheckCustomdesc();
	afx_msg LRESULT OnChangeMessage(WPARAM wParam,LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TXSRPAG1_H__4F214919_6971_11D5_8752_00D009619765__INCLUDED_)
