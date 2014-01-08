#include "basDirectory.h"
#include "user.h"
#include "BrowseForFolerModule.h"
#include "modRegistry.h"

#if !defined(AFX_FRMFOLDERLOCATION_H__0770B2F6_60AE_11D5_8750_00D009619765__INCLUDED_)
#define AFX_FRMFOLDERLOCATION_H__0770B2F6_60AE_11D5_8750_00D009619765__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FrmFolderLocation.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFrmFolderLocation dialog

class CFrmFolderLocation : public CDialog
{
private:
	bool INIchanged,PrjXDirChanged;
// Construction
public:
	CFrmFolderLocation(CWnd* pParent = NULL);   // standard constructor
	void LoadCaption();
	CString GetDir(CString key);
    void SetDir (CString key,CString setval);

// Dialog Data
	//CString	m_strFolderLocation;
	//{{AFX_DATA(CFrmFolderLocation)
	enum { IDD = IDD_FOLDER_LOCATION };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFrmFolderLocation)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFrmFolderLocation)
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSf1();
	afx_msg void OnSf2();
	afx_msg void OnSf3();
	afx_msg void OnSf4();
	afx_msg void OnSf5();
	afx_msg void OnSf6();
	afx_msg void OnSf7();
	//}}AFX_MSG
	afx_msg void OnSf(UINT uID);
	DECLARE_MESSAGE_MAP()
};
extern CFrmFolderLocation frmFolderLocation;
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRMFOLDERLOCATION_H__0770B2F6_60AE_11D5_8750_00D009619765__INCLUDED_)
