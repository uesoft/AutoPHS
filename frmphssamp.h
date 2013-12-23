#if !defined(AFX_FRMPHSSAMP_H__7E57CA05_76AD_11D5_8752_00D009619765__INCLUDED_)
#define AFX_FRMPHSSAMP_H__7E57CA05_76AD_11D5_8752_00D009619765__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FrmPhsSamp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFrmPhsSamp dialog

enum mnuDwgAndBmpIndex{
   mnuiPALimited=0,
   mnuiSALimited,
   mnuiPASALimited,
   mnuIAvailableSamp,
   mnuIALLSamp,
   mnuIALLSPRSamp,
   mnuIALLCSPRSamp,
   mnuIALLNotSPRCSPRSamp,
   mnuISep,
   mnuICHECKALLindex,
   mnuIEDITName,
   mnuICheckID
};
#include "DlgListBox.h"
class CFrmPhsSamp : public CDialog
{
// Construction
private:
	enum OptIndex{
   optSortIndex = IDC_RADIO1,
   optSortFREQ=IDC_RADIO2,
   optSortiNumOfPart=IDC_RADIO3,
   optSortName=IDC_RADIO4,
   optSortPAID=IDC_RADIO5
};
enum mnuSMIndex{
   mnuIGETACADImage=0,
   mnuIGETSampleBmp,
   mnuISaveSampleBMP
};
public:
	virtual  ~CFrmPhsSamp();
	CString SortFieldName;
	void LoadListSelPhs();
	CString sBmpName;
	void Data1_Reposition();
	CMenu m_popMenu;
	void mnuDWGM_Click(int Index);
	int SelectItemOpt;
	int SelectItemMenu;

	CDaoRecordset* Data1;
	CDaoRecordset Data2;
	CFrmPhsSamp(CWnd* pParent = NULL);   // standard constructor
	CString strOrderByDesc;
// Dialog Data
	//{{AFX_DATA(CFrmPhsSamp)
	enum { IDD = IDD_PHSSAMP };
	CComboBox	m_comboSA;
	CComboBox	m_comboPA;
	CButton	m_FrameSelPhs;
	CDlgListBox	m_ListSelPhs;
	BOOL	m_bOrderByDesc;
	CString	m_nRodNum;
	CString	m_nSprNum;
	BOOL	m_bFilter;
	BOOL	m_bSh;
	BOOL	m_bSABEAM;
	BOOL	m_bSACANTil;
	BOOL	m_bSALBRACE;
	BOOL	m_bDH;
	CString	m_nCSPRNum;
	BOOL	m_bCurrentSA;
	BOOL	m_bCurrentPA;
	BOOL	m_bSAGCEMENT;
	BOOL	m_bCurrentZdjhPA;
	BOOL	m_bCurrentZdjhSA;
	BOOL	m_bCurrentZdjhAvPA;
	BOOL	m_bCurrentZdjhAvSA;
	BOOL	m_bAutoSprNum;
	//}}AFX_DATA
	long * m_ListID;
	long m_ListIDCount;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFrmPhsSamp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	bool m_bIsSelSA;
	bool m_bIsSelPA;
	bool m_bLoadSA;
	bool m_bLoadPA;
	CString * m_strListPA;
	void LoadListSA();
	void LoadListPA();
	void UpdateRecordset();
	void GetFilterStr();
	CString m_strFilter;

	// Generated message map functions
	//{{AFX_MSG(CFrmPhsSamp)
	afx_msg 	void OnActivate(UINT nState, CWnd *pWndOther, BOOL bMinimized);
	virtual BOOL OnInitDialog();
	afx_msg void OnOptSortIndex();
	afx_msg void OnOleSize();
	afx_msg void OnOptSortFREQ();
	afx_msg void OnOptSortiNumOfPart();
	afx_msg void OnOptSortName();
	afx_msg void OnOptSortPAID();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnByDesc();
	afx_msg void OnmnuiPALimited();
	afx_msg void OnmnuiSALimited();
	afx_msg void OnmnuiPASALimited();
	afx_msg void OnmnuIAvailableSamp();
	afx_msg void OnmnuIALLSamp();
	afx_msg void OnmnuIALLSPRSamp();
	afx_msg void OnmnuIALLCSPRSamp();
	afx_msg void OnmnuIALLNotSPRCSPRSamp();
	afx_msg void OnmnuICheckID();
	afx_msg void OnmnuICHECKALLindex();
	afx_msg void OnmnuIEDITName();
	afx_msg void OnFrmSel();
	afx_msg void OnSelChangeListSelPhs();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClose();
	afx_msg void OnCheckFilter();
	afx_msg void OnSelchangeComboSprnum();
	afx_msg void OnSelchangeComboCsprnum();
	afx_msg void OnCheckCurrentPA();
	afx_msg void OnCheckCurrentSA();
	afx_msg void OnCheckCurrentZdjhPA();
	afx_msg void OnCheckCurrentZdjhSA();
	afx_msg void OnCheckCurrentZdjhAvPA();
	afx_msg void OnCheckCurrentZdjhAvSA();
	afx_msg void OnCheckAutosprnum();
	afx_msg void OnDropdownComboPa();
	afx_msg void OnDropdownComboSa();
	afx_msg void OnSelchangeComboPa();
	afx_msg void OnSelchangeComboSa();
	afx_msg void OnCheckDh();
	afx_msg void OnCheckSh();
	afx_msg void OnCheckSabeam();
	afx_msg void OnCheckSacant();
	afx_msg void OnCheckSalbrace();
	afx_msg void OnCheckSagcement();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
extern CFrmPhsSamp FrmPhsSamp;
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRMPHSSAMP_H__7E57CA05_76AD_11D5_8752_00D009619765__INCLUDED_)
