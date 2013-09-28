#if !defined(AFX_TEXTPROGRESSCTRL_H__4C78DBBE_EFB6_11D1_AB14_203E25000000__INCLUDED_)
#define AFX_TEXTPROGRESSCTRL_H__4C78DBBE_EFB6_11D1_AB14_203E25000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// TextProgressCtrl.h : header file
//
// Written by Chris Maunder (chrismaunder@codeguru.com)
// Copyright 1998.
//
// Modified : 26/05/98 Jeremy Davis, jmd@jvf.co.uk
//				Added colour routines
//
// TextProgressCtrl is a drop-in replacement for the standard 
// CProgressCtrl that displays text in a progress control.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed by any means PROVIDING it is not sold for
// profit without the authors written consent, and providing that this
// notice and the authors name is included. If the source code in 
// this file is used in any commercial application then an email to
// the me would be nice.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to your
// computer, causes your pet cat to fall ill, increases baldness or
// makes you car start emitting strange noises when you start it up.
//
// Expect bugs.
// 
// Please use and enjoy. Please let me know of any bugs/mods/improvements 
// that you have found/implemented and I will fix/incorporate them into this
// file. 

/////////////////////////////////////////////////////////////////////////////
// CTextProgressCtrl window

class CTextProgressCtrl : public CProgressCtrl
{
// Construction
public:
	CTextProgressCtrl();

// Attributes
public:

// Operations
public:
    int			SetPos(int nPos);//设置CTextProgressCtrl的m_nBarwith的值,返回上一次的m_nPos 
    int			StepIt();//返回调用SetPos的返回值
    void		SetRange(int nLower, int nUpper);//设置最大、最小值
    int			OffsetPos(int nPos);//返回调用SetPos的返回值
    int			SetStep(int nStep);//设置m_nStepSize的值，并返回nSetp的值
	void		SetForeColour(COLORREF col);//设置前景色，即m_colFore的值
	void		SetBkColour(COLORREF col);//设置背景色，即m_colBk的值
	void		SetTextForeColour(COLORREF col);//设置文本前景色，即m_colTextFore的值
	void		SetTextBkColour(COLORREF col);//设置文本背景色，即m_colTextBk的值
	COLORREF	GetForeColour();//取得前景色
	COLORREF	GetBkColour();//取得背景色
	COLORREF	GetTextForeColour();//取得文本前景色
	COLORREF	GetTextBkColour();//取得文本背景色

    void		SetShowText(BOOL bShow);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextProgressCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetText(LPCTSTR lpszText);
	void SetTextFont(LPCTSTR lpszFontName,int nHeight,BOOL bBold=FALSE);//设置文本的字体
	virtual ~CTextProgressCtrl();

	// Generated message map functions
protected:
	CFont m_TextFont;
    int			m_nPos, 
				m_nStepSize, 
				m_nMax, 
				m_nMin;
    CString		m_strText;
    BOOL		m_bShowText;
    int			m_nBarWidth;
	COLORREF	m_colFore,
				m_colBk,
				m_colTextFore,
				m_colTextBk;

	//{{AFX_MSG(CTextProgressCtrl)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
    afx_msg LRESULT OnSetText(WPARAM wParam,LPARAM lParam);
    afx_msg LRESULT OnGetText(WPARAM wParam,LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTPROGRESSCTRL_H__4C78DBBE_EFB6_11D1_AB14_203E25000000__INCLUDED_)
