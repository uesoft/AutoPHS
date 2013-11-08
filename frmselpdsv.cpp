// FrmSelPDSV.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "FrmSelPDSV.h"
#include "user.h"
#include "EDIBgbl.H"
#include "modSelPdSV.h"
#include "edibdb.H"
#include "MODREGISTRY.H"
#include "basDirectory.h"
#include "Columns.h"
#include "Column.h"
#include "PhsData.h"
//#include "_recordset.h"
//#include "Field.h"
//#include "Fields.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFrmSelPDSV dialog

CFrmSelPDSV frmSelPDSV;
CFrmSelPDSV::CFrmSelPDSV(CWnd* pParent /*=NULL*/)
	: CDialog(CFrmSelPDSV::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFrmSelPDSV)
	m_csDBDsgn = _T("");
	m_csDBSpec = _T("");
	m_csDBCategory = _T("");
	//}}AFX_DATA_INIT
	m_bIsNew=false;
	m_bIsVlmNew=false;
}


void CFrmSelPDSV::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFrmSelPDSV)
	DDX_Control(pDX, IDC_DBGENG, m_DBGeng);
	DDX_Control(pDX, IDC_DBGVIM, m_DBGvlm);
	DDX_Control(pDX, IDC_COMBO_DSGN, m_DBComboDsgn);
	DDX_Control(pDX, IDC_COMBO_SPEC, m_DBComboSpec);
	DDX_Control(pDX, IDC_COMBO_CATEGORY, m_DBComboCategory);
	DDX_CBStringExact(pDX, IDC_COMBO_DSGN, m_csDBDsgn);
	DDX_CBStringExact(pDX, IDC_COMBO_SPEC, m_csDBSpec);
	DDX_CBStringExact(pDX, IDC_COMBO_CATEGORY, m_csDBCategory);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFrmSelPDSV, CDialog)
	//{{AFX_MSG_MAP(CFrmSelPDSV)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_CBN_EDITCHANGE(IDC_COMBO_CATEGORY, OnEditchangeComboHY)
	ON_CBN_SELCHANGE(IDC_COMBO_CATEGORY, OnSelchangeComboHY)
	ON_CBN_SELCHANGE(IDC_COMBO_SPEC, OnSelchangeComboSpe)
	ON_CBN_SELCHANGE(IDC_COMBO_DSGN, OnSelchangeComboDsgn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFrmSelPDSV message handlers

BOOL CFrmSelPDSV::OnInitDialog() 
{
	CDialog::OnInitDialog();
	::SetWindowCenter(this->m_hWnd);
	if( EDIBgbl::SelPrjID == _T("")  ) EDIBgbl::SelPrjID = modSelPDSV::defPrjID;
	if( EDIBgbl::SelSpecID <= 0 ) EDIBgbl::SelSpecID = modSelPDSV::defSpecID;
	if( EDIBgbl::SelDsgnID <= 0 ) EDIBgbl::SelDsgnID = modSelPDSV::defDsgnID;
	if( EDIBgbl::SelHyID < 0 ) EDIBgbl::SelHyID = modSelPDSV::defHyID;
	m_SelPrjID=EDIBgbl::SelPrjID;
	m_SelSpecID=EDIBgbl::SelSpecID;
	m_SelDsgnID=EDIBgbl::SelDsgnID;
	m_SelHyID=EDIBgbl::SelHyID;
	this->InitENG();
	this->InitDBVlm();
	this->initPrjDb();
	m_DBGeng.SetAllowRowSizing(false);
	m_DBGvlm.SetAllowRowSizing(false);
	//m_SelZyID=EDIBgbl::SelZyID;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFrmSelPDSV::initPrjDb()
{
	_variant_t tmpvar;
	COleVariant v;
	CString SQLx;
	try{
//   		m_DataCurrWork.m_pDatabase=&EDIBgbl::dbPRJ;
// 		m_DataCurrWork.Open(dbOpenDynaset,_T("Select * From CurrentWork"));
		SQLx = _T("Select * From CurrentWork");
		m_DataCurrWork->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
			adOpenKeyset, adLockOptimistic, adCmdText); 
		
		//首先必须确定行业
// 		m_DataCategory.m_pDatabase=&EDIBgbl::dbDSize;//20071101 "dbSORT" 改为 "dbDSize"
// 		m_DataCategory.Open(dbOpenSnapshot,_T("SELECT * FROM DrawSize"));
		SQLx = _T("SELECT * FROM DrawSize");
		m_DataCategory->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbDSize,true), 
			adOpenKeyset, adLockOptimistic, adCmdText); 
		m_DataCategory->MoveFirst();
		//设计行业/公司
		LoadDBComboCategory();
// 		m_DataCategory.FindFirst(_T("sjhyid= ") + ltos(m_SelHyID));
		m_DataCategory->MoveFirst();
		HRESULT hr = S_OK;
		CString strFind;
		strFind = _T("sjhyid= ") + ltos(m_SelHyID);
		hr = m_DataCategory->Find((_bstr_t)strFind, 0, adSearchBackward, m_DataCategory->Bookmark);

// 		m_DataDsgn.m_pDatabase=&EDIBgbl::dbDSize;//20071101 "dbSORT" 改为 "dbDSize"
// 		m_DataDsgn.Open(dbOpenDynaset,_T("SELECT * FROM DesignStage WHERE sjhyid=") + ltos(m_SelHyID));
		SQLx = _T("SELECT * FROM DesignStage WHERE sjhyid=") + ltos(m_SelHyID);
		m_DataDsgn->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbDSize,true), 
			adOpenKeyset, adLockOptimistic, adCmdText); 

