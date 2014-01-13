// modPHScal.h
//

#if !defined(AFX_MODPHSCAL_H__66EB6429_712C_11D5_8752_00D009619765__INCLUDED_)
#define AFX_MODPHSCAL_H__66EB6429_712C_11D5_8752_00D009619765__INCLUDED_

#include "resource.h"
#include "FrmStatus.h"
#include "CadPoint.h"
#include "MObject.h"

struct SelphsINFO
{
	long	SampleID;
   CString PictureClipData;
   CString FREQUENCE;
   CString BmpName;
};
enum PhsSAfxIndex{
   //根部方向索引
   iPX,
   iOZ,
   iOX,
   iPZ
};
enum PhsOutInfoTableTypeIndex{
   iDisplacementLoadSep,
   iDisplacementLoadCombine,
   iDisplacementLoadUser
};

enum PictureClipDataSPCIndex{
   //组件类别表，用于将部件分组为根部/管部/连接件/型钢
   iSA = 0,
   iPA, //1,
   iConnectPART ,//2
   iSectionSteel , //3
   iAttachment ,//4
   iBoltsNuts,  //5
	iOtherPART //6
};

enum PictureClipDataClassID
{
//下面的顺序号从0开始，
//其值必须保持与ZDJCrude.mdb表中的CrudeXXXXid表中的ClassID字段值一致。
   iSAh =0    ,      //直接吊,0
   iSACantilever , //悬臂,1
   iSAbeam     ,   //简支梁2
   iSALbrace   ,   //三角架3
   iSALbraceFixG47 , //三角支架G47,4
   iG51   ,        //抱梁悬臂吊5
   iG52_55     ,   //板肋螺栓简支吊6
   iG56       ,    //夹柱悬臂支架7
   iG57       ,    //夹柱三角支架8
   iGCement ,//9,混凝土梁/板/支墩/etc.,不需要查找数据库
   iSALbraceFixG48 ,   //三角支架G48,10
   iG100  ,  //11,用户自定义根部
   iGx12   , //10,根部结构备用
   iGx13   , //10,根部结构备用
   iGx14   , //10,根部结构备用
   iGx15   , //10,根部结构备用
   iGx16   , //10,根部结构备用
   iGx17   , //10,根部结构备用
   iGx18   , //10,根部结构备用
   iPALX   , //19,管部,矩形管道搁(吊)架
   iPAh    , //20,吊架
   iPAfixZ1, //Z1型(管夹固定支座)
   iPAfixZ2, //Z2型(焊接固定支座)
   iPASS   , //23滑动支架
   iPAGS   , //24导向支架
   iPAs    , //25各种支座
   iPAD4LA ,    //26,水平圆形管道无垫板焊接吊架(烟风煤粉管道LA)
   iPAD11LD ,   //27,垂直圆形管道无垫板焊接吊架(烟风煤粉管道LD)
   iPADLR   ,   //28,水平矩形管道拉条吊架(烟风煤粉管道LR)
   iPAD4LQ  ,   //29,水平矩形管道单槽钢单拉杆焊接吊架(烟风煤粉管道LQ)
   iROD     ,//30,螺纹拉杆
   iConnected , //31,连接件
   iL15   , //32,钢板,连接件结构备用
   iXL1  ,  //33,刚性连杆,改为30,与拉杆一样
   //iConnx33  ,  //32,连接件结构备用
   iConnx34  ,  //32,连接件结构备用
   iConnx35  ,  //32,连接件结构备用
   iConnx36   , //32,连接件结构备用
   iConnx37  ,  //32,连接件结构备用
   iConnx38 ,   //32,连接件结构备用
   iConnx39  ,  //39,连接件结构备用
   iAttached ,  //40,附件
   iNuts     ,  //41,螺母
   iBolts    ,  //42,螺栓
   iAttx43   ,  //43,附件结构备用
   iAttx44    , //44,附件结构备用
   iAttx45    , //45,附件结构备用
   iSPR      ,  //46,弹簧
   iCSPR     ,  //47,恒力弹簧
   iSPR48    ,  //48,弹簧结构备用
   iSPR49     , //49,弹簧结构备用
   iSTEELc    , //50,槽钢       [20a
   iSTEELangle,    //51,角钢    <75x8
   iSTEELH     ,   //52,工字钢  I32
   iSTEELround ,   //圆钢53 %%C64
   iSTEELflat  ,   //扁钢54 -120x10
   iSTEELPlate ,   //钢板55 -100x100x8
   iSTEELUnknown,		//56未知的型钢
   iSTEELUnknown1,  //57未知的型钢
   iSTEELUnknown2,  //58未知的型钢
   iSTEELPipe,  //59钢管Φ108x4.5
   iSABoltsNutsUnknown,  //60未知的根部螺栓螺母附件
   iBoltsNutsUnknown,    //61未知的螺栓螺母
   iSPRPV,    //62,华东院减震弹簧TH6
   iPAXZ1_XZ3=200,//200,限位支架XZ1-XZ3

