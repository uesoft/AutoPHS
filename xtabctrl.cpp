// XTabCtrl.cpp : implementation file
//

//	文件：XTabCtrl.CPP
//	作者：许朝
//	日期：2002年6月

#include "stdafx.h"
//#include "autophs2002.h"

#include "XTabCtrl.h"
#include <afxpriv.h>

#if(WINVER < 0x0500)
	#define AW_HOR_POSITIVE             0x00000001
	#define AW_HOR_NEGATIVE             0x00000002
	#define AW_VER_POSITIVE             0x00000004
	#define AW_VER_NEGATIVE             0x00000008
	#define AW_CENTER                   0x00000010
	#define AW_HIDE                     0x00010000
	#define AW_ACTIVATE                 0x00020000
	#define AW_SLIDE                    0x00040000
	#define AW_BLEND                    0x00080000
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXTabCtrl
typedef BOOL (WINAPI * __PROCAnimateWindow) (
  HWND hwnd,     // handle to the window to animate
  DWORD dwTime,  // duration of animation
  DWORD dwFlags  // animation type
);

CXTabCtrl::CXTabCtrl()
{
	m_hActiveWnd=NULL;
}

CXTabCtrl::~CXTabCtrl()
{
}


BEGIN_MESSAGE_MAP(CXTabCtrl, CTabCtrl)
	//{{AFX_MSG_MAP(CXTabCtrl)
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnSelchange)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTabCtrl message handlers

void CXTabCtrl::OnSelchange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	HWND hOldWnd=m_hActiveWnd;
	int iCurSel=this->GetCurSel();
	if(iCurSel!=-1)
	{
		TCITEM item;
		item.mask=TCIF_PARAM;
		this->GetItem(iCurSel,&item);
		m_hActiveWnd=(HWND)(DWORD)item.lParam;
		if(IsWindow(m_hActiveWnd))
		{
			//this->SetTimer(TM_WNDSHOW,20,NULL);
			CRect rect;
			GetClientRect(&rect);
			AdjustRect(FALSE,&rect);
			HWND hParent=::GetParent(m_hActiveWnd);
			this->ClientToScreen(&rect);
			if(hParent)
			{
				::ScreenToClient(hParent, (LPPOINT)&rect);
				::ScreenToClient(hParent, ((LPPOINT)&rect)+1);
			}
			::SetWindowPos(m_hActiveWnd,NULL,rect.left,rect.top,rect.Width(),rect.Height(),SWP_NOZORDER|SWP_NOACTIVATE|SWP_HIDEWINDOW);
			HMODULE hMod=::GetModuleHandle(_T("USER32.dll"));
			__PROCAnimateWindow  PROCAnimateWindow=NULL;
			if(hMod != NULL)
			{
				PROCAnimateWindow=(__PROCAnimateWindow )::GetProcAddress(hMod,_T("AnimateWindow"));
				
			}
			if(PROCAnimateWindow!=NULL)
			{
				PROCAnimateWindow(m_hActiveWnd,250,AW_SLIDE|AW_HOR_POSITIVE);
			}
			else
				::ShowWindow(m_hActiveWnd,SW_SHOW);
			if(IsWindow(hOldWnd))
				::ShowWindow(hOldWnd,SW_HIDE);
		}
	}
	*pResult = 0;
	CWnd* pParent=GetParent();
	if(pParent)
	{
		AFX_NOTIFY notify;
		notify.pResult = pResult;
		notify.pNMHDR = pNMHDR;
		pParent->OnCmdMsg(GetDlgCtrlID(), MAKELONG(TCN_SELCHANGE, WM_NOTIFY), &notify, NULL);
		//pParent->SendNotifyMessage(WM_NOTIFY,this->GetDlgCtrlID(),(LPARAM)(LPVOID)pNMHDR);
	}
}


BOOL CXTabCtrl::AddWnd(CWnd *pWnd,LPCTSTR lpszLabel )
{
	if(pWnd->GetSafeHwnd()==NULL)
		return FALSE;

	DWORD Style=::GetWindowLong(pWnd->GetSafeHwnd(),GWL_STYLE);

	_TCHAR szTitle[80];
	if((Style & WS_CAPTION) != 0)
	{
		Style &= ~WS_CAPTION;
		if(lpszLabel==NULL || _tcscmp(lpszLabel,_T(""))==0)
		{
			pWnd->GetWindowText(szTitle,80);
			lpszLabel=szTitle;
		}
	}
	Style &= ~WS_SYSMENU;

	::SetWindowLong(pWnd->GetSafeHwnd(),GWL_STYLE,Style);
	TCITEM item;
	item.mask=TCIF_TEXT|TCIF_PARAM;
	item.pszText=(LPTSTR)lpszLabel;
	item.lParam=(DWORD)(LPVOID)pWnd->GetSafeHwnd();
	this->InsertItem(this->GetItemCount(),&item);
	if(this->GetItemCount() == 1)
	{
		m_hActiveWnd=pWnd->GetSafeHwnd();
	}
	if(m_hActiveWnd)
	{
		CRect rect;
		this->GetClientRect(&rect);
		this->AdjustRect(FALSE,&rect);
		::SetWindowPos(m_hActiveWnd,NULL,rect.left,rect.top,rect.Width(),rect.Height(),SWP_NOZORDER|SWP_SHOWWINDOW|SWP_NOACTIVATE);
	}
	if(pWnd->GetSafeHwnd() != m_hActiveWnd)
		pWnd->ShowWindow(SW_HIDE);
	return TRUE;
}



CWnd* CXTabCtrl::GetActiveWnd()
{
	return CWnd::FromHandle(m_hActiveWnd);
}

