// PhsData.cpp : implementation file
//

#include "stdafx.h"
#include "AutoPHS.h"
#include "_recordset.h"
#include "PhsData.h"
#include "EDIBgbl.h"
#include "EDIBDB.h"
#include "FrmTxsr.h"
#include "Column.h"
#include "Columns.h"
#include "Field.h"
#include "Fields.h"
//#include "ClsCDC.h"
#include "EDIBAcad.h"
#include "FrmTxsr.h"
#include "mainfrm.h" 
#include "user.h"
#include "basDirectory.h"
#include "modRegistry.h"
#include "modPHScal.h"
////////////////////
#include "Guide.h"

extern "C" __declspec(dllexport) bool DLCreateTemplate(CString& strTemplateName, const int nTableId, const CString strDbPath);
extern "C" __declspec(dllexport) bool DLFillExcelContent(char* cSFileName,char* cDFileName,
														 const int nTableId,const _RecordsetPtr& pRs,
					                                   const char* cDbPath, const bool bAddTable=false);
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#include "Font.h"
#include "selbookmarks.h"

#define IDC_COMBOGEN		2000
#define IDC_COMBOGUAN		2001

/////////////////////////////////////////////////////////////////////////////
// CPhsData dialog
CPhsData FrmPhsData;
//extern bool DelCol; //唯一的作用，控制“粘贴列”的激活，不要混乱
extern void GDataCopyCol(long RowNum,int StartCol,int EndCol,CDataGrid &m_DGrid);
extern void GDataPasteCol(long RowNum,int StartCol,int EndCol,CDataGrid &m_DGrid);


CPhsData::CPhsData(CWnd* pParent /*=NULL*/)
	: CDialog(CPhsData::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPhsData)
	m_RawData = _T("");
	m_StuLab = _T("");
	//}}AFX_DATA_INIT
	m_AddNewStat=false;
	m_bActive=true;
	m_gridDel=false;
	m_AutoDZ=false;
	m_DataSumRs=NULL;
	m_ActiveRs=NULL;
	m_DataBillRs=NULL;
	m_RepError=true;
	m_IsDelRs=false;
	m_bIsAlDel=false;
}


void CPhsData::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPhsData)
	DDX_Control(pDX, IDC_EDIT1, m_TextINPUT);
	DDX_Control(pDX, IDC_RAWDATA, m_cRawData);
	DDX_Control(pDX, IDC_STULAB, m_cStuLab);
	DDX_Text(pDX, IDC_RAWDATA, m_RawData);
	DDX_Text(pDX, IDC_STULAB, m_StuLab);
	DDX_Control(pDX, IDC_DATAGRID1, m_DBGbill);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPhsData, CDialog)
	//{{AFX_MSG_MAP(CPhsData)
	ON_WM_ACTIVATE()
	ON_WM_CREATE()
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_COMMAND(ID_TAB_EXIT, OnTabExit)
	ON_COMMAND(ID_TAB_RET, OnRetMainMnu)
	ON_COMMAND(ID_EDIT_NUM, OnAutoML)
	ON_COMMAND(ID_SUM_AUTODZ, OnAutoDZ)
	ON_COMMAND(ID_EDIT_DEL_RS, OnDelRs)
	ON_COMMAND(ID_SUM_MATERIAL, OnSumMaterial)
	ON_COMMAND(ID_SUM_DISPLAY, OnSumDisplay)
	ON_COMMAND(ID_EDIT_DEL_TAB, OnEditDelTab)
	ON_COMMAND(ID_DATA_CAL, OnDataCal)
	ON_COMMAND(ID_DATA_DRAW, OnDrawZdjh)
	ON_COMMAND(ID_EDIT_ADD, OnEditAdd)
	ON_COMMAND(ID_EDIT_HIDE_COLUMN,OnEditHideColumn)
	ON_COMMAND(ID_EDIT_SHOW_COLUMNS,OnEditShowColumns)
	ON_COMMAND(ID_EDIT_SELECT_ALL_ROWS,OnEditSelectAllRows)
	ON_COMMAND(ID_EDIT_COPY_RS, OnEditCopyRs)
	ON_COMMAND(ID_EDIT_PASTE_RS, OnEditPasteRs)
    ON_COMMAND(ID_EDIT_DEL_RS1,OnEditDelRs)	
	ON_COMMAND(ID_EDIT_CANCLE, OnUndoPasteRs)
    ON_COMMAND(ID_EDIT_COPY_COL,OnEditCopyCol)
	ON_COMMAND(ID_EDIT_DEL_COL,OnEditDelCol)
	ON_COMMAND(IDM_EXPORT_TO_EXCEL,OnExportToExcel)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_WM_GETMINMAXINFO()
	ON_WM_MOVING()
	ON_WM_SIZING()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPhsData message handlers

//DEL void CPhsData::OnIntTabint() 
//DEL {
//DEL 	// TODO: Add your command handler code here
//DEL 
//DEL }

//DEL void CPhsData::OnDestroy() 
//DEL {
//DEL 	CDialog::OnDestroy();
//DEL 	
//DEL 	// TODO: Add your message handler code here
//DEL 	
//DEL }

int CPhsData::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here

	return 0;
}


// CHENG     //关闭时隐藏ComBol
void CPhsData::OnClose()
{	

	if(m_ComboGenDlg.m_hWnd!=NULL)
        m_ComboGenDlg.ShowWindow(SW_HIDE);
	FrmPhsData.ShowWindow(SW_HIDE);
}

// CHENG    //ComBol跟踪窗口移动
void CPhsData::OnMove(int x,int y)
{
		if (m_ComboGenDlg.m_hWnd!=NULL)
		{
            m_DBGbill.GetWindowRect(&RectDataGrid);
			Rect.top=RectCombol.top+RectDataGrid.top;
			Rect.left=RectCombol.left+RectDataGrid.left;
			Rect.bottom=RectCombol.bottom+RectDataGrid.top;
			Rect.right=RectCombol.right+RectDataGrid.left;
            m_ComboGenDlg.MoveWindow(&Rect);
            m_ComboGenDlg.MoveComboBox();
		}
}

void CPhsData::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	// TODO: Add your message handler code here
	
	const int cl=5,cr=0,ct=0,cb=5;
	int w,h;
	CRect rcClient(0,0,cx,cy);
	CRect rcTemp;
	rcClient.InflateRect(-cl,-cr,-ct,-cb);
	CWnd* pWnd;
	pWnd=GetDlgItem(IDC_RAWDATA);
	if(IsWindow(pWnd->GetSafeHwnd()))
	{
		pWnd->GetWindowRect(&rcTemp);
		ScreenToClient(&rcTemp);
		rcTemp.top=rcClient.top;
		rcTemp.bottom=rcClient.bottom-cb;
		w=rcTemp.Width();
		rcTemp.left=rcClient.left;
		rcTemp.right=rcClient.left+w;
		pWnd->MoveWindow(&rcTemp);

		rcClient.left=rcTemp.right;
	}

	pWnd=GetDlgItem(IDC_STULAB);
	if(IsWindow(pWnd->GetSafeHwnd()))
	{
		pWnd->GetWindowRect(&rcTemp);
		ScreenToClient(&rcTemp);
		rcTemp.left=rcClient.left;
		rcTemp.right=rcClient.right;
		h=rcTemp.Height();
		rcTemp.bottom=rcClient.bottom;
		rcTemp.top=rcClient.bottom-h;
		pWnd->MoveWindow(&rcTemp);

		rcClient.bottom=rcTemp.top;
	}

	pWnd=GetDlgItem(IDC_DATAGRID1);
	if(IsWindow(pWnd->GetSafeHwnd()))
	{
		rcClient.bottom-=3;
		pWnd->MoveWindow(&rcClient);
	}

	if (m_ComboGenDlg.m_hWnd==NULL) return;
	    m_ComboGenDlg.ShowWindow(SW_HIDE);

}


//UPDATE CHENG
void CPhsData::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	

	CRect rc1;
	::SystemParametersInfo(SPI_GETWORKAREA,0,(LPVOID)&rc1,0);
	((CMainFrame*)AfxGetApp()->m_pMainWnd)->InitPos();
	this->SetWindowPos(NULL,0,((CMainFrame*)AfxGetApp()->m_pMainWnd)->GetMenuHeight(),rc1.Width(),
		rc1.Height()-((CMainFrame*)AfxGetApp()->m_pMainWnd)->GetMenuHeight(),SWP_NOZORDER|SWP_NOACTIVATE);	
   
	//CHENG   当窗口隐藏ComBol跟踪隐藏
	 if(nStatus==SW_HIDE && m_ComboGenDlg.m_hWnd!=NULL)
		m_ComboGenDlg.ShowWindow(SW_HIDE);

}

