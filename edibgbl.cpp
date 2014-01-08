// EDIBgbl.cpp
//

#include "stdafx.h"
#include "EDIBgbl.h"
//#include "Recordset.h"
//#include "TableDefs.h"
//#include "Form.h"

#include "frmFolderLocation.h"
#include "Cphs.h"

#include "basDirectory.h"
#include "modRegistry.h"
#include "modselpdsv.h"
#include "modPHScal.h"
#include "user.h"
#include "EDIBDB.h"
#include "EDIBAcad.h"
#include "FrmStatus.h"
#include "phsdata.h"
#include "FrmTxsr.h"
#include "PhsStart.h"
#include "Column.h"
#include "Columns.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BOOL EDIBgbl::g_bPhsManual=FALSE;
BOOL EDIBgbl::g_bPhsPicture=FALSE;
BOOL EDIBgbl::g_bPhsSetNum=FALSE;
//BOOL EDIBgbl::g_bSysDesc=FALSE;
BOOL EDIBgbl::g_bUserDesc=FALSE;
BOOL EDIBgbl::g_bOnlyUserDesc=FALSE;
BOOL EDIBgbl::g_bOnlySysDesc=TRUE;
BOOL EDIBgbl::g_bCustomDesc=FALSE;
//bool EDIBgbl::g_bSysAndUserDesc=true;
CString EDIBgbl::PhsNo;
CString EDIBgbl::VolumeNo;
CString EDIBgbl::SelZyID;
CString EDIBgbl::SelJcdm;
MnuProperty EDIBgbl::Cbtype[81];//20071026 "51" 改为 "71"
CString EDIBgbl::Btype[81];//20071026 改为 "71"
bool EDIBgbl::SplashState = FALSE;
int EDIBgbl::gnMURetryCnt = 0;
int EDIBgbl::gnMUDelay = 0;
int EDIBgbl::gnMULocking = 0;
bool EDIBgbl::gbDisplayBOM = FALSE;
bool EDIBgbl::gbSelSPECset = FALSE;
_variant_t EDIBgbl::DBstruArr = _variant_t();
CString EDIBgbl::SelPrjID = CString();
CString EDIBgbl::SelPrjName = CString();
CString EDIBgbl::SelPrjEnglishName = CString();
CString EDIBgbl::strSelDsgn;
CString EDIBgbl::strExportPaint = CString();//导出支吊架数据到AutoIPED的文件路径.
bool EDIBgbl::IsCalc=false;//是否正在计算pfg20051021
BOOL EDIBgbl::bSymbol = TRUE;
long EDIBgbl::SelSpecID = 0;
CString EDIBgbl::strSelSpec =_T("J");
CString EDIBgbl::SelSpecName = CString();
long EDIBgbl::SelDsgnID = 0;
CString EDIBgbl::SelDsgnName = CString();
long EDIBgbl::SelHyID =0;
CString EDIBgbl::SelHyName;
long EDIBgbl::SelVlmID = 0;
CString EDIBgbl::SelVlmName = CString();
CString EDIBgbl::strDwgVersion = CString();
double EDIBgbl::MaxLGLong=110;
long EDIBgbl::SelSjhyIndex = (long) 0;
long EDIBgbl::SelBillType = (long) 0;
CString EDIBgbl::CompanyID = CString();
long EDIBgbl::gStartPage = (long) 0;
int EDIBgbl::giUnitNum = 0;
CString EDIBgbl::gsSectionKey = CString();
CString EDIBgbl::gsTitle = CString();
CString EDIBgbl::CurMdb = CString();
CString EDIBgbl::TBNSelPrjSpec = CString();
CString EDIBgbl::TBNSelPrjVlm = CString();
CString EDIBgbl::TBNSelSpecVlm = CString();
CString EDIBgbl::tbnphsBOM = CString();
CString EDIBgbl::TBNRawData = CString();
CString EDIBgbl::SQLx = CString();
CString EDIBgbl::SQL1 = CString();
_variant_t EDIBgbl::Bmk = _variant_t();
long EDIBgbl::SelFuncFlag = (long) 0;
const int EDIBgbl::SEL_DRAWBILL = (int) 0;
const int EDIBgbl::SEL_CALCULATION = (int) 1;
const int EDIBgbl::CAL_STRESS = (int) 0;
const int EDIBgbl::CAL_COMBUSTION = (int) 1;
const int EDIBgbl::CAL_RESISTANCE = (int) 2;
const int EDIBgbl::CAL_EXHAUSTPIPE = (int) 3;
const int EDIBgbl::PRJ = (int) 0, VOLUME = 1;

const int EDIBgbl::TzDLuser=21,EDIBgbl::TZD800=22;
const int EDIBgbl::TZP=23;//巴陵石化设计院支吊架一览表
const int EDIBgbl::TZU=24;//用户自定义表格，自己可以在sort.mdb中的表TableINFO中ID=24的记录行按需要添加表定义。
const int EDIBgbl::TZD800HSY=32;//核四院支吊架明细一览表
const int EDIBgbl::TZD800QGY=33;//青工院支吊架明细一览表
const int EDIBgbl::TZD800HBYJ=34;//华北冶建支吊架明细一览表

const int EDIBgbl::TDE = (int) 0;
const int EDIBgbl::IC = 1;
const int EDIBgbl::EPE = 2;
const int EDIBgbl::TE = 3;
const int EDIBgbl::TML = (int) 0;
const int EDIBgbl::TMLE = (int) 27;
const int EDIBgbl::TCL = 1;
const int EDIBgbl::TCLE = 26;
const int EDIBgbl::TLJ = 2;
const int EDIBgbl::TLJ_BJBW = 31;
const int EDIBgbl::TSC = (int) 3;
const int EDIBgbl::TZD = 4;
const int EDIBgbl::TCD = 5;
const int EDIBgbl::TZA = (int) 6;
const int EDIBgbl::TZB = 7;
const int EDIBgbl::TZC = 8;
const int EDIBgbl::TZX = 9;
const int EDIBgbl::TCLA4 = (int) 10;
const int EDIBgbl::TCL4 = 11;
const int EDIBgbl::TZD180 = 12;
const int EDIBgbl::TZDSWED180 = 29;
const int EDIBgbl::TZDExcel=25;
const int EDIBgbl::TZD270L=24;
const int EDIBgbl::TCLMX = (int) 13;
const int EDIBgbl::TZF = 14;
const int EDIBgbl::TZG = 15;
const int EDIBgbl::TZLJ = (int) 16;
const int EDIBgbl::TCAD = 17;
const int EDIBgbl::TzDIST = (int) 18;
const int EDIBgbl::TzLOAD = 19;
const int EDIBgbl::TzDLOAD = 20;
const int EDIBgbl::TzDLOADNEW = 30;
const int EDIBgbl::TzDLOADS = 28;

const int EDIBgbl::SmallMetalBANK = (int) 0;
const int EDIBgbl::CircularFLANGE = 1;
const int EDIBgbl::RectFLANGE = 2;
//CString EDIBgbl::ListHead = CString();
const int EDIBgbl::GDTLB = (int) 0;
const int EDIBgbl::GDLJB = 1;
const int EDIBgbl::GDZDJ270 = 2;
const int EDIBgbl::GDZDJ180 = 3;
const int EDIBgbl::INSERT_PARTNO = (int) 0;
const int EDIBgbl::INSERT_ZDJH = (int) 1;
const int EDIBgbl::INSERT_PIPEDIA = (int) 2;
const int EDIBgbl::INSERT_NODENO = (int) 3;
const int EDIBgbl::INSERT_BG_INPUT = (int) 4;
const int EDIBgbl::INSERT_BG_Y = (int) 5;
const int EDIBgbl::INSERT_BG_Z = (int) 6;
const int EDIBgbl::INSERT_BG_DDATTE = (int) 7;
const int EDIBgbl::PrjIDw = (int) 1200;
const int EDIBgbl::PrjNw = 2400;
_variant_t EDIBgbl::FieldAttrNum = _variant_t();
//CString EDIBgbl::Btype = CString();
//int EDIBgbl::DBfields = 0;
//_variant_t EDIBgbl::LSteel = _variant_t();
const _TCHAR* EDIBgbl::YLJSBAT = _T("YLJS.bat");
CString EDIBgbl::PSA_OutDataUnit = CString();
const double EDIBgbl::kgf2N =  9.80665;
const double EDIBgbl::kgf2MPa =  0.0980665;
const int EDIBgbl::cm2mm = (int) 10;
int EDIBgbl::DrawNoSel =0;
int EDIBgbl::inc = 0;
int EDIBgbl::MaxExtRatio =0;
int EDIBgbl::DrawType  =0;
bool EDIBgbl::Tbhq =false;
bool EDIBgbl::gbTbOS=false;
bool EDIBgbl::TKsubzone =false;
bool EDIBgbl::TkLspLoaded = FALSE;
float EDIBgbl::gsngSumBomWeight = 0;
CString EDIBgbl::HelpFile = CString();
_variant_t EDIBgbl::zdjBmpNum = _variant_t();
_variant_t EDIBgbl::zdjBmp = _variant_t();
long EDIBgbl::glCurRecordNo =0;
bool EDIBgbl::pbFrmLoadedFrmStatus = FALSE;
bool EDIBgbl::pbFrmLoadedSELPDSV = FALSE;
bool EDIBgbl::pbFrmLoadedphsData = FALSE;
bool EDIBgbl::pbFrmLoadedTxsr = FALSE;
bool EDIBgbl::pbFrmLoadedPhsSamp = FALSE;

_ConnectionPtr EDIBgbl::dbPRJ;
_ConnectionPtr EDIBgbl::dbSORT;
_ConnectionPtr EDIBgbl::dbPRJDB;
_ConnectionPtr EDIBgbl::dbTable=NULL;//20071015

_ConnectionPtr EDIBgbl::dbDSize=NULL;//20071018
_ConnectionPtr EDIBgbl::dbMaterial=NULL;//20071018
_ConnectionPtr EDIBgbl::dbPHScode=NULL;//20071018
_ConnectionPtr EDIBgbl::dbSACal=NULL;//20071018

bool EDIBgbl::FdExists(_RecordsetPtr  rs, CString  strFieldName)
{
	return true;
}
/*
bool EDIBgbl::tdfExists(_ConnectionPtr pConn, CString  tbn)
{
	if(pConn==NULL || tbn==_T(""))
		return false;
	_RecordsetPtr rs;
	if(tbn.Left(1)!=_T("["))
		tbn=_T("[")+tbn;
	if(tbn.Right(1)!=_T("]"))
		tbn+=_T("]");
	try{
		rs=pConn->Execute(_bstr_t(tbn),NULL,adCmdTable);
		rs->Close();
		return true;
	}
	catch(_com_error )
	{
		return false;
	}
}
*/
bool EDIBgbl::InitBillType()
{
	HRESULT hr = S_OK;
	_RecordsetPtr rs;
	hr = rs.CreateInstance(__uuidof(Recordset));
	if(!::DirExists(basDirectory::ProjectDBDir))
	{
		frmFolderLocation.DoModal();
	}
	//20071024 10:58 (end) "ProjectDBDir" 改为 "DBShareDir"
	if(!FileExists(basDirectory::DBShareDir + _T("TableFormat.mdb"))) //2007.10.14 16:00(start) 把"Sort.mdb"改为"TableFormat.mdb"
		CopyFile(basDirectory::TemplateDir + _T("TableFormat.mdb"), basDirectory::DBShareDir + _T("TableFormat.mdb"),true);

	_ConnectionPtr db;
	hr = db.CreateInstance(__uuidof(Connection));

	try{
		hr = db->Open((_bstr_t)("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" + basDirectory::DBShareDir+_T("TableFormat.mdb")), 
			"", "", adConnectUnspecified);
		hr = rs->Open((_bstr_t)_T("select * from TableINFO"), _variant_t((IDispatch*)db,true), 
			adOpenStatic, adLockReadOnly, adCmdText); 

   _variant_t Var1,Var2;
   int v1;
   while(!rs->adoEOF)
   {
	   rs->get_Collect((_variant_t)(_T("ID")),&Var1);
	   v1=vtoi(Var1);
		if( v1 < 0) //20071026 "v1>50 ||" 删除
			continue;
		rs->get_Collect((_variant_t)(_T("TableNameSuffix")),&Var2);
		Btype[v1]=vtos(Var2);
		rs->get_Collect((_variant_t)(_T("TableCaption")),&Var2);
		Cbtype[v1].MnuCaption=vtos(Var2);
		rs->get_Collect((_variant_t)(_T("menuItemVisible")),&Var2);
		Cbtype[v1].MnuVisible=Var2.boolVal;
		rs->get_Collect((_variant_t)(_T("menuItemEnabled")),&Var2);
		Cbtype[v1].MnuEnabled=Var2.boolVal;

        rs->MoveNext();
	}
   rs->Close();
   rs.Release();
   db->Close();
   db.Release();
	return true;
   }
	catch(_com_error &e)
	{
		e.ErrorMessage();
		return false;
	}
}

