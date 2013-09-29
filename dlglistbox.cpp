// DlgListBox.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "DlgListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgListBox

CDlgListBox::CDlgListBox()
{
	m_bkColor=-1;
}

CDlgListBox::~CDlgListBox()
{
}


BEGIN_MESSAGE_MAP(CDlgListBox, CListBox)
	//{{AFX_MSG_MAP(CDlgListBox)

	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgListBox message handlers

void CDlgListBox::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CListBox::OnRButtonUp(nFlags, point);
}

void CDlgListBox::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CPoint po=point;
	CListBox::OnRButtonDown(nFlags, point);
	CWnd* pWnd=this->GetParent();
	if(pWnd!=NULL && IsWindow(pWnd->m_hWnd))
	{
		this->ClientToScreen(&po);
		pWnd->ScreenToClient(&po);
		DWORD lp;
		lp=po.x | po.y<<16;
		pWnd->SendMessage(WM_RBUTTONUP,0,lp);
	}
}

void CDlgListBox::GetText(int ix, CString &rString)
{
	if(ix > this->GetCount()-1)
	{
		rString="";
		return ;
	}
	CListBox::GetText(ix,rString);
}

int CDlgListBox::GetText(int ix, LPTSTR lpszBuffer)
{
	if(ix > this->GetCount()-1)
	{
		return LB_ERR;
	}
	return (int)CListBox::GetText(ix,lpszBuffer);
}

void CDlgListBox::SetBkColor(DWORD color)
{
	m_bkColor=color;
	m_bkBr.DeleteObject();
	LOGBRUSH logBr;
	logBr.lbStyle=BS_SOLID;
	logBr.lbColor=m_bkColor;
	m_bkBr.CreateBrushIndirect(&logBr);
	this->Invalidate();
}

HBRUSH CDlgListBox::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	// TODO: Change any attributes of the DC here
	
	// TODO: Return a non-NULL brush if the parent's handler should not be called
	if(m_bkColor==-1)
		return NULL;
	if(/*nCtlColor==CTLCOLOR_STATIC && */pDC!=NULL)
	{
		pDC->SetBkMode(TRANSPARENT);
		//return NULL;
	}
	return (HBRUSH)m_bkBr;
}

int CDlgListBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	return 0;
}
