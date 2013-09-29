#if !defined(AFX_DATAINPUT_H__5ED46D15_E751_4F75_8F6A_7255599960C9__INCLUDED_)
#define AFX_DATAINPUT_H__5ED46D15_E751_4F75_8F6A_7255599960C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DataInput.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CDataInput command target

class CDataInput : public CCmdTarget
{
	DECLARE_DYNCREATE(CDataInput)

	CDataInput();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDataInput)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CDataInput();

	// Generated message map functions
	//{{AFX_MSG(CDataInput)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(CDataInput)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CDataInput)
	afx_msg void DataInputImg();
	afx_msg void DataInputModifyZdjh();
	afx_msg void DataInputPipeana();
	afx_msg void DataInputSArec();
	afx_msg void DataInputSelPro();
	afx_msg void DataInputTab();
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATAINPUT_H__5ED46D15_E751_4F75_8F6A_7255599960C9__INCLUDED_)
