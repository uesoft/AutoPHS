#if !defined(AFX_PHSSTART_H__CDA12625_7AE8_11D6_874B_00D009619765__INCLUDED_)
#define AFX_PHSSTART_H__CDA12625_7AE8_11D6_874B_00D009619765__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PhsStart.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPhsStart dialog

class CPhsStart : public CDialog
{
// Construction
public:
	CBitmap m_bm;
	CPhsStart(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPhsStart)
	enum { IDD = IDD_PHSSTART };
	//CStatic	m_Pic;
	CString	m_label1;
	CString	m_LabelRegister;
	CString	m_LabelVersion;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPhsStart)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	afx_msg void OnActivateApp( BOOL bActive,DWORD hTask /*HTASK hTask */);
	// Generated message map functions
	//{{AFX_MSG(CPhsStart)
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaint();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CPhsStart* FrmSplash;
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PHSSTART_H__CDA12625_7AE8_11D6_874B_00D009619765__INCLUDED_)
