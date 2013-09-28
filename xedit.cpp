// XEdit.cpp : implementation file
//

#include "stdafx.h"
#include "XEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXEdit

CXEdit::CXEdit()
{
	m_crBk=::GetSysColor(COLOR_WINDOW);
	m_brBk.CreateSolidBrush(m_crBk);
	m_crText=::GetSysColor(COLOR_WINDOWTEXT);
}

CXEdit::~CXEdit()
{
	m_brBk.DeleteObject();
}


BEGIN_MESSAGE_MAP(CXEdit, CEdit)
	//{{AFX_MSG_MAP(CXEdit)
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXEdit message handlers

HBRUSH CXEdit::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	// TODO: Change any attributes of the DC here
	pDC->SetBkColor(m_crBk);
	pDC->SetTextColor(m_crText);
	return m_brBk;
	// TODO: Return a non-NULL brush if the parent's handler should not be called

}

void CXEdit::SetTextColor(COLORREF color)
{
	m_crText=color;
	this->RedrawWindow();
}

void CXEdit::SetBkColor(COLORREF color)
{
	m_brBk.DeleteObject();
	m_crBk=color;
	m_brBk.CreateSolidBrush(color);
	this->RedrawWindow();
}
