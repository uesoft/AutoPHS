// ModEncrypt.cpp
//

#include "stdafx.h"
#include "ModEncrypt.h"
#include "Qsort.h"
#include "modRegistry.h"
#include "user.h"
#include "Netdog.h"
#define RUN_NUM  400
#define RUN_DAYS_NUM 300
#define INSTALL_NUM 50
#include <afxconv.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
enum enumInstallType
{
	iDogMH,
	iDogNHClient,
	iDogNHServer
};
//网络狗数据
short int NetDogAddr,NetDogBytes;
unsigned long NetDogPassword=5602521;
unsigned long NetDogResult;
void FAR * NetDogData=NULL;
unsigned long FAR * NetDogHandle=NULL;

unsigned long Doghandle;


const char* ModEncrypt::FilejmBASE0 =  "XYZ.Dab";
const char* ModEncrypt::FilejmBASE1 =  "LEEGB.MPX";
const char* ModEncrypt::FilejmCompanyName0 =  "XYZ.Dac";
const char* ModEncrypt::FilejmCompanyName1 =  "LEEGB.ocx";
const char* ModEncrypt::FilejmCombinedXYZ =  "XYZ.daa";
const char* ModEncrypt::FilePsw =  "LEEGB.asd";
const char* ModEncrypt::FileIns =  "LEEGB.ins";
const char* ModEncrypt::FileOutDate =  "LEEGB.OUT";
const char* ModEncrypt::FileFlag =  "LEEGB.LEG";
const char* ModEncrypt::FileWinCom =  "win.com";
const char* ModEncrypt::FileWinDir =  "c:\\windir.txt";
bool ModEncrypt::gbLegalUser = bool();
bool ModEncrypt::gbLegalStandardUser = bool();
bool ModEncrypt::gbLegalLearningUser = bool();
bool ModEncrypt::gbLegalDemoUser = bool();
CString ModEncrypt::gstrLegalCompanyName = CString();
long ModEncrypt::glngUseDemoVersionDays =  0;
long ModEncrypt::glngLimitedDemoVersionDays = 0;
DWORD ModEncrypt::gLngVersionID=0;

DWORD glngVersion=0;
//const DWORD glngAppVersion=0x0410;//PFG20050609原代码	//人为指定的版本号 4.1 ，以便和加密文件中的版本号作比较，防止不同版本之间的密码文件通用。
const DWORD glngAppVersion=0x0700;//pfg20050609
long ModEncrypt::glngMicroDogID = 0;
long ModEncrypt::gLngSavedMicroDogID =0;

// 从测试版本来看，不需要密码
//CString ModEncrypt::gstrDBZdjCrudePassWord="ProductDB888";
CString ModEncrypt::gstrDBZdjCrudePassWord="";
CString ModEncrypt::gstrDBProductPassWord="";
const char* ModEncrypt::conStrDBPasswordSuffix =  "DB888";

bool bIsNetDog=false;

bool ModEncrypt::CombineEncryptedFile()
{
	return true;
}

bool ModEncrypt::SplitEncryptedFile()
{
	return true;
}

void ModEncrypt::MakeEncrypt()
{
}

long ModEncrypt::MakeEncryptFile(CString /*ByVal*/ p, CString /*ByVal*/ CompanyName)
{
    return (long)0;
}

