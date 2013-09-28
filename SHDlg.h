#if !defined(AFX_SHDLG_H__1F6E9E10_7FF8_4DAB_BD0E_DFDC1018A71C__INCLUDED_)
#define AFX_SHDLG_H__1F6E9E10_7FF8_4DAB_BD0E_DFDC1018A71C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SHDlg.h : header file
//
#include "columns.h"
#include "column.h"
#include <vector>
using std::vector;
typedef vector<BOOL> store;

/////////////////////////////////////////////////////////////////////////////
// CSHDlg dialog

class CSHDlg : public CDialog
{
// Construction
public:
	CCheckListBox m_CheckListBox;
	store bIsVisible;
	void AddStrings();
private:
	void StrToInt(CString str, short& result);
public:
	CSHDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSHDlg)
	enum { IDD = IDD_DLG_SHOWHIDE_COLUMNS };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSHDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSHDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnDisplay();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHDLG_H__1F6E9E10_7FF8_4DAB_BD0E_DFDC1018A71C__INCLUDED_)
