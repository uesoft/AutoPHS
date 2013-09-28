#if !defined(AFX_CALCULATE_H__E6D5EC06_80B0_46B3_BA11_0D0C519E1BC8__INCLUDED_)
#define AFX_CALCULATE_H__E6D5EC06_80B0_46B3_BA11_0D0C519E1BC8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Calculate.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CCalculate command target

class CCalculate : public CCmdTarget
{
	DECLARE_DYNCREATE(CCalculate)

	CCalculate();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCalculate)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CCalculate();

	// Generated message map functions
	//{{AFX_MSG(CCalculate)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(CCalculate)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CCalculate)
	afx_msg void CalculateAllZdj();
	afx_msg void CalculateOneZdj();
	afx_msg void CalculateShowTab();
	afx_msg void VBCal(long zdjh, long nth);
	afx_msg BOOL AutoCalPhs(const VARIANT FAR& varRs, short zdjh, short nth);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CALCULATE_H__E6D5EC06_80B0_46B3_BA11_0D0C519E1BC8__INCLUDED_)
