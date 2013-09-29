#if !defined(AFX_DLGSARECORD_H__705AC9A4_F922_11D5_AE99_00D00961973B__INCLUDED_)
#define AFX_DLGSARECORD_H__705AC9A4_F922_11D5_AE99_00D00961973B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSARecord.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSARecord dialog
#include "MyListCtrl.h"
class CDlgSARecord : public CDialog
{
// Construction
public:
	void InitLab();
	void RefData();
	void LoadList();
	CDlgSARecord(_RecordsetPtr pRs,BOOL bAddNew=FALSE,int nBH=1,CString strCustomID="G100",CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSARecord)
	enum { IDD = IDD_DLGSARECORD_DIALOG };
	CMyListCtrl	m_lstMaterial;
	CString	m_strName;
	CString	m_strNum;
	CString	m_strLength;
	CString	m_strCustomID;
	float	m_fPMax;
	CString	m_strSIZEH;
	CString	m_strSIZE2;
	CString	m_strM;
	CString	m_strL1;
	CString	m_strCHDIST;
	CString	m_strGDW1;
	CString	m_strC;
	CString	m_strA;
	CString	m_strTJ;
	CString	m_strPMAXSS100;
	CString	m_strPMAXSS150;
	CString	m_strPMAXSF;
	CString	m_strWEIGTH;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSARecord)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int m_nDH;
	BOOL m_bAddNew;
	int m_nNode;
	void ClearEditData();
	void RefEditData();
	_RecordsetPtr m_pRs;

	// Generated message map functions
	//{{AFX_MSG(CDlgSARecord)
	virtual BOOL OnInitDialog();
	afx_msg void OnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAddMaterial();
	virtual void OnOK();
	afx_msg void OnBtnRemove();
	afx_msg void OnChangeEditNum();
	afx_msg void OnChangeEditLen();
	afx_msg void OnEndlabeleditList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeEdit1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSARECORD_H__705AC9A4_F922_11D5_AE99_00D00961973B__INCLUDED_)
