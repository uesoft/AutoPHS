//{{AFX_INCLUDES()
#include "datagrid.h"
//}}AFX_INCLUDES
#if !defined(AFX_FRMDATAEDIT_H__82DAC6C5_7510_11D5_AE93_00D00961973B__INCLUDED_)
#define AFX_FRMDATAEDIT_H__82DAC6C5_7510_11D5_AE93_00D00961973B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FrmDataEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFrmDataEdit dialog

class CFrmDataEdit : public CDialog
{
// Construction
public:
	int m_iRodNum;
	_RecordsetPtr m_Data1;
	CFrmDataEdit(CWnd* pParent = NULL);   // standard constructor
	bool m_bAllowUpd;
// Dialog Data
	//{{AFX_DATA(CFrmDataEdit)
	enum { IDD = IDD_DATAEDIT };
	CDataGrid	m_DBGrid1;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFrmDataEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString m_strCols[50];

	// Generated message map functions
	//{{AFX_MSG(CFrmDataEdit)
	virtual BOOL OnInitDialog();
	afx_msg 	void OnActivate(UINT nState, CWnd *pWndOther, BOOL bMinimized);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnColResizeDatagrid1(short ColIndex, short FAR* Cancel);
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnErrorDatagrid1(short DataError, short FAR* Response);
	afx_msg void OnBeforeDeleteDatagrid1(short FAR* Cancel);
	afx_msg void OnAfterDeleteDatagrid1();
	afx_msg void OnMouseMoveDatagrid1(short Button, short Shift, long X, long Y);
	afx_msg void OnAfterUpdateDatagrid1();
	afx_msg void OnBeforeUpdateDatagrid1(short FAR* Cancel);
	afx_msg void OnRowColChangeDatagrid1(VARIANT FAR* LastRow, short LastCol);
	afx_msg void OnBeforeColEditDatagrid1(short ColIndex, short KeyAscii, short FAR* Cancel);
	afx_msg void OnAfterColEditDatagrid1(short ColIndex);
	afx_msg void OnAfterColUpdateDatagrid1(short ColIndex);
	afx_msg void OnChangeDatagrid1();
	afx_msg void OnBeforeColUpdateDatagrid1(short ColIndex, VARIANT FAR* OldValue, short FAR* Cancel);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
extern CFrmDataEdit FrmDataEDIT;
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRMDATAEDIT_H__82DAC6C5_7510_11D5_AE93_00D00961973B__INCLUDED_)
