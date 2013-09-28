#if !defined(AFX_FILES_H__707ED53C_305A_4FAD_B2DE_61D0D1B6F055__INCLUDED_)
#define AFX_FILES_H__707ED53C_305A_4FAD_B2DE_61D0D1B6F055__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Files.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CFiles command target

class CFiles : public CCmdTarget
{
	DECLARE_DYNCREATE(CFiles)

	CFiles();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFiles)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CFiles();

	// Generated message map functions
	//{{AFX_MSG(CFiles)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(CFiles)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CFiles)
	afx_msg void FilesOpt();
	afx_msg void FilesSelDir();
	afx_msg void FilesSelPreferences();
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILES_H__707ED53C_305A_4FAD_B2DE_61D0D1B6F055__INCLUDED_)
