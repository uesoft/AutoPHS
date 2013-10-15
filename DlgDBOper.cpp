// DlgDBOper.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "DlgDBOper.h"
#include "mainFrm.h"
#include "FileDialogEx.h"
#include "CommDBOper.h"
#include "basDirectory.h"
#include "user.h"
#include "columns.h"
#include "Column.h"
#include "modregistry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgDBOper dialog


IMPLEMENT_DYNCREATE(CDlgDBOper, CDialog)
//_declspec(dllexport) CDlgDBOper dlgDBOper;
/*static UINT indicators[] =
{

	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};*/



CDlgDBOper::CDlgDBOper(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDBOper::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgDBOper)
	m_bAutoOpen = false;
	m_pDGCon = NULL;
	m_pCon = NULL;
	m_pSortCon = NULL;
	//}}AFX_DATA_INIT
}


void CDlgDBOper::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDBOper)
	DDX_Control(pDX, IDC_BUTUNDOCHANGE, m_ButUndoChange);
	DDX_Control(pDX, IDC_BUTSAVECHANGE, m_ButSaveChange);
	DDX_Control(pDX, IDC_CHECKDISPALLTABLE, m_CheckDispAllTable);
	DDX_Control(pDX, IDC_CHECKAUTOSAVE, m_CheckAutoSave);
	DDX_Control(pDX, IDC_STANDARDLIST, m_StandardList);
	DDX_Control(pDX, IDC_STATICRECORDPOS, m_StaticRecordPos);
	DDX_Control(pDX, IDC_CHECKALLOWEDIT, m_CheckAllowEdit);
	DDX_Control(pDX, IDC_CHECKALLOWDELETE, m_CheckAllowDelete);
	DDX_Control(pDX, IDC_CHECKALLOWAPPEND, m_CheckAllowAppend);
	DDX_Control(pDX, IDC_BUTSETDESCRIBE, m_ButSetDescribe);
	DDX_Control(pDX, IDC_TABLETYPELIST, m_TableTypeList);
	DDX_Control(pDX, IDC_TABLELIST, m_TableList);
	DDX_Control(pDX, IDC_DATAGRID, m_TableDG);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgDBOper, CDialog)
	//{{AFX_MSG_MAP(CDlgDBOper)
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_WM_CTLCOLOR()
	ON_COMMAND(IDM_OPENDB, OnOpenDB)
	ON_BN_CLICKED(IDM_CLOSECOMMDBDLG, OnClosecommdbdlg)
	ON_LBN_SELCHANGE(IDC_TABLELIST, OnSelchangeTablelist)
	ON_LBN_SELCHANGE(IDC_TABLETYPELIST, OnSelchangeTabletypelist)
	ON_LBN_SELCHANGE(IDC_STANDARDLIST, OnSelchangeStandardlist)
	ON_BN_CLICKED(IDC_CHECKALLOWAPPEND, OnCheckallowappend)
	ON_BN_CLICKED(IDC_CHECKALLOWDELETE, OnCheckallowdelete)
	ON_BN_CLICKED(IDC_CHECKALLOWEDIT, OnCheckallowedit)
	ON_BN_CLICKED(IDC_CHECKAUTOSAVE, OnCheckautosave)
	ON_BN_CLICKED(IDC_CHECKDISPALLTABLE, OnCheckdispalltable)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTCLOSEDLG, OnButclosedlg)
	ON_BN_CLICKED(IDC_BUTSAVECHANGE, OnButsavechange)
	ON_BN_CLICKED(IDC_BUTUNDOCHANGE, OnButundochange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgDBOper message handlers



BOOL CDlgDBOper::Create(UINT IDD ,CWnd* pParentWnd)
{
	return CDialog::Create(IDD, pParentWnd);
}

BOOL CDlgDBOper::OnInitDialog() 
{
	CDialog::OnInitDialog();
	InitDlgData();
	InitDlgWithPhs();
	
	m_TableDG.InitMouseWheel();//滚动功能 
	m_lRun = 0;

	COLORREF virColor = 0xff0000;
	m_TableList.SetVirtualTextColor(virColor); //设置不存在表在数据表列表框中显示颜色
	m_ButSaveChange.EnableWindow(false);  //保存按钮置灰
	m_ButUndoChange.EnableWindow(false);  //取消按钮置灰
	CString strTemp;
	strTemp.Format("0");
	m_strTemp = GetRegKey(_T("Settings"),_T("DlgDbOperAutoSaveChange"),strTemp);
	if (m_strTemp == "1")
	{
		m_CheckAutoSave.SetCheck(1);
	}
	else
	{
		m_CheckAutoSave.SetCheck(0);
	}

	CString strSQL;
	CString strError;
	CString strPassword;
	strPassword.Empty();
	strSQL.Format("%s%s",basDirectory::ProjectDBDir,_T("sort.mdb"));
	strError.Format("初始化数据库出错,对 %s 数据库 执行  %s 语句出错,请检查数据库!!!",strSQL,"%s");

	if (CCommDBOper::ADOOpenDBCon(m_pSortCon,strSQL,strPassword,0) == -1)
	{
		m_strTemp.Format("初始化出错!打开数据库 %s 出错,请检查数据库!!!",strSQL);
		AfxMessageBox(strTemp);
		OnClose();   //关闭本对话框
		return FALSE;
	}
	strSQL.Format("SELECT * FROM PhsManuTbDescribe");
	if (CCommDBOper::ADORecordsetOpen(m_pSortCon,m_pPhsManuTbRs,strSQL,"",0) == -1)
	{
		m_strTemp.Format(strError,strSQL);
		AfxMessageBox(m_strTemp);
		OnClose();   //关闭本对话框
		return FALSE;
	}

	strSQL.Format("SELECT * FROM PhsManuTbStruDescribe");
	if (CCommDBOper::ADORecordsetOpen(m_pSortCon,m_pPhsManuTbStruRs,
		strSQL,"",0) == -1)
	{

		m_strTemp.Format(strError,strSQL);
		AfxMessageBox(m_strTemp);
		CDialog::OnClose();   //关闭本对话框
		return FALSE;
	}
	
	strSQL.Format("SELECT * FROM PhsDataTbStruDescribe");
	if (CCommDBOper::ADORecordsetOpen(m_pSortCon,m_pPhsDataRs,strSQL,"",0) == -1)
	{
		m_strTemp.Format(strError,strSQL);
		AfxMessageBox(m_strTemp);
		OnClose();   //关闭本对话框
		return FALSE;
	}


	if (m_bAutoOpen == true && !m_strDBName.IsEmpty())
	{
		OpenDB(0);
		m_bAutoOpen = false;
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CDlgDBOper::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add extra initialization here
	HICON hIcon=::LoadIcon(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON_FRMDATA));
	SetIcon(hIcon,TRUE);
	CRect rc1;
	::SystemParametersInfo(SPI_GETWORKAREA,0,(LPVOID)&rc1,0);
	((CMainFrame*)AfxGetApp()->m_pMainWnd)->InitPos();
	this->SetWindowPos(NULL,0,((CMainFrame*)AfxGetApp()->m_pMainWnd)->GetMenuHeight(),rc1.Width(),
		rc1.Height()-((CMainFrame*)AfxGetApp()->m_pMainWnd)->GetMenuHeight(),SWP_NOZORDER|SWP_NOACTIVATE);	

	//建立状态栏
	//m_StatusBar.Create(this);
	//m_StatusBar.SetIndicators(indicators,sizeof(indicators)/sizeof(UINT));
	//RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0);
	return 0;
}


int CDlgDBOper::InitDlgData()
{
	m_CheckAllowAppend.SetCheck(0);
	m_CheckAllowDelete.SetCheck(0);
	m_CheckAllowEdit.SetCheck(0);
	OnCheckallowappend();
	OnCheckallowdelete();
	OnCheckallowedit();
	m_iModified = 0;
	return TRUE;  
	
}

void CDlgDBOper::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	CRect rc1;
	::SystemParametersInfo(SPI_GETWORKAREA,0,(LPVOID)&rc1,0);
	((CMainFrame*)AfxGetApp()->m_pMainWnd)->InitPos();
	this->SetWindowPos(NULL,0,((CMainFrame*)AfxGetApp()->m_pMainWnd)->GetMenuHeight(),rc1.Width(),
		rc1.Height()-((CMainFrame*)AfxGetApp()->m_pMainWnd)->GetMenuHeight(),SWP_NOZORDER|SWP_NOACTIVATE);	

	
}

