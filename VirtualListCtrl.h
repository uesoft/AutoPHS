#if !defined(AFX_VIRTUALLISTCTRL_H__2912026C_CCEC_42EB_8056_A4E616BDD197__INCLUDED_)
#define AFX_VIRTUALLISTCTRL_H__2912026C_CCEC_42EB_8056_A4E616BDD197__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VirtualListCtrl.h : header file
//
#include "MyListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CVirtualListCtrl window

//该类专为支吊架组装模板管理对话框中的ListCtrl控件设计，
//

class CVirtualListCtrl : public CMyListCtrl
{
// Construction
public:
	CVirtualListCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVirtualListCtrl)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	DWORD GetItemData( int nItem ) const;


	_RecordsetPtr  m_prsNAME;
	virtual ~CVirtualListCtrl();

	// Generated message map functions
protected:
	_RecordsetPtr m_prsREF;
	long m_lRefRecords;
	//{{AFX_MSG(CVirtualListCtrl)
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIRTUALLISTCTRL_H__2912026C_CCEC_42EB_8056_A4E616BDD197__INCLUDED_)
