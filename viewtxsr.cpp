// FrmTxsr.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "ViewTxsr.h"
#include "EDIBgbl.h"
#include "PHSData.h"
#include "modPHScal.h"
#include "basDirectory.h"
#include "user.h"
#include "EDIBDB.h"
#include "_Recordset.h"
#include "Fields.h"
#include "Field.h"
#include "cphs.h"
#include "modRegistry.h"
#include "EDIBAcad.h"
#include "READPSARESULT.h"
#include "MainFrm.h"
#include "FrmDataEDIT.h"
#include <math.h>
#include "SelZdjhdlg.h"
#include "RstEvent.h"
#include "ReadPSAResult.h"
#include "SelBookmarks.h"
#include "FrmTxsr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CTxsrPag1  *pTestWnd;
/////////////////////////////////////////////////////////////////////////////
// CViewTxsr dialog
long GetFileNameId(long FileNameID,IDispatch *pConID);
extern void Delete1();
IMPLEMENT_DYNCREATE(CViewTxsr, CFormView)
CViewTxsr::CViewTxsr()
	: CFormView(CViewTxsr::IDD)
{
	m_OptPag4=NULL;
	m_OptPag5=NULL;
	m_OptPag6=NULL;
	m_PagOption=NULL;
	m_PagItem=NULL;
	m_bIsAddNew=false;
	m_bUpd=true;
	m_bIsInit=false;
	m_bIsInitHint=false;
	m_bIsUpd=false;
	m_bChangeZDJH=false;
	m_bIsActiveing=false;
	m_bAllowUpd=true;
	m_FrictionFree=0.3;
	//{{AFX_DATA_INIT(CViewTxsr)
	m_txtXZU1 = _T("");
	m_txtXZU2 = _T("");
	m_txtXZU3 = _T("");
	m_txtXZU4 = _T("");
	m_txtXZU5 = _T("");
	m_txtXZU6 = _T("");
	m_txtXZU7 = _T("");
	m_txtXZU8 = _T("");
	m_txtZZU1 = _T("");
	m_txtZZU2 = _T("");
	m_txtZZU3 = _T("");
	m_txtZZU4 = _T("");
	m_txtZZU5 = _T("");
	m_txtZZU6 = _T("");
	m_txtZZU7 = _T("");
	m_txtZZU8 = _T("");
	m_txtType = _T("");
	m_fDZA1 = 0.0f;
	m_fGH1 = 0.0f;
	m_fDH1 = 0.0f;
	m_fDXA1 = 0.0f;
	m_fLC1 = 0.0f;
	m_fB01 = 0.0f;
	m_fA01 = 0.0f;
	m_fCoefOfVarLoad = 0.0f;
	m_lSelSampleID = 0;
	m_fSATotalLength = 0.0f;
	m_csPhsStructure = _T("");
	m_csParallelingNum = _T("");
	m_csT01 = _T("");
	m_csGDW1 = _T("");
	m_csDJ1 = _T("");
	m_csDwB = _T("");
	m_csJSDBH1 = _T("");
	m_csZDJH = _T("");
	m_lngZDJH = 0;
	m_strBornSA = _T("");
	//}}AFX_DATA_INIT
	m_InitWnd=false;

	InitialUndo();
}

void CViewTxsr::InitialUndo(void)
{
	_RecordsetPtr pRsUndo;
	
	pRsUndo.CreateInstance (__uuidof(Recordset));
	pRsUndo->CursorLocation = adUseServer;
	
	try
	{
		pRsUndo->Open("SELECT * FROM Undo", conPRJDB.GetInterfacePtr(),
			adOpenDynamic, adLockOptimistic, adCmdUnknown);
	}
	catch(...)
	{		
		try
		{
			conPRJDB->Execute("SELECT ZA.* INTO Undo FROM ZA", NULL, adExecuteNoRecords);
			conPRJDB->Execute("ALTER TABLE Undo ADD Times INT NULL", NULL, adExecuteNoRecords);
		}
		catch(...)
		{
			m_UndoCount = -1;
			return;
		}

        try
		{
			pRsUndo->Open("SELECT * FROM [Undo]", _variant_t((IDispatch*)conPRJDB,true),
				adOpenDynamic, adLockOptimistic, adCmdText);
		}
		catch(...)
		{
			m_UndoCount = -1;
			return;
		}
	}
		
	try
	{		
		pRsUndo->MoveFirst();		
		while (!pRsUndo->adoEOF && !pRsUndo->BOF)
		{
			pRsUndo->Delete (adAffectCurrent);
			pRsUndo->MoveNext ();
		}
	}
	catch(...)
	{
		m_UndoCount = -1;
		pRsUndo->Close();
		return;
	}

	pRsUndo->Close();
}

void CViewTxsr::DoDataExchange(CDataExchange* pDX)
{
	m_bIsUpd=true;
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewTxsr)
	DDX_Control(pDX, IDC_COMBO_BORNSA, m_ComBoBornSA);
	DDX_Control(pDX, IDC_TAB_TXSR, m_tabTxsr);
	DDX_Control(pDX, IDC_EDT_ZDJH, m_edtZdjh);
	DDX_Control(pDX, IDC_BORAND, m_wndBorand);
	DDX_Control(pDX, IDC_MSG_BOX, m_LabelMsgbox);
	DDX_Control(pDX, IDC_IMG_VIEWG0, m_ImageViewG0);
	DDX_Control(pDX, IDC_IMG_VIEWG1, m_ImageViewG1);
	DDX_Control(pDX, IDC_IMG_IMG1, m_Image1);
	DDX_Control(pDX, IDC_IMG_VIEWD0, m_ImageViewD0);
	DDX_Control(pDX, IDC_IMG_VIEWD1, m_ImageViewD1);
	DDX_Control(pDX, IDC_IMG_XY1, m_ImageXY1);
	DDX_Control(pDX, IDC_IMG_XY0, m_ImageXY0);
	DDX_Control(pDX, IDC_COMBO3, m_comboDW);
	DDX_Control(pDX, IDC_COMBO8, m_comboJSJDH);
	DDX_Control(pDX, IDC_COMBO6, m_comboZDJH1);
	DDX_Control(pDX, IDC_BUTTON5, m_cmdSelPhs);
	DDX_Control(pDX, IDC_COMBO4, m_comboDwB);
	DDX_Control(pDX, IDC_COMBO1, m_comboGDW1);
	DDX_Control(pDX, IDC_COMBO5, m_comboT01);
	DDX_Control(pDX, IDC_LIST1, m_LstPhsStructure);
	DDX_Control(pDX, IDC_ADODC1, m_Databill);
	DDX_Text(pDX, IDC_EDIT18, m_txtXZU1);
	DDX_Text(pDX, IDC_EDIT29, m_txtXZU2);
	DDX_Text(pDX, IDC_EDIT26, m_txtXZU3);
	DDX_Text(pDX, IDC_EDIT30, m_txtXZU4);
	DDX_Text(pDX, IDC_EDIT27, m_txtXZU5);
	DDX_Text(pDX, IDC_EDIT31, m_txtXZU6);
	DDX_Text(pDX, IDC_EDIT28, m_txtXZU7);
	DDX_Text(pDX, IDC_EDIT32, m_txtXZU8);
	DDX_Text(pDX, IDC_EDIT17, m_txtZZU1);
	DDX_Text(pDX, IDC_EDIT22, m_txtZZU2);
	DDX_Text(pDX, IDC_EDIT19, m_txtZZU3);
	DDX_Text(pDX, IDC_EDIT23, m_txtZZU4);
	DDX_Text(pDX, IDC_EDIT20, m_txtZZU5);
	DDX_Text(pDX, IDC_EDIT24, m_txtZZU6);
	DDX_Text(pDX, IDC_EDIT21, m_txtZZU7);
	DDX_Text(pDX, IDC_EDIT25, m_txtZZU8);
	DDX_Text(pDX, IDC_EDT_TYPE, m_txtType);
	DDX_Text(pDX, IDC_EDIT15, m_fDZA1);
	DDX_Text(pDX, IDC_EDT_GH1, m_fGH1);
	DDX_Text(pDX, IDC_EDIT14, m_fDH1);
	DDX_Text(pDX, IDC_EDIT16, m_fDXA1);
	DDX_Text(pDX, IDC_EDIT7, m_fLC1);
	DDX_Text(pDX, IDC_EDIT8, m_fB01);
	DDX_Text(pDX, IDC_EDT_A01, m_fA01);
	DDX_Text(pDX, IDC_EDIT3, m_fCoefOfVarLoad);
	DDX_Text(pDX, IDC_EDT_SSID, m_lSelSampleID);
	DDX_Text(pDX, IDC_EDT_SATL, m_fSATotalLength);
	DDX_LBString(pDX, IDC_LIST1, m_csPhsStructure);
	DDX_CBStringExact(pDX, IDC_COMBO5, m_csT01);
	DDX_CBStringExact(pDX, IDC_COMBO1, m_csGDW1);
	DDX_CBStringExact(pDX, IDC_COMBO3, m_csDJ1);
	DDX_CBStringExact(pDX, IDC_COMBO4, m_csDwB);
	DDX_Text(pDX, IDC_EDT_ZDJH, m_lngZDJH);
	DDX_CBString(pDX, IDC_COMBO_BORNSA, m_strBornSA);
	//}}AFX_DATA_MAP
	if(!pDX->m_bSaveAndValidate)
	{
		if(m_bIsInitHint)
		{
			CString sHint;
			if(atoi(m_txtType) > 1000 && atoi(m_txtType) < 5000)
			{
				sHint.Format(GetResStr(IDS_xNumberSpringXserialXparallel),Mid(m_txtType, 2, 2),Mid(m_txtType, 0, 1),Mid(m_txtType, 1, 1));
			}
			else if(atoi(m_txtType) < 1000)
			{
				sHint=GetResStr(IDS_RIGIDITYPHS);
			}
			else 
				sHint=GetResStr(IDS_LastingPowerSpringPHS);

			m_ctrlHint.UpdateTipText(sHint,GetDlgItem(IDC_EDT_TYPE));

		}
	}
	m_bIsUpd=false;
}


BEGIN_MESSAGE_MAP(CViewTxsr, CFormView)
	//{{AFX_MSG_MAP(CViewTxsr)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON5, OnButton5)
	ON_BN_CLICKED(IDC_IMG_VIEWG1, OnImgViewg1)
	ON_BN_CLICKED(IDC_IMG_VIEWD1, OnImgViewd1)
	ON_BN_CLICKED(IDC_IMG_IMG1, OnImgImg1)
	ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeList1)
	ON_CBN_SELCHANGE(IDC_COMBO6, OnSelchangeZdjh)
	ON_BN_CLICKED(IDC_IMG_XY0, OnImgXy0)
	ON_BN_CLICKED(IDC_BTN_ADD, OnBtnAdd)
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	ON_CBN_SELCHANGE(IDC_COMBO8, OnSelchangeJSJDH)
	ON_COMMAND(IDC_TXSR_MNUGPPD, OnGetPrevoiusPhsData)
	ON_COMMAND(IDC_TXSR_MNUPCS1, OnSelectionAll)
	ON_COMMAND(IDC_TXSR_MNUPCS2, OnCalSelectionNotSuccess)
	ON_COMMAND(IDC_TXSR_MNUPCS3, OnSelectionSuccess)
	ON_COMMAND(IDC_TXSR_MNUPCS4, OnSelectionSPR)
	ON_COMMAND(IDC_TXSR_MNUPCS5, OnSelectionCSPR)
	ON_COMMAND(IDC_TXSR_MNUPCS6, OnSelectionNotSPRAndCSPR)
	ON_EN_CHANGE(IDC_EDT_TYPE, OnChangeEdtType)
	ON_EN_UPDATE(IDC_EDT_TYPE, OnUpdateEdtType)
	ON_EN_SETFOCUS(IDC_EDT_TYPE, OnSetfocusEdtType)
	ON_COMMAND(ID_GET_ZDJHDATA, OnGetZdjhdata)
	ON_COMMAND(IDC_TXSR_MNUPCS7, OnSelectionSPRandNotCal)
	ON_COMMAND(IDC_TXSR_MNUPCS8, OnSelectionSPRandCal)
	ON_COMMAND(IDC_TXSR_MNUPCS9, OnSelectionCSPRandNotCal)
	ON_COMMAND(IDC_TXSR_MNUPCS10, OnSelectionCSPRandCal)
	ON_WM_SETCURSOR()
	ON_WM_CLOSE()
//	ON_EN_CHANGE(IDC_EDIT_BORNSA, OnChangeEditBornSA)//PFG2007.6.15
	ON_BN_CLICKED(IDC_BUTTON_SAVEJSJDH, OnButtonSaveJSJDH)
	ON_EN_KILLFOCUS(IDC_EDT_SSID, OnKillfocusEdtSsid)
	ON_BN_CLICKED(IDC_IMG_VIEWG0, OnImgViewg1)
	ON_BN_CLICKED(IDC_IMG_XY1, OnImgXy0)
	ON_CBN_EDITCHANGE(IDC_COMBO6, OnSelchangeZdjh)
	ON_BN_CLICKED(IDC_IMG_VIEWD0, OnImgViewd1)
	ON_EN_KILLFOCUS(IDC_EDT_TYPE, OnKillfocusEdtType)
	//}}AFX_MSG_MAP
	ON_EN_CHANGE(IDC_TXSR_PAG1_EDIT6, OnPage1EditChange)
	ON_EN_CHANGE(IDC_EDIT3, OnPage2EditChange)
	ON_EN_CHANGE(IDC_EDIT13, OnPage2EditChange)
	ON_EN_CHANGE(IDC_EDIT4, OnPage2EditChange)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewTxsr message handlers

void CViewTxsr::OnInitialUpdate() 
{
	InitFromReg();

	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	this->ChangeImg(m_wndBorand,IDB_PHSDATA);
	HCURSOR hcr=AfxGetApp()->LoadCursor(IDC_HPYELINK);
	m_ImageViewG0.m_cr=hcr;
	m_ImageViewG1.m_cr=hcr;
	m_Image1.m_cr=hcr;
	m_ImageViewD0.m_cr=hcr;
	m_ImageViewD1.m_cr=hcr;
	m_ImageXY1.m_cr=hcr;
	m_ImageXY0.m_cr=hcr;

	m_popMenu.LoadMenu(IDR_TXSR_MENU);

	m_PagOption=new CPagTxsrOption();
	m_OptPag4=new CTxsrPag1();
	m_OptPag5=new CTxsrPag2();
	m_OptPag6=new CTxsrPag3();
	m_PagItem=new CTxsrPagItem();

	m_PagOption->Create(IDD_TXSR_PAG_OPTION,&m_tabTxsr);
	m_OptPag4->Create(IDD_TXSR_PAG1,&m_tabTxsr);
	m_OptPag5->Create(IDD_TXSR_PAG2,&m_tabTxsr);
	m_OptPag6->Create(IDD_TXSR_PAG3,&m_tabTxsr);
	m_PagItem->Create(IDD_TXSR_PAG_ITEM,&m_tabTxsr);

	pTestWnd = m_OptPag4;

	m_tabTxsr.AddWnd(m_PagOption);
	m_tabTxsr.AddWnd(m_OptPag4);
	m_tabTxsr.AddWnd(m_OptPag5);
	m_tabTxsr.AddWnd(m_OptPag6);
	m_tabTxsr.AddWnd(m_PagItem);
	
	m_Image1.SetWindowPos(&wndTop,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	m_ImageViewG1.SetWindowPos(&wndTop,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	m_ImageViewG0.SetWindowPos(&wndTop,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	m_ImageViewD1.SetWindowPos(&wndTop,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	m_ImageViewD0.SetWindowPos(&wndTop,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	m_wndBorand.SetWindowPos(&wndTop,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	m_ctrlHint.Create(this);
	this->m_ctrlHint.AddTool(&m_LstPhsStructure,IDS_ClickhereViewBOM);
	m_ctrlHint.AddTool(GetDlgItem(IDC_EDT_TYPE),GetResStr(IDS_xNumberSpringXserialXparallel));
	m_ctrlHint.AddTool(GetDlgItem(IDC_EDIT7),IDS_PleaseInputSAdims);
	m_ctrlHint.AddTool(GetDlgItem(IDC_EDIT8),IDS_PleaseInputSAdims);
	m_ctrlHint.AddTool(GetDlgItem(IDC_EDT_A01),IDS_PleaseInputSAdims);
	m_ctrlHint.AddTool(&m_comboGDW1,IDS_PleaseInputSAdims);
	m_ctrlHint.AddTool(GetDlgItem(IDC_EDT_GH1),IDS_PleaseInputSAEL);
	m_ctrlHint.AddTool(GetDlgItem(IDC_EDIT14),IDS_PleaseInputPAEL);
	m_ctrlHint.SetDelayTime(TTDT_INITIAL,300);
	m_ctrlHint.SetDelayTime(TTDT_AUTOPOP,5000);
	m_bIsInitHint=true;
	try
	{
	//m_Databill.SetRefRecordset(m_ActiveRs);
	//this->m_pRStEvent=new CRstEvent;
	m_ActiveRs.CreateInstance(__uuidof(Recordset));
	EDIBgbl::InitDBTBN( EDIBgbl::SQLx);
	InitRs();

	
	if( EDIBAcad::g_bCustomPicNo )
	{
		EDIBAcad::g_strCustomPicNoFileName = vtos(m_ActiveRs->GetCollect("CustomDwgName"));
		if( EDIBAcad::g_strCustomPicNoFileName.IsEmpty() )
		{
			m_ActiveRs->PutCollect("CustomDwgName",(long)modPHScal::zdjh);
			EDIBAcad::g_strCustomPicNoFileName = ltos(modPHScal::zdjh);
		}
	}



   //设置坐标轴方向
   //先假定y轴向上
	HBITMAP bmp0;
	HBITMAP bmp1;
	DeleteObject(m_ImageXY0.GetBitmap());
	DeleteObject(m_ImageXY1.GetBitmap());
   if (giUPxyz == 2)
   {
      //y方向垂直向上
	   bmp0=LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_UPYRX));
	   bmp1=LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_UPYRZ));
       this->m_ImageXY0.SetBitmap((HBITMAP)bmp0);
       this->m_ImageXY1.SetBitmap((HBITMAP)bmp1);
   }
   else if(giUPxyz==3)
   {
      //z方向垂直向上
	   bmp0=LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_UPZRX));
	   bmp1=LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_UPZRY));
       this->m_ImageXY0.SetBitmap((HBITMAP)bmp0);
       this->m_ImageXY1.SetBitmap((HBITMAP)bmp1);
   }
   else if(giUPxyz==1)
   {
      //x方向垂直向上
	   bmp0=LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_UPXRY));
	   bmp1=LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_UPXRZ));
       this->m_ImageXY0.SetBitmap((HBITMAP)bmp0);
	   this->m_ImageXY1.SetBitmap((HBITMAP)bmp1);
   }
   //::CStatic::GetBitmap();

   //bmp0.DeleteObject();bmp1.DeleteObject();

   //下段程序设置几个数据表操作按钮
  /*CmdCancelAdd.Caption = BUTTON1
  cmdUpdate.Caption = BUTTON2
  CmdDelete.Caption = BUTTON3
  //cmdFind.Caption = BUTTON4
  cmdRefresh.Caption = BUTTON5
  //cmdClose.Caption = BUTTON6
  cmdAdd.Caption = BUTTON7*/
  
  //data1.recordset 设置为一个全局类Crs的数据集属性
  //必须在显示（Show）这个窗体之前设置好
   //RefreshBoundData();
   //DatabillReposition();
   //SetDutyValue();   //设置冷热工况标签值
   //LoadXZUandZZU();  //装入柱子编号
   
   //OldIndex = -1
   //TraceMainFrm Me.hwnd
   
   //设置数据源
   /*If Crs.bFormDataRecordsetSet Then
      Set Data1.Recordset = Crs.rsDS
   Else
      Set Data1.Recordset = Crs.rsSource
   End If*/
   //必须且只需上面1句,即可使两个窗体中的数据完全同步,
   //且能反映任何修改.
   //mlNumRows = Data1.Recordset.RecordCount
   //TxtPAZ0.Locked = True
   
   //LstphsStructure.Top = data1.Top + LstphsStructure.Height
   //LstPhsStructure.Visible = True
   //SetActivePage(3);
   SetOptionPZ();
   //Timer1.Interval = 1000
   //m_cmdSelPhs.EnableWindow(false);
      
   //LoadDiameterItem2ComboDW();//装入管径数据到组合框
   //LoadPAMaterialItem2ComboDCL1();//装入管部材料到组合框
   //LoadGDWItem2ComboGDW1 //放到data1_reposition事件中

   
   //LoadStrFLDItem2MyComboBox( m_comboZDJH1, "zdjh");//装入支吊架号
   //LoadStrFLDItem2MyComboBox( m_comboJSJDH, "jsdbh1");//装入计算节点号
   
   //Data1_Reposition
   //下面的查找动作可以使控件获得数值。
	//CString sText;
   //m_comboZDJH1.GetWindowText(sText);
   //sText.TrimLeft();sText.TrimRight();
   //CString sTmp="ZDJH="+sText;
   //m_ActiveRs->Find(_bstr_t(sTmp),(int)0,(SearchDirectionEnum)-1,(int)-1);
      
   //获取单位制
   //没有必要，因为单位制保存在原始数据表TZA的每一个记录中（字段Unit_Force和Unit_Momental）
   //Unit_Momental = UEGetSetting("EDInBox", "AutoPHS", "Unit_Momental", "N.m")
   //Unit_Force = UEGetSetting("EDInBox", "AutoPHS", "Unit_Force", "N")
   //设置单位选项
   //if(m_ActiveRs->GetCollect(_variant_t("unit_Force"))==_variant_t("kgf"))
	//   CheckRadioButton(IDC_UNIT_KGF,IDC_UNIT_N,IDC_UNIT_KGF);
   //else
	//   CheckRadioButton(IDC_UNIT_KGF,IDC_UNIT_N,IDC_UNIT_N);
   m_iCSnum = 0;
//   m_strBornSA = vtos(m_ActiveRs->GetCollect("BornSA"));
   UpdateData(FALSE);
   //DisPlaySANum();   //确定根部槽钢数:0/1=单/双
	//OnImgViewg1();
   //pbFrmLoadedTxsr = True
   //SetWindowTop Me.hwnd
   //SetCtrlSizes Me, 0.9 缩小窗口效果不明显,反而造成很多移位
   //Screen.MousePointer = vbDefault
	m_Databill.SetEnabled(false);
	m_InitWnd=true;
	m_bIsInit=true;
	return;
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}

	catch(...)
	{
		return ;
	}

return ;
}

//DEL BOOL CViewTxsr::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
//DEL {
//DEL 	// TODO: Add your specialized code here and/or call the base class
//DEL 	
//DEL 	return CFormView::Create(IDD, pParentWnd);
//DEL }

HBRUSH CViewTxsr::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	
	// TODO: Return a different brush if the default is not desired
	/*static CBrush br;
	if(pWnd!=NULL && (pWnd->m_hWnd==GetDlgItem(IDC_EDT_SSID)->m_hWnd || pWnd->m_hWnd==GetDlgItem(IDC_EDT_SATL)->m_hWnd))
		return CFormView::OnCtlColor(pDC,pWnd,nCtlColor);
	bool f=false;
	LOGBRUSH logbr;
	logbr.lbStyle=BS_SOLID;
	switch(nCtlColor)
	{
	case CTLCOLOR_DLG:
	//case CTLCOLOR_BTN:
		//logbr.lbColor=0xdddddd;
		//f=true;
		return CFormView::OnCtlColor(pDC, pWnd, nCtlColor);
		break;
	case CTLCOLOR_EDIT:
	case CTLCOLOR_LISTBOX:
		logbr.lbColor=0x00FF80FF;
		pDC->SetBkColor(0x00ff80ff);
		pDC->SetTextColor(0x000000);
		f=true;
		break;
	case CTLCOLOR_STATIC:
		logbr.lbStyle=BS_NULL;
		pDC->SetBkMode(TRANSPARENT);
		f=true;
		break;
	/*default:
		return CFormView::OnCtlColor(pDC, pWnd, nCtlColor);
		break;*/
	/*}
	if(nCtlColor==CTLCOLOR_LISTBOX)
	{
		if(m_comboZDJH1.GetDroppedState() || m_comboJSJDH.GetDroppedState())
		{
				logbr.lbStyle=BS_SOLID;
				logbr.lbColor=0x00FFC0C0;
				if(pDC!=NULL)
				{
					pDC->SetTextColor(0x000000FF);
					pDC->SetBkColor(0x00FFC0C0);
				}
		}
	}
	/*if(nCtlColor==CTLCOLOR_STATIC)
	{
				logbr.lbStyle=BS_NULL;
				pDC->SetBkMode(TRANSPARENT);
	}*/
	/*if(IsWindow(pWnd->m_hWnd))
	{
		CWnd* pw=pWnd->GetParent();
		if(pw!=NULL && IsWindow(pw->m_hWnd))
		{
			if(pw->m_hWnd==m_comboZDJH1.m_hWnd
				|| pw->m_hWnd==m_comboJSJDH.m_hWnd )
			{
				logbr.lbStyle=BS_SOLID;
				logbr.lbColor=0x00FFC0C0;
				pDC->SetTextColor(0x000000FF);
				pDC->SetBkColor(0x00FFC0C0);
			}
		}
		if(pWnd->m_hWnd==m_comboZDJH1.m_hWnd
			|| pWnd->m_hWnd==m_comboJSJDH.m_hWnd || pWnd->m_hWnd==m_edtZdjh.m_hWnd)
		{
			logbr.lbStyle=BS_SOLID;
			logbr.lbColor=0x00FFC0C0;
			pDC->SetTextColor(0x000000FF);
			pDC->SetBkColor(0x00FFC0C0);
		}
		else if(pWnd->m_hWnd==m_LstPhsStructure.m_hWnd)
		{
			logbr.lbStyle=BS_SOLID;
			logbr.lbColor=0x0080FF80;
			pDC->SetBkColor(0x0080FF80);
			pDC->SetBkMode(OPAQUE);
		}
/*		else if(pWnd->m_hWnd==m_TextSPsprNum.m_hWnd)
		{
			logbr.lbStyle=BS_SOLID;
			logbr.lbColor=0x8000000D;
			pDC->SetTextColor(0xffffff);
			pDC->SetBkColor(0x8000000D);
		}*/
