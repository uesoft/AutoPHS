
#include "stdafx.h"
#include "user.h"
#include "modRegistry.h"
 
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CLSID clsid;
const _TCHAR szSoftwareKey[]=_T("Software\\长沙优易软件开发有限公司\\AutoPHS\\8.0\\");
const _TCHAR szAutoPSAKey []=_T("Software\\长沙优易软件开发有限公司\\AutoPSA\\");
const _TCHAR szCAESARIIKey[]=_T("Software\\COADE, Inc.\\CAESAR II\\CAESAR II\\");//ligb 2006.06.30
CString GetRegKey(LPCTSTR pszKey, LPCTSTR pszName,const CString Default)
{
	HKEY hKey;
	_TCHAR sv[256];
	unsigned long vtype;
	unsigned long vlen;
	CString subKey=szSoftwareKey;
	subKey+=pszKey;
	if(RegOpenKey(HKEY_LOCAL_MACHINE,subKey,&hKey)!=ERROR_SUCCESS)
	{
		return Default;
	}
	vlen=256*sizeof(TCHAR);
	if(RegQueryValueEx(hKey,pszName,NULL,&vtype,(LPBYTE)sv,&vlen)!=ERROR_SUCCESS)
	{
		RegSetValueEx(hKey,pszName,NULL,REG_SZ,(LPBYTE)(LPCTSTR)Default,(Default.GetLength()+1)*sizeof(_TCHAR));
		RegCloseKey(hKey);
		return Default;
	}
	CString ret=Default;
	if(vtype==REG_SZ)
		ret=sv;
	ret.TrimLeft();ret.TrimRight();
	if(ret==_T(""))
	{
		RegSetValueEx(hKey,pszName,NULL,REG_SZ,(LPBYTE)(LPCTSTR)Default,(Default.GetLength()+1)*sizeof(_TCHAR));
		ret = Default;
	}
	if(vtype==REG_DWORD)
	{
		DWORD dwVal=*((DWORD*)(void*)sv);
		ret.Format(_T("%d"),dwVal);
	}
	RegCloseKey(hKey);
	return ret;
}

//20071026 (start)
CString GetRegKey(LPCTSTR pSoftShareKey,LPCTSTR pszKey, LPCTSTR pszName,const CString Default)
{
	HKEY hKey;
	_TCHAR sv[256];
	unsigned long vtype;
	unsigned long vlen;
	CString subKey=pSoftShareKey;
	subKey+=pszKey;
	if(RegOpenKey(HKEY_LOCAL_MACHINE,subKey,&hKey)!=ERROR_SUCCESS)
	{
		return Default;
	}
	vlen=256*sizeof(TCHAR);
	if(RegQueryValueEx(hKey,pszName,NULL,&vtype,(LPBYTE)sv,&vlen)!=ERROR_SUCCESS)
	{
		RegSetValueEx(hKey,pszName,NULL,REG_SZ,(LPBYTE)(LPCTSTR)Default,(Default.GetLength()+1)*sizeof(_TCHAR));
		RegCloseKey(hKey);
		return Default;
	}
	CString ret=Default;
	if(vtype==REG_SZ)
		ret=sv;
	ret.TrimLeft();ret.TrimRight();
	if(ret==_T(""))
	{
		RegSetValueEx(hKey,pszName,NULL,REG_SZ,(LPBYTE)(LPCTSTR)Default,(Default.GetLength()+1)*sizeof(_TCHAR));
		ret = Default;
	}
	if(vtype==REG_DWORD)
	{
		DWORD dwVal=*((DWORD*)(void*)sv);
		ret.Format(_T("%d"),dwVal);
	}
	RegCloseKey(hKey);
	return ret;
}
//20071026 (end)

