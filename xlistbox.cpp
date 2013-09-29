// XListBox.cpp : implementation file
//

#include "stdafx.h"
#include "XListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXListBox

CXListBox::CXListBox()
{
	m_crSelBk=::GetSysColor(COLOR_HIGHLIGHT);
	m_crSelText=::GetSysColor(COLOR_HIGHLIGHTTEXT);
	m_crVirtualText=::GetSysColor(COLOR_GRAYTEXT);
	m_crText=::GetSysColor(COLOR_WINDOWTEXT);
	m_crBk=::GetSysColor(COLOR_WINDOW);
}

CXListBox::~CXListBox()
{
}


BEGIN_MESSAGE_MAP(CXListBox, CListBox)
	//{{AFX_MSG_MAP(CXListBox)
	ON_WM_DRAWITEM_REFLECT()
	ON_WM_MEASUREITEM_REFLECT()
	ON_WM_COMPAREITEM_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXListBox message handlers

void CXListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your message handler code here
	COLORREF crBk,crText;
	if(lpDrawItemStruct->itemState & ODS_SELECTED)
	{
		crBk=m_crSelBk;
		crText=m_crSelText;
	}
	else
	{
		if(IsVirtual(lpDrawItemStruct->itemID))
		{
			crText=m_crVirtualText;
		}
		else
		{
			crText=m_crText;
		}
		crBk=m_crBk;
	}
	CBrush brBk;
	brBk.CreateSolidBrush(crBk);
	::FillRect(lpDrawItemStruct->hDC,&lpDrawItemStruct->rcItem,(HBRUSH)brBk);
	COLORREF crOldBk=::SetBkColor(lpDrawItemStruct->hDC,crBk);
	COLORREF crOldText=::SetTextColor(lpDrawItemStruct->hDC,crText);
	CString strText;
	this->GetText(lpDrawItemStruct->itemID,strText);
	//::TextOut(lpDrawItemStruct->hDC,lpDrawItemStruct->rcItem.left,lpDrawItemStruct->rcItem.top,strText,strText.GetLength()+1 );
	::TextOut(lpDrawItemStruct->hDC,lpDrawItemStruct->rcItem.left,lpDrawItemStruct->rcItem.top,strText,strText.GetLength());
	::SetBkColor(lpDrawItemStruct->hDC,crOldBk);
	::SetTextColor(lpDrawItemStruct->hDC,crOldText);
}

BOOL CXListBox::SetVirtual(int iItem, BOOL bVirtual)
{
	POSITION pos=m_lstVirtualItem.GetHeadPosition();
	POSITION tmpPos;
	while(pos)
	{
		tmpPos=pos;
		int index=m_lstVirtualItem.GetNext(pos);
		if(index==iItem)
		{
			if(!bVirtual)
			{
				m_lstVirtualItem.RemoveAt(tmpPos);
			}
			return TRUE;
		}
	}
	if(bVirtual)
		m_lstVirtualItem.AddTail(iItem);
	return FALSE;
}

BOOL CXListBox::IsVirtual(int iItem)
{
	POSITION pos=m_lstVirtualItem.GetHeadPosition();
	while(pos)
	{
		int index=m_lstVirtualItem.GetNext(pos);
		if(index==iItem)
			return TRUE;
	}
	return FALSE;
}

int CXListBox::CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct) 
{
	// TODO: Replace the next line with your message handler code
	return 0;
}

void CXListBox::SetBkColor(COLORREF color)
{
	m_crBk=color;
	this->RedrawWindow();
}

void CXListBox::SetSelBkColor(COLORREF color)
{
	m_crSelBk=color;
	this->RedrawWindow();
}

void CXListBox::SetSelTextColor(COLORREF color)
{
	m_crSelText=color;
	this->RedrawWindow();
}

void CXListBox::SetTextColor(COLORREF color)
{
	m_crText=color;
	this->RedrawWindow();
}

void CXListBox::SetVirtualTextColor(COLORREF color)
{
	m_crVirtualText=color;
	this->RedrawWindow();
}