/*		else if(f==false)
			return CFormView::OnCtlColor(pDC, pWnd, nCtlColor);
	}
	else if(f==false)
		return CFormView::OnCtlColor(pDC, pWnd, nCtlColor);
	//return ::CreateBrushIndirect(&logbr);
	br.DeleteObject();
	br.CreateBrushIndirect(&logbr);
	return (HBRUSH)br;*/
	//return (HBRUSH)br.GetSafeHandle();
	return hbr;
}

void CViewTxsr::SetActivePage(int ix)
{

/*	static int ActivePage=0;
	static int ActivePageHeight=4;
	if(ActivePage==ix || ix>6 || ix<1)
		return;
	CWnd* pWnd;
	if(ActivePage>0 && ActivePage<4)
	{
		switch(ActivePage)
		{
		case 1:
			GetDlgItem(IDC_PAG_UNIT)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_UNIT_KGF)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_UNIT_N)->ShowWindow(SW_HIDE);
			break;
		case 2:
			GetDlgItem(IDC_PAG_PZ)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_PZ_D)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_PZ_G)->ShowWindow(SW_HIDE);
			break;
		case 3:
			GetDlgItem(IDC_LIST1)->ShowWindow(SW_HIDE);
			break;
		case 4:
			m_OptPag4->ShowWindow(SW_HIDE);
			break;
		case 5:
			m_OptPag5->ShowWindow(SW_HIDE);
			break;
		case 6:
			m_OptPag6->ShowWindow(SW_HIDE);
			break;
		}
	}
	if(ix>3)
	{
		switch(ActivePageHeight)
		{
		case 4:
			m_OptPag4->ShowWindow(SW_HIDE);
			break;
		case 5:
			m_OptPag5->ShowWindow(SW_HIDE);
			break;
		case 6:
			m_OptPag6->ShowWindow(SW_HIDE);
			break;
		}
	}
	if(ix>0 && ix<7)
	{
		switch(ix)
		{
		case 1:
			GetDlgItem(IDC_PAG_UNIT)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_UNIT_KGF)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_UNIT_N)->ShowWindow(SW_SHOW);
			break;
		case 2:
			GetDlgItem(IDC_PAG_PZ)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_PZ_D)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_PZ_G)->ShowWindow(SW_SHOW);
			break;
		case 3:
			GetDlgItem(IDC_LIST1)->ShowWindow(SW_SHOW);
			break;
		case 4:
			m_OptPag4->ShowWindow(SW_SHOW);
			break;
		case 5:
			m_OptPag5->ShowWindow(SW_SHOW);
			break;
		case 6:
			m_OptPag6->ShowWindow(SW_SHOW);
			break;
		}
	}
	ActivePage=ix;
	if(ix>3)
		ActivePageHeight=ix;*/
}


BEGIN_EVENTSINK_MAP(CViewTxsr, CFormView)
    //{{AFX_EVENTSINK_MAP(CViewTxsr)
	ON_EVENT(CViewTxsr, IDC_ADODC1, 200 /* WillMove */, OnWillMoveDatabill, VTS_I4 VTS_PI4 VTS_DISPATCH)
	ON_EVENT(CViewTxsr, IDC_ADODC1, 201 /* MoveComplete */, OnMoveCompleteDatabill, VTS_I4 VTS_DISPATCH VTS_PI4 VTS_DISPATCH)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CViewTxsr::OnWillMoveDatabill(long adReason, long FAR* adStatus, LPDISPATCH pRecordset) 
{
	// TODO: Add your control notification handler code here
	try
	{
		if(m_ActiveRs->adoEOF || m_ActiveRs->BOF)
			return;
		if(m_bAllowUpd && m_bActive && m_bUpd && m_InitWnd && !m_bIsAddNew && !m_bIsActiveing)
			UpdateBoundData();
	
	//EDIBAcad::DisplayDataZB ();
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}

	catch(...)
	{
	}
}

void CViewTxsr::OnMoveCompleteDatabill(long adReason, LPDISPATCH pError, long FAR* adStatus, LPDISPATCH pRecordset) 
{
	// TODO: Add your control notification handler code here
	try
	{
		if(m_ActiveRs->adoEOF || m_ActiveRs->BOF)
			return;
		if(m_bAllowUpd && m_bActive && m_InitWnd && !m_bIsAddNew && !m_bIsActiveing)
		{
			RefreshBoundData();
			DatabillReposition();
			modPHScal::zdjh=vtoi(m_ActiveRs->GetCollect("Zdjh"));
			SetRegValue("Settings","PhsCurZdjhNo",modPHScal::zdjh);
			SetRegValue("Settings","PhsCurVlmID",EDIBgbl::SelVlmID );
			modPHScal::gsDwgFN=modPHScal::GetSaveAsDwgFileName(modPHScal::zdjh,m_ActiveRs);
		
			if(vtos(m_ActiveRs->GetCollect("DwgName")).IsEmpty())
			{
				m_ActiveRs->PutCollect("DwgName",_variant_t(modPHScal::gsDwgFN));
				SetRegValue("Settings","CurZdjhDwgFileName",modPHScal::gsDwgFN);
			}
			m_PagItem->m_LblPrjName = EDIBgbl::SelPrjName;
			m_PagItem->m_txtCLB1 = EDIBAcad::g_strCustomPicNoFileName;
			m_PagItem->UpdateData(FALSE);

		}
		//UpdateWindow();
		//Invalidate();
		this->m_Databill.Invalidate();
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}

	catch(...)
	{
	}
}



//DEL void CViewTxsr::SetDutyValue()
//DEL {
//DEL    //设置热态数据>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//DEL    /*If Unit_Displacement = "" Then Unit_Displacement = "mm"
//DEL    If Unit_Force = "" Then Unit_Force = "kgf"
//DEL    If Unit_Momental = "" Then Unit_Momental = "kgf.m"*/
//DEL    
//DEL   /* mvaHeatDuty(iDIlineDispX).sDataField = "xr1"
//DEL    mvaHeatDuty(iDIlineDispY).sDataField = "yr1"
//DEL    mvaHeatDuty(iDIlineDispZ).sDataField = "Zr1"
//DEL    mvaHeatDuty(iDIFx).sDataField = "pxr1"
//DEL    mvaHeatDuty(iDIFz).sDataField = "pzr1"
//DEL    mvaHeatDuty(iDImX).sDataField = "mxr1"
//DEL    mvaHeatDuty(iDImY).sDataField = "myr1"
//DEL    mvaHeatDuty(iDImZ).sDataField = "mzr1"
//DEL    mvaHeatDuty(iDIRotDispX).sDataField = "rxr1"
//DEL    mvaHeatDuty(iDIRotDispY).sDataField = "ryr1"
//DEL    mvaHeatDuty(iDIRotDispZ).sDataField = "rzr1"
//DEL 
//DEL    mvaHeatDuty(iDIlineDispX).sUnit = Unit_Displacement
//DEL    mvaHeatDuty(iDIlineDispY).sUnit = Unit_Displacement
//DEL    mvaHeatDuty(iDIlineDispZ).sUnit = Unit_Displacement
//DEL    mvaHeatDuty(iDIFx).sUnit = Unit_Force
//DEL    mvaHeatDuty(iDIFz).sUnit = Unit_Force
//DEL    mvaHeatDuty(iDImX).sUnit = Unit_Momental
//DEL    mvaHeatDuty(iDImY).sUnit = Unit_Momental
//DEL    mvaHeatDuty(iDImZ).sUnit = Unit_Momental
//DEL    mvaHeatDuty(iDIRotDispX).sUnit = ""
//DEL    mvaHeatDuty(iDIRotDispY).sUnit = ""
//DEL    mvaHeatDuty(iDIRotDispZ).sUnit = ""
//DEL 
//DEL    //设置冷态数据>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//DEL //    mvaCoolDuty(iDIlineDispX).sCaption = "X轴线位移"
//DEL    mvaCoolDuty(iDIlineDispX).sCaption = ResolveResString(iUE_MsgBox_60616)
//DEL //    mvaCoolDuty(iDIlineDispY).sCaption = "Y轴线位移"
//DEL    mvaCoolDuty(iDIlineDispY).sCaption = ResolveResString(iUE_MsgBox_60617)
//DEL //    mvaCoolDuty(iDIlineDispZ).sCaption = "Z轴线位移"
//DEL    mvaCoolDuty(iDIlineDispZ).sCaption = ResolveResString(iUE_MsgBox_60618)
//DEL //    mvaCoolDuty(iDIFx).sCaption = "X轴推力"
//DEL    mvaCoolDuty(iDIFx).sCaption = ResolveResString(iUE_MsgBox_60619)
//DEL //    mvaCoolDuty(iDIFz).sCaption = "Z轴推力"
//DEL    mvaCoolDuty(iDIFz).sCaption = ResolveResString(iUE_MsgBox_60620)
//DEL //    mvaCoolDuty(iDImX).sCaption = "X轴力矩"
//DEL    mvaCoolDuty(iDImX).sCaption = ResolveResString(iUE_MsgBox_60621)
//DEL //    mvaCoolDuty(iDImY).sCaption = "Y轴力矩"
//DEL    mvaCoolDuty(iDImY).sCaption = ResolveResString(iUE_MsgBox_60622)
//DEL //    mvaCoolDuty(iDImZ).sCaption = "Z轴力矩"
//DEL    mvaCoolDuty(iDImZ).sCaption = ResolveResString(iUE_MsgBox_60623)
//DEL //    mvaCoolDuty(iDIRotDispX).sCaption = "X轴角位移"
//DEL    mvaCoolDuty(iDIRotDispX).sCaption = ResolveResString(iUE_MsgBox_60624)
//DEL //    mvaCoolDuty(iDIRotDispY).sCaption = "Y轴角位移"
//DEL    mvaCoolDuty(iDIRotDispY).sCaption = ResolveResString(iUE_MsgBox_60625)
//DEL //    mvaCoolDuty(iDIRotDispZ).sCaption = "Z轴角位移"
//DEL    mvaCoolDuty(iDIRotDispZ).sCaption = ResolveResString(iUE_MsgBox_60626)
//DEL 
//DEL    mvaCoolDuty(iDIlineDispX).sDataField = "xl1"
//DEL    mvaCoolDuty(iDIlineDispY).sDataField = "yl1"
//DEL    mvaCoolDuty(iDIlineDispZ).sDataField = "Zl1"
//DEL    mvaCoolDuty(iDIFx).sDataField = "pxl1"
//DEL    mvaCoolDuty(iDIFz).sDataField = "pzl1"
//DEL    mvaCoolDuty(iDImX).sDataField = "mxl1"
//DEL    mvaCoolDuty(iDImY).sDataField = "myl1"
//DEL    mvaCoolDuty(iDImZ).sDataField = "mzl1"
//DEL    mvaCoolDuty(iDIRotDispX).sDataField = "rxl1"
//DEL    mvaCoolDuty(iDIRotDispY).sDataField = "ryl1"
//DEL    mvaCoolDuty(iDIRotDispZ).sDataField = "rzl1"
//DEL 
//DEL    mvaCoolDuty(iDIlineDispX).sUnit = Unit_Displacement
//DEL    mvaCoolDuty(iDIlineDispY).sUnit = Unit_Displacement
//DEL    mvaCoolDuty(iDIlineDispZ).sUnit = Unit_Displacement
//DEL    mvaCoolDuty(iDIFx).sUnit = Unit_Force
//DEL    mvaCoolDuty(iDIFz).sUnit = Unit_Force
//DEL    mvaCoolDuty(iDImX).sUnit = Unit_Momental
//DEL    mvaCoolDuty(iDImY).sUnit = Unit_Momental
//DEL    mvaCoolDuty(iDImZ).sUnit = Unit_Momental
//DEL    mvaCoolDuty(iDIRotDispX).sUnit = ""
//DEL    mvaCoolDuty(iDIRotDispY).sUnit = ""
//DEL    mvaCoolDuty(iDIRotDispZ).sUnit = ""*/
//DEL }

//DEL void CViewTxsr::LoadXZUandZZU()
//DEL {
//DEL 
//DEL    For i = 1 To 8
//DEL       If i > 1 Then
//DEL          Load txtXZU(i)
//DEL          Load txtZZU(i)
//DEL          
//DEL          txtXZU(i).TabIndex = txtXZU(1).TabIndex + i
//DEL          txtXZU(i).Width = txtXZU(1).Width
//DEL          txtXZU(i).Height = txtXZU(1).Height
//DEL          txtXZU(i).Left = txtXZU(1).Left + (txtXZU(1).Width + 10) * ((i - 1) Mod 2)
//DEL          txtXZU(i).Top = txtXZU(1).Top + (txtXZU(1).Height + 3) * Int((i - 1) / 2)
//DEL          txtXZU(i).DataField = "XZU" & i
//DEL          txtXZU(i).Visible = True
//DEL          
//DEL          txtZZU(i).TabIndex = txtZZU(1).TabIndex + i
//DEL          txtZZU(i).Width = txtZZU(1).Width
//DEL          txtZZU(i).Height = txtZZU(1).Height
//DEL          txtZZU(i).Left = txtZZU(1).Left + (txtZZU(1).Width + 10) * ((i - 1) Mod 2)
//DEL          txtZZU(i).Top = txtZZU(1).Top + (txtZZU(1).Height + 3) * Int((i - 1) / 2)
//DEL          txtZZU(i).DataField = "ZZU" & i
//DEL          txtZZU(i).Visible = True
//DEL       End If
//DEL    Next
//DEL    Data1.UpdateControls
//DEL }

void CViewTxsr::SetOptionPZ()
{
/*	if(m_ActiveRs==NULL || m_ActiveRs->
	If IsNull(Data1.Recordsetm_ActiveRs->PutCollect("DG1")) Then
      Data1.Recordset.Edit: Data1.Recordsetm_ActiveRs->PutCollect("DG1") = "G": Data1.Recordset.Update
   ElseIf UCase(Trim(Data1.Recordsetm_ActiveRs->PutCollect("DG1"))) = "D" Then
      frmtxsr!OptionPZ(iPZPA) = True
   ElseIf UCase(Trim(Data1.Recordsetm_ActiveRs->PutCollect("DG1"))) = "G" Then
      frmtxsr!OptionPZ(iPZSA) = True
   End If*/
}

void CViewTxsr::OnButton5() 
{
	// TODO: Add your control notification handler code here
	FrmTxsr.m_IsFirst=false;
	((CMainFrame*)AfxGetApp()->m_pMainWnd)->OnSampleManage();
}

void CViewTxsr::LoadDiameterItem2ComboDW()
{
//装入管径数据到组合框
	try
	{
		LoadtbnPAItem2ComBox(m_comboDW,"dw");
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}
}

void CViewTxsr::LoadPAMaterialItem2ComboDCL1()
{
//装入管部材料到组合框
	try
	{
		LoadtbnPAItem2ComBox(m_PagOption->m_comboDCL1,"Material");
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}
}

void CViewTxsr::LoadStrFLDItem2MyComboBox(CComboBox &comBox, CString strFLD)
{

   //装入管径数据到组合框
   //SQLx = "SELECT DISTINCT " & Trim(strFLD) & " FROM [" & TBNSelPrjSpec & Btype(TZA) & "] ORDER BY " & Trim(strFLD) & " ASC"
	try{
		CString MyComboBoxCurrent,sTmp;
		comBox.GetWindowText(MyComboBoxCurrent);
		comBox.ResetContent();
		_RecordsetPtr bkRs=m_ActiveRs->Clone(adLockReadOnly);
		bkRs->MoveFirst();
		bkRs->Filter=m_ActiveRs->Filter;
		_variant_t tmpvar;
		int i=0;
		while(!bkRs->adoEOF)
		{
			tmpvar=bkRs->GetCollect(_variant_t(strFLD));
			//rs.GetFieldValue(strFLD,tmpvar);
			sTmp=vtos(tmpvar);
			/*if(tmpvar.vt==VT_BSTR)
				sTmp=tmpvar.pcVal;
			else
				sTmp.Format("%.2f",vtof(tmpvar));*/
			
			comBox.AddString(sTmp);
			bkRs->MoveNext();
			i++;
		}
		//m_ActiveRs->Bookmark=bok;
		bkRs->Close();
		if(i>0)
		{
			i=comBox.FindStringExact(-1,MyComboBoxCurrent);
			if(i!=CB_ERR)
				comBox.SetCurSel(i);
	   }
		bkRs=NULL;
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}
}

//DEL void CViewTxsr::OnPagDw() 
//DEL {
//DEL 	// TODO: Add your control notification handler code here
//DEL 	
//DEL }

void CViewTxsr::DisPlaySANum()
{
	try{
		if(vtoi(m_ActiveRs->GetCollect("Gnum"))<0)
		{
			m_ActiveRs->PutCollect("Gnum",_variant_t((long)1));
			modPHScal::glNumSA = 1;
		}
		else
			modPHScal::glNumSA =vtoi(m_ActiveRs->GetCollect("Gnum"));

		
		//   modPHScal::CalSpringParallelNum();//'目的:计算弹簧并联数 返回:gintParallelNum=弹簧并联数

		if (modPHScal::gintParallelNum == 1)
			//并联数=1，根部数量必然<=1
			if(modPHScal::glNumSA >= 1 )
				modPHScal::glNumSA = 1;
			m_ActiveRs->PutCollect("Gnum",_variant_t((long)modPHScal::glNumSA));
			_variant_t tmpvar;
			tmpvar.ChangeType(VT_I4);
			tmpvar.intVal=modPHScal::glNumSA;
			//modPHScal::UpdateTZB4zdjh("Gnum", tmpvar);//目的:更新TZB表
			CString fs;
			fs.Format(GetResStr(IDS_MsgBox_60627)+"%d",modPHScal::glNumSA);
			m_PagOption->m_CmdSAnum.SetWindowText(fs);
	}
	catch(_com_error e)
	{
		//ShowMessage(e.Description());
	}
}

