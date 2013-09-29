#if !defined(AFX_SELPSAPROJECTNAMEDLG_H__161AC98E_A720_43C2_B2FC_B579AA1B770C__INCLUDED_)
#define AFX_SELPSAPROJECTNAMEDLG_H__161AC98E_A720_43C2_B2FC_B579AA1B770C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelPSAProjectNameDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelPSAProjectNameDlg dialog
#include "resource.h"
class CSelPSAProjectNameDlg : public CDialog
{
// Construction
public:
	_RecordsetPtr m_pRsJOBNAME;//pfg20050705所有工程的工况,注意在使用时要过滤
	CString m_ForceUnits;
	CString m_strTempLGK;
	CString m_strTempRGK;
	int iUpxyz;					//力的方向
	CString* m_pstrGKName;	//工况名
	CString* m_pstrProjectName;//工程文件名
	int GKNum;					//工况数
	int JobNameNum;			//文件名数
	CString m_strTempProject;
	CString m_strTempGK;
	CSelPSAProjectNameDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelPSAProjectNameDlg)
	enum { IDD = IDD_DIALOG_PASPROJECT };
	CListBox	m_listLGKname;
	CListBox	m_listRGKname;
	CListBox	m_listGKname;
	CButton	m_check;
	CListBox	m_listPSAprojectname;
//	BOOL	m_bCheckRigPHSMaxLoad;
	int		m_iSpringLoadType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelPSAProjectNameDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelPSAProjectNameDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnX();
	afx_msg void OnY();
	afx_msg void OnZ();
	afx_msg void OnCancle();
	afx_msg void OnRADIOkgf();
	afx_msg void OnRadioN();
	afx_msg void OnSelchangeListProjectname();
	afx_msg void OnDrowUnitsBUTTON();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int iR;
	int iL;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELPSAPROJECTNAMEDLG_H__161AC98E_A720_43C2_B2FC_B579AA1B770C__INCLUDED_)
