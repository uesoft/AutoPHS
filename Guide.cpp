// Guide.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "columns.h"
#include "column.h"
#include "Guide.h"
#include "mainfrm.h"
#include "selbookmarks.h"
#include "user.h"
#include "afx.h"
//


#include "modregistry.h"
#include "modphscal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Guide dialog


Guide::Guide(CWnd* pParent /*=NULL*/)
	: CDialog(Guide::IDD, pParent)
{
	//{{AFX_DATA_INIT(Guide)
	m_radio = -1;
	//}}AFX_DATA_INIT
}


void Guide::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Guide)
	DDX_Control(pDX, IDC_CHECK1, m_check);
	DDX_Radio(pDX, IDC_RADIO1, m_radio);
	DDX_Control(pDX, IDC_DATAGRID, m_GData);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Guide, CDialog)
	//{{AFX_MSG_MAP(Guide)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_RADIO3, OnRadio3)
	ON_COMMAND(ID_EDIT_ADD_RS,OnEditAddRs)
	ON_COMMAND(ID_EDIT_DEL_RS,OnEditDelRs)
	ON_COMMAND(ID_EDIT_FILTER_RS,OnEditFilterRs)
	ON_COMMAND(ID_EDIT_CANSEL_RS,OnEditCanSelRs)
	ON_COMMAND(ID_EDIT_INDEX_RS,OnEditIndexRs)
	ON_BN_CLICKED(IDC_BUTTON_Guide, OnBUTTONGuide)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Guide message handlers


BOOL Guide::PreCreateWindow(CREATESTRUCT& cs) 
{
	return CDialog::PreCreateWindow(cs);
}

