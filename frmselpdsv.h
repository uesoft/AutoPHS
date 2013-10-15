//{{AFX_INCLUDES()
#include "datagrid.h"
//#include "datacombo.h"
#include "adodc.h"
//}}AFX_INCLUDES
#if !defined(AFX_FRMSELPDSV_H__0292ED4D_7465_11D5_8752_00D009619765__INCLUDED_)
#define AFX_FRMSELPDSV_H__0292ED4D_7465_11D5_8752_00D009619765__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FrmSelPDSV.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFrmSelPDSV dialog
#include "DBComboBox.h"
class CFrmSelPDSV : public CDialog
{
// Construction
public:
	bool m_bIsVlmNew;
	bool m_bIsNew;
	long GetMaxVolumeID();
	long m_SelHyID;
	long m_SelZyID;
	long m_SelSpecID;
	CString m_SelPrjID;
	void SelToCurrWork();
	void SetComboItem(CDBComboBox & bc,char* sItem);
	void LoadDBComboSpec();
	void LoadDBComboCategory();
	void LoadDBComboDsgn();
	long m_SelDsgnID;
	_RecordsetPtr m_DataCurrWork;
	_RecordsetPtr m_DataCategory;
	_RecordsetPtr m_DataSpe;
	_RecordsetPtr m_DataDsgn;
	_RecordsetPtr	m_DataEng;
	_RecordsetPtr	m_DataVlm;
	CString m_SelPrjName;
	void InitDBVlm();
	void InitENG();
	void initPrjDb();
	CFrmSelPDSV(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFrmSelPDSV)
	enum { IDD = IDD_SELPDSV };
	CDataGrid	m_DBGeng;
	CDataGrid	m_DBGvlm;
	CDBComboBox	m_DBComboDsgn;
	CDBComboBox	m_DBComboSpec;
	CDBComboBox	m_DBComboCategory;
	CString	m_csDBDsgn;
	CString	m_csDBSpec;
	CString	m_csDBCategory;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFrmSelPDSV)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	 LRESULT SelChangeCmbo();
	// Generated message map functions
	//{{AFX_MSG(CFrmSelPDSV)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblClickDbgvim();
	afx_msg void OnBeforeDeleteDbgeng(short FAR* Cancel);
	afx_msg void OnDblClickDbgeng();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnRowColChangeDbgvim(VARIANT FAR* LastRow, short LastCol);
	afx_msg void OnRowColChangeDbgeng(VARIANT FAR* LastRow, short LastCol);
	afx_msg void OnColResizeDbgeng(short ColIndex, short FAR* Cancel);
	afx_msg void OnColResizeDbgvim(short ColIndex, short FAR* Cancel);
	afx_msg void OnBeforeUpdateDbgvim(short FAR* Cancel);
	afx_msg void OnOnAddNewDbgvim();
	afx_msg void OnOnAddNewDbgeng();
	afx_msg void OnAfterUpdateDbgeng();
	afx_msg void OnAfterUpdateDbgvim();
	afx_msg void OnBeforeDeleteDbgvim(short FAR* Cancel);
	afx_msg void OnEditchangeComboHY();
	afx_msg void OnSelchangeComboHY();
	afx_msg void OnSelchangeComboSpe();
	afx_msg void OnSelchangeComboDsgn();
	virtual void OnOK();
	afx_msg void OnEditchangeComboCategory();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
extern CFrmSelPDSV frmSelPDSV;
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRMSELPDSV_H__0292ED4D_7465_11D5_8752_00D009619765__INCLUDED_)