HBRUSH CDlgDBOper::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	

	// TODO: Return a different brush if the default is not desired
	return hbr;
}

void CDlgDBOper::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	PromptSaveChange();  //保存更改
	CDialog::OnCancel();
}

void CDlgDBOper::OnOpenDB() 
{
	// TODO: Add your command handler code here
	CFileDialogEx fileDlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,_T("Database Files(*.mdb)|*.mdb|All Files(*.*)|*.*||"));
	if(fileDlg.DoModal()==IDOK)
	{
		m_strDBName=fileDlg.GetPathName();
		m_strDBPassword.Empty();
		//m_strCurTableName.Empty();
		OpenDB(0);
	}
	return;

}

int CDlgDBOper::SetTableDGFromListBox(int index,int iWarn)
{
	//显示当前工作数据表内容
	
	//m_ButSaveChange.EnableWindow(false);
	//m_ButUndoChange.EnableWindow(false);
	//如果标准列表框中未有记录,则不显示 "显示未存在支吊架表"检测框
	if (m_StandardList.GetCount() > 0 && m_TableList.GetCount() > 0 && m_StandardList.IsWindowVisible())
	{
		m_CheckDispAllTable.ShowWindow(SW_SHOW);
	}
	else
	{
		m_CheckDispAllTable.ShowWindow(SW_HIDE);
	}
	m_TableDG.EnableWindow(false);  

	if ((m_TableList.GetCount() > index) && (index >= 0))
	{
		PromptSaveChange();  //保存更改
		
		CString strSql;
		try
		{
			CCommDBOper::ADOCloseRecordset(m_pRs,0);
			if (m_pRs == NULL)
			{
				m_pRs.CreateInstance(__uuidof(Recordset));
				//m_pRs->CursorLocation = adUseClient;   //不能加这句语句,加入后会导致莫名其妙的错误
			}
			else if (m_pRs->State == adStateOpen)
			{
				m_pRs->Close();
			}
			
			
			m_TableList.GetText(index,m_strCurTbName);
			if (m_strCurTbName.Find("管理表",0) != -1)
			{
				//打开的是管理表
				m_strCurTbName.Delete(0,7);
				strSql.Format("SELECT * FROM %s",m_strCurTbName);
				m_pRs->Open(_variant_t(strSql),(IDispatch*)m_pSortCon,adOpenKeyset,adLockOptimistic,adCmdText);
				m_pDGCon = m_pSortCon;
				m_bPHSManu = true;
			}
			else
			{
				//打开的不是管理表,目前只能判断本软件工作数据库中的表
				strSql.Format("SELECT * FROM %s",m_strCurTbName);
				m_pRs->Open(_variant_t(strSql),(IDispatch*)m_pCon,adOpenKeyset,adLockOptimistic,adCmdText);
				m_pDGCon = m_pCon;
				m_bPHSManu = false;
			}
		}
		catch (_com_error e)
		{
			TRACE("Open Table ERROR:%s,%s\n",(char *)e.Description(),e.ErrorMessage());
			m_TableDG.SetRefDataSource(NULL);
			m_strTemp.Format("%s表 不存在(请在Access中建立此表)或正在使用中!!!",m_strCurTbName);
			m_TableDG.SetWindowText(m_strTemp);
			m_TableDG.Refresh();
			return FALSE;
		}

		//打开数据表成功
		m_lRun++;   //暂时没用

		//m_TableDG.ShowWindow(SW_SHOW);
		m_TableDG.SetRefDataSource(m_pRs);
		m_TableDG.SetWindowText(m_strCurTbName);
		m_TableDG.ReBind();

		try
		{
			m_pDGCon->BeginTrans();
		}
		catch (_com_error e)
		{
			AfxMessageBox(e.ErrorMessage());
			return FALSE;
		}

		try
		{
			if (m_pRs->RecordCount == 0 )
			{
				m_pRs->AddNew();
			}
			m_pRs->MoveFirst();
		}
		catch (...)
		{
			m_strTemp.Format("%s表中没有记录且含有关键字段,请在ACCESS中添加记录!!!",m_strCurTbName);
			m_TableDG.SetWindowText(m_strTemp);
			return FALSE;
		}

		m_TableDG.EnableWindow(); 
		//如果当前工作数据库中有支吊架数据管理表
		//显示帮助信息和替换DataGrid控件(IDC_DATAGRID)中的字段标题
		if (m_StandardList.GetCount() > 0)
		{
			m_pPhsManuTbStruRs->MoveFirst();
			CString strFieldName;
			while (!m_pPhsManuTbStruRs->adoEOF)
			{
				try 
				{
					strFieldName = vtos(m_pPhsManuTbStruRs->GetCollect("FieldName"));
					m_strTemp = vtos(m_pCurPhsManuTbRs->GetCollect(_variant_t(strFieldName))) ;
				}
				catch (_com_error e)
				{
					m_pPhsManuTbStruRs->MoveNext();
					continue;
				}
				if ( m_strTemp == m_strCurTbName)
				{
					//如果在phsManuTbStruDescribe表中查找到与当前工作表名相关联的记录
					//设置 m_TableDG控件标题为当前工作数据表名 + 其意思
					m_strTemp = vtos(m_pPhsManuTbStruRs->GetCollect("TableMeaning"));
					if ( !m_strTemp.IsEmpty() )
					{
						m_strTSQL.Format("表名 %s  作用: %s",m_strCurTbName,m_strTemp);
						m_TableDG.SetWindowText(m_strTSQL);
					}
					CString strPreField;   //要替换的当前字段名
					CString strPostField;  //替换后的当前字段名
					CColumns columns;
					columns = m_TableDG.GetColumns();
					FieldsPtr fields;
					fields = m_pRs->GetFields();
					
					m_strTSQL.Format("SELECT * FROM PhsDataTbStruDescribe \
						WHERE ManuTbName = '%s' AND ManuTbField = '%s'",m_strCurManuTbName,strFieldName);
					CCommDBOper::ADORecordsetOpen(m_pSortCon,m_pPhsDataRs,m_strTSQL,"",0);
					
					while (!m_pPhsDataRs->adoEOF)
					{
						strPreField = vtos(m_pPhsDataRs->GetCollect("FieldName"));
						strPostField = vtos(m_pPhsDataRs->GetCollect("FieldMeaning"));
						if (strPostField.IsEmpty())
						{
							//如果替换后的字段为空,则不替换
							m_pPhsDataRs->MoveNext();
							continue;
						}
						try
						{
							//如当前工作数据表中不存在要替换的当前字段
							fields->GetItem(_variant_t(strPreField));
						}
						catch (_com_error e)
						{
							m_pPhsDataRs->MoveNext();
							continue;
						}
						
						columns.GetItem(_variant_t(strPreField)).SetCaption(strPostField);
						m_pPhsDataRs->MoveNext();
					}
					break;
				}
				m_pPhsManuTbStruRs->MoveNext();
			}
		}
		
		
	}
	
	//如果打开的是管理表，则跳到当前管理表中的当前所选规范的哪条记录
	if (m_bPHSManu)
	{
		try
		{
			_variant_t varValue;
			CString strTemp;
			int iRow = 0;
			
			m_StandardList.GetText(m_StandardList.GetCurSel(),m_strTemp);
			m_strTemp.TrimLeft();
			m_strTemp.TrimRight();
			while (!m_pRs->adoEOF)
			{
				varValue = m_pRs->GetCollect(0L);
				strTemp = vtos (varValue);
				strTemp.TrimLeft();
				strTemp.TrimRight();
				if (m_strTemp == strTemp)
				{
					break;
				}
				iRow ++;
				m_pRs->MoveNext();
			}
			//m_TableDG.SetRow(iRow);
		}
		catch (...)
		{
			;
		}
	}

	return TRUE;
}






