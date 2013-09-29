// ExportPaint.cpp: implementation of the CExportPaint class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "autophs2002.h"
#include "ExportPaint.h"
#include "SelEngVolDll.h"
#include "basDirectory.h"
#include "EDIBgbl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CExportPaint::CExportPaint()
{
	  m_strVlmCODE = "";
	  m_strPrjID = "";

}

CExportPaint::~CExportPaint()
{

}
//选择工程卷册
BOOL CExportPaint::SelEngVol()
{
	CSelEngVolDll dlg;
	LPTSTR lpstrAllPrjDBPathName;
	LPTSTR lpstrsortPathName;
	LPTSTR lpstrworkprjPathName;
		
	if( !GetIPEDPath() )
	{
		AfxMessageBox("当前机器没有安装AutoIPED软件!");
		//选择工程使用的数据库用AutoPHS的库
		m_strDbPath = basDirectory::DBShareDir;//20071109 "ProjectDBDir" 改为 "DBShareDir"
		SetCodePath( m_strDbPath );
		m_strWorkPath = basDirectory::ProjectDir;
		SetCodePath( m_strWorkPath );
		lpstrAllPrjDBPathName=(LPTSTR) (LPCTSTR) ( m_strDbPath + "AllPrjDB.mdb");
	}
	else
		//使用IPED的库
	{
		lpstrAllPrjDBPathName =(LPTSTR) (LPCTSTR) (m_strDbPath + "AutoIPED.mdb");
	}
	lpstrsortPathName     =(LPTSTR) (LPCTSTR) (m_strDbPath + "sort.mdb");
	lpstrworkprjPathName  =(LPTSTR) (LPCTSTR) (m_strWorkPath + "Workprj.mdb");
	if( !dlg.ShowEngVolDlg(lpstrworkprjPathName,lpstrAllPrjDBPathName,lpstrsortPathName) )
	{
		return false;
	}
	m_strPrjID = CString(dlg.m_SelPrjID);
	m_strVlmCODE = "J" + CString(dlg.m_SelVlmCODE);

	return true;
}
//得到IPED注册表信息
BOOL CExportPaint::GetIPEDPath()
{
	HKEY hKey,hKey1;

	char vd[256];
	unsigned long len;
	unsigned long vtype;

	CString strSubKey = "SOFTWARE\\长沙优易软件开发有限公司\\AutoIPED\\";
	if( RegOpenKey(HKEY_LOCAL_MACHINE, strSubKey, &hKey1) != ERROR_SUCCESS )
	{
		return false;
	}
	len = 256;
	if( RegQueryValueEx(hKey1, "AutoIPED_Version", NULL, &vtype, (unsigned char*)vd, &len) != ERROR_SUCCESS )
	{
		RegCloseKey( hKey1 );
		return false;
	}
	RegCloseKey( hKey1 );
	strSubKey += vd;
	strSubKey += "\\Directory\\";
	memset(vd,'\0',256);
	len = 256;

	if( RegOpenKey(HKEY_LOCAL_MACHINE, strSubKey, &hKey) != ERROR_SUCCESS )
	{
		return false;
	}
	//项目数据库路径
	if( RegQueryValueEx(hKey, "EDInBox_prjDbDir", NULL, &vtype, (unsigned char*)vd, &len) != ERROR_SUCCESS )
	{
		RegCloseKey(hKey);
		return false;
	}
	m_strDbPath = vd;
	SetCodePath( m_strDbPath );

	//临时数据库路径
	memset(vd,'\0',256);
	len = 256;
	if( RegQueryValueEx(hKey, "EDInBox_prjDir", NULL, &vtype, (unsigned char*)vd, &len) != ERROR_SUCCESS )
	{
		RegCloseKey( hKey );
		return false;
	}
	m_strWorkPath  = vd;
	SetCodePath( m_strWorkPath );
	
	//安装程序路径
	memset(vd,'\0',256);
	len = 256;
	if(	RegQueryValueEx(hKey, "EDinBox_InsDir", NULL, &vtype, (unsigned char*)vd, &len) != ERROR_SUCCESS )
	{
		RegCloseKey( hKey );
		return false;
	}
	m_strInsPath = vd;
	SetCodePath( m_strInsPath );
	RegCloseKey( hKey );

	return true;

}
//设置为标准的路径名。末尾加'\'
void CExportPaint::SetCodePath(CString &strPath)
{
	strPath.TrimRight();
	while( strPath.Right(1) == "\\" )
	{
		strPath.Delete( strPath.GetLength()-1 );
	}
	strPath += "\\";
}
//启动AutoIPED
bool CExportPaint::StartupAutoIPED()
{
	static PROCESS_INFORMATION	 ProcessInformation;
	STARTUPINFO	 StartupInfo;

	ZeroMemory(&StartupInfo, sizeof(STARTUPINFO) );
	StartupInfo.cb=sizeof(StartupInfo);
	StartupInfo.dwFlags = STARTF_USESHOWWINDOW;

	//向先前打开的程序发送WM_QUIT消息，等待程序的关闭
	::PostThreadMessage(ProcessInformation.dwThreadId, WM_QUIT, 0, 0 );
	::WaitForSingleObject(ProcessInformation.hThread, INFINITE);
	::CloseHandle(ProcessInformation.hThread);
	CloseHandle(ProcessInformation.hProcess);
	ZeroMemory(&ProcessInformation, sizeof(PROCESS_INFORMATION));

	//工程号与卷册代号为AutoPHS当前的.
	m_strPrjID = EDIBgbl::SelPrjID;
	m_strVlmCODE =  EDIBgbl::SelJcdm;
	//命令行
	m_strInsPath += "AutoIPED.exe";
	m_strInsPath += "  -DATA_FILENAME "  + EDIBgbl::strExportPaint;
	m_strInsPath += "  -PROJECT_ID "     + m_strPrjID; 
	m_strInsPath += "  -PROJECT_ENGVOL " + m_strVlmCODE;
//	m_strInsPath += "  -RECORD_COUNT "   + strID;



	//启动程序
	BOOL bo=CreateProcess(NULL,m_strInsPath.GetBuffer(0),
		NULL,NULL,FALSE,NORMAL_PRIORITY_CLASS ,NULL,NULL,
		&StartupInfo,&ProcessInformation);

	/*	
		static PROCESS_INFORMATION ProcessInfomation;
		STARTUPINFO StartupInfo;
		
		ZeroMemory(&StartupInfo,sizeof(STARTUPINFO));
		StartupInfo.cb=sizeof(StartupInfo);
		StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
		StartupInfo.wShowWindow = SW_SHOWDEFAULT;
		
		//向先前打开的程序发送WM_QUIT消息，等待程序的关闭
		::PostThreadMessage(ProcessInfomation.dwThreadId,WM_QUIT,0,0);
		::WaitForSingleObject(ProcessInfomation.hThread,INFINITE);
		::CloseHandle(ProcessInfomation.hThread);
		CloseHandle(ProcessInfomation.hProcess);
		ZeroMemory(&ProcessInfomation,sizeof(PROCESS_INFORMATION));
		strID.Format("%d",ExportInsul.m_ID);
		strVal.Format("%ld",ExportInsul.m_lEngVol);

		strPrjPath += "  -DATA_FILENAME "  + ExportInsul.m_strDataFileName;
		strPrjPath += "  -PROJECT_ID "     + ExportInsul.m_strProjectID; 
		strPrjPath += "  -PROJECT_ENGVOL " + strVal;
		strPrjPath += "  -RECORD_COUNT "   + strID;
		
		BOOL bo=CreateProcess(NULL,strPrjPath.GetBuffer(0),
			NULL,NULL,FALSE,NORMAL_PRIORITY_CLASS ,NULL,NULL,
			&StartupInfo,&ProcessInfomation);
	}	

*/
	return true;
}