void ModEncrypt::EncryptCompanyName(LPCSTR /*ByVal*/ strPassWord, bool /*ByVal*/ bGetPassWord)
{
//目的：生成或获取单位名称
   //Dim EncryptedMessage As String, TextChar As String, KeyChar As String, EncryptedChar
   //Dim f, i As Long, lngStrLen As Long
	USES_CONVERSION;
	try
	{
		DWORD i;
		CFile f;
		//CString EncryptedMessage;
		wchar_t EncryptedMessage[256];
		char TextChar[2];
		wchar_t EncryptedChar[2];
		EncryptedChar[1]=0;
		memset(EncryptedMessage,0,sizeof(EncryptedMessage));
		DWORD C;
		if( bGetPassWord )
		{
			//如果是获取公司名称
			if (FileExists(user::gstrWinDir + FilejmCompanyName1) )
			{
				f.Open(user::gstrWinDir + FilejmCompanyName1,CFile::modeRead);
				
				f.Read(&C,sizeof(DWORD));
				f.Read(EncryptedMessage,C*sizeof(wchar_t));
				gstrLegalCompanyName = "";
				for( i = 0 ;i<C;i++)
				{
					TextChar[0]=((char*)EncryptedMessage)[i*2+1];
					TextChar[1]=((char*)EncryptedMessage)[i*2];
					short xx=(*(wchar_t*)TextChar);
					short xx2=strPassWord[i%strlen(strPassWord)];
					EncryptedChar[0] =xx ^ xx2;
					((char*)EncryptedMessage)[i*2+1]=((char*)&EncryptedChar)[0];
					((char*)EncryptedMessage)[i*2]=((char*)&EncryptedChar)[1];
					//xx=EncryptedChar;
					EncryptedChar[0]=EncryptedMessage[i];
					gstrLegalCompanyName+=(char*)EncryptedChar;
				}
				f.Close();
			}

			else
				ShowMessage(GetResStr(IDS_ItsDemoVersionOrIllegalUser));
		}
		else
		{
			if(!f.Open(user::gstrWinDir + FilejmCompanyName0,/*CFile::modeNoTruncate|*/CFile::modeCreate|CFile::modeReadWrite))
				return;
			DWORD i1=0,iC=0;
			wchar_t c2;
			wchar_t c3;
			c3=0;
			char* pComName=T2A((LPTSTR)(LPCTSTR)gstrLegalCompanyName);
			for(i=0;i<gstrLegalCompanyName.GetLength();i++)
			{
				c2=0;
				char c1=pComName[i];
				if((c1 & 0x80)!=0)
				{
					((char*)&c2)[0]=pComName[i];
					i++;
					((char*)&c2)[1]=pComName[i];
				}
				else
				{
					((char*)&c2)[0]=pComName[i];
				}
				((char*)&c3)[1]=strPassWord[i1%strlen(strPassWord)];
				EncryptedMessage[i1]=c2 ^ c3;
				i1++;
				iC++;
			}
			f.Write(&iC,sizeof(DWORD));
			f.Write(EncryptedMessage,iC*sizeof(wchar_t));
			f.Close();
		}
	}
	catch(CFileException * e)
	{
		e->ReportError();
		e->Delete();
	}
}

