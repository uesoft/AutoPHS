// EDIBAcad.h
//

#if !defined(AFX_EDIDBACAD_H__66EB6429_712C_11D5_8752_00D009619765__INCLUDED_)
#define AFX_EDIDBACAD_H__66EB6429_712C_11D5_8752_00D009619765__INCLUDED_


#include "resource.h"


#include "MObject.h"
#include "excel9.h"
#include "CadPoint.h"
#include "VariantArray.h"
#define iViewX	0
#define iViewZ	1
#define iViewXZ 2
typedef struct {CString Name;CString Text;long iWidth;} CADatt;
enum acadColor
{
	acByBlock=0,
	acByLayer=256,
	acRed=1,
	acYellow=2,
	acGreen=3,
	acCyan=4,
	acBlue=5,
	acMagenta=6,
	acWhite=7
};
enum acadSelectMode
{
	acSelectionSetWindow=0,
	acSelectionSetCrossing=1,
	acSelectionSetFence=2,
	acSelectionSetPrevious=3,
	acSelectionSetLast=4,
	acSelectionSetAll=5,
	acSelectionSetWindowPolygon=6,
	acSelectionSetCrossingPolygon=7
};
enum acadHorizontalAlignment
{
	acHorizontalAlignmentLeft=0,
	acHorizontalAlignmentCenter=1 ,
	acHorizontalAlignmentRight=2 ,
	acHorizontalAlignmentAligned=3, 
	acHorizontalAlignmentMiddle=4, 
	acHorizontalAlignmentFit=5 
};
enum acadVerticalAlignment 
{
	acVerticalAlignmentBaseline=0,
	acVerticalAlignmentBottom=1,
	acVerticalAlignmentMiddle=2,
	acVerticalAlignmentTop=3 
};
enum acadEntityType
{
	ac3DFace=1,
	ac3DPolyline=2,
	ac3DSolid=3,
	acArc=4,
	acAttribute=5,
	acAttributeReference=6,
	acBlockReference=7,
	acCircle=8,
	acDimAligned=9,
	acDimAngular=10,
	acDimDiametric=11,
	acDimOrdinate=12,
	acDimRadial=13,
	acDimRotated=14,
	acEllipse=15,
	acGroup=16,
	acHatch=17,
	acLeader=18,
	acLine=19,
	acMText=20,
	acPoint=21,
	acPolyline=22,
	acPolylineLight=23,
	acPolymesh=24,
	acPviewport=25,
	acRaster=26,
	acRay=27,
	acRegion=28,
	acShape=29,
	acSolid=30,
	acSpline=31,
	acText=32,
	acTolerance=33,
	acTrace=34,
	acXline=36
	/*
	acRed=1
	acWhite=2
	acGreen=3
	*/
};
enum XlFindLookIn
{
    xlFormulas = -4123,
    xlComments = -4144,
    xlValues = -4163
};
enum XlLookAt
{
    xlPart = 2,
    xlWhole = 1
};
enum XlSearchOrder
{
    xlByColumns = 2,
    xlByRows = 1
};
enum XlSearchDirection
{
    xlNext = 1,
    xlPrevious = 2
};
class EDIBAcad
{
public:
	static void LoadMyArx(CString strArxAppName);
	static void SetACADSupportPath();
    static BOOL ApplyTextStyleProperty(CMObject &obj, CString strStyleName,CString strChinaName = _T(""));
    static CString GetTextStyleName(CString strTextType, CString strDefaultStyleName);

	static double GetTextStyleWidthFactor(LPCTSTR lpszStyle);
	static BOOL LoadArx(CString strArxName);
	static void GetRange(CString strCell,CString& strCell1,CString & strCell2);
	static CString GetCell(CString strInp,int row,int col);
	static void EDIBAcad::DrawTableExcel(long BillID, _RecordsetPtr& rsText);
	static bool SelectLayerV(long Count, va_list argList);
	static void DisplayDataZB2();
	static void GetActiveAcadDoc();
	static void FindAllFileToTxt(CString dir,CFile* f);
	static bool SelectLayerAllV(long Count,va_list argList);
	static void GetBoundingBox(CMObject &AcObj,	VARIANT* minPoint, VARIANT* maxPoint);
	static HWND GetWindowHandle(CString WindowText);
	static bool bExistEntityInSelectionSet(COleSafeArray& pPointList);
	static void ModifyTextOfBG(CCadPoint& InsPnt,float ELvalue,CString BGtype = "PEL",BOOL bBJBW=FALSE,BOOL bPA=FALSE);
	static bool GetDisPid(IDispatch *lpDispatch, CString csName,DISPID* lpDispid);

