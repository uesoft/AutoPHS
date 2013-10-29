// FrmPreferences.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "FrmPreferences.h"
#include "modPHScal.h"
#include "EDIBgbl.h"
#include "modRegistry.h"
#include "user.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "basDirectory.h"
#include "ModEncrypt.h"
/////////////////////////////////////////////////////////////////////////////
// CFrmPreferences dialog
#define ID_BTN_DEFAULT 5

CFrmPreferences::CFrmPreferences()
{
	AddPage(&m_wndCalPag);
	AddPage(&m_wndDrawPag);
	AddPage(&m_wndPointPag);
	AddPage(&m_wndStylePag);
	//{{AFX_DATA_INIT(CFrmPreferences)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_psh.dwFlags |= PSH_NOAPPLYNOW;

	m_bMove=false;//窗口是否移动
}


//DEL void CFrmPreferences::DoDataExchange(CDataExchange* pDX)
//DEL {
//DEL 	CDialog::DoDataExchange(pDX);
//DEL 	//{{AFX_DATA_MAP(CFrmPreferences)
//DEL 		// NOTE: the ClassWizard will add DDX and DDV calls here
//DEL 	//}}AFX_DATA_MAP
//DEL }


BEGIN_MESSAGE_MAP(CFrmPreferences, CPropertySheet)
	//{{AFX_MSG_MAP(CFrmPreferences)
	ON_BN_CLICKED(ID_BTN_DEFAULT, OnRestoreDefault)
	ON_WM_MOVING()
	ON_BN_CLICKED(IDOK, OnOK)
	ON_WM_MOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFrmPreferences message handlers