void ModEncrypt::VerifyEncryptFile(/*CString& strPassWord*/)
{
	char *szPassWord=NULL;
	char *MP=NULL;
	char *EP=NULL;
	char *EP1=NULL;
	try
	{
		long i=0,iPos=0;
		char tmp;
		// a$
		// bTmp  String * 1
		CFile f;
		DWORD lngEP,lngMP;
		if(!f.Open(user::gstrWinDir+FilejmBASE1,CFile::modeRead))
			return;
		//开始字符串长度
		f.SeekToBegin();
		f.Read(&lngMP,sizeof(lngMP));
		MP=new char[lngMP+1];
		f.Read(MP,lngMP);
		MP[lngMP]='\0';
		//然后是密码长度
		f.Read(& lngEP,sizeof(lngEP));
		f.Read(& glngLimitedDemoVersionDays,sizeof(DWORD));
		f.Read(& gLngSavedMicroDogID,sizeof(DWORD));
		f.Read(& gLngVersionID,sizeof(DWORD));
		f.Read(& glngVersion,sizeof(glngVersion));
		EP=new char[lngEP+1];
		f.Read(EP,lngEP);
		EP[lngEP]='\0';
		f.Close();
		
		//if(strPassWord=="")
		{
			if(!f.Open(user::gstrWinDir + FilePsw,CFile::modeRead))
				goto FunOut;
			int len=f.GetLength();
			len-=16;
			f.Seek(16,CFile::begin);
			szPassWord=new char[len+1];
			f.Read(szPassWord,len);
			szPassWord[len]='\0';
			f.Close();
		}
		//MP.ReleaseBuffer();
		//EP.ReleaseBuffer();
		//strPassWord.ReleaseBuffer();
		int passlen=strlen(szPassWord);
		EP1=new char [passlen+1];
		for( i = 0 ; i<passlen;i++)
		{
			tmp=szPassWord[i];
			
			if( '0' <= tmp && tmp <='9' ){
				iPos = tmp - '0' + 2;
			}else if( 'A' <= tmp && tmp <= 'Z' ){
				iPos = tmp - 'A' + 12;
			}
			else if('a' <= tmp && tmp <= 'z' ){
				iPos = tmp - 'a' + 12;
			}
			EP1[i]=(char)MP[iPos-1];
			//MsgBox EP1
		}
		EP1[i]='\0';
		if( strcmp(EP1, EP)==0 )
		{
			EncryptCompanyName(szPassWord, true);
			//EncryptCompanyName(strPassWord, false);
			/*#if DebugMode = -1 ){
			//MsgBox "你是合法用户!"
      #endif*/
			if( gLngVersionID == 1236 && glngVersion == glngAppVersion ){
				//标准版标志
				gbLegalUser = true;
			}else{
				gbLegalUser = false;
			}
			if( FileExists(user::gstrWinDir + FileOutDate) ) 
				::DeleteFile( user::gstrWinDir + FileOutDate);
			if( FileExists(user::gstrWinDir + FileFlag) )
				::DeleteFile( user::gstrWinDir + FileFlag);
			if(!f.Open(user::gstrWinDir+FileFlag,CFile::modeWrite|CFile::modeCreate))
				goto FunOut;
			lngEP = 1234;
			f.Write(&lngEP,sizeof(DWORD));
			f.Close();
			SetFileTime(user::gstrWinDir + FileWinCom, user::gstrWinDir + FileFlag);
		}
		else
		{
			//MsgBox "你是非法用户!"
			gstrLegalCompanyName = GetResStr(IDS_YouAreIllegalUser);
			gLngVersionID=0;
			gbLegalUser = false;
		}
	}
	catch(CFileException * e)
	{
		e->ReportError();
		e->Delete();
	}
FunOut:
	delete [] szPassWord;
	delete [] MP;
	delete [] EP;
	delete [] EP1;
}

void ModEncrypt::VerifyID()
{
}

bool ModEncrypt::CheckLegalUser()
{
	//CString strPassWord;
   int i;
	DWORD dwReturnCode;
	CString strMsg;
	
	//检测网络狗
	if(UEGetSettingInstallNumber("4",iDogMH) ==iDogMH)
	{
		bIsNetDog=false;
	}
	else
		bIsNetDog=true;
	NetDogHandle = (unsigned long *)&Doghandle;
	//if(!gbLegalUser)
	{
		//运行次数计数
		i = UEGetSettingInstallNumber("2", 0);
		UESaveSettingInstallNumber("2", i + 1);
		gbLegalUser = false;
		//检查是否正版软件
		if (FileExists(user::gstrWinDir + FilejmBASE1) && FileExists(user::gstrWinDir + FilePsw) && FileExists(user::gstrWinDir + FileIns) )
		{
			//VerifyEncryptFile(strPassWord);
			VerifyEncryptFile();
			//gstrDBZdjCrudePassWord = strPassWord + conStrDBPasswordSuffix
//			gstrDBProductPassWord =CString( "Product") + conStrDBPasswordSuffix;
			gstrDBZdjCrudePassWord = gstrDBProductPassWord;
		}
	}
	if(bIsNetDog)
	{
		gbLegalUser=false;
		NetDogPassword=5602521;
		dwReturnCode = NetDogLogin();
		if(dwReturnCode!=DOGSUCCESS)
		{
			gbLegalUser=false;
			bIsNetDog=false;
			if(110003==dwReturnCode)
				strMsg.Format(IDS_USER_EXCESS_UPPER_LIMIT,dwReturnCode);
			else
				strMsg.Format(_T("NetDogLogin function error! code：%d"),dwReturnCode);
			ShowMessage(strMsg);
		}
		else
		{
			DWORD CurrentDogNo=GetNHDogNo();
			if(gLngVersionID==0 || CurrentDogNo== 0 || gLngSavedMicroDogID ==0 || gLngSavedMicroDogID != CurrentDogNo || glngVersion != glngAppVersion)
			{
				gbLegalUser=false;
				strMsg=GetResStr(IDS_DOG_NO_ERROR);
				ShowMessage(strMsg);
				dwReturnCode=NetDogLogout();
				if(dwReturnCode!=0)
				{
					strMsg.Format(_T("NetDogLogout function error! code：%d"),dwReturnCode);
					AfxMessageBox(strMsg);
				}
				NetDogHandle=NULL;
				bIsNetDog=false;
			}
			else
			{
				gbLegalUser=true;
				bIsNetDog=true;
			}
		}
	}
	else
	{
		//如果是演示版，检查是否时间到期
		if( ! gbLegalUser ){
			CheckInstallDate();
		}
	}
   return gbLegalUser;
}

