#if !defined(AFX_SPLASHTHREAD_H__DEF10478_8E03_4F4B_916B_BB46FB6FE5B1__INCLUDED_)
#define AFX_SPLASHTHREAD_H__DEF10478_8E03_4F4B_916B_BB46FB6FE5B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SplashThread.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CSplashThread thread

class CSplashThread : public CWinThread
{
	DECLARE_DYNCREATE(CSplashThread)
protected:
	CSplashThread();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplashThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSplashThread();

	// Generated message map functions
	//{{AFX_MSG(CSplashThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
extern HANDLE hSplashWndCreate;
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPLASHTHREAD_H__DEF10478_8E03_4F4B_916B_BB46FB6FE5B1__INCLUDED_)