   iPALU=80,//80,烟风煤粉管道垂直矩形管道支架LU
   iPALV=81,//81,烟风煤粉管道排粉风机进口弯管支架LV
   iPALW=82,//82,烟风煤粉管道风机进风箱支架LW
};

enum AttachIncludedFlag{
   iAttachIncluded,
   iAttachNotIncluded
};
//用于组合多个查找条件的常数,参见frmphsSamp中的ChkConditionIndex枚举.
//可用于在Cphs类中进行判别SQL条件,以便选出满足一定条件的可用零件
enum gcChkCondition{
   iPJG = 1,
   iTJ = 2,
   iGDW1 = 4,
   iCNT = 8,
   iDW = 16,
   iOnlyPAh = 32
};

//弹簧选择的规则
enum giSO{
   lngSOAuto = 0,
   lngSOSerialNumMinSEQmin ,//选择串联数最小弹簧号最小的弹簧组件
   lngSOWeight, //选择重量最小的弹簧
   lngSOSpringNoMin, //选择组件号最小的弹簧
   lngSODifToMidLine,  //选择弹簧工作位移最靠近中线的弹簧
   lngSOCoefOfVarLoad   //选择荷载变化系数最大的弹簧
};

//以下8个枚举型常量表示支吊架类型,0/1/.../7=弹簧吊架/.../支架,详见sort.mdb库PhsTypeName表。
enum ZdjTxNameIndex{
   iZdjSPRh,
   iZdjCSPRh,
   iZdjRigidH,
   iZdjGuide,
   iZdjSlide,
   iZDJfix,
   iZdjSprGuide,
   iZdjSupport
};

enum ZdjClassTypeIndex{
   iPhsSPR,
   iPhsCSPR,
   iPhsRigid
};

struct  SprInfo  //选择的弹簧数据
{
   int SerialNum;
   int SprNo;
   int DH;  //= SerialNum*100+SprNo
   float Hpcmprs;	//与压缩值mm
   float sizeC;	//T5弹簧中心距
   float H0;	//弹簧自由高度mm
   float haz;	//工作范围内安装变形量haz=安装变形量hazTotal-预压缩值Hpcmprs
	float hgz;	//工作范围内工作变形量hgz=工作变形量hgzTotal-预压缩值Hpcmprs
   /*float hazTotal;	//全变形范围内安装变形量hazTotal=Paz*K=haz+Hpcmprs
	float hgzTotal;	//全变形范围内工作变形量hgzTotal=Pgz*K=hgz+Hpcmprs
   float Her;	//安装高度Her=H0-Paz*K=H0-hazTotal
	float Hop;	//工作高度Hop=H0-Pgz*K=H0-hgzTotal
   float DELTAaz;		//工作范围内安装压缩值DELTAaz=DELTAaz1-Hpcmprs=hazTotal-Hcmprs=(haz+Hpcmprs)-Hpcmprs=haz
   float DELTAgz;		//工作范围内工作压缩值DELTAgz=DELTAgz1-Hpcmprs=hgzTotal-Hcmprs=(hgz+Hpcmprs)-Hpcmprs=hgz
   float DELTAaz1;	//全范围内安装压缩值DELTAaz1=H0-Her=H0-(H0-hazTotal)=hazTotal
   float DELTAgz1;	//全范围内工作压缩值DELTAgz1=H0-Hop=H0-(H0-hgzTotal)=hgzTotal
	*/
   //校核恒力弹簧最小最大位移的位移值(根据变量gbCheckMinMaxDispByActualDisp的值，确定用实际热位移HeatDisp还是采用放大20%的计算值)
	//gbCheckMinMaxDispByActualDisp=true,CheckDisp=HeatDisp
	//gbCheckMinMaxDispByActualDisp=false,CheckDisp=1.2*HeatDisp or HeatDisp+15mm
	float SumDisp; //分配给恒力弹簧组件的计算选型热位移，一般比实际热位移HeatDisp大20%。因为大连弹簧和巴威公司说要按选型热位移计算安装高度，故增加此项。
	short SumDistID; //分配给恒力弹簧组件的计算选型热位移代号，只有2007版苏源恒力弹簧规格编号需要这个字段值
	float CheckDisp;   
   float HeatDisp;   //分配给弹簧组件的热位移
   float OpLoad;   //分配给弹簧组件的工作荷载
   float InstallLoad;   //分配给弹簧组件的安装荷载
   float FiJ ;   //恒力弹簧组件的拉杆直径
   float FiK  ;   //恒力弹簧组件的螺栓孔直径
	CString CustomID;
	CString BH;		//BH,用于查出拉杆直径后，T5/LHG等重新生成CLgg时，因为它们的松紧螺母直径必须在计算完成后才能确定，即在GetBoltsNutsAndAttachmentsCLgg函数中,而不在GetPhsBHandSizes中解决。
	CString BHFormat;
	CString CLgg;
};