void CViewTxsr::RefreshBoundData()
{
	try
	{
		if(m_ActiveRs->adoEOF || m_ActiveRs->BOF)
			return;
		if(m_ActiveRs==NULL || m_ActiveRs->State!=adStateOpen)
			return;

		//设置热态数据>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

		if(!m_bAllowUpd)
			return;
		m_lngZDJH=vtoi(m_ActiveRs->GetCollect("Zdjh"));

		if(modPHScal::Unit_Displacement=="")
			modPHScal::Unit_Displacement=vtos(m_ActiveRs->GetCollect(_variant_t("Unit_Displacement")));
		if(modPHScal::Unit_Force=="")
			modPHScal::Unit_Force=vtos(m_ActiveRs->GetCollect(_variant_t("Unit_Force")));
		if(modPHScal::Unit_Momental=="")
			modPHScal::Unit_Momental=vtos(m_ActiveRs->GetCollect(_variant_t("Unit_Momental")));
		if(modPHScal::Unit_Displacement=="") modPHScal::Unit_Displacement="mm";
		if(modPHScal::Unit_Force=="") modPHScal::Unit_Force="kgf";
		if(modPHScal::Unit_Momental=="") modPHScal::Unit_Momental="kgf.m";
		ConvertUnit();
		if(vtos(m_ActiveRs->GetCollect("dg1"))=="")
		{
			m_ActiveRs->PutCollect("dg1",_variant_t("G"));
			m_ActiveRs->Update();
		}
		if(vtos(m_ActiveRs->GetCollect("dg1"))=="G")
			m_PagOption->CheckRadioButton(IDC_PZ_G,IDC_PZ_D,IDC_PZ_G);
		else
			m_PagOption->CheckRadioButton(IDC_PZ_G,IDC_PZ_D,IDC_PZ_D);

		if(vtoi(m_ActiveRs->GetCollect(_variant_t("iSelSampleID")))!=0)
			m_lSelSampleID=(long)m_ActiveRs->GetCollect(_variant_t("iSelSampleID"));
		else
		{
			m_ActiveRs->PutCollect("iSelSampleID",_variant_t((long)2));
			m_lSelSampleID=2;
		}
		

		//装入柱子编号
		if(/*m_ActiveRs->Fields->Item[_variant_t("XZU1")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("XZU1")).vt!=VT_NULL)
			m_txtXZU1=(char*)_bstr_t(m_ActiveRs->GetCollect(_variant_t("XZU1")));
		else
			m_txtXZU1="";
		if(/*m_ActiveRs->Fields->Item[_variant_t("XZU2")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("XZU2")).vt!=VT_NULL)
			m_txtXZU2=(char*)_bstr_t(m_ActiveRs->GetCollect(_variant_t("XZU2")));
		else
			m_txtXZU2="";
		if(/*m_ActiveRs->Fields->Item[_variant_t("XZU3")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("XZU3")).vt!=VT_NULL)
			m_txtXZU3=(char*)_bstr_t(m_ActiveRs->GetCollect(_variant_t("XZU3")));
		else
			m_txtXZU3="";
		if(/*m_ActiveRs->Fields->Item[_variant_t("XZU4")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("XZU4")).vt!=VT_NULL)
			m_txtXZU4=(char*)_bstr_t(m_ActiveRs->GetCollect(_variant_t("XZU4")));
		else
			m_txtXZU4="";
		if(/*m_ActiveRs->Fields->Item[_variant_t("XZU5")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("XZU5")).vt!=VT_NULL)
			m_txtXZU5=(char*)_bstr_t(m_ActiveRs->GetCollect(_variant_t("XZU5")));
		else
			m_txtXZU5="";
		if(/*m_ActiveRs->Fields->Item[_variant_t("XZU6")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("XZU6")).vt!=VT_NULL)
			m_txtXZU6=(char*)_bstr_t(m_ActiveRs->GetCollect(_variant_t("XZU6")));
		else
			m_txtXZU6="";
		if(/*m_ActiveRs->Fields->Item[_variant_t("XZU7")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("XZU7")).vt!=VT_NULL)
			m_txtXZU7=(char*)_bstr_t(m_ActiveRs->GetCollect(_variant_t("XZU7")));
		else
			m_txtXZU7="";
		if(/*m_ActiveRs->Fields->Item[_variant_t("XZU8")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("XZU8")).vt!=VT_NULL)
			m_txtXZU8=(char*)_bstr_t(m_ActiveRs->GetCollect(_variant_t("XZU8")));
		else
			m_txtXZU8="";

		if(/*m_ActiveRs->Fields->Item[_variant_t("ZZU1")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("ZZU1")).vt!=VT_NULL)
			m_txtZZU1=(char*)_bstr_t(m_ActiveRs->GetCollect(_variant_t("ZZU1")));
		else
			m_txtZZU1="";
		if(/*m_ActiveRs->Fields->Item[_variant_t("ZZU2")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("ZZU2")).vt!=VT_NULL)
			m_txtZZU2=(char*)_bstr_t(m_ActiveRs->GetCollect(_variant_t("ZZU2")));
		else
			m_txtZZU2="";
		if(/*m_ActiveRs->Fields->Item[_variant_t("ZZU3")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("ZZU3")).vt!=VT_NULL)
			m_txtZZU3=(char*)_bstr_t(m_ActiveRs->GetCollect(_variant_t("ZZU3")));
		else
			m_txtZZU3="";
		if(/*m_ActiveRs->Fields->Item[_variant_t("ZZU4")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("ZZU4")).vt!=VT_NULL)
			m_txtZZU4=(char*)_bstr_t(m_ActiveRs->GetCollect(_variant_t("ZZU4")));
		else
			m_txtZZU4="";
		if(/*m_ActiveRs->Fields->Item[_variant_t("ZZU5")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("ZZU5")).vt!=VT_NULL)
			m_txtZZU5=(char*)_bstr_t(m_ActiveRs->GetCollect(_variant_t("ZZU5")));
		else
			m_txtZZU5="";
		if(/*m_ActiveRs->Fields->Item[_variant_t("ZZU6")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("ZZU6")).vt!=VT_NULL)
			m_txtZZU6=(char*)_bstr_t(m_ActiveRs->GetCollect(_variant_t("ZZU6")));
		else
			m_txtZZU6="";
		if(/*m_ActiveRs->Fields->Item[_variant_t("ZZU7")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("ZZU7")).vt!=VT_NULL)
			m_txtZZU7=(char*)_bstr_t(m_ActiveRs->GetCollect(_variant_t("ZZU7")));
		else
			m_txtZZU7="";
		if(/*m_ActiveRs->Fields->Item[_variant_t("ZZU8")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("ZZU8")).vt!=VT_NULL)
			m_txtZZU8=(char*)_bstr_t(m_ActiveRs->GetCollect(_variant_t("ZZU8")));
		else
			m_txtZZU8="";
		
		if(/*m_ActiveRs->Fields->Item[_variant_t("DH1")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("DH1")).vt!=VT_NULL)
			m_fDH1=(float)m_ActiveRs->GetCollect(_variant_t("DH1"));
		else
		{
			m_ActiveRs->PutCollect("DH1",_variant_t((float)12));
			m_fDH1=12;
		}

		//
		if(/*m_ActiveRs->Fields->Item[_variant_t("GH1")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("GH1")).vt!=VT_NULL)
			m_fGH1=(float)m_ActiveRs->GetCollect(_variant_t("GH1"));
		else
		{
			m_fGH1=m_fDH1+3;
			m_ActiveRs->PutCollect("GH1",_variant_t((float)m_fGH1));
		}
		//支吊架类型
		if(/*m_ActiveRs->Fields->Item[_variant_t("TYPE")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("TYPE")).vt!=VT_NULL)
			m_txtType=(char*)(_bstr_t)m_ActiveRs->GetCollect(_variant_t("TYPE"));
		else
			m_txtType="";
		//不统计根部
		if(/*m_ActiveRs->Fields->Item[_variant_t("bNotSumSA")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("bNotSumSA")).vt!=VT_NULL)
			m_PagOption->m_bNotSumSA=(bool)m_ActiveRs->GetCollect(_variant_t("bNotSumSA"));
		else
			m_PagOption->m_bNotSumSA=false;
		//弹簧荷载变化系数
		if(/*m_ActiveRs->Fields->Item[_variant_t("CoefOfVarLoad")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("CoefOfVarLoad")).vt!=VT_NULL)
			m_fCoefOfVarLoad=(float)m_ActiveRs->GetCollect(_variant_t("CoefOfVarLoad"));
		else
			m_fCoefOfVarLoad=0;
		//弹簧并联数
		if(/*m_ActiveRs->Fields->Item[_variant_t("ParallelingNum")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("ParallelingNum")).vt!=VT_NULL)
			m_csParallelingNum.Format("%d",(long)m_ActiveRs->GetCollect(_variant_t("ParallelingNum")));
		else
		{
			m_ActiveRs->PutCollect("ParallelingNum",_variant_t((long)1));
			m_csParallelingNum="1";
		}
		//根部l值
		if(/*m_ActiveRs->Fields->Item[_variant_t("GDW1")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("GDW1")).vt!=VT_NULL)
			m_csGDW1.Format("%g",(float)m_ActiveRs->GetCollect(_variant_t("GDW1")));
		else
		{
			m_ActiveRs->PutCollect("GDW1",_variant_t((float)200));
			m_csGDW1="200";
		}
		//根部l1
		if(/*m_ActiveRs->Fields->Item[_variant_t("LC1")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("LC1")).vt!=VT_NULL)
			m_fLC1=(float)m_ActiveRs->GetCollect(_variant_t("LC1"));
		else
		{
			m_fLC1=1800;
			m_ActiveRs->PutCollect("LC1",_variant_t((float)m_fLC1));
		}
		//根部B值
		if(/*m_ActiveRs->Fields->Item[_variant_t("B01")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("B01")).vt!=VT_NULL)
			m_fB01=(float)m_ActiveRs->GetCollect(_variant_t("B01"));
		else
		{
			m_fB01=300;
			m_ActiveRs->PutCollect("B01",_variant_t((float)m_fB01));
		}
		//根部A(或H)值
		if(/*m_ActiveRs->Fields->Item[_variant_t("A01")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("A01")).vt!=VT_NULL)
			m_fA01=(float)m_ActiveRs->GetCollect(_variant_t("A01"));
		else
		{
			m_fA01=200;
			m_ActiveRs->PutCollect("A01",_variant_t((float)m_fA01));
		}
		//根部总长度L
		if(/*m_ActiveRs->Fields->Item[_variant_t("SATotalLength")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("SATotalLength")).vt!=VT_NULL)
			m_fSATotalLength=(float)m_ActiveRs->GetCollect(_variant_t("SATotalLength"));
		else
			m_fSATotalLength=0;

		if(/*m_ActiveRs->Fields->Item[_variant_t("DXA1")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("DXA1")).vt!=VT_NULL)
			m_fDXA1=(float)m_ActiveRs->GetCollect(_variant_t("DXA1"));
		else
		{
			m_ActiveRs->PutCollect("DXA1",_variant_t((float)1500));
			m_fDXA1=1500;
		}
		if(/*m_ActiveRs->Fields->Item[_variant_t("DZA1")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("DZA1")).vt!=VT_NULL)
			m_fDZA1=(float)m_ActiveRs->GetCollect(_variant_t("DZA1"));
		else
		{
			m_ActiveRs->PutCollect("DZA1",_variant_t((float)3200));
			m_fDZA1=3200;
		}
		//本支吊架数量
		if(/*m_ActiveRs->Fields->Item[_variant_t("Num")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("Num")).vt!=VT_NULL)
			m_PagOption->m_iNum=(long)m_ActiveRs->GetCollect(_variant_t("Num"));
		else
		{
			m_ActiveRs->PutCollect("Num",_variant_t((long)1));
			m_PagOption->m_iNum=1;
		}
		//管部材料
		if(/*m_ActiveRs->Fields->Item[_variant_t("DCL1")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("DCL1")).vt!=VT_NULL)
			m_PagOption->m_csDCL1=(char*)(_bstr_t)m_ActiveRs->GetCollect(_variant_t("DCL1"));
		else
			m_PagOption->m_csDCL1="";
		//较长垂直管道的刚性支吊架
		if(/*m_ActiveRs->Fields->Item[_variant_t("ifLongVertPipe")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("ifLongVertPipe")).vt!=VT_NULL)
			m_PagOption->m_bifLongVertPipe=(bool)m_ActiveRs->GetCollect(_variant_t("ifLongVertPipe"));
		else
			m_PagOption->m_bifLongVertPipe=false;
		//管外径
		if(/*m_ActiveRs->Fields->Item[_variant_t("DJ1")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("DJ1")).vt!=VT_NULL)
			m_csDJ1.Format("%g",(float)m_ActiveRs->GetCollect(_variant_t("DJ1")));
		else
			m_csDJ1="";
		if(/*m_ActiveRs->Fields->Item[_variant_t("DwB")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("DwB")).vt!=VT_NULL)
			m_csDwB.Format("%g",(float)m_ActiveRs->GetCollect(_variant_t("DwB")));
		else
		{
			m_ActiveRs->PutCollect("DwB",_variant_t((float)800));
			m_csDwB="800";
		}

		
		if(/*m_ActiveRs->Fields->Item[_variant_t("ZDJH")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("T01")).vt!=VT_NULL)
			m_csT01.Format("%g",(float)m_ActiveRs->GetCollect(_variant_t("T01")));
		else
		{
			m_ActiveRs->PutCollect("T01",_variant_t((float)50));
			m_csT01="50";
		}

		if(m_ActiveRs->GetCollect(_variant_t("BornSA")).vt!=VT_NULL)
			m_strBornSA=vtos(m_ActiveRs->GetCollect(_variant_t("BornSA")));
		else
		{
			m_strBornSA="";
			m_ActiveRs->PutCollect("BornSA",_variant_t(m_strBornSA));
		}//pfg2007.06.15
		if(m_ActiveRs->GetCollect(_variant_t("JSDBH1")).vt!=VT_NULL)
			m_csJSDBH1.Format("%d",(long)m_ActiveRs->GetCollect(_variant_t("JSDBH1")));
		else
			m_csJSDBH1="";
		int ix;
		ix=m_comboJSJDH.FindStringExact(-1,m_csJSDBH1);
		if(ix==-1 && m_csJSDBH1!="")
		{
			ix=0;
			m_comboJSJDH.InsertString(0,m_csJSDBH1);
		}
		m_comboJSJDH.SetCurSel(ix);
		if(m_ActiveRs->GetCollect(_variant_t("ZDJH")).vt!=VT_NULL)
			m_csZDJH.Format("%d",(long)m_ActiveRs->GetCollect(_variant_t("ZDJH")));
		else
			m_csZDJH="";
		ix=m_comboZDJH1.FindStringExact(-1,m_csZDJH);
		if(ix==-1 && m_csZDJH!="")
		{
			ix=0;
			m_comboZDJH1.InsertString(0,m_csZDJH);
		}
		m_comboZDJH1.SetCurSel(ix);
	}
	catch(_com_error e)
	{
	}
	UpdateData(FALSE);
	m_OptPag4->UpdateData(FALSE);
	m_OptPag5->UpdateData(FALSE);
	m_OptPag6->UpdateData(FALSE);
	m_PagItem->UpdateData(FALSE);
	m_PagOption->UpdateData(FALSE);
}

void CViewTxsr::UpdateBoundData()
{
    try
	{

		if(m_ActiveRs->adoEOF || m_ActiveRs->BOF)
			return;
		if(m_ActiveRs->Status==adRecDeleted || m_ActiveRs->Status==adRecDBDeleted)
			return;
		if(!m_bAllowUpd)
			return;
		UpdateData();
		m_OptPag4->UpdateData();
		m_OptPag5->UpdateData();
		m_OptPag6->UpdateData();
		m_PagItem->UpdateData();
		m_PagOption->UpdateData();
		if(m_ActiveRs->Fields->Item[_variant_t("ZDJH")]->Value != _variant_t((long)m_lngZDJH))	
		{
			long oldzdjh=vtoi(m_ActiveRs->Fields->Item[_variant_t("ZDJH")]->Value);
			m_ActiveRs->Fields->Item[_variant_t("ZDJH")]->Value = _variant_t((long)m_lngZDJH);
			CString stmp;
			stmp.Format("%d",oldzdjh);
			int fl=m_comboZDJH1.FindStringExact(-1,stmp);
			stmp.Format("%d",m_lngZDJH);
			if(fl!=CB_ERR && oldzdjh != m_lngZDJH)
			{
				m_comboZDJH1.DeleteString(fl);
				m_comboZDJH1.InsertString(fl,stmp);
				m_comboZDJH1.SetCurSel(fl);
			}
			else 
			{
				m_comboZDJH1.AddString(stmp);
				m_comboZDJH1.SetCurSel(0);
			}

		}
		m_ActiveRs->Fields->Item[_variant_t("VolumeID")]->Value=_variant_t(EDIBgbl::SelVlmID);


		if(m_ActiveRs==NULL || m_ActiveRs->State==adStateClosed)
			return;
		_variant_t vSel;
		vSel = m_ActiveRs->Fields->Item[_variant_t("iSelSampleID")]->Value;
		if(vSel !=_variant_t((long)m_lSelSampleID))
			m_ActiveRs->Fields->Item[_variant_t("iSelSampleID")]->Value=_variant_t((long)m_lSelSampleID);
		if(m_ActiveRs->Fields->Item[_variant_t("T01")]->Value!=_variant_t((float)atof(m_csT01)))
			m_ActiveRs->Fields->Item[_variant_t("T01")]->Value=_variant_t((float)atof(m_csT01));
		if(m_ActiveRs->Fields->Item[_variant_t("BornSA")]->Value!=_variant_t(m_strBornSA))//pfg2007.07.15
			m_ActiveRs->Fields->Item[_variant_t("BornSA")]->Value=_variant_t(m_strBornSA);
		GetBornSA(m_strBornSA);//pfg2007.07.18
		if(m_ActiveRs->Fields->Item[_variant_t("BornSAEngName")]->Value!=_variant_t(m_strBornSAEng))
			m_ActiveRs->Fields->Item[_variant_t("BornSAEngName")]->Value=_variant_t(m_strBornSAEng);
		if(m_ActiveRs->Fields->Item[_variant_t("BornSAEngChina")]->Value!=_variant_t(m_strBornSAEngChina))
			m_ActiveRs->Fields->Item[_variant_t("BornSAEngChina")]->Value=_variant_t(m_strBornSAEngChina);

		//设置热态数据>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		if(m_ActiveRs->Fields->Item[_variant_t("xr1")]->Value!=_variant_t(m_OptPag5->m_fDIlineDispX))
			m_ActiveRs->Fields->Item[_variant_t("xr1")]->Value=_variant_t(m_OptPag5->m_fDIlineDispX);
		if(m_ActiveRs->Fields->Item[_variant_t("yr1")]->Value!=_variant_t(m_OptPag5->m_fDIlineDispY))
			m_ActiveRs->Fields->Item[_variant_t("yr1")]->Value=_variant_t(m_OptPag5->m_fDIlineDispY);
		if(m_ActiveRs->Fields->Item[_variant_t("zr1")]->Value!=_variant_t(m_OptPag5->m_fDIlineDispZ))
			m_ActiveRs->Fields->Item[_variant_t("zr1")]->Value=_variant_t(m_OptPag5->m_fDIlineDispZ);
		if(m_ActiveRs->Fields->Item[_variant_t("pxr1")]->Value!=_variant_t(m_OptPag5->m_fDIFx))
			m_ActiveRs->Fields->Item[_variant_t("pxr1")]->Value=_variant_t(m_OptPag5->m_fDIFx);
		if(m_ActiveRs->Fields->Item[_variant_t("pyr1")]->Value!=_variant_t(m_OptPag5->m_fDIFy))
			m_ActiveRs->Fields->Item[_variant_t("pyr1")]->Value=_variant_t(m_OptPag5->m_fDIFy);
		if(m_ActiveRs->Fields->Item[_variant_t("pzr1")]->Value!=_variant_t(m_OptPag5->m_fDIFz))
			m_ActiveRs->Fields->Item[_variant_t("pzr1")]->Value=_variant_t(m_OptPag5->m_fDIFz);
		if(m_ActiveRs->Fields->Item[_variant_t("mxr1")]->Value!=_variant_t(m_OptPag5->m_fDImX))
			m_ActiveRs->Fields->Item[_variant_t("mxr1")]->Value=_variant_t(m_OptPag5->m_fDImX);
		if(m_ActiveRs->Fields->Item[_variant_t("myr1")]->Value!=_variant_t(m_OptPag5->m_fDImY))
			m_ActiveRs->Fields->Item[_variant_t("myr1")]->Value=_variant_t(m_OptPag5->m_fDImY);
		if(m_ActiveRs->Fields->Item[_variant_t("mzr1")]->Value!=_variant_t(m_OptPag5->m_fDImZ))
			m_ActiveRs->Fields->Item[_variant_t("mzr1")]->Value=_variant_t(m_OptPag5->m_fDImZ);
		if(m_ActiveRs->Fields->Item[_variant_t("rxr1")]->Value!=_variant_t(m_OptPag5->m_fDIRotDispX))
			m_ActiveRs->Fields->Item[_variant_t("rxr1")]->Value=_variant_t(m_OptPag5->m_fDIRotDispX);
		if(m_ActiveRs->Fields->Item[_variant_t("ryr1")]->Value!=_variant_t(m_OptPag5->m_fDIRotDispY))
			m_ActiveRs->Fields->Item[_variant_t("ryr1")]->Value=_variant_t(m_OptPag5->m_fDIRotDispY);
		if(m_ActiveRs->Fields->Item[_variant_t("rzr1")]->Value!=_variant_t(m_OptPag5->m_fDIRotDispZ))
			m_ActiveRs->Fields->Item[_variant_t("rzr1")]->Value=_variant_t(m_OptPag5->m_fDIRotDispZ);
		////设置冷态数据>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		if(m_ActiveRs->Fields->Item[_variant_t("xl1")]->Value!=_variant_t(m_OptPag6->m_fDIlineDispX))
			m_ActiveRs->Fields->Item[_variant_t("xl1")]->Value=_variant_t(m_OptPag6->m_fDIlineDispX);
		if(m_ActiveRs->Fields->Item[_variant_t("yl1")]->Value!=_variant_t(m_OptPag6->m_fDIlineDispY))
			m_ActiveRs->Fields->Item[_variant_t("yl1")]->Value=_variant_t(m_OptPag6->m_fDIlineDispY);
		if(m_ActiveRs->Fields->Item[_variant_t("zl1")]->Value!=_variant_t(m_OptPag6->m_fDIlineDispZ))
			m_ActiveRs->Fields->Item[_variant_t("zl1")]->Value=_variant_t(m_OptPag6->m_fDIlineDispZ);
		if(m_ActiveRs->Fields->Item[_variant_t("pxl1")]->Value!=_variant_t(m_OptPag6->m_fDIFx))
			m_ActiveRs->Fields->Item[_variant_t("pxl1")]->Value=_variant_t(m_OptPag6->m_fDIFx);
		if(m_ActiveRs->Fields->Item[_variant_t("pyl1")]->Value!=_variant_t(m_OptPag6->m_fDIFy))
			m_ActiveRs->Fields->Item[_variant_t("pyl1")]->Value=_variant_t(m_OptPag6->m_fDIFy);
		if(m_ActiveRs->Fields->Item[_variant_t("pzl1")]->Value!=_variant_t(m_OptPag6->m_fDIFz))
			m_ActiveRs->Fields->Item[_variant_t("pzl1")]->Value=_variant_t(m_OptPag6->m_fDIFz);
		if(m_ActiveRs->Fields->Item[_variant_t("mxl1")]->Value!=_variant_t(m_OptPag6->m_fDImX))
			m_ActiveRs->Fields->Item[_variant_t("mxl1")]->Value=_variant_t(m_OptPag6->m_fDImX);
		if(m_ActiveRs->Fields->Item[_variant_t("myl1")]->Value!=_variant_t(m_OptPag6->m_fDImY))
			m_ActiveRs->Fields->Item[_variant_t("myl1")]->Value=_variant_t(m_OptPag6->m_fDImY);
		if(m_ActiveRs->Fields->Item[_variant_t("mzl1")]->Value!=_variant_t(m_OptPag6->m_fDImZ))
			m_ActiveRs->Fields->Item[_variant_t("mzl1")]->Value=_variant_t(m_OptPag6->m_fDImZ);
		if(m_ActiveRs->Fields->Item[_variant_t("rxl1")]->Value!=_variant_t(m_OptPag6->m_fDIRotDispX))
			m_ActiveRs->Fields->Item[_variant_t("rxl1")]->Value=_variant_t(m_OptPag6->m_fDIRotDispX);
		if(m_ActiveRs->Fields->Item[_variant_t("ryl1")]->Value!=_variant_t(m_OptPag6->m_fDIRotDispY))
			m_ActiveRs->Fields->Item[_variant_t("ryl1")]->Value=_variant_t(m_OptPag6->m_fDIRotDispY);
		if(m_ActiveRs->Fields->Item[_variant_t("rzl1")]->Value!=_variant_t(m_OptPag6->m_fDIRotDispZ))
			m_ActiveRs->Fields->Item[_variant_t("rzl1")]->Value=_variant_t(m_OptPag6->m_fDIRotDispZ);

		

		//装入柱子编号

		if(m_ActiveRs->Fields->Item[_variant_t("XZU1")]->Value!=stov(m_txtXZU1))
			m_ActiveRs->Fields->Item[_variant_t("XZU1")]->Value=stov(m_txtXZU1);
		if(m_ActiveRs->Fields->Item[_variant_t("XZU2")]->Value!=stov(m_txtXZU2))
			m_ActiveRs->Fields->Item[_variant_t("XZU2")]->Value=stov(m_txtXZU2);
		if(m_ActiveRs->Fields->Item[_variant_t("XZU3")]->Value!=stov(m_txtXZU3))
			m_ActiveRs->Fields->Item[_variant_t("XZU3")]->Value=stov(m_txtXZU3);
		if(m_ActiveRs->Fields->Item[_variant_t("XZU4")]->Value!=stov(m_txtXZU4))
			m_ActiveRs->Fields->Item[_variant_t("XZU4")]->Value=stov(m_txtXZU4);
		if(m_ActiveRs->Fields->Item[_variant_t("XZU5")]->Value!=stov(m_txtXZU5))
			m_ActiveRs->Fields->Item[_variant_t("XZU5")]->Value=stov(m_txtXZU5);
		if(m_ActiveRs->Fields->Item[_variant_t("XZU6")]->Value!=stov(m_txtXZU6))
			m_ActiveRs->Fields->Item[_variant_t("XZU6")]->Value=stov(m_txtXZU6);
		if(m_ActiveRs->Fields->Item[_variant_t("XZU7")]->Value!=stov(m_txtXZU7))
			m_ActiveRs->Fields->Item[_variant_t("XZU7")]->Value=stov(m_txtXZU7);
		if(m_ActiveRs->Fields->Item[_variant_t("XZU8")]->Value!=stov(m_txtXZU8))
			m_ActiveRs->Fields->Item[_variant_t("XZU8")]->Value=stov(m_txtXZU8);

		if(m_ActiveRs->Fields->Item[_variant_t("ZZU1")]->Value!=stov(m_txtZZU1))
			m_ActiveRs->Fields->Item[_variant_t("ZZU1")]->Value=stov(m_txtZZU1);
		if(m_ActiveRs->Fields->Item[_variant_t("ZZU2")]->Value!=stov(m_txtZZU2))
			m_ActiveRs->Fields->Item[_variant_t("ZZU2")]->Value=stov(m_txtZZU2);
		if(m_ActiveRs->Fields->Item[_variant_t("ZZU3")]->Value!=stov(m_txtZZU3))
			m_ActiveRs->Fields->Item[_variant_t("ZZU3")]->Value=stov(m_txtZZU3);
		if(m_ActiveRs->Fields->Item[_variant_t("ZZU4")]->Value!=stov(m_txtZZU4))
			m_ActiveRs->Fields->Item[_variant_t("ZZU4")]->Value=stov(m_txtZZU4);
		if(m_ActiveRs->Fields->Item[_variant_t("ZZU5")]->Value!=stov(m_txtZZU5))
			m_ActiveRs->Fields->Item[_variant_t("ZZU5")]->Value=stov(m_txtZZU5);
		if(m_ActiveRs->Fields->Item[_variant_t("ZZU6")]->Value!=stov(m_txtZZU6))
			m_ActiveRs->Fields->Item[_variant_t("ZZU6")]->Value=stov(m_txtZZU6);
		if(m_ActiveRs->Fields->Item[_variant_t("ZZU7")]->Value!=stov(m_txtZZU7))
			m_ActiveRs->Fields->Item[_variant_t("ZZU7")]->Value=stov(m_txtZZU7);
		if(m_ActiveRs->Fields->Item[_variant_t("ZZU8")]->Value!=stov(m_txtZZU8))
			m_ActiveRs->Fields->Item[_variant_t("ZZU8")]->Value=stov(m_txtZZU8);

		
		//
		if(m_ActiveRs->Fields->Item[_variant_t("GH1")]->Value!=_variant_t(m_fGH1))
			m_ActiveRs->Fields->Item[_variant_t("GH1")]->Value=_variant_t(m_fGH1);
		//支吊架类型
		//	if(m_ActiveRs->Fields->Item[_variant_t("TYPE")]->Value!=stov(m_txtType))
		//		m_ActiveRs->Fields->Item[_variant_t("TYPE")]->Value=stov(m_txtType);
		//不统计根部
		if(m_ActiveRs->Fields->Item[_variant_t("bNotSumSA")]->Value!=_variant_t((long)m_PagOption->m_bNotSumSA))
			m_ActiveRs->Fields->Item[_variant_t("bNotSumSA")]->Value=_variant_t((long)m_PagOption->m_bNotSumSA);
		//弹簧荷载变化系数
		if(m_ActiveRs->Fields->Item[_variant_t("CoefOfVarLoad")]->Value!=_variant_t(m_fCoefOfVarLoad))
			m_ActiveRs->Fields->Item[_variant_t("CoefOfVarLoad")]->Value=_variant_t(m_fCoefOfVarLoad);

		//弹簧并联数
		_variant_t tmpVar=_variant_t(atol(m_csParallelingNum));
		if(m_ActiveRs->Fields->Item[_variant_t("ParallelingNum")]->Value!=tmpVar)
			m_ActiveRs->Fields->Item[_variant_t("ParallelingNum")]->Value=tmpVar;
		//根部l值
		tmpVar=_variant_t((float)atof(m_csGDW1));
		if(m_ActiveRs->Fields->Item[_variant_t("GDW1")]->Value!=tmpVar)
			m_ActiveRs->Fields->Item[_variant_t("GDW1")]->Value=tmpVar;
		//根部l1
		tmpVar=_variant_t(m_fLC1);
		if(m_ActiveRs->Fields->Item[_variant_t("LC1")]->Value!=tmpVar)
			m_ActiveRs->Fields->Item[_variant_t("LC1")]->Value=tmpVar;
		//根部B值
		tmpVar=_variant_t(m_fB01);
		if(m_ActiveRs->Fields->Item[_variant_t("B01")]->Value!=tmpVar)
			m_ActiveRs->Fields->Item[_variant_t("B01")]->Value=tmpVar;
		//根部A(或H)值
		tmpVar=_variant_t(m_fA01);
		if(m_ActiveRs->Fields->Item[_variant_t("A01")]->Value!=tmpVar)
			m_ActiveRs->Fields->Item[_variant_t("A01")]->Value=tmpVar;
		//根部总长度L
		//tmpVar=_variant_t(m_fSATotalLength);
		//if(m_ActiveRs->Fields->Item[_variant_t("SATotalLength")]->Value!=tmpVar)
		//	m_ActiveRs->Fields->Item[_variant_t("SATotalLength")]->Value=tmpVar;

		tmpVar=_variant_t(m_fDXA1);
		if(m_ActiveRs->Fields->Item[_variant_t("DXA1")]->Value!=tmpVar)
			m_ActiveRs->Fields->Item[_variant_t("DXA1")]->Value=tmpVar;
		tmpVar=_variant_t(m_fDZA1);
		if(m_ActiveRs->Fields->Item[_variant_t("DZA1")]->Value!=tmpVar)
			m_ActiveRs->Fields->Item[_variant_t("DZA1")]->Value=tmpVar;
		//本支吊架数量
		tmpVar=_variant_t((long)m_PagOption->m_iNum);
		if(m_ActiveRs->Fields->Item[_variant_t("Num")]->Value!=tmpVar)
			m_ActiveRs->Fields->Item[_variant_t("Num")]->Value=tmpVar;
		//管部材料
		tmpVar=stov(m_PagOption->m_csDCL1);
		if(m_ActiveRs->Fields->Item[_variant_t("DCL1")]->Value!=tmpVar)
			m_ActiveRs->Fields->Item[_variant_t("DCL1")]->Value=tmpVar;
		//较长垂直管道的刚性支吊架
		tmpVar=_variant_t((long)m_PagOption->m_bifLongVertPipe);
		if(m_ActiveRs->Fields->Item[_variant_t("ifLongVertPipe")]->Value!=tmpVar)
			m_ActiveRs->Fields->Item[_variant_t("ifLongVertPipe")]->Value=tmpVar;
		//管外径
		tmpVar=_variant_t((float)atof(m_csDJ1));
		if(m_ActiveRs->Fields->Item[_variant_t("DJ1")]->Value!=tmpVar)
			m_ActiveRs->Fields->Item[_variant_t("DJ1")]->Value=tmpVar;
		tmpVar=_variant_t((float)atof(m_csDwB));
		if(m_ActiveRs->Fields->Item[_variant_t("DwB")]->Value!=tmpVar)
			m_ActiveRs->Fields->Item[_variant_t("DwB")]->Value=tmpVar;

		tmpVar=_variant_t(m_fDH1);
		if(m_ActiveRs->Fields->Item[_variant_t("DH1")]->Value!=tmpVar)
			m_ActiveRs->Fields->Item[_variant_t("DH1")]->Value=tmpVar;

		/*tmpVar=_variant_t(m_OptPag4->m_fHAZ);
		if(m_ActiveRs->Fields->Item[_variant_t("HAZ")]->Value!=tmpVar)
			m_ActiveRs->Fields->Item[_variant_t("HAZ")]->Value=tmpVar;*/
		tmpVar=_variant_t(m_OptPag4->m_fPAZ1);
		if(m_ActiveRs->Fields->Item[_variant_t("PAZ1")]->Value!=tmpVar)
			m_ActiveRs->Fields->Item[_variant_t("PAZ1")]->Value=tmpVar;
		tmpVar=_variant_t(m_OptPag4->m_fPAZ0);
		if(m_ActiveRs->Fields->Item[_variant_t("PAZ0")]->Value!=tmpVar)
			m_ActiveRs->Fields->Item[_variant_t("PAZ0")]->Value=tmpVar;
		tmpVar=_variant_t(m_OptPag4->m_fPGZ1);
		if(m_ActiveRs->Fields->Item[_variant_t("PGZ1")]->Value!=tmpVar)
			m_ActiveRs->Fields->Item[_variant_t("PGZ1")]->Value=tmpVar;
		tmpVar=_variant_t(m_OptPag4->m_fPJG1);
		if(m_ActiveRs->Fields->Item[_variant_t("PJG1")]->Value!=tmpVar)
			m_ActiveRs->Fields->Item[_variant_t("PJG1")]->Value=tmpVar;
		//根部BornSA值
		tmpVar=_variant_t(m_strBornSA);
		if(m_ActiveRs->Fields->Item[_variant_t("BornSA")]->Value!=tmpVar)
			m_ActiveRs->Fields->Item[_variant_t("BornSA")]->Value=tmpVar;
		GetBornSA(m_strBornSA); //pfg2007.06.18
		tmpVar=_variant_t(m_strBornSAEng);
		if(m_ActiveRs->Fields->Item[_variant_t("BornSAEngName")]->Value!=tmpVar)
			m_ActiveRs->Fields->Item[_variant_t("BornSAEngName")]->Value=tmpVar;
		tmpVar=_variant_t(m_strBornSAEngChina);
		if(m_ActiveRs->Fields->Item[_variant_t("BornSAEngChina")]->Value!=tmpVar)
			m_ActiveRs->Fields->Item[_variant_t("BornSAEngChina")]->Value=tmpVar;
	}
	catch(_com_error e)
	{
#ifdef _DEBUG
		ShowMessage(e.Description());
#endif
	}
}

void CViewTxsr::ChangeImg(CStatic &con, UINT nID,BOOL bt)
{
	HBITMAP bmp;
	bmp=LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(nID));
	if(con.GetBitmap()!=NULL)
		DeleteObject(con.GetBitmap());
	if(bmp==NULL) return;
	if(bt)
	{
		COLORREF color;//,tmpcl;
		color=::GetSysColor(COLOR_3DFACE);
		CDC memDC;
		CDC* pDC;
		CPen pen;
		CBrush br;
		pDC=con.GetDC();
		CPen Pen(PS_NULL,0,(COLORREF)0);
		br.CreateSolidBrush(color);
//		int oldMod;
		BITMAP bm;
		::GetObject(bmp,sizeof(bm),(LPVOID)&bm);
		memDC.CreateCompatibleDC(pDC);
		::SelectObject((HDC)memDC,bmp);
		int iROP2=memDC.SetROP2(R2_MASKPEN);//像素为笔颜色与屏幕颜色的组合色
		CBrush* pOldBr = memDC.SelectObject(&br);
		CPen* pOldPen=memDC.SelectObject(&Pen);
		memDC.Rectangle(0,0,bm.bmWidth+1,bm.bmHeight+1);
		memDC.SetROP2(iROP2);
		memDC.SelectObject(pOldPen);
		memDC.SelectObject(pOldBr);
		memDC.DeleteDC();
		br.DeleteObject();
		pen.DeleteObject();
		con.ReleaseDC(pDC);
	}
	
	con.SetBitmap(bmp);
}


