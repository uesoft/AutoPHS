#include "stdafx.h"
#include "basDirectory.h"
#include "modRegistry.h"
#include "user.h"
#include "EDIBAcad.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
  CString basDirectory::InstallDir; 
 CString basDirectory::DbfDir ;
//##ModelId=3CFEA0340244
 CString basDirectory::PipeDBDir ;
//##ModelId=3CFEA0340245
 CString basDirectory::PipeSupportDBDir; 
//##ModelId=3CFEA0340280
 CString basDirectory::HVACDBDir ;
//##ModelId=3CFEA0340281
 CString basDirectory::HVACSupportDBDir  ;
//##ModelId=3CFEA03402B2
 CString basDirectory::ProjectDir ;
//##ModelId=3CFEA03402EE
 CString basDirectory::ProjectDBDir  ;
 CString basDirectory::DBShareDir;//20071024
//##ModelId=3CFEA03402EF
 CString basDirectory::ProjectDWGDir  ;
//##ModelId=3CFEA0340320
 CString basDirectory::PrjDBDir  ;
//##ModelId=3CFEA034035C
 CString basDirectory::StressAnaDir  ;
//##ModelId=3CFEA034035D
 CString basDirectory::YLJSJKDir  ;
//##ModelId=3CFEA034038E
 CString basDirectory::CCCFPDir  ;
//##ModelId=3CFEA03403CA
 CString basDirectory::PhsBlkDir  ;
//##ModelId=3CFEA03403CB
 CString basDirectory::BlkDir  ;
//##ModelId=3CFEA0350014
 CString basDirectory::TemplateDir  ;
 CString basDirectory::CommonDir;//20071030
//##ModelId=3CFEA0350050
 CString basDirectory::AcadwinDir  ;
//##ModelId=3CFEA0350082
 CString basDirectory::LspDir  ;
//##ModelId=3CFEA03500BE
 CString basDirectory::RCDir  ;
//##ModelId=3CFEA03500F0
 CString basDirectory::WATERDir  ;
//##ModelId=3CFEA035012C
 CString basDirectory::STEAMDir  ;
//##ModelId=3CFEA035015E
 CString basDirectory::EDITEXE  ;
//##ModelId=3CFEA035019A
 CString basDirectory::EDSetupDir  ;

//##ModelId=3CFEA03501CC
CString basDirectory::gstrEDIBdir[7];

const _TCHAR szUesoftShareKey []=_T("Software\\长沙优易软件开发有限公司\\");//20071025
const _TCHAR szCommonKey []=_T("Software\\UESOFT Shared\\");//20071030

void basDirectory::InitgstrEDIBdir(){
	gstrEDIBdir[iEDIBBlkDir] = user::gstrAppName + _T("_BlkDir");
   gstrEDIBdir[iEDIBphsBlkDir] = user::gstrAppName + _T("_phsBlkDir");
   gstrEDIBdir[iEDIBprjDir] = user::gstrAppName + _T("_prjDir");
   gstrEDIBdir[iEDIBprjDbDir] = user::gstrAppName + _T("_prjDbDir");
   gstrEDIBdir[iEDIBprjDwgDir] = user::gstrAppName + _T("_prjDwgDir");
   gstrEDIBdir[iEDIBTemplateDir] = user::gstrAppName + _T("_TemplateDir");
   gstrEDIBdir[iEDIBinstallDir] = user::gstrAppName + _T("_InsDir");
}


