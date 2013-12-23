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
#include "ADORecordset.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//#include _T("msado15.tlh")


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
COleVariant EDIBgbl::DBstruArr = COleVariant();
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
COleVariant EDIBgbl::Bmk = COleVariant();
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
COleVariant EDIBgbl::FieldAttrNum = COleVariant();
//CString EDIBgbl::Btype = CString();
//int EDIBgbl::DBfields = 0;
//COleVariant EDIBgbl::LSteel = COleVariant();
const _TCHAR* EDIBgbl::YLJSBAT = _T("YLJS.bat");
CString EDIBgbl::PSA_OutDataUnit = CString();
const float EDIBgbl::kgf2N =  9.80665;
const float EDIBgbl::kgf2MPa =  0.0980665;
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
COleVariant EDIBgbl::zdjBmpNum = COleVariant();
COleVariant EDIBgbl::zdjBmp = COleVariant();
long EDIBgbl::glCurRecordNo =0;
bool EDIBgbl::pbFrmLoadedFrmStatus = FALSE;
bool EDIBgbl::pbFrmLoadedSELPDSV = FALSE;
bool EDIBgbl::pbFrmLoadedphsData = FALSE;
bool EDIBgbl::pbFrmLoadedTxsr = FALSE;
bool EDIBgbl::pbFrmLoadedPhsSamp = FALSE;

CDaoDatabase EDIBgbl::dbPRJ;
CDaoDatabase EDIBgbl::dbSORT;
CDaoDatabase EDIBgbl::dbPRJDB;
CDaoDatabase EDIBgbl::dbTable=NULL;//20071015

CDaoDatabase EDIBgbl::dbDSize=NULL;//20071018
CDaoDatabase EDIBgbl::dbMaterial=NULL;//20071018
CDaoDatabase EDIBgbl::dbPHScode=NULL;//20071018
CDaoDatabase EDIBgbl::dbSACal=NULL;//20071018

bool EDIBgbl::FdExists(_RecordsetPtr  rs, CString  strFieldName)
{
	return true;
}

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