void EDIBgbl::InitCurrWork()
{

	SelPrjID = GetRegKey(_T("RecentWork"), user::gstrAppName + _T("_PrjID"), CString(modSelPDSV::defPrjID));
	SelDsgnID = GetRegKey(_T("RecentWork"), user::gstrAppName + _T("_DsgnID"), modSelPDSV::defDsgnID);
	SelSpecID = GetRegKey(_T("RecentWork"), user::gstrAppName + _T("_SpecID"), modSelPDSV::defSpecID);
	strSelDsgn = GetRegKey(_T("RecentWork"), user::gstrAppName + _T("_Dsgn"), CString(modSelPDSV::defDsgn));
	strSelSpec = GetRegKey(_T("RecentWork"), user::gstrAppName + _T("_Spec"), CString(modSelPDSV::defSpec));

	SelVlmID = GetRegKey(_T("RecentWork"), user::gstrAppName + _T("_VlmID"), modSelPDSV::defVlmID);
	SelHyID = GetRegKey(_T("RecentWork"), user::gstrAppName + _T("_HyID"), modSelPDSV::defHyID);
	SelBillType = GetRegKey(_T("RecentWork"), user::gstrAppName + _T("_BillID"), TZA);
	//EDIBDB::SJHY = GetRegKey(_T("RecentWork"), _T("SJHY"), CString(modSelPDSV::defSJHY));
	EDIBDB::SJHYIndex = GetRegKey(_T("RecentWork"), _T("SJHYIndex"), modSelPDSV::defSJHYIndex);
	//获取工程名称卷册名称
	try{

		if(dbPRJDB == NULL)
		{
			dbPRJDB.CreateInstance(__uuidof(Connection));
			dbPRJDB->Open((_bstr_t)("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" + basDirectory::DBShareDir+_T("AllPrjDB.mdb")), "", "", adConnectUnspecified);//20071025 "ProjectDBDir" 改为 "DBShareDir"
		}
		if(dbPRJ== NULL)
		{
			dbPRJ.CreateInstance(__uuidof(Connection));
			dbPRJ->Open((_bstr_t)("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" + basDirectory::ProjectDir+_T("WorkPrj.mdb")), "", "", adConnectUnspecified);
		}
		if(dbSORT== NULL)
		{
			dbSORT.CreateInstance(__uuidof(Connection));
			dbSORT->Open((_bstr_t)("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" + basDirectory::ProjectDBDir+_T("sort.mdb")), "", "", adConnectUnspecified);
		}
		//20071015 10:22(start)
		if(dbTable== NULL)
		{
			dbTable.CreateInstance(__uuidof(Connection));
			dbTable->Open((_bstr_t)("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" + basDirectory::DBShareDir+_T("TableFormat.mdb")), "", "", adConnectUnspecified);
		}
		//20071015  10:22(end)
		//20071018(start)
		if(dbDSize== NULL)
		{
			dbDSize.CreateInstance(__uuidof(Connection));
			dbDSize->Open((_bstr_t)("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" + basDirectory::DBShareDir+_T("DrawingSize.mdb")), "", "", adConnectUnspecified);
		}

		if(dbMaterial== NULL)
		{
			dbMaterial.CreateInstance(__uuidof(Connection));
			dbMaterial->Open((_bstr_t)("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" + basDirectory::ProjectDBDir+_T("Material.mdb")), "", "", adConnectUnspecified);
		}

		if(dbPHScode== NULL)
		{
			dbPHScode.CreateInstance(__uuidof(Connection));
			dbPHScode->Open((_bstr_t)("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" + basDirectory::ProjectDBDir+_T("PHScode.mdb")), "", "", adConnectUnspecified);
		}

		if(dbSACal== NULL)
		{
			dbSACal.CreateInstance(__uuidof(Connection));
			dbSACal->Open((_bstr_t)("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" + basDirectory::ProjectDBDir+_T("SAStructureCal.mdb")), "", "", adConnectUnspecified);
		}
		//20071018(end)

		GetSelPrjName();
	}
	catch(CException *e)
	{
		e->ReportError();
		e->Delete();
	}
	catch(_com_error &e)
	{
		ShowMessage(CString((char*)e.ErrorMessage())+(char*)e.Description());
		AfxGetApp()->ExitInstance();
	}
}

void EDIBgbl::ConVertDB(CString  sDBN, CString  dDBN, _variant_t dVer, _variant_t  Pwd)
{
}

void EDIBgbl::CheckAllRecords(_RecordsetPtr rs)
{
}

void EDIBgbl::CheckAllFields(_RecordsetPtr rs)
{
}

void EDIBgbl::InitDBTBN(CString  &strSQL)
{
	CString tbn;
	_RecordsetPtr rs;
	rs.CreateInstance(__uuidof(Recordset));

	_RecordsetPtr rs1;
	rs1.CreateInstance(__uuidof(Recordset));
   try
   {

	if(FrmTxsr.m_pViewTxsr->m_bIsInit)
		FrmTxsr.m_pViewTxsr->CloseRs();
	   _variant_t tmpvar;
		_variant_t v;
		_variant_t v2;
//		rs->put_ActiveConnection((_variant_t)conPRJDB);
//		rs->Open((_bstr_t)(_T("Select * From [Volume] Where VolumeID=") + ltos(EDIBgbl::SelVlmID));
		SQLx = _T("Select * From [Volume] Where VolumeID=") + ltos(EDIBgbl::SelVlmID);
		rs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)conPRJDB,true), 
			adOpenKeyset, adLockOptimistic, adCmdText); 
		if(rs->adoEOF && rs->BOF)
		{

			rs->Close();
//			rs->Open((_bstr_t)(_T("Select * From [Volume] "));
			rs->Open((_bstr_t)_T("Select * From [Volume] "),_variant_t((IDispatch*)conPRJDB,true), 
				adOpenKeyset, adLockOptimistic, adCmdText); 
			rs->get_Collect((_variant_t)(_T("VolumeID")),&v);
			SelVlmID=vtoi(v);
		}
		rs->get_Collect((_variant_t)(_T("EnginID")),&v);
		EDIBgbl::SelPrjID=vtos(v);
		rs->get_Collect((_variant_t)(_T("JCDM")),&v);
		EDIBgbl::SelJcdm=vtos(v);
		rs->get_Collect((_variant_t)(_T("SJHYID")),&v);
		EDIBgbl::SelHyID=vtoi(v);
		rs->get_Collect((_variant_t)(_T("ZYID")),&v);
		EDIBgbl::SelSpecID =vtoi(v);
		rs->get_Collect((_variant_t)(_T("SJJDID")),&v);
		EDIBgbl::SelDsgnID=vtoi(v);
		rs->Close();
//		rs1.m_pDatabase=&EDIBgbl::dbDSize;//20071018 "dbSORT" 改为 "dbDSize"
		SQLx.Format(_T("SELECT [zymc] FROM [Speciality] WHERE ZYID=%d "),EDIBgbl::SelSpecID);
//		rs1->Open((_bstr_t)(dbOpenSnapshot,SQLx);
		rs1->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbDSize,true), 
			adOpenKeyset, adLockOptimistic, adCmdText); 
		if(!rs1->BOF && ! rs1->adoEOF)
		{
			rs1->get_Collect((_variant_t)0L,&v2);
			EDIBgbl::SelSpecName=vtos(v2);
		}
		else
		{CString str;
		str.LoadString(IDS_HOTENGINE);
			EDIBgbl::SelSpecName=str;
		}
		rs1->Close();


		SQLx.Format(_T("SELECT [sjjdmc] FROM [DesignStage] WHERE SJJDID=%d "),EDIBgbl::SelDsgnID);
//		rs1->Open((_bstr_t)(dbOpenSnapshot,SQLx);
		rs1->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbDSize,true), 
			adOpenKeyset, adLockOptimistic, adCmdText); 
		if(!rs1->BOF && ! rs1->adoEOF)
		{
			rs1->get_Collect((_variant_t)0L,&v2);
			EDIBgbl::SelDsgnName=vtos(v2);
		}
		else
		{CString str;
		str.LoadString(IDS_SHOP_DRAWING);
			EDIBgbl::SelDsgnName=str;
		}
		rs1->Close();

		SQLx.Format(_T("SELECT [SJHY] FROM [DrawSize] WHERE SJHYID=%d "),EDIBgbl::SelHyID);
//		rs1->Open((_bstr_t)(dbOpenSnapshot,SQLx);
		rs1->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbDSize,true), 
			adOpenKeyset, adLockOptimistic, adCmdText); 
		if(!rs1->BOF && ! rs1->adoEOF)
		{
			rs1->get_Collect((_variant_t)0L, &v2);
			EDIBgbl::SelHyName=vtos(v2);
		}
		else
		{CString str;
		str.LoadString(IDS_ELECTRIC_POWER);
			EDIBgbl::SelHyName=str;
		}
		rs1->Close();


		//TBNSelPrjSpec = SelPrjID+strSelDsgn+_T("-")+strSelSpec;
		CString sTmpNo;
		sTmpNo = EDIBAcad::GetDrawIDAndName(modPHScal::zdjh + modPHScal::glDrawNoStartNumber - 1, EDIBDB::DrawName);
		//下句修改原始表ZA成功标志bCalSuccess，使其与结果表ZB一致。
		SQLx=_T("Update [") + Btype[TZA] + _T("] SET bCalSuccess =0 WHERE VolumeID=") +  ltos(SelVlmID) + _T(" AND NOT EXISTS ( SELECT ZDJH FROM [") + Btype[TZB] 
			+_T("] WHERE [") + Btype[TZA] + _T("].VolumeID = [") + Btype[TZB] + _T("].VolumeID AND [")
		   + Btype[TZA] + _T("].ZDJH = [") + Btype[TZB] + _T("].ZDJH)");
		EDIBgbl::dbPRJDB->Execute((_bstr_t)SQLx, NULL, adCmdText);

		if (SelBillType == TLJ || SelBillType == TLJ_BJBW)
			SQLx = _T("SELECT * FROM [")+Btype[SelBillType]+_T("] WHERE VolumeID=") + ltos(SelVlmID) + _T(" AND [ifLJ]=TRUE ");
		else 
		{
			if (SelBillType == TZA)
				SQLx = _T("SELECT * FROM [")+Btype[SelBillType]+_T("]  WHERE VolumeID=") + ltos(SelVlmID)+ _T(" ORDER BY zdjh");
			else
				SQLx = _T("SELECT * FROM [")+Btype[SelBillType]+_T("] WHERE VolumeID=") + ltos(SelVlmID);
		}
		 strSQL = SQLx;
//		rs.m_ActiveConnection=conPRJDB;
//		rs->Open((_bstr_t)(SQLx);
		 rs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)conPRJDB,true), 
			 adOpenKeyset, adLockOptimistic, adCmdText); 
		if(rs->adoEOF && rs->BOF )
		{
			//如果数据库为空，则添加一个新记录作为示范，
			//使用户易于掌握输入格式
			rs->AddNew();
		  rs->put_Collect((_variant_t)_T("VolumeID"),_variant_t(SelVlmID));
		  CString strPart;
		  switch(SelBillType)
		  {
		  case TML:
				rs->put_Collect((_variant_t)_T("SEQ"),_variant_t((long)1));
				rs->put_Collect((_variant_t)_T("DRAWNO"),_variant_t(TBNSelPrjSpec+SelJcdm+_T("01")));
			  
			  rs->put_Collect((_variant_t)_T("DRAWNA"),_variant_t(GetResStr(IDS_ASSISTANTINSTALL_FIRSTCHART)));
				  break;
		  case TCL:
		  case TLJ:
			rs->put_Collect((_variant_t)_T("CLID"),_variant_t(_T("DD1404-01")));
			rs->put_Collect((_variant_t)_T("CLmc"),_variant_t(GetResStr(IDS_JOINTING_ELBOW)));
			rs->put_Collect((_variant_t)_T("CLgg"),_variant_t(_T("Dn600R1900A30")));
			rs->put_Collect((_variant_t)_T("CLcl"),_variant_t(_T("Q235-A")));
			strPart.Format(_T("%s"),_T("件"));
			rs->put_Collect((_variant_t)_T("CLdw"),_variant_t(strPart));
			rs->put_Collect((_variant_t)_T("CLdz"),_variant_t((float)152.8));
			rs->put_Collect((_variant_t)_T("CLnum"),_variant_t((float)8));
			rs->put_Collect((_variant_t)_T("CLzz"),_variant_t((float)(152.8 * 8.0)));
			break;
		  case TZD:
				rs->put_Collect((_variant_t)_T("ZDID"),_variant_t(EDIBAcad::GetDrawIDAndName(gStartPage, strSQL)));
				
				rs->put_Collect((_variant_t)_T("ZDmc"),_variant_t(GetResStr(IDS_FIXUP_BRACKET)));
				rs->put_Collect((_variant_t)_T("ZDnum"),_variant_t((float)8));
				rs->put_Collect((_variant_t)_T("ZDgg"),_variant_t(_T("%%c1520x5")));
				rs->put_Collect((_variant_t)_T("jghz"),_variant_t((long)18000));
				rs->put_Collect((_variant_t)_T("ZDbz"),_variant_t(GetResStr(IDS_INPUTREMARK)));
				break;
		  case TSC:
				rs->put_Collect((_variant_t)_T("SCID"),_variant_t(_T("Y630-2-4")));
				rs->put_Collect((_variant_t)_T("SCmc"),_variant_t(GetResStr(IDS_ELECTROMOTOR)));
				break;
		  case TZA:
			  rs->put_Collect((_variant_t)_T("ZDJH"),_variant_t((long)1));
			  rs->put_Collect((_variant_t)_T("JSDBH1"),_variant_t((long)1));
			  rs->put_Collect((_variant_t)_T("IDdn1"),_variant_t(_T("D2")));
			  rs->put_Collect((_variant_t)_T("IDgn1"),_variant_t(_T("G21")));
			  rs->put_Collect((_variant_t)_T("dj1"),_variant_t((float)108));
			  rs->put_Collect((_variant_t)_T("t01"),_variant_t((float)300));
			  rs->put_Collect((_variant_t)_T("pgz1"),_variant_t((float)100));
			  rs->put_Collect((_variant_t)_T("pjg1"),_variant_t((float)150));
			  rs->put_Collect((_variant_t)_T("iSelSampleID"),_variant_t((float)1));
			  //rs->Update();
					//支吊架原始数据表，再添加一条记录，显示图形化输入窗体后才不易出错。
			  break;
		  }
			rs->Update();
		}
		rs->Close();
		FrmTxsr.m_pViewTxsr->m_SQL=SQLx;
		

		if(FrmTxsr.m_pViewTxsr->m_bIsInit)
			FrmTxsr.m_pViewTxsr->InitRs();
   }
	catch(_com_error &e)
	{
		ShowMessage(e.Description());
	}

	rs.Release();
}

 void EDIBgbl::CloseAll_ConnectionPtr()
 {
 	try{
		if(conPRJDB)
		{
 			if(conPRJDB->State!=adStateClosed)
 				conPRJDB->Close();
 			conPRJDB=NULL;
		}

		if(conPRJDB4)
		{
 			if(conPRJDB4->State!=adStateClosed)
 				conPRJDB4->Close();
 			conPRJDB4=NULL;
		}
		if(conPHSConfig)
		{
			if(conPHSConfig->State!=adStateClosed)
				conPHSConfig->Close();
			conPHSConfig=NULL;
		}
 	}
 	catch(_com_error &e)
 	{
 		ShowMessage(CString((char*)e.ErrorMessage())+CString((char*)e.Description()));
 	}
 }

 void EDIBgbl::LoadMenuSelBill(CMenu* mnuObj)
{
}