// 		m_DataSpe.m_pDatabase=&EDIBgbl::dbDSize;//20071101 "dbSORT" 改为 "dbDSize"
// 		m_DataSpe.Open(dbOpenDynaset,_T("SELECT * FROM Speciality WHERE sjhyid=") + ltos(m_SelHyID));
		SQLx = _T("SELECT * FROM Speciality WHERE sjhyid=") + ltos(m_SelHyID);
		m_DataSpe->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbDSize,true), 
			adOpenKeyset, adLockOptimistic, adCmdText); 

		//设计阶段
		LoadDBComboDsgn();
		//专业
		LoadDBComboSpec();
		COleVariant v;
		if (m_SelDsgnID <= 0 )
		{
			//默认:电力行业施工图设计阶段
			m_SelDsgnID=4;
		}
		m_DataDsgn->MoveFirst();

//		m_DataDsgn.FindFirst(_T("sjjdID= ")+ ltos(m_SelDsgnID));
		strFind = _T("sjjdID= ")+ ltos(m_SelDsgnID);
		hr = m_DataCategory->Find((_bstr_t)strFind, 0, adSearchBackward, m_DataCategory->Bookmark);
   
		if(m_SelSpecID <= 0 )
		{
			//默认:电力行业热机专业
			m_SelSpecID=3;
		}
		m_DataSpe->MoveFirst();
// 		m_DataSpe.FindFirst(_T("zyid= ")+ ltos(m_SelSpecID));
		strFind = _T("zyid= ")+ ltos(m_SelSpecID);
		hr = m_DataCategory->Find((_bstr_t)strFind, 0, adSearchBackward, m_DataCategory->Bookmark);

		this->m_DBComboCategory.RefLst();
		this->m_DBComboDsgn.RefLst();
		this->m_DBComboSpec.RefLst();
   }
   catch(CException *e)
	{
		e->ReportError();
		e->Delete();
   }
   catch(_com_error e)
   {
	   ShowMessage(e.Description());
   }
}

void CFrmSelPDSV::InitENG()
{
   //定位到前次工作的工程	
	try
	{
		m_DataEng.CreateInstance(__uuidof(Recordset));
		m_DataEng->CursorLocation=adUseClient;

		m_DataEng->Open(_bstr_t(_T("Select * From engin")),(IDispatch*)conPRJDB,adOpenStatic,adLockOptimistic,adCmdText);

		CString sTmp=CString(_T("EnginID = \'"))+Trim(m_SelPrjID)+_T("\'");

		m_DataEng->Find(_bstr_t(sTmp),0,adSearchBackward);
		if(m_DataEng->adoEOF)
		{
			m_DataEng->MoveLast();
		}
		m_SelPrjID=vtos(m_DataEng->GetCollect(_T("EnginID")));
		m_DBGeng.SetRefDataSource(m_DataEng->GetDataSource());
		m_DBGeng.SetAllowAddNew(true);
		m_DBGeng.SetAllowDelete(true);
		m_DBGeng.SetAllowUpdate(true);
		m_DBGeng.SetForeColor(0xff0000);
		m_DBGeng.SetBackColor(0xffc0c0);
		m_DBGeng.GetColumns().GetItem(_variant_t(_T("EnginID"))).SetCaption(GetResStr(IDS_PRJ_NO));
		EDIBgbl::SetDBGridColumnCaptionAndWidth(m_DBGeng, _T("teng"));
	}
	catch(CException *e)
	{
		e->Delete();
	}
}

