#if !defined(AFX_DLGSPAN_H__5FA3FC82_035E_11D6_BBA7_00D009619765__INCLUDED_)
#define AFX_DLGSPAN_H__5FA3FC82_035E_11D6_BBA7_00D009619765__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSpan.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSpan dialog
#include "MyListCtrl.h"
class CDlgSpan : public CDialog
{
// Construction
public:
	int m_iMediaItemIndex;
	int m_iMaterialItemIndex;
	int m_iDWItemIndex;
	CDlgSpan(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSpan)
	enum { IDD = IDD_DIALOG_SPAN };
	CComboBox	m_comboTemp;
	CListCtrl	m_lstMedia;
	CListCtrl	m_lstMaterial;
	CMyListCtrl	m_lstDW;
	CString	m_strLMaxR;
	CString	m_strLMaxS;
	CString	m_strMD;
	CString	m_strIsoWeight;
	CString	m_strQ;
	CString	m_strTemp;
	CString	m_strLMaxRNew;
	CString	m_strLMaxSNew;
	CString	m_strQp;
	BOOL	m_bHighCrSteel;
	BOOL	m_bPG40;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSpan)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void LoadTemp();
	CFont m_outFont;
	int m_iMaterialHotIndex;
	int m_iMediaHotIndex;
	int m_iDWHotIndex;
	void Cal();
	void FreeListMediaData();
	void LoadListMaterial();
	void LoadListMedia();
	void LoadListDW();

	// Generated message map functions
	//{{AFX_MSG(CDlgSpan)
	afx_msg void OnHottrackListMedia(NMHDR* pNMHDR,LRESULT* pResult);
	afx_msg void OnHottrackListMaterial(NMHDR* pNMHDR,LRESULT* pResult);
	afx_msg void OnHottrackListDw(NMHDR* pNMHDR,LRESULT* pResult);
	afx_msg void OnActivate(UINT nState, CWnd *pWndOther, BOOL bMinimized);
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnItemchangedListDw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedListMaterial(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedListMedia(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeEditMediadensity();
	afx_msg void OnChangeEditIsoweight();
	afx_msg void OnEditchangeComboTemp();
	afx_msg void OnSelchangeComboTemp();
	afx_msg void OnCheckHighCr();
	afx_msg void OnCheckPg40();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
extern CDlgSpan gDlgSpan;
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSPAN_H__5FA3FC82_035E_11D6_BBA7_00D009619765__INCLUDED_)
