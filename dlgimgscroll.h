#if !defined(AFX_DLGIMGSCROLL_H__7AD465A1_80DF_11D5_8752_00D009619765__INCLUDED_)
#define AFX_DLGIMGSCROLL_H__7AD465A1_80DF_11D5_8752_00D009619765__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgImgScroll.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgImgScroll dialog
#include "DlgImgLst.h"
class CDlgImgScroll : public CWnd
{
// Construction
public:
	virtual  ~CDlgImgScroll();
	short m_nScrWidth;
	short m_nScrHeight;
	short m_nScrColWidth;
	short m_nScrLineHeight;
	void OnImgClick(int Index);
	CDlgImgScroll(CListBox*,CListBox* pListBox1);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgImgScroll)
	enum { IDD = IDD_IMG_SCROLL };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	CDlgImgLst * m_pDlgImgLst;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgImgScroll)
	public:
	virtual BOOL Create(CWnd* pParentWnd=NULL);
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	CListBox* m_pListBox;
	CBrush m_bkbr;
	// Generated message map functions
	//{{AFX_MSG(CDlgImgScroll)
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGIMGSCROLL_H__7AD465A1_80DF_11D5_8752_00D009619765__INCLUDED_)
