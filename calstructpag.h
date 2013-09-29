//{{AFX_INCLUDES()
#include "datagrid.h"
//}}AFX_INCLUDES
#if !defined(AFX_CALSTRUCTPAG_H__8CCE7B84_0456_4355_B155_EB8D744BFF00__INCLUDED_)
#define AFX_CALSTRUCTPAG_H__8CCE7B84_0456_4355_B155_EB8D744BFF00__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CalStructPag.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCalStructPag dialog

class CCalStructPag : public CDialog
{
// Construction
	friend class CCalStructDlg;
public:
	int m_iComNo;
	CCalStructPag(_ConnectionPtr connSort,long lStructID,long lComponentID,CString strResult,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCalStructPag)
	enum { IDD = IDD_PAGE_CALSTR };
	CString	m_strFormula;
	CString	m_strWhere;
	CString	m_strCaption;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCalStructPag)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	CBrush m_brBk;
	CString m_strResult;
	long m_lComponentID;
	long m_lStructID;
	_ConnectionPtr m_connSort;

	// Generated message map functions
	//{{AFX_MSG(CCalStructPag)
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CALSTRUCTPAG_H__8CCE7B84_0456_4355_B155_EB8D744BFF00__INCLUDED_)
