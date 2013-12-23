// DlgFindAutoPSAFile.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "DlgFindAutoPSAFile.h"
#include "modregistry.h"
#include "user.h"
#include "modphscal.h"
#include "EDIBgbl.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgFindAutoPSAFile dialog


CDlgFindAutoPSAFile::CDlgFindAutoPSAFile(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFindAutoPSAFile::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgFindAutoPSAFile)
    m_iFileOpen = 0;
	//}}AFX_DATA_INIT
	m_bFlag = 0 ;
}


void CDlgFindAutoPSAFile::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgFindAutoPSAFile)
	DDX_Control(pDX, IDC_LIST1, m_ctrlFileList);
    DDX_Radio(pDX, IDC_RADIO_PHS, m_iFileOpen);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgFindAutoPSAFile, CDialog)
	//{{AFX_MSG_MAP(CDlgFindAutoPSAFile)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkFind)
	ON_BN_CLICKED(IDCONFIRM, OnConfirm)
	ON_BN_CLICKED(IDC_RADIO_PHS, OnRecenfile)
	ON_BN_CLICKED(IDC_RADIO_PSA, OnRecenfile)
	ON_BN_CLICKED(IDC_RADIO_CAESARII, OnRecenfile)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, OnClickList1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgFindAutoPSAFile message handlers