struct SprDirectionChar   //弹簧件位移方向描述字符（用于弹性件规格编号）
{
	CString CSPR_UP;
   CString CSPR_Down;
   CString SPR_UP;
   CString SPR_Down;
};
extern float vf7850;
class modPHScal
{
private:
	static float fltTmp;
public:	
	static void UpdatePipeDimHeight(); // 更新Z8表管部的标高（冷态，热态）
	static int FindPSAType(CString strPSAType, _RecordsetPtr rs);
	static int SpringTypeCompare(CString strType);
	//LFX 2005.6.30 焊缝绘制选项
	static bool gbDrawWeldSymbol;   //是否绘制焊缝符号  
	static bool gbAutoWeldViewInsertPoint;//是否自动确定辅助视图插入点
	static bool gbDrawWeldViewDim;  //是否绘制视图标注
	static bool gbWeldAutoDirectAndPos;  //是否自动为焊缝符号确定插入位置和方向
	//LFX 2005.6.30 焊缝绘制选项
	static float gmiDiameter;//保存靠近管部的第一根拉杆直径，在1-3次计算之间
	static float giDiameter;//保存拉杆直径，在3次计算之间
	static int SumLugNum();
	static float gfSeqCircleDiameter;
	static float gfSeqTxtHeight;
	static CString g_strDHTGG;
	static BOOL g_bDHT;
	static BOOL bInstalledAndFirstRun;
	static void DrawUserDesc(int& iNo ,CCadPoint basePt, CMObject& oSpace, CString strTextStyle );
	static float g_fLHGWeight;
	static void WndDataCheck(CWnd* pCtrl, CString strCaption);
	static int iAlignLoad;
	static int iAlignDim;
	static int iAlignLjmx;
	static float fFWLoad;
	static CString strBigFontLoad;
	static CString strSHXFontLoad;

	static CString strBigFontLjmx;
	static CString strSHXFontLjmx;
	static CString strBigFontDim;
	static CString strSHXFontDim;
	static float	fFWDim;
	static float    fFWLjmx;
	static bool bE_FormulaPlusHalf;//恒力弹簧高度值是否加实际热位移的一半
	static int iMaxRodDia;
	static CString gsPSAOutFilePath;//选择的应力计算结果文件目录
	static int g_iPSAFilterIndex;//选择的应力计算结果文件类型过滤器索引
	static CString gsSelectProductMdb;//选择的产品数据库目录
	static double gdZJOverValue;
	