void CViewTxsr::OnImgViewg1() 
{
	// TODO: Add your control notification handler code here
	//On Error Resume Next
   //先确定根部方向
	modPHScal::CalfxSA(m_ActiveRs);
   //每次璇转90度
	modPHScal::gf1=modPHScal::GetPhsSAfx((modPHScal::GetPhsSAfx(modPHScal::gf1) + 1)%4);
   CString gf2=modPHScal::GetPhsSAfx((modPHScal::GetPhsSAfx(modPHScal::gf1) + 1)%4);
   //再判断当根部槽钢公用时，是否与管部方向冲突
   modPHScal::CalSpringParallelNum(/*m_ActiveRs,*/modPHScal::dn);
   if(vtoi(m_ActiveRs->GetCollect(_variant_t("Gnum")))==1 
	   && modPHScal::gintParallelNum>=2)
   {
      if (modPHScal::df == "X")
	  {
         if (modPHScal::gf1.Right( 1) != "Z")
            modPHScal::gf1 = "PZ";
	  } 
      else if(modPHScal::gf1.Right( 1) !="X")
            modPHScal::gf1 = "PX";
   }
   //Data1.Recordset.Edit: Data1.Recordset("gf1") = gf1: Data1.Recordset.Update*/
   m_ActiveRs->PutCollect(_variant_t("gf1"),_variant_t(modPHScal::gf1));
   modPHScal::gf1.TrimLeft();modPHScal::gf1.TrimRight();
	modPHScal::gf1.MakeUpper();
	ShowPictureGenbuFX();
	ShowPicturePASA (iSA);
   ShowPicturePASA (iPA);
	/*gf2.TrimLeft();gf2.TrimRight();
	gf2.MakeUpper();
	CString ID1=basDirectory::TemplateDir+(m_iCSnum==0 ? "GD" : "GS")+modPHScal::gf1+".bmp";
	//CString("IDB_")+(m_iCSnum==0 ? "GD" : "GS")+modPHScal::gf1;
   CString ID2=basDirectory::TemplateDir+(m_iCSnum==0 ? "GD" : "GS")+gf2+".bmp";
	//CString("IDB_")+(m_iCSnum==0 ? "GD" : "GS")+gf2; 
	ChangeImg(m_ImageViewG1,ID1);
	ChangeImg(m_ImageViewG0,ID2);
	m_ImageViewG1.SetWindowPos(&wndTop,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	m_ImageViewG0.SetWindowPos(&wndTop,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	m_wndBorand.SetWindowPos(&wndTop,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);*/
	//UpdateBoundData();
	//DatabillReposition();
	
}

void CViewTxsr::ChangeImg(CStatic &con, LPCSTR nID,BOOL bt)
{
	HBITMAP bmp=NULL;
	if(con.GetBitmap()!=NULL)
		DeleteObject(con.GetBitmap());
	bmp=LoadBitmap(::AfxGetInstanceHandle(),nID);
	if(bmp==NULL)
	{
		if(::FileExists(nID)) 
		{
			bmp=(HBITMAP)::LoadImage(AfxGetInstanceHandle(),nID,IMAGE_BITMAP,0,0,
			LR_DEFAULTCOLOR|LR_DEFAULTSIZE|LR_LOADFROMFILE);
		}
	}
	if(bmp==NULL) return;
	if(bt)
	{
		COLORREF color;//,tmpcl;
		color=::GetSysColor(COLOR_3DFACE);
		CDC memDC;
		CDC* pDC;
		CPen pen;
		CBrush br;
		pDC=con.GetDC();
		CPen Pen(PS_SOLID,1,(COLORREF)0);
		br.CreateSolidBrush(color);
//		int oldMod;
		BITMAP bm;
		::GetObject(bmp,sizeof(bm),(LPVOID)&bm);
		memDC.CreateCompatibleDC(pDC);
		HBITMAP hOldBm=(HBITMAP)::SelectObject((HDC)memDC,bmp);
		int iOldROP2=memDC.SetROP2(R2_MASKPEN);
		CBrush* pOldBr=memDC.SelectObject(&br);
		CPen* pOldPen=memDC.SelectObject(&Pen);
		memDC.Rectangle(0,0,bm.bmWidth,bm.bmHeight);
		memDC.SetROP2(iOldROP2);
		memDC.SelectObject(pOldPen);
		memDC.SelectObject(pOldBr);
		::SelectObject((HDC)memDC,hOldBm);
		memDC.DeleteDC();
		br.DeleteObject();
		pen.DeleteObject();
		con.ReleaseDC(pDC);
	}
	con.SetBitmap(bmp);

}

//DEL void CViewTxsr::OnStaticc() 
//DEL {
//DEL 	// TODO: Add your control notification handler code here
//DEL 	OnImgViewg1();
//DEL }

//DEL void CViewTxsr::OnImgViewg0() 
//DEL {
//DEL 	// TODO: Add your control notification handler code here
//DEL 	
//DEL }

void CViewTxsr::DatabillReposition()
{
  //错误处理十分重要
	if(m_bIsAddNew)
		return;
	static long iPreviousZDJH; //保存前个支吊架

	if(!m_bAllowUpd)
	{
		m_bAllowUpd=true;
		return;
	}
   
	try
	{
		if(m_ActiveRs->adoEOF && m_ActiveRs->BOF)
			return;
		modPHScal::CalUnitX(m_ActiveRs);
		//下句在绘图时改变记录号到第一个记录   
		modPHScal::InitializeCrudeData(m_ActiveRs, true);
   
		if(m_ActiveRs->GetCollect(_variant_t("zdjh")).vt!=VT_NULL)
		{
			modPHScal::zdjh=(long)m_ActiveRs->GetCollect(_variant_t("zdjh"));
			SetRegValue("Settings","PhsCurZdjhNo",modPHScal::zdjh);
			SetRegValue("Settings","PhsCurVlmID",EDIBgbl::SelVlmID );
		}
		else
		{
			m_ActiveRs->PutCollect(_variant_t("zdjh"),_variant_t((long)1));
			modPHScal::zdjh=1;
			m_csZDJH="1";
		}
		CString sTmp,sTmp2;
		_variant_t tmpvar;
		if(m_ActiveRs->GetCollect(_variant_t("PSAver")).vt!=VT_NULL)
			m_PagItem->m_PSAver.Format(GetResStr(IDS_PipingStressAnanysis),(char*)_bstr_t(m_ActiveRs->GetCollect(_variant_t("PSAver"))));
		m_PagItem->m_SPRWorks=modPHScal::gsPhsSPRINGSel;//& 弹簧；
		m_PagItem->m_conSPRworks=modPHScal::gsPhsConstantSpringSel; //& 恒吊；
		if (fabs(modPHScal::yr) > modPHScal::gUseSpringMinDist) 
		{
			if(m_txtType=="9999" || m_txtType=="5000")
				m_PagItem->m_ConstantSPRNo.Format(GetResStr(IDS_ConstantSpringTypeIsXXXX),  (char*)_bstr_t(m_ActiveRs->GetCollect("ConstantSPRNo")));
			else
				m_PagItem->m_ConstantSPRNo= "";
		}
		else
		{
			if(m_txtType.GetLength()==4)
			{
				//m_ConstantSPRNo.Format(GetResStr(IDS_ConstantSpringTypeIsXXXX),  (char*)_bstr_t(rs.GetFields().GetItem(_variant_t("ConstantSPRNo")).GetValue()));
				//m_ConstantSPRNo.Format(GetResStr(IDS_ConstantSpringTypeIsXXXX),  "");
				m_PagItem->m_ConstantSPRNo= "";
			}
			else
				m_PagItem->m_ConstantSPRNo= "";
		}

		CString sz;
	   m_Databill.SetCaption(GetResStr(IDS_SelectPHS));
	   sTmp2.Format("%d",m_lSelSampleID);
	   sTmp.Format(GetResStr(IDS_SelectedSampleID),sTmp2);
	   m_cmdSelPhs.SetWindowText(sTmp);
      //cmdSelPhs.Caption = ResolveResString(iUE_, "|1", m_lSelSampleID);
      //The following cause switching to another form--selPDSV
      //phsData!databill.Recordset.AbsolutePosition = rs.AbsolutePosition
		ShowPicturePASA (iSA);
      ShowPicturePASA (iPA);
      ShowPictureGenbuFX();
      InitBornSA();
	  LoadDiameterItem2ComboDW();
      LoadGDWItem2ComboGDW1();  //因为重新加载，会破坏当前值
      LoadTJItem2ComboT01();  //因为重新加载，会破坏当前值
	  
      //CalSpringParallelNum(modPHScal::dn);   //必须计算出来，因为在显示根部数量时需要根据此值计算
      //ComboParallelingNum = gintParallelNum
      SetOptionPZ();    //设置偏装
      //if( iPreviousZDJH != modPHScal::zdjh )
      {
         DisPlaySANum();    //显示根部数量glNumSA：该值可用于共用根部结构。
         //DisplayDataZB();
         iPreviousZDJH = modPHScal::zdjh;
      }

      
	  m_PagItem->m_txtCLB1= EDIBAcad::g_strCustomPicNoFileName;
	  m_PagItem->m_LblPrjName = EDIBgbl::SelPrjName;
	  
      //单位转换
      //ConvertUnit();

		//显示坐标轴方向		
		if( vtoi(m_ActiveRs->GetCollect("UPxyz")) < 1 || vtoi(m_ActiveRs->GetCollect("UPxyz")) >3 )
		{
			m_ActiveRs->PutCollect("UPxyz",_variant_t((long)2));
	   }
		giUPxyz=vtoi(m_ActiveRs->GetCollect("UPxyz"));

		if( giUPxyz == 2)
		{
			//y方向垂直向上
			//ChangeImg(m_ImageXY0,basDirectory::TemplateDir+"UPyRx.bmp");
			//ChangeImg(m_ImageXY1,basDirectory::TemplateDir+"UPyRz.bmp");
			ChangeImg(m_ImageXY0,IDB_UPYRX);
			ChangeImg(m_ImageXY1,IDB_UPYRZ);
		}
		else if( giUPxyz == 3)
		{
			//z方向垂直向上
			//ChangeImg(m_ImageXY0,basDirectory::TemplateDir+"UPzRx.bmp");
			//ChangeImg(m_ImageXY1,basDirectory::TemplateDir+"UPzRy.bmp");
			ChangeImg(m_ImageXY0,IDB_UPZRX);
			ChangeImg(m_ImageXY1,IDB_UPZRY);
		}
		else if( giUPxyz == 1)
		{
			//x方向垂直向上
			//ChangeImg(m_ImageXY0,basDirectory::TemplateDir+"UPxRy.bmp");
			//ChangeImg(m_ImageXY1,basDirectory::TemplateDir+"UPxRz.bmp");
			ChangeImg(m_ImageXY0,IDB_UPXRY);
			ChangeImg(m_ImageXY1,IDB_UPXRZ);
		}
		if(modPHScal::dn=="Z2" || modPHScal::dn=="Z2A" || modPHScal::dn=="SZ5")
		{
			m_PagOption->GetDlgItem(IDC_EDIT_PSLENGTH)->EnableWindow(TRUE);
			m_PagOption->GetDlgItem(IDC_EDIT_PSWIDTH)->EnableWindow(TRUE);
			m_PagOption->GetDlgItem(IDC_EDIT_PSTHICK)->EnableWindow(TRUE);
		}
		else
		{
			m_PagOption->GetDlgItem(IDC_EDIT_PSLENGTH)->EnableWindow(FALSE);
			m_PagOption->GetDlgItem(IDC_EDIT_PSWIDTH)->EnableWindow(FALSE);
			m_PagOption->GetDlgItem(IDC_EDIT_PSTHICK)->EnableWindow(FALSE);
		}
		if(vtos(m_ActiveRs->GetCollect("GN1"))=="G100")
		{
			m_PagOption->GetDlgItem(IDC_COMBO1)->EnableWindow(TRUE);
		}
		else
		{
			m_PagOption->GetDlgItem(IDC_COMBO1)->EnableWindow(FALSE);
		}
		UpdateData(false);
		m_PagOption->UpdateData(FALSE);
	}
   catch(_com_error e)
   {
	   //ShowMessage(e.Description());
   }
}

