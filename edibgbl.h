// EDIBgbl.h
//

#if !defined(EDIBgbl_h)
#define EDIBgbl_h

#include "resource.h"
#include "DataGrid.h"

struct MnuProperty {
   CString MnuCaption;
   bool MnuVisible;
   bool MnuEnabled;
};
struct _VolumeDef{
	long	 VolumeID;
	CString EnginID;
	CString jcdm;
	CString jcmc;
	long SJHYID;
	long SJJDID;
	long ZYID;
};
enum drawType
{
	NewDwg = 0,
	OpenDwg = 1,
	SaveDwg = 2,
	SaveAsDwg = 3,
	SaveAsSampleDwg = 4,
	OutPutPhsBlock = 5,
};
enum dbIndex {
//用于MakeRsTZB过程
   idbPRJ,      //'把数据存入临时工作数据库
   idbPRJDB  //'把数据存入项目数据库
};

class EDIBgbl
{
public:
	static BOOL  g_bPhsManual;
	static BOOL g_bPhsPicture;
	static BOOL g_bPhsSetNum;
//	static BOOL g_bSysDesc;//将来可能用于‘偏装值’和‘本支吊架拉杆直径按单侧承受全部结构荷载设计‘两项的输出控制
	static BOOL g_bUserDesc;
	static BOOL g_bOnlyUserDesc;
	static BOOL g_bOnlySysDesc;
	static BOOL g_bCustomDesc;
//	static bool g_bSysAndUserDesc;
public:
	static double MaxLGLong;
	static void UpdateDBForUpdate();
	static bool bFieldExists(CDaoTableDef *rd, CString sName);
	static bool bIndexExists(CDaoTableDef* pTd,CString strIndex);
	static void UpdateDBTable(_ConnectionPtr &SourceDB, CString SourceTBName, _ConnectionPtr &DesDB, CString DesTBName);
	static void CloseAll_ConnectionPtr();
	static BOOL UpdateSortDB();
	static CString VolumeNo;
	static CString PhsNo;
	static bool ChangeColumnsToRows(_ConnectionPtr &db, CString TblName, CString OutTblName,int iRowStart=1,int iRowCount=1);
	static bool ChangeDatabase(_ConnectionPtr& dDb,_ConnectionPtr &sDb);
	static CString GetTblField(CDaoTableDef& tbldef,CString tblName);
	static bool IsExistentQuery(_ConnectionPtr &db,CString strQueryName);
	static bool bFieldExists(_RecordsetPtr &rs, CString sName);
	static bool DataTabAddRs(_RecordsetPtr &dRsData, _RecordsetPtr &sRsData, long VolumeID);
	
	static int FindStr(CString *pStr,int Count, CString sName);
	static bool VolTabAddRs(_RecordsetPtr &RsVolume, _VolumeDef &VolumeDef);
	static bool FindTable(_ConnectionPtr &db, CString sTab,CString * pSTabName,CString *pSDataTabName,int *n1 ,int *n2);
	static bool UpdateAllPrjDB();
	static bool UpgradeDatabase();
	static void InitWorkTable(_ConnectionPtr& db,CString tbn,int type);
	static CString GetDBName(_ConnectionPtr& db);
	static bool tdfExists(_ConnectionPtr & db,CString tbn);
	static CString GetDBName(_ConnectionPtr db);
	static const int PrjNw;
	static const int GDLJB;
	static const int GDZDJ270;
	static const int GDZDJ180 ;
	static const int CircularFLANGE;
	static const int RectFLANGE;
	static const int IC ;
	static const int EPE ;
	static const int TE ;
	static const int TML ;
	static const int TCL ;//材料汇总表
	static const int TLJ;//零件明细表
	static const int TLJ_BJBW;//北京巴威零件明细表
	static const int TSC ;
	static const int TZD ;
	static const int TCD ;
	static const int TZA ;
	static const int TZB ;
	static const int TZC ;
	static const int TZX ;
	static const int TCLA4 ;
	static const int TCL4;
	static const int TZD180;
	static const int TZDSWED180;
	static const int TZDExcel;
	static const int TCLMX;
	static const int TZF;
	static const int TZG;
	static const int TZLJ;
	static const int TCAD;
	static const int TzDIST ;
	static const int TzLOAD;
	static const int TzDLOAD;
	static const int TzDLOADNEW;
	static const int TzDLOADS;
	static const int TZD270L;
	static const int TMLE;
	static const int TCLE;
	static const int TzDLuser,TZD800;
	static const int TZP;//巴陵石化设计院支吊架一览表
	static const int TZU;//用户自定义表格，自己可以在sort.mdb中的表TableINFO中ID=24的记录行按需要添加表定义。
	static const int TZD800HSY;////核四院支吊架明细一览表
	static const int TZD800QGY;////青工院支吊架明细一览表
	static const int TZD800HBYJ;    //华冶建支吊架明细一览表

