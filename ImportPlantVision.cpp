// ImportPlantVision.cpp: implementation of the CImportPlantVision class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "user.h"
#include "ImportPlantVision.h"
#include "basdirectory.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImportPlantVision::CImportPlantVision()
{
	this->SetRegSubKey("Settings");
}

CImportPlantVision::~CImportPlantVision()
{

}

void CImportPlantVision::BeginImport()
{ 
	int iPos;
	CPropertyWnd::ElementDef ElementDef;
	ImportFromXLSStruct ImportTable;
	ImportFromXLSElement ImportTableItem[100];
	int TableItemCount;

	CWaitCursor WaitCursor;

	// 中文名称与字段的对照表
	struct
	{
		LPCTSTR NameCh;		// pre_calc的字段名的中文名称
		LPCTSTR NameField;	// pre_calc的字段名
	}FieldsName[]=
	{
		{_T("支吊架号"),		_T("ZDJH")},
	};

	this->m_HintInformation=_T("开始导入数据");
	this->UpdateData(FALSE);
	this->UpdateWindow();

    this->SetProjectDbConnect( conPRJDB );

	//
	// 填写ImportFromXLSStruct结构信息
	//
	ImportTable.ProjectDBTableName=_T("ZA1");

	ImportTable.XLSFilePath=this->m_XLSFilePath;

	iPos=this->m_PropertyWnd.FindElement(0,_T("导入数据开始行号"));
	this->m_PropertyWnd.GetElementAt(&ElementDef,iPos);
	ImportTable.BeginRow=_ttoi(ElementDef.RightElementText);

	iPos=this->m_PropertyWnd.FindElement(0,_T("Excel工作表名"));
	this->m_PropertyWnd.GetElementAt(&ElementDef,iPos);
	ImportTable.XLSTableName=ElementDef.RightElementText;

	iPos=this->m_PropertyWnd.FindElement(0,_T("导入多少条数据"));
	this->m_PropertyWnd.GetElementAt(&ElementDef,iPos);
	ImportTable.RowCount=_ttoi(ElementDef.RightElementText);


	TableItemCount=0;
	iPos++;
	//
	// 将属性控件中的内容添入ImportTableItem结构
	//
	while(TRUE)
	{
		if(iPos>=this->m_PropertyWnd.GetElementCount())
			break;

		this->m_PropertyWnd.GetElementAt(&ElementDef,iPos);
		iPos++;

		// 当右单元不为空时才添入
		if(!ElementDef.RightElementText.IsEmpty())
		{
			ImportTableItem[TableItemCount].SourceFieldName=ElementDef.RightElementText;
			ImportTableItem[TableItemCount].DestinationName = ElementDef.strFieldName;
			TableItemCount++;
		}
	}

	ImportTable.pElement=ImportTableItem;
	ImportTable.ElementNum=TableItemCount;

	try
	{
		// ZSY  [2005/12/02]
		if ( !ImportExcelToAccess( &ImportTable ) )
		{
			CDialog::OnCancel();
			return;
		}
	}
	catch(_com_error &e)
	{
		this->m_HintInformation=_T("数据导入失败");
		this->UpdateData(FALSE);

		AfxMessageBox(e.Description());

		return;
	}
	catch(COleDispatchException *e1)
	{
		this->m_HintInformation=_T("数据导入失败");
		this->UpdateData(FALSE);

		AfxMessageBox(e1->m_strDescription);
		e1->Delete();
		return;
	}
	this->m_HintInformation=_T("数据导入成功");
	this->UpdateData(FALSE);

	CDialog::OnOK();
}