bool EDIBgbl::InitBillType()
{
   CDaoRecordset rs;
	try{
		if(!::DirExists(basDirectory::ProjectDBDir))
		{
			frmFolderLocation.DoModal();
		}
		//20071024 10:58 (end) "ProjectDBDir" 改为 "DBShareDir"
		if(!FileExists(basDirectory::DBShareDir + _T("TableFormat.mdb"))) //2007.10.14 16:00(start) 把"Sort.mdb"改为"TableFormat.mdb"
      CopyFile(basDirectory::TemplateDir + _T("TableFormat.mdb"), basDirectory::DBShareDir + _T("TableFormat.mdb"),true);
	CDaoDatabase db;
	db.Open(basDirectory::DBShareDir+_T("TableFormat.mdb")); //20071024 10:58 (end) "ProjectDBDir" 改为 "DBShareDir"
   rs.m_pDatabase=&db;														//2007.10.14 16:00(end) 把"Sort.mdb"改为"TableFormat.mdb"
   rs.Open(dbOpenSnapshot,_T("select * from TableINFO"));
   COleVariant Var1,Var2;
   int v1;
   while(!rs.IsEOF())
   {
	   rs.GetFieldValue(_T("ID"),Var1);
	   v1=vtoi(Var1);
		if( v1 < 0) //20071026 "v1>50 ||" 删除
			continue;
		rs.GetFieldValue(_T("TableNameSuffix"),Var2);
		Btype[v1]=vtos(Var2);
		rs.GetFieldValue(_T("TableCaption"),Var2);
		Cbtype[v1].MnuCaption=vtos(Var2);
		rs.GetFieldValue(_T("menuItemVisible"),Var2);
		Cbtype[v1].MnuVisible=Var2.boolVal;
		rs.GetFieldValue(_T("menuItemEnabled"),Var2);
		Cbtype[v1].MnuEnabled=Var2.boolVal;

        rs.MoveNext();
	}
   rs.Close();
   db.Close();
	return true;
   }
	catch(::CDaoException * e)
	{
		e->ReportError();
		e->Delete();
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

		if(!dbPRJDB.IsOpen())
		{
			dbPRJDB.Open(basDirectory::DBShareDir+_T("AllPrjDB.mdb"));//20071025 "ProjectDBDir" 改为 "DBShareDir"
		}
		if(!dbPRJ.IsOpen())
		{
			dbPRJ.Open(basDirectory::ProjectDir+_T("WorkPrj.mdb"),false,false);
		}
		if(!dbSORT.IsOpen())
		{
			dbSORT.Open(basDirectory::ProjectDBDir+_T("sort.mdb"),false,false);
		}
		//20071015 10:22(start)
		if(!dbTable.IsOpen())
		{
			dbTable.Open(basDirectory::DBShareDir+_T("TableFormat.mdb"),false,false);
		}
		//20071015  10:22(end)
		//20071018(start)
		if(!dbDSize.IsOpen())
		{
			dbDSize.Open(basDirectory::DBShareDir+_T("DrawingSize.mdb"),false,false);
		}

		if(!dbMaterial.IsOpen())
		{
			dbMaterial.Open(basDirectory::ProjectDBDir+_T("Material.mdb"),false,false);
		}

		if(!dbPHScode.IsOpen())
		{
			dbPHScode.Open(basDirectory::ProjectDBDir+_T("PHScode.mdb"),false,false);
		}

		if(!dbSACal.IsOpen())
		{
			dbSACal.Open(basDirectory::ProjectDBDir+_T("SAStructureCal.mdb"),false,false);
		}
		//20071018(end)

		GetSelPrjName();
	}
	catch(::CDaoException * e)
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

void EDIBgbl::ConVertDB(CString  sDBN, CString  dDBN, COleVariant dVer, COleVariant  Pwd)
{
}

void EDIBgbl::CheckAllRecords(_RecordsetPtr rs)
{
}

void EDIBgbl::CheckAllFields(_RecordsetPtr rs)
{
	/*if(rs==NULL || rs->State==adStateClosed)
	return;
	long i,j;
	CDaoRecordset rs1;
	CString FD;
	try{
	rs1.m_pDatabase=&dbSORT;
	rs1.Open(dbOpenSnapshot,_T("SELECT FieldName,DefaultValue FROM TZA"));
	COleVariant v;
	rs1.MoveLast();
	CString sTmp;
	_variant_t tmpvar;
	for( j = rs1.GetRecordCount() - 1 ;j>= 0 ;j--)
	{
	rs1.GetFieldValue(_T("FieldName"),v);
	FD=vtos(v);
	FD.MakeUpper();

		//_T("PSaver")不取默认值,以便人工修改坐标系.
		if((FD.GetLength()==4 && (FD.Left(3)==_T("XZU") || FD.Left(3)==_T("ZZU"))) || FD==_T("PSAVER"));
		else
		{
		//柱子编号不取默认值,为空的柱号不绘制出来
		for( i = 0 ;i< rs->GetFields()->GetCount();i++)
		{
		sTmp=(char*)rs->GetFields()->Item[_variant_t((long)i)]->GetName();
		sTmp.MakeUpper();
            if(sTmp==FD)
			{

				tmpvar=rs->GetCollect(_variant_t((long)i));
               if (tmpvar.vt==VT_NULL || vtos(tmpvar) == _T(""))
			   {
				   rs1.GetFieldValue(_T("DefaultValue"),v);
					if(v.vt!=VT_BSTR)
					rs->PutCollect(_variant_t((long)i),v);
					//rs->Update();
					else
						rs->PutCollect(_variant_t((long)i),_variant_t(vtos(v)));
               }
               break;
            }
		 }
	  }
	  rs1.MovePrev();
   }
	}
	catch(::CDaoException * e)
	{
		e->ReportError();
		e->Delete();
	}
	catch(_com_error &e)
	{
		ShowMessage(e.Description());
	}*/
}

void EDIBgbl::InitDBTBN(CString  &strSQL)
{
	CString tbn;
   CADORecordset rs;

	CDaoRecordset rs1;
   try
   {

	if(FrmTxsr.m_pViewTxsr->m_bIsInit)
		FrmTxsr.m_pViewTxsr->CloseRs();
	   _variant_t tmpvar;
		_variant_t v;
		COleVariant v2;
		rs.m_ActiveConnection=::conPRJDB;
		rs.Open(_T("Select * From [Volume] Where VolumeID=") + ltos(EDIBgbl::SelVlmID));
		if(rs.IsEOF() && rs.IsBOF())
		{

			rs.Close();
			rs.Open(_T("Select * From [Volume] "));
			rs.GetFieldValue(_T("VolumeID"),v);
			SelVlmID=vtoi(v);
		}
		rs.GetFieldValue(_T("EnginID"),v);
		EDIBgbl::SelPrjID=vtos(v);
		rs.GetFieldValue(_T("JCDM"),v);
		EDIBgbl::SelJcdm=vtos(v);
		rs.GetFieldValue(_T("SJHYID"),v);
		EDIBgbl::SelHyID=vtoi(v);
		rs.GetFieldValue(_T("ZYID"),v);
		EDIBgbl::SelSpecID =vtoi(v);
		rs.GetFieldValue(_T("SJJDID"),v);
		EDIBgbl::SelDsgnID=vtoi(v);
		rs.Close();
		rs1.m_pDatabase=&EDIBgbl::dbDSize;//20071018 "dbSORT" 改为 "dbDSize"
		SQLx.Format(_T("SELECT [zymc] FROM [Speciality] WHERE ZYID=%d "),EDIBgbl::SelSpecID);
		rs1.Open(dbOpenSnapshot,SQLx);
		if(!rs1.IsBOF() && ! rs1.IsEOF())
		{
			rs1.GetFieldValue(0,v2);
			EDIBgbl::SelSpecName=vtos(v2);
		}
		else
		{CString str;
		str.LoadString(IDS_HOTENGINE);
			EDIBgbl::SelSpecName=str;
		}
		rs1.Close();


		SQLx.Format(_T("SELECT [sjjdmc] FROM [DesignStage] WHERE SJJDID=%d "),EDIBgbl::SelDsgnID);
		rs1.Open(dbOpenSnapshot,SQLx);
		if(!rs1.IsBOF() && ! rs1.IsEOF())
		{
			rs1.GetFieldValue(0,v2);
			EDIBgbl::SelDsgnName=vtos(v2);
		}
		else
		{CString str;
		str.LoadString(IDS_SHOP_DRAWING);
			EDIBgbl::SelDsgnName=str;
		}
		rs1.Close();

		SQLx.Format(_T("SELECT [SJHY] FROM [DrawSize] WHERE SJHYID=%d "),EDIBgbl::SelHyID);
		rs1.Open(dbOpenSnapshot,SQLx);
		if(!rs1.IsBOF() && ! rs1.IsEOF())
		{
			rs1.GetFieldValue(0,v2);
			EDIBgbl::SelHyName=vtos(v2);
		}
		else
		{CString str;
		str.LoadString(IDS_ELECTRIC_POWER);
			EDIBgbl::SelHyName=str;
		}
		rs1.Close();


		//TBNSelPrjSpec = SelPrjID+strSelDsgn+_T("-")+strSelSpec;
		CString sTmpNo;
		sTmpNo = EDIBAcad::GetDrawIDAndName(modPHScal::zdjh + modPHScal::glDrawNoStartNumber - 1, EDIBDB::DrawName);
		//下句修改原始表ZA成功标志bCalSuccess，使其与结果表ZB一致。
		SQLx=_T("Update [") + Btype[TZA] + _T("] SET bCalSuccess =0 WHERE VolumeID=") +  ltos(SelVlmID) + _T(" AND NOT EXISTS ( SELECT ZDJH FROM [") + Btype[TZB] 
			+_T("] WHERE [") + Btype[TZA] + _T("].VolumeID = [") + Btype[TZB] + _T("].VolumeID AND [")
		   + Btype[TZA] + _T("].ZDJH = [") + Btype[TZB] + _T("].ZDJH)");
		EDIBgbl::dbPRJDB.Execute(SQLx);

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
		rs.m_ActiveConnection=conPRJDB;
		rs.Open(SQLx);
		if(rs.IsEOF() && rs.IsBOF() )
		{
			//如果数据库为空，则添加一个新记录作为示范，
			//使用户易于掌握输入格式
			rs.AddNew();
		  rs.SetFieldValue(_T("VolumeID"),_variant_t(SelVlmID));
		  CString strPart;
		  switch(SelBillType)
		  {
		  case TML:
				rs.SetFieldValue(_T("SEQ"),_variant_t((long)1));
				rs.SetFieldValue(_T("DRAWNO"),_variant_t(TBNSelPrjSpec+SelJcdm+_T("01")));
			  
			  rs.SetFieldValue(_T("DRAWNA"),_variant_t(GetResStr(IDS_ASSISTANTINSTALL_FIRSTCHART)));
				  break;
		  case TCL:
		  case TLJ:
			rs.SetFieldValue(_T("CLID"),_variant_t(_T("DD1404-01")));
			rs.SetFieldValue(_T("CLmc"),_variant_t(GetResStr(IDS_JOINTING_ELBOW)));
			rs.SetFieldValue(_T("CLgg"),_variant_t(_T("Dn600R1900A30")));
			rs.SetFieldValue(_T("CLcl"),_variant_t(_T("Q235-A")));
			strPart.Format(_T("%s"),_T("件"));
			rs.SetFieldValue(_T("CLdw"),_variant_t(strPart));
			rs.SetFieldValue(_T("CLdz"),_variant_t((float)152.8));
			rs.SetFieldValue(_T("CLnum"),_variant_t((float)8));
			rs.SetFieldValue(_T("CLzz"),_variant_t((float)(152.8 * 8.0)));
			break;
		  case TZD:
				rs.SetFieldValue(_T("ZDID"),_variant_t(EDIBAcad::GetDrawIDAndName(gStartPage, strSQL)));
				
				rs.SetFieldValue(_T("ZDmc"),_variant_t(GetResStr(IDS_FIXUP_BRACKET)));
				rs.SetFieldValue(_T("ZDnum"),_variant_t((float)8));
				rs.SetFieldValue(_T("ZDgg"),_variant_t(_T("%%c1520x5")));
				rs.SetFieldValue(_T("jghz"),_variant_t((long)18000));
				rs.SetFieldValue(_T("ZDbz"),_variant_t(GetResStr(IDS_INPUTREMARK)));
				break;
		  case TSC:
				rs.SetFieldValue(_T("SCID"),_variant_t(_T("Y630-2-4")));
				rs.SetFieldValue(_T("SCmc"),_variant_t(GetResStr(IDS_ELECTROMOTOR)));
				break;
		  case TZA:
			  rs.SetFieldValue(_T("ZDJH"),_variant_t((long)1));
			  rs.SetFieldValue(_T("JSDBH1"),_variant_t((long)1));
			  rs.SetFieldValue(_T("IDdn1"),_variant_t(_T("D2")));
			  rs.SetFieldValue(_T("IDgn1"),_variant_t(_T("G21")));
			  rs.SetFieldValue(_T("dj1"),_variant_t((float)108));
			  rs.SetFieldValue(_T("t01"),_variant_t((float)300));
			  rs.SetFieldValue(_T("pgz1"),_variant_t((float)100));
			  rs.SetFieldValue(_T("pjg1"),_variant_t((float)150));
			  rs.SetFieldValue(_T("iSelSampleID"),_variant_t((float)1));
			  //rs.Update();
					//支吊架原始数据表，再添加一条记录，显示图形化输入窗体后才不易出错。
			  /*rs.AddNew();
			  v.fltVal=2;
			  rs.SetFieldValue(_T("ZDJH"),v);
			  rs.SetFieldValue(_T("JSDBH1"),v);
			  rs.SetFieldValue(_T("IDdn1"),_variant_t(_T("D2")));
			  rs.SetFieldValue(_T("IDgn1"),_variant_t(_T("G21")));
			  v.fltVal=89;
			  rs.SetFieldValue(_T("dj1"),v);
			  v.fltVal=200;
			  rs.SetFieldValue(_T("t01"),v);
			  v.fltVal=100;
			  rs.SetFieldValue(_T("pgz1"),v);
			  v.fltVal=150;
			  rs.SetFieldValue(_T("pjg1"),v);
			  v.ChangeType(VT_I4);
			  v.lVal=1;
			  rs.SetFieldValue(_T("iSelSampleID"),v);*/
			  break;
		  }
			rs.Update();
		}
		rs.Close();
		FrmTxsr.m_pViewTxsr->m_SQL=SQLx;
		

		if(FrmTxsr.m_pViewTxsr->m_bIsInit)
			FrmTxsr.m_pViewTxsr->InitRs();
   }
   catch(::CDaoException * e)
   {
		e->ReportError();
	   e->Delete();
	   //ShowMessage(CString((char*)e.ErrorMessage())+(char*)e.Description());
   }
   catch(::COleException * e)
   {
		e->ReportError();
	   e->Delete();
	   //ShowMessage(CString((char*)e.ErrorMessage())+(char*)e.Description());
   }
	catch(_com_error e)
	{
		ShowMessage(e.Description());
	}
	catch(CException *e)
	{
		e->Delete();
	}
	catch(...)
	{
	}

   //dbPRJDB.Recordsets.Refresh
   //////////1
   //modPHScal::Crs.rsSource = dbPRJDB.OpenRecordset(SQLx, dbOpenDynaset)*/
}

 void EDIBgbl::CloseAll_ConnectionPtr()
 {
 	try{
		/*if(dbPRJ.IsOpen())
		{
 			dbPRJ.Close();
		}
		if(dbPRJDB.IsOpen())
		{
 			dbPRJDB.Close();
		}
		if(dbSORT.IsOpen() )
		{
 			dbSORT.Close();
		}*/
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
 	catch(::CDaoException * e)
 	{
 		e->ReportError();
 		e->Delete();
 	}
 	catch(_com_error &e)
 	{
 		ShowMessage(CString((char*)e.ErrorMessage())+CString((char*)e.Description()));
 	}
 }
CString EDIBgbl::GetDBName(CDaoDatabase& db)
{
	return db.GetName();
}
void EDIBgbl::LoadMenuSelBill(CMenu* mnuObj)
{
/*	if(mnuObj==NULL)
		return;
	CMenu tmpMnu;
	mnuObj->SetM
	mnuObj[0].mnuCaption = Cbtype[0].mnuCaption;
	CString stmp;
   for(int i = LBound[Btype] ;i<= UBound[Btype];i++)
   {
	   stmp=Btye[i];
	   stmp.TrimLeft();
	   stmp.TrimRight();
      if (i > 0 && i<51 && stmp!=_T(""))
	  {
         //Load mnuObj[i]
         mnuObj[i].Caption = Cbtype[i].MnuCaption;
         mnuObj[i].Visible = Cbtype[i].MnuVisible;
         mnuObj[i].Enabled = Cbtype[i].MnuEnabled;
         
         if(i = SelBillType) 
            mnuObj[i].Checked = vbChecked;
	  }
	 }
   mnuObj[TZX].Enabled = false;
   mnuObj[TZF].Enabled = false;*/
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
		CDaoRecordset rs;
		CString SQLx;
		int i=0;
		if (tbn==_T(""))
			SQLx = _T("SELECT * FROM T") + EDIBgbl::Btype[EDIBgbl::SelBillType] + _T(" WHERE trim(LocalCaption)<>\'\'");
		else
			SQLx = _T("SELECT * FROM [") + tbn + _T("] WHERE trim(LocalCaption)<>\'\'");
		
		//float Width;
		//COleVariant sngWidth;
		float sngWidth;
		_variant_t ix;
		ix.ChangeType(VT_I4);
		ix.intVal =ColIndex;
		rs.m_pDatabase = &EDIBgbl::dbSORT;
		
		rs.Open(dbOpenDynaset,SQLx);
		CString sTmp;
		sTmp=MyDBGrid.GetColumns().GetItem(ix).GetDataField();
		sTmp.TrimLeft();sTmp.TrimRight();sTmp.MakeUpper();
		rs.FindFirst (_T("ucase(trim(FieldName))=\'") + sTmp + _T("\'"));
		sngWidth=MyDBGrid.GetColumns().GetItem(ix).GetWidth()*20;
		//twips/20->pixels
		ix.ChangeType(VT_R4);
		ix.fltVal=sngWidth;
		rs.Edit();
		rs.SetFieldValue(_T("width"),ix);
		rs.Update();
	}
	catch(CString e)
	{
		ShowMessage(e);
	}
	catch(CDaoException *e)
	{
		e->ReportError();
		e->Delete();
	}
}

void EDIBgbl::SetDBGridColumnCaptionAndWidth(CDataGrid& MyDBGrid, CString  tbn)
{
	CDaoRecordset rs; int i,iWC;
	try{
	_variant_t tmpvar;
	COleVariant v;
	tbn.TrimLeft();
	tbn.TrimRight();
   if (tbn==_T(""))
      SQLx = _T("SELECT * FROM T") + Btype[SelBillType] + _T(" WHERE trim(LocalCaption)<>\'\'");
   else
      SQLx = _T("SELECT * FROM ") + tbn + _T(" WHERE trim(LocalCaption)<>\'\'");
   rs.m_pDatabase=&dbTable;//20071015 "dbSORT"改为"dbTable"
   rs.Open(dbOpenSnapshot,SQLx);

   
   _variant_t ix;
   ix.ChangeType(VT_I4);
   CString sTmp;
   for(i=0;i<MyDBGrid.GetColumns().GetCount();i++)
   {
	   ix.intVal=i;
	   sTmp=MyDBGrid.GetColumns().GetItem(ix).GetDataField();
	   sTmp.MakeUpper();
        if(rs.FindFirst(CString(_T("Trim(FieldName)=\'")) + sTmp + _T("\'")))
		{
			tmpvar.ChangeType(VT_I4);
			rs.GetFieldValue(_T("LocalCaption"),v);
			MyDBGrid.GetColumns().GetItem(ix).SetCaption(vtos(v));
			
			rs.GetFieldValue(_T("width"),v);
			if(v.vt==VT_NULL)
				continue;
			iWC=vtoi(v);
			iWC=iWC>0 ? iWC : 2000;
			MyDBGrid.GetColumns().GetItem(ix).SetWidth((float)iWC/20);
      }
   }
	}
	catch(::CDaoException * e)
	{
		e->ReportError();
		e->Delete();
	}
}

COleVariant EDIBgbl::va(/*Object Obj*/)
{
	return COleVariant();
}

void EDIBgbl::LocateFrm(CWnd* frm, COleVariant  bDisplayWindow)
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
		CDaoRecordset rs;
		CDaoRecordset rs2;

		//获得所有含有属性的块的属性值,便于以后输出

		_variant_t tmpVar;
		//取得工程名
		if(dbPRJDB.IsOpen())
		{
			SQLx = _T("SELECT * FROM engin WHERE ucase(trim(EnginID))=\'") + SelPrjID + _T("\' AND NOT IsNull(gcmc) AND trim(gcmc)<>\'\'");
			rs.m_pDatabase=&dbPRJDB;
			rs.Open(dbOpenDynaset,SQLx);
			COleVariant v;
			if(!rs.IsEOF())
			{
				rs.GetFieldValue(_T("gcmc"),v);
				SelPrjName=vtos(v);

				rs.GetFieldValue(_T("e_gcmc"),v);
				SelPrjEnglishName=vtos(v);

			}
			SelPrjName.TrimLeft();
			SelPrjName.TrimRight();

			SelPrjEnglishName.TrimLeft();
			SelPrjEnglishName.TrimRight();

			rs.Close();
		}
		
		//取得设计阶段
		COleVariant v;
		if(dbDSize.IsOpen())//20071018 11:29(start)"dbDSize"改为"dbDSize"
		{
			rs2.m_pDatabase=&dbDSize;
			SQLx = _T("SELECT * FROM DesignStage WHERE ucase(trim(sjjdid))=") + ltos(SelDsgnID) + _T(" AND NOT IsNull(sjjddm) AND trim(sjjddm)<>\'\'") + _T(" AND trim(SJHYid)=") + ltos(SelHyID);
			rs2.Open(dbOpenSnapshot,SQLx);
			if(!rs2.IsEOF())
			{
				rs2.GetFieldValue(_T("sjjdmc"),v);
				SelDsgnName=vtos(v);
			}
			SelDsgnName.TrimLeft();
			SelDsgnName.TrimRight();
			rs2.Close();
			
			
			//取得专业名称
			SQLx = _T("SELECT * FROM Speciality WHERE trim(zyid)=") + ltos(SelSpecID) + _T(" AND trim(SJHYid)=") + ltos(SelHyID);
			rs2.Open(dbOpenSnapshot,SQLx);
			if(!rs2.IsEOF())
			{
				rs2.GetFieldValue(_T("zymc"),v);
				SelSpecName=vtos(v);
			}
			SelSpecName.TrimLeft();
			SelSpecName.TrimRight();
			rs2.Close();
		}//20071018 11:29(end)"dbDSize"改为"dbDSize"
		//取得卷册名称
		if(dbPRJDB.IsOpen())
		{
			SQLx = _T("SELECT * FROM [Volume] WHERE VolumeID=") + ltos(SelVlmID) + _T(" ORDER BY jcdm");
			rs.m_pDatabase=&dbPRJDB;
			rs.Open(dbOpenDynaset,SQLx);
			COleVariant v;
			if(!rs.IsEOF())
			{
				rs.GetFieldValue(_T("jcmc"),v);
				SelVlmName=vtos(v);
			}
			SelVlmName.TrimLeft();
			SelVlmName.TrimRight();
			rs.Close();

			SQLx.Format("SELECT SUM(CLzz) AS SumWeight FROM [ZB] WHERE [VolumeID]=%d AND [ZDJH]=%d AND IsSAPart<>-1",EDIBgbl::SelVlmID,modPHScal::zdjh);
		
			rs.m_pDatabase=&dbPRJDB;
			rs.Open(dbOpenDynaset,SQLx);
			if(!rs.IsEOF())
			{
				rs.GetFieldValue(_T("SumWeight"),v);
				EDIBAcad::g_fSumWeight=vtof(v);
			}
			rs.Close();
		}
	}
	catch(::CDaoException * e)
	{
		e->ReportError();
		e->Delete();
	}
}