void EDIBgbl::LSteelInit()
{
}

void EDIBgbl::SortField(/*Object MyDBGrid, Object MyData, */int MyColIndex)
{
}

void EDIBgbl::SaveDBGridColumnCaptionAndWidth(CDataGrid& MyDBGrid, long  ColIndex, CString tbn)
{
//Sub SaveDBGridColumnCaptionAndWidth(MyDBGrid As Object, ByVal ColIndex As Long, Optional tbn)
   try
	{
		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(Recordset));
		CString SQLx;
		int i=0;
		if (tbn==_T(""))
			SQLx = _T("SELECT * FROM T") + EDIBgbl::Btype[EDIBgbl::SelBillType] + _T(" WHERE trim(LocalCaption)<>\'\'");
		else
			SQLx = _T("SELECT * FROM [") + tbn + _T("] WHERE trim(LocalCaption)<>\'\'");
		
		//float Width;
		//_variant_t sngWidth;
		float sngWidth;
		_variant_t ix;
		ix.ChangeType(VT_I4);
		ix.intVal =ColIndex;
//		rs.m_pDatabase = &EDIBgbl::dbSORT;
		
//		rs->Open((_bstr_t)(dbOpenDynaset,SQLx);
		rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbSORT,true), 
			adOpenKeyset, adLockOptimistic, adCmdText); 
		CString sTmp;
		sTmp=MyDBGrid.GetColumns().GetItem(ix).GetDataField();
		sTmp.TrimLeft();sTmp.TrimRight();sTmp.MakeUpper();
// 		_variant_t vTmp;
// 		rs->Find((_bstr_t)(_T("((FieldName))=\'") + sTmp + _T("\'")), 0, adSearchBackward);
		HRESULT hr = S_OK;
		CString strFind;
		strFind = _T("((FieldName))=\'") + sTmp + _T("\'");
		hr = rs->Find((_bstr_t)strFind, 0, adSearchForward);
		sngWidth=MyDBGrid.GetColumns().GetItem(ix).GetWidth()*20;
		//twips/20->pixels
		ix.ChangeType(VT_R4);
		ix.fltVal=sngWidth;
//		rs.Edit();
		rs->put_Collect((_variant_t)_T("width"),ix);
		rs->Update();
	}
	catch(CString e)
	{
		ShowMessage(e);
	}
	catch(CException *e)
	{
		e->ReportError();
		e->Delete();
	}
}

void EDIBgbl::SetDBGridColumnCaptionAndWidth(CDataGrid& MyDBGrid, CString  tbn)
{
	_RecordsetPtr rs;
	rs.CreateInstance(__uuidof(Recordset));
	int i,iWC;
	_variant_t tmpvar;
	_variant_t v;
	tbn.TrimLeft();
	tbn.TrimRight();
   if (tbn==_T(""))
	   SQLx = _T("SELECT * FROM T") + Btype[SelBillType] + _T(" WHERE trim(LocalCaption)<>\'\'");
   else
	   SQLx = _T("SELECT * FROM ") + tbn + _T(" WHERE trim(LocalCaption)<>\'\'");
   HRESULT hr = S_OK;
   _variant_t ix;
   ix.ChangeType(VT_I4);
   CString sTmp;

	try{
	//   rs.m_pDatabase=&dbTable;//20071015 "dbSORT"改为"dbTable"
	//   rs->Open((_bstr_t)(dbOpenSnapshot,SQLx);
	   rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)dbTable,true), 
		   adOpenKeyset, adLockOptimistic, adCmdText); 

   
	   for(i=0;i<MyDBGrid.GetColumns().GetCount();i++)
	   {
		   ix.intVal=i;
		   sTmp=MyDBGrid.GetColumns().GetItem(ix).GetDataField();
		   sTmp.MakeUpper();

		   rs->MoveFirst();
		   CString strTmp = _T("FieldName=\'") + sTmp + _T("\'");
		   hr = rs->Find((_bstr_t)strTmp, 0, adSearchForward);
			if( !rs->adoEOF)
			{
				tmpvar.ChangeType(VT_I4);
				rs->get_Collect((_variant_t)_T("LocalCaption"),&v);
				MyDBGrid.GetColumns().GetItem(ix).SetCaption(vtos(v));
				
				rs->get_Collect((_variant_t)_T("width"),&v);
				if(v.vt==VT_NULL)
					continue;
				iWC=vtoi(v);
				iWC=iWC>0 ? iWC : 2000;
				MyDBGrid.GetColumns().GetItem(ix).SetWidth((float)iWC/20);
		  }
	   }
	}
	catch(_com_error e)
	{
		CString strErrorMsg;
		strErrorMsg.Format(_T("%s: %d, %s"), __FILE__, __LINE__, (LPTSTR)e.Description());
		AfxMessageBox(strErrorMsg);
	}
}

_variant_t EDIBgbl::va(/*Object Obj*/)
{
	return _variant_t();
}

void EDIBgbl::LocateFrm(CWnd* frm, _variant_t  bDisplayWindow)
{
}

void EDIBgbl::TraceMainFrm(long  hwnd)
{
}

bool EDIBgbl::bFrmLoaded(CString frmName)
{
	return true;
}

void EDIBgbl::InitPrjAndPrjdb()
{
CString srcs;
	CString sval;
	if(!FileExists(basDirectory::ProjectDir+_T("workprj.mdb")))
	{
		if(FileExists(basDirectory::TemplateDir+_T("workprj.mdb")))
		{
			//使用拷贝方法更好：因为数据库经过Access处理之后，即可成为共享数据库，便于多个用户使用。
			CopyFile(basDirectory::TemplateDir+_T("workprj.mdb"),basDirectory::ProjectDir+_T("workprj.mdb"),TRUE);
		}
		else
		{
			srcs.LoadString(IDS_NotFoundThisFile);
			sval.Format(srcs,basDirectory::TemplateDir+_T("workprj.mdb"));
			ShowMessage(sval);
		}
	}
	else
	{
		//看是否安装后第一次运行
		if(modPHScal::bInstalledAndFirstRun)
			CopyFile(basDirectory::TemplateDir+_T("workprj.mdb"),basDirectory::ProjectDir+_T("workprj.mdb"),TRUE);
	}
	
	if(!FileExists(basDirectory::DBShareDir+_T("AllPrjDB.mdb")))//20071025 "ProjectDBDir" 改为 "DBShareDir"
	{
		CopyFile(basDirectory::TemplateDir+_T("AllPrjdb.mdb"),basDirectory::DBShareDir+_T("AllPrjDB.mdb"),TRUE);//20071025 "ProjectDBDir" 改为 "DBShareDir"
	}
	
	if(!FileExists(basDirectory::ProjectDBDir+_T("sort.mdb")) )
	{
		if(	FileExists(basDirectory::TemplateDir+_T("sort.mdb")))
			CopyFile(basDirectory::TemplateDir+_T("sort.mdb"),basDirectory::ProjectDBDir+_T("sort.mdb"),TRUE);
	}
	else
	{
		//看是否安装后第一次运行
		if(modPHScal::bInstalledAndFirstRun)
			CopyFile(basDirectory::TemplateDir+_T("sort.mdb"),basDirectory::ProjectDBDir+_T("sort.mdb"),TRUE);
	}

	//20071031 (start)

	if(!FileExists(basDirectory::DBShareDir+_T("TableFormat.mdb")) )
	{
		if(	FileExists(basDirectory::CommonDir+_T("TableFormat.mdb")))
			CopyFile(basDirectory::CommonDir+_T("TableFormat.mdb"),basDirectory::DBShareDir+_T("TableFormat.mdb"),TRUE);
	}
	else
	{
		//看是否安装后第一次运行
		if(modPHScal::bInstalledAndFirstRun)
			CopyFile(basDirectory::CommonDir+_T("TableFormat.mdb"),basDirectory::DBShareDir+_T("TableFormat.mdb"),TRUE);
	}
	
	if(!FileExists(basDirectory::DBShareDir+_T("DrawingSize.mdb")) )
	{
		if(	FileExists(basDirectory::CommonDir+_T("DrawingSize.mdb")))
			CopyFile(basDirectory::CommonDir+_T("DrawingSize.mdb"),basDirectory::DBShareDir+_T("DrawingSize.mdb"),TRUE);
	}
	else
	{
		//看是否安装后第一次运行
		if(modPHScal::bInstalledAndFirstRun)
			CopyFile(basDirectory::CommonDir+_T("DrawingSize.mdb"),basDirectory::DBShareDir+_T("DrawingSize.mdb"),TRUE);
	}


	if(!FileExists(basDirectory::ProjectDBDir+_T("Material.mdb")) )
	{
		if(	FileExists(basDirectory::TemplateDir+_T("Material.mdb")))
			CopyFile(basDirectory::TemplateDir+_T("Material.mdb"),basDirectory::ProjectDBDir+_T("Material.mdb"),TRUE);
	}
	else
	{
		//看是否安装后第一次运行
		if(modPHScal::bInstalledAndFirstRun)
			CopyFile(basDirectory::TemplateDir+_T("Material.mdb"),basDirectory::ProjectDBDir+_T("Material.mdb"),TRUE);
	}

	if(!FileExists(basDirectory::ProjectDBDir+_T("PHScode.mdb")) )
	{
		if(	FileExists(basDirectory::TemplateDir+_T("PHScode.mdb")))
			CopyFile(basDirectory::TemplateDir+_T("PHScode.mdb"),basDirectory::ProjectDBDir+_T("PHScode.mdb"),TRUE);
	}
	else
	{
		//看是否安装后第一次运行
		if(modPHScal::bInstalledAndFirstRun)
			CopyFile(basDirectory::TemplateDir+_T("PHScode.mdb"),basDirectory::ProjectDBDir+_T("PHScode.mdb"),TRUE);
	}

	if(!FileExists(basDirectory::ProjectDBDir+_T("SAStructureCal.mdb")) )
	{
		if(	FileExists(basDirectory::TemplateDir+_T("SAStructureCal.mdb")))
			CopyFile(basDirectory::TemplateDir+_T("SAStructureCal.mdb"),basDirectory::ProjectDBDir+_T("SAStructureCal.mdb"),TRUE);
	}
	else
	{
		//看是否安装后第一次运行
		if(modPHScal::bInstalledAndFirstRun)
			CopyFile(basDirectory::TemplateDir+_T("SAStructureCal.mdb"),basDirectory::ProjectDBDir+_T("SAStructureCal.mdb"),TRUE);
	}
	//20071031 (end)
	
	if(!FileExists(basDirectory::ProjectDBDir+_T("Zdjcrude.mdb")))
		CopyFile(basDirectory::TemplateDir+_T("ZdjcrudeXBDLY.mdb"),basDirectory::ProjectDBDir+_T("Zdjcrude.mdb"),TRUE);
	else
	{
		//看是否安装后第一次运行
		if(modPHScal::bInstalledAndFirstRun)
		{
			CopyFile(basDirectory::TemplateDir+_T("ZdjcrudeXBDLY.mdb"),basDirectory::ProjectDBDir+_T("Zdjcrude.mdb"),FALSE);		
			
		}
	}
	
	//LFX 2005.7.18 加
	if(!FileExists(basDirectory::ProjectDBDir+_T("Weld.mdb")))
		CopyFile(basDirectory::TemplateDir+_T("Weld.mdb"),basDirectory::ProjectDBDir+_T("Weld.mdb"),TRUE);
	else
	{
		//看是否安装后第一次运行
		if(modPHScal::bInstalledAndFirstRun)
		{
			CopyFile(basDirectory::TemplateDir+_T("Weld.mdb"),basDirectory::ProjectDBDir+_T("Weld.mdb"),FALSE);		
			
		}
	}
	//END LFX 2005.7.18 加
}