void CDlgDBOper::OnCancel() 
{
	// TODO: Add extra cleanup here
	//取消ESC键的影响
	//CDialog::OnCancel();
}



int CDlgDBOper::OpenDB(int iWarn)
{
	int iRet;
	m_TableTypeList.ResetContent();
	ResetTableList();

	//如果已经打开过数据库表,则要提交事务
	PromptSaveChange();

	if ( CCommDBOper::ADOOpenDBCon(m_pCon,m_strDBName,m_strDBPassword,0) != 0)
	{
		AfxMessageBox("数据库打开失败!!!");
		m_TableDG.SetRefDataSource(NULL);
		m_TableDG.SetWindowText("未打开数据表！！！");
		m_TableDG.EnableWindow(false);
		m_TableDG.Refresh();
		return FALSE;
	}
	this->SetWindowText("当前数据库    "+m_strDBName);
	

	try
	{
		m_pPhsManuTbRs->MoveFirst();
	}
	catch (_com_error e)
	{
		return FALSE;
	}



	_variant_t varFieldValue;
	CString strManuTbName;
	while (!m_pPhsManuTbRs->adoEOF)
	{
		strManuTbName = vtos(m_pPhsManuTbRs->GetCollect("ManuTbName"));
		m_strTSQL.Format("SELECT * FROM %s",strManuTbName);
		iRet = CCommDBOper::ADORecordsetOpen(m_pCon,m_pTRs,m_strTSQL,"",0);
		if (iRet == 0) //当前工作数据库中存在strManuTbName字段值管理表
		{
			m_TableTypeList.AddString(vtos(m_pPhsManuTbRs->GetCollect("ShortDescribe")));
		}
		m_pPhsManuTbRs->MoveNext();
	}
	m_TableTypeList.AddString("所有数据表");

	m_CheckDispAllTable.ShowWindow(SW_HIDE);  //暂时隐藏 "显示所有表" 检查框

	InitDlgData();

	if (m_TableTypeList.GetCount() > 1)
	{
		InitDlgWithPhs();
		if (m_strTableType.IsEmpty())
		{
			m_TableTypeList.SetCurSel(0);
		}
		else
		{
			m_TableTypeList.SetCurSel(m_TableTypeList.FindStringExact(-1,m_strTableType));
			m_strTableType.Empty();
		}
		OnSelchangeTabletypelist();
	}
	else
	{
		InitDlgWithoutPhs();
		DisplayAllTable();
	}

	return TRUE;
}

