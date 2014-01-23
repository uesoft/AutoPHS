// user.cpp
//

#include "stdafx.h"
#include "user.h"
#include "EDIbgbl.h"
#include "modRegistry.h"
#include "FileDialogEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//#include "Form.h"
//#include "PictureBox.h"

 /*const int user::resSTRabout =  49;
 const int user::resSTRBASE =  50;
 const int user::resSTRCompanyTel =  53;
 const int user::resSTRCompanyMTel =  54;
 const int user::resSTRCompanyEmail =  59;
 const int user::resSTR_EDInBox =  150;
 const int user::resBMP_EDInBox =  50;
 const int user::resSTR_WATER =  151;
 const int user::resBMP_WATER =  51;
 const int user::resSTR_RC =  152;
 const int user::resBMP_RC =  52;
 const int user::resSTR_YLJSJK =  153;
 const int user::resBMP_YLJSJK =  53;
 const int user::resSTR_VENTPIPE =  154;
 const int user::resBMP_VENTPIPE =  54;
 const int user::resSTR_STRESSANA =  155;
 const int user::resBMP_STRESSANA =  55;
 const int user::resSTR_PHS =  156;
 const int user::resBMP_PHS =  56;
 const int user::resSTR_eManual =  157;
 const int user::resBMP_eManual =  57;*/
const char* user::gstrNULL = "";
const char* user::gstrSEP_DIR = "\\";
const char* user::gstrSEP_DIRALT = "/";
const char* user::gstrSEP_EXT = ".";
const char* user::gstrCOLON = ":";
 const char* user::gstrSwitchPrefix1 =  "-";
 const char* user::gstrSwitchPrefix2 =  "/";
const char* user::gstrCOMMA = ",";
const char* user::gstrDECIMAL = ".";
 const char* user::gstrINI_PROTOCOL =  "Protocol";
 const int user::gintMAX_SIZE = 255;
 const int user::gintMIN_BUTTONWIDTH = 60;
const float user::gsngBUTTON_BORDER =(float)1.4;
 const int user::intDRIVE_REMOVABLE =2;
 const int user::intDRIVE_FIXED =  3;
 const int user::intDRIVE_REMOTE =  4;
 const int user::gintNOVERINFO =  32767;
CString user::gstrWinDir = CString();
CString user::gstrWinSysDir = CString();
CString user::gstrAppName = CString();
CString user::gstrChnAppName = CString();
CString user::LF = CString();
CString user::LS = CString();
//CWnd* user::gObjMainFrm=NULL;
CString gObjMainFrm;
//DEL void user::CompressFile(_variant_t SrcFile, _variant_t Dest, _variant_t Optional r)
//DEL {
//DEL 
//DEL }

int  vtoi2(_variant_t & v)
{
	int ret=0;
	switch(v.vt)
	{
	case VT_I2:
		return (int)V_I2(&v);
		break;
	case VT_I4:
		return V_I4(&v);
	case VT_R4:
		return (int)V_R4(&v);
	case VT_R8:
		return (int)V_R8(&v);
	case VT_BSTR:
		CString tmps;
		tmps=V_BSTRT(&v);
		return atoi((LPCSTR)tmps);
	}
	return 0;
}

