// FileDialogEx.cpp : implementation file
//

#include "stdafx.h"
#include "FileDialogEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileDialogEx

IMPLEMENT_DYNAMIC(CFileDialogEx, CFileDialog)

CFileDialogEx::CFileDialogEx(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
	OSVERSIONINFO VerInfo;
	memset(&VerInfo,0,sizeof(VerInfo));
	VerInfo.dwOSVersionInfoSize=sizeof(VerInfo);
	::GetVersionEx(&VerInfo);
	if(VerInfo.dwMajorVersion>=5/*WIN2000 or WINXP*/ || VerInfo.dwMinorVersion==90/*WIN Me*/)
		this->m_ofn.lStructSize+=sizeof(DWORD)*3;
}


BEGIN_MESSAGE_MAP(CFileDialogEx, CFileDialog)
	//{{AFX_MSG_MAP(CFileDialogEx)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()