void EDIBgbl::DataMoveByKey(_RecordsetPtr  rs, int  KeyCode)
{
}

/*Object*/void EDIBgbl::CreateCopyQuerydef(_ConnectionPtr  sDb, /*Object  dDb, */CString  strQdfName)
{
}

void EDIBgbl::InitPhsStructureREFandName()
{
 AfxGetApp()->BeginWaitCursor();
   const int i = 11;
   if(!IsWindow(frmStatus.m_hWnd))
	   frmStatus.Create(IDD_STATUS,AfxGetMainWnd());
   //frmStatus.ShowWindow(SW_SHOW);
   //frmStatus.UpdateWindow();
   frmStatus.m_Label1 = GetResStr(IDS_CreatingSpecification);
   frmStatus.m_Label2 = _T("");
   frmStatus.SetWindowText(GetResStr(IDS_InitializeAutoPHS));
   //frmStatus.Show
   frmStatus.UpdateData(false);
   frmStatus.UpdateStatus(0.11 / i,true);
   
   frmStatus.m_Label1= GetResStr(IDS_CheckDuplicateIndex);
   frmStatus.UpdateStatus ( 2/i, true);
   //1st
   frmStatus.UpdateData(false);
   Cavphs->CheckDuplicateIndex();
   
   frmStatus.m_Label1 = GetResStr(IDS_CheckAllCustomIDandIDinPhsStructureREF);
   frmStatus.UpdateData(false);
   frmStatus.UpdateStatus(5 / i, true);
   //2nd
   frmStatus.ShowWindow(SW_SHOW);
   Cavphs->CheckAllCustomIDandIDinPhsStructureREF();
   
   frmStatus.m_Label1 = GetResStr(IDS_CheckDuplicateREFRecord);
   frmStatus.UpdateData(false);
   frmStatus.UpdateStatus(8 / i, true);
   //3rd  注释掉加快速度。如果要调用可以在frmPhsSamp窗体中按菜单_T("检查重复记录")。
   //Cavphs->CheckDuplicateREFRecord
   
   frmStatus.m_Label1= GetResStr(IDS_ChangeNameInphsStructureName);
   frmStatus.UpdateData(false);
   frmStatus.UpdateStatus (10 / i, true);
   //4th
   Cavphs->ChangeNameInphsStructureName();
   
   frmStatus.UpdateStatus(1, true);
   //frmStatus.DestroyWindow();
   frmStatus.ShowWindow(SW_HIDE);
   AfxGetApp()->EndWaitCursor();
}

void EDIBgbl::LoadRecentFiles()
{
}

CString EDIBgbl::GetRegistryString(CString  vsItem, CString  vsDefault)
{
	return _T("");
}

//DEL void EDIBgbl::AddMRU()
//DEL {
//DEL }

void EDIBgbl::SaveRecentFiles()
{
}

void EDIBgbl::GetSelPrjName()
{
	try
	{
		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(Recordset));
		_RecordsetPtr rs2;
		rs2.CreateInstance(__uuidof(Recordset));

		//获得所有含有属性的块的属性值,便于以后输出

		_variant_t tmpVar;
		//取得工程名
// 		if(dbPRJDB->State != adStateOpen)
		{
			SQLx = _T("SELECT * FROM engin WHERE ((EnginID))=\'") + SelPrjID + _T("\' AND NOT IsNull(gcmc) AND trim(gcmc)<>\'\'");
// 			rs.m_pDatabase=&dbPRJDB;
// 			rs->Open((_bstr_t)(dbOpenDynaset,SQLx);
			rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)dbPRJDB,true), 
				adOpenKeyset, adLockOptimistic, adCmdText); 
			_variant_t v;
			if(!rs->adoEOF)
			{
				rs->get_Collect((_variant_t)(_T("gcmc")), &v);
				SelPrjName=vtos(v);

				rs->get_Collect((_variant_t)(_T("e_gcmc")), &v);
				SelPrjEnglishName=vtos(v);

			}
			SelPrjName.TrimLeft();
			SelPrjName.TrimRight();

			SelPrjEnglishName.TrimLeft();
			SelPrjEnglishName.TrimRight();

			rs->Close();
		}
		
		//取得设计阶段
		_variant_t v;
// 		if(dbDSize->State != adStateOpen)//20071018 11:29(start)"dbDSize"改为"dbDSize"
		{
//			rs2.m_pDatabase=&dbDSize;
			SQLx = _T("SELECT * FROM DesignStage WHERE ((sjjdid))=") + ltos(SelDsgnID) + _T(" AND NOT IsNull(sjjddm) AND trim(sjjddm)<>\'\'") + _T(" AND trim(SJHYid)=") + ltos(SelHyID);
			rs2->Open((_bstr_t)SQLx, _variant_t((IDispatch*)dbDSize,true), 
				adOpenKeyset, adLockOptimistic, adCmdText); 
			if(!rs2->adoEOF)
			{
				rs2->get_Collect((_variant_t)(_T("sjjdmc")), &v);
				SelDsgnName=vtos(v);
			}
			SelDsgnName.TrimLeft();
			SelDsgnName.TrimRight();
			rs2->Close();
			
			
			//取得专业名称
			SQLx = _T("SELECT * FROM Speciality WHERE trim(zyid)=") + ltos(SelSpecID) + _T(" AND trim(SJHYid)=") + ltos(SelHyID);
			rs2->Open((_bstr_t)SQLx, _variant_t((IDispatch*)dbDSize,true), 
				adOpenKeyset, adLockOptimistic, adCmdText); 
			if(!rs2->adoEOF)
			{
				rs2->get_Collect((_variant_t)(_T("zymc")), &v);
				SelSpecName=vtos(v);
			}
			SelSpecName.TrimLeft();
			SelSpecName.TrimRight();
			rs2->Close();
		}//20071018 11:29(end)"dbDSize"改为"dbDSize"
		//取得卷册名称
// 		if(dbPRJDB->State != adStateOpen)
		{
			SQLx = _T("SELECT * FROM [Volume] WHERE VolumeID=") + ltos(SelVlmID) + _T(" ORDER BY jcdm");
// 			rs.m_pDatabase=&dbPRJDB;
// 			rs->Open((_bstr_t)(dbOpenDynaset,SQLx);
			rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)dbPRJDB,true), 
				adOpenKeyset, adLockOptimistic, adCmdText); 
			_variant_t v;
			if(!rs->adoEOF)
			{
				rs->get_Collect((_variant_t)(_T("jcmc")), &v);
				SelVlmName=vtos(v);
			}
			SelVlmName.TrimLeft();
			SelVlmName.TrimRight();
			rs->Close();

			SQLx.Format("SELECT SUM(CLzz) AS SumWeight FROM [ZB] WHERE [VolumeID]=%d AND [ZDJH]=%d AND IsSAPart<>-1",EDIBgbl::SelVlmID,modPHScal::zdjh);
		
// 			rs.m_pDatabase=&dbPRJDB;
// 			rs->Open((_bstr_t)(dbOpenDynaset,SQLx);
			rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)dbPRJDB,true), 
				adOpenKeyset, adLockOptimistic, adCmdText); 
			if(!rs->adoEOF)
			{
				rs->get_Collect((_variant_t)(_T("SumWeight")), &v);
				EDIBAcad::g_fSumWeight=vtof(v);
			}
			rs->Close();
		}
	}
	catch(CException *e)
	{
		e->ReportError();
		e->Delete();
	}
}



CString EDIBgbl::GetDBName(_ConnectionPtr db)
{
	CString strDefaultDatabase;
	CString strContext;
	strDefaultDatabase = (LPTSTR)db->ConnectionString;

	for (int i = 0; i < 10; i++)
	{
		AfxExtractSubString(strContext, strDefaultDatabase, i, ';');
		if (strContext.Find(_T("Data Source")) >= 0)
		{
			break;
		}
	}

	if (strContext.IsEmpty())
	{
		return "";
	}

	AfxExtractSubString(strDefaultDatabase, strContext, 1, '=');
	
	return strDefaultDatabase;
}

bool EDIBgbl::tdfExists(_ConnectionPtr db, CString tbn)
{
	//11/3
//	if(db.m_pDAODatabase==NULL||tbn.IsEmpty())
	if(db ==NULL||tbn.IsEmpty())
	{
		return false;
	}
	_RecordsetPtr TempSet;
	TempSet.CreateInstance(__uuidof(Recordset));
	CString strSQL;
	strSQL="select * from ["+tbn+"]";
//	strSQL = "SELECT * FROM from MSysObjects where name=\'"+tbn+"\'";
	//11/3

	try{

		//11/3
// 		TempSet.m_pDatabase=&db;
// 		TempSet->Open((_bstr_t)(dbOpenSnapshot,strSQL);
		TempSet->Open((_bstr_t)strSQL, _variant_t((IDispatch*)db,true), 
			adOpenKeyset, adLockOptimistic, adCmdText); 
		TempSet->Close();
		TempSet.Release();
		return true;
		//11/3

	}
	catch(_com_error &e)
	{
		return false;
	}	 
}

void EDIBgbl::InitWorkTable(_ConnectionPtr db, CString tbn, int type)
{

}



CStringList* GetTableNameList(_ConnectionPtr pDB)
{
/*
* 作者：罗峰
* 功能：升级数据库
* 日期：2003-07-18
*/
//	ASSERT_VALID(pDB);
	CStringList *tableNameList;
	tableNameList = new CStringList();
/*
	int iCount = pDB->GetTableDefCount();
	for(int i=0; i<iCount; i++)
	{
		pDB->GetTableDefInfo(i, tDefInfo);
		CString strTemp = tDefInfo.m_strName;
		if((tDefInfo.m_lAttributes & dbSystemObject) == 0)
		{			
			tableNameList->AddTail(tDefInfo.m_strName);
		}
	}
*/
	_bstr_t bt;
	_RecordsetPtr rs(__uuidof(Recordset));
	rs = pDB->Execute((_bstr_t)_T("SELECT * FROM sysobjects"), NULL, adCmdText);
	while (!rs->adoEOF)
	{
		bt = rs->Fields->GetItem(_T("TABLE_TYPE"))->Value;
		if (!strcmp((char*)bt, "TABLE"))
		{
			bt = rs->Fields->GetItem(_T("TABLE_NAME"))->Value;
			tableNameList->AddTail((char*)bt);
		}
		rs->MoveNext();
	}
	return tableNameList;
}

CStringList* GetFieldNameList(_RecordsetPtr pTDef)
{
/*
* 作者：罗峰
* 功能：升级数据库
* 日期：2003-07-18
*/
//	ASSERT_VALID(pTDef);
	CStringList *pNameList;
	FieldPtr tDefInfo;
	pNameList = new CStringList();
	int iCount = pTDef->GetFields();
	for(LONG i=0; i<iCount; i++)
	{
		pTDef->GetFields()->get_Item((_variant_t)i, &tDefInfo);
		pNameList->AddTail(tDefInfo->Name);
	}
	return pNameList;
}

void PrintList(CStringList *strList)
{
/*
* 作者：罗峰
* 功能：升级数据库
* 日期：2003-07-18
*/
	for(POSITION pos = strList->GetHeadPosition(); pos != NULL; )
	{
		CString str = strList->GetNext(pos);
		str += _T("\n");
		TRACE0(str);
	}
}

BOOL StringInList(CString strTableName, CStringList *strList)
{
/*
* 作者：罗峰
* 功能：升级数据库
* 日期：2003-07-18
*/
	for(POSITION pos = strList->GetHeadPosition(); pos != NULL; )
	{
		CString str = strList->GetNext(pos);
		if(_tcscmp(str, strTableName)==0)
		{
			return TRUE;
		}
	}
	return FALSE;
}

