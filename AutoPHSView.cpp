// AutoPHSView.cpp : implementation of the CAutoPHSView class
//

#include "stdafx.h"
#include "AutoPHS.h"

#include "AutoPHSDoc.h"
#include "AutoPHSView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAutoPHSView

IMPLEMENT_DYNCREATE(CAutoPHSView, CView)

BEGIN_MESSAGE_MAP(CAutoPHSView, CView)
	//{{AFX_MSG_MAP(CAutoPHSView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoPHSView construction/destruction

CAutoPHSView::CAutoPHSView()
{
	// TODO: add construction code here

}

CAutoPHSView::~CAutoPHSView()
{
}

BOOL CAutoPHSView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CAutoPHSView drawing

void CAutoPHSView::OnDraw(CDC* pDC)
{
	CAutoPHSDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CAutoPHSView diagnostics

#ifdef _DEBUG
void CAutoPHSView::AssertValid() const
{
	CView::AssertValid();
}

void CAutoPHSView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CAutoPHSDoc* CAutoPHSView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAutoPHSDoc)));
	return (CAutoPHSDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAutoPHSView message handlers
