// DlgImgScroll.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "DlgImgScroll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgImgScroll dialog


CDlgImgScroll::CDlgImgScroll(CListBox* pListBox,CListBox* pListBox1)
: m_pListBox(pListBox)
{
	//{{AFX_DATA_INIT(CDlgImgScroll)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	this->m_nScrColWidth=this->m_nScrWidth=IMG_W;
	this->m_nScrLineHeight=this->m_nScrHeight=IMG_H;
	m_pDlgImgLst=new CDlgImgLst(m_pListBox,pListBox1);
}





BEGIN_MESSAGE_MAP(CDlgImgScroll, CWnd)
	//{{AFX_MSG_MAP(CDlgImgScroll)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_SIZE()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgImgScroll message handlers

LRESULT CDlgImgScroll::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(message==IMG_CLICKED)
		OnImgClick(wParam);
	return CWnd::WindowProc(message, wParam, lParam);
}

void CDlgImgScroll::OnImgClick(int Index)
{
	CRect rc,rc2,rc3,rc4;
	int posx,posy;
	int CW;
	if(Index >=0 && Index<m_pDlgImgLst->m_imgCount && IsWindow(m_pDlgImgLst->m_Image[Index]->m_hWnd))
	{
		m_pDlgImgLst->m_Image[Index]->GetWindowRect(&rc);
		rc3=rc;
		this->ScreenToClient(rc3);
		m_pDlgImgLst->GetWindowRect(&rc2);
		m_pDlgImgLst->ScreenToClient(&rc);
		this->GetClientRect(&rc4);
		this->ScreenToClient(&rc2);
		if(rc4.Width()>=rc2.Width())
		{
			m_pDlgImgLst->SetWindowPos(NULL,0,rc2.top,0,0,SWP_NOSIZE|SWP_NOZORDER);
		}
		else
		{
			if(rc3.left<0)
			{
				CW=rc2.Width()-rc4.Width();
				posx=(rc.left*m_nScrWidth)/CW;
				if(posx>m_nScrWidth)
					posx=m_nScrWidth;
				if(posx<0)
					posx=0;
				this->OnHScroll(SB_THUMBPOSITION,posx,NULL);
			}
			else if(rc3.right>rc4.right)
			{
				CW=rc2.Width()-rc4.Width();
				posx=((rc.right-rc4.Width())*m_nScrWidth)/CW;
				if(posx>m_nScrWidth)
					posx=m_nScrWidth;
				if(posx<0)
					posx=0;
				this->OnHScroll(SB_THUMBPOSITION,posx,NULL);
			}
		}
		if(rc4.Height()>=rc2.Height())
		{
			m_pDlgImgLst->GetWindowRect(&rc2);
			this->ScreenToClient(&rc2);
			m_pDlgImgLst->SetWindowPos(NULL,rc2.left,0,0,0,SWP_NOSIZE|SWP_NOZORDER);
		}
		else
		{
			if(rc3.top<0)
			{
				CW=rc2.Height()-rc4.Height();
				posy=(rc.top*m_nScrHeight)/CW;
				if(posy>m_nScrHeight)
					posy=m_nScrHeight;
				if(posy<0)
					posy=0;
				this->OnVScroll(SB_THUMBPOSITION,posy,NULL);
			}
			else if(rc3.bottom>rc4.bottom)
			{
				CW=rc2.Height()-rc4.Height();
				posy=((rc.bottom-rc4.Height())*m_nScrHeight)/CW;
				if(posy>m_nScrHeight)
					posy=m_nScrHeight;
				if(posy<0)
					posy=0;
				this->OnVScroll(SB_THUMBPOSITION,posy,NULL);
			}
		}
	}
}

