#if !defined(AFX_TXSRPAG2_H__4F21491A_6971_11D5_8752_00D009619765__INCLUDED_)
#define AFX_TXSRPAG2_H__4F21491A_6971_11D5_8752_00D009619765__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TxsrPag2.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTxsrPag2 dialog

class CTxsrPag2 : public CDialog
{
// Construction
public:
	CTxsrPag2(CWnd* pParentWnd = NULL );   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTxsrPag2)
	enum { IDD = IDD_TXSR_PAG2 };
	float	m_fDIlineDispX;
	float	m_fDIlineDispY;
	float	m_fDIlineDispZ;
	float	m_fDIFx;
	float	m_fDIFz;
	float	m_fDIFy;
	float	m_fDImX;
	float	m_fDImY;
	float	m_fDImZ;
	float	m_fDIRotDispX;
	float	m_fDIRotDispY;
	float	m_fDIRotDispZ;
	CString	m_labDx;
	CString	m_labDy;
	CString	m_labDz;
	CString	m_labFx;
	CString	m_labFy;
	CString	m_labFz;
	CString	m_labMx;
	CString	m_labMy;
	CString	m_labMz;	
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTxsrPag2)
	public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTxsrPag2)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TXSRPAG2_H__4F21491A_6971_11D5_8752_00D009619765__INCLUDED_)