void CViewTxsr::ShowPicturePASA(int Index)
{
	try
	{
		_variant_t v;
		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(Recordset));

		_variant_t varTmp;
		CString strSQL;
		strSQL.Format("SELECT CustomID FROM PhsStructureREF WHERE SampleID=%d ORDER BY SEQ ",vtoi(m_ActiveRs->GetCollect("iSelSampleID")));
		rs->Open((_bstr_t)strSQL,_variant_t((IDispatch*)EDIBgbl::dbPHScode,true), 
			adOpenStatic, adLockOptimistic, adCmdText); 
		if(rs->BOF && rs->adoEOF)
		{
			rs->Close();
			return;
		}
		rs->get_Collect((_variant_t)0L, &varTmp);
		m_ActiveRs->PutCollect("dn1",_variant_t(vtos(varTmp)));
		if(varTmp.vt!=VT_NULL)
			modPHScal::IDdn = modPHScal::sFindID(vtos(varTmp));
		rs->MoveLast();
		rs->get_Collect((_variant_t)0L, &varTmp);
		m_ActiveRs->PutCollect("gn1",_variant_t(vtos(varTmp)));
		if(varTmp.vt!=VT_NULL)
			modPHScal::IDgn = modPHScal::sFindID(vtos(varTmp));
		rs->Close();
		  CString tmp =Index == iSA ? modPHScal::IDgn : modPHScal::IDdn;
		  tmp.TrimLeft();
		  tmp.TrimRight();
		  if(tmp=="")
			  return;
		CString strBmpName = vtos(modPHScal::sFindFLD("ID", "BmpName", tmp));
		tmp = basDirectory::TemplateDir + strBmpName + ".bmp";
		if(Index==iSA ) ChangeImg(m_Image1,tmp);
		//不可加在此处，否则死循环.
		//经过2000.02.12修改程序直接调用SQL更新数据表，避免使用会引起重新定位事件的数据控件的更新方法。可以正常工作了。
		modPHScal::CalfxPA(m_ActiveRs);//确定管部方向
		if(Index == iPA)
		{
			//管部左视图
			//tmp=CString("IDB_")+(modPHScal::df=="Z" ? "" : "Z")+strBmpName;
			tmp = basDirectory::TemplateDir + (modPHScal::df == "Z" ? "" : "Z") + strBmpName + ".bmp";
			ChangeImg(m_ImageViewD0,tmp);
      
			//管部右视图
	   			tmp = basDirectory::TemplateDir + (modPHScal::df == "X" ? "" : "Z") + strBmpName + ".bmp";
		  //tmp=CString("IDB_")+(modPHScal::df=="X" ? "" : "X")+strBmpName;
		  ChangeImg(m_ImageViewD1,tmp);
			//tmp = TemplateDir & IIf(df = "X", "", "Z") & strBmpName & ".bmp"
		}
		m_Image1.SetWindowPos(&wndTop,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		m_ImageViewD1.SetWindowPos(&wndTop,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		m_ImageViewD0.SetWindowPos(&wndTop,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		m_wndBorand.SetWindowPos(&wndTop,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		/*m_comboDwB.SetWindowPos(&wndBottom,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		m_comboDW.SetWindowPos(&wndBottom,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		GetDlgItem(IDC_EDIT14)->SetWindowPos(&wndBottom,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		GetDlgItem(IDC_EDT_SATL)->SetWindowPos(&wndBottom,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		GetDlgItem(IDC_EDT_A01)->SetWindowPos(&wndBottom,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);*/
		//m_ImageViewD1.SetWindowPos(&wndBottom,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}

	catch(...)
	{
	}
}

void CViewTxsr::ShowPictureGenbuFX()
{
   //先确定根部方向,再显示根部方向的位图,位图文件前缀单槽钢为GS,双槽钢为GD
   //死循环CalfxSA
   //经过2000.02.12修改程序直接调用SQL更新数据表，避免使用会引起重新定位事件的数据控件的更新方法。可以正常工作了。
	modPHScal::CalfxSA(m_ActiveRs);
   CString tmp;
   modPHScal::fx = modPHScal::gf1.Left(1);
   //fx:"P","O"
   if (modPHScal::gf1.Right(1) == "X")
      modPHScal::gf = "X";
   else
      modPHScal::gf = "Z";
   tmp=CString("IDB_")+(m_iCSnum==0 ? "GD" : "GS")+modPHScal::gf1;
   //tmp = basDirectory::TemplateDir + (m_iCSnum = 0, "GD", "GS") + modPHScal::gf1 + ".bmp"; //PROBLEM
   ChangeImg(m_ImageViewG1,tmp);
   tmp = CString("IDB_") + (m_iCSnum == 0 ? "GD" : "GS") + modPHScal::GetPhsSAfx((modPHScal::GetPhsSAfx(modPHScal::gf1) + 1)% 4);
   //tmp = basDirectory::TemplateDir + (m_iCSnum = 0, "GD", "GS") + modPHScal::GetPhsSAfx((modPHScal::GetPhsSAfx(modPHScal::gf1) + 1)% 4) + ".bmp";
   //tmp=CString("IDB_")+(m_iCSnum==0 ? "GD" : "GS")+modPHScal::GetPhsSAfx((modPHScal::GetPhsSAfx(modPHScal::gf1) + 1)% 4);
   ChangeImg(m_ImageViewG0,tmp);
	m_ImageViewD1.SetWindowPos(&wndTop,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	m_ImageViewD0.SetWindowPos(&wndTop,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	m_wndBorand.SetWindowPos(&wndTop,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
}

void CViewTxsr::OnImgViewd1() 
{
	// TODO: Add your control notification handler code here
	//先确定管部方向
	modPHScal::CalfxPA(m_ActiveRs);
	modPHScal::df = (modPHScal::df == "X" ? "Z" : "X");
	m_ActiveRs->PutCollect(_variant_t("DF1"),stov(modPHScal::df));
   //Data1.Recordset.Edit: Data1.Recordsetm_ActiveRs->PutCollect("DF1") = df: Data1.Recordset.Update
   //UpdateBoundData();
	//DatabillReposition();
	ShowPictureGenbuFX();
	ShowPicturePASA (iSA);
   ShowPicturePASA (iPA);
}

void CViewTxsr::LoadTJItem2ComboT01()
{
	//装入管部温度数据到组合框
	LoadtbnPAItem2ComBox(m_comboT01,"tj");

}

void CViewTxsr::CloseRs()
{
	try
	{
		/*m_Databill.SetRecordSource("");
		m_ActiveRs=NULL;*/
		//if(IsWindow(FrmPhsData.m_hWnd))
		//	FrmPhsData.CloseRs();
		if(m_ActiveRs!=NULL && m_ActiveRs->State!=adStateClosed)
		{
			//m_ActiveRs->Update();
			try
			{
				m_ActiveRs->Update();
			}
			catch(...)
			{
				m_ActiveRs->CancelUpdate();
			}
			m_ActiveRs->Close();
		}
	}
	catch(_com_error e)
	{
		//ShowMessage(e.Description());
	}
}

void CViewTxsr::InitRs()
{
	try{
		if(m_ActiveRs->State!=adStateClosed)
		{
			if(!m_ActiveRs->adoEOF && !m_ActiveRs->BOF)
			{
				try
				{
					m_ActiveRs->Update();
				}
				catch(...)
				{
					m_ActiveRs->CancelUpdate();
				}
			}
			try
			{
				m_ActiveRs->Close();
			}
			catch(...)
			{
			}
				
		}
		try
		{
			if(m_ActiveRs==NULL)
			{
				m_ActiveRs.CreateInstance(__uuidof(Recordset));
			}

		}
		catch(...)
		{
		}
		m_bUpd=false;
		m_ActiveRs->CursorLocation=adUseServer;//adUseClient;//adUseServer;
		m_popMenu.GetSubMenu(0)->CheckMenuRadioItem(2,12,2,MF_BYPOSITION);
		m_ActiveRs->Filter=_variant_t((long)adFilterNone);
		m_popMenu.GetSubMenu(0)->CheckMenuRadioItem(3,12,3,MF_BYPOSITION);
		m_ActiveRs->Open(_variant_t(m_SQL),(IDispatch*)conPRJDB,/*adOpenDynamic*/adOpenKeyset,adLockOptimistic,adCmdText);
		m_Databill.SetRefRecordset(m_ActiveRs);
		this->RefreshBoundData();
		if(IsWindow(FrmPhsData.m_hWnd))
			FrmPhsData.InitDBbill();
		m_bUpd=true;
		
		SetUndoCount();
	}
	catch(_com_error e)
	{
		//ShowMessage(e.Description());
	}
}



void CViewTxsr::OnImgImg1() 
{
	// TODO: Add your control notification handler code here
   //必须通过下面2句先设置管部根部类型数据值
	modPHScal::iSelSampleID = Cavphs->GetPhsStructSampleID();
   //下句可以获得并且设置管部根部(iSelSampleID<>Null)
	modPHScal::iSelSampleID = Cavphs->GetPhsStructSampleID();
   ShowPicturePASA(0);
}

void CViewTxsr::OnSelchangeList1() 
{
	// TODO: Add your control notification handler code here
	EDIBgbl::gbDisplayBOM = true;
	FrmDataEDIT.m_bAllowUpd=true;
	if(!IsWindow(FrmDataEDIT.m_hWnd))
	{
		FrmDataEDIT.Create(IDD_DATAEDIT,AfxGetApp()->m_pMainWnd);
		FrmDataEDIT.UpdateWindow();
	}
	::SetWindowPos(FrmDataEDIT.GetSafeHwnd(),HWND_TOP,0,0,0,0,/*SWP_NOACTIVATE|*/SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
}

//DEL void CViewTxsr::OnEditchangeZdjh() 
//DEL {
//DEL 	// TODO: Add your control notification handler code here
//DEL 	m_bActive=false;
//DEL 	UpdateData();
//DEL 	UpdateBoundData();
//DEL 	m_ActiveRs->MoveFirst();
//DEL 	m_ActiveRs->Find(_bstr_t(CString("zdjh=")+m_csZDJH),0,adSearchForward);
//DEL 	m_bActive=true;
//DEL 	RefreshBoundData();
//DEL 	DatabillReposition();
//DEL }

void CViewTxsr::OnSelchangeZdjh() 
{
	// TODO: Add your control notification handler code here
	try
	{
		m_bActive=false;
		CString sZdjh;
		if(m_comboZDJH1.GetCount()==0)
			return;
		int ix=m_comboZDJH1.GetCurSel();
		if(ix<0) ix=0;
		m_comboZDJH1.GetLBText(ix,sZdjh);
		UpdateBoundData();
		m_ActiveRs->MoveFirst();
		m_ActiveRs->Find(_bstr_t(CString("zdjh=")+sZdjh),0,adSearchForward);
		m_bActive=true;
		RefreshBoundData();
		DatabillReposition();
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}

	catch(...)
	{
		//MessageBox(e.Description());
	}
}

void CViewTxsr::OnImgXy0() 
{
	// TODO: Add your control notification handler code here
	try
	{
		if(m_ActiveRs->adoEOF || m_ActiveRs->BOF)
			return;
		if(vtos(m_ActiveRs->GetCollect("PSAver"))!="")
			return;
		giUPxyz = (giUPxyz + 1) % 3;
	   if( giUPxyz == 0 ) giUPxyz = 3;
	   if( giUPxyz == 2)
		{
		  //y方向垂直向上
			//ChangeImg(m_ImageXY0,basDirectory::TemplateDir+"UPyRx.bmp");
			//ChangeImg(m_ImageXY1,basDirectory::TemplateDir+"UPyRz.bmp");
			ChangeImg(m_ImageXY0,IDB_UPYRX);
			ChangeImg(m_ImageXY1,IDB_UPYRZ);
		}
	   else if( giUPxyz == 3)
		{
		  //z方向垂直向上
			//ChangeImg(m_ImageXY0,basDirectory::TemplateDir+"UPzRx.bmp");
			//ChangeImg(m_ImageXY1,basDirectory::TemplateDir+"UPzRy.bmp");
			ChangeImg(m_ImageXY0,IDB_UPZRX);
			ChangeImg(m_ImageXY1,IDB_UPZRY);
	   }
		else if( giUPxyz == 1)
		{
		  //x方向垂直向上
			//ChangeImg(m_ImageXY0,basDirectory::TemplateDir+"UPxRy.bmp");
			//ChangeImg(m_ImageXY1,basDirectory::TemplateDir+"UPxRz.bmp");
			ChangeImg(m_ImageXY0,IDB_UPXRY);
			ChangeImg(m_ImageXY1,IDB_UPXRZ);
	   }
		m_ActiveRs->PutCollect("UPxyz",_variant_t((long)giUPxyz));
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.Description());
	}
}

void CViewTxsr::SetUndoCount()
{
	VARIANT vRecord;
	short nCount = 0;
	
	_RecordsetPtr pRsUndo;
	CString strSQL;
	pRsUndo.CreateInstance (__uuidof(Recordset));
	pRsUndo->CursorLocation = adUseClient;

	strSQL.Format("SELECT * FROM Undo WHERE VolumeID=%ld", EDIBgbl::SelVlmID);	
	
	try
	{
		pRsUndo->Open(_variant_t(strSQL), conPRJDB.GetInterfacePtr(),
			adOpenDynamic, adLockOptimistic, adCmdUnknown);
	}
	catch(...)
	{
		m_UndoCount = 0;
		return;
	}

	try
	{
		while (!pRsUndo->adoEOF && !pRsUndo->BOF)
		{			
			vRecord = pRsUndo->GetCollect ("Times");
			if(vRecord.iVal > nCount)
			{
				nCount = vRecord.iVal;
			}			
			
			pRsUndo->MoveNext();
		}
	}
	catch(...)
	{
		m_UndoCount = 0;
		return;
	}

	try
	{
		pRsUndo->Close();
	}
	catch(...)
	{
	}

	m_UndoCount = nCount;
	return; 
}

void CViewTxsr::UndoCopy(void)
{
	long i;
	VARIANT vRecord;
	VARIANT vBookMark;
	_RecordsetPtr pRsUndo;
	long nCountColforCopy;
	
	nCountColforCopy = m_ActiveRs->GetFields()->GetCount();
	
	pRsUndo.CreateInstance (__uuidof(Recordset));
	pRsUndo->CursorLocation = adUseClient;

	if (m_UndoCount != -1)
	{		
		try
		{
			pRsUndo->Open("SELECT * FROM Undo", conPRJDB.GetInterfacePtr(),
				adOpenDynamic, adLockOptimistic, adCmdUnknown);
		}
		catch(...)
		{
			return;
		}
		
		vBookMark = m_ActiveRs->Bookmark;
		try
		{			
			m_ActiveRs->MoveFirst();
			if(!m_ActiveRs->adoEOF && !m_ActiveRs->BOF)
			{
				m_UndoCount += 1;
				while(!m_ActiveRs->adoEOF && !m_ActiveRs->BOF)
				{				
					pRsUndo->AddNew();
					//	pRsUndo->MoveLast();
					
					for(i = 0; i < nCountColforCopy; i ++)
					{
						vRecord = m_ActiveRs->GetCollect (_variant_t(i));
						pRsUndo->PutCollect(_variant_t(i), vRecord);
					}
					
					vRecord.vt = VT_I2;
					vRecord.iVal = m_UndoCount;
					pRsUndo->PutCollect(_variant_t(i), vRecord);
					m_ActiveRs->MoveNext();
				}
			}
			else
			{
				m_ActiveRs->Bookmark = vBookMark;
				return;
			}
		}
		catch(...)
		{
			m_ActiveRs->Bookmark = vBookMark;
			pRsUndo->Close();			
			InitialUndo();
			m_UndoCount = 0;
			return;
		}
		
		m_ActiveRs->Bookmark = vBookMark;
		try
		{
			pRsUndo->Update ();
			pRsUndo->Close();
		}
		catch(...)
		{
			return;
		}     	
	}		
}

void CViewTxsr::UndoPaste(void)
{
	long i;
	VARIANT vRecord;
	_RecordsetPtr pRsUndo;
	long nCountColforCopy;
	
	CString strSQL;
	strSQL.Format("SELECT * FROM Undo WHERE VolumeID=%ld AND Times=%d", EDIBgbl::SelVlmID, m_UndoCount);
	
	pRsUndo.CreateInstance (__uuidof(Recordset));
	pRsUndo->CursorLocation = adUseServer;
	
	if (m_UndoCount > 0)
	{		
		try
		{
			pRsUndo->Open(_variant_t(strSQL), conPRJDB.GetInterfacePtr(),
				adOpenDynamic, adLockOptimistic, adCmdUnknown);
		}
		catch(...)
		{
			return;
		}
		
		nCountColforCopy = pRsUndo->GetFields()->GetCount() - 1;
		try
		{			
			m_ActiveRs->MoveFirst();
			while(!m_ActiveRs->adoEOF && !m_ActiveRs->BOF)
			{
				m_ActiveRs->Delete(adAffectCurrent);
				m_ActiveRs->MoveFirst();
			}
			
			while(!pRsUndo->adoEOF && !pRsUndo->BOF)
			{
				m_ActiveRs->AddNew();
				
				for (i = 0; i < nCountColforCopy; i ++)
				{
					vRecord = pRsUndo->GetCollect(_variant_t(i));
					m_ActiveRs->PutCollect(_variant_t(i), vRecord);
				}
				
				pRsUndo->MoveNext();				
			}            
			
			m_ActiveRs->MoveFirst();
			m_ActiveRs->Update ();
		
			try
			{
				FrmPhsData.m_DBGbill.Scroll (0,-1);
			}
			catch(...)
			{
			}

			pRsUndo->Close();
			UndoDelete();
		}
		catch(...)
		{	
			try
			{
				pRsUndo->Close();			
			}
			catch(...)
			{			
			}
			
			InitialUndo();
			m_UndoCount = 0;
		}
	}
}

void CViewTxsr::UndoDelete()
{
	_RecordsetPtr pRsUndo;
	
	CString strSQL;
	strSQL.Format("SELECT * FROM Undo WHERE VolumeID=%ld AND Times=%d", EDIBgbl::SelVlmID, m_UndoCount);
	
	pRsUndo.CreateInstance (__uuidof(Recordset));
	pRsUndo->CursorLocation = adUseServer;

	if (m_UndoCount > 0)
	{		
		try
		{
			pRsUndo->Open(_variant_t(strSQL), conPRJDB.GetInterfacePtr(),
				adOpenDynamic, adLockOptimistic, adCmdUnknown);
		}
		catch(...)
		{
			return;
		}

		try
		{
			while(!pRsUndo->adoEOF && !pRsUndo->BOF)
			{
				pRsUndo->Delete (adAffectCurrent);
				pRsUndo->MoveFirst ();
			}
		}
		catch(...)
		{
			try
			{
				pRsUndo->Close ();
			}
			catch (_com_error &e)
			{
				CString strMsg;
				strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
				AfxMessageBox(strMsg);
			}
			catch(...)
			{				
			}

			m_UndoCount = 0;
			InitialUndo();
			return;
		}

		try
		{
			pRsUndo->Close ();
		}
		catch(...)
		{
			m_UndoCount = 0;
			InitialUndo();
			return;
		}

		m_UndoCount -= 1;
	}
}

void CViewTxsr::EditCopy()
{
	short i;
	short nCountRowforCopy;
	long j;	
	long nCountColforCopy;
	
	VARIANT vBookMark;
	VARIANT vRecord;
	_RecordsetPtr pRsClip;
	
	nCountRowforCopy = FrmPhsData.m_DBGbill.GetSelBookmarks().GetCount();
	nCountColforCopy = FrmPhsData.m_DBGbill.GetColumns ().GetCount ();
	vBookMark.vt = VT_I2;

	pRsClip.CreateInstance (__uuidof(Recordset));
	pRsClip->CursorLocation = adUseServer;	
		
	
	try
	{
		pRsClip->Open("SELECT * FROM Clip", conPRJDB.GetInterfacePtr(),
			adOpenDynamic, adLockOptimistic, adCmdUnknown);
	}
	catch(...)
	{		
		try
		{
			conPRJDB->Execute ("SELECT ZA.* INTO Clip FROM ZA", NULL, adExecuteNoRecords);	    
		    
		}
		catch (_com_error &e)
		{
			CString strMsg;
			strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
			AfxMessageBox(strMsg);
		}
		catch(...)
		{
			return;
		}
		
		try
		{
			pRsClip->Open("SELECT * FROM Clip", conPRJDB.GetInterfacePtr(),
				adOpenDynamic, adLockOptimistic, adCmdUnknown);
		}
		catch (_com_error &e)
		{
			CString strMsg;
			strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
			AfxMessageBox(strMsg);
		}
		catch(...)
		{
			return;
		}
	}

	while(!pRsClip->adoEOF && !pRsClip->BOF)
	{
		pRsClip->Delete (adAffectCurrent);
		pRsClip->MoveFirst ();
	}

    try
	{			
		for(i = 0; i < nCountRowforCopy; i ++)
		{
			vBookMark.iVal = i;
			m_ActiveRs->Bookmark = FrmPhsData.m_DBGbill.GetSelBookmarks().GetItem(vBookMark);		

			if(!pRsClip->adoEOF && !pRsClip->BOF)
			{
			}
			else
			{
				pRsClip->AddNew ();		
				pRsClip->Update();
			}

			for(j = 0; j < nCountColforCopy; j ++)
			{
				vRecord = m_ActiveRs->GetCollect(_variant_t(j));
				pRsClip->PutCollect(_variant_t(j), _variant_t(vRecord));
			}
			
			pRsClip->MoveNext ();
		}	
    }
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
		while(!pRsClip->adoEOF && !pRsClip->BOF)
		{
			pRsClip->Delete (adAffectCurrent);
			pRsClip->MoveFirst ();	
		}
	}
	pRsClip->Close ();
}

//cheng
//delete 多条记录
void CViewTxsr::EditDel()
{
try
{
	short i;
	short nCountRowforCopy;	
	VARIANT vBookMark;
	
	//得到选中的行数
	try
	{
		nCountRowforCopy = FrmPhsData.m_DBGbill.GetSelBookmarks().GetCount();
		
		vBookMark.vt = VT_I2;
		//FirstRow=(long)FrmPhsData.m_DBGbill.GetFirstRow().dblVal-1792;
		//row=FrmPhsData.m_DBGbill.GetRow();
		//row=row+FirstRow-nCountRowforCopy;

		for(i = 0; i < nCountRowforCopy; i++)
		{       
			//筛除选中的记录
			vBookMark.iVal = i;
			m_ActiveRs->Bookmark = FrmPhsData.m_DBGbill.GetSelBookmarks().GetItem(vBookMark);
			m_ActiveRs->Delete(adAffectCurrent);		
		}
		//adAffectCurrent
		m_ActiveRs->MoveNext();
		m_ActiveRs->MovePrevious();
		//更新记录使控件记录跟着改变
		vBookMark=m_ActiveRs->GetBookmark();
       
		//int scrol=FrmPhsData.m_DBGbill.GetScrollPos(SB_HORZ);
		m_ActiveRs->Requery(-1);
		FrmPhsData.m_DBGbill.SetRefDataSource(m_ActiveRs);
		FrmPhsData.m_DBGbill.ReBind();
    
		EDIBDB::SetColumnsProperty(FrmPhsData.m_DBGbill, EDIBgbl::SelBillType);
	

	//改变控件的标签


	   if(vBookMark.vt!=VT_NULL)
	      m_ActiveRs->Bookmark=vBookMark;
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
		m_ActiveRs->Requery(-1);
		FrmPhsData.m_DBGbill.SetRefDataSource(m_ActiveRs);
		FrmPhsData.m_DBGbill.ReBind();
		EDIBDB::SetColumnsProperty(FrmPhsData.m_DBGbill, EDIBgbl::SelBillType);
	}

	catch(...)
	{
		m_ActiveRs->Requery(-1);
		FrmPhsData.m_DBGbill.SetRefDataSource(m_ActiveRs);
		FrmPhsData.m_DBGbill.ReBind();
		EDIBDB::SetColumnsProperty(FrmPhsData.m_DBGbill, EDIBgbl::SelBillType);
	}

	   //FrmPhsData.m_DBGbill.Scroll(0,FirstRow);
		  
}
catch(_com_error *e)
{
     ShowMessage(e->Description());
}
}

void CViewTxsr::EditPaste()
{
	long j;
	long lno;
	long nCountColforCopy;
    long startColIndex = 3;	//The program haven't copy fields VolumeId, ZDJH and JSDBH1
	long FileNameID; //Add by 程百练
    IDispatch *pCon; //Add by 程百练

	lno = GetMaxZdjh(); 
	nCountColforCopy = FrmPhsData.m_DBGbill.GetColumns ().GetCount () - 3;
	VARIANT vRecord;
	_RecordsetPtr pRsClip;

	pRsClip.CreateInstance (__uuidof(Recordset));
	pRsClip->CursorLocation = adUseServer;
	
	m_bIsAddNew=true;
	
	try
	{
		pRsClip->Open("SELECT * FROM Clip", conPRJDB.GetInterfacePtr(),
			adOpenDynamic, adLockOptimistic, adCmdUnknown);
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.Description());
	}

	try
	{
		Delete1();
		pCon=m_ActiveRs->GetActiveConnection();
		while(!pRsClip->adoEOF && !pRsClip->BOF)
		{	
			lno ++;
			m_ActiveRs->AddNew();
			m_ActiveRs->PutCollect("VolumeID",_variant_t(EDIBgbl::SelVlmID));
			m_ActiveRs->PutCollect("ZDJH",_variant_t(lno));

			for(j = startColIndex; j < startColIndex + nCountColforCopy; j ++)
			{
				vRecord = pRsClip->GetCollect (_variant_t(j));					
				m_ActiveRs->PutCollect (_variant_t(j), _variant_t(vRecord));
				m_ActiveRs->Update();//pfg20050802

			}
            
			//改变粘贴出记录的FileNameID   //cheng
			if(pRsClip->GetCollect("FileNameID").vt!=VT_NULL)
			{   
				FileNameID=(long)pRsClip->GetCollect("FileNameID");
				FileNameID=GetFileNameId(FileNameID,pCon);
				m_ActiveRs->PutCollect("FileNameID",_variant_t(FileNameID));
				m_ActiveRs->Update();//pfg20050802
			}
			
			m_ActiveRs->MoveNext ();
			pRsClip-> MoveNext();
		}
    }
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
		while(!pRsClip->adoEOF && !pRsClip->BOF)
		{
			pRsClip->Delete (adAffectCurrent);
			pRsClip->MoveFirst ();
		}
	}
	FrmPhsData.m_DBGbill.SetRefDataSource(NULL);//pfg20050803
	FrmPhsData.m_DBGbill.SetRefDataSource(m_ActiveRs->GetDataSource());//pfg20050803
	EDIBDB::SetColumnsProperty(FrmPhsData.m_DBGbill, EDIBgbl::SelBillType);//pfg20050803

	m_bIsAddNew = false;
	pRsClip->Close ();
	m_ActiveRs->MovePrevious();
}


//添加一个支吊架
void CViewTxsr::OnBtnAdd() 
{
	// TODO: Add your control notification handler code here
	try
	{
		//update cheng

	    //m_ActiveRs->Requery(-1);
		//FrmPhsData.m_DBGbill.SetRefDataSource(m_ActiveRs);
	//	FrmPhsData.m_DBGbill.ReBind();
	//	EDIBDB::SetColumnsProperty(FrmPhsData.m_DBGbill, EDIBgbl::SelBillType);
	
	   m_bIsAddNew=true;
		long lno=0;
		int C=0;
		UpdateBoundData();
		try
		{
			m_ActiveRs->Filter=_variant_t((long)adFilterNone);
		}
		catch (_com_error &e)
		{
			CString strMsg;
			strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
			AfxMessageBox(strMsg);
		}
		catch(...)
		{
		}
		try
		{
			m_Databill.SetRefRecordset(NULL);
		}
		catch (_com_error &e)
		{
			CString strMsg;
			strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
			AfxMessageBox(strMsg);
		}
		catch(...)
		{
		}
		if(EDIBgbl::SelBillType==EDIBgbl::TZA)
		{
			try
			{
				FrmPhsData.m_DBGbill.SetRefDataSource(NULL);
			}
			catch (_com_error &e)
			{
#ifdef _DEBUG
				CString strMsg;
				strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
				AfxMessageBox(strMsg);
#endif
			}
			catch(...)
			{
			}
		}
		m_popMenu.GetSubMenu(0)->CheckMenuRadioItem(3,12,3,MF_BYPOSITION);
		if(m_ActiveRs->adoEOF && m_ActiveRs->BOF)
		{
			C=0;
			lno=0;
		}
		else
		{
			lno=GetMaxZdjh();
			C=1;
		}
		lno++;
		m_ActiveRs->AddNew();
		m_ActiveRs->PutCollect("VolumeID",_variant_t(EDIBgbl::SelVlmID));
		m_ActiveRs->PutCollect("ZDJH",_variant_t(lno));
		m_ActiveRs->Update();
       

		//update cheng  重新打开记录级,      
	    m_ActiveRs->Requery(-1);
		FrmPhsData.m_DBGbill.SetRefDataSource(m_ActiveRs);
		FrmPhsData.m_DBGbill.ReBind();
		//EDIBDB::SetColumnsProperty(FrmPhsData.m_DBGbill, EDIBgbl::SelBillType);

		m_ActiveRs->MoveLast();
		m_bIsAddNew=false;
		RefreshBoundData();
		LoadStrFLDItem2MyComboBox(m_comboZDJH1, "zdjh");
		LoadStrFLDItem2MyComboBox(m_comboJSJDH, "jsdbh1");
		DatabillReposition();
		m_Databill.SetRefRecordset(m_ActiveRs);
		m_comboZDJH1.SetCurSel(m_comboZDJH1.GetCount()-1);
		m_csZDJH.Format("%d",lno);
		UpdateData(FALSE);
	//	if(EDIBgbl::SelBillType==EDIBgbl::TZA)
	//	{
			FrmPhsData.m_DBGbill.SetRefDataSource(m_ActiveRs->GetDataSource());
			FrmPhsData.m_DBGbill.ReBind();
			EDIBDB::SetColumnsProperty(FrmPhsData.m_DBGbill, EDIBgbl::SelBillType);
	//	}
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
		m_bIsAddNew=false;
	}
	m_bIsAddNew=false;
}

void CViewTxsr::OnDestroy() 
{
	CFormView::OnDestroy();
	
	// TODO: Add your message handler code here
	try
	{
		if (m_ActiveRs->State == adStateOpen)
		{
			m_ActiveRs->Close();
		}
		
		m_ActiveRs.Release();
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}
	catch(...)
	{
	}
	//user::SavePos(this,"FrmDataEdit");
}


void CViewTxsr::ConvertUnit()
{
	
	if(modPHScal::Unit_Force=="kgf")
		m_PagOption->CheckRadioButton(IDC_UNIT_KGF,IDC_UNIT_N,IDC_UNIT_KGF);
	else
		m_PagOption->CheckRadioButton(IDC_UNIT_KGF,IDC_UNIT_N,IDC_UNIT_N);

	CString strFdUnit_Force;
	CString strFdUnit_Momental=vtos(m_ActiveRs->GetCollect("UNIT_Momental"));
	
	if(strFdUnit_Momental!="kgf.m" && strFdUnit_Momental!="N.m")
	{
		strFdUnit_Momental="kgf.m";
		m_ActiveRs->PutCollect("UNIT_Momental",stov(strFdUnit_Momental));
	}
	if( vtos(m_ActiveRs->GetCollect("UNIT_FORCE"))=="" )
	{
		//单位为空，视为kgf
		if( modPHScal::Unit_Force == "kgf" )
		{               
         	m_ActiveRs->PutCollect("UNIT_FORCE",stov(modPHScal::Unit_Force));
			if( strFdUnit_Momental=="N.m" )
			{
				m_ActiveRs->PutCollect("UNIT_Momental","kgf.m");
				m_ActiveRs->PutCollect("mxl1",_variant_t(vtof(m_ActiveRs->GetCollect("mxl1")) / EDIBgbl::kgf2N));
				m_ActiveRs->PutCollect("myl1",_variant_t(vtof(m_ActiveRs->GetCollect("myl1")) / EDIBgbl::kgf2N));
				m_ActiveRs->PutCollect("mzl1",_variant_t(vtof(m_ActiveRs->GetCollect("mzl1")) / EDIBgbl::kgf2N));
				m_ActiveRs->PutCollect("mxr1",_variant_t(vtof(m_ActiveRs->GetCollect("mxr1")) / EDIBgbl::kgf2N));
				m_ActiveRs->PutCollect("myr1",_variant_t(vtof(m_ActiveRs->GetCollect("myr1")) / EDIBgbl::kgf2N));
				m_ActiveRs->PutCollect("mzr1",_variant_t(vtof(m_ActiveRs->GetCollect("mzr1")) / EDIBgbl::kgf2N));
			}
			m_ActiveRs->Update();
		}
		else
		{
			m_ActiveRs->PutCollect("UNIT_FORCE",stov(modPHScal::Unit_Force));
            m_ActiveRs->PutCollect("PAZ1",_variant_t(vtof(m_ActiveRs->GetCollect("PAZ1")) * EDIBgbl::kgf2N));
            m_ActiveRs->PutCollect("PAZ0",_variant_t(vtof(m_ActiveRs->GetCollect("PAZ0")) * EDIBgbl::kgf2N));
            m_ActiveRs->PutCollect("Pgz1",_variant_t(vtof(m_ActiveRs->GetCollect("Pgz1")) * EDIBgbl::kgf2N));
            m_ActiveRs->PutCollect("PJG1",_variant_t(vtof(m_ActiveRs->GetCollect("PJG1")) * EDIBgbl::kgf2N));
            
            m_ActiveRs->PutCollect("pxl1",_variant_t(vtof(m_ActiveRs->GetCollect("pxl1")) * EDIBgbl::kgf2N));
            m_ActiveRs->PutCollect("pyl1",_variant_t(vtof(m_ActiveRs->GetCollect("pyl1")) * EDIBgbl::kgf2N));
            m_ActiveRs->PutCollect("pzl1",_variant_t(vtof(m_ActiveRs->GetCollect("pzl1")) * EDIBgbl::kgf2N));
            m_ActiveRs->PutCollect("pxr1",_variant_t(vtof(m_ActiveRs->GetCollect("pxr1")) * EDIBgbl::kgf2N));
            m_ActiveRs->PutCollect("pyr1",_variant_t(vtof(m_ActiveRs->GetCollect("pyr1")) * EDIBgbl::kgf2N));
            m_ActiveRs->PutCollect("pzr1",_variant_t(vtof(m_ActiveRs->GetCollect("pzr1")) * EDIBgbl::kgf2N));
            m_ActiveRs->Update();
            
			if( strFdUnit_Momental=="kgf.m")
			{
				m_ActiveRs->PutCollect("UNIT_Momental","N.m");
				m_ActiveRs->PutCollect("mxl1",_variant_t(vtof(m_ActiveRs->GetCollect("mxl1")) * EDIBgbl::kgf2N));
				m_ActiveRs->PutCollect("myl1",_variant_t(vtof(m_ActiveRs->GetCollect("myl1")) * EDIBgbl::kgf2N));
				m_ActiveRs->PutCollect("mzl1",_variant_t(vtof(m_ActiveRs->GetCollect("mzl1")) * EDIBgbl::kgf2N));
				m_ActiveRs->PutCollect("mxr1",_variant_t(vtof(m_ActiveRs->GetCollect("mxr1")) * EDIBgbl::kgf2N));
				m_ActiveRs->PutCollect("myr1",_variant_t(vtof(m_ActiveRs->GetCollect("myr1")) * EDIBgbl::kgf2N));
				m_ActiveRs->PutCollect("mzr1",_variant_t(vtof(m_ActiveRs->GetCollect("mzr1")) * EDIBgbl::kgf2N));
			}

		}
	}
	else
	{
		//单位不为空
	//	CString strTestUnitForce = vtos(m_ActiveRs->GetCollect("UNIT_FORCE"));
		if(vtos(m_ActiveRs->GetCollect("UNIT_FORCE")) != modPHScal::Unit_Force )
		{
			//单位不相同时进行单位转换
			if( (modPHScal::Unit_Force == "kgf") )
			{
				m_ActiveRs->PutCollect("UNIT_FORCE",stov(modPHScal::Unit_Force));
				m_ActiveRs->PutCollect("PAZ1",_variant_t(vtof(m_ActiveRs->GetCollect("PAZ1")) / EDIBgbl::kgf2N));
				m_ActiveRs->PutCollect("PAZ0",_variant_t(vtof(m_ActiveRs->GetCollect("PAZ0")) / EDIBgbl::kgf2N));
				m_ActiveRs->PutCollect("Pgz1",_variant_t(vtof(m_ActiveRs->GetCollect("Pgz1")) / EDIBgbl::kgf2N));
				m_ActiveRs->PutCollect("PJG1",_variant_t(vtof(m_ActiveRs->GetCollect("PJG1")) / EDIBgbl::kgf2N));
				
				if( strFdUnit_Momental=="N.m")
				{
					m_ActiveRs->PutCollect("UNIT_Momental","kgf.m");
					m_ActiveRs->PutCollect("mxl1",_variant_t(vtof(m_ActiveRs->GetCollect("mxl1")) / EDIBgbl::kgf2N));
					m_ActiveRs->PutCollect("myl1",_variant_t(vtof(m_ActiveRs->GetCollect("myl1")) / EDIBgbl::kgf2N));
					m_ActiveRs->PutCollect("mzl1",_variant_t(vtof(m_ActiveRs->GetCollect("mzl1")) / EDIBgbl::kgf2N));
					m_ActiveRs->PutCollect("mxr1",_variant_t(vtof(m_ActiveRs->GetCollect("mxr1")) / EDIBgbl::kgf2N));
					m_ActiveRs->PutCollect("myr1",_variant_t(vtof(m_ActiveRs->GetCollect("myr1")) / EDIBgbl::kgf2N));
					m_ActiveRs->PutCollect("mzr1",_variant_t(vtof(m_ActiveRs->GetCollect("mzr1")) / EDIBgbl::kgf2N));
				}
				m_ActiveRs->PutCollect("pxl1",_variant_t(vtof(m_ActiveRs->GetCollect("pxl1")) / EDIBgbl::kgf2N));
				m_ActiveRs->PutCollect("pyl1",_variant_t(vtof(m_ActiveRs->GetCollect("pyl1")) / EDIBgbl::kgf2N));
				m_ActiveRs->PutCollect("pzl1",_variant_t(vtof(m_ActiveRs->GetCollect("pzl1")) / EDIBgbl::kgf2N));
				m_ActiveRs->PutCollect("pxr1",_variant_t(vtof(m_ActiveRs->GetCollect("pxr1")) / EDIBgbl::kgf2N));
				m_ActiveRs->PutCollect("pyr1",_variant_t(vtof(m_ActiveRs->GetCollect("pyr1")) / EDIBgbl::kgf2N));
				m_ActiveRs->PutCollect("pzr1",_variant_t(vtof(m_ActiveRs->GetCollect("pzr1")) / EDIBgbl::kgf2N));
				m_ActiveRs->Update();
			}
			else
			{
				m_ActiveRs->PutCollect("UNIT_FORCE",stov(modPHScal::Unit_Force));
				m_ActiveRs->PutCollect("PAZ1",_variant_t(vtof(m_ActiveRs->GetCollect("PAZ1")) * EDIBgbl::kgf2N));
				m_ActiveRs->PutCollect("PAZ0",_variant_t(vtof(m_ActiveRs->GetCollect("PAZ0")) * EDIBgbl::kgf2N));
				m_ActiveRs->PutCollect("Pgz1",_variant_t(vtof(m_ActiveRs->GetCollect("Pgz1")) * EDIBgbl::kgf2N));
				m_ActiveRs->PutCollect("PJG1",_variant_t(vtof(m_ActiveRs->GetCollect("PJG1")) * EDIBgbl::kgf2N));
				
				if( strFdUnit_Momental=="kgf.m")
				{
					m_ActiveRs->PutCollect("UNIT_Momental","N.m");
					m_ActiveRs->PutCollect("mxl1",_variant_t(vtof(m_ActiveRs->GetCollect("mxl1")) * EDIBgbl::kgf2N));
					m_ActiveRs->PutCollect("myl1",_variant_t(vtof(m_ActiveRs->GetCollect("myl1")) * EDIBgbl::kgf2N));
					m_ActiveRs->PutCollect("mzl1",_variant_t(vtof(m_ActiveRs->GetCollect("mzl1")) * EDIBgbl::kgf2N));
					m_ActiveRs->PutCollect("mxr1",_variant_t(vtof(m_ActiveRs->GetCollect("mxr1")) * EDIBgbl::kgf2N));
					m_ActiveRs->PutCollect("myr1",_variant_t(vtof(m_ActiveRs->GetCollect("myr1")) * EDIBgbl::kgf2N));
					m_ActiveRs->PutCollect("mzr1",_variant_t(vtof(m_ActiveRs->GetCollect("mzr1")) * EDIBgbl::kgf2N));
				}

				m_ActiveRs->PutCollect("pxl1",_variant_t(vtof(m_ActiveRs->GetCollect("pxl1")) * EDIBgbl::kgf2N));
				m_ActiveRs->PutCollect("pyl1",_variant_t(vtof(m_ActiveRs->GetCollect("pyl1")) * EDIBgbl::kgf2N));
				m_ActiveRs->PutCollect("pzl1",_variant_t(vtof(m_ActiveRs->GetCollect("pzl1")) * EDIBgbl::kgf2N));
				m_ActiveRs->PutCollect("pxr1",_variant_t(vtof(m_ActiveRs->GetCollect("pxr1")) * EDIBgbl::kgf2N));
				m_ActiveRs->PutCollect("pyr1",_variant_t(vtof(m_ActiveRs->GetCollect("pyr1")) * EDIBgbl::kgf2N));
				m_ActiveRs->PutCollect("pzr1",_variant_t(vtof(m_ActiveRs->GetCollect("pzr1")) * EDIBgbl::kgf2N));
				m_ActiveRs->Update();
			}
		}
	}
	modPHScal::Unit_Momental=vtos(m_ActiveRs->GetCollect("UNIT_Momental"));
	m_OptPag4->m_labPaz=modPHScal::Unit_Force;
	m_OptPag4->m_labPaz0=modPHScal::Unit_Force;
	m_OptPag4->m_labPgz=modPHScal::Unit_Force;
	m_OptPag4->m_labPjg=modPHScal::Unit_Force;

	m_OptPag5->m_labDx=modPHScal::Unit_Displacement;
	m_OptPag5->m_labDy=modPHScal::Unit_Displacement;
	m_OptPag5->m_labDz=modPHScal::Unit_Displacement;
	m_OptPag5->m_labFx=modPHScal::Unit_Force;
	m_OptPag5->m_labFy=modPHScal::Unit_Force;
	m_OptPag5->m_labFz=modPHScal::Unit_Force;
	m_OptPag5->m_labMx=modPHScal::Unit_Momental;
	m_OptPag5->m_labMy=modPHScal::Unit_Momental;
	m_OptPag5->m_labMz=modPHScal::Unit_Momental;

	m_OptPag6->m_labDx=modPHScal::Unit_Displacement;
	m_OptPag6->m_labDy=modPHScal::Unit_Displacement;
	m_OptPag6->m_labDz=modPHScal::Unit_Displacement;
	m_OptPag6->m_labFx=modPHScal::Unit_Force;
	m_OptPag6->m_labFy=modPHScal::Unit_Force;
	m_OptPag6->m_labFz=modPHScal::Unit_Force;
	m_OptPag6->m_labMx=modPHScal::Unit_Momental;
	m_OptPag6->m_labMy=modPHScal::Unit_Momental;
	m_OptPag6->m_labMz=modPHScal::Unit_Momental;
	
	RefreshOptData();
	m_OptPag4->UpdateData(FALSE);
	m_OptPag5->UpdateData(FALSE);
	m_OptPag6->UpdateData(FALSE);
	m_PagItem->UpdateData(FALSE);
}

void CViewTxsr::RefreshOptData()
{
	try
	{
	if(/*m_ActiveRs->Fields->Item[_variant_t("HAZ")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("HAZ")).vt!=VT_NULL)
		m_OptPag4->m_fHAZ=(float)m_ActiveRs->GetCollect(_variant_t("HAZ"));
	else
		m_OptPag4->m_fHAZ=0;
	if(/*m_ActiveRs->Fields->Item[_variant_t("PAZ1")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("PAZ1")).vt!=VT_NULL)
		m_OptPag4->m_fPAZ1=(float)m_ActiveRs->GetCollect(_variant_t("PAZ1"));
	else
		m_OptPag4->m_fPAZ1=0;
	if(/*m_ActiveRs->Fields->Item[_variant_t("PAZ0")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("PAZ0")).vt!=VT_NULL)
		m_OptPag4->m_fPAZ0=(float)m_ActiveRs->GetCollect(_variant_t("PAZ0"));
	else
		m_OptPag4->m_fPAZ0=0;
	if(/*m_ActiveRs->Fields->Item[_variant_t("PGZ1")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("PGZ1")).vt!=VT_NULL)
		m_OptPag4->m_fPGZ1=(float)m_ActiveRs->GetCollect(_variant_t("PGZ1"));
	else
		m_OptPag4->m_fPGZ1=0;
	if(/*m_ActiveRs->Fields->Item[_variant_t("PJG1")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("PJG1")).vt!=VT_NULL)
		m_OptPag4->m_fPJG1=(float)m_ActiveRs->GetCollect(_variant_t("PJG1"));
	else
		m_OptPag4->m_fPJG1=0;

	if(/*m_ActiveRs->Fields->Item[_variant_t("xr1")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("xr1")).vt!=VT_NULL)
		m_OptPag5->m_fDIlineDispX=(float)m_ActiveRs->GetCollect(_variant_t("xr1"));
	else
		m_OptPag5->m_fDIlineDispX=0;
	if(/*m_ActiveRs->Fields->Item[_variant_t("yr1")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("yr1")).vt!=VT_NULL)
		m_OptPag5->m_fDIlineDispY=(float)m_ActiveRs->GetCollect(_variant_t("yr1"));
	else
		m_OptPag5->m_fDIlineDispY=0;
	if(/*m_ActiveRs->Fields->Item[_variant_t("zr1")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("zr1")).vt!=VT_NULL)
		m_OptPag5->m_fDIlineDispZ=(float)m_ActiveRs->GetCollect(_variant_t("zr1"));
	else
		m_OptPag5->m_fDIlineDispZ=0;

	if(/*m_ActiveRs->Fields->Item[_variant_t("pxr1")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("pxr1")).vt!=VT_NULL)
		m_OptPag5->m_fDIFx=(float)m_ActiveRs->GetCollect(_variant_t("pxr1"));
	else
		m_OptPag5->m_fDIFx=0;
	if(/*m_ActiveRs->Fields->Item[_variant_t("pxr1")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("pyr1")).vt!=VT_NULL)
		m_OptPag5->m_fDIFy=(float)m_ActiveRs->GetCollect(_variant_t("pyr1"));
	else
		m_OptPag5->m_fDIFy=0;
	if(/*m_ActiveRs->Fields->Item[_variant_t("pzr1")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("pzr1")).vt!=VT_NULL)
		m_OptPag5->m_fDIFz=(float)m_ActiveRs->GetCollect(_variant_t("pzr1"));
	else
		m_OptPag5->m_fDIFz=0;

	if(/*m_ActiveRs->Fields->Item[_variant_t("mxr1")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("mxr1")).vt!=VT_NULL)
		m_OptPag5->m_fDImX=(float)m_ActiveRs->GetCollect(_variant_t("mxr1"));
	else
		m_OptPag5->m_fDImX=0;
	if(/*m_ActiveRs->Fields->Item[_variant_t("myr1")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("myr1")).vt!=VT_NULL)
		m_OptPag5->m_fDImY=(float)m_ActiveRs->GetCollect(_variant_t("myr1"));
	else
		m_OptPag5->m_fDImY=0;
	if(/*m_ActiveRs->Fields->Item[_variant_t("mzr1")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("mzr1")).vt!=VT_NULL)
		m_OptPag5->m_fDImZ=(float)m_ActiveRs->GetCollect(_variant_t("mzr1"));
	else
		m_OptPag5->m_fDImZ=0;

	if(/*m_ActiveRs->Fields->Item[_variant_t("rxr1")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("rxr1")).vt!=VT_NULL)
		m_OptPag5->m_fDIRotDispX=(float)m_ActiveRs->GetCollect(_variant_t("rxr1"));
	else
		m_OptPag5->m_fDIRotDispX=0;
	if(/*m_ActiveRs->Fields->Item[_variant_t("ryr1")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("ryr1")).vt!=VT_NULL)
		m_OptPag5->m_fDIRotDispY=(float)m_ActiveRs->GetCollect(_variant_t("ryr1"));
	else
		m_OptPag5->m_fDIRotDispY=0;
	if(/*m_ActiveRs->Fields->Item[_variant_t("rzr1")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("rzr1")).vt!=VT_NULL)
		m_OptPag5->m_fDIRotDispZ=(float)m_ActiveRs->GetCollect(_variant_t("rzr1"));
	else
		m_OptPag5->m_fDIRotDispZ=0;


   //设置冷态数据>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	if(/*m_ActiveRs->Fields->Item[_variant_t("xl1")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("xl1")).vt!=VT_NULL)
		m_OptPag6->m_fDIlineDispX=(float)m_ActiveRs->GetCollect(_variant_t("xl1"));
	else
		m_OptPag6->m_fDIlineDispX=0;
	if(/*m_ActiveRs->Fields->Item[_variant_t("yl1")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("yl1")).vt!=VT_NULL)
		m_OptPag6->m_fDIlineDispY=(float)m_ActiveRs->GetCollect(_variant_t("yl1"));
	else
		m_OptPag6->m_fDIlineDispY=0;
	if(/*m_ActiveRs->Fields->Item[_variant_t("zl1")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("zl1")).vt!=VT_NULL)
		m_OptPag6->m_fDIlineDispZ=(float)m_ActiveRs->GetCollect(_variant_t("zl1"));
	else
		m_OptPag6->m_fDIlineDispZ=0;

	if(/*m_ActiveRs->Fields->Item[_variant_t("pxl1")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("pxl1")).vt!=VT_NULL)
		m_OptPag6->m_fDIFx=(float)m_ActiveRs->GetCollect(_variant_t("pxl1"));
	else
		m_OptPag6->m_fDIFx=0;
	if(/*m_ActiveRs->Fields->Item[_variant_t("pyl1")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("pyl1")).vt!=VT_NULL)
		m_OptPag6->m_fDIFy=(float)m_ActiveRs->GetCollect(_variant_t("pyl1"));
	else
		m_OptPag6->m_fDIFy=0;
	if(/*m_ActiveRs->Fields->Item[_variant_t("pzl1")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("pzl1")).vt!=VT_NULL)
		m_OptPag6->m_fDIFz=(float)m_ActiveRs->GetCollect(_variant_t("pzl1"));
	else
		m_OptPag6->m_fDIFz=0;

	if(/*m_ActiveRs->Fields->Item[_variant_t("mxl1")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("mxl1")).vt!=VT_NULL)
		m_OptPag6->m_fDImX=(float)m_ActiveRs->GetCollect(_variant_t("mxl1"));
	else
		m_OptPag6->m_fDImX=0;
	if(/*m_ActiveRs->Fields->Item[_variant_t("myl1")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("myl1")).vt!=VT_NULL)
		m_OptPag6->m_fDImY=(float)m_ActiveRs->GetCollect(_variant_t("myl1"));
	else
		m_OptPag6->m_fDImY=0;
	if(/*m_ActiveRs->Fields->Item[_variant_t("mzl1")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("mzl1")).vt!=VT_NULL)
		m_OptPag6->m_fDImZ=(float)m_ActiveRs->GetCollect(_variant_t("mzl1"));
	else
		m_OptPag6->m_fDImZ=0;

	if(/*m_ActiveRs->Fields->Item[_variant_t("rxl1")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("rxl1")).vt!=VT_NULL)
		m_OptPag6->m_fDIRotDispX=(float)m_ActiveRs->GetCollect(_variant_t("rxl1"));
	else
		m_OptPag6->m_fDIRotDispX=0;
	if(/*m_ActiveRs->Fields->Item[_variant_t("ryl1")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("ryl1")).vt!=VT_NULL)
		m_OptPag6->m_fDIRotDispY=(float)m_ActiveRs->GetCollect(_variant_t("ryl1"));
	else
		m_OptPag6->m_fDIRotDispY=0;
	if(/*m_ActiveRs->Fields->Item[_variant_t("rzl1")]->Status==adFieldOK &&*/ m_ActiveRs->GetCollect(_variant_t("rzl1")).vt!=VT_NULL)
		m_OptPag6->m_fDIRotDispZ=(float)m_ActiveRs->GetCollect(_variant_t("rzl1"));
	else
		m_OptPag6->m_fDIRotDispZ=0;
	m_OptPag4->UpdateData(false);
	m_OptPag5->UpdateData(false);
	m_OptPag6->UpdateData(false);
	m_PagItem->UpdateData(false);
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}
	catch(...)
	{
	}
}

void CViewTxsr::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
	m_popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x,point.y,this);
	
}

void CViewTxsr::OnSelchangeJSJDH() 
{
	// TODO: Add your control notification handler code here
	try
	{
		if(m_comboJSJDH.GetCount()==0)
			return;
	int ix=m_comboJSJDH.GetCurSel();
	if(ix<0) ix=0;
	CString szdjh;
	m_comboZDJH1.GetLBText(ix,szdjh);
	m_bActive=false;
	UpdateData();
	UpdateBoundData();
	m_ActiveRs->MoveFirst();
	m_ActiveRs->Find(_bstr_t(CString("zdjh=")+szdjh),0,adSearchForward);
	m_bActive=true;
	RefreshBoundData();
	DatabillReposition();
	}
	catch(_com_error e)
	{
		//MessageBox(e.Description());
	}
}

void CViewTxsr::OnGetPrevoiusPhsData() 
{
	// TODO: Add your command handler code here
	try
	{
		if(m_ActiveRs->BOF || m_ActiveRs->adoEOF)
			return;
		_RecordsetPtr bkRs=m_ActiveRs->Clone(adLockReadOnly);
		bkRs->MoveFirst();
		CString strFind;
		strFind.Format("zdjh = %d ",(long)m_ActiveRs->GetCollect("zdjh"));
		bkRs->Find(_bstr_t(strFind),0,adSearchForward);
		bkRs->MovePrevious();
		if(bkRs->BOF)
		{
			bkRs->Close();
			bkRs=NULL;
			return;
		}
		m_ActiveRs->PutCollect("iSelSampleID",bkRs->GetCollect("iSelSampleID")); 
		
      m_ActiveRs->PutCollect("gdw1",bkRs->GetCollect("gdw1"));
      m_ActiveRs->PutCollect("LC1",bkRs->GetCollect("LC1"));
      
      m_ActiveRs->PutCollect("dn1",bkRs->GetCollect("dn1"));
      m_ActiveRs->PutCollect("gn1",bkRs->GetCollect("gn1"));
      
      m_ActiveRs->PutCollect("df1",bkRs->GetCollect("df1"));
      m_ActiveRs->PutCollect("gf1",bkRs->GetCollect("gf1"));
      m_ActiveRs->PutCollect("SPRFitType",bkRs->GetCollect("SPRFitType"));
                  
      m_ActiveRs->PutCollect("dj1",bkRs->GetCollect("dj1"));
      m_ActiveRs->PutCollect("t01",bkRs->GetCollect("t01"));
      
      m_ActiveRs->PutCollect("dh1",bkRs->GetCollect("dh1"));
      m_ActiveRs->PutCollect("gh1",bkRs->GetCollect("gh1"));
      
      m_ActiveRs->PutCollect("dxa1",bkRs->GetCollect("dxa1"));
      m_ActiveRs->PutCollect("dza1",bkRs->GetCollect("dza1"));
      
      m_ActiveRs->PutCollect("xzu1",bkRs->GetCollect("xzu1"));
      m_ActiveRs->PutCollect("xzu2",bkRs->GetCollect("xzu2"));
      m_ActiveRs->PutCollect("xzu3",bkRs->GetCollect("xzu3"));
      m_ActiveRs->PutCollect("xzu4",bkRs->GetCollect("xzu4"));
      m_ActiveRs->PutCollect("xzu5",bkRs->GetCollect("xzu5"));
      m_ActiveRs->PutCollect("xzu6",bkRs->GetCollect("xzu6"));
      m_ActiveRs->PutCollect("xzu7",bkRs->GetCollect("xzu7"));
      m_ActiveRs->PutCollect("xzu8",bkRs->GetCollect("xzu8"));
      
      m_ActiveRs->PutCollect("zzu1",bkRs->GetCollect("zzu1"));
      m_ActiveRs->PutCollect("zzu2",bkRs->GetCollect("zzu2"));
      m_ActiveRs->PutCollect("zzu3",bkRs->GetCollect("zzu3"));
      m_ActiveRs->PutCollect("zzu4",bkRs->GetCollect("zzu4"));
      m_ActiveRs->PutCollect("zzu5",bkRs->GetCollect("zzu5"));
      m_ActiveRs->PutCollect("zzu6",bkRs->GetCollect("zzu6"));
      m_ActiveRs->PutCollect("zzu7",bkRs->GetCollect("zzu7"));
      m_ActiveRs->PutCollect("zzu8",bkRs->GetCollect("zzu8"));
		bkRs->Close();
		bkRs=NULL;
		m_ActiveRs->Update();
		RefreshBoundData();
		DatabillReposition();
	}
	catch(_com_error e)
	{
		//MessageBox(e.Description());
	}
}

void CViewTxsr::PhsCalSelection(int index)
{

	try
	{
		if(EDIBgbl::SelBillType==EDIBgbl::TZA)
		{
			//FrmPhsData.m_DBGbill.SetRefDataSource(m_ActiveRs->GetDataSource());
			FrmPhsData.m_DBGbill.SetRefDataSource(NULL);
		}
		switch(index)
		{
		case iPhsCalSelectionSuccess:
				m_ActiveRs->Filter = "bCalSuccess<>0";
				break;
		case iPhsCalSelectionNotSuccess:
				m_ActiveRs->Filter = "bCalSuccess=0";
				break;
		case iPhsCalSelectionSPR:
				m_ActiveRs->Filter =_variant_t(CString( "(type<>\'9999\' AND type<>\'5000\' AND xr1 <> null AND Upxyz=1   AND  xr1 >") + ftos(modPHScal::gUseSpringMinDist) + ") OR (type<>\'9999\' AND type<>\'5000\' AND xr1 <> null AND Upxyz=1   AND  xr1 < " + ftos(-modPHScal::gUseSpringMinDist) + " ) "
														+CString( " OR (type<>\'9999\' AND type<>\'5000\' AND yr1 <> null AND UPxyz=2   AND  yr1 >") + ftos(modPHScal::gUseSpringMinDist) + ") OR (type<>\'9999\' AND type<>\'5000\' AND yr1 <> null AND UPxyz=2   AND  yr1 < " + ftos(-modPHScal::gUseSpringMinDist) + " ) "
														+CString( " OR (type<>\'9999\' AND type<>\'5000\' AND zr1 <> null AND UPxyz=3   AND  zr1 >") + ftos(modPHScal::gUseSpringMinDist) + ") OR (type<>\'9999\' AND type<>\'5000\' AND zr1 <> null AND UPxyz=3   AND  zr1 < " + ftos(-modPHScal::gUseSpringMinDist) + " ) " );
				break;
		case iPhsCalSelectionSPRAndNotSuccess:
				m_ActiveRs->Filter =_variant_t(CString( "(bCalSuccess=0 AND type<>\'9999\' AND type<>\'5000\' AND xr1 <> null AND Upxyz=1   AND  xr1 >") + ftos(modPHScal::gUseSpringMinDist) + ") OR (bCalSuccess=0 AND type<>\'9999\' AND type<>\'5000\' AND xr1 <> null AND Upxyz=1   AND  xr1 < " + ftos(-modPHScal::gUseSpringMinDist) + " ) " 
														+CString( " OR (bCalSuccess=0 AND type<>\'9999\' AND type<>\'5000\' AND yr1 <> null AND UPxyz=2   AND  yr1 >") + ftos(modPHScal::gUseSpringMinDist) + ") OR (bCalSuccess=0 AND type<>\'9999\' AND type<>\'5000\' AND yr1 <> null AND UPxyz=2   AND  yr1 < " + ftos(-modPHScal::gUseSpringMinDist) + " ) " 
														+CString( " OR (bCalSuccess=0 AND type<>\'9999\' AND type<>\'5000\' AND zr1 <> null AND UPxyz=3   AND  zr1 >") + ftos(modPHScal::gUseSpringMinDist) + ") OR (bCalSuccess=0 AND type<>\'9999\' AND type<>\'5000\' AND zr1 <> null AND UPxyz=3   AND  zr1 < " + ftos(-modPHScal::gUseSpringMinDist) + " ) " );
				break;
		case iPhsCalSelectionSPRAndSuccess:
				m_ActiveRs->Filter =_variant_t(CString( "(bCalSuccess<>0 AND type<>\'9999\' AND type<>\'5000\' AND xr1 <> null AND Upxyz=1   AND  xr1 >") + ftos(modPHScal::gUseSpringMinDist) + ") OR (bCalSuccess<>0 AND type<>\'9999\' AND type<>\'5000\' AND xr1 <> null AND Upxyz=1   AND  xr1 < " + ftos(-modPHScal::gUseSpringMinDist) + " ) " 
														+CString( " OR (bCalSuccess<>0 AND type<>\'9999\' AND type<>\'5000\' AND yr1 <> null AND UPxyz=2   AND  yr1 >") + ftos(modPHScal::gUseSpringMinDist) + ") OR (bCalSuccess<>0 AND type<>\'9999\' AND type<>\'5000\' AND yr1 <> null AND UPxyz=2   AND  yr1 < " + ftos(-modPHScal::gUseSpringMinDist) + " ) " 
														+CString( " OR (bCalSuccess<>0 AND type<>\'9999\' AND type<>\'5000\' AND zr1 <> null AND UPxyz=3   AND  zr1 >") + ftos(modPHScal::gUseSpringMinDist) + ") OR (bCalSuccess<>0 AND type<>\'9999\' AND type<>\'5000\' AND zr1 <> null AND UPxyz=3   AND  zr1 < " + ftos(-modPHScal::gUseSpringMinDist) + " ) " );
				break;
		case iPhsCalSelectionCSPR:
				m_ActiveRs->Filter =_variant_t(CString( "(type=\'9999\' AND xr1 <> null AND Upxyz=1   AND  xr1 >") + ftos(modPHScal::gUseSpringMinDist) + ") OR (type=\'9999\' AND xr1 <> null AND Upxyz=1   AND  xr1 < " + ftos(-modPHScal::gUseSpringMinDist) + " ) " + CString( " OR (type=\'5000\' AND xr1 <> null AND Upxyz=1   AND  xr1 >") + ftos(modPHScal::gUseSpringMinDist) + ") OR (type=\'5000\' AND xr1 <> null AND Upxyz=1   AND  xr1 < " + ftos(-modPHScal::gUseSpringMinDist) + " ) " 
														+CString( " OR (type=\'9999\' AND yr1 <> null AND UPxyz=2   AND  yr1 >") + ftos(modPHScal::gUseSpringMinDist) + ") OR (type=\'9999\' AND yr1 <> null AND UPxyz=2   AND  yr1 < " + ftos(-modPHScal::gUseSpringMinDist) + " ) " + CString( " OR (type=\'5000\' AND yr1 <> null AND UPxyz=2   AND  yr1 >") + ftos(modPHScal::gUseSpringMinDist) + ") OR (type=\'5000\' AND yr1 <> null AND UPxyz=2   AND  yr1 < " + ftos(-modPHScal::gUseSpringMinDist) + " ) " 
														+CString( " OR (type=\'9999\' AND zr1 <> null AND UPxyz=3   AND  zr1 >") + ftos(modPHScal::gUseSpringMinDist) + ") OR (type=\'9999\' AND zr1 <> null AND UPxyz=3   AND  zr1 < " + ftos(-modPHScal::gUseSpringMinDist) + " ) " + CString( " OR (type=\'5000\' AND zr1 <> null AND UPxyz=3   AND  zr1 >") + ftos(modPHScal::gUseSpringMinDist) + ") OR (type=\'5000\' AND zr1 <> null AND UPxyz=3   AND  zr1 < " + ftos(-modPHScal::gUseSpringMinDist) + " ) " );
				break;
		case iPhsCalSelectionCSPRAndNotSuccess:
				m_ActiveRs->Filter =_variant_t(CString( "(bCalSuccess=0 AND type=\'9999\' AND xr1 <> null AND Upxyz=1   AND  xr1 >") + ftos(modPHScal::gUseSpringMinDist) + ") OR (bCalSuccess=0 AND type=\'9999\' AND xr1 <> null AND Upxyz=1   AND  xr1 < " + ftos(-modPHScal::gUseSpringMinDist) + " ) " + CString( " OR (type=\'5000\' AND xr1 <> null AND Upxyz=1   AND  xr1 >") + ftos(modPHScal::gUseSpringMinDist) + ") OR (type=\'5000\' AND xr1 <> null AND Upxyz=1   AND  xr1 < " + ftos(-modPHScal::gUseSpringMinDist) + " ) " 
														+CString( " OR (bCalSuccess=0 AND type=\'9999\' AND yr1 <> null AND UPxyz=2   AND  yr1 >") + ftos(modPHScal::gUseSpringMinDist) + ") OR (bCalSuccess=0 AND type=\'9999\' AND yr1 <> null AND UPxyz=2   AND  yr1 < " + ftos(-modPHScal::gUseSpringMinDist) + " ) " + CString( " OR (type=\'5000\' AND yr1 <> null AND UPxyz=2   AND  yr1 >") + ftos(modPHScal::gUseSpringMinDist) + ") OR (type=\'5000\' AND yr1 <> null AND UPxyz=2   AND  yr1 < " + ftos(-modPHScal::gUseSpringMinDist) + " ) "
														+CString( " OR (bCalSuccess=0 AND type=\'9999\' AND zr1 <> null AND UPxyz=3   AND  zr1 >") + ftos(modPHScal::gUseSpringMinDist) + ") OR (bCalSuccess=0 AND type=\'9999\' AND zr1 <> null AND UPxyz=3   AND  zr1 < " + ftos(-modPHScal::gUseSpringMinDist) + " ) " + CString( " OR (type=\'5000\' AND zr1 <> null AND UPxyz=3   AND  zr1 >") + ftos(modPHScal::gUseSpringMinDist) + ") OR (type=\'5000\' AND zr1 <> null AND UPxyz=3   AND  zr1 < " + ftos(-modPHScal::gUseSpringMinDist) + " ) " );
				break;
		case iPhsCalSelectionCSPRAndSuccess:
				m_ActiveRs->Filter =_variant_t(CString( "(bCalSuccess<>0 AND type=\'9999\' AND xr1 <> null AND Upxyz=1   AND  xr1 >") + ftos(modPHScal::gUseSpringMinDist) + ") OR (bCalSuccess<>0 AND type=\'9999\' AND xr1 <> null AND Upxyz=1   AND  xr1 < " + ftos(-modPHScal::gUseSpringMinDist) + " ) " + CString( " OR (type=\'5000\' AND xr1 <> null AND Upxyz=1   AND  xr1 >") + ftos(modPHScal::gUseSpringMinDist) + ") OR (type=\'5000\' AND xr1 <> null AND Upxyz=1   AND  xr1 < " + ftos(-modPHScal::gUseSpringMinDist) + " ) "
														+CString( " OR (bCalSuccess<>0 AND type=\'9999\' AND yr1 <> null AND UPxyz=2   AND  yr1 >") + ftos(modPHScal::gUseSpringMinDist) + ") OR (bCalSuccess<>0 AND type=\'9999\' AND yr1 <> null AND UPxyz=2   AND  yr1 < " + ftos(-modPHScal::gUseSpringMinDist) + " ) " + CString( " OR (type=\'5000\' AND yr1 <> null AND UPxyz=2   AND  yr1 >") + ftos(modPHScal::gUseSpringMinDist) + ") OR (type=\'5000\' AND yr1 <> null AND UPxyz=2   AND  yr1 < " + ftos(-modPHScal::gUseSpringMinDist) + " ) "
														+CString( " OR (bCalSuccess<>0 AND type=\'9999\' AND zr1 <> null AND UPxyz=3   AND  zr1 >") + ftos(modPHScal::gUseSpringMinDist) + ") OR (bCalSuccess<>0 AND type=\'9999\' AND zr1 <> null AND UPxyz=3   AND  zr1 < " + ftos(-modPHScal::gUseSpringMinDist) + " ) " + CString( " OR (type=\'5000\' AND zr1 <> null AND UPxyz=3   AND  zr1 >") + ftos(modPHScal::gUseSpringMinDist) + ") OR (type=\'5000\' AND zr1 <> null AND UPxyz=3   AND  zr1 < " + ftos(-modPHScal::gUseSpringMinDist) + " ) " );
				break;
		case iPhsCalSelectionNotSPRAndCSPR:
				//ADO的Filter 不能先将由 OR 联接的子句分组然后将该组用 and 联接到其他子句
				m_ActiveRs->Filter = _variant_t(CString("( xr1 = null AND UPXyz=1 ) OR ( xr1 >=0 AND UPXyz=1 AND xr1 <=") + ftos(modPHScal::gUseSpringMinDist) + ")  OR ( xr1 < 0 AND  UPXyz=1 AND xr1 >= " + ftos(-modPHScal::gUseSpringMinDist)+ ")"
														 +CString(" OR ( yr1 = null AND UPXyz=2 ) OR ( yr1 >=0  AND UPXyz=2 AND yr1 <=") + ftos(modPHScal::gUseSpringMinDist) + ") OR ( yr1 < 0 AND  UPXyz=2 AND yr1 >= " + ftos(-modPHScal::gUseSpringMinDist)+ ")" 
														 +CString(" OR ( zr1 = null AND UPXyz=3 ) OR ( zr1 >=0  AND UPXyz=3 AND zr1 <=") + ftos(modPHScal::gUseSpringMinDist) + ") OR ( zr1 < 0 AND  UPXyz=3 AND zr1 >= " + ftos(-modPHScal::gUseSpringMinDist)+ ")"  );
				break;
		case iPhsCalSelectionALL:
				m_ActiveRs->Filter=_variant_t((long)adFilterNone);
				/*if(EDIBgbl::SelBillType==EDIBgbl::TZA)
				{
					FrmPhsData.m_DBGbill.SetRefDataSource(m_ActiveRs->GetDataSource());
					EDIBDB::SetColumnsProperty(FrmPhsData.m_DBGbill, EDIBgbl::SelBillType);
				}*/
				break;
		}
		if(EDIBgbl::SelBillType==EDIBgbl::TZA)
		{
			//FrmPhsData.m_DBGbill.SetRefDataSource(m_ActiveRs->GetDataSource());
			FrmPhsData.m_DBGbill.SetRefDataSource(m_ActiveRs->GetDataSource());
			EDIBDB::SetColumnsProperty(FrmPhsData.m_DBGbill, EDIBgbl::SelBillType);
		}
		//记录指针
		//更新支吊架号和计算点编号组合框
		//LoadStrFLDItem2MyComboBox comboZDJH1, "zdjh"
		//LoadStrFLDItem2MyComboBox comboJSJDH, "jsdbh1"
		//刷新就会丢失子对象
		//Data1.Refresh
		//Exit Sub
		m_Databill.SetRefRecordset(m_ActiveRs);
		m_Databill.SetEnabled(TRUE);
		LoadStrFLDItem2MyComboBox(m_comboZDJH1, "zdjh");
		LoadStrFLDItem2MyComboBox(m_comboJSJDH, "jsdbh1");
	}
	catch(_com_error e)
	{
		MessageBox(e.Description());
	}
}

void CViewTxsr::OnSelectionAll() 
{
	// TODO: Add your command handler code here
	PhsCalSelection(iPhsCalSelectionALL);
	m_popMenu.GetSubMenu(0)->CheckMenuRadioItem(3,12,3,MF_BYPOSITION);
}

void CViewTxsr::OnCalSelectionNotSuccess() 
{
	// TODO: Add your command handler code here
	PhsCalSelection(iPhsCalSelectionNotSuccess);
	m_popMenu.GetSubMenu(0)->CheckMenuRadioItem(3,12,4,MF_BYPOSITION);
}

void CViewTxsr::OnSelectionSuccess() 
{
	// TODO: Add your command handler code here
	PhsCalSelection(iPhsCalSelectionSuccess);
	m_popMenu.GetSubMenu(0)->CheckMenuRadioItem(3,12,5,MF_BYPOSITION);
}

void CViewTxsr::OnSelectionSPR() 
{
	// TODO: Add your command handler code here
	PhsCalSelection(iPhsCalSelectionSPR);
	m_popMenu.GetSubMenu(0)->CheckMenuRadioItem(3,12,6,MF_BYPOSITION);
}

void CViewTxsr::OnSelectionCSPR() 
{
	// TODO: Add your command handler code here
	PhsCalSelection(iPhsCalSelectionCSPR);
	m_popMenu.GetSubMenu(0)->CheckMenuRadioItem(3,12,7,MF_BYPOSITION);
}

void CViewTxsr::OnSelectionNotSPRAndCSPR() 
{
	// TODO: Add your command handler code here
	PhsCalSelection(iPhsCalSelectionNotSPRAndCSPR);
	m_popMenu.GetSubMenu(0)->CheckMenuRadioItem(3,12,8,MF_BYPOSITION);
}

void CViewTxsr::LoadGDWItem2ComboGDW1()
{
/*		//装入根部定位数据到组合框
		CTime timeb,timee;
		timeb=CTime::GetCurrentTime();*/
	try
	{
		for(int nnn=0;nnn<1;nnn++)
		{
		CString ComboGDW1Current;
		CString tbn1;
		modPHScal::gn = vtos(m_ActiveRs->GetCollect("gn1"));
		tbn1 = modPHScal::sFindTBN(modPHScal::gn);
		tbn1="tmpCustomIDGDW1";
		CString sTmp;
		if (tbn1 == "")
		{
			sTmp.Format(GetResStr(IDS_NoRecordFLDEqvThisValueInPictureClipData),"CustomID",modPHScal::gn);
			throw sTmp;
		}
		CString SQLx;
		switch(modPHScal::glClassID)
		{
		case iSACantilever:
		case iSALbrace:
		//	SQLx = "SELECT DISTINCT GDW1 FROM [" + tbn1 + "] WHERE trim(CustomID)=\'" + modPHScal::gn + "\' and Not Isnull(GDW1) ORDER BY GDW1 ASC";//pfg20050218原代码
			SQLx = "SELECT DISTINCT GDW1 FROM [" + tbn1 + "] WHERE trim(CustomID)=\'" + modPHScal::gn + "\' ORDER BY GDW1 ASC";//pfg20050218
			break;
		case iSAbeam:
		//	SQLx = "SELECT DISTINCT GDW1 FROM [" + tbn1 + "] WHERE Not IsNull(GDW1) ORDER BY GDW1 ASC";//pfg20050218原代码
			SQLx = "SELECT DISTINCT GDW1 FROM [" + tbn1 + "] ORDER BY GDW1 ASC";//pfg20050218
			break;
		case iSALbraceFixG47:
		case iSALbraceFixG48:
		//	SQLx = "SELECT DISTINCT GDW1 FROM [" + tbn1 + "]  WHERE Not IsNull(GDW1) ORDER BY GDW1 ASC";//pfg20050218原代码
			SQLx = "SELECT DISTINCT GDW1 FROM [" + tbn1 + "] ORDER BY GDW1 ASC";//pfg20050218
			break;
		default:
			sTmp.Format(GetResStr(IDS_NoNeedGDW1forThisSA), modPHScal::gn);
			throw sTmp;
		}
   
		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(Recordset));
		rs->Open(_variant_t(SQLx),(IDispatch*)EDIBgbl::dbPRJ,adOpenStatic, adLockOptimistic,adCmdText);
		m_comboGDW1.GetWindowText(ComboGDW1Current);
		m_comboGDW1.ResetContent();
		_variant_t vTmp;
		while(!rs->adoEOF)
		{
			rs->get_Collect((_variant_t)"GDW1", &vTmp);
			if(!(vTmp.vt==VT_NULL||vTmp.vt==VT_EMPTY))//PFG20050218
			{
				m_comboGDW1.AddString(vtos(vTmp));
			}
			rs->MoveNext();
		}
		m_comboGDW1.SetWindowText(ComboGDW1Current);
		}
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}
   catch(...)
	{
	}
/*	timee=CTime::GetCurrentTime();
	CTimeSpan t=(timee-timeb);
	CString s=t.Format(_T("%M分%S秒"));
	s=_T("共用时:")+s;
	ShowMessage(s);*/
}

BOOL CViewTxsr::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(IsWindow(m_ctrlHint.m_hWnd))
	{
		MSG msg=*pMsg;
		msg.hwnd=(HWND)(DWORD)m_ctrlHint.SendMessage(TTM_WINDOWFROMPOINT,0,(DWORD)(LPVOID)&msg.pt);
		::ScreenToClient(msg.hwnd,&msg.pt);
		m_ctrlHint.RelayEvent(&msg);
	}
	//TranslateAccelerator(this->m_hWnd,((CAutoPHS2002App*)AfxGetApp())->m_hAccelData,pMsg);
	return CFormView::PreTranslateMessage(pMsg);
}