void CFrmSelPDSV::InitDBVlm()
{
	try{
		
		//EDIBgbl::TBNSelPrjSpec = EDIBgbl::SelPrjID + EDIBgbl::SelDsgnID + _T("-") + EDIBgbl::SelSpecID;
   
		_variant_t tmpvar;     
	   m_DataVlm.CreateInstance(__uuidof(Recordset));
	   m_DataVlm->CursorLocation=adUseClient;
		CString tmpSQL=CString(_T("SELECT *   FROM [Volume] WHERE EnginID ")) +(m_SelPrjID ==_T("") ? CString(_T(" IS NULL ")) : CString(_T(" = \'")) + m_SelPrjID + CString(_T("\' "))) ;
		tmpSQL +=_T(" AND SJHYID=") + ltos(EDIBgbl::SelHyID);
		tmpSQL +=_T(" AND SJJDID=") + ltos(EDIBgbl::SelDsgnID);
		tmpSQL +=_T(" AND ZYID=") + ltos(EDIBgbl::SelSpecID);
		tmpSQL +=_T(" ORDER BY jcdm ");
	   m_DataVlm->Open(_bstr_t(tmpSQL),(IDispatch*)conPRJDB,adOpenStatic,adLockOptimistic,adCmdText);
		m_DataVlm->Find(_bstr_t(CString(_T("VolumeID=") )+ ltos(EDIBgbl::SelVlmID)),0,adSearchBackward);
		if(m_DataVlm->adoEOF)
		{
			if(m_DataVlm->BOF)
			{
				long maxvid=GetMaxVolumeID()+1;
				m_DataVlm->AddNew();
				m_DataVlm->PutCollect(_T("VolumeID"),_variant_t(maxvid));
				m_DataVlm->PutCollect(_T("EnginID"),_variant_t(m_SelPrjID));
				m_DataVlm->PutCollect(_T("jcdm"),_variant_t(EDIBgbl::SelJcdm));
				m_DataVlm->PutCollect(_T("SJHYID"),_variant_t(EDIBgbl::SelHyID));
				m_DataVlm->PutCollect(_T("SJJDID"),_variant_t(EDIBgbl::SelDsgnID));
				m_DataVlm->PutCollect(_T("ZYID"),_variant_t(EDIBgbl::SelSpecID));
				m_DataVlm->Update();
			}
			else
				m_DataVlm->MoveLast();
		}
		this->m_SelDsgnID=vtoi(m_DataVlm->GetCollect(_T("SJJDID")));
		this->m_SelSpecID=vtoi(m_DataVlm->GetCollect(_T("ZYID")));
		this->m_SelHyID=vtoi(m_DataVlm->GetCollect(_T("SJHYID")));
		//m_DataVlm.GetRecordset().Find(_bstr_t(CString(_T("jcdm=\'") )+ Trim(EDIBgbl::SelVlmID) + _T("\'")),0,adSearchBackward,_variant_t((long)0));
      //定位到前次工作的卷册
		m_DBGvlm.SetRefDataSource(m_DataVlm->GetDataSource());
		m_DBGvlm.SetAllowAddNew(true);
		m_DBGvlm.SetAllowDelete(true);
		m_DBGvlm.SetAllowUpdate(true);
		m_DBGvlm.SetForeColor(0xff0000);
		m_DBGvlm.SetBackColor(0xffc0c0);
		m_DBGvlm.GetColumns().GetItem(_variant_t(_T("EnginID"))).SetVisible(FALSE);
		m_DBGvlm.GetColumns().GetItem(_variant_t(_T("VolumeID"))).SetVisible(FALSE);
		m_DBGvlm.GetColumns().GetItem(_variant_t(_T("SJJDID"))).SetVisible(FALSE);
		m_DBGvlm.GetColumns().GetItem(_variant_t(_T("SJHYID"))).SetVisible(FALSE);
		m_DBGvlm.GetColumns().GetItem(_variant_t(_T("ZYID"))).SetVisible(FALSE);
		EDIBgbl::SetDBGridColumnCaptionAndWidth (m_DBGvlm, _T("tvlm"));

	}
	catch(_com_error e)
	{
		ShowMessage(e.Description());
	}
	catch(CException *e)
	{
		e->Delete();
	}
}


void CFrmSelPDSV::LoadDBComboDsgn()
{
//设计阶段
	try
	{
		m_DBComboDsgn.m_Rs=m_DataCurrWork;
		m_DBComboDsgn.m_RowRs=m_DataDsgn;
		m_DBComboDsgn.m_Field=_T("sjjddm");
		m_DBComboDsgn.m_BoundField=_T("sjjddm");
		m_DBComboDsgn.m_ListField=_T("sjjdmc");
		m_DBComboDsgn.LoadList();
	}
	catch(_com_error &e)
	{
		ShowMessage(e.Description());
	}
}

void CFrmSelPDSV::LoadDBComboSpec()
{
//设计专业
	try{
		m_DBComboSpec.m_Rs=m_DataCurrWork;
		m_DBComboSpec.m_RowRs=m_DataSpe;
		m_DBComboSpec.m_Field=_T("zydm");
		m_DBComboSpec.m_BoundField=_T("zydm");
		m_DBComboSpec.m_ListField=_T("zymc");
		m_DBComboSpec.LoadList();
	}
	catch(_com_error e)
	{
		ShowMessage(e.Description());
	}
}

void CFrmSelPDSV::LoadDBComboCategory()
{
	//设计行业/公司
   //DBComboCategory.RowSource = DataCategory
   /*DBComboCategory.ListField = _T("SJHY")
   DBComboCategory.BoundColumn = _T("SJHYIndex")
   //DBComboCategory.DataSource = DataCurrWork
   DBComboCategory.DataField = _T("SJHYIndex")
   //DBComboCategory.MatchEntry = dblExtendedMatching
   DBComboCategory.Refresh
   DBComboCategory.ReFill*/
	try
	{
		m_DBComboCategory.m_Rs=m_DataCurrWork;
		m_DBComboCategory.m_RowRs=m_DataCategory;
		m_DBComboCategory.m_Field=_T("SJHYINDEX");
		m_DBComboCategory.m_BoundField=_T("SJHYID");
		m_DBComboCategory.m_ListField=_T("SJHY");
		m_DBComboCategory.LoadList();
	}
	catch(_com_error e)
	{
		ShowMessage(e.Description());
	}
}

void CFrmSelPDSV::SetComboItem(CDBComboBox & bc,char *sItem)
{
	if(sItem==NULL)
		return;
	CString ss=sItem;
	if(ss==_T(""))
		return;
	int ix=bc.FindStringExact(-1,ss);
	if(ix>=0)
		bc.SetCurSel(ix);
	bc.OnSelchange();
}