CString	GetUSER_RegKey(LPCTSTR pzsPath, LPCTSTR pszKey, LPCTSTR pszName,const CString Default)
{
	HKEY hKey;
	_TCHAR sv[256];
	unsigned long vtype;
	unsigned long vlen;
	CString subKey=pzsPath;
	if( subKey.Right(1) != "\\" )
	{
		subKey += "\\";
	}
	subKey+=pszKey;
	if(RegOpenKey(HKEY_CURRENT_USER,subKey,&hKey)!=ERROR_SUCCESS)
	{
		return Default;
	}
	vlen=256*sizeof(TCHAR);
	if(RegQueryValueEx(hKey,pszName,NULL,&vtype,(LPBYTE)sv,&vlen)!=ERROR_SUCCESS)
	{
		RegSetValueEx(hKey,pszName,NULL,REG_SZ,(LPBYTE)(LPCTSTR)Default,(Default.GetLength()+1)*sizeof(_TCHAR));
		RegCloseKey(hKey);
		return Default;
	}
	CString ret=Default;
	if(vtype==REG_SZ)
		ret=sv;
	ret.TrimLeft();ret.TrimRight();
	if(ret==_T(""))
	{
		RegSetValueEx(hKey,pszName,NULL,REG_SZ,(LPBYTE)(LPCTSTR)Default,(Default.GetLength()+1)*sizeof(_TCHAR));
		ret = Default;
	}
	if(vtype==REG_DWORD)
	{
		DWORD dwVal=*((DWORD*)(void*)sv);
		ret.Format(_T("%d"),dwVal);
	}
	RegCloseKey(hKey);
	return ret;

}

void SetRegValue(LPCTSTR pszKey, LPCTSTR pszName, const CString vValue)
{
	HKEY hKey;
	CString subKey=szSoftwareKey;
	subKey+=pszKey;
	if(RegOpenKey(HKEY_LOCAL_MACHINE,subKey,&hKey)!=ERROR_SUCCESS)
	{
		RegCreateKey(HKEY_LOCAL_MACHINE,subKey,&hKey);
	}
	if(RegSetValueEx(hKey,pszName,NULL,REG_SZ,(LPBYTE)(LPCTSTR)vValue,(vValue.GetLength()+1)*sizeof(TCHAR))!=ERROR_SUCCESS)
	{
		if(::RegDeleteValue(hKey,pszName)==ERROR_SUCCESS)
			RegSetValueEx(hKey,pszName,NULL,REG_SZ,(LPBYTE)(LPCTSTR)vValue,(vValue.GetLength()+1)*sizeof(TCHAR));
	}
	RegCloseKey(hKey);
}

//20071026 (start)
void SetRegValue(LPCTSTR pSoftShareKey,LPCTSTR pszKey, LPCTSTR pszName, const CString vValue)
{
	HKEY hKey;
	CString subKey=pSoftShareKey;
	subKey+=pszKey;
	if(RegOpenKey(HKEY_LOCAL_MACHINE,subKey,&hKey)!=ERROR_SUCCESS)
	{
		RegCreateKey(HKEY_LOCAL_MACHINE,subKey,&hKey);
	}
	if(RegSetValueEx(hKey,pszName,NULL,REG_SZ,(LPBYTE)(LPCTSTR)vValue,(vValue.GetLength()+1)*sizeof(TCHAR))!=ERROR_SUCCESS)
	{
		if(::RegDeleteValue(hKey,pszName)==ERROR_SUCCESS)
			RegSetValueEx(hKey,pszName,NULL,REG_SZ,(LPBYTE)(LPCTSTR)vValue,(vValue.GetLength()+1)*sizeof(TCHAR));
	}
	RegCloseKey(hKey);
}
//20071026 (end)

void SetRegValue(LPCTSTR pszKey, LPCTSTR pszName, const int vValue)
{
	HKEY hKey;
	CString subKey=szSoftwareKey;
	subKey+=pszKey;
	if(RegOpenKey(HKEY_LOCAL_MACHINE,subKey,&hKey)!=ERROR_SUCCESS)
	{
		RegCreateKey(HKEY_LOCAL_MACHINE,subKey,&hKey);
	}
	if(RegSetValueEx(hKey,pszName,NULL,REG_DWORD,(unsigned char*)&vValue,sizeof(vValue))!=ERROR_SUCCESS)
	{
		if(::RegDeleteValue(hKey,pszName)==ERROR_SUCCESS)
			RegSetValueEx(hKey,pszName,NULL,REG_DWORD,(unsigned char*)&vValue,sizeof(vValue));
	}
	RegCloseKey(hKey);
}

