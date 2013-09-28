#if !defined(AFX_DLGEXPORTVOLUME_H__A0363FAA_7FA4_4449_80B0_04620D8AD99B__INCLUDED_)
#define AFX_DLGEXPORTVOLUME_H__A0363FAA_7FA4_4449_80B0_04620D8AD99B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgExportVolume.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgExportVolume dialog

class CDlgExportVolume : public CDialog
{
// Construction
public:
	CDlgExportVolume(CWnd* pParent = NULL);   // standard constructor
	CMapStringToString m_map;

// Dialog Data
	//{{AFX_DATA(CDlgExportVolume)
	enum { IDD = IDD_DLG_BACKUPDB_TABLENAME };
	CComboBox	m_comboHadBackup;
	CString	m_strDes;
	CString	m_strCurBackupDes;
	CString	m_strCurBackupTablename;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgExportVolume)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgExportVolume)
	afx_msg void OnSelchangeComboHadBackup();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEXPORTVOLUME_H__A0363FAA_7FA4_4449_80B0_04620D8AD99B__INCLUDED_)