void CViewTxsr::OnChangeEdtType() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitialUpdate()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
		CString sHint,sTmp,sTmp1;
		GetDlgItem(IDC_EDT_TYPE)->GetWindowText(sTmp);
		if(atoi(sTmp) > 1000 && atoi(sTmp) < 5000)
		{			
			sTmp1.Format("%#2s",Mid(sTmp, 2, 2));
			sHint.Format(GetResStr(IDS_xNumberSpringXserialXparallel),sTmp1,Mid(sTmp, 0, 1),Mid(sTmp, 1, 1));
		}
		else if(atoi(sTmp) < 1000)
		{
			sHint=GetResStr(IDS_RIGIDITYPHS);
		}
		else sHint=GetResStr(IDS_LastingPowerSpringPHS);
		
		m_ctrlHint.UpdateTipText(sHint,GetDlgItem(IDC_EDT_TYPE));
}

void CViewTxsr::OnUpdateEdtType() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitialUpdate()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	
}

void CViewTxsr::OnSetfocusEdtType() 
{
	if(EDIBgbl::IsCalc)//pfg20051021计算时不能执行此消息响应函数
	{
		EDIBgbl::IsCalc=false;
		return;
	}
	static b=true;
	if(b)
	{
		b=false;
		SelSPR();
		b=true;
	}
}