BOOL CPhsData::OnInitDialog() 
{
	CDialog::OnInitDialog();
	

	HICON hIcon=::LoadIcon(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON_FRMDATA));
	SetIcon(hIcon,TRUE);
	CRect rc1;
	::SystemParametersInfo(SPI_GETWORKAREA,0,(LPVOID)&rc1,0);
	((CMainFrame*)AfxGetApp()->m_pMainWnd)->InitPos();
	this->SetWindowPos(NULL,0,((CMainFrame*)AfxGetApp()->m_pMainWnd)->GetMenuHeight(),rc1.Width(),
		rc1.Height()-((CMainFrame*)AfxGetApp()->m_pMainWnd)->GetMenuHeight(),SWP_NOZORDER|SWP_NOACTIVATE);	

 
    m_RawData.Format(IDS_MsgBox_60587, EDIBgbl::Cbtype[EDIBgbl::SelBillType].MnuCaption);
   this->InitDBbill();

	m_Menu.LoadMenu(MENU_PHSDATA);
	this->SetMenu(&m_Menu);
	m_PopUpMenu.CreatePopupMenu();
	m_PopUpMenu.AppendMenu(MF_STRING,IDM_EXPORT_TO_EXCEL,"导出记录到Excel表");
	m_PopUpMenu.AppendMenu(MF_STRING,ID_EDIT_COPY_RS,"复制记录");
	m_PopUpMenu.AppendMenu(MF_STRING,ID_EDIT_PASTE_RS,"粘贴记录");
	m_PopUpMenu.AppendMenu(MF_STRING,ID_EDIT_DEL_RS1,"删除记录");
	m_PopUpMenu.AppendMenu(MF_STRING,ID_EDIT_COPY_COL,"复制列");
	m_PopUpMenu.AppendMenu(MF_STRING,ID_EDIT_DEL_COL,"粘贴列");
	m_PopUpMenu.AppendMenu(MF_STRING,ID_EDIT_HIDE_COLUMN,"隐藏列");
	m_PopUpMenu.AppendMenu(MF_STRING,ID_EDIT_SHOW_COLUMNS,"取消隐藏列");
	m_PopUpMenu.AppendMenu(MF_STRING,ID_EDIT_SELECT_ALL_ROWS,"全选");
	m_DBGbill.SetAllowRowSizing(false);
	
	
	UpdateVector(SHDlg.bIsVisible);
	SHDlg.Create(IDD_DLG_SHOWHIDE_COLUMNS,this);
	nRowStart=nRowEnd=-1;
	UpdateData(false);
	vZeroRow=m_DBGbill.GetFirstRow();
	IsPastable=0;
	PositionLT=-100;
	
//	IncOrDec=false;

	//::SetWindowPos(m_hWnd,AfxGetApp()->m_pMainWnd->m_hWnd,0,0,0,0,SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE);
	//::SetWindowPos(AfxGetApp()->m_pMainWnd->m_hWnd,HWND_TOPMOST,0,0,0,0,SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPhsData::InitDBbill()
{
	try
	{
      CString tbn;
   //mnuSumDISPLAY.Enabled = False
   
   
	  //m_DBGbill.SetAllowAddNew(true);
	  m_DBGbill.SetAllowAddNew(false);
	  m_DBGbill.SetAllowDelete(true);
	  m_DBGbill.SetAllowUpdate(true);
	  //m_DBGbill.GetFont().SetName("system");
	  m_DBGbill.SetForeColor(0xff0000);
	  m_DBGbill.SetBackColor(0xffc0c0);
      //因为有些表的数据是共用的,只是视图不同,所以在此处设置可见性
      //字段(列)的可见性由sort.mdb中的表TableINFO和t??两个表共同管理,这些表可以用户化
      /*if(m_DataBillRs!=NULL)
		{
			if(m_DataBillRs->State!=adStateClosed)
				m_DataBillRs->Close();
			m_DataBillRs=NULL;
		}*/

	m_DataBillRs=FrmTxsr.m_pViewTxsr->m_ActiveRs;
   m_DBGbill.SetRefDataSource(m_DataBillRs->GetDataSource());
   EDIBDB::RefreshGrid(m_DBGbill,m_DataBillRs);
	//m_DBGbill.SetDataMember(FrmTxsr.m_Databill.GetRecordset().GetDataMember());
   m_DBGbill.Refresh();
   m_ActiveRs=m_DataBillRs;
   EDIBgbl::SetDBGridColumnCaptionAndWidth(m_DBGbill);
   EDIBDB::SetColumnsProperty(m_DBGbill, EDIBgbl::SelBillType);
   //DBGBill.ReBind
   //不能使用rebind,否则前面设置的caption,width均无效.
   m_DBGbill.Refresh();
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.Description());
	}
}