int CDlgDBOper::OpenDB(CString strDBName,int iWarn)
{
	m_strDBName = strDBName;
	//m_strCurTableName = strTableName;
	OpenDB(iWarn);
	return TRUE;
}



int CDlgDBOper::InitDlgCommFormat()
{
	//本函数用于初始化对话框通用界面
	CWnd *pWnd;
	this->GetClientRect(m_rcMainDlg);
	m_fPerH = m_rcMainDlg.Height() / 100;


	//初始化静态文本框(用于显示当前编辑编辑数据表信息)位置
	CRect rcStaticRecordPos;
	m_StaticRecordPos.GetWindowRect(rcStaticRecordPos);
	this->ScreenToClient(rcStaticRecordPos);
	m_iH = rcStaticRecordPos.Height();
	m_iW = rcStaticRecordPos.Width();
	rcStaticRecordPos.bottom = m_rcMainDlg.bottom - 2 * (int)m_fPerH;
	rcStaticRecordPos.top = rcStaticRecordPos.bottom - m_iH;
	//设置DataGrid(IDC_DATAGRID)的宽为 整个对话框的75%
	rcStaticRecordPos.left = m_rcMainDlg.Width() * 27/ 100;   
	rcStaticRecordPos.right = rcStaticRecordPos.left + m_iW;
	m_StaticRecordPos.MoveWindow(rcStaticRecordPos);
	

	//初始化数据表DataGrid控件位置
	CRect rcTableDG;
	rcTableDG.left = rcStaticRecordPos.left;
	rcTableDG.top = m_rcMainDlg.top + 2 * (int)m_fPerH;
	//rcTableDG.top = 2 * (int)m_fPerH;
	rcTableDG.right = m_rcMainDlg.right - (int)m_fPerH;
	rcTableDG.bottom = rcStaticRecordPos.top -  (int)m_fPerH;
	m_TableDG.MoveWindow(rcTableDG);



	//初始化选项检测按扭组位置
	CRect rcStaticSetCheck;
	pWnd = GetDlgItem(IDC_STATICSETCHECK);
	pWnd->GetWindowRect(rcStaticSetCheck);
	m_iH = rcStaticSetCheck.Height();
	m_iW = rcStaticSetCheck.Width();
	rcStaticSetCheck.right = rcTableDG.left - 2 * (int)m_fPerH;
	rcStaticSetCheck.top = m_rcMainDlg.top + 2 * (int)m_fPerH;
	rcStaticSetCheck.bottom = rcStaticSetCheck.top + m_iH;
	rcStaticSetCheck.left = rcStaticSetCheck.right - m_iW;
	pWnd->MoveWindow(rcStaticSetCheck);

	float fCheckH;
	CRect rcCheckAllowAppend;
	m_CheckAllowAppend.GetWindowRect(rcCheckAllowAppend);
	this->ScreenToClient(rcCheckAllowAppend);
	fCheckH = (rcStaticSetCheck.Height() - (rcCheckAllowAppend.Height() * 4)) / 6;//这里多除了一个1.6倍 
	m_iH = rcCheckAllowAppend.Height();
	m_iW = rcCheckAllowAppend.Width();
	rcCheckAllowAppend.left = rcStaticSetCheck.left + 3 * (int)m_fPerH;
	rcCheckAllowAppend.right = rcCheckAllowAppend.left + m_iW;
	rcCheckAllowAppend.top = rcStaticSetCheck.top + 2 * (int)fCheckH;
	rcCheckAllowAppend.bottom = rcCheckAllowAppend.top + m_iH;
	m_CheckAllowAppend.MoveWindow(rcCheckAllowAppend);

	CRect rcCheckAllowDelete;
	m_CheckAllowDelete.GetWindowRect(rcCheckAllowDelete);
	this->ScreenToClient(rcCheckAllowDelete);
	rcCheckAllowDelete.right = rcCheckAllowAppend.right;
	rcCheckAllowDelete.left =  rcCheckAllowAppend.left;
	rcCheckAllowDelete.top = rcCheckAllowAppend.bottom + fCheckH;
	rcCheckAllowDelete.bottom = rcCheckAllowDelete.top + m_iH;
	m_CheckAllowDelete.MoveWindow(rcCheckAllowDelete);

	CRect rcCheckAllowEdit;
	m_CheckAllowEdit.GetWindowRect(rcCheckAllowEdit);
	this->ScreenToClient(rcCheckAllowEdit);
	rcCheckAllowEdit.right = rcCheckAllowAppend.right;
	rcCheckAllowEdit.left =  rcCheckAllowAppend.left;
	rcCheckAllowEdit.top = rcCheckAllowDelete.bottom + fCheckH;
	rcCheckAllowEdit.bottom = rcCheckAllowEdit.top + m_iH;
	m_CheckAllowEdit.MoveWindow(rcCheckAllowEdit);

	CRect rcCheckAutoSave;
	m_CheckAutoSave.GetWindowRect(rcCheckAutoSave);
	rcCheckAutoSave.left = rcCheckAllowAppend.left;
	rcCheckAutoSave.right = rcCheckAllowAppend.right;
	rcCheckAutoSave.top = rcCheckAllowEdit.bottom + fCheckH;
	rcCheckAutoSave.bottom = rcCheckAutoSave.top + m_iH;
	m_CheckAutoSave.MoveWindow(rcCheckAutoSave);




	//设置按钮组的位置,将三按钮放在屏幕右下角
	CRect rcButCloseDlg;
	pWnd = GetDlgItem(IDC_BUTCLOSEDLG);
	pWnd->GetWindowRect(rcButCloseDlg);
	this->ScreenToClient(rcButCloseDlg);
	m_iH = rcButCloseDlg.Height();
	m_iW = rcButCloseDlg.Width();
	rcButCloseDlg.top = rcTableDG.bottom + m_fPerH;
	rcButCloseDlg.bottom = rcButCloseDlg.top + m_iH;
	rcButCloseDlg.right = m_rcMainDlg.right - m_fPerH;
	rcButCloseDlg.left = rcButCloseDlg.right - m_iW;
	pWnd->MoveWindow(rcButCloseDlg);
	
	CRect rcButSetDescribe;
	m_ButSetDescribe.GetWindowRect(rcButSetDescribe);
	this->ScreenToClient(rcButSetDescribe);
	rcButSetDescribe.top = rcButCloseDlg.top;
	rcButSetDescribe.bottom = rcButCloseDlg.bottom;
	rcButSetDescribe.right = rcButCloseDlg.left - 2 * (int)m_fPerH;
	rcButSetDescribe.left = rcButSetDescribe.right - m_iW;
	m_ButSetDescribe.MoveWindow(rcButSetDescribe);

	CRect rcButSaveChange;
	m_ButSaveChange.GetWindowRect(rcButSaveChange);
	this->ScreenToClient(rcButSaveChange);
	rcButSaveChange.top = rcButCloseDlg.top;
	rcButSaveChange.bottom = rcButCloseDlg.bottom;
	rcButSaveChange.right	= rcButSetDescribe.left - 2 * (int)m_fPerH;
	rcButSaveChange.left = rcButSaveChange.right - m_iW;
	m_ButSaveChange.MoveWindow(rcButSaveChange);

	CRect rcButUndoChange;
	m_ButUndoChange.GetWindowRect(rcButUndoChange);
	this->ScreenToClient(rcButUndoChange);
	rcButUndoChange.top = rcButCloseDlg.top;
	rcButUndoChange.bottom = rcButCloseDlg.bottom;
	rcButUndoChange.right	= rcButSaveChange.left - 2 * (int)m_fPerH;
	rcButUndoChange.left = rcButUndoChange.right - m_iW;
	m_ButUndoChange.MoveWindow(rcButUndoChange);

	
	

	// END 本函数用于初始化对话框通用界面
	return TRUE;
}