void CFrmPreferences::OnOK() 
{
	// TODO: Add extra validation here
	bool FLAG = modPHScal::gbSPRBHLOADUNITBYNEWTON; //保存

	m_wndCalPag.SaveIniValue();
	m_wndDrawPag.SaveIniValue();
	m_wndPointPag.SaveIniValue();
	if(m_wndStylePag.GetActiveTimes() > 1)
	{
		m_wndStylePag.SaveIniValue();
	}
	SetRegValue(_T("Settings"), _T("CSPRAutoLugDia"), modPHScal::gbCSPRAutoLugDia);
	SetRegValue(_T("Settings"), _T("AutoTZ"), modPHScal::AutoOrdinateDrawing);


	SetRegValue(_T("Settings"), _T("CalSAbyConservativenessMethod"), (modPHScal::gbCalSAbyConservativenessMethod));
	SetRegValue(_T("Settings"), _T("CalSAbyPJG"), (modPHScal::gbCalSAbyPJG));
	SetRegValue(_T("Settings"), _T("CalCSPRLugDiaByPjg"), (modPHScal::gbCalCSPRLugDiaByPjg));

	SetRegValue(_T("Settings"), _T("CalPARTbyMaxLoad"), (modPHScal::gbCalPARTbyMaxLoad));
	SetRegValue(_T("Settings"), _T("AddPartWeight2PMax"), (modPHScal::gbAddPartWeight2PMax));
	SetRegValue(_T("Settings"), _T("AddPartWeight2CSPRPgz"), (modPHScal::gbAddPartWeight2CSPRPgz));
	SetRegValue(_T("Settings"), _T("AddPartWeight2SPRPgz"),  (modPHScal::gbAddPartWeight2SPRPgz));//2007.07.30
	SetRegValue(_T("Settings"),_T("PAForceZero"),(modPHScal::gbPAForceZero));//2007.09.25
	SetRegValue(_T("Settings"), _T("MaxPgzPaz"),  (modPHScal::gbMaxPgzPaz));//2007.09.03
	SetRegValue(_T("Settings"), _T("MinMaxDispByActualDisp"), (modPHScal::gbMinMaxDispByActualDisp));


	SetRegValue(_T("Settings"), _T("PhsOutINFOTableType"), (modPHScal::giPhsOutINFOTableType));
	SetRegValue(_T("Settings"), _T("DrawSIZE_A3A4"), (modPHScal::giDrawSIZE_A3A4));
	SetRegValue(_T("Settings"), _T("IsACAD"), (modPHScal::gIsACAD));
	SetRegValue(_T("Settings"), _T("SpringOrder"), (modPHScal::giSpringOrder));
	SetRegValue(_T("Settings"), _T("strSpringOrder"), modPHScal::gstrSpringOrder);
	SetRegValue(_T("Settings"), _T("strSpringOrderSQL"), modPHScal::gstrSpringOrderSQL);
	SetRegValue(_T("Settings"), _T("PhsColor"), modPHScal::gsPhsColor);
	SetRegValue(_T("Settings"), _T("Draw_BOM"), (modPHScal::gbDraw_BOM));
	SetRegValue(_T("Settings"), _T("AnnotateCSPR"), (modPHScal::gbAnnotateCSPR));
	SetRegValue(_T("Settings"), _T("Draw_OVERWRITE"), (modPHScal::gbDrawOVERWRITE));
	SetRegValue(_T("Settings"), _T("WorksWithBoltsNuts"), (modPHScal::gbWorksWithBoltsNuts));
	SetRegValue(_T("Settings"), _T("WorksWithSABoltsNuts"), (modPHScal::gbWorksWithSABoltsNuts));
	SetRegValue(_T("Settings"), _T("DW_delta"), (modPHScal::gnDW_delta));

	SetRegValue(_T("Settings"), _T("ZDJH"), (EDIBgbl::glCurRecordNo));
	SetRegValue(_T("Settings"), _T("DrawNoPreMethod"), (modPHScal::gbDrawNoPreMethod));
	SetRegValue(_T("Settings"), _T("AutoSaveDrawing"), (modPHScal::gbAutoSaveDrawing));
	SetRegValue(_T("Settings"), _T("DrawNoStartNumber"), (modPHScal::glDrawNoStartNumber));
	SetRegValue(_T("Settings"), _T("DwgVersion"), (EDIBgbl::strDwgVersion));
	SetRegValue(_T("Settings"), _T("MaxLGLong"), (EDIBgbl::MaxLGLong));//pfg20050922

	SetRegValue(_T("Settings"), _T("DWGPreMethod"), (modPHScal::glDWGPreMethod));
	SetRegValue(_T("Settings"), _T("DWGPre"), modPHScal::gsDWGPre);
	SetRegValue(_T("Settings"), _T("CSPRCanSpecialDesign"), (modPHScal::gbCSPRCanSpecialDesign));
	SetRegValue(_T("Settings"), _T("LimitLugMinDia"), (modPHScal::gbLimitLugMinDia));

	SetRegValue(_T("Settings"), _T("AutoAlignCSPRtoSA"), (modPHScal::gbAutoAlignCSPRtoSA));
	SetRegValue(_T("Settings"), _T("SumAllVolume"), (modPHScal::gbSumAllVolume));

	
	////////////////////////////
	if(FLAG!=modPHScal::gbSPRBHLOADUNITBYNEWTON)//单位改变时要初始化 
	{									   
		//this->ShowWindow(SW_HIDE);
		//modPHScal::blnSelphsSPEC(true);

		
		COleVariant tmpvar;
		int inttbExist=0;//0表示第一次进入循环，1表示表存在，2表示表不存在
		_RecordsetPtr rsX;
		rsX.CreateInstance(__uuidof(Recordset));
		_ConnectionPtr db, db1;
		db.CreateInstance(__uuidof(Connection));
		db1.CreateInstance(__uuidof(Connection));

		long  lngErrNum ;
		CString sTmp;

// 		db.Open( basDirectory::ProjectDBDir+_T("zdjcrude.mdb"),false,false,_T(";pwd=") + ModEncrypt::gstrDBZdjCrudePassWord);
		db->Open((_bstr_t)("Provider=Microsoft.Jet.OLEDB.4.0;Data Source="+basDirectory::ProjectDBDir+_T("zdjcrude.mdb")),
			(_bstr_t)"", (_bstr_t)ModEncrypt::gstrDBZdjCrudePassWord,adConnectUnspecified);

// 		db1.Open(basDirectory::ProjectDBDir+_T("sort.mdb"));
		db1->Open((_bstr_t)("Provider=Microsoft.Jet.OLEDB.4.0;Data Source="+basDirectory::ProjectDBDir+_T("sort.mdb")),
			(_bstr_t)"", (_bstr_t)"",adConnectUnspecified);

		if(rsX->State == adStateOpen)
			rsX->Close();
// 		rsX.m_pDatabase = &EDIBgbl::dbSORT;
// 		rsX.Open(dbOpenDynaset,_T("SELECT * FROM phsManuSPRING ORDER BY [Observation],[standard] ASC"));//,(IDispatch*)EDIBgbl::dbSORT,adOpenStatic,adLockOptimistic,adCmdText);
		rsX->Open((_bstr_t)_T("SELECT * FROM phsManuSPRING ORDER BY [Observation],[standard] ASC"), _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
			adOpenForwardOnly, adLockReadOnly, adCmdText); 
		modPHScal::gsPhsSPRINGSel.TrimLeft();modPHScal::gsPhsSPRINGSel.TrimRight(); 
		if( rsX->adoEOF && rsX->BOF )
		{
			lngErrNum = IDS_NotFoundThisStandardInSortMdb;
			sTmp.Format(GetResStr(lngErrNum),db1->DefaultDatabase,_T("phsManuBoltsNuts"),modPHScal::gsPhsBoltsNutsSel);
			ShowMessage(sTmp);
			if(rsX->State == adStateOpen)
				rsX->Close();
			
			if(db->State == adStateOpen)
				db->Close();
			
			if(db1->State == adStateOpen)
				db1->Close();
			CPropertySheet::EndDialog(IDOK);
		}
		if(modPHScal::gsPhsSPRINGSel != _T("") ){
			// rsX->Filter=_variant_t(CString(_T("standard=\'") )+ gsPhsPASel + _T("\'"));
			//rsX.FindFirst
			//rsX->Find(_bstr_t(CString(_T("standard=\'"))+gsPhsSPRINGSel+_T("\'")),0,adSearchForward);
			_variant_t vTmp;
			rsX->Find((_bstr_t)(CString(_T("Trim(standard)=\'"))+modPHScal::gsPhsSPRINGSel+_T("\'")), 0, adSearchForward, vTmp);
			if( !rsX->adoEOF)	   
			{
				rsX->MoveFirst();
				inttbExist=2;
			}
		}
		else
		{
			rsX->MoveFirst();
			inttbExist=2;
		}
		while((inttbExist!=1)&&(!rsX->adoEOF))
		{
			rsX->get_Collect((_variant_t)_T("standard"),&tmpvar);
			if(tmpvar.vt==VT_NULL)
				modPHScal::gsPhsSPRINGSel=_T("");
			else{
				modPHScal::gsPhsSPRINGSel=vtos(tmpvar);
				modPHScal::gsPhsSPRINGSel.TrimLeft();modPHScal::gsPhsSPRINGSel.TrimRight();
			}
			rsX->get_Collect((_variant_t)_T("Spring_property"),&tmpvar);
			if(tmpvar.vt==VT_NULL)
				modPHScal::tbnSPRINGproperty=_T("");
			else{
				modPHScal::tbnSPRINGproperty=vtos(tmpvar);
				modPHScal::tbnSPRINGproperty.TrimLeft();modPHScal::tbnSPRINGproperty.TrimRight();
			}
			rsX->get_Collect((_variant_t)_T("Spring_propertyMaxDist"),&tmpvar);
			if(tmpvar.vt==VT_NULL)
				modPHScal::tbnDiscSPRINGpropertyMaxDist=_T("");
			else{
				modPHScal::tbnDiscSPRINGpropertyMaxDist=vtos(tmpvar);
				modPHScal::tbnDiscSPRINGpropertyMaxDist.TrimLeft();modPHScal::tbnDiscSPRINGpropertyMaxDist.TrimRight();
			}
			rsX->get_Collect((_variant_t)_T("Spring_id"),&tmpvar);
			if(tmpvar.vt==VT_NULL)
				modPHScal::tbnSPRINGid=_T("");
			else{
				modPHScal::tbnSPRINGid=vtos(tmpvar);
				modPHScal::tbnSPRINGid.TrimLeft();modPHScal::tbnSPRINGid.TrimRight();
			}
			rsX->get_Collect((_variant_t)_T("Spring_Crude"),&tmpvar);
			if(tmpvar.vt==VT_NULL)
				modPHScal::tbnSPRINGCrude=_T("");
			else{
				modPHScal::tbnSPRINGCrude=vtos(tmpvar);
				modPHScal::tbnSPRINGCrude.TrimLeft();modPHScal::tbnSPRINGCrude.TrimRight();
			}
			rsX->get_Collect((_variant_t)_T("CrudeTurnbuckle"),&tmpvar);
			if(tmpvar.vt==VT_NULL)
				modPHScal::tbnSPRINGL5Crude=_T("");
			else{
				modPHScal::tbnSPRINGL5Crude=vtos(tmpvar);
				modPHScal::tbnSPRINGL5Crude.TrimLeft();modPHScal::tbnSPRINGL5Crude.TrimRight();
			}
			//以下判断弹簧原始数据表在数据库中是否存在
			rsX->get_Collect((_variant_t)_T("DiameterSerial"),&tmpvar);
			if(tmpvar.vt==VT_NULL)
				modPHScal::tbnSPRDiameterSerial=_T("");
			else{
				modPHScal::tbnSPRDiameterSerial=vtos(tmpvar);
				modPHScal::tbnSPRDiameterSerial.TrimLeft();modPHScal::tbnSPRDiameterSerial.TrimRight();
			}
			if(!modPHScal::HStbExists(db,db1,modPHScal::tbnSPRDiameterSerial,_T("phsManuSPRING"),_T("DiameterSerial"),modPHScal::gsPhsSPRINGSel))
			{	
				if(inttbExist==0)	//第一次移到第一条记录上
				{
					rsX->MoveFirst();
					inttbExist=2;
				}
				else					//不是第一次，向后移一条记录
					rsX->MoveNext();
				continue;
			}
			if(!modPHScal::HStbExists(db,db1,modPHScal::tbnSPRINGCrude,_T("phsManuSPRING"),_T("Spring_Crude"),modPHScal::gsPhsSPRINGSel))
			{	
				if(inttbExist==0)	//第一次移到第一条记录上
				{
					rsX->MoveFirst();
					inttbExist=2;
				}
				else					//不是第一次，向后移一条记录
					rsX->MoveNext();
				continue;
			}
			if(!modPHScal::HStbExists(db,db1,modPHScal::tbnSPRINGL5Crude,_T("phsManuSPRING"),_T("CrudeTurnbuckle"),modPHScal::gsPhsSPRINGSel))
			{	
				if(inttbExist==0)	//第一次移到第一条记录上
				{
					rsX->MoveFirst();
					inttbExist=2;
				}
				else					//不是第一次，向后移一条记录
					rsX->MoveNext();
				continue;
			}
			if(!modPHScal::HStbExists(db,db1,modPHScal::tbnSPRINGid,_T("phsManuSPRING"),_T("Spring_id"),modPHScal::gsPhsSPRINGSel))
			{	
				if(inttbExist==0)	//第一次移到第一条记录上
				{
					rsX->MoveFirst();
					inttbExist=2;
				}
				else					//不是第一次，向后移一条记录
					rsX->MoveNext();
				continue;
			}
			if(!modPHScal::HStbExists(db,db1,modPHScal::tbnSPRINGproperty,_T("phsManuSPRING"),_T("Spring_property"),modPHScal::gsPhsSPRINGSel))
			{	
				if(inttbExist==0)	//第一次移到第一条记录上
				{
					rsX->MoveFirst();
					inttbExist=2;
				}
				else					//不是第一次，向后移一条记录
					rsX->MoveNext();
				continue;
			}
			if(!modPHScal::HStbExists(db,db1,modPHScal::tbnDiscSPRINGpropertyMaxDist,_T("phsManuSPRING"),_T("Spring_propertyMaxDist"),modPHScal::gsPhsSPRINGSel))
			{	
				if(inttbExist==0)	//第一次移到第一条记录上
				{
					rsX->MoveFirst();
					inttbExist=2;
				}
				else					//不是第一次，向后移一条记录
					rsX->MoveNext();
				continue;
			}
			inttbExist=1;
	}
	if(inttbExist!=1)
	{
		if(rsX->State == adStateOpen)
			rsX->Close();
		
		if(db->State == adStateOpen)
			db->Close();
		
		if(db1->State == adStateOpen)
			db1->Close();
		CPropertySheet::EndDialog(IDOK);
	}
	inttbExist=0;
	   rsX->get_Collect((_variant_t)_T("SerialNumPerSPR"),&tmpvar);
	   if(tmpvar.vt==VT_NULL)
		   modPHScal::giAllowedMaxSerialNumPerSPR=0;
	   else{
		   modPHScal::giAllowedMaxSerialNumPerSPR=vtoi(tmpvar);
		   //   giAllowedMaxSerialNumPerSPR.TrimLeft();giAllowedMaxSerialNumPerSPR.TrimRight();
	   }
	   rsX->get_Collect((_variant_t)_T("SerialNumSPRs"),&tmpvar);
	   if(tmpvar.vt==VT_NULL)
		   modPHScal::giAllowedMaxSerialNumSPRs=0;
	   else{
		   modPHScal::giAllowedMaxSerialNumSPRs=vtoi(tmpvar);
		   //giAllowedMaxSerialNumSPRs.TrimLeft();giAllowedMaxSerialNumSPRs.TrimRight();
	   }
	   //当导入了规范或者启动过之后
		bool	 bSPECchanged = true;
	   if((bSPECchanged || modPHScal::gbStartedOnceAPHS) && !(modPHScal::gsPhsSPRINGSel == modPHScal::gsOldPhsSPRINGSel))
		   modPHScal::CreateTmpSPRPropertyTable(modPHScal::giAllowedMaxSerialNumSPRs);
	   //获得弹簧或恒力弹簧编号规格的单位制
	   //以下判断恒力弹簧/碟簧规格编号力单位制描述字段值是否存在
	   rsX->get_Collect((_variant_t)_T("Unit_Force"),&tmpvar);
	   if(tmpvar.vt==VT_NULL)
	   {
		   lngErrNum = IDS_NullXfieldInphsManuXOfsortMdb;
		   sTmp.Format(GetResStr(lngErrNum),db1->DefaultDatabase,_T("phsManuSPRING"),modPHScal::gsPhsSPRINGSel ,_T("Unit_Force"));
		   ShowMessage(sTmp);
		   if(rsX->State == adStateOpen)
			   rsX->Close();
		   
		   if(db->State == adStateOpen)
			   db->Close();
		   
		   if(db1->State == adStateOpen)
			   db1->Close();
		   CPropertySheet::EndDialog(IDOK);
	   }
	   else{
		   sTmp=vtos(tmpvar);
		   sTmp.TrimLeft();sTmp.TrimRight();
		   if(sTmp.Left(1)==_T("N"))
		   {
			   if(!modPHScal::gbSPRBHLOADUNITBYNEWTON)
			   {
				   modPHScal::gsSPRUnitOfLoad = _T("kgf");
//				   rsX.Edit();
				   rsX->put_Collect((_variant_t)_T("Unit_Force"),STR_VAR("kgf"));
				   rsX->Update();
				   //如果没有导入，只修改模板库下的原始数据库；
				   //修改方法：
				   rsX->get_Collect((_variant_t)_T("Folder"),tmpvar);
				   
				   if(tmpvar.vt==VT_NULL||vtos(tmpvar)=="")
					   ;
				   else
				   {
					   //如果有导入，还要修改导入路径下的原始数据库；
				   }
			   }
			   else
			   {
				   modPHScal::gsSPRUnitOfLoad = _T("N");
			   }
			   
			   // gsSPRUnitOfLoad = _T("N");
		   }
		   else if(sTmp.Left(3)==_T("daN"))
			   modPHScal::gsSPRUnitOfLoad = _T("daN");
		   else if(sTmp.Left(3)==_T("kgf"))
		   {
			   if(modPHScal::gbSPRBHLOADUNITBYNEWTON)
			   {
				   modPHScal::gsSPRUnitOfLoad = _T("N");
//				   rsX.Edit();
				   rsX->put_Collect((_variant_t)_T("Unit_Force"),STR_VAR("N"));
				   rsX->Update();
				   //如果没有导入，只修改模板库下的原始数据库；
				   //修改方法：
				   rsX->get_Collect((_variant_t)_T("Folder"),&tmpvar);
				   
				   if(tmpvar.vt==VT_NULL||vtos(tmpvar)=="")
					   ;
				   else
				   {
					   //如果有导入，还要修改导入路径下的原始数据库；
				   }
			   }
			   else
			   {
				   modPHScal::gsSPRUnitOfLoad = _T("kgf");
			   }
		   }
		   else
			   //没找任何荷载单位制识别字符
		   {
			   sTmp.Format(GetResStr(IDS_NotFoundUnitStringInBHFormat),db1->DefaultDatabase,_T("phsManuSPRING"),GetResStr(IDS_SPR),vtos(tmpvar));
		   ShowMessage(sTmp);
		   modPHScal::gsSPRUnitOfLoad = _T("daN");
		   }
	   }
	}

	CPropertySheet::EndDialog(IDOK);
}