void CViewTxsr::SelSPR()
{
  //选择弹簧型号
	AfxGetApp()->BeginWaitCursor();
	try
	{
		if( m_ActiveRs->adoEOF || m_ActiveRs->BOF ) 
		{
			AfxGetApp()->EndWaitCursor();
			return;
		}
		if(m_bActive)
			UpdateBoundData();
		if(m_ActiveRs->GetCollect("Type").vt!=VT_NULL ) 
			modPHScal::ZdjType = vtos(m_ActiveRs->GetCollect("Type"));
		CString strPSAVER;
		strPSAVER = vtos(m_ActiveRs->GetCollect("PSAVER"));
		
		if(modPHScal::ZdjType.GetLength()>3 || strPSAVER=="")
		{//自动导入应力分析的弹性支吊架或者手工添加的支吊架
			if( giUPxyz == 1 )
			{
				modPHScal::yr = vtof(m_ActiveRs->GetCollect("xr1"));
			}
			else if( giUPxyz == 2 )
			{
				modPHScal::yr = vtof(m_ActiveRs->GetCollect("yr1"));
			}
			else if( giUPxyz == 3 )
			{
				modPHScal::yr = vtof(m_ActiveRs->GetCollect("zr1"));
			}
		}
		else
		{
			modPHScal::yr = vtof(m_ActiveRs->GetCollect("yr1"));
		}
   
		if( modPHScal::yr == 0 )
		{//2 //向上的热位移等于零(start)
			if( modPHScal::ZdjType.GetLength() != 3 )
			{//1
				if( giUPxyz == 1 )
					m_ActiveRs->PutCollect("type","110");
				else if( giUPxyz == 2 )
					m_ActiveRs->PutCollect("type","120");
				else if( giUPxyz == 3 )
					m_ActiveRs->PutCollect("type","130");
				m_ActiveRs->Update();
			}//1
		}//2 //向上的热位移等于零(end)
		else
		{//3 //向上的热位移不等于零(start)
			if( m_ActiveRs->GetCollect("pgz1").vt!=VT_NULL )
			{//2
				modPHScal::pgz = vtof(m_ActiveRs->GetCollect("pgz1")) * modPHScal::CalUnitCovertCoef(m_ActiveRs);
				if( modPHScal::pgz == 0 )
				{//1
					if(modPHScal::Unit_Force=="N")
						modPHScal::pgz =100 * EDIBgbl::kgf2N;
					else
						modPHScal::pgz = 100;
				}//1
			}//2
			else
			{//1				
				if(modPHScal::Unit_Force=="N")
					modPHScal::pgz =100 * EDIBgbl::kgf2N;
				else
					modPHScal::pgz = 100;
			}//1
			if( fabs(modPHScal::yr) > modPHScal::gUseSpringMinDist && modPHScal::pgz != 0 )
			{//1 
			//	modPHScal::CalSpringParallelNum(modPHScal::dn); //计算并联数
				modPHScal::AutoSelSpringNo(m_ActiveRs,modPHScal::pgz/modPHScal::gintParallelNum,(modPHScal::paz)/(modPHScal::gintParallelNum),modPHScal::yr,modPHScal::gnSPR_CHZBH,
					modPHScal::gsngTotalInstallCompressValue,modPHScal::iSelSampleID,modPHScal::iRepZdjType,modPHScal::gSumDistOfSpring,
					modPHScal::bHintRepZdjType,modPHScal::gstrSpringOrder,modPHScal::ZdjType,modPHScal::gbAutoApplyChzbh,modPHScal::tbnSPRINGproperty,modPHScal::tbnDiscSPRINGpropertyMaxDist);
				m_txtType = vtos(m_ActiveRs->GetCollect("type"));
			}//1
		}//3 //向上的热位移不等于零(end)
	}
	catch(_com_error)
	{
	}
	AfxGetApp()->EndWaitCursor();
	UpdateData(false);
}