CString EDIBgbl::GetDBName(_ConnectionPtr db)
{
	if(db==NULL || db->GetState()==adStateClosed)
		return _T("");
		  PropertiesPtr prP;

		  prP=db->GetProperties();
		  CString dbName=vtos(prP->GetItem( _variant_t(_T("Data Source")))->GetValue());
		  return dbName;

}

bool EDIBgbl::tdfExists(CDaoDatabase &db, CString tbn)
{
	//11/3
	if(db.m_pDAODatabase==NULL||tbn.IsEmpty())
	{
		return false;
	}
	CDaoRecordset TempSet;
	CString strSQL;
	strSQL="select * from "+tbn+"";
	//11/3
/*11/3	CDaoTableDefInfo tbInfo;
	tbn.TrimLeft();tbn.TrimRight();
	
	if(tbn.Left(1)!=_T("["))
		tbn=_T("[")+tbn;
	if(tbn.Right(1)!=_T("]"))
		tbn+=_T("]");
	tbn.MakeUpper();//11/3*/
	try{
	/*11/1	DAOTableDefs * tdfs=NULL;
		db.m_pDAODatabase->get_TableDefs(&tdfs);
		tdfs->Refresh();
		tdfs->Release();
		db.GetTableDefInfo(tbn,tbInfo);
		return true;   * /11/1
		/*CDaoTableDef tdf(&db);
		tdf.Open(tbn);
		tdf.Close();*/
		//11/3
		TempSet.m_pDatabase=&db;
		TempSet.Open(dbOpenSnapshot,strSQL);
		return true;
		//11/3
/*11/3		int i,c;
		CString strTbn;
		c=db.GetTableDefCount();
		for(i=0;i<c;i++)
		{
			db.GetTableDefInfo(i,tbInfo);
			strTbn=tbInfo.m_strName;
			strTbn.TrimLeft();strTbn.TrimRight();
			if(strTbn.Left(1)!=_T("["))
				strTbn=_T("[")+strTbn;
			if(strTbn.Right(1)!=_T("]"))
				strTbn+=_T("]");
			strTbn.MakeUpper();			
			if(tbn==strTbn)
				return true;
		}
		return false;//11/3*/
	}
	catch(::CDaoException * e)
	{
		//e->ReportError();
		e->Delete();
		return false;
	}	 
}