void  UpgradeDB(CString strDestDB ,CString strSourceDB)
{
/*
* 作者：罗峰
* 功能：升级数据库
* 日期：2003-07-18
*/
	//conPRJDB4 4.0的驱动，只有升级时用
	

	_ConnectionPtr sDB, dDB;
	sDB.CreateInstance(__uuidof(Connection));
	dDB.CreateInstance(__uuidof(Connection));

// 	sTDef(&sDB), dTDef(&dDB);
	FieldPtr fieldInfo,fieldInfoS,fieldInfoD;
	CString strSql;
	try
	{
		CString ConnectionString;
		
		ConnectionString="Provider=Microsoft.Jet.OLEDB.4.0;Persist Security Info=False;Data Source=" + strSourceDB;
		sDB->Open((_bstr_t)ConnectionString, "", "", adConnectUnspecified);
		ConnectionString="Provider=Microsoft.Jet.OLEDB.4.0;Persist Security Info=False;Data Source=" + strDestDB;
		dDB->Open((_bstr_t)ConnectionString, "", "", adConnectUnspecified);
		CStringList *sListTableName, *dListTableName, *sListFieldName, *dListFieldName;
		sListTableName = GetTableNameList(sDB);
		dListTableName = GetTableNameList(dDB);
		for(POSITION pos = sListTableName->GetHeadPosition(); pos != NULL; )
		{
			CString strTableName = sListTableName->GetNext(pos);
			if(StringInList(strTableName, dListTableName))
			{
// 				sTDef->Open((_bstr_t)(strTableName);
// 				dTDef->Open((_bstr_t)(strTableName);
				_RecordsetPtr sTDef;
				sTDef = sDB->Execute((_bstr_t)("SELECT * FROM " + strTableName), NULL, adCmdText);

				_RecordsetPtr dTDef;
				dTDef = dDB->Execute((_bstr_t)("SELECT * FROM " + strTableName), NULL, adCmdText);

				sListFieldName = GetFieldNameList(sTDef);
				dListFieldName = GetFieldNameList(dTDef);	
				for(POSITION pos = sListFieldName->GetHeadPosition(); pos != NULL; )
				{
					CString strFieldName = sListFieldName->GetNext(pos);
					if(!StringInList(strFieldName, dListFieldName))
					{
// 						fieldInfo.m_bAllowZeroLength = FALSE;
// 						fieldInfo.m_bRequired = FALSE;
// 						_variant_t fieldValue;
// 						dTDef->GetFields()->Append(fieldInfo->Name, fieldInfo->Type, fieldInfo->DefinedSize, 
// 							fieldInfo->Attributes, (_variant_t)fieldValue);
						sTDef->GetFields()->get_Item((_variant_t)strFieldName, &fieldInfo);
						//添加字段
						sTDef->Fields->Append(fieldInfo->Name, fieldInfo->Type, fieldInfo->DefinedSize, (adoFieldAttributeEnum)fieldInfo->Attributes);
					}
					else
					{
						sTDef->GetFields()->get_Item((_variant_t)strFieldName, &fieldInfoS);
						dTDef->GetFields()->get_Item((_variant_t)strFieldName, &fieldInfoD);
// 						sTDef.GetFieldInfo(strFieldName, fieldInfoS);
// 						dTDef.GetFieldInfo(strFieldName, fieldInfoD);
						if(fieldInfoS->ActualSize != fieldInfoD->ActualSize)
						{
							//改变字段大小
							strSql.Format("ALTER TABLE %s ALTER COLUMN %s VARCHAR(%d)",strTableName,strFieldName,fieldInfoS->DefinedSize );
							
							::conPRJDB4->Execute(_bstr_t(strSql),NULL,adCmdText);
						}
					
					}
				}
				sListFieldName->RemoveAll();
				dListFieldName->RemoveAll();
				delete sListFieldName;
				delete dListFieldName;
				sTDef->Close();
				dTDef->Close();
			}
			else
			{
				CString	SQLx;
				SQLx.Format(_T("SELECT * INTO %s FROM %s IN \'%s\'"),
					strTableName,strTableName,EDIBgbl::GetDBName(sDB));
				dDB->Execute((_bstr_t)SQLx, NULL, adCmdText);	
			}			

		}
		sListTableName->RemoveAll();
		dListTableName->RemoveAll();
		delete sListTableName;
		delete dListTableName;
		sDB->Close();
		dDB->Close();
	}
	catch(CException *e)
	{
		e->ReportError();
	}
}

void  UpgradeDB(CString strDestDB, CString strSourceDB, CStringList *sListTableName, CString strDestPwd, CString strSourcePwd)
{
/*
* 作者：罗峰
* 功能：升级数据库
* 日期：2003-07-18
*/
	if(sListTableName->IsEmpty())
		return;
	_ConnectionPtr sDB, dDB;
	sDB.CreateInstance(__uuidof(Connection));
	dDB.CreateInstance(__uuidof(Connection));
	FieldPtr fieldInfo;
	try
	{
		CString ConnectionString;

		ConnectionString="Provider=Microsoft.Jet.OLEDB.4.0;Persist Security Info=False;Data Source=" + strSourceDB;
		sDB->Open((_bstr_t)ConnectionString, "", (_bstr_t)strSourcePwd, adConnectUnspecified);
		ConnectionString="Provider=Microsoft.Jet.OLEDB.4.0;Persist Security Info=False;Data Source=" + strDestDB;
		dDB->Open((_bstr_t)ConnectionString, "", (_bstr_t)strDestPwd, adConnectUnspecified);

		CStringList *dListTableName, *sListFieldName, *dListFieldName;
		dListTableName = GetTableNameList(dDB);
		for(POSITION pos = sListTableName->GetHeadPosition(); pos != NULL; )
		{
			CString strTableName = sListTableName->GetNext(pos);
			if(StringInList(strTableName, dListTableName))
			{
// 				sTDef(&sDB), dTDef(&dDB);
// 				sTDef->Open((_bstr_t)(strTableName);
// 				dTDef->Open((_bstr_t)(strTableName);
				_RecordsetPtr sTDef;
				sTDef = sDB->Execute((_bstr_t)("SELECT * FROM " + strTableName), NULL, adCmdText);

				_RecordsetPtr dTDef;
				dTDef = sDB->Execute((_bstr_t)("SELECT * FROM " + strTableName), NULL, adCmdText);
				sListFieldName = GetFieldNameList(sTDef);
				dListFieldName = GetFieldNameList(dTDef);	
				CString strSql;
				for(POSITION pos = sListFieldName->GetHeadPosition(); pos != NULL; )
				{
					CString strFieldName = sListFieldName->GetNext(pos);
					if(!StringInList(strFieldName, dListFieldName))
					{
						sTDef->GetFields()->get_Item((_variant_t)strFieldName, &fieldInfo);
// 						sTDef.GetFieldInfo(strFieldName, fieldInfo);
// 						fieldInfo.m_bAllowZeroLength = FALSE;
// 						fieldInfo.m_bRequired = FALSE;
// 						dTDef.CreateField(fieldInfo);
						//添加字段
						strSql.Format("ALTER TABLE %s ADD COLUMN %s VARCHAR(%d)",strTableName,strFieldName,fieldInfo->DefinedSize );
						dDB->Execute(_bstr_t(strSql),NULL,adCmdText);
					}
				}
				sListFieldName->RemoveAll();
				dListFieldName->RemoveAll();
				delete sListFieldName;
				delete dListFieldName;
				sTDef->Close();
				dTDef->Close();
			}
			else
			{
				CString	SQLx;
				SQLx.Format(_T("SELECT * INTO %s FROM %s IN \'%s\'"),
					strTableName,strTableName,EDIBgbl::GetDBName(sDB));
				dDB->Execute((_bstr_t)SQLx, NULL, adCmdText);	
			}
		}
		sListTableName->RemoveAll();
		dListTableName->RemoveAll();
		delete sListTableName;
		delete dListTableName;
		sDB->Close();
		dDB->Close();
	}
	catch(CException *e)
	{
		e->ReportError();
	}
}

CStringList*  GetFilterTableList(CString strDBPath, CString strFilter, CString strPwd)
{
/*
* 作者：罗峰
* 功能：升级数据库
* 日期：2003-07-18
*/
	_ConnectionPtr db;
	db.CreateInstance(__uuidof(Connection));
	CStringList *listTableName;
	CString strTableName;
	POSITION pos1,pos2;
	int iLen;
	iLen = strFilter.GetLength();
	try
	{
// 		db->Open((_bstr_t)(strDBPath, FALSE, FALSE, _T(";pwd=") + strPwd + _T(";"));
		CString ConnectionString;

		ConnectionString="Provider=Microsoft.Jet.OLEDB.4.0;Persist Security Info=False;Data Source=" + strDBPath;
		db->Open((_bstr_t)ConnectionString, "", (_bstr_t)strPwd, adConnectUnspecified);
		listTableName = GetTableNameList(db);
		for( pos1 = listTableName->GetHeadPosition(); (pos2=pos1)!=NULL; )
		{
			strTableName = listTableName->GetNext(pos1);
			if(_tcscmp(strTableName.Left(iLen), strFilter))
			{
				listTableName->RemoveAt(pos2);
			}
		}
		db->Close();
	}
	catch(CException *e)
	{
		e->ReportError();
		e->Delete();
	}
	return listTableName;
}

