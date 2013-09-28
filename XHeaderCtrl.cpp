// XHeaderCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "XHeaderCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
 
/////////////////////////////////////////////////////////////////////////////
// XHeaderCtrl
//const UINT XHeaderCtrl::m_nChickMsg=::RegisterWindowMessage(_T("XHeaderCtrl_Chicked"));;

XHeaderCtrl::XHeaderCtrl()
{
}

XHeaderCtrl::~XHeaderCtrl()
{
}


BEGIN_MESSAGE_MAP(XHeaderCtrl, CHeaderCtrl)
	//{{AFX_MSG_MAP(XHeaderCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// XHeaderCtrl message handlers

//DEL void XHeaderCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
//DEL {
//DEL 	// TODO: Add your message handler code here and/or call default
//DEL 	int c=this->GetItemCount();
//DEL 	for(int i=0;i<c;i++)
//DEL 	{
//DEL 		CRect rect;
//DEL 		this->GetItemRect(i,&rect);
//DEL 		if(rect.PtInRect(point))
//DEL 		{
//DEL 			CWnd* pParent=this->GetParent()->GetParent();
//DEL 			if(pParent)
//DEL 			{
//DEL 				//pParent->SendMessage(m_nChickMsg,(DWORD)this->GetSafeHwnd(),i);
//DEL 				break;
//DEL 			}
//DEL 		}
//DEL 	}
//DEL 	CHeaderCtrl::OnLButtonUp(nFlags, point);
//DEL }
