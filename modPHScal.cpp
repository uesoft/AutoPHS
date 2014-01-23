// modPHScal.cpp
//
#include "stdafx.h"
//#include <float.h>
#include "modPHScal.h"
#include "modRegistry.h"
#include "basDirectory.h"
#include "EDIBgbl.h"
#include "Cphs.h"

#include "ModEncrypt.h"
#include "user.h"
#include "EDIBAcad.h"
#include "EDIBDB.h"
#include "PhsData.h"
#include <math.h>
#include "FrmTxsr.h"
#include "PagTxsrOption.h"
#include "frmStatus.h"
#include "FrmPhsSamp.h"
#include "InputBox.h"
#include "MessageBoxEx.h"

#include "fstream.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static int g_itest = 0;

//LFX 2005.6.30 加  焊缝绘制选项
bool modPHScal::gbDrawWeldSymbol = TRUE; 
bool modPHScal::gbAutoWeldViewInsertPoint = TRUE; 
bool modPHScal::gbDrawWeldViewDim = TRUE; 
bool modPHScal::gbWeldAutoDirectAndPos = TRUE;
//END LFX 2005.6.30 加  焊缝绘制选项

float modPHScal::gmiDiameter=0;//保存靠近管部的第一根拉杆直径，在1-3次计算之间
float modPHScal::giDiameter=0;//保存拉杆直径，在3次计算之间

float modPHScal::gfSeqCircleDiameter=6.0;//用圆圈标注尺寸时,圆的直径
float modPHScal::gfSeqTxtHeight=5.5;//标注零件编号时,编号文字的高度
BOOL modPHScal::g_bDHT=FALSE;//在材料汇总表中CLcl字段写上“电焊条”,
CString modPHScal::g_strDHTGG="";//在材料汇总表中CLcl字段写上“电焊条”时，电焊条的规格，这个字符串出现在CLgg字段
BOOL modPHScal::bInstalledAndFirstRun=FALSE;//第一次安装且第一次运行标志,用于升级数据库
float modPHScal::g_fLHGWeight=0;//下方恒吊LHG的重量
BOOL modPHScal::gbNoFenQu=FALSE;//图框外框不分区
BOOL modPHScal::gbTBhq=TRUE;//图框带有会签栏
BOOL modPHScal::gbNoWaiKuang=FALSE;//不要图框外框
bool modPHScal::gbOrientationPicture=false;//pfg20050531
int modPHScal::iRepZdjType=-1;//用AutoPHS计算的弹簧号取代应力分析AutoPSA计算的弹簧号
bool modPHScal::bHintRepZdjType=false;//提示用AutoPHS计算的弹簧号取代应力分析AutoPSA计算的弹簧号
int giUPxyz;//向上的坐标轴

bool gbHotStatus2Zero;//热态吊零计算弹簧
float modPHScal::DblHangerRodDist=0;//双吊拉杆间距
float modPHScal::fltTmp=-9999;
SprInfo modPHScal::sSprInfo[16];//弹簧信息数组
int modPHScal::SprInfoIndex=16;//弹簧组序号，如一个支吊架配置中弹簧组从下到上依次为T1.209、T1.109,则T1.209序号=0
const _TCHAR*	modPHScal::gcsCustomID=_T("CustomID");
const _TCHAR*	modPHScal::gcsID=_T("ID");
_ConnectionPtr modPHScal::dbZDJcrude;//支吊架原始数据库名称
bool modPHScal::gbStartedOnceAPHS = false;//AutoPHS运行了
CString modPHScal::Unit_Force = CString();//力单位
CString modPHScal::Unit_Displacement = CString();//位移单位
CString modPHScal::Unit_Momental = CString();//力矩单位
_variant_t modPHScal::gBookmark = _variant_t();
CString modPHScal::Ax = CString();//图纸大小,A3,A4
int modPHScal::gintParallelNum = 0;//弹簧并联数，即拉杆并联数
int modPHScal::gSumDistOfSpring = 0;//弹簧总位移
SprDirectionChar modPHScal::gSprDirectionChar;//弹簧方向字符，“向上”、“向下”、“UP”、“Down”
CString modPHScal::df1 = CString();//管部方向
CString modPHScal::gf1 = CString();//根部方向
CString modPHScal::df = CString();//
CString modPHScal::gf = CString();
CString modPHScal::fx = CString();
float modPHScal::haz = 0;//弹簧安装压缩值
CString modPHScal::dn = CString();//管部代号CustomID
CString modPHScal::IDdn = CString();//管部通用代号ID
float modPHScal::dj = 0;//管径,mm
float modPHScal::dwB = 0;//矩形管道外径，宽度mm
float modPHScal::t0 = 0;//介质温度,C
float modPHScal::sngPEL = 0;//管部标高,m
CString modPHScal::gn = CString();//根部代号CustomID
CString modPHScal::IDgn = CString();//根部通用代号ID
float modPHScal::sngSEL = 0;//根部标高,m
long modPHScal::zdjh = (long) 0;//支吊架编号
CString modPHScal::dg = CString();//管部、根部偏装标志=D、G
float modPHScal::gdw1 = 0;//根部支吊架荷载作用点到梁侧的距离，mm
float modPHScal::gdw = 0;//gdw1经过偏装修正后的值，mm

float modPHScal::gdwx = 0;//根部定位x值,mm
float modPHScal::gdwz = 0;//根部定位z值,mm
float modPHScal::yr = 0;//支吊架热态位移，mm
float modPHScal::yr1 = 0;//保存y轴热位移
float modPHScal::xr = 0;
float modPHScal::zr = 0;
float modPHScal::yl = 0;
float modPHScal::xl = 0;
float modPHScal::zl = 0;
float modPHScal::dyr = 0;
float modPHScal::dxr = 0;
float modPHScal::dzr = 0;
float modPHScal::dyl = 0;
float modPHScal::dxl = 0;
float modPHScal::dzl = 0;
float modPHScal::dxa0 = 0;//管部定位x值,mm
float modPHScal::dza0 = 0;//管部定位z值,mm
float modPHScal::dxa = 0;//dxa0偏装修正的值,mm
float modPHScal::dza = 0;//dza0偏装修正的值,mm
float modPHScal::gpzx = 0;//根部偏装x值,mm
float modPHScal::gpzz = 0;//根部偏装z值,mm
float modPHScal::pjg = 0;//结构荷载,kgf
float modPHScal::pgz = 0;//工作荷载,kgf
float modPHScal::paz = 0;//安装荷载,kgf
	//用于固定支架Z2/Z2A垫钢板计算的长宽高信息
	float modPHScal::gfPSLength=0;
	float modPHScal::gfPSWidth=0;
	float modPHScal::gfPSThickness=0;
	int modPHScal::giSAG100No=1;
	//固定支架计算模型坐标系的冷热态承受的力,kgf
	float modPHScal::pxlc=0;
	float modPHScal::pylc=0;
	float modPHScal::pzlc=0;
	float modPHScal::pxrc=0;
	float modPHScal::pyrc=0;
	float modPHScal::pzrc=0;
	float modPHScal::mxlc=0;
	float modPHScal::mylc=0;
	float modPHScal::mzlc=0;
	float modPHScal::mxrc=0;
	float modPHScal::myrc=0;
	float modPHScal::mzrc=0;
	float modPHScal::PAfixH=0;//固定支架管部高度H,cm

int modPHScal::iMaxRodDia = 0;
int modPHScal::giNumRod = 0;
float modPHScal::Lspan = 0;
float modPHScal::WidthB = 0;
float modPHScal::WidthA = 0;
CString modPHScal::gstrMaterial = CString();
CString modPHScal::gsPhsArrangementDwgNo = CString();
CString modPHScal::gsPhsScheduleDwgNo = CString();
CString modPHScal::clb = CString();
float modPHScal::SpringRigid = 0;
float modPHScal::SpringLMax = 0;
long modPHScal::iSelSampleID = (long) 0;
bool modPHScal::pbFrmLoadedBmp = false;
int modPHScal::SelInputMethod = 0;
CString modPHScal::CSZdjType = CString();
CString modPHScal::ZdjType = CString();
int modPHScal::giMinRodLen=300;
bool modPHScal::gbPhsIsCSPR = false;
bool modPHScal::gbPhsIsSPR = false;
BOOL modPHScal::g_bPipeClampBoltDiameterChanged = TRUE;

CString modPHScal::gsOldPhsPASel = CString();
CString modPHScal::gsOldPhsPARTSel = CString();
CString modPHScal::gsOldPhsSASel = CString();
CString modPHScal::gsOldPhsSPRINGSel = CString();
CString modPHScal::gsOldPhsConstantSpringSel = CString();
CString modPHScal::gsOldPhsBoltsNutsSel = CString();
CString modPHScal::gsPhsPASel = CString();
CString modPHScal::gsPhsPARTSel = CString();
CString modPHScal::gsPhsSASel = CString();
CString modPHScal::gsPhsSPRINGSel = CString();
CString modPHScal::gsPhsConstantSpringSel = CString();
CString modPHScal::gsPhsBoltsNutsSel = CString();
CString modPHScal::gsPhsDescPASel = CString();
CString modPHScal::gsPhsDescPARTSel = CString();
CString modPHScal::gsPhsDescSASel = CString();
CString modPHScal::gsPhsDescSPRINGSel = CString();
CString modPHScal::gsPhsDescConstantSpringSel = CString();
CString modPHScal::gsPhsDescBoltsNutsSel = CString();
CString modPHScal::gsCSPRmanufactory = CString();
CString modPHScal::gsSPRmanufactory = CString();
CString modPHScal::gsCSPRUnitOfLoad = CString();
CString modPHScal::gsSPRUnitOfLoad = CString();
CString modPHScal::tbnTZB = CString();
CString modPHScal::tbnBoltsNuts = CString();
CString modPHScal::tbnBoltsNutsID = CString();
CString modPHScal::tbnAttachment = CString();
CString modPHScal::tbnAttachmentID = CString();
CString modPHScal::tbnSABoltsNuts = CString();
CString modPHScal::tbnSABoltsNutsID = CString();
CString modPHScal::tbnBoltsSurplusLength = CString();
CString modPHScal::tbnPA = CString();
CString modPHScal::tbnSA = CString();
CString modPHScal::tbnPAfix = CString();
CString modPHScal::tbnPART = CString();
CString modPHScal::tbnPAID = CString();
CString modPHScal::tbnPARTID = CString();
CString modPHScal::tbnSAID = CString();
CString modPHScal::tbnSectionSteel = CString();
CString modPHScal::tbnSectionSteelID = CString();
CString modPHScal::tbnLugInfo = CString();
CString modPHScal::tbnLugLenCal = CString();
CString modPHScal::tbnCONNECT = CString();//厂家标准特殊连接规则表，一般主要用于适应华东院拉杆两端自带螺母的情况
CString modPHScal::tbnAllowStress = CString();
CString modPHScal::tbnHDCrude = CString();
CString modPHScal::tbnHDproperty = CString();
CString modPHScal::tbnHDid = CString();
CString modPHScal::tbnDisplacementSerial = CString();
CString modPHScal::tbnCSPRDiameterSerial = CString();
CString modPHScal::tbnSPRDiameterSerial = CString();
CString modPHScal::tbnLugDiaOfCSPR = CString();
CString modPHScal::tbnSPRINGCrude = CString();
CString modPHScal::tbnSPRINGL5Crude = CString();
CString modPHScal::tbnCSPRINGL5Crude = CString();
CString modPHScal::tbnSPRINGproperty = CString();
CString modPHScal::tbnDiscSPRINGpropertyMaxDist = CString();
CString modPHScal::tbnSPRINGid = CString();
bool modPHScal::gbH1IncludeB = false;
bool modPHScal::gH1Pos = false;
bool modPHScal::gH1distPos = false;
bool modPHScal::gbCH1IncludeB = false;
bool modPHScal::gbCH1IncludeT = false;
int modPHScal::gCH1Pos = 0;
int modPHScal::gCH1distPos = 0;
bool modPHScal::bE_FormulaPlusHalf = 0;
CString modPHScal::strBigFontLjmx;//零件明细表字体
CString modPHScal::strBigFontDim;	//尺寸字体
CString modPHScal::strSHXFontLjmx;//零件明细表字体
CString modPHScal::strSHXFontDim;	//尺寸字体
float modPHScal::fFWDim;	//尺寸字体宽度因子
float modPHScal::fFWLjmx;	//零件明细表宽度因子
float modPHScal::gsngTotalInstallCompressValue = 0;
bool modPHScal::gbByForce = false;
float modPHScal::gUseConstantSpringMinSurplusDist = 0;
float modPHScal::gUseConstantSpringPercentSurplusDist = 0;
float modPHScal::gUseSpringMinDist = 0;
float modPHScal::gnSPR_CHZBH = 0;
float modPHScal::gsngRatioOfPjg2Pgz = 0;
float modPHScal::gnConnectTJ = 0;
	//获取零部件选材标准索引号ClassIndex,
	//Observation字段指出了材料选择的索引号ClassIndex,根据该索引和温度、零部件代码ID，
	//可以从sort.mdb库中SpecificationOfMaterial表查到还没有选择材料的零部件可用的材料。
	//例如: ClassIndex=0,为汽水管道支吊架零部件可用的材料（新管规DL/T5054-1996）;ClassIndex=1,为烟风煤粉管道支吊架零部件可用的材料（新六道规程DL/T5121-2000）。
int modPHScal::giClassIndex=0;
int modPHScal::gnRodLenRoundVal = 100;
int modPHScal::gnRodInsertLenRoundVal = 1;
//拉杆直径按最大值配置
bool modPHScal::gbRodDiaByMax=true;
bool modPHScal::gbPipeClampBolt = false;

bool modPHScal::gbConnectHoleDimMatch = false;
bool modPHScal::gbSPRAutoLugDia = false;
bool modPHScal::gbCSPRAutoLugDia = false;
bool modPHScal::gbAutoApplyChzbh = false;
bool modPHScal::gbOutputSA = false;
bool modPHScal::gbCalSAbyConservativenessMethod = false;
bool modPHScal::gbCalSAbyPJG = false;
bool modPHScal::gbCalPARTbyMaxLoad = false;
bool modPHScal::gbAddPartWeight2PMax = false;
bool modPHScal::gbAddPartWeight2CSPRPgz = true;
bool modPHScal::gbAddPartWeight2SPRPgz = false;//2007.07.30
bool modPHScal::gbPAForceZero=false;//2007.09.25
bool modPHScal::gbMaxPgzPaz=false;//2007.09.03
//bool modPHScal::gbMinMaxDispByActualDisp = true;
bool modPHScal::gbMinMaxDispByActualDisp = false;	
//bool modPHScal::gbCalcCSPRHeight_BySelDisp = false;
bool modPHScal::gbCalcCSPRHeight_BySelDisp=true;
bool modPHScal::gbCalCSPRLugDiaByPjg = false;
bool modPHScal::gbCSPRneedSpecialDesign = false;
bool modPHScal::gbCSPRCanSpecialDesign = false;
bool modPHScal::gbAutoAlignCSPRtoSA = false;


bool modPHScal::gbLimitLugMinDia = false;
bool modPHScal::gbSumAllVolume = false;
bool modPHScal::gbAutoPASA = false;
bool modPHScal::gbAutoApplyCSPRLugDia = false;
bool modPHScal::gbNotPrompt_ACAD_SHIFT = false;
int modPHScal::gIsACAD = 0;
bool modPHScal::gbNotPrompt_Catlog_OVERWRITE = false;
int modPHScal::giDrawSIZE_A3A4 = 0;
int modPHScal::giPhsOutINFOTableType = 0;
CString modPHScal::gsPhsColor = CString();
bool modPHScal::gbAnnotateCSPR = false;
bool modPHScal::gbAutoSPRRodDiaDecrease=false;
bool modPHScal::gbSPRBHLOADBYInstallLoad=false;
bool modPHScal::gbSPRBHLOADUNITBYNEWTON=false;
bool modPHScal::gbDraw_BOM = false;
bool modPHScal::gbDrawOVERWRITE = false;
bool modPHScal::gbWorksWithBoltsNuts = false;
bool modPHScal::gbWorksWithSABoltsNuts = false;
float modPHScal::gnDW_delta = 0;
int modPHScal::giAllowedMaxSerialNumPerSPR = 0;
int modPHScal::giAllowedMaxSerialNumSPRs = 0;
int modPHScal::giSpringOrder = 0;
CString modPHScal::gstrSpringOrder = CString();
CString modPHScal::gstrSpringOrderSQL = CString();
bool modPHScal::AutoOrdinateDrawing = false;
long modPHScal::gnSEQyOffset = (long) 8;
bool modPHScal::gbDrawNoPreMethod = false;
bool modPHScal::gbAutoSaveDrawing = false;
long modPHScal::glDrawNoStartNumber = (long) 0;
long modPHScal::glDWGPreMethod = (long) 0;
CString modPHScal::gsDWGPre = CString();
CString modPHScal::gsDwgFN = CString();
int modPHScal::giZDJHinDwg = 0;
bool modPHScal::blResizable = false;
CString modPHScal::gsPhsTypeRigid = CString();
CString modPHScal::gsPhsTypeSPR = CString();
CString modPHScal::gsPhsTypeCSPR = CString();
int modPHScal::giWholeSprNum = 0;
int modPHScal::giSumSerialNum = 0;
int modPHScal::glNumSA = 0;
int modPHScal::iCSnum = 0;
CString modPHScal::gsPartType = CString();
CString modPHScal::gsOldPartType = CString();
CString modPHScal::gsaSPRPos = CString();
bool modPHScal::gbStopOLE = false;
bool modPHScal::gbCalAllPhs = false;
bool modPHScal::gbStopCalAllPHS = false;
double modPHScal::VX_pt0x = 0.0;
double modPHScal::VX_pt0y = 0.0;
double modPHScal::VZ_pt0x = 0.0;
float modPHScal::pt1x = 0;
float modPHScal::pt1y = 0;
float modPHScal::pt2x = 0;
float modPHScal::pt2y = 0;
float modPHScal::pt3x = 0;
float modPHScal::pt3y = 0;
float modPHScal::Pt0X = 0;
float modPHScal::giDimOffset = 0;
float modPHScal::giAttDxOffsetX = 0;
float modPHScal::giAttDzOffsetX = 0;
float modPHScal::giAttDOffsetY = 0;
CString modPHScal::iPhsSAfx[4];
int* modPHScal::PtypeDiameter = NULL;//在getphsBHandSizes中，恒吊拉杆的直径保存在这个数组里，以便所有的螺纹连接件被约束。
int modPHScal::PtypeIndexForDiameter=0;
int* modPHScal::giCSPRindex = NULL;
int modPHScal::giCSPRindexSize=0;
int modPHScal::glIDIndex = 0;
long modPHScal::glClassID = (long) 0;
long modPHScal::glPAid = (long) 0;
long modPHScal::glSAid = (long) 0;
float vf7850=7850.0;
CString modPHScal::gsPhsName = CString();
CString modPHScal::gsPhsEnglishName = CString();
CString modPHScal::ZdjTxName[20];
bool modPHScal::gbSumRodByRoundSteel=0;
CString modPHScal::gsPSAOutFilePath;
int modPHScal::g_iPSAFilterIndex=1;
CString modPHScal::gsSelectProductMdb;
int modPHScal::iNumCSVal=0;
float modPHScal::fDiameter=0;//

int modPHScal::iAlignLoad=0;
int modPHScal::iAlignDim=0;
int modPHScal::iAlignLjmx=0;
float modPHScal::fFWLoad=1.0f;
CString modPHScal::strBigFontLoad=_T("");
CString modPHScal::strSHXFontLoad=_T("");
double modPHScal::gdZJOverValue = 30.0;//支架偏装需要超过位移值 modify 2008.10.16

void modPHScal::InitZdjTxName()
{
 //此过程必须位于修改名称GetPhsAssembleName过程之前，因为它需要使用gsPhsTypeSPR等变量。
   //开始获得支吊架图形名称，用于图框绘制
	_ConnectionPtr db=NULL;
	try{
	_RecordsetPtr rs;
	db.CreateInstance(__uuidof(Connection));
	_variant_t tmpvar;
	db->Open(_bstr_t(::dbConnectionString+basDirectory::ProjectDBDir + _T("PHScode.mdb")),_T(""),_T(""),0);//20071018"sort.mdb"改为"PHScode.mdb"
   rs=db->Execute(_T("SELECT * FROM PhsTypeName"),&tmpvar,adCmdText);
	while(!rs->adoEOF)
	{
		tmpvar=rs->GetCollect(_T("Index"));
		if(tmpvar.vt!=VT_NULL)
		{
			switch((long)tmpvar)
			{
			case iZdjSPRh:
            ZdjTxName[iZdjSPRh] = Trim(vtos(rs->GetCollect(_T("PhsTypeName"))));
			break;
			case iZdjCSPRh:
            ZdjTxName[iZdjCSPRh] = Trim(vtos(rs->GetCollect(_T("PhsTypeName"))));
			break;
			case iZdjRigidH:
            ZdjTxName[iZdjRigidH] = Trim(vtos(rs->GetCollect(_T("PhsTypeName"))));
			break;
			case iZdjGuide:
            ZdjTxName[iZdjGuide] = Trim(vtos(rs->GetCollect(_T("PhsTypeName"))));
			break;
			case iZdjSlide:
            ZdjTxName[iZdjSlide] = Trim(vtos(rs->GetCollect(_T("PhsTypeName"))));
			break;
			case iZDJfix:
            ZdjTxName[iZDJfix] = Trim(vtos(rs->GetCollect(_T("PhsTypeName"))));
			break;
			case iZdjSprGuide:
            ZdjTxName[iZdjSprGuide] = Trim(vtos(rs->GetCollect(_T("PhsTypeName"))));
			break;
			default:
            ZdjTxName[iZdjSupport] = Trim(vtos(rs->GetCollect(_T("PhsTypeName"))));
			}
		}
		rs->MoveNext();
	}
   
   //开始获得支吊架类别名称，用于模板名称管理
	rs->Close();
	rs=NULL;
	rs=db->Execute(_T("SELECT * FROM PhsClassType"),&tmpvar,adCmdText);
   while(!rs->adoEOF)
	{
		tmpvar=rs->GetCollect(_T("Index"));
		if(tmpvar.vt!=VT_NULL)
		{
			switch((long)tmpvar)
			{
			case iPhsSPR:
            gsPhsTypeSPR = Trim(vtos(rs->GetCollect(_T("PhsClassType"))));
			break;
			case iPhsCSPR:
            gsPhsTypeCSPR = Trim(vtos(rs->GetCollect(_T("PhsClassType"))));
			break;
			case iPhsRigid:
            gsPhsTypeRigid = Trim(vtos(rs->GetCollect(_T("PhsClassType"))));
			break;
			}
		}
		rs->MoveNext();
   }
   rs->Close();
   rs=NULL;
   db->Close();
   db=NULL;
	}
	catch(_com_error &e)
	{
		ShowMessage(e.Description());
	}
}

void modPHScal::InitPhsSAfx()
{
	iPhsSAfx[iPX] = _T("PX");
   iPhsSAfx[iOZ] = _T("OZ");
   iPhsSAfx[iOX] = _T("OX");
   iPhsSAfx[iPZ] = _T("PZ");
}

int modPHScal::GetPhsSAfx(CString  SAfx)
{
 //获得根部方向,Key数值或字符串
	SAfx.MakeUpper();
	if(SAfx==_T("X") || SAfx==_T("Z"))
		SAfx=CString(_T("P"))+SAfx;
     else if(SAfx.Left(1)==_T("-"))
		 SAfx.SetAt(0,_T('0'));
	 for(int i=0;i<4;i++)
	 {
		 if(iPhsSAfx[i]==SAfx)
			 return i;
	 }
	 return -1;
}

void modPHScal::GetHeatDisplacement(_RecordsetPtr /*ByVal*/ rstbl)
{
}

int modPHScal::iBOMStartNo(int /*Optional*/ SN)
{
	CInputBox dlg;
	dlg.m_strTitle.LoadString(IDS_PleaseInputBOMstartDrawNo);
	dlg.m_strWndText.LoadString(IDS_FirstPageBOMstartDrawNo);
	dlg.m_strValue.Format(_T("%d"),SN);
	dlg.DoModal();
	while(dlg.m_strValue==_T("") || dlg.m_strValue.GetAt(0) < _T('0') || dlg.m_strValue.GetAt(0) > _T('9'))
		dlg.DoModal();
   return _ttoi(dlg.m_strValue);
}

_variant_t modPHScal::sFindBlkPosFLD(CString  sFLD, CString  dFLD, CString  sID)
{
	//根据sFLD字段的值为ID的记录,返回dFLD字段的值
	_variant_t vtmp;
	vtmp.ChangeType(VT_NULL);
   try
   {
	   _RecordsetPtr rs;
	   rs.CreateInstance(__uuidof(Recordset));
	   sFLD.TrimLeft();sFLD.TrimRight();
	   sID.TrimLeft();sID.TrimRight();
	   dFLD.TrimLeft();dFLD.TrimRight();
	   CString SQLx;
	   SQLx+=_T("SELECT * FROM PhsBlkDimPos WHERE (");
		SQLx+=sFLD;
		SQLx+=_T(")=\'");
		SQLx+=sID;
		SQLx+=_T("\'");
		rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPHScode,true), 
			adOpenStatic, adLockOptimistic, adCmdText); 
   if(rs->BOF && rs->adoEOF)
   {
	   ShowMessage(GetResStr(IDS_NosFLDinPhsBlkDimPos));
	   return vtmp;
   }
   rs->get_Collect((_variant_t)dFLD,&vtmp);
   return vtmp;
   }
   catch (_com_error &e)
   {
	   CString strMsg;
	   strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
	   AfxMessageBox(strMsg);
	}

	   return vtmp;
}

_variant_t modPHScal::sFindFLD(CString /*ByVal*/ strSourceFLD, CString /*ByVal*/ strDestFLD, CString strSourceFLDvalue)
{
//根据strSourceFLD字段的值为strSourceFLDvalue的记录,返回strDestFLD字段的值
//测试:无论使用rs的findfirst方法还是使用SQL获得表，结果都一样。Access查找时不区分大小写。因此CustomID必须唯一。
	try{
		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(Recordset));
   CString sTmp;
   _variant_t var;
   strDestFLD.TrimLeft();strDestFLD.TrimRight();
   strSourceFLD.TrimLeft();strSourceFLD.TrimRight();
   if(strSourceFLDvalue.Left(1)!=(_T("\'")))
	   strSourceFLDvalue=_T("\'")+strSourceFLDvalue;
   if(strSourceFLDvalue.Right(1)!=(_T("\'")))
	   strSourceFLDvalue=strSourceFLDvalue+_T("\'");
   sTmp=CString(_T("SELECT "))+strDestFLD +_T(" FROM PictureClipData WHERE (")+strSourceFLD+_T(") = ")+strSourceFLDvalue;
   //rs=EDIBgbl::dbPRJ->Execute(_bstr_t(sTmp),NULL,adCmdText);
   rs->Open((_bstr_t)sTmp, _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
	   adOpenStatic, adLockOptimistic, adCmdText); 
   if(rs->adoEOF)
         return _variant_t((long)0);
     else
     {
		rs->get_Collect((_variant_t)strDestFLD,&var);
		 return var;
     }
	 rs->Close();
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	return _variant_t((long)0);
}
}
/*
_variant_t modPHScal::sFindAnyTableField(_ConnectionPtr  db, CString  strSourceTable, CString  strSourceFLD, CString  strDestFLD, CString  strSourceFLDvalue)
{
	_variant_t ret;
	ret.ChangeType(VT_NULL);
	_variant_t tmpvar;
	if(db==NULL || strSourceTable == _T(""))
		return ret;
	CString sTmp;
	try{
		sTmp=_T("Select ")+strDestFLD+_T(" from ")+strSourceTable+_T(" where ")+strSourceFLD+_T(" = ")+strSourceFLDvalue;
		_RecordsetPtr rs=db->Execute(_bstr_t(sTmp),&tmpvar,adCmdText);
		if(rs->adoEOF)
			return ret;
		ret=rs->GetCollect(_variant_t(strDestFLD));
		rs->Close();
		return ret;
	}
	catch(_com_error &e)
	{
		return ret;
	}
}
*/
_variant_t modPHScal::sFindAnyTableField(_ConnectionPtr  db, CString  strSourceTable, CString  strSourceFLD, 
										  CString  strDestFLD, CString  strSourceFLDvalue)
{
	_variant_t ret;
	ret.ChangeType(VT_NULL);
	_variant_t tmpvar;
	if(strSourceTable == _T(""))
		return ret;
	CString sTmp;
	try{
		sTmp=_T("Select ")+strDestFLD+_T(" from ")+strSourceTable+_T(" where ")+strSourceFLD+_T(" = ")+strSourceFLDvalue;
		//_RecordsetPtr rs=db->Execute(_bstr_t(CString(_T("select * from "))+strSourceTable),&tmpvar,adCmdText);
		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(Recordset));
		rs->Open((_bstr_t)sTmp, _variant_t((IDispatch*)db,true), 
			adOpenStatic, adLockOptimistic, adCmdText); 
		if(rs->adoEOF)
			return ret;
		rs->get_Collect((_variant_t)strDestFLD,&tmpvar);
		rs->Close();
		ret=tmpvar;
		return ret;
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
		return ret;
	}
}
CString modPHScal::sFindID(CString CustomID)
{
//根据CustomID查找ID
	try
	{
	CustomID.TrimLeft();CustomID.TrimRight();
	CString ret;
   if (CustomID == _T(""))
      return  _T("");
   else
   {
	   _RecordsetPtr rs;
	   rs.CreateInstance(__uuidof(Recordset));
	  _variant_t tmpvar;
	 // _bstr_t bs=(_bstr_t)CString(_T("SELECT * FROM PictureClipData WHERE (CustomID)=\'")+CustomID+_T("\'"));
      //rs = EDIBgbl::dbPRJ->Execute(_bstr_t(CString(_T("SELECT * FROM PictureClipData WHERE (CustomID)=\'")+CustomID+_T("\'"))),
	///	  &tmpvar,adCmdText);
	  if(CustomID.Left(1)!=(_T("\'")))
		  CustomID=_T("\'")+CustomID;
	  if(CustomID.Right(1)!=(_T("\'")))
		  CustomID+=_T("\'");
	  CString sTmp;
	  sTmp = _T("SELECT * FROM PictureClipData WHERE (CustomID)=")+CustomID;
	  rs->Open((_bstr_t)sTmp, _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
		  adOpenStatic, adLockOptimistic, adCmdText); 
     if(rs->BOF && rs->adoEOF)
         //MsgBox ResolveResString(iUE_NoRecordFLDEqvThisValueInPictureClipData, _T("|1"), _T("CustomID"), _T("|2"), CustomID), vbMsgBoxSetForeground
         ret = _T("");
      else
      {
		  rs->get_Collect((_variant_t)_T("id"),&tmpvar);
		  ret=vtos(tmpvar);
		  //ret =(char*)(_bstr_t)rs->GetCollect(_variant_t(_T("id")));
		  ret.TrimLeft();ret.TrimRight();
	  }
	  rs->Close();
   return ret;
   }
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
		return _T("");
	}
}

CString modPHScal::sFindCustomID(CString ID)
{
//根据ID查找CustomID
	try
	{
	ID.TrimLeft();
	ID.TrimRight();
	if(ID==_T(""))
		return _T("");
   if(ID.Left(1)!=_T("\'"))
	   ID=_T("\'")+ID;
   if(ID.Right(1)!=_T("\'"))
	   ID=ID+_T("\'");
	CString SQLx=CString(_T("select * From PictureClipData Where Trim(ID)="))+ID;
	_RecordsetPtr rs;
	rs.CreateInstance(__uuidof(Recordset));
	rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
		adOpenStatic, adLockOptimistic, adCmdText); 
	if(rs->adoEOF || rs->BOF)
	{
		rs->Close();
		/*CString sTmp;
		sTmp.Format(GetResStr(IDS_NoRecordFLDEqvThisValueInPictureClipData),_T("ID"),ID);
		ShowMessage(sTmp);*/
		return _T("");
	}
	_variant_t v;
	rs->get_Collect((_variant_t)_T("CustomID"),&v);
	rs->Close();
	return vtos(v);
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
	return _T("");
}


CString modPHScal::sFindTBN(CString /*ByVal*/ PartCustomID)
{
	_RecordsetPtr rs;
	rs.CreateInstance(__uuidof(Recordset));
//	int i;
   //iSA=0,iCSPR=7
   //Debug.Print PartCustomID
	_variant_t tmpvar;
	PartCustomID.TrimLeft();PartCustomID.TrimRight();
	glClassID = -1;
      glIDIndex = -1;
	if(PartCustomID==_T(""))
	{
		glClassID = -1;
      glIDIndex = -1;
		return _T("");
	}
	CString SQLx;
	SQLx = _T("SELECT * FROM PictureClipData WHERE  (CustomID)=\'")+PartCustomID+_T("\'");
	rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
		adOpenStatic, adLockOptimistic, adCmdText); 
   if(rs->BOF && rs->adoEOF )
   {
      //MsgBox ResolveResString(iUE_NoRecordFLDEqvThisValueInPictureClipData,_T("|1"),_T("CustomID"), _T("|2"), PartCustomID), vbMsgBoxSetForeground
      
      glClassID = -1;
      glIDIndex = -1;
	  rs->Close();
	  return _T("");
	}
   else
   {
	   rs->get_Collect((_variant_t)_T("ClassID"),&tmpvar);
	   if(tmpvar.vt!=VT_NULL)
			glClassID = vtoi( tmpvar);
	   rs->get_Collect((_variant_t)_T("Index"),&tmpvar);
	   if(tmpvar.vt!=VT_NULL)
      glIDIndex = vtoi(tmpvar);
	   rs->Close();
		switch(glClassID)
		{
		case iSAh:
		case iSACantilever:
		case iSAbeam:
		case iSALbrace:
		case iSALbraceFixG47:
		case iSALbraceFixG48:
		case iG51:
		case iG52_55:
		case iG56:
		case iG100:
		case iGCement:
             return tbnSA;
				break;
		case iPAfixZ1:
		case iPAfixZ2:
            return tbnPAfix;
			break;
		case iPAh:
		case iPAs:
		case iPASS:
		case iPAGS:
		case iPAD4LA:
		case iPADLR:
		case iPAD11LD:
		case iPAD4LQ:
		case iPALX:
		case iPAXZ1_XZ3:
         	return tbnPA;
			break;
		case iROD:
		case iConnected:
		case iL15:
		case iXL1:
            //连接件数据表
            return tbnPART;
			break;
		case iAttached:
            //附件数据表
            return tbnAttachment;
			break;
		case iBolts:
		case iNuts:
            //连接表储存的螺栓螺母是西北院的CustomID,或者说是ID，因此在使用其它标准查找连接螺栓时需要先进行变换。
            //变换过程在Cphs.GetphsBHandSizes中实现。
            return tbnBoltsNuts;
			break;
		case iSPR:
            return tbnSPRINGCrude;
			break;
		case iCSPR:
            return tbnHDCrude;
			break;
        default:
            return tbnSA;
		}
   }
}

void modPHScal::AutoSEQforPictureClipData()
{
}

void modPHScal::InitZDJID()
{
}

void modPHScal::GetRecentWorkSPEC()
{
   //读取最近工作支吊架规范
	CString s=_T("");
	gsPhsPASel = Trim(GetRegKey(_T("RecentWork"), user::gstrAppName + _T("_phsPASel"),s));
	gsPhsPARTSel =Trim(GetRegKey(_T("RecentWork"), user::gstrAppName + _T("_phsPARTSel"), s));
	gsPhsSASel = Trim(GetRegKey(_T("RecentWork"), user::gstrAppName + _T("_phsSASel"), s));
	gsPhsSPRINGSel = Trim(GetRegKey(_T("RecentWork"), user::gstrAppName + _T("_phsSpringSel"), s));
	gsPhsConstantSpringSel = Trim(GetRegKey(_T("RecentWork"), user::gstrAppName + _T("_phsConstantSpringSel"), s));
	gsPhsBoltsNutsSel = Trim(GetRegKey(_T("RecentWork"), user::gstrAppName + _T("_phsBoltsNutsSel"), s));

}

void modPHScal::SaveRecentWorkSPEC()
{
   //保存最近工作支吊架规范
	::SetRegValue(_T("RecentWork"), user::gstrAppName + _T("_phsPASel"), gsPhsPASel);
   ::SetRegValue(_T("RecentWork"), user::gstrAppName + _T("_phsPARTSel"), gsPhsPARTSel);
   ::SetRegValue(_T("RecentWork"), user::gstrAppName + _T("_phsSASel"), gsPhsSASel);
   ::SetRegValue(_T("RecentWork"), user::gstrAppName + _T("_phsSpringSel"), gsPhsSPRINGSel);
   ::SetRegValue(_T("RecentWork"), user::gstrAppName + _T("_phsConstantSpringSel"), gsPhsConstantSpringSel);
   ::SetRegValue(_T("RecentWork"), user::gstrAppName + _T("_phsBoltsNutsSel"), gsPhsBoltsNutsSel);

}

bool  modPHScal::bPAIsHanger()
{
 //目的：判断支吊架是否是吊架
   //如果根部标高大于管部标高，则是吊架;否则是支架。
	try
	{
		sngSEL=vtof(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("GH1")));
		sngPEL=vtof(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("DH1")));
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
   if(sngSEL > sngPEL)
		return true;
	else return false;
}

bool modPHScal::blnSelAndSaveSPEC()
{
	bool ret=false;
	try
	{
		//启动程序时，必须经过CreateTmpCustomIDForPART(), blnSelPhsSPEC()函数至少一次，因为有一些变量还要在该函数中获得，否则那些变量会=0，产生除零错。		
		if (EDIBgbl::gbSelSPECset)
			ret =true;
		else
		{
			GetRecentWorkSPEC();
			if (blnSelphsSPEC(true))
			{
				SaveRecentWorkSPEC();
				ret = true;
			}
			else
				ret = false;
			EDIBgbl::gbSelSPECset = ret;
		}
	}
	catch(_com_error &e)
	{
		ret=false;
		ShowMessage(e.Description());
	}
	return ret;
}


bool modPHScal::blnSelphsSPEC(bool /*ByVal*/ mbSPECchanged)
{
	//目的：根据规范选择，确定螺栓螺母、弹簧、恒吊、管部根部等零部件的表名称
	//返回：规范选择成功，返回true;出错，返回False
	//RemAuthor:LEE Guobin
	//作者:LEE Guobin
	//LFX 2005.2.24 注 本函数中在调用第个HStbExists函数参数末尾添加了一个bWarn用于决定是否显示警告
	bool bWarn = gbStartedOnceAPHS;   //LFX 2005.2.24 加,用于在启动程序后再选择规范时显时警告
	bool ret=FALSE;
	bool bSPECchanged=false;
	_variant_t tmpvar,vTmp;
	CString strTmp;
	int inttbExist=0;//0表示第一次进入循环，1表示表存在，2表示表不存在
	_ConnectionPtr db,db1;
	db.CreateInstance(__uuidof(Connection));
	db1.CreateInstance(__uuidof(Connection));
	_RecordsetPtr rsX;
	rsX.CreateInstance(__uuidof(Recordset));
	//add by lfx 2005.4.20
	CString strStandardConnect;   //标准连接件数据表名
	strStandardConnect.Format("CONNECTNW");
	//END add by lfx 2005.4.20
	try
	{
		//如果规范导入新的数据，或者选择规范发生变化(除开刚运行时)，则重新生成规范数据。
		//减少时间消耗。
		if( mbSPECchanged )
		{
		/*if((gsPhsConstantSpringSel == _T("")) 
		|| (gsPhsSPRINGSel == _T("")) 
		|| (gsPhsPASel == _T("")) 
		|| (gsPhsPARTSel == _T("")) 
		|| (gsPhsSASel == _T("")) 
		|| (gsPhsBoltsNutsSel == _T("")) )
		return false;
		else
			bSPECchanged=true;*/
			bSPECchanged=true;
		}
		else
		{
			if(gbStartedOnceAPHS)
			{
				if((gsPhsConstantSpringSel == gsOldPhsConstantSpringSel) 
					&& (gsPhsSPRINGSel == gsOldPhsSPRINGSel) 
					&& (gsPhsPASel == gsOldPhsPASel) 
					&& (gsPhsPARTSel == gsOldPhsPARTSel) 
					&& (gsPhsSASel == gsOldPhsSASel) 
					&& (gsPhsBoltsNutsSel == gsOldPhsBoltsNutsSel))
				{
					bSPECchanged=false;
					return TRUE;
				}
				else
				{
					bSPECchanged=true;					
				}
			}
			else
			{
				bSPECchanged=false;				
			}
		}

		long  lngErrNum ;
		CString sTmp;
		//过程较长
		AfxGetApp()->BeginWaitCursor();
		sTmp.Format(IDS_GetDataTableInfo,GetResStr(IDS_InfoPA));
		frmStatus.m_Label1 = sTmp;
		frmStatus.m_Label2 = _T("");
		frmStatus.SetWindowText(GetResStr(IDS_InitializeAutoPHS));
		frmStatus.UpdateData(false);
		frmStatus.ShowWindow(SW_SHOW);

		CString strConnect;
		strConnect.Format(_T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s;Jet OLEDB:Database Password=%s"),
			basDirectory::ProjectDBDir + _T("zdjcrude.mdb"), ModEncrypt::gstrDBZdjCrudePassWord);
		db->Open((_bstr_t)strConnect, "", "", adModeUnknown);

		strConnect="Provider=Microsoft.Jet.OLEDB.4.0;Persist Security Info=False;Data Source=" + basDirectory::ProjectDBDir+_T("sort.mdb");
		db1->Open((_bstr_t)strConnect, "", "", adConnectUnspecified);
		//正在获取管部信息...
		frmStatus.UpdateStatus(1.0 /9.0,true);
		frmStatus.UpdateData(false);
		frmStatus.UpdateWindow();
		//支吊架管部原始数据表名称
		CString SQLx;
		SQLx = _T("SELECT * FROM phsManuPA ORDER BY [Observation],[standard] ASC");
		rsX->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbSORT,true), 
			adOpenStatic, adLockOptimistic, adCmdText); 
		gsPhsPASel.TrimLeft();gsPhsPASel.TrimRight();
		if( rsX->BOF && rsX->adoEOF )
		{
			lngErrNum = IDS_NotFoundThisStandardInSortMdb;
			sTmp.Format(GetResStr(lngErrNum),EDIBgbl::GetDBName(db1),_T("phsManuPA"),gsPhsPASel);
			ShowMessage(sTmp);
			goto errH;
		}
		if(	gsPhsPASel != _T("") )
		{
			_variant_t vTmp;
			strTmp = (_T("standard=\'")+gsPhsPASel+_T("\'"));
			if( rsX->adoEOF)
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
		//以下判断管部原始数据表在数据库中是否存在
		while((inttbExist!=1)&&(!rsX->adoEOF))
		{
			rsX->get_Collect((_variant_t)_T("standard"),&tmpvar);
			if(tmpvar.vt==VT_NULL)
				gsPhsPASel=_T("");
			else
			{
				gsPhsPASel=vtos(tmpvar);
				gsPhsPASel.TrimLeft();gsPhsPASel.TrimRight();
			}
			rsX->get_Collect((_variant_t)_T("crudePA"),&tmpvar);
			if(tmpvar.vt==VT_NULL)
				tbnPA=_T("");
			else
			{
				tbnPA=vtos(tmpvar);
				tbnPA.TrimLeft();tbnPA.TrimRight();
			}
			rsX->get_Collect((_variant_t)_T("crudePAfix"),&tmpvar);
			if(tmpvar.vt==VT_NULL)
				tbnPAfix=_T("");
			else{
				tbnPAfix=vtos(tmpvar);
				tbnPAfix.TrimLeft();tbnPAfix.TrimRight();
			}
			rsX->get_Collect((_variant_t)_T("tbnPAid"),&tmpvar);
			if(tmpvar.vt==VT_NULL)
				tbnPAID=_T("");
			else{
				tbnPAID=vtos(tmpvar);
				tbnPAID.TrimLeft();tbnPAID.TrimRight();
			}
			if(!HStbExists(db,db1,tbnPA,_T("phsManuPA"),_T("crudePA"),gsPhsPASel,bWarn))
			{	
				if(inttbExist==0)	//第一次移到第一条记录上
				{
					rsX->MoveFirst();
					inttbExist=2;
				}
				else				//不是第一次，向后移一条记录
					rsX->MoveNext();
				continue;
			}

			if(!HStbExists(db,db1,tbnPAfix,_T("phsManuPA"),_T("crudePAfix"),gsPhsPASel,bWarn))
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
			if(!HStbExists(db,db1,tbnPAID,_T("phsManuPA"),_T("crudePAid"),gsPhsPASel,bWarn))
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
		if(inttbExist!=1)	//没找到原数据表
			goto errH;
		inttbExist=0;		//将条件变量设为初值，供后面的情况使用
		rsX->get_Collect((_variant_t)_T("standardDesc"),&tmpvar);
		if(tmpvar.vt==VT_NULL)
			gsPhsDescPASel=_T("");
		else
		{
			gsPhsDescPASel=vtos(tmpvar);
			gsPhsDescPASel.TrimLeft();gsPhsDescPASel.TrimRight();
		}
		//获取零部件选材标准索引号ClassIndex,
		//Observation字段指出了材料选择的索引号ClassIndex,根据该索引和温度、零部件代码ID，
		//可以从sort.mdb库中SpecificationOfMaterial表查到还没有选择材料的零部件可用的材料。
		//例如: ClassIndex=0,为汽水管道支吊架零部件可用的材料（新管规DL/T5054-1996）;ClassIndex=1,为烟风煤粉管道支吊架零部件可用的材料（新六道规程DL/T5121-2000）。
		rsX->get_Collect((_variant_t)_T("Observation"),&tmpvar);
		if(tmpvar.vt==VT_NULL)
			//默认按新管规DL/T5054-1996 p25 4.3.3 p67 7.5.2选择
			giClassIndex=0;
		else
			giClassIndex=vtoi(tmpvar);

		//以下删除tbnPAfix中与tbnPA相同的项目，防止CreateTmpCustomIDForPART函数中产生索引重复错误。
		sTmp=_T("DELETE FROM [") + tbnPAfix + _T("] WHERE CustomID IN ( SELECT DISTINCT CustomID FROM [") + tbnPA + _T("]) ");
		db->Execute((_bstr_t)sTmp, NULL, adCmdText);
		
		//正在获取连接件信息...
		sTmp.Format(IDS_GetDataTableInfo,GetResStr(IDS_InfoPART));
		frmStatus.m_Label1 = sTmp;
		frmStatus.UpdateStatus(2.0 /9.0,true);
		frmStatus.UpdateData(false);
		frmStatus.UpdateWindow();
		//支吊架连接件原始数据表名称
		if(rsX->State == adStateOpen)
			rsX->Close();
		sTmp = _T("SELECT * FROM phsManuPART ORDER BY [Observation],[standard] ASC");
		rsX->Open((_bstr_t)sTmp,_variant_t((IDispatch*)EDIBgbl::dbSORT,true), 
			adOpenStatic, adLockOptimistic, adCmdText); 
		gsPhsPARTSel.TrimLeft();gsPhsPARTSel.TrimRight(); 
		if( rsX->BOF && rsX->adoEOF )
		{
			lngErrNum = IDS_NotFoundThisStandardInSortMdb;
			sTmp.Format(GetResStr(lngErrNum),EDIBgbl::GetDBName(db1),_T("phsManuPART"),gsPhsPARTSel);
			ShowMessage(sTmp);
			goto errH;
		}
		if(gsPhsPARTSel != _T("") )
		{
			HRESULT hr = S_OK;
			rsX->MoveFirst();
			strTmp = (_T("standard=\'")+gsPhsPARTSel+_T("\'"));
			hr = rsX->Find((_bstr_t)strTmp, 0, adSearchForward);
			if( rsX->adoEOF)
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
		//以下判断连接件原始数据表在数据库中是否存在
		while((inttbExist!=1)&&(!rsX->adoEOF))
		{
			rsX->get_Collect((_variant_t)_T("standard"),&tmpvar);
			if(tmpvar.vt==VT_NULL)
				gsPhsPARTSel=_T("");
			else
			{
				gsPhsPARTSel=vtos(tmpvar);
				gsPhsPARTSel.TrimLeft();gsPhsPARTSel.TrimRight();
			}
			rsX->get_Collect((_variant_t)_T("crudePART"),&tmpvar);
			if(tmpvar.vt==VT_NULL)
				tbnPART=_T("");
			else
			{
				tbnPART=vtos(tmpvar);
				tbnPART.TrimLeft();tbnPART.TrimRight();
			}
			rsX->get_Collect((_variant_t)_T("tbnPARTid"),&tmpvar);
			if(tmpvar.vt==VT_NULL)
				tbnPARTID=_T("");
			else
			{
				tbnPARTID=vtos(tmpvar);
				tbnPARTID.TrimLeft();tbnPARTID.TrimRight();
			}
			rsX->get_Collect((_variant_t)_T("LugInfo"),&tmpvar);
			if(tmpvar.vt==VT_NULL)
				tbnLugInfo=_T("");
			else
			{
				tbnLugInfo=vtos(tmpvar);
				tbnLugInfo.TrimLeft();tbnLugInfo.TrimRight();
			}
			rsX->get_Collect((_variant_t)_T("LugLenCal"),&tmpvar);
			if(tmpvar.vt==VT_NULL)
				tbnLugLenCal=_T("");
			else
			{
				tbnLugLenCal=vtos(tmpvar);
				tbnLugLenCal.TrimLeft();tbnLugLenCal.TrimRight();
			}
			//add by ligb/lfx on 2005.04.20
			//华东院拉杆和花兰螺丝两端自带了扁螺母，所以其连接规则与西北院等其它标准不一样，
			//增加连接表功能满足这种情况
			rsX->get_Collect((_variant_t)_T("tbnCONNECT"),&tmpvar);
			if(tmpvar.vt==VT_NULL)
				tbnCONNECT = strStandardConnect;
			else
			{
				tbnCONNECT=vtos(tmpvar);
				tbnCONNECT.TrimLeft();tbnCONNECT.TrimRight();
			}
			//END add by ligb/lfx on 2005.04.20

			if(!HStbExists(db,db1,tbnLugInfo,_T("phsManuPART"),_T("LugInfo"),gsPhsPARTSel,bWarn))
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
			if(!HStbExists(db,db1,tbnLugLenCal,_T("phsManuPART"),_T("LugLenCal"),gsPhsPARTSel,bWarn))
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
			if(!HStbExists(db,db1,tbnPART,_T("phsManuPART"),_T("crudePART"),gsPhsPARTSel,bWarn))
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

			if(!HStbExists(db,db1,tbnPARTID,_T("phsManuPART"),_T("tbnPARTid"),gsPhsPARTSel,bWarn))
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
			goto errH;
		inttbExist=0;
		rsX->get_Collect((_variant_t)_T("standardDesc"),&tmpvar);
		if(tmpvar.vt==VT_NULL)
			gsPhsDescPARTSel=_T("");
		else
		{
			gsPhsDescPARTSel=vtos(tmpvar);
			gsPhsDescPARTSel.TrimLeft();gsPhsDescPARTSel.TrimRight();
		}
		
		//正在获取根部信息...
		sTmp.Format(IDS_GetDataTableInfo,GetResStr(IDS_InfoSA));
		frmStatus.m_Label1 = sTmp;
		frmStatus.UpdateStatus(3.0 /9.0,true);
		frmStatus.UpdateData(false);
		frmStatus.UpdateWindow();
		//支吊架根部原始数据表名称
		if(rsX->State == adStateOpen)
			rsX->Close();
		sTmp = _T("SELECT * FROM phsManuSA ORDER BY [Observation],[standard] ASC");
		rsX->Open((_bstr_t)sTmp,_variant_t((IDispatch*)EDIBgbl::dbSORT,true), 
			adOpenStatic, adLockOptimistic, adCmdText); 
		gsPhsSASel.TrimLeft();gsPhsSASel.TrimRight(); 
		if( rsX->BOF && rsX->adoEOF )
		{
			lngErrNum = IDS_NotFoundThisStandardInSortMdb;
			sTmp.Format(GetResStr(lngErrNum),EDIBgbl::GetDBName(db1),_T("phsManuSA"),gsPhsSASel);
			ShowMessage(sTmp);
			goto errH;
		}
		if(gsPhsSASel != _T("") )
		{
			HRESULT hr = S_OK;
			rsX->MoveFirst();
			strTmp = _T("standard=\'")+gsPhsSASel+_T("\'");
			hr = rsX->Find((_bstr_t)strTmp, 0, adSearchForward);
			if( rsX->adoEOF)
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
		BOOL bCrudeSA=FALSE,btbnSAid=FALSE,bCrudeSS=FALSE,tCrudeSSID=FALSE;
		while((inttbExist!=1)&&(!rsX->adoEOF))
		{
			rsX->get_Collect((_variant_t)_T("standard"),&tmpvar);
			if(tmpvar.vt==VT_NULL)
				gsPhsSASel=_T("");
			else
			{
				gsPhsSASel=vtos(tmpvar);
				gsPhsSASel.TrimLeft();gsPhsSASel.TrimRight();
			}
			rsX->get_Collect((_variant_t)_T("crudeSA"),&tmpvar);
			if(tmpvar.vt==VT_NULL)
				tbnSA=_T("");
			else
			{
				tbnSA=vtos(tmpvar);
				tbnSA.TrimLeft();tbnSA.TrimRight();
			}
			rsX->get_Collect((_variant_t)_T("tbnSAid"),&tmpvar);
			if(tmpvar.vt==VT_NULL)
				tbnSAID=_T("");
			else
			{
				tbnSAID=vtos(tmpvar);
				tbnSAID.TrimLeft();tbnSAID.TrimRight();
			}
			rsX->get_Collect((_variant_t)_T("crudeSectionSteel"),&tmpvar);
			if(tmpvar.vt==VT_NULL)
				tbnSectionSteel=_T("");
			else
			{
				tbnSectionSteel=vtos(tmpvar);
				tbnSectionSteel.TrimLeft();tbnSectionSteel.TrimRight();
			}
			rsX->get_Collect((_variant_t)_T("crudeSectionSteelID"),&tmpvar);
			if(tmpvar.vt==VT_NULL)
				tbnSectionSteelID=_T("");
			else
			{
				tbnSectionSteelID=vtos(tmpvar);
				tbnSectionSteelID.TrimLeft();tbnSectionSteelID.TrimRight();
			}
			//根部螺栓螺母数据表
			rsX->get_Collect((_variant_t)_T("crudeSABoltsNuts"),&tmpvar);
			if(tmpvar.vt==VT_NULL)
				tbnSABoltsNuts=_T("");
			else
			{
				tbnSABoltsNuts=vtos(tmpvar);
				tbnSABoltsNuts.TrimLeft();tbnSABoltsNuts.TrimRight();
			}
			rsX->get_Collect((_variant_t)_T("crudeAttachmentID"),&tmpvar);
			if(tmpvar.vt==VT_NULL)
				tbnSABoltsNutsID=_T("");
			else
			{
				tbnSABoltsNutsID=vtos(tmpvar);
				tbnSABoltsNutsID.TrimLeft();tbnSABoltsNutsID.TrimRight();
			}
			//根部附件数据表
			rsX->get_Collect((_variant_t)_T("crudeAttachment"),&tmpvar);
			if(tmpvar.vt==VT_NULL)
				tbnAttachment=_T("");
			else
			{
				tbnAttachment=vtos(tmpvar);
				tbnAttachment.TrimLeft();tbnAttachment.TrimRight();
			}
			rsX->get_Collect((_variant_t)_T("crudeAttachmentID"),&tmpvar);
			if(tmpvar.vt==VT_NULL)
				tbnAttachmentID=_T("");
			else
			{
				tbnAttachmentID=vtos(tmpvar);
				tbnAttachmentID.TrimLeft();tbnAttachmentID.TrimRight();
			}
			//以下判断根部使用的附件表在数据库中是否存在
			if(!HStbExists(db,db1,tbnAttachment,_T("phsManuSA"),_T("crudeAttachment"),gsPhsSASel,bWarn))
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
			if(!HStbExists(db,db1,tbnAttachmentID,_T("phsManuSA"),_T("crudeAttachmentID"),gsPhsSASel,bWarn))
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
			//以下判断根部使用的螺栓螺母表在数据库中是否存在
			if(!HStbExists(db,db1,tbnSABoltsNuts,_T("phsManuSA"),_T("crudeSABoltsNuts"),gsPhsSASel,bWarn))
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
			if(!HStbExists(db,db1,tbnSABoltsNutsID,_T("phsManuSA"),_T("crudeSABoltsNutsID"),gsPhsSASel,bWarn))
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
			//以下判断根部原始数据表在数据库中是否存在
			if(!HStbExists(db,db1,tbnSA,_T("phsManuSA"),_T("crudeSA"),gsPhsSASel,bWarn))
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
			if(!HStbExists(db,db1,tbnSAID,_T("phsManuSA"),_T("tbnSAID"),gsPhsSASel,bWarn))
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
			//以下判断根部使用的型钢表在数据库中是否存在
			if(!HStbExists(db,db1,tbnSectionSteel,_T("phsManuSA"),_T("crudeSectionSteel"),gsPhsSASel,bWarn))
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
			if(!HStbExists(db,db1,tbnSectionSteelID,_T("phsManuSA"),_T("crudeSectionSteelID"),gsPhsSASel,bWarn))
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
		goto errH;
	inttbExist=0;
	rsX->get_Collect((_variant_t)_T("standardDesc"),&tmpvar);
	if(tmpvar.vt==VT_NULL)
		gsPhsDescSASel=_T("");
	else
	{
		gsPhsDescSASel=vtos(tmpvar);
		gsPhsDescSASel.TrimLeft();gsPhsDescSASel.TrimRight();
	}
	//正在获取螺栓螺母信息...
	sTmp.Format(IDS_GetDataTableInfo,GetResStr(IDS_InfoBoltsNuts));
	frmStatus.m_Label1 = sTmp;
	frmStatus.UpdateStatus(4.0 /9.0,true);
	frmStatus.UpdateData(false);
	frmStatus.UpdateWindow();
	//螺栓螺母原始数据表名称
	if(rsX->State == adStateOpen)
		rsX->Close();
	sTmp = _T("SELECT * FROM phsManuBoltsNuts ORDER BY [Observation],[standard] ASC");
	rsX->Open((_bstr_t)sTmp,_variant_t((IDispatch*)EDIBgbl::dbSORT,true), 
		adOpenStatic, adLockOptimistic, adCmdText); 
	gsPhsBoltsNutsSel.TrimLeft();gsPhsBoltsNutsSel.TrimRight(); 
	if( rsX->BOF && rsX->adoEOF )
	{
		lngErrNum = IDS_NotFoundThisStandardInSortMdb;
		sTmp.Format(GetResStr(lngErrNum),EDIBgbl::GetDBName(db1),_T("phsManuBoltsNuts"),gsPhsBoltsNutsSel);
		ShowMessage(sTmp);
		goto errH;
	}
	if(gsPhsBoltsNutsSel != _T("") )
	{
		HRESULT hr = S_OK;
		rsX->MoveFirst();
		strTmp = _T("standard=\'")+gsPhsBoltsNutsSel+_T("\'");
		hr = rsX->Find((_bstr_t)strTmp, 0, adSearchForward);
		if( rsX->adoEOF)
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
			gsPhsBoltsNutsSel=_T("");
		else
		{
			gsPhsBoltsNutsSel=vtos(tmpvar);
			gsPhsBoltsNutsSel.TrimLeft();gsPhsBoltsNutsSel.TrimRight();
		}
		rsX->get_Collect((_variant_t)_T("crudeBoltsNuts"),&tmpvar);
		if(tmpvar.vt==VT_NULL)
			tbnBoltsNuts=_T("");
		else
		{
			tbnBoltsNuts=vtos(tmpvar);
			tbnBoltsNuts.TrimLeft();tbnBoltsNuts.TrimRight();
		}
		rsX->get_Collect((_variant_t)_T("crudeBoltsNutsID"),&tmpvar);
		if(tmpvar.vt==VT_NULL)
			tbnBoltsNutsID=_T("");
		else
		{
			tbnBoltsNutsID=vtos(tmpvar);
			tbnBoltsNutsID.TrimLeft();tbnBoltsNutsID.TrimRight();
		}
		rsX->get_Collect((_variant_t)_T("BoltsSurplusLength"),&tmpvar);
		if(tmpvar.vt==VT_NULL)
			tbnBoltsSurplusLength=_T("");
		else
		{
			tbnBoltsSurplusLength=vtos(tmpvar);
			tbnBoltsSurplusLength.TrimLeft();tbnBoltsSurplusLength.TrimRight();
		}
		//以下判断螺栓螺母表在数据库中是否存在
		if(!HStbExists(db,db1,tbnBoltsNuts,_T("phsManuBoltsNuts"),_T("crudeBoltsNuts"),gsPhsBoltsNutsSel,bWarn))
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
		if(!HStbExists(db,db1,tbnBoltsNutsID,_T("phsManuBoltsNuts"),_T("crudeBoltsNutsID"),gsPhsBoltsNutsSel,bWarn))
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
		if(!HStbExists(db,db1,tbnBoltsSurplusLength,_T("phsManuBoltsNuts"),_T("BoltsSurplusLength"),gsPhsBoltsNutsSel,bWarn))
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
		goto errH;
	inttbExist=0;
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	
	//正在获取弹簧信息...
	sTmp.Format(IDS_GetDataTableInfo,GetResStr(IDS_InfoSPRING));
	frmStatus.m_Label1 = sTmp;
	frmStatus.UpdateStatus(5.0 /9.0,true);
	frmStatus.UpdateData(false);
	frmStatus.UpdateWindow();
	//弹簧原始数据
	if(rsX->State == adStateOpen)
		rsX->Close();
	sTmp = _T("SELECT * FROM phsManuSPRING ORDER BY [Observation],[standard] ASC");
	rsX->Open((_bstr_t)sTmp,_variant_t((IDispatch*)EDIBgbl::dbSORT,true), 
		adOpenDynamic, adLockOptimistic, adCmdText); 
	gsPhsSPRINGSel.TrimLeft();gsPhsSPRINGSel.TrimRight(); 
	if( rsX->BOF && rsX->adoEOF ){
		lngErrNum = IDS_NotFoundThisStandardInSortMdb;
		sTmp.Format(GetResStr(lngErrNum),EDIBgbl::GetDBName(db1),_T("phsManuBoltsNuts"),gsPhsBoltsNutsSel);
		ShowMessage(sTmp);
		goto errH;
	}
	if(gsPhsSPRINGSel != _T("") )
	{
		  HRESULT hr = S_OK;
		  rsX->MoveFirst();
		  strTmp = _T("standard=\'")+gsPhsSPRINGSel+_T("\'");
		  hr = rsX->Find((_bstr_t)strTmp, 0, adSearchForward);
		  if( rsX->adoEOF)
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
			gsPhsSPRINGSel=_T("");
		else{
			gsPhsSPRINGSel=vtos(tmpvar);
			gsPhsSPRINGSel.TrimLeft();gsPhsSPRINGSel.TrimRight();
		}
		rsX->get_Collect((_variant_t)_T("Spring_property"),&tmpvar);
		if(tmpvar.vt==VT_NULL)
			tbnSPRINGproperty=_T("");
		else{
			tbnSPRINGproperty=vtos(tmpvar);
			tbnSPRINGproperty.TrimLeft();tbnSPRINGproperty.TrimRight();
		}
		rsX->get_Collect((_variant_t)_T("Spring_propertyMaxDist"),&tmpvar);
		if(tmpvar.vt==VT_NULL)
			tbnDiscSPRINGpropertyMaxDist=_T("");
		else{
			tbnDiscSPRINGpropertyMaxDist=vtos(tmpvar);
			tbnDiscSPRINGpropertyMaxDist.TrimLeft();tbnDiscSPRINGpropertyMaxDist.TrimRight();
		}
		rsX->get_Collect((_variant_t)_T("Spring_id"),&tmpvar);
		if(tmpvar.vt==VT_NULL)
			tbnSPRINGid=_T("");
		else{
			tbnSPRINGid=vtos(tmpvar);
			tbnSPRINGid.TrimLeft();tbnSPRINGid.TrimRight();
		}
		rsX->get_Collect((_variant_t)_T("Spring_Crude"),&tmpvar);
		if(tmpvar.vt==VT_NULL)
			tbnSPRINGCrude=_T("");
		else{
			tbnSPRINGCrude=vtos(tmpvar);
			tbnSPRINGCrude.TrimLeft();tbnSPRINGCrude.TrimRight();
		}
		rsX->get_Collect((_variant_t)_T("CrudeTurnbuckle"),&tmpvar);
		if(tmpvar.vt==VT_NULL)
			tbnSPRINGL5Crude=_T("");
		else{
			tbnSPRINGL5Crude=vtos(tmpvar);
			tbnSPRINGL5Crude.TrimLeft();tbnSPRINGL5Crude.TrimRight();
		}
		//以下判断弹簧原始数据表在数据库中是否存在
		rsX->get_Collect((_variant_t)_T("DiameterSerial"),&tmpvar);
		if(tmpvar.vt==VT_NULL)
			tbnSPRDiameterSerial=_T("");
		else{
			tbnSPRDiameterSerial=vtos(tmpvar);
			tbnSPRDiameterSerial.TrimLeft();tbnSPRDiameterSerial.TrimRight();
		}
		if(!HStbExists(db,db1,tbnSPRDiameterSerial,_T("phsManuSPRING"),_T("DiameterSerial"),gsPhsSPRINGSel,bWarn))
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
		if(!HStbExists(db,db1,tbnSPRINGCrude,_T("phsManuSPRING"),_T("Spring_Crude"),gsPhsSPRINGSel,bWarn))
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
		if(!HStbExists(db,db1,tbnSPRINGL5Crude,_T("phsManuSPRING"),_T("CrudeTurnbuckle"),gsPhsSPRINGSel,bWarn))
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
		if(!HStbExists(db,db1,tbnSPRINGid,_T("phsManuSPRING"),_T("Spring_id"),gsPhsSPRINGSel,bWarn))
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
		if(!HStbExists(db,db1,tbnSPRINGproperty,_T("phsManuSPRING"),_T("Spring_property"),gsPhsSPRINGSel,bWarn))
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
		if(!HStbExists(db,db1,tbnDiscSPRINGpropertyMaxDist,_T("phsManuSPRING"),_T("Spring_propertyMaxDist"),gsPhsSPRINGSel,bWarn))
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
		goto errH;
	inttbExist=0;
	   rsX->get_Collect((_variant_t)_T("SerialNumPerSPR"),&tmpvar);
	   if(tmpvar.vt==VT_NULL)
		   giAllowedMaxSerialNumPerSPR=0;
	   else{
		   giAllowedMaxSerialNumPerSPR=vtoi(tmpvar);
		   //   giAllowedMaxSerialNumPerSPR.TrimLeft();giAllowedMaxSerialNumPerSPR.TrimRight();
	   }
	   rsX->get_Collect((_variant_t)_T("SerialNumSPRs"),&tmpvar);
	   if(tmpvar.vt==VT_NULL)
		   giAllowedMaxSerialNumSPRs=0;
	   else{
		   giAllowedMaxSerialNumSPRs=vtoi(tmpvar);
		   //giAllowedMaxSerialNumSPRs.TrimLeft();giAllowedMaxSerialNumSPRs.TrimRight();
	   }
	   //当导入了规范或者启动过之后
	   if((bSPECchanged || gbStartedOnceAPHS) && !(gsPhsSPRINGSel == gsOldPhsSPRINGSel))
		   CreateTmpSPRPropertyTable(giAllowedMaxSerialNumSPRs);

	   //获得弹簧或恒力弹簧编号规格的单位制
	   //以下判断恒力弹簧/碟簧规格编号力单位制描述字段值是否存在
	   rsX->get_Collect((_variant_t)_T("Unit_Force"),&tmpvar);
	   if(tmpvar.vt==VT_NULL)
	   {
		   lngErrNum = IDS_NullXfieldInphsManuXOfsortMdb;
		   sTmp.Format(GetResStr(lngErrNum),EDIBgbl::GetDBName(db1),_T("phsManuSPRING"),gsPhsSPRINGSel ,_T("Unit_Force"));
		   ShowMessage(sTmp);
		   goto errH;
	   }
	   else{
		   sTmp=vtos(tmpvar);
		   sTmp.TrimLeft();sTmp.TrimRight();
		   if(sTmp.Left(1)==_T("N"))
		   {
			   if(!modPHScal::gbSPRBHLOADUNITBYNEWTON)
			   {
				   gsSPRUnitOfLoad = _T("kgf");
				   rsX->put_Collect((_variant_t)_T("Unit_Force"),STR_VAR("kgf"));rsX->Update();
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
					gsSPRUnitOfLoad = _T("N");
			   }

		   }
		   else if(sTmp.Left(3)==_T("daN"))
			   gsSPRUnitOfLoad = _T("daN");
		   else if(sTmp.Left(3)==_T("kgf"))
		   {
			   if(modPHScal::gbSPRBHLOADUNITBYNEWTON)
			   {
				   gsSPRUnitOfLoad = _T("N");
				   rsX->put_Collect((_variant_t)_T("Unit_Force"),_variant_t("N"));
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
					gsSPRUnitOfLoad = _T("kgf");
			   }
		   }
		   else  
		   {//没找任何荷载单位制识别字符
			   sTmp.Format(GetResStr(IDS_NotFoundUnitStringInBHFormat),EDIBgbl::GetDBName(db1),_T("phsManuSPRING"),GetResStr(IDS_SPR),vtos(tmpvar));
			   ShowMessage(sTmp);
			   gsSPRUnitOfLoad = _T("daN");
		   }
	   }
	   rsX->get_Collect((_variant_t)_T("DOWNchar"),&tmpvar);
	   if(tmpvar.vt==VT_NULL)
		   gSprDirectionChar.SPR_Down=_T("");
	   else
	   {
		   gSprDirectionChar.SPR_Down=vtos(tmpvar);
		   gSprDirectionChar.SPR_Down.TrimLeft();gSprDirectionChar.SPR_Down.TrimRight();
	   }
	   rsX->get_Collect((_variant_t)_T("UPchar"),&tmpvar);
	   if(tmpvar.vt==VT_NULL)
		   gSprDirectionChar.SPR_UP=_T("");
	   else
	   {
		   gSprDirectionChar.SPR_UP=vtos(tmpvar);
		   gSprDirectionChar.SPR_UP.TrimLeft();gSprDirectionChar.SPR_UP.TrimRight();
	   }
	   //以下判断弹簧/碟簧方向描述字段值是否存在
	   if (gSprDirectionChar.SPR_Down.GetLength()<=0 || gSprDirectionChar.SPR_Down == _T("") )
	   {
		   gSprDirectionChar.SPR_Down = _T("");
		   lngErrNum = IDS_NullXfieldInphsManuXOfsortMdb;
		   sTmp.Format(GetResStr(lngErrNum),EDIBgbl::GetDBName(db1),_T("phsManuSPRING"),gsPhsSPRINGSel,_T("DOWNchar"));
		   //ShowMessage(sTmp); //by ligb on 2008.05.10 2007版苏源恒力弹簧方向字符向下用-号表示，但位移中已经含有-号，故向下方向代号只能用空表示，因此修改代码适应这情况
		   //goto errH;
	   }
	   if (gSprDirectionChar.SPR_UP.GetLength()<=0 || gSprDirectionChar.SPR_UP == _T("") )
	   {
		   gSprDirectionChar.SPR_UP = _T("");
		   lngErrNum = IDS_NullXfieldInphsManuXOfsortMdb;
		   sTmp.Format(GetResStr(lngErrNum),EDIBgbl::GetDBName(db1),_T("phsManuSPRING"),gsPhsSPRINGSel,_T("UPchar"));
		   //ShowMessage(sTmp); //by ligb on 2008.05.10 2007版苏源恒力弹簧方向字符向下用-号表示，但位移中已经含有-号，故向下方向代号只能用空表示，因此修改代码适应这情况
		   //goto errH;
	   }
	   
	   //以下判断弹簧/碟簧安装尺寸描述字段值是否存在
	   rsX->get_Collect((_variant_t)_T("H1IncludeB"), &tmpvar);
	   if( tmpvar.vt==VT_NULL) {
		   gbH1IncludeB = TRUE;
		   lngErrNum = IDS_NullXfieldInphsManuXOfsortMdb;
		   sTmp.Format(GetResStr(lngErrNum), EDIBgbl::GetDBName(db1), _T("phsManuSPRING"), gsPhsSPRINGSel, _T("H1IncludeB"));
		   ShowMessage(sTmp);
		   goto errH;
	   }else{
		   gbH1IncludeB = tmpvar.boolVal;
	   }

	   rsX->get_Collect((_variant_t)_T("H1Pos"),&tmpvar);
	   if(tmpvar.vt==VT_NULL){
		   gH1Pos = 0;
		   lngErrNum = IDS_NullXfieldInphsManuXOfsortMdb;
		   sTmp.Format(GetResStr(lngErrNum), EDIBgbl::GetDBName(db1), _T("phsManuSPRING"), gsPhsSPRINGSel, _T("H1Pos"));
		   ShowMessage(sTmp);
		   goto errH;
	   }else{
		   gH1Pos = vtoi(tmpvar);
	   }

	   rsX->get_Collect((_variant_t)_T("H1distPos"),&tmpvar);
	   if(tmpvar.vt==VT_NULL){
		   gH1distPos = 0;
		   lngErrNum = IDS_NullXfieldInphsManuXOfsortMdb;
		   sTmp.Format(GetResStr(lngErrNum), EDIBgbl::GetDBName(db1), _T("phsManuSPRING"), gsPhsSPRINGSel, _T("H1distPos"));
		   ShowMessage(sTmp);
		   goto errH;
	   }else{
		   gH1distPos =vtoi(tmpvar);
	   }
	   rsX->get_Collect((_variant_t)_T("manufactory"),&tmpvar);
	   if(tmpvar.vt==VT_NULL)
		   gsSPRmanufactory=_T("");
	   else{
		   gsSPRmanufactory=vtos(tmpvar);
		   gsSPRmanufactory.TrimLeft();gsSPRmanufactory.TrimRight();
	   }

	   //正在获取恒力弹簧信息...
	   sTmp.Format(IDS_GetDataTableInfo,GetResStr(IDS_InfoCSPRING));
	   frmStatus.m_Label1 = sTmp;
	   frmStatus.UpdateStatus(6.0 /9.0,true);
	   frmStatus.UpdateData(false);
	   frmStatus.ShowWindow(SW_SHOW);
	   frmStatus.UpdateWindow();
	   //恒力弹簧/碟簧原始数据
	   if(rsX->State == adStateOpen)
		   rsX->Close();
	   SQLx = _T("SELECT * FROM phsManuConstantSPRING ORDER BY [Observation],[standard] ASC");
	   rsX->Open(_variant_t(SQLx),(IDispatch*)EDIBgbl::dbSORT,
		   adOpenStatic, adLockOptimistic,adCmdText);
	   gsPhsConstantSpringSel.TrimLeft();gsPhsConstantSpringSel.TrimRight(); 
	   /*if( rsX->BOF && rsX->adoEOF ){
	   lngErrNum = IDS_NotFoundThisStandardInSortMdb;
	   sTmp.Format(GetResStr(lngErrNum),EDIBgbl::GetDBName(db1),_T("phsManuBoltsNuts"),gsPhsBoltsNutsSel);
	   ShowMessage(sTmp);
	   goto errH;
         }*/
	   if(gsPhsConstantSpringSel != _T("") ){
		   HRESULT hr = S_OK;
		   rsX->MoveFirst();
		   strTmp = _T("standard=\'")+gsPhsConstantSpringSel+_T("\'");
		   hr = rsX->Find((_bstr_t)strTmp, 0, adSearchForward);
		   if( rsX->adoEOF)
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
	   //以下判断恒力弹簧/碟原始数据表在数据库中是否存在,
	   //如果不存在，则将记录指针移到第一条记录上，从头开始检查每条记录
	   //如果所有的记录的原始数据表在数据库中都不存在,则报错
	   //如果遇到原始数据表在数据库中存在的记录,则选用这条记录
	   while((inttbExist!=1)&&(!rsX->adoEOF))
	   {
		   rsX->get_Collect((_variant_t)_T("standard"),&tmpvar);
		   if(tmpvar.vt==VT_NULL)
			   gsPhsConstantSpringSel=_T("");
		   else{
			   gsPhsConstantSpringSel=vtos(tmpvar);
			   gsPhsConstantSpringSel.TrimLeft();gsPhsConstantSpringSel.TrimRight();
		   }
		   rsX->get_Collect((_variant_t)_T("manufactory"),&tmpvar);
		   if(tmpvar.vt==VT_NULL)
			   gsCSPRmanufactory=_T("");
		   else{
			   gsCSPRmanufactory=vtos(tmpvar);
			   gsCSPRmanufactory.TrimLeft();gsCSPRmanufactory.TrimRight();
		   }
		   rsX->get_Collect((_variant_t)_T("Spring_Crude"),&tmpvar);
		   if(tmpvar.vt==VT_NULL)
			   tbnHDCrude=_T("");
		   else{
			   tbnHDCrude=vtos(tmpvar);
			   tbnHDCrude.TrimLeft();tbnHDCrude.TrimRight();
		   }
		   rsX->get_Collect((_variant_t)_T("Spring_property"),&tmpvar);
		   if(tmpvar.vt==VT_NULL)
			   tbnHDproperty=_T("");
		   else{
			   tbnHDproperty=vtos(tmpvar);
			   tbnHDproperty.TrimLeft();tbnHDproperty.TrimRight();
		   }
		   rsX->get_Collect((_variant_t)_T("Spring_id"),&tmpvar);
		   if(tmpvar.vt==VT_NULL)
			   tbnHDid=_T("");
		   else{
			   tbnHDid=vtos(tmpvar);
			   tbnHDid.TrimLeft();tbnHDid.TrimRight();
		   }
		   rsX->get_Collect((_variant_t)_T("CrudeTurnbuckle"),&tmpvar);
		   if(tmpvar.vt==VT_NULL)
			   tbnCSPRINGL5Crude=_T("");
		   else{
			   tbnCSPRINGL5Crude=vtos(tmpvar);
			   tbnCSPRINGL5Crude.TrimLeft();tbnCSPRINGL5Crude.TrimRight();
		   }
		   rsX->get_Collect((_variant_t)_T("LugDiaOfCSPR"),&tmpvar);
		   if(tmpvar.vt==VT_NULL)
			   tbnLugDiaOfCSPR=_T("");
		   else{
			   tbnLugDiaOfCSPR=vtos(tmpvar);
			   tbnLugDiaOfCSPR.TrimLeft();tbnLugDiaOfCSPR.TrimRight();
		   }
		   rsX->get_Collect((_variant_t)_T("DisplacementSerial"),&tmpvar);
		   if(tmpvar.vt==VT_NULL)
			   tbnDisplacementSerial=_T("");
		   else{
			   tbnDisplacementSerial=vtos(tmpvar);
			   tbnDisplacementSerial.TrimLeft();tbnDisplacementSerial.TrimRight();
		   }
		   rsX->get_Collect((_variant_t)_T("DiameterSerial"),&tmpvar);
		   if(tmpvar.vt==VT_NULL)
			   tbnCSPRDiameterSerial=_T("");
		   else{
			   tbnCSPRDiameterSerial=vtos(tmpvar);
			   tbnCSPRDiameterSerial.TrimLeft();tbnCSPRDiameterSerial.TrimRight();
		   }


		   if(!HStbExists(db,db1,tbnCSPRDiameterSerial,_T("phsManuConstantSPRING"),_T("DiameterSerial"),gsPhsConstantSpringSel,bWarn))
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
		   if(!HStbExists(db,db1,tbnHDCrude,_T("phsManuConstantSPRING"),_T("Spring_Crude"),gsPhsConstantSpringSel,bWarn))
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
		   if(!HStbExists(db,db1,tbnHDid,_T("phsManuConstantSPRING"),_T("Spring_id"),gsPhsConstantSpringSel,bWarn))
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
		   if(!HStbExists(db,db1,tbnHDproperty,_T("phsManuConstantSPRING"),_T("Spring_property"),gsPhsConstantSpringSel,bWarn))
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
		   if(!HStbExists(db,db1,tbnLugDiaOfCSPR,_T("phsManuConstantSPRING"),_T("Spring_property"),gsPhsConstantSpringSel,bWarn))
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
		   if(!HStbExists(db,db1,tbnDisplacementSerial,_T("phsManuConstantSPRING"),_T("DisplacementSerial"),gsPhsConstantSpringSel,bWarn))
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
		   if(!HStbExists(db,db1,tbnCSPRINGL5Crude,_T("phsManuConstantSPRING"),_T("CrudeTurnbuckle"),gsPhsConstantSpringSel,bWarn))
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
	if (inttbExist!=1)
		goto errH;
	inttbExist=0;
	//以下判断恒力弹簧/碟簧规格编号力单位制描述字段值是否存在

	rsX->get_Collect((_variant_t)_T("Unit_Force"),&tmpvar);
	if(tmpvar.vt==VT_NULL)
	{
		lngErrNum = IDS_NullXfieldInphsManuXOfsortMdb;
		sTmp.Format(GetResStr(lngErrNum),EDIBgbl::GetDBName(db1),_T("phsManuConstantSPRING"),gsPhsConstantSpringSel ,_T("Unit_Force"));
		ShowMessage(sTmp);
		goto errH;
	}
	else{
		sTmp=vtos(tmpvar);
		sTmp.TrimLeft();sTmp.TrimRight();
		if(sTmp.Left(1)==_T("N"))
			gsCSPRUnitOfLoad = _T("N");
		else if(sTmp.Left(3)==_T("daN"))
			gsCSPRUnitOfLoad = _T("daN");
		else if(sTmp.Left(3)==_T("kgf"))
			gsCSPRUnitOfLoad = _T("kgf");
		else
			//没找任何荷载单位制识别字符
		{
			sTmp.Format(GetResStr(IDS_NotFoundUnitStringInBHFormat),EDIBgbl::GetDBName(db1),_T("phsManuConstantSPRING"),GetResStr(IDS_CSPR),vtos(tmpvar));
			ShowMessage(sTmp);
			gsCSPRUnitOfLoad = _T("daN");
		}

	}

	//按照荷载/位移选择恒力弹簧的拉杆直径TRUE/FALSE
	//以下判断恒力弹簧/碟簧安装尺寸描述字段值是否存在
	rsX->get_Collect((_variant_t)_T("bByForce"), &tmpvar);
	if(tmpvar.vt==VT_NULL ){
		gbByForce = TRUE;
		lngErrNum = IDS_NullXfieldInphsManuXOfsortMdb;
		sTmp.Format(GetResStr(lngErrNum), EDIBgbl::GetDBName(db1), _T("phsManuConstantSPRING"), gsPhsConstantSpringSel, _T("bByForce"));
		ShowMessage(sTmp);
		goto errH;
	}else{
		gbByForce = tmpvar.boolVal;
	}
	//以下判断恒力弹簧/碟簧安装尺寸描述字段值是否存在
	rsX->get_Collect((_variant_t)_T("bE_FormulaPlusHalf"), &tmpvar);
	if(tmpvar.vt==VT_NULL ){
		bE_FormulaPlusHalf = false;
		lngErrNum = IDS_NullXfieldInphsManuXOfsortMdb;
		sTmp.Format(GetResStr(lngErrNum), EDIBgbl::GetDBName(db1), _T("phsManuConstantSPRING"), gsPhsConstantSpringSel, _T("bE_FormulaPlusHalf"));
		ShowMessage(sTmp);
		goto errH;
	}else{
		bE_FormulaPlusHalf = tmpvar.boolVal;
	}


	rsX->get_Collect((_variant_t)_T("H1IncludeB"), &tmpvar);
	if(tmpvar.vt==VT_NULL )
	{
		gbCH1IncludeB = TRUE;
		lngErrNum = IDS_NullXfieldInphsManuXOfsortMdb;
		sTmp.Format(GetResStr(lngErrNum), EDIBgbl::GetDBName(db1), _T("phsManuConstantSPRING"), gsPhsConstantSpringSel, _T("H1IncludeB"));
		ShowMessage(sTmp);
		goto errH;
	}
	else
	{
		gbCH1IncludeB = vtob(tmpvar);//tmpvar.boolVal
	}

	rsX->get_Collect((_variant_t)_T("bH1IncludeT"), &tmpvar);
	if(tmpvar.vt==VT_NULL )
	{
		gbCH1IncludeT = TRUE;
		lngErrNum = IDS_NullXfieldInphsManuXOfsortMdb;
		sTmp.Format(GetResStr(lngErrNum), EDIBgbl::GetDBName(db1), _T("phsManuConstantSPRING"), gsPhsConstantSpringSel, _T("bH1IncludeT"));
		ShowMessage(sTmp);
		goto errH;
	}
	else
	{
		gbCH1IncludeT = vtob(tmpvar);//tmpvar.boolVal
	}

	rsX->get_Collect((_variant_t)_T("H1Pos"), &tmpvar);
	if(tmpvar.vt==VT_NULL )
	{
		gCH1Pos = 0;
		lngErrNum = IDS_NullXfieldInphsManuXOfsortMdb;
		sTmp.Format(GetResStr(lngErrNum), EDIBgbl::GetDBName(db1), _T("phsManuConstantSPRING"), gsPhsConstantSpringSel, _T("H1Pos"));
		ShowMessage(sTmp);
		goto errH;
	}
	else
	{
		gCH1Pos = vtoi(tmpvar);
	}
	rsX->get_Collect((_variant_t)_T("H1distPos"), &tmpvar);
	if(tmpvar.vt==VT_NULL ){
		gCH1distPos = 0;
		lngErrNum = IDS_NullXfieldInphsManuXOfsortMdb;
		sTmp.Format(GetResStr(lngErrNum), EDIBgbl::GetDBName(db1), _T("phsManuConstantSPRING"), gsPhsConstantSpringSel, _T("H1distPos"));
		ShowMessage(sTmp);
		goto errH;
	}else{
		gCH1distPos = vtoi(tmpvar);
	}

	rsX->get_Collect((_variant_t)_T("DOWNchar"),&tmpvar);
	if(tmpvar.vt==VT_NULL)
		gSprDirectionChar.CSPR_Down=_T("");
	else{
		gSprDirectionChar.CSPR_Down=vtos(tmpvar);
		gSprDirectionChar.CSPR_Down.TrimLeft();gSprDirectionChar.CSPR_Down.TrimRight();
	}
	rsX->get_Collect((_variant_t)_T("UPchar"),&tmpvar);
	if(tmpvar.vt==VT_NULL)
		gSprDirectionChar.CSPR_UP=_T("");
	else{
		gSprDirectionChar.CSPR_UP=vtos(tmpvar);
		gSprDirectionChar.CSPR_UP.TrimLeft();gSprDirectionChar.CSPR_UP.TrimRight();
	}
	//以下判断恒力弹簧/碟方向描述字段值是否存在
	if( gSprDirectionChar.CSPR_Down.GetLength()<=0 || gSprDirectionChar.CSPR_Down == _T("") ){
		gSprDirectionChar.CSPR_Down = _T("");
		lngErrNum = IDS_NullXfieldInphsManuXOfsortMdb;
		sTmp.Format(GetResStr(lngErrNum),EDIBgbl::GetDBName(db1),_T("phsManuConstantSPRING"),gsPhsConstantSpringSel,_T("DOWNchar"));
		//ShowMessage(sTmp);
		//goto errH;
	}
	if( gSprDirectionChar.CSPR_UP.GetLength()<=0 || gSprDirectionChar.CSPR_UP == _T("") ){
		gSprDirectionChar.CSPR_UP = _T("");
		lngErrNum = IDS_NullXfieldInphsManuXOfsortMdb;
		sTmp.Format(GetResStr(lngErrNum),EDIBgbl::GetDBName(db1),_T("phsManuConstantSPRING"),gsPhsConstantSpringSel,_T("UPchar"));
		//ShowMessage(sTmp);
		//goto errH;
	}
	
	//默认热位移计算裕量为20%
	rsX->get_Collect((_variant_t)_T("PercentSurplusDist"), &tmpvar);
	if( tmpvar.vt==VT_NULL){
		rsX->put_Collect((_variant_t)_T("PercentSurplusDist"),_variant_t((float)0.2));
		//rsX->PutCollect(_T("PercentSurplusDist"),_variant_t((float)0.2));
		
	}
	//默认热位移计算裕量最小值为15mm
	rsX->get_Collect((_variant_t)_T("MinSurplusDist"), &tmpvar);
	if( tmpvar.vt==VT_NULL){
		rsX->put_Collect((_variant_t)_T("MinSurplusDist"),_variant_t((float)15));
		// rsX->PutCollect(_T("MinSurplusDist"),_variant_t((float)15));
	}
	rsX->get_Collect((_variant_t)_T("PercentSurplusDist"),&tmpvar);
	if(tmpvar.vt==VT_NULL)
		gUseConstantSpringPercentSurplusDist=0;
	else{
		gUseConstantSpringPercentSurplusDist=tmpvar.fltVal;
		//   gUseConstantSpringPercentSurplusDist.TrimLeft();gUseConstantSpringPercentSurplusDist.TrimRight();
	}
	rsX->get_Collect((_variant_t)_T("MinSurplusDist"),&tmpvar);
	if(tmpvar.vt==VT_NULL)
		gUseConstantSpringMinSurplusDist=0;
	else{
		gUseConstantSpringMinSurplusDist=tmpvar.fltVal;
		//  gUseConstantSpringMinSurplusDist.TrimLeft();gUseConstantSpringMinSurplusDist.TrimRight();
	}
	
	//正在获取临时CustomID信息...
	sTmp.Format(IDS_GetDataTableInfo,GetResStr(IDS_InfoTmpCustomIDForPART));
	frmStatus.m_Label1 = sTmp;
	frmStatus.UpdateStatus(7.0 /9.0,true);
	frmStatus.UpdateData(false);
	frmStatus.UpdateWindow();
	//建立当前标准CustomID表
	frmStatus.m_Label1 = _T("建立当前标准CustomID表");
	frmStatus.UpdateStatus(8.0 /9.0,true);
	frmStatus.UpdateData(false);
	frmStatus.UpdateWindow();
	modPHScal::CreateTmpCustomIDForPART();
	//正在获取当前标准CustomID信息...
	sTmp.Format(IDS_GetDataTableInfo,GetResStr(IDS_InfoCustomID));
	frmStatus.m_Label1 = sTmp;
	frmStatus.UpdateStatus(8.0 /9.0,true);
	frmStatus.UpdateData(false);
	frmStatus.UpdateWindow();
	//在dbprj中建立临时ID-CustomID对照表PictureClipData,
	CreateTmpIDCustomIDTable();
	//在dbprj中建立临时支吊架连接准则表connect,
	//正在获取TmpConnectTable信息...
	sTmp.Format(IDS_GetDataTableInfo,GetResStr(IDS_InfoTmpConnectTable));
	frmStatus.m_Label1 = sTmp;
	frmStatus.UpdateStatus(9.0 /9.0,true);
	frmStatus.UpdateData(false);
	frmStatus.UpdateWindow();
	CreateTmpConnectTable();
	//导入厂家数据库中的CONNECTxx表     LFX 2005.4.19添
	//此操作必须放在 	CreateTmpConnectTable(); 语句之后
	CString strDBTableName;
	CString strPRJTableName;
	CString strSQL;
	while (true)
	{
		strDBTableName=tbnCONNECT;
		if (tbnCONNECT == strStandardConnect)
		{
			//如果当前选用连接件没有自带连接表，则使用系统默认连接表
			//strSQL.Format("DELETE * FROM CONNECT");
			//EDIBgbl::dbPRJ->Execute((_bstr_t)strSQL);
			//strSQL.Format("INSERT INTO CONNECT SELECT * FROM %s IN '%s'",strStandardConnect,db));
			//EDIBgbl::dbPRJ->Execute((_bstr_t)strSQL);
		}
		else
		{
			//当前选用连接件自带了连接表，将系统连接表中与选用连接件自带连接表
			//的 CNTB,CNTE,CNTN 字段相同的记录删除，并将选用连接件自带连连接表
			//中的记录复制到系统连接表中
			strPRJTableName.Format("[connect]");
			if( EDIBgbl::tdfExists(db, strDBTableName) )
			{
				strSQL.Format("DELETE * FROM %s AS A WHERE EXISTS (SELECT * FROM %s AS B IN '%s'\
					where a.cntb = b.cntb and a.cnte = b.cnte and \
					(a.cntn = b.cntn or (a.cntn is null and b.cntn is null)))",
					strPRJTableName,strDBTableName,EDIBgbl::GetDBName(db));
				EDIBgbl::dbPRJ->Execute((_bstr_t)strSQL, NULL, adCmdText);
				
				//导入厂家数据
				strSQL.Format("INSERT INTO %s IN '%s' Select * FROM %s",
					strPRJTableName,EDIBgbl::GetDBName(EDIBgbl::dbPRJ),strDBTableName);
				db->Execute((_bstr_t)strSQL, NULL, adCmdText);
			}
		}
		break;
	}
	//END 导入厂家数据库中的CONNECT表     LFX 2005.4.19添			
	//
	

	//规范改变并且启动了之后才进入下段代码调整相关数据表

	if(bSPECchanged && gbStartedOnceAPHS 
		|| !EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("PhsStructureNAME")) 
		|| !EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("PhsStructureREF"))
		|| modPHScal::bInstalledAndFirstRun )
	{
		//在dbprj中建立与dbsort的表PhsStructureREF和PhsStructureName表相同的拷贝,
		//用户工作在dbPrj,增加模板后要同时修改dbsort和dbPrj，防止多个用户选择不同的规范后发生冲突.
		CreatePhsStructureNameAndREFIndbPRJ();
		//对dbPRJ的表PhsStructureREF表逐条扫描，
		//按新选择的规范和标准进行本地化，修改CustomID与新规范一致。
		Cavphs->CheckAllCustomIDandIDinPhsStructureREF();
		//必须每次进行下面的动作
		//对dbPRJ的表PhsStructureName表逐条扫描，
		//按新选择的规范和标准进行本地化，如修改名称等与新规范相关的值。

		Cavphs->ChangeNameInphsStructureName();

	}
	if( ! gbStartedOnceAPHS ){
		//启动标志，防止检查名称重复两次
		gbStartedOnceAPHS = TRUE;
	}
	
	//保留前次工作规范，防止重新进入时再次计算相同规范，浪费时间。
	gsOldPhsConstantSpringSel = gsPhsConstantSpringSel;
	gsOldPhsSPRINGSel = gsPhsSPRINGSel;
	gsOldPhsPASel = gsPhsPASel;
	gsOldPhsPARTSel = gsPhsPARTSel;
	gsOldPhsSASel = gsPhsSASel;
	gsOldPhsBoltsNutsSel = gsPhsBoltsNutsSel;
	
	ret = TRUE;
   }
   
   catch (_com_error &e)
   {
	   CString strMsg;
	   strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
	   AfxMessageBox(strMsg);
	   ret = FALSE;
	   //Debug.Print SQLx
	   // ShowMessage(e.Description());
   }
errH:
   if(rsX->State == adStateOpen)
	   rsX->Close();
   rsX.Release();

   if(db->State == adStateOpen)
	   db->Close();
	db.Release();
	
   if(db1->State == adStateOpen)
	   db1->Close();
   db1.Release();

   AfxGetApp()->EndWaitCursor();
   frmStatus.ShowWindow(SW_HIDE);
   return ret;

}


void modPHScal::ImportDataFromZdjCrudeXXXX(CString  strFN, bool  bReplacement, bool & mbSPECchanged)
{
	//目的：从厂商提供的产品数据库中导入数据或在产品数据库中检查数据表是否齐全
	AfxGetApp()->BeginWaitCursor();

	CString ConnectionString;
	try
	{
		if(modPHScal::dbZDJcrude->State == adStateOpen)
		{
			modPHScal::dbZDJcrude->Close();
		}
		ConnectionString.Format("Provider=Microsoft.Jet.OLEDB.4.0;Persist Security Info=False;Data Source=%s;Jet OLEDB:Database Password=%s",
			basDirectory::ProjectDBDir+_T("zdjcrude.mdb"), ModEncrypt::gstrDBZdjCrudePassWord);
		modPHScal::dbZDJcrude->Open((_bstr_t)ConnectionString, "", "", adConnectUnspecified);
	}catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
		return;
	}

//	try {
		CString strTbn, strCrudeTbn;
		CMessageBoxEx MsgDlg;
		bool bAllYes=false;
		bool bAllNo=false;
		UINT nDlgID=0;

		long i=0;
		bool bFound=false;
		_ConnectionPtr db;
		db.CreateInstance(__uuidof(Connection));
		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(Recordset));
		_RecordsetPtr rs1;
		rs1.CreateInstance(__uuidof(Recordset));
		_RecordsetPtr rs2;
		rs2.CreateInstance(__uuidof(Recordset));
		strFN.TrimLeft();
		strFN.TrimRight();
		CString strTmp;
		_variant_t vTmp;
		CString SQLx;
		

			
		if(strFN == _T("") )
		{
			throw GetResStr(IDS_NoSelectFileName);
		}
		else
		{
			ConnectionString.Format("Provider=Microsoft.Jet.OLEDB.4.0;Persist Security Info=False;Data Source=%s;Jet OLEDB:Database Password=%s",
				strFN, ModEncrypt::gstrDBZdjCrudePassWord);
		try {
			db->Open((_bstr_t)ConnectionString, "", "", adConnectUnspecified);
		}
		catch (_com_error &e)
		{
			CString strMsg;
			strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
			AfxMessageBox(strMsg);
			return;
		}
			//取出管理表的名称
			CString strSql;
			strSql = _T("SELECT * FROM phsManu ORDER BY seq");
		try {
			rs->Open((_bstr_t)strSql, _variant_t((IDispatch*)EDIBgbl::dbSORT,true), 
				adOpenStatic, adLockOptimistic, adCmdText); 
		}
		catch (_com_error &e)
		{
			CString strMsg;
			strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
			AfxMessageBox(strMsg);
			return;
		}
			if( rs->adoEOF && rs->BOF )
			{
				strTmp.Format(GetResStr(IDS_NullPhsManuTableInSortMdb),EDIBgbl::GetDBName(EDIBgbl::dbSORT));
				throw strTmp;
			}
			while(!rs->adoEOF)
			{
				rs->get_Collect((_variant_t)_T("TableName"),&vTmp);
				strTbn = vtos(vTmp);
				if( EDIBgbl::tdfExists(db, strTbn) )
				{
					//假设全部表都存在
					bFound = true;
					rs->get_Collect((_variant_t)_T("SQL"),&vTmp);
					//下面要判断是否所有的原始数据表都存在，不存在的要排除，不能写到可用列表框，防止用户误点击。
					try {
					rs1->Open((_bstr_t)vTmp, _variant_t((IDispatch*)db,true), 
						adOpenStatic, adLockOptimistic, adCmdText); 
					}
					catch (_com_error &e)
					{
						CString strMsg;
						strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
						AfxMessageBox(strMsg);
						return;
					}
					if( rs1->adoEOF || rs1->BOF ){
						strTmp.Format(GetResStr(IDS_NullPhsManuXtableInProductMdb),strTbn);
						ShowMessage(strTmp);
					}
					else
					{
						while(!rs1->adoEOF)
						{
							rs->get_Collect((_variant_t)_T("CrudeDataTableNum"),&vTmp);
							int C=vtoi(vTmp);
							for( i = 1 ;i<= C;i++)
							{
								rs1->get_Collect((_variant_t)i,&vTmp);
								if( vTmp.vt!=VT_NULL)
								{
									strCrudeTbn = vtos(vTmp);
									if( EDIBgbl::tdfExists(db, strCrudeTbn) )
									{
										//这个表在厂家提供的数据库或原始数据库中存在
										if( EDIBgbl::tdfExists(modPHScal::dbZDJcrude, strCrudeTbn) )
										{
											if( bReplacement )
											{
												strTmp.Format(GetResStr(IDS_ExistsThisCrudeDataTableInZDJcrudeOrproductMdb),EDIBgbl::GetDBName(modPHScal::dbZDJcrude),strCrudeTbn);
												MsgDlg.m_strPrompt=strTmp;
												if(! bAllYes)
												{
													nDlgID=IDALLYES;
													if(nDlgID==IDALLYES)
													{
														AfxGetApp()->BeginWaitCursor();
														bAllYes=true;
													}
													else if(nDlgID == IDALLNO)
													{
														bAllNo=true;
														if(rs1->State == adStateOpen) rs1->Close();
														if(rs2->State == adStateOpen) rs2->Close();
														if(rs->State == adStateOpen) rs->Close();
														if(db->State == adStateOpen) db->Close();
														AfxGetApp()->EndWaitCursor();
														return;
													}
												}
												if(bAllYes || nDlgID== IDYES)
												{
													SQLx = _T("drop table ") + strCrudeTbn;
													try {
													dbZDJcrude->Execute((_bstr_t)SQLx, NULL, adCmdText);
													}
													catch (_com_error &e)
													{
														CString strMsg;
														strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
														AfxMessageBox(strMsg);
														return;
													}

													SQLx = "Select * INTO [" + strCrudeTbn + _T("] IN \'") + EDIBgbl::GetDBName(modPHScal::dbZDJcrude) + _T("\'  FROM ") + strCrudeTbn;
													try {
													db->Execute((_bstr_t)SQLx, NULL, adCmdText);
													mbSPECchanged = true;
													}
													catch (_com_error &e)
													{
														CString strMsg;
														strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
														AfxMessageBox(strMsg);
														return;
													}
												}

											}
										}
										else
										{
											if( bReplacement ){

												//ZDJCrude.mdb中不存在这个表，则从产品库中导入它
												//ADO将检查密码,详见phs.arx
												SQLx = _T("SELECT * INTO [") + strCrudeTbn + _T("] IN \'") + EDIBgbl::GetDBName(dbZDJcrude) + _T("\' FROM ") + strCrudeTbn;
												try {
												db->Execute((_bstr_t) SQLx, NULL, adCmdText);
												}
												catch (_com_error &e)
												{
													// 有时存在此表
// 													CString strMsg;
// 													strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
// 													AfxMessageBox(strMsg);
// 													return;
												}
												mbSPECchanged = true;
											}
										}
									}
									else
									{
										//这个表在厂家提供的数据库中不存在
										if( EDIBgbl::tdfExists(dbZDJcrude, strCrudeTbn) )
										{
											//这个表在原始数据库中存在
										}
										else
										{
											//这个表在原始数据库中不存在
											strTmp.Format(GetResStr(IDS_NoThisCrudeDataTableInProductMdb),EDIBgbl::GetDBName(dbZDJcrude),strCrudeTbn);
											//这种提示令用户烦恼。
											//ShowMessage(strTmp);
										}
									}
								}
							}
							//更新管理表
							rs1->get_Collect((_variant_t)_T("standard"),&vTmp);
							SQLx=_T("SELECT * FROM ") + strTbn + _T(" WHERE (standard)=\'") +vtos(vTmp) + _T("\'");
							try {
							rs2->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbSORT,true), 
								adOpenStatic, adLockOptimistic, adCmdText); 
							//创建一临时表保存将要追加的查询
							if( rs2->adoEOF && rs2->BOF ){
							}else{
								SQLx = _T("DELETE * FROM ") + strTbn + _T(" WHERE (standard)=\'") + vtos(vTmp) + _T("\'");
								EDIBgbl::dbSORT->Execute((_bstr_t) SQLx, NULL, adCmdText);
							}							
							rs2->Close();
							EDIBgbl::UpdateDBTable(db,strTbn,EDIBgbl::dbSORT,strTbn);
							SQLx = _T("INSERT INTO [") + strTbn + _T("] IN \'") + EDIBgbl::GetDBName(EDIBgbl::dbSORT) + _T("\' SELECT * FROM ") + strTbn + _T(" WHERE (standard)=\'") +vtos(vTmp) + _T("\'");
							db->Execute((_bstr_t) SQLx, NULL, adCmdText);
							//保存来源库文件信息
							SQLx = _T("UPDATE [") + strTbn + _T("] SET Folder=\'") + strFN + _T("\' WHERE (standard)=\'") + vtos(vTmp) + _T("\'");
							EDIBgbl::dbSORT->Execute((_bstr_t) SQLx, NULL, adCmdText);							
							rs1->MoveNext();
							}
							catch (_com_error &e)
							{
								CString strMsg;
								strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
								AfxMessageBox(strMsg);
								return;
							}
						}
					}
					rs1->Close();
				}
				rs->MoveNext();
			}
// 		}catch (_com_error &e)
// 		{
// 			CString strMsg;
// 			strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
// 			AfxMessageBox(strMsg);
// 		}
			
			//循环完成
			strTmp.Format(GetResStr(IDS_FinishedImportProductMdb),strFN);			
			ShowMessage(strTmp);
	}
	AfxGetApp()->EndWaitCursor();
}

void modPHScal::ImportDataFromZdjCrude(CString  strFN, bool  bReplacement, bool & mbSPECchanged)
{
	//目的：从厂商提供的产品数据库中导入数据或在产品数据库中检查数据表是否齐全
	AfxGetApp()->BeginWaitCursor();
	EXECUTE_TIME_BEGIN
	try
	{
		CString strTbn, strCrudeTbn;
		CMessageBoxEx MsgDlg;
		bool bAllYes=false;
		bool bAllNo=false;
		UINT nDlgID=0;

		long i=0;
		bool bFound=false;
		_ConnectionPtr db;
		db.CreateInstance(__uuidof(Connection));
		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(Recordset));
		_RecordsetPtr rs1;
		rs1.CreateInstance(__uuidof(Recordset));
		_RecordsetPtr rs2;
		rs2.CreateInstance(__uuidof(Recordset));
		strFN.TrimLeft();
		strFN.TrimRight();
		CString strTmp;
		_variant_t vTmp;
		CString SQLx;
		if(strFN == _T("") )
		{
			//MsgBox ResolveResString(iUE_NoSelectFileName)
			//Exit Sub
			throw GetResStr(IDS_NoSelectFileName);
		}
		else
		{
			CString ConnectionString;
			ConnectionString.Format("Provider=Microsoft.Jet.OLEDB.4.0;Persist Security Info=False;Data Source=%s;Jet OLEDB:Database Password=%s",
				strFN, ModEncrypt::gstrDBProductPassWord);
			db->Open((_bstr_t)ConnectionString, "", "", adConnectUnspecified);
			//取出管理表的名称
			SQLx = _T("SELECT * FROM phsManu ORDER BY seq");
			rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbSORT,true), 
				adOpenStatic, adLockOptimistic, adCmdText); 
			if( rs->adoEOF || rs->BOF )
			{
				//MsgBox ResolveResString(iUE_NullPhsManuTableInSortMdb)
				//Exit Sub
				strTmp.Format(GetResStr(IDS_NullPhsManuTableInSortMdb),EDIBgbl::GetDBName(EDIBgbl::dbSORT));
				throw strTmp;
			}
			while(!rs->adoEOF)
			{
				rs->get_Collect((_variant_t)_T("TableName"),&vTmp);
				strTbn = vtos(vTmp);
				if( EDIBgbl::tdfExists(db, strTbn) )
				{
					//假设全部表都存在
					bFound = true;
					rs->get_Collect((_variant_t)_T("SQL"),&vTmp);
					//下面要判断是否所有的原始数据表都存在，不存在的要排除，不能写到可用列表框，防止用户误点击。
					rs1->Open((_bstr_t)vTmp, _variant_t((IDispatch*)db,true), 
						adOpenStatic, adLockOptimistic, adCmdText); 
					if( rs1->adoEOF || rs1->BOF ){
						strTmp.Format(GetResStr(IDS_NullPhsManuXtableInProductMdb),strTbn);
						ShowMessage(strTmp);
					}
					else
					{
						while(!rs1->adoEOF)
						{
							rs->get_Collect((_variant_t)_T("CrudeDataTableNum"),&vTmp);
							int C=vtoi(vTmp);
							for( i = 1 ;i<= C;i++)
							{
								rs1->get_Collect((_variant_t)i,&vTmp);
								if( vTmp.vt!=VT_NULL)
								{
									strCrudeTbn = vtos(vTmp);
									//if( strCrudeTbn == _T("BoltsSurplusLength") ){
									//	Debug.Print strCrudeTbn
									//}
									if( EDIBgbl::tdfExists(db, strCrudeTbn) )
									{
										//这个表在厂家提供的数据库或原始数据库中存在
										if( EDIBgbl::tdfExists(modPHScal::dbZDJcrude, strCrudeTbn) )
										{
											if( bReplacement )
											{
												strTmp.Format(GetResStr(IDS_ExistsThisCrudeDataTableInZDJcrudeOrproductMdb),EDIBgbl::GetDBName(modPHScal::dbZDJcrude),strCrudeTbn);
												MsgDlg.m_strPrompt=strTmp;
												//if(IDYES == ShowMessage(strTmp, MB_DEFBUTTON1|MB_ICONQUESTION|MB_SYSTEMMODAL|MB_YESNO) )
												if(! bAllYes)
												{
													nDlgID=MsgDlg.DoModal();
													if(nDlgID==IDALLYES)
													{
														AfxGetApp()->BeginWaitCursor();
														bAllYes=true;
													}
													else if(nDlgID == IDALLNO)
													{
														bAllNo=true;
														if(rs1->State == adStateOpen) rs1->Close();
														if(rs2->State == adStateOpen) rs2->Close();
														if(rs->State == adStateOpen) rs->Close();
														if(db->State == adStateOpen) db->Close();
														AfxGetApp()->EndWaitCursor();
														return;
													}
												}
												if(bAllYes || nDlgID== IDYES)
												{
													SQLx = _T("drop table ") + strCrudeTbn;
													dbZDJcrude->Execute((_bstr_t)SQLx, NULL, adCmdText);

													SQLx = "Select * INTO [" + strCrudeTbn + _T("] IN \'") + EDIBgbl::GetDBName(modPHScal::dbZDJcrude) + _T("\'  FROM ") + strCrudeTbn;
													db->Execute((_bstr_t)SQLx, NULL, adCmdText);
													mbSPECchanged = true;
												}

											}
										}
										else
										{
											if( bReplacement ){
												//ZDJCrude.mdb中不存在这个表，则从产品库中导入它
												//ADO将检查密码,详见phs.arx
												SQLx = _T("SELECT * INTO [") + strCrudeTbn + _T("] IN \'") + EDIBgbl::GetDBName(dbZDJcrude) + _T("\' FROM ") + strCrudeTbn;
												db->Execute((_bstr_t) SQLx, NULL, adCmdText);
												mbSPECchanged = true;
											}
										}
									}
									else
									{
										//这个表在厂家提供的数据库中不存在
										if( EDIBgbl::tdfExists(dbZDJcrude, strCrudeTbn) )
										{
											//这个表在原始数据库中存在
										}
										else
										{
											//这个表在原始数据库中不存在
											strTmp.Format(GetResStr(IDS_NoThisCrudeDataTableInProductMdb),EDIBgbl::GetDBName(dbZDJcrude),strCrudeTbn);
											//这种提示令用户烦恼。
											//ShowMessage(strTmp);
										}
									}
								}
							}
							//更新管理表
							rs1->get_Collect((_variant_t)_T("standard"),&vTmp);
							SQLx=_T("SELECT * FROM ") + strTbn + _T(" WHERE (standard)=\'") +vtos(vTmp) + _T("\'");
							rs2->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbSORT,true), 
								adOpenStatic, adLockOptimistic, adCmdText); 
							if( rs2->adoEOF || rs2->BOF ){
							}else{
								SQLx = _T("DELETE * FROM ") + strTbn + _T(" WHERE (standard)=\'") + vtos(vTmp) + _T("\'");
								EDIBgbl::dbSORT->Execute((_bstr_t) SQLx, NULL, adCmdText);
							}							
							//alter strTbn table structure before inserting records
							//at first close rs2,otherwise to produce a error,because strTbn is still opened.
							rs2->Close();
							EDIBgbl::UpdateDBTable(db,strTbn,EDIBgbl::dbSORT,strTbn);
							SQLx = _T("INSERT INTO [") + strTbn + _T("] IN \'") + EDIBgbl::GetDBName(EDIBgbl::dbSORT) + _T("\' SELECT * FROM ") + strTbn + _T(" WHERE (standard)=\'") +vtos(vTmp) + _T("\'");
							db->Execute((_bstr_t) SQLx, NULL, adCmdText);
							//保存来源库文件信息
							SQLx = _T("UPDATE [") + strTbn + _T("] SET Folder=\'") + strFN + _T("\' WHERE (standard)=\'") + vtos(vTmp) + _T("\'");
							EDIBgbl::dbSORT->Execute((_bstr_t) SQLx, NULL, adCmdText);							
							rs1->MoveNext();
						}
					}
					rs1->Close();
				}
				rs->MoveNext();
			}
			//循环完成
			strTmp.Format(GetResStr(IDS_FinishedImportProductMdb),strFN);			
			ShowMessage(strTmp);
		}
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}
	EXECUTE_TIME_END
	AfxGetApp()->EndWaitCursor();
}

void modPHScal::SetSPRPosValue()
{
}

long modPHScal::CSLength(CString /*ByVal*/ sBHFormat, CString /*ByVal*/ LengthSQL, CString & sBH)
{
	//目的：计算根部槽钢梁或拉杆(ClassID)长度.
	//输入:sBHformat--编号格式，LengthSQL--长度计算表达式，sBH--获得的规格结果。
	long ret=0;
	try
	{
		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(Recordset));
		//CSLength = 0;
		//获得型钢长度(由字段sngLen确定)
		CString SQL1;
		CString sTmp;
		_variant_t vTmp;
		SQL1 = _T("SELECT ") + LengthSQL + _T(" AS sngLen FROM tmpCSLen");
		rs->Open((_bstr_t)SQL1, _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
			adOpenStatic, adLockOptimistic, adCmdText); 
		
		if(rs->BOF && rs->adoEOF)
		{
			sTmp.Format(GetResStr(IDS_NoRecordInXtableInXmdb),EDIBgbl::GetDBName(EDIBgbl::dbPRJ),_T("tmpCSLen"),SQL1);
			throw sTmp;
		}
		else
		{
			rs->MoveFirst();
			rs->get_Collect((_variant_t)_T("sngLen"),&vTmp);
			ret=vtoi(vTmp);
			CString sTmp1;
			sTmp1.Format(_T("%g"),vtof(vTmp));

			SQL1 = _T("UPDATE tmpCSLen SET L1=") + sTmp1;
			EDIBgbl::dbPRJ->Execute((_bstr_t)SQL1, NULL, adCmdText );
			//获得CLgg或BH
			SQL1 = _T("SELECT (") + sBHFormat + _T(") AS sBH FROM tmpCSLen");
			rs->Close();
			rs->Open((_bstr_t)SQL1, _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
				adOpenStatic, adLockOptimistic, adCmdText); 
			
			if(rs->BOF && rs->adoEOF)
			{
				sTmp.Format(GetResStr(IDS_NoRecordAtTmpCSLenTableInWorkprjMdb),EDIBgbl::GetDBName(EDIBgbl::dbPRJ),SQL1);
				ShowMessage(sTmp);
			}
			else
			{
				rs->MoveFirst();
				//规格结果表达式返回给sBH变量
				rs->get_Collect((_variant_t)_T("sBH"),&vTmp);
				sBH =vtos(vTmp);
			}
		}
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}
	return ret;
}

	bool modPHScal::CheckFieldIfNull(_RecordsetPtr rs)
{
	return true;
}

/*Object*/void modPHScal::GetZAdataRs(CWnd* /*ByVal*/ frm)
{
}

long modPHScal::lngPreCal()
{
	try{
   //PreCalCrudeData(FrmTxsr.m_pViewTxsr->m_ActiveRs);
   
   SetBasePoint();
   if( IDdn != _T(""))
      Cavphs->PA = IDdn;
   else
      throw GetResStr(IDS_NullPA);
   
   if( IDgn != _T(""))
      Cavphs->SA = IDgn;
   else
      throw GetResStr(IDS_NullSA);
   
   
   Cavphs->iLIST = mnuiPASALimited;
   if( iSelSampleID <= 0 )
      iSelSampleID = Cavphs->GetPhsStructSampleID();
   else
      //iSelSampleID = Cavphs->GetPhsStructSampleID()
   ;
   CString sTmp1,sTmp2;
   if( iSelSampleID == -1)
   {
	   sTmp1.Format(_T("%d"),iSelSampleID);
	   sTmp2.Format(GetResStr(IDS_SelSampleIDisIllegal),sTmp1);
      throw sTmp2;
	}
   else
   
      return Cavphs->SavephsStructTorsTmpREF();
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	return 0;
}
}

CString modPHScal::GetSaveAsDwgFileName(long zdjh,_RecordsetPtr rs)
{
	//目的：获得存盘文件名
	CString sTmp;


	CString sDwgFileName;
	if( EDIBAcad::g_bCustomPicNo )
	{		
		CString sSQL;
		sSQL.Format("SELECT CustomDwgName FROM ZA WHERE [VolumeID]=%d AND [ZDJH]=%d",EDIBgbl::SelVlmID,zdjh);
		if( rs != NULL )
		{
			if( rs->adoEOF && rs->BOF ) 
			{
			}
			else
			{
				if( vtos(rs->GetCollect("CustomDwgName")).IsEmpty() )
				{
					rs->PutCollect("CustomDwgName",(long)modPHScal::zdjh);
					EDIBAcad::g_strCustomPicNoFileName = ltos(modPHScal::zdjh);
				}
				else
				{
					EDIBAcad::g_strCustomPicNoFileName = vtos(rs->GetCollect("CustomDwgName"));
				}

				sDwgFileName = EDIBAcad::g_strCustomPicNoFileName;
			}
		}

	}
	else
	{
		if( glDWGPreMethod == 0)
			sDwgFileName = EDIBAcad::GetDrawIDAndName(zdjh + glDrawNoStartNumber - 1, sTmp);
		else
			sDwgFileName.Format(_T("%s%d"),gsDWGPre,zdjh + glDrawNoStartNumber - 1);

		EDIBAcad::g_strCustomPicNoFileName=sDwgFileName;
	}


	if( strstr(sDwgFileName,".dwg") != NULL )
	{
		gsDwgFN = basDirectory::ProjectDWGDir + sDwgFileName;
	}
	else
	{
		gsDwgFN = basDirectory::ProjectDWGDir + sDwgFileName + _T(".dwg");
	}
	return gsDwgFN;
}

void modPHScal::DrawPhs(_RecordsetPtr rsza)
{
	try
	{
		static long iDwg=0;
		//long i;
		//dim rsza As Recordset
		//Set rsza = Crs.rsDS  //GetZAdataRs(frm)
		//预备计算
		AfxGetApp()->BeginWaitCursor();
		PreCalCrudeData(rsza);
		::SetRegValue(_T("Settings"),_T("PhsCurSANo"),modPHScal::giSAG100No);
		::SetRegValue(_T("Settings"),_T("PhsCurSATbn"),modPHScal::tbnSA);

		//SetWindowNoTop PhsData.hwnd
     
		if(!gbNotPrompt_ACAD_SHIFT)
		{
		  if(MessageBox(NULL,GetResStr(IDS_atFirstGoBackACADsaveDwgThenDraw), GetResStr(IDS_SaveDwg) ,MB_YESNO|MB_DEFBUTTON2|MB_SYSTEMMODAL) ==IDYES)
			  EDIBAcad::SetAcadTop();
		}

		SetPhsTK3();         //生成3号图框尺寸及图名等信息

		//支架自动取消操作系统图标:画框架、制图表、设计表
		EDIBAcad::DrawTKphs((Ax == _T("A3")? 3 : 4), 0, 0, modPHScal::gbTBhq, (Ax == _T("A3") ? EDIBgbl::gbTbOS : 0), (long)gbNoFenQu, modPHScal::gbNoWaiKuang,1);


		Cavphs->GetMaterial();
		Cavphs->GetphsSEQ(rsza);   //计算编号
		//以下语句均不成功!
		//SQLx = _T("SELECT * FROM [") & TBNSelPrjspec & Btype(TZB) & _T("] WHERE zdjh=") + ltos(zdjh) + _T(" AND (VolumeID)=\'") + EDIBgbl::SelVlmID + _T("\'")
		//SQL1 = _T("SELECT * FROM [") & TBNSelPrjspec & Btype(TZF) & _T("] WHERE zdjh=") + ltos(zdjh) + _T(" AND (VolumeID)=\'") + EDIBgbl::SelVlmID + _T("\'")  & _T(" ORDER BY recno")
		//SQLx = SQLx & _T(" UNION ALL ") & SQL1

		Cavphs->GetPhsSAELandPAdxdydz();  //计算根部标高和管部定位尺寸
		Cavphs->GetPhsBlkIDandCrd(rsza);
		_RecordsetPtr rsTmpZB;
		rsTmpZB.CreateInstance(__uuidof(Recordset));
		CString SQLx = CString(_T("SELECT * FROM [")) + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] WHERE zdjh=") + ltos(modPHScal::zdjh) + _T(" AND VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" AND IsSAPart<>-1 order by recno");
		rsTmpZB->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
			adOpenDynamic, adLockOptimistic, adCmdText); 

		EDIBAcad::DrawPhsAssemble(rsTmpZB, iViewXZ) ; //绘制支吊架组装图


		gsDwgFN = GetSaveAsDwgFileName(modPHScal::zdjh,rsza);

		rsza->PutCollect(_T("DwgName"),_variant_t(gsDwgFN));
		CString sTmp;
		sTmp.Format(_T("Drawing%d"),(iDwg % 4) + 1);
		if( FileExists(gsDwgFN))
		{
		  if( gbDrawOVERWRITE)
		  {
			  SetRegValue("AutoCAD General", sTmp, gsDwgFN);
			 iDwg++;
		  }
		}
		else
		{
			SetRegValue("AutoCAD General", sTmp, gsDwgFN);
			iDwg++;
		}
  
		//绘制零件明细表
		if( gbDraw_BOM )
		{
			EDIBDB::MakeTmp2ToBom();
			_RecordsetPtr tmprs;
			tmprs.CreateInstance(__uuidof(Recordset));
			SQLx = _T("SELECT * FROM TMP2");
			tmprs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
				adOpenStatic, adLockOptimistic, adCmdText); 
			CCadPoint p0(pt2x,pt2y,0);
			if( EDIBAcad::g_bLJMX_BJBW )
			{
				EDIBAcad::DrawTableACAD(p0, EDIBgbl::TLJ_BJBW, atan((double)1.0) * 0, tmprs, 100, 1.0f,_T("bom"),_T("bom"),_T("%g"),modPHScal::iAlignLjmx);
			}
			else
			{
				EDIBAcad::DrawTableACAD(p0, EDIBgbl::TLJ, atan((double)1.0) * 0, tmprs, 100, 1.0f,_T("bom"),_T("bom"),_T("%g"),modPHScal::iAlignLjmx);
			}
		}

		//绘制荷载位移表(字体用零件明细表字体bom)
		if(EDIBAcad::g_bDLOADBJBW)
		{
			PhsDisplacementLoadINFOMake((_T("Load")),modPHScal::iAlignLoad,EDIBgbl::TzDLOADNEW);
		}
		else
		{
			PhsDisplacementLoadINFOMake((_T("Load")),modPHScal::iAlignLoad,EDIBgbl::TzDLOAD);
		}

	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}
	AfxGetApp()->EndWaitCursor();
}

void modPHScal::MakeTmpCSLen()
{
//调用GetphsBHandSizes方法计算一个支吊架之前，必须在dbPRJ中建立表tmpCSLen
	CString strExecute;
   if(EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("tmpCSLen")))
   {
      //这样比较方便，而且版本升级时兼容性好。
	   strExecute = _T("DROP TABLE tmpCSLen");
		EDIBgbl::dbPRJ->Execute((_bstr_t)strExecute, NULL, adCmdText);
   }
   strExecute = _T("CREATE TABLE tmpCSLen (ID char(20),CustomID char(50),SprNo SHORT,SerialNum SHORT,P2 char(20),P3 char(20),P4 char(20),P5 char(20),P6 char(20),P7 char(20),P8 char(20),P9 char(20),t0 REAL,BH CHAR (80),SelOpLoad REAL,OpLoad REAL,InstallLoad REAL,SumDist long,SumDistID short,HeatDist long,DistFX CHAR (50),LugDiameter long,sizeH REAL,sizeC REAL,Length long, GDW1 REAL,sumL REAL,L1 REAL,A REAL,Lspan REAL, H REAL,B REAL,B1 REAL,A1 REAL,B0 REAL,M REAL,C REAL, CHDist REAL, CHheight REAL,CHLegheight REAL,BWIDTH  REAL,size2 REAL,PL1 REAL,SpecialDesign char(50),UnitForce char(50),Dw REAL,P1 char(50),DW1 REAL,S REAL,K REAL,K2 REAL,temperature REAL)");
   EDIBgbl::dbPRJ->Execute((_bstr_t)strExecute, NULL, adCmdText);

   strExecute = _T("INSERT INTO tmpCSLen (sizeH) VALUES (0)");
   EDIBgbl::dbPRJ->Execute((_bstr_t)strExecute, NULL, adCmdText );
}



void modPHScal::MakeZDJ_ZD(long zdjh)
{
//目的:生成支吊架一览表结构。
//输入:支吊架号。如果缺省支吊架号，则清除当前卷册所有一览表项目。否则，只清除当前支吊架一览表内容。
   try
	{
//	bool Found ;
      //删除以前计算出的支吊架号为zdjh的一览表内容
		CString strExecute;
		strExecute = _T("DELETE * FROM [") + EDIBgbl::Btype[EDIBgbl::TZD] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + 
			(zdjh==0 ? _T("") : CString(_T(" AND zdjh=")) + ltos(zdjh));
     EDIBgbl::dbPRJDB->Execute((_bstr_t)strExecute, NULL, adCmdText);
	}
   catch (_com_error &e)
   {
	   CString strMsg;
	   strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
	   AfxMessageBox(strMsg);
	}
}

void modPHScal::PhsMLmake(_variant_t ZDJNo)
{
   //目的：添加或修改zdjh=zdjno支吊架组装图图纸的目录信息。
	try
	{
   
	   CString tbn;
	   int i ;
	   _RecordsetPtr rsX;
	   rsX.CreateInstance(__uuidof(Recordset));
	   tbn = EDIBgbl::Btype[EDIBgbl::TML];
	   CString SQL1x=_T("");
	   if(!EDIBgbl::tdfExists(EDIBgbl::dbPRJDB, tbn))
	   {
		   //copy ml table structure from f4511s.mdb in templatedir into ml of allprjdb.mdb in projectdbdir
		   SQL1x = _T("SELECT * INTO [") + tbn + _T("] FROM [") + tbn + _T("] IN \'") + basDirectory::TemplateDir + _T("F4511s.mdb\' WHERE DrawNo=\'\'"); 
		   EDIBgbl::dbPRJDB->Execute((_bstr_t) SQL1x, NULL, adCmdText);
	   }
   
	   CString sTmpDrawNo, sTmpDrawNa,sTmp;
	   sTmpDrawNo = EDIBAcad::GetDrawIDAndName((zdjh + glDrawNoStartNumber - 1), sTmpDrawNa);
		CString sZdjh;
	   sZdjh.Format(_T("%d"),zdjh);
	   if (sTmpDrawNo != _T("") && sTmpDrawNa != _T(""))
	   {
		  //开始更新目录信息
		   SQL1x = _T("SELECT * FROM [") + tbn + _T("] where VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" AND NOT IsNull(DrawNo) AND NOT IsNull(DrawNa) AND NOT (DrawNo)=\'\' AND NOT (DrawNa)=\'\' AND zdjh=") + sZdjh + _T(" ORDER BY SEQ");
		   rsX->Open((_bstr_t)SQL1x, _variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
			   adOpenDynamic, adLockOptimistic, adCmdText); 
		  if( rsX->BOF&& rsX->adoEOF)// Then
		  {
			 //MsgBox ResolveResString(iUE_NoRecordInTMLtableInAllPrjDbMdb, _T("|1"), dbPRJDB.Name, _T("|2"), tbn, _T("|3"), SQL1x), vbMsgBoxSetForeground + vbSystemModal
			 //开始添加目录信息
			 rsX->AddNew();
			 rsX->put_Collect((_variant_t)_T("SEQ"),_variant_t((long)(zdjh + glDrawNoStartNumber - 1)));
			 rsX->put_Collect((_variant_t)_T("Pages"),_variant_t((long)1));
			 rsX->put_Collect((_variant_t)_T("VolumeID"),_variant_t(EDIBgbl::SelVlmID));
			 rsX->put_Collect((_variant_t)_T("DrawNo"),STR_VAR(sTmpDrawNo));
			 rsX->put_Collect((_variant_t)_T("DrawNa") ,STR_VAR( sTmpDrawNa));
			 rsX->put_Collect((_variant_t)_T("zdjh"),_variant_t((long)zdjh));
				
			 rsX->Update();
		  }
		  else
		  {
			 //目录信息已经存在
			 if( gbNotPrompt_Catlog_OVERWRITE )
				i = IDYES;
			 else
			 {
				//必须防止AutoCAD遮住提示窗口
				EDIBAcad::SetAcadNoTop();
				sTmp.Format(GetResStr(IDS_ExistsDrawNameAndDrawID),sTmpDrawNa,sTmpDrawNo);
				ShowMessage(sTmp,MB_YESNO|MB_DEFBUTTON2|MB_SYSTEMMODAL|MB_TOPMOST);
				EDIBAcad::SetAcadTop();
			 }
			 if (i == IDYES)
			 {
				rsX->put_Collect((_variant_t)_T("SEQ"),_variant_t((long)(zdjh + glDrawNoStartNumber - 1)));
				rsX->put_Collect((_variant_t)_T("Pages"),_variant_t((long)1));
				rsX->put_Collect((_variant_t)_T("VolumeID"),_variant_t(EDIBgbl::SelVlmID));
				rsX->put_Collect((_variant_t)_T("DrawNo"),STR_VAR( sTmpDrawNo));
				rsX->put_Collect((_variant_t)_T("DrawNa"),STR_VAR(sTmpDrawNa));

				rsX->Update();
			 }
		  }
		  rsX->Close();
	   }
	   }
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}
}

void modPHScal::PhsDisplacementLoadINFOMake(LPCTSTR lpszTextStyle,int iAlign,int iZDLOADtype)
{
	//功能:生成管道位移表、支吊架荷载表(华东院格式）或位移荷载合并表(西北院格式)tmpDisplacementLoad
	try
	{
		_Recordset* rsza=FrmTxsr.m_pViewTxsr->m_ActiveRs;
		EDIBAcad::DeleteAllEntitiesInLayers(1,_T("DLOAD"));
		CCadPoint p0;
		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(Recordset));
		_RecordsetPtr rs1;
		rs1.CreateInstance(__uuidof(Recordset));
		_RecordsetPtr rsText;
		rsText.CreateInstance(__uuidof(Recordset));
		CString tmpStr;
		CString SQLx;
		CMObject objEnt;
		//力单位制换算系数
		float UnitF=(Unit_Force == _T("kgf") ? EDIBgbl::kgf2N : 1);

		EDIBgbl::g_bOnlyUserDesc  = GetRegKey(_T("Settings"),_T("OnlyUserDesc"),FALSE);
		EDIBgbl::g_bOnlySysDesc	  = GetRegKey(_T("Settings"),_T("OnlySysDesc"),TRUE);
		EDIBgbl::g_bCustomDesc	  = GetRegKey(_T("Settings"),_T("CustomDesc"),FALSE);
		
		//如果选择了“自定义注释”则应该读出其自定义设置		
		EDIBgbl::g_bPhsManual=  GetRegKey(_T("Settings"),_T("Custom_PhsManual"), EDIBgbl::g_bPhsManual);
		EDIBgbl::g_bPhsPicture= GetRegKey(_T("Settings"),_T("Custom_PhsPicture"),EDIBgbl::g_bPhsPicture);
		EDIBgbl::g_bPhsSetNum=  GetRegKey(_T("Settings"),_T("Custom_PhsSetNum"), EDIBgbl::g_bPhsSetNum);
		EDIBgbl::g_bUserDesc=   GetRegKey(_T("Settings"),_T("Custom_UserDesc"),  EDIBgbl::g_bUserDesc);	

		CString strExecute;
		if(giPhsOutINFOTableType == iDisplacementLoadSep)
		{
			//华东院格式
			//管道位移表tmpPipeDisplacement与荷载表分开
			if(EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("tmpPipeDisplacement")))
				EDIBgbl::dbPRJ->Execute((_bstr_t)_T("DROP TABLE tmpPipeDisplacement"), NULL, adCmdText);
			EDIBgbl::dbPRJ->Execute((_bstr_t) _T("CREATE TABLE tmpPipeDisplacement (duty char(10),design REAL,cool REAL,heat REAL,Tj char(20))"), NULL, adCmdText);

			strExecute = _T("INSERT INTO tmpPipeDisplacement (duty,design,cool,heat,Tj) VALUES (\'x\',0,") +vtos(rsza->GetCollect(_T("xl1"))) + _T(",") + vtos(rsza->GetCollect(_T("xr1"))) + _T(",\'") + ftos(t0) + _T("\')");
			EDIBgbl::dbPRJ->Execute((_bstr_t)strExecute, NULL, adCmdText);

			strExecute = _T("INSERT INTO tmpPipeDisplacement (duty,design,cool,heat) VALUES (\'y\',0,") + vtos(rsza->GetCollect(_T("yl1"))) + _T(",") + vtos(rsza->GetCollect(_T("yr1"))) + _T(")");
			EDIBgbl::dbPRJ->Execute((_bstr_t)strExecute, NULL, adCmdText );

			strExecute = _T("INSERT INTO tmpPipeDisplacement (duty,design,cool,heat) VALUES (\'z\',0,") + vtos(rsza->GetCollect(_T("zl1")) )+ _T(",") + vtos(rsza->GetCollect(_T("zr1"))) + _T(")");
			EDIBgbl::dbPRJ->Execute((_bstr_t)strExecute, NULL, adCmdText );
			//支吊架荷载表tmpPhsLoad
			if(EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("tmpPhsLoad")))
				EDIBgbl::dbPRJ->Execute((_bstr_t)_T("DROP TABLE tmpPhsLoad"), NULL, adCmdText);
			
			EDIBgbl::dbPRJ->Execute((_bstr_t)_T("CREATE TABLE tmpPhsLoad (duty char(10),design REAL,cool REAL,heat REAL,HydraulicPressure REAL,SafetyValveAction REAL,Tj char(20))"), NULL, adCmdText);

			strExecute = _T("INSERT INTO tmpPhsLoad (duty,design,cool,heat,Tj) VALUES (\'Px\',0,");
			strExecute += (rsza->GetCollect(_T("pxl1")).vt==VT_NULL ? _T("0") : ftos(fabs(vtof(rsza->GetCollect(_T("pxl1"))))));
			strExecute += _T(","); 
			strExecute += (rsza->GetCollect(_T("pxr1")).vt==VT_NULL ? _T("0") : ftos(fabs(vtof(rsza->GetCollect(_T("pxr1")))))); 
			strExecute += _T(",\'"); 
			strExecute += ftos(t0); 
			strExecute += _T("\')");
			EDIBgbl::dbPRJ->Execute((_bstr_t)strExecute, NULL, adCmdText);
			//pfg20050422原代码
			/*	EDIBgbl::dbPRJ->Execute((_bstr_t) CString(_T("INSERT INTO tmpPhsLoad (duty,design,cool,heat) VALUES (\'Py\',")) + 
				(rsza->GetCollect(_T("pjg1")).vt==VT_NULL ? CString(_T("0")) : ftos(fabs(vtof(rsza->GetCollect(_T("pjg1"))) * UnitF))) 
				+ CString(_T(","))
				+ (rsza->GetCollect(_T("paz1")).vt==VT_NULL? CString(_T("0")) : ftos(fabs(vtof(rsza->GetCollect(_T("paz1"))) * UnitF))) 
				+ CString(_T(",")) 
				+ (rsza->GetCollect(_T("pgz1")).vt==VT_NULL ? _T("0") : ftos(vtof(rsza->GetCollect(_T("pgz1"))) * UnitF)) + CString(_T(")")));*/
			//pfg20050422原代码

			//pfg20050422
			strExecute =  CString(_T("INSERT INTO tmpPhsLoad (duty,design,cool,heat) VALUES (\'Py\',"));
			strExecute += (rsza->GetCollect(_T("pjg1")).vt==VT_NULL ? _T("0") : ftos((vtof(rsza->GetCollect(_T("pjg1"))) * UnitF)));
			strExecute += _T(","); 
			strExecute += (rsza->GetCollect(_T("paz1")).vt==VT_NULL ? _T("0") : ftos((vtof(rsza->GetCollect(_T("paz1"))) * UnitF))); 
			strExecute += _T(",\'"); 
			strExecute += (rsza->GetCollect(_T("pgz1")).vt==VT_NULL ? _T("0") : ftos((vtof(rsza->GetCollect(_T("pgz1"))) * UnitF))); 
			strExecute += _T(",\'"); 
			strExecute += ftos(t0); 
			strExecute += _T("\')");
			EDIBgbl::dbPRJ->Execute((_bstr_t)strExecute, NULL, adCmdText);
			//pfg20050422

			strExecute = _T("INSERT INTO tmpPhsLoad (duty,design,cool,heat) VALUES (\'Pz\',0,");
			strExecute += (rsza->GetCollect(_T("pzl1")).vt==VT_NULL ? _T("0") : ftos(fabs(vtof(rsza->GetCollect(_T("pzl1"))) * UnitF)));
			strExecute += _T(","); 
			strExecute += (rsza->GetCollect(_T("pzr1")).vt==VT_NULL ? _T("0") : ftos(fabs(vtof(rsza->GetCollect(_T("pzr1"))) * UnitF))); 
			strExecute += _T(")");
			EDIBgbl::dbPRJ->Execute((_bstr_t)strExecute, NULL, adCmdText );

			strExecute = _T("INSERT INTO tmpPhsLoad (duty,design,cool,heat) VALUES (\'Mx\',0,");
			strExecute += (rsza->GetCollect(_T("mxl1")).vt==VT_NULL ? _T("0") : ftos(fabs(vtof(rsza->GetCollect(_T("mxl1"))))));
			strExecute += _T(","); 
			strExecute += (rsza->GetCollect(_T("mxr1")).vt==VT_NULL ? _T("0") : ftos(fabs(vtof(rsza->GetCollect(_T("mxr1")))))); 
			strExecute += _T(")");
			EDIBgbl::dbPRJ->Execute((_bstr_t)strExecute, NULL, adCmdText );

			strExecute = _T("INSERT INTO tmpPhsLoad (duty,design,cool,heat) VALUES (\'My\',0,");
			strExecute += (rsza->GetCollect(_T("myl1")).vt==VT_NULL ? _T("0") : ftos(fabs(vtof(rsza->GetCollect(_T("myl1"))))));
			strExecute += _T(","); 
			strExecute += (rsza->GetCollect(_T("myr1")).vt==VT_NULL ? _T("0") : ftos(fabs(vtof(rsza->GetCollect(_T("myr1")))))); 
			strExecute += _T(")");
			EDIBgbl::dbPRJ->Execute((_bstr_t)strExecute, NULL, adCmdText );

			strExecute = _T("INSERT INTO tmpPhsLoad (duty,design,cool,heat) VALUES (\'Mz\',0,");
			strExecute += (rsza->GetCollect(_T("mzl1")).vt==VT_NULL ? _T("0") : ftos(fabs(vtof(rsza->GetCollect(_T("mzl1"))))));
			strExecute += _T(","); 
			strExecute += (rsza->GetCollect(_T("mzr1")).vt==VT_NULL ? _T("0") : ftos(fabs(vtof(rsza->GetCollect(_T("mzr1")))))); 
			strExecute += _T(")");
			EDIBgbl::dbPRJ->Execute((_bstr_t)strExecute, NULL, adCmdText );
			
			//开始绘制表格
			p0.SetPoint(pt3x, pt3y);
			rsText->Open((_bstr_t)_T("SELECT * FROM tmpPipeDisplacement"), _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
				adOpenStatic, adLockOptimistic, adCmdText); 
			EDIBAcad::DrawTableACAD(p0, EDIBgbl::TzDIST, atan((double)1.0) * 0, rsText, 100, 1.0f , (_T("DLOAD")),lpszTextStyle,(_T("%.f")),iAlign);
			rsText->Close();


			//计算支吊架荷载表的基点
			SQLx = _T("SELECT * FROM tableINFO WHERE ID=") + ltos(EDIBgbl::TzDIST);
			rs1->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbTable,true), 
				adOpenStatic, adLockOptimistic, adCmdText); 
			CString tmpStr;
			if(rs1->BOF && rs1->adoEOF)
			{
				tmpStr.Format(GetResStr(IDS_NoRecordInXtableInXmdb),EDIBgbl::GetDBName(EDIBgbl::dbSORT),_T("tableINFO"),SQLx);
				throw tmpStr;
			}
			rs1->MoveFirst();
			p0.SetX(pt3x);


			//往上移7mm，以尽量避免与Bom碰撞,不可改变全局变量pt3y的值
			//pt3y = pt3y + 7
			_variant_t vTmp;
			rs1->get_Collect((_variant_t)_T("CADh0"),&vTmp);
			if(vTmp.vt==VT_NULL)
				p0.SetY(pt3y - 17);
			else
				p0.SetY(pt3y - fabs(vtof(vTmp)));
			rs1->get_Collect((_variant_t)_T("CADRowHeight"),&vTmp);
			if(vTmp.vt==VT_NULL)
				p0.SetY(p0[1] - 7 * 3);
			else
				p0.SetY(p0[1] - vtof(vTmp) * 3);
			p0.SetY(p0[1] - 14 + 7);
			p0.SetZ(0.0);
			rsText->Open((_bstr_t)_T("SELECT * FROM tmpPhsLoad"), _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
				adOpenStatic, adLockOptimistic, adCmdText); 
			EDIBAcad::DrawTableACAD(p0, EDIBgbl::TzLOAD, atan((double)1.0) * 0, rsText, 100, 1.0f , _T("DLOAD"),lpszTextStyle,(_T("%.f")),iAlign);
			
			
			//写注释
			int iNo=1;
			p0.SetX(pt3x - (Ax = _T("A3"), 305, 290));
			p0.SetY(pt3y - 23);	
			
			if( EDIBgbl::g_bCustomDesc || EDIBgbl::g_bOnlySysDesc )
			{
				if( EDIBgbl::g_bCustomDesc )
				{
					if( EDIBgbl::g_bPhsPicture )
					{
						//1.支吊架布置图见x1,支吊架一览表见x2
						tmpStr.Format(IDS_MsgBox_60674,iNo,gsPhsArrangementDwgNo,gsPhsScheduleDwgNo);
						iNo++;
						objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
						objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                        EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
						p0.SetY(p0[1] - 7);
					}
				}
				else 
				{
					//1.支吊架布置图见x1,支吊架一览表见x2
					tmpStr.Format(IDS_MsgBox_60674,iNo,gsPhsArrangementDwgNo,gsPhsScheduleDwgNo);
					iNo++;
					objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
					objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                    EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
					p0.SetY(p0[1] - 7);
				}

				
				if( EDIBgbl::g_bCustomDesc )
				{
					if( EDIBgbl::g_bPhsSetNum )
					{
						//2.本期工程按图制作x套
						tmpStr.Format(IDS_MsgBox_60676,iNo,vtoi(rsza->GetCollect(_T("num"))));
						iNo++;
						objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
						objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                        EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
						p0.SetY(p0[1] - 7);
					}
				}
				else 
				{
					//2.本期工程按图制作x套
					tmpStr.Format(IDS_MsgBox_60676,iNo,vtoi(rsza->GetCollect(_T("num"))));
					iNo++;
					objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
					objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                    EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
					p0.SetY(p0[1] - 7);
				}

				CString strDg=vtos(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("dg1")));
				strDg.MakeUpper();
				float fPzx,fPzz;
				fPzx=vtof(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("pzx")));
				fPzz=vtof(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("pzz")));
				if(fabs(fPzx-0.0f) > 0.0001 || fabs(fPzz-0.0f) > 0.0001 )
				{
					//sys 这里的输出，无论何时只要有就输出
					if(strDg==_T("D"))
					{
						//%d. 管部尺寸已包含偏装值
						tmpStr.Format(IDS_PZ_D,iNo++);
						objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
						objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                        EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
						p0.SetY(p0[1] - 7);
					}
					else if(strDg==_T("G"))
					{
						//%d. 根部尺寸已包含偏装值
						tmpStr.Format(IDS_PZ_G,iNo++);
						objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
						objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                        EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
						p0.SetY(p0[1] - 7);
					} 
				}

				//sys 这里的输出，无论何时只要有就输出
				//4.本支吊架拉杆直径按单侧承受全部结构荷载设计
				if( vtob(rsza->GetCollect(_T("ifLongVertPipe"))) )
				{
					tmpStr.Format(IDS_SINGLEROD_CARRYALLPJG,iNo++);
					objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
					objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                    EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
					p0.SetY(p0[1] - 7);
				}			
				
				//如果使用系统输出或者用户自定义输出
				CCadPoint oldP0=p0;
				if( EDIBgbl::g_bCustomDesc )
				{
					if( EDIBgbl::g_bPhsManual )
					{
						//5.焊接要求及焊缝高度见如下手册:
						tmpStr.Format(IDS_WeldingDemandAsFollow,iNo++);
						objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
						objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                        EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
						
						p0.SetX(p0[0] + 43);
						//管部:焊接要求及焊缝高度见xx
						tmpStr.Format(IDS_WeldingDemandSeeManual,GetResStr(IDS_FrmSelSpecification_Option1_PA),gsPhsDescPASel);
						objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
						objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                        EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
						p0.SetY(p0[1] - 7);
						//连接件:焊接要求及焊缝高度见xx
						tmpStr.Format(GetResStr(IDS_WeldingDemandSeeManual),GetResStr(IDS_FrmSelSpecification_Option1_PART),gsPhsDescPARTSel);
						objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
						objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                        EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
						p0.SetY(p0[1] - 7);
						//根部:焊接要求及焊缝高度见xx
						tmpStr.Format(GetResStr(IDS_WeldingDemandSeeManual),GetResStr(IDS_FrmSelSpecification_Option1_SA),gsPhsDescSASel);				
						objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
						objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                        EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
						p0.SetY(p0[1] - 7);
					}
				}
				else 
				{
					//5.焊接要求及焊缝高度见如下手册:
					tmpStr.Format(IDS_WeldingDemandAsFollow,iNo++);
					objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
					objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                    EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
					
					p0.SetX(p0[0] + 43);
					//管部:焊接要求及焊缝高度见xx
					tmpStr.Format(IDS_WeldingDemandSeeManual,GetResStr(IDS_FrmSelSpecification_Option1_PA),gsPhsDescPASel);
					objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
					objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                    EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
					p0.SetY(p0[1] - 7);
					//连接件:焊接要求及焊缝高度见xx
					tmpStr.Format(GetResStr(IDS_WeldingDemandSeeManual),GetResStr(IDS_FrmSelSpecification_Option1_PART),gsPhsDescPARTSel);
					objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
					objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                    EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
					p0.SetY(p0[1] - 7);
					//根部:焊接要求及焊缝高度见xx
					tmpStr.Format(GetResStr(IDS_WeldingDemandSeeManual),GetResStr(IDS_FrmSelSpecification_Option1_SA),gsPhsDescSASel);
					
					objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
					objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                    EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
					p0.SetY(p0[1] - 7);
				}

				//用户自定义的注释信息
				if( EDIBgbl::g_bCustomDesc )
				{
					if( EDIBgbl::g_bUserDesc )
					{
						oldP0.SetY(p0[1]);
						DrawUserDesc( iNo, oldP0, EDIBAcad::MoSpace, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")) );
					}

				}
				else
				{
					if( EDIBgbl::g_bOnlyUserDesc )
					{
						oldP0.SetY(p0[1]);
						DrawUserDesc( iNo, oldP0, EDIBAcad::MoSpace, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")) );
					}
				}
			}
			else if ( EDIBgbl::g_bOnlyUserDesc )//用户自定义的注释信息
			{
				DrawUserDesc(iNo, p0, EDIBAcad::MoSpace, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")) );
			}
		}
		else
		{
			//西北院格式
			//位移荷载合并表tmpDisplacementLoad
			if(!bPAIsHanger() && /*!(giUPxyz==1 && yr==0 || giUPxyz==2 && yr==0 || giUPxyz==3 && yr==0 ) &&*/ ZdjType.GetLength()<4 )
			{
				//如果是非弹性支架，采用专门的详细荷载位移表格式，
				//含有推力/力矩/线位移/角位移共12个数据，2行以上，冷态/热态两种工况
				if(EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("tmpPipeDisplacement")))
					EDIBgbl::dbPRJ->Execute((_bstr_t)_T("DROP TABLE tmpPipeDisplacement"), NULL, adCmdText);
				EDIBgbl::dbPRJ->Execute((_bstr_t) _T("CREATE TABLE tmpPipeDisplacement (Jsdbh1 LONG,duty char(20),px REAL,py REAL,pz REAL,mx REAL,my REAL,mz REAL,dx REAL,dy REAL,dz REAL,rx REAL,ry REAL,rz REAL,haz REAL,ZDbz char(20))"), NULL, adCmdText);
				//手工计算，计算节点号为空，此时给jsdbh1字段赋值“无”，否则，该字段无值，INSERT INTO语句会出错。
				/*//pfg20050416 测试 原代码
				SQLx=GetResStr(IDS_INSERT_INTO_tmpPipeDisplacement2)
					+ (vtoi(rsza->GetCollect(_T("jsdbh1")))==0?"无":vtos(rsza->GetCollect(_T("jsdbh1"))) ) + _T(",") 
					+ ftos(fabs(UnitF*vtof(rsza->GetCollect(_T("pxr1"))))) + _T(",") + ftos(fabs(UnitF*vtof(rsza->GetCollect(_T("pyr1")))))  + _T(",") + ftos(fabs(UnitF*vtof(rsza->GetCollect(_T("pzr1"))))) + _T(",") 
					+ ftos(fabs(UnitF*vtof(rsza->GetCollect(_T("mxr1"))))) + _T(",") + ftos(fabs(UnitF*vtof(rsza->GetCollect(_T("myr1")))))  + _T(",") + ftos(fabs(UnitF*vtof(rsza->GetCollect(_T("mzr1"))))) + _T(",") 
					+ vtos(rsza->GetCollect(_T("xr1"))) + _T(",") + vtos(rsza->GetCollect(_T("yr1")))  + _T(",") + vtos(rsza->GetCollect(_T("zr1")))  + _T(",")
					+ vtos(rsza->GetCollect(_T("rxr1"))) + _T(",") + vtos(rsza->GetCollect(_T("ryr1")))  + _T(",") + vtos(rsza->GetCollect(_T("rzr1"))) + _T(",") 
					+ vtos(rsza->GetCollect(_T("haz"))) + _T(",\'tj=") + ftos(t0) + _T("%%dC\')");
					*///pfg20050416


				//pfg20050416 人工输入数据为何不能画表，因为没有输入支吊架节点号,0表示没有支吊架号
				SQLx=GetResStr(IDS_INSERT_INTO_tmpPipeDisplacement2)
					+ (vtoi(rsza->GetCollect(_T("jsdbh1")))==0?"-10000":vtos(rsza->GetCollect(_T("jsdbh1"))) ) + _T(",") 
					+ ftos(fabs(UnitF*vtof(rsza->GetCollect(_T("pxr1"))))) + _T(",") + ftos(fabs(UnitF*vtof(rsza->GetCollect(_T("pyr1")))))  + _T(",") + ftos(fabs(UnitF*vtof(rsza->GetCollect(_T("pzr1"))))) + _T(",") 
					+ ftos(fabs(UnitF*vtof(rsza->GetCollect(_T("mxr1"))))) + _T(",") + ftos(fabs(UnitF*vtof(rsza->GetCollect(_T("myr1")))))  + _T(",") + ftos(fabs(UnitF*vtof(rsza->GetCollect(_T("mzr1"))))) + _T(",") 
					+ vtos(rsza->GetCollect(_T("xr1"))) + _T(",") + vtos(rsza->GetCollect(_T("yr1")))  + _T(",") + vtos(rsza->GetCollect(_T("zr1")))  + _T(",")
					+ vtos(rsza->GetCollect(_T("rxr1"))) + _T(",") + vtos(rsza->GetCollect(_T("ryr1")))  + _T(",") + vtos(rsza->GetCollect(_T("rzr1"))) + _T(",") 
					+ vtos(rsza->GetCollect(_T("haz"))) + _T(",\'tj=") + ftos(t0) + _T("%%dC\')");
				//pfg20050416				

				EDIBgbl::dbPRJ->Execute((_bstr_t) SQLx, NULL, adCmdText);
				SQLx=GetResStr(IDS_INSERT_INTO_tmpPipeDisplacement)  
					
					+ ftos(fabs(UnitF*vtof(rsza->GetCollect(_T("pxl1"))))) + _T(",") + ftos(fabs(UnitF*vtof(rsza->GetCollect(_T("pyl1")))))  + _T(",") + ftos(fabs(UnitF*vtof(rsza->GetCollect(_T("pzl1"))))) + _T(",") 
					+ ftos(fabs(UnitF*vtof(rsza->GetCollect(_T("mxl1"))))) + _T(",") + ftos(fabs(UnitF*vtof(rsza->GetCollect(_T("myl1")))))  + _T(",") + ftos(fabs(UnitF*vtof(rsza->GetCollect(_T("mzl1"))))) + _T(",") 
					+ vtos(rsza->GetCollect(_T("xl1"))) + _T(",") + vtos(rsza->GetCollect(_T("yl1")))  + _T(",") + vtos(rsza->GetCollect(_T("zl1")))  + _T(",")
					+ vtos(rsza->GetCollect(_T("rxl1"))) + _T(",") + vtos(rsza->GetCollect(_T("ryl1")))  + _T(",") + vtos(rsza->GetCollect(_T("rzl1"))) + _T(",") 
					+ vtos(rsza->GetCollect(_T("haz"))) + _T(",\'t0=20%%dC\')");
				EDIBgbl::dbPRJ->Execute((_bstr_t) SQLx, NULL, adCmdText);
				
				//CString SQLx;
				//开始绘制表格
				p0.SetPoint(pt3x, pt3y);
				SQLx = _T("SELECT * FROM tmpPipeDisplacement");
				rsText->Open((_bstr_t)SQLx, _variant_t((IDispatch*)dbZDJcrude,true), 
					adOpenStatic, adLockOptimistic, adCmdText); 
				EDIBAcad::DrawTableACAD(p0, EDIBgbl::TzDLOADS, atan((double)1.0) * 0, rsText, 100, 1.0f , _T("DLOAD"),lpszTextStyle,(_T("%.f")),iAlign);
				rsText->Close();
				
				
				
				//写注释
				int iNo=1;
				p0.SetX(pt3x - (Ax == _T("A3") ? 193 : 178));
				p0.SetY(p0[1] - 8 * 7 + 3);

				//如果需要输出系统定义的信息
				if( EDIBgbl::g_bCustomDesc || EDIBgbl::g_bOnlySysDesc )
				{
					if( EDIBgbl::g_bCustomDesc )
					{
						if( EDIBgbl::g_bPhsPicture )
						{
							//1.支吊架布置图见x1,支吊架一览表见x2
							tmpStr.Format(IDS_MsgBox_60674,iNo,gsPhsArrangementDwgNo,gsPhsScheduleDwgNo);
							iNo++;
							objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
							objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                            EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
							p0.SetY(p0[1] - 7);
						}
					}
					else 
					{
						//1.支吊架布置图见x1,支吊架一览表见x2
						tmpStr.Format(IDS_MsgBox_60674,iNo,gsPhsArrangementDwgNo,gsPhsScheduleDwgNo);
						iNo++;
						objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
						objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                        EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
						p0.SetY(p0[1] - 7);
					}

					if( EDIBgbl::g_bCustomDesc )
					{
						if( EDIBgbl::g_bPhsSetNum )
						{
							//2.本期工程按图制作x套
							tmpStr.Format(IDS_MsgBox_60676,iNo,vtoi(rsza->GetCollect(_T("num"))));
							iNo++;
							objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
							objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                            EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
							p0.SetY(p0[1] - 7);
						}
					}
					else 
					{
						//2.本期工程按图制作x套
						tmpStr.Format(IDS_MsgBox_60676,iNo,vtoi(rsza->GetCollect(_T("num"))));
						iNo++;
						objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
						objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                        EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
						p0.SetY(p0[1] - 7);
					}


					//3.本组装图采用|1号支吊架模板(AutoPHS定义)
					CString strDg=vtos(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("dg1")));
					strDg.MakeUpper();
					float fPzx,fPzz;
					fPzx=vtof(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("pzx")));
					fPzz=vtof(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("pzz")));
					if(fabs(fPzx-0.0f) > 0.0001 || fabs(fPzz-0.0f) > 0.0001 )
					{
						//sys
						if(strDg==_T("D"))
						{
							//%d.管部尺寸已包含偏装值。
							tmpStr.Format(IDS_PZ_D,iNo);
							iNo++;
							objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
							objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                            EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
							p0.SetY(p0[1] - 7);
						}
						else if(strDg==_T("G"))
						{
							//%d.根部尺寸已包含偏装值。
							tmpStr.Format(IDS_PZ_G,iNo);
							iNo++;
							objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
							objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                            EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
							p0.SetY(p0[1] - 7);
						}
					}

					//sys
					//4.本支吊架拉杆直径按单侧承受全部结构荷载设计
					if( vtob(rsza->GetCollect(_T("ifLongVertPipe"))) )
					{
						tmpStr.Format(IDS_SINGLEROD_CARRYALLPJG,iNo++);
						objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
						objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                        EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
						p0.SetY(p0[1] - 7);
					}			

					CCadPoint oldP0=p0;
					if( EDIBgbl::g_bCustomDesc )
					{
						if( EDIBgbl::g_bPhsManual )
						{
							//5.焊接要求及焊缝高度见如下手册:
							tmpStr.Format(IDS_WeldingDemandAsFollow,iNo);
							iNo++;
							objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
							objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                            EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));

							p0.SetX(p0[0] + 43);
							//管部:焊接要求及焊缝高度见xx
							tmpStr.Format(GetResStr(IDS_WeldingDemandSeeManual),GetResStr(IDS_FrmSelSpecification_Option1_PA),gsPhsDescPASel);
							objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
							objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                            EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
							p0.SetY(p0[1] - 7);
							//连接件:焊接要求及焊缝高度见xx
							tmpStr.Format(GetResStr(IDS_WeldingDemandSeeManual),GetResStr(IDS_FrmSelSpecification_Option1_PART),gsPhsDescPARTSel);
							objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
							objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                            EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
							p0.SetY(p0[1] - 7);
							//根部:焊接要求及焊缝高度见xx
							tmpStr.Format(GetResStr(IDS_WeldingDemandSeeManual),GetResStr(IDS_FrmSelSpecification_Option1_SA),gsPhsDescSASel);
							objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
							objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                            EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
							p0.SetY(p0[1] - 7);
						}
					}
					else
					{
						//5.焊接要求及焊缝高度见如下手册:
						tmpStr.Format(IDS_WeldingDemandAsFollow,iNo);
						iNo++;
						objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
						objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                        EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));

						p0.SetX(p0[0] + 43);
						//管部:焊接要求及焊缝高度见xx
						tmpStr.Format(GetResStr(IDS_WeldingDemandSeeManual),GetResStr(IDS_FrmSelSpecification_Option1_PA),gsPhsDescPASel);
						objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
						objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                        EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
						p0.SetY(p0[1] - 7);
						//连接件:焊接要求及焊缝高度见xx
						tmpStr.Format(GetResStr(IDS_WeldingDemandSeeManual),GetResStr(IDS_FrmSelSpecification_Option1_PART),gsPhsDescPARTSel);
						objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
						objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                        EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
						p0.SetY(p0[1] - 7);
						//根部:焊接要求及焊缝高度见xx
						tmpStr.Format(GetResStr(IDS_WeldingDemandSeeManual),GetResStr(IDS_FrmSelSpecification_Option1_SA),gsPhsDescSASel);
						objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
						objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                        EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
						p0.SetY(p0[1] - 7);
					}

					//用户自定义的注释信息
					if( EDIBgbl::g_bCustomDesc )
					{
						if( EDIBgbl::g_bUserDesc )
						{
							oldP0.SetY(p0[1]);
							DrawUserDesc( iNo, oldP0, EDIBAcad::MoSpace, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")) );
						}
					}
					else
					{
						if( EDIBgbl::g_bOnlyUserDesc )
						{
							oldP0.SetY(p0[1]);
							DrawUserDesc( iNo, oldP0, EDIBAcad::MoSpace, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")) );
						}
					}
					}
					else if ( EDIBgbl::g_bOnlyUserDesc )//用户自定义的注释信息
					{
						DrawUserDesc(iNo, p0, EDIBAcad::MoSpace, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")) );
					}
			}
			else
			{
				//西北院位移荷载表格式，含有paz/pgz/pjg/冷热位移等数据，一行。
				if(EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("tmpDisplacementLoad")) )
					EDIBgbl::dbPRJ->Execute((_bstr_t)_T("DROP TABLE tmpDisplacementLoad"), NULL, adCmdText);
				MakeZDJ_ZD(zdjh);
				PhsYLBMake(zdjh);
				SQLx = _T("SELECT * INTO tmpDisplacementLoad IN \'") + EDIBgbl::GetDBName(EDIBgbl::dbPRJ) + _T("\' FROM [") + EDIBgbl::Btype[EDIBgbl::TZD] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" AND zdjh=") + ltos(modPHScal::zdjh);
				EDIBgbl::dbPRJDB->Execute((_bstr_t)SQLx, NULL, adCmdText);
				//更新备注字段为温度值

				if(EDIBAcad::g_bHzabs)
					SQLx = _T("UPDATE tmpDisplacementLoad SET ZDbz=\'tj=") + ftos(t0) + _T("%%dC\' , pgz1 = abs(pgz1), pjg1 = abs(pjg1)  WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" AND zdjh=") + ltos(zdjh);
				else
					SQLx = _T("UPDATE tmpDisplacementLoad SET ZDbz=\'tj=") + ftos(t0) + _T("%%dC\' , pgz1 = pgz1, pjg1 = pjg1  WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" AND zdjh=") + ltos(zdjh);

				EDIBgbl::dbPRJ->Execute((_bstr_t)SQLx, NULL, adCmdText);

				//开始绘制表格
				p0.SetPoint(pt3x,pt3y,0);
				SQLx = _T("SELECT * FROM tmpDisplacementLoad WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" AND zdjh=") + ltos(zdjh);
				rsText->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
					adOpenStatic, adLockOptimistic, adCmdText); 
				//pfg20050519绝对压缩值
				if(modPHScal::giPhsOutINFOTableType==3)
				{
					iZDLOADtype=35;
				}
				//pfg20050519
				
				EDIBAcad::DrawTableACAD(p0, iZDLOADtype, atan((double)1.0) * 0, rsText, 100, 1.0f ,  _T("DLOAD"),lpszTextStyle,(_T("%.f")),iAlign);
				
				
				
				
				//写注释
				int iNo=1;
				p0.SetX(pt3x - (Ax == _T("A3") ? 193 : 178));
				p0.SetY(p0[1] - 7 * 7 + 3);


				//如果需要输出系统定义的信息
				if( EDIBgbl::g_bCustomDesc || EDIBgbl::g_bOnlySysDesc )
				{
					if( EDIBgbl::g_bCustomDesc )
					{
						if( EDIBgbl::g_bPhsPicture )
						{
							//1.支吊架布置图见x1,支吊架一览表见x2
							tmpStr.Format(IDS_MsgBox_60674,iNo,gsPhsArrangementDwgNo,gsPhsScheduleDwgNo);
							iNo++;
							objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
							objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                            EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
							p0.SetY(p0[1] - 7);
						}
					}
					else 
					{
						//1.支吊架布置图见x1,支吊架一览表见x2
						tmpStr.Format(IDS_MsgBox_60674,iNo,gsPhsArrangementDwgNo,gsPhsScheduleDwgNo);
						iNo++;
						objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
						objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                        EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
						p0.SetY(p0[1] - 7);
					}

					if( EDIBgbl::g_bCustomDesc )
					{
						if( EDIBgbl::g_bPhsSetNum )
						{
							//2.本期工程按图制作x套
							tmpStr.Format(IDS_MsgBox_60676,iNo,vtoi(rsza->GetCollect(_T("num"))));
							iNo++;
							objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
							objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                            EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
							p0.SetY(p0[1] - 7);
						}
					}
					else
					{
						//2.本期工程按图制作x套
						tmpStr.Format(IDS_MsgBox_60676,iNo,vtoi(rsza->GetCollect(_T("num"))));
						iNo++;
						objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
						objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                        EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
						p0.SetY(p0[1] - 7);
					}
					
					CString strDg=vtos(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("dg1")));
					strDg.MakeUpper();
					float fPzx,fPzz;
					fPzx=vtof(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("pzx")));
					fPzz=vtof(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("pzz")));
					if(fabs(fPzx-0.0f) > 0.0001 || fabs(fPzz-0.0f) > 0.0001 )
					{
						//sys
						if(strDg==_T("D"))
						{
							//%d.管部尺寸已包含偏装值。
							tmpStr.Format(IDS_PZ_D,iNo);
							iNo++;
							objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
							objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                            EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
							p0.SetY(p0[1] - 7);
						}
						else if(strDg==_T("G"))
						{
							//%d.根部尺寸已包含偏装值。
							tmpStr.Format(IDS_PZ_G,iNo);
							iNo++;
							objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
							objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                            EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
							p0.SetY(p0[1] - 7);
						}
					}
					//sys
					//4.本支吊架拉杆直径按单侧承受全部结构荷载设计
					if( vtob(rsza->GetCollect(_T("ifLongVertPipe"))) )
					{
						tmpStr.Format(IDS_SINGLEROD_CARRYALLPJG,iNo++);
						objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
						objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                        EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
						p0.SetY(p0[1] - 7);
					}			


					CCadPoint oldP0=p0;
					if( EDIBgbl::g_bCustomDesc )
					{
						if( EDIBgbl::g_bPhsManual )
						{
							//5.焊接要求及焊缝高度见如下手册:
							tmpStr.Format(IDS_WeldingDemandAsFollow,iNo);
							iNo++;
							objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
							objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                            EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
							p0.SetX(p0[0] + 43);
							//管部:焊接要求及焊缝高度见xx
							tmpStr.Format(GetResStr(IDS_WeldingDemandSeeManual),GetResStr(IDS_FrmSelSpecification_Option1_PA),gsPhsDescPASel);
							objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
							objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                            EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
							p0.SetY(p0[1] - 7);
							//连接件:焊接要求及焊缝高度见xx
							tmpStr.Format(GetResStr(IDS_WeldingDemandSeeManual),GetResStr(IDS_FrmSelSpecification_Option1_PART),gsPhsDescPARTSel);
							
							objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
							objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                            EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
							p0.SetY(p0[1] - 7);
							//根部:焊接要求及焊缝高度见xx
							tmpStr.Format(GetResStr(IDS_WeldingDemandSeeManual),GetResStr(IDS_FrmSelSpecification_Option1_SA),gsPhsDescSASel);
							objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
							objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                            EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
							p0.SetY(p0[1] - 7);
						}
					}
					else
					{
						//5.焊接要求及焊缝高度见如下手册:
						tmpStr.Format(IDS_WeldingDemandAsFollow,iNo);
						iNo++;
						objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
						objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                        EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
						p0.SetX(p0[0] + 43);
						//管部:焊接要求及焊缝高度见xx
						tmpStr.Format(GetResStr(IDS_WeldingDemandSeeManual),GetResStr(IDS_FrmSelSpecification_Option1_PA),gsPhsDescPASel);
						objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
						objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                        EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
						p0.SetY(p0[1] - 7);
						//连接件:焊接要求及焊缝高度见xx
						tmpStr.Format(GetResStr(IDS_WeldingDemandSeeManual),GetResStr(IDS_FrmSelSpecification_Option1_PART),gsPhsDescPARTSel);
						
						objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
						objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                        EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
						p0.SetY(p0[1] - 7);
						//根部:焊接要求及焊缝高度见xx
						tmpStr.Format(GetResStr(IDS_WeldingDemandSeeManual),GetResStr(IDS_FrmSelSpecification_Option1_SA),gsPhsDescSASel);
						objEnt=EDIBAcad::MoSpace.Invoke(_T("AddText"),3,&_variant_t(tmpStr),(LPVARIANT)p0,&_variant_t((double)4.0)).pdispVal;
						objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM"))));
                        EDIBAcad::ApplyTextStyleProperty(objEnt, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")),_T("注释"));
						p0.SetY(p0[1] - 7);
					}
					
					//用户自定义的注释信息
					if( EDIBgbl::g_bCustomDesc )
					{
						if( EDIBgbl::g_bUserDesc )
						{
							oldP0.SetY(p0[1]);
							DrawUserDesc( iNo, oldP0, EDIBAcad::MoSpace, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")) );
						}
					}
					else
					{
						if( EDIBgbl::g_bOnlyUserDesc )
						{
							oldP0.SetY(p0[1]);
							DrawUserDesc( iNo, oldP0, EDIBAcad::MoSpace, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")) );
						}
					}
				}
				else if ( EDIBgbl::g_bOnlyUserDesc )//用户自定义的注释信息
				{
					DrawUserDesc(iNo, p0, EDIBAcad::MoSpace, EDIBAcad::GetTextStyleName(_T("注释"), _T("BOM")) );
				}
			}
		}
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}
}

void modPHScal::PhsYLBMake(long zdjh)
{
	//目的:生成支吊架号=zdjh的支吊架一览表
	//此前，调用makezdj_zd过程，清除支吊架号=zdjh的支吊架一览表
	//返回:支吊架一览表保存到TZD表。
	_RecordsetPtr rs;
	rs.CreateInstance(__uuidof(Recordset));
	_RecordsetPtr rs1;
	rs1.CreateInstance(__uuidof(Recordset));
	_variant_t Bmk ;
	_Recordset* rsza=FrmTxsr.m_pViewTxsr->m_ActiveRs;
	CString strDrawName;
	CString strHgz,strHaz;
	CString SQLx,sTmp;
	float UnitF=(Unit_Force == _T("kgf") ? EDIBgbl::kgf2N : 1);
	bool b1st=false;
	SQLx=CString( _T("SELECT * FROM [")) + EDIBgbl::Btype[EDIBgbl::TZD] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) +(zdjh==0 ? _T("") : CString(_T(" AND zdjh=")) + ltos(zdjh));
	
	try
	{
		rs1->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
			adOpenDynamic, adLockOptimistic, adCmdText); 
		if(zdjh!=0)
		{
			//生成西北院格式的支吊架明细表
			PreCalCrudeData(rsza);
			rs1->AddNew();
			_variant_t vTmp;
			CString tmpStr;
			if(gbDrawNoPreMethod)
				tmpStr=EDIBAcad::GetDrawIDAndName((zdjh + glDrawNoStartNumber - 1), strDrawName);
			else
				tmpStr = EDIBAcad::GetDrawIDAndName((zdjh + glDrawNoStartNumber - 1), strDrawName);
			if( sngSEL < sngPEL )
				rs1->put_Collect((_variant_t)_T("HSid"),STR_VAR(GetResStr(IDS_BRANCH)));
			else				
				rs1->put_Collect((_variant_t)_T("HSid"),STR_VAR(GetResStr(IDS_SUSPEND)));
			
			rs1->put_Collect((_variant_t)_T("ZDID"),STR_VAR(tmpStr));
			rs1->put_Collect((_variant_t)_T("VolumeID"),_variant_t(EDIBgbl::SelVlmID));
			rs1->put_Collect((_variant_t)_T("PLName"),STR_VAR(EDIBgbl::SelVlmName));
			rs1->put_Collect((_variant_t)_T("PLN"),STR_VAR(EDIBgbl::SelJcdm));//石化行业用，管线编号，暂时用卷册号代替
			rs1->put_Collect((_variant_t)_T("ConDwg"),STR_VAR(EDIBgbl::VolumeNo));
			rs1->put_Collect((_variant_t)_T("TagNo"),STR_VAR(EDIBgbl::VolumeNo));
			rs1->put_Collect((_variant_t)_T("PhsNo"),STR_VAR(EDIBgbl::PhsNo));
			rs1->put_Collect((_variant_t)_T("zdjh"),rsza->GetCollect(_T("ZDJH")));
			
			rs1->put_Collect((_variant_t)_T("ZDmc"),STR_VAR(gsPhsName));         //写入“支吊架名称”
			CString strGG;//写入“管径”
			if( EDIBgbl::bSymbol )
				strGG = _T("%%C") + vtos(rsza->GetCollect("dj1"));
			else
				strGG = vtos(rsza->GetCollect("dj1"));
			rs1->put_Collect((_variant_t)_T("ZDgg"),STR_VAR(strGG));
			rs1->put_Collect((_variant_t)_T("MCAndGG"),STR_VAR(gsPhsName+"   "+strGG));//写入“名称及规格”
			rs1->put_Collect((_variant_t)_T("BornSA"),STR_VAR(vtos(rsza->GetCollect("BornSA"))));  //写入“生根地点”
			rs1->put_Collect((_variant_t)_T("BornSAEngName"),STR_VAR(vtos(rsza->GetCollect("BornSAEngName"))));  //写入“生根地点”
			rs1->put_Collect((_variant_t)_T("BornSAEngChina"),STR_VAR(vtos(rsza->GetCollect("BornSAEngChina"))));  //写入“生根地点”
			rs1->put_Collect((_variant_t)_T("ZDnum"),rsza->GetCollect(_T("Num")));
			if(EDIBAcad::g_bHzabs)
				rs1->put_Collect((_variant_t)_T("paz1"),_variant_t(fabs(vtof(rsza->GetCollect(_T("paz1"))) * (float)UnitF)));
			else
				rs1->put_Collect((_variant_t)_T("paz1"),_variant_t(vtof(rsza->GetCollect(_T("paz1"))) * (float)UnitF));
			rs1->put_Collect((_variant_t)_T("pgz1"),_variant_t(vtof(rsza->GetCollect(_T("pgz1"))) * (float)UnitF));
			rs1->put_Collect((_variant_t)_T("pjg1"),_variant_t(vtof(rsza->GetCollect(_T("pjg1"))) * (float)UnitF));
			rs1->put_Collect((_variant_t)_T("xr1"),rsza->GetCollect(_T("xr1")));
			vTmp=rsza->GetCollect(_T("yr1"));
			float fyr1=(int(vtof(vTmp)*10))/10.0;
			rs1->put_Collect((_variant_t)_T("yr1"),_variant_t(fyr1));
			rs1->put_Collect((_variant_t)_T("zr1"),rsza->GetCollect(_T("zr1")));
			rs1->put_Collect((_variant_t)_T("xl1"),rsza->GetCollect(_T("xl1")));
			rs1->put_Collect((_variant_t)_T("yl1"),rsza->GetCollect(_T("yl1")));
			rs1->put_Collect((_variant_t)_T("zl1"),rsza->GetCollect(_T("zl1")));
			rs1->put_Collect((_variant_t)_T("jsdbh1"),rsza->GetCollect(_T("jsdbh1")));
			rs1->put_Collect((_variant_t)_T("temp"),rsza->GetCollect(_T("T01")));
			rs1->put_Collect((_variant_t)_T("InstallPEL"),rsza->GetCollect(_T("dh1")));
			rs1->put_Collect((_variant_t)_T("SEL"),rsza->GetCollect(_T("gh1")));
			rs1->Update();
			rs1->MoveLast();

			UpdatePipeDimHeight();
			//现在处理管部
			SQLx=CString( _T("SELECT * FROM [")) + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" AND zdjh=") + ltos(zdjh) + _T(" AND Index=") + ltos(iPA);
			rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
				adOpenDynamic, adLockOptimistic, adCmdText); 
			
			if( !rs->BOF && !rs->adoEOF ){
			
				
				while( !rs->adoEOF){
					if( b1st ){
						//(vtoi(GetFields(rs,_T("CLnum1"))) ==1 ? _T("") : vtos(GetFields(rs,_T("CLnum1"))) + _T("x")) + vtos(GetFields(rs,_T("CLgg")))
						rs1->put_Collect((_variant_t)_T("PAbh"), STR_VAR(vtos(GetFields(rs1,_T("PAbh")))+ _T(";") + (vtoi(GetFields(rs,_T("CLnum"))) ==1 ? _T("") : vtos(GetFields(rs,_T("CLnum"))) + _T("x")) + vtos(GetFields(rs,_T("CLgg")))));
						rs1->put_Collect((_variant_t)_T("PAweight"), _variant_t(vtof(GetFields(rs1,_T("PAweight"))) +vtof(GetFields(rs,_T("CLzz")))));
					}else{
						rs1->put_Collect((_variant_t)_T("PAbh"),STR_VAR((vtoi(GetFields(rs,_T("CLnum"))) ==1 ? _T("") : vtos(GetFields(rs,_T("CLnum"))) + _T("x")) + vtos(GetFields(rs,_T("CLgg")))));
						rs1->put_Collect((_variant_t)_T("PAweight"),_variant_t(vtof(GetFields(rs,_T("CLzz")))));
						//是第一个，设置标志。
						b1st = true;
					}
					rs1->Update();
					rs->MoveNext();					
				}
				
			}
			//复位标志
			b1st = false;
			if(rs->State == adStateOpen)
				rs->Close();
			//现在处理恒力弹簧或弹簧
			SQLx=CString( _T("SELECT ClassID,GDW1,CLmc,CLgg,CLid,CLcl,CLdw,CLdz,sum(CLnum) as CLnum1,sum(CLzz) as CLzz1 FROM [")) + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" AND zdjh=") + ltos(zdjh) + _T(" AND (ClassID=") + ltos(iCSPR) + _T(" OR ClassID=") + ltos(iSPR) + _T(") GROUP BY CLmc,CLgg,CLid,CLcl,CLdw,CLdz,CLnum,CLzz,GDW1,ClassID");
			rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
				adOpenDynamic, adLockOptimistic, adCmdText); 
			if( !rs->BOF && !rs->adoEOF )
			{
				int iSEQofSPR=0;
				while( !rs->adoEOF)
				{
					if( b1st )
					{						
						rs1->put_Collect((_variant_t)_T("SPRbh"), STR_VAR(vtos(GetFields(rs1,_T("SPRbh"))) + _T(";") + (vtoi(GetFields(rs,_T("CLnum1"))) ==1 ? _T("") : vtos(GetFields(rs,_T("CLnum1"))) + _T("x")) + vtos(GetFields(rs,_T("CLgg")))));
						rs1->put_Collect((_variant_t)_T("SPRweight"), _variant_t(vtof(GetFields(rs1,_T("SPRweight"))) +vtof(GetFields(rs,_T("CLzz1")))));
						//对于弹簧，函数GetPhsBHandSizes已经将相对安装压缩值写到TZB的GDW1字段，以便在函数PhsYLBmake输出一览表使用
						if( vtoi(GetFields(rs,_T("ClassID"))) == iSPR ) 
						{								
							sTmp.Format(_T("%g"),int((sSprInfo[iSEQofSPR].haz + sSprInfo[iSEQofSPR].Hpcmprs)*10)/10.0);
							rs1->put_Collect((_variant_t)_T("hazTotal"),STR_VAR(vtos(GetFields(rs1,_T("hazTotal"))) + _T(";") + sTmp));
							sTmp.Format(_T("%g"),int((sSprInfo[iSEQofSPR].haz - sSprInfo[iSEQofSPR].HeatDisp + sSprInfo[iSEQofSPR].Hpcmprs)*10)/10.0);
							rs1->put_Collect((_variant_t)_T("hgzTotal"),STR_VAR(vtos(GetFields(rs1,_T("hgzTotal"))) + _T(";") + sTmp));
							strHaz.Format(_T("%g"),int(sSprInfo[iSEQofSPR].haz*10)/(double)10.0);
							strHgz.Format(_T("%g"),int((sSprInfo[iSEQofSPR].haz - sSprInfo[iSEQofSPR].HeatDisp)*10)/10.0);
							rs1->put_Collect((_variant_t)_T("Haz"),STR_VAR(vtos(GetFields(rs1,_T("Haz"))) + _T(";") + strHaz));
							rs1->put_Collect((_variant_t)_T("Hgz"),STR_VAR(vtos(GetFields(rs1,_T("Hgz"))) + _T(";") + strHgz));
							rs1->put_Collect((_variant_t)_T("SPRpcmprs"), STR_VAR(vtos(GetFields(rs1,_T("SPRpcmprs"))) + _T(";") + ltos((long)modPHScal::sSprInfo[iSEQofSPR].Hpcmprs)));
							rs1->put_Collect((_variant_t)_T("Diameter"), STR_VAR(vtos(GetFields(rs1,_T("Diameter"))) + _T(";") + ltos((long)modPHScal::sSprInfo[iSEQofSPR].FiJ)));
							rs1->put_Collect((_variant_t)_T("SpringNo"), STR_VAR(vtos(GetFields(rs1,_T("SpringNo"))) + _T(";") + ltos((long)modPHScal::sSprInfo[iSEQofSPR].DH)));
							rs1->put_Collect((_variant_t)_T("BH"), STR_VAR(vtos(GetFields(rs1,_T("BH"))) + _T(";") + modPHScal::sSprInfo[iSEQofSPR].BH));
						}
					}
					else
					{						
						rs1->put_Collect((_variant_t)_T("SPRbh"),STR_VAR((vtoi(GetFields(rs,_T("CLnum1"))) ==1 ? _T("") : vtos(GetFields(rs,_T("CLnum1"))) + _T("x")) + vtos(GetFields(rs,_T("CLgg")))));
						rs1->put_Collect((_variant_t)_T("SPRweight"),_variant_t(vtof(GetFields(rs,_T("CLzz1")))));
						//对于弹簧，函数GetPhsBHandSizes已经将相对安装压缩值写到TZB的GDW1字段，以便在函数PhsYLBmake输出一览表使用
						if( vtoi(GetFields(rs,_T("ClassID"))) == iSPR ) 
						{
							sTmp.Format(_T("%g"),int((sSprInfo[iSEQofSPR].haz + sSprInfo[iSEQofSPR].Hpcmprs)*10)/10.0);
							rs1->put_Collect((_variant_t)_T("hazTotal"),STR_VAR(sTmp));	
							sTmp.Format(_T("%g"),int((sSprInfo[iSEQofSPR].haz - sSprInfo[iSEQofSPR].HeatDisp + sSprInfo[iSEQofSPR].Hpcmprs)*10)/10.0);
							rs1->put_Collect((_variant_t)_T("hgzTotal"),STR_VAR(sTmp));	
							strHaz.Format(_T("%g"),int(sSprInfo[iSEQofSPR].haz*10)/(double)10.0);
							strHgz.Format(_T("%g"),int((sSprInfo[iSEQofSPR].haz - sSprInfo[iSEQofSPR].HeatDisp)*10)/10.0);
							rs1->put_Collect((_variant_t)_T("Haz"),STR_VAR(strHaz));							
							rs1->put_Collect((_variant_t)_T("Hgz"),STR_VAR(strHgz));							
							rs1->put_Collect((_variant_t)_T("SPRpcmprs"),STR_VAR(ltos((long)modPHScal::sSprInfo[iSEQofSPR].Hpcmprs)));
							rs1->put_Collect((_variant_t)_T("Diameter"),STR_VAR(ltos((long)modPHScal::sSprInfo[iSEQofSPR].FiJ)));
							rs1->put_Collect((_variant_t)_T("SpringNo"),STR_VAR(ltos((long)modPHScal::sSprInfo[iSEQofSPR].DH)));
							rs1->put_Collect((_variant_t)_T("BH"),STR_VAR(modPHScal::sSprInfo[iSEQofSPR].BH));
						}
						//是第一个，设置标志。
						b1st = true;
					}
					rs1->Update();
					rs->MoveNext();
					iSEQofSPR++;
				}				
			}
			//复位标志
			b1st = false;
			rs->Close();      
      
			//现在处理拉杆
			SQLx=CString( _T("SELECT CLmc,CLgg,CLid,CLcl,CLdw,CLdz,sum(CLnum) as CLnum1,sum(CLzz) as CLzz1 FROM [")) + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" AND zdjh=") + ltos(zdjh) + _T(" AND ClassID=") + ltos(iROD) + _T(" GROUP BY CLmc,CLgg,CLid,CLcl,CLdw,CLdz,CLnum,CLzz");
			rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
				adOpenDynamic, adLockOptimistic, adCmdText); 
			if( !rs->BOF && !rs->adoEOF ){
				
				while( !rs->adoEOF){
					//当循环不止一次时，rs1.EDIT()方法必须在每次设置字段之前使用。否则显示无Edit方法。
					if( b1st ){
						rs1->put_Collect((_variant_t)_T("RODDiaLen"), STR_VAR(vtos(GetFields(rs1,_T("RODDiaLen"))) + _T(";") + (vtoi(GetFields(rs,_T("CLnum1"))) ==1 ? _T("") : vtos(GetFields(rs,_T("CLnum1"))) + _T("x")) + vtos(GetFields(rs,_T("CLgg")))));
						rs1->put_Collect((_variant_t)_T("RODweight"),_variant_t(vtof(GetFields(rs1,_T("RODweight"))) +vtof(GetFields(rs,_T("CLzz1")))));
					}else{
						rs1->put_Collect((_variant_t)_T("RODDiaLen"),STR_VAR((vtoi(GetFields(rs,_T("CLnum1"))) ==1 ? _T("") : vtos(GetFields(rs,_T("CLnum1"))) + _T("x")) + vtos(GetFields(rs,_T("CLgg")))));
						rs1->put_Collect((_variant_t)_T("RODweight"),_variant_t(vtof(GetFields(rs,_T("CLzz1")))));
						//是第一个，设置标志。
						b1st = true;
					}
					rs1->Update();
					rs->MoveNext();
				}
				
			}
			//复位标志
			b1st = false;
			rs->Close();
      
      
			//现在处理非弹性连接件（不包括拉杆）
			SQLx=CString( _T("SELECT CLmc,CLgg,CLid,CLcl,CLdw,CLdz,sum(CLnum) as CLnum1,sum(CLzz) as CLzz1 FROM [")) + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" AND zdjh=") + ltos(zdjh) + _T(" AND NOT IsNull(Index) AND Index=") + ltos(iConnectPART) + _T(" AND ClassID<>") + ltos(iROD) + _T(" AND ClassID<>") + ltos(iCSPR) + _T(" AND ClassID<>") + ltos(iSPR) + _T(" AND IsSAPart<>-1 GROUP BY CLmc,CLgg,CLid,CLcl,CLdw,CLdz,CLnum,CLzz");
			rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
				adOpenDynamic, adLockOptimistic, adCmdText); 
			if( !rs->BOF && !rs->adoEOF ){
				while( !rs->adoEOF){
					//当循环不止一次时，rs1.EDIT()方法必须在每次设置字段之前使用。否则显示无Edit方法。
					if( b1st ){
						rs1->put_Collect((_variant_t)_T("CntPart"), STR_VAR(vtos(GetFields(rs1,_T("CntPart"))) + _T(";") + (vtoi(GetFields(rs,_T("CLnum1"))) ==1 ? _T("") : vtos(GetFields(rs,_T("CLnum1"))) + _T("x")) + vtos(GetFields(rs,_T("CLgg")))));
						rs1->put_Collect((_variant_t)_T("CntPartweight"),_variant_t(vtof(GetFields(rs1,_T("CntPartweight")))+vtof(GetFields(rs,_T("CLzz1")))));
					}else{
						rs1->put_Collect((_variant_t)_T("CntPart"),STR_VAR((vtoi(GetFields(rs,_T("CLnum1"))) ==1 ? _T("") : vtos(GetFields(rs,_T("CLnum1"))) + _T("x")) + vtos(GetFields(rs,_T("CLgg")))));
						//是第一个，设置标志。
						rs1->put_Collect((_variant_t)_T("CntPartweight"),_variant_t(vtof(GetFields(rs,_T("CLzz1")))));
						b1st = true;
					}
					rs1->Update();
					rs->MoveNext();
				}				
			}
			//复位标志
			b1st = false;
			rs->Close();
          
          
			//现在处理非根部中的螺母,螺栓
			//由于在明细一览表中附件中没有统计"F3"(根部已经统计),由西安华瑞提出.
			//原代码SQLx=CString( _T("SELECT CLmc,CLgg,CLid,CLcl,CLdw,CLdz,sum(CLnum) as CLnum1,sum(CLzz) as CLzz1 FROM [")) + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" AND zdjh=") + ltos(zdjh) + _T(" AND (ClassID=") + ltos(iBolts) + _T(" OR ClassID=") + ltos(iNuts) + _T(") AND Not IsNull(SEQ) GROUP BY CLmc,CLgg,CLid,CLcl,CLdw,CLdz,CLnum,CLzz");
			SQLx=CString( _T("SELECT CLmc,CLgg,CLid,CLcl,CLdw,CLdz,sum(CLnum) as CLnum1,sum(CLzz) as CLzz1 FROM [")) + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" AND zdjh=") + ltos(zdjh) + _T(" AND (ClassID=") + ltos(iBolts) + _T(" OR ClassID=") + ltos(iNuts) +_T("OR (ID='F3' AND IsSAPart <> -1 )")+ _T(") AND Not IsNull(SEQ) GROUP BY CLmc,CLgg,CLid,CLcl,CLdw,CLdz,CLnum,CLzz");
			rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
				adOpenDynamic, adLockOptimistic, adCmdText); 
			if( !rs->BOF && !rs->adoEOF ){
				
				while( !rs->adoEOF){
					if( b1st ){
						rs1->put_Collect((_variant_t)_T("Attachment"), STR_VAR(vtos(GetFields(rs1,_T("Attachment"))) + _T(";") + (vtoi(GetFields(rs,_T("CLnum1"))) ==1 ? _T("") : vtos(GetFields(rs,_T("CLnum1"))) + _T("x")) + vtos(GetFields(rs,_T("CLgg")))));
						rs1->put_Collect((_variant_t)_T("Attachmentweight"),_variant_t(vtof(GetFields(rs1,_T("Attachmentweight")))+vtof(GetFields(rs,_T("CLzz1")))));
						rs1->put_Collect((_variant_t)_T("Attachment1"), STR_VAR(vtos(GetFields(rs1,_T("Attachment1"))) + _T(";") + (vtoi(GetFields(rs,_T("CLnum1"))) ==1 ? _T("") : vtos(GetFields(rs,_T("CLnum1"))) + _T("x")) + vtos(GetFields(rs,_T("CLgg")))));
						rs1->put_Collect((_variant_t)_T("Attachmentweight1"),_variant_t(vtof(GetFields(rs1,_T("Attachmentweight1")))+vtof(GetFields(rs,_T("CLzz1")))));
					}else{
						rs1->put_Collect((_variant_t)_T("Attachment"),STR_VAR((vtoi(GetFields(rs,_T("CLnum1"))) ==1 ? _T("") : vtos(GetFields(rs,_T("CLnum1"))) + _T("x")) + vtos(GetFields(rs,_T("CLgg")))));
						//是第一个，设置标志。
						rs1->put_Collect((_variant_t)_T("Attachmentweight"),_variant_t(vtof(GetFields(rs,_T("CLzz1")))));
						rs1->put_Collect((_variant_t)_T("Attachment1"),STR_VAR((vtoi(GetFields(rs,_T("CLnum1"))) ==1 ? _T("") : vtos(GetFields(rs,_T("CLnum1"))) + _T("x")) + vtos(GetFields(rs,_T("CLgg")))));
						//是第一个，设置标志。
						rs1->put_Collect((_variant_t)_T("Attachmentweight1"),_variant_t(vtof(GetFields(rs,_T("CLzz1")))));
						b1st = true;
					}
					rs1->Update();
					rs->MoveNext();
				}
				
			}
			rs->Close();
			//复位标志
			//需要把下面的根部附件连接到螺栓螺母字符串，故关闭下面开关。
			//b1st = false
      
			//SELECT CLmc,CLgg,CLid,CLcl,CLdw,CLdz,sum(CLnum) as CLnum1,sum(CLzz) as CLzz1 FROM TMP2 GROUP BY CLmc,CLgg,CLid,CLcl,CLdw,CLdz,CLnum,CLzz
			//现在处理根部附件(不含型钢)
			SQLx=CString( _T("SELECT * FROM [")) + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" AND zdjh=") + ltos(zdjh) + _T(" AND IsSAPart=-1 AND (ClassID=") + ltos(iAttached) + _T(" OR ClassID=") + ltos(iBolts )+ _T(" OR ClassID=") + ltos(iNuts) + _T(")");
			rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
				adOpenDynamic, adLockOptimistic, adCmdText); 
			if(!( rs->BOF && rs->adoEOF || (vtoi(rsza->GetCollect(_T("bNotSumSA")))!=0) )){
			
				//只选择根部材料需要统计的支吊架
				
				while( !rs->adoEOF){
					if( b1st ){
						rs1->put_Collect((_variant_t)_T("Attachment"), STR_VAR(vtos(GetFields(rs1,_T("Attachment"))) + _T(";") + (vtoi(GetFields(rs,_T("CLnum"))) ==1 ? _T("") : vtos(GetFields(rs,_T("CLnum"))) + _T("x")) + vtos(GetFields(rs,_T("CLgg")))));
						rs1->put_Collect((_variant_t)_T("Attachmentweight"),_variant_t(vtof(GetFields(rs1,_T("Attachmentweight")))+vtof(GetFields(rs,_T("CLzz")))));
					}else{
						rs1->put_Collect((_variant_t)_T("Attachment"),STR_VAR((vtoi(GetFields(rs,_T("CLnum"))) ==1 ? _T("") : vtos(GetFields(rs,_T("CLnum"))) + _T("x")) + vtos(GetFields(rs,_T("CLgg")))));
						rs1->put_Collect((_variant_t)_T("Attachmentweight"),_variant_t(vtof(GetFields(rs,_T("CLzz")))));
						//是第一个，设置标志。
						b1st = true;
					}
					rs1->Update();
					rs->MoveNext();
				}
				
			}
			//复位标志
			b1st = false;
			rs->Close();
      
			//现在处理根部型钢
			SQLx=CString( _T("SELECT * FROM [")) + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" AND zdjh=") + ltos(zdjh) + _T(" AND IsSAPart=-1 AND ClassID<>") + ltos(iBolts) + _T(" AND ClassID<>") + ltos(iNuts) + _T(" AND ClassID<>") + ltos(iAttached);
			rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
				adOpenDynamic, adLockOptimistic, adCmdText); 
			if(!( rs->BOF && rs->adoEOF || (vtoi(rsza->GetCollect(_T("bNotSumSA")))!=0) ))
			{							
				//只选择根部材料需要统计的支吊架
				while( !rs->adoEOF)
				{
					if( b1st )
					{
						if( vtoi(GetFields(rs,_T("CLnum"))) ==1  )
							rs1->put_Collect((_variant_t)_T("SAssAndLen"),STR_VAR(vtos(GetFields(rs1,_T("SAssAndLen"))) + _T(";") +vtos(GetFields(rs,_T("CLgg"))) + _T(",L=") +vtos(GetFields(rs,_T("L1")))));
						else
							rs1->put_Collect((_variant_t)_T("SAssAndLen"), STR_VAR(vtos(GetFields(rs1,_T("SAssAndLen"))) + _T(";") +vtos(GetFields(rs,_T("CLnum"))) + _T("x(") +vtos(GetFields(rs,_T("CLgg"))) + _T(",L=") +vtos(GetFields(rs,_T("L1"))) + _T(")")));
						
					}
					else
					{
						if( vtoi(GetFields(rs,_T("CLnum"))) ==1  )
							rs1->put_Collect((_variant_t)_T("SAssAndLen"),STR_VAR(vtos(GetFields(rs,_T("CLgg"))) + _T(",L=") +vtos(GetFields(rs,_T("L1")))));
						else
							rs1->put_Collect((_variant_t)_T("SAssAndLen"),STR_VAR(vtos(GetFields(rs,_T("CLnum"))) + _T("x(") +vtos(GetFields(rs,_T("CLgg"))) + _T(",L=") +vtos(GetFields(rs,_T("L1"))) + _T(")")));
						
						//是第一个，设置标志。
						b1st = true;
					}
					rs1->Update();
					rs->MoveNext();
				}
				//由西安华瑞提出,双根部在支吊架一览表中没有乘2,原来是表示单个根部的材料.
				if(modPHScal::glNumSA==2)
				{
					rs1->put_Collect((_variant_t)_T("SAssAndLen"),STR_VAR(_T("2x( ")+vtos(GetFields(rs1,_T("SAssAndLen")))+_T(" )")));
					rs1->Update();
				}
			}
			//复位标志
			b1st = false;
			rs->Close();
      
			//现在处理根部整体
			SQLx=CString( _T("SELECT * FROM [")) + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" AND zdjh=") + ltos(zdjh) + _T(" AND Index=") + ltos(iSA) + _T(" AND ClassID<>") + ltos(iGCement);
			rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
				adOpenDynamic, adLockOptimistic, adCmdText); 
			if( !rs->BOF && !rs->adoEOF ){
				
				while( !rs->adoEOF){
					rs1->put_Collect((_variant_t)_T("XBL1"),_variant_t(vtof(GetFields(rs,_T("XBL1")))));//悬臂长度，跟根部绑定 Add by luorijin 2008.10.30
					if( b1st ){
						rs1->put_Collect((_variant_t)_T("SAbh"), STR_VAR((vtos(GetFields(rs1,_T("SAbh"))) + _T(";") + (vtoi(GetFields(rs,_T("CLnum"))) ==1 ? _T("") : vtos(GetFields(rs,_T("CLnum"))) + _T("x")) + vtos(GetFields(rs,_T("CLgg"))))));
						rs1->put_Collect((_variant_t)_T("SAweight"), _variant_t(vtof(GetFields(rs1,_T("SAweight"))) +vtof(GetFields(rs,_T("CLzz")))));
					}else{
						rs1->put_Collect((_variant_t)_T("SAbh"),STR_VAR(((vtoi(GetFields(rs,_T("CLnum"))) ==1 ? _T("") : vtos(GetFields(rs,_T("CLnum"))) + _T("x")) + vtos(GetFields(rs,_T("CLgg"))))));
						rs1->put_Collect((_variant_t)_T("SAweight"),_variant_t(vtof(GetFields(rs,_T("CLzz")))));
						//是第一个，设置标志。
						b1st = true;
					}
					rs1->Update();
					rs->MoveNext();
				}
				
			}
			//复位标志
			b1st = false;
			rs->Close();
			rs1->Close();
			
		}
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}
}


void modPHScal::CreateTmpIDCustomIDTable()
{
   //目的:在dbPRJ中建立ID-CustomID对照表PictureClipData;
	//同时在dbPRJ中建立与选择规范相对应的模板表PhsStructureName/REF,
	//这两个Name/REF表结构与sort.mdb中的一样，只是过滤掉了不合适当前标准(管部/根部)的模板。
	//这样的目的是以一次性的长时间，大大改善frmPhsSamp对话框模板过滤的速度。
	
	//过程可能费时较长
   //Screen.MousePointer = vbHourglass
	FILE* fp = fopen("CreateTmpIDCustomIDTable.txt", "w");

	AfxGetApp()->BeginWaitCursor();
	try
	{
		//建立本地PictureClipData表,它由多个厂家提供的ID-CustomID表组成
		//Dim rs As Recordset, tdf As TableDef, FD As Field, bLineSPR As Boolean
		//Dim i As Integer, j As Integer
		//首先判别PictureClipData表是否设置给Cavphs->rsID
		//首先判别表是否已经打开。如果打开，则先关闭对象Cavphs->rsID
		if (Cavphs->rsID->State == adStateOpen)
			Cavphs->rsID->Close();
   
		_variant_t tmpvar;
		if (EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("PictureClipData")))
		{
			fprintf(fp, "%s:%d\n", __FILE__, __LINE__);
			EDIBgbl::dbPRJ->Execute((_bstr_t)_T("DELETE * FROM PictureClipData"), NULL, adCmdText);
			EDIBgbl::SQLx = CString(_T("INSERT INTO PictureClipData IN \'")) + EDIBgbl::GetDBName(EDIBgbl::dbPRJ) + 
				_T("\' SELECT * FROM [") + tbnPAID + _T("]");
			dbZDJcrude->Execute((_bstr_t)EDIBgbl::SQLx, NULL, adCmdText);
		}
		else
		{
			fprintf(fp, "%s:%d\n", __FILE__, __LINE__);
			EDIBgbl::SQLx = CString(_T("SELECT * INTO PictureClipData IN \'")) + EDIBgbl::GetDBName(EDIBgbl::dbPRJ) + 
				_T("\' FROM [") + tbnPAID + _T("]");
			dbZDJcrude->Execute((_bstr_t)EDIBgbl::SQLx, NULL, adCmdText);
		}
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}

	try
	{
		fprintf(fp, "%s:%d\n", __FILE__, __LINE__);
	   EDIBgbl::SQLx = CString(_T("INSERT INTO PictureClipData IN \'")) + EDIBgbl::GetDBName(EDIBgbl::dbPRJ) + 
		   _T("\' SELECT * FROM [") + tbnPARTID + _T("]");
	   dbZDJcrude->Execute((_bstr_t) EDIBgbl::SQLx, NULL, adCmdText);

	   fprintf(fp, "%s:%d\n", __FILE__, __LINE__);
		EDIBgbl::SQLx = CString(_T("INSERT INTO PictureClipData IN \'")) + EDIBgbl::GetDBName(EDIBgbl::dbPRJ) + 
		   _T("\' SELECT * FROM [") + tbnSAID + _T("]");
	   dbZDJcrude->Execute((_bstr_t)EDIBgbl::SQLx, NULL, adCmdText);

	   fprintf(fp, "%s:%d\n", __FILE__, __LINE__);
	   EDIBgbl::SQLx = CString(_T("INSERT INTO PictureClipData IN \'")) + EDIBgbl::GetDBName(EDIBgbl::dbPRJ) + 
		   _T("\' SELECT * FROM [") + tbnAttachmentID + _T("]");
	   dbZDJcrude->Execute((_bstr_t)EDIBgbl::SQLx, NULL, adCmdText );
	   //MsgBox dbPRJ.RecordsAffected
			fprintf(fp, "%s:%d\n", __FILE__, __LINE__);
	   EDIBgbl::SQLx = CString(_T("INSERT INTO PictureClipData IN \'") )+ EDIBgbl::GetDBName(EDIBgbl::dbPRJ) + 
		   _T("\' SELECT * FROM [") + tbnBoltsNutsID + _T("]");
	   dbZDJcrude->Execute((_bstr_t)EDIBgbl::SQLx, NULL, adCmdText );
		
	   //弹性件的ID表字段可能多两个InstallHeightOfUP,InstallHeightOfDown
	   fprintf(fp, "%s:%d\n", __FILE__, __LINE__);
	   EDIBgbl::SQLx = CString(_T("INSERT INTO PictureClipData IN \'")) + EDIBgbl::GetDBName(EDIBgbl::dbPRJ) + 
		   _T("\' SELECT Index,SEQ,ID,CustomID,ClassID,BmpName,Description,ChineseDescription,ParalleledNum,BHFormat FROM [") + tbnHDid + _T("]");
	   dbZDJcrude->Execute((_bstr_t)EDIBgbl::SQLx, NULL, adCmdText );
	   fprintf(fp, "%s:%d\n", __FILE__, __LINE__);
	   EDIBgbl::SQLx = CString(_T("INSERT INTO PictureClipData IN \'")) + EDIBgbl::GetDBName(EDIBgbl::dbPRJ)+ 
		   _T("\' SELECT Index,SEQ,ID,CustomID,ClassID,BmpName,Description,ChineseDescription,ParalleledNum,BHFormat FROM [") + tbnSPRINGid + _T("]");
	   dbZDJcrude->Execute((_bstr_t)EDIBgbl::SQLx, NULL, adCmdText );
	   fprintf(fp, "%s:%d\n", __FILE__, __LINE__);
	   EDIBgbl::SQLx = CString(_T("INSERT INTO PictureClipData IN \'")) + EDIBgbl::GetDBName(EDIBgbl::dbPRJ) + 
		   _T("\' SELECT * FROM [") + tbnSectionSteelID + _T("]");
	   dbZDJcrude->Execute((_bstr_t)EDIBgbl::SQLx, NULL, adCmdText );   
	
		//打开一个ID-CustomID对照表快照，是否可以加快运行速度。
	   fprintf(fp, "%s:%d\n", __FILE__, __LINE__);
		EDIBgbl::SQLx = _T("SELECT * FROM PictureClipData");
		if(Cavphs->rsID->State)
			Cavphs->rsID->Close();
		Cavphs->rsID->Open((_bstr_t)EDIBgbl::SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
			adOpenDynamic, adLockOptimistic, adCmdText); 
		Cavphs->brsIDStatus=TRUE;
		//此后一般应该检查ID和修改模板名称
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}
	fclose(fp);
   AfxGetApp()->EndWaitCursor();
}

void modPHScal::CreateTmpConnectTable()
{
   //目的：建立本地connect表,它由connectPASA/connectCSPR/connectSPR三个表联合组成。这样每个用户的配置不会互相干扰。
   //Dim rs As Recordset, FD As Field, bLineSPR As Boolean
   //Dim i As Integer, j As Integer
   //On Error Resume Next
   //过程时间可能较长
	AfxGetApp()->BeginWaitCursor();
	_variant_t tmpvar;
	if(EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("connect")))
	{
		//如果Cphs.rsConnect对象打开，关闭它。以便删除。
		Cavphs->CloseRecordsets();
		try 
		{
			EDIBgbl::dbPRJ->Execute((_bstr_t) _T("DROP TABLE [connect]"), NULL, adCmdText);
		} catch (...)
		{
		}
	}
    //20071018(start) "dbSORT" 改为 "dbPHScode"
	EDIBgbl::SQLx = _T("SELECT * INTO [connect] FROM connectPASA IN \'");
		EDIBgbl::SQLx += EDIBgbl::GetDBName(EDIBgbl::dbPHScode);
		EDIBgbl::SQLx += _T("\'");
	EDIBgbl::dbPRJ->Execute((_bstr_t)EDIBgbl::SQLx, NULL, adCmdText);
   EDIBgbl::SQLx = _T("INSERT INTO [connect] SELECT * FROM connectCSPR IN \'");
		EDIBgbl::SQLx += EDIBgbl::GetDBName(EDIBgbl::dbPHScode);
		EDIBgbl::SQLx += _T("\'");
	EDIBgbl::dbPRJ->Execute((_bstr_t)EDIBgbl::SQLx, NULL, adCmdText);
   EDIBgbl::SQLx = _T("INSERT INTO [connect] SELECT * FROM connectSPR IN \'");
		EDIBgbl::SQLx += EDIBgbl::GetDBName(EDIBgbl::dbPHScode);
		EDIBgbl::SQLx += _T("\'");

   EDIBgbl::dbPRJ->Execute((_bstr_t)EDIBgbl::SQLx, NULL, adCmdText);
	Cavphs->InitListRs(); 
   AfxGetApp()->EndWaitCursor();//20071018(end) "dbSORT" 改为 "dbPHScode"
}

void modPHScal::CreateTmpSPRPropertyTable(int /*Optional*/ SprMaxSerialNum)
{
//目的:建立弹簧荷载位移特性表
//处理:线性弹簧,根据其特性表生成一个与非线性弹簧结构相同的表,
//     以便对线性和非线性弹簧使用同一个查找弹簧型号算法
//输入:SprMaxSerialNum-总的允许最大串联弹簧片数，圆柱形弹簧一般=4，蝶簧可能=5
	try
	{
			FieldPtr sFD;
			CString sTD1;

			_RecordsetPtr rs;
			rs.CreateInstance(__uuidof(Recordset));
			_RecordsetPtr rs1;
			rs1.CreateInstance(__uuidof(Recordset));

			bool bLineSPR=false;//默认为碟簧(非线性弹簧)
			bool bFinish =false;//写完碟簧某个规格号的载荷位移特性数据
			LONG i=0 , j =0;
			CString SQLx=_T("");
			_variant_t v;
   
		   //过程可能较长
		   AfxGetApp()->BeginWaitCursor();
   
		   SQLx = _T("SELECT * FROM [") + tbnSPRINGproperty + _T("]");
		   rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)dbZDJcrude,true), 
			   adOpenDynamic, adLockOptimistic, adCmdText); 

		   rs1->Open((_bstr_t)_T("select * from tmpSPRProperty"), _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
			   adOpenDynamic, adLockOptimistic, adCmdText); 

		   //根据字段内容确定是否是线性弹簧特性表
		   //线性弹簧有刚度字段G
		   
		   int fc=rs->Fields->Count; 
		   for (i=0;i<fc;i++)
		   {
			   rs->Fields->get_Item((_variant_t)i, &sFD);
			   CString strName;
			   strName = (LPTSTR)sFD->Name;
			   if(strName ==_T("G") || strName ==_T("g"))
			   {
				   bLineSPR=true;
				   break;
			   }
		   }


		   //临时弹簧特性表建立在dbprj中(本地),加快访问速度
			if( EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("tmpSPRProperty")))		   
				EDIBgbl::dbPRJ->Execute((_bstr_t)_T("DELETE * FROM tmpSPRProperty"), NULL, adCmdText);
			else
				EDIBgbl::dbPRJ->Execute((_bstr_t)_T("CREATE TABLE tmpSPRProperty (SerialNum SHORT,seq SHORT,Dist SINGLE,Pgz SINGLE,Pgzmax SINGLE,PreCmprs SINGLE,Hopmax SINGLE)"), NULL, adCmdText);

			long n=0,m=0;//记录计数
   
		   if( bLineSPR) 
			{
				while (!rs->adoEOF)
				{
					//先取出值，记录计数
					rs->get_Collect((_variant_t)_T("HopMax"),&v);
					long Hopmax=vtof(v);
					n=n+(Hopmax+1)*SprMaxSerialNum;
					rs->MoveNext();
				}
		   }

		   if( bLineSPR) 
			{
			   frmStatus.m_Label1 = GetResStr(IDS_CreatingSPRpropertyTable);
			   frmStatus.m_Label2 = _T("");
			   frmStatus.SetWindowText(GetResStr(IDS_InitializeAutoPHS));
			   frmStatus.UpdateData(false);
			   frmStatus.ShowWindow(SW_SHOW);
			   rs->MoveFirst();
				while (!rs->adoEOF)
				{
					//先取出值，加快速度
					rs->get_Collect((_variant_t)_T("HopMax"),&v);
					long Hopmax=vtof(v);
					rs->get_Collect((_variant_t)_T("PreCmprs"),&v);
					float PreCmprs=vtof(v);
					rs->get_Collect((_variant_t)_T("G"),&v);
					float G=vtof(v);
					rs->get_Collect((_variant_t)_T("SEQ"),&v);
					CString SEQ=vtos(v);
					long SEQ2 = vtoi(v);
					 //从最小的规格号开始
					 //从最小的串联数j=1开始

//					EXECUTE_TIME_BEGIN
					
					 for ( j = 1;j<= SprMaxSerialNum;j++)
					 {
						//从最小的位移i=0开始
						 
						for (i = 0;i<= Hopmax;i++)
						{
							rs1->AddNew();
							rs1->put_Collect((_variant_t)_T("SerialNum"), (_variant_t)j);
							rs1->put_Collect((_variant_t)_T("seq"), (_variant_t)(SEQ2));
							rs1->put_Collect((_variant_t)_T("Dist"), (_variant_t)(i * j));
							rs1->put_Collect((_variant_t)_T("Pgz"), (_variant_t)((i +PreCmprs) * G));
							rs1->put_Collect((_variant_t)_T("Pgzmax"), (_variant_t)((Hopmax + PreCmprs) *G));
							rs1->put_Collect((_variant_t)_T("Hopmax"), (_variant_t)(Hopmax * j));
							rs1->put_Collect((_variant_t)_T("PreCmprs"), (_variant_t)(PreCmprs * j));
							rs1->Update();
/*
							SQLx=_T("INSERT INTO tmpSPRProperty (SerialNum,seq,Dist,Pgz,Pgzmax,Hopmax,PreCmprs) VALUES (") + ltos(j) + _T(",") + SEQ +  _T(",") + ltos(i * j) + _T(",") + ftos((i +PreCmprs) * G) +  _T(",") + ftos((Hopmax + PreCmprs) *G) + _T(",") + ftos(Hopmax * j )+ _T(",") + ftos(PreCmprs * j) + _T(")");
							EDIBgbl::dbPRJ->Execute((_bstr_t)SQLx);
*/
							m++;
							frmStatus.UpdateStatus((float)m /(float) n,true);
							frmStatus.UpdateWindow();
						}
					 }

//					EXECUTE_TIME_END

					 rs->MoveNext();
				}
				frmStatus.ShowWindow(SW_HIDE);
			}
			else
			{
			  //非线性弹簧特性表的生成，依赖于厂家产品样本的格式，此处针对江阴石化设备厂的蝶簧样本格式编制
				SQLx = _T("INSERT INTO tmpSPRProperty IN \'") + EDIBgbl::GetDBName(EDIBgbl::dbPRJ) + _T("\' SELECT SerialNum,seq,Dist,Pgz,Pgzmax,Hopmax,PreCmprs FROM [") + tbnSPRINGproperty + _T("]");
				dbZDJcrude->Execute((_bstr_t)SQLx, NULL, adCmdText);
		   }
		   
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}
	AfxGetApp()->EndWaitCursor();
}

void modPHScal::CreateTmpSPRPropertyTableUnfinish(int /*Optional*/ SprMaxSerialNum)
{
}

void modPHScal::SetSpringPhsInfo(_RecordsetPtr rsTmpSelSpring, _RecordsetPtr rsTZA, bool  bLineSPR,CString& mZdjType)
{
 //目的：设置原始数据表中有关弹簧支吊架的计算结果数据
	try
	{
	int iSumSerialNum,signOfPaz0;
	float mvHaz;
	float sngInstallLoad,sngOpLoad;
	CString sTmp;
   if(rsTmpSelSpring==NULL || rsTZA==NULL)
	{
		sTmp=GetResStr(IDS_NullRsOrRss);
		throw sTmp;
	}
	_variant_t v1,v2,v3;
	rsTmpSelSpring->get_Collect((_variant_t)_T("SerialNum"), &v1);
	rsTmpSelSpring->get_Collect((_variant_t)_T("Installload"),&v2);
	rsTmpSelSpring->get_Collect((_variant_t)_T("Opload"),&v3);
      iSumSerialNum = vtoi(v1);
      sngInstallLoad = vtof(v2);
      sngOpLoad = vtof(v3);
      //弹簧支吊类型格式
	  sTmp.Format(_T("%d"),iSumSerialNum);
	  mZdjType=sTmp;
	  sTmp.Format(_T("%d"),gintParallelNum);
	  mZdjType+=sTmp;
	  rsTmpSelSpring->get_Collect((_variant_t)_T("SEQ"),&v1);
	  sTmp.Format(_T("%02d"),vtoi(v1));
	  mZdjType+=sTmp;
      //《火力发电厂汽水管道设计技术规定(电力部,DL/T5054-1996)》p66
      //第7.4.5.1条热态吊零管道
      //(1)工作高度Hop=弹簧自由高度H0-弹簧工作位移Dop
      //(2)安装高度Her=工作高度Hop-热位移yr
      //(3)弹簧绝对压缩值Habs=弹簧安装压缩值Haz+弹簧预压缩值Hprecmprs=弹簧工作位移Dop
      //故推论，
      //弹簧安装压缩值Haz=弹簧工作位移Dop-弹簧预压缩值Hprecmprs
      //见sub AutoSelSpringNo
	  rsTmpSelSpring->get_Collect((_variant_t)_T("OpDist"),&v1);
      mvHaz =vtof(v1) + yr;
      
      
      rsTZA->PutCollect(_T("type"),_variant_t( mZdjType));
      sTmp.Format(_T("%d"),mvHaz);
	  rsTZA->PutCollect(_T("haz"),_variant_t(mvHaz));
	  _variant_t tmpvar;
	  int Sgn=1;
     //支吊架的安装荷载=单个弹簧的安装荷载*弹簧并联数
	 if(gbHotStatus2Zero)
	 {
		if(pgz<0)
		  Sgn=-1;
		else if(pgz==0)
		  Sgn=0;
		if(vtoi(rsTZA->GetCollect(_T("paz0")))==0)
			rsTZA->PutCollect(_T("paz0"),_variant_t((float)(Sgn * sngInstallLoad * gintParallelNum * (Unit_Force == _T("kgf") ? 1 : EDIBgbl::kgf2N))));
		rsTZA->PutCollect(_T("paz1"),_variant_t((float)(Sgn * sngInstallLoad * gintParallelNum * (Unit_Force == _T("kgf") ? 1 : EDIBgbl::kgf2N))));
		signOfPaz0 = Sgn;
	 }
	 else
	 {
		if(paz<0)
		  Sgn=-1;
		else if(paz==0)
		  Sgn=0;
		if(vtoi(rsTZA->GetCollect(_T("paz0")))==0)
			rsTZA->PutCollect(_T("paz0"),_variant_t((float)(Sgn * sngInstallLoad * gintParallelNum * (Unit_Force == _T("kgf") ? 1 : EDIBgbl::kgf2N))));
		rsTZA->PutCollect(_T("pgz1"),_variant_t((float)(Sgn * sngOpLoad * gintParallelNum * (Unit_Force == _T("kgf") ? 1 : EDIBgbl::kgf2N))));
		signOfPaz0 = Sgn;
	 }

	  rsTmpSelSpring->get_Collect((_variant_t)_T("CoefOfVarLoad"),&v1);
      rsTZA->PutCollect(_T("CoefOfVarLoad"),_variant_t(vtof(v1)));
      //rsTZA->Update();
      CString SQLx;
	  sTmp.Format(_T("%d"),_ttoi(mZdjType.Mid(2,2)));
      SQLx = _T("SELECT * FROM [") + tbnSPRINGproperty + _T("] WHERE seq=") + sTmp;
	  _RecordsetPtr rs1;
	  rs1.CreateInstance(__uuidof(Recordset));
	  rs1->Open((_bstr_t)SQLx, _variant_t((IDispatch*)dbZDJcrude,true), 
		  adOpenStatic, adLockOptimistic, adCmdText); 
      if(rs1->BOF && rs1->adoEOF)
         FrmTxsr.m_pViewTxsr->m_PagItem->m_SPRrigid=_T("");
      else
	  {
         if( bLineSPR )
		 {
            rs1->MoveLast();
			rs1->get_Collect((_variant_t)_T("G"),&v1);
			FrmTxsr.m_pViewTxsr->m_PagItem->m_SPRrigid.Format(_T("%d%s%.2fkgf/mm"),iSumSerialNum,mZdjType.Mid(2,2)+GetResStr(IDS_MsgBox_60684),vtof(v1)/iSumSerialNum);
         }
		else
			 FrmTxsr.m_pViewTxsr->m_PagItem->m_SPRrigid=_T("");
	  }
	  FrmTxsr.m_pViewTxsr->UpdateData(false);
	  FrmTxsr.m_pViewTxsr->m_PagItem->UpdateData(false);
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}
}

void modPHScal::AutoSelSpringNo(_RecordsetPtr rss,float fpgz,float fpaz,float fyr,float fSPR_CHZBH,	float& fTotalInstallCompressValue,
								long lSelSampleID,int nRepZdjType,int iSumDistOfSpring,bool BHintRepZdjType,CString strSpringOrder,
								CString strZdjType,bool bAutoApplyChzbh,CString strbnSPRINGproperty,CString strbnDiscSPRINGpropertyMaxDist)
{
   //目的:计算并且选择线性/非线性弹簧号
   //输入：rss--当前工程当前卷册的支吊架原始数据记录集
   //输出：弹簧编号(包括串联数)，支吊架类型，弹簧质量，
   //方法：选择的弹簧信息存入PrjDir目录下workprj.mdb库中tmpSelectedSpring表，对其按荷载变化系数、重量排序。符合要求的荷载变化系数且重量最小的弹簧入选。
	try{
	   /*if(!ModEncrypt::gbLegalUser)
		{
			CTime tm=CTime::GetCurrentTime();
			if(tm.GetSecond()% 8 !=0)
			{
				//非法用户，只有1/3的时间可以计算
				ShowMessage(GetResStr(IDS_DemoVersionOutDate));
				return;
			}
		}*/
	   CString strErrDesc;

	   //保存原来的弹簧类型。
	   CString strOldZdjType;

		LONG i=0 ,j=0;
		float deltaZ1 =0, Pmax=0 , Tmpyr=0 , TmpPgz=0 , TmpPaz=0 , mvCoefOfVarLoad=0 , mvHaz=0 ;
//		int iSumSerialNum;
		float paz0=0, mvF=0 ;
		CString FD;
		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(Recordset));
		_RecordsetPtr rs1;
		rs1.CreateInstance(__uuidof(Recordset));
		_RecordsetPtr rs2;
		rs2.CreateInstance(__uuidof(Recordset));
		_RecordsetPtr rsw;
		rsw.CreateInstance(__uuidof(Recordset));
		//FD As Field, rs As Recordset, rs1 As Recordset, rs2 As Recordset, rsw As Recordset
		bool bLineSPR =false;//默认为碟簧
		float mvDn=0 , mvFn=0 , mvD =0, mvSN =0, mvCoef=0 , mvChzbh=0 ;
		float mvMaxDistActual=0; //弹（碟）簧实际最大位移,mm
		float mvMaxDistTheoretical=0; //弹（碟）理论最大位移,mm
		bool bFoundSPR=0;
		float sngWeight=0,sngHeight=0,sngDifPSAinstallLoad=0;
	   CString sTmpSPRPROPERTY = _T("tmpSPRProperty");
	   CString sTmpSelectedSpring = _T("tmpSelectedSpring");
	   CString SQLx,sTmp,sTmp2;
	   //初始化所有（一路）弹簧总的安装压缩值之和
	   fTotalInstallCompressValue = 0;

		strOldZdjType=vtos(rss->GetCollect(_T("psaTYPE")));
	   //保证sTmpSelectedSpring存在
	   if( EDIBgbl::tdfExists(EDIBgbl::dbPRJ, sTmpSelectedSpring) )
		{
			SQLx = _T("DROP TABLE [") + sTmpSelectedSpring + _T("]");
		  EDIBgbl::dbPRJ->Execute((_bstr_t)SQLx, NULL, adCmdText);
	   }
	   //拷贝SelectedSpring表结构从sort.mdb中到workprj.mdb
	   if( ! EDIBgbl::tdfExists(EDIBgbl::dbPRJ, sTmpSelectedSpring.Mid(3)) )
		{
		  //MsgBox ResolveResString(iUE_NotExistsSelectedSpringTableInWorkPrjMdb)
		  //Err.Raise iUE_NotExistsSelectedSpringTableInWorkPrjMdb, , ResolveResString(iUE_NotExistsSelectedSpringTableInWorkPrjMdb, _T("|1"), ProjectDir)
		  SQLx = _T("SELECT * INTO [") + sTmpSelectedSpring.Mid(3) + _T("] IN \'") + EDIBgbl::GetDBName(EDIBgbl::dbPRJ) + _T("\' FROM [") + sTmpSelectedSpring.Mid(3) + _T("]");
		  EDIBgbl::dbPHScode->Execute((_bstr_t)SQLx, NULL, adCmdText);//20071101 "dbSORT" 改为 "dbPHScode"
	   }
	   SQLx = _T("DELETE * FROM [") + sTmpSelectedSpring.Mid(3) + _T("]");
	   EDIBgbl::dbPRJ->Execute((_bstr_t)SQLx, NULL, adCmdText);
	   SQLx = _T("SELECT * INTO [") + sTmpSelectedSpring + _T("] FROM [") + sTmpSelectedSpring.Mid(3) + _T("]");
	   EDIBgbl::dbPRJ->Execute((_bstr_t)SQLx, NULL, adCmdText);
	   //荷载变化系数mvChzbh应该尽量与应力分析程序计算时一致,为此,先获得此值(如果有的话)。
	   //这是AutoPHS41的特色算法之一，我们推测，这将尽量减少弹簧代换对管系应力分析造成的偏差。
	   if( bAutoApplyChzbh )
	   {
		  if( ! IsNull(rss->GetCollect(_T("Paz0"))) && vtof(rss->GetCollect(_T("Paz0"))) != 0 )
		  {
			 mvChzbh = fabs(1 - fabs(vtof(rss->GetCollect(_T("Paz0"))) / (fpgz*gintParallelNum)));
			 paz0 = vtof(rss->GetCollect(_T("paz0"))) * (Unit_Force == _T("kgf") ? 1 : 1 / EDIBgbl::kgf2N);
		  }
		  else
		  {
			 mvChzbh = fSPR_CHZBH;
			 paz0 = 0;
		  }
	   }
	   else
	   {
		  //自适应荷载变化开关关闭时
		  mvChzbh = fSPR_CHZBH;
	   }
   
	   //判断是否为线性弹簧?
	   SQLx = _T("SELECT * FROM [") + strbnSPRINGproperty + _T("]");
	   rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)dbZDJcrude,true), 
		   adOpenStatic, adLockOptimistic, adCmdText); 
		int CF=rs->Fields->Count;
		FieldPtr fldInfo;
		for(i=0;i<CF;i++)
		{
			rs->Fields->get_Item((_variant_t)i, &fldInfo);
			CString strName;
			strName = (LPTSTR)fldInfo->Name;
			if(strName==_T("G"))
			{
			 //线性弹簧有刚度字段_T("G")
				bLineSPR=true;
				break;
			}
		}
		rs->Close();
	   i = Cavphs->GetPhsStructFromSampleID(lSelSampleID);
		//dn=Cavphs->PA;
	   dn = vtos(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("dn1")));
	   //i = CheckFieldifNull(rss)
	   //Calculate paralleling spring numbers
	   CalSpringParallelNum(dn); //计算并联数
	   //MsgBox iParallelNum
	   //弹簧工作荷载=fpgz/iParallelNum
	   
// 	   if(gbHotStatus2Zero)
// 	   {
// 		   TmpPgz = fpgz / iParallelNum;
// 		   TmpPaz = fpaz / iParallelNum;
		   TmpPgz = fpgz ;
		   TmpPaz = fpaz ;
		   TmpPgz = fabs(TmpPgz);
		   TmpPaz = fabs(TmpPaz);
// 	   }
// 	   else
// 	   {
// // 		   //TmpPaz = fpaz / iParallelNum;
// 		   TmpPaz = fpaz ;
// // 		   //保存冷态吊零的工作荷载
// // 		   //TmpPgz = fpgz / iParallelNum;
//  		   TmpPgz =fpgz ;
// 		   TmpPgz = fabs(TmpPgz);
// 		   TmpPaz = fabs(TmpPaz);
// 	   }
	   //if( rss(_T("ifLongVertPipe")) ){
		  //按规程,长垂直管道上部的刚性吊架，荷载按全部承受考虑;
		  //因为是弹簧，故要注释掉
		  //TmpPgz = TmpPgz * 2
	   //}
	   
	   Tmpyr = fyr;           //fyr 单位:mm
   
	   //开始弹簧选择
		  //线性/非线性弹簧选择
		  //选择工作荷载满足要求的各种弹簧号，按号排序，这将生成从Ns-Ne的一个弹簧号表
	   if(gbHotStatus2Zero)
	   {
		   //热态吊零，从工作荷载开始查
			sTmp.Format(_T("%g"),TmpPgz);
	   }
	   else
	   {
		   //冷态吊零，从安装荷载开始查
		   sTmp.Format(_T("%g"),TmpPaz);
	   }
		  SQLx = _T("SELECT DISTINCT seq FROM [") + sTmpSPRPROPERTY + _T("] WHERE Pgz>=") + sTmp + _T(" ORDER BY seq");
		  //没有必要作如下判断和限制条件！因为在随后的查表过程中可以判别出是否最小荷载超过工作荷载或安装荷载。
		  //if( bLineSPR ){
			 //SQLx = _T("SELECT seq FROM [") + TBNSPRINGproperty + _T("] WHERE Pgzmin<=") + TmpPgz + _T(" AND Pgzmax>=") + TmpPgz + _T(" ORDER BY seq")
		  //}else{
			 //SQLx = _T("SELECT seq FROM [") + strbnDiscSPRINGpropertyMaxDist + _T("] WHERE Pgzmin<=") + TmpPgz + _T(" AND Pgzmax>=") + TmpPgz + _T(" ORDER BY seq")
		  //}
		  rs1->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
			  adOpenStatic, adLockOptimistic, adCmdText); 
		  if( rs1->BOF && rs1->adoEOF ){
			 //找不到符合条件的弹簧号
			  sTmp.Format(GetResStr(IDS_NotFoundDiscSPRNoInsTmpSPRPROPERTY),SQLx);
			  throw sTmp;
		  }
		  else
		  {//9
			 //从所有工作荷载满足条件的弹簧中逐个选择，先从最小号开始
			 //该号弹簧的当前串联片数为j,
			 //例如弹簧号12，当前串联片数为3，弹簧型号就是312
				   
				   CString seq,Sj;
			 while(!rs1->BOF)
			 {//8
				//确定碟簧串联片数,江阴石化弹吊厂样本为从1到5
				for( j = 1 ;j<= giAllowedMaxSerialNumSPRs;j++)
				{//7
				   //通过增加串联数，使该号弹簧满足位移及荷载变化系数要求
					_variant_t v,v2;
					rs1->get_Collect((_variant_t)_T("seq"),&v);
					seq.Format(_T("%d"),vtoi(v));
					Sj.Format(_T("%d"),j);
				   //该表只需要前向滚动，以减少资源消耗。注意该表在ZdjCrude.mdb中
					if( bLineSPR )//取最大理论位移(start)
					{//1 //是否是线性弹簧(start)
					  //线性弹簧，查线性弹簧表
					  SQLx = _T("SELECT * FROM [") + strbnSPRINGproperty + _T("] WHERE seq=") + seq + _T(" ORDER BY Seq");
					}//1
					else
					{//1
					  //非线性弹簧，查非线性弹簧表
					  SQLx = _T("SELECT * FROM [") + strbnDiscSPRINGpropertyMaxDist + _T("] WHERE seq=") + seq;// + _T(" ORDER BY Seq,SerialNum,Dist");
					}//1 //是否是线性弹簧(end)
				   if(rs2->State == adStateOpen)
					   rs2->Close();
				   rs2->Open((_bstr_t)SQLx, _variant_t((IDispatch*)dbZDJcrude,true), 
					   adOpenStatic, adLockOptimistic, adCmdText); 
				   if(rs2->adoEOF && rs2->BOF)
				   {//1
					  //MsgBox iUE_NotFoundDiscSPRNoInsTmpSPRPROPERTY
					  //Exit Sub
					   sTmp.Format(GetResStr(IDS_NotFoundDiscSPRNoInsTmpSPRPROPERTY),SQLx);
					  throw sTmp;
					   //Err.Raise iUE_NotFoundDiscSPRNoInsTmpSPRPROPERTY, , ResolveResString(iUE_NotFoundDiscSPRNoInsTmpSPRPROPERTY, _T("|1"), SQLx)
				   }//1
				   else
				   {//1					  
					  //rs2.MoveFirst //因为是前向滚动表，不能执行这种操作
					   rs2->get_Collect((_variant_t)(_T("Theoretical")+Sj),&v);
					   rs2->get_Collect((_variant_t)Sj,&v2);
					  if(v.vt==VT_NULL || v2.vt==VT_NULL)
						  break;
					  //最大理论位移
					  mvMaxDistTheoretical = vtof(v);
				   }//1//取最大理论位移(end)
				   //选择同一号弹簧的荷载位移表，按位移值排序。该表的SerialNum字段记录该号弹簧的当前串联数
				   SQLx = _T("SELECT * FROM [") + sTmpSPRPROPERTY + _T("] WHERE seq=") + seq + _T(" AND SerialNum=") + Sj + _T(" AND NOT ISNULL(pgz) ORDER BY Seq,SerialNum,Dist");
				   if(rs->State == adStateOpen)
					   rs->Close();
				   rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
					   adOpenStatic, adLockOptimistic, adCmdText); 
					if(!rs->adoEOF && !rs->BOF)
				   {//6
					  rs->MoveLast();
					  //最后一个记录的位移值最大，这是当前串联数时弹簧实际工作位移范围的上界
					  rs->get_Collect((_variant_t)_T("dist"),&v);
					  mvMaxDistActual = vtof(v);
					  //如果当前弹簧最大工作位移满足需要的热位移值
						rs2->get_Collect((_variant_t)Sj,&v);
					  if( vtof(v) >= fabs(fyr) )
					  {//5
						  //找出工作荷载满足要求时，当前弹簧的位移值
						  if(gbHotStatus2Zero)
							  sTmp.Format(_T("%g"),TmpPgz);
						  else
							  sTmp.Format(_T("%g"),TmpPaz);					 
						  _variant_t vTmp;
						  rs->Find((_bstr_t)(_T("Pgz>=") +sTmp), 0, adSearchForward);
						  if( !rs->adoEOF)
						  {//4
							//找到啦，需要插值法确定工作荷载处的弹簧位移
							//记录当前位移值mvDn，它对应于当前rs(_T("seq"))号弹簧串联rs(_T("SerialNum"))片时的位移值
							rs->get_Collect((_variant_t)_T("dist"),&v);
							mvDn =vtof(v); 
							//记录当前荷载值
							rs->get_Collect((_variant_t)_T("pgz"),&v);
							mvFn =vtof(v);
							if( mvDn > 0 )
							{//1
							   //工作荷载处的弹簧位移>0,则当前记录
							   //不是第一个记录,可以移动到前一个记录,以便插值
							   rs->MovePrevious();
							   rs->get_Collect((_variant_t)_T("dist"),&v);
							   rs->get_Collect((_variant_t)_T("pgz"),&v2);
							   if(gbHotStatus2Zero)
								   mvD = (mvDn - vtof(v)) / (mvFn - vtof(v2)) * (TmpPgz - vtof(v2)) + vtof(v);
							   else
								   mvD = (mvDn - vtof(v)) / (mvFn - vtof(v2)) * (TmpPaz - vtof(v2)) + vtof(v);
							}//1
							else
							{//1
							   //工作荷载处的弹簧位移正好是0,说明是第一个记录
							   mvD = mvDn;
							}//1
							if(gbHotStatus2Zero && ((fyr > 0 && (mvD + fyr) > mvMaxDistActual) || (fyr < 0 && (mvD + fyr) < 0) ) || !gbHotStatus2Zero && ((fyr < 0 && (mvD - fyr) > mvMaxDistActual) || (fyr > 0 && (mvD - fyr) < 0) ) )
							{//1
								////热态吊零时:
							   //查表时向上的热位移是正值,往下查,大于最大值;
							   //向下的热位移是负值,往上查,小于最小值0;
							   //说明此时热位移太大，超出该号弹簧工作位移允许范围
							   //MsgBox rs.Fields(_T("seq")) + _T(",dist=") +GetFields(rs,_T("dist"))
								////冷态吊零时:
							   //查表时向下的热位移是负值,往下查,大于最大值;
							   //向上的热位移是正值,往上查,小于最小值0;
							   //说明此时热位移太大，超出该号弹簧工作位移允许范围
							   //MsgBox rs.Fields(_T("seq")) + _T(",dist=") +GetFields(rs,_T("dist"))
							}//1
							else
							{//3
							   //热位移在弹簧工作范围之内,
							   //按热位移查出安装荷载
								if(gbHotStatus2Zero)
									sTmp.Format(_T("%g"),(mvD + fyr));
								else
									sTmp.Format(_T("%g"),(mvD - fyr));
                           
								rs->Find((_bstr_t)(_T("Dist>=") +sTmp), 0, adSearchForward);
								if( !rs->adoEOF)
								{//2
								  //需要插值法确定安装荷载
								   rs->get_Collect((_variant_t)_T("dist"),&v);
									mvDn =vtof(v); 
									rs->get_Collect((_variant_t)_T("pgz"),&v);
									mvFn =vtof(v);
									if( mvDn > 0 )
									{//1
									 //不是第一个记录,可以移动到前一个记录,以便插值
										rs->MovePrevious();
										rs->get_Collect((_variant_t)_T("dist"),&v);
										rs->get_Collect((_variant_t)_T("pgz"),&v2);
										if(gbHotStatus2Zero)
											mvF = (mvFn - vtof(v2)) / (mvDn - vtof(v)) * (mvD + fyr - vtof(v)) + vtof(v2);
										else
											mvF = (mvFn - vtof(v2)) / (mvDn - vtof(v)) * (mvD - fyr - vtof(v)) + vtof(v2);

									}//1
									else
									{//1
									 //工作荷载处的弹簧位移正好是0,说明是第一个记录
										mvF = mvFn;
									}//1
									//弹簧荷载变化系数mvCoefOfVarLoad
									if(gbHotStatus2Zero)
										mvCoefOfVarLoad = fabs(1 - mvF / TmpPgz);
									else
										mvCoefOfVarLoad = fabs(1 - mvF / TmpPaz);
								  //弹簧(安装)绝对压缩值=弹簧安装压缩值+弹簧预压缩值
								  //弹簧工作压缩值=mvD
								  //弹簧安装压缩值mvDaz=mvD+fyr=mvHaz
								  //mvD是在当前串联数=j，以当前预压缩值作为0位移，而计算的当前弹簧的工作（相对）压缩值。
								if(gbHotStatus2Zero)
									mvHaz = mvD + fyr;
								else
									mvHaz = mvD - fyr;
								  //mvHaz = mvD / j + fyr
								  //Debug.Print rs.Fields(_T("seq")), mvHaz, mvF
								  //弹簧重量
									rs->get_Collect((_variant_t)_T("seq"),&v);
									sTmp.Format(_T("%2d"),vtoi(v));
								  sngWeight = sngSpringWeight(Sj + _T("1") + sTmp, rss, 1, sngHeight);
								  //下句偶尔产生INSERT INTO语句错误
				        		  //sngWeight = sngSpringWeightNormal(j + _T("1") + Format(rs(_T("seq")), _T("00")), rss, 1, sngHeight)
								  //MsgBox sngHeight
								  if( paz0 == 0 )
								  {//1
									 sngDifPSAinstallLoad = 1;
								  }//1
								  else
								  {//1
									 sngDifPSAinstallLoad = fabs(1 - fabs(mvF / paz0));
								  }//1
								  //存入SelectedSpring表以便结束时选择弹簧
								  short nSEQ;//弹簧号
								  rs->get_Collect((_variant_t)_T("seq"),&v);
								  nSEQ=vtoi(v);

								  float mPreCmprs;//预压缩值,mm
								  rs->get_Collect((_variant_t)_T("PreCmprs"),&v);
								  mPreCmprs=vtof(v);
								  								  								  
								  float mOpDist;//工作位移,mm
								  if(gbHotStatus2Zero)
									  mOpDist=mvD;
								  else
									  mOpDist=mvHaz;
								  
								  float mHaz;//安装位移,mm
								  if(gbHotStatus2Zero)
									  mHaz=mvHaz;
								  else
									  mHaz=mvD;

								  float mDifToMidLine;//工作状态偏离中线的值,一个<0.5的非负数
 								  rs2->get_Collect((_variant_t)Sj,&v);
								  mDifToMidLine=fabs(0.5 - mOpDist/vtof(v));
								 
								 float mOpLoad;//工作荷载,kgf
								 if(gbHotStatus2Zero)
									mOpLoad=TmpPgz;
								 else
									mOpLoad=mvF;

								 float mInstallLoad;//安装荷载,kgf
								 if(gbHotStatus2Zero)
									mInstallLoad=mvF;
								 else
									mInstallLoad=TmpPaz;
								 
								 //应力计算荷载paz0
								
								  SQLx.Format("INSERT INTO [%s] (SerialNum,SEQ,SpringTypeNo,CoefOfVarLoad,PreCmprs,MaxDist,        OpDist,Haz,DifToMidLine,   Weight,Height,difPSAinstallLoad,        OpLoad,InstallLoad,paz0)\
									                     VALUES (%s,      %d,  \'%s%02d\',        %g,            %g,      %g,      %g,   %g,  %g,             %g,     %g,    %g,                      %g,    %g,         %g)",\
									         sTmpSelectedSpring, Sj,   nSEQ,    Sj,nSEQ,   mvCoefOfVarLoad,mPreCmprs,mvMaxDistActual,mOpDist,mHaz,mDifToMidLine,sngWeight,sngHeight,sngDifPSAinstallLoad,mOpLoad,mInstallLoad,paz0);
								EDIBgbl::dbPRJ->Execute((_bstr_t)SQLx, NULL, adCmdText);
							   }//2
							   else
							   {//1
								  //没找到TmpPgz匹配值
							   }//1
							}//3
						 }//4
					  }//5
				   }//6
				}//7
				rs1->MoveNext();
			 }//8
		  }//9
   
		  rs->Close();
		  rs2->Close();
		  //开始按照选定的弹簧选择规则从结果表中过滤需要的弹簧
		   SQLx = _T("SELECT * FROM [") + sTmpSelectedSpring + _T("]  WHERE CoefOfVarLoad<=");
		   sTmp.Format(_T("%g"),mvChzbh);
		   SQLx+= sTmp + _T(" ") + gstrSpringOrderSQL;
		   rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
			   adOpenStatic, adLockOptimistic, adCmdText); 
		   if( rs->BOF && rs->adoEOF )
		   {//是恒力弹簧(start)
			  //选择不到需要的弹簧，就选择恒力弹簧
			  //MsgBox ResolveResString(iUE_OverAllowedMaxSerialNumSPRs)
			  CalSpringParallelNum();
			  AutoSelConstantSpringNo(rss,fpgz,fyr,gbCSPRneedSpecialDesign,gUseConstantSpringMinSurplusDist,lSelSampleID,
									strZdjType,giWholeSprNum,gbAutoPASA,giSumSerialNum,tbnHDproperty,tbnDisplacementSerial);
			  bFoundSPR = false;
		   }//是恒力弹簧(end)
		   else
		   {//2	//不是恒力弹簧(start)
			  //if( strSpringOrder = _T("手工") ){
			  if( strSpringOrder == GetResStr(IDS_ManualSelSpring) )
			  {//1
			  }//1
			  else
			  {//1
				 //计算弹簧荷载变化系数<=允许的弹簧荷载变化系数,则输出结果
				  _variant_t v;
				 rs->MoveFirst();
				 if(strOldZdjType!="")
				 {
					FindPSAType(strOldZdjType, rs);//pfg20051101 应力计算的支吊架类型是否存在
				 }
				 rs->get_Collect((_variant_t)_T("MaxDist"),&v);
				 iSumDistOfSpring = vtoi(v);//最大总位移，用于编号中。
				 //设置弹簧计算结果信息
				 SetSpringPhsInfo(rs, rss, bLineSPR,strZdjType);
				 rs->get_Collect((_variant_t)_T("OpDist"),&v);
				if(gbHotStatus2Zero)
					fTotalInstallCompressValue = vtof(v) + fyr;
				else
					fTotalInstallCompressValue = vtof(v) + fyr;
				 sngSpringWeightNormal(strZdjType, rss, fTotalInstallCompressValue);
				 //tmp = sngSpringWeight(strZdjType, rss, fTotalInstallCompressValue)
			  }//1
		   }//2	//不是恒力弹簧(end)

		 // nRepZdjType=-1：无选择；nRepZdjType=1：是； nRepZdjType=2：否
		 if(BHintRepZdjType && (nRepZdjType==-1 || nRepZdjType==2) && strOldZdjType!=_T("") && strOldZdjType!=strZdjType && strOldZdjType.GetLength()==4)
		 {
			 if(nRepZdjType==-1)
			 {
					//只有当新老弹簧都可以选择到时，才会出现第一次提示
					//AutoPHS计算的弹簧与应力分析程序GLIF计算的结果不一致，且按照应力分析程序计算的结果
					rs->Close();
				   SQLx = _T("SELECT * FROM [") + sTmpSelectedSpring + _T("]  WHERE CoefOfVarLoad<=");
				   sTmp.Format(_T("%g"),mvChzbh);
				   SQLx+= sTmp + _T(" AND SerialNum=") + strOldZdjType.Left(1) + _T(" AND SEQ=") + strOldZdjType.Right(2);
				   rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
					   adOpenStatic, adLockOptimistic, adCmdText); 
				   //ShowMessage(rs.GetRecordCount());
				   if( rs->BOF && rs->adoEOF )
				   {
					  //选择不到需要的GLIF计算弹簧，再选择AutoPHS计算弹簧
						rs->Close();
						SQLx = _T("SELECT * FROM [") + sTmpSelectedSpring + _T("]  WHERE CoefOfVarLoad<=");
						sTmp.Format(_T("%g"),mvChzbh);
						SQLx+= sTmp + _T(" AND SerialNum=") + strZdjType.Left(1) + _T(" AND SEQ=") + strZdjType.Right(2);
						rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
							adOpenStatic, adLockOptimistic, adCmdText); 
						//ShowMessage(rs.GetRecordCount());
						if( rs->BOF && rs->adoEOF )
						{
							//选择不到需要的GLIF计算的弹簧，也选择不到AutoPHS计算的弹簧	
						  //只能选择恒力弹簧
						  //MsgBox ResolveResString(iUE_OverAllowedMaxSerialNumSPRs)
						  CalSpringParallelNum();
						  AutoSelConstantSpringNo(rss,fpgz/gintParallelNum,fyr,gbCSPRneedSpecialDesign,gUseConstantSpringMinSurplusDist,lSelSampleID,
												strZdjType,giWholeSprNum,gbAutoPASA,giSumSerialNum,tbnHDproperty,tbnDisplacementSerial);
						  bFoundSPR = false;
						}
						else
						{
						  //if( strSpringOrder = _T("手工") ){
						  if( strSpringOrder == GetResStr(IDS_ManualSelSpring) )
						  {
						  }
						  else
						  {
								//只能选择AutoPHS计算的弹簧
								nRepZdjType=1;
								//前面已经选择了一次
						  }
						}
				   }
				   else
				   {
					  //可以选择到需要的GLIF计算弹簧，再看是否能选择到AutoPHS计算弹簧
						rs->Close();
						SQLx = _T("SELECT * FROM [") + sTmpSelectedSpring + _T("]  WHERE CoefOfVarLoad<=");
						sTmp.Format(_T("%g"),mvChzbh);
						SQLx+= sTmp + _T(" AND SerialNum=") + strZdjType.Left(1) + _T(" AND SEQ=") + strZdjType.Right(2);
						rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
							adOpenStatic, adLockOptimistic, adCmdText); 
						//ShowMessage(rs.GetRecordCount());
						if( rs->BOF && rs->adoEOF )
						{
							//选择到需要的GLIF计算弹簧，选择不到AutoPHS计算弹簧	
						  //只能选择GLIF计算弹簧,执行代换
							nRepZdjType=2;
						}
						else
						{
						  //if( strSpringOrder = _T("手工") ){
						  if( strSpringOrder == GetResStr(IDS_ManualSelSpring) )
						  {
						  }
						  else
						  {
								//选择到需要的GLIF计算弹簧，也选择到AutoPHS计算弹簧
							  //发出提示
								CString sMessage;
								sMessage.Format(GetResStr(IDS_YESNOCALSPR),strZdjType,strOldZdjType);
								if(ShowMessage(sMessage,MB_YESNO) == IDNO)
									nRepZdjType=2;
								else
									nRepZdjType=1;
						  }
						}
				   }
			 }
			 if(nRepZdjType==2)
			 {
					//AutoPHS计算的弹簧与应力分析程序GLIF计算的结果不一致，且按照应力分析程序计算的结果
					strZdjType=strOldZdjType;
					rs->Close();
				   SQLx = _T("SELECT * FROM [") + sTmpSelectedSpring + _T("]  WHERE CoefOfVarLoad<=");
				   sTmp.Format(_T("%g"),mvChzbh);
				   SQLx+= sTmp + _T(" AND SerialNum=") + strOldZdjType.Left(1) + _T(" AND SEQ=") + strOldZdjType.Right(2);
				   rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
					   adOpenStatic, adLockOptimistic, adCmdText); 
				   //ShowMessage(rs.GetRecordCount());
				   if( rs->BOF && rs->adoEOF )
				   {
					  //选择不到需要的弹簧，就选择恒力弹簧
					  //MsgBox ResolveResString(iUE_OverAllowedMaxSerialNumSPRs)
					 CalSpringParallelNum();
					 AutoSelConstantSpringNo(rss,fpgz/gintParallelNum,fyr,gbCSPRneedSpecialDesign,gUseConstantSpringMinSurplusDist,lSelSampleID,
											strZdjType,giWholeSprNum,gbAutoPASA,giSumSerialNum,tbnHDproperty,tbnDisplacementSerial);
					  bFoundSPR = false;
				   }
				   else
				   {
					  //if( strSpringOrder = _T("手工") ){
					  if( strSpringOrder == GetResStr(IDS_ManualSelSpring) )
					  {
					  }
					  else
					  {
							//计算弹簧荷载变化系数<=允许的弹簧荷载变化系数,则输出结果
							_variant_t v;
							rs->MoveFirst();
							rs->get_Collect((_variant_t)_T("MaxDist"),&v);
							gSumDistOfSpring = vtoi(v);//最大总位移，用于编号中。
							//设置弹簧计算结果信息
							SetSpringPhsInfo(rs, rss, bLineSPR,strZdjType);
							rs->get_Collect((_variant_t)_T("OpDist"),&v);
							if(gbHotStatus2Zero)
								fTotalInstallCompressValue = vtof(v) + fyr;
							else
								fTotalInstallCompressValue = vtof(v) + fyr;
							sngSpringWeightNormal(strZdjType, rss, fTotalInstallCompressValue);
							//tmp = sngSpringWeight(strZdjType, rss, fTotalInstallCompressValue)
					  }
				   }
			 }
		 }
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}
	FrmTxsr.m_pViewTxsr->UpdateData(false);
}

void modPHScal::AutoSelConstantSpringNo(_RecordsetPtr rss,float fpgz,float fyr,bool bCSPRneedSpecialDesign,float fUseConstantSpringMinSurplusDist,long lSelSampleID,
										CString strZdjType,int& iWholeSprNum,bool bAutoPASA,int nSumSerialNum,CString strbnHDproperty,CString strbnDisplacementSerial)
{	
	try
	{
		int i=0;
		long iSumSerialNum=0;
		float Tmpyr=0.0, TmpPgz=0.0, tmpCapacity=0.0;
		float Cmin=0 , Cmax=0 ;  //第一个/最后一个规格的载荷容量
		float C1=0, C2=0;
		CString sTmp1,sTmp2,sTmp,SQLx;
		_variant_t vTmp;
		float tmpDist=0 ;
		bool bFoundCSPR=false;

		_ConnectionPtr db;
		db.CreateInstance(__uuidof(Connection));
		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(Recordset));
		_RecordsetPtr rs1;
		rs1.CreateInstance(__uuidof(Recordset));
		long lngMaxSerialCSPR=0;

		//设置恒吊需要特殊设计标志为假
		bCSPRneedSpecialDesign = false;
		 CalSpringParallelNum();  //计算并联数			 
		//弹簧工作荷载=fpgz/gintParallelNum
		//If rss(_T("ifLongVertPipe")) Then
		//按规程,长垂直管道上部的刚性吊架，荷载按全部承受考虑;
		//因为是弹簧，故要注释掉

		//TmpPgz = fabs(fpgz) / gintParallelNum;
		TmpPgz = fabs(fpgz) ;
		Tmpyr = fyr ;          //fyr 单位:mm
		if (fabs(Tmpyr) * gUseConstantSpringPercentSurplusDist < fUseConstantSpringMinSurplusDist )
			Tmpyr = fabs(Tmpyr) + fUseConstantSpringMinSurplusDist;
		else
			Tmpyr = fabs(Tmpyr) * (1 + gUseConstantSpringPercentSurplusDist);
		
		//使用循环来确定需要的恒力弹簧串联数,最多两个串联
		lSelSampleID = vtoi(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("iSelSampleID")));

		//下面的函数返回了弹簧的CustomID到
		lngMaxSerialCSPR = Cavphs->GetPhsIsCSPR(iCSPR, lSelSampleID);
		//显示提醒文本框
		if( lngMaxSerialCSPR == 0 )
		{
			sTmp1.Format(_T("%d"),lSelSampleID);
			sTmp2.Format( GetResStr(IDS_PleaseSelectCSPRSampleID), sTmp1);
			FrmTxsr.m_pViewTxsr->m_LabelMsgbox.SetWindowText(sTmp2);
			FrmTxsr.m_pViewTxsr->m_LabelMsgbox.ShowWindow(SW_SHOW);

			//开始自动选择恒力吊架
			strZdjType = _T("9999");
			iWholeSprNum = 1;
			bAutoPASA = false;

			lSelSampleID = Cavphs->GetphsStructIDsemiAuto();
			if( lSelSampleID == -1)
			{
				strZdjType = _T("9999");
				iWholeSprNum = 2;
				bAutoPASA = false;
				lSelSampleID = Cavphs->GetphsStructIDsemiAuto();
				if( lSelSampleID ==-1)
				{
					strZdjType = _T("9999");
					iWholeSprNum = 1;
					bAutoPASA = true;
					lSelSampleID = Cavphs->GetphsStructIDsemiAuto();
					if( lSelSampleID == -1 )
					{
						strZdjType = _T("9999");
						iWholeSprNum = 2;
						bAutoPASA = true;
						lSelSampleID = Cavphs->GetphsStructIDsemiAuto();
						if( lSelSampleID == -1)
						//始终选择不到合适的恒吊
							return;
					}
					else
						lngMaxSerialCSPR = Cavphs->GetPhsIsCSPR(iCSPR, lSelSampleID);
				}
				else
					lngMaxSerialCSPR = Cavphs->GetPhsIsCSPR(iCSPR, lSelSampleID);
			}
			else
				lngMaxSerialCSPR = Cavphs->GetPhsIsCSPR(iCSPR, lSelSampleID);
		}
		else
		{
			FrmTxsr.m_pViewTxsr->m_LabelMsgbox.SetWindowText(_T(""));
			lngMaxSerialCSPR = Cavphs->GetPhsIsCSPR(iCSPR, lSelSampleID);
		}

		SQLx = _T("SELECT * FROM [") + strbnHDproperty + _T("] ORDER BY Capacity");
		rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)dbZDJcrude,true), 
			adOpenStatic, adLockOptimistic, adCmdText); 
		if( rs->BOF && rs->adoEOF)
		{
			//恒吊载荷容量表为空，不可能
			sTmp1.Format(GetResStr(IDS_NullTableInXMdb),EDIBgbl::GetDBName(dbZDJcrude), strbnHDproperty);
			throw sTmp1;
		}
		rs->get_Collect((_variant_t)_T("Capacity"),&vTmp);
		Cmin =vtof(vTmp);
		rs->MoveLast();
		rs->get_Collect((_variant_t)_T("Capacity"),&vTmp);
		Cmax =vtof(vTmp);
		rs->MoveFirst();
		for( iSumSerialNum =1 ;iSumSerialNum<= lngMaxSerialCSPR;iSumSerialNum++)
		{
			tmpDist = Tmpyr / iSumSerialNum;
			sTmp1.Format(_T("%d"),(long)tmpDist);
			CString m_strFilter=CString(_T(" DisplacementSerial>=")) + sTmp1;
			SQLx = _T("SELECT * FROM [") + strbnDisplacementSerial + _T("]") + _T(" WHERE ") + m_strFilter + _T(" ORDER BY DisplacementSerial");
			rs1->Open((_bstr_t)SQLx, _variant_t((IDispatch*)dbZDJcrude,true), 
				adOpenStatic, adLockOptimistic, adCmdText); 
			if( rs1->BOF && rs1->adoEOF);
			//位移超出系列最大值
			else
			{
				rs1->get_Collect((_variant_t)_T("DisplacementSerial"),&vTmp);
				tmpDist =vtof(vTmp);
				//载荷容量单位为kN*mm,即N*m,位移单位为mm,荷载单位为kgf(或daN=10N)
				tmpCapacity = TmpPgz * tmpDist * EDIBgbl::kgf2N / 1000;
				if( tmpCapacity < Cmin)
				{
				//荷载容量比最小的还小
					sTmp1.Format(GetResStr(IDS_CapacityLitterCmin),_T("\%g"),_T("\%g"));
					sTmp2.Format(sTmp1,tmpCapacity, Cmin);
					throw sTmp2;
				}
				else
				{
					if( tmpCapacity > Cmax);
					//荷载容量比最大的还大
					//串联数加1，继续循环
					else
					{
					//荷载容量合适
						sTmp1.Format(_T("%g"),tmpCapacity);
						_variant_t vTmp;
						rs->Find((_bstr_t)(_T("Capacity>=")+sTmp1), 0, adSearchForward);
						if( !rs->adoEOF)
						{
							//找到了
							rs->get_Collect((_variant_t)_T("minDist"),&vTmp);
							if (tmpDist < vtof(vTmp))
							{
								//比最小位移小，应该用弹簧
								sTmp1.Format(GetResStr(IDS_ShouldUseSpring),_T("\%g"),_T("\%g"));
								sTmp2.Format(sTmp1,tmpDist,vtof(vTmp));
								throw sTmp2;
							}
							rs->get_Collect((_variant_t)_T("maxDist"),&vTmp);
							if( tmpDist > vtof(vTmp)) 
							{
								//比最大位移还大
								if( iSumSerialNum >= lngMaxSerialCSPR)
								{
									//超过模板中的串联数
									//If gbCSPRCanSpecialDesign Then
									//恒吊需要特殊设计（即恒吊不允许串联使用）,
									//只要一个恒吊就可以了
									//就用这种位移作为订货依据
									bCSPRneedSpecialDesign = true;
									bFoundCSPR = true;
									break;
								}
							}
							//恒吊不需要特殊设计（即恒吊允许串联使用）
							//继续循环
							//小于模板中的串联数
							//继续循环		
							else
							{
								//合适
								//这位移可以作为订货依据
								bFoundCSPR = true;
								break;
							}
						}
					}
				}
			}
		}

		if(!bFoundCSPR)
		{
			FrmTxsr.m_pViewTxsr->m_PagItem->m_ConstantSPRNo= _T("");
			//串联数达到最大值，还没有合适的恒力弹簧
			throw GetResStr(IDS_NotFoundConstantSPRInTBNHDproperty);
		}
		else
		{
			//找到合适的恒吊
			//按就近原则选择恒吊
			//记录当前载荷容量
			rs->get_Collect((_variant_t)_T("Capacity"),&vTmp);
			C1 = vtof(vTmp);

			//记录前一个载荷容量
			rs->MovePrevious();
			rs->get_Collect((_variant_t)_T("Capacity"),&vTmp);
			C2 = vtof(vTmp);
			if( fabs(C1 - tmpCapacity) >= fabs(C2 - tmpCapacity))
				;
				//载荷容量更接近C2
			else
				//载荷容量更接近C1
				//回到容量接近的记录
				rs->MoveNext();

			strZdjType = _T("9999");
			rs->get_Collect((_variant_t)_T("DH"),&vTmp);
			sTmp.Format(_T("%d%d%2d"),iSumSerialNum,gintParallelNum,vtoi(vTmp));
            //sTmp.Format(_T("%d%d"),iSumSerialNum,vtoi(vTmp)) ;
			rss->PutCollect(_T("type"),_variant_t(_T("9999")));
			rss->PutCollect(_T("ConstantSPRNo"),_variant_t(sTmp));
			//给恒吊赋值
			iWholeSprNum = iSumSerialNum;
			//对于恒力弹簧，串联数=串联组数
			nSumSerialNum = iSumSerialNum;
			rs->get_Collect((_variant_t)_T("DH"),&vTmp);
			for(i = 0;i<iSumSerialNum;i++)
			{

				sSprInfo[i].SerialNum = 1;
				sSprInfo[i].DH =vtoi(vTmp);
				sSprInfo[i].haz = fyr / iSumSerialNum * sSprInfo[i].SerialNum;
				sSprInfo[i].HeatDisp = sSprInfo[i].haz;
				sSprInfo[i].CheckDisp = 0;

			}
			FrmTxsr.m_pViewTxsr->m_PagItem->m_ConstantSPRNo.Format(GetResStr(IDS_ConstantSpringTypeIsXXXX),vtos((_variant_t)rss->GetCollect(_T("ConstantSPRNo"))));

		}
		
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}
}

void modPHScal::SetPhsTK3()
{
//目的:将图框设置为A3图框.
//返回:所有的信息设置完成
	try
	{
		EDIBDB::DrawID = EDIBAcad::GetDrawIDAndName(zdjh + glDrawNoStartNumber - 1, EDIBDB::DrawName);
		if( EDIBDB::PrjName == CString(_T("")) || EDIBDB::DrawName == CString(_T("")) || EDIBDB::DrawID == CString(_T("")) || EDIBDB::DwgScale == CString(_T("")) )
		  EDIBDB::DrawInfoGet();
		
		EDIBDB::GetTBsize();

		EDIBDB::PrjName = EDIBgbl::SelPrjName;

		if(gbDrawNoPreMethod)
		  EDIBDB::DrawID = EDIBAcad::GetDrawIDAndName(zdjh + glDrawNoStartNumber - 1, EDIBDB::DrawName);
		else
		  EDIBDB::DrawID = EDIBAcad::GetDrawIDAndName(zdjh + glDrawNoStartNumber - 1, EDIBDB::DrawName);
		
		if( EDIBDB::DwgScale = 0)
			EDIBDB::DwgScale = 100;
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}
}

void modPHScal::CalSpringParallelNum(CString& PipeA)
{
	//'目的:计算弹簧并联数
//返回:gintParallelNum=弹簧并联数
  
	_RecordsetPtr rstbl=::FrmTxsr.m_pViewTxsr->m_ActiveRs;
	try{
		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(Recordset));
		_variant_t v;
		_variant_t tmpvar;
		if(PipeA==_T(""))
		{
			tmpvar=rstbl->GetCollect(_variant_t(_T("dn1")));
			if(tmpvar.vt==VT_NULL)
				PipeA=dn;
			else
			{
				PipeA=vtos(tmpvar);
				PipeA.TrimLeft();PipeA.TrimRight();
				if(PipeA.GetLength()<=0)
					PipeA=dn;
			}
		}
	   //计算管部标高和根部标高，用于setBasepoint过程确定图幅大小
		sngPEL = rstbl->GetCollect(_variant_t(_T("dh1"))).vt==VT_NULL ? 0 : (float)rstbl->GetCollect(_variant_t(_T("dh1")));
	   sngSEL =rstbl->GetCollect(_variant_t(_T("gh1"))).vt==VT_NULL ? 0 : (float)rstbl->GetCollect(_variant_t(_T("gh1")));

		  if (PipeA != _T(""))
		  {
			  CString SQLx;
			  SQLx = _T("SELECT * FROM PictureClipData WHERE (CustomID)=\'") + PipeA + _T("\'");
			  rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
				  adOpenStatic, adLockOptimistic, adCmdText); 
			 if (rs->BOF && rs->adoEOF)
			 {
				//Err.Raise iUE_NoRecordFLDEqvThisValueInPictureClipData, , ResolveResString(iUE_NoRecordFLDEqvThisValueInPictureClipData, _T("|1"), _T("CustomID"), _T("|2"), PipeA)
				 if(rstbl->GetCollect(_variant_t(_T("ParallelingNum"))).vt==VT_NULL)
				   gintParallelNum = 1;
				else
				{
				   if ((float)rstbl->GetCollect(_variant_t(_T("ParallelingNum"))) == 0)
					  gintParallelNum = 1;
				   else
					{
					  gintParallelNum =(long)rstbl->GetCollect(_variant_t(_T("ParallelingNum")));
						if (gintParallelNum>2) gintParallelNum=2;
					}
				}
			 }
			 else
			 {
				 rs->get_Collect((_variant_t)_T("ParalleledNum"),&v);
				if(v.vt==VT_NULL ||vtoi(v)<=0)
					gintParallelNum = 1;
				else
					  gintParallelNum = vtoi(v);
			 }
			 rs->Close();
		  }
		  else
		  {
			gintParallelNum = 1;
		  }

	   //更新字段值
			rstbl->PutCollect(_variant_t(_T("ParallelingNum")),_variant_t((long)gintParallelNum));
	}
	catch(_com_error &e)
	{
		gintParallelNum = 1;
		ShowMessage(e.Description());
	}
}

void modPHScal::PreCalCrudeData(_RecordsetPtr rstbl,int nth)
{
//*******************************
// 功  能: 处理原始数据,获得初始值
// 输  入: 原始数据
// 输  出: 处理后的原始数据
//*******************************
	if(rstbl==NULL || rstbl->State==adStateClosed)
		return;

	//2007.09.03
	float fTmpPgz=0.0;
	float fTmpPaz=0.0;

   _RecordsetPtr rs;
   rs.CreateInstance(__uuidof(Recordset));
  bool bTmp=false;
  long lngErrNum=0;
  _variant_t tmpvar;
  _variant_t v;
  CString sTmp,sTmp2,SQLx,tbn;
  float UTmp=0.0;
  try{
	   //不可加在此处，这会造成始终停留在第1个记录处。
	  modPHScal::zdjh=vtoi(rstbl->GetCollect(_T("zdjh")));

	  //2007.09.03
	  fTmpPaz=vtof(rstbl->GetCollect(_T("paz1")));
	  fTmpPgz=vtof(rstbl->GetCollect(_T("pgz1")));

	   if(!IsNull(rstbl->GetCollect(_T("type"))) )
	   { 
		   modPHScal::ZdjType = Trim(vtos(rstbl->GetCollect(_T("type"))));
	   }
		if( vtoi(rstbl->GetCollect(_T("UPxyz"))) < 1 || vtoi(rstbl->GetCollect(_T("UPxyz"))) >3 )
		{
		  rstbl->PutCollect(_T("UPxyz"),_variant_t((long)2));
	   }
		giUPxyz=vtoi(rstbl->GetCollect(_T("UPxyz")));
	   if( IsNull(rstbl->GetCollect(_T("dj1"))) || rstbl->GetCollect(_T("dj1")).fltVal <= 0 )
	   {
		  rstbl->PutCollect(_T("dj1"),_variant_t((float)76));
	   }
	   modPHScal::dj = rstbl->GetCollect(_T("dj1")).fltVal;
	   //矩形管道垂直方向管径B
	   if( IsNull(rstbl->GetCollect(_T("dwB"))) || rstbl->GetCollect(_T("dwB")).fltVal <= 0 )
	   {
		  rstbl->PutCollect(_T("dwB"),_variant_t((float)800));
	   }
	   modPHScal::dwB = rstbl->GetCollect(_T("dwB")).fltVal;
	   //dxa1/dza1负值允许，表示定位尺寸标注时要在管部右侧。
	   if( IsNull(rstbl->GetCollect(_T("t01"))) )
	   {
		  rstbl->PutCollect(_T("t01"),_variant_t((float)0));
	   }
	   modPHScal::t0 = rstbl->GetCollect(_T("t01")).fltVal;
	   if( IsNull(rstbl->GetCollect(_T("dxa1"))) )
	   {
		  //设置x方向定位默认值
		 rstbl->PutCollect(_T("dxa1"),_variant_t((float)3200));
	   }
	   //取绝对值，使其标注为尺寸时显示为正值
	   modPHScal::dxa0 = rstbl->GetCollect(_T("dxa1")).fltVal;
	   //使dxa获得初值
	   modPHScal::dxr = dxa0;
	   if( IsNull(rstbl->GetCollect(_T("dza1"))) )
	   {
		  //设置z方向定位默认值
		   rstbl->PutCollect(_T("dza1"),_variant_t((float)4500));
	   }
	   //取绝对值，使其标注为尺寸时显示为正值
	   modPHScal::dza0 = rstbl->GetCollect(_T("dza1")).fltVal;
	   //使dza获得初值
	   modPHScal::dzr = dza0;
	   if( IsNull(rstbl->GetCollect(_T("dh1"))) )
	   {
		  //设置管部标高默认值
		  rstbl->PutCollect(_T("dh1"),_variant_t((float)8));
	   }
	   modPHScal::sngPEL = rstbl->GetCollect(_T("dh1")).fltVal;
	   if( IsNull(rstbl->GetCollect(_T("iCSnum"))) )
	   {
		  modPHScal::iCSnum = 0;
	   }
	   else
	   {
		  modPHScal::iCSnum = vtoi(rstbl->GetCollect(_T("iCSnum")));
	   }
	   if( IsNull(rstbl->GetCollect  (_T("gdw1"))) || rstbl->GetCollect(_T("gdw1")).fltVal <= 0 )
	   {
		  rstbl->PutCollect(_T("gdw1"),_variant_t((float)0));
	   }
	   modPHScal::gdw1 = rstbl->GetCollect(_T("gdw1")).fltVal;  //根部L值
	   //0/1=根部共用槽钢/根部不共用槽钢
	   if( IsNull(rstbl->GetCollect(_T("Gnum"))) || vtoi(rstbl->GetCollect(_T("Gnum")) )<= 0 )
	   {
		  rstbl->PutCollect(_T("Gnum"),_variant_t((short)1));
	   }
	   modPHScal::glNumSA = vtoi(rstbl->GetCollect(_T("Gnum")));
   
	   //单位制
	   modPHScal::CalUnitX(rstbl);
   
	   //确定管部方向
	   modPHScal::CalfxPA (rstbl);
	   //确定根部方向
	   modPHScal::CalfxSA (rstbl);
	   if( IsNull(rstbl->GetCollect(_T("Num"))) || vtoi(rstbl->GetCollect(_T("Num"))) <= 0 )
	   {
		  rstbl->PutCollect(_T("Num"),_variant_t((short)1));
	   }
	   if( IsNull(rstbl->GetCollect(_T("gh1"))) )
	   {
		  rstbl->PutCollect(_T("gh1"),_variant_t((float)12.6));
	   }
	   modPHScal::sngSEL = rstbl->GetCollect(_T("gh1")).fltVal;  //根部标高
	   if( IsNull(rstbl->GetCollect(_T("haz"))) || rstbl->GetCollect(_T("haz")).fltVal <= 0 )
	   {
		  rstbl->PutCollect(_T("haz"),_variant_t((long)0));
	   }
	   if( IsNull(rstbl->GetCollect(_T("xl1"))) )
	   {
		  rstbl->PutCollect(_T("xl1"),_variant_t((float)0));
	   }
	   modPHScal::xl = rstbl->GetCollect(_T("xl1")).fltVal;
	   if( IsNull(rstbl->GetCollect(_T("yl1"))) )
	   {
		  rstbl->PutCollect(_T("yl1"),_variant_t((float)0));
	   }
	   modPHScal::yl = rstbl->GetCollect(_T("yl1")).fltVal;
	   if( IsNull(rstbl->GetCollect(_T("zl1"))) )
	   {
		  rstbl->PutCollect(_T("zl1"),_variant_t((float) 0));
	   }
	   modPHScal::zl = rstbl->GetCollect(_T("zl1")).fltVal;
	   if( IsNull(rstbl->GetCollect(_T("xr1"))) )
	   {
		  rstbl->PutCollect(_T("xr1"),_variant_t((float)0));
	   }
	   modPHScal::xr = rstbl->GetCollect(_T("xr1")).fltVal;
	   if( IsNull(rstbl->GetCollect(_T("yr1"))) )
	   {
		  rstbl->PutCollect(_T("yr1"),_variant_t((float) 0));
	   }
	   modPHScal::yr1 = rstbl->GetCollect(_T("yr1")).fltVal;
	   //垂直方向热位移
	   if( giUPxyz == 1 )
	   {
		  modPHScal::yr = rstbl->GetCollect(_T("xr1")).fltVal;
	   }
	   else if( giUPxyz == 2 )
	   {
		  modPHScal::yr = rstbl->GetCollect(_T("yr1")).fltVal;
	   }
	   else if( giUPxyz == 3 )
	   {
		  modPHScal::yr = rstbl->GetCollect(_T("zr1")).fltVal;
	   }
	   if( IsNull(rstbl->GetCollect(_T("zr1"))) )
	   {
		  rstbl->PutCollect(_T("zr1"),_variant_t((float)0));
	   }
	   modPHScal::zr = rstbl->GetCollect(_T("zr1"));
	   if( (IsNull(rstbl->GetCollect(_T("pjg1"))) || rstbl->GetCollect(_T("pjg1")).fltVal == 0) )
	   {
		  if( (! IsNull(rstbl->GetCollect(_T("pgz1"))) && rstbl->GetCollect(_T("pgz1")).fltVal != 0) )
		  {
		  }
		  else
		  {
			 if( IsNull(rstbl->GetCollect(_T("pgz1"))) || rstbl->GetCollect(_T("pgz1")).fltVal == 0 )
			 {
				rstbl->PutCollect(_T("pgz1"),_variant_t((float) 100));
			 }
		  }
		  //结构荷载=gsngRatioOfPjg2Pgz倍工作荷载
		  if( IsNull(rstbl->GetCollect(_T("pjg1"))) || rstbl->GetCollect(_T("pjg1")).fltVal == 0 )
		  {
			  rstbl->PutCollect(_T("pjg1"), _variant_t((float)(modPHScal::gsngRatioOfPjg2Pgz * rstbl->GetCollect(_T("pgz1")).fltVal)));
			  //2007.09.03
			  if( gbMaxPgzPaz && fabs(fTmpPaz)>fabs(fTmpPgz) )
			  {
				  rstbl->PutCollect(_T("pjg1"), _variant_t((float)(modPHScal::gsngRatioOfPjg2Pgz * fTmpPaz)));
			  }
			  
		  }
	   }
	   else
	   {
		  if( (!IsNull(rstbl->GetCollect(_T("pgz1"))) && rstbl->GetCollect(_T("pgz1")).fltVal != 0) )
		  {
		  }
		  else
		  {
			 if( IsNull(rstbl->GetCollect(_T("pgz1"))) || rstbl->GetCollect(_T("pgz1")).fltVal == 0 )
			 {
				 rstbl->PutCollect(_T("pgz1"),_variant_t((float)( rstbl->GetCollect(_T("pjg1")).fltVal / modPHScal::gsngRatioOfPjg2Pgz)));
				  
				 //2007.09.03
				  if( gbMaxPgzPaz && fabs(fTmpPaz)>fabs(fTmpPgz) )
				  {
					  rstbl->PutCollect(_T("pjg1"), _variant_t((float)(modPHScal::gsngRatioOfPjg2Pgz * fTmpPaz)));
				  }
				  
			 }
		  }
	   }
	   if( fabs(rstbl->GetCollect(_T("pjg1")).fltVal)+0.01 < modPHScal::gsngRatioOfPjg2Pgz * fabs(rstbl->GetCollect(_T("pgz1")).fltVal) )//增加"+0.01"pfg2006.12.28
	   {
		  if( IsNull(rstbl->GetCollect(_T("PSAver"))) || Trim(vtos(rstbl->GetCollect(_T("PSAver")))) == _T("") )
		  {
		  //非应力计算程序获得的结构荷载值，必须进行荷载校核。
				sTmp.Format(GetResStr(IDS_StructureLoadLessthanOperatingLoad),_T("\%g"),_T("\%g"),_T("\%g"));
				sTmp2.Format(sTmp,rstbl->GetCollect(_T("pjg1")).fltVal,rstbl->GetCollect(_T("pgz1")).fltVal,modPHScal::gsngRatioOfPjg2Pgz);
				ShowMessage(sTmp2,MB_TOPMOST|MB_OK);
				rstbl->PutCollect(_T("pjg1"),_variant_t((float)(modPHScal::gsngRatioOfPjg2Pgz*rstbl->GetCollect(_T("pgz1")).fltVal)));

				 //2007.09.03
				  if( gbMaxPgzPaz && fabs(fTmpPaz)>fabs(fTmpPgz) )
				  {
					  rstbl->PutCollect(_T("pjg1"), _variant_t((float)(modPHScal::gsngRatioOfPjg2Pgz * fTmpPaz)));
				  }
         
		  }
	   }
	   UTmp=CalUnitCovertCoef(rstbl);
	   modPHScal::pjg = rstbl->GetCollect(_T("pjg1")).fltVal * UTmp;
	   modPHScal::pgz = rstbl->GetCollect(_T("pgz1")).fltVal * UTmp;
   
		//下面的获取结构必须位于找管部根部dn1/gn1之前
	   tmpvar=rstbl->GetCollect(_T("iSelSampleID"));
	   if( tmpvar.vt==VT_NULL || tmpvar.fltVal <= 0 )
	   {
		  iSelSampleID = Cavphs->GetPhsStructSampleID();
		  if( iSelSampleID <= 0 )
		  {
			  sTmp.Format(_T("%d"),iSelSampleID);
				sTmp2.Format(GetResStr(IDS_SelSampleIDisIllegal),sTmp);
				ShowMessage(sTmp2);
				goto errH;
		  }
		  else
			 rstbl->PutCollect(_T("iSelSampleID"),_variant_t((long) iSelSampleID));
	   }
	   else
		  iSelSampleID = vtoi(rstbl->GetCollect(_T("iSelSampleID")));
		Cavphs->GetPhsStructFromSampleID(iSelSampleID);
		//上面这段代码必须位于下段之前
	   //下句CalSAPA函数要用到dj,t0,pjg等参数
		//获取管部根部及其通用代号,如果不成功,则退出
		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(Recordset));
		_variant_t v;
		float tmppjg=fabs(modPHScal::pjg);
	   //获取管部根部及其通用代号
	   if(vtos(rstbl->GetCollect(_T("dn1")))==_T(""))
		{
		   SQLx = _T("SELECT DISTINCT CustomID FROM PictureClipData IN \'") + EDIBgbl::GetDBName(EDIBgbl::dbPRJ) + 
					_T("\' WHERE CustomID IN (SELECT DISTINCT CustomID FROM [") + modPHScal::tbnPA + _T("] IN \"\" [\; DATABASE=") 
								+ EDIBgbl::GetDBName(modPHScal::dbZDJcrude) + _T(" ;PWD=") + ModEncrypt::gstrDBZdjCrudePassWord + _T("] WHERE (Pmax >=") 
								+ ftos(tmppjg) + _T(" AND PictureClipData.ClassID<>") + ltos(iPAfixZ1) + _T(" AND PictureClipData.ClassID<>") + ltos(iPAfixZ2) 
								+ _T(" OR PictureClipData.ClassID=") + ltos(iPAfixZ1) + _T(" OR PictureClipData.ClassID=") + ltos(iPAfixZ2) + _T(") AND (Dw >= ") + ftos(modPHScal::dj* (1 - modPHScal::gnDW_delta * 0.01))
								+ _T(" AND Dw <= ") + ftos(modPHScal::dj* (1 + modPHScal::gnDW_delta * 0.01)) 
								+ _T(" AND Dw>0 OR Dw IS NULL OR Dw=0) AND tj >= ") + ftos(modPHScal::t0) + _T(")");
			if(rs->State)
				rs->Close();

			_CommandPtr cmd;
			cmd.CreateInstance(__uuidof(Command));
			cmd->ActiveConnection = modPHScal::dbZDJcrude;
			cmd->CommandText = (_bstr_t)SQLx;
			cmd->CommandType = adCmdText;
			
			AfxMessageBox(SQLx);
//			rs = cmd->Execute(NULL, NULL, adCmdText);
			rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
				adOpenKeyset, adLockOptimistic, adCmdText); 
			if(rs->adoEOF && rs->BOF)
			{
				//当前标准找不到管部
			}
			else
			{
				rs->get_Collect((_variant_t)_T("CustomID"),&v);
				dn=vtos(v);
				rstbl->PutCollect(_T("dn1"),_variant_t(dn));
			}
		}	   
		else
		{
			//管部/根部/连接件区分大小写
			dn = vtos(rstbl->GetCollect(_T("dn1")));
		}
	   //确定管部通用代号，以便程序适应各种标准的支吊架类型，
	   //具有通用性
	   IDdn = sFindID(dn);
	   if(IDdn != _T("") )
		  UpdateTZB4zdjh(_T("IDdn1"), _variant_t(IDdn));
	   else
	   {
		  IDdn = vtos(rstbl->GetCollect(_T("IDdn1")));
		  if( Trim(IDdn) != _T(""))
		  {
			 //当改变规范或厂家的时候，因为原始数据中的dn字段或gn字段未作相应变化，将出现查找不到CustomID的错误。
			 //此处试图修正这种错误。
			 dn = sFindCustomID(IDdn);
			 UpdateTZB4zdjh(_T("dn1"), _variant_t(dn));
		  }
		  else
		  {
			 IDdn = _T("");
			 //MsgBox _T("未找到通用管部代码!请重新定义通用代码之后再运行")
			 throw GetResStr(IDS_NotFoundGeneralPAID);
		  }
	   }
	   
	   //获取根部根部及其通用代号
	   if(vtos(rstbl->GetCollect(_T("gn1")))==_T(""))
		{		   
		   int Gnum;			
			Gnum= (modPHScal::glNumSA!=0 ? modPHScal::glNumSA : 1);
			SQLx = _T("SELECT CustomID FROM PictureClipData IN \'") + EDIBgbl::GetDBName(EDIBgbl::dbPRJ) + _T("\'")
								+ _T(" WHERE EXISTS ( Select CustomID FROM [") + modPHScal::tbnSA + _T("] IN \"\" [; DATABASE=") 
								+ EDIBgbl::GetDBName(modPHScal::dbZDJcrude) + _T(" ;PWD=") + ModEncrypt::gstrDBZdjCrudePassWord + _T("] WHERE PictureClipData.CustomID = CustomID AND (PictureClipData.ClassID= ") + ltos(iG100) + _T(" OR PMAXH >=") 
								+ ftos(tmppjg / Gnum * (vtob(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("ifLongVertPipe"))) ? Gnum : 1))
								+ _T(" AND (( PictureClipData.ClassID = ")
								+ ltos(iSACantilever) + _T(" OR PictureClipData.ClassID = ") + ltos(iSALbrace) + _T(" OR PictureClipData.ClassID = ") + ltos(iSALbraceFixG47) + _T(" OR PictureClipData.ClassID = ") + ltos(iSALbraceFixG48) + _T(" OR PictureClipData.ClassID = ") + ltos(iG51) + _T(" OR PictureClipData.ClassID = ") + ltos(iG56) + _T(" OR PictureClipData.ClassID = ") + ltos(iG57)  + _T(") AND GDW1 >=")
								+ ftos(modPHScal::gdw1) + _T(" OR ( PictureClipData.ClassID =") + ltos(iSAbeam) +_T(" OR PictureClipData.ClassID =") + ltos(iG52_55) + _T(" ) AND GDW1 >=")
								+ _T(" IIF(")+ ftos(modPHScal::gdw1+(modPHScal::glNumSA==1 ? modPHScal::DblHangerRodDist : 0)/2) +_T(" > L1/2 , L1 - ") + ftos(modPHScal::gdw1+(modPHScal::glNumSA == 1 ? modPHScal::DblHangerRodDist : 0) / 2)
								+ _T(" , ")+ ftos(modPHScal::gdw1+(modPHScal::glNumSA == 1 ? modPHScal::DblHangerRodDist : 0) / 2) + _T(") AND L1> ") + ftos(modPHScal::Lspan)
								+ _T("  OR PictureClipData.ClassID= ") + ltos(iGCement) + _T(")) )");
			if(rs->State == adStateOpen)
				rs->Close();
			rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
				adOpenStatic, adLockOptimistic, adCmdText); 
			if(rs->adoEOF && rs->BOF)
			{
				//当前标准找不到根部
			}
			else
			{
				rs->get_Collect((_variant_t)_T("CustomID"),&v);
				gn=vtos(v);
				rstbl->PutCollect(_T("gn1"),_variant_t(gn));
			}
		}	   
		else
		{
			//管部/根部/连接件区分大小写
			gn = vtos(rstbl->GetCollect(_T("gn1")));
		}
	   //确定根部通用代号，以便程序适应各种标准的支吊架类型，
	   //具有通用性
	   IDgn = sFindID(gn);
	   if(IDgn != _T("") )
		  UpdateTZB4zdjh(_T("IDgn1"), _variant_t(IDgn));
	   else
	   {
		  IDgn = vtos(rstbl->GetCollect(_T("IDgn1")));
		  if( Trim(IDgn) != _T(""))
		  {
			 //当改变规范或厂家的时候，因为原始数据中的gn字段或gn字段未作相应变化，将出现查找不到CustomID的错误。
			 //此处试图修正这种错误。
			 gn = sFindCustomID(IDgn);
			 UpdateTZB4zdjh(_T("gn1"), _variant_t(gn));
		  }
		  else
		  {
			 IDgn = _T("");
		  }
	   }


	   if( fabs(yr) > gUseSpringMinDist )
	   {
		  if( IsNull(rstbl->GetCollect(_T("paz1"))) || rstbl->GetCollect(_T("paz1")).fltVal == 0 )
		  {
			rstbl->PutCollect(_T("paz1"),_variant_t((float)((1 - gnSPR_CHZBH) * rstbl->GetCollect(_T("pgz1")).fltVal)));
		  }
		  paz = rstbl->GetCollect(_T("paz1")).fltVal * UTmp;
      
		  if( pgz != 0.0 )
		  {
		  }
		  else
		  {
			  sTmp2.Format(_T("%d"),zdjh);
			  sTmp.Format(GetResStr(IDS_ZDJNoXOperatingLoadIsZero),sTmp2);
			  ShowMessage(sTmp);
			  goto errH;
		  }
	   }
	   else
	   {
		  //计算刚性支吊架类型和名称
		  lngErrNum = CalRigidZDJType(rstbl);
		  //对于刚性支吊架安装荷载不一定等于工作荷载
	   }
	   //再次计算支吊架的名称
	   lngErrNum = CalRigidZDJType(rstbl);
	   sTmp=vtos(rstbl->GetCollect(_T("type")));
	   if( sTmp== _T("5000") || sTmp == _T("9999") )
	   {
		  if( IsNull(rstbl->GetCollect(_T("paz1"))) || rstbl->GetCollect(_T("paz1")).fltVal!= rstbl->GetCollect(_T("pgz1")).fltVal )
		  {
			 rstbl->PutCollect(_T("paz1"),rstbl->GetCollect(_T("pgz1")));
		  }
	   }
	   else
	   {
		  tmpvar=rstbl->GetCollect(_T("type"));
		  if( ! tmpvar.vt!=VT_NULL )
		  {
			  sTmp=vtos(tmpvar);
			  sTmp.TrimLeft();sTmp.TrimRight();
			 if( sTmp.GetLength()< 4 )
			 {
				//不是弹簧支吊架
				 //对于刚性支吊架安装荷载不一定等于工作荷载
			 }
		  }
	   }
	   tmpvar=rstbl->GetCollect(_T("t01"));
	   if( tmpvar.vt==VT_NULL )//|| (float)tmpvar == 0 )
	   {
		  rstbl->PutCollect(_T("t01"),_variant_t((float)0.0));
	   }
   
	   //图形文件前缀方式
	   if( glDWGPreMethod == 0 )
	   {
		   clb = EDIBAcad::GetDrawIDAndName(zdjh + glDrawNoStartNumber - 1, EDIBDB::DrawName);
	   }
	   else
	   {
		   sTmp.Format(_T("%2d"),zdjh + glDrawNoStartNumber - 1);
			clb = gsDWGPre+sTmp;
	   }
   
		tmpvar=rstbl->GetCollect(_T("dg1"));
	   if( tmpvar.vt==VT_NULL || vtos(tmpvar)==_T(""))
	   {
		  rstbl->PutCollect(_T("dg1"),_T("G"));//: rstbl.Update
	   }
      
	   //管部或根部偏装
	   //必须放在外面
		tbn = sFindTBN(dn);
		ZdjType=modPHScal::ZdjType;
		if( tbn == _T(""))
		{
			sTmp.Format(GetResStr(IDS_CantHandlePA),vtos(rstbl->GetCollect(_T("dn1"))));
			return;//throw sTmp;//原代码"throw sTmp;" 改为: "return;" pfg2006.12.20
		}
		else
		{
			if( glClassID == iPAh || glClassID == iPAs)
			{
				if( fabs(yr) > gUseSpringMinDist )
				{
					if( ZdjType == _T("9999") );
						//恒力
					else
					{
						if (Trim(ZdjType).GetLength() == 4 )
						{
							//弹簧吊架
							//必须传递安装压缩值给函数，否则弹簧数组的安装压缩值=0
							sngSpringWeightNormal(ZdjType, rstbl, vtof(rstbl->GetCollect(_T("haz"))));
						}
					}
				}
				else
				{
					//刚性支吊架
					giWholeSprNum = 0;
					ZdjType=_T("");
				}					
			}
			else
			{
				//固定支架或者是导向支架
				giWholeSprNum = 0;
				ZdjType=_T("");
			}
		}
    
	   tmpvar=rstbl->GetCollect(_T("LC1"));
	   if( tmpvar.vt==VT_NULL || tmpvar.fltVal <= 0.0 )
	   {
		  rstbl->PutCollect(_T("LC1"),_variant_t((float) 1800));
	   }
	   Lspan = rstbl->GetCollect(_T("LC1")).fltVal;  //根部L1值=跨距
	   tmpvar=rstbl->GetCollect(_T("b01"));
	   if( tmpvar.vt==VT_NULL || tmpvar.fltVal <= 0.0 )
	   {
		  rstbl->PutCollect(_T("b01"),_variant_t((float) 300));
	   }
	   WidthB = rstbl->GetCollect(_T("b01")).fltVal;   //根部柱子宽度B值
	   tmpvar=rstbl->GetCollect(_T("A01"));
	   if( tmpvar.vt==VT_NULL || tmpvar.fltVal <= 0 )
		  rstbl->PutCollect(_T("A01"),_variant_t((float) 300));
   
	   WidthA = rstbl->GetCollect(_T("A01")).fltVal;   //根部柱子宽度A值或梁高度H(mm)
   
		//计算偏装,因为其中的gdw1要先算出来。
		CalOffset(rstbl);
		//gdw经过了热位移偏装修正，在GetPhsBHandSizes中根据偏装后的根部定位值查表
   }
   
	catch(_com_error &e)
	{
		ShowMessage(e.Description());
	}
	catch(CString &err)
	{
		ShowMessage(err);
	}
errH: ;
}

float modPHScal::sngSpringWeight(CString /*ByVal*/ strZdjType, _RecordsetPtr rstbl, float /*Optional*/ mvDist, float& /*Optional*/ sngTotalHeight)
{
//目的:计算串联数为iSerialNum的弹簧总质量，以便排序和选择
//本段程序不从弹簧原始数据表检查弹簧串联数是否满足
//输入:支吊架类型,4个字符，strZdjType
//返回:弹簧总质量
		float ret=0;
		CString sTmp;
		CString strSprNo;
		CString SQLx;
		float sngWeight=0.0,sngHeight=0.0;
		int iSumSerialNum=0;
		int i=0;
//		float haz0,haz1;
		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(Recordset));
		_variant_t v;
	try
	{	
		//获取弹簧号，例如支吊架号为1208,则弹簧号strSprNo=08.
	   strSprNo = strZdjType.Mid(2);
   
	   iSumSerialNum = _ttoi(strZdjType.Left(1));
	   giSumSerialNum = iSumSerialNum;
	   if( (iSumSerialNum % giAllowedMaxSerialNumPerSPR) != 0 ){
		  giWholeSprNum = iSumSerialNum / giAllowedMaxSerialNumPerSPR + 1;
	   }else{
		  giWholeSprNum = iSumSerialNum / giAllowedMaxSerialNumPerSPR;
	   }
	   //Rsx(_T("PreCmprs"))=弹簧的预压缩值
	   //每个弹簧安装时的绝对压缩值haz0=K*Pgz+yr/iSumSerialNum
	   //每个弹簧安装时的相对压缩值haz1=K*Pgz+yr/iSumSerialNum-rsx.fields(_T("PreCmprs"))
	   //K--弹簧系数,mm/kgf;
	   //Pgz--工作荷载,kgf;
	   //yr--热位移,方向向上为+,向下为-,mm.
	   //总高度
	   sngTotalHeight = 0;
	   for( i = 0 ;i<giWholeSprNum ;i++)
	   {
		  if( i == giWholeSprNum - 1 && (iSumSerialNum % giAllowedMaxSerialNumPerSPR != 0) )
		  {
			 //最后一组弹簧的串联数
			 sSprInfo[i].SerialNum = iSumSerialNum % giAllowedMaxSerialNumPerSPR;
		  }
		  else
		  {
			 sSprInfo[i].SerialNum = giAllowedMaxSerialNumPerSPR;
		  }
		  sSprInfo[i].SprNo = _ttoi(strSprNo);
		  sSprInfo[i].DH = sSprInfo[i].SerialNum * 100 + sSprInfo[i].SprNo;
		  sSprInfo[i].haz = mvDist / iSumSerialNum * sSprInfo[i].SerialNum;
		  sSprInfo[i].HeatDisp = yr / iSumSerialNum * sSprInfo[i].SerialNum;
		  //以下查找质量,假定都是T1，这对于仅仅作为排序比较是可行的。
		  sTmp.Format(_T("%d"),sSprInfo[i].DH);
		  SQLx = _T("SELECT * FROM [") + tbnSPRINGCrude + _T("] WHERE DH=") + sTmp + _T(" And (CustomID)=\'") + sFindCustomID(_T("T1")) + _T("\'");
		  if(rs->State == adStateOpen)
			  rs->Close();
		  rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)dbZDJcrude,true), 
			  adOpenStatic, adLockOptimistic, adCmdText); 
		  if( rs->BOF && rs->adoEOF ){
			 sngWeight = 0;
			 sngHeight = 0;
		  }
		  else
		  {
			 rs->MoveFirst();
			rs->get_Collect((_variant_t)_T("weight"),&v);
			 if(v.vt==VT_NULL){
				sngWeight = 0;
				sngHeight = 0;
			 }else{
				 rs->get_Collect((_variant_t)_T("weight"),&v);
				 sngWeight=vtof(v);
				 rs->get_Collect((_variant_t)_T("sizeH"),&v);
				sngHeight = vtof(v);
			 }
		  }
		  //质量统计和*******
		  ret += sngWeight;
		  sngTotalHeight = sngTotalHeight + sngHeight;
	   }
	   return ret;
	  }
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}
	return ret;
}

float modPHScal::sngSpringWeightNormal(CString /*ByVal*/ strZdjType, _RecordsetPtr rstbl, float mvDist, float sngTotalHeight)
{
	//目的:计算串联数为iSerialNum的弹簧总质量，以便排序和选择
	//本段程序从弹簧原始数据表检查弹簧串联数是否满足
	//输入:支吊架类型,4个字符，strZdjType
	//处理:尽量选择相同的弹簧，尽量选择最少的弹簧。例如414=2个214串联，而<>314串114。514=314串214。
	//返回:弹簧总质量
	 //CString *Ptype=NULL;   //记录支吊架结构组件，不包括附件
	 //long *mlPartClassID=NULL;   //记录组件的子类别
	 //long *mlPartIndex=NULL;     //记录组件的类别
	 float ret=0.0;
	try
	{
		 bool bFound;   //找到满足串联数的弹簧
		 float sngWeight=0.0 , sngHeight=0.0;
		 long k;  //临时串联弹簧片数计数
		 long i; //临时串联弹簧组件数计数
		 CString mstrCurrentPart;    //当前组件ID
   
		 long iSumSerialNum ;CString strSprNo ;float  haz0=0.0 , haz1=0.0;
		 iSumSerialNum=0;
		 _RecordsetPtr rs;
		 rs.CreateInstance(__uuidof(Recordset));
		 _RecordsetPtr rs1;
		 rs1.CreateInstance(__uuidof(Recordset));
		 _RecordsetPtr rsw;
		 rsw.CreateInstance(__uuidof(Recordset));
		 _variant_t v;
		//保存当前支吊架组合结构内容（不含附件，便于绘图）
		 long lngLastPartNo;  //记录整个支吊架的组件数（不含附件）
		i=0;
		k=0;
		bFound=false;
		lngLastPartNo=0;
		iSelSampleID = vtoi(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("iSelSampleID")));
		//下面的函数返回了弹簧的CustomID到
		giWholeSprNum = Cavphs->GetPhsIsCSPR(iSPR, iSelSampleID);
		//ReDim sSprInfo(giWholeSprNum - 1)
   
		strSprNo = strZdjType.Mid(2);
		CString SQLx,sTmp;
		_variant_t vTmp;
		giSumSerialNum = _ttoi(strZdjType.Left(1));
		if( giSumSerialNum > giAllowedMaxSerialNumSPRs ){
			ShowMessage(GetResStr(IDS_OverAllowedMaxSerialNumSPRs));
		}
		else
		{
			//弹簧串联数在允许范围内
			while(true)
			{
				if( i + 1 > giWholeSprNum ){
					sTmp.Format(GetResStr(IDS_NeedSprNumMoreSprNumOfSampleID),ltos(i + 1),ltos(giWholeSprNum));
					//throw sTmp;
					return ret;
				}
				SQLx = _T("SELECT * FROM [") + tbnSPRINGCrude + _T("] WHERE (CustomID)=\'") + sSprInfo[i].CustomID + _T("\' AND (dh mod 100)=") + strSprNo + _T(" ORDER BY DH DESC");
				rs1->Open((_bstr_t)SQLx, _variant_t((IDispatch*)dbZDJcrude,true), 
					adOpenStatic, adLockOptimistic, adCmdText); 
				//if(!rs1->adoEOF && !rs1->BOF)
				//{
				rs1->get_Collect((_variant_t)_T("DH"),&vTmp);
				if( (i == 0? giSumSerialNum: k) > ( vtoi(vTmp) / 100 )){
					//弹簧组需要串联
					if( (giSumSerialNum % 2) == 0 ){
						//偶数，尽量使弹簧组相同
						k = giSumSerialNum / 2;
					}else{
						//奇数，尽量使弹簧组最少
						k = giSumSerialNum - iSumSerialNum;
					}
				}else{
					k = giSumSerialNum - iSumSerialNum;
				}
				SQLx = _T("SELECT * FROM [") + tbnSPRINGCrude + _T("] WHERE (CustomID)=\'") + sSprInfo[i].CustomID + _T("\' AND (dh mod 100)=") + strSprNo + _T(" AND int(dh/100)<=") + ltos(k) + _T(" ORDER BY DH DESC");
				rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)dbZDJcrude,true), 
					adOpenStatic, adLockOptimistic, adCmdText); 
				//只有从实际的数据库中才能判别这个弹簧的最大串联数,数据表中字段DH=串联数 *100+弹簧号
				if( rs->BOF && rs->adoEOF ){
					//数据表没有这种弹簧的数据
				}
				else
				{
					//数据表有这种弹簧的数据
					rs->MoveFirst();
					rs->get_Collect((_variant_t)_T("DH"),&vTmp);
					sSprInfo[i].SerialNum = vtoi(vTmp) / 100;
					iSumSerialNum = iSumSerialNum + sSprInfo[i].SerialNum;
					if( iSumSerialNum >= giSumSerialNum ){
						//串联数满足要求
						giWholeSprNum = i + 1;
						bFound = true;
						break;
					}
					//else{
						//不串联数满足要求
						//继续循环
					//}
         
				}
				i = i + 1;
				rs->Close();
				rs1->Close();
			}
			if( bFound )
			{
				//Rsx(_T("PreCmprs"))=弹簧的预压缩值
				//每个弹簧安装时的绝对压缩值haz0=K*Pgz+yr/iSumSerialNum
				//每个弹簧安装时的相对压缩值haz1=K*Pgz+yr/iSumSerialNum-rsx.fields(_T("PreCmprs"))
				//K--弹簧系数,mm/kgf;
				//Pgz--工作荷载,kgf;
				//yr--热位移,方向向上为+,向下为-,mm.
				ret = 0.0;
				//总高度
				sngTotalHeight = 0;
				for( i = 0 ;i<giWholeSprNum ;i++)
				{
					sSprInfo[i].SprNo = _ttoi(strSprNo);
					sSprInfo[i].DH = sSprInfo[i].SerialNum * 100 + sSprInfo[i].SprNo;
					sSprInfo[i].haz = mvDist / giSumSerialNum * sSprInfo[i].SerialNum;
					sSprInfo[i].HeatDisp = yr / giSumSerialNum * sSprInfo[i].SerialNum;
					//以下查找弹簧预压缩值，用于PhsYLBMake函数输出各种压缩值。
				  SQLx = _T("SELECT * FROM [") + tbnSPRINGproperty + _T("] WHERE SEQ=") + strSprNo;// + _T(" ORDER BY seq");
				  if(rs->State == adStateOpen)
					  rs->Close();
				  rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)dbZDJcrude,true), 
					  adOpenStatic, adLockOptimistic, adCmdText); 
				  if( rs->BOF && rs->adoEOF )
				  {
					  //没有发现这种弹簧,暂时预设为0
					  sSprInfo[i].Hpcmprs=0;
				  }
				  else
				  {
					  //存在这种弹簧
					  rs->get_Collect((_variant_t)_T("PreCmprs"),&v);
					  sSprInfo[i].Hpcmprs=sSprInfo[i].SerialNum*vtoi(v);
				  }
				  
					//以下查找质量,假定都是T1，这对于仅仅作为排序比较是可行的。
					SQLx = _T("SELECT * FROM [") + tbnSPRINGCrude + _T("] WHERE DH=") + ltos(sSprInfo[i].DH) + _T(" AND (CustomID)=\'") + sSprInfo[i].CustomID + _T("\'");
					rsw->Open((_bstr_t)SQLx, _variant_t((IDispatch*)dbZDJcrude,true), 
						adOpenStatic, adLockOptimistic, adCmdText); 
					if( rsw->BOF && rsw->adoEOF ){
						sngWeight = 0;
						sngHeight = 0;
					}
					else
					{
						rsw->MoveFirst();
						rsw->get_Collect((_variant_t)_T("weight"),&vTmp);
						if( vTmp.vt==VT_NULL){
							sngWeight = 0;
							sngHeight = 0;
						}else{
							rsw->get_Collect((_variant_t)_T("weight"),&vTmp);
							sngWeight = vtof(vTmp);
							rsw->get_Collect((_variant_t)_T("sizeH"),&vTmp);
							sngHeight =vtof(vTmp);
						}
					}
					//质量统计和*******
					ret +=sngWeight;
					sngTotalHeight = sngTotalHeight + sngHeight;
					rsw->Close();
				}
			}
		}
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}
	return ret;
}

long modPHScal::CalRigidZDJType(_RecordsetPtr rstbl)
{
//目的:计算刚性支吊架的type值及所有类型支吊架的图形名称
//返回:成功，0，ZdjType=支吊架类型值;失败，错误码。

	long iNumSPR,iNumCSPR;
   //使用循环来确定需要的恒力弹簧串联数,最多两个串联
   //保存当前支吊架组合结构内容（不含附件，便于绘图）
	try
	{
	   iSelSampleID =vtoi(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("iSelSampleID")));
	   //下面的函数返回了弹簧的CustomID到
	   iNumSPR = Cavphs->GetPhsIsCSPR(iSPR, iSelSampleID);
	   iNumCSPR = Cavphs->GetPhsIsCSPR(iCSPR, iSelSampleID);
   
	//Public Enum ZdjTxNameIndex
	 //  iZdjSPRh
	  // iZdjCSPRh
	   //iZdjRigidH
	//   iZdjGuide
	 //  iZdjSlide
	  // iZDJfix
	   //iZdjSprGuide
	//End Enum
		CString sTmp; 
		CString tbn;
	//   long lngErrNum;
		tbn = sFindTBN(dn);
		if( tbn == _T(""))
		{
			sTmp.Format(IDS_CantHandlePA,dn);
			//ShowMessage(sTmp); //原代码 pfg2006.12.20
			return IDS_CantHandlePA;
		}
		if( sngPEL < sngSEL)
		{
			 //吊架
			 if( iNumCSPR > 0)
				//恒力弹簧
				gsPhsName = ZdjTxName[iZdjCSPRh];
			 else
			 {
				 if( iNumSPR > 0 ||(iNumSPR==0&&fabs(modPHScal::yr)>=modPHScal::gUseSpringMinDist&&modPHScal::yr>0))
				   //弹簧
				   gsPhsName = ZdjTxName[iZdjSPRh];
				else
				{
				   //刚性
					if(glClassID== iPAh)
					{
						//ZdjType = _T("120");
						gsPhsName = ZdjTxName[iZdjRigidH];
						//rstbl->PutCollect(_T("type"),_variant_t(ZdjType));
					}
					else
						//未处理
						gsPhsName = ZdjTxName[iZdjRigidH];				   
				}
            
			 }
		}
		else
		{
			 //支架
			 if( iNumCSPR > 0 )
			 {
				 //恒力弹簧
				 gsPhsName = ZdjTxName[iZdjCSPRh];
			 }
			 else
			 {
				//非恒力弹簧
				if( iNumSPR > 0||(iNumSPR==0&&fabs(modPHScal::yr)>=modPHScal::gUseSpringMinDist&&modPHScal::yr>0))
				   //弹簧
				   gsPhsName = ZdjTxName[iZdjSprGuide];
				else
				{
				   //刚性
				   //if( IsNull(rstbl.Fields(_T("type"))) || Len(ZdjType) <> 3 Then
					  switch(glClassID)
					  {
					  case iPAfixZ1:
					  case iPAfixZ2:
							ZdjType = _T("177");
							gsPhsName = ZdjTxName[iZDJfix];
							break;
					  case iPAh:
							ZdjType = _T("120");
							gsPhsName = ZdjTxName[iZdjRigidH];
							break;
					  case iPAs:
							//仅适用于GLif(V1.2~3.x
							if( df.Right(1) == _T("X"))
							   ZdjType = _T("130");
							else
							   ZdjType = _T("110");
                        
							gsPhsName = ZdjTxName[iZDJfix];
							break;
					  default:
							ZdjType = _T("");
							gsPhsName = ZdjTxName[iZdjSupport];
							//Err.Raise iUE_CantHandlePA, , ResolveResString(iUE_CantHandlePA, _T("|1"), dn)
					}
					rstbl->PutCollect(_T("type"),_variant_t(ZdjType));
				}
			}
		} 
		SetRegValue(_T("Settings"),_T("PhsCurZDJName"), gsPhsName);    //zsy 05/1/4
		return 0;
	}
	catch(_com_error &e)
	{
		ShowMessage(e.Description());
		return IDS_CantHandlePA;
	}
}

void modPHScal::CalfxSA(_RecordsetPtr rstbl)
{
//目的：确定根部方向
   try
   {
	   if(rstbl->GetCollect(_variant_t(_T("gf1"))).vt==VT_NULL)
		   //UpdateTZB4zdjh(_T("gf1"),_T("PX"));
		   rstbl->PutCollect(_variant_t(_T("gf1")),_variant_t(_T("PX")));
	   else
	   {
		   gf1 = vtos(rstbl->GetCollect(_variant_t(_T("gf1"))));
		   gf1.TrimLeft();gf1.TrimRight();
		   gf1.MakeUpper();
		   if(gf1.Left(1)==_T("P") || gf1.Left(1)==_T("O"))
		   {
			   if(gf1.GetLength()!=2)
				   //UpdateTZB4zdjh(_T("gf1"), _T("PX"));
				   rstbl->PutCollect(_variant_t(_T("gf1")),_variant_t(_T("PX")));
			   else if(gf1.Mid(1,1)!=_T("X") && gf1.Mid(1,1)!=_T("Z"))
				   //UpdateTZB4zdjh(_T("gf1"),_T("PX"));
				   rstbl->PutCollect(_variant_t(_T("gf1")),_variant_t(_T("PX")));
		   }
		   else
			   rstbl->PutCollect(_variant_t(_T("gf1")),_variant_t(_T("PX")));
	   }
	   gf1 = vtos(rstbl->GetCollect(_variant_t(_T("gf1"))));
	   gf1.TrimLeft();gf1.TrimRight();
	   gf1.MakeUpper();
	   CalSpringParallelNum();
	   //双路支吊架共用一个根部时，根部走向受到下面情况的限制。
	   if(rstbl->GetCollect(_variant_t(_T("Gnum"))).vt!=VT_NULL && (long)rstbl->GetCollect(_variant_t(_T("Gnum")))==1 && gintParallelNum==2)
	   {
		   if( df == _T("X"))
		   {
			   if(gf1 != _T("PZ") && gf1 != _T("OZ") )
				   gf1 = _T("PZ");
		   }
		   else
		   {
			   if(gf1 != _T("PX") && gf1 != _T("OX"))
				   gf1 = _T("PX");
		   }
		   //rstbl.Edit: rstbl.Fields(_T("gf1")) = gf1: rstbl.Update
		   rstbl->PutCollect(_variant_t(_T("gf1")),_variant_t(gf1));
		   //UpdateTZB4zdjh(_T("gf1"), gf1);
	   }
	   gf1 = vtos(rstbl->GetCollect(_variant_t(_T("gf1"))));
	   gf1.TrimLeft();gf1.TrimRight();
	   gf1.MakeUpper();     //根部方向
	   //根部gf1有4种方向,按逆时针方向旋转,
	   //依次为PX,OZ,OX,PZ
	   //判断根部方向
	   fx=gf1.Left(1);
	   //fx:_T("P"),_T("O")
	   if(gf1.Right(1)==_T("X"))
		   gf=_T("X");
	   else
		   gf=_T("Z");
   }
   catch(_com_error &e)
   {
	   ShowMessage(e.Description());
   }
}

void modPHScal::CalfxPA(_RecordsetPtr rstbl)
{
//目的：确定管部方向
	CString sTmp;
	if(rstbl->GetCollect(_variant_t(_T("df1"))).vt==VT_NULL)
	{
		rstbl->PutCollect(_variant_t(_T("df1")),_variant_t(_T("X")));
		df=_T("X");
	}
	else
	{
		sTmp=vtos(rstbl->GetCollect(_variant_t(_T("df1"))));
		sTmp.TrimLeft();sTmp.TrimRight();
		sTmp.MakeUpper();
		if(sTmp==_T(""))
		{
			rstbl->PutCollect(_variant_t(_T("df1")),_variant_t(_T("X")));
			df=_T("X");
		}
		else
			df=sTmp;
		//管部只有2种方向,即X || Z
		//实际上Z11、Z13也有OX方向
		if(df!=_T("X") && df!=_T("-X") && df!=_T("Z") && df!=_T("-Z"))
		{
			rstbl->PutCollect(_variant_t(_T("df1")),_variant_t(_T("X")));
			df=_T("X");
		}
	}
}

void modPHScal::CalDG(_RecordsetPtr rstbl)
{
}

void modPHScal::CalUnitX(_RecordsetPtr rstbl)
{
 //目的:获得原始数据表中的单位制
	CString sTmp;
   if (rstbl->GetCollect(_variant_t(_T("Unit_Force"))).vt==VT_NULL)
      rstbl->PutCollect(_variant_t(_T("Unit_Force")),_variant_t(_T("kgf")));
   else
   {
		sTmp=vtos(rstbl->GetCollect(_variant_t(_T("Unit_Force"))));
		sTmp.TrimLeft();sTmp.TrimRight();
		if(sTmp==_T(""))
			rstbl->PutCollect(_variant_t(_T("Unit_Force")),_variant_t(_T("kgf")));
		else
		{
			if(sTmp!= _T("kgf") && sTmp!= _T("N"))
			{
				rstbl->PutCollect(_variant_t(_T("Unit_Force")),_variant_t(_T("kgf")));
			}
         //MsgBox ResolveResString(iUE_xUnitIsIllegal, _T("|1"), ResolveResString(iUE_UNIT_FORCE), _T("|2"), LCase(Trim(rstbl.Fields(_T("Unit_Force"))))), vbMsgBoxSetForeground
		}
   }
   if (rstbl->GetCollect(_variant_t(_T("Unit_Displacement"))).vt==VT_NULL)
      rstbl->PutCollect(_variant_t(_T("Unit_Displacement")),_variant_t(_T("mm")));
   if(true)
   {
		sTmp=vtos(rstbl->GetCollect(_variant_t(_T("Unit_Displacement"))));
		sTmp.TrimLeft();sTmp.TrimRight();
		sTmp.MakeLower();
		if(sTmp==_T(""))
			rstbl->PutCollect(_variant_t(_T("Unit_Displacement")),_variant_t(_T("mm")));
		else
		{
			if(sTmp!= _T("mm"))
			{
				rstbl->PutCollect(_variant_t(_T("Unit_Displacement")),_variant_t(_T("mm")));
			}
         //MsgBox ResolveResString(iUE_xUnitIsIllegal, _T("|1"), ResolveResString(iUE_UNIT_FORCE), _T("|2"), LCase(Trim(rstbl.Fields(_T("Unit_Force"))))), vbMsgBoxSetForeground
		}
   }
   if (rstbl->GetCollect(_variant_t(_T("Unit_Momental"))).vt==VT_NULL)
      rstbl->PutCollect(_variant_t(_T("Unit_Momental")),_variant_t(_T("kgf.m")));
   if(true)
   {
		sTmp=vtos(rstbl->GetCollect(_variant_t(_T("Unit_Momental"))));
		sTmp.TrimLeft();sTmp.TrimRight();
		if(sTmp==_T(""))
			rstbl->PutCollect(_variant_t(_T("Unit_Momental")),_variant_t(_T("kgf.m")));
		else
		{
			if(sTmp!= _T("kgf.m") && sTmp!= _T("N*m") && sTmp!= _T("kgf*m") && sTmp!= _T("N.m"))
			{
				rstbl->PutCollect(_variant_t(_T("Unit_Momental")),_variant_t(_T("kgf.m")));
			}
         //MsgBox ResolveResString(iUE_xUnitIsIllegal, _T("|1"), ResolveResString(iUE_UNIT_FORCE), _T("|2"), LCase(Trim(rstbl.Fields(_T("Unit_Force"))))), vbMsgBoxSetForeground
		}
   }
   //如果单位制变量尚未赋值，则赋初始值
   if (Unit_Force == _T("")) Unit_Force =vtos(rstbl->GetCollect(_variant_t(_T("Unit_Force"))));
   if (Unit_Displacement == _T("")) Unit_Displacement = vtos( rstbl->GetCollect(_variant_t(_T("Unit_Displacement"))));
   if (Unit_Momental == _T("")) Unit_Momental =vtos(rstbl->GetCollect(_variant_t(_T("Unit_Momental"))));
   //MsgBox Err.Description, vbMsgBoxSetForeground
}

float modPHScal::CalUnitCovertCoef(_RecordsetPtr rstbl, int iUNIT)
{
//目的：计算单位转换系数，默认iUNIT=0是力转换
   //单位
   CalUnitX( rstbl);
   CString sTmp;
   _variant_t tmpvar;
   switch(iUNIT)
   {
   case 0:
         //力单位换算
	   sTmp=vtos(rstbl->GetCollect(_T("Unit_Force")));
	   sTmp.TrimLeft();sTmp.TrimRight();sTmp.MakeLower();
         if(sTmp== _T("kgf") )
            return 1;
         else
			 return 1 / EDIBgbl::kgf2N;
         
   case 1:
         //长度单位换算
	   sTmp=vtos(rstbl->GetCollect(_T("Unit_Displacement")));
	   sTmp.TrimLeft();sTmp.TrimRight();sTmp.MakeLower();
         if (sTmp== _T("mm") )
            return 1;
         else
            return (float)EDIBgbl::cm2mm;
         
   case 2:
         //力矩单位换算
	   sTmp=vtos(rstbl->GetCollect(_T("Unit_Momental")));
	   sTmp.TrimLeft();sTmp.TrimRight();sTmp.MakeLower();
         if(sTmp==_T("kgf.m") )
            return  1;
         else
			 return 1 / EDIBgbl::kgf2N;
        
   }
	return 0;
}

void modPHScal::UpdateTZB4zdjh(CString FieldName,_variant_t FieldValue)
{
//目的:更新TZB表
   //On Error GoTo errH
	try
	{
		CString sTmp,sFieldValue;
		sTmp.Format(_T("%d"),zdjh);
		FieldName.TrimLeft();FieldName.TrimRight();
		if(FrmTxsr.m_pViewTxsr->m_ActiveRs!=NULL && FrmTxsr.m_pViewTxsr->m_ActiveRs->State!=adStateClosed)
		{
			FrmTxsr.m_pViewTxsr->m_ActiveRs->PutCollect(_variant_t(FieldName),FieldValue);
			//FrmTxsr.m_pViewTxsr->m_ActiveRs->Update();
		}
		else
		{
			if(FieldValue.vt==VT_BSTR)
			{
				sFieldValue=_T("\'");
				sFieldValue+=vtos(FieldValue);
				sFieldValue+=_T("\'");
			}
			else sFieldValue=vtos(FieldValue);
			EDIBgbl::SQLx = CString(_T("UPDATE [")) + EDIBgbl::Btype[EDIBgbl::TZA] + _T("] SET ") 
				+ FieldName + _T("= ") +  sFieldValue +_T(" WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) 
				+ _T(" AND zdjh=") + sTmp;
			EDIBgbl::dbPRJDB->Execute((_bstr_t)EDIBgbl::SQLx, NULL, adCmdText);
		}
		//_variant_t tmpvar;
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}
}

void modPHScal::VB_Cal(_RecordsetPtr rs, long zdjh,CFrmStatus &frmStatus,int nth)
{
	float fPgz,fPaz;//工作荷载，安装荷载
	iRepZdjType=-1;
	//每次计算之前必须将当前支吊架最大拉杆直径置为最小值0，防止其受其它支吊架最大直径干扰。
	modPHScal::iMaxRodDia=0;
	modPHScal::g_fLHGWeight=0;
	int inthCount = 0;//路数的数量
	try
	{
		modPHScal::g_bPipeClampBoltDiameterChanged = FALSE;
		CString strSQL;
		CString strTmp;
		strSQL.Format(_T("SELECT COUNT(*) FROM [z1] WHERE VolumeID=%d AND zdjh=%d AND nth<>1"),
			EDIBgbl::SelVlmID,zdjh);
		_RecordsetPtr rsZ1=::conPRJDB->Execute(_bstr_t(strSQL),NULL,adCmdText);
		_RecordsetPtr rsTmpZB;
		rsTmpZB.CreateInstance(__uuidof(Recordset));
		inthCount = vtoi(rsZ1->GetCollect((long)0)); 
		if(inthCount >0)
		{
		}
		rsZ1->Close();
		if(inthCount == 0)
		{
			//对于使用AutoPHS绘制的支吊架，为其生成Z1表中的路数记录
			strSQL.Format((_T("SELECT count(*) FROM [Z1] WHERE VolumeID=%d AND ZDJH=%d AND [nth]=1")),
							EDIBgbl::SelVlmID,zdjh);
			rsZ1=::conPRJDB->Execute(_bstr_t(strSQL),NULL,adCmdText);
			if(vtoi(rsZ1->GetCollect((long)0)) <=0)
			{
				strSQL.Format((_T("INSERT INTO [Z1] (VolumeID,ZDJH,nth) VALUES(%d,%d,%d)")),
								EDIBgbl::SelVlmID,zdjh,1);
				conPRJDB->Execute(_bstr_t(strSQL),NULL,adCmdText);
			}

			strSQL.Format("SELECT * FROM [ZB] WHERE VolumeID = %d AND ZDJH = %d AND NTH = %d ",EDIBgbl::SelVlmID,modPHScal::zdjh,nth);
			rsTmpZB->Open((_bstr_t)strSQL, _variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
				adOpenStatic, adLockOptimistic, adCmdText); 
			if(rsTmpZB->BOF && rsTmpZB->adoEOF)
			{
			  Cavphs->SavephsAllStructToTZB(zdjh,nth);
			}
			rsTmpZB->Close();
		}

		if(inthCount > 0)
		{
			strSQL.Format("SELECT * FROM [ZB] WHERE VolumeID = %d AND ZDJH = %d AND NTH = %d ",EDIBgbl::SelVlmID ,modPHScal::zdjh, nth );
			rsTmpZB->Open((_bstr_t)strSQL, _variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
				adOpenStatic, adLockOptimistic, adCmdText); 
			if(rsTmpZB->BOF && rsTmpZB->adoEOF)
			{
				Cavphs->SavephsAllStructToTZB (zdjh,nth);
			}
			rsTmpZB->Close();
		}

		FrmTxsr.m_pViewTxsr->m_PagOption->UpdateData();

		short iCalSuccess=0;
		int iNumCSPR,iNumSPR,iNumSPROrCSPR,lngLastPartNo,i;
		static long iPreviousZDJH=-1;//前次计算的支吊架号
		static bool bFirst=false;//当前支吊架第一次计算
		bool bFoundLHG;//发现下方恒吊 
		bFoundLHG=false;
	    //材料应力表
	    modPHScal::tbnAllowStress = _T("MechanicalOfMaterialSIGMAt");
		//弹簧松紧螺母直径不自动缩小，即弹簧松紧螺母直径是最小拉杆直径
		modPHScal::gbAutoSPRRodDiaDecrease=false;

		Cavphs->InitListRs();
		PreCalCrudeData(rs,nth);
		//PreCalCrudeData必须位于下句之前，否则 pgz 无值，lngPreCal 中不再需要调用 PreCalCrudeData
		
		  if (fabs(yr) > gUseSpringMinDist && pgz != 0 )
		  {
			 // dn = vtos(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("dn1")));
	    	 // CalSpringParallelNum(dn); //计算并联数
			  CalSpringParallelNum(vtos(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("dn1")))); //计算并联数
		      AutoSelSpringNo(rs,pgz/gintParallelNum,paz/gintParallelNum,yr,gnSPR_CHZBH,gsngTotalInstallCompressValue,iSelSampleID,
			     iRepZdjType,gSumDistOfSpring,bHintRepZdjType,gstrSpringOrder,ZdjType,gbAutoApplyChzbh,tbnSPRINGproperty,tbnDiscSPRINGpropertyMaxDist);
		  }
		  else			  
			iPreviousZDJH = zdjh;
   
	   //下面过程将破坏sSprInfo()全局数组，以至弹簧信息被破坏。因此必须放在开头。
	   CString strDeleteUserMaterial;
	   _RecordsetPtr rsTZB;
	   rsTZB.CreateInstance(__uuidof(Recordset));
	   strSQL=_T("SELECT zdjh FROM [") + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" AND ZDJH=") + ltos(modPHScal::zdjh) + _T(" AND bUserAdd is not null AND bUserAdd=-1");
	   rsTZB->Open((_bstr_t)strSQL, _variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
		   adOpenDynamic, adLockOptimistic, adCmdText); 
		if(!(rsTZB->BOF && rsTZB->adoEOF))
		{
			if(AfxMessageBox(IDS_YESNODELETEUSERMATERIAL,MB_YESNO)==IDNO)
				strDeleteUserMaterial=_T(" AND ( bUserAdd is null OR bUserAdd=0) "); 
			else
			{
				while(!rsTZB->adoEOF)
				{
					rsTZB->Delete(adAffectCurrent);
					if(rsTZB->adoEOF )
					{	
						rsTZB->MoveLast();					
					}
				}
			}

			
		}
		rsTZB->Close();
		//串联弹簧或恒力弹簧数量,还要返回是否含有滚动部件L9/L10,以便确定偏装值:
		//iScrollPartType=0/1/2(无滚动部件/只有L9/只有L10)
	   iNumCSPR = Cavphs->GetPhsIsCSPR(iCSPR, iSelSampleID, lngLastPartNo);
		if(iNumCSPR>0)
		{
			iNumSPROrCSPR=iNumCSPR;
			gbPhsIsCSPR = true;
			gbPhsIsSPR = false;
		}
		else
		{
			iNumSPR = Cavphs->GetPhsIsCSPR(iSPR, iSelSampleID, lngLastPartNo);
			if(iNumSPR>0)
			{
				iNumSPROrCSPR=iNumSPR;
				gbPhsIsCSPR = false;
				gbPhsIsSPR = true;
			}
			else
			{
				iNumSPROrCSPR=0;
				gbPhsIsCSPR = false;
				gbPhsIsSPR = false;
			}
		}

	   if(PtypeDiameter!=NULL)
	   {
		   delete [] PtypeDiameter;
			PtypeDiameter=NULL;
	   }
	   PtypeIndexForDiameter=0;
	   PtypeDiameter=new int[lngLastPartNo];
	   g_itest = (int)PtypeDiameter;
	   PtypeIndexForDiameter=lngLastPartNo;
	   for(i=0 ; i<lngLastPartNo ; i++)
		   PtypeDiameter[i]=0;//清拉杆直径为0，在getphsBHandSizes中，恒吊拉杆的直径保存在这个数组里，以便所有的螺纹连接件被约束。
	   CString sTmp2,sTmp;
	   //数据预先处理


	   if(lngPreCal() == -1)
	   {
		   if( fabs(yr) > gUseSpringMinDist && pgz != 0 )
		   {
			  //dn = vtos(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("dn1")));
	    	 // CalSpringParallelNum(dn); //计算并联数
			   CalSpringParallelNum(vtos(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("dn1")))); //计算并联数
			   AutoSelSpringNo(rs,pgz/gintParallelNum,paz/gintParallelNum,yr,gnSPR_CHZBH,gsngTotalInstallCompressValue,iSelSampleID,
			       iRepZdjType,gSumDistOfSpring,bHintRepZdjType,gstrSpringOrder,ZdjType,gbAutoApplyChzbh,tbnSPRINGproperty,tbnDiscSPRINGpropertyMaxDist);
		   }

		  if( iSelSampleID > 0 )
		  {
			 //调用GetphsBHandSizes方法计算一个支吊架之前，必须在dbPRJ中建立表tmpCSLen
			 MakeTmpCSLen();
			 //调用GetphsBHandSizes方法计算一个支吊架之前，必须在dbPRJ中建立表tmpCalFixPhs
			 Cavphs->iChkCondition = iPJG + iTJ + iGDW1 + iCNT + iDW;   //设置计算条件
         
			 iSelSampleID = vtoi(rs->GetCollect(_T("iSelSampleID")));
         			 if( iNumSPROrCSPR > 0 )
			 {
				giWholeSprNum = iNumSPROrCSPR;
				for( i = 0; i<SprInfoIndex;i++)
				{
				   if( sFindID(sSprInfo[i].CustomID) ==_T("LHG"))
					  bFoundLHG = true;
				}
				if(giCSPRindex!=NULL)
				{
					delete [] giCSPRindex;
					giCSPRindex=NULL;
				}
				giCSPRindex=new int[giWholeSprNum];				
				if( bFoundLHG)
				{
				   //第一遍计算找出恒吊拉杆直径，并且将拉杆直径赋给全局数组PtypeDiameter()
				   iCalSuccess = Cavphs->GetphsBHandSizes(frmStatus,1, 3, nth,fPaz,fPgz);
				   //第二遍计算找出各零件规格，直径除一般连接条件外，还要受到PtypeDiameter()数值约束
				   //可能要把管部零件重量累加到弹簧计算荷载，因此弹簧可能要重新选
				  // dn = vtos(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("dn1")));
				   
	    	       CalSpringParallelNum(vtos(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("dn1")))); //计算并联数
				   AutoSelSpringNo(rs,fPgz,fPaz,yr,gnSPR_CHZBH,gsngTotalInstallCompressValue,iSelSampleID,
					   iRepZdjType,gSumDistOfSpring,bHintRepZdjType,gstrSpringOrder,ZdjType,gbAutoApplyChzbh,tbnSPRINGproperty,tbnDiscSPRINGpropertyMaxDist);
				   iCalSuccess = Cavphs->GetphsBHandSizes(frmStatus,2, 3, nth,fPaz,fPgz);
				   //第三遍计算找出下方恒吊的松紧螺母直径，该直径=之后连接的拉杆直径
				   iCalSuccess = Cavphs->GetphsBHandSizes(frmStatus,3, 3, nth,fPaz,fPgz);
				}
				else
				{
				   //第一遍计算找出恒吊拉杆直径，并且将拉杆直径赋给全局数组PtypeDiameter()
				   iCalSuccess = Cavphs->GetphsBHandSizes(frmStatus,1, 2, nth,fPaz,fPgz);
				   //第二遍计算找出各零件规格，直径除一般连接条件外，还要受到PtypeDiameter()数值约束
				   //可能要把管部零件重量累加到弹簧计算荷载，因此弹簧可能要重新选
				   	//dn = vtos(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("dn1")));
				   
	    	        CalSpringParallelNum(vtos(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("dn1")))); //计算并联数
				    AutoSelSpringNo(rs,fPgz,fPaz,yr,gnSPR_CHZBH,gsngTotalInstallCompressValue,iSelSampleID,
					   iRepZdjType,gSumDistOfSpring,bHintRepZdjType,gstrSpringOrder,ZdjType,gbAutoApplyChzbh,tbnSPRINGproperty,tbnDiscSPRINGpropertyMaxDist);
				   iCalSuccess = Cavphs->GetphsBHandSizes(frmStatus,2, 2, nth,fPaz,fPgz);
				}
				if(giCSPRindex!=NULL)
				{
					delete [] giCSPRindex;
					giCSPRindex=NULL;
				}
			 }
			 else
			 {
				//直接运算第1遍
				//第二遍计算找出各零件规格，直径除一般连接条件外，还要受到PtypeDiameter()数值约束
				iCalSuccess = Cavphs->GetphsBHandSizes(frmStatus,1, 1, nth,fPaz,fPgz);
			 }
			 if( iCalSuccess!=false)
			 {
				if( bPAIsHanger())
				{
				   //如果是吊架
				   //计算螺栓螺母规格号，因为有些螺栓螺母规格在拉杆直径确定之前无法选择。
				   Cavphs->GetBoltsNutsAndAttachmentsCLgg();
				   //计算拉杆长度
				   if( CalRodLength(rs, zdjh))
				   {
					  MakeZDJ_ZD( zdjh);
					  PhsYLBMake(zdjh);   //生成某个支吊架的一览表数据
					  Cavphs->GetMaterial();
					  Cavphs->GetphsSEQ(rs);
					  //计算管部定位值,必须在计算了sizeH字段之后
					  Cavphs->GetPhsSAELandPAdxdydz();
					  CalOffset(rs);
					  frmStatus.UpdateStatus(1,true);
					  frmStatus.UpdateWindow();
					  _variant_t vSuccess;
					  vSuccess.ChangeType(VT_BOOL);
					  vSuccess.boolVal=(bool)iCalSuccess;
					  rs->PutCollect(_T("bCalSuccess"),vSuccess);
                  
					  //成功!  
					  if (! gbCalAllPhs)   
					  {
						  sTmp.Format(GetResStr(IDS_ZDJHphsCalculating),_T("\%d"),GetResStr(IDS_Success));
						  sTmp2.Format(sTmp,zdjh);
						  frmStatus.MessageBox(sTmp2);
					  }
				   }
				}
				else
				{
				   //如果是支架
					//当用户输入的根部标高<=管部标高，程序当作支架处理。而如果此时选择了吊架模板，则螺栓螺母材料规格未被处理，导致GetphsSEQ函数产生错误提示。
				   //计算螺栓螺母规格号，因为有些螺栓螺母规格在拉杆直径确定之前无法选择。
				   Cavphs->GetBoltsNutsAndAttachmentsCLgg();
				   MakeZDJ_ZD(zdjh);
				   PhsYLBMake(zdjh);   //生成某个支吊架的一览表数据
				   Cavphs->GetMaterial();
				   Cavphs->GetphsSEQ(rs);
				   //计算管部定位值,必须在计算了sizeH字段之后
				   Cavphs->GetPhsSAELandPAdxdydz();
				   frmStatus.UpdateStatus(1,true);
				   frmStatus.UpdateWindow();
				   _variant_t vSuccess;
					  vSuccess.ChangeType(VT_BOOL);
					  vSuccess.boolVal=(bool)iCalSuccess;
					  rs->PutCollect(_T("bCalSuccess"),vSuccess);
				   //成功!
				   if(!gbCalAllPhs)
				   {
					   sTmp.Format(GetResStr(IDS_ZDJHphsCalculating),_T("\%d"),GetResStr(IDS_Success));
						  sTmp2.Format(sTmp,zdjh);
						  frmStatus.MessageBox(sTmp2);
				   }
				}
			 }
			 else
			 {
				frmStatus.UpdateStatus(1,true);
				frmStatus.UpdateWindow();
				rs->PutCollect(_T("bCalSuccess"),_variant_t(false));
				//失败!
				sTmp.Format(GetResStr(IDS_ZDJHphsCalculating),_T("\%d"),GetResStr(IDS_Failure));
				sTmp2.Format(sTmp,zdjh);
				frmStatus.MessageBox(sTmp2);
			 }
		  }
	   }
	   else
	   {
		   frmStatus.UpdateStatus(1,true);
		   frmStatus.UpdateWindow();
			rs->PutCollect(_T("bCalSuccess"),_variant_t(false));
    		//失败!
			frmStatus.MessageBox(GetResStr(IDS_PreCalculationFailed));
			sTmp.Format(GetResStr(IDS_ZDJHphsCalculating),_T("\%d"),GetResStr(IDS_Failure));
			  sTmp2.Format(sTmp,zdjh);
			  frmStatus.MessageBox(sTmp2);
	   }
	   rs->PutCollect(_T("IsCal"),rs->GetCollect(_T("bCalSuccess")));
	   PhsMLmake(zdjh);    //把支吊架图号\图名写入目录库
   }
   catch (_com_error &e)
   {
	   CString strMsg;
	   strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
	   AfxMessageBox(strMsg);
		frmStatus.MessageBox(e.Description());
   }

   if ( g_itest != (int)PtypeDiameter )
	   AfxMessageBox ( "hello" );
   if(PtypeDiameter!=NULL)
   {
	   delete [] PtypeDiameter;
		PtypeDiameter=NULL;
   }
   iRepZdjType=-1;
}

bool modPHScal::CalRodLength(_RecordsetPtr  rstbl, long  zdjh)
{
//目的：自动计算和分配拉杆长度
//原则：1，尽量使用最大长度的拉杆，也就是尽量使拉杆数最少;
      //2，在拉杆数确定时，使每节的长度尽量接近平均长度。
      //3，按华东电力设计院支吊架手册设计规则对拉杆长度尾数取整。
//输入：tbnLugInfo--拉杆长度设计规则表名称变量，在选择规范时赋值
//返回：每节拉杆的长度写到结果数据表TZB的sizeDIM字段，同时计算单重。
	bool ret=false;
	CString *Ptype=NULL;   //记录支吊架结构组件，不包括附件
	long *mlPartClassID=NULL;    //记录组件的子类别
	long *mlPartIndex=NULL;     //记录组件的类别
	long *mlPartDia=NULL;     //记录组件的直径
	long *mlPartRecno=NULL;     //记录组件的记录号
	float (*mvActualLugExt)[2]=NULL;      //记录组件的实际拉杆长度插入值，该值大于最小值AllowedMinLugExt而小于最大值AllowedMaxLugExt
	float (*AllowedMinLugExt)[2]=NULL;    //记录连接件的允许调整最小长度值,拉杆插入该连接件的长度不得小于此值;对于穿过根部或管部的拉杆,取数据库中螺栓螺母的裕量,第二维0表示拉杆上端，1表示下端
	float (*AllowedMaxLugExt)[2]=NULL;   //记录连接件的允许调整最大长度值,拉杆插入该连接件的长度不得大于此值，经过连接匹配修正后的值（即减去了L8另外一端零件占据的空间后剩下的最大可插入值）
	long *PreviousPartInsertLen=NULL;   //前一个组件或后一个组件插入本组件(L8)的长度值

	try
	{
		_variant_t vTmp1,vTmp2,vTmp3;
		 _RecordsetPtr rs;
		 rs.CreateInstance(__uuidof(Recordset));
		 _RecordsetPtr rsTZB;
		 rsTZB.CreateInstance(__uuidof(Recordset));
		 _RecordsetPtr rsTmp;
		 rsTmp.CreateInstance(__uuidof(Recordset));

		 CString tbn1, sBHFormat , sBH;
		 float sngLugAvgLen=0.0 , mvL=0.0 , sngTmpSumLen=0.0;
		 float iDifRodLen=0.0 ; //拉杆长度圆整前与圆整后的差值mm
		 long i=0, k=0, n =0, iNumRod=0 ;
		 long RodLen[36];
		 memset(RodLen,0,sizeof(RodLen));
		 float MinLength=0.0, MaxLength=0.0;
		 float IncrementStart1=0.0, IncrementLength1=0.0;
		 float IncrementStart2=0.0, IncrementLength2=0.0;
		 bool bLengthChanged=0;
		 CString mstrCurrentPart;    //当前组件CustomID
//		 long lngLastPartNo;  //记录整个支吊架的组件数（不含附件）
//		 long lngCurrentPartNo;  //记录当前组件（不含附件）的序号
//		 long lngAttachedPartNo;   //记录当前组件与前一个组件之间的附件数
		 CString sLugPASA;  //长度修正表达式标志
		 CString tmpSQLlugLen;
		 giDiameter=0.0;   //拉杆直径
		 float lngLastRodPos=0; //最后一根拉杆的顺序号
		 float iTotalDeltaRodLen=0.0;    //总的拉杆余量调节长度（=拉杆总的实际长度-拉杆总的标注尺寸)
		 float sngDim=0.0;   //调整长度
		 float tempT3=0.0 ; //上方弹簧T3高度
		 float tempT3A=0.0 ;   //下方弹簧T3A高度
		 float tempMinLength =0;  //允许单个拉杆零件的最小长度（每根都可能不一样）
		 float iRodMinLen=0.0;    //允许拉杆最小总长度（长度圆整后需>=此值）
		 float iRodMaxLen=0.0;   //允许拉杆最大总长度（长度圆整后需<=此值）
		 float iSumRodLength=0.0; //拉杆计算总长度
		 float iSumRodLength1=0.0;//支吊架的根部标高和管部标高之间的总长度(不包括拉杆)//pfg2005.12.21
		 float fTmp;//临时变量
		 float mfNutsHeight;//记录连接螺母的厚度
		CString SQLx,sTmp,sTmp1;
		_variant_t vTmp;


		//注意下句中必须是IsSAPart<>-1，而不能是NOT IsNull(seq)，以便于后面呼应一致：因为根部也可能没有编号（公用根部），而根部要参与零件判断
		SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] WHERE zdjh=") + ltos(zdjh) + _T(" AND VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" AND NOT IsNull(ClassID) AND ClassID<>") + ltos(iAttached) + _T(" AND ClassID<>") + ltos(iBolts) + _T(" AND ClassID<>") + ltos(iNuts) + _T(" AND IsSAPart<>-1 ORDER BY recno");
		rsTZB->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
			adOpenDynamic, adLockOptimistic, adCmdText); 
		if( rsTZB->BOF && rsTZB->adoEOF )
		{
			sTmp.Format(GetResStr(IDS_NoThisZDJHResultInTBNSelPrjspecTZB),EDIBgbl::GetDBName(EDIBgbl::dbPRJDB), EDIBgbl::Btype[EDIBgbl::TZB],SQLx);
			throw sTmp;
		}
		else 
		{
			rsTZB->MoveLast();
			int iTZBRcount = rsTZB->RecordCount;
			i = iTZBRcount;
			rsTZB->MoveFirst();//加快速度
			//拉杆数
			//第一个零件下标i=0			
			Ptype=new CString [i];
			mlPartClassID=new long [i];
			mlPartIndex=new long [i];
			mlPartRecno=new long [i];
			mlPartDia=new long [i];
			mvActualLugExt=new float [i][2];
			AllowedMinLugExt=new float [i][2];
			AllowedMaxLugExt=new float [i][2];
			PreviousPartInsertLen=new long [i];

			memset(mlPartClassID,0,sizeof(long)*i);
			memset(mlPartIndex,-1,sizeof(long)*i);//赋-1，防止和根部iSA=0混淆
			memset(mlPartRecno,0,sizeof(long)*i);
			memset(mlPartDia,0,sizeof(long)*i);
			memset(PreviousPartInsertLen,0,sizeof(long)*i);
			memset(mvActualLugExt,0,sizeof(float)*i*2);
			memset(AllowedMaxLugExt,0,sizeof(float)*i*2);
			memset(AllowedMinLugExt,0,sizeof(float)*i*2);


			for( i = 0 ;i< iTZBRcount;i++)
			{//4 ZA表中的每一条记录进行循环取值(start)
				rsTZB->get_Collect((_variant_t)_T("ID"),&vTmp);
				Ptype[i] =vtos(vTmp);

				rsTZB->get_Collect((_variant_t)_T("ClassID"),&vTmp);
				mlPartClassID[i] =vtoi(vTmp);

				rsTZB->get_Collect((_variant_t)_T("recno"),&vTmp);
				mlPartRecno[i]=vtoi(vTmp);

				mlPartIndex[i] = vtoi(sFindFLD(_T("ID"), _T("Index"), Ptype[i]));
				rsTZB->get_Collect((_variant_t)_T("sizeC"),&vTmp);
				mlPartDia[i] = vtoi(vTmp);

				if( mlPartClassID[i] == iROD )
				{//3
					SQLx.Format(_T("SELECT SizeH,sizeHH FROM [%s] WHERE size2 = %d AND CustomID = \'%s\'"),modPHScal::tbnBoltsNuts,mlPartDia[i],modPHScal::sFindCustomID("F2"));	
					rsTmp->Open((_bstr_t)SQLx, _variant_t((IDispatch*)dbZDJcrude,true), 
						adOpenDynamic, adLockOptimistic, adCmdText); 
					if(rsTmp->BOF && rsTmp->adoEOF )
					{//1
						sTmp.Format(GetResStr(IDS_NoRecordInXtableInXmdb),EDIBgbl::GetDBName(modPHScal::dbZDJcrude), modPHScal::tbnBoltsNuts,SQLx);
						throw sTmp;
//						mfNutsHeight = 0;
					}//1
					else
					{//2
						rsTmp->get_Collect((_variant_t)"SizeH",&vTmp);
						mfNutsHeight=vtof(vTmp);
						if(mfNutsHeight == 0)
						{//1
							rsTmp->get_Collect((_variant_t)"sizeHH",&vTmp);
							mfNutsHeight = vtof(vTmp);
						}//1
					}//2

					rsTmp->Close();
					//螺纹吊杆
					if(Ptype[i]==_T("L2"))
					{//1
						//拉杆上端插入长度=L2左螺纹长度
						rsTZB->get_Collect((_variant_t)_T("AllowedMinLugExt"),&vTmp);
						AllowedMinLugExt[i][0]= vtof(vTmp);
						AllowedMaxLugExt[i][0] = vtof(vTmp);
						//拉杆下端插入长度=右螺纹长度
						rsTZB->get_Collect((_variant_t)_T("AllowedMaxLugExt"),&vTmp);
						AllowedMinLugExt[i][1]= vtof(vTmp)-mfNutsHeight;
						AllowedMaxLugExt[i][1] = vtof(vTmp)-mfNutsHeight;						
					}//1
					else
					{//1
						//拉杆上端插入长度=L2左螺纹长度或L1右螺纹长度
						rsTZB->get_Collect((_variant_t)_T("AllowedMinLugExt"),&vTmp);
						AllowedMinLugExt[i][0]= vtof(vTmp)-mfNutsHeight;
						AllowedMaxLugExt[i][0] = vtof(vTmp)-mfNutsHeight;
						//拉杆下端插入长度=右螺纹长度
						rsTZB->get_Collect((_variant_t)_T("AllowedMaxLugExt"),&vTmp);
						AllowedMinLugExt[i][1]= vtof(vTmp)-mfNutsHeight;
						AllowedMaxLugExt[i][1] = vtof(vTmp)-mfNutsHeight;						
					}//1
				}//3
				else
				{//2
					//其它连接件，预先取插入值
					if( Ptype[i] == _T("LHA") ||Ptype[i] == _T("LHB") || Ptype[i] == _T("PHA") || Ptype[i] == _T("PHB"))
					{//1
						rsTZB->get_Collect((_variant_t)_T("AllowedMinLugExtUp"),&vTmp);
						AllowedMinLugExt[i][0]= vtof(vTmp);
						rsTZB->get_Collect((_variant_t)_T("AllowedMinLugExt"),&vTmp);
						AllowedMinLugExt[i][1]= vtof(vTmp);
						
						rsTZB->get_Collect((_variant_t)_T("AllowedMaxLugExtUp"),&vTmp);
						AllowedMaxLugExt[i][0] = vtof(vTmp);
						rsTZB->get_Collect((_variant_t)_T("AllowedMaxLugExt"),&vTmp);
						AllowedMaxLugExt[i][1] = vtof(vTmp);
					}//1
					else
					{//1						
						rsTZB->get_Collect((_variant_t)_T("AllowedMinLugExt"),&vTmp);
						AllowedMinLugExt[i][0]= vtof(vTmp);
						AllowedMinLugExt[i][1]= vtof(vTmp);
						
						rsTZB->get_Collect((_variant_t)_T("AllowedMaxLugExt"),&vTmp);
						AllowedMaxLugExt[i][0] = vtof(vTmp);
						AllowedMaxLugExt[i][1] = vtof(vTmp);
					}//1
				}//2
				rsTZB->MoveNext();
			}//4 ZA表中的每一条记录进行循环取值(end)
						
			//在此之前必须先获得下一个零件的ClassID和Index
			//必须在第一次循环之后，因为要获取下一个零件的ClassID和Index
			bool bHasPA=false,bHasSA=false,bHasT3=false,bHasT3A=false;
			int iRodPosHasPA,iRodPosHasSA;
			rsTZB->MoveFirst();
			giNumRod = 0;
			rsTZB->MoveFirst();
			for( i = 0 ;i< iTZBRcount;i++)
			{
				rsTZB->get_Collect((_variant_t)_T("ID"),&vTmp);
				Ptype[i] =vtos(vTmp);

				//修改于2003.06.09，原为 
				//if(mlPartClassID[i] == iROD)
				if( mlPartClassID[i] == iROD || mlPartClassID[i] == iXL1)
				{ //3 是拉杆(start)
					//是拉杆,拉杆数加1
					giNumRod++;
					//记住最后一根拉杆的位置，以便后面计算最后拉杆时好调整拉杆长度
					lngLastRodPos = i;
					//获得拉杆直径值(此前通过GetPhsBHandSizes函数暂时保存在sizeC字段)，以便现在或以后使用
					//rsTZB->get_Collect((_variant_t)_T("sizeC"),vTmp);
					//iDiameter = vtof(vTmp);
					//在过程GetBoltsNutsAndAttachmentsCLgg中，字段C保存了拉杆在与管部、根部、上下方弹簧连接的情况下拉杆伸出的最小长度(华东院),对于西北院C可能大于拉杆螺纹长度
					//支吊架零部件高度和（含拉杆与管部、根部、上下方弹簧等的出头裕量）
					//保存下方出头裕量
					if( Ptype[i-1] == _T("T3A") || mlPartIndex[i-1]==iPA)
					{//2
						//管部或下方弹簧，加下段
						rsTZB->get_Collect((_variant_t)_T("C"),&vTmp);
						fTmp=vtof(vTmp);
						//拉杆下端插入长度
						//受到螺纹长度限制
						//插入值取较小值
						if(fTmp < AllowedMaxLugExt[i][1])
						{//1
							AllowedMinLugExt[i-1][0]= fTmp;
							AllowedMaxLugExt[i-1][0]= AllowedMaxLugExt[i][1];
							AllowedMinLugExt[i-1][1]= fTmp;
							AllowedMaxLugExt[i-1][1]= AllowedMaxLugExt[i][1];
							//iSumRodLength -=fTmp;
						}//1
						else
						{//1
							AllowedMinLugExt[i-1][0]= AllowedMinLugExt[i][1];
							AllowedMaxLugExt[i-1][0]= AllowedMaxLugExt[i][1];
							AllowedMinLugExt[i-1][1]= AllowedMinLugExt[i][1];
							AllowedMaxLugExt[i-1][1]= AllowedMaxLugExt[i][1];
							//iSumRodLength -=AllowedMinLugExt[i-1][0];
						}//1
					}//2

					if((mlPartClassID[i - 1] == iCSPR && (Ptype[i - 1] != _T("ZHB") || Ptype[i - 1] != _T("LHA") || Ptype[i - 1] != _T("PHA")) ) || ( mlPartClassID[i - 1] == iSPR && Ptype[i - 1] == _T("T3A"))||Ptype[i - 1].Left (2) == _T("L8") ||Ptype[i - 1].Left (2) == _T("L7"))
					{//2
						if( AllowedMaxLugExt[i - 1][0] > AllowedMaxLugExt[i][1] )
						{//1
							AllowedMaxLugExt[i - 1][0] = AllowedMaxLugExt[i][1];
							//AllowedMaxLugExt[i - 1][1] = AllowedMaxLugExt[i][1];//测试2005.11.16 李总、pfg
						}//1
					}//2
					if((mlPartClassID[i + 1] == iCSPR && Ptype[i + 1] != _T("ZHB")) || ( mlPartClassID[i + 1] == iSPR && Ptype[i + 1] == _T("T3"))||Ptype[i + 1].Left (2) == _T("L8") ||Ptype[i + 1].Left (2) == _T("L7"))
					{//2
						if( AllowedMaxLugExt[i + 1][1] > AllowedMaxLugExt[i][0] )
						{//1
							//AllowedMaxLugExt[i + 1][0] = AllowedMaxLugExt[i][0];//测试2005.11.16 李总、pfg
							AllowedMaxLugExt[i + 1][1] = AllowedMaxLugExt[i][0];
						}//1
					}//2

					//保存上方出头裕量
					if( Ptype[i+1] == _T("T3") || mlPartIndex[i+1]==iSA)
					{//2
						//根部或上方弹簧，加尺寸上段
						rsTZB->get_Collect((_variant_t)_T("C"),&vTmp);
						fTmp=vtof(vTmp);
						//拉杆上端插入长度
						//受到螺纹长度限制
						if(fTmp < AllowedMaxLugExt[i][1])
						{//1
							AllowedMinLugExt[i+1][0] = fTmp;
							AllowedMaxLugExt[i+1][0] = AllowedMaxLugExt[i][0];
							AllowedMinLugExt[i+1][1] = fTmp;
							AllowedMaxLugExt[i+1][1] = AllowedMaxLugExt[i][0];
							//iSumRodLength -=fTmp;
						}//1
						else
						{//1
							AllowedMinLugExt[i+1][0] = AllowedMinLugExt[i][0];
							AllowedMaxLugExt[i+1][0] = AllowedMaxLugExt[i][0];
							AllowedMinLugExt[i+1][1] = AllowedMinLugExt[i][0];
							AllowedMaxLugExt[i+1][1] = AllowedMaxLugExt[i][0];
							//iSumRodLength -=AllowedMinLugExt[i+1][0];
						}//1
					}//2
					//L5,L6,L7,L8
					if( Ptype[i-1] == _T("L5") || Ptype[i-1] == _T("L6") )
					{//2
						//花篮螺丝，下端插入值受到前一个拉杆上端左螺纹长度限制
						//受到螺纹长度限制
						if(AllowedMaxLugExt[i-2][0] < AllowedMaxLugExt[i-1][1])
						{//1
							//下拉杆上端螺纹长度<花篮螺丝下端允许最大插入值
							//AllowedMinLugExt[i-1][1] = AllowedMinLugExt[i-2][0];
							if( AllowedMaxLugExt[i-2][0] <= 0 )
								;
							else
								AllowedMaxLugExt[i-1][1] = AllowedMaxLugExt[i-2][0];
						}//1
						if(AllowedMaxLugExt[i][1] < AllowedMaxLugExt[i-1][0])
						{//1
							//上拉杆下端螺纹长度<花篮螺丝上端允许最大插入值
							//AllowedMinLugExt[i-1][0] = AllowedMinLugExt[i][1];
							if( AllowedMaxLugExt[i][1] <= 0 )
								;
							else
								AllowedMaxLugExt[i-1][0] = AllowedMaxLugExt[i][1];
						}//1
					}//2
				}//3 是拉杆(end)
				else 
				{//6 //不是拉杆(start)
					rsTZB->get_Collect((_variant_t)_T("sizeDIM"),&vTmp);
					if( Ptype[i] == _T("T3A") )
						tempT3A = fabs(vtof(vTmp));
					else if( Ptype[i] == _T("T3") )
						tempT3 = fabs(vtof(vTmp));
					if( Ptype[i] == _T("T3A") || mlPartIndex[i]==iPA)
					{//3
						//已经保存
						//如果管部或下方弹簧与拉杆连接，拉杆长度是可调整的
						//第一个零件i=0
						if((i+1)<iTZBRcount)
						{
							if(mlPartClassID[i+1] == iROD)
							{
								bHasPA=true;
								iRodPosHasPA=i;
							}
						}
					}//3
					else
					{//5
						if( Ptype[i] == _T("T3") || mlPartIndex[i]==iSA)
						{//4
							//已经保存
							//如果根部或上方弹簧与拉杆连接，拉杆长度是可调整的
							if((i-1)>=0)
							{//3
								if(mlPartClassID[i-1] == iROD)
								{
									bHasSA=true;
									iRodPosHasSA=i;
								}
								else if(mlPartClassID[i-1] == iCSPR)
								{//2
									//
									if(Ptype[i-1]==_T("PHD1"))
									{
										//这种下方恒吊不能计算根部高度
										rsTZB->get_Collect((_variant_t)_T("sizeDIM"),&vTmp);//pfg?
										iSumRodLength+=vtof(vTmp);
									}
									else
									{
									}
								}//2
							}//3
						}//4
						else
						{
							//已经获得插入值了
						}
					}//5
					rsTZB->get_Collect((_variant_t)_T("PreviousPartInsertLen"),&vTmp);
					PreviousPartInsertLen[i] = vtoi(vTmp);
					//支吊架零部件高度和（不含拉杆与管部、根部、上下方弹簧等的最小伸出长度，不计其它螺纹件内的拉杆插入长度）
					//根部的sizeH字段在此之前必须=0，而sizeDIM字段必须=根部高度，拉杆长度才能计算正确。
					rsTZB->get_Collect((_variant_t)_T("sizeH"),&vTmp);
					iSumRodLength+=vtof(vTmp);
					//iSumRodLength += abs(vtof(vTmp));
				}//6	//不是拉杆(end)
				rsTZB->MoveNext();
			}
      
			//拉杆长度=(根部标高-管部标高)*1000-其它零部件高度之和+拉杆长度调整值
			//此时获得的拉杆总长度是一个最小值，即无论怎么调整花篮螺丝或其它，拉杆总长度都必须>=此值
			//此时的拉杆总长度考虑了上下方弹簧或管部根部连接的裕量。但是没有考虑螺纹接头插入长度。
			iSumRodLength = (sngSEL - sngPEL) * 1000 - iSumRodLength;
			iSumRodLength1=iSumRodLength;//pfg2005.12.21
			//设置最小值初始值准备累加
			iRodMinLen = iSumRodLength;
			iRodMaxLen = iSumRodLength;
      
			//重新开始，考虑螺纹连接件伸入长度，来修正拉杆总长度
			rsTZB->MoveFirst();
			for( i = 0 ;i<iTZBRcount;i++)
			{
				//if( Ptype[i] == _T("L8") )
				//2003.05.23夏中伟修改，原为上句
				if( Ptype[i].Left(2) == _T("L8") )
				{//5 //当前是L8(start)
					if( i + 1 <= iTZBRcount - 1 )
					{//4
						if( Ptype[i + 1] == _T("L7") || Ptype[i + 1] == _T("G12") || Ptype[i + 1] == _T("L7Dd") || Ptype[i + 1] == _T("G12Lc") )
						{//3
							//当前是L8,下一个是L7、G12
							//修正L8的插入长度,该长度在后一个零件的PreviousPartInsertLen字段
							AllowedMaxLugExt[i][0] -= PreviousPartInsertLen[i + 1];
							if( AllowedMaxLugExt[i][0] < AllowedMinLugExt[i][0] )
							{
								//拉杆允许最大插入长度<最小值
								if(Ptype[i] != _T("L8Dd"))
								{
									sTmp.Format(GetResStr(IDS_PreviousPartInsertLenOverAllowedValue),ltos(PreviousPartInsertLen[i+1]),ftos(AllowedMaxLugExt[i][0]-AllowedMinLugExt[i][0]));
									ShowMessage(sTmp);
								}

								//2007.09.10
								iRodMaxLen += AllowedMaxLugExt[i][0];
								iRodMinLen += AllowedMinLugExt[i][0];
							}
							else
							{
								iRodMaxLen += AllowedMaxLugExt[i][0];
								iRodMinLen += AllowedMinLugExt[i][0];
							}
						}//3
						else 
						{//2
							//当前是L8,下一个不是L7、G12
							//修正L8的插入长度,该长度在前一个零件的PreviousPartInsertLen字段
							AllowedMaxLugExt[i][0] -=PreviousPartInsertLen[i - 1];	
							if( AllowedMaxLugExt[i][0] < AllowedMinLugExt[i][0] )
							{
								//拉杆允许最大插入长度<最小值
								sTmp.Format(GetResStr(IDS_PreviousPartInsertLenOverAllowedValue),ltos(PreviousPartInsertLen[i+1]),ftos(AllowedMaxLugExt[i][0]-AllowedMinLugExt[i][0]));
								ShowMessage(sTmp);
								
								//2007.09.10
								iRodMaxLen += AllowedMaxLugExt[i][0];
								iRodMinLen += AllowedMinLugExt[i][0];
							}
							else 
							{
								iRodMaxLen += AllowedMaxLugExt[i][0];
								iRodMinLen += AllowedMinLugExt[i][0];
							}
						}//2
					}//4					
				}//5 //当前是L8(end)
				else
				{ //当前不是L8(start)
					if( Ptype[i].Left(2)==_T("L5") || Ptype[i].Left(2)==_T("L6") || Ptype[i].Left(2)==_T("L7") )
					{//2 //当前不是L8,是L5、L6、L7(start)
						if( Ptype[i] == _T("L7") || Ptype[i] == _T("L7Dd") )
						{
							iRodMaxLen = iRodMaxLen + AllowedMaxLugExt[i][0];
							iRodMinLen = iRodMinLen + AllowedMinLugExt[i][0];
						}
						else
						{
							//花篮螺丝或螺纹接头，plus下段和上段
							iRodMaxLen = iRodMaxLen + AllowedMaxLugExt[i][1] + AllowedMaxLugExt[i][0] ;
							iRodMinLen = iRodMinLen + AllowedMinLugExt[i][1] + AllowedMinLugExt[i][0] ;
						}
					}//2 //当前不是L8,是L5、L6、L7(end)
					else 
					{
						//当前不是L8,不是L5、L6、L7
						if( mlPartClassID[i] == iSPR )
						{
							//当前不是L8,不是L5、L6、L7,是弹簧
							if( Ptype[i] != _T("T3") && Ptype[i] != _T("T3A") && Ptype[i] != _T("T4") )
							{
								//当前不是L8,不是L5、L6、L7,是弹簧，且不是上下方弹簧、支架弹簧
								if( mlPartClassID[i - 1] == iROD )
								{
									SQLx = _T("SELECT * FROM [") + tbnSPRINGL5Crude + _T("] WHERE size2=") + ltos(mlPartDia[i - 1]);
									rsTmp->Open((_bstr_t)SQLx, _variant_t((IDispatch*)dbZDJcrude,true), 
										adOpenStatic, adLockOptimistic, adCmdText); 

									if( rsTmp->BOF && rsTmp->adoEOF )
									{
										sTmp.Format(GetResStr(IDS_NotFoundThisDia),EDIBgbl::GetDBName(dbZDJcrude),tbnSPRINGL5Crude,ltos(mlPartDia[i-1]));
										ShowMessage(sTmp);
									}
									else
									{
										//请看数据库定义说明书或江阴石化设备厂蝶簧样本p54
										rsTmp->get_Collect((_variant_t)_T("size4"),&vTmp);

										AllowedMinLugExt[i][0] = vtof(vTmp);
										rsTmp->get_Collect((_variant_t)_T("size5"),&vTmp1);
										rsTmp->get_Collect((_variant_t)_T("size3"),&vTmp2);
										rsTmp->get_Collect((_variant_t)_T("size2"),&vTmp3);
										AllowedMaxLugExt[i][0] =(vTmp1.vt==VT_NULL ? (vtof(vTmp2) - vtof(vTmp3)) / 2 : vtof(vTmp1));
										//插入长度必须<=拉杆上端螺纹长度
										if(AllowedMaxLugExt[i-1][0]<AllowedMaxLugExt[i][0])	AllowedMaxLugExt[i][0]=AllowedMaxLugExt[i-1][0];
										//因为弹簧或恒力弹簧的松紧螺母只能有一端可插入拉杆，故可插入长度即是sngDim。
										iRodMaxLen +=AllowedMaxLugExt[i][0];
										iRodMinLen +=AllowedMinLugExt[i][0];
									}
									rsTmp->Close();
								}
								else 
								{
									if( mlPartClassID[i + 1] == iROD )
									{
										//这是有松紧螺母的弹性件
										SQLx = _T("SELECT * FROM [") + tbnSPRINGL5Crude + _T("] WHERE size2=") + ltos(mlPartDia[i + 1]);
										rsTmp->Open((_bstr_t)SQLx, _variant_t((IDispatch*)dbZDJcrude,true), 
											adOpenStatic, adLockOptimistic, adCmdText); 
										if( rsTmp->BOF && rsTmp->adoEOF )
										{
											sTmp.Format(GetResStr(IDS_NotFoundThisDia),  EDIBgbl::GetDBName(dbZDJcrude), tbnSPRINGL5Crude,ltos(mlPartDia[i + 1])); ShowMessage(sTmp);
										}
										else 
										{
											//请看数据库定义说明书PHSdbManual.DOC或江阴石化设备厂蝶簧样本p54
											rsTmp->get_Collect((_variant_t)_T("size4"),&vTmp);
											AllowedMinLugExt[i][0] = vtof(vTmp);
											rsTmp->get_Collect((_variant_t)_T("size5"),&vTmp1);
											rsTmp->get_Collect((_variant_t)_T("size3"),&vTmp2);
											rsTmp->get_Collect((_variant_t)_T("size2"),&vTmp3);
											AllowedMaxLugExt[i][0] =(vTmp1.vt==VT_NULL ? (vtof(vTmp2) - vtof(vTmp3)) / 2 : vtof(vTmp1));
											//插入长度必须<=拉杆下端螺纹长度
											if(AllowedMaxLugExt[i+1][1]<AllowedMaxLugExt[i][0])	AllowedMaxLugExt[i][0]=AllowedMaxLugExt[i+1][1];
											//因为弹簧或恒力弹簧的松紧螺母只能有一端可插入拉杆，故可插入长度即是sngDim。
											iRodMaxLen +=AllowedMaxLugExt[i][0];
											iRodMinLen +=AllowedMinLugExt[i][0];
										}
										rsTmp->Close();
									}
								}
							}
							else
							{
								//当前不是L8,不是L5、L6、L7,是弹簧，是上下方弹簧、支架弹簧
								iRodMaxLen +=AllowedMaxLugExt[i][0];
								iRodMinLen +=AllowedMinLugExt[i][0];
							}
						}
						else 
						{
							//当前不是L8,不是L5、L6、L7,不是弹簧
							if( mlPartClassID[i] == iCSPR && Ptype[i] != _T("ZHB") )
							{
								//当前不是L8,不是L5、L6、L7,不是弹簧，是恒力弹簧，但不是ZHB
								if( Ptype[i].Find(_T("HA"))==1 || Ptype[i].Find(_T("HB"))==1 )
								{
									//PHA,LHA,PHB,LHB
									//拉杆连接恒吊，拉杆必须伸入恒吊一段长度PP,由getphsBHandSizes函数保存在A字段中
									//下面两句导致尺寸计算错误，恢复原状
									//iRodMaxLen = iRodMaxLen + AllowedMaxLugExt(i) + IIf(IsNull(rsTZB.Fields(_T("A"))), 0, rsTZB.Fields(_T("A")))
									//iRodMinLen = iRodMinLen + AllowedMinLugExt(i) + IIf(IsNull(rsTZB.Fields(_T("A"))), 0, rsTZB.Fields(_T("A")))
									
									//夏中伟改于2003.06.03原为下面两式
									//iRodMaxLen +=AllowedMaxLugExt[i][0];
									//iRodMinLen +=AllowedMinLugExt[i][0];
									iRodMaxLen +=AllowedMaxLugExt[i][0]+AllowedMaxLugExt[i][1];
									iRodMinLen +=AllowedMinLugExt[i][0]+AllowedMinLugExt[i][1];
								}
								else 
								{
									//因为弹簧或恒力弹簧的松紧螺母只能有一端可插入拉杆，故可插入长度。
									//在GetPhsBHandSizes中已经获得AllowedMaxLugExt,AllowedMinLugExt
									//iRodMaxLen +=AllowedMaxLugExt[i][0];//pfg2005.12.27原代码
									//iRodMinLen +=AllowedMinLugExt[i][0];//pfg2005.12.27原代码
									iRodMaxLen +=AllowedMaxLugExt[i][1];//pfg2005.12.27
									iRodMinLen +=AllowedMinLugExt[i][1];//pfg2005.12.27

								}
							}
							else 
							{
								//当前不是L8,不是L5、L6、L7,不是弹簧，不是(恒力弹簧，但不是ZHB)
								//管部、根部、拉杆
								if(mlPartClassID[i]==iROD)
								{
									//拉杆的伸出段长度在其它连接件/管部/根部中统计;
								}
								else//mlPartIndex[i-1]==iPA;
								{									
									iRodMaxLen +=AllowedMaxLugExt[i][0];
									iRodMinLen +=AllowedMinLugExt[i][0];
								}
							}
						}
					}
				}
				rsTZB->MoveNext();
			}


			//重新开始，为了修正拉杆两端伸入连接件的长度
			//因为其它情况已经在GetBoltsNutsAndAttachmentsCLgg处理
      
 			//拉杆长度=(根部标高-管部标高)*1000-其它零部件高度之和+拉杆长度调整值
			//此时拉杆总长度iSumRodLength仍然是一个最小值，即无论怎么调整花篮螺丝或其它，拉杆总长度都必须>=此值
			//拉杆总长度可以在iSumRodLength~iSumRodLength+iTotalDeltaRodLen之间调整。
			//拉杆圆整后的总长度必须位于这个区间。
			//iSumRodLength = iSumRodLength + iTotalDeltaRodLen
      
			//计算拉杆长度的安装和调节裕度，并且取整。（优易软件自行开发的最合理的拉杆长度圆整规则）
			//1，L5~L8、（弹簧、恒力弹簧类似于L5）都有可调整长度，即都有一个允许最小高度值和一个允许最大高度值；
			//2，由此可计算出拉杆长度的最大值和最小值
			//3，拉杆的长度从最大值开始圆整到其下最接近的数，
			//4，如果拉杆的最大值圆整后<=最小值
			
			//iTotalDeltaRodLen是最大与最小的差值，等于各零件的可调整间隙之和；
			//iDifRodLen=拉杆圆整前的最大长度-拉杆圆整后的长度；
			//圆整后，由于拉杆长度比最大长度要小，因此要重新计算拉杆在各零件的间隙中伸入的长度，以维持拉杆的实际长度与标高差不矛盾。
			//计算方法是，按照各零件间隙大小，将iDifRodLen按比例(正比于iTotalDeltaRodLen)分配到各间隙。			
			bLengthChanged = false;
			float fLenPlus=0;//加到拉杆上下方弹簧端的长度增加值
			
			//拉杆上方下方可调整时的iTotalDeltaRodLen1
			float iTotalDeltaRodLen1=0.0,iDifRodLen1=0.0;
			//最多圆整2次，适合于六道
			bool bRoundFA=false;
      
			do
			{
				iTotalDeltaRodLen = iRodMaxLen - iRodMinLen;
				iSumRodLength = iRodMaxLen;
				if( iTotalDeltaRodLen == 0 )
				{
					if(bHasPA || bHasSA)
						//可上下调整拉杆时，不必报警
						;
					else
						ShowMessage(GetResStr(IDS_totalDeltaLugLenEQV0));
				}

				if( (long)iRodMinLen % modPHScal::gnRodLenRoundVal == 0 )
				{//2
					//最小值可以整除
					if( int(iRodMaxLen / modPHScal::gnRodLenRoundVal) * modPHScal::gnRodLenRoundVal >= int(iRodMinLen / modPHScal::gnRodLenRoundVal) * modPHScal::gnRodLenRoundVal )
					{
						//初次圆整成功
						iDifRodLen = iSumRodLength - int(iRodMaxLen / modPHScal::gnRodLenRoundVal) * modPHScal::gnRodLenRoundVal;
						iSumRodLength = int(iRodMaxLen / modPHScal::gnRodLenRoundVal) * modPHScal::gnRodLenRoundVal;
						break;
					}
					else 
					{
						//不可能的情况!!!
						//初次圆整不成功
					}
				}//2
				else
				{//2
					//最小值不能整除
					if( int(iRodMaxLen / modPHScal::gnRodLenRoundVal) * modPHScal::gnRodLenRoundVal > int(iRodMinLen / modPHScal::gnRodLenRoundVal) * modPHScal::gnRodLenRoundVal )
					//if( iTotalDeltaRodLen >= modPHScal::gnRodLenRoundVal )
					{
						//圆整成功
						iDifRodLen = iSumRodLength - int(iRodMaxLen / modPHScal::gnRodLenRoundVal) * modPHScal::gnRodLenRoundVal;
						iSumRodLength = int(iRodMaxLen / modPHScal::gnRodLenRoundVal) * modPHScal::gnRodLenRoundVal;
						break;
					}
					else 
					{
						//初次圆整不成功
						//初次不能圆整成功,取最大最小的平均值
						iDifRodLen = iSumRodLength - int((iRodMaxLen + iRodMinLen) / 2);
						iSumRodLength = int((iRodMaxLen + iRodMinLen) / 2);
						sTmp.Format(GetResStr(IDS_CannotRoundSumLugLength),ftos(iRodMaxLen), ftos(iRodMinLen),ftos(iSumRodLength));
						ShowMessage(sTmp);
						break;
					}
				}//2
			}while(true);
      
            
			//首先按照拉杆长度是否可以调整排序，不能调整长度的拉杆排在最前面。
			//这样排序后，长度分配积累误差可以通过可调整螺纹接头消除。
			int  bA , bB; //A,B端可调整长度
			iNumRod = 0;
		
			rsTZB->MoveFirst();
			for( i = 0 ;i< iTZBRcount;i++)
			{//4
				bA = 1;
				bB = 1;
				if( mlPartClassID[i] == iROD )
				{//3
					iNumRod ++;
					if( i - 1 >= 0 )
					{
						if( mlPartIndex[i - 1] == iPA || Ptype[i-1] == _T("T3A") || Ptype[i-1] == _T("L6") )
						{
							bA = 0;
						}
					}
					if( i + 1 < iTZBRcount )
					{
						if( mlPartIndex[i + 1] == iSA || Ptype[i+1] == _T("T3") ||  Ptype[i+1] == _T("L6") )
						{
							bB = 0;
						}
					}
					rsTZB->put_Collect((_variant_t)_T("size"),_variant_t((float)(bA + bB)));
					rsTZB->Update();
				}//3
				else 
				{
					rsTZB->put_Collect((_variant_t)_T("size"),_variant_t(3.0));
					rsTZB->Update();
				}
				rsTZB->MoveNext();
			}//4
      
			//圆整后的拉杆长度差值按可调整间隙大小比例分配到伸入长度中
			//并且按两端分配和为10的倍数取整
			if(gnRodInsertLenRoundVal>1) 
			{
				CString sTmp;
				sTmp.Format(IDS_SetPoleInsertLenRoundVal,gnRodInsertLenRoundVal,gnRodInsertLenRoundVal);
				AfxMessageBox(sTmp);
			}
			iNumRod = 0;
			k = 0;//剩余总插入长度
			n = 0;//当前拉杆某端允许插入长度
			long  ns, ne; //下端分配总长度=ns(对应ZB表字段A),上端分配总长度=ne(对应ZB表字段B)
			//注意下句中必须是IsSAPart<>-1，而不能是NOT IsNull(seq)，因为根部也可能没有编号（公用根部），而根部要参与零件判断
			
			SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] WHERE zdjh=") + ltos(zdjh) + _T(" AND VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" AND NOT IsNull(ClassID) AND ClassID<>") + ltos(iAttached) + _T(" AND ClassID<>") + ltos(iBolts) + _T(" AND ClassID<>") + ltos(iNuts) + _T(" AND IsSAPart<>-1 AND (bUserAdd IS NULL OR bUserAdd=0) ORDER BY size");
			rsTmp->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
				adOpenStatic, adLockOptimistic, adCmdText); 
			long delta = 0;
			while(!rsTmp->BOF)
			{//9
				for( i = 0 ;i< iTZBRcount;i++)
				{
					rsTmp->get_Collect((_variant_t)_T("recno"),&vTmp);
					if( mlPartRecno[i] == vtoi(vTmp) )
						break;
				}
				_variant_t vTmp;
				rs->Find((_bstr_t)(_T("recno=") + ltos(mlPartRecno[i])), 0, adSearchForward);
				if( mlPartClassID[i] == iROD )
				{//8 //当前是拉杆(start)
					iNumRod ++;
					//清除头尾长度，以便保存值。
					ns = 0;
					ne = 0;
					//不是上方弹簧，可以在拉杆上、下端同时调整

					//夏中伟修改于2003.06.03,原为下式
					//if( Ptype[i - 1] == _T("T3A") || mlPartIndex[i - 1] == iPA || Ptype[i-1] == _T("L5") || Ptype[i-1] == _T("L6") || Ptype[i-1] == _T("L7") || Ptype[i-1] == _T("L8") || Ptype[i-1] == _T("L7Dd") || Ptype[i-1] == _T("L8Dd") || Ptype[i-1] == _T("T5") || Ptype[i-1] == _T("LHG") )
					//恒力弹簧HA，HB上面的拉杆插入恒力弹簧，长度要正确的话，必须加上Ptype[i - 1].Right (2) == _T("HA") || Ptype[i - 1].Right (2) == _T("HB")条件
					//T1,T2倒过来前面接管部后面接拉杆，必须加上Ptype[i -1] == _T("T1") || Ptype[i -1] == _T("T2") 条件，才能保证插入弹簧的拉杆长度是正确的
					if(Ptype[i -1] == _T("T1") || Ptype[i -1] == _T("T2") || Ptype[i - 1].Right (2) == _T("HA") || Ptype[i - 1].Right (2) == _T("HB") || Ptype[i - 1] == _T("T3A") || mlPartIndex[i - 1] == iPA || Ptype[i-1] == _T("L5") || Ptype[i-1] == _T("L6") || Ptype[i-1] == _T("L7") || Ptype[i-1] == _T("L8") || Ptype[i-1] == _T("L7Dd") || Ptype[i-1] == _T("L8Dd") || Ptype[i-1] == _T("T5") || Ptype[i-1] == _T("LHG") )
					{//4 当前是拉杆前面连接件为T1,T2,HA,HB,T3A,iPA,L5,L6,L7,L8,L7Dd,L8Dd,T5,LHG (start)
						if(false/* Ptype[i - 1] == _T("L6") */)
						{//1
							//2003.05.22李国斌注释：由于L6数据记录被开发人员误用，以致size4<size5(应该size4=size5)，导致拉杆长度计算出现误差值2*(size5-size4)。
							//解决办法就是把L6当成L5处理，可以确保计算误差<=2.
							n = AllowedMinLugExt[i-1][0];
							//不可圆整拉杆插入值
						}//1
						else 
						{//3
							if( iTotalDeltaRodLen == 0 )
							{//1
								n = 0;
							}//1
							else
							{//1
								if( Ptype[i - 1] == _T("L5")||Ptype[i - 1] == _T("L6") )
									//见2003.05.22李国斌注释：此处L6 是新增的条件
									n = int(0.5 + AllowedMaxLugExt[i-1][0] - (AllowedMaxLugExt[i-1][0] - AllowedMinLugExt[i-1][0]) * iDifRodLen / iTotalDeltaRodLen);
								else
									n = int(0.5 + AllowedMaxLugExt[i-1][0] - (AllowedMaxLugExt[i-1][0] - AllowedMinLugExt[i-1][0]) * iDifRodLen / iTotalDeltaRodLen);
							}//1
							if( n < AllowedMinLugExt[i-1][0] )
							{//1
								n = AllowedMinLugExt[i-1][0];
							}//1
							if((int(n/gnRodInsertLenRoundVal)+1)*gnRodInsertLenRoundVal<=AllowedMaxLugExt[i-1][0])
							{//1
								//如果拉杆插入长度向上圆整后不超过允许最大插入值，
								//则可以向上圆整
								n=(int(n/gnRodInsertLenRoundVal)+1)*gnRodInsertLenRoundVal;//圆整拉杆插入值到5、10,etc.
							}//1
							else
							{//2
								if(int(n/gnRodInsertLenRoundVal)*gnRodInsertLenRoundVal>=AllowedMinLugExt[i-1][0])
								{//1
									//如果拉杆插入长度向下圆整后不小于允许最小插入值
									n=int(n/gnRodInsertLenRoundVal)*gnRodInsertLenRoundVal;//圆整拉杆插入值到5、10,etc.
								}//1
								else
								{//1
									//无法圆整，不处理
								}//1
							}//2
							k = k + AllowedMaxLugExt[i-1][0] - n;
						}//3
						rsTZB->put_Collect((_variant_t)_T("A"),_variant_t((float)n));
						rsTZB->Update();
					}//4 当前是拉杆前面连接件为T1,T2,HA,HB,T3A,iPA,L5,L6,L7,L8,L7Dd,L8Dd,T5,LHG (end)	

					if( Ptype[i + 1] == _T("T3") || mlPartIndex[i + 1] == iSA || Ptype[i+1] == _T("L5") || Ptype[i+1] == _T("L6") || Ptype[i+1] == _T("L7") || Ptype[i+1] == _T("L8") || Ptype[i+1] == _T("L7Dd") || Ptype[i+1] == _T("L8Dd") || Ptype[i+1] == _T("T1") || Ptype[i+1] == _T("T2") || Ptype[i+1] == _T("T6") || mlPartClassID[i+1] == iCSPR && Ptype[i+1] != _T("ZHB") )
					{//7
						if( false /*Ptype[i + 1] == _T("L6")*/ )
						{//1
							//见2003.05.22李国斌注释：此处L6 是新增的条件
							n = AllowedMinLugExt[i+1][0];
						}//1
						else 
						{//6
							if( iNumRod < giNumRod )
							{//5
								if( iTotalDeltaRodLen == 0 )
								{//1
									n = 0;
								}//1
								else
								{//4
									if( Ptype[i + 1] == _T("L5") || Ptype[i + 1] == _T("L6")||mlPartClassID[i+1] == iCSPR && Ptype[i+1] != _T("ZHB"))//pfg2005.12.23增加了mlPartClassID[i+1] == iCSPR && Ptype[i+1] != _T("ZHB")
									{//3
										//见2003.05.22李国斌注释：此处L6 是新增的条件
										n = int(0.5 + AllowedMaxLugExt[i+1][1] - (AllowedMaxLugExt[i+1][1] - AllowedMinLugExt[i+1][1]) * iDifRodLen / iTotalDeltaRodLen);
										if( n < AllowedMinLugExt[i+1][1] )
										{//1
											n = AllowedMinLugExt[i+1][1];
										}//1
										if((int(n/gnRodInsertLenRoundVal)+1)*gnRodInsertLenRoundVal<=AllowedMaxLugExt[i+1][1])
										{//1
											//如果拉杆插入长度向上圆整后不超过允许最大插入值，
											//则可以向上圆整
											n=(int(n/gnRodInsertLenRoundVal)+1)*gnRodInsertLenRoundVal;//圆整拉杆插入值到5、10,etc.
										}//1
										else
										{//2
											if(int(n/gnRodInsertLenRoundVal)*gnRodInsertLenRoundVal>=AllowedMinLugExt[i+1][1])
											{//1
												//如果拉杆插入长度向下圆整后不小于允许最小插入值
												n=int(n/gnRodInsertLenRoundVal)*gnRodInsertLenRoundVal;//圆整拉杆插入值到5、10,etc.
											}//1
											else
											{//1
												//无法圆整，不处理
											}//1
										}//2

									}//3
									else
									{//3
										n = int(0.5 + AllowedMaxLugExt[i+1][0] - (AllowedMaxLugExt[i+1][0] - AllowedMinLugExt[i+1][0]) * iDifRodLen / iTotalDeltaRodLen);
										if( n < AllowedMinLugExt[i+1][0] )
										{//1
											n = AllowedMinLugExt[i+1][0];
										}//1

										if((int(n/gnRodInsertLenRoundVal)+1)*gnRodInsertLenRoundVal<=AllowedMaxLugExt[i+1][0])
										{//1
											//如果拉杆插入长度向上圆整后不超过允许最大插入值，
											//则可以向上圆整
											n=(int(n/gnRodInsertLenRoundVal)+1)*gnRodInsertLenRoundVal;//圆整拉杆插入值到5、10,etc.
										}//1
										else
										{//2
											if(int(n/gnRodInsertLenRoundVal)*gnRodInsertLenRoundVal>=AllowedMinLugExt[i+1][0])
											{//1
												//如果拉杆插入长度向下圆整后不小于允许最小插入值
												n=int(n/gnRodInsertLenRoundVal)*gnRodInsertLenRoundVal;//圆整拉杆插入值到5、10,etc.
											}//1
											else
											{//1
												//无法圆整，不处理
											}//1
										}//2
									}//3
								}//4
							}//5
							else
							{//4
								//最后一个，
								if( Ptype[i + 1] == _T("L5") || Ptype[i + 1] == _T("L6")||mlPartClassID[i+1] == iCSPR && Ptype[i+1] != _T("ZHB"))//增加了mlPartClassID[i+1] == iCSPR && Ptype[i+1] != _T("ZHB")
								{//3
									//见2003.05.22李国斌注释：此处L6 是新增的条件
									n = AllowedMaxLugExt[i+1][1] - (iDifRodLen - k);
									if( n > AllowedMaxLugExt[i+1][1] )
									{//2
										//如果最后一个拉杆下端插入值超过了允许最大插入值，则把拉杆向上移动两者之间的差值
										//同时保证拉杆上端插入值不超过允许最大插入值
										delta = n - AllowedMaxLugExt[i+1][1];
										n = AllowedMaxLugExt[i+1][1];
									
										_variant_t tmp;
										rsTZB->get_Collect((_variant_t)_T("B"), &tmp);
										float b = vtof(tmp);

										if( b + delta <= AllowedMaxLugExt[i+1][0])
										{//1
											rsTZB->put_Collect((_variant_t)_T("B"),_variant_t((float)(b+delta))); 
											rsTZB->Update();
										}//1
										else
										{//1
											CString sTmp;
											sTmp.Format(IDS_OverL5AllowedMaxRodExtOfxEnd,_T("Down"),n,(int)AllowedMaxLugExt[i+1][1]);
											AfxMessageBox(sTmp);
										}//1
									}//2
									if((int(n/gnRodInsertLenRoundVal)+1)*gnRodInsertLenRoundVal<=AllowedMaxLugExt[i+1][1])
									{//1
										//如果拉杆插入长度向上圆整后不超过允许最大插入值，
										//则可以向上圆整
									   //n=(int(n/gnRodInsertLenRoundVal)+1)*gnRodInsertLenRoundVal;//原代码 pfg2005.12.23圆整拉杆插入值到5、10,etc.
									}//1
									else
									{//2
										if(int(n/gnRodInsertLenRoundVal)*gnRodInsertLenRoundVal>=AllowedMinLugExt[i+1][1])
										{//1
											//如果拉杆插入长度向下圆整后不小于允许最小插入值
											//n=int(n/gnRodInsertLenRoundVal)*gnRodInsertLenRoundVal;//原代码pfg2005.12.23圆整拉杆插入值到5、10,etc.
										}//1
										else
										{//1
											//无法圆整，不处理
										}//1
									}//2
								}//3
								else
								{//3
									n = AllowedMaxLugExt[i+1][0] - (iDifRodLen - k);
									if( n > AllowedMaxLugExt[i+1][0] )
									{//2
										delta = n - AllowedMaxLugExt[i+1][0];
										n = AllowedMaxLugExt[i+1][0];
									
										_variant_t tmp;
										rsTZB->get_Collect((_variant_t)_T("B"), &tmp);
										float a = vtof(tmp);

										if( a + delta <= AllowedMaxLugExt[i+1][1])
										{//1
											rsTZB->put_Collect((_variant_t)_T("B"),_variant_t((float)(a+delta))); 
											rsTZB->Update();
										}//1
										else
										{//1
											CString sTmp;
											sTmp.Format(IDS_OverL5AllowedMaxRodExtOfxEnd,_T("Down"),n,(int)AllowedMaxLugExt[i+1][1]);
											AfxMessageBox(sTmp);
										}//1	
									}//2
									if( n < AllowedMinLugExt[i+1][0] )
									{//1
										//应该报警，拉杆长度插入值小于最小允许值
										n = AllowedMinLugExt[i+1][0];
									}//1
									if((int(n/gnRodInsertLenRoundVal)+1)*gnRodInsertLenRoundVal<=AllowedMaxLugExt[i+1][0])
									{//1
										//如果拉杆插入长度向上圆整后不超过允许最大插入值，
										//则可以向上圆整
										//n=(int(n/gnRodInsertLenRoundVal)+1)*gnRodInsertLenRoundVal;//原代码pfg2005.12.23圆整拉杆插入值到5、10,etc.
									
									}//1
									else
									{//2
										if(int(n/gnRodInsertLenRoundVal)*gnRodInsertLenRoundVal>=AllowedMinLugExt[i+1][0])
										{//1
											//如果拉杆插入长度向下圆整后不小于允许最小插入值
											//n=int(n/gnRodInsertLenRoundVal)*gnRodInsertLenRoundVal;//原代码pfg2005.12.23圆整拉杆插入值到5、10,etc.
										}//1
										else
										{//1
											//无法圆整，不处理
										}//1
									}//2
								}//3
							}//4
							//pfg2005.12.23 这种处理办法很不好不合符一般的思维方式，我也没办法因为重新修改一下
							//会用很多时间，还有拉杆插入的最小长度没有很好地解决
							if(mlPartClassID[i+1] == iCSPR && Ptype[i+1] != _T("ZHB")||Ptype[i + 1] == _T("L5"))//pfg2005.12.23//pfg2005.12.26增加Ptype[i + 1] == _T("L5")
							{
								k = k + AllowedMaxLugExt[i+1][1] - n;//pfg2005.12.23
							}
							else
							{
								k = k + AllowedMaxLugExt[i+1][0] - n;
							}
						}//6
						rsTZB->put_Collect((_variant_t)_T("B"),_variant_t((float)n)); rsTZB->Update();
					}//7
				}//8
				rsTmp->MoveNext();
			}//9
			rsTmp->Close();   

		
		//计算并且分配每根拉杆最小长度，然后按最小长度排序，优先处理最小长度最大的拉杆。
		//先将最小长度保存在size字段
   
		rsTZB->MoveFirst();
		iNumRod = 1;
		for( i = 0 ;i< iTZBRcount;i++)
		{
			rsTZB->get_Collect((_variant_t)_T("CustomID"),&vTmp);
			tbn1 = sFindTBN(vtos(vTmp));
			if( mlPartClassID[i] == iROD )
			{//5 当前是拉杆(start)
				//获得拉杆数据
				SQLx = _T("SELECT * FROM [") + tbnLugInfo + _T("] WHERE (CustomID)=\'") +vtos(vTmp) + _T("\'");
				rs->Open((_bstr_t)SQLx, dbZDJcrude.GetInterfacePtr(), 
					adOpenDynamic, adLockOptimistic, adCmdText); 
				if( rs->BOF && rs->adoEOF )
				{
					sTmp.Format(GetResStr(IDS_NoLugCustomIDInLugINFO), EDIBgbl::GetDBName(dbZDJcrude),tbnLugInfo,SQLx);
					throw sTmp;
				}
				rs->MoveFirst();
				//拉杆最小长度
				rs->get_Collect((_variant_t)_T("minLength"),&vTmp);
				MinLength = vtof(vTmp);
				//拉杆最大长度
				rs->get_Collect((_variant_t)_T("maxLength"),&vTmp);
				MaxLength = vtof(vTmp);
				//拉杆增量长度起点1，比如若从最小长度200开始增量为100,
				//直到1000，增量又为500，直到最大长度2000止。
				//则IncrementStart1=200,IncrementLength1=100
				//则IncrementStart2=1000,IncrementLength2=500
				rs->get_Collect((_variant_t)_T("IncrementStart1"),&vTmp);
				if( vtof(vTmp) <= MinLength )
				{
					IncrementStart1 = MinLength;
				}
				else
				{
					IncrementStart1 = vtof(vTmp);
				}
				rs->get_Collect((_variant_t)_T("IncrementLength1"),&vTmp);
				IncrementLength1 = vtof(vTmp);
				//拉杆增量长度起点2，比如从1000开始增量为500，
				rs->get_Collect((_variant_t)_T("IncrementStart2"),&vTmp);
				if( vtof(vTmp) <= MinLength )
				{
					IncrementStart2 = MaxLength;
				}
				else
				{
					IncrementStart2 =vtof(vTmp);
				}
				rs->get_Collect((_variant_t)_T("IncrementLength2"),&vTmp);
				if( vtof(vTmp) >= MaxLength - MinLength )
				{
					IncrementLength2 = 0;
				}
				else 
				{
					IncrementLength2 = vtof(vTmp);
				}
				if( IncrementStart2 == MaxLength )
				{
					IncrementLength2 = 0;
				}
         
				//置初始值tempMinLength=0
				tempMinLength = 0;
				if( i >= 1 )
				{//4
					if( Ptype[i - 1] == _T("T3A") )
					{//1
						//前一个是下方弹簧
						rsTZB->get_Collect((_variant_t)_T("A"),&vTmp);
						tempMinLength = fabs(tempT3A) + vtof(vTmp);
						/*if( mlPartIndex[i - 2] = iPA && i - 2 >= 0 ){
							//前一个是下方弹簧，再前一个是管部
							//加上槽钢的高度
						}else {
							//前一个是下方弹簧，再前一个不是管部
							//不可能
						}*/
					}//1
					else {//1
						//前一个不是下方弹簧
						/*if( mlPartIndex[i - 1] == iPA ){
							//前一个不是下方弹簧,是管部
						}else {
							//前一个不是下方弹簧,不是管部
							if( Ptype(i - 1) Like _T("L[5-8]") ){
								//前一个不是下方弹簧,不是管部，是螺纹连接件
							}else {
							}
						}*/
						rsTZB->get_Collect((_variant_t)_T("A"),&vTmp);
						tempMinLength = vtof(vTmp);
					}//1
            
					if( i + 1 <= iTZBRcount - 1 )
					{//3
						if( Ptype[i + 1] == _T("T3") )
						{//2
							//后一个是上方弹簧
							if( mlPartIndex[i + 2] == iSA && i + 2 <= iTZBRcount - 1 )
							{//1
								//后一个是上方弹簧,再后一个是根部
								rsTZB->get_Collect((_variant_t)_T("B"),&vTmp);
								tempMinLength = tempMinLength + fabs(tempT3) + vtof(vTmp) + Cavphs->SACSHeight;
							}//1
							//else {
								//后一个是上方弹簧,再后一个不是根部
                     
							//}
						}//2
						else
						{//2
							//后一个不是上方弹簧
							if( mlPartIndex[i + 1] == iSA )
							{//1
								//后一个不是上方弹簧,后一个是根部
								rsTZB->get_Collect((_variant_t)_T("C"),&vTmp);
								tempMinLength = tempMinLength + vtof(vTmp) + Cavphs->SACSHeight;
							}//1
							else
							{//1
								//后一个不是上方弹簧,后一个不是根部
								/*if( Ptype(i + 1) Like _T("L[5-8]") ){
									//后一个不是上方弹簧,不是根部，是螺纹连接件
                        
								}else {
									//后一个不是上方弹簧,不是根部，不是螺纹连接件
                        
								}*/
								rsTZB->get_Collect((_variant_t)_T("C"),&vTmp);
								tempMinLength = tempMinLength + vtof(vTmp);
							}//1
						}//2
					}//3
				}//4
				tempMinLength = (tempMinLength > MinLength ? tempMinLength : MinLength);
				//最小尺寸临时保存在拉杆size字段
				rsTZB->put_Collect((_variant_t)_T("size"),_variant_t((float)tempMinLength)); rsTZB->Update();
				rs->Close();
			}//5当前是拉杆(end)
			rsTZB->MoveNext();
		}     
		//计算并且分配每根拉杆长度
		SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] WHERE zdjh=") + ltos(zdjh) + _T(" AND VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" AND NOT IsNull(ClassID) AND ( ClassID=") + ltos(iROD) + _T(" OR ClassID=") + ltos(iXL1) + _T(") AND NOT IsNull(SEQ) ORDER BY C DESC");
		rsTmp->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
			adOpenStatic, adLockOptimistic, adCmdText); 
		rsTmp->MoveLast();
		rsTmp->MoveFirst();
		iNumRod = 1;
		for( i = 0 ;i< rsTmp->RecordCount ;i++)
		{
				//获得拉杆数据
				rsTmp->get_Collect((_variant_t)_T("CustomID"),&vTmp);
				SQLx = _T("SELECT * FROM [") + tbnLugInfo + _T("] WHERE (CustomID)=\'") + vtos(vTmp) + _T("\'");
				rsTmp->Open((_bstr_t)SQLx, _variant_t((IDispatch*)dbZDJcrude,true), 
					adOpenDynamic, adLockOptimistic, adCmdText); 
				if( rs->BOF && rs->adoEOF )
				{
					sTmp.Format(GetResStr(IDS_NoLugCustomIDInLugINFO),EDIBgbl::GetDBName(dbZDJcrude),tbnLugInfo,SQLx);
					throw sTmp;
				}
				rs->MoveFirst();
				//拉杆最小长度
				rs->get_Collect((_variant_t)_T("minLength"),&vTmp);
				MinLength = vtof(vTmp);
				rs->get_Collect((_variant_t)_T("maxLength"),&vTmp);
				//拉杆最大长度
				MaxLength =vtof(vTmp);
				//拉杆增量长度起点1，比如若从最小长度200开始增量为100,
				//直到1000，增量又为500，直到最大长度2000止。
				//则IncrementStart1=200,IncrementLength1=100
				//则IncrementStart2=1000,IncrementLength2=500
				rs->get_Collect((_variant_t)_T("IncrementStart1"),&vTmp);
				if( vtof(vTmp)<= MinLength )
				{
					IncrementStart1 = MinLength;
				}
				else
				{
					IncrementStart1 = vtof(vTmp);
				}
				rs->get_Collect((_variant_t)_T("IncrementLength1"),&vTmp);
				IncrementLength1 = vtof(vTmp);
				//拉杆增量长度起点2，比如从1000开始增量为500，
				rs->get_Collect((_variant_t)_T("IncrementStart2"),&vTmp);
				if(  vtof(vTmp)<= MinLength ){
					IncrementStart2 = MaxLength;
				}else {
					IncrementStart2 = vtof(vTmp);
				}
				rs->get_Collect((_variant_t)_T("IncrementLength2"),&vTmp);
				if( vtof(vTmp) >= MaxLength - MinLength ){
					IncrementLength2 = 0;
				}else {
					IncrementLength2 = vtof(vTmp);
				}
				if( IncrementStart2 == MaxLength ){ //2007.09.14 "="改为"=="
					IncrementLength2 = 0;
				}
         
				//ReDim Preserve RodLen(iNumRod)
				//尽量使用最大长度的拉杆，同时
				//使每节的长度尽量接近平均长度。
				sngLugAvgLen = (iSumRodLength - sngTmpSumLen) / (giNumRod - iNumRod + 1);
				//记住每根拉杆的最小允许长度
				rsTmp->get_Collect((_variant_t)_T("size"),&vTmp);
				tempMinLength = vtof(vTmp);
				if( iNumRod != giNumRod )
				{
				//使每节的长度尽量接近平均长度。
				//需要一个循环测试：从最小长度开始直到它超过平均长度
         
					while( RodLen[iNumRod] < sngLugAvgLen || RodLen[iNumRod] < tempMinLength)
					{//4
						if( RodLen[iNumRod] < MinLength )
						{//1
							RodLen[iNumRod] = MinLength;
						}//1
						else 
						{//3
							if( RodLen[iNumRod] > MaxLength )
							{//1
								sTmp.Format(GetResStr(IDS_LugLengthBiggerMaxLength), ftos(RodLen[iNumRod]),ftos(MaxLength));
								throw sTmp;
							}//1
							else 
							{//2
								if( RodLen[iNumRod] >= IncrementStart2 )//2007.09.14 ">"改为">="
								{//1
									//如果下一步将要超过平均长度，并且要超过最小允许长度，则退出。
									if( RodLen[iNumRod] + IncrementLength2 / (giNumRod - iNumRod + 1) > sngLugAvgLen && RodLen[iNumRod] >= tempMinLength )
										break;
									RodLen[iNumRod] = RodLen[iNumRod] + IncrementLength2;
									//2007.09.14(start)
									if( RodLen[iNumRod] >=(long)MaxLength )
									{
										break;
									}
									//2007.09.14(end)
								}//1
								else 
								{//1
									//如果下一步将要超过平均长度，并且要超过最小允许长度，则退出。
									if( RodLen[iNumRod] + IncrementLength1 / (giNumRod - iNumRod + 1) > sngLugAvgLen && RodLen[iNumRod] >= tempMinLength )
										break;
									RodLen[iNumRod] = RodLen[iNumRod] + IncrementLength1;
								}//1
							}//2
						}//3
					}//4
					sngTmpSumLen = sngTmpSumLen + RodLen[iNumRod];
				}
				else 
				{
					//最后一节长度=剩余
					RodLen[iNumRod] = iSumRodLength - sngTmpSumLen;
					if( RodLen[iNumRod] < MinLength )
					{
						//吊杆长度太短或吊杆节数太多
						if( RodLen[iNumRod] >= giMinRodLen )
						{
							//如果拉杆长度>=100
							RodLen[iNumRod] = RodLen[iNumRod];
						}
						else
						{
							sTmp.Format(GetResStr(IDS_LugLengthLessthanMinLength),ltos(sngLugAvgLen),ltos(MinLength));
							throw sTmp;
						}
					}
					else 
					{
						if( RodLen[iNumRod] > MaxLength )
						{
							//吊杆长度太长或吊杆节数太少
							sTmp.Format(GetResStr(IDS_LugLengthBiggerMaxLength),ltos(sngLugAvgLen),ltos(MaxLength));
							throw sTmp;
						}
					}
				}

				//获得拉杆规格->sBH
				rsTmp->get_Collect((_variant_t)_T("CustomID"),&vTmp);
				sBHFormat = vtos(sFindFLD(_T("CustomID"), _T("BHformat"), vtos(vTmp)));
				rsTmp->get_Collect((_variant_t)_T("CLgg"),&vTmp);
				sBH = vtos(vTmp);
				SQLx = _T("UPDATE tmpCSLen SET BH=\'") + vtos(vTmp) + _T("\'");
				EDIBgbl::dbPRJ->Execute((_bstr_t)SQLx, NULL, adCmdText);
				_variant_t vBH = vTmp;

				SQLx = _T("UPDATE tmpCSLen SET L1=") + ftos(RodLen[iNumRod]);
				EDIBgbl::dbPRJ->Execute((_bstr_t)SQLx, NULL, adCmdText);
				mvL = CSLength(sBHFormat, _T("L1"), sBH);
				rsTmp->get_Collect((_variant_t)_T("recno"),&vTmp);
				_variant_t vTmp3;
				_variant_t vTmp;
				rsTZB->Find((_bstr_t)(_T("recno=") + vtos(vTmp)), 0, adSearchForward);
				if(!rsTZB->adoEOF)
				{
					rsTZB->get_Collect((_variant_t)_T("GDW1"),&vTmp);
					rsTZB->get_Collect((_variant_t)_T("CLnum"),&vTmp1);
					rsTmp->get_Collect((_variant_t)_T("B"),&vTmp2);
					rsTmp->get_Collect((_variant_t)_T("A"),&vTmp3);
					rsTZB->put_Collect((_variant_t)_T("BH"),vBH);
					
					//2007.09.12(start)
					if( iNumRod<giNumRod )
					{
						iSumRodLength1-=RodLen[iNumRod]-vtof(vTmp3)-vtof(vTmp2);
					}
					else if( iNumRod==giNumRod )
					{
						if(iSumRodLength1 != RodLen[iNumRod] - vtof(vTmp3)-vtof(vTmp2))
						{
							RodLen[iNumRod] = (int)iSumRodLength1+vtof(vTmp3)+vtof(vTmp2);
							//发现拉杆圆整时在尾数为9时比组装图上少1毫米，尾数为1时多1毫米
							//故作如下处理 modify by ligb luorijin   2008.10.16
							//iSumRodLength1是Float型，取整之后就会把拉杆的实际长度的小数部分去掉，导致拉杆圆整值总是比实际值少1
							if (RodLen[iNumRod]%10 == 9||iSumRodLength1 - (int)iSumRodLength1 >= 0.1)
							{
								RodLen[iNumRod] = RodLen[iNumRod] + 1;
							}
							else if(RodLen[iNumRod]%10 == 1)
							{
								RodLen[iNumRod] = RodLen[iNumRod]-1;
							}
							
						}
						SQLx = _T("UPDATE tmpCSLen SET L1=") + ftos(RodLen[iNumRod]);
						EDIBgbl::dbPRJ->Execute((_bstr_t)SQLx, NULL, adCmdText);
						mvL = CSLength(sBHFormat, _T("L1"), sBH);
					}
					//2007.09.12(end)

					rsTZB->put_Collect((_variant_t)_T("sizeH"),_variant_t((double)RodLen[iNumRod]));
					//标注尺寸=实际长度-两端调整长度
					rsTZB->put_Collect((_variant_t)_T("sizeDIM"),_variant_t((long)(RodLen[iNumRod] - vtof(vTmp3)-vtof(vTmp2))));
					//拉杆作为零件计算整体单重. 单重/m由Cphs.GetPhsBHandSizes方法保存在rsTzb.Fields(_T("GDW1"))
					//gbConnectHoleDimMatch
					rsTZB->put_Collect((_variant_t)_T("L1"),_variant_t((float)RodLen[iNumRod]));
					rsTZB->put_Collect((_variant_t)_T("CLdz"),_variant_t((float)(RodLen[iNumRod] / 1000.0 * vtof(vTmp))));					
					rsTZB->put_Collect((_variant_t)_T("CLzz"),_variant_t((float)(RodLen[iNumRod] / 1000.0 * vtof(vTmp) * vtof(vTmp1))));
					rsTZB->put_Collect((_variant_t)_T("CLgg"),STR_VAR(sBH));
					rsTZB->Update();
				}
				//Debug.Print RodLen(iNumRod)
				iNumRod++;
				rs->Close();
				rsTmp->MoveNext();
			}
			rsTmp->Close();
		}
		ret=true;
	}
	catch(CString e)
	{
		ShowMessage(e);

	}
	catch(CException e)
	{
		AfxMessageBox(_T("Other general error!"));
	}
	if(Ptype!=NULL)
		delete [] Ptype;
   if(mlPartClassID!=NULL)
		delete [] mlPartClassID;
   if(mlPartIndex!=NULL)
		delete [] mlPartIndex;
   if(mlPartDia!=NULL)
		delete [] mlPartDia;
   if(mlPartRecno!=NULL)
		delete [] mlPartRecno;
   if(mvActualLugExt!=NULL)
		delete [] mvActualLugExt;
   if(AllowedMinLugExt!=NULL)
		delete [] AllowedMinLugExt;
   if(AllowedMaxLugExt!=NULL)
		delete [] AllowedMaxLugExt;
   if(PreviousPartInsertLen!=NULL)
		delete [] PreviousPartInsertLen;
	return ret;
}

void modPHScal::SetBasePoint()
{
	try
	{
	   CalSpringParallelNum();
	   CString FDp, FDp1;
	   _RecordsetPtr rs;
	   rs.CreateInstance(__uuidof(Recordset));
	   CString SQLx;
	   SQLx = _T("SELECT * FROM phsDrawPos");
	   rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPHScode,true), 
		   adOpenKeyset, adLockOptimistic, adCmdText); 
	   if (giDrawSIZE_A3A4 == 2)
	   {
		  //图幅自动
		  if( sngSEL <= sngPEL)
			 //支架，A4
			 Ax = _T("A4");
		  else
			 //吊架，A3
			 Ax = _T("A3");
	   }
	   else
	   {
		  if( giDrawSIZE_A3A4 == 1)
			 //强制使用A4
			 Ax = _T("A4");
		  else
			 //强制使用A3
			 Ax = _T("A3");
	   }
	   if( bPAIsHanger() )
	   {
		  //吊架
		  if( gintParallelNum == 1)
			 //单吊
			 FDp1 = _T("1");
		  else if( gintParallelNum == 2)
				//双吊
				FDp1 = _T("2");
			 //Else
				//未知
			 //End If
	   }
	   else
	   {
		  //支架
		  FDp1 = _T("S");
	   }
   
	   FDp = FDp1 + _T("_") + (gbDraw_BOM? _T("bom"): _T(""));
	   FDp.MakeUpper();
	   FDp.TrimLeft();
	   FDp.TrimRight();
	   _variant_t vTmp;

	   rs->Find((_bstr_t)(_T("((name))=\'") + (FDp) + (_T("VX_pt0x")) + _T("\'")), 0, adSearchForward);
	   if(!rs->adoEOF)
	   {
		   rs->get_Collect((_variant_t)(Ax+_T("Pos")),&vTmp);
		   VX_pt0x=vtof(vTmp);
	   }
   
	   rs->Find((_bstr_t)(_T("((name))=\'") + (FDp) + (_T("VX_pt0y")) + _T("\'")), 0, adSearchForward);
	   if(!rs->adoEOF)
	   {	   
		   rs->get_Collect((_variant_t)(Ax+_T("Pos")),&vTmp);
			VX_pt0y=vtof(vTmp);
	   }
   
	   rs->Find((_bstr_t)(_T("((name))=\'") + (FDp) + (_T("VZ_pt0x")) + _T("\'")), 0, adSearchForward);
	   if(!rs->adoEOF)
	   {
		   rs->get_Collect((_variant_t)(Ax+_T("Pos")),&vTmp);
		   VZ_pt0x=vtof(vTmp);
	   }
   
	   rs->Find((_bstr_t)(_T("((name))=\'") + (FDp) + (_T("pt1x")) + _T("\'")), 0, adSearchForward);
	   if(!rs->adoEOF)
	   {
		   rs->get_Collect((_variant_t)(Ax+_T("Pos")),&vTmp);
		   pt1x=vtof(vTmp);
	   }
   
	   rs->Find((_bstr_t)(_T("((name))=\'") + (FDp) + (_T("pt1y")) + _T("\'")), 0, adSearchForward);
	   if(!rs->adoEOF)
	   {
		   rs->get_Collect((_variant_t)(Ax+_T("Pos")),&vTmp);
		   pt1y=vtof(vTmp);
	   }

	   rs->Find((_bstr_t)(_T("((name))=\'") + (FDp) + (_T("pt2x")) + _T("\'")), 0, adSearchForward);
	   if(!rs->adoEOF)
	   {
		   rs->get_Collect((_variant_t)(Ax+_T("Pos")),&vTmp);
		   pt2x=vtof(vTmp);
	   }
   
	   rs->Find((_bstr_t)(_T("((name))=\'") + (FDp) + (_T("pt2y")) + _T("\'")), 0, adSearchForward);
	   if(!rs->adoEOF)
	   {
		   rs->get_Collect((_variant_t)(Ax+_T("Pos")),&vTmp);
		   pt2y=vtof(vTmp);
	   }

	   rs->Find((_bstr_t)(_T("((name))=\'") + (FDp) + (_T("pt3x")) + _T("\'")), 0, adSearchForward);
	   if(!rs->adoEOF)
	   {
		   rs->get_Collect((_variant_t)(Ax+_T("Pos")),&vTmp);
		   pt3x=vtof(vTmp);
	   }
   
	   rs->Find((_bstr_t)(_T("((name))=\'") + (FDp) + (_T("pt3y")) + _T("\'")), 0, adSearchForward);
	   if(!rs->adoEOF)
	   {
		   rs->get_Collect((_variant_t)(Ax+_T("Pos")),&vTmp);
		   pt3y=vtof(vTmp);
	   }

	   rs->Find((_bstr_t)(_T("((name))=\'") + (FDp1) + _T("_") + (_T("pt0x")) + _T("\'")), 0, adSearchForward);
	   if(!rs->adoEOF)
	   {
		   rs->get_Collect((_variant_t)(Ax+_T("Pos")),&vTmp);
		   Pt0X=vtof(vTmp);
	   }

	   rs->Find((_bstr_t)(_T("((name))=\'GIDIMOFFSET\'")), 0, adSearchForward);
	   if(!rs->adoEOF)
	   {
		   rs->get_Collect((_variant_t)(Ax+_T("Pos")),&vTmp);
		   giDimOffset=vtof(vTmp);
	   }

	   rs->Find((_bstr_t)(_T("((name))=\'GIATTDOFFSETX\'")), 0, adSearchForward);
	   if(!rs->adoEOF)
	   {
		   rs->get_Collect((_variant_t)(Ax+_T("Pos")),&vTmp);
		   giAttDxOffsetX=vtof(vTmp);
	   }
	   giAttDzOffsetX = giAttDxOffsetX;

	   rs->Find((_bstr_t)(_T("((name))=\'GIATTDOFFSETY\'")), 0, adSearchForward);
	   if(!rs->adoEOF)
	   {
		   rs->get_Collect((_variant_t)(Ax+_T("Pos")),&vTmp);
		   giAttDOffsetY=vtof(vTmp);
	   }
	 }
	 catch (_com_error &e)
	 {
		 CString strMsg;
		 strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		 AfxMessageBox(strMsg);
	 }
}

void modPHScal::ReadAutoPHSiniValue()
{
	gbSumRodByRoundSteel = GetRegKey((_T("Settings")), _T("SumRodByRoundSteel"), 0);

	iAlignLoad=GetRegKey((_T("Settings")),(_T("CADAlignLoad")),0);
	iAlignDim=GetRegKey((_T("Settings")),(_T("CADAlignDim")),0);
	iAlignLjmx=GetRegKey((_T("Settings")),(_T("CADAlignLjmx")),0);
	strBigFontLoad=GetRegKey((_T("Settings")),(_T("CADBigFontLoad")),CString((_T("hzfs"))));
	strSHXFontLoad=GetRegKey((_T("Settings")),(_T("CADSHXFontLoad")),CString((_T("hzfs"))));
	fFWLoad=GetRegKey((_T("Settings")),(_T("CADFontWidthFactorLoad")),1.0f);

	strBigFontLjmx=GetRegKey((_T("Settings")),(_T("CADBigFontLjmx")),CString((_T("hzfs"))));
	strBigFontDim=GetRegKey((_T("Settings")),(_T("CADBigFontDim")),CString((_T("hzfs"))));
	fFWDim=GetRegKey((_T("Settings")),(_T("CADFontWidthFactorDim")),1.0f);
	strSHXFontLjmx=GetRegKey((_T("Settings")),(_T("CADSHXFontLjmx")),CString((_T("txt"))));
	strSHXFontDim=GetRegKey((_T("Settings")),(_T("CADSHXFontDim")),CString((_T("txt"))));
	fFWLjmx=GetRegKey((_T("Settings")),(_T("CADFontWidthFactorLjmx")),1.0f);

	gUseSpringMinDist = GetRegKey((_T("Settings")), _T("UseSpringMinDist"),float(0));
	gnSPR_CHZBH = GetRegKey((_T("Settings")), _T("SPR_Chzbh"), (float)0.35);
	gnSEQyOffset = GetRegKey((_T("Settings")), _T("SEQyOffset"), 8);
	gsngRatioOfPjg2Pgz = GetRegKey((_T("Settings")), _T("Pjg2Pgz"), (float)1.4);
	gnConnectTJ = GetRegKey((_T("Settings")), _T("ConnectTJ"), (float)200);
	gnRodLenRoundVal = GetRegKey((_T("Settings")), _T("RodLenRoundVal"), (int)100);
	gnRodInsertLenRoundVal = GetRegKey((_T("Settings")), _T("RodInsertLenRoundVal"), (int)1);
	if (gnRodLenRoundVal<1)
	{
		gnRodLenRoundVal=100;
		SetRegValue(_T("Settings"), _T("RodLenRoundVal"), gnRodLenRoundVal);
	}
	if (gnRodInsertLenRoundVal<1)
	{
		gnRodInsertLenRoundVal=1;
		SetRegValue(_T("Settings"), _T("RodInsertLenRoundVal"), gnRodInsertLenRoundVal);
	}
	
	gbConnectHoleDimMatch = GetRegKey((_T("Settings")), _T("ConnectHoleDimMatch"), -1);
	gbPipeClampBolt = GetRegKey((_T("Settings")), _T("PipeClampBolt"), -1);//Modified by Shuli Luo
	gbSPRAutoLugDia = GetRegKey((_T("Settings")), _T("SPRAutoLugDia"), -1);
	gbCSPRAutoLugDia = GetRegKey((_T("Settings")), _T("CSPRAutoLugDia"), 0);
	gbAutoSPRRodDiaDecrease=GetRegKey((_T("Settings")), _T("AutoSPRRodDiaDecrease"), 0);
	gbSPRBHLOADBYInstallLoad=GetRegKey((_T("Settings")), _T("SPRBHLOADBYInstallLoad"), 0);
	gbSPRBHLOADUNITBYNEWTON=GetRegKey((_T("Settings")), _T("SPRBHLOADUNITBYNEWTON"), 0);
	gbAutoApplyCSPRLugDia = GetRegKey((_T("Settings")), _T("AutoApplyCSPRLugDia"), -1);
	AutoOrdinateDrawing = GetRegKey((_T("Settings")), _T("AutoTZ"), true);
	gbStopOLE = GetRegKey(_T("Settings"), _T("StopOLE"), -1);
	gbAutoApplyChzbh = GetRegKey((_T("Settings")), _T("AutoApplyChzbh"), 0);
	gbOutputSA = GetRegKey((_T("Settings")),_T("OutputSA"), -1);
	gbAutoPASA = GetRegKey((_T("Settings")), _T("AutoPASA"), 0);
	gbCSPRCanSpecialDesign = GetRegKey((_T("Settings")), _T("CSPRCanSpecialDesign"), -1);
	gbLimitLugMinDia = GetRegKey((_T("Settings")), _T("LimitLugMinDia"), 0);
	gbRodDiaByMax = GetRegKey((_T("Settings")), _T("RodDiaByMax"), 1);
   //int gbTbOS;
	gbAutoAlignCSPRtoSA = GetRegKey((_T("Settings")), _T("AutoAlignCSPRtoSA"), -1);
	gbSumAllVolume = GetRegKey((_T("Settings")), _T("SumAllVolume"), -1);//
	EDIBgbl::gbTbOS = (bool)GetRegKey((_T("Settings")), _T("TbOS"), -1);
	gbCalSAbyConservativenessMethod = GetRegKey((_T("Settings")), _T("CalSAbyConservativenessMethod"), -1);
	gbCalSAbyPJG = GetRegKey((_T("Settings")), _T("CalSAbyPJG"), 0);
	gbCalCSPRLugDiaByPjg = GetRegKey((_T("Settings")), _T("CalCSPRLugDiaByPjg"), -1);
   
	giMinRodLen=GetRegKey((_T("Settings")), _T("MinRodLen"), 300);
	//gbCalPARTbyMaxLoad = GetRegKey((_T("Settings")), _T("CalPARTbyMaxLoad"), 0);
	gbCalPARTbyMaxLoad = FALSE;
	gbAddPartWeight2PMax = GetRegKey((_T("Settings")), _T("AddPartWeight2PMax"), -1);
	gbAddPartWeight2CSPRPgz = GetRegKey((_T("Settings")), _T("AddPartWeight2CSPRPgz"), -1);
	gbAddPartWeight2SPRPgz = GetRegKey((_T("Settings")), _T("AddPartWeight2SPRPgz"), 0);//2007.07.30
	gbPAForceZero=GetRegKey((_T("Settings")),_T("PAForceZero"),0);//2007.09.25
	gbMaxPgzPaz = GetRegKey((_T("Settings")), _T("MaxPgzPaz"), 0);//2007.09.03
//	gbMinMaxDispByActualDisp = GetRegKey((_T("Settings")), _T("MinMaxDispByActualDisp"), -1);
	gbMinMaxDispByActualDisp = GetRegKey((_T("Settings")), _T("MinMaxDispByActualDisp"), 0);
//	gbCalcCSPRHeight_BySelDisp = GetRegKey((_T("Settings")), _T("CalcCSPRHeight_BySelDisp"), 0);
	gbCalcCSPRHeight_BySelDisp = GetRegKey((_T("Settings")), _T("CalcCSPRHeight_BySelDisp"), -1);
	gbNotPrompt_ACAD_SHIFT = GetRegKey((_T("Settings")), _T("NotPrompt_ACAD_SHIFT"), -1);
	gIsACAD = GetRegKey((_T("Settings")), _T("IsACAD"), 0);
	gbNotPrompt_Catlog_OVERWRITE = GetRegKey((_T("Settings")), _T("NotPrompt_Catlog_OVERWRITE"), -1);
	giPhsOutINFOTableType = GetRegKey((_T("Settings")), _T("PhsOutINFOTableType"), 1);
	giDrawSIZE_A3A4 = GetRegKey((_T("Settings")), _T("DrawSIZE_A3A4"), 2);
	giSpringOrder = GetRegKey((_T("Settings")), _T("SpringOrder"), (int)lngSOSerialNumMinSEQmin);
	gstrSpringOrder = GetRegKey(_T("Settings"), _T("strSpringOrder"), CString(_T("")));
	gstrSpringOrderSQL = GetRegKey((_T("Settings")), _T("strSpringOrderSQL"), CString(_T("")));
	gsPhsColor = GetRegKey((_T("Settings")), _T("PhsColor"),CString( _T("White")));
	gbDraw_BOM = GetRegKey((_T("Settings")), _T("Draw_BOM"), true);
	gbAnnotateCSPR = GetRegKey((_T("Settings")), _T("AnnotateCSPR"), -1);
	gbDrawOVERWRITE = GetRegKey((_T("Settings")), _T("Draw_OVERWRITE"), -1);
	gbWorksWithBoltsNuts = GetRegKey((_T("Settings")), _T("WorksWithBoltsNuts"), -1);
	gbWorksWithSABoltsNuts = GetRegKey((_T("Settings")), _T("WorksWithSABoltsNuts"), -1);
	gnDW_delta = GetRegKey((_T("Settings")), _T("DW_Delta"), (float)7);
   
	gsPhsArrangementDwgNo = GetRegKey((_T("Settings")), _T("phsArrangementDwgNo"), CString(_T("F4452S-J0501-02")));
	gsPhsScheduleDwgNo = GetRegKey((_T("Settings")), _T("phsScheduleDwgNo"), CString(_T("F4452S-J0501-03")));
	EDIBgbl::glCurRecordNo = (long)GetRegKey((_T("Settings")), _T("ZDJH"), 1);
	gbDrawNoPreMethod = GetRegKey((_T("Settings")), _T("DrawNoPreMethod"), 1);
	gbAutoSaveDrawing = GetRegKey((_T("Settings")), _T("AutoSaveDrawing"),0);
	glDrawNoStartNumber = GetRegKey((_T("Settings")), _T("DrawNoStartNumber"), 5);
	EDIBgbl::strDwgVersion = GetRegKey((_T("Settings")), _T("DwgVersion"), CString( _T("")));
	EDIBgbl::MaxLGLong = GetRegKey((_T("Settings")), _T("MaxLGLong"), 110);//pfg20050922

	glDWGPreMethod = GetRegKey((_T("Settings")), _T("DWGPreMethod"), 0);
	gsDWGPre = GetRegKey((_T("Settings")), _T("DWGPre"),CString( _T("")));
	bHintRepZdjType=GetRegKey((_T("Settings")),_T("HintRepZdjType"),false);
	gbNoFenQu=GetRegKey((_T("Settings")),_T("bNoFenQu"),false);
	gbTBhq=GetRegKey((_T("Settings")),_T("bTBhq"),true);
	gbNoWaiKuang=GetRegKey((_T("Settings")),_T("bNoWaiKuang"),false);
	//pfg20050531
	gbOrientationPicture=GetRegKey((_T("Settings")),_T("OrientationPicture"),false);
	
	iNumCSVal=GetRegKey((_T("Settings")),_T("NumCSVal"),0);

	gsPSAOutFilePath = GetRegKey((_T("Settings")), _T("PSAOutFilePath"),basDirectory::TemplateDir);
	g_iPSAFilterIndex = GetRegKey((_T("Settings")), _T("PSAOutFilterIndex"),(int)1);
	gsSelectProductMdb = GetRegKey((_T("Settings")), _T("SelectProductMdb"),basDirectory::TemplateDir);

	gbHotStatus2Zero = GetRegKey((_T("Settings")), _T("HotStatus2Zero"),-1);

	EDIBAcad::g_bBomSprFactory     = GetRegKey((_T("Settings")), _T("BomSprFactory"),0);
	EDIBAcad::g_bSumBomSprFactory  = GetRegKey((_T("Settings")), _T("SumBomSprFactory"),0);
	EDIBAcad::g_bLocationDimension = GetRegKey((_T("Settings")), _T("LocationDimension"),-1);
	EDIBAcad::g_bDLOADBJBW = GetRegKey((_T("Settings")), _T("DLOADBJBW"),0);
	EDIBAcad::g_bDrawNameWithoutNo = GetRegKey((_T("Settings")), _T("DrawNameWithoutNo"),0);

	EDIBAcad::g_bDrawSumWeight  = GetRegKey((_T("Settings")), _T("Draw_SumWeight"),-1);
	EDIBAcad::g_bGroupWare  = GetRegKey((_T("Settings")), _T("GroupWare"),0);

	CString str = CString();
	EDIBAcad::g_strGroupWareName = GetRegKey((_T("Settings")), _T("strGroupWareName"),str);

	EDIBAcad::g_bTagUsingCircle  = GetRegKey((_T("Settings")), _T("TagUsingCircle"),0);
	EDIBAcad::g_bCustomPicNo  = GetRegKey((_T("Settings")), _T("CustomPicNo"),0);

	EDIBAcad::g_bLJMX_BJBW  = GetRegKey((_T("Settings")), _T("LJMX_BJBW"),0);
	EDIBAcad::g_bBGForBJBW  = GetRegKey((_T("Settings")), _T("BGForBJBW"),0);
	EDIBAcad::g_bDWForBJBW  = GetRegKey((_T("Settings")), _T("DWForBJBW"),0);//巴威形式的定位

	modPHScal::gfSeqCircleDiameter  = GetRegKey((_T("Settings")), _T("CircleDiameter"),6.0f);
	modPHScal::gfSeqTxtHeight = GetRegKey((_T("Settings")), _T("SeqTxtHeight"),5.5f);

	modPHScal::g_bDHT = GetRegKey((_T("Settings")), _T("DHT"),-1);
	modPHScal::g_strDHTGG = GetRegKey((_T("Settings")), _T("DHTGG"),CString(""));
	EDIBAcad::g_bNotDrawSingleWeight  = GetRegKey((_T("Settings")), _T("NotDrawSingleWeight"),-1);
	
	EDIBAcad::g_bInsertLength = GetRegKey((_T("Settings")),_T("InsertLength"),-1);

	EDIBAcad::g_bPrintSA_Value= GetRegKey((_T("Settings")),_T("PrintSA_Value"),-1);

	EDIBAcad::g_bInstallElevation = GetRegKey((_T("Settings")),_T("InstallElevation"),-1);
	EDIBAcad::g_bColdHeatState  = GetRegKey((_T("Settings")),_T("ColdHeatState"),-1);
	EDIBAcad::g_bHzabs  = GetRegKey((_T("Settings")),_T("HzAbs"),0);
	EDIBgbl::bSymbol = GetRegKey(_T("Settings"),_T("DiaSymbol"),1);
	
	EDIBAcad::g_fDistanceOffset  = GetRegKey((_T("Settings")),_T("DistanceOffset"),0.0f);
	EDIBAcad::g_fOvertopDim  = GetRegKey((_T("Settings")),_T("OvertopDim"),2.0f);
	EDIBAcad::g_bDimNumber = GetRegKey((_T("Settings")),_T("DimNumber"),1);

	EDIBAcad::g_fSetTKWidth = GetRegKey((_T("Settings")),_T("SetTKWidth"),0.3f);
	EDIBAcad::g_fSetTableWidth = GetRegKey((_T("Settings")),_T("SetTableWidth"),0.0f);

	//LFX 2005.6.30 加 焊缝绘制选项
	modPHScal::gbDrawWeldSymbol = GetRegKey((_T("Settings")),_T("DrawWeldSymbol"),-1);
	modPHScal::gbAutoWeldViewInsertPoint = GetRegKey((_T("Settings")),_T("AutoWeldViewInsertPoint"),-1);
	modPHScal::gbDrawWeldViewDim = GetRegKey((_T("Settings")),_T("DrawWeldViewDim"),-1);
	modPHScal::gbWeldAutoDirectAndPos = GetRegKey((_T("Settings")),_T("WeldAutoDirectAndPos"),-1);
	//END lfx 2005.6.30
}

void modPHScal::InitializeCrudeData(_RecordsetPtr rs, bool /*ByVal*/ bFrmTxsrLoaded)
{ 
	//long	 mZdjh;//内部支吊架号
   //rs.Bookmark = rs.Bookmark
	if(rs==NULL)
		return;
	try{
		if (!rs->adoEOF && !rs->BOF)
		{
			zdjh = rs->GetCollect(_T("zdjh")).operator long();
		}
		//增加下句则溢出堆栈空间
		//rs.FindFirst _T("zdjh=") + zdjh
		_variant_t v1;
		v1=rs->GetCollect(_T("dn1"));
		CString stmp=vtos(v1);
		stmp.TrimLeft();
		stmp.TrimRight();
		if(stmp.GetLength()>0)
			dn = stmp;
      
		GetHeatDisplacement( rs);
		if(vtoi(rs->GetCollect(_T("iSelSampleID")))<=0)
		{
			iSelSampleID=Cavphs->GetPhsStructSampleID();
		}
		else
			iSelSampleID=rs->GetCollect(_T("iSelSampleID")).operator long();
			
		//下句造成斯循环
		//iSelSampleID = Cavphs->GetPhsStructSampleID()
		//设置方向初始值，以便在没有计算时能够选择根部或管部方向
		if( df = _T(""))  df = _T("X");
		if( fx = _T("") ) fx = _T("P");
		if( gf = _T("") ) gf = _T("X");
   
		if( bFrmTxsrLoaded)
		{
			Cavphs->TZBResultObj = &FrmTxsr.m_pViewTxsr->m_LstPhsStructure;
			Cavphs->GetPhsStructFromTZB(zdjh);
		}
		clb.Format(_T("%2d"),zdjh+glDrawNoStartNumber-1);
	}
	catch(_com_error &e)
	{
		ReportError(e.Description(),__FILE__,__LINE__);
	}
}

void modPHScal::CalPAfixZ1Z2(CString dn, float tmpSelPJG, float tmpT0, int mvariNumPart, _RecordsetPtr rsX)
{
//目的：计算固定支架Z1或Z2
//原因:此过程加入到GetPhsBHandSizes中，导致VB6.0过程超过64K而使运行编译停止。
//故只能将其从GetPhsBHandSizes移出。
//纯粹的Make生成编译则可以通过。
   //以下一组变量用于计算固定支架的当量力sngSAfixP
//    float sngSAfixP  ; //固定支架的当量力sngSAfixP
//    float sngSAfixPl ; //冷态固定支架的当量力sngSAfixPl
//    float sngSAfixPr ; //热态固定支架的当量力sngSAfixPr
//    float PAfixH  ; //固定支架管部高度H,cm
//    float F1ToWx1 ;    //与根部型钢号有关的特性数据，储存在zdjcrudeXBDLY的槽钢特性表SSteelPropertyCS的F1ToWx1字段
    float PAfixC1  ;   //固定支架C1值,mm
//    float mvBWidth  ;    //固定支架根部主槽钢间距,cm,G47=根部表chdist字段值，G48=TZA表A01字段值
    float pnl, pnr ;  //固定支架Z1管部螺栓冷/热态承受的力或Z2合成力,kgf
    float ml, mr    ; //固定支架Z2的合成力矩,kgf.m
    float pxl, pyl, pzl, pxr,pyr, pzr;   //固定支架冷热态承受的力,kgf
    float mxl, myl, mzl, mxr, myr, mzr;
    float Sigma0, Sigmat,Sigma100;
    float wk, wkl, wkr1, wkr2;  
    float sngCoef;   //固定支架的系数
    long k ;
	 //CString sXYZ;	//记录坐标系转换前的坐标方向

    _Recordset* rsza;
	_RecordsetPtr rsTmp;
	rsTmp.CreateInstance(__uuidof(Recordset));
  
	rsza=FrmTxsr.m_pViewTxsr->m_ActiveRs;
	if(rsX->State == adStateOpen)
		rsX->Close();
	CString SQLx,sTmp,sTmp2,sTmp3;
	_variant_t vTmp,vTmp2,vTmp3;
	switch(glClassID)
	{
	case iPAfixZ1:
	   for( k = 0 ;k<= gnDW_delta;k++)
	   {
		SQLx.Format(_T("SELECT * FROM %s WHERE Dw>= %g AND Dw<= %g AND Pmax>=%g AND Tj>=%g AND (CustomID)=\'%s\' ORDER BY Dw,tj,Weight"),
			tbnPAfix,
			dj * (1 - k * 0.01) ,
			dj * (1 + k * 0.01),
			tmpSelPJG / mvariNumPart,
			tmpT0 ,
			dn);
			if(rsX->State == adStateOpen)
				rsX->Close();
			rsX->Open((_bstr_t)SQLx, _variant_t((IDispatch*)dbZDJcrude,true), 
				adOpenStatic, adLockOptimistic, adCmdText); 
         if( rsX->BOF && rsX->adoEOF )
			{
         }
			else
			{
             //找到，设置k超界，以便退出循环
             k=gnDW_delta+1;
         }
      }

		if(dj > 273)
		{
			sngCoef = 1;
		}
		else 
		{
			if(dj>=219)
			{
				sngCoef = 0.95;
			}
			else
			{
				if(dj>= 168 ) 
				{
					sngCoef = 0.83;
				}
				else
				{
					//if(dj< 168) 
					sngCoef = 0.7;					
				}
			}
		}

		if(rsX->State == adStateOpen)
			rsX->Close();
		rsX->Open((_bstr_t)SQLx, _variant_t((IDispatch*)dbZDJcrude,true), 
			adOpenStatic, adLockOptimistic, adCmdText); 
       if( rsX->BOF && rsX->adoEOF )
	   {
		   sTmp.Format(GetResStr(IDS_NoThisZ1orZ2inCrudePAfix),dn);
			throw sTmp;
       }
	   else
	   {
          rsX->MoveFirst();
			rsX->get_Collect((_variant_t)_T("size5"),&vTmp);
          PAfixC1 = vtof(vTmp);
          
          if( vtos(rsza->GetCollect(_T("Unit_Force"))) == _T("N") )
		  {
             pxl = vtof(rsza->GetCollect(_T("pxl1"))) / EDIBgbl::kgf2N;
             pyl = vtof(rsza->GetCollect(_T("pyl1"))) / EDIBgbl::kgf2N;
             pzl = vtof(rsza->GetCollect(_T("pzl1"))) / EDIBgbl::kgf2N;
             pxr = vtof(rsza->GetCollect(_T("pxr1"))) / EDIBgbl::kgf2N;
             pyr = vtof(rsza->GetCollect(_T("pyr1"))) / EDIBgbl::kgf2N;
             pzr = vtof(rsza->GetCollect(_T("pzr1"))) / EDIBgbl::kgf2N;
          }
		  else
		  {
             pxl = vtof(rsza->GetCollect(_T("pxl1")));
             pyl = vtof(rsza->GetCollect(_T("pyl1")));
             pzl = vtof(rsza->GetCollect(_T("pzl1")));
             pxr = vtof(rsza->GetCollect(_T("pxr1")));
             pyr = vtof(rsza->GetCollect(_T("pyr1")));
             pzr = vtof(rsza->GetCollect(_T("pzr1")));
          }
          if( vtos(rsza->GetCollect(_T("Unit_Momental"))) == _T("N.m") )
		  {
             mxl = vtof(rsza->GetCollect(_T("mxl1"))) / EDIBgbl::kgf2N;
             myl = vtof(rsza->GetCollect(_T("myl1"))) / EDIBgbl::kgf2N;
             mzl = vtof(rsza->GetCollect(_T("mzl1"))) / EDIBgbl::kgf2N;
             mxr = vtof(rsza->GetCollect(_T("mxr1"))) / EDIBgbl::kgf2N;
             myr = vtof(rsza->GetCollect(_T("myr1"))) / EDIBgbl::kgf2N;
             mzr = vtof(rsza->GetCollect(_T("mzr1"))) / EDIBgbl::kgf2N;
          }
		  else
		  {
             mxl = vtof(rsza->GetCollect(_T("mxl1")));
             myl = vtof(rsza->GetCollect(_T("myl1")));
             mzl = vtof(rsza->GetCollect(_T("mzl1")));
             mxr = vtof(rsza->GetCollect(_T("mxr1")));
             myr = vtof(rsza->GetCollect(_T("myr1")));
             mzr = vtof(rsza->GetCollect(_T("mzr1")));
          }
          
		  //实际坐标系的力/力矩转换到计算坐标系
		  if(giUPxyz==1)
		  {
			  //实际坐标系x轴向上
			  if(df==_T("Z"))
			  {
				  //管部走向平行z轴
				  pxlc=pzl;
				  pylc=-pxl;
				  pzlc=-pyl;
				  pxrc=pzr;
				  pyrc=-pxr;
				  pzrc=-pyr;

				  mxlc=mzl;
				  mylc=-mxl;
				  mzlc=-myl;
				  mxrc=mzr;
				  myrc=-mxr;
				  mzrc=-myr;
			  }
			  else
			  {
				  //管部走向平行y轴
				  pxlc=pyl;
				  pylc=-pxl;
				  pzlc=pzl;
				  pxrc=pyr;
				  pyrc=-pxr;
				  pzrc=pzr;

				  mxlc=myl;
				  mylc=-mxl;
				  mzlc=mzl;
				  mxrc=myr;
				  myrc=-mxr;
				  mzrc=mzr;
			  }
		  }
		  else if(giUPxyz==2)
		  {
			  //实际坐标系y轴向上
			  if(df==_T("Z"))
			  {
				  //管部走向平行z轴,不用变换坐标系
				  pxlc=pzl;
				  pylc=-pyl;
				  pzlc=pxl;
				  pxrc=pzr;
				  pyrc=-pyr;
				  pzrc=pxr;

				  mxlc=mzl;
				  mylc=-myl;
				  mzlc=mxl;
				  mxrc=mzr;
				  myrc=-myr;
				  mzrc=mxr;
			  }
			  else
			  {
				  //管部走向平行x轴
				  pxlc=-pxl;
				  pylc=-pyl;
				  pzlc=pzl;
				  pxrc=-pxr;
				  pyrc=-pyr;
				  pzrc=pzr;

				  mxlc=-mxl;
				  mylc=-myl;
				  mzlc=mzl;
				  mxrc=-mxr;
				  myrc=-myr;
				  mzrc=mzr;
			  }
		  }
		  else if(giUPxyz==3)
		  {
			  //实际坐标系z轴向上
			  if(df==_T("Z"))
			  {
				  //管部走向平行y轴,不用变换坐标系
				  pxlc=-pyl;
				  pylc=-pzl;
				  pzlc=pxl;
				  pxrc=-pyr;
				  pyrc=-pzr;
				  pzrc=pxr;

				  mxlc=-myl;
				  mylc=-mzl;
				  mzlc=mxl;
				  mxrc=-myr;
				  myrc=-mzr;
				  mzrc=mxr;
			  }
			  else
			  {
				  //管部走向平行x轴
				  pxlc=-pxl;
				  pylc=-pzl;
				  pzlc=-pyl;
				  pxrc=-pxr;
				  pyrc=-pzr;
				  pzrc=-pyr;

				  mxlc=-mxl;
				  mylc=-mzl;
				  mzlc=-myl;
				  mxrc=-mxr;
				  myrc=-mzr;
				  mzrc=-myr;
			  }
		  }

		  //当Py向下时，Py值不考虑。见西北院支吊架手册P44。by ligb on 2005.08.08
          if( pylc > 0 )
		  {
             pnl = pylc * (1 + dj / PAfixC1) / 4 + 1000 * mzlc / (2 * PAfixC1) / sngCoef;
          }
		  else
		  {
             pnl = 1000 * mzlc / (2 * PAfixC1) / sngCoef;
          }

          if( pyrc > 0 )
		  {
             pnr = pyrc * (1 + dj / PAfixC1) / 4 + 1000 * mzrc / (2 * PAfixC1) / sngCoef;
          }
		  else
		  {
             pnr = 1000 * mzrc / (2 * PAfixC1) / sngCoef;
          }

          rsX->get_Collect((_variant_t)_T("pmax"),&vTmp);
          if( fabs(pnl) > vtof(vTmp) )
		  {
			  sTmp2.Format(GetResStr(IDS_FixSupportBoltsNutsLoadOverAllowedMaxLoad),GetResStr(IDS_frmtxsr_OptCoolStatus),_T("\%g"),_T("\%g"));
			  sTmp.Format(sTmp2,pnl, vtof(vTmp));
			  throw sTmp;
          }
          if( fabs(pnr) > vtof(vTmp) )
		  {
			  sTmp2.Format(GetResStr(IDS_FixSupportBoltsNutsLoadOverAllowedMaxLoad),GetResStr(IDS_frmtxsr_OptHeatStatus),_T("\%g"),_T("\%g"));
			  sTmp.Format(sTmp2,pnr, vtof(vTmp));
			  throw sTmp;
          }
          rsX->get_Collect((_variant_t)_T("px0"),&vTmp);
          if( fabs(pxlc) > vtof(vTmp) )
		  {
			  sTmp2.Format(GetResStr(IDS_XYZdirectionLoadTooLarger),_T("\%d"),_T("X"),GetResStr(IDS_frmtxsr_OptCoolStatus),_T("\%g"),_T("\%g"));
			  sTmp.Format(sTmp2,zdjh,pxlc, vtof(vTmp));
			  throw sTmp;
          }
          if( fabs(pxrc) > vtof(vTmp) )
		  {
			  sTmp2.Format(GetResStr(IDS_XYZdirectionLoadTooLarger),_T("\%d"),_T("X"),GetResStr(IDS_frmtxsr_OptHeatStatus),_T("\%g"),_T("\%g"));
			  sTmp.Format(sTmp2,zdjh,pxrc, vtof(vTmp));
			  throw sTmp;
          }
          
          rsX->get_Collect((_variant_t)_T("pz0"),&vTmp);
          if( fabs(pzlc) > vtof(vTmp) )
		  {
			  sTmp2.Format(GetResStr(IDS_XYZdirectionLoadTooLarger),_T("\%d"),_T("Z"),GetResStr(IDS_frmtxsr_OptCoolStatus),_T("\%g"),_T("\%g"));
			  sTmp.Format(sTmp2,zdjh,pzlc, vtof(vTmp));
			  throw sTmp;
          }
          if( fabs(pzrc) > vtof(vTmp) )
		  {
			  sTmp2.Format(GetResStr(IDS_XYZdirectionLoadTooLarger),_T("\%d"),_T("Z"),GetResStr(IDS_frmtxsr_OptHeatStatus),_T("\%g"),_T("\%g"));
			  sTmp.Format(sTmp2,zdjh,pzrc, vtof(vTmp));
			  throw sTmp;
          }
          
          rsX->get_Collect((_variant_t)_T("mx0"),&vTmp);
          if( fabs(mxlc) > vtof(vTmp) )
		  {
			  sTmp2.Format(GetResStr(IDS_XYZdirectionLoadTooLarger),_T("\%d"),_T("X"),GetResStr(IDS_frmtxsr_OptCoolStatus),_T("\%g"),_T("\%g"));
			  sTmp.Format(sTmp2,zdjh,mxlc, vtof(vTmp));
			  throw sTmp;
          }
          if( fabs(mxrc) > vtof(vTmp) )
		  {
			  sTmp2.Format(GetResStr(IDS_XYZdirectionLoadTooLarger),_T("\%d"),_T("X"),GetResStr(IDS_frmtxsr_OptHeatStatus),_T("\%g"),_T("\%g"));
			  sTmp.Format(sTmp2,zdjh,mxrc, vtof(vTmp));
			  throw sTmp;
          }
          
          rsX->get_Collect((_variant_t)_T("my0"),&vTmp);
          if( fabs(mylc) > vtof(vTmp) )
		  {
			  sTmp2.Format(GetResStr(IDS_XYZdirectionLoadTooLarger),_T("\%d"),_T("Y"),GetResStr(IDS_frmtxsr_OptCoolStatus),_T("\%g"),_T("\%g"));
			  sTmp.Format(sTmp2,zdjh,mylc, vtof(vTmp));
			  throw sTmp;
          }
          if( fabs(myrc) > vtof(vTmp) )
		  {
			  sTmp2.Format(GetResStr(IDS_XYZdirectionLoadTooLarger),_T("\%d"),_T("Y"),GetResStr(IDS_frmtxsr_OptHeatStatus),_T("\%g"),_T("\%g"));
			  sTmp.Format(sTmp2,zdjh,myrc, vtof(vTmp));
			  throw sTmp;
          }
          
		  rsX->get_Collect((_variant_t)_T("sizeH"),&vTmp);
          modPHScal::PAfixH = vtof(vTmp);
       }
	   break;
    case iPAfixZ2:
       if( !IsNull(rsza->GetCollect(_T("dcl1"))) )  gstrMaterial= vtos(rsza->GetCollect(_T("dcl1")));
		 sTmp=gstrMaterial;
		 sTmp.MakeLower();sTmp.TrimLeft();sTmp.TrimRight();
       if( sTmp==_T("st45.8"))
	   {
          gstrMaterial = _T("st45.8");
       }
	   else if(sTmp!=_T("20"))
	   {
          //MsgBox _T("原始数据中没有输入主管材料,或主管材料输入不对!")
		   sTmp.Format(GetResStr(IDS_NullMaterialFieldOrNoThisMaterial),gstrMaterial);
		   ShowMessage(sTmp);
          gstrMaterial = _T("20");
       }
       for (k = 0 ;k<= gnDW_delta;k++)
	   {
			 //排序按管径dw，温度tj，管部焊缝抗弯矩size7，管部焊缝高度size4，根部焊缝高度size5，
			 //排序按管径dw，温度tj，支管外径sizeC，管部焊缝高度size4，根部焊缝高度size5，
			 //世纪上两者等价，下者适应性更好。如华东院不提供size7字段值，依靠计算。
          SQLx.Format(_T("SELECT * FROM %s WHERE Dw>= %g AND Dw<= %g AND Tj>=%g AND (Material)=\'%s\' AND (CustomID)=\'%s\' ORDER BY Dw,tj,sizeC,size4,size5,Weight"),
				tbnPAfix,
				dj * (1 - k * 0.01),
				dj * (1 + k * 0.01),
				tmpT0,
				gstrMaterial,
				dn);
          if(rsX->State == adStateOpen)
				rsX->Close();
		  rsX->Open((_bstr_t)SQLx, _variant_t((IDispatch*)dbZDJcrude,true), 
			  adOpenStatic, adLockOptimistic, adCmdText); 
          if( rsX->BOF && rsX->adoEOF )
			{
          }
		  else
		  {
             //找到，退出循环
             k=gnDW_delta+1;
          }
       }
       if(dj > 273) 
		   sngCoef = 1;
       else if(dj>=219 && dj<= 273) 
		   sngCoef = 0.95;
       else if(dj>= 168 && dj<=  219) 
		   sngCoef = 0.83;
       if(dj< 168) 
		   sngCoef = 0.7;
	  if(rsX->State == adStateOpen)
		  rsX->Close();
	  rsX->Open((_bstr_t)SQLx, _variant_t((IDispatch*)dbZDJcrude,true), 
		  adOpenStatic, adLockOptimistic, adCmdText); 
       if( rsX->BOF && rsX->adoEOF )
	   {
		   sTmp.Format(GetResStr(IDS_NoThisZ1orZ2inCrudePAfix),dn);
			throw sTmp;
       }
	   else
	   {
          rsX->MoveFirst();
          
          if( vtos(rsza->GetCollect(_T("Unit_Force"))) == _T("N") )
		  {
             pxl = vtof(rsza->GetCollect(_T("pxl1"))) / EDIBgbl::kgf2N;
             pyl = vtof(rsza->GetCollect(_T("pyl1"))) / EDIBgbl::kgf2N;
             pzl = vtof(rsza->GetCollect(_T("pzl1"))) / EDIBgbl::kgf2N;
             pxr = vtof(rsza->GetCollect(_T("pxr1"))) / EDIBgbl::kgf2N;
             pyr = vtof(rsza->GetCollect(_T("pyr1"))) / EDIBgbl::kgf2N;
             pzr = vtof(rsza->GetCollect(_T("pzr1"))) / EDIBgbl::kgf2N;
          }
		  else
		  {
             pxl = vtof(rsza->GetCollect(_T("pxl1")));
             pyl = vtof(rsza->GetCollect(_T("pyl1")));
             pzl = vtof(rsza->GetCollect(_T("pzl1")));
             pxr = vtof(rsza->GetCollect(_T("pxr1")));
             pyr = vtof(rsza->GetCollect(_T("pyr1")));
             pzr = vtof(rsza->GetCollect(_T("pzr1")));
          }
          if( vtos(rsza->GetCollect(_T("Unit_Momental"))) == _T("N.m") )
		  {
             mxl = vtof(rsza->GetCollect(_T("mxl1"))) / EDIBgbl::kgf2N;
             myl = vtof(rsza->GetCollect(_T("myl1"))) / EDIBgbl::kgf2N;
             mzl = vtof(rsza->GetCollect(_T("mzl1"))) / EDIBgbl::kgf2N;
             mxr = vtof(rsza->GetCollect(_T("mxr1"))) / EDIBgbl::kgf2N;
             myr = vtof(rsza->GetCollect(_T("myr1"))) / EDIBgbl::kgf2N;
             mzr = vtof(rsza->GetCollect(_T("mzr1"))) / EDIBgbl::kgf2N;
          }
		  else
		  {
             mxl = vtof(rsza->GetCollect(_T("mxl1")));
             myl = vtof(rsza->GetCollect(_T("myl1")));
             mzl = vtof(rsza->GetCollect(_T("mzl1")));
             mxr = vtof(rsza->GetCollect(_T("mxr1")));
             myr = vtof(rsza->GetCollect(_T("myr1")));
             mzr = vtof(rsza->GetCollect(_T("mzr1")));
          }
          
		  //实际坐标系的力/力矩转换到计算坐标系
		  //开始按西北院Z2/Z2A坐标系变换
		  if(dn==_T("Z2") || dn==_T("Z2A"))
		  {
			  if(giUPxyz==1)
			  {
				  //实际坐标系x轴向上
				  if(df==_T("Z"))
				  {
					  //管部走向平行z轴
					  pxlc=pzl;
					  pylc=-pxl;
					  pzlc=-pyl;
					  pxrc=pzr;
					  pyrc=-pxr;
					  pzrc=-pyr;

					  mxlc=mzl;
					  mylc=-mxl;
					  mzlc=-myl;
					  mxrc=mzr;
					  myrc=-mxr;
					  mzrc=-myr;
				  }
				  else
				  {
					  //管部走向平行y轴
					  pxlc=pyl;
					  pylc=-pxl;
					  pzlc=pzl;
					  pxrc=pyr;
					  pyrc=-pxr;
					  pzrc=pzr;

					  mxlc=myl;
					  mylc=-mxl;
					  mzlc=mzl;
					  mxrc=myr;
					  myrc=-mxr;
					  mzrc=mzr;
				  }
			  }
			  else if(giUPxyz==2)
			  {
				  //实际坐标系y轴向上
				  if(df==_T("Z"))
				  {
					  //管部走向平行z轴,不用变换坐标系
					  pxlc=pzl;
					  pylc=-pyl;
					  pzlc=pxl;
					  pxrc=pzr;
					  pyrc=-pyr;
					  pzrc=pxr;

					  mxlc=mzl;
					  mylc=-myl;
					  mzlc=mxl;
					  mxrc=mzr;
					  myrc=-myr;
					  mzrc=mxr;
				  }
				  else
				  {
					  //管部走向平行x轴
					  pxlc=-pxl;
					  pylc=-pyl;
					  pzlc=pzl;
					  pxrc=-pxr;
					  pyrc=-pyr;
					  pzrc=pzr;

					  mxlc=-mxl;
					  mylc=-myl;
					  mzlc=mzl;
					  mxrc=-mxr;
					  myrc=-myr;
					  mzrc=mzr;
				  }
			  }
			  else if(giUPxyz==3)
			  {
				  //实际坐标系z轴向上
				  if(df==_T("Z"))
				  {
					  //管部走向平行y轴,不用变换坐标系
					  pxlc=-pyl;
					  pylc=-pzl;
					  pzlc=pxl;
					  pxrc=-pyr;
					  pyrc=-pzr;
					  pzrc=pxr;

					  mxlc=-myl;
					  mylc=-mzl;
					  mzlc=mxl;
					  mxrc=-myr;
					  myrc=-mzr;
					  mzrc=mxr;
				  }
				  else
				  {
					  //管部走向平行x轴
					  pxlc=-pxl;
					  pylc=-pzl;
					  pzlc=-pyl;
					  pxrc=-pxr;
					  pyrc=-pzr;
					  pzrc=-pyr;

					  mxlc=-mxl;
					  mylc=-mzl;
					  mzlc=-myl;
					  mxrc=-mxr;
					  myrc=-mzr;
					  mzrc=-myr;
				  }
			  }
			  //结束西北院坐标系变换

			  //开始西北院Z2/Z2A查找计算
				 //冷、热态合成推力,单位:kgf
				 pnl = sqrt(pxlc * pxlc + pzlc * pzlc);
				 pnr = sqrt(pxrc * pxrc + pzrc * pzrc);
				 //冷、热态合成推力矩,单位:kgf.m
				 ml = sqrt(mxlc * mxlc + mylc * mylc + mzlc * mzlc);
				 mr = sqrt(mxrc * mxrc + myrc * myrc + mzrc * mzrc);

				 //冷态推力存储在pmax字段
				//冷态推力矩存储在mx0字段
				 //热态推力矩存储在my0字段
				rsX->get_Collect((_variant_t)_T("sizeH"),&vTmp);//unit=mm
				modPHScal::PAfixH =vtof(vTmp);
				rsX->get_Collect((_variant_t)_T("size4"),&vTmp);//unit=mm
				modPHScal::gfPSThickness=vtof(vTmp);//钢板厚度>=K,mm
				FrmTxsr.m_pViewTxsr->m_PagOption->m_fPSThickness=modPHScal::gfPSThickness;
				FrmTxsr.m_pViewTxsr->m_PagOption->UpdateData(FALSE);
				rsX->get_Collect((_variant_t)_T("Pmax"),&vTmp);//unit=kgf
				rsX->get_Collect((_variant_t)_T("mx0"),&vTmp2);//unit=kgf.m
				rsX->get_Collect((_variant_t)_T("my0"),&vTmp3);//unit=kgf.m
				if( pnl <= vtof(vTmp) && pnr <= vtof(vTmp) && ml <= vtof(vTmp2) && mr <= vtof(vTmp3) )
				{
					 //此种情况不必验算焊缝抗弯矩						
				 }
			  else
			  {
				  //查找许用应力
					modPHScal::StressOfMaterial(gstrMaterial,modPHScal::t0,Sigmat);
					modPHScal::StressOfMaterial(gstrMaterial,100,Sigma100);
					modPHScal::StressOfMaterial(gstrMaterial,20,Sigma0);
					//热态时，上焊缝(管部焊缝)抗弯矩wkr1,下焊缝(根部焊缝)抗弯矩wkr2:unit=(cm)3
					wkr2 = (pnr * modPHScal::PAfixH/10 + mr*100) / (0.85 * Sigmat);
					wkr1 = (pnr * (dj/10) / 2 + mr*100) / (0.85 * Sigmat);
					//冷态时，上焊缝(管部焊缝)抗弯矩wkl1,下焊缝(根部焊缝)抗弯矩wkl2
					//西北院手册认为冷态只要验算下焊缝即可。因为modPHScal::PAfixH=Dw/2+h>dj/2
					//wkl2 = (pnl * modPHScal::PAfixH/10 + ml*100) / (0.85 * Sigma0);
					//wkr1 = (pnl * dj/10 / 2 + ml*100) / (0.85 * Sigma0);
					wkl = (pnl * modPHScal::PAfixH/10 + ml*100) / (0.85 * Sigma0);

					//焊缝抗弯矩wk保存在size7字段
					rsX->get_Collect((_variant_t)_T("size7"),&vTmp);
					wk=vtof(vTmp);
					if( wkr2 > wkr1 )
					{
						if( wkr2 > wkl )
						{
							//热态根部焊缝需要的抗弯矩最大
							//wk=wkr2;
							_variant_t vTmp;
							rsX->Find((_bstr_t)(_T("size7>=")+ftos(wkr2)), 0, adSearchForward);
							if(!rsX->adoEOF)
							{
								//%s号支吊架%s热态时，下焊缝(根部焊缝)需要的抗弯矩%s>允许值%s
								sTmp.Format(GetResStr(IDS_SAWeldingAgainstMomental),ltos(zdjh),GetResStr(IDS_frmtxsr_OptHeatStatus),ftos(wkr2),ftos(wk));
								throw sTmp;                
							}
						}
						else
						{
							if(wkr1<wkl)
							{
								//冷态根部焊缝需要的抗弯矩最大
								//wk=wkl;
								_variant_t vTmp;
								rsX->Find((_bstr_t)(_T("size7>=")+ftos(wkl)), 0, adSearchForward);
								if(!rsX->adoEOF)
								{
									//%s号支吊架%s冷态时，下焊缝(根部焊缝)需要的抗弯矩%s>允许值%s
									sTmp.Format(GetResStr(IDS_SAWeldingAgainstMomental),ltos(zdjh),GetResStr(IDS_frmtxsr_OptHeatStatus),ftos(wkl),ftos(wk));
									throw sTmp;                
								}
							}
							else
								//不可能wkr2>wkr1 and wkr2<=wkl and wkl<=wkr1
								;		
						}
					}
					else
					{
						if( wkr2 > wkl )
						{
							//热态管部焊缝需要的抗弯矩最大
							//wk=wkr1;
							_variant_t vTmp;
							rsX->Find((_bstr_t)(_T("size7>=")+ftos(wkr1)), 0, adSearchForward);
							if(!rsX->adoEOF)
							{
								//%s号支吊架%s热态时，上焊缝(管部焊缝)需要的抗弯矩%s>允许值%s
								sTmp.Format(GetResStr(IDS_PAWeldingAgainstMomental),ltos(zdjh),GetResStr(IDS_frmtxsr_OptHeatStatus),ftos(wkr1),ftos(wk));
								throw sTmp;                
							}
						}
						else
						{
							if(wkr1<wkl)
							{
								//wkr2<=wkr1 and wkr2<=wkl and wkr1<wkl
								//冷态根部焊缝需要的抗弯矩最大
								//wk=wkl;
								_variant_t vTmp;
								rsX->Find((_bstr_t)(_T("size7>=")+ftos(wkl)), 0, adSearchForward);
								if(!rsX->adoEOF)
								{
									//%s号支吊架%s冷态时，下焊缝(根部焊缝)需要的抗弯矩%s>允许值%s
									sTmp.Format(GetResStr(IDS_SAWeldingAgainstMomental),ltos(zdjh),GetResStr(IDS_frmtxsr_OptHeatStatus),ftos(wkl),ftos(wk));
									throw sTmp;                
								}
							}
							else
							{
								//wkr2<=wkr1 and wkr2<=wkl and wkr1>=wkl
								//热态管部焊缝需要的抗弯矩最大
								_variant_t vTmp;
								rsX->Find((_bstr_t)(_T("size7>=")+ftos(wkr1)), 0, adSearchForward);
								if(!rsX->adoEOF)
								{
									//%s号支吊架%s热态时，上焊缝(管部焊缝)需要的抗弯矩%s>允许值%s
									sTmp.Format(GetResStr(IDS_PAWeldingAgainstMomental),ltos(zdjh),GetResStr(IDS_frmtxsr_OptHeatStatus),ftos(wkr1),ftos(wk));
									throw sTmp;                
								}
							}
						}
					}
				}
			  //结束西北院Z2/Z2A查找计算
			}
			//结束西北院Z2/Z2A坐标系变换和计算

			else if(dn==_T("SZ5"))//固定支架SZ5按华东院坐标系计算
			{
				//开始华东院SZ5坐标变换
			  if(giUPxyz==1)
			  {
				  //实际坐标系x轴向上
				  if(df==_T("Z"))
				  {
					  //管部走向平行z轴
					  pxlc=-pyl;
					  pylc=pxl;
					  pzlc=pzl;
					  pxrc=-pyr;
					  pyrc=pxr;
					  pzrc=pzr;

					  mxlc=-myl;
					  mylc=mxl;
					  mzlc=mzl;
					  mxrc=-myr;
					  myrc=mxr;
					  mzrc=mzr;
				  }
				  else
				  {
					  //管部走向平行y轴
					  pxlc=pzl;
					  pylc=pxl;
					  pzlc=-pyl;
					  pxrc=pzr;
					  pyrc=pxr;
					  pzrc=-pyr;

					  mxlc=mzl;
					  mylc=mxl;
					  mzlc=-myl;
					  mxrc=mzr;
					  myrc=mxr;
					  mzrc=-myr;
				  }
			  }
			  else if(giUPxyz==2)
			  {
				  //实际坐标系y轴向上
				  if(df==_T("Z"))
				  {
					  //管部走向平行z轴,不用变换坐标系
					  pxlc=pxl;
					  pylc=pyl;
					  pzlc=pzl;
					  pxrc=pxr;
					  pyrc=pyr;
					  pzrc=pzr;

					  mxlc=mxl;
					  mylc=myl;
					  mzlc=mzl;
					  mxrc=mxr;
					  myrc=myr;
					  mzrc=mzr;
				  }
				  else
				  {
					  //管部走向平行x轴
					  pxlc=pzl;
					  pylc=pyl;
					  pzlc=pxl;
					  pxrc=pzr;
					  pyrc=pyr;
					  pzrc=pxr;

					  mxlc=mzl;
					  mylc=myl;
					  mzlc=mxl;
					  mxrc=mzr;
					  myrc=myr;
					  mzrc=mxr;					 
				  }
			  }
			  else if(giUPxyz==3)
			  {
				  //实际坐标系z轴向上
				  if(df==_T("Z"))
				  {
					  //管部走向平行y轴,不用变换坐标系
					  pxlc=pxl;
					  pylc=pzl;
					  pzlc=-pyl;
					  pxrc=pxr;
					  pyrc=pzr;
					  pzrc=-pyr;

					  mxlc=mxl;
					  mylc=mzl;
					  mzlc=-myl;
					  mxrc=mxr;
					  myrc=mzr;
					  mzrc=-myr;
				  }
				  else
				  {
					  //管部走向平行x轴
					  pxlc=-pyl;
					  pylc=pzl;
					  pzlc=pxl;
					  pxrc=-pyr;
					  pyrc=pzr;
					  pzrc=pxr;

					  mxlc=-myl;
					  mylc=mzl;
					  mzlc=mxl;
					  mxrc=-myr;
					  myrc=mzr;
					  mzrc=mxr;
				  }
			  }
			  //结束华东院SZ5坐标变换

			  //开始华东院SZ5查找计算
				modPHScal::StressOfMaterial(gstrMaterial,modPHScal::t0,Sigmat);
				modPHScal::StressOfMaterial(gstrMaterial,100,Sigma100);
				modPHScal::StressOfMaterial(gstrMaterial,20,Sigma0);

					//先做一遍查询
					//支管外径dw,mm
					rsX->get_Collect((_variant_t)_T("sizeC"),&vTmp);
					float dw=vtof(vTmp);
					//支管壁厚S,mm
					rsX->get_Collect((_variant_t)_T("size3"),&vTmp);
					float S=vtof(vTmp);
					//上焊缝(管部侧焊缝)k1,mm
					rsX->get_Collect((_variant_t)_T("size4"),&vTmp);
					float k1=vtof(vTmp);
					//下焊缝(根部侧焊缝)k2,mm
					rsX->get_Collect((_variant_t)_T("size5"),&vTmp);
					float k2=vtof(vTmp);			
					//高度H,mm
					rsX->get_Collect((_variant_t)_T("sizeH"),&vTmp);
					modPHScal::PAfixH=vtof(vTmp);			

					rsX->get_Collect((_variant_t)_T("size4"),&vTmp);//unit=mm
					modPHScal::gfPSThickness=vtof(vTmp);//钢板厚度>=K,mm
					FrmTxsr.m_pViewTxsr->m_PagOption->m_fPSThickness=modPHScal::gfPSThickness;
					FrmTxsr.m_pViewTxsr->m_PagOption->UpdateData(FALSE);
			  
					bool bEOF=false;//没到尾
					bool bErr=false;//超限标志
				do
				{
				  //管部侧焊缝抗弯矩W1=(pow(dw,4)-(dw-1.4*k1)*(dw-1.4*k1))*atan((double)1.0)*4/32/dw :unit,mm3
				  float W1=(pow(dw,4)-(dw-1.4*k1)*(dw-1.4*k1))*atan((double)1.0)*4/32/dw;
				  //管部侧焊缝抗扭矩Wp1=2*W1 :unit,mm3
				  float Wp1=2*W1;
				  //管部侧焊缝的截面积F1=(dw*dw-(dw-1.4*k1)*(dw-1.4*k1))*atan((double)1.0): unit,mm2
				  float F1=(dw*dw-(dw-1.4*k1)*(dw-1.4*k1))*atan((double)1.0);

				  //固定支架SZ5管部侧焊缝的计算公式TAO1=sqrt(TAOy1*TAOy1+TAOxz1*TAOxz1)<=0.6*SIGMAjt
				  float TAOy1=1000*sqrt(mxrc*mxrc+mzrc*mzrc)/W1+fabs(pyrc)/F1; //kgf/mm2
				  float TAOxz1=sqrt(pxrc*pxrc+pzrc*pzrc)/F1+fabs(1000*myrc)/Wp1;//kgf/mm2
				  float TAO1=sqrt(TAOy1*TAOy1+TAOxz1*TAOxz1);
				  if(TAO1>0.6*Sigmat/100 )
				  {
					  if(bEOF)
					  {
							//%s号支吊架%s热态时，上焊缝(管部焊缝)合成应力%s>允许值%s
							sTmp.Format(GetResStr(IDS_PAWeldingStress),ltos(zdjh),GetResStr(IDS_frmtxsr_OptHeatStatus),ftos(TAO1),ftos(0.6*Sigmat/100));
							throw sTmp;                
					  }
					  else
							bErr=true;
				  }

				  TAOy1=1000*sqrt(mxlc*mxlc+mzlc*mzlc)/W1+fabs(pylc)/F1; //kgf/mm2
				  TAOxz1=sqrt(pxlc*pxlc+pzlc*pzlc)/F1+fabs(1000*mylc)/Wp1;//kgf/mm2
				  TAO1=sqrt(TAOy1*TAOy1+TAOxz1*TAOxz1);
				  if(TAO1>0.6*Sigma0/100 )
				  {
					  if(bEOF)
					  {
							//%s号支吊架%s冷态时，上焊缝(管部焊缝)合成应力%s>允许值%s
							sTmp.Format(GetResStr(IDS_PAWeldingStress),ltos(zdjh),GetResStr(IDS_frmtxsr_OptCoolStatus),ftos(TAO1),ftos(0.6*Sigma0/100));
							throw sTmp;
					  }
					  else
						  bErr=true;
				  }

				  //根部侧焊缝抗弯矩W2=(pow(dw+1.4*k2,4)-pow(dw,4))*atan((double)1.0)*4/32/(dw+1.4*k2) :unit,mm3
				  float pi=atan((double)1.0)*4;
				  float W2=(pow(dw+1.4*k2,4)-pow(dw,4))*atan((double)1.0)*4/32/(dw+1.4*k2);
				  //根部侧焊缝抗扭矩Wp2=2*W2 :unit,mm3
				  float Wp2=2*W2;
				  //根部侧焊缝的截面积F2=((dw+1.4*k2)*(dw+1.4*k2)-dw*dw)*atan((double)1.0): unit,mm2
				  float F2=((dw+1.4*k2)*(dw+1.4*k2)-dw*dw)*atan((double)1.0);
				  //固定支架SZ5根部侧焊缝的计算公式TAO2=sqrt(TAOy2*TAOy2+TAOxz2*TAOxz2)<=0.6*SIGMAj20
				  float TAOy2=sqrt((1000*mxrc+pzrc*modPHScal::PAfixH)*(1000*mxrc+pzrc*modPHScal::PAfixH)+(1000*mzrc-pzrc*modPHScal::PAfixH)*(1000*mzrc-pzrc*modPHScal::PAfixH))/W2+fabs(pyrc)/F2; //kgf/mm2
				  float TAOxz2=sqrt(pxrc*pxrc+pzrc*pzrc)/F2+fabs(1000*myrc)/Wp2;//kgf/mm2
				  float TAO2=sqrt(TAOy2*TAOy2+TAOxz2*TAOxz2);
				  if(TAO2>0.6*Sigma0/100 )
				  {
					  if(bEOF)
					  {
							//%s号支吊架%s热态时，下焊缝(根部焊缝)合成应力%s>允许值%s
							sTmp.Format(GetResStr(IDS_SAWeldingStress),ltos(zdjh),GetResStr(IDS_frmtxsr_OptHeatStatus),ftos(TAO2),ftos(0.6*Sigma0/100));
							throw sTmp;
					  }
					  else
						  bErr=true;
				  }

				  TAOy2=sqrt((1000*mxlc+pzlc*modPHScal::PAfixH)*(1000*mxlc+pzlc*modPHScal::PAfixH)+(1000*mzlc-pzlc*modPHScal::PAfixH)*(1000*mzlc-pzlc*modPHScal::PAfixH))/W2+fabs(pylc)/F2; //kgf/mm2
				  TAOxz2=sqrt(pxlc*pxlc+pzlc*pzlc)/F2+fabs(1000*mylc)/Wp2;//kgf/mm2
				  TAO2=sqrt(TAOy2*TAOy2+TAOxz2*TAOxz2);
				  if(TAO2>0.6*Sigma0/100)
				  {
					  if(bEOF)
					  {
							//%s号支吊架%s冷态时，下焊缝(根部焊缝)合成应力%s>允许值%s
							sTmp.Format(GetResStr(IDS_SAWeldingStress),ltos(zdjh),GetResStr(IDS_frmtxsr_OptHeatStatus),ftos(TAO2),ftos(0.6*Sigma0/100));
							throw sTmp;                
					  }
					  else
						  bErr=true;
				  }
				  
				  if(!bErr)
					  //没超限，找到
					  break;
				  else
				  {
						rsX->MoveNext();
						if(rsX->adoEOF)
						{
							//已经到记录尾，重新循环一次，以便报警
							bEOF=true;
							continue;
						}
						else
							//重新开始之前，设置超限标志
							bErr=false;

						//支管外径dw,mm
						rsX->get_Collect((_variant_t)_T("sizeC"),&vTmp);
						float dw=vtof(vTmp);
						//支管壁厚S,mm
						rsX->get_Collect((_variant_t)_T("size3"),&vTmp);
						float S=vtof(vTmp);
						//上焊缝(管部侧焊缝)k1,mm
						rsX->get_Collect((_variant_t)_T("size4"),&vTmp);
						float k1=vtof(vTmp);
						//下焊缝(根部侧焊缝)k2,mm
						rsX->get_Collect((_variant_t)_T("size5"),&vTmp);
						float k2=vtof(vTmp);			
						//高度H,mm
						rsX->get_Collect((_variant_t)_T("sizeH"),&vTmp);
						modPHScal::PAfixH=vtof(vTmp);	
						
						rsX->get_Collect((_variant_t)_T("size4"),&vTmp);//unit=mm
						modPHScal::gfPSThickness=vtof(vTmp);//钢板厚度>=K,mm
						FrmTxsr.m_pViewTxsr->m_PagOption->m_fPSThickness=modPHScal::gfPSThickness;
						FrmTxsr.m_pViewTxsr->m_PagOption->UpdateData(FALSE);
				  }
				}while(true);

				//结束华东院SZ5查找计算
			}//结束华东院SZ5坐标变换和查找计算
			  

		  //管部已经计算完毕,下面开始固定支架根部G47/G48、SJ8的计算：
		//根部计算由模块GetPhsBHandSizes函数完成。
		//根部计算前，将坐标系转换到根部计算坐标系：
			//开始计算G47/G48固定支架需要的当量力PmaxG47,kgf
		//开始按西北院G47/G48坐标系变换
		  if(gn==_T("G47") || gn==_T("G48"))
		  {
			  if(giUPxyz==1)
			  {
				  //实际坐标系x轴向上
				  if(df==_T("Z"))
				  {
					  //管部走向平行z轴
					  pxlc=pzl;
					  pylc=-pxl;
					  pzlc=-pyl;
					  pxrc=pzr;
					  pyrc=-pxr;
					  pzrc=-pyr;

					  mxlc=mzl;
					  mylc=-mxl;
					  mzlc=-myl;
					  mxrc=mzr;
					  myrc=-mxr;
					  mzrc=-myr;
				  }
				  else
				  {
					  //管部走向平行y轴
					  pxlc=pyl;
					  pylc=-pxl;
					  pzlc=pzl;
					  pxrc=pyr;
					  pyrc=-pxr;
					  pzrc=pzr;

					  mxlc=myl;
					  mylc=-mxl;
					  mzlc=mzl;
					  mxrc=myr;
					  myrc=-mxr;
					  mzrc=mzr;
				  }
			  }
			  else if(giUPxyz==2)
			  {
				  //实际坐标系y轴向上
				  if(df==_T("Z"))
				  {
					  //管部走向平行z轴,不用变换坐标系
					  pxlc=pzl;
					  pylc=-pyl;
					  pzlc=pxl;
					  pxrc=pzr;
					  pyrc=-pyr;
					  pzrc=pxr;

					  mxlc=mzl;
					  mylc=-myl;
					  mzlc=mxl;
					  mxrc=mzr;
					  myrc=-myr;
					  mzrc=mxr;
				  }
				  else
				  {
					  //管部走向平行x轴
					  pxlc=-pxl;
					  pylc=-pyl;
					  pzlc=pzl;
					  pxrc=-pxr;
					  pyrc=-pyr;
					  pzrc=pzr;

					  mxlc=-mxl;
					  mylc=-myl;
					  mzlc=mzl;
					  mxrc=-mxr;
					  myrc=-myr;
					  mzrc=mzr;
				  }
			  }
			  else if(giUPxyz==3)
			  {
				  //实际坐标系z轴向上
				  if(df==_T("Z"))
				  {
					  //管部走向平行y轴,不用变换坐标系
					  pxlc=-pyl;
					  pylc=-pzl;
					  pzlc=pxl;
					  pxrc=-pyr;
					  pyrc=-pzr;
					  pzrc=pxr;

					  mxlc=-myl;
					  mylc=-mzl;
					  mzlc=mxl;
					  mxrc=-myr;
					  myrc=-mzr;
					  mzrc=mxr;
				  }
				  else
				  {
					  //管部走向平行x轴
					  pxlc=-pxl;
					  pylc=-pzl;
					  pzlc=-pyl;
					  pxrc=-pxr;
					  pyrc=-pzr;
					  pzrc=-pyr;

					  mxlc=-mxl;
					  mylc=-mzl;
					  mzlc=-myl;
					  mxrc=-mxr;
					  myrc=-mzr;
					  mzrc=-myr;
				  }
			  }
			  //结束西北院G47/G48坐标系变换

			else if(gn==_T("SJ8"))//固定支架根部SJ8按华东院坐标系计算
			{
				//开始华东院SJ8坐标变换
			  if(giUPxyz==1)
			  {
				  //实际坐标系x轴向上
				  if(df==_T("Z"))
				  {
					  //管部走向平行z轴
					  pxlc=-pyl;
					  pylc=pxl;
					  pzlc=pzl;
					  pxrc=-pyr;
					  pyrc=pxr;
					  pzrc=pzr;

					  mxlc=-myl;
					  mylc=mxl;
					  mzlc=mzl;
					  mxrc=-myr;
					  myrc=mxr;
					  mzrc=mzr;
				  }
				  else
				  {
					  //管部走向平行y轴
					  pxlc=pzl;
					  pylc=pxl;
					  pzlc=-pyl;
					  pxrc=pzr;
					  pyrc=pxr;
					  pzrc=-pyr;

					  mxlc=mzl;
					  mylc=mxl;
					  mzlc=-myl;
					  mxrc=mzr;
					  myrc=mxr;
					  mzrc=-myr;
				  }
			  }
			  else if(giUPxyz==2)
			  {
				  //实际坐标系y轴向上
				  if(df==_T("Z"))
				  {
					  //管部走向平行z轴,不用变换坐标系
					  pxlc=pxl;
					  pylc=pyl;
					  pzlc=pzl;
					  pxrc=pxr;
					  pyrc=pyr;
					  pzrc=pzr;

					  mxlc=mxl;
					  mylc=myl;
					  mzlc=mzl;
					  mxrc=mxr;
					  myrc=myr;
					  mzrc=mzr;
				  }
				  else
				  {
					  //管部走向平行x轴
					  pxlc=pzl;
					  pylc=pyl;
					  pzlc=pxl;
					  pxrc=pzr;
					  pyrc=pyr;
					  pzrc=pxr;

					  mxlc=mzl;
					  mylc=myl;
					  mzlc=mxl;
					  mxrc=mzr;
					  myrc=myr;
					  mzrc=mxr;					 
				  }
			  }
			  else if(giUPxyz==3)
			  {
				  //实际坐标系z轴向上
				  if(df==_T("Z"))
				  {
					  //管部走向平行y轴,不用变换坐标系
					  pxlc=pxl;
					  pylc=pzl;
					  pzlc=-pyl;
					  pxrc=pxr;
					  pyrc=pzr;
					  pzrc=-pyr;

					  mxlc=mxl;
					  mylc=mzl;
					  mzlc=-myl;
					  mxrc=mxr;
					  myrc=mzr;
					  mzrc=-myr;
				  }
				  else
				  {
					  //管部走向平行x轴
					  pxlc=-pyl;
					  pylc=pzl;
					  pzlc=pxl;
					  pxrc=-pyr;
					  pyrc=pzr;
					  pzrc=pxr;

					  mxlc=-myl;
					  mylc=mzl;
					  mzlc=mxl;
					  mxrc=-myr;
					  myrc=mzr;
					  mzrc=mxr;
				  }
			  }
			  //结束华东院SJ8坐标系变换
			 }			 
			}		
       }
		 break;
   }
}


CString modPHScal::GetPhsSAfx(int SAfx)
{
	if(SAfx<0 || SAfx>3)
		return _T("");
	return iPhsSAfx[SAfx];
}

bool modPHScal::tbExists(_ConnectionPtr db,_ConnectionPtr db1, CString &tbn, CString s1, CString s2,CString s3)
{
	CString sTmp;
	UINT lngErrNum;
	if( tbn.GetLength()<=0 || tbn == _T("") ){
      tbn = _T("");
      lngErrNum = IDS_NullXfieldInphsManuXOfsortMdb;
	  sTmp.Format(GetResStr(lngErrNum),EDIBgbl::GetDBName(db1),s1,s3,s2);
	ShowMessage(sTmp);
		return FALSE;
   }else
	{
		if( !EDIBgbl::tdfExists(db, tbn) )
	  {
			lngErrNum = IDS_NoThisXCrudeDataTableInZDJcrudeMdb;
			sTmp.Format(GetResStr(lngErrNum),EDIBgbl::GetDBName(db),tbn);
			ShowMessage(sTmp);
			return FALSE;
      }
   }
   return TRUE;
}
bool modPHScal::HStbExists(_ConnectionPtr db/*Zdjcrude.mdb*/,_ConnectionPtr db1/*Sort.mdb*/, CString &tbn, 
						   CString s1, CString s2,CString s3,bool bWarn)
{
	CString sTmp;
	UINT lngErrNum;
	if( tbn.GetLength()<=0 || tbn == _T("") )
	{
		tbn = _T("");
		lngErrNum = IDS_NullXfieldInphsManuXOfsortMdb;
		sTmp.Format(GetResStr(lngErrNum),EDIBgbl::GetDBName(db1),s1,s3,s2);
		ShowMessage(sTmp);
		return FALSE;
	}
	else
	{
		if( !EDIBgbl::tdfExists(db,tbn) )
		{
			if (bWarn)
			{
				CString strWarn;
				strWarn.Format("警告:     %s 数据库中 %s 表未能找到,\n(表名在数据库 %s 的 %s 表的 %s 字段中)\
					\n可能数据库源文件内容有错!!! 请修改正确后再导入数据库然后再重新选择规范!\
					\n注意:  你选的规范将不起作用,系统将自动从当前可用规范中\
					\n选择一个合适的规范,如有不妥,请再选择其它正确的规范!!!",
					basDirectory::ProjectDBDir+_T("zdjcrude.mdb"),
					tbn,basDirectory::ProjectDBDir+_T("SORT"),s1,s2);
				AfxMessageBox(strWarn);
			}
			return FALSE;
		}
	}
	return TRUE;
}

BOOL modPHScal::CalOffset(_RecordsetPtr rsZA)
{
	//目的:计算偏装值。因为被2次调用，所以写成函数。
	//管部、根部偏装值计算（见林其略、周美芳编著《管道支吊技术》P60）
	BOOL bResult = TRUE;
	if( rsZA == NULL )
		return FALSE;
	else
	{
		if( rsZA->adoEOF && rsZA->BOF )
			return FALSE;
	}
	CString sTmp;
	if( giUPxyz == 1 ){
		//x轴向上,左视图水平轴向右为y轴正方向
		gpzx = yl + 0.5 * yr1;
		gpzz = zl + 0.5 * zr;
		//笔者怀疑上述公式应该不加冷位移，即如下：
		//gpzx = 0.5 * yr1
		//gpzz = 0.5 * zr
	}else if( giUPxyz == 2 ){
		//y轴向上,左视图水平轴向右为x轴正方向
		gpzx = xl + 0.5 * xr;
		gpzz = zl + 0.5 * zr;
		//笔者怀疑上述公式应该不加冷位移，即如下：
		//gpzx = 0.5 * xr
		//gpzz = 0.5 * zr
	}else if( giUPxyz == 3 ){
		//z轴向上,左视图水平轴向右为x轴正方向
		gpzx = xl + 0.5 * xr;
		gpzz = yl + 0.5 * yr1;
		//笔者怀疑上述公式应该不加冷位移，即如下：
		//gpzx = 0.5 * xr
		//gpzz = -0.5 * yr1
	}
	//吊架偏斜角A>4度时偏装,sin(A)=a/l=sqr((xr)^2+(zr)^2)/(sngSEL-sngPEL)
	//tan(A)^2=Sin(A)^2/(1-Sin(A)^2)
	float alfa=0;   //不偏装的热态偏转角，角度
	float alfa1=0; //偏装后的热态偏转角，角度
	float mvSinA=0;    //偏转角alfa的正弦
	float alfaLimit=0;  //弹簧/刚性支吊架偏转角上限值:弹簧4度(1/15)，刚性3度(1/20)。
	//gdw1:用户输入的根部定位值mm
	//gdw:经过偏装修正的根部定位值mm


	if( fabs(yr) > gUseSpringMinDist ){
		alfaLimit = 4;
	}else{
		alfaLimit = 3;
	}

	if( sngSEL > sngPEL )
	{
		if( giUPxyz == 1 ){
			//mvSinA = sqrt(yr1 *yr1 + zr * zr) / fabs(sngSEL * 1000.0 - dyr);//pfg2005.12.28原代码
			mvSinA = sqrt(yr1 *yr1 + zr * zr) / fabs(sngSEL * 1000.0 - sngPEL*1000);//pfg2005.12.28
		}else if( giUPxyz == 2 ){
			//mvSinA = sqrt(xr * xr + zr * zr) / fabs(sngSEL * 1000.0 - dyr);//pfg2005.12.28原代码
			mvSinA = sqrt(xr * xr + zr * zr) / fabs(sngSEL * 1000.0 - sngPEL*1000);//pfg2005.12.28
		}else if( giUPxyz == 3 ){
			//mvSinA = sqrt(yr1 * yr1 + xr * xr) / fabs(sngSEL * 1000.0 - dyr);//pfg2005.12.28原代码
			mvSinA = sqrt(yr1 * yr1 + xr * xr) / fabs(sngSEL * 1000.0 - sngPEL*1000);//pfg2005.12.28
		}
	}
	
	if( sngPEL >= sngSEL )
	{//3 //支架(start)
		//支架
		if( dg == _T("D") )
		{//2
			//管部偏装p
			if( df == _T("Z") )
			{//1
				//左视图管部走向沿Z方向
				//只能在左视图反映偏装
				dxa = dxa0 - gpzx;
				dza = dza0;
			}//1
			else
			{//1
				//只能在右视图反映偏装
				dxa = dxa0;
				dza = dza0 - gpzz;
			}//1
				gdw = gdw1;
				gdwx = dxa0;
				gdwz = dza0;	
			}//2
			else
			{//1
				//根部偏装
				/*
				if( gf == _T("X") )
					gdw = gdw1 + gpzx;
				else
					gdw = gdw1 + gpzz;
				
				//根部对管部安装态的偏装值
				dxa = dxa0; //和下面两句位置互换.
				dza = dza0;
				gdwx = dxa + gpzx;//20071127 "-" 改为 "+"
				gdwz = dza + gpzz;//20071127 "-" 改为 "+"
				*/
				//根部对管部安装态的偏装值
				dxa = dxa0; //和下面两句位置互换.
				dza = dza0;
				//支架偏装原为上述，如下根据客户需求任一方向上的热位移超过给定值(在选项中提供客户输入)时偏装
				//上述的偏装算法中，1.对于Y轴向上的时组装图和明细表正确  2.对于X轴向上的时组装图错误和明细表正确
				//3.对于Z轴向上的时组装图正确和明细表错误（方向）
				//modify by ligb luorijin  2008.10.16

				if ( gf == _T("X") )
				{
					if ( 1 == giUPxyz )
					{
						gpzx = gpzx*(fabs(yr1)>=gdZJOverValue ? 1 : 0);
						gpzz = gpzz*(fabs(zr)>=gdZJOverValue ? 1 : 0);					
						gdw = gdw1 - Sgn(dza0 * (fx == _T("P") ? 1 : -1)) * gpzx;
						gdwx = dxa - gpzx;
						gdwz = dza + gpzz;

					}
					else if ( 2 == giUPxyz )
					{
						gpzx = gpzx*(fabs(xr)>=gdZJOverValue ? 1 : 0);
						gpzz = gpzz*(fabs(zr)>=gdZJOverValue ? 1 : 0);
						gdw = gdw1 + Sgn(dza0 * (fx == _T("P") ? 1 : -1)) * gpzx;
						gdwx = dxa + gpzx;
						gdwz = dza + gpzz;
					}
					else if ( 3 == giUPxyz )
					{
						gpzx = gpzx*(fabs(xr)>=gdZJOverValue ? 1 : 0);
						gpzz = gpzz*(fabs(yr1)>=gdZJOverValue ? 1 : 0);
						gdwx = dxa + Sgn(dza0 * (fx == _T("P") ? 1 : -1)) * gpzx;
						gdw = gdw1 + gpzx;
						gdwz = dza - gpzz;
					}
				}
				else
				{
					//针对不同坐标轴视图标注的处理
					if ( 1 == giUPxyz )
					{
						gpzx = gpzx*(fabs(yr1)>=gdZJOverValue ? 1 : 0);
						gpzz = gpzz*(fabs(zr)>=gdZJOverValue ? 1 : 0);
						gdw = gdw1 + Sgn(dza0 * (fx == _T("P") ? 1 : -1)) * gpzz;
						gdwx = dxa - gpzx;
						gdwz = dza + gpzz;
					}
					else if ( 2 == giUPxyz )
					{
						gpzx = gpzx*(fabs(xr)>=gdZJOverValue ? 1 : 0);
						gpzz = gpzz*(fabs(zr)>=gdZJOverValue ? 1 : 0);
						gdw = gdw1 + Sgn(dza0 * (fx == _T("P") ? 1 : -1)) * gpzz;
						gdwx = dxa + gpzx;
						gdwz = dza + gpzz;	
					}
					else if ( 3 == giUPxyz )
					{
						gpzx = gpzx*(fabs(xr)>=gdZJOverValue ? 1 : 0);
						gpzz = gpzz*(fabs(yr1)>=gdZJOverValue ? 1 : 0);
						gdw = gdw1 - Sgn(dza0 * (fx == _T("P") ? 1 : -1)) * gpzz;
						gdwx = dxa + gpzx;
						gdwz = dza - gpzz;
					}
				}

			}//1
			rsZA->PutCollect((_T("pzx")),(float)gpzx);
			rsZA->PutCollect((_T("pzz")),(float)gpzz);
		}//3 //支架(end)
		else
		{//4//吊架(start)
			alfa = atan(sqrt(mvSinA * mvSinA / (1.0 - mvSinA * mvSinA))) / atan((double)1.0) / 4.0 * 180.0;
			//根部偏装或管部偏装
			if( alfa > alfaLimit )
			{//3
				//需要偏装
				alfa = atan(sqrt(mvSinA * mvSinA / (1.0 - mvSinA * mvSinA))) / atan((double)1.0) / 4.0 * 180.0;
				if( giUPxyz == 1 )
					//x轴向上,左视图水平轴向右为y轴正方向
					mvSinA = sqrt((yr1 - gpzx) * (yr1 - gpzx) + (zr - gpzz) * (zr - gpzz)) / fabs(sngSEL * 1000.0 - dyr);
				else if( giUPxyz == 2 )
					//y轴向上,左视图水平轴向右为x轴正方向
					mvSinA = sqrt((xr - gpzx)* (xr - gpzx) + (zr - gpzz)*(zr - gpzz)) / fabs(sngSEL * 1000.0 - dyr);
				else if( giUPxyz == 3 )
					//z轴向上,左视图水平轴向右为x轴正方向
					mvSinA = sqrt((xr - gpzx) * (xr - gpzx) + (yr1 - gpzz) * (yr1 - gpzz)) / fabs(sngSEL * 1000.0 - dyr);
				
         
				alfa1 = atan(sqrt(mvSinA * mvSinA / (1.0 - mvSinA * mvSinA))) / atan((double)1.0) / 4.0 * 180.0;
				if( alfa1 > alfaLimit )
				{//1
					//拉杆长度太短，即使偏装，偏转角仍然太大
					sTmp.Format(IDS_TooShortFromSAToPA,ftos(alfa1),ftos(alfaLimit));
					ShowMessage(sTmp);
					bResult = FALSE;
				}//1
				if( dg == _T("D") )
				{//2
					//管部偏装
					if( df == _T("Z") )
					{//1
						//左视图管部走向沿Z方向
						//只能在左视图反映偏装
						dxa = dxa0 - gpzx;
					}//1
					else
					{//1
						//只能在右视图反映偏装
						dza = dza0 - gpzz;
					}//1
					gdw = gdw1;
					gdwx = dxa0;
					gdwz = dza0;

				}//2
				else
				{//2
					//根部偏装
					/*
					if( gf == _T("X") )
					{//1
						gdw = gdw1 + Sgn(dxa0 * (fx == _T("P") ? 1 : -1)) * gpzx;
						dxa=dxa0;
						dza=dza0;
						gdwx = dxa + gpzx;
						gdwz = dza + gpzz;
					}//1
					else
					{//1
						gdw = gdw1 + Sgn(dza0 * (fx == _T("P") ? 1 : -1)) * gpzz;
						dxa=dxa0;
						dza=dza0;
						gdwx = dxa + gpzx;
						gdwz = dza + gpzz;
					}//1
					*/
					//上述的偏装算法中，1.对于Y轴向上的时组装图和明细表正确  2.对于X轴向上的时组装图错误和明细表正确
					//3.对于Z轴向上的时组装图正确和明细表错误（方向）
					//modify by ligb luorijin  2008.10.16
					dxa=dxa0;
					dza=dza0;
					if (gf == _T("X"))
					{
						if ( 1 == giUPxyz )
						{
							gdw = gdw1 - Sgn(dza0 * (fx == _T("P") ? 1 : -1)) * gpzx;
							gdwx = dxa - gpzx;
							gdwz = dza + gpzz;
						}
						else if ( 2 == giUPxyz )
						{
							gdw = gdw1 + Sgn(dza0 * (fx == _T("P") ? 1 : -1)) * gpzx;
							gdwx = dxa + gpzx;
							gdwz = dza + gpzz;				
							
						}
						else if ( 3 == giUPxyz )
						{
							gdw = gdw1 + Sgn(dza0 * (fx == _T("P") ? 1 : -1)) * gpzx;
							gdwx = dxa + gpzx;
							gdwz = dza - gpzz;
						}
					}
					else
					{
						if ( 1 == giUPxyz )
						{
							gdw = gdw1 + Sgn(dza0 * (fx == _T("P") ? 1 : -1)) * gpzz;
							gdwx = dxa - gpzx;
							gdwz = dza + gpzz;
						}
						else if ( 2 == giUPxyz )
						{
							gdw = gdw1 + Sgn(dza0 * (fx == _T("P") ? 1 : -1)) * gpzz;
							gdwx = dxa + gpzx;
							gdwz = dza + gpzz;				
							
						}
						else if ( 3 == giUPxyz )
						{
							gdw = gdw1 - Sgn(dza0 * (fx == _T("P") ? 1 : -1)) * gpzz;
							gdwx = dxa + gpzx;
							gdwz = dza - gpzz;
						}
					}

				}//2

			}//3
			else
			{//1
				//不需要偏装
				gpzx = 0;//不偏装时，明细表中的偏装值应该为0;
				gpzz = 0;
				gdw = gdw1;
				gdwx = dxa0;
				gdwz = dza0;
				dxa=dxa0;
				dza=dza0;
			}//1
			rsZA->PutCollect((_T("pzx")),(float)modPHScal::gpzx);
			rsZA->PutCollect((_T("pzz")),(float)modPHScal::gpzz);
		}//4//吊架(end)
		//结束管部、根部偏装值计算
		return bResult;
}

void modPHScal::StressOfMaterial(CString Material,float temperature,float& Sigmat,int iMODE,float& Density)
{
	CString SQLx,sTmp,strTbn,strFD;
	_variant_t vTmp;
	float t1,t2,Sigma1,Sigma2;

	_RecordsetPtr rsTmp;
	rsTmp.CreateInstance(__uuidof(Recordset));
	Sigmat=0.0;
	Density=0.0;
	float Coef;
	if(iMODE==0)
	{
		//查找材料在某温度temperature下的许用应力sigma,返回值单位unit:kgf/cm2,
		//数据表中单位MPa(N/mm2)
		//查找许用应力Sigmat,MPa->kgf/cm2
		if(modPHScal::tbnAllowStress==_T("")) modPHScal::tbnAllowStress=_T("MechanicalOfMaterialSIGMAt");
		strTbn=modPHScal::tbnAllowStress;
		strFD=_T("SIGMA");
		Coef=100.0/EDIBgbl::kgf2N;	
	}
	else if(iMODE==1)
	{
		//查找弹性模量Et,kN/mm,
		strTbn=_T("MechanicalOfMaterialEt");
		strFD=_T("Et");
		Coef=1.0;	
	}
	else if(iMODE==2)
	{
		//查找线性膨胀系数Alphat,unit:10e-6/C
		strTbn=_T("MechanicalOfMaterialALPHAt");
		strFD=_T("ALPHAt");
		Coef=1.0;	
	}
	else return;

	try
	{
		SQLx = _T("SELECT * FROM [") + strTbn + _T("] WHERE Trim(material)=\'") + Trim(Material) + _T("\' ORDER BY t");
		rsTmp->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbMaterial,true), 
			adOpenStatic, adLockOptimistic, adCmdText); 
		if(rsTmp->adoEOF && rsTmp->BOF)
			;
		else
		{
			//查找计算温度t0时的许用应力值Sigma1
			SQLx.Format(_T("t>=%g"),temperature);
			_variant_t vTmp;
			rsTmp->Find((_bstr_t)SQLx, 0, adSearchForward);
			if(!rsTmp->adoEOF)
			{
				rsTmp->get_Collect((_variant_t)strFD,&vTmp);
				Sigma1 = vtof(vTmp);
				rsTmp->get_Collect((_variant_t)_T("t"),&vTmp);
				t1 = vtof(vTmp);
				if(iMODE==1)
				{
					rsTmp->get_Collect((_variant_t)_T("Density"),&vTmp);
					Density=vtof(vTmp);
				}
				//找到温度值,前移一个记录，其温度值肯定比计算温度低。
				rsTmp->MovePrevious();
				if(rsTmp->adoEOF)
				{
					if(t1!=temperature)
					{
						//不可能出现这种情况
						//在材料许用应力表%s中没找到>=%sC的温度值。
						sTmp.Format(GetResStr(IDS_NotFoundTemperatureValueInTableAllowStress),_T("\%d"),modPHScal::tbnAllowStress,temperature);
						throw sTmp;
					}
					else
					{
						Sigmat = Sigma1 *Coef;
					}
				}
				else
				{
					rsTmp->get_Collect((_variant_t)strFD,&vTmp);
					Sigma2 = vtof(vTmp);
					rsTmp->get_Collect((_variant_t)_T("t"),&vTmp);
					t2 = vtof(vTmp);
					Sigmat = Sigma2 + (temperature - t2) * (Sigma1 - Sigma2) / (t1 - t2);
					Sigmat = Sigmat *Coef;
					if(iMODE==1)
					{
						rsTmp->get_Collect((_variant_t)_T("Density"),&vTmp);
						Density=vtof(vTmp);
					}
				}
			}
			else
			{
				//在材料许用应力表%s中没找到>=%sC的温度值。
				//sTmp.Format(GetResStr(IDS_NotFoundTemperatureValueInTableAllowStress),_T("\%d"),modPHScal::tbnAllowStress,temperature);
				//throw sTmp;
				//取最大温度下的值作为查询值
				rsTmp->MoveLast();
				rsTmp->get_Collect((_variant_t)strFD,&vTmp);
				Sigmat = vtof(vTmp) *Coef;
				if(iMODE==1)
				{
					rsTmp->get_Collect((_variant_t)_T("Density"),&vTmp);
					Density=vtof(vTmp);
				}
			}
		}
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}
}

void modPHScal::MakeTmpCalFixPhs()
{
//调用GetphsBHandSizes方法计算一个支吊架之前，必须在dbPRJ中建立表tmpCalFixPhs
	//下面的代码可能因为SQL语句太长，导致出错。只有当缩短到一定长度后才能成功。
	/*
	CString SQLx;
	try
	{
		if(EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("tmpCalFixPhs")))
			//dbPRJ.Execute _T("DELETE * FROM tmpCalFixPhs")
			//这样比较方便，而且版本升级时兼容性好。
			;
			//EDIBgbl::dbPRJ->Execute((_bstr_t)_T("DROP TABLE tmpCalFixPhs"));
		//Else
			//不存在,则建立临时表tmpCalFixPhs,准备计算固定支架：双臂三角架SJ8
		//End If		
		SQLx=_T("CREATE TABLE tmpCalFixPhs (ID char(20),CustomID char(50),BH char(50),tj REAL,H REAL,Px REAL,Py REAL,Pz REAL,Mx REAL,My REAL,Mz REAL,P1 char(20),P2 char(20),P3 char(20),dw1 REAL,s REAL,K REAL,K1 REAL,K2 REAL,W1pa REAL,Wp1pa REAL,F1pa REAL,W2pa REAL,Wp2pa REAL,F2pa REAL,Py1 REAL,Py2 REAL,SIGMAjt REAL,SIGMAj20 REAL,SIGMAj100 REAL,J1y REAL,J2y REAL,J1z REAL,J2z REAL,W1y REAL,W2y REAL,W1z REAL,W2z REAL,Wyh1 REAL,Wyh2 REAL,Wzh1 REAL,Wzh2 REAL,SIGMA1sa,TAO1sa REAL,TAOx1sa REAL,TAOy1sa REAL,TAOz1sa REAL)");
			
			_T(",SIGMA2sa,TAO2sa REAL,TAOx2sa REAL,TAOy2sa REAL,TAOz2sa REAL,")
			_T("F1sa REAL,F2sa REAL,h1 REAL,h2 REAL,b1 REAL,b2 REAL,B1 REAL,bb REAL,PL3 REAL,gdw2 REAL)");
			//上句gdw2字段存储了偏装修正过的根部定位值gdw
			
		EDIBgbl::dbPRJ->Execute((_bstr_t)SQLx);
		EDIBgbl::dbPRJ->Execute((_bstr_t)"ALTER TABLE ADD COLUMN );
		EDIBgbl::dbPRJ->Execute((_bstr_t) _T("INSERT INTO tmpCalFixPhs (H) VALUES (0)"));
	}
	catch (_com_error &e)
	{
	CString strMsg;
	strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
	AfxMessageBox(strMsg);
	}
	*/
}

bool modPHScal::CreateTmpCustomIDForPART()
{
	// 目的:为加快过滤可用模板的速度，先设置三个临时表，存储PA/PART/SA/SPR/CSPR的CustomID,
	// 每次只要找这几个表，即可确定零件是否可用于当前标准
	FILE* fp;
	fp = fopen("CreateTmpCustomIDForPART.txt", "w");

	CString strSQL;
	HRESULT hr = S_OK;
	try
	{
		if(EDIBgbl::tdfExists(EDIBgbl::dbPRJ ,_T("tmpCustomIDPA")))
		{
			fprintf(fp, "%d: dbPRJ->Execute\n", __LINE__);
			EDIBgbl::dbPRJ->Execute((_bstr_t)_T("DELETE FROM tmpCustomIDPA"), NULL, adCmdText);			
			strSQL=_T("INSERT INTO tmpCustomIDPA IN \'") + EDIBgbl::GetDBName(EDIBgbl::dbPRJ) + _T("\'  SELECT DISTINCT CustomID FROM [")+ modPHScal::tbnPA+_T("]");
			fprintf(fp, "%d: dbZDJcrude->Execute\n", __LINE__);
			modPHScal::dbZDJcrude->Execute((_bstr_t)strSQL, NULL, adCmdText);
		}
		else
		{
			//不要建立唯一索引，可能冲突
			strSQL = _T("CREATE TABLE tmpCustomIDPA (CustomID Char(20) CONSTRAINT index1 UNIQUE)");
			fprintf(fp, "%d: dbPRJ->Execute\n", __LINE__);
			hr = EDIBgbl::dbPRJ->Execute((_bstr_t)strSQL, NULL, adCmdText);
			strSQL=_T("INSERT INTO tmpCustomIDPA IN \'") + EDIBgbl::GetDBName(EDIBgbl::dbPRJ) + _T("\'  SELECT DISTINCT CustomID FROM [")+ modPHScal::tbnPA+_T("]");
			fprintf(fp, "%d: dbZDJcrude->Execute\n", __LINE__);
			modPHScal::dbZDJcrude->Execute((_bstr_t)strSQL, NULL, adCmdText);
		}
	}
	catch(...)
	{
		fprintf(fp, "...");
		fclose(fp);
		return false;
	}
	try
	{
		//建立固定支架数据表tbnPAfix时，绝对不能出现管部数据表tbnPA中已经出现的CustomID，否则出错.
		if(modPHScal::tbnPAfix!=modPHScal::tbnPA)
		{
			//strSQL=_T("INSERT INTO tmpCustomIDPA IN \'") + EDIBgbl::dbPRJ) + _T("\'  SELECT DISTINCT CustomID FROM [")+ modPHScal::tbnPAfix+_T("] WHERE CustomID NOT IN ( SELECT DISTINCT CustomID FROM [") + modPHScal::tbnPA + _T("] ) ");
			strSQL=_T("INSERT INTO tmpCustomIDPA IN \'") + EDIBgbl::GetDBName(EDIBgbl::dbPRJ) + _T("\'  SELECT DISTINCT CustomID FROM [")+ modPHScal::tbnPAfix+_T("] ");
			fprintf(fp, "%d: dbZDJcrude->Execute\n", __LINE__);
			modPHScal::dbZDJcrude->Execute((_bstr_t)strSQL, NULL, adCmdText);		
		}
	}
	catch(...)
	{
		fprintf(fp, "...");
		fclose(fp);
		return false;
	}

	try
	{
		if(EDIBgbl::tdfExists(EDIBgbl::dbPRJ ,_T("tmpCustomIDSA")))
		{
			fprintf(fp, "%d: ->Execute\n", __LINE__);
			EDIBgbl::dbPRJ->Execute((_bstr_t)_T("DELETE FROM tmpCustomIDSA"), NULL, adCmdText);			
			strSQL=_T("INSERT INTO tmpCustomIDSA IN \'") + EDIBgbl::GetDBName(EDIBgbl::dbPRJ) + _T("\'  SELECT DISTINCT CustomID FROM [")+ modPHScal::tbnSA+_T("]");
			fprintf(fp, "%d: ->Execute\n", __LINE__);
			modPHScal::dbZDJcrude->Execute((_bstr_t)strSQL, NULL, adCmdText);		
		}
		else
		{
			fprintf(fp, "%d: ->Execute\n", __LINE__);
			EDIBgbl::dbPRJ->Execute((_bstr_t)_T("CREATE TABLE tmpCustomIDSA (CustomID Char(20) CONSTRAINT index1 UNIQUE)"), NULL, adCmdText);
			strSQL=_T("INSERT INTO tmpCustomIDSA IN \'") + EDIBgbl::GetDBName(EDIBgbl::dbPRJ) + _T("\'  SELECT DISTINCT CustomID FROM [")+ modPHScal::tbnSA+_T("]");
			fprintf(fp, "%d: ->Execute\n", __LINE__);
			modPHScal::dbZDJcrude->Execute((_bstr_t)strSQL, NULL, adCmdText);		
		}
	}
	catch(...)
	{
		fprintf(fp, "...");
		fclose(fp);
		return false;
	}

	try
	{
		if(EDIBgbl::tdfExists(EDIBgbl::dbPRJ ,_T("tmpCustomIDPART")))
		{
			fprintf(fp, "%d: ->Execute\n", __LINE__);
			EDIBgbl::dbPRJ->Execute((_bstr_t)_T("DELETE FROM tmpCustomIDPART"), NULL, adCmdText);			
			strSQL=_T("INSERT INTO tmpCustomIDPART IN \'") + EDIBgbl::GetDBName(EDIBgbl::dbPRJ) + _T("\'  SELECT DISTINCT CustomID FROM [") + modPHScal::tbnPART+_T("]");
			fprintf(fp, "%d: ->Execute: %s\n", __LINE__, strSQL);
			modPHScal::dbZDJcrude->Execute((_bstr_t)strSQL, NULL, adCmdText);		
		}		
		else
		{
			fprintf(fp, "%d: ->Execute\n", __LINE__);
			EDIBgbl::dbPRJ->Execute((_bstr_t)_T("CREATE TABLE tmpCustomIDPART (CustomID Char(20) CONSTRAINT index1 UNIQUE)"), NULL, adCmdText);
			strSQL=_T("INSERT INTO tmpCustomIDPART IN \'") + EDIBgbl::GetDBName(EDIBgbl::dbPRJ) + _T("\'  SELECT DISTINCT CustomID FROM [") + modPHScal::tbnPART+_T("]");
			fprintf(fp, "%d: ->Execute\n", __LINE__);
			modPHScal::dbZDJcrude->Execute((_bstr_t)strSQL, NULL, adCmdText);		
		}
	}
	catch(...)
	{
		fprintf(fp, "...");
		fclose(fp);
		return false;
	}

	try
	{
		if(EDIBgbl::tdfExists(EDIBgbl::dbPRJ ,_T("tmpCustomIDSPR")))
		{
			fprintf(fp, "%d: ->Execute\n", __LINE__);
			EDIBgbl::dbPRJ->Execute((_bstr_t)_T("DELETE FROM tmpCustomIDSPR"), NULL, adCmdText);			
			strSQL=_T("INSERT INTO tmpCustomIDSPR IN \'") + EDIBgbl::GetDBName(EDIBgbl::dbPRJ) + _T("\'  SELECT DISTINCT CustomID FROM [")+ modPHScal::tbnSPRINGCrude+_T("]");
			fprintf(fp, "%d: ->Execute\n", __LINE__);
			modPHScal::dbZDJcrude->Execute((_bstr_t)strSQL, NULL, adCmdText);		
		}		
		else
		{
			fprintf(fp, "%d: ->Execute\n", __LINE__);
			EDIBgbl::dbPRJ->Execute((_bstr_t)_T("CREATE TABLE tmpCustomIDSPR (CustomID Char(20) CONSTRAINT index1 UNIQUE)"), NULL, adCmdText);
			strSQL=_T("INSERT INTO tmpCustomIDSPR IN \'") + EDIBgbl::GetDBName(EDIBgbl::dbPRJ) + _T("\'  SELECT DISTINCT CustomID FROM [")+ modPHScal::tbnSPRINGCrude+_T("]");
			fprintf(fp, "%d: ->Execute\n", __LINE__);
			modPHScal::dbZDJcrude->Execute((_bstr_t)strSQL, NULL, adCmdText);		
		}
	}
	catch(...)
	{
		fprintf(fp, "...");
		fclose(fp);
		return false;
	}

	try
	{
		if(EDIBgbl::tdfExists(EDIBgbl::dbPRJ ,_T("tmpCustomIDCSPR")))
		{
			fprintf(fp, "%d: ->Execute\n", __LINE__);
			EDIBgbl::dbPRJ->Execute((_bstr_t)_T("DELETE FROM tmpCustomIDCSPR"), NULL, adCmdText);			
			strSQL=_T("INSERT INTO tmpCustomIDCSPR IN \'") + EDIBgbl::GetDBName(EDIBgbl::dbPRJ) + _T("\'  SELECT DISTINCT CustomID FROM [")+ modPHScal::tbnHDCrude+_T("]");
			fprintf(fp, "%d: ->Execute\n", __LINE__);
			modPHScal::dbZDJcrude->Execute((_bstr_t)strSQL, NULL, adCmdText);		
		}		
		else
		{
			fprintf(fp, "%d: ->Execute\n", __LINE__);
			EDIBgbl::dbPRJ->Execute((_bstr_t)_T("CREATE TABLE tmpCustomIDCSPR (CustomID Char(20) CONSTRAINT index1 UNIQUE)"), NULL, adCmdText);
			strSQL=_T("INSERT INTO tmpCustomIDCSPR IN \'") + EDIBgbl::GetDBName(EDIBgbl::dbPRJ) + _T("\'  SELECT DISTINCT CustomID FROM [")+ modPHScal::tbnHDCrude+_T("]");
			fprintf(fp, "%d: ->Execute\n", __LINE__);
			modPHScal::dbZDJcrude->Execute((_bstr_t)strSQL, NULL, adCmdText);		
		}
	}
	catch(...)
	{
		fprintf(fp, "...\n");
		fclose(fp);
		return false;
	}

	try
	{
		//add by pfg,ligb on 2005.02.18
		//为了提高图形输入界面中LoadGDWItem2ComboGDW1()函数的速度，
		//加一个临时表tmpCustomID_GDW1。因为tbnSA根部数据表常多达7000条以上记录，
		//而临时表tmpCustomID_GDW1只有300条左右记录，速度可提高7000/300=23倍以上。
		if(EDIBgbl::tdfExists(EDIBgbl::dbPRJ ,_T("tmpCustomIDGDW1")))
		{
			fprintf(fp, "%d: ->Execute\n", __LINE__);
			EDIBgbl::dbPRJ->Execute((_bstr_t)_T("DELETE FROM tmpCustomIDGDW1"), NULL, adCmdText);			
			strSQL=_T("INSERT INTO tmpCustomIDGDW1 IN \'") + EDIBgbl::GetDBName(EDIBgbl::dbPRJ) + _T("\'  SELECT DISTINCT CustomID,GDW1 FROM [")+ modPHScal::tbnSA+_T("]");
			fprintf(fp, "%d: ->Execute\n", __LINE__);
			modPHScal::dbZDJcrude->Execute((_bstr_t)strSQL, NULL, adCmdText);		
		}		
		else
		{
			fprintf(fp, "%d: ->Execute\n", __LINE__);
			EDIBgbl::dbPRJ->Execute((_bstr_t)_T("CREATE TABLE tmpCustomIDGDW1 (CustomID Char(20),GDW1 single)"), NULL, adCmdText);
			strSQL=_T("INSERT INTO tmpCustomIDGDW1 IN \'") + EDIBgbl::GetDBName(EDIBgbl::dbPRJ) + _T("\'  SELECT DISTINCT CustomID,GDW1 FROM [")+ modPHScal::tbnSA+_T("]");
			fprintf(fp, "%d: ->Execute\n", __LINE__);
			modPHScal::dbZDJcrude->Execute((_bstr_t)strSQL, NULL, adCmdText);		
		}
	}
	catch(_com_error e)
	{
		fprintf(fp, "%d: _com_error: %s\n", __LINE__, (LPCSTR)e.Description());
		fclose(fp);
		return false;
	}
	fclose(fp);

	return true;
}

void modPHScal::CreatePhsStructureNameAndREFIndbPRJ()
{
	CString SQLx;
		//在此之前，名称和ID都应该被检查
		//先复制PhsStructureNAME从sort.mdb->dbPRJ,因为Jet一次只能连接1个外部表
		if (EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("PhsStructureNAME")))
		{
			//这样做可以避免锁定
			try
			{
				EDIBgbl::dbPRJ->Execute((_bstr_t)_T("DELETE FROM PhsStructureNAME"), NULL, adCmdText | adExecuteNoRecords);
			}
			catch (_com_error &e)
			{
				CString strMsg;
				strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
				AfxMessageBox(strMsg);
			}
			SQLx = CString(_T("INSERT INTO PhsStructureNAME IN \'")) + EDIBgbl::GetDBName(EDIBgbl::dbPRJ) + _T("\' SELECT * FROM PhsStructureNAME ");
			try
			{
				EDIBgbl::dbPRJ->Close();
				EDIBgbl::dbPHScode->Execute((_bstr_t)SQLx, NULL, adCmdText | adExecuteNoRecords);//20071018 "dbSORT" 改为 "dbPHScode"

				EDIBgbl::dbPRJ->Open((_bstr_t)("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" + 
					basDirectory::ProjectDir+_T("WorkPrj.mdb")), "", "", adModeUnknown);

			}
			catch (_com_error &e)
			{
				CString strMsg;
				strMsg.Format("%s:%d %s, \nSQL=%s", __FILE__, __LINE__, (LPSTR)e.Description(), SQLx);
				AfxMessageBox(strMsg);
			}
		}
		else
		{
			//报警：表不存在。
			SQLx = CString(_T("SELECT * INTO PhsStructureNAME IN \'")) + EDIBgbl::GetDBName(EDIBgbl::dbPRJ) + _T("\' FROM PhsStructureNAME ");
			try
			{
				EDIBgbl::dbPHScode->Execute((_bstr_t)SQLx, NULL, adCmdText);//20071018 "dbSORT" 改为 "dbPHScode"
			}
			catch (_com_error &e)
			{
				CString strMsg;
				strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
				AfxMessageBox(strMsg);
			}
		}
		//先复制PhsStructureREF从sort.mdb->dbPRJ,因为Jet一次只能连接1个外部表
		if (EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("PhsStructureREF")))
		{
			try
			{
				EDIBgbl::dbPRJ->Execute((_bstr_t)_T("DELETE FROM PhsStructureREF"), NULL, adCmdText);		
			}
			catch (_com_error &e)
			{
				CString strMsg;
				strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
				AfxMessageBox(strMsg);
			}
			SQLx = CString(_T("INSERT INTO PhsStructureREF IN \'")) + EDIBgbl::GetDBName(EDIBgbl::dbPRJ) + _T("\' SELECT * FROM PhsStructureREF ");
			try
			{
				EDIBgbl::dbPHScode->Execute((_bstr_t)SQLx, NULL, adCmdText);//20071018 "dbSORT" 改为 "dbPHScode"
			}
			catch (_com_error &e)
			{
				CString strMsg;
				strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
//				AfxMessageBox(strMsg);
			}
		}
		else
		{
			//报警：表不存在。
			SQLx = CString(_T("SELECT * INTO PhsStructureREF IN \'")) + EDIBgbl::GetDBName(EDIBgbl::dbPRJ) + _T("\' FROM PhsStructureREF ");
			try
			{
				EDIBgbl::dbPHScode->Execute((_bstr_t)SQLx, NULL, adCmdText);//20071018 "dbSORT" 改为 "dbPHScode"
			}
			catch (_com_error &e)
			{
				CString strMsg;
				strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
				AfxMessageBox(strMsg);
			}
		}
}

void modPHScal::CreatePhsStructureNameAndREFatStart()
{
	//目的:在dbPRJ不存在模板表时,从dbSORT复制两个表.
	CString SQLx;
	try
	{
		//在此之前，名称和ID都应该被检查
		//先复制PhsStructureNAME从sort.mdb->dbPRJ,因为Jet一次只能连接1个外部表
		if (!EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("PhsStructureNAME")))
		{
			SQLx = CString(_T("SELECT * INTO PhsStructureNAME IN \'")) + EDIBgbl::GetDBName(EDIBgbl::dbPRJ) + _T("\' FROM PhsStructureNAME ");
			EDIBgbl::dbPHScode->Execute((_bstr_t)SQLx, NULL, adCmdText);//20071101 "dbSORT" 改为 "dbPHScode"
		}
		//先复制PhsStructureREF从sort.mdb->dbPRJ,因为Jet一次只能连接1个外部表
		if (!EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("PhsStructureREF")))
		{
			SQLx = CString(_T("SELECT * INTO PhsStructureREF IN \'")) + EDIBgbl::GetDBName(EDIBgbl::dbPRJ) + _T("\' FROM PhsStructureREF ");
			EDIBgbl::dbPHScode->Execute((_bstr_t)SQLx, NULL, adCmdText);//20071101 "dbSORT" 改为 "dbPHScode"
		}
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}
}

void modPHScal::WndDataCheck(CWnd *pCtrl, CString strCaption)
{
	ASSERT( pCtrl != NULL );
	ASSERT( IsWindow(pCtrl->GetSafeHwnd()) );
	HWND hCtrl = pCtrl->GetSafeHwnd();

	TCHAR szBuffer[20];
	if( IsWindow(hCtrl) )
	{
		::GetWindowText(hCtrl, szBuffer, 20);
		CString str(szBuffer);
		if( str.IsEmpty() )
		{
			CString strInfo;
			strInfo.Format("请在[%s]中输入数据",strCaption);
			AfxMessageBox(strInfo);
		}
	}
}

void modPHScal::DrawUserDesc(int& iNo,CCadPoint basePt, CMObject &oSpace, CString strTextStyle)
{
	CMObject objEnt;
	CString strReadDesc;
	CFile fileDescription;
	CFileException ex;

	try
	{
		basePt.SetY(basePt[1]+4.13);
		user::AddDirSep( basDirectory::ProjectDir);
		if( !fileDescription.Open(basDirectory::ProjectDir+"UserDesc.txt",CFile::modeRead,&ex) )
		{
			ex.ReportError();
			ex.Delete();
			return;
		}
		UINT nFileLength=fileDescription.GetLength();

		//一次读入所有数据
		int iEnd;
		if( nFileLength>0 )
		{
			iEnd = fileDescription.Read(strReadDesc.GetBufferSetLength(nFileLength),nFileLength);
		}
		else
			return;

		strReadDesc.TrimLeft();
		strReadDesc.TrimRight();

		bool bHasFH=false;
		int iPosA;
		int iPos =  0;//存放最终的分号位置
		int nRChangeLine=0;//回车换行符数
		CString strDesc;
		while( !strReadDesc.IsEmpty() )
		{
			iPosA = strReadDesc.Find(";");
			if( iPosA!=-1 )//只有存在分号的情况才能继续
			{
				iPos = iPosA;
				strDesc = strReadDesc.Mid(0,++iPos);
				strReadDesc = strReadDesc.Mid(iPos);

				strDesc.TrimRight();
				strReadDesc.TrimLeft();

				strDesc.Remove(';');

				nRChangeLine=strDesc.Replace("\r\n","\\P ");

				CString strText;
				strText.Format("%d.%s",iNo++,strDesc);

				//CMObject tmpObj=EDIBAcad::objAcadDoc.GetPropertyByName(_T("Text"));
				//int iWidth = vtoi(tmpObj.GetPropertyByName("Width"));
				//可能会出现 'MText' 对象根据自身 'Width' 属性而换行的动作
				objEnt=oSpace.Invoke(_T("AddMText"),3, (LPVARIANT)basePt, &_variant_t((double)190.0), &_variant_t(strText)).pdispVal;
				objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(strTextStyle));
				objEnt.PutPropertyByName(_T("Height"),&_variant_t((double)4.0));
				
				if( nRChangeLine )
					basePt.SetY(basePt[1] - 7*(nRChangeLine+1));
				else
					basePt.SetY(basePt[1] - 7);
			}
			else
			{
				CString strText;
				strText.Format("%d.%s",iNo++,strReadDesc);

				//可能会出现 'MText' 对象根据自身 'Width' 属性而换行的动作
				objEnt=oSpace.Invoke(_T("AddMText"),3, (LPVARIANT)basePt, &_variant_t((double)190.0), &_variant_t(strText)).pdispVal;
				objEnt.PutPropertyByName(_T("StyleName"),&_variant_t(strTextStyle));
				objEnt.PutPropertyByName(_T("Height"),&_variant_t((double)4.0));
				
				if( nRChangeLine )
					basePt.SetY(basePt[1] - 7*(nRChangeLine+1));
				else
					basePt.SetY(basePt[1] - 7);
				break;
			}
		}
		fileDescription.Close();
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}
}

int modPHScal::SumLugNum()
{  int n,i;
   float gh1,dh1,fRodLen;
   _RecordsetPtr rs;
   rs.CreateInstance(__uuidof(Recordset));
   CString strSQL;
   _variant_t vTmp;
   gh1=vtof(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("GH1")));
   dh1=vtof(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("DH1")));
//   strSQL.Format(_T("SELECT * FROM [%s] WHERE ID='L1'"
  strSQL = _T("SELECT * FROM [") + tbnLugInfo + _T("] WHERE ID=\'L1\'");
   rs->Open((_bstr_t)strSQL, _variant_t((IDispatch*)dbZDJcrude,true), 
	   adOpenKeyset, adLockOptimistic, adCmdText); 
   if(!(rs->BOF && rs->adoEOF))
   {	rs->MoveFirst();
	    rs->get_Collect((_variant_t)_T("maxLength"),&vTmp);
	    fRodLen= vtof(vTmp);
		if((i=int((gh1-dh1)*1000)%int(fRodLen))==0)
			n=(gh1-dh1)*1000/fRodLen;
		else n=(gh1-dh1)*1000/fRodLen+1;
		rs->Close();
   }
   else
   { rs->Close();
     return 0;
   }

   return n;
}

//by ligb on 2005.01.25
//LISEGA恒力弹簧编号生成函数
//方法与步骤:
//1.打开库ZDJcrudeLISEGA.MDB,有三个表:把LISEGALoad荷载系列表里的每一个荷载值LoadValue循环，
//  行程范围代号循环从LISEGALoad荷载系列表里的TravelRangeMin字段到TravelRangeMax字段，
//  根据行程范围代号从TravelSeriesValue行程系列表查得对应的TravelMaxValue值，
//  把荷载值LoadValue循环与行程TravelMaxValue值相乘，结果写到
//  HDforceDist_LISEGA_new荷载容量表的capacity字段，把CSPRNo字段连接TravelRange字段
//  组合成LISEGACSPRNo字段。TravelMaxValue->MaxDist,MinDist->MinDist。
//  manufactory="LISEGA"
//2.按capacity字段值升序排序HDforceDist_LISEGA_new表，对DH字段顺序编号1-73。
//3.这样形成了一个LISEGA恒力弹簧的载荷容量表，它比普通的表多了一个字段LISEGACSPRNo。
//4.以后对恒吊尺寸数据表HDcrude_LISEGA处理：循环该表每个记录，取BH的2、3和4位组成新的
//  荷载行程识别码，当识别码开头字符为6(恒力支架)或7(伺服吊架)或9(横担吊架)时，设置识别码开头字符为1。从HDforceDist_LISEGA_new表
//  LISEGACSPRNo字段值查得与识别码相等的记录，取
//  HDforceDist_LISEGA_new表DH字段值,写入恒吊尺寸数据表HDcrude_LISEGA的DH字段。
//5.完工。
void modPHScal::MakeLISEGA_ConstantSPR_DH()
{//LISEGA恒力弹簧编号生成函数，作用见上。此函数现在驻留于DBConvert.dsp项目中。ligb on 2005.11.21


}
//pfg20051028 根据当前支吊架的类型所得的并联数和根据当
//前模板计算所得的并联数进行比较
//返回:0 表示相等或者当前支吊架类型为空或者不相等而用户愿意更改支吊架类型
//返回:1 表示不相等且用户不愿意更改支吊架类型
//注意：还没有写完!!!
int modPHScal::SpringTypeCompare(CString strType)
{
	CString str=_T("");
	CString strMessage=_T("");
	int flag=0;
	int n;
	if(strType==""||strType.GetLength()<=3||strType.GetLength()>4)
	{
		return 0;
	}
	str=strType.Mid(0,1);
	if(str>="5")
	{//恒力弹簧(start)
		strMessage=_T("提示:\n当前支吊架类型为恒力弹簧,而计算所得的不是恒力弹簧。");
		strMessage+=_T("\n\n用计算所得的支吊架类型替换当前的支吊架类型请按 '是';");
		strMessage+=_T("\n\n否则,请按 '否'(修改初选的弹簧荷载变化系数或支吊架模板后,重试)!");
		if(AfxMessageBox(strMessage,MB_YESNO,1)==IDNO)
		{
			flag=1;	
		}
	}//恒力弹簧(start)
	else
	{//不是恒力弹簧(start)
		str=strType.Mid(1,1);
		if(str>="0"&&str<="9")
		{
			n=atoi(str);
		}
		if(gintParallelNum==n)
		{
		}
	}//不是恒力弹簧(end)
	return flag;
}

//author: pfg20051101
//function: 根据应力计算所得的支吊架类型在符合条件的支吊架类型集中查找
//return:  找到返回1; 不找到返回0;   
int modPHScal::FindPSAType(CString strPSAType, _RecordsetPtr rs)
{
	int flag=0;
	CString strZDJType=_T("");
	_variant_t var;
	CString str=_T("");
	rs->MoveFirst();
	int iSumSerialNum;
	while(!rs->adoEOF)
	{//循环(start)
		rs->get_Collect((_variant_t)_T("SerialNum"),&var);
		iSumSerialNum=vtoi(var);
		str.Format("%d",iSumSerialNum);
		strZDJType=str;
		str.Format("%d",gintParallelNum);
		strZDJType+=str;
		rs->get_Collect((_variant_t)_T("SEQ"),&var);
		str.Format("%02d",vtoi(var));
		strZDJType+=str;
		if(strZDJType==strPSAType)
		{
			flag=1;//应力计算的支吊架类型存在
			break;
		}
		rs->MoveNext();
	}//循环(end)
	if(!flag)
	{
		rs->MoveFirst();//没有找到应按原来的算法选择第一个
	}
	return flag;
}

// 更新Z8表管部的标高（冷态，热态）.添加更新支架计算后的生根面SEL值 20090707
void modPHScal::UpdatePipeDimHeight()
{
	CString strSQL=_T("");
	try
	{
		strSQL.Format("Update Z8 set Design_DH=%f,DH1=%f,work_DH=%f,SEL=%f  where VolumeID=%d and zdjh=%d",
			modPHScal::sngPEL, modPHScal::dyl / 1000, modPHScal::dyr / 1000, modPHScal::sngSEL, EDIBgbl::SelVlmID,
			modPHScal::zdjh );
		EDIBgbl::dbPRJDB->Execute((_bstr_t) strSQL, NULL, adCmdText );
	}
	catch(_com_error e)
	{
	}
}
