#if !defined(AFX_FRMSTATUS_H__0770B2F7_60AE_11D5_8750_00D009619765__INCLUDED_)
#define AFX_FRMSTATUS_H__0770B2F7_60AE_11D5_8750_00D009619765__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FrmStatus.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFrmStatus dialog
#include "TextProgressCtrl.h"
#define WM_CLOSEWINDOW1 WM_USER+101
class CFrmStatus : public CDialog
{
// Construction
public:
	BOOL m_bThread;
	virtual  ~CFrmStatus();
	//CRITICAL_SECTION m_crUpd;
	//CRITICAL_SECTION m_crStat;
	CRect m_wndRc;
	void UpdateStatus(float sngPercent,bool fBorderCase=false);
	CFrmStatus(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFrmStatus)
	enum { IDD = IDD_STATUS };
	CStatic	m_ctrLabel2;
	CStatic	m_ctrLabel1;
	CButton	m_btnStop;
	CTextProgressCtrl	m_picStatus;
	CString	m_Label2;
	CString	m_Label1;
	long m_nPos;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFrmStatus)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnOK();
	afx_msg LRESULT OnCloseWindow1(/*UINT msg,*/WPARAM wParam,LPARAM lParam);

	// Generated message map functions
	//{{AFX_MSG(CFrmStatus)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnStop();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
extern CFrmStatus frmStatus;
extern HANDLE hStatusStopEvent;
extern HANDLE hStatusCreateEvent;
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRMSTATUS_H__0770B2F7_60AE_11D5_8750_00D009619765__INCLUDED_)