CString FormatVBString(LPCTSTR lpszFmt,double dVal)
{
	int iVal=(int)dVal;    //获得的dVal整数部分
	double dVal2=dVal-iVal;//获得的dVal小数部分

	CString strInt,strFlt,strInt2,strFlt2;
	CString strFmt(lpszFmt);

	strInt.Format(_T("%d"),iVal);
	strFlt.Format(_T("%f"),dVal2);
	strFlt=strFlt.Mid(2);//取小数点后的数
	if(strFmt.GetLength()==0)
	{
		strFlt.Format(_T("%g"),dVal);
		return strFlt;
	}
	CString strFmtInt,strFmtFlt;
	int iFind=strFmt.Find(_T('.'));
	if(iFind==-1)//格式串中不要小数部分
	{
		strFmtInt=lpszFmt;
		//但如果有小数,则将其四舍五入到整数部分
		if( !strFlt.IsEmpty() )//HS->Add
		{
			if( dVal2 >= 0.5 )
			{
				iVal = int(dVal+1);
				strInt.Format(_T("%d"),iVal);
			}
		}
	}
	else
	{
		strFmtInt=strFmt.Left(iFind);
		strFmtFlt=strFmt.Mid(iFind+1);
	}
	
	int i,i1;
	i=strFmtInt.GetLength()-1;
	i1=strInt.GetLength()-1;
	for(;i>=0;i--,i1--)
	{
		TCHAR ch;
		if(strFmtInt[i]==_T('0'))
		{
			if(i1<0)
				ch=_T('0');
			else
				ch=strInt[i1];
		}
		else if(strFmtInt[i]==_T('#'))
		{
			if(i1<0)
				ch=_T('\0');
			else
				ch=strInt[i1];
		}
		else 
		{
			ch=strFmtInt[i];
			i1++;
		}
		if(ch!=0)
			strInt2=ch+strInt2;
	}
	
	if(i1>=0)
		strInt2=strInt;
	int len=strFmtFlt.GetLength();
	int len1=strFlt.GetLength();
	for(i=0,i1=0;i<len;i++,i1++)
	{
		TCHAR ch;
		if(strFmtFlt[i]==_T('0'))//'0'
		{
			if(i1>=len1)
				ch=_T('0');
			else
				ch=strFlt[i1];
		}
		else if(strFmtFlt[i]==_T('#'))	//'#'
		{
			if(i1>=len1)
				ch=_T('\0');
			else 
			{
				if(i==len-1 && strFlt[i1]==_T('0'))
					ch=_T('\0');
				else
					ch=strFlt[i1];
			}
		}
		else
		{
			ch=strFmtFlt[i];
			i1--;
		}
		if(ch!=0)
			strFlt2+=ch;
	}

	if(strFlt2.GetLength()==0)
	{
		return strInt2;
	}
	return strInt2 + _T('.') + strFlt2;
}
void user::GetSysDir()
{
	::GetWindowsDirectory(gstrWinDir.GetBuffer(MAX_PATH),MAX_PATH);
	gstrWinDir.ReleaseBuffer();
	::GetSystemDirectory(gstrWinSysDir.GetBuffer(MAX_PATH),MAX_PATH);
   gstrWinSysDir.ReleaseBuffer();
   AddDirSep(gstrWinDir);
   AddDirSep(gstrWinSysDir);
}

CString user::ChangePathSlash(CString Path)
{
	int i=0;
	int t;
	while((t=Path.Find("\\",i))!=-1)
	{
		Path=Path.Left(t)+"\\\\"+Path.Mid(t+1);
		i=t+2;
	}
	return Path;
}

CString user::SelFile(CString DlgTitle, CString filter, int FilterInd, CString InitPath, bool CreatePrompt)
{
	CFileDialog fdg(TRUE);
	int i=0;
	char* lpszFilter=NULL;
	int len=filter.GetLength();
	lpszFilter=new char[len+2];
	memcpy(lpszFilter,(LPCTSTR)filter,len);
	lpszFilter[len]='\0';
	lpszFilter[len+1]='\0';
	i=0;
	while(lpszFilter[i]!='\0')
	{
		if(lpszFilter[i]=='|')
			lpszFilter[i]='\0';
		i++;
	}
	fdg.m_ofn.lpstrInitialDir=(LPCSTR)InitPath;
	fdg.m_ofn.lpstrFilter=lpszFilter;
	fdg.m_ofn.nFilterIndex=FilterInd;
	fdg.m_ofn.lpstrTitle=DlgTitle;
	if(CreatePrompt)
		fdg.m_ofn.Flags |= OFN_CREATEPROMPT;
	//fdg.m_ofn.Flags=OFN_PATHMUSTEXIST|OFN_EXPLORER;
	if(fdg.DoModal()==IDOK)
	{
		delete [] lpszFilter;
		return fdg.GetPathName();
	}
	delete [] lpszFilter;
	return "";
}

int user::s_iFilterIndex=1;//默认的文件类型过滤器索引

