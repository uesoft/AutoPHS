#if !defined BASDIRECTORY_H
#define BASDIRECTORY_H


enum iEDInBoxXdir
{
//'如果要改变下列排序，也请相应修改资源文件,从resID=60088开始
   iEDIBBlkDir=0,
   iEDIBphsBlkDir,
   iEDIBTemplateDir,
   iEDIBprjDwgDir,
   iEDIBprjDbDir,
   iEDIBprjDir,
   iEDIBinstallDir
};
class basDirectory{
public:
	static bool GetEDinBoxDir();
	static bool GetComDir();
//##ModelId=3CFEA0340212
static  CString InstallDir; 
//##ModelId=3CFEA0340213
static CString DbfDir ;
//##ModelId=3CFEA0340244
static CString PipeDBDir ;
//##ModelId=3CFEA0340245
static CString PipeSupportDBDir; 
//##ModelId=3CFEA0340280
static CString HVACDBDir ;
//##ModelId=3CFEA0340281
static CString HVACSupportDBDir  ;
//##ModelId=3CFEA03402B2
static CString  ProjectDir ;
//##ModelId=3CFEA03402EE
static CString ProjectDBDir  ;
static CString DBShareDir;//20071024
//##ModelId=3CFEA03402EF
static CString ProjectDWGDir  ;
//##ModelId=3CFEA0340320
static CString PrjDBDir  ;
//##ModelId=3CFEA034035C
static CString StressAnaDir  ;
//##ModelId=3CFEA034035D
static CString YLJSJKDir  ;
//##ModelId=3CFEA034038E
static CString CCCFPDir  ;
//##ModelId=3CFEA03403CA
static CString PhsBlkDir  ;
//##ModelId=3CFEA03403CB
static CString BlkDir  ;
//##ModelId=3CFEA0350014
static CString TemplateDir  ;
static CString CommonDir;//20071030
//##ModelId=3CFEA0350050
static CString AcadwinDir  ;
//##ModelId=3CFEA0350082
static CString LspDir  ;
//##ModelId=3CFEA03500BE
static CString RCDir  ;
//##ModelId=3CFEA03500F0
static CString WATERDir  ;
//##ModelId=3CFEA035012C
static CString STEAMDir  ;
//##ModelId=3CFEA035015E
static CString EDITEXE  ;
//##ModelId=3CFEA035019A
static CString EDSetupDir  ;

static CString gstrEDIBdir[7];

static void InitgstrEDIBdir();
};
#endif