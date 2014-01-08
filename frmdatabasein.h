#if !defined(AFX_FRMDATABASEIN_H__2A68CBC4_A04B_11D5_AE95_00D00961973B__INCLUDED_)
#define AFX_FRMDATABASEIN_H__2A68CBC4_A04B_11D5_AE95_00D00961973B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FrmDatabaseIn.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFrmDatabaseIn dialog

class CFrmDatabaseIn : public CDialog
{
// Construction
public:
	bool m_bIsCurDB;
	CString m_strEnginID;
	void LoadListVlm();
	_ConnectionPtr m_db;
	void LoadListEngin();
	long inline GetMaxVlmID(_ConnectionPtr & db);
	bool m_bDataSelected;
	CString m_strFileName;
	void getDatabase();
	CFrmDatabaseIn(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFrmDatabaseIn)
	enum { IDD = IDD_DATABASEIN };
	CStatic	m_staticPID;
	CComboBox	m_comboEngin;
	CListBox	m_List1;
	CString	m_sVlmID;
	CString	m_sGc;
	CString	m_sList1;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFrmDatabaseIn)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool m_bIsNewDB;

	// Generated message map functions
	//{{AFX_MSG(CFrmDatabaseIn)
	virtual BOOL OnInitDialog();
	afx_msg void OnDataIn();
	afx_msg void OnButton3();
	afx_msg void OnSelchangeComboEngin();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRMDATABASEIN_H__2A68CBC4_A04B_11D5_AE95_00D00961973B__INCLUDED_)
