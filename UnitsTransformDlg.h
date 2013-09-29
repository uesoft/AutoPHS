#if !defined(AFX_UNITSTRANSFORMDLG_H__F6AF4BCB_C493_4D40_90C5_C2F058974491__INCLUDED_)
#define AFX_UNITSTRANSFORMDLG_H__F6AF4BCB_C493_4D40_90C5_C2F058974491__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UnitsTransformDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUnitsTransformDlg dialog

class CUnitsTransformDlg : public CDialog
{
// Construction
public:
	CUnitsTransformDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CUnitsTransformDlg)
	enum { IDD = IDD_UNITS_TRANSFORM };
	CButton	m_conIdokBut;
	CListCtrl	m_ListConversion;
	_Recordset *m_pRsUnits;//显示在CListCtl控件中的记录集(表UnitsToUeUnites)
	double	m_dGene;
	double	m_dOffset;
	CString	m_strSourceUnits;
	CString	m_strDestinationUnits;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUnitsTransformDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CUnitsTransformDlg)
	afx_msg void OnSave();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnClickListConversion(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#endif // !defined(AFX_UNITSTRANSFORMDLG_H__F6AF4BCB_C493_4D40_90C5_C2F058974491__INCLUDED_)
