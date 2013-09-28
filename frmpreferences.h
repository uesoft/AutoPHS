#if !defined(AFX_FRMPREFERENCES_H__D3BA0639_6A41_11D5_8752_00D009619765__INCLUDED_)
#define AFX_FRMPREFERENCES_H__D3BA0639_6A41_11D5_8752_00D009619765__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FrmPreferences.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFrmPreferences dialog

#include "PreCalPag.h"
#include "PreDrawPag.h"
#include "PrePointPag.h"
#include "PreStylePag.h"

class CFrmPreferences : public CPropertySheet
{
// Construction
public:
	CButton m_btnDefault;
	CPreCalPag  m_wndCalPag;
	CPreDrawPag	m_wndDrawPag;
	CPrePointPag	m_wndPointPag;
	CPreStylePag	m_wndStylePag;
	CFrmPreferences();   // standard constructor

// Dialog Data


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFrmPreferences)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	CRect m_Rect;
	bool m_bMove;
	// Generated message map functions
	//{{AFX_MSG(CFrmPreferences)
	afx_msg void OnOK();
	afx_msg void OnRestoreDefault();
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	afx_msg void OnMove(int x, int y);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRMPREFERENCES_H__D3BA0639_6A41_11D5_8752_00D009619765__INCLUDED_)
