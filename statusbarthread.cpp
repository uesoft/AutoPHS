// StatusBarThread.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "StatusBarThread.h"
#include "FrmStatus.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStatusBarThread

IMPLEMENT_DYNCREATE(CStatusBarThread, CWinThread)

CStatusBarThread::CStatusBarThread()
{
}

CStatusBarThread::~CStatusBarThread()
{
}

BOOL CStatusBarThread::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	m_bAutoDelete = TRUE;//让线程自己结束
	CWnd wnd;
	wnd.m_hWnd=NULL;
	m_StatusBar.m_bThread=TRUE;
	if( !IsWindow(m_StatusBar.GetSafeHwnd()) )
	{
		m_StatusBar.Create(IDD_STATUS,&wnd);
	}
	m_pMainWnd=&m_StatusBar;
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	m_StatusBar.m_btnStop.ShowWindow(SW_SHOW);
	SetEvent(hStatusCreateEvent);
	return TRUE;
}

int CStatusBarThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	if(IsWindow(m_pMainWnd->GetSafeHwnd()))
	{
		//m_pMainWnd->DestroyWindow();
		m_pMainWnd->ShowWindow(SW_HIDE);
	}
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CStatusBarThread, CWinThread)
	//{{AFX_MSG_MAP(CStatusBarThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatusBarThread message handlers

void CStatusBarThread::EndThread()
{
	if(IsWindow(m_pMainWnd->GetSafeHwnd()))
	{
		//m_pMainWnd->DestroyWindow();
		m_pMainWnd->ShowWindow(SW_HIDE);
	}
}