void CDlgDBOper::OnClosecommdbdlg() 
{
	// TODO: Add your control notification handler code here
	OnClose();
	return;
	
}

int CDlgDBOper::SetTableList(int index,int iWarn)
{
	return TRUE;
}

void CDlgDBOper::OnSelchangeTablelist() 
{
	// TODO: Add your control notification handler code here
	if (m_TableList.GetCount() > 0)
	{
		SetTableDGFromListBox(m_TableList.GetCurSel(),1);
	}
	return;
}

void CDlgDBOper::OnSelchangeTabletypelist() 
{
	// TODO: Add your control notification handler code here

	m_StandardList.ResetContent();
	m_StandardList.SetHorizontalExtent(400);
	CString strTableType;
	
	long index;
	index = m_TableTypeList.GetCurSel();
	m_TableTypeList.GetText(index,strTableType);
	if ( index < (m_TableTypeList.GetCount() - 1))
	{
		//如果未选择 "所有表格"
		m_pPhsManuTbRs->MoveFirst();
		while (!m_pPhsManuTbRs->adoEOF)
		{
			if (strTableType == vtos(m_pPhsManuTbRs->GetCollect("ShortDescribe")))
			{
				break;
			}
			m_pPhsManuTbRs->MoveNext();
		}
		if (m_pPhsManuTbRs->adoEOF)
		{
			AfxMessageBox("读取记录出错!!!");
			return;
		}
		m_strCurManuTbName = vtos(m_pPhsManuTbRs->GetCollect("ManuTBName"));
		m_strTSQL.Format("SELECT * FROM %s ORDER BY STANDARD",m_strCurManuTbName);
		
		//如果打开的是支吊架工作数据库 zdjcrude.mdb 则支吊架部件管理表从 sort表中读
		if (m_strDBName == (basDirectory::ProjectDBDir+_T("Zdjcrude.mdb")))
		{
			CCommDBOper::ADORecordsetOpen(m_pSortCon,m_pCurPhsManuTbRs,m_strTSQL,"",1);
		}
		else
		{
			CCommDBOper::ADORecordsetOpen(m_pCon,m_pCurPhsManuTbRs,m_strTSQL,"",1);
		}
		m_pCurPhsManuTbRs->MoveFirst();
		while(!m_pCurPhsManuTbRs->adoEOF)
		{
			m_StandardList.AddString(vtos(m_pCurPhsManuTbRs->GetCollect("Standard")));
			m_pCurPhsManuTbRs->MoveNext();
		}
		if (m_StandardList.GetCount() > 0)
		{
			if (m_strTableStandard.IsEmpty())
			{
				m_StandardList.SetCurSel(0);
			}
			else
			{
				m_StandardList.SetCurSel(m_StandardList.FindStringExact(-1,m_strTableStandard));
				m_strTableStandard.Empty();
			}
			OnSelchangeStandardlist();
		}
	}
	else
	{
		DisplayAllTable();
	}

	return;
}

