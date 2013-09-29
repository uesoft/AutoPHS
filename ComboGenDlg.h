#if !defined(AFX_COMBOGENDLG_H__D6AC19A9_FC9D_4467_8F87_86BA12E935D2__INCLUDED_)
#define AFX_COMBOGENDLG_H__D6AC19A9_FC9D_4467_8F87_86BA12E935D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ComboGenDlg.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CComboGenDlg dialog
#include "SelTemplate.h"

class CComboGenDlg : public CDialog
{
// Construction
public:
	short m_nRow;
	short m_nCol;
	void MoveComboBox();
	void UpdateValue(BOOL flag);
	BOOL m_bIsVisible;
	CString m_strFilter;
	

	void SetVisibleState(BOOL flag);
	void LoadComboBox(int i);//i=1时，装根部;i=2时，装管部;

public:
	CComboGenDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CComboGenDlg)
	enum { IDD = IDD_COMBOGEN_DLG };
	CComboBox	m_ComboBoxGen;
	CString	m_ComboBoxGenVal;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComboGenDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CComboGenDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMBOGENDLG_H__D6AC19A9_FC9D_4467_8F87_86BA12E935D2__INCLUDED_)