BEGIN_EVENTSINK_MAP(CPhsData, CDialog)
    //{{AFX_EVENTSINK_MAP(CPhsData)
	ON_EVENT(CPhsData, IDC_DATAGRID1, 218 /* RowColChange */, OnRowColChangeDBGbill, VTS_PVARIANT VTS_I2)
	ON_EVENT(CPhsData, IDC_DATAGRID1, 209 /* BeforeUpdate */, OnBeforeUpdateDBGbill, VTS_PI2)
	ON_EVENT(CPhsData, IDC_DATAGRID1, -607 /* MouseUp */, OnMouseUpDBGbill, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	ON_EVENT(CPhsData, IDC_DATAGRID1, 207 /* BeforeDelete */, OnBeforeDeleteDBGbill, VTS_PI2)
	ON_EVENT(CPhsData, IDC_DATAGRID1, 215 /* Error */, OnErrorDatagrid1, VTS_I2 VTS_PI2)
	ON_EVENT(CPhsData, IDC_DATAGRID1, -601 /* DblClick */, OnDblClickDatagrid1, VTS_NONE)
	ON_EVENT(CPhsData, IDC_DATAGRID1, -606 /* MouseMove */, OnMouseMoveDatagrid1, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	ON_EVENT(CPhsData, IDC_DATAGRID1, -605 /* MouseDown */, OnMouseDownDatagrid1, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	ON_EVENT(CPhsData, IDC_DATAGRID1, 112 /* Scroll */, OnScrollDatagrid1, VTS_PI2)
	ON_EVENT(CPhsData, IDC_DATAGRID1, -602 /* KeyDown */, OnKeyDownDatagrid1, VTS_PI2 VTS_I2)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()


void CPhsData::OnDestroy() 
{
	CDialog::OnDestroy();
	try
	{
        
	if(m_DataBillRs!=NULL)
	{
		//m_DataBillRs->UpdateBatch(adAffectAll);
		if(m_DataBillRs->State!=adStateClosed)
		m_DataBillRs->Close();
		m_DataBillRs=NULL;
	}
	if(m_DataSumRs!=NULL)
	{
		//m_DataSumRs->UpdateBatch(adAffectAll);
		if(m_DataSumRs->State!=adStateClosed)
			m_DataSumRs->Close();
		m_DataSumRs=NULL;
	}
	m_ActiveRs=NULL;
	}
	catch(_com_error e)
	{
		MessageBox(e.Description());
	}
	// TODO: Add your message handler code here
	
}

void CPhsData::OnRowColChangeDBGbill(VARIANT FAR* LastRow, short LastCol) 
{
	// TODO: Add your control notification handler code here
	try
	{
		if(LastRow !=NULL && LastRow->vt!=VT_EMPTY && LastRow->vt!=VT_NULL)
		{
			//long lBillCount = ::FrmTxsr.m_pViewTxsr->m_ActiveRs->GetRecordCount();
			//m_StuLab.Format(GetResStr(IDS_FrmPhsData_nthRecordOfRecordCount),ltos(m_DBGbill.GetRow()+1 ),ltos(lBillCount));
			modPHScal::zdjh = vtoi(::FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect("ZDjh"));
			SetRegValue(_T("Settings"),_T("PhsCurZdjhNo"),modPHScal::zdjh);
			UpdateLabel();
		}
	}
	catch(_com_error e)
	{

	}
	catch(CException *e)
	{
		e->Delete();
	}
	UpdateData(false);
}


void CPhsData::OnBeforeUpdateDBGbill(short FAR* Cancel) 
{
	// TODO: Add your control notification handler code here
//	if(m_ActiveRs==m_DataBillRs)
//	{
		//_variant_t v;
		//v=EDIBDB::GridCol(m_DBGbill,"VolumeID");
//		m_DBGbill.GetColumns().GetItem(_variant_t((long)0)).SetText(EDIBgbl::SelVlmID);
//	}
	if(m_IsDelRs || m_AddNewStat || m_DataBillRs->Status==adRecDeleted || m_DataBillRs->Status==adRecDBDeleted/* || m_bActive==false*/)
		return;
	_variant_t tmpvar;
	tmpvar.ChangeType(VT_NULL);
		if(EDIBgbl::SelBillType==EDIBgbl::TZA)
		m_DBGbill.GetColumns().GetItem(_variant_t((long)0)).SetText(ltos(EDIBgbl::SelVlmID));

	//int C=m_DBGbill.GetColumns().GetCount();
	//for(int i=0;i<C;i++)
	//{
	//	if(m_DBGbill.GetColumns().GetItem(_variant_t((long)i)).GetText()=="")
	//	{
			/*sTmp=m_DBGbill.GetColumns().GetItem(_variant_t((long)i)).GetDataField();
			if(m_DataBillRs->Fields->Item[_variant_t(sTmp)]->GetType()==202)
				m_DBGbill.SetText(" ");
			else*/
			//m_DBGbill.GetColumns().GetItem(_variant_t((long)i)).SetValue(tmpvar);
	//	}
	//}
	
	/*_variant_t v;
	_variant_t ix;
	CString sTmp;
	v=EDIBDB::GridCol(m_DBGbill,"VolumeID");
	m_DBGbill.GetColumns().GetItem(v).SetText(EDIBgbl::SelVlmID);

	//if(m_gridDel)
	//{
	ix.ChangeType(VT_I4);
	v.ChangeType(VT_EMPTY);
	int FC=m_DBGbill.GetColumns().GetCount();
	
	for(int i=0;i<FC;i++)
	{
		ix.intVal=i;
		sTmp=m_DBGbill.GetColumns().GetItem(ix).GetText();
		if(sTmp.GetLength()<=0)
		{
			sTmp=m_DBGbill.GetColumns().GetItem(ix).GetDataField();
			if(this->m_DataBillRs->Fields->Item[_variant_t(sTmp)]->GetType()==202)
				m_DBGbill.GetColumns().GetItem(ix).SetText(" ");
			else
				m_DBGbill.GetColumns().GetItem(ix).SetValue(v);
		}
	}
	//m_gridDel=false;
	//}*/
}


void CPhsData::OnMouseUpDBGbill(short Button, short Shift, long X, long Y) 
{
	// TODO: Add your control notification handler code here
	KillTimer(2000);
	
	if( EDIBgbl::SelBillType != EDIBgbl::TCL && EDIBgbl::SelBillType != EDIBgbl::TLJ )
		return;
	m_TextINPUT.ShowWindow(SW_HIDE);
	if (Button != 1 || m_DBGbill.GetCol() <= 3 )
		return;
	//Static Tm0
	CRect rc,rc2,rc3;
	_variant_t v;
	v.ChangeType(VT_I4);
	int ix=m_DBGbill.GetCol();
	v.intVal=ix;
	m_DBGbill.GetWindowRect(&rc2);
	ScreenToClient(&rc2);
	rc.left=rc2.left+m_DBGbill.GetColumns().GetItem(v).GetLeft();
	int c1=m_DBGbill.GetRow();
	int c2=m_DBGbill.RowTop(c1);
	rc.top=rc2.top-c2;
	rc.bottom=rc.top+m_DBGbill.GetRowHeight();
	rc.right=rc.left+m_DBGbill.GetColumns().GetItem(v).GetWidth();
	CString sTmp=EDIBgbl::Btype[EDIBgbl::SelBillType]+_T("num");
	v=EDIBDB::GridCol(m_DBGbill,sTmp);
	m_TextINPUT.MoveWindow(&rc);
	m_TextINPUT.SetWindowText(m_DBGbill.GetColumns().GetItem(v).GetText()+"+");
	
	m_TextINPUT.SetFocus();
	m_TextINPUT.SetSel(0);
}

void CPhsData::OnBeforeDeleteDBGbill(short FAR* Cancel) 
{
	// TODO: Add your control notification handler code here
	//if(m_ActiveRs!=NULL && m_ActiveRs->State==adStateOpen)
	//{
//		m_ActiveRs->Delete(adAffectCurrent);
//		*Cancel=true;
//	}
	//m_gridDel=true;
/*	if(m_gridDel)
		return;*/
	if(m_bIsAlDel)
	{
		m_bIsAlDel=false;
		return;
	}
	CString mstrPrompt,mstrTitle;
	_variant_t tmpvar;
	tmpvar.ChangeType(VT_I4);
	tmpvar.intVal=1;
	mstrPrompt.Format(GetResStr(IDS_ReallyDeleteCurrentNoXrecord),m_DBGbill.GetColumns().GetItem(tmpvar).GetText());
	mstrTitle=GetResStr(IDS_DeleteCurrentRecord);
	if(MessageBox( mstrPrompt,mstrTitle,MB_ICONQUESTION|MB_DEFBUTTON2|MB_YESNO|MB_TASKMODAL|MB_TOPMOST)==IDNO)
	{
		*Cancel=true;
	}

	try
	{
		m_ActiveRs->Update();
	}
	catch(CException *e)
	{
		e->Delete();
		m_ActiveRs->CancelUpdate();
	}
}

void CPhsData::OnTabExit() 
{
	// TODO: Add your command handler code here
	ShowWindow(SW_HIDE);
}



void CPhsData::OnRetMainMnu() 
{
	// TODO: Add your command handler code here
	AfxGetApp()->m_pMainWnd->ShowWindow(SW_SHOW);
	ShowWindow(SW_HIDE);
}

void CPhsData::OnAutoML() 
{
	// TODO: Add your command handler code here
	if (EDIBgbl::SelBillType != EDIBgbl::TML)
		return;
   /*Dim i As Integer
   Dim sTmp
   Dim rs As Recordset*/
	try
	{
	bool bf=false;
	_variant_t pCon=m_ActiveRs->GetActiveConnection();
	_variant_t sour=m_ActiveRs->GetSource();
	if(m_ActiveRs!=NULL && m_ActiveRs->State!=adStateClosed)
	{
		bf=true;
		m_ActiveRs->Close();
	}
	_variant_t tmpvar;
	CString strExecute;
	strExecute = CString("DELETE * FROM [") + EDIBgbl::TBNSelPrjSpec + EDIBgbl::Btype[EDIBgbl::SelBillType] + "] WHERE trim(drawNa)=\'\'";
	EDIBgbl::dbPRJDB->Execute((_bstr_t)strExecute, NULL, adCmdText);
	if(bf)
		m_ActiveRs->Open(sour,pCon,adOpenStatic,adLockOptimistic,adCmdText);
   //InitDBbill
	CString sTmp,sz;
	int i=0;
	while(!m_ActiveRs->adoEOF)
	{
		sz=EDIBAcad::GetDrawIDAndName(i + 1,sTmp);
		m_ActiveRs->PutCollect(_variant_t("DrawNo"),_variant_t(sz));
		m_ActiveRs->MoveNext();
        i++;
	}
	}
	catch(_com_error e)
	{
		MessageBox(e.Description());
	}
}

void CPhsData::OnAutoDZ() 
{
	// TODO: Add your command handler code here
	MENUITEMINFO mi;
	mi.cbSize=sizeof(mi);
	if(m_AutoDZ)
	{
		m_AutoDZ=false;
		mi.fMask=MIIM_STATE;
		mi.fState =MFS_ENABLED;
	}
	else
	{
		m_AutoDZ=true;
		mi.fMask=MIIM_STATE;
		mi.fState=MFS_CHECKED|MFS_ENABLED;
	}
	::SetMenuItemInfo(GetMenu()->GetSubMenu(1)->m_hMenu,ID_SUM_AUTODZ,false,&mi);
	
}

void CPhsData::OnDelRs() 
{
	///cheng 改动 添加了delete多条记录
	// TODO: Add your command handler code here
	try
	{
	/*if(m_ActiveRs!=NULL && m_ActiveRs->State!=adStateClosed && !m_ActiveRs->BOF && !m_ActiveRs->adoEOF)
	{
		CString mstrPrompt,mstrTitle;
		_variant_t tmpvar;
		tmpvar.ChangeType(VT_I4);
		tmpvar.intVal=1;
		mstrPrompt.Format(GetResStr(IDS_ReallyDeleteCurrentNoXrecord),m_DBGbill.GetColumns().GetItem(tmpvar).GetText());
		mstrTitle=GetResStr(IDS_DeleteCurrentRecord);
		if(MessageBox(mstrPrompt,mstrTitle,MB_ICONQUESTION|MB_DEFBUTTON2|MB_YESNO)==IDNO)
			return;
		m_bIsAlDel=true;
		m_IsDelRs=true;
		try
		{
			m_ActiveRs->Update();
		}
		catch(CException *e)
		{
			m_ActiveRs->CancelUpdate();
		}
		m_ActiveRs->Delete(adAffectCurrent);

       //Update cheng 
		if(m_ComboGenDlg.m_hWnd!=NULL)
			m_ComboGenDlg.ShowWindow(SW_HIDE);

		UpdateLabel();
	}
	*/
	if(EDIBgbl::SelBillType==EDIBgbl::TZA)
        FrmTxsr.m_pViewTxsr->EditDel();
	if(m_ComboGenDlg.m_hWnd!=NULL)
		m_ComboGenDlg.ShowWindow(SW_HIDE);
	}
	catch(_com_error e)
	{
		MessageBox(e.Description());
	}
	nRowStart=nRowEnd=-1;
	//m_IsDelRs=false;
}

void CPhsData::OnSumMaterial() 
{
	// TODO: Add your command handler code here
	/*EDIBDB::SumMaterial();
	MessageBox(EDIBgbl::SelPrjName+GetResStr(IDS_MsgBox_60588));
	GetMenu()->GetSubMenu(2)->EnableMenuItem(1,MF_BYPOSITION|MF_ENABLED);*/
}

void CPhsData::OnSumDisplay() 
{
	// TODO: Add your command handler code here
	if( EDIBgbl::SelBillType != EDIBgbl::TCL && EDIBgbl::SelBillType != EDIBgbl::TZC)
      EDIBgbl::SelBillType = EDIBgbl::TCL;
   
   EDIBgbl::SQLx = CString("SELECT * FROM [") + EDIBgbl::TBNSelPrjSpec + EDIBgbl::Btype[EDIBgbl::TCL] + "] ORDER BY VolumeID,CLgg";
   //m_DataBillRs->Update();
   //m_DataBillRs->Close();
   try{
	//this->m_DataSumRs->CursorLocation=adUseClient;
		if(m_DataSumRs->State!=adStateClosed)
			m_DataSumRs->Close();
		this->m_DataSumRs->Open(_variant_t(EDIBgbl::SQLx),(IDispatch*)::conPRJDB,adOpenStatic,adLockOptimistic,adCmdText);
   //m_DBGbill.UpdateData();
		EDIBDB::RefreshGrid(m_DBGbill,this->m_DataSumRs);
		m_ActiveRs=this->m_DataSumRs;
		EDIBDB::SetColumnsProperty(m_DBGbill,EDIBgbl::SelBillType);
   }
   catch(_com_error &e)
   {
	   AfxMessageBox(e.Description());
   }
}

//DEL void CPhsData::OnAfterUpdateDBGbill() 
//DEL {
//DEL 	// TODO: Add your control notification handler code here
//DEL 	//FrmTxsr.RefreshBoundData();
//DEL }

void CPhsData::OnActivate(UINT nState, CWnd *pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState,pWndOther,bMinimized);
	static UINT oldState=0;
	try
	{

		//变量 bIn , bOut 防止多次执行窗口激活事件产生死循环！ 
		static bool bIn=false; 
		static bool bOut=false;
		if(nState==WA_INACTIVE)			//窗口退出激活状态
		{
			if(!bOut)
			{
				bOut=true;
				m_bActive=false;

				if(!this->m_ActiveRs->adoEOF && !this->m_ActiveRs->BOF)
				{
					try
					{
						m_ActiveRs->Update();
					}
					catch(CException *e)
					{
						m_ActiveRs->CancelUpdate();
					}
				}
				bOut=false;
			}
			else
				bOut=false;
		}
		else						////窗口进入激活状态
		{
			if(!bIn)
			{
				bIn=true;
				if(!this->m_ActiveRs->adoEOF && !this->m_ActiveRs->BOF)
				{
				}
				UpdateLabel();
				SetWindowText(EDIBgbl::Cbtype[EDIBgbl::SelBillType].MnuCaption + "    " + EDIBgbl::TBNSelPrjSpec + EDIBgbl::Btype[EDIBgbl::SelBillType] + " (" + EDIBgbl::SelJcdm + ")");
				m_bActive=true;
				bIn=false;
			}
			else
				bIn=false;
		}
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.Description());
	}
	oldState=nState;
}

void CPhsData::CloseRs()
{

}

void CPhsData::OnErrorDatagrid1(short DataError, short FAR* Response) 
{
	*Response=0;

	if(!m_RepError)
		*Response=0;
}


void CPhsData::OnEditDelTab() 
{
try
{
	conPRJDB->Execute(_bstr_t(CString("DELETE * FROM [") + EDIBgbl::Btype[EDIBgbl::SelBillType] + "] WHERE trim(VolumeID)=" + ltos(EDIBgbl::SelVlmID)),NULL,adCmdText);
	m_ActiveRs->Requery(-1);
			m_DBGbill.SetRefDataSource(m_ActiveRs);
			m_DBGbill.ReBind();
			EDIBDB::SetColumnsProperty(FrmPhsData.m_DBGbill, EDIBgbl::SelBillType);
	if(m_ComboGenDlg.m_hWnd!=NULL)
				m_ComboGenDlg.ShowWindow(SW_HIDE);
}
catch(_com_error *e)
{
	ShowMessage(e->Description());
}
}

void CPhsData::OnDataCal() 
{
	// TODO: Add your command handler code here
	((CMainFrame*)AfxGetApp()->m_pMainWnd)->OnCalcZdjh();
}

void CPhsData::OnDblClickDatagrid1() 
{
	// TODO: Add your control notification handler code here
	((CMainFrame*)AfxGetApp()->m_pMainWnd)->OnCalcZdjh();
	
}

BOOL CPhsData::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	TranslateAccelerator(this->m_hWnd,((CAutoPHSApp*)AfxGetApp())->m_hAccelData,pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

void CPhsData::OnDrawZdjh() 
{
	// TODO: Add your command handler code here
	((CMainFrame*)AfxGetApp()->m_pMainWnd)->OnDrawZdj();
}

BOOL CPhsData::Create(UINT IDD ,CWnd* pParentWnd) 
{
	return CDialog::Create(IDD, NULL);
}

CPhsData::~CPhsData()
{
	try
	{
		m_DataBillRs=NULL;
		m_DataSumRs=NULL;
		m_ActiveRs=NULL;
	}
	catch(CException *e)
	{
		e->Delete();
	}
}

void CPhsData::OnEditAdd() 
{
	// TODO: Add your command handler code here
	if(EDIBgbl::SelBillType==EDIBgbl::TZA)
	{
		FrmTxsr.m_pViewTxsr->AddNewRec();
	}

}

void CPhsData::OnGetMinMaxInfo(MINMAXINFO *lpMMI)
{
	CRect rect,rc1;
	AfxGetApp()->m_pMainWnd->GetWindowRect(&rect);
	::SystemParametersInfo(SPI_GETWORKAREA,0,(LPVOID)&rc1,0);
	lpMMI->ptMaxSize.y=rc1.Height()-rect.Height();
	lpMMI->ptMaxPosition.y=rect.Height();
	lpMMI->ptMaxPosition.x=0;
	lpMMI->ptMaxSize.x=rc1.Width();
}

void CPhsData::OnMoving(UINT fwSide, LPRECT pRect)
{
	if(pRect->top < ((CMainFrame*)AfxGetMainWnd())->GetMenuHeight())
	{
		CRect rc;
		GetWindowRect(&rc);
		pRect->top=((CMainFrame*)AfxGetMainWnd())->GetMenuHeight();
		pRect->bottom=pRect->top+rc.Height();
	}
}

void CPhsData::OnSizing(UINT fwSide, LPRECT pRect)
{
	if(pRect->top < ((CMainFrame*)AfxGetMainWnd())->GetMenuHeight())
		pRect->top=((CMainFrame*)AfxGetMainWnd())->GetMenuHeight();
}

void CPhsData::UpdateLabel()
{
	try
	{  
		_RecordsetPtr rsTmp=FrmTxsr.m_pViewTxsr->m_ActiveRs->Clone(adLockReadOnly);
		rsTmp->Filter=FrmTxsr.m_pViewTxsr->m_ActiveRs->Filter;
		int iIndex=0;
		if(!rsTmp->adoEOF && !rsTmp->BOF)
		{
			rsTmp->Bookmark=FrmTxsr.m_pViewTxsr->m_ActiveRs->Bookmark;
			int zdjh=vtoi(rsTmp->GetCollect(L"Zdjh"));
			for(rsTmp->MoveFirst();!rsTmp->adoEOF;rsTmp->MoveNext())
			{
				iIndex++;
				if(vtoi(rsTmp->GetCollect(L"Zdjh"))==zdjh)
					break;
			}
		}
		long lBillCount=rsTmp->GetRecordCount();
		m_StuLab.Format(GetResStr(IDS_FrmPhsData_nthRecordOfRecordCount),ltos(iIndex),ltos(lBillCount));
		UpdateData(FALSE);
		rsTmp->Close();
		rsTmp=NULL;
	}
	catch(CException *e)
	{
		e->Delete();
	}
}

void CPhsData::OnEditHideColumn()
{
	
	CColumns Columns;
	CColumn Column;
	VARIANT Var;

	short nColStart,nColEnd;
	nColStart=m_DBGbill.GetSelStartCol();
	nColEnd=m_DBGbill.GetSelEndCol();
	for (short i=nColStart;i<=nColEnd;i++)
	{
		SHDlg.m_CheckListBox.SetCheck(i,0);
		Columns=m_DBGbill.GetColumns();
		Var.vt=VT_I2;
		Var.iVal=i;
		Column=Columns.GetItem(Var);
		Column.SetVisible(false);
	}
}

void CPhsData::OnEditShowColumns()
{
	SHDlg.ShowWindow(SW_SHOW);
}

void CPhsData::OnEditSelectAllRows()
{
	nRowBegin=0;
	nRowFinish=GetRowsCount();
	nRowStart=nRowEnd=0;
	int Counter=0;
	m_DBGbill.Scroll(0,-nRowFinish);
	while (Counter<(nRowFinish))
	{
		for (short i=nRowBegin;i<m_DBGbill.GetVisibleRows();i++)
		{
			m_DBGbill.GetSelBookmarks().Add(m_DBGbill.RowBookmark(i));
		}
		Counter=Counter+m_DBGbill.GetVisibleRows();
		m_DBGbill.Scroll(0,m_DBGbill.GetVisibleRows());

	}
	//在没有记录时单机了“全选“，所以要用IF判断 cheng
	if(m_DBGbill.GetSelBookmarks().GetCount()>=1)
	    UpdateMenu();
}


long CPhsData::GetRowsCount()
{
	ASSERT(FrmTxsr.m_pViewTxsr->m_ActiveRs != NULL);
	try
	{
		long count = FrmTxsr.m_pViewTxsr->m_ActiveRs->GetRecordCount();

		// 如果ado不支持此属性，则手工计算记录数目 --------
		if (count < 0)
		{
			long pos = FrmTxsr.m_pViewTxsr->m_ActiveRs->GetAbsolutePosition();
			FrmTxsr.m_pViewTxsr->m_ActiveRs->MoveFirst();
			count = 0;
			while (!FrmTxsr.m_pViewTxsr->m_ActiveRs->adoEOF) 
			{
				count++;
				FrmTxsr.m_pViewTxsr->m_ActiveRs->MoveNext();
			}
			FrmTxsr.m_pViewTxsr->m_ActiveRs->PutAbsolutePosition((enum PositionEnum)pos);
		}
		return count;
	}
	catch (_com_error e)
	{
		TRACE(_T("Warning: GetRecordCount 方法发生异常. 错误信息: %s; 文件: %s; 行: %d\n"), e.ErrorMessage(), __FILE__, __LINE__);
		return -1;
	} 
}

void CPhsData::OnMouseMoveDatagrid1(short Button, short Shift, long X, long Y) 
{
	// TODO: Add your control notification handler code here
	//float Top=27;
	//float Height=24;
	long CurRow;
	
	if (Button==1&&nRowStart!=-1&&!(FrmTxsr.m_pViewTxsr->m_ActiveRs->adoEOF))
	{
		
		nRowEnd=m_DBGbill.RowContain(Y);
		CurRow=GetAbsoluteRow(nRowEnd);
		switch (UpOrDown(Y))
		{
		case 1://down
			//  chenbl 为了不影响 Ctrl键多选且不了现致命错误，所以屏蔽 chenbl 2011.06.20 
			if (CurRow<nRowBegin&&CurRow!=RowLT)
			{
				VARIANT vVal;
				vVal.vt=VT_I2;
				vVal.lVal=m_DBGbill.GetSelBookmarks().GetCount()-1;
				//IncOrDec=false;
				m_DBGbill.GetSelBookmarks().Remove(vVal);
				vVal.lVal=m_DBGbill.GetSelBookmarks().GetCount()-1;
				FrmTxsr.m_pViewTxsr->m_ActiveRs->Bookmark=m_DBGbill.GetSelBookmarks().GetItem(vVal);

			}
			else if (CurRow==nRowBegin)
			{
				if (m_DBGbill.GetSelBookmarks().GetCount()>1)
				{
					VARIANT vVal;
					vVal.vt=VT_I2;
					vVal.lVal=1;
//					IncOrDec=true;
					m_DBGbill.GetSelBookmarks().Remove(vVal);
					vVal.lVal=0;
					FrmTxsr.m_pViewTxsr->m_ActiveRs->Bookmark=m_DBGbill.GetSelBookmarks().GetItem(vVal);
				}
			}
			else if (nRowEnd<m_DBGbill.GetVisibleRows())
			{
				m_DBGbill.GetSelBookmarks().Add(m_DBGbill.RowBookmark(nRowEnd));
				FrmTxsr.m_pViewTxsr->m_ActiveRs->Bookmark=m_DBGbill.RowBookmark(nRowEnd);
			}
			
			/*
			m_DBGbill.GetSelBookmarks().Add(m_DBGbill.RowBookmark(nRowEnd));
				FrmTxsr.m_pViewTxsr->m_ActiveRs->Bookmark=m_DBGbill.RowBookmark(nRowEnd);
				*/
			break;
		case -1://up
			//   chenbl 为了不影响 Ctrl键多选且不了现致命错误，所以屏蔽 chenbl 2011.06.20 
			if (CurRow>nRowBegin&&CurRow!=RowLT&&nRowEnd<m_DBGbill.GetVisibleRows())
			{
				
				VARIANT vVal;
				vVal.vt=VT_I2;
				vVal.lVal=m_DBGbill.GetSelBookmarks().GetCount()-1;
			//	IncOrDec=false;
				m_DBGbill.GetSelBookmarks().Remove(vVal);
				vVal.lVal=m_DBGbill.GetSelBookmarks().GetCount()-1;

				FrmTxsr.m_pViewTxsr->m_ActiveRs->Bookmark=m_DBGbill.GetSelBookmarks().GetItem(vVal);
				
			}
			else if (CurRow==nRowBegin)
			{
				if (m_DBGbill.GetSelBookmarks().GetCount()>1)
				{
					VARIANT vVal;
					vVal.vt=VT_I2;
					vVal.lVal=1;
				//	IncOrDec=true;
					m_DBGbill.GetSelBookmarks().Remove(vVal);
					vVal.lVal=0;
					FrmTxsr.m_pViewTxsr->m_ActiveRs->Bookmark=m_DBGbill.GetSelBookmarks().GetItem(vVal);
				}
			}
			else
			{
			//	IncOrDec=true;
			
				m_DBGbill.GetSelBookmarks().Add(m_DBGbill.RowBookmark(nRowEnd));
				FrmTxsr.m_pViewTxsr->m_ActiveRs->Bookmark=m_DBGbill.RowBookmark(nRowEnd);
				
			}
			
			/*
			m_DBGbill.GetSelBookmarks().Add(m_DBGbill.RowBookmark(nRowEnd));
			FrmTxsr.m_pViewTxsr->m_ActiveRs->Bookmark=m_DBGbill.RowBookmark(nRowEnd);
			*/
			
			break;
		}
		RowLT=CurRow;
		nRowFinish=CurRow;
	}
	else//Update Menu
		
		UpdateMenu();
}

void CPhsData::OnMouseDownDatagrid1(short Button, short Shift, long X, long Y) 
{
	// TODO: Add your control notification handler code here
	nShresh=RowsInOnePage()-1;
	if (Button==1)
	{
		//int Top=26;
		//int Height=24;
		ClearBookmarks(); //为了不影响 Ctrl键多选，所以屏蔽 chenbl 2011.06.07
		if (X>=0&&X<=19&&Y>=0&&Y<=26)//Selecting All
		{
			HideComboBox();
			OnEditSelectAllRows();
			return;
		}
		else if (Y<=26)//Not Selecting Row
		{
			HideComboBox();
			UpdateMenu();
			return;
		}
		else if (X>=0&&X<=19)//Selecting Rows
		{
			KillTimer(2000);
			nRowStart=m_DBGbill.RowContain(Y);
			nRowBegin=GetAbsoluteRow(nRowStart); //为了不影响 Ctrl键多选，所以屏蔽 chenbl 2011.06.07
			nRowFinish=nRowBegin;
			m_DBGbill.GetSelBookmarks().Add(m_DBGbill.RowBookmark(nRowStart)); //为了不影响 Ctrl键多选，所以屏蔽 chenbl 2011.06.07
			FrmTxsr.m_pViewTxsr->m_ActiveRs->Bookmark=m_DBGbill.RowBookmark(nRowStart);
			nRowEnd=nRowStart;
			HideComboBox();//This function can only be put here.
			UpdateMenu();
			if (Y>26)
				PositionLT=Y;
			else
				PositionLT==-100;
			SetTimer(2000,300,0);
			return;
		}
		else if (X>19)//Selecting a cell
		{
			HideComboBox();//Show之前先Hide一下，更新数据
			
			/*VARIANT val;
			float posXS=20;
			short col;
			float temp;
			//col=m_DBGbill.ColContaining(X);
			val.vt=VT_I2;//以下取代了ColContaining函数，因为原来的ColContaining是错的
			float ratio;
			CSize sizeScreen;
			sizeScreen.cx=GetSystemMetrics(SM_CXSCREEN);
			sizeScreen.cy=GetSystemMetrics(SM_CYSCREEN);
			switch (sizeScreen.cx*sizeScreen.cy)
			{
			case 640*480:ratio=112.0/59.0;
				break;
			case 800*600:ratio=90.0/59.0;
				break;
			case 1024*768:ratio=70.0/59.0;
				break;
			}
			for (short i=m_DBGbill.GetLeftCol();posXS<=X;i++)
			{
				val.iVal=i;
				if (!SHDlg.bIsVisible[i]) continue;
				temp=m_DBGbill.GetColumns().GetItem(val).GetWidth();	
				posXS+=m_DBGbill.GetColumns().GetItem(val).GetWidth()*ratio;
			}
			col=i-1;
			if (col==3||col==11)*/
			ShowComboBox(X, Y);

			if (Y>26)
				PositionLT=Y;
			else
				PositionLT==-100;
		}
	}
	else
	{
		HideComboBox();
		m_PopUpMenu.TrackPopupMenu(TPM_LEFTBUTTON,X+32,Y+92,this);
	}
}

//控制菜单的活动状态

void CPhsData::UpdateMenu()
{
	if (m_DBGbill.GetSelStartCol()!=-1&&m_DBGbill.GetSelEndCol()!=-1)
	{
		m_Menu.EnableMenuItem(ID_EDIT_HIDE_COLUMN,MF_ENABLED);
		m_PopUpMenu.EnableMenuItem(ID_EDIT_HIDE_COLUMN,MF_ENABLED);
	}
	else
	{
		m_Menu.EnableMenuItem(ID_EDIT_HIDE_COLUMN,MF_GRAYED);
		m_PopUpMenu.EnableMenuItem(ID_EDIT_HIDE_COLUMN,MF_GRAYED);
	}
	
	if (nRowStart!=-1&&nRowEnd!=-1)
	{
		m_Menu.EnableMenuItem(ID_EDIT_COPY_RS,MF_ENABLED);
		m_PopUpMenu.EnableMenuItem(ID_EDIT_COPY_RS,MF_ENABLED);
		/////////////////////////////////////////////////
		m_Menu.EnableMenuItem(ID_EDIT_DEL_RS,MF_ENABLED);
		m_PopUpMenu.EnableMenuItem(ID_EDIT_DEL_RS1,MF_ENABLED);
		//导出记录到Excel表   项
		m_Menu.EnableMenuItem(IDM_EXPORT_TO_EXCEL,MF_ENABLED);
		m_PopUpMenu.EnableMenuItem(IDM_EXPORT_TO_EXCEL,MF_ENABLED);
	}
	else
	{
		m_Menu.EnableMenuItem(ID_EDIT_COPY_RS,MF_GRAYED);
		m_PopUpMenu.EnableMenuItem(ID_EDIT_COPY_RS,MF_GRAYED);
		////////////////////////////////////////////////
		m_Menu.EnableMenuItem(ID_EDIT_DEL_RS,MF_GRAYED);
		m_PopUpMenu.EnableMenuItem(ID_EDIT_DEL_RS1,MF_GRAYED);
		//导出记录到Excel表   项
		m_Menu.EnableMenuItem(IDM_EXPORT_TO_EXCEL,MF_GRAYED);
		m_PopUpMenu.EnableMenuItem(IDM_EXPORT_TO_EXCEL,MF_GRAYED);
	}
	
	if (IsPastable)
	{
		m_Menu.EnableMenuItem(ID_EDIT_PASTE_RS,MF_ENABLED);
		m_PopUpMenu.EnableMenuItem(ID_EDIT_PASTE_RS,MF_ENABLED);
	}
	else
	{
		m_Menu.EnableMenuItem(ID_EDIT_PASTE_RS,MF_GRAYED);
		m_PopUpMenu.EnableMenuItem(ID_EDIT_PASTE_RS,MF_GRAYED);
	}
	///改变"复制列"菜单
	if(m_DBGbill.GetSelStartCol()!=-1)
		m_PopUpMenu.EnableMenuItem(ID_EDIT_COPY_COL,MF_ENABLED);
	else
        m_PopUpMenu.EnableMenuItem(ID_EDIT_COPY_COL,MF_GRAYED);
	//改变"粘贴列"菜单

	if(m_DBGbill.GetSelStartCol()!=-1)
			m_PopUpMenu.EnableMenuItem(ID_EDIT_DEL_COL,MF_ENABLED);
	     else
           m_PopUpMenu.EnableMenuItem(ID_EDIT_DEL_COL,MF_GRAYED);

}

void CPhsData::ClearBookmarks()
{
	short i;
	VARIANT Var;
	Var.vt=VT_I2;
	for (i=m_DBGbill.GetSelBookmarks().GetCount()-1;i>=0;i--)
	{
		Var.iVal=i;
		m_DBGbill.GetSelBookmarks().Remove(Var);
	}
	nRowStart=nRowEnd=-1;
}

int CPhsData::UpOrDown(long Y)
{
	if (Y>PositionLT)
	{
		PositionLT=Y;
		return 1;//down
	}
	else if (Y==PositionLT)
	{
		PositionLT=Y;
		return 0;//still
	}
	else
	{
		PositionLT=Y;
		return -1;//up
	}
}

long CPhsData::GetFirstRow()//虽然取得的值不一定是对的，但是够用了。
{
	VARIANT vFirstRow=m_DBGbill.GetFirstRow();
	return (long)(vFirstRow.dblVal-vZeroRow.dblVal);

	/*VARIANT Bookmark;//以下取得的值是对的，但是用了会很惨。
	long i=0;
	ASSERT(FrmTxsr.m_pViewTxsr->m_ActiveRs != NULL);
	Bookmark=FrmTxsr.m_pViewTxsr->m_ActiveRs->Bookmark;
	FrmTxsr.m_pViewTxsr->m_ActiveRs->Bookmark=m_DBGbill.GetFirstRow();
	while (!FrmTxsr.m_pViewTxsr->m_ActiveRs->BOF)
	{
		FrmTxsr.m_pViewTxsr->m_ActiveRs->MovePrevious();
		i++;
	}
	FrmTxsr.m_pViewTxsr->m_ActiveRs->Bookmark=Bookmark;
	return i;*/
}

long CPhsData::GetAbsoluteRow(long Row)//虽然取得的值不一定是对的，但是够用了。
{
	long nFirstRow=GetFirstRow();
	return nFirstRow+Row;

	/*VARIANT Bookmark;//以下取得的值是对的，但是用了会很惨。
	long i=0;
	ASSERT(FrmTxsr.m_pViewTxsr->m_ActiveRs != NULL);
	Bookmark=FrmTxsr.m_pViewTxsr->m_ActiveRs->Bookmark;
	FrmTxsr.m_pViewTxsr->m_ActiveRs->Bookmark=m_DBGbill.RowBookmark(Row);
	while (!FrmTxsr.m_pViewTxsr->m_ActiveRs->BOF)
	{
		FrmTxsr.m_pViewTxsr->m_ActiveRs->MovePrevious();
		i++;
	}
	FrmTxsr.m_pViewTxsr->m_ActiveRs->Bookmark=Bookmark;
	return i;*/
}

long CPhsData::RowsInOnePage()
{
	CRect Rect;
	int nRows;
	m_DBGbill.GetClientRect(Rect);
	nRows=(Rect.bottom-Rect.top-45)/24;
	return nRows;
}

void CPhsData::UpdateVector(store& bIsVisible)
{
	VARIANT val;
	bIsVisible.clear();
	val.vt=VT_I2;
	for (short i=0;i<m_DBGbill.GetColumns().GetCount();i++)
	{
		val.iVal=i;
		bIsVisible.push_back(m_DBGbill.GetColumns().GetItem(val).GetVisible());
	}
}

void CPhsData::HideComboBox()
{
	if (m_ComboGenDlg.m_hWnd==NULL||!m_ComboGenDlg.m_bIsVisible) return;
	m_ComboGenDlg.UpdateValue(true);
	m_ComboGenDlg.ShowWindow(SW_HIDE);
	m_ComboGenDlg.SetVisibleState(false);
}

void CPhsData::ShowComboBox(long X, long Y)
{
	
	CString ColName; //cheng
	short row,col;
	VARIANT val;
	float posXS=20;
	float ratio,preadd,ratio1;
	CSize sizeScreen;
	sizeScreen.cx=GetSystemMetrics(SM_CXSCREEN);
	sizeScreen.cy=GetSystemMetrics(SM_CYSCREEN);
	/*CClientDC dc(&m_DBGbill);
	SIZE sizewindow,sizeviewport;
	GetWindowExtEx(dc,&sizewindow);
	GetViewportExtEx(dc,&sizeviewport);
	POINT ptviewport,ptwindow;
	GetViewportOrgEx(dc,&ptviewport);
	GetWindowOrgEx(dc,&ptwindow);
    */

	//cheng  由于暂时不能动态获得比例,只有分开求出  ComBol控件只能在以下分辨率工作   
	switch (sizeScreen.cx*sizeScreen.cy)
	{
	case 640*480:ratio=2.0;
			     ratio1=112.0/59.0;
		break;
	case 720*480:ratio=24.0/14.0;
		         ratio1=98.0/58.0;
				 break;
	case 720*576:ratio=24.0/14.0;
		          ratio1=98.0/58.0;
				 break;
	case 800*600:
		ratio=8.0/5.0;
        ratio1=89.5/59.0;
		break;
	case 848*480:
		 ratio=24.0/16.0;
		 ratio1=98.0/68.5;
		 break;
	case 1024*768:ratio=6.0/5.0;
		          ratio1=69.7/59.0;
		break;
	case 1152*864:ratio=24.0/22.0;
		          ratio1=98.0/93.0;
		break;
	case 1280*1024:ratio=1.0;
		           ratio1=98.0/103.5;
		break;
    
	}

	
	row=m_DBGbill.RowContain(Y);
	val.vt=VT_I2;//以下取代了ColContaining函数，因为原来的ColContaining是错的
	for (short i=m_DBGbill.GetLeftCol();posXS<=X;i++)
	{
		val.iVal=i;
		if (!SHDlg.bIsVisible[i]) continue;
		posXS+=m_DBGbill.GetColumns().GetItem(val).GetWidth()*ratio;
	}
	
	col=i-1;
    val.iVal=col;
	ColName=m_DBGbill.GetColumns().GetItem(val).GetCaption();
	if (ColName!="管部类型" && ColName!="根部类型") return;

	m_DBGbill.GetWindowRect(&RectDataGrid);

	//cheng    ComBol 在DataGrid里得坐标
	RectCombol.top=(row+1)*m_DBGbill.GetRowHeight()*ratio+m_DBGbill.GetHeadLines()*ratio;
	RectCombol.left=m_DBGbill.GetColumns().GetItem(val).GetLeft()*ratio1;
	RectCombol.bottom=RectCombol.top+m_DBGbill.GetRowHeight()*ratio;
	RectCombol.right=RectCombol.left+m_DBGbill.GetColumns().GetItem(val).GetWidth()*ratio1;
	
	//cheng  ComBol 在屏幕上的坐标
    Rect.top=RectCombol.top+RectDataGrid.top;
	Rect.left=RectCombol.left+RectDataGrid.left;
	Rect.bottom=RectCombol.bottom+RectDataGrid.top;
	Rect.right=RectCombol.right+RectDataGrid.left;
	if(Rect.right>RectDataGrid.right)
		Rect.right=RectDataGrid.right;

	FrmTxsr.m_pViewTxsr->m_ActiveRs->Bookmark=m_DBGbill.RowBookmark(row);
	if (m_ComboGenDlg.m_hWnd==NULL)
	{
		m_ComboGenDlg.Create(IDD_COMBOGEN_DLG,this);
		m_ComboGenDlg.m_nRow=row;
		m_ComboGenDlg.m_nCol=col;
		m_ComboGenDlg.UpdateValue(false);
		m_ComboGenDlg.MoveWindow(Rect);
		m_ComboGenDlg.MoveComboBox();
		switch (col)
		{
		case 3:
			m_ComboGenDlg.LoadComboBox(1);
			break;
		case 11:
			m_ComboGenDlg.LoadComboBox(2);
			break;
		}
		m_ComboGenDlg.ShowWindow(SW_SHOW);
		m_ComboGenDlg.SetVisibleState(true);
	}
	else
	{
		m_ComboGenDlg.m_nRow=row;
		m_ComboGenDlg.m_nCol=col;
		m_ComboGenDlg.UpdateValue(false);
		m_ComboGenDlg.MoveWindow(Rect);
		m_ComboGenDlg.MoveComboBox();
		switch (col)
		{
		case 3:
			m_ComboGenDlg.LoadComboBox(1);
			break;
		case 11:
			m_ComboGenDlg.LoadComboBox(2);
			break;
		}
		m_ComboGenDlg.ShowWindow(SW_SHOW);
		m_ComboGenDlg.SetVisibleState(true);
	}
}

void CPhsData::OnEditCopyRs() 
{
	// TODO: Add your command handler code here
    if(EDIBgbl::SelBillType==EDIBgbl::TZA)
	{
    	FrmTxsr.m_pViewTxsr->EditCopy();
		IsPastable=1;
	}
	
}

void CPhsData::OnUndoPasteRs() 
{
	// TODO: Add your command handler code here
    if(EDIBgbl::SelBillType==EDIBgbl::TZA)
	{
    	FrmTxsr.m_pViewTxsr->UndoPaste();
	}	
}

void CPhsData::OnEditPasteRs() 
{
	// TODO: Add your command handler code here
	if(EDIBgbl::SelBillType==EDIBgbl::TZA)
	{
		FrmTxsr.m_pViewTxsr->EditPaste();
	}
	nRowStart=nRowEnd=-1;
	IsPastable=0;//pfg20050803原代码
}

//单机删除记录时发生
void CPhsData::OnEditDelRs()
{
 if(EDIBgbl::SelBillType==EDIBgbl::TZA)
    FrmTxsr.m_pViewTxsr->EditDel();
 nRowEnd=nRowStart=-1;
}

void CPhsData::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	long RowBeforeScroll,RowAfterScroll,CurRow;
	CurRow=RowBeforeScroll=GetAbsoluteRow(nRowEnd);
	static BOOL IncOrDec;
	
	if (!(FrmTxsr.m_pViewTxsr->m_ActiveRs->adoEOF))
	{
		if (nIDEvent==2000&&GetAsyncKeyState(VK_LBUTTON)==-32768)
		{
			if (nRowEnd>=nShresh)//down
			{
				m_DBGbill.Scroll(0,1);
				RowAfterScroll=GetAbsoluteRow(nRowEnd);
				if (RowBeforeScroll>=nRowBegin)
				{
					if (RowAfterScroll>nRowBegin+m_DBGbill.GetSelBookmarks().GetCount()-1)
						IncOrDec=true;
				}
				else if (RowAfterScroll>nRowBegin-m_DBGbill.GetSelBookmarks().GetCount()+1)
					IncOrDec=false;
				if (IncOrDec)
				{
					m_DBGbill.GetSelBookmarks().Add(m_DBGbill.RowBookmark(nRowEnd));
					FrmTxsr.m_pViewTxsr->m_ActiveRs->Bookmark=m_DBGbill.RowBookmark(nRowEnd);
					nRowFinish=GetAbsoluteRow(nRowEnd);
				}
				else
				{
					if (CurRow==nRowBegin)
					{
						if (m_DBGbill.GetSelBookmarks().GetCount()>1)
						{
							VARIANT vVal;
							vVal.vt=VT_I2;
							vVal.lVal=1;
							m_DBGbill.GetSelBookmarks().Remove(vVal);
							vVal.lVal=0;
							FrmTxsr.m_pViewTxsr->m_ActiveRs->Bookmark=m_DBGbill.GetSelBookmarks().GetItem(vVal);
						}
						IncOrDec=true;
					}
					else
					{
						VARIANT vVal;
						vVal.vt=VT_I2;
						vVal.lVal=m_DBGbill.GetSelBookmarks().GetCount()-1;
						m_DBGbill.GetSelBookmarks().Remove(vVal);
						vVal.lVal=m_DBGbill.GetSelBookmarks().GetCount()-1;
						FrmTxsr.m_pViewTxsr->m_ActiveRs->Bookmark=m_DBGbill.GetSelBookmarks().GetItem(vVal);
						nRowFinish=GetAbsoluteRow(nRowEnd);
					}
				}
			}
			if (nRowEnd==0)//up
			{
				m_DBGbill.Scroll(0,-1);
				RowAfterScroll=GetAbsoluteRow(nRowEnd);
				if (RowBeforeScroll<=nRowBegin)
				{
					if (RowAfterScroll<nRowBegin-m_DBGbill.GetSelBookmarks().GetCount()+1)
						IncOrDec=true;
				}
				else if (RowAfterScroll<nRowBegin+m_DBGbill.GetSelBookmarks().GetCount()-1)
					IncOrDec=false;
				if (IncOrDec)
				{
					m_DBGbill.GetSelBookmarks().Add(m_DBGbill.RowBookmark(nRowEnd));
					FrmTxsr.m_pViewTxsr->m_ActiveRs->Bookmark=m_DBGbill.RowBookmark(nRowEnd);
					nRowFinish=GetAbsoluteRow(nRowEnd);
				}
				else
				{
					if (CurRow==nRowBegin)
					{
						if (m_DBGbill.GetSelBookmarks().GetCount()>1)
						{
							VARIANT vVal;
							vVal.vt=VT_I2;
							vVal.lVal=1;
							m_DBGbill.GetSelBookmarks().Remove(vVal);
							vVal.lVal=0;
							FrmTxsr.m_pViewTxsr->m_ActiveRs->Bookmark=m_DBGbill.GetSelBookmarks().GetItem(vVal);
						}
						IncOrDec=true;
					}
					else
					{
						VARIANT vVal;
						vVal.vt=VT_I2;
						vVal.lVal=m_DBGbill.GetSelBookmarks().GetCount()-1;
						m_DBGbill.GetSelBookmarks().Remove(vVal);
						vVal.lVal=m_DBGbill.GetSelBookmarks().GetCount()-1;
						FrmTxsr.m_pViewTxsr->m_ActiveRs->Bookmark=m_DBGbill.GetSelBookmarks().GetItem(vVal);
						nRowFinish=GetAbsoluteRow(nRowEnd);
					}
				}
			}
		}
	}
	CDialog::OnTimer(nIDEvent);
}

void CPhsData::OnScrollDatagrid1(short FAR* Cancel) 
{
	// TODO: Add your control notification handler code here
	HideComboBox();
}

void CPhsData::OnKeyDownDatagrid1(short FAR* KeyCode, short Shift) 
{
	// TODO: Add your control notification handler code here
	// cheng 
	//
	if(*KeyCode==46)
	{
	if(EDIBgbl::SelBillType==EDIBgbl::TZA)
        FrmTxsr.m_pViewTxsr->EditDel();
	HideComboBox();

	}
}

void CPhsData::OnEditCopyCol()
{
   int RCount;
   RCount=FrmTxsr.m_pViewTxsr->m_ActiveRs->RecordCount;
   if(RCount<=0)
	   return;
	int StartCol,EndCol;
	StartCol=m_DBGbill.GetSelStartCol();
	EndCol=m_DBGbill.GetSelEndCol();
	FrmTxsr.m_pViewTxsr->m_ActiveRs->MoveFirst();
	GDataCopyCol(RCount,StartCol,EndCol,m_DBGbill);
	//DelCol=1;


}

void CPhsData::OnEditDelCol()   //paste
{
	int RCount;
	RCount=FrmTxsr.m_pViewTxsr->m_ActiveRs->RecordCount;
	if(RCount<=0)
		return;
	int StartCol,EndCol;
	StartCol=m_DBGbill.GetSelStartCol();
	EndCol=m_DBGbill.GetSelEndCol();
	FrmTxsr.m_pViewTxsr->m_ActiveRs->MoveFirst();
    GDataPasteCol(RCount,StartCol,EndCol,m_DBGbill);
}
////////////////////////////////////////////////
//这样做 ，速度一样慢，
/*
void CPhsData::OnEditCopyCol()
{
	int StartCol,EndCol;
	StartCol=m_DBGbill.GetSelStartCol();
	EndCol=m_DBGbill.GetSelEndCol();
    GDataCopyCol(FrmTxsr.m_pViewTxsr->m_ActiveRs,StartCol,EndCol);
	DelCol=1;
}


void CPhsData::OnEditDelCol()
{
	int StartCol,EndCol;
	StartCol=m_DBGbill.GetSelStartCol();
	EndCol=m_DBGbill.GetSelEndCol();
    GDataPasteCol( FrmTxsr.m_pViewTxsr->m_ActiveRs,StartCol,EndCol,m_DBGbill);

}
*/
/////////////////////////////////////////////////
//------------------------------------------------------------------
// DATE         :[2005/08/03]
// Parameter(s) :
// Return       :
// Remark       :将选择的记录导出到Excel表中。
//------------------------------------------------------------------
void CPhsData::OnExportToExcel()
{
	AfxGetApp()->BeginWaitCursor();
	//首先执行复制的操件，将选择的记录复制到一个临时表中。
    if(EDIBgbl::SelBillType==EDIBgbl::TZA)
	{
    	FrmTxsr.m_pViewTxsr->EditCopy();
		IsPastable=0;
	}
	
	CString	strSFileDir = basDirectory::ProjectDir;		//由于模板文件是重新写的，所以使用相同的路径.
	CString strDFileDir = basDirectory::ProjectDir;		//工作路径

	CString	strTempFileName = basDirectory::TemplateDir+_T("phsData.xls");
	CString	strXls = basDirectory::ProjectDir + _T("phsData")  + _T(".xls");
	
	if( FileExists(strXls) )
	{
		::DeleteFile( strXls );
	}
	
	if( !FileExists(strXls) )
	{
		if( FileExists(basDirectory::TemplateDir+_T("phsData.xls")) )
		{
			CopyFile(basDirectory::TemplateDir+_T("phsData.xls"),strXls,TRUE);
		}
		else
		{
			CString strInfo;
			strInfo.Format(IDS_NOTEXIST_EXCELFILE,basDirectory::TemplateDir,_T("phsData.xls"));
			AfxMessageBox(strInfo);
			AfxGetApp()->EndWaitCursor();
			return;
		}
	}
	try
	{
		_RecordsetPtr pRsClip;
		pRsClip.CreateInstance(__uuidof(Recordset));
		pRsClip->Open(_variant_t("SELECT * FROM [Clip]"), conPRJDB.GetInterfacePtr(),
			adOpenStatic, adLockOptimistic, adCmdText);
		if (pRsClip->GetRecordCount() <= 0)
		{
			AfxMessageBox("请选择记录！");
			return;
		}
		
		//首先EXCEL模板文件.
		if( !DLCreateTemplate(strXls,6,basDirectory::DBShareDir+"TableFormat.mdb") )
		{	
			CString strInfo;
			strInfo.LoadString(IDS_NOTCREATE_EXCELTEMPLATE);
			AfxMessageBox(strInfo);
			return;
		}
		//将打开的表写入到EXCEL中
		DLFillExcelContent(strSFileDir.GetBuffer(strSFileDir.GetLength()+1),strDFileDir.GetBuffer(strDFileDir.GetLength()+1),6,pRsClip,basDirectory::DBShareDir+"TableFormat.mdb");
		
	}
	catch (_com_error& e) 
	{
		AfxMessageBox(e.Description());
		AfxGetApp()->EndWaitCursor();
		return;
	}
	
	AfxGetApp()->EndWaitCursor();
}