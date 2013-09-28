//{{AFX_INCLUDES()
#include "datagrid.h"
#include "adodc.h"
#include "SHDlg.h"
#include "ComboGenDlg.h"
#include "SelTemplate.h"
//#include "DopeEdit.h"
//#include "msdgridctrl.h"
//}}AFX_INCLUDES
#if !defined(AFX_PHSDATA_H__5A806D3D_7932_11D6_874B_00D009619765__INCLUDED_)
#define AFX_PHSDATA_H__5A806D3D_7932_11D6_874B_00D009619765__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PhsData.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPhsData dialog

class CPhsData : public CDialog
{
// Construction
public:
	int IsMouseMoving;
	long nRowStart,nRowEnd;
	long nRowBegin,nRowFinish;
	long RowLT;
	long PositionLT,PositionLLT;
	BOOL IsPastable;
	long nShresh;
	VARIANT vZeroRow;
	CSHDlg SHDlg;
	CMenu m_Menu;
	CMenu m_PopUpMenu;
	CComboBox m_ComboBox_Gen,m_ComboBox_Guan;
	CComboGenDlg m_ComboGenDlg;
	//SelTemplate Dlg;

	void ClearBookmarks();
	int UpOrDown(long Y);
	long GetFirstRow();
	long GetAbsoluteRow(long Row);
	long GetRowsCount();
	long RowsInOnePage();
	void UpdateMenu();
	void UpdateVector(store& bIsVisible);
	void HideComboBox();
	void ShowComboBox(long X, long Y);

private:
public:
	bool m_AutoDZ;
	_RecordsetPtr m_DataBillRs;
	_RecordsetPtr m_DataSumRs;
	_RecordsetPtr m_ActiveRs;
	CPhsData(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPhsData)
	enum { IDD = IDD_PHSDATA };
	CEdit	m_TextINPUT;
	CStatic	m_cRawData;
	CStatic	m_cStuLab;
	CString	m_RawData;
	CString	m_StuLab;
	CDataGrid	m_DBGbill;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPhsData)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL Create(UINT IDD ,CWnd* pParentWnd=NULL); 
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateLabel();
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	bool m_AddNewStat;

	// Generated message map functions
	//{{AFX_MSG(CPhsData)
	
	afx_msg void OnMove(int x,int y);
	afx_msg 	void OnActivate(UINT nState, CWnd *pWndOther, BOOL bMinimized);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnRowColChangeDBGbill(VARIANT FAR* LastRow, short LastCol);
	afx_msg void OnBeforeUpdateDBGbill(short FAR* Cancel);
	afx_msg void OnMouseUpDBGbill(short Button, short Shift, long X, long Y);
	afx_msg void OnBeforeDeleteDBGbill(short FAR* Cancel);
	afx_msg void OnTabExit();
	afx_msg void OnRetMainMnu();
	afx_msg void OnAutoML();
	afx_msg void OnAutoDZ();
	afx_msg void OnDelRs();
	afx_msg void OnSumMaterial();
	afx_msg void OnSumDisplay();
	afx_msg void OnErrorDatagrid1(short DataError, short FAR* Response);
	afx_msg void OnEditDelTab();
	afx_msg void OnDataCal();
	afx_msg void OnDblClickDatagrid1();
	afx_msg void OnDrawZdjh();
	afx_msg void OnEditAdd();
	afx_msg void OnEditHideColumn();
	afx_msg void OnEditShowColumns();
	afx_msg void OnEditSelectAllRows();
	afx_msg void OnMouseMoveDatagrid1(short Button, short Shift, long X, long Y);
	afx_msg void OnMouseDownDatagrid1(short Button, short Shift, long X, long Y);
	afx_msg void OnEditCopyRs();
	afx_msg void OnEditPasteRs();
	afx_msg void OnEditDelRs();
	afx_msg void OnExportToExcel();
	afx_msg void OnUndoPasteRs();
	afx_msg void OnEditCopyCol();
	afx_msg void OnEditDelCol();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnScrollDatagrid1(short FAR* Cancel);
	afx_msg void OnKeyDownDatagrid1(short FAR* KeyCode, short Shift);
	afx_msg void OnClose();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CRect Rect,RectDataGrid,RectCombol;
	bool m_gridDel;
public:
	virtual  ~CPhsData();
	bool m_bIsAlDel;
	bool m_IsDelRs;
	bool m_RepError;
	bool m_bActive;
	void CloseRs();
	void InitDBbill();
	
};
extern CPhsData FrmPhsData;
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PHSDATA_H__5A806D3D_7932_11D6_874B_00D009619765__INCLUDED_)
