#if !defined(AFX_BESTROWJSJDH_H__44AF4678_B6EA_4848_B8C8_71005F0B6201__INCLUDED_)
#define AFX_BESTROWJSJDH_H__44AF4678_B6EA_4848_B8C8_71005F0B6201__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BestrowJSJDH.h : header file
//
#include "GridCtrl_src/GridCtrl.h"


/////////////////////////////////////////////////////////////////////////////
// BestrowJSJDH dialog

class BestrowJSJDH : public CDialog
{
// Construction
public:
	BestrowJSJDH(CWnd* pParent = NULL);   // standard constructor
    static CString WriteFileName;
// Dialog Data
	//{{AFX_DATA(BestrowJSJDH)
	enum { IDD = IDD_DIALOG_JSJDH };
	int		m_AddOrUpdate;
	//}}AFX_DATA
    CGridCtrl *m_pGridCtrl;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(BestrowJSJDH)
	public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
    _RecordsetPtr rs;
	// Generated message map functions
	//{{AFX_MSG(BestrowJSJDH)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
    
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BESTROWJSJDH_H__44AF4678_B6EA_4848_B8C8_71005F0B6201__INCLUDED_)
