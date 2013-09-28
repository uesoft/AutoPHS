//{{AFX_INCLUDES()
#include "msmask.h"
//}}AFX_INCLUDES
#if !defined(AFX_FRMTK_H__203874E3_AB4D_11D5_AE95_00D00961973B__INCLUDED_)
#define AFX_FRMTK_H__203874E3_AB4D_11D5_AE95_00D00961973B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FrmTK.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFrmTK dialog

class CFrmTK : public CDialog
{
// Construction
public:
	void LoadItem2TKsize();
	CFrmTK(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFrmTK)
	enum { IDD = IDD_TK };
	CSpinButtonCtrl	m_Spin1;
	CComboBox	m_ComboTkSize;
	BOOL	m_bSigned;
	BOOL	m_bOSflag;
	BOOL	m_bSubzone;
	CString	m_strTkExtRatio;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFrmTK)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFrmTK)
	virtual BOOL OnInitDialog();
	afx_msg void OnSigned();
	afx_msg void OnOsflag();
	afx_msg void OnSubzone();
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnDestroy();
	afx_msg void OnSelchangeCombo1();
	afx_msg void OnChangeTkextratio();
	afx_msg void OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRMTK_H__203874E3_AB4D_11D5_AE95_00D00961973B__INCLUDED_)
