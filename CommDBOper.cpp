// CommDBOper.cpp: implementation of the CCommDBOper class.
//
//////////////////////////////////////////////////////////////////////



#include "stdafx.h"
#include "CommDBOper.h"
#include "autophs.h"
#include "dlgeditdb.h"
#include "InputBox.h"
#include "Shlwapi.h"
#pragma comment( lib, "Shlwapi" )



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#define RET_OK    0   //函数执行正确返回码
#define RET_FAILED -1 //函灵执行错误返回通用码

extern const _TCHAR dbConnectionString[];


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCommDBOper::CCommDBOper()
{

}

CCommDBOper::~CCommDBOper()
{

}

void CCommDBOper::WarnMessage(CString strWarn,int iWarn) 
{
	if (iWarn != 0) 
	{
		AfxMessageBox(strWarn);
	}
}


//打开指定数据库对象连接
int CCommDBOper::ADOOpenDBCon(_ConnectionPtr &pCon,const CString strDBPathName,
							  const CString strDBFileName,CString &strPassword,int iWarn)
{
	//函数功能   打开指定数据库
	//输入参数说明   pCon 要打开的数据库对象,strDBPathName 要连接的ACCESS数据库文件目录
	//				strDBFileName 要打开的数据库文件名,strPassword  密码
	//				iWarn 出错警告方式  0,不警告,1,警告,2,警告且退出程序
	//作者  LFX
	CString tstrDBPathName;
	tstrDBPathName = strDBPathName;
	
	if (!strDBFileName.IsEmpty()) 
	{
		//目录是否合法
		int iret1;
		iret1 = tstrDBPathName.FindOneOf("*?\"<>|");
		if (iret1 != -1)
		{
			CString strTemp;
			strTemp.Format("数据库(%s)的目录(%s)非法!!!",strDBFileName,strDBPathName);
			WarnMessage(strTemp,iWarn);
			return RET_FAILED;
		}
		
		iret1 = tstrDBPathName.ReverseFind('\\');
		if (iret1  != 1)
		{
			tstrDBPathName += "\\";
		}
	}

	CString strDB;
	strDB.Empty();
	strDB += tstrDBPathName;
	strDB += strDBFileName;
	return ADOOpenDBCon(pCon,strDB,strPassword,iWarn);
}
int CCommDBOper::ADOOpenDBCon(_ConnectionPtr &pCon,const CString strDB,CString &strPassword,int iWarn)
{
	//函数功能   打开指定数据库
	//输入参数说明   pCon 要打开的数据库对象,strDB 要连接的ACCESS数据库文件全路径
	//	strPassword  密码 iWarn 出错警告方式  0,不警告,1,警告,2,警告且退出程序
	//作者  LFX
	CString strCon;
	_TCHAR dbConnectionString[]=_T("Provider=Microsoft.Jet.OLEDB.4.0;Persist Security Info=False;Data Source=%s;Jet OLEDB:Database Password=%s");
	strCon.Format(dbConnectionString,strDB,strPassword);
	if(!PathFileExists(_T(strDB)))
	{
		WarnMessage("要打开的数据库文件名不存在!!!",iWarn);
		return RET_FAILED;
	}

	if (pCon == NULL)
	{
		pCon.CreateInstance(__uuidof(Connection) );
	}
	else
	{
		ADOCloseConnectDB(pCon,0);
	}
	int iLoop = 0;
	while (true)
	{
		//连接ACCESS数据库
		iLoop++;
		//如果没密码，且为第一次循环
		if ( strPassword.IsEmpty() && (iLoop <= 1))   
		{
			try
			{
				pCon->Open(_bstr_t(strCon),"","",-1);         
			}
			catch(_com_error &e)
			{
				CString strTemp;
				CInputBox inputBox;
				inputBox.m_bIsProtected = TRUE;
				strTemp.LoadString(IDS_NEEDPASSWORD);
				inputBox.m_strWndText.Format("%s%s",strDB,strTemp);
				inputBox.m_strTitle.LoadString(IDS_PLEASEINPUTDBPASSWORD);
				if (inputBox.DoModal() != IDOK)
				{
					WarnMessage("密码错误,打开数据库失败!!!",iWarn);	
					return RET_FAILED;
				}
				else
				{
					strPassword = inputBox.m_strValue;
				}
			}
			break;
		}
		//如果有密码
		try
		{
			pCon->Open(_bstr_t(strCon),"","",-1); 
		}
		catch (_com_error &e)
		{
			CString strMsg;
			strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
			AfxMessageBox(strMsg);
			CString errormessage;
			errormessage.Format("打开数据库 %s 失败!\r\n错误信息:%s",strDB,e.ErrorMessage());
			WarnMessage(errormessage,iWarn);
			return RET_FAILED;			
		}
		break;
		
	} //END while(true)

	return RET_OK;
}


int CCommDBOper::ADOConExec(_ConnectionPtr &pCon,const CString strCmdSql,CString strDcrCmd,int iWarn)
{	
	//函数功能说明: 由pCon(Connection)对象执行指定SQL命令
	//pCon 数据库连接对象
	//strCmdSql  要执行SQL的命令语句
	//strDcrCmd是对要使用SQL命令的描述
	if (ADOConIsOpened(pCon,iWarn) != RET_OK)   //判断当前连接对象是否已打开
	{
		WarnMessage("指定数据库对象未连接,退出!",iWarn);
		return RET_FAILED;
	}
	try
	{
		pCon->Execute(_bstr_t(strCmdSql),NULL,adCmdText);
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
		CString errormessage;
		errormessage.Format("功能说明: %s\r\n数据库连接对象执行下列SQL命令\r\n%s\r\n失败!  错误信息:%s",strDcrCmd,strCmdSql,e.ErrorMessage());
		WarnMessage(errormessage,iWarn);///显示错误信息
		return RET_FAILED;
	}
	return RET_OK;
}

