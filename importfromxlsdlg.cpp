// ImportFromXLSDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ImportFromXLSDlg.h"
#include "resource.h"
#include "modregistry.h"
#include "user.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImportFromXLSDlg dialog


CImportFromXLSDlg::CImportFromXLSDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CImportFromXLSDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CImportFromXLSDlg)
	m_XLSFilePath = _T("");
	m_HintInformation = _T("");
	//}}AFX_DATA_INIT

	//设置默认的子键的名称
	m_RegSubKey=_T("ImportFromXLSDlg");
}

CImportFromXLSDlg::~CImportFromXLSDlg()
{
	CPropertyWnd::ElementDef Element;

	//
	// 释放用于保存提示信息的内存
	//

	for(int i=0;i<this->m_PropertyWnd.GetElementCount();i++)
	{
		this->m_PropertyWnd.GetElementAt(&Element,i);

		if(Element.pVoid)
		{
			delete ((CString*)Element.pVoid);
			Element.pVoid=NULL;
		}
	}
}

void CImportFromXLSDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImportFromXLSDlg)
	DDX_Control(pDX, IDC_OPEN_FILEDLG, m_OpenFileDlgButton);
	DDX_Text(pDX, IDC_FILE_FULLPATH, m_XLSFilePath);
	DDX_Text(pDX, IDC_HINT_INFORMATION, m_HintInformation);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CImportFromXLSDlg, CDialog)
	//{{AFX_MSG_MAP(CImportFromXLSDlg)
	ON_BN_CLICKED(IDC_OPEN_FILEDLG, OnOpenFiledlg)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BEGIN_IMPORT1, OnBeginImport)
	//}}AFX_MSG_MAP
	ON_NOTIFY(PWN_SELECTCHANGE,IDC_PROPERTY_WND, OnSelectChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImportFromXLSDlg message handlers

BOOL CImportFromXLSDlg::OnInitDialog() 
{
	CBitmap Bitmap;

	CDialog::OnInitDialog();

	Bitmap.LoadBitmap(IDB_OPENFILE);
	this->m_OpenFileDlgButton.SetBitmap((HBITMAP)Bitmap.Detach());


	CRect Rect;

	//
	// 以隐藏的Group控件来定位属性控件
	//
	GetDlgItem(IDC_PROP_WND_RECT)->GetWindowRect(&Rect);

	this->ScreenToClient(&Rect);

	m_PropertyWnd.Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_BORDER|WS_TABSTOP,Rect,this,IDC_PROPERTY_WND);

	//初始化属性控件
	InitPropertyWnd();
	
	//从注册表获得初始信息
	InitFromReg();

	UpdateData( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

////////////////////////////////////////////////
//
// 响应“选择文件按钮”
//
void CImportFromXLSDlg::OnOpenFiledlg() 
{
	CFileDialog  FileDlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("Worksheet Files (*.xls)|*.xls|All Files (*.*)|*.*||"));
	
	FileDlg.m_ofn.lpstrInitialDir = m_strPrecFilePath;			 //05/1/4
	//
	// 如果选择了XLS文件就打开它，并获得工作表名
	//
	if(FileDlg.DoModal()==IDOK)
	{
		CWaitCursor WaitCursor;
		
		this->m_XLSFilePath=FileDlg.GetPathName();
		this->UpdateData(FALSE);
		
		int iPos;
		CString strSheetName;
		CPropertyWnd::ElementDef ElementInfo;	//用以返回属性控件单元的信息
		CComboBox * pCtlTblName;
		
		//返回"Excel工作表名"单元的信息
		iPos=this->m_PropertyWnd.FindElement(0,_T("Excel工作表名"));
		this->m_PropertyWnd.CreateElementControl(iPos);
		this->m_PropertyWnd.GetElementAt(&ElementInfo,iPos);
		//获得当前状态的工作表名   1/11     
		strSheetName = ElementInfo.RightElementText;
		//
		pCtlTblName = (CComboBox*)ElementInfo.pControlWnd;
		pCtlTblName->ResetContent();
		
		if ( ConnectExcelFile( m_XLSFilePath, m_pConExcel) )
		{
			_RecordsetPtr pRsTmp;
			pRsTmp = m_pConExcel->OpenSchema( adSchemaTables );
			if ( !pRsTmp->adoEOF && !pRsTmp->BOF )
			{
				CString strTableName;
				for ( pRsTmp->MoveFirst(); !pRsTmp->adoEOF; pRsTmp->MoveNext() )
				{
					strTableName = vtos( pRsTmp->GetCollect(_variant_t("TABLE_NAME")));
					strTableName.TrimLeft("\'");
					strTableName.TrimRight("\'");
					strTableName.TrimRight("$");
					if ( !strTableName.IsEmpty() && -1 == pCtlTblName->FindString( -1, strTableName ) )
					{
						pCtlTblName->AddString( strTableName );
					}
				}
			}
		}
		///////获得指定的工作表名      05/1/4	
		iPos = pCtlTblName->FindString(0, strSheetName);
		iPos = (iPos>0) ? iPos : 0;
		((CComboBox*)ElementInfo.pControlWnd)->SetCurSel(iPos);
		//////05/1/4
		
		//重绘属性控件的窗口
		this->m_PropertyWnd.Invalidate();
		
		///当前的05/1/4
		m_strPrecFilePath = m_XLSFilePath;
	}
}

void CImportFromXLSDlg::OnDestroy() 
{
	CDialog::OnDestroy();

	//关闭窗口时保存数据到注册表
	SaveSetToReg();
}

//////////////////////////////////////////////////////////////

///////////////////////////////////////////
//
// 初始化属性窗口
//
// 函数成功返回TRUE，否则返回FALSE
//
// 此函数应该在WM_INITDIALOG消息中调用
//
BOOL CImportFromXLSDlg::InitPropertyWnd()
{
	//
	// 初始化属性控件
	//
	CPropertyWnd::ElementDef ElementDef;

	struct
	{
		LPCTSTR ElementName;	// 单元的名称
		UINT	Style;			// 显示样式
		LPCTSTR HintInfo;		// 与单元相关的提示信息
	}ElementSet[]=
	{
		{_T("Excel"),			CPropertyWnd::TitleElement,NULL},								//0

		{_T("Excel工作表名"),	CPropertyWnd::ChildElement|CPropertyWnd::ComBoxElement,
		 _T("请选择将哪张表导入进来")},	//1

		{_T("导入数据开始行号"),CPropertyWnd::ChildElement|CPropertyWnd::EditElement,NULL},		//2
		{_T("导入多少条数据"),	CPropertyWnd::ChildElement|CPropertyWnd::EditElement,NULL},		//3
	};


	for(int i=0;i<sizeof(ElementSet)/sizeof(ElementSet[0]);i++)
	{
		ElementDef.szElementName=ElementSet[i].ElementName;
		ElementDef.Style=ElementSet[i].Style;

		ElementDef.pVoid=NULL;

		if(ElementSet[i].HintInfo!=NULL)
		{
			
			ElementDef.pVoid=new CString(ElementSet[i].HintInfo);
		}

		m_PropertyWnd.InsertElement(&ElementDef);

	}

	m_PropertyWnd.RefreshData();

	return TRUE;
}
///////////////////////////////////////////
//
// 从注册表获得数据初始化
//
void CImportFromXLSDlg::InitFromReg()
{
	LONG lRet;
	DWORD dwDisposition;
	HKEY hKey;
	CString SubKey;
	CPropertyWnd::ElementDef ElementDef;
	TCHAR szTemp[256];
	DWORD dwData;

	SubKey=szSoftwareKey;
	SubKey+=m_RegSubKey;

	//打开注册表
	lRet=::RegCreateKeyEx(HKEY_LOCAL_MACHINE,SubKey,0,NULL,REG_OPTION_NON_VOLATILE,KEY_READ,NULL,&hKey,&dwDisposition);
	
	if(lRet!=ERROR_SUCCESS)
	{
		AfxMessageBox(_T("打开注册表失败"));
		return;
	}
	// 从注册表获得数据放入属性控件
	//
//	for(int i=0;i<this->m_PropertyWnd.GetElementCount();i++)
	int nPos = this->m_PropertyWnd.FindElement(0, "导入多少条数据");
	if ( nPos < 0 )
		nPos = 3;
	
	for(int i=0;i<=nPos;i++)
	{
		this->m_PropertyWnd.CreateElementControl(i);
		this->m_PropertyWnd.GetElementAt(&ElementDef,i);

		dwData=256;

		lRet=RegQueryValueEx(hKey,ElementDef.szElementName,NULL,NULL,(BYTE*)szTemp,&dwData);

		if(lRet==ERROR_SUCCESS)
		{
			if(ElementDef.pControlWnd && IsWindow(ElementDef.pControlWnd->GetSafeHwnd()))
			{
				if(dwData>0)
					ElementDef.pControlWnd->SetWindowText(szTemp);
			}
		}
	}
	/////////  初始Excel 文件路径
	dwData=256;
	lRet=RegQueryValueEx(hKey,"ExcelFilePath",NULL,NULL,(BYTE*)szTemp,&dwData);  

	if(lRet==ERROR_SUCCESS)
	{
		m_strPrecFilePath = szTemp;
	}
	m_XLSFilePath = m_strPrecFilePath;
	//////
	::RegCloseKey(hKey);
	
}

////////////////////////////////////////////
//
// 将信息保存到注册表内
//
void CImportFromXLSDlg::SaveSetToReg()
{
	LONG lRet;
	DWORD dwDisposition;
	HKEY hKey;
	CString SubKey;
	CPropertyWnd::ElementDef ElementDef;

	SubKey=szSoftwareKey;
	SubKey+=m_RegSubKey;

	// 打开注册表
	lRet=::RegCreateKeyEx(HKEY_LOCAL_MACHINE,SubKey,0,NULL,REG_OPTION_NON_VOLATILE,KEY_WRITE,NULL,&hKey,&dwDisposition);
	
	if(lRet!=ERROR_SUCCESS)
	{
		AfxMessageBox(_T("打开注册表失败"));
		return;
	}

	//
	// 将属性控件的内容存如注册表
	//
//	for(int i=0;i<this->m_PropertyWnd.GetElementCount();i++)
	int nPos = this->m_PropertyWnd.FindElement(0, "导入多少条数据");
	if ( nPos < 0 )
		nPos = 3;

	for(int i=0;i<=nPos;i++)
	{
		this->m_PropertyWnd.GetElementAt(&ElementDef,i);

		if(!ElementDef.RightElementText.IsEmpty())
		{
			lRet=RegSetValueEx(hKey,ElementDef.szElementName,NULL,REG_SZ,(BYTE*)(LPCTSTR)ElementDef.RightElementText,
						  strlen(ElementDef.RightElementText)+sizeof(TCHAR));
		}
		else
		{
			lRet=RegSetValueEx(hKey,ElementDef.szElementName,NULL,REG_SZ,(BYTE*)NULL,
						  0);
		}

	}
	///////  保存路径
	if ( !m_strPrecFilePath.IsEmpty() )
	{
			lRet=RegSetValueEx(hKey,"ExcelFilePath",NULL,REG_SZ,(BYTE*)(LPCTSTR)m_strPrecFilePath,
						  strlen(m_strPrecFilePath)+sizeof(TCHAR));
	}
	//////

	::RegCloseKey(hKey);
	
}


/////////////////////////////////////////////////////////////////////
//
// 响应属性控件选中的通知
//
void CImportFromXLSDlg::OnSelectChange(NMHDR *pNMHDR, LRESULT *pResult)
{
	CPropertyWnd::PWNSelectChangeStruct *pSelect;

	pSelect=(CPropertyWnd::PWNSelectChangeStruct*)pNMHDR;

	this->m_HintInformation=(pSelect->szElementName);

	if(pSelect->pVoid)
	{
		this->m_HintInformation+=_T(":\r\n");

		this->m_HintInformation+=*((CString*)pSelect->pVoid);
	}

	this->UpdateData(FALSE);
	*pResult=0;
}

///////////////////////////////////////////
//
// 响应开始”导入“
//
void CImportFromXLSDlg::OnBeginImport() 
{
	BOOL bRet;

	this->UpdateData();

	// 验证数据输入 是否有效
	bRet=ValidateData();

	if(!bRet)
	{
		return;
	}

	BeginImport();
}

/////////////////////////////////////////////
//
// 验证输入数据的有效性
//
BOOL CImportFromXLSDlg::ValidateData()
{
	CPropertyWnd::ElementDef ElementDef;
	CString strTemp;

	this->UpdateData();

	//判断文件路径是否为空
	if(this->m_XLSFilePath.IsEmpty())
	{
		AfxMessageBox(_T("文件路径不能为空"));
		return FALSE;
	}

	//
	// 检查属性控件中指定的单元是否为空
	//
	for(int i=0;i<this->m_PropertyWnd.GetElementCount();i++)
	{
		LPCTSTR szTableName[]=
		{
			_T("Excel工作表名"),
			_T("导入数据开始行号"),
			_T("导入多少条数据"),
		};

		this->m_PropertyWnd.GetElementAt(&ElementDef,i);

		for(int j=0;j<sizeof(szTableName)/sizeof(szTableName[0]);j++)
		{
			if(ElementDef.szElementName==szTableName[j] && ElementDef.RightElementText.IsEmpty())
			{
				strTemp.Format(_T("%s不能为空"),szTableName[j]); 
				AfxMessageBox(strTemp); 
				return FALSE; 
			}
		}

	}

	return TRUE;
}

//////////////////////////////////////////////////////////
//
// 设置工程编号
//
// szProjectID[in]	工程编号
//
void CImportFromXLSDlg::SetProjectID(LPCTSTR szProjectID)
{
	if(szProjectID==NULL)
	{
		m_strProjectID=_T("");
	}
	else
	{
		m_strProjectID=szProjectID;
	}
}

//////////////////////////////////////////////////////
//
// 返回工程的编号
//
CString CImportFromXLSDlg::GetProjectID()
{
	return m_strProjectID;
}

//////////////////////////////////////////////////////////////////////
//
// 设置与工程相关数据库的连接
//
// IConnection[in]	数据库的连接
//
void CImportFromXLSDlg::SetProjectDbConnect(_ConnectionPtr IConnection)
{
	this->m_ProjectDbConnect=IConnection;
}

////////////////////////////////////////////////////////////////////
//
// 返回与数据库的连接
//
_ConnectionPtr CImportFromXLSDlg::GetProjectDbConnect()
{
	return this->m_ProjectDbConnect;
}

//////////////////////////////////////////////////////////
//
// 设置用于存放信息的子键名
//
// szRegSubKey[in]	用于存放信息的子键名
//
void CImportFromXLSDlg::SetRegSubKey(LPCTSTR szRegSubKey)
{
	if(szRegSubKey)
		m_RegSubKey=szRegSubKey;
}

////////////////////////////////////////////////////////
//
// 返回用于存放信息的子键名
//
CString CImportFromXLSDlg::GetRegSubKey()
{
	return m_RegSubKey;
}

///////////////////////////////////////////////////////////
//
// 设置需在对话框下部提示信息区显示的内容
//
// szHint[in]	在对话框下部提示信息区显示的内容
//
void CImportFromXLSDlg::SetHintInformation(LPCTSTR szHint)
{
	if(szHint)
		m_HintInformation=szHint;
	else
		m_HintInformation=_T("");

	if(IsWindow(GetSafeHwnd()))
	{
		UpdateData(FALSE);
	}
}

////////////////////////////////////////////////////////
//
// 返回需在对话框下部提示信息区显示的内容
//
CString CImportFromXLSDlg::GetHintInformation()
{
	return m_HintInformation;
}

////////////////////////////////////////////////////////
//
// 获得属性控件的窗口
//
CPropertyWnd* CImportFromXLSDlg::GetPropertyWnd()
{
	return &m_PropertyWnd;
}

//////////////////////////////////////////////////
//
// 执行导入数据的操作
//
void CImportFromXLSDlg::BeginImport()
{

}

BOOL CImportFromXLSDlg::ImportExcelToAccess(ImportFromXLSStruct *pImportStruct)
{
	CString strSQL;			//SQL语句
	CString strTblName = pImportStruct->ProjectDBTableName;		//ACCESS数据库的表名
	long nBeginRow		= pImportStruct->BeginRow;
	long nExcelRecCount = pImportStruct->RowCount;
	long nAllFieldCount = pImportStruct->ElementNum;			//字段个数
	CString strExcelFileName = pImportStruct->XLSFilePath;
	CString strWorksheetName = pImportStruct->XLSTableName;
	ImportFromXLSElement * pAllFieldStruct = pImportStruct->pElement;
	CString strTmp;
	_variant_t varTmp;
	long	nColNO;
	int		i;
	_ConnectionPtr pConDes;
	_RecordsetPtr pRsExcel;
	_RecordsetPtr pRsAccess;
	pRsExcel.CreateInstance(__uuidof(Recordset));
	pRsAccess.CreateInstance(__uuidof(Recordset));
	try
	{
		pConDes = GetProjectDbConnect();
		if ( pConDes == NULL )
		{
			return FALSE;
		}
		if ( pImportStruct == NULL )
		{
			return FALSE;
		}
		//连接EXCEL文件
		if ( !OpenExcelTable( pRsExcel, strWorksheetName, strExcelFileName ) )
		{
			return FALSE;
		}
		if (pRsExcel->adoEOF && pRsExcel->BOF)
		{
			return TRUE;
		}
		//使用记录集的方式访问EXCEL文件时，如果一个字段存在多种数据类型时，有一些数据可能取不出来。(字段中的值使用最多的一种类型作为整个字段的类型。）
		//以下的代码考虑了，在一个字段中有数字型和字符型，空字符串全部删除。这样数字型的值才能取出来。
		// 替换记录集中的空字符串  [2005/12/06]
		if (nExcelRecCount <= 0)
		{
			nExcelRecCount = pRsExcel->GetRecordCount();
		}
		for (pRsExcel->MoveFirst(), i = 2; !pRsExcel->adoEOF && i < nBeginRow; pRsExcel->MoveNext(), i++)
			;		//定位到开始行
		for (; !pRsExcel->adoEOF; pRsExcel->MoveNext())
		{
			for (int i=0; i < nAllFieldCount; i++)
			{
				try
				{
					//如果字段名为空时会出错 
					if (pAllFieldStruct[i].SourceFieldName.IsEmpty() || pAllFieldStruct[i].DestinationName.IsEmpty())
						continue;
					StringToNumber( pAllFieldStruct[i].SourceFieldName, nColNO );

					varTmp = pRsExcel->GetCollect(_variant_t( nColNO ));
					if ( varTmp.vt == VT_BSTR && vtos( varTmp ).IsEmpty() )
					{	//删除空字符串
						pRsExcel->PutCollect(_variant_t( nColNO ), _variant_t("") );
						pRsExcel->Update();
					}
				}
				catch (_com_error) 
				{
					continue;
				}
			}
		}

		//重新连接EXCEL文件
		m_pConExcel->Close();
		if ( !OpenExcelTable( pRsExcel, strWorksheetName, strExcelFileName ) )
		{
			return FALSE;
		}
		//导入之前，先清空数据表
		pConDes->Execute(_bstr_t("DELETE * FROM ["+strTblName+"]"), NULL, -1);
		//打开ACCESS原始数据表
		strSQL = "SELECT * FROM ["+strTblName+"] ";
		pRsAccess->Open(_variant_t(strSQL), pConDes.GetInterfacePtr(), adOpenKeyset, adLockOptimistic, adCmdText);
		
		if (nExcelRecCount <= 0)
		{
			nExcelRecCount = pRsExcel->GetRecordCount();
		}
		for (pRsExcel->MoveFirst(), i = 2; !pRsExcel->adoEOF && i < nBeginRow; pRsExcel->MoveNext(), i++)
			;

		//在ACCESS原始表中处理以前导入过的同一类型的记录。
		for (; !pRsExcel->adoEOF && nExcelRecCount > 0; pRsExcel->MoveNext(), nExcelRecCount--)
		{
			pRsAccess->AddNew();
			for (int i=0; i < nAllFieldCount; i++)
			{
				try
				{
					//如果字段名为空时会出错 
					if (pAllFieldStruct[i].SourceFieldName.IsEmpty() || pAllFieldStruct[i].DestinationName.IsEmpty())
						continue;
					StringToNumber( pAllFieldStruct[i].SourceFieldName, nColNO );

					varTmp = pRsExcel->GetCollect(_variant_t( nColNO ));
				}catch (_com_error& e) 
				{
					if (e.Error() == -2146825023)
					{
						strTmp = "原始数据表中没有字段 '"+pAllFieldStruct[i].SourceFieldName+"' 。";
						AfxMessageBox(strTmp);
						continue;
					}
					AfxMessageBox(e.Description());
					return FALSE;
				}				
				pRsAccess->PutCollect(_variant_t(pAllFieldStruct[i].DestinationName),varTmp);
			}
			pRsAccess->Update();
		}
	}
	catch (_com_error& e)
	{
		AfxMessageBox(e.Description());
		return FALSE;
	}	
	return TRUE;
}

void CImportFromXLSDlg::StringToNumber(CString strNO, long &nNO)
{
	nNO = -1;
	TCHAR cNum;
	strNO.MakeUpper();
	int  nLeng = strNO.GetLength();
	for (int i = 1; i <= nLeng; i++)
	{ 
		cNum = strNO[nLeng - i];
		if ( 1 == i )
			nNO = cNum - 'A';
		else
			nNO += ( cNum - 'A' + 1 ) * 26;
	}
}


//------------------------------------------------------------------
// DATE         :[2005/12/05]
// Parameter(s) :
// Return       :
// Remark       :用ADO的方式，联接EXCEL文件.
//------------------------------------------------------------------
BOOL CImportFromXLSDlg::ConnectExcelFile(const CString strExcelName, _ConnectionPtr &pConExcel)
{
	CString strSQL;
	try
	{
		if (pConExcel == NULL)
		{
			pConExcel.CreateInstance(__uuidof(Connection));
		}
		if ( pConExcel->State == adStateOpen )
		{
			pConExcel->Close();
		}

		strSQL = CONNECTSTRING_EXCEL + strExcelName;
		pConExcel->Open(_bstr_t(strSQL), "", "", adCmdUnspecified);
	}
	catch (_com_error& e)
	{
		AfxMessageBox(e.Description());
		return FALSE;
	}
	return TRUE;

}

//------------------------------------------------------------------
// DATE         :[2005/12/05]
// Parameter(s) :
// Return       :
// Remark       :用ADO记录集的方式打开，EXCEL文件中的表。
//------------------------------------------------------------------
BOOL CImportFromXLSDlg::OpenExcelTable(_RecordsetPtr pRsTbl, CString& strSheetName, CString strExcelFileName)
{ 
	CString strSQL;
	try
	{
		if ( m_pConExcel == NULL || m_pConExcel->State == adStateClosed )
		{
			//使用一个成员的数据库连接对象，
			if ( !ConnectExcelFile( strExcelFileName, m_pConExcel ) )
			{
				return FALSE;
			}
		}
		if (pRsTbl == NULL)
		{
			pRsTbl.CreateInstance(__uuidof(Recordset));
		}
		try
		{
			if (pRsTbl->State == adStateOpen)
			{
				pRsTbl->Close();
			}
			strSQL = "SELECT * FROM ["+strSheetName+"$] ";		//在EXCEL中的表名加上$
			pRsTbl->Open(_variant_t(strSQL), m_pConExcel.GetInterfacePtr(), adOpenKeyset, adLockOptimistic, adCmdText);
			strSheetName += "$";								//在调用该函数之后的程序中作为EXCEL表名
		}
		catch (_com_error)
		{
			strSQL = "SELECT * FROM ["+strSheetName+"] ";
			pRsTbl->Open(_variant_t(strSQL), m_pConExcel.GetInterfacePtr(), adOpenKeyset, adLockOptimistic, adCmdText);
		} 
	}
	catch (_com_error& e)
	{
		AfxMessageBox(e.Description());
		return FALSE;
	}
	return TRUE;
}