BEGIN_EVENTSINK_MAP(CFrmSelPDSV, CDialog)
    //{{AFX_EVENTSINK_MAP(CFrmSelPDSV)
	ON_EVENT(CFrmSelPDSV, IDC_DBGVIM, -601 /* DblClick */, OnDblClickDbgvim, VTS_NONE)
	ON_EVENT(CFrmSelPDSV, IDC_DBGENG, 207 /* BeforeDelete */, OnBeforeDeleteDbgeng, VTS_PI2)
//	ON_EVENT(CFrmSelPDSV, IDC_DBGENG, 209 /* BeforeUpdate */, OnBeforeUpdateDbgeng, VTS_PI2)
	ON_EVENT(CFrmSelPDSV, IDC_DBGENG, -601 /* DblClick */, OnDblClickDbgeng, VTS_NONE)
	ON_EVENT(CFrmSelPDSV, IDC_DBGVIM, 218 /* RowColChange */, OnRowColChangeDbgvim, VTS_PVARIANT VTS_I2)
	ON_EVENT(CFrmSelPDSV, IDC_DBGENG, 218 /* RowColChange */, OnRowColChangeDbgeng, VTS_PVARIANT VTS_I2)
	ON_EVENT(CFrmSelPDSV, IDC_DBGENG, 213 /* ColResize */, OnColResizeDbgeng, VTS_I2 VTS_PI2)
	ON_EVENT(CFrmSelPDSV, IDC_DBGVIM, 213 /* ColResize */, OnColResizeDbgvim, VTS_I2 VTS_PI2)
//	ON_EVENT(CFrmSelPDSV, IDC_DBGENG, 201 /* AfterColUpdate */, OnAfterColUpdateDbgeng, VTS_I2)
	ON_EVENT(CFrmSelPDSV, IDC_DBGVIM, 209 /* BeforeUpdate */, OnBeforeUpdateDbgvim, VTS_PI2)
	ON_EVENT(CFrmSelPDSV, IDC_DBGVIM, 217 /* OnAddNew */, OnOnAddNewDbgvim, VTS_NONE)
	ON_EVENT(CFrmSelPDSV, IDC_DBGENG, 217 /* OnAddNew */, OnOnAddNewDbgeng, VTS_NONE)
	ON_EVENT(CFrmSelPDSV, IDC_DBGENG, 204 /* AfterUpdate */, OnAfterUpdateDbgeng, VTS_NONE)
	ON_EVENT(CFrmSelPDSV, IDC_DBGVIM, 204 /* AfterUpdate */, OnAfterUpdateDbgvim, VTS_NONE)
	ON_EVENT(CFrmSelPDSV, IDC_DBGVIM, 207 /* BeforeDelete */, OnBeforeDeleteDbgvim, VTS_PI2)
//	ON_EVENT(CFrmSelPDSV, IDC_DBGVIM, -600 /* Click */, OnClickDbgvim, VTS_NONE)
//	ON_EVENT(CFrmSelPDSV, IDC_DBGENG, 219 /* RowResize */, OnRowResizeDbgeng, VTS_PI2)
//	ON_EVENT(CFrmSelPDSV, IDC_DBGVIM, 202 /* AfterDelete */, OnAfterDeleteDbgvim, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()


void CFrmSelPDSV::OnDblClickDbgvim() 
{
	// TODO: Add your control notification handler code here
	try
	{
		if(m_DataVlm->adoEOF && m_DataVlm->BOF)
			return;
		int i=m_DBGvlm.GetAddNewMode();
		if(i!=0) return;
		//if(m_bIsVlmNew)
		//	return;
		m_DataVlm->Update();
		AfxGetApp()->BeginWaitCursor();
		m_DBGeng.SetRefDataSource(NULL);
		m_DBGvlm.SetRefDataSource(NULL);
		SelToCurrWork();
		this->SendMessage(WM_CLOSE);
		//this->CloseWindow();
		FrmPhsData.ShowWindow(SW_SHOW);
		AfxGetApp()->EndWaitCursor();
		CString ss=EDIBgbl::gsTitle+(EDIBgbl::TBNSelPrjSpec == _T("") ? _T("") : (_T("    ") + EDIBgbl::TBNSelPrjSpec + EDIBgbl::Btype[EDIBgbl::SelBillType] + _T(" (") + EDIBgbl::SelJcdm + _T(")")));
		AfxGetApp()->m_pMainWnd->SetWindowText(ss);
	}
	catch(_com_error e)
	{
		ShowMessage(e.Description());
	}
	catch(CException *e)
	{
		e->Delete();
	}
}