void SetRegValue(LPCTSTR pszKey, LPCTSTR pszName, const float vValue)
{
	CString strTemp;
	strTemp.Format(_T("%f"),vValue);
	SetRegValue(pszKey,pszName,strTemp);
}

void SetRegValue(LPCTSTR pszKey, LPCTSTR pszName, const double vValue)
{
	CString strTemp;
	strTemp.Format(_T("%f"),vValue);
	SetRegValue(pszKey,pszName,strTemp);
}
void SetRegValue(LPCTSTR pszKey, LPCTSTR pszName, const bool vValue)
{
	DWORD dwVal=(DWORD)vValue;
	SetRegValue(pszKey,pszName,(int)dwVal);
}

long GetRegKey(LPCTSTR pszKey, LPCTSTR pszName,const long Default)
{
	CString def;
	def.Format(_T("%d"),Default);
	CString var=GetRegKey(pszKey,pszName,def);
	var.MakeUpper();
	if(var==_T("TRUE"))
		return true;
	else if(var==_T("FALSE"))
		return false;
	return _ttol(var);
}

int GetRegKey(LPCTSTR pszKey,LPCTSTR pszName,const int Default)
{
	CString def;
	def.Format(_T("%d"),Default);
	CString var=GetRegKey(pszKey,pszName,def);
	var.MakeUpper();
	if(var==_T("TRUE"))
		return true;
	else if(var==_T("FALSE"))
		return false;
	return _ttoi(var);
}

float GetRegKey(LPCTSTR pszKey, LPCTSTR pszName,const float Default)
{
	CString def;
	def.Format(_T("%f"),Default);
	CString var=GetRegKey(pszKey,pszName,def);
	return (float)_tcstod(var,NULL);
}
bool GetRegKey(LPCTSTR pszKey, LPCTSTR pszName,const bool Default)
{
	CString var=GetRegKey(pszKey,pszName,Default!=false ? CString(_T("True")) : CString(_T("False")));
	var.MakeUpper();
	if(var==_T("TRUE"))
		return true;
	else if(var==_T("FALSE"))
		return false;
	return _ttoi(var);
}
bool UEGetValue(HKEY KeyRoot,CString keyName,CString ValueName,CString& DefValue)
{
	HKEY hKey;
	char sv[256];
	unsigned long vtype;
	unsigned long vlen;
	if(RegOpenKey(KeyRoot,keyName,&hKey)!=ERROR_SUCCESS)
	{
		RegCreateKey(KeyRoot,keyName,&hKey);
	}
	vlen=256;
	if(RegQueryValueEx(hKey,ValueName,NULL,&vtype,(unsigned char*)sv,&vlen)!=ERROR_SUCCESS)
	{
		//RegSetValueEx(hKey,ValueName,NULL,REG_SZ,(unsigned char*)LPCSTR(Default),Default.GetLength()+1);
		DefValue=_T("");	
		RegCloseKey(hKey);
		return false;
	}
	RegCloseKey(hKey);
	if(vtype==REG_SZ)
	{
		DefValue=CString(sv);
		return true;
	}
	if(vtype==REG_DWORD)
	{
		DefValue.Format(_T("%d"),(DWORD)*sv);
		return true;
	}
	return false;
}
bool RegSetValueEx(HKEY KeyRoot,CString keyName,CString ValueName,CString regValue)
{
	HKEY hKey;
	if(RegOpenKey(KeyRoot,keyName,&hKey)!=ERROR_SUCCESS)
	{
		return FALSE;
		//RegCreateKey(KeyRoot,keyName,&hKey);
	}
	if(RegSetValueEx(hKey,ValueName,NULL,REG_SZ,(LPBYTE)LPCTSTR(regValue),(regValue.GetLength()+1)*sizeof(TCHAR))!=ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return FALSE;
	}
	RegCloseKey(hKey);
	return TRUE;
}

