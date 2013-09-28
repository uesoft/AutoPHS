// AutoPHSView.h : interface of the CAutoPHSView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUTOPHSVIEW_H__449C1A3D_7239_48F2_B909_2FB5C3E17979__INCLUDED_)
#define AFX_AUTOPHSVIEW_H__449C1A3D_7239_48F2_B909_2FB5C3E17979__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CAutoPHSView : public CView
{
protected: // create from serialization only
	CAutoPHSView();
	DECLARE_DYNCREATE(CAutoPHSView)

// Attributes
public:
	CAutoPHSDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoPHSView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAutoPHSView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CAutoPHSView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in AutoPHSView.cpp
inline CAutoPHSDoc* CAutoPHSView::GetDocument()
   { return (CAutoPHSDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOPHSVIEW_H__449C1A3D_7239_48F2_B909_2FB5C3E17979__INCLUDED_)