    static bool FdExists(_RecordsetPtr  rs, CString  strFieldName);
//    static bool tdfExists(_ConnectionPtr pConn, CString  tbn);
    static bool InitBillType();
    static void InitCurrWork();
    static void ConVertDB(CString  sDBN, CString  dDBN, COleVariant dVer, COleVariant  Pwd);
    static void CheckAllRecords(_RecordsetPtr rs);
    static void CheckAllFields(_RecordsetPtr rs);
    static void InitDBTBN(CString  &strSQL=CString());
    static void LoadMenuSelBill(CMenu* mnuObj);
    static void LSteelInit();
    static void SortField(/*Object MyDBGrid, Object MyData,*/ int MyColIndex);
    static void SaveDBGridColumnCaptionAndWidth(CDataGrid& MyDBGrid, long  ColIndex, CString tbn);
    static void SetDBGridColumnCaptionAndWidth(CDataGrid& MyDBGrid, CString  tbn="");
    static COleVariant va(/*Object Obj*/);
    static void LocateFrm(CWnd* frm, COleVariant  bDisplayWindow);
    static void TraceMainFrm(long  hwnd);
    static bool bFrmLoaded(CString frmName);
    static void InitPrjAndPrjdb();
    static void DataMoveByKey(_RecordsetPtr  rs, int  KeyCode);
    static /*Object*/void CreateCopyQuerydef(_ConnectionPtr  sDb, /*Object  dDb,*/ CString  strQdfName);
    static void InitPhsStructureREFandName();
    static void LoadRecentFiles();
    static CString GetRegistryString(CString  vsItem, CString  vsDefault);
    static void SaveRecentFiles();
    static void GetSelPrjName();
    static bool	SplashState;
	static CString strSelSpec;
    /*
     * Public phsdataloadonce As bool 'frmphsdata
     * multi user variables
     */
    static int	gnMURetryCnt;
    static int	gnMUDelay;
    static int	gnMULocking;
    /*
     * 确定frmDataEdit窗体显示何种数据的全局变量
     */
    static bool	gbDisplayBOM;
    static bool	gbSelSPECset;
    static COleVariant	DBstruArr;
    static CString	SelPrjID;
    static CString	SelPrjName;//工程名
	static CString SelPrjEnglishName;//工程英文名
    static long	SelSpecID;
    static CString	SelSpecName;//专业名称
    static long	SelDsgnID;
    static CString	SelDsgnName;//设计阶段
    static long	SelVlmID;
    static CString	SelVlmName;//卷册名称
    static CString	strDwgVersion;//图形版本号
	static CString SelHyName;
    static long	SelSjhyIndex;
    static long	SelBillType;
    static CString	CompanyID;
    static long	gStartPage;
    static int	giUnitNum;
    static CString	gsSectionKey;
    static CString	gsTitle;
    static CString	CurMdb;
    static CString	TBNSelPrjSpec;
    static CString	TBNSelPrjVlm;
    static CString	TBNSelSpecVlm;
    static CString	tbnphsBOM;
    /*
     * raw data table for zdj calculation
     */
    static CString	TBNRawData;
    static CString	SQLx;
    static CString	SQL1;
    static COleVariant	Bmk;
    /*
     * 功能标志:绘图/计算
     */
    static long	SelFuncFlag;
    static const int	SEL_DRAWBILL;
    static const int	SEL_CALCULATION;
    /*
     * 计算功能
     */
    static const int	CAL_STRESS;
    static const int	CAL_COMBUSTION;
    static const int	CAL_RESISTANCE;
    static const int	CAL_EXHAUSTPIPE;
    static const int	PRJ;
    static const int	TDE;
    static const int	SmallMetalBANK;
    static  CString	ListHead[11];
    static const int	GDTLB;
    /*
     * 插入
     */
    static const int	INSERT_PARTNO;
    static const int	INSERT_ZDJH;
    static const int	INSERT_PIPEDIA;
    static const int	INSERT_NODENO;
    static const int	INSERT_BG_INPUT;
    static const int	INSERT_BG_Y;
    static const int	INSERT_BG_Z;
    static const int	INSERT_BG_DDATTE;
    static const int	PrjIDw;
    static COleVariant	FieldAttrNum;
    static CString	Btype[81];//20071016 "51" 改为 "71"
	static MnuProperty Cbtype[81]; //20071016 "51" 改为 "71"
    static int	DBfields[81];//20071016 "51" 改为 "71"
	 static CString strSelDsgn;
    /*
     * 角钢边长壁厚数据
     */
    //static COleVariant	LSteel;
    /*
     * 静力计算批文件名，位于StressAnaDir
     */
    static const _TCHAR*	YLJSBAT;
    static CString	PSA_OutDataUnit;
    static const float	kgf2N;
    static const float	kgf2MPa;
    static const int	cm2mm;
    /*
     * 以下为TK窗体使用的变量和常数
     */
    static int	DrawNoSel;
    static int	inc;
    static int	MaxExtRatio;
    static int	DrawType;
    static bool	Tbhq;
    static bool	gbTbOS;
    static bool	TKsubzone;
    static bool	TkLspLoaded;
    static float	gsngSumBomWeight;
    static CString	HelpFile;
    static COleVariant	zdjBmpNum;
    static COleVariant	zdjBmp;
    static long	glCurRecordNo;
    static bool	pbFrmLoadedFrmStatus;
    static bool	pbFrmLoadedSELPDSV;
    static bool	pbFrmLoadedphsData;
    static bool	pbFrmLoadedTxsr;
    static bool	pbFrmLoadedPhsSamp;
    /*
     * 公用数据库及变量
     */
	static long SelHyID;
	 static CString SelJcdm;//卷册代号
	 static CString SelZyID;
	static _ConnectionPtr dbPRJ;
	static _ConnectionPtr dbSORT;
    static _ConnectionPtr	dbTable;//20071015

	static _ConnectionPtr dbDSize;//20071018
	static _ConnectionPtr dbMaterial;//20071018
	static _ConnectionPtr dbPHScode;//20071018
	static _ConnectionPtr dbSACal;//20071018

    static _ConnectionPtr	dbPRJDB;
	static CString		strExportPaint;
	static bool IsCalc;//pfg20051021是否正在计算
	static BOOL bSymbol;
protected:

private:


};

#endif /* EDIBgbl_h */
