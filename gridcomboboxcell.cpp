// GridComboBoxCell.cpp: implementation of the CGridComboBoxCell class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "autophs.h"
#include "GridComboBoxCell.h"
#include "GridCtrl_src\InPlaceList.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CGridComboBoxCell, CGridCell)
CGridComboBoxCell::CGridComboBoxCell()
: m_iCurSel(-1)
{
	m_dwStyle=CBS_DROPDOWN|WS_VSCROLL;
}

CGridComboBoxCell::~CGridComboBoxCell()
{

}

BOOL CGridComboBoxCell::Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar)
{
    /*DWORD dwStyle = ES_LEFT;
    if (GetFormat() & DT_RIGHT) 
        dwStyle = ES_RIGHT;
    else if (GetFormat() & DT_CENTER) 
        dwStyle = ES_CENTER;*/
    m_bEditing = TRUE;
    
    // InPlaceEdit auto-deletes itself
    CGridCtrl* pGrid = GetGrid();
    m_pEditWnd = new CInPlaceList((CWnd*)pGrid, rect, m_dwStyle, nID, nRow, nCol, m_Items,GetText(), nChar);
    
    return TRUE;
}

void CGridComboBoxCell::EndEdit()
{
	if (m_pEditWnd)
        ((CInPlaceList*)m_pEditWnd)->EndEdit();
}

int CGridComboBoxCell::GetCurSel()
{
	if(m_iCurSel==-1)
	{
		int i=0;
		for( i=0;i<m_Items.GetSize() && m_Items[i] != this->m_strText ; i++);
		if(i<m_Items.GetSize())
			m_iCurSel=i;
	}
	return m_iCurSel;
}

void CGridComboBoxCell::SetCurSel(int iIndex)
{
	int c=m_Items.GetSize();
	if(iIndex<0 || iIndex>=c) return;
	this->SetText(m_Items[iIndex]);
	m_iCurSel=iIndex;
}

int CGridComboBoxCell::FindStringExact(LPCTSTR lpszText)
{
	int i=0;
	for(i=0;i<m_Items.GetSize() && m_Items[i].Compare(lpszText)!=0 ;i++);
	if(i>=m_Items.GetSize()) return -1;
	return i;
}