BOOL CImportPlantVision::InitPropertyWnd()
{
	CImportFromXLSDlg::InitPropertyWnd();
	
	this->SetWindowText("提取PlantVision数据(.xls格式)");
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
		{_T("PlantVision原始数据"),	CPropertyWnd::TitleElement,NULL},								//4
	};

	for(int i=0;i<sizeof(ElementSet)/sizeof(ElementSet[0]);i++)
	{
		ElementDef.szElementName=ElementSet[i].ElementName;
		ElementDef.Style=ElementSet[i].Style;
		ElementDef.pVoid=NULL;
		if(ElementSet[i].HintInfo!=NULL)
			ElementDef.pVoid=new CString(ElementSet[i].HintInfo);
		m_PropertyWnd.InsertElement(&ElementDef);
	}
	
	// zsy  [2005/12/02]
	try
	{
		_ConnectionPtr pConSort;
		pConSort.CreateInstance(__uuidof(Connection));
		_RecordsetPtr pRsInfo;
		pRsInfo.CreateInstance(__uuidof(Recordset));
		CString strSQL;
		//连接数据库
		strSQL = ::dbConnectionString + basDirectory::ProjectDBDir + _T("Sort.mdb");
		pConSort->Open(_bstr_t(strSQL), "", "", -1);
		
		//打开结构表
		strSQL = "SELECT * FROM [TZA1] WHERE CADFieldSeq >= 0 ORDER BY CADFieldSeq";
		pRsInfo->Open(_variant_t(strSQL), pConSort.GetInterfacePtr(), adOpenKeyset, adLockOptimistic, adCmdText);
		if ( pRsInfo->adoEOF && pRsInfo->BOF )
		{
			return FALSE;
		}
		int iPos;
		CString strTmp;
		TCHAR ColumeName[128];
		for (pRsInfo->MoveFirst(); !pRsInfo->adoEOF; pRsInfo->MoveNext())
		{
			//原始数据表中对应的字段名称
			strTmp = vtos( pRsInfo->GetCollect(_variant_t("FieldName")) );
			if ( strTmp.IsEmpty() )
				continue;
			ElementDef.strFieldName = strTmp;
			//字段描述
			strTmp = vtos( pRsInfo->GetCollect(_variant_t("LocalCaption")) );
			if ( strTmp.IsEmpty() )
				continue;
			ElementDef.szElementName = strTmp;
			//提示信息
			ElementDef.pVoid = new CString("请选择与"+strTmp+"对应的Excel列作为输入");
			//风格
			ElementDef.Style = CPropertyWnd::ChildElement|CPropertyWnd::ComBoxElement;			
			
			//默认的EXCEL列号
			strTmp = vtos( pRsInfo->GetCollect(_variant_t("ExcelColNO")) );
			ElementDef.RightElementText = strTmp;

			//插入一项
			iPos = m_PropertyWnd.InsertElement(&ElementDef);
			//创建子控件
			if ( m_PropertyWnd.CreateElementControl(iPos) )
			{
				//右边提供选的EXCEL列号
				for(int k=0;k<100;k++)
				{
					int j,m,m2;				
					for(j=k,m=1;j/26!=0;m++,j/=26);				
					ColumeName[m]=_T('\0');				
					m--;
					j=k;
					m2=m;
					while(m>=0)
					{
						ColumeName[m]=j%26+_T('A');					
						j=j/26;					
						if(m2!=m)
							ColumeName[m]--;					
						m--;
					}
					this->m_PropertyWnd.GetElementAt(&ElementDef,iPos);
					((CComboBox*)ElementDef.pControlWnd)->AddString(ColumeName);
				}	
			}

		}
		//
		m_PropertyWnd.RefreshData();
	}
	catch (_com_error& e)
	{
		AfxMessageBox( e.Description() );
		return FALSE;
	}

	return TRUE;
}

//------------------------------------------------------------------
// DATE         :[2005/12/05]
// Parameter(s) :
// Return       :
// Remark       :保存用户改写的状态
//------------------------------------------------------------------
void CImportPlantVision::SaveSetToReg()
{
	CImportFromXLSDlg::SaveSetToReg();
	
	try
	{
		_ConnectionPtr pConSort;
		pConSort.CreateInstance(__uuidof(Connection));
		_RecordsetPtr pRsInfo;
		pRsInfo.CreateInstance(__uuidof(Recordset));
		CString strSQL;
		CPropertyWnd::ElementDef ElementDef;

		//连接数据库
		strSQL = ::dbConnectionString + basDirectory::ProjectDBDir + _T("Sort.mdb");
		pConSort->Open(_bstr_t(strSQL), "", "", -1);
		
		//打开结构表
		strSQL = "SELECT * FROM [TZA1] WHERE CADFieldSeq >= 0 ORDER BY CADFieldSeq";
		pRsInfo->Open(_variant_t(strSQL), pConSort.GetInterfacePtr(), adOpenKeyset, adLockOptimistic, adCmdText);
		if ( pRsInfo->adoEOF && pRsInfo->BOF )
		{
			return ;
		}
		int iPos;
		CString strTmp;
		for (pRsInfo->MoveFirst(); !pRsInfo->adoEOF; pRsInfo->MoveNext())
		{
			//字段描述
			strTmp = vtos( pRsInfo->GetCollect(_variant_t("LocalCaption")) );
			if ( strTmp.IsEmpty() )
				continue;
			iPos = this->m_PropertyWnd.FindElement( 0, strTmp );
			if (  -1 == iPos )
			{
				continue;
			}
			this->m_PropertyWnd.GetElementAt( &ElementDef, iPos );

			//写入到数据库表中
			pRsInfo->PutCollect(_variant_t("ExcelColNO"), _variant_t(ElementDef.RightElementText) );
			pRsInfo->Update();

		}
	}
	catch (_com_error& e) 
	{
		AfxMessageBox( e.Description() );
		return;
	}

}
