#if !defined(AFX_DLGDBOPER_H__674AC3F5_C982_4151_8F14_BEF7B105C8CA__INCLUDED_)
#define AFX_DLGDBOPER_H__674AC3F5_C982_4151_8F14_BEF7B105C8CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgDBOper.h : header file
//
#include "datagridEx.h"
#include "XListBox.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgDBOper dialog

class CDlgDBOper : public CDialog
{
// Construction
private:
	int InitDlgData();
	int PromptSaveChange();
	int DisplayAllTable();
	int InitDlgWithoutPhs();
	int InitDlgWithPhs();
	int SetTableList(int index = 0,int iWarn = 1);
	int InitDlgCommFormat();
	int SetTableDGFromListBox(int index,int iWarn = 1);
	int ResetTableList();

public:

	int CloseDlg();

	int OpenDB(CString strDBName,int iWarn = 1);
	int OpenDB(int iWarn = 1);
	CDlgDBOper(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgDBOper)
	enum { IDD = IDD_DLG_DBOPER };
	CButton	m_ButUndoChange;
	CButton	m_ButSaveChange;
	CButton	m_CheckDispAllTable;
	CButton	m_CheckAutoSave;
	CXListBox	m_StandardList;
	CStatic	m_StaticRecordPos;
	CButton	m_CheckAllowEdit;
	CButton	m_CheckAllowDelete;
	CButton	m_CheckAllowAppend;
	CButton	m_ButSetDescribe;
	CXListBox	m_TableTypeList;
	CXListBox	m_TableList;
	CDataGridEx	m_TableDG;
	//}}AFX_DATA
	//CStatusBar m_StatusBar;   //状态栏变量

	
	_ConnectionPtr m_pSortCon;  //Sort数据库连接对象指针
	_ConnectionPtr m_pCon;//工作数据库对象指针
	//之所以定义 m_pDGCon,是因为在本对话框主界面中的查看到的数据表所在的数据库对象有变化
	//当查看的是当前系统数据表 zdjcrude表时，会从Sort表中读取管理表，因此查看管理表时要
	//从Sort表中读取管理表内容，为方便以后的编程，如事务处理，再定义这样一个指针
	_ConnectionPtr m_pDGCon;   //当前所查看数据表所在的数据库对象指针
	_RecordsetPtr m_pRs;   //当前工作表对象指针
	_RecordsetPtr m_pCurPhsManuTbRs;   //当前工作数据库中的当前管理表记录集对象指针
	_RecordsetPtr m_pPhsManuTbRs;   //指向SORT数据库中PhsManuTbDescribe的记录集指针
	_RecordsetPtr m_pPhsManuTbStruRs;//指向SORT数据库中PhsManuTbStruDescribe的记录集指针
	_RecordsetPtr m_pPhsDataRs;      //指向SORT数据库中PhsDataTbStruDescribe的记录集指针
	_RecordsetPtr m_pTRs;  //临时指针

	CString m_strDBName;    //当前工作数据库名
	CString m_strDBPassword; //当前工作数据库密码

	//CWnd *m_pCallWnd;             //用于别的窗口调用本窗口时,存储该窗口的句柄指针	
	bool m_bAutoOpen;		//true 为当发现 m_strDBName不为空时,窗口初始化时自动打开该数据库

	CString m_strTableType;       //用于外部调用本窗口时,存储要设置的数据表类型
	CString m_strTableStandard;   //用于外部调用本窗口时,存储要设置的数据表标准

private:
	CString m_strCurTbName;//当前工作数据表表名
	CString m_strCurTableType;//当前工作数据表类型
	CString m_strCurManuTbName;//当前工作数据表的管理表名
	CString m_strTSQL;         //临时SQL语句
	CString m_strTemp;

	int m_iRet;          //用于接受整型函数返回值
	int m_iTemp;

	int m_iModified;             //修改标志
	bool m_bPHSManu;             //打前打开数据表是否为管理表   

	long m_lRun;          //当前对话框启动后DATAGRID连接数据表的次数

	CRect m_rcMainDlg;
	int m_iH;    //控件原高
	int m_iW;    //控件原宽
	int m_fPerH;






// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgDBOper)
	public:
	virtual BOOL Create(UINT IDD ,CWnd* pParentWnd=NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	DECLARE_DYNCREATE(CDlgDBOper)
	// Generated message map functions
	//{{AFX_MSG(CDlgDBOper)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnOpenDB();
	virtual void OnCancel();
	afx_msg void OnClosecommdbdlg();
	afx_msg void OnSelchangeTablelist();
	afx_msg void OnSelchangeTabletypelist();
	afx_msg void OnSelchangeStandardlist();
	afx_msg void OnCheckallowappend();
	afx_msg void OnCheckallowdelete();
	afx_msg void OnCheckallowedit();
	afx_msg void OnCheckautosave();
	afx_msg void OnChangeDatagrid();
	afx_msg void OnCheckdispalltable();
	afx_msg void OnClose();
	afx_msg void OnButclosedlg();
	afx_msg void OnButsavechange();
	afx_msg void OnButundochange();
	afx_msg void OnAfterDeleteDatagrid();
	virtual void OnOK();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//declspec(dllexport) extern CDlgDBOper dlgDBOper;
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGDBOPER_H__674AC3F5_C982_4151_8F14_BEF7B105C8CA__INCLUDED_)
