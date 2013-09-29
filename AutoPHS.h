// AutoPHS.h : main header file for the AUTOPHS application
//

#if !defined(AFX_AUTOPHS_H__599942AB_18C8_4E2F_A7F4_E09235C26BDE__INCLUDED_)
#define AFX_AUTOPHS_H__599942AB_18C8_4E2F_A7F4_E09235C26BDE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CAutoPHSApp:
// See AutoPHS.cpp for the implementation of this class
//

class CAutoPHSApp : public CWinApp
{
public:
	HACCEL m_hAccelData;
	CAutoPHSApp();
	virtual ~CAutoPHSApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoPHSApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	COleTemplateServer m_server;
		// Server object for document creation
	//{{AFX_MSG(CAutoPHSApp)
	afx_msg void OnAppAbout();
	afx_msg void OnContextHelp();
	afx_msg void OnHelpFinder();
	afx_msg void OnHelpIndex();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOPHS_H__599942AB_18C8_4E2F_A7F4_E09235C26BDE__INCLUDED_)