	static bool gbAutoSPRRodDiaDecrease;
	static bool gbSPRBHLOADBYInstallLoad;
	static bool gbSPRBHLOADUNITBYNEWTON;
	static bool gbPhsIsSPR;
	static int giMinRodLen;
	static float fDiameter;
	static bool gbRodDiaByMax;
	static BOOL gbNoFenQu;
	static BOOL gbTBhq;
	static BOOL gbNoWaiKuang;
	static bool gbOrientationPicture;//pfg20050531
	static float DblHangerRodDist;
	static int giClassIndex;
	static void CreatePhsStructureNameAndREFatStart();
	static void CreatePhsStructureNameAndREFIndbPRJ();
	static bool CreateTmpCustomIDForPART();
	static int giSAG100No;
	static float gfPSThickness;
	static float gfPSWidth;
	static float gfPSLength;
	static void MakeTmpCalFixPhs();
	static void StressOfMaterial(CString Material,float temperature,float& Sigmat,int iMODE=0,float& Density=vf7850);
	static BOOL CalOffset(_RecordsetPtr rsZA);
	static int SprInfoIndex;
	static bool tbExists(_ConnectionPtr db,_ConnectionPtr db1, CString &tbn, CString s1, CString s2,CString s3);
	// LFX 加 bool bWarn 以此标此来确定在表未找到时是否提出警告  2005.2.21
	static bool HStbExists(_ConnectionPtr db,_ConnectionPtr db1, CString &tbn, CString s1, CString s2,CString s3,bool bWarn = false);  
	static _variant_t sFindAnyTableField(_ConnectionPtr  db, CString  strSourceTable, CString  strSourceFLD, CString  strDestFLD, CString  strSourceFLDvalue);
// 	static bool tbExists(_ConnectionPtr db,_ConnectionPtr db1,CString& tbn,CString s1,CString s2,CString s3);
	static CString GetPhsSAfx(int  SAfx);
	static _ConnectionPtr dbZDJcrude;
//	static int gbTbOS;
//	static long glCurRecordNo;
    static void InitZdjTxName();
    static void InitPhsSAfx();
    static int GetPhsSAfx(CString  SAfx);
    static void GetHeatDisplacement(_RecordsetPtr  rstbl);
    static int iBOMStartNo(int SN=10);
    static _variant_t sFindBlkPosFLD(CString  sFLD, CString  dFLD, CString  sID);
    static _variant_t sFindFLD(CString  strSourceFLD, CString  strDestFLD, CString strSourceFLDvalue);
//    static _variant_t sFindAnyTableField(_ConnectionPtr  db, CString  strSourceTable, CString  strSourceFLD, CString  strDestFLD, CString  strSourceFLDvalue);
    static CString sFindID(CString  CustomID);
    static CString sFindCustomID(CString  ID);
    static CString sFindTBN(CString  PartCustomID);
    static void AutoSEQforPictureClipData();
    static void InitZDJID();
    static void GetRecentWorkSPEC();
    static void SaveRecentWorkSPEC();
    static bool bPAIsHanger();
    static bool blnSelAndSaveSPEC();
    static bool blnSelphsSPEC(bool  mbSPECchanged);
    static void ImportDataFromZdjCrudeXXXX(CString  strFN, bool  bReplacement, bool & mbSPECchanged);
    static void ImportDataFromZdjCrude(CString  strFN, bool  bReplacement, bool & mbSPECchanged);
    static void SetSPRPosValue();
    static long CSLength(CString  sBHFormat, CString  LengthSQL, CString& sBH);
    static bool CheckFieldIfNull(_RecordsetPtr rs);
    static /*Object*/void GetZAdataRs(CWnd*  frm);
    static long lngPreCal();
    static CString GetSaveAsDwgFileName(long zdjh,_RecordsetPtr rs=NULL);
    static void DrawPhs(_RecordsetPtr rsza);
    static void RecordAppend(_RecordsetPtr rs, CString FldName, _variant_t fldValue);
    static void MakeTBDtmpzdj();
    static void MakeTmpCSLen();
    static void MakeZDJ_ZD(long zdjh=10000);
    static _variant_t PhsDrawNameGet(_variant_t ZDJNo);
    static void PhsMLmake(_variant_t ZDJNo);
    static void PhsDisplacementLoadINFOMake(LPCTSTR lpszTextStyle=_T("hz"),int iAlign=0,int iZDLOADtype=20);//TzDLOAD=20
    static void PhsYLBMake(long zdjh=10000);
    static void CreateTmpIDCustomIDTable();
    static void CreateTmpConnectTable();
    static void CreateTmpSPRPropertyTable(int /*Optional*/ SprMaxSerialNum);
    static void CreateTmpSPRPropertyTableUnfinish(int /*Optional*/ SprMaxSerialNum);
    static void SetSpringPhsInfo(_RecordsetPtr rsTmpSelSpring, _RecordsetPtr rsTZA, bool  bLineSPR,CString& mZdjType);
    static void AutoSelSpringNo(_RecordsetPtr rss,float fpgz ,float fpaz,float fyr,float fSPR_CHZBH,float& fTotalInstallCompressValue,
						long lSelSampleID, int nRepZdjType,int iSumDistOfSpring,bool BHintRepZdjType,CString strSpringOrder,
						CString strZdjType,bool bAutoApplyChzbh,CString strbnSPRINGproperty,CString strbnDiscSPRINGpropertyMaxDist);
    static void AutoSelConstantSpringNo(_RecordsetPtr rss,float fpgz,float fyr,bool bCSPRneedSpecialDesign,float fUseConstantSpringMinSurplusDist,
		                long lSelSampleID,CString strZdjType,int& iWholeSprNum,bool bAutoPASA,int nSumSerialNum,CString strbnHDproperty,CString strbnDisplacementSerial);
    static void SetPhsTK3();
    static void CalSpringParallelNum(CString& PipeA=CString(""));
    static void PreCalCrudeData(_RecordsetPtr rstbl,int nth = 1);
    static float sngSpringWeight(CString  strZdjType, _RecordsetPtr rstbl, float /*Optional*/ mvDist=-9999, float& /*Optional*/ sngTotalHeight=fltTmp);
    static float sngSpringWeightNormal(CString  strZdjType, _RecordsetPtr rstbl, float mvDist=-9999, float sngTotalHeight=-9999);
    static long CalRigidZDJType(_RecordsetPtr rstbl);
    static void CalfxSA(_RecordsetPtr rstbl);
    static void CalfxPA(_RecordsetPtr rstbl);
    static void CalDG(_RecordsetPtr rstbl);
    static void CalUnitX(_RecordsetPtr rstbl);
    static float CalUnitCovertCoef(_RecordsetPtr rstbl, int /*Optional*/ iUNIT=0);
    static void UpdateTZB4zdjh(CString FieldName,_variant_t FieldValue);
    static void VB_Cal(_RecordsetPtr  rs, long  zdjh,CFrmStatus &frmStatus,int nth = 1);
    static bool CalRodLength(_RecordsetPtr  rstbl, long  zdjh);
    static void SetBasePoint();
    static void ReadAutoPHSiniValue();
    static void InitializeCrudeData(_RecordsetPtr rs, bool  bFrmTxsrLoaded);
    static void CalPAfixZ1Z2(CString /*ByVal*/ dn, float /*ByVal*/ tmpSelPJG, float /*ByVal*/ tmpT0, int /*ByVal*/ mvariNumPart, _RecordsetPtr rsX);
    /*
     * 当查找connect数据表时使用的字段名称,
     * 默认字段是ID,此时只需要在connect表中增加用户特殊的匹配准则,工作量很小
     * 用户化时也可改为CustomID,
     * 但是这样就得重新输入匹配规则数据表connect,工作量很大.
     */

