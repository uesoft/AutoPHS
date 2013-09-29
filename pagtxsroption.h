#if !defined(AFX_PAGTXSROPTION_H__4555CD42_FDFA_11D5_AE99_00D00961973B__INCLUDED_)
#define AFX_PAGTXSROPTION_H__4555CD42_FDFA_11D5_AE99_00D00961973B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PagTxsrOption.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPagTxsrOption dialog

class CPagTxsrOption : public CDialog
{

// Construction
public:
	void LoadSAG100();
	CPagTxsrOption(CWnd* pParentWnd = NULL );
	~CPagTxsrOption();

// Dialog Data
	//{{AFX_DATA(CPagTxsrOption)
	enum { IDD = IDD_TXSR_PAG_OPTION };
	CComboBox	m_comboSAG100;
	CButton	m_CmdSAnum;
	CComboBox	m_comboDCL1;
	int		m_iNum;
	CString	m_csDCL1;
	BOOL	m_bifLongVertPipe;
	BOOL	m_bNotSumSA;
	float	m_fPSLength;
	float	m_fPSThickness;
	float	m_fPSWidth;
	CString	m_csSAG100No;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPagTxsrOption)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPagTxsrOption)
	afx_msg void OnUnitKgf();
	afx_msg void OnUnitN();
	afx_msg void OnPzG();
	afx_msg void OnPzD();
	afx_msg void OnBtnSanum();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGTXSROPTION_H__4555CD42_FDFA_11D5_AE99_00D00961973B__INCLUDED_)
