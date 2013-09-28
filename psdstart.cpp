// PSDstart.cpp
//

#include "stdafx.h"
#include "PSDstart.h"
//#include "EDIBgbl.h"
#include "PHsStart.h"
#include "user.h"
#include "ModEncrypt.h"
//#include "ComStart.h"
#include "EDIBAcad.h"
//#include "modPHScal.h"
#include "basDirectory.h"
#include "EDIBgbl.h"
#include "modPHScal.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
CTime PSDstart::sTime =CTime::GetCurrentTime();
CTime PSDstart::eTime =CTime::GetCurrentTime();
	
void PSDstart::ConvertAllDb(COleVariant dVer, COleVariant Pwd)
{
}

void PSDstart::tmpT()
{
}



void PSDstart::tmpSub2DB()
{
}

void PSDstart::tmpTelmdb2BitWareDefaultDBF()
{
}


bool PSDstart::Start()
{

	
   //开始时间
	sTime=CTime::GetCurrentTime();

	EDIBgbl::SplashState=TRUE;   
   
	user::gstrAppName = "EDInBox";
   
	EDIBgbl::gsSectionKey = "Settings";
   
	user::GetSysDir();
   
   //CreateAppIni gstrWinDir & gstrAppName & ".ini"
   
   if(!basDirectory::GetComDir())
	   goto ErrExit;
	if(!basDirectory::GetEDinBoxDir())
	{
		goto ErrExit;
	}
   //ComStart::CheckCmdLine();
   //<<<<<<<<<<<<<<<<<<<<<<<
	ModEncrypt::CheckLegalUser();


	//此处只是暂时打开
	ModEncrypt::gbLegalUser = true;


	

   
	EDIBgbl::InitPrjAndPrjdb();
   if(!EDIBgbl::InitBillType())
		goto ErrExit;
   //升级用户数据库sort.mdb和allprjdb.mdb,必须位于initBilltype之后

   //conPRJDB4 4.0的驱动，只有升级时用
	conPRJDB4.CreateInstance(__uuidof(Connection));
	conPRJDB4->Open(_bstr_t(::dbConnectionString4+basDirectory::DBShareDir+_T("AllPrjDB.mdb")),//20071025 "ProjectDBDir" 改为 "//20071025 "ProjectDBDir" 改为 "DBShareDir""
		_T(""),_T(""),0);
	
	if(!EDIBgbl::UpgradeDatabase())
		goto ErrExit;

   //InitZdjTxName
   modPHScal::InitPhsSAfx();
   EDIBgbl::SplashState = false;
   //AutoPHSD.Visible = True
   //PhsData.Visible = True
   return TRUE;

ErrExit:
	   
	   return FALSE;
}