	static SprInfo sSprInfo[16];
	static SprDirectionChar gSprDirectionChar;
    static const _TCHAR*	gcsCustomID;
    static const _TCHAR*	gcsID;
    static bool	gbStartedOnceAPHS;
    static CString	Unit_Force;
    static CString	Unit_Displacement;
    static CString	Unit_Momental;
    static _variant_t	gBookmark;
    static CString	Ax;
    static int	gintParallelNum;
    static int	gSumDistOfSpring;
    static CString	df1;
    static CString	gf1;
    static CString	df;
    static CString	gf;
    static CString	fx;
    static float	haz;
    static CString	dn;
    static CString	IDdn;
    static float	dj;//管外径
    static float	dwB;
    static float	t0;
    static float	sngPEL;
    static CString	gn;
    static CString	IDgn;
    static float	sngSEL;
    static long	zdjh;
    static CString	dg;
    static float	gdw;
    static float	gdw1;

    static float	gdwx;
    static float	gdwz;
    static float	yr;
    static float	yr1;
    static float	xr;
    static float	zr;
    static float	yl;
    static float	xl;
    static float	zl;
    static float	dyr;
    static float	dxr;
    static float	dzr;
    static float	dyl;
    static float	dxl;
    static float	dzl;
    static float	dxa0;
    static float	dza0;
    static float	dxa;
    static float	dza;
    static float	gpzx;
    static float	gpzz;
    static float	pjg;
    static float	pgz;
    static float	paz;

	//固定支架计算模型坐标系的冷热态承受的力,kgf
	static float pxlc;
	static float pylc;
	static float pzlc;
	static float pxrc;
	static float pyrc;
	static float pzrc;
	static float mxlc;
	static float mylc;
	static float mzlc;
	static float mxrc;
	static float myrc;
	static float mzrc;
	static float PAfixH;