void CDlgDBOper::OnSelchangeStandardlist() 
{
	// TODO: Add your control notification handler code here
	long index;
	index = m_StandardList.GetCurSel();
	CString strStandard;
	m_StandardList.GetText(index,strStandard);
	
	ResetTableList();

	m_pCurPhsManuTbRs->MoveFirst();
	while (!m_pCurPhsManuTbRs->adoEOF)
	{
		if (strStandard == vtos(m_pCurPhsManuTbRs->GetCollect("Standard")))
		{
			break;
		}
		m_pCurPhsManuTbRs->MoveNext();
	}
	if (m_pCurPhsManuTbRs->adoEOF)
	{
		AfxMessageBox("读取记录出错!!!");
		return;
	}

	m_strTSQL.Format("SELECT * FROM PhsManuTbStruDescribe WHERE ManuTBName = '%s'\
		AND FieldValueIsTBName = '1'",m_strCurManuTbName);
	CCommDBOper::ADORecordsetOpen(m_pSortCon,m_pPhsManuTbStruRs,m_strTSQL,"",0);
	
	m_pPhsManuTbStruRs->MoveFirst();
	CString strFieldName;
	
	
	//m_TableList.AddString("管理表:"+m_strCurManuTbName);   LFX 2005.4.7取消掉 
	while (!m_pPhsManuTbStruRs->adoEOF)
	{
		strFieldName = vtos(m_pPhsManuTbStruRs->GetCollect("FieldName"));
		m_strTemp = vtos(m_pCurPhsManuTbRs->GetCollect(_variant_t(strFieldName)));
		m_strTSQL.Format("SELECT * FROM %s",m_strTemp);
		m_iRet = CCommDBOper::ADORecordsetOpen(m_pCon,m_pTRs,m_strTSQL,"",0);
		if (m_iRet >= 0)   //打开成功
		{
			m_iTemp = m_TableList.AddString(m_strTemp);
		}
		else
		{
			if (m_CheckDispAllTable.GetCheck())
			{
				m_iTemp = m_TableList.AddString(m_strTemp);
				m_TableList.SetVirtual(m_iTemp);   //如表不存在,添上标记
			}
		}
		m_pPhsManuTbStruRs->MoveNext();
		
	}

	m_TableList.AddString("管理表:"+m_strCurManuTbName);  //lfx 2005.4.7 加
	if (m_TableList.GetCount() > 0)
	{
		m_TableList.SetCurSel(0);
		OnSelchangeTablelist();
	}
	else
	{
		m_TableDG.SetRefDataSource(NULL);
		m_TableDG.Refresh();
	}
	return;
}

