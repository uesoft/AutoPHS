// SplashThread.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "SplashThread.h"
#include "user.h"
#include "PhsStart.h"
#include "ModEncrypt.h"
#include "PreDrawPag.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSplashThread

IMPLEMENT_DYNCREATE(CSplashThread, CWinThread)
HANDLE hSplashWndCreate=NULL;
CSplashThread::CSplashThread()
{
}

CSplashThread::~CSplashThread()
{
}

BOOL CSplashThread::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	CWnd wndTemp;
	CPhsStart* pSplashWnd=new CPhsStart(&wndTemp);
	pSplashWnd->m_LabelRegister=ModEncrypt::gstrLegalCompanyName;
	DWORD version;
	version=user::GetVersion();
	pSplashWnd->m_LabelVersion.Format("%d.%d",MAJOR(version),MINOR(version));
	pSplashWnd->Create(IDD_PHSSTART,&wndTemp);
	::SetEvent(hSplashWndCreate);
	pSplashWnd->ShowWindow(SW_SHOW);
	pSplashWnd->UpdateWindow();
	this->m_pMainWnd=pSplashWnd;
	FrmSplash=pSplashWnd;
	return TRUE;
}

int CSplashThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CSplashThread, CWinThread)
	//{{AFX_MSG_MAP(CSplashThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSplashThread message handlers