void CViewTxsr::OnGetZdjhdata() 
{
	// TODO: Add your command handler code here
	//pfg128
	float Tempgdw1=0.0;
	//pfg128
	try
	{
		CSelZdjhDlg dlg;
		dlg.m_rs=m_ActiveRs->Clone(adLockReadOnly);
		if(dlg.DoModal()==IDOK)
		{
			_Recordset* bkRs=dlg.m_rs;
			if(bkRs->BOF || bkRs->adoEOF || bkRs->GetCollect("ZDJH") == m_ActiveRs->GetCollect("ZDJH"))
			{
				bkRs->Close();
				bkRs=NULL;
				return;
			}
			m_ActiveRs->PutCollect("iSelSampleID",bkRs->GetCollect("iSelSampleID")); 
	
			m_ActiveRs->PutCollect("gdw1",bkRs->GetCollect("gdw1"));
			//pfg128
			try
			{
				Tempgdw1=vtof(bkRs->GetCollect("gdw1"));
				if(Tempgdw1==0.0)
				{
					Tempgdw1=200.0;
					m_ActiveRs->PutCollect("gdw1",_variant_t(Tempgdw1));
				}
				
			}
			catch(_com_error e)
			{}
			//psg128
			
			m_ActiveRs->PutCollect("LC1",bkRs->GetCollect("LC1"));
      
			m_ActiveRs->PutCollect("dn1",bkRs->GetCollect("dn1"));
			m_ActiveRs->PutCollect("gn1",bkRs->GetCollect("gn1"));
      
			m_ActiveRs->PutCollect("df1",bkRs->GetCollect("df1"));
			m_ActiveRs->PutCollect("gf1",bkRs->GetCollect("gf1"));
			m_ActiveRs->PutCollect("SPRFitType",bkRs->GetCollect("SPRFitType"));
                  
			m_ActiveRs->PutCollect("dj1",bkRs->GetCollect("dj1"));
			m_ActiveRs->PutCollect("t01",bkRs->GetCollect("t01"));
      
			m_ActiveRs->PutCollect("dh1",bkRs->GetCollect("dh1"));
			m_ActiveRs->PutCollect("gh1",bkRs->GetCollect("gh1"));
      
			m_ActiveRs->PutCollect("dxa1",bkRs->GetCollect("dxa1"));
			m_ActiveRs->PutCollect("dza1",bkRs->GetCollect("dza1"));
      
			m_ActiveRs->PutCollect("xzu1",bkRs->GetCollect("xzu1"));
			m_ActiveRs->PutCollect("xzu2",bkRs->GetCollect("xzu2"));
			m_ActiveRs->PutCollect("xzu3",bkRs->GetCollect("xzu3"));
			m_ActiveRs->PutCollect("xzu4",bkRs->GetCollect("xzu4"));
			m_ActiveRs->PutCollect("xzu5",bkRs->GetCollect("xzu5"));
			m_ActiveRs->PutCollect("xzu6",bkRs->GetCollect("xzu6"));
			m_ActiveRs->PutCollect("xzu7",bkRs->GetCollect("xzu7"));
			m_ActiveRs->PutCollect("xzu8",bkRs->GetCollect("xzu8"));
      
			m_ActiveRs->PutCollect("zzu1",bkRs->GetCollect("zzu1"));
			m_ActiveRs->PutCollect("zzu2",bkRs->GetCollect("zzu2"));
			m_ActiveRs->PutCollect("zzu3",bkRs->GetCollect("zzu3"));
			m_ActiveRs->PutCollect("zzu4",bkRs->GetCollect("zzu4"));
			m_ActiveRs->PutCollect("zzu5",bkRs->GetCollect("zzu5"));
			m_ActiveRs->PutCollect("zzu6",bkRs->GetCollect("zzu6"));
			m_ActiveRs->PutCollect("zzu7",bkRs->GetCollect("zzu7"));
			m_ActiveRs->PutCollect("zzu8",bkRs->GetCollect("zzu8"));
			bkRs->Close();
			bkRs=NULL;
			m_ActiveRs->Update();
			RefreshBoundData();
			DatabillReposition();
		}
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}
	catch(...)
	{
	}
}

long CViewTxsr::GetFirstRow(VARIANT vBookmark)
{
	m_ActiveRs->Bookmark = vBookmark;
	m_ActiveRs->Move(0);
	return m_ActiveRs->GetAbsolutePosition();
}

long CViewTxsr::GetMaxZdjh()
{
	long tmpZdjh=0;
	try
	{
		if(m_ActiveRs->BOF || m_ActiveRs->adoEOF)
			return 0;
		_RecordsetPtr bkRs=m_ActiveRs->Clone(adLockReadOnly);
		bkRs->MoveFirst();
		while(!bkRs->adoEOF)
		{
			if(tmpZdjh < vtoi(bkRs->GetCollect("Zdjh")))
				tmpZdjh= vtoi(bkRs->GetCollect("Zdjh"));
			bkRs->MoveNext();
		}
		bkRs->Close();
		bkRs=NULL;
		return tmpZdjh;
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}
	catch(...)
	{
		return tmpZdjh;
	}
}

//DEL BOOL CViewTxsr::Create(UINT IDD ,CWnd* pParentWnd) 
//DEL {
//DEL 	// TODO: Add your specialized code here and/or call the base class
//DEL 	//CWnd tmpWnd;
//DEL 	//tmpWnd.m_hWnd=NULL;
//DEL 	//if(pParentWnd==NULL) pParentWnd=&tmpWnd;
//DEL 	return CFormView::Create(IDD, NULL);
//DEL }

LRESULT CViewTxsr::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(message==WM_COMMAND && (/*wParam==2 || */wParam==1))
	{
		return TRUE;
	}

	return CFormView::WindowProc(message, wParam, lParam);
}


void CViewTxsr::OnSelectionSPRandNotCal() 
{
	// TODO: Add your command handler code here
	PhsCalSelection(iPhsCalSelectionSPRAndNotSuccess);
	m_popMenu.GetSubMenu(0)->CheckMenuRadioItem(3,12,9,MF_BYPOSITION);	
}

void CViewTxsr::OnSelectionSPRandCal() 
{
	// TODO: Add your command handler code here
	PhsCalSelection(iPhsCalSelectionSPRAndSuccess);
	m_popMenu.GetSubMenu(0)->CheckMenuRadioItem(3,12,10,MF_BYPOSITION);	
	
}

void CViewTxsr::OnSelectionCSPRandNotCal() 
{
	// TODO: Add your command handler code here
	PhsCalSelection(iPhsCalSelectionCSPRAndNotSuccess);
	m_popMenu.GetSubMenu(0)->CheckMenuRadioItem(3,12,11,MF_BYPOSITION);	
	
}

void CViewTxsr::OnSelectionCSPRandCal() 
{
	// TODO: Add your command handler code here
	PhsCalSelection(iPhsCalSelectionCSPRAndSuccess);
	m_popMenu.GetSubMenu(0)->CheckMenuRadioItem(3,12,12,MF_BYPOSITION);	
	
}

CViewTxsr::~CViewTxsr()
{
	try
	{
		m_ActiveRs=NULL;
	}
	catch(...)
	{
	}
	delete m_OptPag4;
	delete m_OptPag5;
	delete m_OptPag6;
	delete m_PagOption;
	delete m_PagItem;
}

void CViewTxsr::AddNewRec()
{
	OnBtnAdd();
}

BOOL CViewTxsr::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
	CRect rc;

	return CFormView::OnSetCursor(pWnd, nHitTest, message);
}


void CViewTxsr::LoadtbnPAItem2ComBox(CComboBox &combo, CString strFLD)
{
   //装入项目数据到组合框
	try
	{
		CString tbn1;	
		combo.ResetContent();	
		if(m_ActiveRs->adoEOF && m_ActiveRs->BOF)
			return;
		if(m_ActiveRs->GetCollect("dn1").vt!=VT_NULL)
			modPHScal::dn = vtos(m_ActiveRs->GetCollect("dn1"));
		else
			modPHScal::dn="";
			
		tbn1 = modPHScal::sFindTBN(modPHScal::dn);
		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(Recordset));
		_variant_t tmpvar;
		if(tbn1=="") 
			tbn1=modPHScal::tbnPA;
		if(modPHScal::dn=="")
			EDIBgbl::SQLx = "SELECT DISTINCT [" + strFLD + "] FROM [" + tbn1 + "] WHERE [" + strFLD + "] IS NOT NULL ORDER BY [" + strFLD + "] ASC";
		else
		{
			EDIBgbl::SQLx = "SELECT COUNT( [" + strFLD + "]) FROM [" + tbn1 + "] WHERE CustomID=\'"+ modPHScal::dn + "\' AND [" + strFLD + "] IS NOT NULL";
			rs->Open((_bstr_t)EDIBgbl::SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
				adOpenStatic, adLockOptimistic, adCmdText); 
			rs->get_Collect((_variant_t)0L, &tmpvar);
			if(vtoi(tmpvar)>0)
				EDIBgbl::SQLx = "SELECT DISTINCT [" + strFLD + "] FROM [" + tbn1 + "] WHERE CustomID=\'"+ modPHScal::dn + "\' AND [" + strFLD + "] IS NOT NULL ORDER BY [" + strFLD + "] ASC";
			else
				EDIBgbl::SQLx = "SELECT DISTINCT [" + strFLD + "] FROM [" + tbn1 + "] WHERE [" + strFLD + "] IS NOT NULL ORDER BY [" + strFLD + "] ASC";
			rs->Close();
		}
		rs->Open((_bstr_t)EDIBgbl::SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
			adOpenStatic, adLockOptimistic, adCmdText); 
		if(rs->adoEOF && rs->BOF) return;
		//获得当前选择值
		CString MyComboBoxCurrent;
		combo.GetWindowText(MyComboBoxCurrent);
		CString sTmp;
		int i=0;
		while(!rs->adoEOF)
		{
			rs->get_Collect((_variant_t)strFLD, &tmpvar);
			sTmp=vtos(tmpvar);
			combo.AddString(sTmp);
			rs->MoveNext();
			i++;
		}
		rs->Close();
		combo.SetWindowText(MyComboBoxCurrent);
		//重新设置选择
		if(i>0)
		{
			i=combo.FindStringExact(-1,MyComboBoxCurrent);
			if(i!=CB_ERR)
				combo.SetCurSel(i);
		}
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}
	catch(...)
	{
	}
}

void CViewTxsr::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
	CFormView::OnClose();
}

void CViewTxsr::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	//CFormView::PostNcDestroy();
}

long GetFileNameId(long FileNameID,IDispatch *pConID)
{
 		//获得文件ID{
	    long FileNameID1;
		CString SourceDataFileName;
		_RecordsetPtr FilenameIdRd,FilenameId1;
		FilenameIdRd.CreateInstance(__uuidof(Recordset));
		FilenameId1.CreateInstance(__uuidof(Recordset));
		CString FSql,FSql1;
		FSql.Format("SELECT * FROM ZY WHERE FileNameID=%ld",FileNameID);
		FilenameId1->Open(_variant_t(FSql),pConID,adOpenDynamic,adLockOptimistic,adCmdText);
	    SourceDataFileName=vtos(FilenameId1->GetCollect("FileName"));
		FilenameId1->Close();
		SourceDataFileName.MakeUpper();
        //FilenameId1->Close();

		FSql="SELECT * FROM ZY ORDER BY FileNameID";	
	    FilenameIdRd->Open(_variant_t(FSql),pConID,adOpenDynamic,adLockOptimistic,adCmdText);
	    FilenameIdRd->MoveLast();
	    FileNameID1=long(FilenameIdRd->GetCollect("FileNameID"))+1;
		FilenameIdRd->Close();
		
		FSql.Format("SELECT * FROM ZY WHERE FileName='%s' AND VolumeID=%ld",SourceDataFileName,EDIBgbl::SelVlmID);
		FilenameIdRd->Open(_variant_t(FSql),pConID,adOpenDynamic,adLockOptimistic,adCmdText);
		if(FilenameIdRd->adoEOF)
		{
			FilenameIdRd->AddNew();
			FilenameIdRd->PutCollect("FileName",_variant_t(SourceDataFileName));
			FilenameIdRd->PutCollect("VolumeID",_variant_t(EDIBgbl::SelVlmID));
			FilenameIdRd->PutCollect("FileNameID",_variant_t(FileNameID1));
		    FilenameIdRd->Update();
		}
	FileNameID1=long(FilenameIdRd->GetCollect("FileNameID"));
    FilenameIdRd->Close();
	return FileNameID1;
}

/*void CViewTxsr::OnZUChange() 
{
	UpdateData();
	_variant_t var;
	var.ChangeType(VT_NULL);
	if( m_txtXZU1.IsEmpty() )
		m_ActiveRs->PutCollect("XZU1",var);
	if( m_txtXZU2.IsEmpty() )
		m_ActiveRs->PutCollect("XZU2",var);
	if( m_txtXZU3.IsEmpty() )
		m_ActiveRs->PutCollect("XZU3",var);
	if( m_txtXZU4.IsEmpty() )
		m_ActiveRs->PutCollect("XZU4",var);
	if( m_txtXZU5.IsEmpty() )
		m_ActiveRs->PutCollect("XZU5",var);
	if( m_txtXZU6.IsEmpty() )
		m_ActiveRs->PutCollect("XZU6",var);
	if( m_txtXZU7.IsEmpty() )
		m_ActiveRs->PutCollect("XZU7",var);
	if( m_txtXZU8.IsEmpty() )
		m_ActiveRs->PutCollect("XZU8",var);

	if( m_txtZZU1.IsEmpty() )
		m_ActiveRs->PutCollect("ZZU1",var);
	if( m_txtZZU2.IsEmpty() )
		m_ActiveRs->PutCollect("ZZU2",var);
	if( m_txtZZU3.IsEmpty() )
		m_ActiveRs->PutCollect("ZZU3",var);
	if( m_txtZZU4.IsEmpty() )
		m_ActiveRs->PutCollect("ZZU4",var);
	if( m_txtZZU5.IsEmpty() )
		m_ActiveRs->PutCollect("ZZU5",var);
	if( m_txtZZU6.IsEmpty() )
		m_ActiveRs->PutCollect("ZZU6",var);
	if( m_txtZZU7.IsEmpty() )
		m_ActiveRs->PutCollect("ZZU7",var);
	if( m_txtZZU8.IsEmpty() )
		m_ActiveRs->PutCollect("ZZU8",var);

}
*/

void CViewTxsr::OnKillfocusEdtSsid() 
{
	// TODO: Add your control notification handler code here
	try
	{
//		CEdit *pSampleID = (CEdit*)GetDlgItem(IDC_EDT_SSID);
//		pSampleID->UpdateData (FALSE);

		UpdateData ();
		CString strTemp;
		long intSampleID=vtoi(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect("iSelSampleID"));
		if(intSampleID != m_lSelSampleID )
		{		//当标准支吊架的模板改变时，删除支吊架原来模板结构，以便重建支吊架的结构，
		
			_RecordsetPtr rsTmpZB;
			rsTmpZB.CreateInstance(__uuidof(Recordset));
			strTemp.Format("SELECT DISTINCT nth FROM ZB WHERE VolumeID = %d AND ZDJH = %d AND bUserAdd <> -1 ",EDIBgbl::SelVlmID ,modPHScal::zdjh );
			rsTmpZB->Open((_bstr_t)strTemp,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
				adOpenStatic, adLockOptimistic, adCmdText); 
			int iCount;
			if(rsTmpZB->BOF && rsTmpZB->adoEOF)
				iCount=0;
			else
			{
				rsTmpZB->MoveLast();
				iCount=rsTmpZB->RecordCount;
			}
			rsTmpZB->Close();
			

			if(iCount <= 1)
			{
				strTemp.Format("DELETE * FROM ZB WHERE VolumeID = %d AND ZDJH = %d AND bUserAdd <> -1 ",EDIBgbl::SelVlmID ,modPHScal::zdjh );
				EDIBgbl::dbPRJDB->Execute((_bstr_t)strTemp, NULL, adCmdText);

				//strTemp.Format("DELETE * FROM Z1 WHERE VolumeID = %d AND ZDJH = %d ",EDIBgbl::SelVlmID ,modPHScal::zdjh );
				//EDIBgbl::dbPRJDB->Execute((_bstr_t)strTemp);
			}

			modPHScal::iSelSampleID=m_lSelSampleID;
			FrmTxsr.m_pViewTxsr->m_ActiveRs->PutCollect("iSelSampleID",_variant_t((long)modPHScal::iSelSampleID));
			FrmTxsr.m_pViewTxsr->m_ActiveRs->Update();
		}
		
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
		return;
	}
	
}



void CViewTxsr::OnPage1EditChange()
{
	_variant_t TempValue;

	try
	{
		TempValue=FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_variant_t("PSAver"));
		
		if(TempValue.vt!=VT_NULL)
		{
			m_OptPag4->UpdateData(FALSE);
			return;
		}
	}
	catch(_com_error &e)
	{
		AfxMessageBox(e.ErrorMessage());
	}

	InitFromReg();

	m_OptPag5->UpdateData();

	m_OptPag5->m_fDIFx=0;
	m_OptPag5->m_fDIFy=0;
	m_OptPag5->m_fDIFz=0;

	m_OptPag4->UpdateData();
	this->UpdateData();
	if(giUPxyz==1)
	{
		m_OptPag5->m_fDIFx=m_OptPag4->m_fPGZ1;
		if(m_fDH1>m_fGH1)
		{
			if(m_OptPag5->m_fDIlineDispY>1e-6)
			{
				m_OptPag5->m_fDIFy=m_OptPag4->m_fPGZ1*m_FrictionFree;
			}

			if(m_OptPag5->m_fDIlineDispZ>1e-6)
			{
				m_OptPag5->m_fDIFz=m_OptPag4->m_fPGZ1*m_FrictionFree;
			}
		}
	}
	else if(giUPxyz==2)
	{
		m_OptPag5->m_fDIFy=m_OptPag4->m_fPGZ1;
		if(m_fDH1>m_fGH1)
		{
			if(m_OptPag5->m_fDIlineDispX>1e-6)
			{
				m_OptPag5->m_fDIFx=m_OptPag4->m_fPGZ1*m_FrictionFree;
			}

			if(m_OptPag5->m_fDIlineDispZ>1e-6)
			{
				m_OptPag5->m_fDIFz=m_OptPag4->m_fPGZ1*m_FrictionFree;
			}
		}
	}
	else if(giUPxyz==3)
	{
		m_OptPag5->m_fDIFz=m_OptPag4->m_fPGZ1;
		if(m_fDH1>m_fGH1)
		{
			if(m_OptPag5->m_fDIlineDispX>1e-6)
			{
				m_OptPag5->m_fDIFx=m_OptPag4->m_fPGZ1*m_FrictionFree;
			}

			if(m_OptPag5->m_fDIlineDispY>1e-6)
			{
				m_OptPag5->m_fDIFy=m_OptPag4->m_fPGZ1*m_FrictionFree;
			}
		}
	}

	m_OptPag5->UpdateData(FALSE);
	return ;
}

void CViewTxsr::OnPage2EditChange()
{
	_variant_t TempValue;
	try
	{
		TempValue=FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_variant_t("PSAver"));
		if(TempValue.vt!=VT_NULL)
		{
			m_OptPag5->UpdateData(FALSE);
			return;
		}
	}
	catch(_com_error &e)
	{
		AfxMessageBox(e.ErrorMessage());
	}

	m_OptPag5->UpdateData();
	if(giUPxyz==1)
	{
		m_OptPag4->m_fPGZ1=m_OptPag5->m_fDIFx;
	}
	else if(giUPxyz==2)
	{
		m_OptPag4->m_fPGZ1=m_OptPag5->m_fDIFy;
	}
	else if(giUPxyz==3)
	{
		m_OptPag4->m_fPGZ1=m_OptPag5->m_fDIFz;
	}
	m_OptPag4->UpdateData(FALSE);

	return ;
}

void CViewTxsr::InitFromReg()
{
	m_FrictionFree=  GetRegKey(_T("Settings"),_T("Friction-free"), m_FrictionFree);
	SetRegValue("Settings","Friction-free",m_FrictionFree);
//	HKEY hKey;
//	long dwErrorCode;
//	DWORD dwDisposition;
//	DWORD dwSize;
//	dwErrorCode=RegCreateKeyEx(HKEY_LOCAL_MACHINE,
//				   _T("SOFTWARE\\长沙优易软件开发有限公司\\AutoPHS\\7.0\\Settings"),//pfg20050609
//				   0,
//				   "",
//				   REG_OPTION_NON_VOLATILE,
//				   KEY_ALL_ACCESS,
//				   NULL,
//				   &hKey,
//				   &dwDisposition);
//
//	if(dwErrorCode!=ERROR_SUCCESS)
//	{
//		return;
//	}
//
//	dwSize=sizeof(m_FrictionFree);
//	dwErrorCode=RegQueryValueEx(hKey,_T("Friction-free"),NULL,NULL,(BYTE*)&this->m_FrictionFree,&dwSize);
//
//	if(dwErrorCode!=ERROR_SUCCESS)
//	{
//		dwErrorCode=::RegSetValueEx(hKey,_T("Friction-free"),0,REG_BINARY,(BYTE*)&m_FrictionFree,sizeof(m_FrictionFree));
//	}
//
//	CloseHandle(hKey);
}

void CViewTxsr::OnButtonSaveJSJDH()
{
	CString strJSJDH;
	//-10000表示没有节点号
	int iJSJDH=-10000;
	m_comboJSJDH.GetWindowText(strJSJDH);
	if(!strJSJDH.IsEmpty())
	{
		iJSJDH=atoi(strJSJDH);
	}
	try
	{
		if(FrmTxsr.m_pViewTxsr->m_ActiveRs!=NULL && FrmTxsr.m_pViewTxsr->m_ActiveRs->State==adStateOpen)
		{
			FrmTxsr.m_pViewTxsr->m_ActiveRs->PutCollect("JSDBH1",_variant_t((long)iJSJDH));
			FrmTxsr.m_pViewTxsr->m_ActiveRs->Update();
		}
	}
	catch(_com_error e)
	{
	}
}

void CViewTxsr::OnKillfocusEdtType() 
{
}

void CViewTxsr::InitBornSA()
{
	UpdateData();
	try
	{
		CString strSQL=_T("");
		_variant_t tmpvar;
		_RecordsetPtr rsBornSA;
		rsBornSA.CreateInstance(__uuidof(Recordset));
		strSQL=_T("select distinct BornSAName from BornSA");
		m_ComBoBornSA.ResetContent();
		rsBornSA->Open((_bstr_t)strSQL,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
			adOpenDynamic, adLockOptimistic, adCmdText); 
		if(rsBornSA->adoEOF && rsBornSA->BOF) return;

		CString strComboCur;
		m_ComBoBornSA.GetWindowText(strComboCur);
		CString sTmp;
		int i=0;
		while(!rsBornSA->adoEOF)
		{
			rsBornSA->get_Collect((_variant_t)"BornSAName", &tmpvar);
			sTmp=vtos(tmpvar);
			m_ComBoBornSA.AddString(sTmp);
			rsBornSA->MoveNext();
			i++;
		}
		rsBornSA->Close();
		m_ComBoBornSA.SetWindowText(strComboCur);
		if(i>0)
		{
			i=m_ComBoBornSA.FindStringExact(-1,strComboCur);
			if(i!=CB_ERR)
				m_ComBoBornSA.SetCurSel(i);
		}
	}
	catch(_com_error e)
	{
	}
}


void CViewTxsr::GetBornSA(CString &m_strBornSA)
{
	UpdateData();
	CString strSQL=_T("");
	_variant_t tmpvar;
	try
	{
		m_strBornSA.TrimLeft();
		m_strBornSA.TrimRight();
		if(m_strBornSA=="") m_strBornSA=" ";
		strSQL.Format("select * from BornSA where BornSAName='%s'",m_strBornSA);
		_RecordsetPtr rsBornSA;
		rsBornSA.CreateInstance(__uuidof(Recordset));
		rsBornSA->Open((_bstr_t)strSQL,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
			adOpenDynamic, adLockOptimistic, adCmdText); 
		if(rsBornSA->adoEOF && rsBornSA->BOF)
		{
			m_strBornSAEng="";
			m_strBornSAEngChina="";
		}
		else
		{
			rsBornSA->get_Collect((_variant_t)"BornSAEngName", &tmpvar);
			m_strBornSAEng=vtos(tmpvar);
			rsBornSA->get_Collect((_variant_t)"BornSAEngChina", &tmpvar);
			m_strBornSAEngChina=vtos(tmpvar);
		}
		rsBornSA->Close();
	}
	catch(_com_error e)
	{
	}
}