CString user::SelFileDwg(LPCTSTR filter, CString& initDir,int& iFilterIndex)
{
	CFileDialogEx fdg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,filter);
	fdg.m_ofn.lpstrInitialDir=initDir;
	fdg.m_ofn.nFilterIndex=iFilterIndex;
	if(fdg.DoModal()==IDOK)
	{
		initDir=::GetFilePath(fdg.GetPathName());
		iFilterIndex=fdg.m_ofn.nFilterIndex;
		return fdg.GetPathName();
	}
	iFilterIndex=fdg.m_ofn.nFilterIndex;
	return "";
}

void user::AddDirSep(CString &strPathName)
{
	if(strPathName=="")
	{
		strPathName+="\\";
		return;
	}
	strPathName.TrimLeft();
	strPathName.TrimRight();
	if(strPathName.GetAt(strPathName.GetLength()-1)!='\\')
		strPathName+="\\";
}

void user::RemoveDirSep(CString& strPathName)
{
	strPathName.TrimLeft();
	strPathName.TrimRight();
	while(strPathName.GetAt(strPathName.GetLength()-1)=='\\')
		strPathName=strPathName.Left(strPathName.GetLength()-1);
}

bool user::GetPos( CString frmName,CRect& wndRect)
{
	CString frmPos;
	int i;
	frmName.TrimLeft();frmName.TrimRight();
	if(frmName=="")
		return false;
	CString szValue="";
   frmPos = GetRegKey(_T("Settings"), frmName + "Pos2", szValue);
   if (frmPos=="") return false;
	int srcW=::GetSystemMetrics(SM_CXSCREEN);
	int srcH=::GetSystemMetrics(SM_CYSCREEN);
   frmPos.TrimLeft();frmPos.TrimRight();

   wndRect.left= atoi(frmPos);
   if(/*rc.left<0 || */wndRect.left>srcW)
	   return false;
   i=frmPos.Find(' ');
   if(i==-1)
	   return false;
   frmPos=frmPos.Mid(i);
   frmPos.TrimLeft();
   wndRect.top=atoi(frmPos);
   if(/*rc.top<0 || */wndRect.top>srcH)
	   return false;

   i=frmPos.Find(' ');
   if(i==-1)
	   return false;
   frmPos=frmPos.Mid(i);
   frmPos.TrimLeft();
   wndRect.right=wndRect.left+atoi(frmPos);
   if(wndRect.Width()<50 || wndRect.Width()>srcW)
	   return false;

   i=frmPos.Find(' ');
   if(i==-1)
	   return false;
   frmPos=frmPos.Mid(i);
   frmPos.TrimLeft();
   wndRect.bottom=wndRect.top+atoi(frmPos);
   if(wndRect.Height()<200 || wndRect.Height()>srcH)
	   return false;

   return true;
}

void user::SavePos(CWnd* frm,CString frmName)
{
	frmName.TrimLeft();frmName.TrimRight();
	if(frmName=="")
		return ;
	if(frm==NULL)
		return;
	CString rcSz;
	CRect rc;
	frm->GetWindowRect(&rc);
	rcSz.Format("%d %d %d %d",rc.left,rc.top,rc.Width(),rc.Height());
	SetRegValue(_T("Settings"), frmName+"Pos2",rcSz);
}


void user::EditTextBox(CEdit* editBox)
{
	if(editBox==NULL)
		return;

}



CString user::GetAppPath()
{
	char sz[MAX_PATH];
	GetModuleFileName( ::AfxGetInstanceHandle(),sz,MAX_PATH);
	return GetFilePath(sz);
}


CString user::GetAppFileName()
{
	char sz[MAX_PATH];
	::GetModuleFileName(::AfxGetInstanceHandle(),sz,MAX_PATH);
	return CString(sz);
}