    static int	giNumRod;
    static float	Lspan;
    static float	WidthB;
    static float	WidthA;
    static CString	gstrMaterial;
    static CString	gsPhsArrangementDwgNo;
    static CString	gsPhsScheduleDwgNo;
    static CString	clb;
    static float	SpringRigid;
    static float	SpringLMax;
    static long	iSelSampleID;
	static bool gbSumRodByRoundSteel;
    /*
     * 下面这个变量是有用的，不能由function bFrmLoaded("FrmBmpSe")取代
     * 主要是在frmBmpSe第一次装入时movePos函数需要它。
     */
    static bool	pbFrmLoadedBmp;
    static int	SelInputMethod;
    static CString	CSZdjType;
    static CString	ZdjType;
    static bool	gbPhsIsCSPR;
    /*
     * 全局字符串不能TRIM掉其后的空格，换成COleVariant试试。
     */
    static CString	gsOldPhsPASel;
    static CString	gsOldPhsPARTSel;
    static CString	gsOldPhsSASel;
    static CString	gsOldPhsSPRINGSel;
    static CString	gsOldPhsConstantSpringSel;
    static CString	gsOldPhsBoltsNutsSel;
   	//以下是支吊架标准版本名称的描述，如“西北电力设计院汽水管道支吊架根部(English)”,
	//它对应的名称仍然是“西北电力设计院汽水管道支吊架根部”,该名称要写在组装图上，以便安装时查找焊缝高度。
	static CString	gsPhsPASel;//选择的管部规范名称
    static CString	gsPhsPARTSel;//选择的连接件规范名称
    static CString	gsPhsSASel;//选择的根部规范名称
    static CString	gsPhsSPRINGSel;//选择的弹簧规范名称
    static CString	gsPhsConstantSpringSel;//选择的恒力弹簧规范名称
    static CString	gsPhsBoltsNutsSel;//选择的螺栓螺母规范名称

	//以下是支吊架标准的名称，如“西北电力设计院汽水管道支吊架根部”
    static CString	gsPhsDescPASel;//选择的管部规范描述
    static CString	gsPhsDescPARTSel;//选择的连接件规范描述	
    static CString	gsPhsDescSASel;//选择的根部规范描述
    static CString	gsPhsDescSPRINGSel;//选择的弹簧规范描述
    static CString	gsPhsDescConstantSpringSel;//选择的恒力弹簧规范描述
    static CString	gsPhsDescBoltsNutsSel;//选择的螺栓螺母规范描述

    static CString	gsCSPRmanufactory;//恒力弹簧制造厂
    static CString	gsSPRmanufactory;//弹簧制造厂
    static CString	gsCSPRUnitOfLoad;//恒力弹簧荷载单位
    static CString	gsSPRUnitOfLoad;//弹簧荷载单位
    static CString	tbnTZB;//支吊架结果数据表名称(一般是ZB)
    static CString	tbnBoltsNuts;//螺栓螺母表名称
    static CString	tbnBoltsNutsID;//螺栓螺母ID表名称
    static CString	tbnAttachment;//附件表名称
    static CString	tbnAttachmentID;//附件ID表名称
    static CString	tbnSABoltsNuts;//根部螺栓螺母表名称
    static CString	tbnSABoltsNutsID;//根部螺栓螺母ID表名称
    static CString	tbnBoltsSurplusLength;//螺栓长度裕量表名称
    static CString	tbnPA;//管部表名称
    static CString	tbnSA;//根部表名称
    static CString	tbnPAfix;//固定支架管部表名称
    static CString	tbnPART;//连接件表名称
    static CString	tbnPAID;//管部ID表名称
    static CString	tbnPARTID;//连接件ID表名称
    static CString	tbnSAID;//根部ID表名称
    static CString	tbnSectionSteel;//型钢表名称
    static CString	tbnSectionSteelID;//型钢ID表名称
    static CString	tbnLugInfo;//拉杆信息表名称
    static CString	tbnLugLenCal;//长度计算表名称
    static CString	tbnCONNECT;//厂家标准特殊连接规则表，一般主要用于适应华东院拉杆两端自带螺母的情况
    static CString	tbnHDCrude;//恒吊原始数据表名称
    static CString	tbnHDproperty;//恒吊特性数据表名称
    static CString	tbnHDid;//恒吊ID表
    static CString	tbnDisplacementSerial;//恒吊位移系列表
    static CString	tbnCSPRDiameterSerial;//恒吊直径系列表
    static CString	tbnSPRDiameterSerial;//弹簧直径系列表
    static CString	tbnLugDiaOfCSPR;//恒吊拉杆直径表
    static CString	tbnSPRINGCrude;//弹簧原始数据表
    static CString	tbnSPRINGL5Crude;//弹簧松紧螺母表
    static CString	tbnCSPRINGL5Crude;//恒力弹簧松紧螺母表
    static CString	tbnSPRINGproperty;//弹簧特性数据表
    static CString	tbnDiscSPRINGpropertyMaxDist;//碟簧最大位移特性数据表
    static CString	tbnSPRINGid;//弹簧ID表

