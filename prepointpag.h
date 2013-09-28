#if !defined(AFX_PREPOINTPAG_H__0888E9C7_9DF0_11D5_AE95_00D00961973B__INCLUDED_)
#define AFX_PREPOINTPAG_H__0888E9C7_9DF0_11D5_AE95_00D00961973B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PrePointPag.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPrePointPag dialog
class CPrePointPag : public CPropertyPage
{
	DECLARE_DYNCREATE(CPrePointPag)

// Construction
public:
	BOOL UpdatePoint();
	void DoDefaultSetting();
	struct pointFld{
		long FldValue;
		CString FldName;
		//UINT nID;
	};
	void SetBasePoint();
	void SaveIniValue();
	void SetCtrlValue();
	CPrePointPag();
	virtual ~CPrePointPag();

// Dialog Data
	pointFld m_posFld[9];
	//{{AFX_DATA(CPrePointPag)
	enum { IDD = IDD_PRE_PAG3 };
	CEdit	m_TextPt3y;
	CEdit	m_TextPt3x;
	CEdit	m_TextPt2y;
	CEdit	m_TextPt2x;
	CEdit	m_TextPt1y;
	CEdit	m_TextPt1x;
	CEdit	m_TextVX_pt0y;
	CEdit	m_TextVZ_pt0x;
	CEdit	m_TextVX_pt0x;
	BOOL	m_bCheckAutoTZ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPrePointPag)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	long oldA3A4;
		bool bSetDefault;
		bool bSetBPOK;
		bool bSetPrevious;
		bool bGetPos;
		long iSetBPbegan;
		long oldix;
	// Generated message map functions
	//{{AFX_MSG(CPrePointPag)
	virtual BOOL OnInitDialog();
	afx_msg void OnAutoadj();
	afx_msg void OnDefault();
	afx_msg void OnPrevious();
	afx_msg void OnOk1();
	afx_msg void OnCancel1();
	afx_msg void OnOptionz1();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREPOINTPAG_H__0888E9C7_9DF0_11D5_AE95_00D00961973B__INCLUDED_)