BOOL CFrmPreferences::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	
	// TODO: Add your specialized code here
	modPHScal::ReadAutoPHSiniValue();
	
	//m_wndPreSheet.ModifyStyleEx(0,WS_EX_CONTROLPARENT);
	//m_wndPreSheet.ModifyStyle(0,WS_TABSTOP);
	SetActivePage(&m_wndCalPag);
	SetActivePage(&m_wndDrawPag);
	SetActivePage(&m_wndPointPag);

	SetActivePage(&m_wndStylePag);
	SetActivePage(&m_wndCalPag);
	SetWindowText(GetResStr(IDS_frmPreferences_frmPreferences));
	//ID_APPLY_NOW
	SetWindowCenter(this->m_hWnd);
	CRect rc1,rc2,rc3;
	GetClientRect(&rc1);
	GetDlgItem(IDOK)->GetWindowRect(&rc2);
	ScreenToClient(&rc2);
	GetDlgItem(IDHELP)->GetWindowRect(&rc3);
	ScreenToClient(&rc3);
	int cx = rc2.left - (rc1.Width() - (rc3.right - rc2.left))/2;

	rc2.left-=cx;
	rc2.right-=cx;
	GetDlgItem(IDOK)->MoveWindow(&rc2);

	GetDlgItem(IDCANCEL)->GetWindowRect(&rc2);
	ScreenToClient(&rc2);
	rc2.left-=cx;
	rc2.right-=cx;
	GetDlgItem(IDCANCEL)->MoveWindow(&rc2);

	GetDlgItem(ID_APPLY_NOW)->GetWindowRect(&rc2);
	ScreenToClient(&rc2);
	rc2.left-=cx;
	rc2.right-=cx;
	GetDlgItem(ID_APPLY_NOW)->MoveWindow(&rc2);

	GetDlgItem(IDHELP)->GetWindowRect(&rc2);
	ScreenToClient(&rc2);
	rc2.left-=cx;
	rc2.right-=cx;
	GetDlgItem(IDHELP)->MoveWindow(&rc2);

	CRect rcBtnDefault;
	GetDlgItem(IDHELP)->GetWindowRect(&rcBtnDefault);
	ScreenToClient(&rcBtnDefault);
	rcBtnDefault.left += 80;
	rcBtnDefault.right += 80;
	CFont* pFont = GetDlgItem(IDHELP)->GetFont();
	m_btnDefault.Create("恢复默认",
		WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_PUSHBUTTON,
		rcBtnDefault,
		this,
		ID_BTN_DEFAULT);
	m_btnDefault.SetFont(pFont);


	
	//m_wndCalPag.SetComboSpringOrder(false);
	/*m_wndCalPag.SetCtrlValue();
	m_wndDrawPag.SetCtrlValue();
	m_wndPointPag.SetCtrlValue();*/
	
	return bResult;
}