    static CString	tbnAllowStress;//许用应力表

    static bool	gbH1IncludeB;//弹簧数据表中高度值H1包含了松紧螺母下缘尺寸B
    static bool	gH1Pos;//弹簧数据表中高度值H1在松紧螺母的位置(上、中、下)
    static bool	gH1distPos;
    static bool	gbCH1IncludeB;
    static bool	gbCH1IncludeT;
    static int	gCH1Pos;
    static int	gCH1distPos;
    static float	gsngTotalInstallCompressValue;
    static bool	gbByForce;
    static float	gUseConstantSpringMinSurplusDist;
    static float	gUseConstantSpringPercentSurplusDist;
    static float	gUseSpringMinDist;
    static float	gnSPR_CHZBH;//弹簧荷载变化系数
    static float	gsngRatioOfPjg2Pgz;
    static float	gnConnectTJ;
    static int	gnRodLenRoundVal;
    static int	gnRodInsertLenRoundVal;
	
    static bool	gbConnectHoleDimMatch;
    static bool	gbSPRAutoLugDia;
    static bool	gbCSPRAutoLugDia;
    static bool	gbAutoApplyChzbh;
    static bool	gbOutputSA;
    static bool	gbCalSAbyConservativenessMethod;
    static bool	gbCalSAbyPJG;
    static bool	gbCalPARTbyMaxLoad;
    static bool	gbAddPartWeight2PMax;
    static bool	gbAddPartWeight2CSPRPgz;
    static bool	gbAddPartWeight2SPRPgz;//2007.07.30
	static bool gbPAForceZero;//2007.09.25
	static bool gbMaxPgzPaz;//2007.09.03
    static bool	gbMinMaxDispByActualDisp;
    static bool	gbCalcCSPRHeight_BySelDisp;
    static bool	gbCalCSPRLugDiaByPjg;
    static bool	gbCSPRneedSpecialDesign;
    static bool	gbCSPRCanSpecialDesign;
    static bool	gbAutoAlignCSPRtoSA;
	static bool gbPipeClampBolt;
    /*
     * 《管道支吊技术》p202 12.1.2(4)与《火力发电厂汽水管道设计技术规定(DL/T 5054-1996)》p67
     * 7.5.4均指出：
     * 拉杆直径DN<=50,d>=10mm;DN>=65,d>=12mm。这些规定与暂行的支吊架标准矛盾。
     * 如西北电力院汽水管道支吊架手册 焊接单板D4.76S孔径13mm,只能与L8.10(d=10mm)连接。这就与规定矛盾。
     * 因此，这里不能按《技术规定》限制直径。否则，连接尺寸检查将失败。
     */
    static bool	gbLimitLugMinDia;//限制拉杆直径
    static bool	gbSumAllVolume;//汇总所有材料
    static bool	gbAutoPASA;//自动选择管部根部
    static bool	gbAutoApplyCSPRLugDia;//自动适应恒吊拉杆直径
    static bool	gbNotPrompt_ACAD_SHIFT;//不提示切换到AutoCAD
    static int	gIsACAD;//选择了AutoCAD绘图
    static bool	gbNotPrompt_Catlog_OVERWRITE;//不提示目录覆盖
    static int	giDrawSIZE_A3A4;//组装图图框使用A3/A4图框：0/1/2=自动/A4/A3
    static int	giPhsOutINFOTableType;//支吊架输出信息表类型 
    static CString	gsPhsColor;//支吊架图块颜色：0/1/2=white/green/auto
    static bool	gbAnnotateCSPR;//清晰标注弹性件规格
    static bool	gbDraw_BOM;//要绘制零件明细表
    static bool	gbDrawOVERWRITE;
    static bool	gbWorksWithBoltsNuts;//厂家自带与连接件连接的螺栓螺母
    static bool	gbWorksWithSABoltsNuts;//厂家自带与根部连接的螺栓螺母
    static float	gnDW_delta;//查找管径范围扩大,0~7(%)
    static int	giAllowedMaxSerialNumPerSPR;//每个弹簧允许最大串联数
    static int	giAllowedMaxSerialNumSPRs;//弹簧组件(包括多个)允许最大串联数
    static int	giSpringOrder;
    static CString	gstrSpringOrder;//弹簧选择的规则，索引值
    static CString	gstrSpringOrderSQL;//弹簧选择的规则
    static bool	AutoOrdinateDrawing;//自动调整组装图布局位置，使其对准空白处中央。
    static long	gnSEQyOffset;//编号之间的最小垂直距离
    static bool	gbDrawNoPreMethod;//支吊架图号前缀符方式
    static bool	gbAutoSaveDrawing;//自动保存当前支吊架图纸
    static long	glDrawNoStartNumber;//支吊架起始图号
    static long	glDWGPreMethod;//支吊架图形文件名前缀符方式
    static CString	gsDWGPre;//支吊架图形文件名前缀符
    static CString	gsDwgFN;//支吊架图形文件名
    static int	giZDJHinDwg;//当前绘制支吊架
    static bool	blResizable;
    static CString	gsPhsTypeRigid;//刚性支吊架
    static CString	gsPhsTypeSPR;  //弹簧支吊架
    static CString	gsPhsTypeCSPR; //恒力弹簧支吊架
    static int	giWholeSprNum;     //总弹簧数
    static int	giSumSerialNum;	   //总串联数
    static int	glNumSA;			//根部数量
    static int	iCSnum;				//根部(元件中)主槽钢数
    static CString	gsPartType;		//零件类型
    static CString	gsOldPartType;	//
    static CString	gsaSPRPos;		//根部弹簧位置
    static bool	gbStopOLE;
    static bool	gbCalAllPhs;		//计算全部支吊架
    static bool	gbStopCalAllPHS;	//停止计算全部支吊架
    /*
     * pt1x,pt1y--注释块右下角X,Y坐标
     * pt2x,pt2y--零件明细表右下角X,Y坐标
     * pt3x,pt3y--荷载位移表右上角X,Y坐标
     */
    static double	VX_pt0x;
    static double	VX_pt0y;
    static double	VZ_pt0x;
    static float	pt1x;
    static float	pt1y;
    static float	pt2x;
    static float	pt2y;
    static float	pt3x;
    static float	pt3y;
    static float	Pt0X;
    static float	giDimOffset;	//尺寸偏移
    static float	giAttDxOffsetX;	//属性Dx偏移X
    static float	giAttDzOffsetX; //属性Dz偏移X
    static float	giAttDOffsetY;  //属性D偏移Y
    /*
     * 根部方向数组：字符串
     */
    static CString	iPhsSAfx[4];
    /*
     * 不含附件的组件零件直径数组，用于保存恒吊拉杆的最小直径，以便准确选择拉杆和其他螺纹零件。
     * 第一遍运行GetphsBHandSizes，找出恒吊拉杆最小直径，荷载已经较精确。
     * 第二遍运行GetphsBHandSizes，所有螺纹零件均要/不要（根据变量gbAutoApplyCSPRLugDia控制）适应恒吊拉杆直径。
     */
    static int	*PtypeDiameter;
	static int  PtypeIndexForDiameter;
    static int	* giCSPRindex;
	static int giCSPRindexSize;
    static int	glIDIndex;
    static long	glClassID;
    static long	glPAid;
    static long	glSAid;
    static CString	gsPhsName;//支吊架类型中文名称
    static CString	gsPhsEnglishName;//支吊架类型英文名称
    static CString	ZdjTxName[20];//支吊架类型中英文混合名称
    //是否取代应力分析程序计算的弹簧结果
	static int iRepZdjType;
	static bool bHintRepZdjType;//用AutoPHS计算的弹簧号取代应力分析AutoPSA计算的弹簧号

	static int iNumCSVal;//根部槽钢数量选择类型  0:自动;1:单;2:双;3:智能自动;

	static BOOL g_bPipeClampBoltDiameterChanged;//指示小荷载大管径管夹螺栓已经更换；
protected:

private:
	void MakeLISEGA_ConstantSPR_DH();

};
extern int giUPxyz;//向上的坐标轴
extern bool gbHotStatus2Zero;//热态吊零计算弹簧
//extern float gfDiameter;//保存拉杆直径，在3次计算之间
#endif 