void ModEncrypt::CheckInstallDate()
{

	COleDateTime InsDate, Today;
    long i , iNum;
	CFile f,f1;
	long st;
	CFileException e;
	try{
		//下面是安装日期记录文件
		f.Open(user::gstrWinDir + FileIns,CFile::modeRead);
		iNum=f.GetLength()/sizeof(time_t) - 1;
		if(Qsort.sortarray !=NULL)
		{
			delete [] Qsort.sortarray;
			Qsort.sortarray=NULL;
		}
		if(Qsort.sortindex!=NULL)
		{
			delete [] Qsort.sortindex;
			Qsort.sortindex=NULL;
		}
		Qsort.sortarray=new DATE [iNum];
		Qsort.sortindex=new int [iNum];
		f.Seek(sizeof(DATE),CFile::begin);
		for(i=0;i<iNum;i++)
		{
			f.Read(&InsDate,sizeof(DATE));
			//移动4个字节
			Qsort.sortarray[i] = InsDate;
			Qsort.sortindex[i] = i;
		}
		Qsort.QuickSort(0, iNum-1);
		//最早的安装日期
		//MsgBox Qsort.sortarray[1)
		//最晚的安装日期
		//MsgBox Qsort.sortarray[iNum)
		InsDate=COleDateTime::GetCurrentTime();
		f.Close();
		SetFileTime(user::gstrWinDir + FileWinCom, user::gstrWinDir + FileIns);
		DATE tmpD;
		f.Open(user::gstrWinDir + FileIns,CFile::modeWrite);
		f.Seek(8,CFile::begin);
		if( ! FileExists(user::gstrWinDir + FileOutDate) )
		{
			if( UEGetSettingInstallNumber("1", 0) >= INSTALL_NUM )
			{
				//如果安装次数超过
				glngUseDemoVersionDays = glngLimitedDemoVersionDays + 1;
			}
			else
			{
				//如果时间超过，则演示版功能受到限制
				if( glngLimitedDemoVersionDays > RUN_DAYS_NUM )
					glngLimitedDemoVersionDays = RUN_DAYS_NUM;
				else if( glngLimitedDemoVersionDays <= 0 )
					glngLimitedDemoVersionDays = RUN_DAYS_NUM;
				else
					glngLimitedDemoVersionDays = RUN_DAYS_NUM;
				
				COleDateTime Date=COleDateTime::GetCurrentTime();
				COleDateTime dateTmp = Qsort.sortarray[iNum-1];
				if(Date >= dateTmp )
				{
					dateTmp=Qsort.sortarray[0];
					if( Date >= dateTmp)
					{
						glngUseDemoVersionDays = Date - COleDateTime(Qsort.sortarray[1]);
					}
					else
					{
						glngUseDemoVersionDays = glngLimitedDemoVersionDays + 1;//因为用户自已调整了时间所以让演示版到期
						tmpD=(DATE)InsDate;
						f.Write(&tmpD,sizeof(DATE));
					}
				}
				else
				{
					glngUseDemoVersionDays = glngLimitedDemoVersionDays + 1;//因为用户自已调整了时间所以让演示版到期
					tmpD=(DATE)InsDate;
					f.Write(&tmpD,sizeof(DATE));
				}
			}
			
			if( glngUseDemoVersionDays >= glngLimitedDemoVersionDays )
			{
				f1.Open(user::gstrWinDir + FileOutDate,CFile::modeCreate|CFile::modeWrite|CFile::modeNoTruncate);
				Today=COleDateTime::GetCurrentTime();
				tmpD=(DATE)Today;
				f1.Write(&tmpD,sizeof(DATE));
				f1.Close();
				tmpD=0;
				st=f.GetLength();
				if(st>=16)
				{	
					st=8;
					f.Seek(st,CFile::begin);
					f.Write(&tmpD,sizeof(DATE));
				}
				SetFileTime(user::gstrWinDir + FileWinCom, user::gstrWinDir + FileOutDate);
				gbLegalUser = FALSE;
				if( FileExists(user::gstrWinDir + FileFlag) )
					DeleteFile(user::gstrWinDir + FileFlag);
			}
			else
			{
				if( UEGetSettingInstallNumber( "2", 0) >= RUN_NUM )
				{
					//如果运行次数超过
					SetFileTime(user::gstrWinDir + FileWinCom, user::gstrWinDir + FileOutDate);
					gbLegalUser = FALSE;
					if( FileExists(user::gstrWinDir + FileFlag) )
						DeleteFile( user::gstrWinDir + FileFlag);
				}	
				else 
				{
					if( UEGetSettingInstallNumber( "2", 0) < 0 )
					{
						UESaveSettingInstallNumber( "2", RUN_NUM);
						gbLegalUser = FALSE;
					}
					else
					{
						CopyFile( user::gstrWinDir + FileWinCom, user::gstrWinDir + FileFlag,TRUE);
						SetFileTime(user::gstrWinDir + FileWinCom, user::gstrWinDir + FileFlag);
						gbLegalUser = TRUE;
					}
				}
			}
		}
		else
		{
			gbLegalUser = FALSE;
			if( FileExists(user::gstrWinDir + FileFlag) ) 
				DeleteFile( user::gstrWinDir + FileFlag);
		}
		f.Close();
	}
	catch(CFileException *e)
	{
		e->ReportError();
		e->Delete();
	}
}