void EDIBgbl::InitWorkTable(CDaoDatabase &db, CString tbn, int type)
{
	/*tbn.TrimLeft();tbn.TrimRight();
	if(!db.IsOpen() || tbn==_T(""))
		return;
	if(tbn.Left(1)!=_T("["))
		tbn=_T("[")+tbn;
	if(tbn.Right(1)!=_T("]"))
		tbn+=_T("]");
	CString sql1,sql2;
	sql1=_T("delete from ")+tbn;
	sql2=CString(_T("create index PrimaryKey on "))+tbn;
	switch(type)
	{
	/*case TML:
	case TCL:
		sql1+=_T(" where IsNull(VolumeID) OR IsNull(zdjh)");
		sql1+=_T("(VolumeID,zdjh) with PRIMARY");
		break;*/
/*	case TZA:
		sql1+=_T(" where IsNull(VolumeID) OR IsNull(zdjh)");
		sql2+=_T("(VolumeID,zdjh) with PRIMARY");
		break;
	}
	try
	{
		db.Execute(sql1);
	}
	catch(::CDaoException* e)
	{
		e->Delete();
	}
	try
	{
		db.Execute(sql2);
	}
	catch(::CDaoException* e)
	{
		e->Delete();
	}*/
	/*try{
		::CDaoFieldInfo fi;
		::CDaoTableDef tbd(&db);
		tbd.Open(tbn);
		int C = tbd.GetFieldCount();
		for(int i=0 ;i<C;i++)
		{
			tbd.GetFieldInfo(i,fi);
			if(fi.m_strName!=_T("VolumeID") && fi.m_strName!=_T("zdjh") && fi.m_nType==dbText)
			{
				fi.m_bAllowZeroLength=TRUE;
				CDaoTableDef::m_pDAOTableDef*/
}



CStringList* GetTableNameList(CDaoDatabase *pDB)
{
/*
* 作者：罗峰
* 功能：升级数据库
* 日期：2003-07-18
*/
	ASSERT_VALID(pDB);
	CStringList *tableNameList;
	CDaoTableDefInfo tDefInfo;
	tableNameList = new CStringList();
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
	return tableNameList;
}