int UEGetSettingInstallNumber(CString key,int  defValue)
{
	CString strTemp;
   CString RegAPHSInstallNumberKey;
   CString section = _T("{22C4E4E1-EC0C-11d6-939B-0000E259AC36}");
   //key = _T("1"),安装次数
   //key = _T("2"),运行次数
   int retSz=0;
   RegAPHSInstallNumberKey = _T("AutoCADBackupDWGFile\\CurrentControlSet\\Control\\DeviceClasses");
   if( UEGetValue(HKEY_CLASSES_ROOT, RegAPHSInstallNumberKey + _T("\\") + section, key, strTemp ))
   {
	   if( strTemp == _T("") )
		  retSz= (defValue==-1 ? 0 : defValue);
      else
         retSz= _ttoi(strTemp);
	}
   else{
      //ShowMessage( _T("Get Value occur Error"));
      retSz= (defValue==-1 ? 0 : defValue);
   }
   
   RegAPHSInstallNumberKey = _T(".dwg\\CurrentControlSet\\Control\\DeviceClasses");
   if( UEGetValue(HKEY_CLASSES_ROOT, RegAPHSInstallNumberKey + _T("\\") + section, key, strTemp ))
   {
      if( strTemp == _T("") )
		  retSz= (defValue==-1 ? ((_ttoi(strTemp) < retSz) ? retSz : 0) : defValue);
      else
         retSz= ((_ttoi(strTemp) < retSz) ? retSz : _ttoi(strTemp));
   }
   else{
      //ShowMessage( _T("Get Value occur Error"));
      retSz= (defValue==-1 ? 0 : defValue);
   }
   
   RegAPHSInstallNumberKey = _T("Microsoft Input Devices\\CurrentControlSet\\Control\\DeviceClasses");
   if( UEGetValue(HKEY_LOCAL_MACHINE, RegAPHSInstallNumberKey + _T("\\") + section, key, strTemp ))
   {
      if( strTemp == _T("") )
         retSz= (defValue==-1 ? ((_ttoi(strTemp) < retSz) ? retSz : 0) : defValue);
      else
         retSz= ((_ttoi(strTemp) < retSz) ? retSz : _ttoi(strTemp));
   }  
   else{
      //ShowMessage( _T("Get Value occur Error"));
      retSz= (defValue==-1 ? 0 : defValue);
   }
   
   RegAPHSInstallNumberKey = _T("System\\CurrentControlSet\\Control\\DeviceClasses");
   if( UEGetValue(HKEY_LOCAL_MACHINE, RegAPHSInstallNumberKey + _T("\\") + section, key, strTemp ))
   {
      if( strTemp == _T("") )
         retSz= (defValue==-1 ? ((_ttoi(strTemp) < retSz) ? retSz : 0) : defValue);
      else
         retSz=(( _ttoi(strTemp) < retSz) ? retSz : _ttoi(strTemp));
   }  
	else
	{
      retSz=( defValue==-1 ? 0 : defValue);
   }

   return retSz;
}
void     UESaveSettingInstallNumber(CString key,int  defValue)
{
   HKEY TempKey;
   CString strTmp ;
   strTmp.Format(_T("%d"),defValue);
   
   CString RegAPHSInstallNumberKey;
   CString section = _T("{22C4E4E1-EC0C-11d6-939B-0000E259AC36}");
   //key = _T("1"),安装次数
   //key = _T("2"),运行次数
   
   RegAPHSInstallNumberKey = _T("System\\CurrentControlSet\\Control\\DeviceClasses");
   BOOL bSucc=TRUE;
   if( RegOpenKey(HKEY_LOCAL_MACHINE, RegAPHSInstallNumberKey + _T("\\") + section, &TempKey) !=ERROR_SUCCESS )
   {
	   if(RegCreateKey(HKEY_LOCAL_MACHINE,RegAPHSInstallNumberKey + _T("\\") + section, &TempKey) != ERROR_SUCCESS)
		   bSucc=FALSE;
   }
   if(bSucc)
	   RegSetValueEx(TempKey, key, NULL,REG_SZ,(LPBYTE)LPCTSTR(strTmp),(strTmp.GetLength()+1)*sizeof(TCHAR));
   RegCloseKey(TempKey);
      
   RegAPHSInstallNumberKey = _T("AutoCADBackupDWGFile\\CurrentControlSet\\Control\\DeviceClasses");

   bSucc=TRUE;
   if( RegOpenKey(HKEY_CLASSES_ROOT, RegAPHSInstallNumberKey + _T("\\") + section, &TempKey) !=ERROR_SUCCESS )
   {
	   if(RegCreateKey(HKEY_CLASSES_ROOT,RegAPHSInstallNumberKey + _T("\\") + section, &TempKey) != ERROR_SUCCESS)
		   bSucc=FALSE;
   }
   if(bSucc)
	   RegSetValueEx(TempKey, key, NULL,REG_SZ,(LPBYTE)LPCTSTR(strTmp),(strTmp.GetLength()+1)*sizeof(TCHAR));
   RegCloseKey(TempKey);
   
   RegAPHSInstallNumberKey = _T(".dwg\\CurrentControlSet\\Control\\DeviceClasses");
   bSucc=TRUE;
   if( RegOpenKey(HKEY_CLASSES_ROOT, RegAPHSInstallNumberKey + _T("\\") + section, &TempKey) !=ERROR_SUCCESS )
   {
	   if(RegCreateKey(HKEY_CLASSES_ROOT,RegAPHSInstallNumberKey + _T("\\") + section, &TempKey) != ERROR_SUCCESS)
		   bSucc=FALSE;
   }
   if(bSucc)
	   RegSetValueEx(TempKey, key, NULL,REG_SZ,(LPBYTE)LPCTSTR(strTmp),(strTmp.GetLength()+1)*sizeof(TCHAR));
   RegCloseKey(TempKey);
   
   RegAPHSInstallNumberKey = _T("Microsoft Input Devices\\CurrentControlSet\\Control\\DeviceClasses");
   
   bSucc=TRUE;
   if( RegOpenKey(HKEY_LOCAL_MACHINE, RegAPHSInstallNumberKey + _T("\\") + section, &TempKey) !=ERROR_SUCCESS )
   {
	   if(RegCreateKey(HKEY_LOCAL_MACHINE,RegAPHSInstallNumberKey + _T("\\") + section, &TempKey) != ERROR_SUCCESS)
		   bSucc=FALSE;
   }
   if(bSucc)
	   RegSetValueEx(TempKey, key, NULL,REG_SZ,(LPBYTE)LPCTSTR(strTmp),(strTmp.GetLength()+1)*sizeof(TCHAR));
   RegCloseKey(TempKey);
}