    static void SetAcadTop();
    static void SetAcadNoTop();
    static void DrawTKphs(long SizeIndex, long TkExtRatio, long DrawType, long TbhqFlag, long tbosFlag, long TKsubzone, long NoWaiKuang,long scal=0);
    static void WriteToProjectDirphsTxt();
    static void WriteToACADR14_lsp();
    static HWND FindAcad();
    static void AcadActivate();
    static CString GetAcadSrchDir(CString sACADVER);
    static BOOL StartAcad(CString /*Optional*/ DwgName="");
    static void SaveDwgAndExitACAD(COleVariant prompt, COleVariant DwgName);
    static void CloseAcadWin(COleVariant /*Optional*/ ExitMode);
    static void ClearAcadDash(CString DwgName);
    static void MakeNewLayer(CString layerName, int LayerColor=-10);
    static void SetDimVariables();
    static void ModifyAttBG(float /*ByVal*/ ELvalue, COleVariant /*Optional*/ BGtype, COleVariant /*Optional*/ strTagStringOfBG);
    static void ModifyAttributes(CADatt* BlkAtt,long acCount, CMObject& AcObject);
    static void DrawPLine(CCadPoint& StartPoint, float  Length, float  sngRotAng=0, float  PLwidth=0);
	static void DrawPLine(CCadPoint& StartPoint, CCadPoint& EndPoint, float  PLwidth=0);

    static void drawTK(long SizeIndex, long TkExtRatio, long DrawType, long TbhqFlag, long tbosFlag, long TKsubzone, CString TbDir, CCadPoint& p0, CADatt* BlkAtt,long acCount, long NoWaiKuang,long scal=0);
    static void SetACADCurrentStyle(CString sStyle="",CString sBigFont="hzfs",CString sShxFont="txt",float fWidthFactor=1.0f);
    static void SetACADStyleBigFont2Hzfs(CString /*Optional*/ sStyle="");
    static void DeleteACADStyle(CString sStyle="");
	static long GetTableHeaderBlockAttributes(_RecordsetPtr& rs, bool  &bATTBEGIN, long &NPage,long& iNPage,long& SPage, CADatt* & BlkAtt,int iTabIndex=1/*默认为"材料汇总表"(即：EDIBgbl::TCL的值*/);
    static void DrawTableACAD(CCadPoint& pB, long BillID, double sngRotAng, _RecordsetPtr& rsText, long lRowsPerPages=0, float fBlkScale=1.0f, LPCTSTR Layer=_T("bom"), LPCTSTR  Style=_T("hz"),LPCTSTR lpszColTxtFmt=_T("%g"),int iAlign=0);
    static void GetAcadImage();
    static bool InsertPhsBLK(CCadPoint& InsPnt, CString  blk, double  xs, double  ys, double  rotA, int  iGetInsPointMODE=0);
    static void AddData2rsTZG(_RecordsetPtr& rs, long  iRecNo, long  zdjh, CString  FD, CString  sView, CString sBlkName, COleSafeArray& InsPnt);
    static void DrawPhsAssemble(_RecordsetPtr& rsTZB, long iView);
    static void DrawphsZUZI(_RecordsetPtr& rsTZB, CCadPoint& InsPnt, long iView = iViewX);
    static void DrawphsDimOfDW(CCadPoint& InsPnt, CMObject& AcObject, long iView=iViewX);
    static void DrawphsDataEdit();
    static void DisplayDataZB();
    static void Data2DWG(int /*ByVal*/ recno, CString /*ByVal*/ FieldName);
    static void DrawTag(_RecordsetPtr& rsTZG, int  iRecNo, CCadPoint& p0, CCadPoint& p1, long  iSEQ, CString  sLine="LINE", CString  sLayer="Tag");
    static void DrawTagByPickEntity(COleVariant /*Optional*/ pnt);
    static void DrawTagAll();
    static void DrawTagAuto(double p0(), int /*ByVal*/ iRecNo, int /*ByVal*/ iSEQ);
    static void DrawTagPoint(CCadPoint& p0, int iRecNo, int iSEQ, CString sLayer = "Tag");
   static bool AddEntities2SSetInLayers(int /*ByVal*/ Mode, int Count=0,...);
    static bool DeleteAllEntitiesInLayers(int Count=0,...);
    static void EraseAllEntities();
    static _variant_t SetSelectionSet(CString /*ByVal*/ ssetName);
//    static void VBSetXdata(/*Object*/VOID ent, COleVariant /*ByVal*/ vbApp, COleVariant ParamArray xData());
//    static bool VBGetXdata(/*Object*/VOID ent, CString /*ByVal*/ vbApp, int xdataType(), COleVariant xData());
//    static bool VBGetEXdata(/*Object*/VOID ent, CString /*ByVal*/ vbApp, COleVariant /*Optional*/ IxdataType, COleVariant /*Optional*/ VxData);
    static void WblockFromPhsToPhsBlkDir(CString /*ByVal*/ DestBlkDir);
    static CString GetDrawIDAndName(long lngSEQ, CString& strDrawName);
    static void DisplayAcadTop();
    static void OrdinateBound();
    static void MoveObj(long Count,...);
	static void AddSupportPath(const CString &strInsPath);
	static void DeleteSupportPath( );