int CDlgDBOper::InitDlgWithPhs()
{
	//初始化对话框以支吊架界面

	InitDlgCommFormat();
	
	//初始化对话框中支吊架相关部分控件位置
	CWnd *pWnd;
	
	CRect rcStaticTableType;
	pWnd = GetDlgItem(IDC_SELECTTABLETYPE);
	pWnd->GetWindowRect(&rcStaticTableType);
	this->ScreenToClient(&rcStaticTableType);
	rcStaticTableType.right = rcStaticTableType.Width() + 5;
	rcStaticTableType.bottom = rcStaticTableType.Height() + 3;
	rcStaticTableType.left = 5;
	rcStaticTableType.top = 3;
	pWnd->MoveWindow(rcStaticTableType);
	pWnd->ShowWindow(SW_SHOW);

	CRect rcTableTypeList;
	m_TableTypeList.GetWindowRect(&rcTableTypeList);
	this->ScreenToClient(&rcTableTypeList);
	rcTableTypeList.right = rcTableTypeList.Width() + (int)m_fPerH;
	rcTableTypeList.left = 5;
	rcTableTypeList.top = rcStaticTableType.bottom + m_fPerH;
	rcTableTypeList.bottom = rcTableTypeList.top + (int)m_fPerH * 20;
	m_TableTypeList.MoveWindow(rcTableTypeList);
	m_TableTypeList.ShowWindow(SW_SHOW);

	CRect rcSelStand;
	pWnd = GetDlgItem(IDC_SELECTSTANDARD);
	pWnd->GetWindowRect(rcSelStand);
	this->ScreenToClient(rcSelStand);
	m_iW = rcSelStand.Width();
	m_iH = rcSelStand.Height();
	rcSelStand.left = rcTableTypeList.left;
	rcSelStand.right = rcSelStand.left + m_iW;
	rcSelStand.top = rcTableTypeList.bottom + (int)m_fPerH;
	rcSelStand.bottom = rcSelStand.top + m_iH;
	pWnd->MoveWindow(rcSelStand);
	pWnd->ShowWindow(SW_SHOW);

	CRect rcStandList;
	m_StandardList.GetWindowRect(rcStandList);
	this->ScreenToClient(rcStandList);
	rcStandList.left = rcTableTypeList.left;
	rcStandList.right = m_rcMainDlg.Width() * 26 /100;
	rcStandList.top = rcSelStand.bottom + (int)m_fPerH;
	rcStandList.bottom =  rcStandList.top + m_fPerH * 20;
	m_StandardList.MoveWindow(rcStandList);
	m_StandardList.ShowWindow(SW_SHOW);

	CRect rcStaticSelTable;
	pWnd = GetDlgItem(IDC_STATICSELTABLE);
	pWnd->GetWindowRect(rcStaticSelTable);
	this->ScreenToClient(rcStaticSelTable);
	m_iH = rcStaticSelTable.Height();
	m_iW = rcStaticSelTable.Width();
	rcStaticSelTable.left = rcTableTypeList.left;
	rcStaticSelTable.right = rcStaticSelTable.left + m_iW;
	rcStaticSelTable.top = rcStandList.bottom + m_fPerH;
	rcStaticSelTable.bottom = rcStaticSelTable.top + m_iH;
	pWnd->MoveWindow(rcStaticSelTable);

	CRect rcCheckDispAllTable;
	m_CheckDispAllTable.GetWindowRect(rcCheckDispAllTable);
	rcCheckDispAllTable.top = rcStaticSelTable.top;
	rcCheckDispAllTable.bottom = rcStaticSelTable.bottom;
	m_iW = rcCheckDispAllTable.Width();
	rcCheckDispAllTable.left = rcStaticSelTable.right + 3 * (int)m_fPerH;
	rcCheckDispAllTable.right = rcCheckDispAllTable.left + m_iW;
	m_CheckDispAllTable.MoveWindow(rcCheckDispAllTable);
	m_CheckDispAllTable.ShowWindow(SW_SHOW);

	CRect rcTableList;
	m_TableList.GetWindowRect(rcTableList);
	rcTableList.left = rcTableTypeList.left;
	rcTableList.right = rcStandList.right;
	rcTableList.top = rcStaticSelTable.bottom + (int)m_fPerH;
	rcTableList.bottom = m_rcMainDlg.bottom - (int)m_fPerH;
	m_TableList.MoveWindow(rcTableList);

	return TRUE;
}

int CDlgDBOper::InitDlgWithoutPhs()
{
	//初始化对话杠以普通数据库界面
	InitDlgCommFormat();
	
	CWnd *pWnd;
	pWnd = GetDlgItem(IDC_SELECTTABLETYPE);
	pWnd->ShowWindow(SW_HIDE);
	m_TableTypeList.ShowWindow(SW_HIDE);
	pWnd = GetDlgItem(IDC_SELECTSTANDARD);
	pWnd->ShowWindow(SW_HIDE);
	m_StandardList.ShowWindow(SW_HIDE);
	
	CRect rcStaticSetCheck;
	pWnd = GetDlgItem(IDC_STATICSETCHECK);
	pWnd->GetWindowRect(rcStaticSetCheck);
	this->ScreenToClient(rcStaticSetCheck);
	
	CRect rcStaticSelTable;
	pWnd = GetDlgItem(IDC_STATICSELTABLE);
	pWnd->GetWindowRect(rcStaticSelTable);
	this->ScreenToClient(rcStaticSelTable);
	m_iH = rcStaticSelTable.Height();
	m_iW = rcStaticSelTable.Width();
	rcStaticSelTable.left = 5;
	rcStaticSelTable.right =rcStaticSelTable.left + m_iW;
	rcStaticSelTable.top = rcStaticSetCheck.bottom + m_fPerH;
	rcStaticSelTable.bottom = rcStaticSelTable.top + m_iH;
	pWnd->MoveWindow(rcStaticSelTable);

	CRect rcTableList;
	m_TableList.GetWindowRect(rcTableList);
	rcTableList.left = rcStaticSelTable.left;
	rcTableList.right = m_rcMainDlg.Width() * 26 /100;
	rcTableList.top = rcStaticSelTable.bottom + (int)m_fPerH;
	rcTableList.bottom = m_rcMainDlg.bottom - (int)m_fPerH;
	m_TableList.MoveWindow(rcTableList);
	
	return TRUE;
}

int CDlgDBOper::DisplayAllTable()
{
	//在数据表列表框中显示所有数据表
	CDaoDatabase db;
	CDaoTableDefInfo tbInfo;
	ResetTableList();

	try
	{
		db.Open(m_strDBName,FALSE,TRUE,_T(";pwd=") + m_strDBPassword);
		int i,c;
		c=db.GetTableDefCount();
		for(i=0;i<c;i++)
		{
			db.GetTableDefInfo(i,tbInfo);
			if(!(tbInfo.m_lAttributes & dbSystemObject) && !(tbInfo.m_lAttributes & dbHiddenObject))
			{
				m_TableList.AddString(tbInfo.m_strName);
			}
		}
		db.Close();
		if (m_TableList.GetCount() > 0)
		{
			m_TableList.SetCurSel(0);
			OnSelchangeTablelist();
		}
		else
		{
			m_TableDG.SetRefDataSource(NULL);
			m_TableDG.Refresh();
		}
	}

	catch(...)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}
	
	return TRUE;
}