int CCommDBOper::ADORecordsetOpen(_ConnectionPtr &pCon,_RecordsetPtr pRs,CString strOpenSql,CString strDcrOpen,int iWarn)
{
	//函数说明:执行指定记录集对象指定SQL操作
	//pCon 数据库连接对象
	//pRs  当前操作用记录集对象
	//strOpenSql  要执行的SQL命令
	//strDcrOpen是对要使用SQL命令的描述

	_variant_t tempStrSql;
	tempStrSql = strOpenSql;
	if (pRs == NULL)
	{
		pRs.CreateInstance(__uuidof(Recordset));
		pRs->CursorLocation = adUseClient;
	}

	ADOCloseRecordset(pRs,0);
	
	while (true)
	{
		try
		{
			//有错 pRs->Open(tempStrSql,_variant_t(pCon),adOpenDynamic,adLockOptimistic,adCmdText);
			pRs->Open(tempStrSql,(IDispatch *)pCon,adOpenDynamic,adLockOptimistic,adCmdText);
			break;
		}
		catch (_com_error &e)
		{
			CString strMsg;
			strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
			AfxMessageBox(strMsg);
		}
		try
		{	
			//如果第一次打开失败,看能否以只读方式打开
			pRs->Open(tempStrSql,(IDispatch *)pCon,adOpenDynamic,adLockReadOnly,adCmdText);
			return 2;
		}
		catch (_com_error &e)
		{
			CString strMsg;
			strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
			AfxMessageBox(strMsg);
			CString errormessage;
			errormessage.Format("功能说明: %s\r\n记录集对象执行下列SQL命令\r\n%s\r\n失败!  错误信息:%s",strDcrOpen,strOpenSql,e.ErrorMessage());
			WarnMessage(errormessage,iWarn);//显示错误信息
			return RET_FAILED;
		}
	}
	return RET_OK;
	
}


//判断当前数据库连接对象是否打开
int CCommDBOper::ADOConIsOpened(_ConnectionPtr &pCon,int iWarn)
{
	//判断当前连接对象是否已打开
	if ((pCon == NULL) || pCon->State !=  adStateOpen)
	{
		//WarnMessage("连接数据库对象没打开!",iWarn);
		return RET_FAILED;
	}
	else
	{
		return RET_OK;
	}
}

//断开指定数据库连接对象连接
int CCommDBOper::ADOCloseConnectDB(_ConnectionPtr &pCon,int iWarn)
{
	if ((pCon != NULL) && (pCon->State == adStateOpen) )
	{
		try
		{
			pCon->Close();
		}
		catch (_com_error &e)
		{
			CString strMsg;
			strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
			AfxMessageBox(strMsg);
		}
	}
	return RET_OK;
}

//关闭指定记录集对象
int CCommDBOper::ADOCloseRecordset(_RecordsetPtr pRs,int iWarn)
{
	if ((pRs != NULL) && (pRs->State == adStateOpen))
	{
		try
		{
			pRs->Close();
		}
		catch (_com_error &e)
		{
			CString strMsg;
			strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
			AfxMessageBox(strMsg);
		}
	}
	return RET_OK;
}

//删除表

int CCommDBOper::ADODropTable(_ConnectionPtr &pCon,const CString strTableName,int iWarn)
{
	CString strSql;
	CString strDcr;
	int iret1;
	
	//测试表存不存在
	iret1 = ADOTestTable(pCon,strTableName,iWarn);
	if ( iret1 == RET_OK)   
	{
		//表存在
		strDcr.Format("删除表");
		strSql.Format("DROP TABLE %s",strTableName);
		iret1 = ADOConExec(pCon,strSql,strDcr,iWarn);
	}
	return iret1;
}

//测试表是否存在
int CCommDBOper::ADOTestTable(_ConnectionPtr &pCon,const CString strTableName,int iWarn)
{	
	_RecordsetPtr pRs;
	TESTHR(pRs.CreateInstance(__uuidof(Recordset)) );
	CString strSql;
	CString strDcr;
	int iret1;
	
	strDcr.Format("打开表");
	strSql.Format("SELECT * FROM %s",strTableName);
	
 	iret1 = ADORecordsetOpen(pCon,pRs,strSql,strDcr,iWarn);
	ADOCloseRecordset(pRs,iWarn);
	return iret1;
}


int CCommDBOper::ADODelRecord(_ConnectionPtr pCon,const CString strTableName,const CString strDelCond,int iWarn)
{
	//函数功能: 从表中删除指定条件的记录
	//delCond  指定删除条件
	int iret1;
	
	iret1 = ADOTestTable(pCon,strTableName,iWarn);
	if (iret1 == RET_OK)
	{
		
		CString strSql;
		CString strDcr;
		
		//如果条件字段为空
		if (strDelCond.IsEmpty())
		{
			strSql.Format("DELETE FROM %s",strTableName);
			strDcr.Format("删除表中所有记录");
		}
		else
		{
			CString strTempDelCond;
			
			strDcr.Format("删除表中指定条件记录");
			strTempDelCond.Format("WHERE ");
			strTempDelCond = strTempDelCond + strDelCond;
			strSql.Format("DELETE FROM %s %s",strTableName,strTempDelCond);
		}
		iret1 = ADOConExec(pCon,strSql,strDcr,iWarn);
	}
	return iret1;
}



#undef RET_OK
#undef RET_FAILED