void CFrmSelPDSV::SelToCurrWork()
{
   //首先要更新数据控件
	try
	{
		CMenu* men=this->GetMenu();
		men->EnableMenuItem(0,MF_ENABLED);
		//mnuSelBillType.Enabled = True
		COleVariant tmpVar;

		m_DataDsgn->get_Collect((_variant_t)_T("SJJDID"),&tmpVar);
		this->m_SelDsgnID=vtoi(tmpVar);
		m_DataVlm->PutCollect(_T("SJJDID"),(VARIANT)tmpVar);

		m_DataSpe->get_Collect((_variant_t)_T("ZYID"),&tmpVar);
		this->m_SelSpecID=vtoi(tmpVar);
		m_DataVlm->PutCollect(_T("ZYID"),(VARIANT)tmpVar);

		this->m_DataCategory->get_Collect((_variant_t)_T("SJHYID"),&tmpVar);
		this->m_SelHyID=vtoi(tmpVar);
		m_DataVlm->PutCollect(_T("SJHYID"),(VARIANT)tmpVar);
		
		this->m_DataCategory->get_Collect((_variant_t)_T("SJHYIndex"),&tmpVar);
		EDIBDB::SJHYIndex=vtoi(tmpVar);
		EDIBgbl::SelHyID=m_SelHyID;
		EDIBgbl::SelPrjID =m_SelPrjID;
		EDIBgbl::SelPrjName =vtos(this->m_DataEng->GetCollect(_T("gcmc")));
		EDIBgbl::SelPrjEnglishName =vtos(this->m_DataEng->GetCollect(_T("e_gcmc")));


		EDIBgbl::SelDsgnID =m_SelDsgnID;
		COleVariant vTmp;
		m_DataDsgn->get_Collect((_variant_t)_T("sjjdmc"),&vTmp);
		EDIBgbl::SelDsgnName =vtos(vTmp);
		EDIBgbl::SelSpecID = m_SelSpecID;
		m_DataSpe->get_Collect((_variant_t)_T("ZYMC"),&vTmp);
		EDIBgbl::SelSpecName = vtos(vTmp);
		EDIBgbl::SelJcdm = vtos(m_DataVlm->GetCollect(_T("jcdm")));
		EDIBgbl::SelVlmID=vtoi(m_DataVlm->GetCollect(_T("VolumeID")));
		EDIBgbl::SelVlmName = vtos(m_DataVlm->GetCollect(_T("jcmc")));
		m_DataDsgn->get_Collect((_variant_t)_T("sjjddm"),&vTmp);
		EDIBgbl::strSelDsgn=vtos(vTmp);
		m_DataSpe->get_Collect((_variant_t)_T("zydm"),&vTmp);
		EDIBgbl::strSelSpec=vtos(vTmp);
		if( EDIBgbl::SelHyID < 0 ) EDIBgbl::SelHyID = modSelPDSV::defHyID;
		if( EDIBDB::SJHYIndex < 0 ) EDIBDB::SJHYIndex = modSelPDSV::defSJHYIndex;
		if( EDIBgbl::CompanyID == _T("") ) EDIBgbl::CompanyID = _T("43");
		_variant_t tmpvar;
		m_DataCategory->get_Collect((_variant_t)_T("SJHYID"),&vTmp);
		m_DataVlm->PutCollect(_T("SJHYID"),(VARIANT)vTmp);

		m_DataDsgn->get_Collect((_variant_t)_T("SJJDID"),&vTmp);
		m_DataVlm->PutCollect(_T("SJJDID"),(VARIANT)vTmp);

		m_DataSpe->get_Collect((_variant_t)_T("ZYID"),&vTmp);
		m_DataVlm->PutCollect(_T("ZYID"),(VARIANT)vTmp);

		if(vtoi(m_DataEng->GetCollect(_variant_t(_T("UnitNum"))))==0)
			m_DataEng->PutCollect(_variant_t(_T("UnitNum")),_variant_t((long)2));
		if (EDIBgbl::giUnitNum == 0 ) EDIBgbl::giUnitNum =vtoi(m_DataEng->GetCollect(_variant_t(_T("UnitNum"))));
   
		SetRegValue(_T("RecentWork"), user::gstrAppName + _T("_PrjID"), EDIBgbl::SelPrjID);
		SetRegValue(_T("RecentWork"), user::gstrAppName + _T("_DsgnID"), EDIBgbl::SelDsgnID);
		SetRegValue(_T("RecentWork"), user::gstrAppName + _T("_SpecID"), EDIBgbl::SelSpecID);
		SetRegValue(_T("RecentWork"), user::gstrAppName + _T("_HyID"), EDIBgbl::SelHyID);
		SetRegValue(_T("RecentWork"), user::gstrAppName + _T("_Dsgn"), EDIBgbl::strSelDsgn);
		SetRegValue(_T("RecentWork"), user::gstrAppName + _T("_Spec"), EDIBgbl::strSelSpec);
		SetRegValue(_T("RecentWork"), user::gstrAppName + _T("_VlmID"), EDIBgbl::SelVlmID);
		SetRegValue(_T("RecentWork"), user::gstrAppName + _T("_BillID"), EDIBgbl::SelBillType);
		SetRegValue(_T("RecentWork"), _T("SJHY"), EDIBDB::SJHY);
		SetRegValue(_T("RecentWork"), _T("SJHYIndex"), EDIBDB::SJHYIndex);

		EDIBgbl::InitDBTBN();
	}
	catch(_com_error e)
	{
		ShowMessage(e.Description());
	}
}

void CFrmSelPDSV::OnBeforeDeleteDbgeng(short FAR* Cancel) 
{
	// TODO: Add your control notification handler code here
	if(ShowMessage( GetResStr(IDS_MsgBox_60601)+m_DBGeng.GetColumns().GetItem(_variant_t((long)0)).GetText()+GetResStr(IDS_MsgBox_60602),MB_YESNO|MB_ICONEXCLAMATION,GetResStr(IDS_MsgBox_60603))==IDNO)
		*Cancel=1;
}