CStringList* GetFieldNameList(CDaoTableDef *pTDef)
{
/*
* 作者：罗峰
* 功能：升级数据库
* 日期：2003-07-18
*/
	ASSERT_VALID(pTDef);
	CStringList *pNameList;
	CDaoFieldInfo tDefInfo;
	pNameList = new CStringList();
	int iCount = pTDef->GetFieldCount();
	for(int i=0; i<iCount; i++)
	{
		pTDef->GetFieldInfo(i, tDefInfo);
		pNameList->AddTail(tDefInfo.m_strName);
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
	

	CDaoDatabase sDB, dDB;
	CDaoTableDef sTDef(&sDB), dTDef(&dDB);
	CDaoFieldInfo fieldInfo,fieldInfoS,fieldInfoD;
	CString strSql;
	try
	{
		sDB.Open(strSourceDB);
		dDB.Open(strDestDB);
		CStringList *sListTableName, *dListTableName, *sListFieldName, *dListFieldName;
		sListTableName = GetTableNameList(&sDB);
		dListTableName = GetTableNameList(&dDB);
		for(POSITION pos = sListTableName->GetHeadPosition(); pos != NULL; )
		{
			CString strTableName = sListTableName->GetNext(pos);
			if(StringInList(strTableName, dListTableName))
			{
				sTDef.Open(strTableName);
				dTDef.Open(strTableName);
				sListFieldName = GetFieldNameList(&sTDef);
				dListFieldName = GetFieldNameList(&dTDef);	
				for(POSITION pos = sListFieldName->GetHeadPosition(); pos != NULL; )
				{
					CString strFieldName = sListFieldName->GetNext(pos);
					if(!StringInList(strFieldName, dListFieldName))
					{
						sTDef.GetFieldInfo(strFieldName, fieldInfo);
						fieldInfo.m_bAllowZeroLength = FALSE;
						fieldInfo.m_bRequired = FALSE;
						dTDef.CreateField(fieldInfo);
					}
					else
					{
						sTDef.GetFieldInfo(strFieldName, fieldInfoS);
						dTDef.GetFieldInfo(strFieldName, fieldInfoD);
						if(fieldInfoS.m_lSize != fieldInfoD.m_lSize)
						{
							//改变字段大小
							strSql.Format("ALTER TABLE %s ALTER COLUMN %s VARCHAR(%d)",strTableName,strFieldName,fieldInfoS.m_lSize);
							
							::conPRJDB4->Execute(_bstr_t(strSql),NULL,adCmdText);
						}
					
					}
				}
				sListFieldName->RemoveAll();
				dListFieldName->RemoveAll();
				delete sListFieldName;
				delete dListFieldName;
				sTDef.Close();
				dTDef.Close();
			}
			else
			{
				CString	SQLx;
				SQLx.Format(_T("SELECT * INTO %s FROM %s IN \'%s\'"),
					strTableName,strTableName,sDB.GetName());
				dDB.Execute(SQLx);	
			}			

		}
		sListTableName->RemoveAll();
		dListTableName->RemoveAll();
		delete sListTableName;
		delete dListTableName;
		sDB.Close();
		dDB.Close();
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
	CDaoDatabase sDB, dDB;
	CDaoFieldInfo fieldInfo;
	try
	{
		sDB.Open(strSourceDB, FALSE, FALSE, _T(";pwd=") + strSourcePwd + _T(";"));
		dDB.Open(strDestDB, FALSE, FALSE, _T(";pwd=") + strDestPwd + _T(";"));
		CStringList *dListTableName, *sListFieldName, *dListFieldName;
		dListTableName = GetTableNameList(&dDB);
		for(POSITION pos = sListTableName->GetHeadPosition(); pos != NULL; )
		{
			CString strTableName = sListTableName->GetNext(pos);
			if(StringInList(strTableName, dListTableName))
			{
				CDaoTableDef sTDef(&sDB), dTDef(&dDB);
				sTDef.Open(strTableName);
				dTDef.Open(strTableName);
				sListFieldName = GetFieldNameList(&sTDef);
				dListFieldName = GetFieldNameList(&dTDef);	
				for(POSITION pos = sListFieldName->GetHeadPosition(); pos != NULL; )
				{
					CString strFieldName = sListFieldName->GetNext(pos);
					if(!StringInList(strFieldName, dListFieldName))
					{
						sTDef.GetFieldInfo(strFieldName, fieldInfo);
						fieldInfo.m_bAllowZeroLength = FALSE;
						fieldInfo.m_bRequired = FALSE;
						dTDef.CreateField(fieldInfo);
					}
				}
				sListFieldName->RemoveAll();
				dListFieldName->RemoveAll();
				delete sListFieldName;
				delete dListFieldName;
				sTDef.Close();
				dTDef.Close();
			}
			else
			{
				CString	SQLx;
				SQLx.Format(_T("SELECT * INTO %s FROM %s IN \'%s\'"),
					strTableName,strTableName,sDB.GetName());
				dDB.Execute(SQLx);	
			}
		}
		sListTableName->RemoveAll();
		dListTableName->RemoveAll();
		delete sListTableName;
		delete dListTableName;
		sDB.Close();
		dDB.Close();
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
	CDaoDatabase db;
	CStringList *listTableName;
	CString strTableName;
	POSITION pos1,pos2;
	int iLen;
	iLen = strFilter.GetLength();
	try
	{
		db.Open(strDBPath, FALSE, FALSE, _T(";pwd=") + strPwd + _T(";"));
		listTableName = GetTableNameList(&db);
		for( pos1 = listTableName->GetHeadPosition(); (pos2=pos1)!=NULL; )
		{
			strTableName = listTableName->GetNext(pos1);
			if(_tcscmp(strTableName.Left(iLen), strFilter))
			{
				listTableName->RemoveAt(pos2);
			}
		}
		db.Close();
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
			CDaoDatabase sDb , dDb ;
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
			CDaoRecordset drsTmp;
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
			//其次升级workprj.mdb
			if ( FileExists(basDirectory::TemplateDir  +  _T("workprj.mdb")) && FileExists(basDirectory::ProjectDir  +  _T("workprj.mdb")) ) 
			{
				sDb.Open(basDirectory::TemplateDir  +  _T("workprj.mdb"));
				dDb.Open(basDirectory::ProjectDir  +  _T("workprj.mdb"));
				ChangeDatabase(dDb,sDb);
				dDb.Close();
				sDb.Close();
			}
			dDb.Open(basDirectory::DBShareDir+ _T("AllPrjDB.mdb"));//20071031 "ProjectDBDir" 改为 "DBShareDir"
			if(!EDIBgbl::tdfExists(dDb,_T("ZA")) || !EDIBgbl::tdfExists(dDb,_T("ZB")))
			{
				dDb.Close();
				if(!UpdateAllPrjDB())
				{
					return false;
				}
				dDb.Open(basDirectory::DBShareDir+ _T("AllPrjDB.mdb"));//20071031 "ProjectDBDir" 改为 "DBShareDir"
			}
		::CDaoTableDef tmptdf(&dDb);
		tmptdf.Open(_T("ML"));
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
		catch(...)
		{
		}
		tmptdf.Close();
		sDb.Open(basDirectory::TemplateDir  +  _T("AllPrjdb.mdb"));
		//按F4511s.mdb结构升级AllPrjdb.mdb中的表的结构
		ChangeDatabase(dDb,sDb);
		//升级ZB表中以前根部附件(recno=NULL)特性字段:recno=NULL升级为IsSAPart=-1,recno=根部recno依次+1
		SQLx.Format(_T("SELECT recno,VolumeID,zdjh,seq,nth,IsSAPart FROM ZB ORDER BY VolumeID,zdjh,recno DESC"));
		drsTmp.m_pDatabase=&dDb;
		//recno按降序排列，则根部排在最前，根部附件排在最后
		drsTmp.Open(dbOpenDynaset,SQLx);
		int maxRecno=1;//current zdjh maxmium recno
		int iPrevZdjh=-1;//previous zdjh
		COleVariant v;
		while(!drsTmp.IsEOF())
		{
			drsTmp.GetFieldValue(_T("zdjh"),v);
			if(iPrevZdjh==vtoi(v))
			{
				iPrevZdjh=vtoi(v);
				maxRecno++;
			}
			else
			{
				iPrevZdjh=vtoi(v);
				drsTmp.GetFieldValue(_T("recno"),v);
				maxRecno=vtoi(v);
			}
			drsTmp.GetFieldValue(_T("recno"),v);
			if(v.vt==VT_NULL)
			{
				//更新旧的根部附件
				drsTmp.Edit();
				drsTmp.SetFieldValue(_T("IsSAPart"),COleVariant((short)-1));
				drsTmp.SetFieldValue(_T("recno"),COleVariant((long)maxRecno));
				drsTmp.Update();
			}
			drsTmp.GetFieldValue(_T("nth"),v);
			if(v.vt==VT_NULL)
			{
				//更新旧的根部附件
				drsTmp.Edit();
				drsTmp.SetFieldValue(_T("nth"),COleVariant((short)1));//标准支吊架路数=1，包括对称双吊和共用根部双吊
				drsTmp.Update();
			}
			
			drsTmp.MoveNext();
		}

		CDaoFieldInfo fi;
		tmptdf.Open(_T("ZA"));
		
		tmptdf.GetFieldInfo(_T("DwgName"),fi,AFX_DAO_ALL_INFO);
		if(fi.m_lSize<255)
		{
			fi.m_lSize=255;
			if(!bFieldExists(&tmptdf,_T("tmpDwgName")))
			{
				fi.m_strName=_T("tmpDwgName");
				tmptdf.CreateField(fi);
			}

			dDb.Execute(_T("Update [ZA] SET [tmpDwgName] = [DwgName] "));
			tmptdf.DeleteField(_T("DwgName"));

			fi.m_strName=_T("DwgName");
			tmptdf.CreateField(fi);

			dDb.Execute(_T("Update [ZA] SET [DwgName] = [tmpDwgName] "));
			tmptdf.DeleteField(_T("tmpDwgName"));
		}
		tmptdf.Close();

		tmptdf.Open(_T("ZG"));

		tmptdf.GetFieldInfo(_T("Handle"),fi,AFX_DAO_ALL_INFO);
		if(fi.m_lSize<16)
		{
			fi.m_lSize=16;
			if(!bFieldExists(&tmptdf,_T("tmpHANDLE")))
			{
				fi.m_strName=_T("tmpHANDLE");
				tmptdf.CreateField(fi);
			}

			dDb.Execute(_T("Update [ZG] SET [tmpHandle] = [Handle] "));
			tmptdf.DeleteField(_T("Handle"));

			fi.m_strName=_T("HANDLE");
			tmptdf.CreateField(fi);

			dDb.Execute(_T("Update [ZG] SET [Handle] = [tmpHandle] "));
			tmptdf.DeleteField(_T("tmpHandle"));
		}
		tmptdf.Close();

		::SetRegValue(_T("Status"), _T("Install"), _T("1"));
		return true;
		}
		return true;
	}
	
	catch(CDaoException *e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}
}
bool EDIBgbl::bIndexExists(CDaoTableDef* pTd,CString strIndex)
{
	int c=pTd->GetIndexCount();
	CDaoIndexInfo info;
	strIndex.MakeUpper();
	for(int i=0;i<c;i++)
	{		
		pTd->GetIndexInfo(i,info);
		info.m_strName.MakeUpper();
		if(info.m_strName==strIndex)
			return true;
	}
	return false;
}

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
		CDaoDatabase sDb,dDb;
		CDaoRecordset sRsEngin,dRsEngin,sRsVolume,dRsVolume;
		CString EnginID;
		COleVariant vTmp1,vTmp,vTmp2;
		sDb.Open(basDirectory::ProjectDBDir+ddbName);
		dDb.Open(basDirectory::ProjectDBDir+tdbName);
		strSQL=CString(_T("DELETE * FROM Volume"));
		dDb.Execute(strSQL);
		strSQL=CString(_T("DELETE * FROM Engin"));
		dDb.Execute(strSQL);
		strSQL=CString(_T("INSERT INTO Engin IN \'")) + dDb.GetName() + _T("\' SELECT DISTINCT gcdm AS EnginID ,gcmc,UnitNum From Engin");
		sDb.Execute(strSQL);

		sRsEngin.m_pDatabase=&sDb;
		sRsEngin.Open(dbOpenSnapshot,_T("Select * From Engin"));
		sRsEngin.MoveLast();
		sRsEngin.MoveFirst();

		int i;
		_VolumeDef VolumeDef;
		char tc;
		int tf;

		DAOTableDefs * tdfs;
		dDb.m_pDAODatabase ->get_TableDefs (&tdfs);
		tdfs->Refresh();
		short Ctd;
		tdfs->get_Count(&Ctd);
		tdfs->Release();
		int VolumeID=1;

		dRsVolume.m_pDatabase=&dDb;
		dRsVolume.Open(dbOpenDynaset,_T("Select * From Volume"));
		CDaoTableDefInfo tdDefInfo;
		CDaoRecordset sRsData,dRsData;
		CDaoRecordset rsJD,rsZY;
		CDaoDatabase tmpSort;
		tmpSort.Open(basDirectory::DBShareDir + _T("DrawingSize"));//20071101 "ProjectDBDir" 改为 "DBShareDir";"Sort"改为 "DrawingSize"
		rsJD.m_pDatabase=&tmpSort;
		rsZY.m_pDatabase=&tmpSort;
		rsJD.Open(dbOpenSnapshot,_T("Select * From DesignStage"));
		rsZY.Open(dbOpenSnapshot,_T("Select * From Speciality"));
		int stc,sti;
		sti=0;
		stc=sRsEngin.GetRecordCount();
		while(!sRsEngin.IsEOF())
		{
			frmStatus.UpdateStatus((float)sti / (float)stc);
			frmStatus.UpdateWindow();
			::DoEvents();
			sti++;
			sRsEngin.GetFieldValue(_T("gcdm"),vTmp);
			EnginID=vtos(vTmp);
			VolumeDef.EnginID=EnginID;
			FindTable(sDb,EnginID,VoltabName,DatatabName,&vc,&dc);
			for(i=0;i<vc;i++)
			{
				tc=VoltabName[i].GetAt(VoltabName[i].GetLength()-1);
				if(rsZY.FindFirst(CString(_T("zydm=\'")) + tc + _T("\'")))
				{
					rsZY.GetFieldValue(_T("ZYID"),vTmp);
					VolumeDef.ZYID=vtoi(vTmp);
				}
				else VolumeDef.ZYID=0;

				tf=VoltabName[i].Find(_T("-"));
				tc=VoltabName[i].GetAt(tf-1);

				if(rsJD.FindFirst(CString(_T("sjjddm=\'")) + tc + _T("\'")))
				{
					rsJD.GetFieldValue(_T("SJJDID"),vTmp);
					VolumeDef.SJJDID=vtoi(vTmp);
				}
				else VolumeDef.SJJDID=0;

				VolumeDef.SJHYID=0;
				sRsVolume.m_pDatabase=&sDb;
				strSQL=_T("Select * From [") + VoltabName[i] + _T("]");
				sRsVolume.Open(dbOpenSnapshot,strSQL,dbForwardOnly);
				while(!sRsVolume.IsEOF())
				{
					sRsVolume.GetFieldValue(_T("jcdm"),vTmp);
					VolumeDef.jcdm=vtos(vTmp);
					sRsVolume.GetFieldValue(_T("jcmc"),vTmp);
					VolumeDef.jcmc=vtos(vTmp);
					
					VolumeDef.VolumeID=VolumeID;
					
					if(VolTabAddRs(dRsVolume,VolumeDef)) 
					{
						VolumeID++;
						for(int ii=0;ii< Ctd;ii++)
						{
							dDb.GetTableDefInfo(ii,tdDefInfo);
							if(tdDefInfo.m_strName.GetLength() ==2 )
							{
								int item=FindStr(DatatabName,dc,VoltabName[i] + tdDefInfo.m_strName);
								if(item!=-1)
								{
									//strSQL=_T("Select * From [") + DatatabName[item] + _T("] Where VolumeID=\'") + VolumeDef.jcdm + _T("\' AND Zdjh Is NOT NULL ");
									//::CDaoTableDef stdf;//(&sDb);
									::CDaoTableDef stdf(&sDb);
									stdf.Open(CString(_T("[")) + DatatabName[item] + _T("]"));
									CString sTab;
									sTab=_T("[") + VoltabName[i] + tdDefInfo.m_strName + _T("]");
									CString SQL1=GetTblField(stdf,sTab);
									stdf.Close();
									SQL1.TrimLeft(); SQL1.TrimRight();
									if(SQL1!=_T(""))
									{
										strSQL = _T("INSERT INTO [") + tdDefInfo.m_strName + _T("]  SELECT DISTINCT ") + ltos(VolumeDef.VolumeID) + _T(" AS VolumeID,")
												+ SQL1 + _T(" FROM [") + DatatabName[item] + _T("] IN \'") + sDb.GetName() + _T("\' ")  
												+ _T(" WHERE VolumeID =\'") + VolumeDef.jcdm + _T("\' AND  ZDJH IS NOT NULL ") ;
										//MessageBox(strSQL);
										try
										{
											dDb.Execute(strSQL);
											//strSQL =_T("DROP TABLE [") + VoltabName[i] + tdDefInfo.m_strName + _T("] ");
											//dDb.Execute(strSQL);
										}
										catch(CDaoException *e)
										{
#ifdef _DEBUG
											e->ReportError();
#endif
											e->Delete();
										}
									}
									//strSQL=_T("Select * From [") + DatatabName[item] + _T("] Where VolumeID=\'") + VolumeDef.jcdm + _T("\' AND Zdjh Is NOT NULL ");
									/*strSQL=_T("Select * From [") + DatatabName[item] + _T("] Where VolumeID=\'") + VolumeDef.jcdm + _T("\' AND Zdjh Is NOT NULL ");

									if(VolumeDef.jcdm==_T("0501"))
									{
										VolumeDef.jcdm=_T("0501");
									}
									sRsData.m_pDatabase=&sDb;
									sRsData.Open(dbOpenSnapshot,strSQL,dbForwardOnly);
									dRsData.m_pDatabase=&dDb;
									strSQL=_T("Select * From [") + tdDefInfo.m_strName + _T("]");
									dRsData.Open(dbOpenDynaset,strSQL);
									while(!sRsData.IsEOF())
									{
										DataTabAddRs(dRsData,sRsData,VolumeDef.VolumeID);
										sRsData.MoveNext();
									}
									sRsData.Close();
									dRsData.Close();*/
									//::DoEvents();
								}
							}
						}
					}
					sRsVolume.MoveNext();
				}
				sRsVolume.Close();
			}
			sRsEngin.MoveNext();
		}

		sRsEngin.Close();
		sDb.Close();
		dDb.Close();
		CopyFile(basDirectory::ProjectDBDir+ddbName,basDirectory::ProjectDBDir+ ddbName + _T("~"),FALSE);
		DeleteFile(basDirectory::ProjectDBDir+ddbName);
		CopyFile(basDirectory::ProjectDBDir+tdbName,basDirectory::ProjectDBDir+ ddbName,FALSE);
		DeleteFile(basDirectory::ProjectDBDir+tdbName);
		AfxGetApp()->EndWaitCursor();
		return true;
	}
	catch(CDaoException* e)
	{
		e->ReportError();
		e->Delete();
		AfxGetApp()->EndWaitCursor();
		return false;
	}
}