bool EDIBgbl::UpgradeDatabase()
{
	//目的:升级数据库allprjdb.mdb,sort.mdb,workprj.mdb。
	try {
		if ( GetRegKey(_T("Status"), _T("Install"), CString(_T("0"))) == _T("0") ) 
		{
			//复位安装标志
			_ConnectionPtr sDb , dDb ;
			sDb.CreateInstance(__uuidof(Connection));
			dDb.CreateInstance(__uuidof(Connection));
			int i =0;
			CString sTmp,SQLx;
			
			//在函数CreateTmpCustomIDForPART中要求tbnPAfix表的项目必须不出现在tbnPA表中

			//由于zdjcrude的结构太复杂，简单起见，拷贝一个。这样的缺点是：用户输入的记录丢失了。
			//if ( Not FileExists(ProjectDBDir  +  _T("Zdjcrude.mdb")) ) {
			//使用拷贝方法更好：因为数据库经过Access处理之后，即可成为共享数据库，便于多个用户使用。
			//使用自建的数据库必须经过Access处理，才能共享。
      
			//首先升级sort.mdb
			//升级sort.mdb/workprj.mdb的最好办法是拷贝
			bool sortCrtFd=false;
			_RecordsetPtr drsTmp;
			drsTmp.CreateInstance(__uuidof(Recordset));
			//20071031 "TemplateDir" 改为 "CommonDir" ; "ProjectDBDir" 改为 "DBShareDir"
			if ( FileExists(basDirectory::TemplateDir  +  _T("AllPrjdb.mdb")) && FileExists(basDirectory::DBShareDir  +  _T("AllPrjDB.mdb")) )  
			{
				UpgradeDB(basDirectory::DBShareDir + _T("AllPrjDB.mdb"), basDirectory::TemplateDir + _T("AllPrjdb.mdb"));
			}
			//20071031 (start)
			if ( FileExists(basDirectory::CommonDir  +  _T("TableFormat.mdb")) && FileExists(basDirectory::DBShareDir  +  _T("TableFormat.mdb")) )  
			{
				UpgradeDB(basDirectory::DBShareDir + _T("TableFormat.mdb"), basDirectory::CommonDir + _T("TableFormat.mdb"));
			}

			if ( FileExists(basDirectory::CommonDir  +  _T("DrawingSize.mdb")) && FileExists(basDirectory::DBShareDir  +  _T("DrawingSize.mdb")) )  
			{
				UpgradeDB(basDirectory::DBShareDir + _T("DrawingSize.mdb"), basDirectory::CommonDir + _T("DrawingSize.mdb"));
			}

			if ( FileExists(basDirectory::TemplateDir  +  _T("Material.mdb")) && FileExists(basDirectory::ProjectDBDir  +  _T("Material.mdb")) )  
			{
				UpgradeDB(basDirectory::ProjectDBDir + _T("Material.mdb"), basDirectory::TemplateDir + _T("Material.mdb"));
			}

			if ( FileExists(basDirectory::TemplateDir  +  _T("PHScode.mdb")) && FileExists(basDirectory::ProjectDBDir  +  _T("PHScode.mdb")) )  
			{
				UpgradeDB(basDirectory::ProjectDBDir + _T("PHScode.mdb"), basDirectory::TemplateDir + _T("PHScode.mdb"));
			}

			if ( FileExists(basDirectory::TemplateDir  +  _T("SAStructureCal.mdb")) && FileExists(basDirectory::ProjectDBDir  +  _T("SAStructureCal.mdb")) )  
			{
				UpgradeDB(basDirectory::ProjectDBDir + _T("SAStructureCal.mdb"), basDirectory::TemplateDir + _T("SAStructureCal.mdb"));
			}
			//20071031 (end)
			if ( FileExists(basDirectory::TemplateDir  +  _T("sort.mdb")) && FileExists(basDirectory::ProjectDBDir  +  _T("sort.mdb")) ) 
			{
				if(!UpdateSortDB())
					return false;
				UpgradeDB(basDirectory::ProjectDBDir + _T("sort.mdb"), basDirectory::TemplateDir + _T("sort.mdb"));
				CStringList *listTable = GetFilterTableList(basDirectory::ProjectDBDir + _T("Zdjcrude.mdb"), _T("PhsManu"), _T("ProductDB888"));
				UpgradeDB(basDirectory::ProjectDBDir  +   _T("sort.mdb"), basDirectory::ProjectDBDir  +  _T("Zdjcrude.mdb"), listTable, _T(""), _T("ProductDB888"));

			}     
			CString ConnectionString;
			//其次升级workprj.mdb
			if ( FileExists(basDirectory::TemplateDir  +  _T("workprj.mdb")) && FileExists(basDirectory::ProjectDir  +  _T("workprj.mdb")) ) 
			{
				ConnectionString="Provider=Microsoft.Jet.OLEDB.4.0;Persist Security Info=False;Data Source=" + basDirectory::TemplateDir  +  _T("workprj.mdb");
				sDb->Open((_bstr_t)ConnectionString, "", "", adConnectUnspecified);
				ConnectionString="Provider=Microsoft.Jet.OLEDB.4.0;Persist Security Info=False;Data Source=" + basDirectory::ProjectDir  +  _T("workprj.mdb");
				dDb->Open((_bstr_t)ConnectionString, "", "", adConnectUnspecified);
// 				sDb->Open((_bstr_t)(basDirectory::TemplateDir  +  _T("workprj.mdb")));
// 				dDb->Open((_bstr_t)(basDirectory::ProjectDir  +  _T("workprj.mdb")));
				ChangeDatabase(dDb,sDb);
				dDb->Close();
				sDb->Close();
			}
			ConnectionString="Provider=Microsoft.Jet.OLEDB.4.0;Persist Security Info=False;Data Source=" + basDirectory::DBShareDir  +  _T("AllPrjDB.mdb");
			dDb->Open((_bstr_t)ConnectionString, "", "", adConnectUnspecified);
// 			dDb->Open((_bstr_t)(basDirectory::DBShareDir+ _T("AllPrjDB.mdb")));//20071031 "ProjectDBDir" 改为 "DBShareDir"
			if(!EDIBgbl::tdfExists(dDb,_T("ZA")) || !EDIBgbl::tdfExists(dDb,_T("ZB")))
			{
				dDb->Close();
				if(!UpdateAllPrjDB())
				{
					return false;
				}
// 				dDb->Open((_bstr_t)(basDirectory::DBShareDir+ _T("AllPrjDB.mdb")));//20071031 "ProjectDBDir" 改为 "DBShareDir"
				ConnectionString="Provider=Microsoft.Jet.OLEDB.4.0;Persist Security Info=False;Data Source=" + basDirectory::DBShareDir  +  _T("AllPrjDB.mdb");
				dDb->Open((_bstr_t)ConnectionString, "", "", adConnectUnspecified);
			}

			// 删除索引
/*
		::CDaoTableDef tmptdf(&dDb);
		tmptdf->Open((_bstr_t)(_T("ML"));
		DAOIndexes* ixs;
		tmptdf.m_pDAOTableDef->get_Indexes(&ixs);
		ixs->Refresh();
		ixs->Release();
		try
		{
			if(bIndexExists(&tmptdf,_T("PrimaryKey")))
				tmptdf.DeleteIndex(_T("PrimaryKey"));
		}
		catch(CException *e)
		{
			e->Delete();
		}
		tmptdf->Close();
*/
			dDb->Execute((_bstr_t)"DROP INDEX PrimaryKey ON ML ", NULL, adCmdText);

// 		sDb->Open((_bstr_t)(basDirectory::TemplateDir  +  _T("AllPrjdb.mdb")));
			sDb->Open(_bstr_t(basDirectory::TemplateDir  +  _T("AllPrjdb.mdb")),_T(""),_T(""), adConnectUnspecified);//20071018"sort.mdb"改为"PHScode.mdb"
		//按F4511s.mdb结构升级AllPrjdb.mdb中的表的结构
		ChangeDatabase(dDb,sDb);
		//升级ZB表中以前根部附件(recno=NULL)特性字段:recno=NULL升级为IsSAPart=-1,recno=根部recno依次+1
		SQLx.Format(_T("SELECT recno,VolumeID,zdjh,seq,nth,IsSAPart FROM ZB ORDER BY VolumeID,zdjh,recno DESC"));
// 		drsTmp.m_pDatabase=&dDb;
		//recno按降序排列，则根部排在最前，根部附件排在最后
// 		drsTmp->Open((_bstr_t)(dbOpenDynaset,SQLx);
		drsTmp->Open(_variant_t(SQLx),(IDispatch*)dDb,adOpenKeyset, adLockOptimistic,adCmdText);
		int maxRecno=1;//current zdjh maxmium recno
		int iPrevZdjh=-1;//previous zdjh
		_variant_t v;
		while(!drsTmp->adoEOF)
		{
			drsTmp->get_Collect((_variant_t)_T("zdjh"), &v);
			if(iPrevZdjh==vtoi(v))
			{
				iPrevZdjh=vtoi(v);
				maxRecno++;
			}
			else
			{
				iPrevZdjh=vtoi(v);
				drsTmp->get_Collect((_variant_t)_T("recno"), &v);
				maxRecno=vtoi(v);
			}
			drsTmp->get_Collect((_variant_t)_T("recno"), &v);
			if(v.vt==VT_NULL)
			{
				//更新旧的根部附件
//				drsTmp.Edit();
				drsTmp->put_Collect((_variant_t)_T("IsSAPart"),_variant_t((short)-1));
				drsTmp->put_Collect((_variant_t)_T("recno"),_variant_t((long)maxRecno));
				drsTmp->Update();
			}
			drsTmp->get_Collect((_variant_t)_T("nth"), &v);
			if(v.vt==VT_NULL)
			{
				//更新旧的根部附件
//				drsTmp.Edit();
				drsTmp->put_Collect((_variant_t)_T("nth"),_variant_t((short)1));//标准支吊架路数=1，包括对称双吊和共用根部双吊
				drsTmp->Update();
			}
			
			drsTmp->MoveNext();
		}

		// 修改表字段长度
/*
		CDaoFieldInfo fi;
		tmptdf->Open((_bstr_t)(_T("ZA"));
		
		tmptdf.GetFieldInfo(_T("DwgName"),fi,AFX_DAO_ALL_INFO);
		if(fi.m_lSize<255)
		{
			fi.m_lSize=255;
			if(!bFieldExists(&tmptdf,_T("tmpDwgName")))
			{
				fi.m_strName=_T("tmpDwgName");
				tmptdf.CreateField(fi);
			}

			dDb->Execute((_bstr_t)_T("Update [ZA] SET [tmpDwgName] = [DwgName] "));
			tmptdf.DeleteField(_T("DwgName"));

			fi.m_strName=_T("DwgName");
			tmptdf.CreateField(fi);

			dDb->Execute((_bstr_t)_T("Update [ZA] SET [DwgName] = [tmpDwgName] "));
			tmptdf.DeleteField(_T("tmpDwgName"));
		}
		tmptdf->Close();
*/
		dDb->Execute((_bstr_t)"ALTER TABLE [ZA] ALTER COLUMN [DwgName] Text(255)", NULL, adCmdText);
/*
// 		tmptdf->Open((_bstr_t)(_T("ZG"));
		tmptdf = dDb->Execute((_bstr_t)"SELECT * FROM ZG")->GetFields();

		tmptdf->get_Item((_variant_t)_T("Handle"), &fi);
		if(fi->DefinedSize<16)
		{
			fi->DefinedSize=16;
			_variant_t fieldValue;
			if(!bFieldExists(&tmptdf,_T("tmpHANDLE")))
			{
				fi->Name=_T("tmpHANDLE");
//				tmptdf.CreateField(fi);
				tmptdf->Append(fi->Name, fi->Type, fi->DefinedSize, 
					fi->Attributes, (_variant_t)fieldValue);
			}

			dDb->Execute((_bstr_t)_T("Update [ZG] SET [tmpHandle] = [Handle] "), NULL, adCmdText);
//			tmptdf.DeleteField(_T("Handle"));
			tmptdf->Delete(_T("Handle"));

			fi->Name=_T("HANDLE");
//			tmptdf.CreateField(fi);
			tmptdf->Append(fi->Name, fi->Type, fi->DefinedSize, 
				fi->Attributes, (_variant_t)fieldValue);

			dDb->Execute((_bstr_t)_T("Update [ZG] SET [Handle] = [tmpHandle] "), NULL, adCmdText);
// 			tmptdf.DeleteField(_T("tmpHandle"));
			tmptdf->Delete(_T("tmpHandle"));
		}
// 		tmptdf->Close();
*/
		dDb->Execute((_bstr_t)"ALTER TABLE [ZG] ALTER COLUMN [DwgName] Text(16)", NULL, adCmdText);

		::SetRegValue(_T("Status"), _T("Install"), CString("1"));
		}
		return true;
	}
	
	catch(CException *e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}
}
/*
bool EDIBgbl::bIndexExists(_TablePtr* pTd,CString strIndex)
{
	int c=pTd->GetIndexCount();
	_IndexPtr info;
	strIndex.MakeUpper();
	for(int i=0;i<c;i++)
	{		
		pTd->GetIndexes()->get_Item(i, &info);
		info.m_strName.MakeUpper();
		if(info.m_strName==strIndex)
			return true;
	}
	return false;
}
*/
bool EDIBgbl::UpdateAllPrjDB()
{
	CString sdbName=_T("AllPrjdb.mdb");
	CString ddbName=_T("AllPrjdb.mdb");
	CString tdbName=_T("tmpAllPrjdb.mdb");
	CopyFile(basDirectory::TemplateDir+sdbName,basDirectory::ProjectDBDir+tdbName,TRUE);
	CString VoltabName[64];
	CString DatatabName[64];
	int Err=0;
	int vc,dc;
	if(!IsWindow(frmStatus.m_hWnd))
		frmStatus.Create(IDD_STATUS,AfxGetMainWnd());
	frmStatus.UpdateStatus(0);
	CString str;
	str.LoadString(IDS_UPDATING_DATABASE);
	frmStatus.SetWindowText(str);
	frmStatus.m_Label1=_T("");
	frmStatus.m_Label2=_T("");
	frmStatus.ShowWindow(SW_SHOW);
	AfxGetApp()->BeginWaitCursor();
	try
	{
		CString strSQL(_T(""));
		_ConnectionPtr sDb,dDb;
		sDb.CreateInstance(__uuidof(Connection));
		dDb.CreateInstance(__uuidof(Connection));

		_RecordsetPtr sRsEngin;
		sRsEngin.CreateInstance(__uuidof(Recordset));
		_RecordsetPtr dRsEngin;
		dRsEngin.CreateInstance(__uuidof(Recordset));
		_RecordsetPtr sRsVolume;
		sRsVolume.CreateInstance(__uuidof(Recordset));
		_RecordsetPtr dRsVolume;
		dRsVolume.CreateInstance(__uuidof(Recordset));

		CString EnginID;
		_variant_t vTmp1,vTmp,vTmp2;
		CString ConnectionString;
		ConnectionString="Provider=Microsoft.Jet.OLEDB.4.0;Persist Security Info=False;Data Source=" + basDirectory::ProjectDBDir+ddbName;
		sDb->Open((_bstr_t)ConnectionString, "", "", adConnectUnspecified);

		ConnectionString="Provider=Microsoft.Jet.OLEDB.4.0;Persist Security Info=False;Data Source=" + basDirectory::ProjectDBDir+tdbName;
		dDb->Open((_bstr_t)ConnectionString, "", "", adConnectUnspecified);
		strSQL=CString(_T("DELETE * FROM Volume"));
		dDb->Execute((_bstr_t)strSQL, NULL, adCmdText);
		strSQL=CString(_T("DELETE * FROM Engin"));
		dDb->Execute((_bstr_t)strSQL, NULL, adCmdText);
		strSQL=CString(_T("INSERT INTO Engin IN \'")) + EDIBgbl::GetDBName(dDb) + _T("\' SELECT DISTINCT gcdm AS EnginID ,gcmc,UnitNum From Engin");
		sDb->Execute((_bstr_t)strSQL, NULL, adCmdText);

// 		sRsEngin.m_pDatabase=&sDb;
// 		sRsEngin->Open((_bstr_t)(dbOpenSnapshot,_T("Select * From Engin"));
		sRsEngin->Open(_variant_t(_T("Select * From Engin")),(IDispatch*)sDb,adOpenKeyset, adLockOptimistic,adCmdText);
		sRsEngin->MoveLast();
		sRsEngin->MoveFirst();

		int i;
		_VolumeDef VolumeDef;
		char tc;
		int tf;

/*
		short Ctd;
		dDb.m_pDAODatabase ->get_TableDefs (&tdfs);
		tdfs->Refresh();
		tdfs->get_Count(&Ctd);
		tdfs->Release();
*/
		int VolumeID=1;

// 		dRsVolume.m_pDatabase=&dDb;
// 		dRsVolume->Open((_bstr_t)(dbOpenDynaset,_T("Select * From Volume"));
		dRsVolume->Open(_variant_t(_T("Select * From Volume")),(IDispatch*)dDb,adOpenKeyset, adLockOptimistic,adCmdText);
		_RecordsetPtr sRsData;
		sRsData.CreateInstance(__uuidof(Recordset));
		_RecordsetPtr dRsData;
		dRsData.CreateInstance(__uuidof(Recordset));
		_RecordsetPtr rsJD;
		rsJD.CreateInstance(__uuidof(Recordset));
		_RecordsetPtr rsZY;
		rsZY.CreateInstance(__uuidof(Recordset));

		_ConnectionPtr tmpSort;
		tmpSort.CreateInstance(__uuidof(Connection));
		tmpSort->Open((_bstr_t)(basDirectory::DBShareDir + _T("DrawingSize")), "", "", adConnectUnspecified);//20071101 "ProjectDBDir" 改为 "DBShareDir";"Sort"改为 "DrawingSize"
// 		rsJD.m_pDatabase=&tmpSort;
// 		rsZY.m_pDatabase=&tmpSort;
// 		rsJD->Open((_bstr_t)(dbOpenSnapshot,_T("Select * From DesignStage"));
// 		rsZY->Open((_bstr_t)(dbOpenSnapshot,_T("Select * From Speciality"));
		rsJD->Open(_variant_t(_T("Select * From DesignStage")),(IDispatch*)tmpSort,adOpenKeyset, adLockOptimistic,adCmdText);
		rsZY->Open(_variant_t(_T("Select * From Speciality")),(IDispatch*)tmpSort,adOpenKeyset, adLockOptimistic,adCmdText);
		int stc,sti;
		sti=0;
		stc=sRsEngin->RecordCount;
		while(!sRsEngin->adoEOF)
		{
			frmStatus.UpdateStatus((float)sti / (float)stc);
			frmStatus.UpdateWindow();
			::DoEvents();
			sti++;
			sRsEngin->get_Collect((_variant_t)(_T("gcdm")), &vTmp);
			EnginID=vtos(vTmp);
			VolumeDef.EnginID=EnginID;
			FindTable(sDb,EnginID,VoltabName,DatatabName,&vc,&dc);
			for(i=0;i<vc;i++)
			{
				tc=VoltabName[i].GetAt(VoltabName[i].GetLength()-1);
//				_variant_t vTmp;
// 				rsZY->Find((_bstr_t)(CString(_T("zydm=\'")) + tc + _T("\'")), 0, adSearchBackward);
				HRESULT hr = S_OK;
				CString strFind;
				strFind = _T("zydm=\'") + CString(tc) + "\'";
				hr = rsZY->Find((_bstr_t)strFind, 0, adSearchBackward);
				if(!rsZY->adoEOF)
				{
					rsZY->get_Collect((_variant_t)(_T("ZYID")), &vTmp);
					VolumeDef.ZYID=vtoi(vTmp);
				}
				else VolumeDef.ZYID=0;

				tf=VoltabName[i].Find(_T("-"));
				tc=VoltabName[i].GetAt(tf-1);

// 				rsJD->Find((_bstr_t)(CString(_T("sjjddm=\'")) + tc + _T("\'")), 0, adSearchBackward);
				strFind = _T("sjjddm=\'") + CString(tc) + _T("\'");
				hr = rsJD->Find((_bstr_t)strFind, 0, adSearchBackward);
				if(!rsJD->adoEOF)
				{
					rsJD->get_Collect((_variant_t)(_T("SJJDID")), &vTmp);
					VolumeDef.SJJDID=vtoi(vTmp);
				}
				else VolumeDef.SJJDID=0;

				VolumeDef.SJHYID=0;
// 				sRsVolume.m_pDatabase=&sDb;
				strSQL=_T("Select * From [") + VoltabName[i] + _T("]");
// 				sRsVolume->Open((_bstr_t)(dbOpenSnapshot,strSQL,dbForwardOnly);
				sRsVolume->Open(_variant_t(strSQL),(IDispatch*)sDb,adOpenKeyset, adLockOptimistic,adCmdText);
				while(!sRsVolume->adoEOF)
				{
					sRsVolume->get_Collect((_variant_t)(_T("jcdm")), &vTmp);
					VolumeDef.jcdm=vtos(vTmp);
					sRsVolume->get_Collect((_variant_t)(_T("jcmc")), &vTmp);
					VolumeDef.jcmc=vtos(vTmp);
					
					VolumeDef.VolumeID=VolumeID;
/*					
					// 此段没看懂
					if(VolTabAddRs(dRsVolume,VolumeDef)) 
					{
						VolumeID++;

						_RecordsetPtr tdfs;
						tdfs.CreateInstance(__uuidof(Recordset));
						tdfs = dDb->Execute((_bstr_t)"SELECT * FROM sysobjects", NULL, adCmdText);
						Ctd = tdfs->RecordCount;
						for(int ii=0;ii< Ctd;ii++)
						{
							 tdDefInfo;
							dDb.GetTableDefInfo(ii,tdDefInfo);
							if(tdDefInfo.m_strName.GetLength() ==2 )
							{
								int item=FindStr(DatatabName,dc,VoltabName[i] + tdDefInfo.m_strName);
								if(item!=-1)
								{
									//strSQL=_T("Select * From [") + DatatabName[item] + _T("] Where VolumeID=\'") + VolumeDef.jcdm + _T("\' AND Zdjh Is NOT NULL ");
									::CDaoTableDef stdf(&sDb);
									stdf->Open((_bstr_t)(CString(_T("[")) + DatatabName[item] + _T("]"));
									CString sTab;
									sTab=_T("[") + VoltabName[i] + tdDefInfo.m_strName + _T("]");
									CString SQL1=GetTblField(stdf,sTab);
									stdf->Close();
									SQL1.TrimLeft(); SQL1.TrimRight();
									if(SQL1!=_T(""))
									{
										strSQL = _T("INSERT INTO [") + tdDefInfo.m_strName + _T("]  SELECT DISTINCT ") + ltos(VolumeDef.VolumeID) + _T(" AS VolumeID,")
												+ SQL1 + _T(" FROM [") + DatatabName[item] + _T("] IN \'") + sDb) + _T("\' ")  
												+ _T(" WHERE VolumeID =\'") + VolumeDef.jcdm + _T("\' AND  ZDJH IS NOT NULL ") ;
										//MessageBox(strSQL);
										try
										{
											dDb->Execute((_bstr_t)strSQL);
											//strSQL =_T("DROP TABLE [") + VoltabName[i] + tdDefInfo.m_strName + _T("] ");
											//dDb->Execute((_bstr_t)strSQL);
										}
										catch(CException *e)
										{
#ifdef _DEBUG
											e->ReportError();
#endif
											e->Delete();
										}
									}
								}
							}
						}
					}
*/
					sRsVolume->MoveNext();
				}
				sRsVolume->Close();
			}
			sRsEngin->MoveNext();
		}

		sRsEngin->Close();
		sDb->Close();
		dDb->Close();
		CopyFile(basDirectory::ProjectDBDir+ddbName,basDirectory::ProjectDBDir+ ddbName + _T("~"),FALSE);
		DeleteFile(basDirectory::ProjectDBDir+ddbName);
		CopyFile(basDirectory::ProjectDBDir+tdbName,basDirectory::ProjectDBDir+ ddbName,FALSE);
		DeleteFile(basDirectory::ProjectDBDir+tdbName);
		AfxGetApp()->EndWaitCursor();
		return true;
	}
	catch(CException *e)
	{
		e->ReportError();
		e->Delete();
		AfxGetApp()->EndWaitCursor();
		return false;
	}
}

