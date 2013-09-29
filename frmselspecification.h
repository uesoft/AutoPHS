#if !defined(AFX_FRMSELSPECIFICATION_H__939A3680_7213_11D5_8752_00D009619765__INCLUDED_)
#define AFX_FRMSELSPECIFICATION_H__939A3680_7213_11D5_8752_00D009619765__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FrmSelSpecification.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFrmSelSpecification dialog
#include "FrmListBox.h"
#include "XListBox.h"

class CFrmSelSpecification : public CDialog
{
// Construction
public:
	CString m_tableName;
	void SetUseCount();
	BOOL m_bShowExitMenuItem;
	bool m_bSPECchanged;	//指示是否规范中已经导入新的数据,
	void Option1_Click(int Index);
	CFrmSelSpecification(BOOL bShowExitMenuItem=TRUE,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	CMenu m_mnu;
	//{{AFX_DATA(CFrmSelSpecification)
	enum { IDD = IDD_SELSPE };
	CListBox	m_List2;
	CXListBox	m_List1;
	CStatic	m_CLab2;
	CString	m_Label2;
	int m_iSelIndex;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFrmSelSpecification)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString m_aSpec[6];

	// Generated message map functions
	//{{AFX_MSG(CFrmSelSpecification)
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnRadio3();
	afx_msg void OnRadio4();
	afx_msg void OnRadio5();
	afx_msg void OnRadio6();
	afx_msg void OnSelchangeList2();
	afx_msg void OnSelchangeList1();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnImpSpec();
	afx_msg void OnSelexit();
	virtual void OnOK();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnDroprecord();
	afx_msg void OnOpenphstable();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
enum Option1Index{
   optIPA=IDC_RADIO1,
   optIPART=IDC_RADIO2,
   optISA=IDC_RADIO3,
   optISPRING=IDC_RADIO4,
   optICSPRING=IDC_RADIO5,
   optIBoltsNuts=IDC_RADIO6
};
//extern CFrmSelSpecification FrmSelSpecification;
extern int inline ShowModalSelSpecification(BOOL bShowExitMenuItem=TRUE);
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRMSELSPECIFICATION_H__939A3680_7213_11D5_8752_00D009619765__INCLUDED_)
