#if !defined(AFX_FORMAT_H__C92373EF_B6D0_49D8_8713_8C822084A612__INCLUDED_)
#define AFX_FORMAT_H__C92373EF_B6D0_49D8_8713_8C822084A612__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Format.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CFormat command target

class CFormat : public CCmdTarget
{
	DECLARE_DYNCREATE(CFormat)

	CFormat();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFormat)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CFormat();

	// Generated message map functions
	//{{AFX_MSG(CFormat)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(CFormat)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CFormat)
	afx_msg void FormatSelTabFormat();
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORMAT_H__C92373EF_B6D0_49D8_8713_8C822084A612__INCLUDED_)