bool EDIBgbl::FindTable(_ConnectionPtr db, CString sTab, CString *pSTabName, CString *pSDataTabName, int *n1, int *n2)
{
	short iTmp=0,iTmp2=0;
	CString strSql;
	CString m_strName;
	try
	{
		strSql = _T("SELECT * FROM ") + sTab;

		m_strName = sTab;
		int sf;

		_RecordsetPtr rs;
		rs = db->Execute((_bstr_t)strSql, NULL, adCmdText);
		if (!rs->adoEOF)
		{
			sf=m_strName.Find(_T("-"));
			if(sf!=-1)
			{
				if(m_strName.GetLength() - (sf +1) ==1)
				{
					pSTabName[iTmp]=m_strName;
					iTmp++;
				}
				else if(m_strName.GetLength() - (sf +1) ==3)
				{
					pSDataTabName[iTmp2]= m_strName;
					iTmp2++;
				}
			}
		}
/*
		 tdfs;
		db.m_pDAODatabase ->get_TableDefs (&tdfs);
		tdfs->Refresh();

		short tdCount;
		tdfs->get_Count(&tdCount);
		tdfs->Release();
		 tdDefInfo;
		int sf;
		sTab.MakeUpper();
		for(short i=0;i<tdCount;i++)
		{
			db.GetTableDefInfo(i,tdDefInfo);
			tdDefInfo.m_strName.MakeUpper();
			if(tdDefInfo.m_strName.Find(sTab)!=-1)
			{
				sf=tdDefInfo.m_strName.Find(_T("-"));
				if(sf!=-1)
				{
					if(tdDefInfo.m_strName.GetLength() - (sf +1) ==1)
					{
						pSTabName[iTmp]=tdDefInfo.m_strName;
						iTmp++;
					}
					else if(tdDefInfo.m_strName.GetLength() - (sf +1) ==3)
					{
						pSDataTabName[iTmp2]=tdDefInfo.m_strName;
						iTmp2++;
					}
				}
			}
		}
		*/
	}
	catch(CException *e)
	{
		e->ReportError();
		e->Delete();
	*n1=iTmp;
	*n2=iTmp2;
		return FALSE;
	}
	*n1=iTmp;
	*n2=iTmp2;
	return TRUE;
}

bool EDIBgbl::VolTabAddRs(_RecordsetPtr RsVolume, _VolumeDef &VolumeDef)
{
	try
	{
		RsVolume->AddNew();
		RsVolume->put_Collect((_variant_t)_T("VolumeID"), COleVariant(VolumeDef.VolumeID));
		RsVolume->put_Collect((_variant_t)_T("EnginID"), COleVariant(VolumeDef.EnginID,VT_BSTRT));
		RsVolume->put_Collect((_variant_t)_T("jcdm"), COleVariant(VolumeDef.jcdm,VT_BSTRT));
		RsVolume->put_Collect((_variant_t)_T("jcmc"), COleVariant(VolumeDef.jcmc,VT_BSTRT));
		RsVolume->put_Collect((_variant_t)_T("SJHYID"), COleVariant(VolumeDef.SJHYID));
		RsVolume->put_Collect((_variant_t)_T("SJJDID"), COleVariant(VolumeDef.SJJDID));
		RsVolume->put_Collect((_variant_t)_T("ZYID"), COleVariant(VolumeDef.ZYID));
		
		RsVolume->Update();
		RsVolume->MoveLast();
		return TRUE;
	}
	catch(CException *e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}
}

int EDIBgbl::FindStr(CString *pStr, int Count, CString sName)
{
	CString sTmp;
	sName.MakeUpper();
	for(int i=0;i<Count;i++)
	{
		pStr[i].MakeUpper();
		if(pStr[i]==sName)
			return i;
	}
	return -1;
}

bool EDIBgbl::DataTabAddRs(_RecordsetPtr dRsData, _RecordsetPtr sRsData, long VolumeID)
{
	try
	{
		int fc=sRsData->RecordCount;
		FieldPtr fi;
		_variant_t vTmp;
		_variant_t vNul;
		vNul.ChangeType(VT_NULL);
//		DAOFields * pF;
		FieldPtr pF;
//		dRsData.m_pDAORecordset->get_Fields(&pF);
		pF = dRsData->Fields;
// 		pF->Refresh();
// 		pF->Release();
//		sRsData.m_pDAORecordset->get_Fields(&pF);
		pF = sRsData->GetFields();
// 		pF->Refresh();
// 		pF->Release();
		dRsData->AddNew();
		dRsData->put_Collect((_variant_t)_T("VolumeID"), COleVariant(VolumeID));
		FieldsPtr fields = sRsData->GetFields();
		CString strName;
		for(LONG i=0;i<fc;i++)
		{
			fields->get_Item((_variant_t)i, &fi);
			strName = (LPTSTR)fi->Name;
			if(strName !=_T("VolumeID"))
			{
				if(bFieldExists(dRsData, strName))
				{
					sRsData->get_Collect((_variant_t)i, &vTmp);
					if(vTmp.vt==VT_BSTR && vtos(vTmp)== _T(""))
						dRsData->put_Collect((_variant_t)fi->Name,vNul);
					else
						dRsData->put_Collect((_variant_t)fi->Name,vTmp);
				}
			}
		}
		dRsData->Update();
		dRsData->MoveLast();
		return TRUE;
	}
	catch(CException *e)
	{
		//e->ReportError();
		e->Delete();
		return FALSE;
	}
}