void CFrmPreferences::OnRestoreDefault()
{
	CPropertyPage* pActivePage = GetActivePage();
	int indexPage = GetPageIndex(pActivePage);
	
	switch(indexPage)
	{
	case 0:
		((CPreCalPag*)pActivePage)->DoDefaultSetting();
		break;
	case 1:
		((CPreDrawPag*)pActivePage)->DoDefaultSetting();
		break;
	case 2:
		((CPrePointPag*)pActivePage)->DoDefaultSetting();
		break;
    case 3:
		((CPreStylePag*)pActivePage)->DoDefaultSetting();
		break;
	default:
		break;
	};
}


void CFrmPreferences::OnMoving(UINT fwSide, LPRECT pRect) 
{//chengbailian 2003.12.12 移动窗口时获得窗口坐标
	m_Rect.left=pRect->left;
	m_Rect.top=pRect->top;
	m_Rect.right=pRect->right;
	m_Rect.bottom=pRect->bottom;
	m_bMove=true;
	CPropertySheet::OnMoving(fwSide, pRect);
	// TODO: Add your message handler code here
	
}

void CFrmPreferences::OnMove(int x, int y) 
{
	//当标题栏移出屏幕之外，系统不会再移动，所以用MoveWindow()函数
	if(m_bMove)
	   this->MoveWindow(m_Rect,true);
	
}
