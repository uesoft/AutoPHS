#if !defined(AFX_STATUSBARTHREAD_H__985B46A1_B101_11D5_AE95_00D00961973B__INCLUDED_)
#define AFX_STATUSBARTHREAD_H__985B46A1_B101_11D5_AE95_00D00961973B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StatusBarThread.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CStatusBarThread thread
#include "FrmStatus.h"
class CStatusBarThread : public CWinThread
{
	DECLARE_DYNCREATE(CStatusBarThread)
public:
	CStatusBarThread();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:
	CFrmStatus m_StatusBar;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatusBarThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
public:
	void EndThread();
	virtual ~CStatusBarThread();

	// Generated message map functions
	//{{AFX_MSG(CStatusBarThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATUSBARTHREAD_H__985B46A1_B101_11D5_AE95_00D00961973B__INCLUDED_)