void ModEncrypt::EncryptLogout()
{
	DWORD dwReturnCode;
	CString strMsg;
	if(bIsNetDog)
	{
		//是网络狗
		if(NetDogHandle!=NULL)
		{
			dwReturnCode=NetDogLogout();
			if(dwReturnCode!=0)
			{
				strMsg.Format(_T("NetDogLogout function error! code：%d"),dwReturnCode);
				AfxMessageBox(strMsg);
			}
			NetDogHandle=NULL;
		}
	}
}

DWORD ModEncrypt::GetNHDogNo()
{
	DWORD retCode;
	DWORD CurrentNO=0;
	DWORD dwReturnCode;
	if(NetDogHandle)
	{
		NetDogAddr=0;
		NetDogBytes=4;
		NetDogData=&CurrentNO;
		dwReturnCode=NetDogRead();
		if(dwReturnCode!=0)
		{
			CString strMsg;
			strMsg.Format(_T("NetDogRead function error! code：%d"),dwReturnCode);
			AfxMessageBox(strMsg);
		}
	}
	return CurrentNO;
}

DWORD ModEncrypt::GetSaveDogNO()
{
	/*DWORD CurrentNO=0;
	DWORD n;
	CFile fB1;
	if(::FileExists(gstrWinDir + FilejmBASE1))
	{
		fB1.Open(gstrWinDir + FilejmBASE1,CFile::modeRead);
		fB1.SeekToBegin();
		fB1.Read(&n,sizeof(n));
		fB1.Seek(n*sizeof(n) + sizeof(DWORD)*2,CFile::current);
*/
	return 0;
}
