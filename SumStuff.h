#if !defined(AFX_SUMSTUFF_H__E7796C77_BB7E_4F67_B064_0288F0768E34__INCLUDED_)
#define AFX_SUMSTUFF_H__E7796C77_BB7E_4F67_B064_0288F0768E34__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SumStuff.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CSumStuff command target

class CSumStuff : public CCmdTarget
{
	DECLARE_DYNCREATE(CSumStuff)

	CSumStuff();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSumStuff)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSumStuff();

	// Generated message map functions
	//{{AFX_MSG(CSumStuff)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(CSumStuff)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CSumStuff)
	afx_msg void SumStuffACAD();
	afx_msg void SumStuffACAD2();
	afx_msg void SumStuffExcel();
	afx_msg void SumStuffExcel2();
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUMSTUFF_H__E7796C77_BB7E_4F67_B064_0288F0768E34__INCLUDED_)
