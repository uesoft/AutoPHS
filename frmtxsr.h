#if !defined(AFX_FRMTXSR_H__1354811D_D389_499C_AA8F_BC9E19574F1F__INCLUDED_)
#define AFX_FRMTXSR_H__1354811D_D389_499C_AA8F_BC9E19574F1F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FrmTxsr.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFrmTxsr frame
#include "Viewtxsr.h"
class CFrmTxsr : public CFrameWnd
{
	DECLARE_DYNCREATE(CFrmTxsr)
public:
	CFrmTxsr();           // protected constructor used by dynamic creation

// Attributes
public:
	bool m_IsFirst;//pfg20050216

// Operations
public:
	BOOL Create(CWnd* pParentWnd);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFrmTxsr)
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	CViewTxsr* m_pViewTxsr;
	virtual ~CFrmTxsr();

	// Generated message map functions
	//{{AFX_MSG(CFrmTxsr)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnDestroy();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
extern CFrmTxsr	FrmTxsr;
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRMTXSR_H__1354811D_D389_499C_AA8F_BC9E19574F1F__INCLUDED_)
