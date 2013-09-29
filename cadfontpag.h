#if !defined(AFX_CADFONTPAG_H__FD73CC92_F1BD_49E4_A1A7_993799C8F6DE__INCLUDED_)
#define AFX_CADFONTPAG_H__FD73CC92_F1BD_49E4_A1A7_993799C8F6DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CadFontPag.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCadFontPag dialog

class CCadFontPag : public CDialog
{
// Construction
public:
	void DoDefaultSetting();
	CString GetCADFontDir();
	void LoadCombo();
	CCadFontPag(CString* pstrSHXFont,CString* pstrBigFont,float * pfWidthFactor,int* pAlign,CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CCadFontPag)
	enum { IDD = IDD_CADFONT_PAG };
	CComboBox	m_comboSHXFont;
	CComboBox	m_comboBigFont;
	CComboBox	m_comboAlign;
	//}}AFX_DATA
	int* m_piAlign;
	CString	*m_pstrSHXFont;
	CString	*m_pstrBigFont;
	float	*m_pfWidthFactor;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCadFontPag)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCadFontPag)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CADFONTPAG_H__FD73CC92_F1BD_49E4_A1A7_993799C8F6DE__INCLUDED_)