BOOL Guide::OnInitDialog() 
{
	
	CRect rc1;
	AfxGetApp()->BeginWaitCursor();
   
	//因为和分辨率有关 所以要初始化坐标
	::SystemParametersInfo(SPI_GETWORKAREA,0,(LPVOID)&rc1,0);
	this->SetWindowPos(NULL,0,((CMainFrame*)AfxGetApp()->m_pMainWnd)->GetMenuHeight(),rc1.Width(),
		rc1.Height()-((CMainFrame*)AfxGetApp()->m_pMainWnd)->GetMenuHeight(),SWP_NOZORDER|SWP_NOACTIVATE);
	CDialog::OnInitDialog();
	m_GData.SetForeColor(0xff0000);
	m_GData.SetBackColor(0xffc0c0);
	this->GetWindowRect(&rc1);
	m_GData.SetWindowPos(NULL,rc1.left+10,rc1.top+20,rc1.right-15,rc1.bottom-rc1.top-110,SWP_DEFERERASE);
	m_radio=0;
	UpdateData(false);
	    //strBackupPath=GetRegKey(_T("Directory"), _T("EDInBox_InsDir"), CString(_T("")))+"BackupDB\\GUIDE";
    //从注册表中得到数据库的路径
	strPrjDBDir = GetRegKey(_T("Directory"),_T("EDInBox_PrjDBDir"),CString(""));
	m_check.SetCheck(1);
	try
	{
		
		ConDB.CreateInstance(__uuidof(Connection));		
		CString Connectring;
	    Connectring="Provider=Microsoft.Jet.OLEDB.4.0;Persist Security Info=False;Data Source="+strPrjDBDir+"PHScode.mdb";//20071101 "sort" 改为 "PHScode"
		ConDB->Open(_bstr_t(Connectring),"","",0);
		afresh();
       
	}
	catch(_com_error *e)
	{
		e->Description();
	}
		
    m_PopMenu.CreatePopupMenu();
    m_PopMenu.AppendMenu(MF_STRING,ID_EDIT_ADD_RS,"增加记录");
    m_PopMenu.AppendMenu(MF_STRING,ID_EDIT_DEL_RS,"删除记录");
    m_PopMenu.AppendMenu(MF_STRING,ID_EDIT_FILTER_RS,"筛选记录");
	m_PopMenu.AppendMenu(MF_STRING,ID_EDIT_CANSEL_RS,"取消筛选");
    m_PopMenu.AppendMenu(MF_STRING,ID_EDIT_INDEX_RS,"排序记录");
	AfxGetApp()->EndWaitCursor();
	  return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


//记录打开以及与控件的绑顶
void Guide::afresh()
{
	
	CString Sql;
	try
	{
		Rd.CreateInstance(__uuidof(Recordset));
		Rd->CursorLocation=adUseClient;
		cansel=1;
		if(m_radio==0)
			Sql="SELECT * FROM connectPASA";
		else
			if(m_radio==1)
				Sql="SELECT * FROM connectCSPR";
			else
				Sql="SELECT * FROM connectSPR";
		
		if(Rd->State==1)
			Rd->Close();
		Rd->Open(_variant_t(Sql),(IDispatch *)ConDB,adOpenDynamic,adLockOptimistic,adCmdText);
		m_GData.SetRefDataSource(Rd->GetDataSource());
		///Ft=Rd->GetFilter();
		//Rd->Close();
		SetColCaption();
	}
	catch(CException *e)
	{
		e->Delete();
	}
}


void Guide::OnRadio1() 
{
	UpdateData(true);
	afresh();
    	
	
}

//关闭时判定用户是否选定了"导入" 是:数据全部插入connect表中;否:关闭; 
void Guide::OnClose() 
{
	if(m_check.GetCheck())   
	{
		modPHScal::CreateTmpConnectTable();
		try
		{
			//_ConnectionPtr Con;
			CString sql;
				//Connectionstr="Provider=Microsoft.Jet.OLEDB.4.0;Persist Security Info=False;Data Source="+strBackupPath+"\\sort.mdb";
				//Con.CreateInstance(__uuidof(Connection));
				// Con->Open(_bstr_t(Connectionstr),"","",0);
			sql="INSERT INTO [connectSPRBAK] SELECT * FROM connectSPR";
			ConDB->Execute(_bstr_t("DELETE FROM [connectSPRBAK]"),NULL,adCmdText);
			ConDB->Execute(_bstr_t(sql),NULL,adCmdText);
			sql="INSERT INTO [connectCSPRBAK] SELECT * FROM connectCSPR";
			ConDB->Execute(_bstr_t("DELETE FROM [connectCSPRBAK]"),NULL,adCmdText);
			ConDB->Execute(_bstr_t(sql),NULL,adCmdText);
			sql="INSERT INTO [connectPASABAK] SELECT * FROM connectPASA";
			ConDB->Execute(_bstr_t("DELETE FROM [connectPASABAK]"),NULL,adCmdText);
			ConDB->Execute(_bstr_t(sql),NULL,adCmdText);
			
			//CreateDirectory(strBackupPath,NULL);
			//CopyFile(strPrjDBDir+"Sort.mdb",strBackupPath+"\\Sort.mdb",FALSE);
			 
		}
		catch(CException *e)
		{
			e->Delete();
		}
	}
	   //Rd->Close();
	   //ConDB->Close();	
	   CDialog::OnClose();
}

void Guide::OnRadio2() 
{
	UpdateData(true);
	afresh();
	
}

void Guide::OnRadio3() 
{
	UpdateData(true);
	afresh();
	// TODO: Add your control notification handler code here	
}


//改变DataGrid控件的列名以及宽度
void Guide::SetColCaption()
{
	int ColNum,i;
	CColumn Column;
	CString ColName;
	ColNum=m_GData.GetColumns().GetCount();
	
	for(i=0;i<ColNum;i++)
	{
		Column=m_GData.GetColumns().GetItem(_variant_t((long)i));
		ColName=Column.GetCaption();
		if(ColName=="CNTB")
		{
			Column.SetCaption("头零件");
			Column.SetWidth(50);
			continue;
		}
		if(ColName=="CNTBnum")
		{
			Column.SetCaption("头数量");
			Column.SetWidth(45);
			continue;
		}
		if(ColName=="CntbXfx")
		{	Column.SetCaption("头X视图");
		    Column.SetWidth(45);
		    continue;
		}
		if(ColName=="CntbRotation")
		{
			Column.SetCaption("头旋转度");
			Column.SetWidth(45);
			continue;
		}
		if(ColName=="CNTBextlenFLD")
		{
			Column.SetCaption("头伸入下一个件长");
			Column.SetWidth(60);
			continue;
		}
		if(ColName=="CNTE")
		{
			Column.SetCaption("尾零件");
			Column.SetWidth(50);
			continue;
		}
		if(ColName=="CNTEnum")
		{
			Column.SetCaption("尾数量");
			Column.SetWidth(45);
            continue;
		}
		if(ColName=="CnteXfx")
		{
			Column.SetCaption("尾X视图");
			Column.SetWidth(45);
			continue;
		}
		if(ColName=="CnteRotation")
		{
			Column.SetCaption("尾旋转度");
			Column.SetWidth(45);
			continue;
		}
		if(ColName=="CNTEFLDN")
		{
			Column.SetCaption("尾匹配尺寸");
			Column.SetWidth(60);
			continue;
		}
		if(ColName=="CNTBF1")
		{
			Column.SetCaption("附件一");
			Column.SetWidth(40);
			continue;
		}
		if(ColName=="CNTBF1num")
		{
			Column.SetCaption("附件一数量");
			Column.SetWidth(50);
			continue;
		}
		if(ColName=="CNTBF2")
		{
			Column.SetCaption("附件二");
			Column.SetWidth(40);
			continue;
		}
		if(ColName=="CNTBF2num")
		{
			Column.SetCaption("附件二数量");
			Column.SetWidth(50);
			continue;
		}
		if(ColName=="CNTBF3")
		{
			Column.SetCaption("附件三");
			Column.SetWidth(40);
			continue;
		}
		if(ColName=="CNTBF3num")
		{
			Column.SetCaption("附件三数量");
			Column.SetWidth(50);
			continue;
		}
		if(ColName=="ConnectCondition")
		{
			Column.SetCaption("连接条件");
			Column.SetWidth(140);
			continue;

		}
		Column.SetVisible(false);
	}
}


void Guide::OnEditAddRs()
{
	try
	{
	    //m_GData.SetAllowAddNew(true);
		Rd->AddNew();
		Rd->PutCollect("CNTBnum",_variant_t(long(0)));
		Rd->Update();

		//这是DataGrid控件的问题,所以要用Requery;不然在改动刚增加的记录会出错;
		Rd->Requery(-1);
        SetColCaption();
		Rd->MoveLast();
	}
	catch(CException *e)
	{
		e->Delete();
	}
}

void Guide::OnEditDelRs()
{
	_variant_t RowBookMark;
	try
	{
		m_GData.Refresh();
		RowBookMark=m_GData.GetSelBookmarks().GetItem(_variant_t(long(0)));
		
	    Rd->Bookmark=RowBookMark;
		Rd->Delete(adAffectCurrent);
	}
	catch(CException *e)
	{
		e->Delete();
	}

}

void Guide::OnEditFilterRs()
{
	try
	{
		CString Ft,ColName,SelText;
		short Column;
		Column=m_GData.GetCol();
		ColName=(CString)(char *)(Rd->GetFields()->GetItem(_variant_t(long(Column)))->GetName());
		SelText=m_GData.GetText();
		Ft=vtos(Rd->GetFilter());
		if(Ft!="0")
             Rd->Filter=_variant_t(Ft+" AND "+ColName+"='"+SelText+"'");
		else
			 Rd->Filter=_variant_t(ColName+"='"+SelText+"'");
		cansel=0;
		
	}
	catch(CException *e)
	{
		e->Delete();
	}
}

void Guide::OnEditCanSelRs()
{
	afresh();
	
	//Rd->Filter=AdFilterNone;
	cansel=1;
}

void Guide::OnEditIndexRs()
{
	    CString ColName;
		short Column;
		Column=m_GData.GetCol();
		ColName=(CString)(char *)(Rd->GetFields()->GetItem(_variant_t(long(Column)))->GetName());
        Rd->PutSort(_bstr_t(ColName));
}

BEGIN_EVENTSINK_MAP(Guide, CDialog)
    //{{AFX_EVENTSINK_MAP(Guide)
	ON_EVENT(Guide, IDC_DATAGRID, -605 /* MouseDown */, OnMouseDownDatagrid, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	ON_EVENT(Guide, IDC_DATAGRID, 206 /* BeforeColUpdate */, OnBeforeColUpdateDatagrid, VTS_I2 VTS_PVARIANT VTS_PI2)
	ON_EVENT(Guide, IDC_DATAGRID, 215 /* Error */, OnErrorDatagrid, VTS_I2 VTS_PI2)
	ON_EVENT(Guide, IDC_DATAGRID, 201 /* AfterColUpdate */, OnAfterColUpdateDatagrid, VTS_I2)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void Guide::OnMouseDownDatagrid(short Button, short Shift, long X, long Y) 
{
	if(Button!=1)
	{
		POINT pt;
		CRect rect;
		m_GData.GetWindowRect(&rect);
		pt.x=X;
		pt.y=Y;
		
		//ClientToScreen(&pt);
		UpdateMenu();
		m_PopMenu.TrackPopupMenu(TPM_LEFTBUTTON,pt.x+rect.left,pt.y+rect.top,this);
	}
 
}


//控制菜单活动
void Guide::UpdateMenu()
{
	if(m_GData.GetSelLength()>0 && m_GData.GetCol()>=0)
       m_PopMenu.EnableMenuItem(ID_EDIT_FILTER_RS,MF_ENABLED);
	else
		m_PopMenu.EnableMenuItem(ID_EDIT_FILTER_RS,MF_GRAYED);
	if(m_GData.GetCol()>=0)
		m_PopMenu.EnableMenuItem(ID_EDIT_INDEX_RS,MF_ENABLED);
	else
		m_PopMenu.EnableMenuItem(ID_EDIT_INDEX_RS,MF_GRAYED);
	if(m_GData.GetSelBookmarks().GetCount()>0)
		m_PopMenu.EnableMenuItem(ID_EDIT_DEL_RS,MF_ENABLED);
	else
		m_PopMenu.EnableMenuItem(ID_EDIT_DEL_RS,MF_GRAYED);
	if(cansel==0)
        m_PopMenu.EnableMenuItem(ID_EDIT_CANSEL_RS,MF_ENABLED);
	else
		m_PopMenu.EnableMenuItem(ID_EDIT_CANSEL_RS,MF_GRAYED);
     
}


void Guide::OnBUTTONGuide() 
{
	AfxGetApp()->BeginWaitCursor();
	CString Sql;
	_variant_t Book;
	try
	{
		
		//Rd->Requery(-1);
		if(m_radio==0)
		{
			ConDB->Execute(_bstr_t("DELETE FROM [connectPASA]"),NULL,adCmdText);
			Sql="INSERT INTO [connectPASA] SELECT * FROM connectPASABAK";
		}
		else
			if(m_radio==1)
			{
				ConDB->Execute(_bstr_t("DELETE FROM [connectCSPR]"),NULL,adCmdText);
				Sql="INSERT INTO [connectCSPR] SELECT * FROM connectCSPRBAK";
			}
		    else
			{
				ConDB->Execute(_bstr_t("DELETE FROM [connectSPR]"),NULL,adCmdText);
				Sql="INSERT INTO [connectSPR] SELECT * FROM connectSPRBAK";
			
			}
				ConDB->Execute(_bstr_t(Sql),NULL,adCmdText);
				Book=m_GData.GetBookmark();
				afresh();
	}
	catch(_com_error *e)
	{
		CString str=CString((char *)e->Description());
		AfxMessageBox(str);
	}
	AfxGetApp()->EndWaitCursor();
}

void Guide::OnBeforeColUpdateDatagrid(short ColIndex, VARIANT FAR* OldValue, short FAR* Cancel) 
{
	Rd->MoveNext();
	Rd->MovePrevious();
/*	try
	{
		
		_variant_t Book;
		Book=Rd->GetBookmark();
		Rd->Close();
		afresh();
		Rd->Bookmark=Book;
	}
	catch(CException *e)
	{
	}
*/
  }

void Guide::OnErrorDatagrid(short DataError, short FAR* Response) 
{
	_variant_t BOOK;
	BOOK=Rd->Bookmark;
	afresh();
	Rd->Requery(-1);
	Rd->Bookmark=BOOK;
	*Response=0;
	
}

void Guide::OnAfterColUpdateDatagrid(short ColIndex) 
{
		Rd->MoveNext();
	    Rd->MovePrevious();	
}




/////////////////////////////////////////////////////////////////////////////////////
/*
int SCol,ECol;

_RecordsetPtr Rset;
FieldPtr Field1;
extern void GDataCopyCol(_RecordsetPtr Rs,int StartCol,int EndCol)
{
	if(ECol<SCol)
	{
		int i=SCol;
		SCol=ECol;
		ECol=i;
	}
	else
	{
		SCol=StartCol;
		ECol=EndCol;
    }
	Rset=Rs;
	Field1=Rs->GetFields()->GetItem(_variant_t(long(StartCol)));
}

/////////////////////////////////////////////////////////////////////////
/*
extern void GDataPasteCol(_RecordsetPtr Rs,int StartCol,int EndCol,CDataGrid &DGrid)
{
// 第一种  速度太慢

	try
	{
		FieldPtr Fd;
		for(int i=0;i<Rs->RecordCount;)
		{
			for(int i1=0;i1<DGrid.GetVisibleRows();i1++)
			{
				Rs->Bookmark=DGrid.RowBookmark(i1);
					if(EndCol>ECol)
					{
						for(int PCol=EndCol,CCol=ECol;(PCol>=StartCol) && (CCol>=SCol);PCol--,CCol--)
						{
							 Fd=Rset->GetFields()->GetItem(_variant_t(long(CCol)));
							 DGrid.GetColumns().GetItem(_variant_t(long(PCol))).SetValue(Fd->GetValue());
							 // Rs->PutCollect(_variant_t(long(PCol)),Fd->GetValue());
						}
					}
					else
					{
						for(int PCol=StartCol,CCol=SCol;(PCol<=EndCol) && (CCol<=ECol);PCol++,CCol++)
						{	 
							 Fd=Rset->GetFields()->GetItem(_variant_t(long(CCol)));
							 DGrid.GetColumns().GetItem(_variant_t(long(PCol))).SetValue(Fd->GetValue());
						}

					}
					//	Rs->MoveNext();

			}
			i=i+i1-1;
			DGrid.Scroll(0,DGrid.GetVisibleRows());		
		}
	}
	catch(CException *e)
	{
		AfxMessageBox("字段类型不同，不能复制");
		return;
	}
	

	//////////////////////////////////////////////////////////////////////
	///////这一中，速度很快，但会出错
	/*
	try
	{
		_ConnectionPtr ConPtr;
		ConPtr.CreateInstance(__uuidof(Connection));
		ConPtr=Rs->GetActiveConnection();
		CString CColName,PColName;
		if(EndCol>ECol)
		{
			for(int PCol=EndCol,CCol=ECol;(PCol>=StartCol) && (CCol>=SCol);PCol--,CCol--)
					{
						  //FieldPtr Fd;
						  CColName=(char *)(Rset->GetFields()->GetItem(_variant_t(long(CCol)))->GetName());
						  PColName=(char *)(Rs->GetFields()->GetItem(_variant_t(long(PCol)))->GetName());
						  ConPtr->Execute(_bstr_t("UPDATE ZA SET "+PColName+"="+CColName),NULL,adCmdText);
						  //DGrid.GetColumns().GetItem(_variant_t(long(PCol))).SetValue(Fd->GetValue());
						 // Rs->PutCollect(_variant_t(long(PCol)),Fd->GetValue());
					  
					}
		}
		else
		{
			for(int PCol=StartCol,CCol=SCol;(PCol<=EndCol) && (CCol<=ECol);PCol++,CCol++)
			{
				CColName=(char *)(Rset->GetFields()->GetItem(_variant_t(long(CCol)))->GetName());
				PColName=(char *)(Rs->GetFields()->GetItem(_variant_t(long(PCol)))->GetName());
				ConPtr->Execute(_bstr_t("UPDATE ZA SET "+PColName+"="+CColName),NULL,adCmdText);		
			}
		}

		DGrid.Refresh();

	}
	catch(_com_error &e)
	{
		CString str;
		str=(char *)(e.Description());
        str=str+"\n字段类型不同,不能复制";
		AfxMessageBox(str);
	}
	*/
	//////////////////////////////////////////////////////////////////////


	/*一样，速度太慢
		int RowNum=DGrid.GetVisibleRows();
	  int RC=Rs->RecordCount;
	  DGrid.RowBookmark(0); 
	  for(int i1=0;i1<RC;)
	  {
		//	val.iVal=i1+1;
		  RowNum=DGrid.GetVisibleRows();
		  for(long i2=0;i2<RowNum;i2++)
		  {
			DGrid.SetBookmark(DGrid.RowBookmark(i2));
			DGrid.GetColumns().GetItem(_variant_t(long(2))).SetValue(Field1->GetValue());
			//Rd->Delete(adAffectCurrent);
	  		//FP->Value=_variant_t(long(0));
		   //Rd->PutCollect(_variant_t(ColName),Field1->GetValue());
			//m_GData.GetSelBookmarks().Add(m_GData.RowBookmark(i2));
			
		  }
		  //i1=i1+i2-1;
		  i1=i1+i2;
		  DGrid.Scroll(0,RowNum-1);
		  
		  
	  }
	  */
///}

///////////////////////////////////////////////////////////////////////////////
//bool DelCol;


///////////////////////////////
extern void GDataCopyCol(long RowNum,int StartCol,int EndCol,CDataGrid &m_GData)
{
	AfxGetApp()->BeginWaitCursor();
	try
	{
		CString StrText;
		for(long Row=0;Row<RowNum;)
		{
			for(int RowVb=0;RowVb<m_GData.GetVisibleRows();RowVb++)
			{
				for(int CCol=StartCol;CCol<=EndCol;CCol++)
				{
					m_GData.SetCol(CCol);
					m_GData.SetRow(RowVb);
					StrText=StrText+m_GData.GetText()+"#";
				}
			}
			Row=Row+RowVb;
			m_GData.Scroll(0,m_GData.GetVisibleRows()-2);
		}
		GLOBALHANDLE hglobal;
		char *pglobal;
		hglobal=::GlobalAlloc(GHND|GMEM_SHARE,StrText.GetLength()+1);
		pglobal=(char *)::GlobalLock(hglobal);
		strcpy(pglobal,StrText);
		::GlobalUnlock(hglobal);
		::OpenClipboard(NULL);
		::EmptyClipboard();
		::SetClipboardData(SF_TEXT,hglobal);
		::CloseClipboard();
	}
	catch(CException *e)
	{
		e->Delete();
	}
	AfxGetApp()->EndWaitCursor();
}


extern void GDataPasteCol(long RowNum,int StartCol,int EndCol,CDataGrid &m_GData)
{
	AfxGetApp()->BeginWaitCursor();
	GLOBALHANDLE hglobal;
	char *pglobal;
	int i=0;
	CString StrText,Str;
    ::OpenClipboard(NULL);
	hglobal=::GetClipboardData(CF_TEXT);
    pglobal=(char *)::GlobalLock(hglobal);
	//StrText=new char[::GlobalSize(hglobal)];
	//strcpy(StrText,pglobal);
	StrText=pglobal;
	::GlobalUnlock(hglobal);
	::CloseClipboard();
	try
	{
		for(long Row=0;Row<RowNum;)
		{
			for(int RowVb=0;RowVb<m_GData.GetVisibleRows();RowVb++)
			{
				m_GData.SetRow(RowVb);	
				for(int CCol=StartCol;CCol<=EndCol;CCol++)
				{					
					m_GData.SetCol(CCol);	
					i=StrText.Find("#");
					Str=StrText.Left(i);
					StrText=StrText.Mid(i+1);
					m_GData.SetText(Str);
				}
			}
			Row=Row+RowVb;
			m_GData.Scroll(0,m_GData.GetVisibleRows()-2);
		}
	}
	catch(CException *e)
	{
		e->Delete();
		AfxMessageBox("字段值不符");
	}
    AfxGetApp()->EndWaitCursor();
}