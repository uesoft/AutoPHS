// MImage.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "MImage.h"
#include "user.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMImage

CMImage::CMImage()
{
	m_Index=-1;
	m_bDst=false;
}

CMImage::~CMImage()
{
}


BEGIN_MESSAGE_MAP(CMImage, CStatic)
	//{{AFX_MSG_MAP(CMImage)
	//ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	ON_WM_CREATE()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMImage message handlers

//DEL void CMImage::OnClicked() 
//DEL {
//DEL 	// TODO: Add your control notification handler code here
//DEL 
//DEL }

BOOL CMImage::Create(CWnd* pParentWnd, UINT nID) 
{
	// TODO: Add your specialized code here and/or call the base class
	CRect rc;
	rc.left=50;
	rc.top=50;
	rc.right=rc.left+IMG_W;
	rc.bottom=rc.top+IMG_H;
	//CString lpN=::AfxRegisterWndClass( 0,LoadCursor(NULL,IDC_ARROW),(HBRUSH)COLOR_WINDOW);
	//return CWnd::Create(lpN,NULL,WS_CHILD|WS_VISIBLE,rc,pParentWnd,nID);
	//return CWnd::Create(NULL,WS_CHILD|WS_VISIBLE|SS_BITMAP|SS_CENTERIMAGE|SS_NOTIFY,rc,pParentWnd,nID);
	return CStatic::Create(NULL,WS_CHILD|WS_VISIBLE|SS_BITMAP|SS_NOTIFY,rc,pParentWnd,nID);
}

void CMImage::SetImage(const _TCHAR *imgName)
{
	HBITMAP bmp;
	DeleteObject(GetBitmap());
	bmp=LoadBitmap(::AfxGetResourceHandle(),imgName);
	if(bmp!=NULL)
	{
		SetBitmap(bmp);
		BITMAP bm;
		::GetObject(bmp,sizeof(bm),&bm);
		CRect rc;
		this->GetWindowRect(&rc);
		CWnd* pWnd=this->GetParent();
		if(pWnd!=NULL)
			pWnd->ScreenToClient(&rc);
		rc.right=rc.left+bm.bmWidth;
		rc.bottom=rc.top+bm.bmHeight;
		//MoveWindow(&rc);
		return;
	}
	if(::FileExists(imgName))
	{
		bmp=(HBITMAP)::LoadImage(AfxGetInstanceHandle(),imgName,IMAGE_BITMAP,0,0,
		LR_DEFAULTCOLOR|LR_DEFAULTSIZE|LR_LOADFROMFILE);
		SetBitmap(bmp);
		return;
	}
	CDC * pDC=GetDC();
	CRect rect;
	GetClientRect(&rect);
	bmp=::CreateCompatibleBitmap(pDC->GetSafeHdc(),rect.Width(),rect.Height());
	if(bmp!=NULL)
	{
		HDC hMemDC=::CreateCompatibleDC(pDC->GetSafeHdc());
		HBITMAP hOldBm=(HBITMAP)::SelectObject(hMemDC,bmp);
		HBRUSH br;
		br=::CreateSolidBrush(RGB(255,255,255));
		::FillRect(hMemDC,&rect,br);
		::DeleteObject(br);
		::SelectObject(hMemDC,hOldBm);
		::DeleteObject(hMemDC);
		SetBitmap(bmp);
	}
	this->ReleaseDC(pDC);
	//this->Invalidate();
}

int CMImage::GetHeight()
{
	CRect rc;
	this->GetWindowRect(&rc);
	return rc.Height();
}

int CMImage::GetWidth()
{
	CRect rc;
	this->GetWindowRect(&rc);
	return rc.Width();
}

void CMImage::SetHeight(int Height)
{
	CRect rc;
	this->GetWindowRect(&rc);
	CWnd* pWnd=this->GetParent();
	if(pWnd!=NULL)
	{
		pWnd->ScreenToClient(&rc);
	}
	rc.bottom=rc.top+Height;
	this->MoveWindow(&rc);
}

void CMImage::SetWidth(int Width)
{
	CRect rc;
	this->GetWindowRect(&rc);
	CWnd* pWnd=this->GetParent();
	if(pWnd!=NULL)
	{
		pWnd->ScreenToClient(&rc);
	}
	rc.right=rc.left+Width;
	this->MoveWindow(&rc);

}

int CMImage::GetTop()
{
	CRect rc;
	this->GetWindowRect(&rc);
	CWnd* pWnd=this->GetParent();
	if(pWnd!=NULL)
	{
		pWnd->ScreenToClient(&rc);
	}
	return rc.top;
}

int CMImage::GetLeft()
{
	CRect rc;
	this->GetWindowRect(&rc);
	CWnd* pWnd=this->GetParent();
	if(pWnd!=NULL)
	{
		pWnd->ScreenToClient(&rc);
	}
	return rc.left;

}

void CMImage::SetTop(int top)
{
	CRect rc;
	this->GetWindowRect(&rc);
	CWnd* pWnd=this->GetParent();
	if(pWnd!=NULL)
	{
		pWnd->ScreenToClient(&rc);
	}
	this->SetWindowPos(NULL,rc.left,top,0,0,SWP_NOZORDER|SWP_NOSIZE);
}

void CMImage::SetLeft(int left)
{
	CRect rc;
	this->GetWindowRect(&rc);
	CWnd* pWnd=this->GetParent();
	if(pWnd!=NULL)
	{
		pWnd->ScreenToClient(&rc);
	}
	this->SetWindowPos(NULL,left,rc.top,0,0,SWP_NOZORDER|SWP_NOSIZE);

}

void CMImage::SetVisible(BOOL newValue)
{
	if(newValue!=m_Visible)
	{
		m_Visible=newValue;
		ShowWindow(m_Visible ? SW_SHOW : SW_HIDE);
	}
}

BOOL CMImage::GetVisible()
{
	return m_Visible;
}



int CMImage::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	//SetHeight(IMG_H);
	//SetWidth(IMG_W);
	//ShowWindow(SW_SHOW);
	return 0;
}

void CMImage::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	CWnd::OnRButtonUp(nFlags, point);
}

void CMImage::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(this->m_bDst==false)
	{
		this->Dstinvert();
	}
	if(m_Index>=0)
	{
		CWnd* pWnd=this->GetParent();
		if(pWnd==NULL)
			return;
		pWnd->SendMessage(IMG_CLICKED,m_Index);
	}
	CWnd::OnLButtonUp(nFlags, point);
}

//DEL HBITMAP CMImage::GetBitmap()
//DEL {
//DEL 	return m_hbm;
//DEL }

//DEL void CMImage::SetBitmap(HBITMAP hbm)
//DEL {
//DEL 	if(m_hbm!=NULL)
//DEL 		DeleteObject(m_hbm);
//DEL 	m_hbm=hbm;
//DEL }

bool CMImage::Dstinvert(bool bDst)
{

	if(IsWindow(m_hWnd) && bDst!=m_bDst && GetBitmap()!=NULL)
	{
		CDC dc;
		CDC* pDC=GetDC();
		dc.CreateCompatibleDC(pDC);
		CBitmap * pOldBm=dc.SelectObject(CBitmap::FromHandle(GetBitmap()));
		BITMAP bm;
		GetObject(GetBitmap(),sizeof(bm),&bm);
		dc.BitBlt(0,0,bm.bmWidth,bm.bmHeight,NULL,0,0,DSTINVERT);
		Invalidate();
		dc.SelectObject(pOldBm);
		dc.DeleteDC();
		ReleaseDC(pDC);
	}
	bool ret=m_bDst;
	m_bDst=bDst;
	return ret;
}