void CFrmSelPDSV::OnDblClickDbgeng() 
{
	// TODO: Add your control notification handler code here
	try
	{
		if(m_DataVlm->adoEOF && m_DataVlm->BOF)
			return;
		AfxGetApp()->BeginWaitCursor();
		m_DBGeng.SetRefDataSource(NULL);
		m_DBGvlm.SetRefDataSource(NULL);
		SelToCurrWork();
		this->SendMessage(WM_CLOSE);
		//this->CloseWindow();
		FrmPhsData.ShowWindow(SW_SHOW);
		AfxGetApp()->EndWaitCursor();
		CString ss=EDIBgbl::gsTitle+(EDIBgbl::TBNSelPrjSpec == _T("") ? _T("") : (_T("    ") + EDIBgbl::TBNSelPrjSpec + EDIBgbl::Btype[EDIBgbl::SelBillType] + _T(" (") + EDIBgbl::SelJcdm + _T(")")));
		AfxGetApp()->m_pMainWnd->SetWindowText(ss);
	}
	catch(CException *e)
	{
		e->Delete();
	}
}

void CFrmSelPDSV::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnClose();
	
	//this->DestroyWindow();
}

void CFrmSelPDSV::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here

}

void CFrmSelPDSV::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	if(bShow)
	{
		//SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
	}
	else;
		//SetWindowPos(&wndNoTopMost,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);

}

void CFrmSelPDSV::OnRowColChangeDbgvim(VARIANT FAR* LastRow, short LastCol) 
{
	// TODO: Add your control notification handler code here 
	try
	{
		try
		{
			if(m_DBGeng.GetAddNewMode()==2/*dbgAddNewPending*/)
			{
				m_DataEng->Update();
				if(!m_DataEng->adoEOF)
					m_DataEng->MoveLast();
			}
			else if(m_DBGeng.GetAddNewMode()==0/*dbgNoAddNew*/)
			{
				/*if(m_DBGeng.GetEditActive())
				{
					m_DBGeng.SetEditActive(false);
					m_DataEng->MoveLast();
				}*/
				m_DataEng->Update();
				//m_DataEng->MoveLast();
			}
			else if(m_DBGeng.GetAddNewMode()==1/*dbgAddNewCurrent*/)
			{
				/*if(m_DBGeng.GetEditActive())
				{
					m_DBGeng.SetEditActive(false);
				}*/
				if(m_DBGeng.GetAddNewMode()!=1)
					m_DataEng->Update();
			}
		}
		catch(CException *e)
		{
			e->Delete();
			m_DataEng->CancelUpdate();
		}
	}
	catch(_com_error e)
	{
		//AfxMessageBox(e.Description());
	}
}

void CFrmSelPDSV::OnRowColChangeDbgeng(VARIANT FAR* LastRow, short LastCol) 
{
	try
	{
		try
		{
			if(m_DBGvlm.GetAddNewMode()==2/*dbgAddNewPending*/)
			{								
				m_DataVlm->Update();			
				if(!m_DataVlm->adoEOF) m_DataVlm->MoveLast();
			}
			else if(m_DBGvlm.GetAddNewMode()==0/*dbgNoAddNew*/)
			{
				//if(m_DBGvlm.GetEditActive())
				//{
					//m_DBGvlm.SetEditActive(false);
					m_DataVlm->Update();	
				//	m_DataVlm->MoveLast();
				//}
			}
			else if(m_DBGvlm.GetAddNewMode()==1/*dbgAddNewCurrent*/)
			{
				//if(m_DBGvlm.GetEditActive())
				//{
					//m_DBGvlm.SetEditActive(false);
				//}
				if(m_DBGeng.GetAddNewMode()!=1)
				m_DataVlm->Update();
			}
		}
		catch (CException *e)
		{
			e->Delete();
			//很可能是更新时出错
			m_DataVlm->CancelUpdate();
		}

		m_SelPrjID = vtos(m_DataEng->GetCollect(_T("EnginID")));
      m_SelPrjName = vtos(m_DataEng->GetCollect(_T("gcmc")));
		COleVariant tmpVar;
		m_DataDsgn->get_Collect((_variant_t)_T("SJJDID"),&tmpVar);
		m_SelDsgnID=vtoi(tmpVar);
		m_DataSpe->get_Collect((_variant_t)_T("ZYID"),&tmpVar);
		m_SelSpecID=vtoi(tmpVar);
		m_DataCategory->get_Collect((_variant_t)_T("SJHYID"),&tmpVar);
		m_SelHyID=vtoi(tmpVar);
		
		m_DBGvlm.SetRefDataSource(NULL);

		CString tmpSQL=CString(_T("SELECT *   FROM [Volume] WHERE EnginID ")) +(m_SelPrjID ==_T("") ? CString(_T(" IS NULL ")) : CString(_T(" = \'")) + m_SelPrjID + CString(_T("\' "))) ;
		tmpSQL +=_T(" AND SJHYID=") + ltos(m_SelHyID);
		tmpSQL +=_T(" AND SJJDID=") + ltos(m_SelDsgnID);
		tmpSQL +=_T(" AND ZYID=") + ltos(m_SelSpecID);
		tmpSQL +=_T(" ORDER BY jcdm ");
		//如果是打开的，必须先关闭,否则出错。关闭前，所有正在编辑的记录必须update,否则出错。
		if(m_DataVlm->State==adStateOpen)
			m_DataVlm->Close();
	   m_DataVlm->Open(_bstr_t(tmpSQL),(IDispatch*)conPRJDB,adOpenStatic,adLockOptimistic,adCmdText);

	   m_DataVlm->Find(_bstr_t(CString(_T("jcdm=\'") )+ Trim(EDIBgbl::SelJcdm) + _T("\'")),0,adSearchBackward);
		
		if(m_DataVlm->adoEOF)
		{
			if(!m_bIsNew && m_DataVlm->BOF && m_SelPrjID!=_T(""))
			{
			}
			else if(!m_DataVlm->BOF)
				m_DataVlm->MoveLast();			
		}
		
		m_DBGvlm.SetRefDataSource(m_DataVlm->GetDataSource());

		m_DBGvlm.GetColumns().GetItem(_variant_t(_T("EnginID"))).SetVisible(FALSE);
		m_DBGvlm.GetColumns().GetItem(_variant_t(_T("VolumeID"))).SetVisible(FALSE);
		m_DBGvlm.GetColumns().GetItem(_variant_t(_T("SJJDID"))).SetVisible(FALSE);
		m_DBGvlm.GetColumns().GetItem(_variant_t(_T("SJHYID"))).SetVisible(FALSE);
		m_DBGvlm.GetColumns().GetItem(_variant_t(_T("ZYID"))).SetVisible(FALSE);
		EDIBgbl::SetDBGridColumnCaptionAndWidth (m_DBGvlm, _T("tvlm"));

	}
	catch(_com_error e)
	{
#ifdef _DEBUG	
		ShowMessage(e.Description());
#endif
	}
}