DWORD user::GetVersion()
{
	VS_FIXEDFILEINFO *verInfo=NULL;
	DWORD uLen;
	LPVOID lpBlack;
	DWORD BlackLen;
	CString AppFileName=GetAppFileName();
	DWORD handle;
	BlackLen=::GetFileVersionInfoSize((char*)(LPCSTR)AppFileName,&handle);
	lpBlack=new BYTE [BlackLen];
	::GetFileVersionInfo((char*)(LPCSTR)GetAppFileName(),handle,BlackLen,lpBlack);
	uLen=BlackLen;
	::VerQueryValue(lpBlack,"\\",(void**)&verInfo,(unsigned int*)&uLen);

	DWORD ret=0;
	if(verInfo)
		ret= verInfo->dwProductVersionMS;
	delete []/*BlackLen]*/ lpBlack;
	return ret;
}
int  vtoi(_variant_t & v)
{
	CString tmps;
	int ret=0;
	switch(v.vt)
	{
	case VT_I2:
		ret= (int)V_I2(&v);
		break;
	case VT_I4:
		ret= V_I4(&v);
		break;
	case VT_R4:
		ret= (int)V_R4(&v);
		break;
	case VT_R8:
		ret =(int)V_R8(&v);
		break;
	case VT_BSTR:
		tmps=(char*)_bstr_t(&v);
		ret=atoi((LPCSTR)tmps);
		break;
	}
	return ret;
}
int  vtoi(COleVariant & v)
{
	CString tmps;
	int ret=0;
	switch(v.vt)
	{
	case VT_I2:
		ret= (int)V_I2(&v);
		break;
	case VT_I4:
		ret= V_I4(&v);
		break;
	case VT_R4:
		ret= (int)V_R4(&v);
		break;
	case VT_R8:
		ret =(int)V_R8(&v);
		break;
	case VT_BSTR:
		tmps=V_BSTRT(&v);
		ret=atoi((LPCSTR)tmps);
		break;
	}
	return ret;
}
//const CString dbConnectionString=
//	"Provider=Microsoft.Jet.OLEDB.4.0 ;Data Source=";//
const _TCHAR dbConnectionString4[]=_T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=");
const _TCHAR dbConnectionString[]=_T("Provider=Microsoft.Jet.OLEDB.3.51;Data Source=");

_ConnectionPtr conPRJDB=NULL;
_ConnectionPtr conPRJDB4=NULL;
_ConnectionPtr conPHSConfig = NULL;
_ConnectionPtr conZDJCrude = NULL;


CString inline GetResStr(UINT id)
{
	CString s;
	s.LoadString(id);
	return s;

}
bool FileExists(CString filename)
{
	CFileStatus fileStatus;
	if(CFile::GetStatus(filename,fileStatus))
		return TRUE;
	return FALSE;
}
bool inline  IsNull(_variant_t &v)
{
	return (v.vt==VT_NULL);
}
bool DirExists(CString Directory)
{
	DWORD Code;
	Code = GetFileAttributes(Directory);
	if(Code!=-1 && (Code & FILE_ATTRIBUTE_DIRECTORY) != 0)
		return TRUE;
	return FALSE;
}
void SetFileTime(CString Sfile,CString Dfile)
{
	CFileStatus DfileStatus;
	CFileStatus SfileStatus;
	if(!CFile::GetStatus(LPCTSTR(Dfile),DfileStatus))
		return;
	if(!CFile::GetStatus(LPCTSTR(Sfile),SfileStatus))
		return;
	DfileStatus.m_ctime=SfileStatus.m_ctime;
	DfileStatus.m_mtime=SfileStatus.m_mtime;
	CFile::SetStatus(LPCTSTR(Dfile),DfileStatus);
}
CString  Trim(LPCTSTR pcs)
{
	CString s=pcs;
	s.TrimLeft();
	s.TrimRight();
	return s;
}
CString GetFilePath(CString FileName)//不带 '\'
{
	//获得文件路径，不含文件名
	int len=FileName.GetLength();
	char c;
	CString ret;
	for(int i=len;i>0;i--)
	{
		c=FileName.GetAt(i-1);
		if(c=='\\')
		{
			if(i>1)
				ret=FileName.Left(i-1);
			else ret="";
			return ret;
		}
	}
	return "";
}
float vtof(_variant_t &v)
{
	float ret=0;
	CString tmps;
	switch(v.vt)
	{
	case VT_R4:
		ret = V_R4(&v);
		break;
	case VT_R8:
		ret=(float)V_R8(&v);
		break;
	case VT_I2:
		ret= (float)V_I2(&v);
		break;
	case VT_I4:
		ret= (float)V_I4(&v);
		break;
	case VT_BSTR:
		tmps=(char*)_bstr_t(&v);
		ret=atof((LPCSTR)tmps);
		break;

	}
	return ret;
}
float vtof(COleVariant &v)
{
	float ret=0;
	CString tmps;
	switch(v.vt)
	{
	case VT_R4:
		ret = V_R4(&v);
		break;
	case VT_R8:
		ret=(float)V_R8(&v);
		break;
	case VT_I2:
		ret= (float)V_I2(&v);
		break;
	case VT_I4:
		ret= (float)V_I4(&v);
		break;
	case VT_BSTR:
		tmps=V_BSTRT(&v);
		ret=atof((LPCSTR)tmps);
		break;

	}
	return ret;
}