BOOL CDlgFindAutoPSAFile::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_ctrlFileList.InsertColumn(0, "序号", LVCFMT_LEFT, 40);
	m_ctrlFileList.InsertColumn(2, "文件路径", LVCFMT_LEFT, 500);

	DWORD styles = m_ctrlFileList.GetExtendedStyle();
	m_ctrlFileList.SetExtendedStyle( styles|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES );

	modPHScal::g_iPSAFilterIndex=1;//文件对话框中文件类型设为AutoPSA/CAESARII(*.out,*.mdb)类型

	if (GetPHSRecentFile() == 0)
	{ //PHS没有打开过文件      
        if (GetPSARecentFile() != 0)
		{//PSA打开过文件
            m_iFileOpen = 1;
	        this->UpdateData(false);
		}
    }
	else
	{
		m_iFileOpen = 0;
	    this->UpdateData(false);
	}
	
	if( m_nCount == 0 )
	{//PSA和PHS都没有打开过文件
		m_bFlag = 0;//当没有文件时,不弹出对话框.m_Flag变量作为一个状态
		CDialog::OnOK();
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//双击.
//以选择的文件名作为初始路径.
void CDlgFindAutoPSAFile::OnDblclkFind(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int pos = m_ctrlFileList.GetSelectionMark();	
	if( pos >= 0 && pos < m_nCount )
	{
		modPHScal::gsPSAOutFilePath = m_strFileList[pos];
		CDialog::OnOK();
	}
	*pResult = 0;
	m_bFlag = 1;  //选择状态.
}

void CDlgFindAutoPSAFile::OnOK() 
{
	//选择浏览 
	//modPHScal::gsPSAOutFilePath = "";//pfg20050309
	this->m_bFlag = 2;
	CDialog::OnOK();
}

void CDlgFindAutoPSAFile::OnCancel() 
{
	m_bFlag = 0;
	CDialog::OnCancel();
}

void CDlgFindAutoPSAFile::OnConfirm() 
{
	//选择确定
    int pos = m_ctrlFileList.GetSelectionMark();	
	if( pos >= 0 && pos < m_nCount )
	{
		modPHScal::gsPSAOutFilePath = m_strFileList[pos];
		m_bFlag = 1;  //选择状态.
		CDialog::OnOK();
	}
//	*pResult = 0;
}

/*****************************************************
 函数功能：根据用户的选择来显示最近打开文件的路径
 输入参数:无
 输出参数:无
 返回值:无
*****************************************************/
void CDlgFindAutoPSAFile::OnRecenfile()
{
	this->UpdateData(true);
    m_ctrlFileList.DeleteAllItems();//清除list ctrl控件中显示的项

	if (m_iFileOpen == 2)
	{//选择从CAESARII打开的文件
		modPHScal::g_iPSAFilterIndex=3;//文件对话框中文件类型设为CAESARII(*.mdb)类型
        GetCAESARIIRecentFile();
	}
	else if (m_iFileOpen == 1)
	{//选择从PSA打开的文件
		modPHScal::g_iPSAFilterIndex=2;//文件对话框中文件类型设为AutoPSA(*.out)类型
        GetPSARecentFile();
	}
	else
	{//选择从PHS打开的文件
		modPHScal::g_iPSAFilterIndex=1;//文件对话框中文件类型设为AutoPSA/CAESARII(*.out,*.mdb)类型
        GetPHSRecentFile();
	}
}

/**************************************************************************
 函数功能：从注册表中得到PSA最近打开的文件路径,并在list ctrl控件中显示出来
 输入参数:无
 输出参数:无
 返回值:得到以显示的从PSA打开的文件的条数
***************************************************************************/
int CDlgFindAutoPSAFile::GetPSARecentFile()
{
	  CString strValue;//文件路径 
	  CString strItem;//序号
	  CString strPathNameKey;//AutoPSA7.0或1.1路径
	  LVITEM  lvItem;
	  short i=0;//最近文件计数
	  m_nCount = 0;

	  //首先获取AutoPSA 7.0版本注册表
	  strPathNameKey.Format("%s8.0\\",szAutoPSAKey);
	  for( i=1; i<=8; i++ )
	  {//最多显示8条文件路径
		strItem.Format("File%d", i);
		strValue = GetUSER_RegKey(strPathNameKey, "Recent File List", strItem, "");
		if (!strValue.IsEmpty() && FileExists(strValue))
		{
			m_strFileList[m_nCount] = strValue;
			strItem.Format("%d", m_nCount+1);
			m_ctrlFileList.InsertItem(m_nCount, strItem);
			lvItem.mask = LVIF_TEXT ;
			lvItem.iItem = m_nCount;
			lvItem.iSubItem = 1;
			lvItem.pszText = strValue.GetBuffer(255);

			m_ctrlFileList.SetItem(&lvItem);
			m_nCount++;
		}
	  }
	  
	  //其次获取AutoPSA 1.1版本注册表
	  strPathNameKey.Format("%s1.1\\",szAutoPSAKey);
	  for( i=1; i<=8; i++ )
	  {//最多显示8条文件路径
		strItem.Format("File%d", i);
		strValue = GetUSER_RegKey(strPathNameKey, "Recent File List", strItem, "");
		if (!strValue.IsEmpty() && FileExists(strValue))
		{
			m_strFileList[m_nCount] = strValue;
			strItem.Format("%d", m_nCount+1);
			m_ctrlFileList.InsertItem(m_nCount, strItem);
			lvItem.mask = LVIF_TEXT ;
			lvItem.iItem = m_nCount;
			lvItem.iSubItem = 1;
			lvItem.pszText = strValue.GetBuffer(255);

			m_ctrlFileList.SetItem(&lvItem);
			m_nCount++;
		}
	  }
	  
	  return m_nCount;
}

/**************************************************************************
 函数功能：从数据库中得到PHS最近打开的文件路径,并在list ctrl控件中显示出来
 输入参数:无
 输出参数:无
 返回值:得到以显示的从PHS打开的文件的条数
**************************************************************************/
int CDlgFindAutoPSAFile::GetPHSRecentFile()
{
	 CDaoRecordset pfileroad;
	 pfileroad.m_pDatabase = &EDIBgbl::dbPRJDB;
	 CString fileSQL;
	
     fileSQL.Format("SELECT * FROM ZY WHERE [VolumeID]=%d Order by FileNameID DESC",EDIBgbl::SelVlmID);
	 pfileroad.Open(dbOpenDynaset,fileSQL);//得到符合条件的记录集

     CString strValue;//文件路径 
	 CString strItem;//序号
   	 LVITEM  lvItem;
	 m_nCount = 0;
	 
	 while (!pfileroad.IsEOF() && m_nCount<8)
	 {//有符合条件的记录集且显示的路径没有超过8条
		strValue = vtos(pfileroad.GetFieldValue(2));//得到文件路径

		if (!strValue.IsEmpty() && FileExists(strValue))
		{
			m_strFileList[m_nCount] = strValue;
			strItem.Format("%d", m_nCount+1);
			m_ctrlFileList.InsertItem(m_nCount, strItem);
			lvItem.mask = LVIF_TEXT ;
			lvItem.iItem = m_nCount;
			lvItem.iSubItem = 1;
			lvItem.pszText = strValue.GetBuffer(255);

			m_ctrlFileList.SetItem(&lvItem);
			m_nCount++;
		}
		pfileroad.MoveNext();//得到下一条记录
	 }

	 return m_nCount;
}

/**************************************************************************
 函数功能：从注册表中得到CAESARII最近打开的文件路径,并在list ctrl控件中显示出来
 输入参数:无
 输出参数:无
 返回值:得到以显示的从CAESARII打开的文件的条数
***************************************************************************/
short CDlgFindAutoPSAFile::GetCAESARIIRecentFile()
{
	  CString strValue;//文件路径 
	  CString strItem;//序号
	  CString strPathNameKey = szCAESARIIKey;
	  LVITEM  lvItem;
	  m_nCount = 0;

	  for( int i=1; i<=8; i++ )
	  {//最多显示8条文件路径
		strItem.Format("File%d", i);
		strValue = GetUSER_RegKey(strPathNameKey, "Recent ._A Files", strItem, "");
		if (!strValue.IsEmpty() && FileExists(strValue))
		{
			m_strFileList[m_nCount] = strValue;
			strItem.Format("%d", m_nCount+1);
			m_ctrlFileList.InsertItem(m_nCount, strItem);
			lvItem.mask = LVIF_TEXT ;
			lvItem.iItem = m_nCount;
			lvItem.iSubItem = 1;
			lvItem.pszText = strValue.GetBuffer(255);

			m_ctrlFileList.SetItem(&lvItem);
			m_nCount++;
		}
	  }
	  
	  return m_nCount;

}

void CDlgFindAutoPSAFile::OnClickList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	int pos = m_ctrlFileList.GetSelectionMark();	
	if( pos >= 0 && pos < m_nCount )
	{
		modPHScal::gsPSAOutFilePath = m_strFileList[pos];
	}
	*pResult = 0;
	m_bFlag = 1;  //选择状态.
}
