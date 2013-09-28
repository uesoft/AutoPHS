// AutoPHSDoc.h : interface of the CAutoPHSDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUTOPHSDOC_H__95F51F78_0A4C_4B00_8DBE_971B596A50FF__INCLUDED_)
#define AFX_AUTOPHSDOC_H__95F51F78_0A4C_4B00_8DBE_971B596A50FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CAutoPHSDoc : public CDocument
{
protected: // create from serialization only
	CAutoPHSDoc();
	DECLARE_DYNCREATE(CAutoPHSDoc)

// Attributes
public:
	ULONG m_ulID;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoPHSDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
public:
	HACCEL m_hAccelData;
	virtual ~CAutoPHSDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CAutoPHSDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CAutoPHSDoc)
	afx_msg LPDISPATCH GetCalculate();
	afx_msg void SetCalculate(LPDISPATCH newValue);
	afx_msg LPDISPATCH GetDataInput();
	afx_msg void SetDataInput(LPDISPATCH newValue);
	afx_msg LPDISPATCH GetFiles();
	afx_msg void SetFiles(LPDISPATCH newValue);
	afx_msg LPDISPATCH GetFormat();
	afx_msg void SetFormat(LPDISPATCH newValue);
	afx_msg LPDISPATCH GetSumStuff();
	afx_msg void SetSumStuff(LPDISPATCH newValue);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOPHSDOC_H__95F51F78_0A4C_4B00_8DBE_971B596A50FF__INCLUDED_)
