#if !defined(AFX_CHANGMATERILNAME_H__8EC3358E_C38D_4FF5_AE6E_D8C3C10F5244__INCLUDED_)
#define AFX_CHANGMATERILNAME_H__8EC3358E_C38D_4FF5_AE6E_D8C3C10F5244__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChangMaterilName.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChangMaterilName dialog

class CChangMaterilName : public CDialog
{
// Construction
public:
	CChangMaterilName(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CChangMaterilName)
	enum { IDD = IDD_DLG_CHANGMATERILNAME };
	CListCtrl	m_materNameList;
	CListCtrl	m_volumeIdList;
	CListCtrl	m_workNameList;
	CString	m_newMaterilName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChangMaterilName)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CChangMaterilName)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickListWorkname(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickListVolume(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHANGMATERILNAME_H__8EC3358E_C38D_4FF5_AE6E_D8C3C10F5244__INCLUDED_)