void CDlgImgScroll::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	int pos=this->GetScrollPos(SB_VERT);
	static long prpos=0;
	switch(nSBCode)
	{
	case SB_LINEDOWN://向下滚动一行
		if(pos<m_nScrHeight)
		pos+=m_nScrLineHeight;
		break;
	case SB_LINEUP://向上滚动一行
		if(pos>0)
			pos-=m_nScrLineHeight;
		break;
	case SB_PAGEDOWN://向下滚动一页
		if(pos< m_nScrHeight-m_nScrLineHeight)
			pos+=m_nScrLineHeight;
		else pos=m_nScrHeight;
		break;
	case SB_PAGEUP://向上滚动一页
		if(pos> m_nScrLineHeight) pos-=m_nScrLineHeight;
		else pos=0;
		break;
	case SB_THUMBPOSITION://滚动到一个绝对位置
		pos=nPos;
		break;
	case SB_THUMBTRACK://拖动滚动框到指定位置
		pos=nPos;
		break;

	}
	this->SetScrollPos(SB_VERT,pos);

	CRect rc,rc2,rc3;
	this->GetClientRect(&rc3);
	m_pDlgImgLst->GetWindowRect(&rc);
	this->ScreenToClient(&rc);
	if(rc3.Height()>=rc.Height())
	{
		m_pDlgImgLst->SetWindowPos(NULL,rc.left,0,0,0,SWP_NOSIZE|SWP_NOZORDER);
		return;
	}
	rc2=rc;
	if(pos==0)
	{
		rc2=rc;
		rc2.top=0;
		rc2.bottom=rc.Height();
	}
	else if(pos==m_nScrHeight)
	{
		rc2=rc;
		rc2.bottom=rc3.Height();
		rc2.top=rc2.bottom-rc.Height();
	}
	else
	{
		int CH=rc.Height()-rc3.Height();
		int hx=(CH*pos)/m_nScrHeight;
		rc2=rc;
		rc2.top=-hx;
		rc2.bottom=rc2.top+rc.Height();
	}
	m_pDlgImgLst->MoveWindow(&rc2);
	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CDlgImgScroll::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	int pos=this->GetScrollPos(SB_HORZ);
	static long prpos=0;
	switch(nSBCode)
	{
	case SB_LINERIGHT://向右滚动一行
		if(pos<m_nScrWidth)
		pos+=m_nScrColWidth;
		break;
	case SB_LINELEFT://向左滚动一行
		if(pos>0)
			pos-=m_nScrColWidth;
		break;
	case SB_PAGERIGHT://向右滚动一页
		if(pos< m_nScrWidth-m_nScrColWidth)
			pos+=m_nScrColWidth;
		else pos=m_nScrWidth;
		break;
	case SB_PAGELEFT://向左滚动一页
		if(pos> m_nScrColWidth) pos-=m_nScrColWidth;
		else pos=0;
		break;
	case SB_THUMBPOSITION://滚动到一个绝对位置
		pos=nPos;
		break;
	case SB_THUMBTRACK://拖动滚动框到指定位置
		pos=nPos;
		break;

	}
	this->SetScrollPos(SB_HORZ,pos);

	CRect rc,rc2,rc3;
	this->GetClientRect(&rc3);
	m_pDlgImgLst->GetWindowRect(&rc);
	this->ScreenToClient(&rc);
	if(rc3.Width()>=rc.Width())
	{
		m_pDlgImgLst->SetWindowPos(NULL,0,rc.top,0,0,SWP_NOSIZE|SWP_NOZORDER);
		return;
	}
	rc2=rc;
	if(pos==0)
	{
		rc2=rc;
		rc2.left=0;
		rc2.right=rc.Width();
	}
	else if(pos==m_nScrWidth)
	{
		rc2=rc;
		rc2.right=rc3.Width();
		rc2.left=rc2.right-rc.Width();
	}
	else
	{
		int CW=rc.Width()-rc3.Width();
		int wx=(CW*pos)/m_nScrWidth;
		rc2=rc;
		rc2.left=-wx;
		rc2.right=rc2.left+rc.Width();
	}
	m_pDlgImgLst->MoveWindow(&rc2);
	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}





void CDlgImgScroll::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	
}

BOOL CDlgImgScroll::Create(CWnd* pParentWnd)
{
	// TODO: Add your specialized code here and/or call the base class
	
	CString lpszClassName=::AfxRegisterWndClass( 0,::LoadCursor(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_ARROW)),
									(HBRUSH)COLOR_GRAYTEXT,NULL);
	return CWnd::CreateEx( 0,lpszClassName, _T(""), WS_CHILD|WS_VSCROLL|WS_HSCROLL,
								CW_USEDEFAULT,CW_USEDEFAULT,
								CW_USEDEFAULT,CW_USEDEFAULT,
								pParentWnd->GetSafeHwnd() , NULL);
}

int CDlgImgScroll::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	if(!m_pDlgImgLst->Create(this))
		return -1;
	m_pDlgImgLst->ShowWindow(SW_SHOW);
	m_pDlgImgLst->UpdateWindow();
	m_pDlgImgLst->SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOSIZE);

	return 0;
}

CDlgImgScroll::~CDlgImgScroll()
{
	delete m_pDlgImgLst;
}