bool basDirectory::GetComDir()
{
	CString key;
	SECURITY_ATTRIBUTES strAtt;
	strAtt.nLength=sizeof(strAtt);
	strAtt.lpSecurityDescriptor=NULL;
	strAtt.bInheritHandle=false;
   key=user::gstrAppName+_T("_DbfDir");
   basDirectory::DbfDir = GetRegKey(_T("Directory"), key,CString(_T("")));
   user::AddDirSep( basDirectory::DbfDir);
   basDirectory::DbfDir.MakeLower();
              
   //管道数据库目录
   key =user::gstrAppName+_T("_PipeDBDir");
	   basDirectory::PipeDBDir = GetRegKey(_T("Directory"), key,CString(_T("")));
  user::AddDirSep( basDirectory::PipeDBDir);
  basDirectory::PipeDBDir.MakeLower();
              
  //支架数据库目录
   key =user::gstrAppName+_T("_PipeSupportDBDir");
   basDirectory::PipeSupportDBDir = GetRegKey(_T("Directory"), key,CString(_T("")));
  user::AddDirSep( basDirectory::PipeSupportDBDir);
  basDirectory::PipeSupportDBDir.MakeLower();
              
   key =user::gstrAppName+_T("_HVACDBDir");
   basDirectory::HVACDBDir = GetRegKey(_T("Directory"), key,CString(_T("")));
  user::AddDirSep( basDirectory::HVACDBDir);
  basDirectory::HVACDBDir.MakeLower();
              
   key =user::gstrAppName+(_T("_HVACSupportDBDir"));
   basDirectory::HVACSupportDBDir = GetRegKey(_T("Directory"), key,CString(_T("")));
  user::AddDirSep( basDirectory::HVACSupportDBDir);
  basDirectory::HVACSupportDBDir.MakeLower();
              
  //模板库目录
   key =user::gstrAppName+(_T("_TemplateDir"));
   basDirectory::TemplateDir = GetRegKey(_T("Directory"), key,CString(_T("")));
   if( basDirectory::TemplateDir == (_T("")) )
   {
	   basDirectory::TemplateDir = CString(user::GetAppPath()) + (_T("\\template"));
      SetRegValue((_T("Directory")), key, basDirectory::TemplateDir);
   }
  user::AddDirSep( basDirectory::TemplateDir);
  basDirectory::TemplateDir.MakeLower();

   key =user::gstrAppName+(_T("_PrjDWGDir"));
	   basDirectory::ProjectDWGDir = GetRegKey(_T("Directory"), key,CString(_T("")));
   if( basDirectory::ProjectDWGDir == (_T("")) )
   {
	   basDirectory::ProjectDWGDir = (_T("c:\\prjdwg"));
      SetRegValue((_T("Directory")), key, basDirectory::ProjectDWGDir);
   }
   if( ! DirExists(basDirectory::ProjectDWGDir) )
	   if( CreateDirectory(basDirectory::ProjectDWGDir,&strAtt) == 0 )
	  {
         ProjectDWGDir = (_T("c:\\")) + basDirectory::ProjectDWGDir.Mid(3);
         ShowMessage(GetResStr(IDS_MsgBox_60687) + basDirectory::ProjectDWGDir);
         SetRegValue((_T("Directory")), key, basDirectory::ProjectDWGDir);
   
		}
  user::AddDirSep(basDirectory::ProjectDWGDir);
  basDirectory::ProjectDWGDir.MakeLower();
   
   key =user::gstrAppName + _T("_PrjDir");
   basDirectory::ProjectDir = GetRegKey(_T("Directory"), key,CString(_T("")));
   if( basDirectory::ProjectDir == _T("") )
   {
	   basDirectory::ProjectDir = _T("c:\\prj");
      SetRegValue(_T("Directory"), key, basDirectory::ProjectDir);
   }
   if( ! DirExists(basDirectory::ProjectDir) )
	   if( CreateDirectory(basDirectory::ProjectDir,&strAtt)==0 )
	   {
         basDirectory::ProjectDir = _T("c:\\") + basDirectory::ProjectDir.Mid( 3);
         ShowMessage(GetResStr(IDS_MsgBox_60688)+basDirectory::ProjectDir);
         SetRegValue(_T("Directory"), key, basDirectory::ProjectDir);
      }
   
  user::AddDirSep( basDirectory::ProjectDir);
  basDirectory::ProjectDir.MakeLower(); 

   key =user::gstrAppName+_T("_PrjDBDir");
   basDirectory::ProjectDBDir = GetRegKey(_T("Directory"), key,CString(_T("")));
   if( basDirectory::ProjectDBDir == _T("") )
   {
      basDirectory::ProjectDBDir = _T("c:\\prjdb");
      SetRegValue(_T("Directory"), key, basDirectory::ProjectDBDir);
   }
   if( ! DirExists(basDirectory::ProjectDBDir) )
   {
	   if( CreateDirectory(basDirectory::ProjectDBDir,&strAtt)==0)
      {
		 basDirectory::ProjectDBDir = _T("c:\\") + basDirectory::ProjectDBDir.Mid( 3);
         ShowMessage(GetResStr(IDS_MsgBox_60689)+basDirectory::ProjectDBDir);
         SetRegValue(_T("Directory"), key, basDirectory::ProjectDBDir);
      }
   }

   //20071024 (start) 
   basDirectory::DBShareDir=GetRegKey(szUesoftShareKey,"shareMDB","shareMDB",(""));
   user::AddDirSep( basDirectory::DBShareDir);
   basDirectory::DBShareDir.MakeLower();
   if( basDirectory::DBShareDir == _T("") )
   {
      basDirectory::DBShareDir = _T("c:\\shareMDB");
      SetRegValue(szUesoftShareKey,_T("shareMDB"), "uesoftshare", basDirectory::DBShareDir);
   }

   if( ! DirExists(basDirectory::DBShareDir) )
   {
	   CreateDirectory(basDirectory::DBShareDir,&strAtt);
   }

   basDirectory::CommonDir=GetRegKey(szCommonKey,"Common mdb","CommonmdbPath",(""));//20071030
   user::AddDirSep(basDirectory::CommonDir);
   basDirectory::CommonDir.MakeLower();
   //20071024 (end)
   
  user::AddDirSep(basDirectory::ProjectDBDir);
   return TRUE;
}