    /*
     * iGetInsPointMODE
     */
    static int	iGIPmanual;
    static int	iGIPAuto;
	static CADatt AcAtt[16];
    static CCadPoint	MinPoint;
    static CCadPoint	MaxPoint;
    static double	tmpBound[4];
    static double	Bound[4];
    static bool	gbACADR14;
	static bool gbACADR2000;
    static /*Object*/CMObject	objAcadApp;
    static /*Object*/CMObject	objAcadDoc;
    static /*Object*/CMObject	UtilObj;
    static /*Object*/CMObject	MoSpace;
    static /*Object*/CMObject	PaSpace;
    static /*Object*/CMObject	AcEnt;
    static /*Object*/CMObject	AcObj;
    static /*Object*/CMObject	sset;
    static COleSafeArray	AcATTR;
    static CString	gsACADVER;//ACAD版本字符串
    static CString	AcadFirstSrchDir;//
    static CString	sysVarName;
	static CString  sysVarData;
	static int osMode;
	static int sdiMode;
	static int iA0PrjNameWidth;
	static int iA2PrjNameWidth;
	static int iA0DrawNoWidth;
	static int iA2DrawNoWidth;
	static int iA0DrawNameWidth;
	static int iA2DrawNameWidth;
	static int iA0VlmNameWidth;
	static int iA2VlmNameWidth;
	static int iA0DateWidth;
	static int iA2DateWidth;

	static float g_fSumWeight;//用于北京巴威的图标属性"WEIGTH"
	//绘图选项
	static BOOL g_bDrawNameWithoutNo;//图号不带支吊架号
	static BOOL g_bDLOADBJBW;//输出新的荷载位移表格式
	static BOOL g_bBGForBJBW;//输出巴威形式的标高
	static BOOL g_bDWForBJBW;//输出巴威形式的定位
	static BOOL g_bLJMX_BJBW;//输出巴威零件明细表
	static BOOL g_bCustomPicNo;//图号用户手工输入
	static CString g_strCustomPicNoFileName;//用户手工输入的图号名
	static BOOL g_bTagUsingCircle;//编号用"圆"表示还是"线"
	static int g_bGroupWare;//材料汇总表,零件明细表中的"材料"字段中是否将"组件"写成"装配件"(北京巴威公司提出)
	static CString g_strGroupWareName;//字符串：存储“组件”或“装配件”
	static BOOL g_bDrawSumWeight;//在零件明细表的上方是否绘出总重
	static BOOL g_bNotDrawSingleWeight;//在零件明细表中如果零件数目为1，则不绘出其重量
	static BOOL g_bSumBomSprFactory;//是否在材料汇总表中标注弹簧/恒力弹簧厂家
	static BOOL g_bBomSprFactory;   //是否在零件明细表中标注弹簧/恒力弹簧厂家
	static BOOL g_bLocationDimension;//是否标注定位尺寸
	static BOOL g_bInsertLength;//是否标注插入长度
	static BOOL g_bPrintSA_Value;//是否标注根部定位值
	static BOOL g_bInstallElevation;  //不需要标高的‘安装’二字
	static BOOL g_bColdHeatState;//去掉热态标高//去掉冷态标高
	static BOOL g_bHzabs;//	荷载位移表中荷载是否用绝对值表示
	
	static float g_fDistanceOffset;		//超出尺寸线
	static float g_fOvertopDim;			//起点便移量
	static BOOL  g_bDimNumber;      //表注件号
	static float g_fSetTKWidth;  //设置图框宽度
	static float g_fSetTableWidth;//设置表格宽度
	static CString gstrApplicatonName;//绘图平台的COM名称

protected:



private:



};


void SaveAndGetControl(Worksheets objWorksheets,const CString &strDefaultPath);

bool StartExcelAndLoadTemplate(const CString &strSFileName,const CString &strDFileName,
							                                    Worksheets &objWorksheets);
#endif /* EDIBAcad_h */