double vtod(_variant_t &v)
{
	double ret=0;
	CString tmps;
	switch(v.vt)
	{
	case VT_R4:
		ret = V_R4(&v);
		break;
	case VT_R8:
		ret=(double)V_R8(&v);
		break;
	case VT_I2:
		ret= (double)V_I2(&v);
		break;
	case VT_I4:
		ret= (double)V_I4(&v);
		break;
	case VT_BSTR:
		tmps=(char*)_bstr_t(&v);
		ret=atof((LPCSTR)tmps);
		break;

	}
	return ret;
}
double vtod(COleVariant &v)
{
	double ret=0;
	CString tmps;
	switch(v.vt)
	{
	case VT_R4:
		ret = V_R4(&v);
		break;
	case VT_R8:
		ret=(double)V_R8(&v);
		break;
	case VT_I2:
		ret= (double)V_I2(&v);
		break;
	case VT_I4:
		ret= (double)V_I4(&v);
		break;
	case VT_BSTR:
		tmps=V_BSTRT(&v);
		ret=atof((LPCSTR)tmps);
		break;

	}
	return ret;
}

void RsDeleteAll(_RecordsetPtr rs)
{
	try{
		if(rs->adoEOF)// || !rs->CanUpdate())
			return ;
		rs->MoveFirst();
		while(!rs->adoEOF)
		{
			rs->Delete(adAffectCurrent);
			rs->MoveFirst();
		}
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}

}
bool  vtob(	VARIANT &v)
{
	switch(v.vt)
	{
	case VT_BOOL:
		return (bool)V_BOOL(&v);
	case VT_I2:
		return (bool)V_I2(&v);
	case VT_I4:
		return (bool)V_I4(&v);
	case VT_R4:
		return (bool)V_R4(&v);
	case VT_R8:
		return (bool)V_R8(&v);
	}
	return false;
}
CString vtos(_variant_t& v)
{
	CString ret;
	switch(v.vt)
	{
	case VT_NULL:
	case VT_EMPTY:
		ret="";
		break;
	case VT_BSTR:
		ret= v.bstrVal;
		break;
	case VT_R4:
		ret.Format("%g", (double)V_R4(&v));
		break;
	case VT_R8:
		ret.Format("%g",V_R8(&v));
		break;
	case VT_I2:
		ret.Format("%d",(int)V_I2(&v));
		break;
	case VT_I4:
		ret.Format("%d",(int)V_I4(&v));
		break;
	case VT_BOOL:
		ret=(V_BOOL(&v) ? "True" : "False");
		break;
	}
	ret.TrimLeft();ret.TrimRight();
	return ret;
}
CString vtos(COleVariant& v)
{
	CString ret;
	switch(v.vt)
	{
	case VT_NULL:
	case VT_EMPTY:
		ret="";
		break;
	case VT_BSTR:
		ret=V_BSTRT(&v);
		break;
	case VT_R4:
		ret.Format("%g", (double)V_R4(&v));
		break;
	case VT_R8:
		ret.Format("%g",V_R8(&v));
		break;
	case VT_I2:
		ret.Format("%d",(int)V_I2(&v));
		break;
	case VT_I4:
		ret.Format("%d",(int)V_I4(&v));
		break;
	case VT_BOOL:
		ret=(V_BOOL(&v) ? "True" : "False");
		break;
	}
	ret.TrimLeft();ret.TrimRight();
	return ret;
}