bool EDIBgbl::bFieldExists(_RecordsetPtr rs, CString sName)
{
	try
	{
		FieldPtr fi;
		FieldsPtr fields;
		fields = rs->GetFields();
		fi = fields->GetItem((_variant_t)sName);
		return TRUE;
	}
	catch(CException *e)
	{
		e->Delete();
		return FALSE;
	}
}
/*
bool EDIBgbl::bFieldExists(CDaoTableDef *rd, CString sName)
{
	try
	{
		FieldPtr fi;
		rd->GetFieldInfo(sName, &fi);
		return TRUE;
	}
	catch(CException *e)
	{
		e->Delete();
		return FALSE;
	}
}
*/
bool EDIBgbl::IsExistentQuery(_ConnectionPtr db, CString strQueryName)
{
	bool bDuplicateQueryName=true;
// 	 queryInfo;

	TRY
	{
		strQueryName.TrimLeft();strQueryName.TrimRight();
		if(strQueryName.Left(1) != _T("["))
			strQueryName= _T("[") + strQueryName;
		if(strQueryName.Right(1) != _T("]"))
			strQueryName += _T("]");
// 		 * qds;
// 		db.m_pDAODatabase->get_QueryDefs(&qds);
// 		qds->Refresh();
// 		qds->Release();
// 		db.GetQueryDefInfo(strQueryName,queryInfo);
	}
	CATCH(CException, e)
	{
// 		if(e->m_pErrorInfo->m_lErrorCode==3265)
// 			bDuplicateQueryName=false;
	}
// 	AND_CATCH(CMemoryException,e)
// 	{}
	END_CATCH
	return bDuplicateQueryName;
}
/*
CString EDIBgbl::GetTblField( &tbldef, CString tblName)
{
	try
	{
		CString ret;
		ret = _T(" ");
		FieldPtr fi;
		fi.CreateInstance(__uuidof(Field));

		DAOFields * pF;
		tbldef.m_pDAOTableDef->get_Fields(&pF);
		pF->Refresh();
		pF->Release();
		int fc=tbldef->RecordCount;
		int iTmp=0;
		for(int i=0;i<fc;i++)
		{
			tbldef.GetFieldInfo(i,fi);
			fi.m_strName.MakeUpper();
			if(fi.m_strName != _T("VOLUMEID"))
			{
				if(iTmp==0)
				{
					ret +=tblName + _T(".[") + fi.m_strName +_T("]");
					iTmp ++;
				}
				else 
				{
						ret +=_T(",") +tblName + _T(".[") + fi.m_strName + _T("]");
				}
			}
		}
		ret+=_T(" ");
		return ret;
	}
	catch(CException *e)
	{
		e->Delete();
		return CString(_T(" "));
	}
}
*/
bool EDIBgbl::ChangeDatabase(_ConnectionPtr dDb, _ConnectionPtr sDb)
{
    bool bIsChange=false;
	try
	{
		CString sTD1 , dTD1 ;
		bool bFDexists =false, bTDexists =false;
		int i =0;
		CString sTmp,SQLx;
		CString m_strName;
		FieldPtr sFD , dFD ;
		FieldsPtr pF;
		_RecordsetPtr sRS(__uuidof(Recordset));
		_RecordsetPtr dRS(__uuidof(Recordset));
		sRS = sDb->OpenSchema(adSchemaTables);
		dRS = dDb->OpenSchema(adSchemaTables);

		_RecordsetPtr sRSTable(__uuidof(Recordset));
		_RecordsetPtr dRSTable(__uuidof(Recordset));
		while (!sRS->adoEOF)
		{
			if (!strcmp((_bstr_t)sRS->Fields->GetItem("TABLE_TYPE")->Value,"TABLE"))
			{
				m_strName = sRS->Fields->GetItem("TABLE_NAME")->Value.bstrVal;
				bTDexists=false;
				SQLx= _T("SELECT * FROM [")+ m_strName+_T("]");

				dRSTable->Open(_variant_t(SQLx), (IDispatch*)sDb,adOpenKeyset, adLockOptimistic,adCmdText);
				if (!dRSTable->adoEOF)
				{
					// 表存在
					bTDexists=true;

					sRSTable->Open(_variant_t(SQLx), (IDispatch*)sDb,adOpenKeyset, adLockOptimistic,adCmdText);
					pF = sRSTable->Fields;
					int sc1= sRSTable->RecordCount;
					for (long i1=0;i1<sc1;i1++)
					{
						int dc1=dRSTable->RecordCount;
						bFDexists = false;
						for (long n1=0;n1<dc1;n1++)
						{
							dRSTable->Fields->get_Item((_variant_t)n1, &dFD);
							if ( dFD->Name == sFD->Name ) 
							{
								//字段存在，设置标志
								bFDexists = true;
								break;
							}
						}
						if ( ! bFDexists )
						{
							//不存在这个字段，则修改项目库中的表结构
							sRSTable->Fields->get_Item((_variant_t)i1, &dFD);

							dRSTable->Fields->Append(dFD->Name, dFD->Type, dFD->DefinedSize, (adoFieldAttributeEnum)dFD->Attributes);
						}
						i++;
					}
					sRSTable->Close();
				} else {
					SQLx.Format(_T("SELECT * INTO %s FROM %s "), m_strName, m_strName);
					dDb->Execute((_bstr_t)SQLx, NULL, adCmdText);
				}
				dRSTable->Close();
			}
			sRS->MoveNext();
		}
		sRS->Close();
	}
	catch(CException *e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}
	return bIsChange;
}

bool EDIBgbl::ChangeColumnsToRows(_ConnectionPtr db, CString TblName, CString OutTblName,int iRowStart,int iRowCount)
{
	//目的:将一个表的列转换成行表示，行转换成列表示。
	//典型的例子：将一个有许多字段的表tmpCalFixPhs的一个记录转换成多行，以便输出到Excel打印
	FieldPtr FdInfo;
	_RecordsetPtr rs;
	rs.CreateInstance(__uuidof(Recordset));
	_RecordsetPtr rs1;
	rs1.CreateInstance(__uuidof(Recordset));

	CString SQLx;
	_variant_t v;
	TblName.TrimLeft();TblName.TrimRight();
	
	if(TblName.Left(1)!=_T("["))
		TblName=_T("[")+TblName;
	if(TblName.Right(1)!=_T("]"))
		TblName+=_T("]");
	TblName.MakeUpper();
	try
	{
		//如果存在，重新建立
		if(EDIBgbl::tdfExists(db,OutTblName))
		{
			SQLx = _T("DROP TABLE ") + OutTblName;
			db->Execute((_bstr_t)SQLx, NULL, adCmdText);
		}
		SQLx = _T("CREATE TABLE ") + OutTblName + _T(" (Name CHAR(30),Data CHAR(40),Formula CHAR(255))");
		db->Execute((_bstr_t)SQLx, NULL, adCmdText);

		SQLx=_T("SELECT * FROM ") + OutTblName;
		if(rs1->State == adStateOpen)
			rs1->Close();
// 		rs1->Open((_bstr_t)(dbOpenDynaset,SQLx);
		rs1->Open(_variant_t(SQLx),(IDispatch*)db,adOpenKeyset, adLockOptimistic,adCmdText);

		SQLx=_T("SELECT * FROM ") + TblName;
		if(rs->State == adStateOpen)
			rs->Close();
// 		rs->Open((_bstr_t)(dbOpenSnapshot,SQLx);
		rs->Open(_variant_t(SQLx),(IDispatch*)db,adOpenKeyset, adLockOptimistic,adCmdText);
		if(rs->BOF || rs->BOF)
		{
		}
		else
		{
			rs->Move(iRowStart-1);
			if(rs->BOF || rs->adoEOF)
			{
				//移动超过起始记录
			}
			else
			{
				for(int iCount=1;iCount<=iRowCount;iCount++)
				{
					if(!rs->adoEOF)
					{
						int iFC=rs->RecordCount;						
						if(rs1->BOF && rs1->adoEOF)
						{
						}
						else
						{
							//准备重新开始将rs另外一行记录修改到rs1的另外一列
							rs1->MoveFirst();
						}

						for(long i=0;i<iFC;i++)
						{
							//把每一个字段的所有行都写完
// 							rs.GetFieldInfo(i, &FdInfo);
							rs->Fields->get_Item((_variant_t)i, &FdInfo);
							rs->get_Collect((_variant_t)i,&v);
							if(iCount==1)
							{
								rs1->AddNew();
								rs1->put_Collect((_variant_t)_T("Name"), (_variant_t)(FdInfo->Name));
								//rs1->put_Collect((_variant_t)_T("Formula"), COleVariant(_T("")));								
								rs1->put_Collect((_variant_t)_T("Data"),v);
								rs1->Update();
							}
							else
							{
//								rs1.Edit();
								rs1->put_Collect((_variant_t)_T("Data"),v);
								rs1->Update();
							}
						}
					}
				}
			}
		}
		return true;
	}
	catch(CException *e)
	{
		//ShowMessage(e->Description);
		e->ReportError();
		e->Delete();
		return false;
	}
}


BOOL EDIBgbl::UpdateSortDB()
{
	try
	{
		_ConnectionPtr sDb,dDb;
		CString strSQL;
		sDb->Open((_bstr_t)(basDirectory::TemplateDir + _T("PHScode.mdb")), "", "", adConnectUnspecified);//20071031 "sort" 改为 "PHScode.mdb"
		strSQL=_T("DELETE FROM [PhsStructureNAME]");
		sDb->Execute((_bstr_t)strSQL, NULL, adCmdText);
		strSQL=_T(" INSERT INTO [PhsStructureNAME] SELECT * FROM [PhsStructureNAME] IN \'") + basDirectory::ProjectDBDir + _T("PHScode.mdb\'");
		sDb->Execute((_bstr_t)strSQL, NULL, adCmdText);
		strSQL=_T("DELETE FROM [PhsStructureREF]");
		sDb->Execute((_bstr_t)strSQL, NULL, adCmdText);
		strSQL=_T("INSERT INTO [PhsStructureREF] SELECT * FROM [PhsStructureREF] IN \'") + basDirectory::ProjectDBDir + _T("PHScode.mdb\'");
		sDb->Execute((_bstr_t)strSQL, NULL, adCmdText);
		sDb->Close();
		
		BOOL bIsSuc=FALSE;
		int i=0;
		do
		{
			i++;
			bIsSuc=::CopyFile(basDirectory::TemplateDir + _T("PHScode.mdb") ,basDirectory::ProjectDBDir + _T("PHScode.mdb"),FALSE);
		}while(bIsSuc==FALSE && i<10);
		return TRUE;
	}
	catch(CException *e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}

}


void EDIBgbl::UpdateDBTable(_ConnectionPtr SourceDB, CString SourceTBName, _ConnectionPtr DesDB, CString DesTBName)
{
		
	int iSourceCount;
	int iDesCount;
	bool bFound ;			//标志是否找到不存在的字段
	FieldPtr SourceDBFieldInfo;
	FieldPtr DesDBFieldInfo;
	try
	{
		_RecordsetPtr tabSource(__uuidof(Recordset));
		_RecordsetPtr tabDes(__uuidof(Recordset));

		CString SQLx;
		SQLx = _T("SELECT * FROM ") + SourceTBName;

		tabSource->Open((_bstr_t)SQLx, _variant_t((IDispatch*)SourceDB,true), 
			adOpenKeyset, adLockOptimistic, adCmdText); 

		SQLx = _T("SELECT * FROM ") + DesTBName;
		tabDes->Open((_bstr_t)SQLx, _variant_t((IDispatch*)DesDB,true), 
			adOpenKeyset, adLockOptimistic, adCmdText); 

		iSourceCount = tabSource->RecordCount;
		iDesCount = tabDes->RecordCount;
		CString strSource;
		CString strDest;
		for ( long i = 0 ; i<iSourceCount;i++)
		{
			bFound = true;
//			tabSource.GetFieldInfo(i,SourceDBFieldInfo,AFX_DAO_ALL_INFO);
			tabSource->Fields->get_Item((_variant_t)i, &SourceDBFieldInfo);

			for ( long j = 0; j<iDesCount;j++)
			{
//				tabDes.GetFieldInfo(j,DesDBFieldInfo,AFX_DAO_ALL_INFO);
				tabDes->Fields->get_Item((_variant_t)i, &DesDBFieldInfo);
				strSource = (LPTSTR)SourceDBFieldInfo->Name;
				strDest = (LPTSTR)DesDBFieldInfo->Name;
				strSource.TrimLeft();
				strSource.TrimRight();
				strDest.TrimLeft();
				strDest.TrimRight();
				strSource.MakeLower();
				strDest.MakeLower();
				if ( strSource == strDest  )
				{
					bFound = false;
					break;
				}
			}
			if ( bFound )
			{				
//				tabDes.CreateField(SourceDBFieldInfo);
				tabDes->Fields->Append(SourceDBFieldInfo->Name, SourceDBFieldInfo->Type, SourceDBFieldInfo->DefinedSize, (adoFieldAttributeEnum)SourceDBFieldInfo->Attributes);
			}
		}
	}
	catch( CException* e)
	{
		e->Delete();
	}
}

void EDIBgbl::UpdateDBForUpdate()
{
}
