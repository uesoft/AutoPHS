#if !defined(AFX_XLISTBOX_H__EAB3FEEE_5A0C_4C44_988F_FBB1D31A4CAD__INCLUDED_)
#define AFX_XLISTBOX_H__EAB3FEEE_5A0C_4C44_988F_FBB1D31A4CAD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// XListBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CXListBox window
#include <afxtempl.h>
class CXListBox : public CListBox
{
// Construction
public:
	CXListBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXListBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetVirtualTextColor(COLORREF color);
	void SetTextColor(COLORREF color);
	void SetSelTextColor(COLORREF color);
	void SetSelBkColor(COLORREF color);
	void SetBkColor(COLORREF color);
	BOOL IsVirtual(int iItem);
	BOOL SetVirtual(int iItem,BOOL bVirtual=TRUE);
	virtual ~CXListBox();

	// Generated message map functions
	CList< int , int& > m_lstVirtualItem;
protected:
	COLORREF m_crSelBk;
	COLORREF m_crBk;
	COLORREF m_crText;
	COLORREF m_crVirtualText;
	COLORREF m_crSelText;

	//{{AFX_MSG(CXListBox)
	afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XLISTBOX_H__EAB3FEEE_5A0C_4C44_988F_FBB1D31A4CAD__INCLUDED_)