void CFrmSelPDSV::OnColResizeDbgeng(short ColIndex, short FAR* Cancel) 
{
	EDIBgbl::SaveDBGridColumnCaptionAndWidth(m_DBGeng,ColIndex,_T("teng"));
}

void CFrmSelPDSV::OnColResizeDbgvim(short ColIndex, short FAR* Cancel) 
{
	EDIBgbl::SaveDBGridColumnCaptionAndWidth(m_DBGvlm,ColIndex,_T("tvlm"));	
}

long CFrmSelPDSV::GetMaxVolumeID()
{
	long ret=0;
	try
	{
		_RecordsetPtr rs;
		rs = conPRJDB->Execute(_bstr_t(_T("Select MAX(VolumeID) FROM [Volume]")),NULL,adCmdText);
		ret=vtoi(rs->GetCollect(_variant_t((long)0)));
		rs->Close();
		rs=NULL;
	}
	catch(CException *e)
	{
		e->Delete();
	}
	return ret;
}

void CFrmSelPDSV::OnBeforeUpdateDbgvim(short FAR* Cancel) 
{
	try
	{
		if(!m_bIsVlmNew)
			return;
		COleVariant tmpVar;
		m_DataDsgn->get_Collect((_variant_t)_T("SJJDID"),&tmpVar);
		m_SelDsgnID=vtoi(tmpVar);
		m_DataSpe->get_Collect((_variant_t)_T("ZYID"),&tmpVar);
		m_SelSpecID=vtoi(tmpVar);
		m_DataCategory->get_Collect((_variant_t)_T("SJHYID"),&tmpVar);
		m_SelHyID=vtoi(tmpVar);
		long maxvid=GetMaxVolumeID()+1;
		m_SelPrjID = vtos(m_DataEng->GetCollect(_T("EnginID")));
		m_SelPrjName = vtos(m_DataEng->GetCollect(_T("gcmc")));
		m_DBGvlm.GetColumns().GetItem(_variant_t(_T("EnginID"))).SetText(m_SelPrjID);
		m_DBGvlm.GetColumns().GetItem(_variant_t(_T("VolumeID"))).SetText(ltos(maxvid));
		
		m_DBGvlm.GetColumns().GetItem(_variant_t(_T("SJJDID"))).SetText(ltos(m_SelDsgnID));
		m_DBGvlm.GetColumns().GetItem(_variant_t(_T("ZYID"))).SetText(ltos(m_SelSpecID));
		m_DBGvlm.GetColumns().GetItem(_variant_t(_T("SJHYID"))).SetText(ltos(m_SelHyID));	}
	catch(CException *e)
	{
		e->ReportError();
		e->Delete();
	}
	catch(_com_error& e)
	{
		AfxMessageBox(e.Description());
	}
}

void CFrmSelPDSV::OnOnAddNewDbgvim() 
{
	m_bIsVlmNew=true;
}

void CFrmSelPDSV::OnOnAddNewDbgeng() 
{
	// TODO: Add your control notification handler code here
	m_bIsNew=true;
}

void CFrmSelPDSV::OnAfterUpdateDbgeng() 
{
	// TODO: Add your control notification handler code here
	m_bIsNew=false;
}

LRESULT CFrmSelPDSV::SelChangeCmbo()
{
	LRESULT result = Default();
	this->OnRowColChangeDbgeng(NULL,0);
	return result;
}

void CFrmSelPDSV::OnAfterUpdateDbgvim() 
{
	// TODO: Add your control notification handler code here
	m_bIsVlmNew=false;
}

LRESULT CFrmSelPDSV::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	//if(message==ON_SELDATACHANGE)
	//	return SelChangeCmbo();
	return CDialog::WindowProc(message, wParam, lParam);
}