void CDlgDBOper::OnCheckallowappend() 
{
	// TODO: Add your control notification handler code here
	m_TableDG.SetAllowAddNew(m_CheckAllowAppend.GetCheck());
	if (m_CheckAllowAppend.GetCheck() == 1)
	{
		m_CheckAllowEdit.SetCheck(true);
		OnCheckallowedit();
	}
}

void CDlgDBOper::OnCheckallowdelete() 
{
	// TODO: Add your control notification handler code here
	m_TableDG.SetAllowDelete(m_CheckAllowDelete.GetCheck());
	
}

void CDlgDBOper::OnCheckallowedit() 
{
	// TODO: Add your control notification handler code here
	m_TableDG.SetAllowUpdate(m_CheckAllowEdit.GetCheck());
	
}

BEGIN_EVENTSINK_MAP(CDlgDBOper, CDialog)
    //{{AFX_EVENTSINK_MAP(CDlgDBOper)
	ON_EVENT(CDlgDBOper, IDC_DATAGRID, 211 /* Change */, OnChangeDatagrid, VTS_NONE)
	ON_EVENT(CDlgDBOper, IDC_DATAGRID, 202 /* AfterDelete */, OnAfterDeleteDatagrid, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()


void CDlgDBOper::OnCheckautosave() 
{
	// TODO: Add your control notification handler code here
	CString strTemp;

	if (m_CheckAutoSave.GetCheck())
	{
		strTemp.Format("1");
		SetRegValue("Settings","DlgDbOperAutoSaveChange",strTemp);
	}
	else
	{
		strTemp.Format("0");
		SetRegValue("Settings","DlgDbOperAutoSaveChange",strTemp);
	}
}

void CDlgDBOper::OnChangeDatagrid() 
{
	// TODO: Add your control notification handler code here

	//修改标志置1   取消,保存按钮有效
	m_ButSaveChange.EnableWindow(true);
	m_ButUndoChange.EnableWindow(true);
	if (m_iModified == 0)
	{
		m_iModified = 1;
	}
	
}

int CDlgDBOper::PromptSaveChange()
{
	try
	{
		if (m_iModified == 1)
		{

			if (m_CheckAutoSave.GetCheck())
			{
				//如果设置了自动保存,则提交事务
				m_TableDG.UpdateData();
				m_pRs->Update();
				m_pDGCon->CommitTrans();
			}
			else
			{
				//如果没有设置自动保存,则出示提示
				if (AfxMessageBox("你已经修改了数据表,要保存你做的修改吗?",MB_OKCANCEL) == IDOK)
				{
					//m_TableDG.UpdateData();
					m_pRs->Update();
					m_pDGCon->CommitTrans();
				}
				else
				{
					m_pDGCon->RollbackTrans();
				}
			}
		}
		else if (m_pRs != NULL && m_pRs->State == adStateOpen)
		{
			//m_pCon->RollbackTrans();
		}
	}
	catch (_com_error e)
	{
		CString strTemp;
		strTemp.Format("更改表内容失败:  %s",e.ErrorMessage());
		AfxMessageBox(strTemp);
	}


	try
	{
		m_TableDG.UpdateData();
		m_pRs->CancelUpdate();
		m_pDGCon->RollbackTrans();
	}
	catch (...)
	{
		;
	}

	try
	{
		//if (m_pRs != NULL && m_pRs->State == adStateOpen)
		//m_TableDG.Refresh();
	}
	catch (...)
	{
		;
	}
	m_iModified = 0;
	m_ButSaveChange.EnableWindow(false);
	m_ButUndoChange.EnableWindow(false);
	return TRUE;
}


void CDlgDBOper::OnCheckdispalltable() 
{
	// TODO: Add your control notification handler code here
	if (m_pCon != NULL && m_pCon->State == adStateOpen && m_StandardList.GetCount() > 0)
	{
		OnSelchangeStandardlist();
	}
}

int CDlgDBOper::CloseDlg()
{
	//关闭本对话框
	OnClose();
	return TRUE;
}


int CDlgDBOper::ResetTableList()     //清空数据表列表框内容
{
	while (!m_TableList.m_lstVirtualItem.IsEmpty())   //清空未存在表颜色标记(m_lstVirtualItem)队列
	{
		m_TableList.m_lstVirtualItem.RemoveTail();
	}
	m_CheckAllowEdit.SetFocus();
	m_TableList.SetCurSel(-1);
	m_TableList.ResetContent();  
	return TRUE;
}



void CDlgDBOper::OnButclosedlg()
{
	OnClose();
}

void CDlgDBOper::OnButsavechange() 
{
	// TODO: Add your control notification handler code here
	int iCheck;
	iCheck = m_CheckAutoSave.GetCheck();  //保存 "自动保存" 检查框原始值
	m_CheckAutoSave.SetCheck(TRUE);   
	PromptSaveChange();   
	m_pCon->BeginTrans();    //再次开始事务处理
	m_CheckAutoSave.SetCheck(iCheck);	  //恢复 "自动保存" 检查框原始值
}

void CDlgDBOper::OnButundochange() 
{
	// TODO: Add your control notification handler code here
	m_iModified = 0;    //修改标志置0   这样在下面调用PromptSaveChange()函数时自动取消所作修改
	OnSelchangeTablelist();   
}

void CDlgDBOper::OnAfterDeleteDatagrid() 
{
	// TODO: Add your control notification handler code here
	m_ButSaveChange.EnableWindow(true);
	m_ButUndoChange.EnableWindow(true);
	if (m_iModified == 0)
	{
		m_iModified = 1;
	}
}

void CDlgDBOper::OnOK() 
{
	// TODO: Add extra validation here
	
	//CDialog::OnOK();
}