bool EDIBgbl::FindTable(CDaoDatabase &db, CString sTab, CString *pSTabName, CString *pSDataTabName, int *n1, int *n2)
{
		short iTmp=0,iTmp2=0;
	try
	{

		DAOTableDefs * tdfs;
		db.m_pDAODatabase ->get_TableDefs (&tdfs);
		tdfs->Refresh();

		short tdCount;
		tdfs->get_Count(&tdCount);
		tdfs->Release();
		CDaoTableDefInfo tdDefInfo;
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
	}
	catch(CDaoException* e)
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

bool EDIBgbl::VolTabAddRs(CDaoRecordset &RsVolume, _VolumeDef &VolumeDef)
{
	try
	{
		RsVolume.AddNew();
		RsVolume.SetFieldValue(_T("VolumeID"),COleVariant(VolumeDef.VolumeID));
		RsVolume.SetFieldValue(_T("EnginID"),COleVariant(VolumeDef.EnginID,VT_BSTRT));
		RsVolume.SetFieldValue(_T("jcdm"),COleVariant(VolumeDef.jcdm,VT_BSTRT));
		RsVolume.SetFieldValue(_T("jcmc"),COleVariant(VolumeDef.jcmc,VT_BSTRT));
		RsVolume.SetFieldValue(_T("SJHYID"),COleVariant(VolumeDef.SJHYID));
		RsVolume.SetFieldValue(_T("SJJDID"),COleVariant(VolumeDef.SJJDID));
		RsVolume.SetFieldValue(_T("ZYID"),COleVariant(VolumeDef.ZYID));
		
		RsVolume.Update();
		RsVolume.MoveLast();
		return TRUE;
	}
	catch(CDaoException * e)
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

bool EDIBgbl::DataTabAddRs(CDaoRecordset &dRsData, CDaoRecordset &sRsData, long VolumeID)
{
	try
	{
		int fc=sRsData.GetFieldCount();
		CDaoFieldInfo fi;
		COleVariant vTmp;
		COleVariant vNul;
		vNul.ChangeType(VT_NULL);
		DAOFields * pF;
		dRsData.m_pDAORecordset->get_Fields(&pF);
		pF->Refresh();
		pF->Release();
		sRsData.m_pDAORecordset->get_Fields(&pF);
		pF->Refresh();
		pF->Release();
		dRsData.AddNew();
		dRsData.SetFieldValue(_T("VolumeID"),COleVariant(VolumeID));
		for(int i=0;i<fc;i++)
		{
			sRsData.GetFieldInfo(i,fi);
			if(fi.m_strName!=_T("VolumeID"))
			{
				if(bFieldExists(dRsData,fi.m_strName))
				{
					sRsData.GetFieldValue(i,vTmp);
					if(vTmp.vt==VT_BSTR && vtos(vTmp)== _T(""))
						dRsData.SetFieldValue(fi.m_strName,vNul);
					else
						dRsData.SetFieldValue(fi.m_strName,vTmp);
				}
			}
		}
		dRsData.Update();
		dRsData.MoveLast();
		return TRUE;
	}
	catch(CDaoException * e)
	{
		//e->ReportError();
		e->Delete();
		return FALSE;
	}
}

bool EDIBgbl::bFieldExists(CDaoRecordset &rs, CString sName)
{
	try
	{
		CDaoFieldInfo fi;
		rs.GetFieldInfo(sName,fi);
		return TRUE;
	}
	catch(CDaoException * e)
	{
		e->Delete();
		return FALSE;
	}
}

bool EDIBgbl::bFieldExists(CDaoTableDef *rd, CString sName)
{
	try
	{
		CDaoFieldInfo fi;
		rd->GetFieldInfo(sName,fi);
		return TRUE;
	}
	catch(CDaoException * e)
	{
		e->Delete();
		return FALSE;
	}
}

bool EDIBgbl::IsExistentQuery(CDaoDatabase &db, CString strQueryName)
{
	bool bDuplicateQueryName=true;
	CDaoQueryDefInfo queryInfo;

	TRY
	{
		strQueryName.TrimLeft();strQueryName.TrimRight();
		if(strQueryName.Left(1) != _T("["))
			strQueryName= _T("[") + strQueryName;
		if(strQueryName.Right(1) != _T("]"))
			strQueryName += _T("]");
		DAOQueryDefs * qds;
		db.m_pDAODatabase->get_QueryDefs(&qds);
		qds->Refresh();
		qds->Release();
		db.GetQueryDefInfo(strQueryName,queryInfo);
	}
	CATCH(CDaoException, e)
	{
		if(e->m_pErrorInfo->m_lErrorCode==3265)
			bDuplicateQueryName=false;
	}
	AND_CATCH(CMemoryException,e)
	{}
	END_CATCH
	return bDuplicateQueryName;
}

CString EDIBgbl::GetTblField(CDaoTableDef &tbldef, CString tblName)
{
	try
	{
		CString ret;
		ret = _T(" ");
		CDaoFieldInfo fi;
		DAOFields * pF;
		tbldef.m_pDAOTableDef->get_Fields(&pF);
		pF->Refresh();
		pF->Release();
		int fc=tbldef.GetFieldCount();
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
	catch(CDaoException * e)
	{
		e->Delete();
		return CString(_T(" "));
	}
}

bool EDIBgbl::ChangeDatabase(CDaoDatabase &dDb, CDaoDatabase &sDb)
{
	bool bIsChange=false;
	try
	{
		CDaoTableDef sTD(&sDb) , dTD(&dDb) ;
		CDaoTableDefInfo sTDInfo,dTDInfo;
		CDaoRecordset drsTmp,srsTmp;
		//DAOFields * sFDs, * dFDs;
		CDaoFieldInfo sFD , dFD ;
		CString sTD1 , dTD1 ;
		bool bFDexists =false, bTDexists =false;
		int i =0;
		CString sTmp,SQLx;
		DAOFields * pF;
		DAOTableDefs * tdfs;
		sDb.m_pDAODatabase ->get_TableDefs (&tdfs);
		tdfs->Refresh();
		tdfs->Release();
		int sc=sDb.GetTableDefCount();

		dDb.m_pDAODatabase ->get_TableDefs (&tdfs);
		tdfs->Refresh();
		tdfs->Release();
		int dc=dDb.GetTableDefCount();

		for ( int si=0;si<sc;si++)
		{	
			sDb.GetTableDefInfo(si,sTDInfo);
			if((sTDInfo.m_lAttributes & dbSystemObject) == 0)
			{
				bTDexists=false;
				sTDInfo.m_strName=_T("[")+sTDInfo.m_strName+_T("]");
				sTD.Open(sTDInfo.m_strName);
				for ( int di=0;di<dc;di++)
				{	
					dDb.GetTableDefInfo(di,dTDInfo);
					dTDInfo.m_strName=_T("[")+dTDInfo.m_strName+_T("]");
					sTDInfo.m_strName.MakeUpper();
					dTDInfo.m_strName.MakeUpper();
					if ( ((sTDInfo.m_lAttributes & dbSystemObject) == 0) && (sTDInfo.m_strName== dTDInfo.m_strName)  )
					{
						//表存在
						bTDexists=true;
						sTD.m_pDAOTableDef->get_Fields(&pF);
						pF->Refresh();
						pF->Release();
						int sc1=sTD.GetFieldCount();
						dTD.Open(dTDInfo.m_strName);
						for (int i1=0;i1<sc1;i1++)
						{
							DAO_CHECK(dTD.m_pDAOTableDef->get_Fields(&pF));
							pF->Refresh();
							pF->Release();
							int dc1=dTD.GetFieldCount();
							sTD.GetFieldInfo(i1,sFD,AFX_DAO_ALL_INFO);
							bFDexists = false;
							for (int n1=0;n1<dc1;n1++)
							{
								dTD.GetFieldInfo(n1,dFD);
								if ( dFD.m_strName == sFD.m_strName ) 
								{
									//字段存在，设置标志
									/*if(dFD.m_nType == dbText && sFD.m_nType == dbText && sFD.m_lSize > dFD.m_lSize)
									{
										dFD.m_lSize=sFD.m_lSize;
										try
										{
											DAO_CHECK(dTD.m_pDAOFields->get_Item(
												COleVariant(dFD.m_strName, VT_BSTRT), &pDAOField));
											if(pDAOField!=NULL)
											{
												pDAOField->put_Size(dFD.m_lSize);
												pDAOField->Release();
											}
										}
										catch(...)
										{
										}
									}*/
									bFDexists = true;
									break;
								}
							}
							if ( ! bFDexists )
							{
								//不存在这个字段，则修改项目库中的表结构
								//dTD.Fields.Append dTD.CreateField(sFD.Name, sFD.Type, sFD.Size)
								try
								{
									dTD.CreateField( sFD);
									bIsChange=true;
								}
								catch(CDaoException *e)
								{
									e->Delete();
								}
								//i = i + 1
								i++;
							}
						}
						
						/*bFDexists=false;
						sc1=sTD.GetIndexCount();
						for (int i1=0;i1<sc1;i1++)
						{
							int dc1=dTD.GetIndexCount();
							sTD.GetIndexInfo(i1,sFD,AFX_DAO_ALL_INFO);
							bFDexists = false;
							for (int n1=0;n1<dc1;n1++)
							{
								dTD.GetIndexInfo(n1,dFD);
								if ( dFD.m_strName == sFD.m_strName ) 
								{
									//索引存在，设置标志
									bFDexists = true;
									break;
								}
							}
							if ( ! bFDexists )
							{
								//不存在这个索引，则修改项目库中的表结构
								//dTD.Fields.Append dTD.CreateField(sFD.Name, sFD.Type, sFD.Size)
								dTD.CreateIndex( sFD);
								//i = i + 1
								i++;
							}
						}*/
						dTD.Close();
						break;
					}
				}
				if(!bTDexists)
				{
					SQLx.Format(_T("SELECT * INTO %s FROM %s IN \'%s\'"),sTDInfo.m_strName,sTDInfo.m_strName,sDb.GetName());
					dDb.Execute(SQLx);
				}
			}
			sTD.Close();
		}
	}
	catch(CDaoException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}
	return bIsChange;
}

bool EDIBgbl::ChangeColumnsToRows(CDaoDatabase &db, CString TblName, CString OutTblName,int iRowStart,int iRowCount)
{
	//目的:将一个表的列转换成行表示，行转换成列表示。
	//典型的例子：将一个有许多字段的表tmpCalFixPhs的一个记录转换成多行，以便输出到Excel打印
	CDaoTableDefInfo tbInfo;
	CDaoFieldInfo FdInfo;
	CDaoRecordset rs(&db),rs1(&db);
	CString SQLx;
	COleVariant v;
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
			db.Execute(_T("DROP TABLE ") + OutTblName);
		}
		db.Execute(_T("CREATE TABLE ") + OutTblName + _T(" (Name CHAR(30),Data CHAR(40),Formula CHAR(255))"));
		SQLx=_T("SELECT * FROM ") + OutTblName;
		if(rs1.IsOpen())
			rs1.Close();
		rs1.Open(dbOpenDynaset,SQLx);

		SQLx=_T("SELECT * FROM ") + TblName;
		if(rs.IsOpen())
			rs.Close();
		rs.Open(dbOpenSnapshot,SQLx);
		if(rs.IsBOF() && rs.IsBOF())
		{
		}
		else
		{
			rs.Move(iRowStart-1);
			if(rs.IsBOF() && rs.IsEOF())
			{
				//移动超过起始记录
			}
			else
			{
				for(int iCount=1;iCount<=iRowCount;iCount++)
				{
					if(!rs.IsEOF())
					{
						int iFC=rs.GetFieldCount();						
						if(rs1.IsBOF() && rs1.IsEOF())
						{
						}
						else
						{
							//准备重新开始将rs另外一行记录修改到rs1的另外一列
							rs1.MoveFirst();
						}

						for(int i=0;i<iFC;i++)
						{
							//把每一个字段的所有行都写完
							rs.GetFieldInfo(i,FdInfo);
							rs.GetFieldValue(i,v);
							if(iCount==1)
							{
								rs1.AddNew();
								rs1.SetFieldValue(_T("Name"),LPCTSTR(FdInfo.m_strName));
								//rs1.SetFieldValue(_T("Formula"),COleVariant(_T("")));								
								rs1.SetFieldValue(_T("Data"),v);
								rs1.Update();
							}
							else
							{
								rs1.Edit();
								rs1.SetFieldValue(_T("Data"),v);
								rs1.Update();
							}
						}
					}
				}
			}
		}
		return true;
	}
	catch(CDaoException * e)
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
		CDaoDatabase sDb,dDb;
		CString strSQL;
		sDb.Open(basDirectory::TemplateDir + _T("PHScode.mdb"));//20071031 "sort" 改为 "PHScode.mdb"
		strSQL=_T("DELETE FROM [PhsStructureNAME]");
		sDb.Execute(strSQL);
		strSQL=_T(" INSERT INTO [PhsStructureNAME] SELECT * FROM [PhsStructureNAME] IN \'") + basDirectory::ProjectDBDir + _T("PHScode.mdb\'");
		sDb.Execute(strSQL);
		strSQL=_T("DELETE FROM [PhsStructureREF]");
		sDb.Execute(strSQL);
		strSQL=_T("INSERT INTO [PhsStructureREF] SELECT * FROM [PhsStructureREF] IN \'") + basDirectory::ProjectDBDir + _T("PHScode.mdb\'");
		sDb.Execute(strSQL);
		sDb.Close();
		
		BOOL bIsSuc=FALSE;
		int i=0;
		do
		{
			i++;
			bIsSuc=::CopyFile(basDirectory::TemplateDir + _T("PHScode.mdb") ,basDirectory::ProjectDBDir + _T("PHScode.mdb"),FALSE);
		}while(bIsSuc==FALSE && i<10);
		return TRUE;
	}
	catch(CDaoException * e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}

}


