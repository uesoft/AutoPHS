// XImgListBox.cpp : implementation file
//

#include "stdafx.h"
#include "XImgListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXImgListBox

CXImgListBox::CXImgListBox()
{
	m_bAutoImgSize=TRUE;
	m_BmpSize.cx=20;
	m_BmpSize.cy=40;
}

CXImgListBox::~CXImgListBox()
{
	POSITION pos=m_lBitmaps.GetHeadPosition();
	for(;pos;)
	{
		HBITMAP hBm=m_lBitmaps.GetNext(pos);
		if(hBm!=NULL)
			::DeleteObject(hBm);
	}
	m_lBitmaps.RemoveAll();
}


BEGIN_MESSAGE_MAP(CXImgListBox, CListBox)
	//{{AFX_MSG_MAP(CXImgListBox)
	ON_WM_DRAWITEM_REFLECT()
	ON_WM_MEASUREITEM_REFLECT()
	ON_WM_DELETEITEM_REFLECT()
	ON_WM_COMPAREITEM_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXImgListBox message handlers

void CXImgListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your message handler code here
	HDC hDC;
	HBITMAP hBm=GetItemBmp(lpDrawItemStruct->itemID);
	CBrush brBk;
	if(hBm==NULL)
	{
		if(lpDrawItemStruct->itemState & ODS_SELECTED)
			brBk.CreateSolidBrush(::GetSysColor(COLOR_HIGHLIGHT));
		else
			brBk.CreateSolidBrush(::GetSysColor(COLOR_WINDOW));
		::FillRect(lpDrawItemStruct->hDC,&lpDrawItemStruct->rcItem,(HBRUSH)brBk);
	}
	else
	{
		brBk.CreateSolidBrush(::GetSysColor(COLOR_WINDOW));
		::FillRect(lpDrawItemStruct->hDC,&lpDrawItemStruct->rcItem,(HBRUSH)brBk);
		hDC=::CreateCompatibleDC(lpDrawItemStruct->hDC);
		HBITMAP hOldBm=(HBITMAP)::SelectObject(hDC,hBm);
		BITMAP bm;
		::GetObject(hBm,sizeof(BITMAP),&bm);
		CRect rect;
		CRect* prect=(CRect*)&lpDrawItemStruct->rcItem;
		if((float)(lpDrawItemStruct->rcItem.right-lpDrawItemStruct->rcItem.left)/(float)(lpDrawItemStruct->rcItem.bottom-lpDrawItemStruct->rcItem.top) >= (float)bm.bmWidth/(float)bm.bmHeight)
		{
			rect.top=prect->top;
			rect.bottom=prect->bottom;
			int W=(bm.bmWidth * prect->Height())/bm.bmHeight;
			rect.left=(prect->Width()-W)/2 + prect->left;
			rect.right=rect.left+W;
		}
		else
		{
			rect.left=prect->left;
			rect.right=prect->right;
			int H=(bm.bmHeight * prect->Width())/bm.bmWidth;
			rect.top=(prect->Height() - H)/2 + prect->top;
			rect.bottom=rect.top + H;
		}
		::StretchBlt(lpDrawItemStruct->hDC,rect.left,
			rect.top,rect.Width(),
			rect.Height(),
			hDC,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
		if(lpDrawItemStruct->itemState & ODS_SELECTED)
		{
			//::BitBlt(lpDrawItemStruct->hDC,rect.left,rect.top,
			///	rect.Width(),rect.Height(),
			//	NULL,0,0,DSTINVERT);
			::BitBlt(lpDrawItemStruct->hDC,prect->left,prect->top,
				prect->Width(),prect->Height(),
				NULL,0,0,DSTINVERT);
		}
		else
		{
		}
		::SelectObject(hDC,hOldBm);
		::DeleteObject(hDC);
	}
	brBk.DeleteObject();
	
}

void CXImgListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	// TODO: Add your message handler code here
	if(m_bAutoImgSize)
	{
		BITMAP bm;
		HBITMAP hBm=GetItemBmp(lpMeasureItemStruct->itemID);
		::GetObject(hBm,sizeof(BITMAP),&bm);
		lpMeasureItemStruct->itemWidth=bm.bmWidth;
		lpMeasureItemStruct->itemHeight=bm.bmHeight;
	}
	else
	{
		lpMeasureItemStruct->itemHeight=m_BmpSize.cy;
		lpMeasureItemStruct->itemWidth=m_BmpSize.cx;
	}
}



void CXImgListBox::SetBmpSize(DWORD cx, DWORD cy)
{
	m_BmpSize.cx=cx;
	m_BmpSize.cy=cy;
	m_bAutoImgSize=FALSE;
}

int CXImgListBox::AddBitmap(HBITMAP hBm)
{
	m_lBitmaps.AddTail(hBm);
	CString strTemp;
	strTemp.Format(_T("Item%d"),this->GetCount());
	return this->AddString(strTemp);
}

HBITMAP CXImgListBox::GetItemBmp(int iItem)
{
	int i=0;
	POSITION pos=m_lBitmaps.GetHeadPosition();
	for(; pos && i<=iItem ; i++)
	{
		HBITMAP hBm=m_lBitmaps.GetNext(pos);
		if(i==iItem)
			return hBm;
	}
	return NULL;
}

int CXImgListBox::AddBitmapID(UINT nID)
{
	HBITMAP hBm=::LoadBitmap(::AfxGetResourceHandle(),MAKEINTRESOURCE(nID));
	return AddBitmap(hBm);
}

int CXImgListBox::AddBitmapFile(LPCTSTR lpszFileName)
{
	HBITMAP hBm=(HBITMAP)::LoadImage(::AfxGetInstanceHandle(),lpszFileName,IMAGE_BITMAP,0,0,LR_DEFAULTCOLOR|LR_DEFAULTSIZE|LR_LOADFROMFILE);
	return AddBitmap(hBm);
}

void CXImgListBox::DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct) 
{
	// TODO: Add your message handler code here
	DeleteBmp(lpDeleteItemStruct->itemID);
}

void CXImgListBox::DeleteBmp(int iItem)
{
	int i=0;
	POSITION pos=m_lBitmaps.GetHeadPosition();
	POSITION tmpPos=pos;
	for(; pos && i<=iItem ; i++)
	{
		tmpPos=pos;
		HBITMAP hBm=m_lBitmaps.GetNext(pos);
		if(i==iItem)
		{
			if(hBm)
				DeleteObject(hBm);
			m_lBitmaps.RemoveAt(tmpPos);
		}
	}
}


void CXImgListBox::SetAutoImgSize(BOOL bAutoImgSize)
{
	m_bAutoImgSize=bAutoImgSize;
}

int CXImgListBox::CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct) 
{
	// TODO: Replace the next line with your message handler code
	return 0;
}