CString vtos2(_variant_t& v)
{
	char ch[14];
	switch(v.vt)
	{
	case VT_NULL:
	case VT_EMPTY:
		{
			return "";
		}
	case VT_BSTR:
		{
			return V_BSTRT(&v);	
		}
	case VT_R4:
		{
			sprintf(ch, "%g", (double)V_R4(&v));
			return ch;
		}
	case VT_R8:
		{
			sprintf(ch, "%g",V_R8(&v));
			return ch;
		}
	case VT_I2:
		{
			sprintf(ch, "%d",(int)V_I2(&v));
			return ch;
		}
	case VT_I4:
		{
			sprintf(ch, "%d",(int)V_I4(&v));
			return ch;
		}
	case VT_BOOL:
		{
			strcpy(ch, (V_BOOL(&v) ? "True" : "False"));
			return ch;
		}
	}
}


float Sgn(float x)
{
	if(x>0)
		return 1;
	else if(x<0)
		return -1;
	return 0;
}
_variant_t stov(LPCTSTR svar)
{
	CString vs=svar;
	vs.TrimLeft();vs.TrimRight();
	_variant_t vtmp;
	vtmp.ChangeType(VT_NULL);
	if(vs.GetLength()>0)
		vtmp.SetString(vs);
	return vtmp;
}
int inline Fix(double dv)
{
	return (dv>=0 ? (int)dv : (int)dv+1);
}
CString inline ftos(float v)
{
	CString s;
	s.Format("%g",v);
	return s;
}
CString inline ftos1(float v)
{
	CString s;
	long lv=(long)(v+0.5);
	s.Format("%d",lv);
	return s;
}
CString inline ltos(long v)
{
	CString s;
	s.Format("%d",v);
	return s;
}
bool GetAllMenu(HMENU hMenu,long pos)
{
	return true;
}
bool DisabledSystemMenu(HWND hWnd,HMENU hMenu)
{
	return true;
}
_variant_t inline STR_VAR( LPCTSTR X )
{
	_variant_t nul;
	nul.ChangeType(VT_NULL);
	if(CString(X)=="")
		return nul;
	return COleVariant(X,VT_BSTRT);
}
CString inline btos(BOOL v)
{
	return v ? "True" : "False";
}
int inline ShowMessage(LPCTSTR lpszText, UINT nType, LPCTSTR lpszTitle,UINT nIDHelp)
{
	return ::MessageBox(NULL,lpszText,lpszTitle,nType|MB_TASKMODAL|MB_TOPMOST );
}
_variant_t inline GetFields(_RecordsetPtr rs,CString FieldName)
{
	_variant_t vTmp;
	rs->get_Collect((_variant_t)FieldName,&vTmp);
	return vTmp;
}
void SetWindowCenter(HWND hWnd)
{
	long SW,SH;
	if(hWnd==NULL)
		return;
	SW=::GetSystemMetrics(SM_CXSCREEN);
	SH=::GetSystemMetrics(SM_CYSCREEN);
	CRect rc;
	::GetWindowRect(hWnd,&rc);
	long x=(SW-rc.Width()) / 2;
	long y=(SH-rc.Height()) / 2;
	::SetWindowPos(hWnd,NULL,x,y,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
}

void inline DoEvents()
{
	MSG msg;
	if(::PeekMessage(&msg,NULL,0,0,PM_REMOVE))
	{
		if(msg.message==WM_ACTIVATEAPP || msg.message==WM_PAINT)
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
}
BOOL MakePath(CString strDirName)
{
	strDirName.TrimLeft();
	strDirName.TrimRight();
	if(strDirName.Right(1)!="\\")
		strDirName+="\\";
	CString sTmpDir;
	int i,iPos=0;
	while((i=strDirName.Find( "\\",iPos))!=-1)
	{
		sTmpDir=strDirName.Left(i);
		iPos=i+1;
		if(sTmpDir=="")
			break;
		DWORD code=GetFileAttributes(sTmpDir);
		if((code==0xFFFFFFFF) || (( code & FILE_ATTRIBUTE_DIRECTORY) ==0))
			if(!CreateDirectory(sTmpDir,NULL))
				return 0;
	}
	return 1;
}
void ModifyControlStyle(HWND &hWnd,DWORD dwRemove,DWORD dwAdd)
{
	RECT rc;
	HWND parWnd=NULL;
	if(!IsWindow(hWnd) || dwRemove==dwAdd)
		return;
	CWnd* pWnd=CWnd::FromHandle(hWnd);
	UINT nID=::GetWindowLong(hWnd,GWL_ID);
	parWnd=::GetParent(hWnd);
	::GetWindowRect(hWnd,&rc);
	RECT rc2;

	DWORD style=::GetWindowLong(hWnd,GWL_STYLE);
	DWORD styleEx=::GetWindowLong(hWnd,GWL_EXSTYLE);
	DWORD tmpstyle=style;
	style &= ~dwRemove;
	style |= dwAdd;
	if(tmpstyle==style)
		return;
	char wndClassName[256];
	::GetClassName(hWnd,wndClassName,256);
	CString strTmp=wndClassName;
	strTmp.MakeUpper();
	int iTmp;
	if(strTmp=="COMBOBOX")
	{
		//iTmp=((CComboBox*)pWnd)->GetDroppedWidth();
		((CComboBox*)pWnd)->GetDroppedControlRect(&rc2);
		rc.bottom=rc.top+(rc2.bottom-rc2.top)+4;
		style|=WS_VSCROLL;
	}
	else if(strTmp=="LISTBOX")
	{
		style|=WS_VSCROLL;
	}
	if(parWnd!=NULL)
		CWnd::FromHandle(parWnd)->ScreenToClient(&rc);
	CFont *pFnt=pWnd->GetFont();
	DestroyWindow(hWnd);
	hWnd=NULL;
	hWnd=::CreateWindowEx(styleEx, wndClassName,NULL,style,rc.left,rc.top,rc.right-rc.left,
		rc.bottom-rc.top,parWnd,(HMENU)nID,::AfxGetInstanceHandle(),NULL);
	pWnd=CWnd::FromHandle(hWnd);
	pWnd->SetFont(pFnt);
	/*if(strTmp=="COMBOBOX")
	{
		((CComboBox*)pWnd)->SetDroppedWidth(iTmp);
	}*/
}

//得到PSA所用单位数据库的路径
CString GetShareDbPath()
{
	HKEY hKey;
	char sv[256];
	unsigned long vlen = 256;
	unsigned long nType = REG_SZ;
	CString strDbPath = _T("");
	CString strSubKey = "SOFTWARE\\长沙优易软件开发有限公司\\shareMDB\\";
	if(RegOpenKey(HKEY_LOCAL_MACHINE,strSubKey,&hKey)!=ERROR_SUCCESS)
	{
		return strDbPath;
	}

	if(RegQueryValueEx(hKey,_T("shareMDB"),NULL,&nType,(unsigned char*)sv,&vlen)!=ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return strDbPath;
	}

	strDbPath.Format("%s",sv);
	strDbPath.TrimLeft();
	strDbPath.TrimRight();

	if( strDbPath.Right(1) != "\\" )
		strDbPath += "\\";

	RegCloseKey(hKey);

	return strDbPath;
}

//返回字符串中子串，(格式：动态库AutoPSA 生成文件中的格式);
CString GetStrAtEx(CString strText,int i)
{
	int start=0,end=0;
	CString str;
	str.Empty(); 
	strText.TrimLeft();
	strText.TrimRight();
	strText = strText+",";
	for(int index=1;index<=i-1;index++)
	{
		start=strText.Find(',',start+1);
	}
	end=strText.Find(',',start+1);
	if(end !=-1)
	{
		if(start != 0)
			start++;
		str=strText.Mid(start,end-start);
	}
	str.TrimLeft();
	str.TrimRight();
	return str;
}

//返回字符串中子串，(以空格隔开的字符串);
int GetStrAtCountEx(CString strText)
{
	int start=0,end=0;
	CString str;
	str.Empty();
	strText.TrimLeft();
	strText.TrimRight();
	strText = strText+",";
	int i=0;
	while(!strText.IsEmpty())
	{
		i++;
		start=strText.Find(',',0);
		if(start == -1)
			break;
		else 
			strText = strText.Mid(start+1);
	//	strText.TrimLeft();
		
	}
	return i;
}
