//{{AFX_INCLUDES()
#include "datagrid.h"
//}}AFX_INCLUDES
#if !defined(AFX_GUIDE_H__0B1F07CF_D028_4A6E_B86E_A2D5BE6735DD__INCLUDED_)
#define AFX_GUIDE_H__0B1F07CF_D028_4A6E_B86E_A2D5BE6735DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Guide.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Guide dialog

class Guide : public CDialog
{
// Construction
public:
	Guide(CWnd* pParent = NULL);   // standard constructor
    void afresh();
	void SetColCaption();
	void UpdateMenu();
	//bool DelCol();
// Dialog Data
	//{{AFX_DATA(Guide)
	enum { IDD = IDD_DIALOG2 };
	CButton	m_check;
	int		m_radio;
	CDataGrid	m_GData;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Guide)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Guide)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadio1();
	afx_msg void OnClose();
	afx_msg void OnRadio2();
	afx_msg void OnRadio3();
	afx_msg void OnEditAddRs();
	afx_msg void OnEditDelRs();
	afx_msg void OnEditFilterRs();
	afx_msg void OnEditCanSelRs();
	afx_msg void OnEditIndexRs();
    afx_msg void OnMouseDownDatagrid(short Button, short Shift, long X, long Y);
	afx_msg void OnBUTTONGuide();
	afx_msg void OnBeforeColUpdateDatagrid(short ColIndex, VARIANT FAR* OldValue, short FAR* Cancel);
	afx_msg void OnErrorDatagrid(short DataError, short FAR* Response);
	afx_msg void OnAfterColUpdateDatagrid(short ColIndex);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	CMenu m_PopMenu;
	int cansel;
	CString strPrjDBDir;
	_variant_t Ft;
	_ConnectionPtr ConDB;
	_RecordsetPtr Rd;
    
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GUIDE_H__0B1F07CF_D028_4A6E_B86E_A2D5BE6735DD__INCLUDED_)