//Retrieves a key value.

void AutoReg()
{
/*
	HKEY hKey1,hKey2,hKey3,hKey4;
	DWORD disp;

	char *obj = "AutoPHS.Object";
	char *doc = "AutoPH Document";
	char *ole = "ole32.dll";

	LPOLESTR lpszCLSID;
	char szCLSID[128]; 
	//"{E8B98C98-5DD5-41B1-A0D6-766045C87464}";
	StringFromCLSID(clsid,&lpszCLSID);  
//	WideCharToMultiByte(CP_ACP,0,lpszCLSID,sizeof(lpszCLSID),szCLSID,sizeof(szCLSID),NULL,NULL);
	W2A (lpszCLSID,szCLSID,128);

	
	char path[255];
	DWORD dwSize = 255; 
	GetModuleFileName(NULL,path,dwSize);

/////处理HKEY_CLASS_ROOT

	if(RegOpenKeyEx(HKEY_CLASSES_ROOT,NULL,0,KEY_ALL_ACCESS,&hKey1) == ERROR_SUCCESS)
	{
		//处理AutoPHS.Object
		if(RegCreateKeyEx(hKey1,"AutoPHS.Object",0,"",REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey2,&disp) == ERROR_SUCCESS)
		{
			if(RegCreateKeyEx(hKey2,"CLSID",0,"",REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey3,&disp) == ERROR_SUCCESS)
			{
				RegSetValueEx(hKey3,NULL,0,REG_SZ, (BYTE*)szCLSID,strlen(szCLSID)+1);
				RegCloseKey(hKey3);
			}
			RegSetValueEx(hKey2,NULL,0,REG_SZ,(BYTE*)doc,strlen(doc)+1);
			RegCloseKey(hKey2);
		}

		//处理CLSID
		if(RegOpenKeyEx(hKey1,"CLSID",0,KEY_ALL_ACCESS,&hKey4) == ERROR_SUCCESS)
		{
			if(RegCreateKeyEx(hKey4,szCLSID,0,"",REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey2,&disp) == ERROR_SUCCESS)
			{
				if(RegCreateKeyEx(hKey2,"InprocHandler32",0,"",REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey3,&disp) == ERROR_SUCCESS)
				{
					RegSetValueEx(hKey3,NULL,0,REG_SZ,(BYTE*)ole,strlen(ole)+1);
					RegCloseKey(hKey3);
				}
				if(RegCreateKeyEx(hKey2,"LocalServer32",0,"",REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey3,&disp) == ERROR_SUCCESS)
				{
					RegSetValueEx(hKey3,NULL,0,REG_SZ,(BYTE*)path,strlen(path)+1);
					RegCloseKey(hKey3);
				}
				if(RegCreateKeyEx(hKey2,"ProgID",0,"",REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey3,&disp) == ERROR_SUCCESS)
				{
					RegSetValueEx(hKey3,NULL,0,REG_SZ,(BYTE*)obj,strlen(obj)+1);
					RegCloseKey(hKey3);
				}

				RegSetValueEx(hKey2,NULL,0,REG_SZ,(BYTE*)doc,strlen(doc)+1);
				RegCloseKey(hKey2);
			}
		}
	}
	RegCloseKey(hKey1);

	/////处理HKEY_LOCAL_MACHINE

	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes",0,KEY_ALL_ACCESS,&hKey1) == ERROR_SUCCESS)
	{
		//处理AutoPHS.Object
		if(RegCreateKeyEx(hKey1,"AutoPHS.Object",0,"",REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey2,&disp) == ERROR_SUCCESS)
		{
			if(RegCreateKeyEx(hKey2,"CLSID",0,"",REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey3,&disp) == ERROR_SUCCESS)
			{
				RegSetValueEx(hKey3,NULL,0,REG_SZ,(BYTE*)szCLSID,strlen(szCLSID)+1);
				RegCloseKey(hKey3);
			}
			RegSetValueEx(hKey2,NULL,0,REG_SZ,(BYTE*)doc,strlen(doc)+1);
			RegCloseKey(hKey2);
		}

		//处理CLSID
		if(RegOpenKeyEx(hKey1,"CLSID",0,KEY_ALL_ACCESS,&hKey4) == ERROR_SUCCESS)
		{
			if(RegCreateKeyEx(hKey4,szCLSID,0,"",REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey2,&disp) == ERROR_SUCCESS)
			{
				if(RegCreateKeyEx(hKey2,"InprocHandler32",0,"",REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey3,&disp) == ERROR_SUCCESS)
				{
					RegSetValueEx(hKey3,NULL,0,REG_SZ,(BYTE*)ole,strlen(ole)+1);
					RegCloseKey(hKey3);
				}
				if(RegCreateKeyEx(hKey2,"LocalServer32",0,"",REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey3,&disp) == ERROR_SUCCESS)
				{
					RegSetValueEx(hKey3,NULL,0,REG_SZ,(BYTE*)path,strlen(path)+1);
					RegCloseKey(hKey3);
				}
				if(RegCreateKeyEx(hKey2,"ProgID",0,"",REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey3,&disp) == ERROR_SUCCESS)
				{
					RegSetValueEx(hKey3,NULL,0,REG_SZ,(BYTE*)obj,strlen(obj)+1);
					RegCloseKey(hKey3);
				}

				RegSetValueEx(hKey2,NULL,0,REG_SZ,(BYTE*)doc,strlen(doc)+1);
				RegCloseKey(hKey2);
			}
		}
	}
	RegCloseKey(hKey1);
*/
}