BOOL CXTabCtrl::SetActiveWnd(CWnd *pWnd)
{
	if(pWnd->GetSafeHwnd()==NULL)
		return FALSE;
	int c,i;
	c=this->GetItemCount();
	TCITEM item;
	item.mask=TCIF_PARAM;
	for(i=0;i<c;i++)
	{
		this->GetItem(i,&item);
		HWND hWnd=(HWND)(DWORD)item.lParam;
		if(hWnd==pWnd->GetSafeHwnd())
			break;
	}
	if(i>=c)
		return FALSE;
	this->SetCurSel(i);
	if(m_hActiveWnd && IsWindow(m_hActiveWnd))
		::ShowWindow(m_hActiveWnd,SW_HIDE);
	CRect rect;
	this->GetClientRect(&rect);
	this->AdjustRect(FALSE,&rect);
	this->ClientToScreen(&rect);
	HWND hParent=::GetParent(pWnd->GetSafeHwnd());
	if(hParent)
	{
		::ScreenToClient(hParent, (LPPOINT)&rect);
		::ScreenToClient(hParent, ((LPPOINT)&rect)+1);
	}
	::SetWindowPos(pWnd->GetSafeHwnd(),NULL,rect.left,rect.top,rect.Width(),rect.Height(),SWP_NOZORDER|SWP_SHOWWINDOW|SWP_NOACTIVATE);
	m_hActiveWnd=pWnd->GetSafeHwnd();
	return TRUE;
}

BOOL CXTabCtrl::RemoveWnd(CWnd* pWnd,BOOL bDestroyWindow)
{
	if(pWnd->GetSafeHwnd()==NULL)
		return FALSE;
	int c,i;
	c=this->GetItemCount();
	TCITEM item;
	item.mask=TCIF_PARAM;
	for(i=0;i<c;i++)
	{
		this->GetItem(i,&item);
		HWND hWnd=(HWND)(DWORD)item.lParam;
		if(hWnd==pWnd->GetSafeHwnd())
			break;
	}
	if(i>=c)
		return FALSE;
	this->DeleteItem(i);
	pWnd->ShowWindow(SW_HIDE);
	
	if(pWnd->GetSafeHwnd()==m_hActiveWnd)
	{
		i=this->GetCurSel();
		if(i<0)
		{
			m_hActiveWnd=NULL;
			return TRUE;
		}
		this->GetItem(i,&item);
		m_hActiveWnd=(HWND)(DWORD)item.lParam;
	}
	if(bDestroyWindow)
	{
		pWnd->DestroyWindow();
	}
	if(m_hActiveWnd==NULL || !IsWindow(m_hActiveWnd))
		return TRUE;

	CRect rect;
	this->GetClientRect(&rect);
	this->AdjustRect(FALSE,&rect);
	this->ClientToScreen(&rect);
	HWND hParent=::GetParent(m_hActiveWnd);
	if(hParent)
	{
		::ScreenToClient(hParent, (LPPOINT)&rect);
		::ScreenToClient(hParent, ((LPPOINT)&rect)+1);
	}
	::SetWindowPos(m_hActiveWnd,NULL,rect.left,rect.top,rect.Width(),rect.Height(),SWP_NOZORDER|SWP_SHOWWINDOW|SWP_NOACTIVATE);
	return TRUE;
}


BOOL CXTabCtrl::RemoveWnd(int index,BOOL bDestroyWindow)
{
	if(index<0 || index>=this->GetItemCount())
		return FALSE;
	TCITEM item;
	item.mask=TCIF_PARAM;
	this->GetItem(index,&item);
	HWND hWnd=(HWND)(LPVOID)item.lParam;
	this->DeleteItem(index);
	
	if(IsWindow(hWnd))
	{
		if(bDestroyWindow)
		{
			::DestroyWindow(hWnd);
		}
		else
		{
			::ShowWindow(hWnd,SW_HIDE);
		}
	}
	int i=this->GetCurSel();
	if(hWnd==m_hActiveWnd)
	{
		if(i<0)
		{
			m_hActiveWnd=NULL;
			return TRUE;
		}
		this->GetItem(i,&item);
		m_hActiveWnd=(HWND)(DWORD)item.lParam;
	}
	if(m_hActiveWnd==NULL || !IsWindow(m_hActiveWnd))
		return TRUE;

	CRect rect;
	this->GetClientRect(&rect);
	this->AdjustRect(FALSE,&rect);
	this->ClientToScreen(&rect);
	HWND hParent=::GetParent(m_hActiveWnd);
	if(hParent)
	{
		::ScreenToClient(hParent, (LPPOINT)&rect);
		::ScreenToClient(hParent, ((LPPOINT)&rect)+1);
	}
	::SetWindowPos(m_hActiveWnd,NULL,rect.left,rect.top,rect.Width(),rect.Height(),SWP_NOZORDER|SWP_SHOWWINDOW|SWP_NOACTIVATE);
	return TRUE;
}	

void CXTabCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CTabCtrl::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if(IsWindow(m_hActiveWnd))
	{
		CRect rect;
		this->GetClientRect(&rect);
		this->AdjustRect(FALSE,&rect);
		this->ClientToScreen(&rect);
		HWND hParent=::GetParent(m_hActiveWnd);
		if(hParent)
		{
			::ScreenToClient(hParent, (LPPOINT)&rect);
			::ScreenToClient(hParent, ((LPPOINT)&rect)+1);
		}
		::SetWindowPos(m_hActiveWnd,NULL,rect.left,rect.top,rect.Width(),rect.Height(),SWP_NOZORDER|SWP_NOACTIVATE);
	}
}

BOOL CXTabCtrl::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	BOOL bRet;
	CWnd *pParent=NULL;

	pParent=GetParent();

	bRet=CTabCtrl::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
	if(!bRet)
	{
		if(pParent)
		{
			return pParent->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
		}
	}

	return bRet;
}