void EDIBgbl::UpdateDBTable(CDaoDatabase &SourceDB, CString SourceTBName, CDaoDatabase &DesDB, CString DesTBName)
{
		
	int iSourceCount;
	int iDesCount;
	bool bFound ;			//标志是否找到不存在的字段
	CDaoFieldInfo SourceDBFieldInfo;
	CDaoFieldInfo DesDBFieldInfo;
	try
	{
		CDaoTableDef tabSource(&SourceDB);
		CDaoTableDef tabDes(&DesDB);
		tabSource.Open(SourceTBName);
		tabDes.Open(DesTBName);
		iSourceCount = tabSource.GetFieldCount();
		iDesCount = tabDes.GetFieldCount();
		CString strSource;
		CString strDest;
		for ( int i = 0 ; i<iSourceCount;i++)
		{
			bFound = true;
			tabSource.GetFieldInfo(i,SourceDBFieldInfo,AFX_DAO_ALL_INFO);
			for ( int j = 0; j<iDesCount;j++)
			{
				tabDes.GetFieldInfo(j,DesDBFieldInfo,AFX_DAO_ALL_INFO);
				strSource = SourceDBFieldInfo.m_strName;
				strDest = DesDBFieldInfo.m_strName;
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
				tabDes.CreateField(SourceDBFieldInfo);
			}
		}
	}
	catch( CDaoException e)
	{
		e.ReportError();
	}
}

void EDIBgbl::UpdateDBForUpdate()
{
	CDaoDatabase db1;
	try
	{
		//db1.Open("D:\\ligb\\vc\\AutoPHS\\Debug\\db2.mdb",false,false);

		CString strSQL;
		//strSQL = "CREATE TABLE tzd180swed (ChineseCaption CHAR(50),LocalCaption CHAR(50),FieldName CHAR(10),width INTEGER,CADFieldWidth SINGLE,CADFieldTxtHeight SINGLE,CADFieldSeq INTEGER,SEQ INTEGER,Visible INTEGER,Locked INTEGER,Format CHAR(50),defFormat CHAR(50))";//
		//EDIBgbl::dbSORT.Execute(strSQL);
		//strSQL = "INSERT INTO tzd180swed IN \'"+db1.GetName()+"\' SELECT * FROM tzd180swed ";
		//EDIBgbl::dbSORT.Execute(strSQL);
	}catch ( CDaoException e )
	{
		e.ReportError();
		e.Delete();
	}
}