bool basDirectory::GetEDinBoxDir()
{
	try{
   
   CString key;
   
   key = user::gstrAppName + _T("_InsDir");
	basDirectory::InstallDir = GetRegKey(_T("Directory"), key,CString(_T("")));
   user::AddDirSep(InstallDir);
   basDirectory::InstallDir.MakeLower();
   
   key = user::gstrAppName + _T("_StressAnaDir");
	basDirectory::StressAnaDir = GetRegKey(_T("Directory"), key,CString(_T("")));
   user::AddDirSep(StressAnaDir);
   basDirectory::StressAnaDir.MakeLower();
      
   key = user::gstrAppName + _T("_YLJSJKDir");
   basDirectory::YLJSJKDir = GetRegKey(_T("Directory"), key,CString(_T("")));
   user::AddDirSep( YLJSJKDir);
   basDirectory::YLJSJKDir.MakeLower();
      
   key = user::gstrAppName + _T("_CCCFPDir");
   basDirectory::CCCFPDir = GetRegKey(_T("Directory"), key,CString(_T("")));
   user::AddDirSep(CCCFPDir);
   basDirectory::CCCFPDir.MakeLower();
      
   key = user::gstrAppName + _T("_RCDir");
	   basDirectory::RCDir =GetRegKey(_T("Directory"), key,CString(_T("")));
	

   if(basDirectory::RCDir == _T("") )
	   RCDir =CString(user::GetAppPath()) + _T("\\RC");
   else
	   user::AddDirSep(basDirectory::RCDir);
   basDirectory::RCDir.MakeLower();
      
   key = user::gstrAppName + _T("_WATERDir");
	   basDirectory::WATERDir =GetRegKey(_T("Directory"), key,CString(_T("")));
	   user::AddDirSep(basDirectory::WATERDir);
      
   key = user::gstrAppName + _T("_BlkDir");
   basDirectory::BlkDir = GetRegKey(_T("Directory"), key,CString(_T("")));
   CString resStr;
   if(BlkDir == _T("") )
   {
	   resStr.Format(GetResStr(IDS_InRegistryBlkDirIsNull),key);
		if(ShowMessage(resStr,MB_YESNO|MB_DEFBUTTON1|MB_SYSTEMMODAL|MB_ICONQUESTION)==IDYES)
			return false;
   }
   user::AddDirSep(basDirectory::BlkDir);
   basDirectory::BlkDir.MakeLower();
     
   key = user::gstrAppName + _T("_phsBlkDir");
   
	basDirectory::PhsBlkDir = GetRegKey(_T("Directory"), key,CString(_T("")));
	if(basDirectory::PhsBlkDir == _T("") )
	{
		resStr.Format(GetResStr(IDS_InRegistryBlkDirIsNull),key);
		if(ShowMessage(resStr,MB_YESNO|MB_DEFBUTTON1|MB_SYSTEMMODAL|MB_ICONQUESTION)==IDYES)
			return false;
	}
	user::AddDirSep(basDirectory::PhsBlkDir);
	basDirectory::PhsBlkDir.MakeLower();

   key = user::gstrAppName + _T("_LspDir");
	basDirectory::LspDir = GetRegKey(_T("Directory"), key,CString(_T("")));
	user::AddDirSep(basDirectory::LspDir);
	basDirectory::LspDir.MakeLower();
   
   key = _T("acadwin");
   basDirectory::AcadwinDir = GetRegKey(_T("Directory"), key,CString(_T("")));
	user::AddDirSep(basDirectory::AcadwinDir);
	basDirectory::AcadwinDir.MakeLower();
   
   key = _T("EDSetupDir");
   basDirectory::EDSetupDir = GetRegKey(_T("Directory"), key,CString(_T("")));
   user::AddDirSep(basDirectory::EDSetupDir);
   basDirectory::EDSetupDir.MakeLower();
   
      if( AcadwinDir != _T("") )
	  {
		  user::AddDirSep(basDirectory::AcadwinDir);
		SetRegValue( _T("Directory"), _T("acadwin"), basDirectory::AcadwinDir);
      }
		else
		{
		  while (basDirectory::AcadwinDir = _T(""))
		  {
			  basDirectory::AcadwinDir.TrimLeft();
			  basDirectory::AcadwinDir.TrimRight();
		  }
		  user::AddDirSep(basDirectory::AcadwinDir);
		}
		return TRUE;
	}
   catch(CException& e)
   {
	   TCHAR szErr[256];
	   e.GetErrorMessage(szErr,256);
	   ShowMessage(szErr);
	   return false;
   }
}