void CFrmSelPDSV::OnBeforeDeleteDbgvim(short FAR* Cancel) 
{
	// TODO: Add your control notification handler code here
	CString sTmp;
	try
	{
		if(vtoi(m_DataVlm->GetCollect(_T("VolumeID"))) == EDIBgbl::SelVlmID)
		{
			AfxMessageBox(IDS_CURRENTVOLUMECANNOTCANCEL);
			*Cancel=1;
			return;
		}
		sTmp.Format(::GetResStr(IDS_DELETE_VOLUME),vtos(m_DataVlm->GetCollect(_T("jcmc"))));
		if(AfxMessageBox(sTmp,MB_YESNO)==IDNO)
			*Cancel=1;
		else
		{
			CString strSQL;
			int vlmID;
			LPCTSTR szTab[]={(_T("Z1")),(_T("ZB")),(_T("ZA")),(_T("ZC")),(_T("ZD")),(_T("ZG")),(_T("CL")),(_T("ML")),(_T("Z8"))};
			int c=sizeof(szTab)/sizeof(LPCTSTR);
			vlmID=vtoi(m_DataVlm->GetCollect(_T("VolumeID")));
			for(int i=0;i<c;i++)
			{
				strSQL.Format((_T("DELETE FROM [%s] WHERE VolumeID=%d")),szTab[i],vlmID);
				try
				{
					::conPRJDB->Execute(_bstr_t(strSQL),NULL,adCmdText);
				}
				catch(_com_error e)
				{
#ifdef _DEBUG
					AfxMessageBox(e.Description());
#endif
				}
			}
		}
	}
	catch(CException *e)
	{
		e->Delete();
		*Cancel=1;
	}
}

void CFrmSelPDSV::OnEditchangeComboHY() 
{
	// TODO: Add your control notification handler code here
	
}

void CFrmSelPDSV::OnSelchangeComboHY() 
{
	// TODO: Add your control notification handler code here
	m_DBComboCategory.OnSelchange();
	try
	{
		COleVariant tmpVar;
		this->m_DataCategory->get_Collect((_variant_t)_T("SJHYID"),&tmpVar);
		this->m_SelHyID=vtoi(tmpVar);
		if(m_DataDsgn->State == adStateOpen)
			m_DataDsgn->Close();
		if(m_DataSpe->State == adStateOpen)
			m_DataSpe->Close();
// 		m_DataDsgn.m_pDatabase=&EDIBgbl::dbDSize;//20071101 "dbSORT" 改为 "dbDSize"
// 		m_DataDsgn.Open(dbOpenDynaset,_T("SELECT * FROM DesignStage WHERE sjhyid=") + ltos(m_SelHyID));
		CString sTmp;
		sTmp = _T("SELECT * FROM DesignStage WHERE sjhyid=") + ltos(m_SelHyID);
		m_DataDsgn->Open((_bstr_t)sTmp,_variant_t((IDispatch*)EDIBgbl::dbDSize,true), 
			adOpenKeyset, adLockOptimistic, adCmdText); 

// 		m_DataSpe.m_pDatabase=&EDIBgbl::dbDSize;//20071101 "dbSORT" 改为 "dbDSize"
// 		m_DataSpe.Open(dbOpenDynaset,_T("SELECT * FROM Speciality WHERE sjhyid=") + ltos(m_SelHyID));
		sTmp = _T("SELECT * FROM Speciality WHERE sjhyid=") + ltos(m_SelHyID);
		m_DataSpe->Open((_bstr_t)sTmp,_variant_t((IDispatch*)EDIBgbl::dbDSize,true), 
			adOpenKeyset, adLockOptimistic, adCmdText); 

		//设计阶段
		LoadDBComboDsgn();
		//设计专业
		LoadDBComboSpec();
		COleVariant v;
		if (m_SelDsgnID <= 0 )
		{
			//默认:电力行业施工图设计阶段
			m_SelDsgnID=4;
		}
// 		m_DataDsgn.FindFirst(_T("sjjdID= ")+ ltos(m_SelDsgnID));
		m_DataDsgn->Find((_bstr_t)(_T("sjjdID= ")+ ltos(m_SelDsgnID)), 0, adSearchBackward);
  
		if(m_SelSpecID <= 0 )
		{
			//默认:电力行业热机专业
			m_SelSpecID=3;
		}
// 		m_DataSpe.FindFirst(_T("zyid= ")+ ltos(m_SelSpecID));
		m_DataSpe->Find((_bstr_t)(_T("zyid= ")+ ltos(m_SelSpecID)), 0, adSearchBackward);

		this->m_DBComboDsgn.RefLst();
		this->m_DBComboSpec.RefLst();
		OnRowColChangeDbgeng(NULL,0);
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.Description());
	}
}

void CFrmSelPDSV::OnSelchangeComboSpe() 
{
	// TODO: Add your control notification handler code here
	m_DBComboSpec.OnSelchange();
	OnRowColChangeDbgeng(NULL,0);
}

void CFrmSelPDSV::OnSelchangeComboDsgn() 
{
	// TODO: Add your control notification handler code here
	m_DBComboDsgn.OnSelchange();
	OnRowColChangeDbgeng(NULL,0);
}

void CFrmSelPDSV::OnOK() 
{
	// TODO: Add extra validation here
	OnDblClickDbgvim();	
}

void CFrmSelPDSV::OnEditchangeComboCategory() 
{
	// TODO: Add your control notification handler code here
	
}
