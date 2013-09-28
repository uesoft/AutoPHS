#if !defined(AFX_FRMLISTBOX_H__7E57CA00_76AD_11D5_8752_00D009619765__INCLUDED_)
#define AFX_FRMLISTBOX_H__7E57CA00_76AD_11D5_8752_00D009619765__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FrmListBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFrmListBox dialog
#include "DlgListBox.h"
class CFrmListBox : public CDialog
{
// Construction
private:
	int tmpPreviousIndex;
	enum ChkConditionIndex{
   iChkBoxPJG=0,
   iChkBoxTJ=1,
   iChkBoxGDW1=2,
   iChkBoxCNT=3,
   iChkBoxDW=4,
   iChkBoxOnlyPAh=5 //只选择吊架管部
	};
public:
	void InitDlg();
	int m_OldIxEDIT;
	int m_OldIxZDG;
	bool mvbBmpshow;
	long iChkCondition;
	void CalChkCondition();
	void ListZDGInit();
	void Bmpshow();
	long m_OldSelSampleID;
	CFrmListBox(CWnd* pParent = NULL);   // standard constructor
	
// Dialog Data
	BOOL    CheckCondition[6];
	UINT	CheckID[6];

	//{{AFX_DATA(CFrmListBox)
	enum { IDD = IDD_LISTBOX };
	CButton	m_CheckPA;
	CDlgListBox	m_ListZDG;
	CDlgListBox	m_ListPhsStruEDIT;
	BOOL	m_bBmpShow;
	BOOL	m_bPA;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFrmListBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool m_bRefImg;

	// Generated message map functions
	//{{AFX_MSG(CFrmListBox)
	afx_msg 	void OnActivate(UINT nState, CWnd *pWndOther, BOOL bMinimized);
	virtual BOOL OnInitDialog();
	afx_msg void OnCmdAdd();
	afx_msg void OnCmdApply();
	afx_msg void OnCmdDel();
	afx_msg void OnCmdSave();
	afx_msg void OnSelchangeList1();
	afx_msg void OnSelchangeList2();
	afx_msg void OnCheckPA();
	afx_msg void OnCheckBmp();
	afx_msg void OnButton3();
	afx_msg void OnDblclkListZDG();
	afx_msg void OnClose();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnCheck6();
	afx_msg void OnCheckOnlypah();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
extern CFrmListBox FrmListBox;
extern bool bLockAct;
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRMLISTBOX_H__7E57CA00_76AD_11D5_8752_00D009619765__INCLUDED_)
