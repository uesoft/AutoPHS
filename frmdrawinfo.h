//{{AFX_INCLUDES()
//}}AFX_INCLUDES
#if !defined(AFX_FRMDRAWINFO_H__CB8E3961_B4DF_11D5_AE95_00D00961973B__INCLUDED_)
#define AFX_FRMDRAWINFO_H__CB8E3961_B4DF_11D5_AE95_00D00961973B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FrmDrawInfo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFrmDrawInfo dialog

class CFrmDrawInfo : public CDialog
{
// Construction
public:
	CFrmDrawInfo(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFrmDrawInfo)
	enum { IDD = IDD_DRAWINFO };
	CString	m_strPrjName;
	CString	m_strDrawName;
	CString	m_strDrawNo;
	CString	m_strScal;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFrmDrawInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFrmDrawInfo)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnChangeEditScale();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRMDRAWINFO_H__CB8E3961_B4DF_11D5_AE95_00D00961973B__INCLUDED_)
