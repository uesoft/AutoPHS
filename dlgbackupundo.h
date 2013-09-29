#if !defined(AFX_DLGBACKUPUNDO_H__09BED629_DDEA_4EFC_8D69_0BFF919EE049__INCLUDED_)
#define AFX_DLGBACKUPUNDO_H__09BED629_DDEA_4EFC_8D69_0BFF919EE049__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgBackupUndo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgBackupUndo dialog

class CDlgBackupUndo : public CDialog
{
// Construction
public:
	CString m_strPrjDBPath;
	CString m_strAppInstallPath;
	CDlgBackupUndo(CWnd* pParent = NULL);   // standard constructor
//	CMapStringToString m_map;

// Dialog Data
	//{{AFX_DATA(CDlgBackupUndo)
	enum { IDD = IDD_DLG_UNDO_BACKUP };
	CComboBox	m_comboHadBackup;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBackupUndo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgBackupUndo)
	afx_msg void OnSelchangeComboHadBackup();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBACKUPUNDO_H__09BED629_DDEA_4EFC_8D69_0BFF919EE049__INCLUDED_)
