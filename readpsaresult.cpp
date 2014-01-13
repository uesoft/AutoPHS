#include "stdafx.h"
#include "user.h"
#include "ReadPSAResult.h"
#include <fstream.h>
#include <math.h>
#include "EDIBDB.h"
#include "phsdata.h"
#include "Frmtxsr.h"
#include "DlgFindAutoPSAFile.h"
#include "UnitsTransformDlg.h"


#include "frmstatus.h"
#include "modPHScal.h"
#include "EDIBgbl.h"
#include "basDirectory.h"
#include "InputBox.h"
#include "modRegistry.h"
#include "SelPSAProjectNameDlg.h"

#include "BestrowJSJDH.h"
#include "PSAPointElevation.h"
#include "Outfunction.h"

#include "CAESARIIToPHS.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
const _TCHAR* strSprNWEPDI;
const _TCHAR* strSprECEPDI;
const _TCHAR* strSprGB10182;
bool gbNewGLIFV31=false;

CString gstrUNIT_FORCE="N";//内部使用N单位
CString gstrUNIT_MOMENTAL="N.m";//内部使用N.m单位

CString YLJSVer="";
int IsCaesarError;//pfg20050627
#pragma warning (disable : 4244)
long GetFileNameId(CString SourceDataFileName,IDispatch *pConID);
void Delete1();
//void ElevationFileToList(CString FileName,ElevationList *pElevationList,int ICC);
//写支吊架数据到PHS(支吊架软件表格)中
//extern "C" __declspec(dllimport) int WritePHSDataToTable(_ConnectionPtr pRefInfoCon,_Recordset *rsResult,CString strDataFileName,long imaxZdjh,long iFileNameID,long iSelVlmID,double dbRatioOfPjg2Pgz);

typedef CList<CNodeUpElevation *,CNodeUpElevation *> ElevationList;


CString Mid(CString& strTmp,int nStart,int nCount)
{
	if(nStart>= strTmp.GetLength())
		return CString(_T(""));
	else
	{
		if(nCount==-1)
			return strTmp.Mid(nStart);
		else 
			return strTmp.Mid(nStart,nCount);
	}
}

//返回字符串中子串，(以空格隔开的字符串);
CString GetStrAt1(CString strText,int i)
{
	int start=0,end=0;
	CString str;
	str.Empty();
	strText.TrimLeft();
	strText.TrimRight();
	strText = strText+" ";
	for(int index=1;index<=i-1;index++)
	{
		start=strText.Find(' ',0);
		if(start != -1)
			strText = strText.Mid(start);
		else 
			return "";
		strText.TrimLeft();
	}
	end=strText.Find(' ',0);
	if(end !=-1)
	{
		str=strText.Left(end);
	}
	str.TrimLeft();
	str.TrimRight();
	return str;
}


struct FileJSDBH
	{
		int JSDBH;
        struct FileJSDBH *NextNode;
	};
//已提取的节点号存入链表中，当文件中的节点号有重复时，链表中存放的节点无重复；//作者： chengbailian
void SaveFileJSDBH(FileJSDBH **Headpos,FileJSDBH **Noncepos,FileJSDBH **Nextpos,int iCalNodeNumber)
{
	FileJSDBH *Movepos=NULL;
	Movepos=*Headpos;
	while(Movepos!=NULL)
	{
		if(Movepos->JSDBH==iCalNodeNumber)
				break;
		Movepos=Movepos->NextNode;
	}
     if(Movepos==NULL)
	 {
		*Nextpos=(FileJSDBH *)malloc(sizeof(FileJSDBH));
		(*Nextpos)->JSDBH=iCalNodeNumber;
		(*Nextpos)->NextNode=NULL;
		if(*Headpos==NULL)
			*Headpos=*Nextpos;
		else
		    (*Noncepos)->NextNode=*Nextpos;
			*Noncepos=*Nextpos;
	 }

}
////把要提取文件的节点号；//作者： chengbailian
void PickFileJSDBH(CString FileName,FileJSDBH **Headpos,FileJSDBH **Noncepos,FileJSDBH **Nextpos,CString YLJSVer)
{
	ifstream f;
	CString temp,strTemp;
	int iCalNodeNumber,fdFlg;
    
	f.open(FileName);//SourceDataFileName
	//根据文件类型不同，用不同方法提取文件中的节点号
	if(YLJSVer=="GLIF1.2")  ///GLIF应力分析程序为热态吊零计算弹簧
	{
		while(!f.eof())
		{
			 f.getline(strTemp.GetBuffer(255),255);
			 strTemp.ReleaseBuffer();
			 if( strTemp.Find("SUMMARY FOR RESTRAINTS") !=-1 ) //找到“SUMMARY FOR RESTRAINTS”
				 break;
		}
						
		while(!f.eof())
		{
			f.getline(temp.GetBuffer(255),255);
			temp.ReleaseBuffer();
			if( atoi(Mid(temp,4, 3)) > 0 )
			{
				iCalNodeNumber = atoi(Mid(temp,0, 7));
              
				//把节点号放入链表中，每一分支都要这样做
                   SaveFileJSDBH(Headpos,Noncepos,Nextpos,iCalNodeNumber);
                //
				f.getline(temp.GetBuffer(255),255);
			    temp.ReleaseBuffer();  //跳过1空行
			}				  
			else
				if(temp.Find("COLD SPRINGS TABLE") !=-1)
					break;
		}
		f.close();
	}
	else if(YLJSVer=="GLIF3.x"||YLJSVer.Left(7)=="AutoPSA")
	{
		//ReadResult_GLIF31New(rs1, DataFileName,maxZdjh,FileNameID1);
		//GLIF应力分析程序为热态吊零计算弹簧
		if(gbNewGLIFV31)
		{
			while(!f.eof())
			{
				f.getline(strTemp.GetBuffer(255),255); strTemp.ReleaseBuffer();
				if(strTemp.Find("CW-DISPLACEMENT (cold/work-status)") !=-1 )//找到"CW-DISPLACEMENT (cold/work-status)"
					break;
			}
			fdFlg=0;
			while(!f.eof())
			{
				f.getline(temp.GetBuffer(255),255); temp.ReleaseBuffer();
			    //湖南电力院1999年5月前使用的GLIFV3.x版本，iCalNodeNumber的字符宽度从第1-10位
			    //贵州电力院2001年5月前使用的GLIFV3.1版本，iCalNodeNumber的字符宽度从第1-5位，
				if( atof(Mid(temp, 0, 5)) >0 )
				{
					fdFlg=1;
					if( gbNewGLIFV31 )
						iCalNodeNumber = atoi(Mid(temp,0, 5)) ;     //节点号
					else
						iCalNodeNumber = atoi(Mid(temp, 0, 10));     //节点号

                      //把节点号放入链表中，每一分支都要这样做
				      SaveFileJSDBH(Headpos,Noncepos,Nextpos,iCalNodeNumber);
				}
				else
				{
					if( fdFlg==1)
					{
						do
						{
							if(temp.Find("CW-DISPLACEMENT (cold/work-status)") != -1 )
								break;
							f.getline(temp.GetBuffer(255),255); temp.ReleaseBuffer();
						}while(!f.eof());
						fdFlg=0;
					}
				}
			}
			f.close();
			f.open(FileName);
			//GLIFV3.x:查找荷载数据头
			while(!f.eof())
			{
				f.getline(strTemp.GetBuffer(255),255); strTemp.ReleaseBuffer();
				if( strTemp.Find("STRUCTURE LOAD OF RESTRAINTS") !=-1 ) //找到"STRUCTURE LOAD OF RESTRAINTS"
					break;
			}
                     
		//找到"STRUCTURE LOAD OF RESTRAINTS"
		//GLIFV3.x:开始读荷载数据
		fdFlg=0;
		while(!f.eof())
		{
			f.getline(temp.GetBuffer(255),255); temp.ReleaseBuffer();
			if( atof(Mid(temp,0, (gbNewGLIFV31 ? 5 : 16))) > 0 )
			{
				fdFlg=1;
				if( gbNewGLIFV31 )
					iCalNodeNumber = atoi(Mid(temp, 0, 5));      //节点号
				else
					iCalNodeNumber = atoi(Mid(temp, 0, 16)); //节点号
				SaveFileJSDBH(Headpos,Noncepos,Nextpos,iCalNodeNumber);
			}
			else
			{
				if( fdFlg==1)
				{
					do
					{
						if(temp.Find("STRUCTURE LOAD OF RESTRAINTS") != -1 )
							break;
						f.getline(temp.GetBuffer(255),255); temp.ReleaseBuffer();
					}while(!f.eof());
					fdFlg=0;
				}
			}
		}
		f.close();
		}else
		{

				//ReadResult_GLIF31(rs1, DataFileName,maxZdjh,FileNameID1);
			while(!f.eof())
			{
				f.getline(strTemp.GetBuffer(255),255); strTemp.ReleaseBuffer();
					if(strTemp.Find("CW-DISPLACEMENT (cold/work-status)") !=-1 ) //找到"CW-DISPLACEMENT (cold/work-status)"
						break;
			}
   
			//找到"CW-DISPLACEMENT (cold/work-status)"
			//GLIFV3.x:开始读位移数据
			while(!f.eof())
			{
				f.getline(temp.GetBuffer(255),255); temp.ReleaseBuffer();
				//湖南电力院1999年5月前使用的GLIFV3.x版本，iPreINT(1)的字符宽度从第1-10位
				//贵州电力院2001年5月前使用的GLIFV3.1版本，iPreINT(1)的字符宽度从第1-5位，
				//iPreINT(2)的字符宽度从第6-10位，
				if( atof(Mid(temp, 0, 10)) >0)
				{
					if( gbNewGLIFV31 )
						iCalNodeNumber = atoi(Mid(temp,0, 5)) ;     //节点号
					else
						iCalNodeNumber = atoi(Mid(temp, 0, 10));     //节点号
					SaveFileJSDBH(Headpos,Noncepos,Nextpos,iCalNodeNumber);
				}
				else
				{
					if( temp.Find("CW-DISPLACEMENT (cold/work-status)") ==-1 )
						break;
				}
			}
			f.close();
			f.open(FileName);
		      //GLIFV3.x:查找荷载数据头
			while(!f.eof())
			{
				f.getline(strTemp.GetBuffer(255),255); strTemp.ReleaseBuffer();
				if( strTemp.Find("STRUCTURE LOAD OF RESTRAINTS") !=-1 ) //找到"STRUCTURE LOAD OF RESTRAINTS"
					break;
			}
                     
		//找到"STRUCTURE LOAD OF RESTRAINTS"
		//GLIFV3.x:开始读荷载数
			while(!f.eof())
			{
				f.getline(temp.GetBuffer(255),255); temp.ReleaseBuffer();
			    if( atof(Mid(temp,0, (gbNewGLIFV31 ? 5 : 16))) > 0 )
				{
					if( gbNewGLIFV31 )
						iCalNodeNumber= atoi(Mid(temp, 0, 5));      //节点号
					else
						iCalNodeNumber = atoi(Mid(temp, 0, 16));  //节点号	
					SaveFileJSDBH(Headpos,Noncepos,Nextpos,iCalNodeNumber);
				}else
					if( temp.Find("STRUCTURE LOAD OF RESTRAINTS") ==-1 )
						break;
			}
			f.close();
		}
		}
		else if(YLJSVer=="ZHDYF3.0")
		{
			//ReadResult_ZHDYF30(rs1, DataFileName,maxZdjh,FileNameID1);
			while(!f.eof())
			{
				f.getline(temp.GetBuffer(255),255);
				temp.ReleaseBuffer();
				if(temp=="")		//跳过空行
					continue; 
				if( atoi(Mid(temp,0, 4)) > 0 )  
				{
					iCalNodeNumber = atoi(Mid(temp,0, 4));
			        SaveFileJSDBH(Headpos,Noncepos,Nextpos,iCalNodeNumber);
				}
			//下一行是管径、温度、标高等数据
				f.getline(temp.GetBuffer(255),255);
				temp.ReleaseBuffer();
			//下一行是约束类型、安装荷载、工作荷载、结构荷载等数据
				f.getline(temp.GetBuffer(255),255);
				temp.ReleaseBuffer();
		
			//接下来3行数据
			//第1行：冷位移(x,y,z),热位移(x,y,z),
				f.getline(temp.GetBuffer(255),255);
				temp.ReleaseBuffer();

			//第2行：冷态力(x,y,z),冷态力矩(x,y,z),热态力(x,y,z),热态力矩(x,y,z)
				f.getline(temp.GetBuffer(255),255);
				temp.ReleaseBuffer();
			
			//第3行：松冷力(x,y,z),松冷力矩(x,y,z),偶然力(x,y,z),偶然力矩(x,y,z)
				f.getline(temp.GetBuffer(255),255);
				temp.ReleaseBuffer();			
			}
			f.close();
		}
		else if(YLJSVer=="XNP 2.0")
		{
					//西南院应力分析程序为热态吊零计算弹簧
					//ReadResult_SWEDPSA(rs1, DataFileName,maxZdjh,FileNameID1);
			while(!f.eof())
			{       
					f.getline(temp.GetBuffer(255),255);
					temp.ReleaseBuffer();
					if(temp.Find("JIE GOU SUPPORT LOAD")!=-1 || temp.Find("WORKING SUPPORT LOAD")!=-1)
					{
						f.getline(temp.GetBuffer(255),255);
						temp.ReleaseBuffer();
                
						f.getline(temp.GetBuffer(255),255);
						temp.ReleaseBuffer();

						do
						{
							temp.TrimLeft();temp.TrimRight();
							PhsInfo tmpPhsInfo;
							iCalNodeNumber = GetParam(temp)*10.0f;
							SaveFileJSDBH(Headpos,Noncepos,Nextpos,iCalNodeNumber);
							f.getline(temp.GetBuffer(255),255);
							temp.ReleaseBuffer();
							temp.TrimLeft();

						}while(temp.GetLength() >0 && ((_T('0') <= temp[0] && temp[0] <= _T('9')) || temp[0]==_T('.')) &&!f.eof());

					}
			}
			f.close();
		}
}


BestrowJSJDH JSDBH_Dlg;
//得到最有可能被覆盖的文件名   //add by chengbailian
CString GetOldFileName(CString NewFileName,IDispatch* pCon,long SelVlmID,CString YLJSVer)
{
    int JSDBHNumer,JSDBHNumer1,NewJSDBHNumer=0,OldJSDBHNumer;
	_RecordsetPtr rs,ZaRs,FileRs,ZaRsCount;
	CString Sql,CSql;
	CString EndFileName=NewFileName;
    FileJSDBH *NewFileHeadpos,*NewFileNoncepos,*NewFileNextpos,*NewFileMovepos;
		     // *OldFileHeadpos,*OldFileNoncepos,*OldFileNextpos,*OldFileMovepos;
    NewFileHeadpos=NewFileNoncepos=NewFileNextpos=NewFileMovepos=NULL;

	rs.CreateInstance(__uuidof(Recordset));
	FileRs.CreateInstance(__uuidof(Recordset));
	ZaRs.CreateInstance(__uuidof(Recordset));
	ZaRsCount.CreateInstance(__uuidof(Recordset));
		//OldFileHeadpos=OldFileNoncepos=OldFileNextpos=OldFileMovepos=NULL;
	try
	{
		

		/*
		Sql="SELECT * FROM ZY WHERE FileName='"+NewFileName+"' AND VolumeID="+ltos(SelVlmID);
		rs->Open(_variant_t(Sql),pCon,adOpenDynamic,adLockOptimistic,adCmdText);
		if(!rs->adoEOF || !rs->BOF)
		{
			rs->Close();
			return NewFileName;
		}
		rs->Close();*/
		
		Sql="SELECT count(*) FROM ZY WHERE FileName='"+NewFileName+"' AND VolumeID="+ltos(SelVlmID);
		rs->Open(_variant_t(Sql),pCon,adOpenDynamic,adLockOptimistic,adCmdText);
		//已提取文件是否有正要提取的文件
		if(long(rs->GetCollect(_variant_t(long(0))))>0)
		{
			rs->Close();
			return NewFileName;
		}
		rs->Close();
		
		Sql="SELECT count(*) FROM zy WHERE VolumeID="+ltos(SelVlmID);	
		rs->Open(_variant_t(Sql),pCon,adOpenDynamic,adLockOptimistic,adCmdText);
	///	rs->Requery(-1);
		//是否有相同的卷册号文件
		if(long(rs->GetCollect(_variant_t(long(0))))<=0)
		{ 
		   rs->Close();
		   return NewFileName;
		}
        rs->Close();
        
		PickFileJSDBH(NewFileName,&NewFileHeadpos,&NewFileNoncepos,&NewFileNextpos,YLJSVer);
		
		NewFileMovepos=NewFileHeadpos;
		
		//求出正要提取文件中节点号个数NewJSDBHNumer
		while(NewFileMovepos!=NULL)
		{
			NewJSDBHNumer++;
			NewFileMovepos=NewFileMovepos->NextNode;
		}
		
		JSDBHNumer=-1;
		
		Sql="SELECT * FROM zy WHERE VolumeID="+ltos(SelVlmID);
		FileRs->Open(_variant_t(Sql),pCon,adOpenDynamic,adLockOptimistic,adCmdText);
		
		while(!FileRs->adoEOF)
		{
			int FileNameId=long(FileRs->GetCollect("FileNameID"));
			CString FileName=vtos(FileRs->GetCollect("FileName"));
			 //JSDBH_Dlg.DoModal();
			//JSDBH_Dlg.m_list.AddString(FileName);
			Sql="SELECT * FROM za WHERE FileNameID="+ltos(FileNameId);
			CSql="SELECT count(*) FROM za WHERE FileNameID="+ltos(FileNameId);
			//ZaRs=ConDB->Execute(_bstr_t(Sql),NULL,adCmdText);
			
			ZaRsCount->Open(_variant_t(CSql),pCon,adOpenDynamic,adLockOptimistic,adCmdText);
			OldJSDBHNumer=long(ZaRsCount->GetCollect(_variant_t(long(0))));
            ZaRsCount->Close();

			ZaRs->Open(_variant_t(Sql),pCon,adOpenDynamic,adLockOptimistic,adCmdText);    
			JSDBHNumer1=0;
			NewFileMovepos=NewFileHeadpos;
			while(NewFileMovepos!=NULL)
			{
				ZaRs->Filter=_variant_t("JSDBH1="+ltos(NewFileMovepos->JSDBH));
				if(!ZaRs->adoEOF || !ZaRs->BOF)
					JSDBHNumer1++;	
				NewFileMovepos=NewFileMovepos->NextNode;
			}
			ZaRs->Close();
			JSDBHNumer1=(OldJSDBHNumer+NewJSDBHNumer)-2*JSDBHNumer1;

			if(JSDBHNumer1<JSDBHNumer || JSDBHNumer==-1)
			{
				JSDBHNumer=JSDBHNumer1;
				EndFileName=FileName;
			}
			FileRs->MoveNext();
			/*SaveFileJSDBH(FileName,&OldFileHeadpos,&OldFileNoncepos,&OldFileNextpos);
			if(OldFileHeadpos==NULL)
			{   rs->MoveNext();
				continue;
			}
			JSDBHNumer1=0;
			NewFileMovepos=NewFileHeadpos;
			while(NewFileMovepos!=NULL)
			{
				
				OldFileMovepos=OldFileHeadpos;
				while(OldFileMovepos!=NULL)
				{
					if(NewFileMovepos->JSDBH==OldFileMovepos->JSDBH)
							JSDBHNumer1++;
					OldFileMovepos=OldFileMovepos->NextNode;
				}
				NewFileMovepos=NewFileMovepos->NextNode;
			}
			if(JSDBHNumer1>JSDBHNumer)
			{
				JSDBHNumer=JSDBHNumer1;
				EndFileName=FileName;
			}
			OldFileMovepos=OldFileHeadpos;
			while(OldFileMovepos!=NULL)
			{
				OldFileHeadpos=OldFileMovepos;
				OldFileMovepos=OldFileMovepos->NextNode;
				free(OldFileHeadpos);
			}
			OldFileHeadpos=OldFileNoncepos=OldFileNextpos=NULL;
			
			rs->MoveNext();
			*/
		}
		//釋放內存
        NewFileMovepos=NewFileHeadpos;
		while(NewFileMovepos!=NULL)
		{
			NewFileHeadpos=NewFileMovepos;
			NewFileMovepos=NewFileMovepos->NextNode;
			free(NewFileHeadpos);
		}
		FileRs->Close();
}
	catch(CException *e)
	{
		e->Delete();
	}
		catch(...)
		{
		
			if(rs->State!=adStateClosed)
				rs->Close();
			if(FileRs->State!=adStateClosed)
				FileRs->Close();
			if(ZaRs->State!=adStateClosed)
				ZaRs->Close();
		}
		
    return EndFileName;


}


extern CString JSJDH_Dlg_FileName;
extern IDispatch* JSJDH_Dlg_pCon;
void ReadYljsResult(_Recordset* rs)
{
	//RemAuthor:
	//作者:
	//目的:提取管道应力计算数据
	//输入:
	//返回: 
	AfxGetApp()->BeginWaitCursor();
	_ConnectionPtr ConnDB;
	ConnDB.CreateInstance(__uuidof(Connection));
	ConnDB=rs->GetActiveConnection();
	
	try
	{ 
		long X=0;
		CString DataFileName;
		CString strTemp;
		//西北电力院应力分析接口文件cad8.dat
		CString strFileNameCAD8;
		bool  bExit=false; //退出标志
        CString strAutoPSAVer;
		//FrmPhsData.ShowWindow(SW_SHOWNORMAL);
		ifstream f;
		//"您要替换" + EDIBgbl::strSelSpec + EDIBgbl::SelJcdm + "册中的支吊架原始数据吗？"
		strTemp.Format(GetResStr(IDS_ReplacePSADataInSpecVolume),EDIBgbl::SelSpecName,EDIBgbl::SelJcdm);

		if(ShowMessage(strTemp, MB_YESNO|MB_ICONQUESTION, GetResStr(IDS_AutoImportPSAData)) ==IDYES)
		{
            IDispatch* pCon=rs->GetActiveConnection();
			_RecordsetPtr RD;
			RD.CreateInstance(__uuidof(Recordset));
			CDlgFindAutoPSAFile dlg;
			int Flg=1;
			while(Flg == 1)
			{
				 if( IDOK != dlg.DoModal() )
				 {
					return;
				 }
				 //如果选择了一个AutoPSA打开的文件名,同时该文件存在,则用该文件定位.
				 if( dlg.m_bFlag==1/*当选择了一个文件名.*/ && !modPHScal::gsPSAOutFilePath.IsEmpty() && FileExists(modPHScal::gsPSAOutFilePath) )
				 {
					DataFileName = user::SelFileDwg(GetResStr(IDS_PSAOutputFileList),modPHScal::gsPSAOutFilePath,modPHScal::g_iPSAFilterIndex);
				 }
				 else
				 {
					 //否则,用最近一次导入的文件定位.如果没有,则用模板文件的路径定位.

					CString StrSql="SELECT * FROM ZY WHERE VolumeID="+ltos(EDIBgbl::SelVlmID);
					RD->Open(_variant_t(StrSql),pCon,adOpenDynamic,adLockOptimistic,adCmdText);
					if(RD->adoEOF)
						DataFileName = user::SelFileDwg(GetResStr(IDS_PSAOutputFileList),modPHScal::gsPSAOutFilePath,modPHScal::g_iPSAFilterIndex);
					else
					{
						CString StrFileNamePath;
						RD->MoveLast();
						StrFileNamePath=vtos(RD->GetCollect("FileName"));
						StrFileNamePath=GetFilePath(StrFileNamePath);
						DataFileName = user::SelFileDwg(GetResStr(IDS_PSAOutputFileList),StrFileNamePath,modPHScal::g_iPSAFilterIndex);
					}
					RD->Close();
				 }
				 if(DataFileName != "" && FileExists(DataFileName) || dlg.m_bFlag == 0/*没有弹出可供选择的AutoPSA文件名时,不管选择了文件,或没选择.都不循环 */) 
					 Flg=0;
			}
				//modPHScal::gsPSAOutFilePath=GetFilePath(DataFileName);
                 //GetOldFileName(DataFileName,pCon,EDIBgbl::SelVlmID);
			SetRegValue(_T("Settings"),"PSAOutFilePath",modPHScal::gsPSAOutFilePath);
			SetRegValue(_T("Settings"),"PSAOutFilterIndex",modPHScal::g_iPSAFilterIndex);
			if(DataFileName == "" || !FileExists(DataFileName)) return;
//add		
			long maxZdjh=FrmTxsr.m_pViewTxsr->GetMaxZdjh();

			
			_variant_t SQLx=rs->GetSource();
		    Delete1();
			UpDataZY();
			if(rs->State!=adStateClosed)
			{
				if(!rs->adoEOF && !rs->BOF)
				{
					try
					{
						rs->Update();
					}
					catch(...)
					{
						rs->CancelUpdate();
					}
				}
				try
				{
					rs->Close();
				}
				catch(...)
				{
				}
					
			}
			::FrmPhsData.m_DBGbill.SetRefDataSource(NULL);
//			FrmTxsr.m_pViewTxsr->m_Databill.SetRefRecordset(NULL);
			//rs->Close();此句必须移至下面带 ***处，否则出错后导致“对象关闭后不能执行这个操作”，死锁
			_RecordsetPtr rs1;
			rs1.CreateInstance(__uuidof(Recordset));
			rs1->Open(SQLx,pCon,adOpenKeyset,adLockOptimistic,adCmdText);
			
			CString temp;
			temp = DataFileName.Right(3);
			temp.MakeLower();
			strFileNameCAD8 = DataFileName.Right(8);
			strFileNameCAD8.MakeLower();

//判断是CAESARII分析的结果还是GLIF分析的结果
			if ( temp == "mdb" )   //是mdb数据库
			{
				try
				{
					CString strTableName = "OUTPUT_GLOBAL_ELEMENT_FORCES";
					CString connTempString="Provider=Microsoft.Jet.OLEDB.4.0;Persist Security Info=False;Data Source=%s";
					CString connString;
					connString.Format(connTempString,DataFileName,"");
					_ConnectionPtr m_pConn;
					m_pConn.CreateInstance(__uuidof(Connection));
					m_pConn->Open(_bstr_t(connString),"","",0);
					if (!EDIBgbl::tdfExists(m_pConn,strTableName))//进一步判断是否有指定的表
					{
						ShowMessage(GetResStr(IDS_PSANameVer)+" Unknown");	
						m_pConn->Close();
						m_pConn = NULL;
					}
					else
					{	
						//处理CAESARII的数据分析结果
						_RecordsetPtr m_prsVersion;  //版本记录
                        CString  Version;
						 //判断表"version"是否存在pfg20050602
						//如果没有 version=4.2
						if(EDIBgbl::tdfExists(m_pConn,"Version"))
						{
							
						//pfg20050602

							_bstr_t  VersionSQL("SELECT VERSION FROM VERSION");
							_variant_t Hoder;

							m_prsVersion=m_pConn->Execute(VersionSQL,NULL,adCmdText);						
							Hoder=m_prsVersion->GetCollect("Version");
							Version.Format("%s",vtos(Hoder));
							Version.TrimLeft();
							Version.TrimRight();
						}
						else//pfg20050602 start
						{
							Version=_T("4.20");
						}//pfg20050602 end
						m_pConn->Close();
						m_pConn = NULL;
						CString  m_version;
						m_version.Format("CAESARII %s",Version);
						ShowMessage(GetResStr(IDS_PSANameVer)+m_version);

						CAESARIIToPHS mCaesarIIToPHS;
						if (Version=="4.50")
						{  
						  long FileNameID1=GetFileNameId(DataFileName,pCon);  //数据库文件没有做当文件名修改后自动覆盖已提取记录功能；
   						  mCaesarIIToPHS.ReadResult_CAESARII45(rs1,DataFileName,maxZdjh,FileNameID1,"CASEARII 4.50");
						  //CAESARII应力分析程序为冷态吊零计算弹簧
						}
						else if (Version=="4.20")
						{						                      
						  long FileNameID1=GetFileNameId(DataFileName,pCon);  //数据库文件没有做当文件名修改后自动覆盖已提取记录功能；
   						  //ReadResult_CAESARII42(rs1,DataFileName,maxZdjh,FileNameID1);//pfg20050624原代码
   						  mCaesarIIToPHS.ReadResult_CAESARII45(rs1,DataFileName,maxZdjh,FileNameID1,"CASEARII 4.20");//pfg20050624
						  //CAESARII应力分析程序为冷态吊零计算弹簧
						}
					    else
						{
						  long FileNameID1=GetFileNameId(DataFileName,pCon);  //数据库文件没有做当文件名修改后自动覆盖已提取记录功能；
   						  //ReadResult_CAESARII42(rs1,DataFileName,maxZdjh,FileNameID1);//pfg20050624原代码
   						  mCaesarIIToPHS.ReadResult_CAESARII45(rs1,DataFileName,maxZdjh,FileNameID1,"CASEARII 4.20");//pfg20050624
						  //CAESARII应力分析程序为冷态吊零计算弹簧
						}
										
						
						gbHotStatus2Zero=0;
						::SetRegValue(_T("Settings"),"HotStatus2Zero",(gbHotStatus2Zero));
					}
				}
				catch(_com_error & e)
				{
					AfxMessageBox(e.Description());
				}
			}
			else			 
			{ 
				if(strFileNameCAD8=="cad8.dat")
				{
					//处理西北电力设计院应力分析程序ZHDYF的数据分析结果
					ShowMessage(GetResStr(IDS_PSANameVer)+" ZHDYF3.0");
					YLJSVer = "ZHDYF3.0";
				}
				else
				{
					//处理GLIF的数据分析结果
					X = 1;
					f.open(DataFileName);
					while((!f.eof() || X<=30) && !bExit)
					{
						f.getline(strTemp.GetBuffer(255),255);
						strTemp.ReleaseBuffer();
						strTemp.TrimLeft();strTemp.TrimRight();
						strTemp.MakeUpper();
						if ( strTemp.Find("#$ VERSION") != -1)
						{
							f.getline(strTemp.GetBuffer(255),255);
							strTemp.ReleaseBuffer();
							strTemp.TrimLeft();strTemp.TrimRight();
							
							YLJSVer = "AutoPSA" + GetStrAt1(strTemp,1);
							ShowMessage(GetResStr(IDS_PSANameVer)+YLJSVer);
							break;
							
							
						}
						else if(strTemp.Find("GLIF---V/1.2")!=-1)
						{
							ShowMessage(GetResStr(IDS_PSANameVer)+" GLIFV1.2");
							YLJSVer = "GLIF1.2";
							break;
						}
						else if(strTemp.Find("GLIF---V/3.")!=-1)
						{
							ShowMessage(GetResStr(IDS_PSANameVer)+" GLIFV3.x");
							YLJSVer = "GLIF3.x";
							while(!f.eof() || X <= 70)
							{
								f.getline(strTemp.GetBuffer(255),255);
								strTemp.ReleaseBuffer();
								if(strTemp.Left(5) == " ++++ ")
								{
									//湖南电力院1999年5月前使用的GLIFV3.1版本，应该是较旧的版本
									gbNewGLIFV31 = false;
									bExit = true;
									break;
								}
								else
								{
									if(strTemp.Left(3)==" + ")
									{
										//贵州院2001年5月前使用的GLIFV3.1版本，以及辽宁院使用的所谓GLIFV3.2,应该是较新的版本
										gbNewGLIFV31 = true;
										bExit = true;
										break;
									}
										//未处理的情况
									//End If
								}
								X ++;
							}
							break;
						}
						else if(strTemp.Find("UESOFT AUTOPSA")!=-1 || strTemp.Find("UESOFT-AUTOPSA")!=-1)
						{
							strAutoPSAVer=strTemp.Mid(strTemp.Find("UESOFT AUTOPSA")+14,3);
							ShowMessage(GetResStr(IDS_PSANameVer)+" AutoPSA"+strAutoPSAVer);
							YLJSVer = "UESOFT AutoPSA"+strAutoPSAVer;
							while(!f.eof() || X <= 70)
							{
								f.getline(strTemp.GetBuffer(255),255);
								strTemp.ReleaseBuffer();
								if(strTemp.Left(3)==" + ")
								{
									//AutoPSA1.0使用GLIFv3.1兼容格式
									gbNewGLIFV31 = true;
									bExit = true;
									break;
								}
								X ++;
							}
							break;
						}
						else if (strTemp.Find("NGZ=")!=-1&&strTemp.Find("NGW=")!=-1)
						{

							ShowMessage(GetResStr(IDS_PSANameVer)+" XNP 2.0");
							YLJSVer = "XNP 2.0";
							break;
						}
						else
						{
							if(X >= 30)
							{
								AfxMessageBox(GetResStr(IDS_PSANameVer)+" Unknown");
								YLJSVer = "";
								break;
							}
						}
						//MsgBox X & strTemp
						X++;
					}
					f.close();
					//结束GLIF处理
				}
      
/*				frmStatus.ShowWindow(SW_SHOW);
				frmStatus.m_Label1= "从文件 " + DataFileName;
				frmStatus.m_Label2= "到数据库 " + EDIBgbl::dbPRJDB.GetName() + "中的表" + EDIBgbl::TBNRawData;
				frmStatus.SetWindowText( "数据转换");
				frmStatus.UpdateData(false);
				frmStatus.UpdateStatus(0,true);
				*/
				 //和已经提取了的文件进行比较，确定是否要显示对话框；
            JSJDH_Dlg_pCon=pCon;
			JSJDH_Dlg_FileName=GetOldFileName(DataFileName,pCon,EDIBgbl::SelVlmID,YLJSVer);
			CString m_listStr;
			long FileNameID1;
			if(JSJDH_Dlg_FileName!=DataFileName)
			{
				BestrowJSJDH::WriteFileName=DataFileName;
				if(JSDBH_Dlg.DoModal()==IDOK && JSJDH_Dlg_FileName!=" ")  
					FileNameID1=GetFileNameId(JSJDH_Dlg_FileName,pCon);
				else
					FileNameID1=GetFileNameId(DataFileName,pCon);
			}
			else
				FileNameID1=GetFileNameId(DataFileName,pCon);  

				frmStatus.m_Label1 = "正在导入文件:";	//初始进度条的显示内容.	  by	zsy 2005.3.1
				frmStatus.m_Label2 = DataFileName;
				frmStatus.SetWindowText( "导入应力计算数据" );
				frmStatus.UpdateData(false);

  
				if( YLJSVer == "AutoPSA7.0" || YLJSVer == "AutoPSA8.0")
				{
					gbHotStatus2Zero=true;
					::SetRegValue(_T("Settings"),"HotStatus2Zero",(gbHotStatus2Zero));
					ReadPSAData(rs1,DataFileName,maxZdjh,FileNameID1);
				}
				else if(YLJSVer=="GLIF1.2")
				{
						//GLIF应力分析程序为热态吊零计算弹簧
						gbHotStatus2Zero=true;
						::SetRegValue(_T("Settings"),"HotStatus2Zero",(gbHotStatus2Zero));
						ReadResult_GLIF12(rs1, DataFileName,maxZdjh,FileNameID1);
				}
				else if(YLJSVer=="GLIF3.x"||YLJSVer.Find("UESOFT AutoPSA") != -1)
				{
						//GLIF应力分析程序为热态吊零计算弹簧
						gbHotStatus2Zero=true;
						::SetRegValue(_T("Settings"),"HotStatus2Zero",(gbHotStatus2Zero));
						if(gbNewGLIFV31)
							ReadResult_GLIF31New(rs1, DataFileName,maxZdjh,FileNameID1);
						else
							ReadResult_GLIF31(rs1, DataFileName,maxZdjh,FileNameID1);
				}
				else if(YLJSVer=="ZHDYF3.0")
				{
						//西北院应力分析程序为冷态吊零计算弹簧
						gbHotStatus2Zero=0;
						::SetRegValue(_T("Settings"),"HotStatus2Zero",(gbHotStatus2Zero));
						ReadResult_ZHDYF30(rs1, DataFileName,maxZdjh,FileNameID1);
				}
				else if(YLJSVer=="XNP 2.0")
				{
					//西南院应力分析程序为热态吊零计算弹簧
					gbHotStatus2Zero=true;
					::SetRegValue(_T("Settings"),"HotStatus2Zero",(gbHotStatus2Zero));
					ReadResult_SWEDPSA(rs1, DataFileName,maxZdjh,FileNameID1);
				}
			}
/*
			try
			{
				rs1->Update();
			}
			catch(...)
			{
				rs1->CancelUpdate();
			}
*/
			rs1->Close();
			rs1=NULL;
			//********注意下句不能移动到函数开头,否则死锁
			//rs->Close();
			rs->Open(SQLx,pCon,adOpenKeyset,adLockOptimistic,adCmdText);
			FrmTxsr.m_pViewTxsr->m_bAllowUpd=false;
			FrmTxsr.m_pViewTxsr->m_Databill.SetRefRecordset(rs);
			FrmTxsr.m_pViewTxsr->m_Databill.SetEnabled(TRUE);
			FrmPhsData.m_DBGbill.SetRefDataSource(rs);
			EDIBDB::SetColumnsProperty(FrmPhsData.m_DBGbill, EDIBgbl::SelBillType);
			FrmTxsr.m_pViewTxsr->m_bAllowUpd=true;
	
		}
	}
    
	catch(_com_error e)
	{
		ShowMessage(e.Description());
		FrmTxsr.m_pViewTxsr->m_bAllowUpd=true;
	}
	
	frmStatus.UpdateStatus(1,true);
	frmStatus.ShowWindow(SW_HIDE);
	AfxGetApp()->EndWaitCursor();
}

//从一行以空格为间隔符的字符串中取出一个double值,并
double GetNextValue(CString &str)
{
	str.TrimLeft();

	if(str.IsEmpty())
	{
		return 0.0;
	}

	int nFind = str.Find(_T(" "));
	double dbVal = atof(Mid(str, 0, nFind));

	if(nFind < 0)
	{
		str.Empty();
	}
	else
	{
		str = str.Mid(nFind);
	}
	
	return dbVal;
}

void ReadResult_ZHDYF30(_Recordset* rsResult ,CString SourceDataFileName,long maxZdjh,long FileNameID)
{
   //Remauther:
   //作者:
   //目的:本程序为读西北电力院组合单元法应力分析程序ZHDYF生成的支吊架接口数据文件cad8.dat文件，可用最后的msgbox语句来测试程序的正确性
   //输入:rsResult数据表内部单位:位移mm,荷载kgf.
   //返回:
	try
	{
		float phsVAL[13][13];//此数组代表后面的单精度数据，X维代表整型数据后面的列，//Y维代表行
		int iPreINT[3];//此数组代表记录前面的两个整型数
		long ltemp=maxZdjh;			 
		_Recordset* rsData;
		_RecordsetPtr tmprs;
		tmprs.CreateInstance(__uuidof(Recordset));

		_variant_t varTmp;
		CString strOldPhsSPRINGSel;
		CString strTemp, temp ;int iTemp=0;
		CString strSQL;		
		CString strSprTbn;

		const char* strPIPING_STRESS_OUTUNIT = "UNIT OF OUTPUTDATA IS";
		const char* strSUMMARY_FOR_RESTRAINTS = "SUMMARY FOR RESTRAINTS";
		const char* strCOLD_SPRING_TABLE = "COLD SPRINGS TABLE";
		const char* strEND_RawData = "10       1       1         1";
		const char* strPSAVER = "ZHDYF3.0";
		const char* strSPRINGType = "SPRING VERSION:";   
		const char* strEND_THIS_PIPING = "*THIS PIPING STRUCTURE *CONTAINS*";
//		int fdFlg;
            
		//截面特性记录:GLIF1.2,NC=6;GLIF3.x,NC=60(弯单元)及61(直单元),成对使用.
		//介质特性记录:GLIF1.2,NC=9;GLIF3.x,NC=90(设计工况).
		//const iNCtj = 90, iNCdw = 60
		const int iNCtj = 9;
		const int iNCdw = 6;   
		float sngDWTemp=0, TjTemp=0;
		int iJSJDH_start=0, iJSJDH_end=0;
		long i=0;   
		int j=0, iC1=0;
		int iFlag=0, iCount=0;
		int mviSPR=0;
		CString mvS;
		ifstream f;
		CArray<long,long> iJSJDH;
		CArray<float,float> sngDW;
		CArray<float,float> Tj;
		
		int nFind = 0;

		iJSJDH.SetSize(100);
		sngDW.SetSize(100);
		Tj.SetSize(100);
   
		f.open(SourceDataFileName);
		//开始读原始数据
				//坐标轴方向x/y/z轴向上=1/2/3
				//ZHDYF3.0与支吊架接口时只能 [Y] 轴向上
	
		CInputBox dlg;
		CString strUPxyz;
		while(strUPxyz == "" || (strUPxyz.GetAt(0) < '1' || strUPxyz.GetAt(0) > '3')  || (strUPxyz != "1" && strUPxyz != "2" && strUPxyz != "3"))
		{
			dlg.m_strTitle=GetResStr(IDS_INPUTUPXYZ);
			dlg.m_strWndText=GetResStr(IDS_SELUPXYZ);
			dlg.m_strValue="3";
			dlg.DoModal();
			strUPxyz =dlg.m_strValue;
			strUPxyz.TrimLeft();
			strUPxyz.TrimRight();
		}

		giUPxyz = atoi(strUPxyz);

		//ZHDYF3.0中,只能按新弹簧计算
		mviSPR=20;
		//mviSPR = temp.Find(strSPRINGType);
		//mviSPR = atoi(Trim(Mid(temp, mviSPR + strlen(strSPRINGType), 3)));
		modPHScal::gnSPR_CHZBH = 0.35f;
		strOldPhsSPRINGSel = modPHScal::gsPhsSPRINGSel;
		//因为应力计算选择的弹簧标准可能不同,故重新更新规范
		//由于生成规范花费时间较长，所以只有当规范变化时，才需要重新更新
		if( mviSPR == 20 )
		{
			strSQL = "SELECT Spring_property FROM phsManuSPRING WHERE StandardDesc=\'";
				strSQL += strSprNWEPDI;
				strSQL += "\'";
			tmprs->Open((_bstr_t)strSQL, _variant_t((IDispatch*)EDIBgbl::dbSORT,true), 
				adOpenKeyset, adLockOptimistic, adCmdText); 
			if(tmprs->BOF && tmprs->adoEOF)
			{
			}
			else
			{
				tmprs->get_Collect((_variant_t)"Spring_property",&varTmp);
				strSprTbn=vtos(varTmp);//获得弹簧属性表
			}
			tmprs->Close();
			if( strOldPhsSPRINGSel != strSprNWEPDI )
			{
				modPHScal::gsPhsSPRINGSel = strSprNWEPDI;
				//if( blnSelphsSPEC(False) ) SaveRecentWorkSPEC();
			}
		}
		else if( mviSPR == 16 )
		{
 
		}

		//荷载变化系数保存到外部
		::SetRegValue(_T("Settings"), "SPR_Chzbh", (modPHScal::gnSPR_CHZBH));
		//西北院应力分析程序为冷态吊零计算弹簧
		gbHotStatus2Zero=0;
		::SetRegValue(_T("Settings"),"HotStatus2Zero",(gbHotStatus2Zero));
  
		_RecordsetPtr rs1;
		rs1.CreateInstance(__uuidof(Recordset));
		strSQL.Format("SELECT G,SEQ FROM %s",strSprTbn);
		rs1->Open((_bstr_t)strSQL, _variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
			adOpenKeyset, adLockOptimistic, adCmdText); 

		iCount = 1;
                    
		rsData = rsResult;
   
		//ZHDYF3.0:开始读结果数据
		while(!f.eof())
		{
			int iType=7,iPnum=1,iSnum=0,iSPRSeq=0;
			float fDW=108,fTj=20,fPEL=0,fPaz=0,fPgz=0,fPjg=0;
			float fThick = 0; //壁厚
			 
			f.getline(temp.GetBuffer(255),255);
			temp.ReleaseBuffer();
			temp.TrimLeft();
			temp.TrimRight();

			if(temp.IsEmpty())		//跳过空行
				continue; 

			//line1
			if( atoi(Mid(temp,0, 4)) > 0 )  iPreINT[1] = atoi(Mid(temp,0, 4));
			
			//line2
			f.getline(temp.GetBuffer(255),255);
			temp.ReleaseBuffer();
	
			fDW = GetNextValue(temp);    //外径			
			fThick = GetNextValue(temp); //壁厚	
			fTj = GetNextValue(temp);    //温度
			fPEL = GetNextValue(temp);   //标高

			//line3
			f.getline(temp.GetBuffer(255),255);
			temp.ReleaseBuffer();

			iType = (int)GetNextValue(temp);   //支吊架类型
			iPnum = (int)GetNextValue(temp);   //并联数
			iSnum = (int)GetNextValue(temp);   //串联数
			iSPRSeq = (int)GetNextValue(temp); //弹簧号
			fPaz = GetNextValue(temp);         //安装荷载
			fPgz = GetNextValue(temp);         //管子荷载
			fPjg = GetNextValue(temp);         //结构荷载

			
			//生成兼容GLIF定义的支吊架类型
			if(iSnum==0 )
			{
				//弹簧串联数=0，非弹簧支吊架
				if(iType==5 || iType==7)
				{
					//刚性吊架
					iPreINT[2]=120;
				}
				else if(iType==3)
				{
					//固定支架
					iPreINT[2]=177;
				}
				else
				{
					//未知
					iPreINT[2]=120;
				}
			}
			else if(iSnum>=1)
			{
				if(iSPRSeq==99)
				{
					//恒力弹簧
					iPreINT[2]=9999;
				}
				else
				{
					iPreINT[2]=iSnum*1000+iPnum*100+iSPRSeq;
				}
			}
		
			
			//line4：冷位移(x,y,z),热位移(x,y,z),
			f.getline(temp.GetBuffer(255),255);
			temp.ReleaseBuffer();
			
			for(j=1; j<=6; j++)//每行6个数据
			{
				phsVAL[j][1] = GetNextValue(temp);
			}
		
			//line5：冷态力(x,y,z),冷态力矩(x,y,z),热态力(x,y,z),热态力矩(x,y,z)
			f.getline(temp.GetBuffer(255),255);
			temp.ReleaseBuffer();
			
			for (j=1; j<=12; j++)//每行12个数据
			{
				phsVAL[j][2] = GetNextValue(temp);
			}
			
			//line6：松冷力(x,y,z),松冷力矩(x,y,z),偶然力(x,y,z),偶然力矩(x,y,z)
			f.getline(temp.GetBuffer(255),255);
			temp.ReleaseBuffer();

			for (j=1; j<=12; j++)//每行12个数据
			{
				phsVAL[j][3] = GetNextValue(temp);
			}

			rsData->Filter=_variant_t(CString("[jsdbh1] = ") + ltos(iPreINT[1])+" AND "+CString("[FileNameID]=")+ltos(FileNameID));
			if( rsData->BOF && rsData->adoEOF )
			{
				rsData->AddNew();
				rsData->PutCollect("VolumeID",_variant_t(EDIBgbl::SelVlmID));
				rsData->PutCollect("Zdjh",_variant_t((long)(ltemp+1)));
				rsData->PutCollect("FileNameID",_variant_t(FileNameID));
				//rsData->PutCollect("Zdjh",_variant_t((long)iPreINT[1]));
				rsData->Update();
				ltemp++;
			}
			rsData->PutCollect("UPxyz",_variant_t((long)giUPxyz));
			rsData->PutCollect("PSAver",strPSAVER);
			rsData->PutCollect("JSDBH1",_variant_t((long)iPreINT[1]));
			rsData->PutCollect("dj1",_variant_t((float)fDW));
			rsData->PutCollect("t01",_variant_t((float)fTj));
			rsData->PutCollect("Type",_variant_t((long)iPreINT[2]));
			rsData->PutCollect("psaType",_variant_t((long)iPreINT[2]));
			rsData->PutCollect("dh1", _variant_t(fPEL));
			rsData->PutCollect("Unit_Force", "N");
			rsData->PutCollect("Unit_Momental", "N.m");
			rsData->PutCollect("Unit_Displacement", "mm");
			rsData->PutCollect("paz0", _variant_t(fPaz));
			rsData->PutCollect("paz1", _variant_t(fPaz));
			rsData->PutCollect("pgz1", _variant_t(fPgz));
			rsData->PutCollect("pjg1", _variant_t(fPjg));
			rsData->PutCollect("pxl1", _variant_t(phsVAL[1][2]));
			rsData->PutCollect("pyl1", _variant_t(phsVAL[2][2]));
			rsData->PutCollect("pzl1", _variant_t(phsVAL[3][2]));
			rsData->PutCollect("pxr1", _variant_t(phsVAL[7][2]));
			rsData->PutCollect("pyr1", _variant_t(phsVAL[8][2]));
			rsData->PutCollect("pzr1", _variant_t(phsVAL[9][2]));


			rsData->PutCollect("xr1", _variant_t(phsVAL[4][1] * 1));
			rsData->PutCollect("yr1", _variant_t(phsVAL[5][1] * 1));
			rsData->PutCollect("zr1", _variant_t(phsVAL[6][1] * 1));					

			if( iSPRSeq!=99 )//只有弹吊才要计算热位移
			{
				float fG,fR;

				if(iPreINT[2]>1000 && iPreINT[2]<5000)
				{
					CString strTmp;
					strTmp.Format("%d",iPreINT[2]);
					_variant_t vTmp;
					rs1->Find((_bstr_t)("SEQ=" + ltos(iSPRSeq)), 0, adSearchForward);
					if(!rs1->adoEOF)
					{
						rs1->get_Collect((_variant_t)"G",&varTmp);
						fG=vtof(varTmp);
						fR=fabs(fPaz) - fabs(fPgz);
						fR/=EDIBgbl::kgf2N;
						fR/=iPnum;
						fR/=(fG/iSnum);
						if(giUPxyz==1)
						{
							rsData->PutCollect("xr1", _variant_t(fR));
						}
						else if(giUPxyz==2)
						{
							rsData->PutCollect("yr1", _variant_t(fR));
						}
						else
						{
							rsData->PutCollect("zr1", _variant_t(fR));
						}
					}
				}else
				{
				   rsData->PutCollect("yr1", _variant_t(phsVAL[5][1] * 1));
				}
			}
			else
			{
			   rsData->PutCollect("yr1", _variant_t(phsVAL[5][1] * 1));
			}

			rsData->PutCollect("xl1", _variant_t(phsVAL[1][1] * 1));
			rsData->PutCollect("yl1", _variant_t(phsVAL[2][1] * 1));
			rsData->PutCollect("zl1", _variant_t(phsVAL[3][1] * 1));

			//rsData->PutCollect("haz", _variant_t(phsVAL[1][8] * 1));
			rsData->PutCollect("mxl1", _variant_t(phsVAL[4][2]));
			rsData->PutCollect("myl1", _variant_t(phsVAL[5][2]));
			rsData->PutCollect("mzl1", _variant_t(phsVAL[6][2]));
			rsData->PutCollect("mxr1", _variant_t(phsVAL[10][2]));
			rsData->PutCollect("myr1", _variant_t(phsVAL[11][2]));
			rsData->PutCollect("mzr1", _variant_t(phsVAL[12][2]));

			rsData->Filter=_variant_t((long)adFilterNone);
			iC1++;
			 if( iCount <= 0 ) iCount = 1;
			 frmStatus.UpdateStatus((float)iC1/(float)iCount,true);
			 frmStatus.UpdateWindow();
		}
		f.close();
		if(!rsData->adoEOF || !rsData->BOF)
			rsData->MoveFirst();
		_variant_t vtmp;
		while(!rsData->adoEOF)
		{
			vtmp=rsData->GetCollect("ZDJH");
			rsData->PutCollect("ZDJH",_variant_t((long)(vtoi(vtmp)+1000)));
			rsData->MoveNext();
		}
		_ConnectionPtr pCon;
		strSQL=(char*)(bstr_t)rsData->GetSource();
		pCon=rsData->GetActiveConnection();
		strSQL.Replace("ORDER BY JSDBH1","ORDER BY FileNameID");
		rsData->Close();
		rsData->Open(_variant_t(strSQL),(IDispatch*)pCon,adOpenKeyset,adLockOptimistic,adCmdText);
		//rsData->Sort=_bstr_t("JSDBH1");
		int itmpZdjh=1;
		while(!rsData->adoEOF)
		{
			rsData->PutCollect("ZDJH",_variant_t((long)itmpZdjh));
			itmpZdjh++;
			rsData->MoveNext();
		}
	}
	catch(_com_error& e)
	{
		ShowMessage(e.Description());
	}
	catch(CException *e)
	{
		e->Delete();
	}
	catch(...)
	{
		ShowMessage("C++ Exception at readpsaresult.cpp::ReadResult_ZHDYF30()");
	}
}

//返加字符串中的数据 //add by cbl
double GetStrAt(CString str,int i)
{
	str = str.Mid(str.Find('+')+1);
	while(str.Find('(') != -1 && str.Find(')') != -1)
	{
		str.Replace('(',' ');
		str.Replace(')',' ');
	}
	str.TrimLeft();
	str = str +"  ";
	for(int index=1; index <=i-1;index++)
	{
		str = str.Mid(str.Find(' ')+1);
		str.TrimLeft();
	}
	return atof(str.Left(str.Find(' ')));

}

//从节点坐标文件中读取数据到链表 add by cbl
void ElevationFileToList(CString FileName,ElevationList *pElevationList,int ICC)
{
	ifstream fin;
	CString strText;
	int NodeNum;
	double UpElevation=0;
	fin.open(FileName);
	if(!fin)
		return;
	CNodeUpElevation *pNodeUpElevationData;
	while(!fin.eof())
	{
		fin.getline(strText.GetBufferSetLength(255),255);
		strText.ReleaseBuffer();
		NodeNum = GetStrAt(strText,1);
	    UpElevation = GetStrAt(strText,ICC+1);

		pNodeUpElevationData = new CNodeUpElevation();
		pElevationList->AddTail(pNodeUpElevationData);
		pNodeUpElevationData->NodeNum = NodeNum;
		pNodeUpElevationData->UpElevation = UpElevation;
	}
	
}


void ReadResult_GLIF12(_Recordset* rsResult ,CString SourceDataFileName,long maxZdjh,long FileNameID)
{
   //Remauther:
   //作者:
   //目的:本程序为读*.out文件，可用最后的msgbox语句来测试程序的正确性
   //输入:rsResult数据表内部单位:位移mm,荷载kgf.
   //返回:
	try
	{
		long ltemp=maxZdjh;

		CString strOldPhsSPRINGSel;
		CString strTemp, temp ;int iTemp=0;
		CString strSQL;
		float phsVAL[7][9];//此数组代表后面的单精度数据，X维代表整型数据后面的列，Y维代表行
		int iPreINT[3];    //此数组代表记录前面的两个整型数
		_Recordset* rsData;

		const char* strPIPING_STRESS_OUTUNIT = "UNIT OF OUTPUTDATA IS";
		const char* strSUMMARY_FOR_RESTRAINTS = "SUMMARY FOR RESTRAINTS";
		const char* strCOLD_SPRING_TABLE = "COLD SPRINGS TABLE";
		const char* strEND_RawData = "10       1       1         1";
		const char* strPSAVER = "GLIF V12";
		const char* strSPRINGType = "SPRING VERSION:";
   
		const char* strEND_THIS_PIPING = "*THIS PIPING STRUCTURE *CONTAINS*";
		CString strSprTbn;
		_RecordsetPtr tmprs;
		tmprs.CreateInstance(__uuidof(Recordset));
		_variant_t varTmp;
//		int fdFlg;
            
		//截面特性记录:GLIF1.2,NC=6;GLIF3.x,NC=60(弯单元)及61(直单元),成对使用.
		//介质特性记录:GLIF1.2,NC=9;GLIF3.x,NC=90(设计工况).
		//const iNCtj = 90, iNCdw = 60
		const int iNCtj = 9;
		const int iNCdw = 6;
   
		float sngDWTemp=0, TjTemp=0;
		int iJSJDH_start=0, iJSJDH_end=0;
		CArray<long,long  > iJSJDH;
		CArray<float,float  > sngDW;
		CArray<float,float  > Tj;
		CArray<float,float &> sngDH1;

		sngDH1.SetSize(100);
		iJSJDH.SetSize(100);
		sngDW.SetSize(100);
		Tj.SetSize(100);
		long i=0;
   
		int j=0, iC1=0;
		ifstream f;
		int iFlag=0, iCount=0;
		CString mvS,tempsign;
		int mviSPR=0;
        bool sign=false;//chengbailian 2003.11.6 为了适应另外一种文件，90记录是否在10记录之录出现		
		long pos;

		//cbl 2004.1.10 增加了每个节点的标高
		ElevationList mElevationList;
		CNodeUpElevation *pNodeUpElevation;
		int ICC;
		
		
		//坐标文件名
		CString ElevationFileName=SourceDataFileName.Left(SourceDataFileName.ReverseFind('.'))+".et";//标高文件名

		//生成坐标文件(记录了每个节点的坐标)(链接库中的函数)
		ICC=PSAPointCoordinateWirteFile(SourceDataFileName,ElevationFileName);
	  
		//从文件中读取每个点的标高放入链表mElevationList中
		ElevationFileToList(ElevationFileName,&mElevationList,ICC);
        ::DeleteFile(ElevationFileName); 
	//cbl

		f.open(SourceDataFileName);
		//开始读原始数据
		while(!f.eof())
		{
			f.getline(temp.GetBuffer(255),255);
			temp.ReleaseBuffer();
			if(temp.Find(strPIPING_STRESS_OUTUNIT)!=-1)
			{
				if( temp.Find("NATIONAL") !=-1 ){
					EDIBgbl::PSA_OutDataUnit = "NATIONAL";
				}else{
					EDIBgbl::PSA_OutDataUnit = "ENGINEERING";
				}
				//坐标轴方向x/y/z轴向上=1/2/3
				//GLIFV1.2下只能Y轴向上
				giUPxyz = 2;
				f.getline(temp.GetBuffer(255),255);
				temp.ReleaseBuffer();
				//GLIFV1.2中,原始数据文件第2个记录第4个数据为弹簧种类,(ITW=20,按新弹簧计算;ITW=16,按管规弹簧)
				mviSPR = temp.Find(strSPRINGType);
				mviSPR = atoi(Trim(Mid(temp, mviSPR + strlen(strSPRINGType), 3)));
				modPHScal::gnSPR_CHZBH = 0.35f;
				strOldPhsSPRINGSel = modPHScal::gsPhsSPRINGSel;
				//因为应力计算选择的弹簧标准可能不同,故重新更新规范
				//由于生成规范花费时间较长，所以只有当规范变化时，才需要重新更新
				if( mviSPR == 20 )
				{
					strSQL = "SELECT Spring_property FROM phsManuSPRING WHERE StandardDesc=\'";
					strSQL += strSprNWEPDI;
					strSQL += "\'";
					tmprs->Open((_bstr_t)strSQL, _variant_t((IDispatch*)EDIBgbl::dbSORT,true), 
						adOpenKeyset, adLockOptimistic, adCmdText); 
					if(tmprs->BOF && tmprs->adoEOF)
					{
					}
					else
					{
						tmprs->get_Collect((_variant_t)0L, &varTmp);
						strSprTbn=vtos(varTmp);
					}
					tmprs->Close();
					if( strOldPhsSPRINGSel != strSprNWEPDI )
					{
						modPHScal::gsPhsSPRINGSel = strSprNWEPDI;
						//if( blnSelphsSPEC(False) ) SaveRecentWorkSPEC();
					}
				}
				else if( mviSPR == 16 )
				{
         
				}
				//荷载变化系数保存到外部
				::SetRegValue(_T("Settings"), "SPR_Chzbh", (modPHScal::gnSPR_CHZBH));
				break;
			}
		}
   
		iCount = 1;
		while(!f.eof())
		{
			iFlag=0;
			do
			{
				f.getline(temp.GetBuffer(255),255);
				temp.ReleaseBuffer();
				//如果遇到原始数据结束标志则退出循环
				if( Trim(Mid(temp,24)).Find(strEND_RawData)==0) break;
				iTemp =atoi(Mid(temp,24,4));
				if( iTemp == iNCtj ) 
				{
					TjTemp = atof(Mid(temp, 51, 10));
				    sign=true;
				}
				if( iTemp == iNCdw ) sngDWTemp = atof(Mid(temp,35, 10));
				//如果是节点记录(NC=10)
				if( (iTemp == iNCdw)  ) 
				{
					sngDWTemp = atof(Mid(temp,35, 10));
					if( iFlag != 0 ) 
						sngDW.SetAtGrow(iCount-1,sngDWTemp);
				}
				if( iTemp == 10 )
				{
                   if(!sign)//chengbailian 2003.11.6
					{//如果10记录出现而9(介质特性）记录还没有出现，先找到90记录读取介质温度，在PSA计算出的文件没有这种情况，而王总的软件计算出的有这种情况
						pos=f.tellg();
						while(!f.eof())
						{
							f.getline(tempsign.GetBuffer(255),255);
						    tempsign.ReleaseBuffer();
							if(atoi(Mid(tempsign, 24, 4))==iNCtj)
							{
								TjTemp = atof(Mid(tempsign, 51, 10));
								break;
							}

						}
						f.seekg(pos);//指针回到原来位置
					}

					//计算点始节点号
					iJSJDH_start = atoi(Mid(temp, 26, 8));
					//计算点末节点号
					iJSJDH_end = atoi(Mid(temp,35 ,8));
					if( iFlag == 0 )
					{
						//第一个节点记录只要简单地保存始末节点
						iJSJDH.SetAtGrow(iCount,iJSJDH_start);
						sngDW.SetAtGrow(iCount,sngDWTemp);
						Tj.SetAtGrow(iCount,TjTemp);
						iCount++;
               
                			pos=f.tellg();
			                int iFlg = 0;
							while(!f.eof() && iFlg==0)
							{
								f.getline(tempsign.GetBuffer(255),255);
								tempsign.ReleaseBuffer();
								if (atoi(Mid(tempsign, 24, 4))==10)
								{
									break;
								}
								if(atoi(Mid(tempsign, 24, 4))==iNCtj)
								{
                                    long Ppos;
                                    float fTjTemp = atof(Mid(tempsign, 51, 10));
									while (!f.eof() && iFlg==0)
									{
                                        Ppos = f.tellg();
								        f.getline(tempsign.GetBuffer(255),255);
								        tempsign.ReleaseBuffer();
										int ipipetype = atoi(Mid(tempsign, 24, 4));
										if (ipipetype == iNCtj)
										{
											f.seekg(Ppos);//指针回到原来位置
											break;
										}
        								if (ipipetype == 10)
										{
											iFlg = 1;
									        break;
										}
										if (ipipetype==0 || ipipetype==1 || ipipetype==2 ||ipipetype==3 ||
											ipipetype==4 || ipipetype==5 || ipipetype==6)
										{
											TjTemp = fTjTemp;
										}
									}
								}
							}
							f.seekg(pos);//指针回到原来位置

						iJSJDH.SetAtGrow(iCount,iJSJDH_end);
						sngDW.SetAtGrow(iCount,sngDWTemp);
						Tj.SetAtGrow(iCount,TjTemp);
						iCount++;
					}
					else
					{
						for( i = 1 ;i<iCount;i++)
						{
							if( iJSJDH_end == iJSJDH[i] ) break;
						}
						if( i >= iCount )
						{
                			pos=f.tellg();
			                int iFlg = 0;
							while(!f.eof() && iFlg==0)
							{
								f.getline(tempsign.GetBuffer(255),255);
								tempsign.ReleaseBuffer();
								if (atoi(Mid(tempsign, 24, 4))==10)
								{
									break;
								}
								if(atoi(Mid(tempsign, 24, 4))==iNCtj)
								{
                                    long Ppos;
                                    float fTjTemp = atof(Mid(tempsign, 51, 10));
									while (!f.eof() && iFlg==0)
									{
                                        Ppos = f.tellg();
								        f.getline(tempsign.GetBuffer(255),255);
								        tempsign.ReleaseBuffer();
										int ipipetype = atoi(Mid(tempsign, 24, 4));
										if (ipipetype == iNCtj)
										{
											f.seekg(Ppos);//指针回到原来位置
											break;
										}
        								if (ipipetype == 10)
										{
											iFlg = 1;
									        break;
										}
										if (ipipetype==0 || ipipetype==1 || ipipetype==2 ||ipipetype==3 ||
											ipipetype==4 || ipipetype==5 || ipipetype==6)
										{
											TjTemp = fTjTemp;
										}
									}
								}
							}
							f.seekg(pos);//指针回到原来位置
							
							iJSJDH.SetAtGrow(iCount,iJSJDH_end);
							sngDW.SetAtGrow(iCount,sngDWTemp);
							Tj.SetAtGrow(iCount,TjTemp);
							iCount++;
						}
					}
					//设置标志区分是否是第一个节点记录
					if( iFlag == 0 ) iFlag = 1;
				}
			}while(Trim(Mid(temp,24)).Find(strEND_RawData)!=0 && !f.eof());
			if( Trim(Mid(temp,24)).Find(strEND_RawData)==0) break;
		}
   
		//GLIFV1.2:查找结果数据头部
		while(!f.eof())
		{
			 f.getline(strTemp.GetBuffer(255),255);
			 strTemp.ReleaseBuffer();
			 if( strTemp.Find(strSUMMARY_FOR_RESTRAINTS) !=-1 ) //找到“SUMMARY FOR RESTRAINTS”
					break;
		}
                  
		rsData = rsResult;
   
		//GLIFV1.2:开始读结果数据
		while(!f.eof())
		{
			 f.getline(temp.GetBuffer(255),255);
			 temp.ReleaseBuffer();
			 if( atoi(Mid(temp,4, 3)) > 0 ){
				  iPreINT[1] = atoi(Mid(temp,0, 7));
				  iPreINT[2] = atoi(Mid(temp, 7, 11));
				  for( i = 1 ;i<= 6;i++)  //每个节点6行数据
				  {
						for (j = 1 ;j<= 8 ;j++)//每行最多8个数据,依次为paz,pgz,pjg,psl,psr,dl,dr,h(uy,预压缩值)
						{
							phsVAL[i][ j] = atof(Mid(temp,19 + (j - 1) * 13-1, 13));
							if( iPreINT[2] == 177 && j == 8 ){
								//由于GLIFV1.2软件BUG,节点类型177结构荷载写到最后一列了,改正此错误.
								phsVAL[i][3] = atof(Mid(temp, 74 + (j - 4) * 13-1, 13));
							}
						}
						f.getline(temp.GetBuffer(255),255);
						temp.ReleaseBuffer();  //跳过1空行
				  }				  
				  rsData->Filter=_variant_t(CString("[jsdbh1] = ") + ltos(iPreINT[1])+" AND "+CString("[FileNameID]=")+ltos(FileNameID));
				  //Set rs = rsData.OpenRecordset()
					
					if( rsData->BOF && rsData->adoEOF ){
						 rsData->AddNew();
						 rsData->PutCollect("UPxyz",_variant_t((long)giUPxyz));
						 rsData->PutCollect("VolumeID",_variant_t(EDIBgbl::SelVlmID));
						 rsData->PutCollect("Zdjh",_variant_t((long)(ltemp+1)));
						 rsData->PutCollect("FileNameID",_variant_t(FileNameID));
						 //rsData->PutCollect("Zdjh",_variant_t((long)iPreINT[1]));
						 rsData->Update();
						 ltemp++;
					}
					for(i = 1 ;i< iCount;i++)
					{
						//确定i值
						if( iJSJDH[i] == iPreINT[1] )break;
						//Debug.Print i, iPreINT(1),iJSJDH(i), sngDW(i), Tj(i)
					}
					//if( IsNull(!PSAver) Or (Not IsNull(!PSAver) And Trim(!PSAver) = "") Or (Not IsNull(!PSAver) And Trim(!PSAver) <> "" And UCase(Trim(!PSAver)) = strPSAVER) ){
					rsData->PutCollect("PSAver",strPSAVER);
					rsData->PutCollect("JSDBH1",_variant_t((long)iPreINT[1]));
					rsData->PutCollect("dj1",_variant_t((float)sngDW[i]));


					//chengbailian
					 POSITION pos=mElevationList.GetHeadPosition();
					 while(pos !=NULL)
					 {
					     //查找当前支吊点的标高	
						 pNodeUpElevation = mElevationList.GetNext(pos);

						 if(pNodeUpElevation->NodeNum == iPreINT[1])
						 {
							 sngDH1.SetAtGrow(i,pNodeUpElevation->UpElevation); 
							 sngDH1[i]=int(sngDH1[i]*1000+0.5)/1000.0; //精确三位小数
							 break;
						 }
						 
					 }

					 if(vtof(rsData->GetCollect("dh1"))==0.0)//如果原来有则不履盖
						rsData->PutCollect("dh1", _variant_t(sngDH1[i]));
                     //end cbl


					rsData->PutCollect("t01",_variant_t((float)Tj[i]));
					rsData->PutCollect("Type",_variant_t((long)iPreINT[2]));
					rsData->PutCollect("psaType",_variant_t((long)iPreINT[2]));
					if( EDIBgbl::PSA_OutDataUnit == "NATIONAL" ){
						//以下3个数据依次为paz1,pgz1,pjg1
						//phsVAL(2, 1) = Format(phsVAL(2, 1) / kgf2N, "0.0")
						//phsVAL(2, 2) = Format(phsVAL(2, 2) / kgf2N, "0.0")
						//phsVAL(2, 3) = Format(phsVAL(2, 3) / kgf2N, "0.0")
             
						//以下4个数据依次为x.z方向推力(冷/热)
						//phsVAL(1, 1) = Format(phsVAL(1, 1) / kgf2N, "0.0")
						//phsVAL(3, 1) = Format(phsVAL(3, 1) / kgf2N, "0.0")
						//phsVAL(1, 2) = Format(phsVAL(1, 2) / kgf2N, "0.0")
						//phsVAL(3, 2) = Format(phsVAL(3, 2) / kgf2N, "0.0")
               
						//以下6个数据依次为x.y.z方向力矩(冷/热)
						//phsVAL(4, 1) = Format(phsVAL(4, 1) / kgf2N, "0.0")
						//phsVAL(5, 1) = Format(phsVAL(5, 1) / kgf2N, "0.0")
						//phsVAL(6, 1) = Format(phsVAL(6, 1) / kgf2N, "0.0")
						//phsVAL(4, 2) = Format(phsVAL(4, 2) / kgf2N, "0.0")
						//phsVAL(5, 2) = Format(phsVAL(5, 2) / kgf2N, "0.0")
						//phsVAL(6, 2) = Format(phsVAL(6, 2) / kgf2N, "0.0")
						rsData->PutCollect("Unit_Force", "N");
						rsData->PutCollect("Unit_Momental", "N.m");
					 }else{
						rsData->PutCollect("Unit_Force", "kgf");
						rsData->PutCollect("Unit_Momental", "kgf.m");
					 }
					 rsData->PutCollect("Unit_Displacement", "mm");
					rsData->PutCollect("paz0", _variant_t(phsVAL[2][1]));
					rsData->PutCollect("paz1", _variant_t(phsVAL[2][1]));
					rsData->PutCollect("pgz1", _variant_t(phsVAL[2][2]));
					rsData->PutCollect("pjg1", _variant_t(phsVAL[2][3]));
					rsData->PutCollect("pxl1", _variant_t(phsVAL[1][1]));
					rsData->PutCollect("pyl1", _variant_t(phsVAL[2][1]));
					rsData->PutCollect("pzl1", _variant_t(phsVAL[3][1]));
					rsData->PutCollect("pxr1", _variant_t(phsVAL[1][2]));
					rsData->PutCollect("pyr1", _variant_t(phsVAL[2][2]));
					rsData->PutCollect("pzr1", _variant_t(phsVAL[3][2]));
					rsData->PutCollect("xl1", _variant_t(phsVAL[1][6] * EDIBgbl::cm2mm));
					rsData->PutCollect("yl1", _variant_t(phsVAL[2][6] * EDIBgbl::cm2mm));
					rsData->PutCollect("zl1", _variant_t(phsVAL[3][6] * EDIBgbl::cm2mm));
					rsData->PutCollect("xr1", _variant_t(phsVAL[1][7] * EDIBgbl::cm2mm));
					rsData->PutCollect("yr1", _variant_t(phsVAL[2][7] * EDIBgbl::cm2mm));
					rsData->PutCollect("zr1", _variant_t(phsVAL[3][7] * EDIBgbl::cm2mm));
					rsData->PutCollect("haz", _variant_t(phsVAL[1][8] * EDIBgbl::cm2mm));
					rsData->PutCollect("mxl1", _variant_t(phsVAL[4][1]));
					rsData->PutCollect("myl1", _variant_t(phsVAL[5][1]));
					rsData->PutCollect("mzl1", _variant_t(phsVAL[6][1]));
					rsData->PutCollect("mxr1", _variant_t(phsVAL[4][2]));
					rsData->PutCollect("myr1", _variant_t(phsVAL[5][2]));
					rsData->PutCollect("mzr1", _variant_t(phsVAL[6][2]));
					
				  rsData->Filter=_variant_t((long)adFilterNone);
				  iC1++;
			 }
			 else
			 {
				if( temp.Find(strCOLD_SPRING_TABLE) !=-1 )
					 break;
			 }
			 
			 if( iCount <= 0 ) iCount = 1;
			 //MsgBox iC1 & ":" & iCount
			 frmStatus.UpdateStatus((float)iC1/(float)iCount,true);
			 frmStatus.UpdateWindow();
			 //frmStatus.picstatus.Refresh
			 //frmStatus.Refresh
		}
		f.close();

		//释放内存 cbl
		POSITION posList=mElevationList.GetHeadPosition();
		POSITION preposList;
		CNodeUpElevation *pNodeUpElevationData;
		while(posList !=NULL)
		{
			preposList=posList;
			pNodeUpElevationData = mElevationList.GetNext(posList);
            mElevationList.RemoveAt(preposList);
			delete pNodeUpElevationData;
		}
		 

        if(!rsData->adoEOF || !rsData->BOF)
			rsData->MoveFirst();
		_variant_t vtmp;
		while(!rsData->adoEOF)
		{
			vtmp=rsData->GetCollect("ZDJH");
			rsData->PutCollect("ZDJH",_variant_t((long)(vtoi(vtmp)+1000)));
			rsData->MoveNext();
		}
		_ConnectionPtr pCon;
		strSQL=(char*)(bstr_t)rsData->GetSource();
		pCon=rsData->GetActiveConnection();
		strSQL.Replace("ORDER BY JSDBH1","ORDER BY FileNameID");
		rsData->Close();
		rsData->Open(_variant_t(strSQL),(IDispatch*)pCon,adOpenKeyset,adLockOptimistic,adCmdText);
		//rsData->Sort=_bstr_t("JSDBH1");
		int itmpZdjh=1;
		while(!rsData->adoEOF)
		{
			rsData->PutCollect("ZDJH",_variant_t((long)itmpZdjh));
			itmpZdjh++;
			rsData->MoveNext();
		}
	}
	catch(_com_error e)
	{
		ShowMessage(e.Description());
	}
	catch(CException *e)
	{
		e->Delete();
	}
	catch(...)
	{
	}
}




void ReadResult_GLIF31(_Recordset* rsResult, CString SourceDataFileName,long maxZdjh,long FileNameID)
{
	  //Remauther:
		//作者:
		//目的:本程序为读*.out文件，可用最后的msgbox语句来测试程序的正确性
		//输入:rsResult数据表内部单位:位移mm,荷载kgf.
		//返回:
   try
	{
		_variant_t nil;
		nil.ChangeType(VT_NULL);
		 CString strOldPhsSPRINGSel; //保存读取应力计算数据之前的弹簧标准
		 float mvMultiple=0;
		 CString sFx, sFx1;
		 CString strTemp;
		 CString temp;
		 int iTemp=0;
		 float phsVAL[7][9]; //此数组代表后面的单精度数据，X维代表整型数据后面的列，
												 //Y维代表行
		 int iPreINT[3]; //此数组代表记录前面的两个整型数
		 _Recordset* rsData;

		LPCTSTR strPIPING_STRESS_OUTUNIT = "++++";   //GLIFV3.1结果数据文件中第一个含"++++"的行是单位控制记录,其中第一个数据IIN是输入单位控制,第二个数据IOU是输出单位控制(IOU=0法定单位制,IOU=1工程制)
		LPCTSTR strSTRUCTURE_LOAD_OF_RESTRAINTS = "STRUCTURE LOAD OF RESTRAINTS";
		LPCTSTR strREACTION_ON_ANCHORS_ColdInitial_CASE4 = "REACTION OF PINING ON ANCHORS (COLD-INITIAL)";
		LPCTSTR strREACTION_ON_ANCHORS_ColdRelease_CASE3 = "REACTION OF PIPING ON ANCHORS (COLD-RELEASE)";
		LPCTSTR strREACTION_ON_ANCHORS_WorkInitial_CASE2 = "REACTION OF PINING ON ANCHORS (WORK-INITIAL)";
		LPCTSTR strREACTION_ON_ANCHORS_Cold_CASE1 = "REACTION OF PINING ON ANCHORS";
		LPCTSTR strCOLD_SPRING_TABLE = "COLD SPRING JOINT POINT";
		LPCTSTR strEND_RawData = "10         1         1         1" ; //空格个数与GLIFV1.2不同
		LPCTSTR strPSAVER = "GLIF V31";
   
		LPCTSTR strCW_DISPLACEMENT = "CW-DISPLACEMENT (cold/work-status)";
   
		LPCTSTR strEND_THIS_PIPING = "*THIS PIPING STRUCTURE *CONTAINS*";
   
		CString strSprTbn;
		_RecordsetPtr tmprs;
		tmprs.CreateInstance(__uuidof(Recordset));
		_variant_t varTmp;
		CString mvsCASE, mvsCASEcool;
		int mviSPR=0;
		long i=0;
		//在显示输入框之前隐藏进度框
		frmStatus.ShowWindow(SW_HIDE);
		CInputBox dlg;
		while(sFx == "" || (sFx.GetAt(0) < '0' || sFx.GetAt(0) > '9')  || (sFx != "1" && sFx != "3" && sFx != "4"))
		{
			dlg.m_strTitle=GetResStr(IDS_SelThrustQuaGist);
			dlg.m_strWndText=GetResStr(IDS_SelWorkCon);
			dlg.m_strValue="4";
			dlg.DoModal();
			sFx =dlg.m_strValue;
			sFx.TrimLeft();
			sFx.TrimRight();
		}
		i = atoi(sFx);
		if( i == 1 ){
			mvsCASEcool = strREACTION_ON_ANCHORS_Cold_CASE1;
		}else{
			if( i == 3 ){
				mvsCASEcool = strREACTION_ON_ANCHORS_ColdRelease_CASE3;
			}else{
				mvsCASEcool = strREACTION_ON_ANCHORS_ColdInitial_CASE4;
			}
		}
		frmStatus.ShowWindow(SW_SHOW);
   
		//截面特性记录:GLIF1.2,NC=6;GLIF3.x,NC=60(弯单元)及61(直单元),成对使用.
		//介质特性记录:GLIF1.2,NC=9;GLIF3.x,NC=90(设计工况).
		const int iNCtj = 90, iNCdw1 = 61, iNCdw = 60;
		//Const iNCtj = 9, iNCdw = 6
   
		float sngDWTemp=0, TjTemp=0;
		long iJSJDH_start=0, iJSJDH_end=0;
		CArray<long,long & > iJSJDH;
		CArray<float,float & > sngDW;
		CArray<float ,float & > Tj;
		CArray<float,float &> sngDH1;

		sngDH1.SetSize(100);
		iJSJDH.SetSize(100);
		sngDW.SetSize(100);
		Tj.SetSize(100);
   
		int j=0, iC1=0;
		ifstream f;
		int iFlag=0, iCount=0;
		CString mvS,tempsign;
		bool bCwDisplacement=false, bStrucLoad=false;
        bool sign=false;//chengbailian 2003.11.6 为了适应另外一种文件，90记录是否在10记录之录出现		
		long pos;

		//cbl 2004.1.10 增加了每个节点的标高
		ElevationList mElevationList;
		CNodeUpElevation *pNodeUpElevation;
		int ICC;
		
		
		//坐标文件名
		CString ElevationFileName=SourceDataFileName.Left(SourceDataFileName.ReverseFind('.'))+".et";//标高文件名

		//生成坐标文件(记录了每个节点的坐标)(链接库中的函数)
		ICC=PSAPointCoordinateWirteFile(SourceDataFileName,ElevationFileName);
	  
		//从文件中读取每个点的标高放入链表mElevationList中
		ElevationFileToList(ElevationFileName,&mElevationList,ICC);
        ::DeleteFile(ElevationFileName); 
	//cbl

		f.open(SourceDataFileName);

		CString strSQL;
		//开始读原始数据
		while(!f.eof())
		{			
			f.getline(temp.GetBuffer(255),255); temp.ReleaseBuffer();
			if( temp.Find(strPIPING_STRESS_OUTUNIT)!=-1){
				//第一个记录第一个字段是输入单位制，第2个字段是输出单位制(=0，法定单位制)，第3个字段是坐标轴方向
				if( atoi(Trim(Mid(temp,16, 4))) == 0 ){
					EDIBgbl::PSA_OutDataUnit = "NATIONAL";
				}else{
					EDIBgbl::PSA_OutDataUnit = "ENGINEERING";
				}
				//坐标轴方向x/y/z轴向上=1/2/3
				giUPxyz = atoi(Mid(temp,20, 12));
				f.getline(temp.GetBuffer(255),255); temp.ReleaseBuffer();
				//GLIFV3.x中,原始数据文件第2个记录第1个数据为弹簧种类,第3个数据为弹簧荷载变化系数
				mviSPR = atoi(Trim(Mid(temp, 6, 10)));
				modPHScal::gnSPR_CHZBH = atof(Trim(Mid(temp, 21, 10)));
				strOldPhsSPRINGSel = modPHScal::gsPhsSPRINGSel;
				//因为应力计算选择的弹簧标准可能不同,故重新更新规范
				//由于生成规范花费时间较长，所以只有当规范变化时，才需要重新更新
				if( mviSPR == 3 ){
					strSQL = "SELECT Spring_property FROM phsManuSPRING WHERE StandardDesc=\'";
					strSQL += strSprNWEPDI;
					strSQL += "\'";
					tmprs->Open((_bstr_t)strSQL, _variant_t((IDispatch*)EDIBgbl::dbSORT,true), 
						adOpenKeyset, adLockOptimistic, adCmdText); 
					if(tmprs->BOF && tmprs->adoEOF)
					{
					}
					else
					{
						tmprs->get_Collect((_variant_t)0L, &varTmp);
						strSprTbn=vtos(varTmp);
					}
					tmprs->Close();
					if( strOldPhsSPRINGSel != strSprNWEPDI ){
						modPHScal::gsPhsSPRINGSel = strSprNWEPDI;
						//if( blnSelphsSPEC(False) ) SaveRecentWorkSPEC();
					}
				}else if( mviSPR == 4 ){
					strSQL = "SELECT Spring_property FROM phsManuSPRING WHERE StandardDesc=\'";
					strSQL += strSprGB10182;
					strSQL += "\'";
					tmprs->Open((_bstr_t)strSQL, _variant_t((IDispatch*)EDIBgbl::dbSORT,true), 
						adOpenKeyset, adLockOptimistic, adCmdText); 
					if(tmprs->BOF && tmprs->adoEOF)
					{
					}
					else
					{
						tmprs->get_Collect((_variant_t)0L, &varTmp);
						strSprTbn=vtos(varTmp);
					}
					tmprs->Close();
					if( strOldPhsSPRINGSel != strSprGB10182 ){
						modPHScal::gsPhsSPRINGSel = strSprGB10182;
						//if( blnSelphsSPEC(False) ) SaveRecentWorkSPEC();
					}
				}
         
				//荷载变化系数保存到外部
				SetRegValue(_T("Settings"), "SPR_Chzbh", (modPHScal::gnSPR_CHZBH));
				break;
			}
		}
   
		iCount = 1;
		while(!f.eof()){
			iFlag=0;
			do
			{
				f.getline(temp.GetBuffer(255),255); temp.ReleaseBuffer();
				//如果遇到原始数据结束标志则退出循环
				if( Trim(Mid(temp,18)).Find( strEND_RawData)==0) break;
				iTemp = atoi(Mid(temp, 16, 4));
				//如果是节点记录(NC=10)
				if( iTemp == iNCtj ) 
				{
					TjTemp = atof(Mid(temp, 32, 7));
					sign=true;
				}
				if( (iTemp == iNCdw)  ) sngDWTemp = atof(Mid(temp, 22, 7));
				if( (iTemp == iNCdw)  ) 
				{
					sngDWTemp = atof(Mid(temp, 22, 7));
					if( iFlag != 0 ) 
						sngDW.SetAtGrow(iCount-1,sngDWTemp);
				}

				if( iTemp == 10 )
				{
					if(!sign)//chengbailian 2003.11.6
					{//如果10记录出现而90记录还没有出现，先找到90记录读取介质温度，在PSA计算出的文件没有这种情况，而王总的软件计算出的有这种情况
						pos=f.tellg();
						while(!f.eof())
						{
							f.getline(tempsign.GetBuffer(255),255);
						    tempsign.ReleaseBuffer();
							if(atoi(Mid(tempsign, 16, 4))==iNCtj)
							{
								TjTemp = atof(Mid(tempsign, 32, 7));
								break;
							}

						}
						f.seekg(pos);//指针回到原来位置
					}

					//计算点始节点号
					iJSJDH_start = atoi(Mid(temp, 21, 10));
					//计算点末节点号
					iJSJDH_end = atoi(Mid(temp, 31, 10));
					if( iFlag == 0 ){
						//第一个节点记录只要简单地保存始末节点
						iJSJDH.SetAtGrow(iCount,iJSJDH_start);
						sngDW.SetAtGrow(iCount,sngDWTemp);
						Tj.SetAtGrow(iCount,TjTemp);
						iCount++;
						
                			pos=f.tellg();
			                int iFlg = 0;
							while(!f.eof() && iFlg==0)
							{
								f.getline(tempsign.GetBuffer(255),255);
								tempsign.ReleaseBuffer();
								if (atoi(Mid(tempsign, 16, 4))==10)
								{
									break;
								}
								if(atoi(Mid(tempsign, 16, 4))==iNCtj)
								{
                                    long Ppos;
                                    float fTjTemp = atof(Mid(tempsign, 32, 7));
									while (!f.eof() && iFlg==0)
									{
                                        Ppos = f.tellg();
								        f.getline(tempsign.GetBuffer(255),255);
								        tempsign.ReleaseBuffer();
										int ipipetype = atoi(Mid(tempsign, 16, 4));
										if (ipipetype == iNCtj)
										{
											f.seekg(Ppos);//指针回到原来位置
											break;
										}
        								if (ipipetype == 10)
										{
											iFlg = 1;
									        break;
										}
										if (ipipetype==0 || ipipetype==1 || ipipetype==2 ||ipipetype==3 ||
											ipipetype==4 || ipipetype==5 || ipipetype==6)
										{
											TjTemp = fTjTemp;
										}
									}
								}
							}
							f.seekg(pos);//指针回到原来位置
               
						iJSJDH.SetAtGrow(iCount,iJSJDH_end);
						sngDW.SetAtGrow(iCount,sngDWTemp);
						Tj.SetAtGrow(iCount,TjTemp);
						iCount++;
					}else{
						for( i = 1 ;i<iCount;i++)
						{
							if( iJSJDH_end == iJSJDH[i] ) break;
						}
						if( i >= iCount ){

                			pos=f.tellg();
			                int iFlg = 0;
							while(!f.eof() && iFlg==0)
							{
								f.getline(tempsign.GetBuffer(255),255);
								tempsign.ReleaseBuffer();
								if (atoi(Mid(tempsign, 16, 4))==10)
								{
									break;
								}
								if(atoi(Mid(tempsign, 16, 4))==iNCtj)
								{
                                    long Ppos;
                                    float fTjTemp = atof(Mid(tempsign, 32, 7));
									while (!f.eof() && iFlg==0)
									{
                                        Ppos = f.tellg();
								        f.getline(tempsign.GetBuffer(255),255);
								        tempsign.ReleaseBuffer();
										int ipipetype = atoi(Mid(tempsign, 16, 4));
										if (ipipetype == iNCtj)
										{
											f.seekg(Ppos);//指针回到原来位置
											break;
										}
        								if (ipipetype == 10)
										{
											iFlg = 1;
									        break;
										}
										if (ipipetype==0 || ipipetype==1 || ipipetype==2 ||ipipetype==3 ||
											ipipetype==4 || ipipetype==5 || ipipetype==6)
										{
											TjTemp = fTjTemp;
										}
									}
								}
							}
							f.seekg(pos);//指针回到原来位置

							iJSJDH.SetAtGrow(iCount,iJSJDH_end);
							sngDW.SetAtGrow(iCount,sngDWTemp);
							Tj.SetAtGrow(iCount,TjTemp);
							iCount++;
						}
					}
					//设置标志区分是否是第一个节点记录
					if( iFlag == 0 ) iFlag = 1;
				}
			}while(Trim(Mid(temp, 18)).Find(strEND_RawData)!=0 && !f.eof());
			if( Trim(Mid(temp, 18)).Find(strEND_RawData)==0) break;
		}
      long ltemp=maxZdjh ;  
		rsData=rsResult;
   
		//管道端点初热数据在文件前部
		//GLIFV3.x:开始查找管道端点初热荷载数据头部
		/*mvsCASE = strREACTION_ON_ANCHORS_WorkInitial_CASE2;
		while(!f.eof()){
			f.getline(strTemp.GetBuffer(255),255); strTemp.ReleaseBuffer();

			if(strTemp.Find(mvsCASE) !=-1){
				break;
			}
		}            
		//rsResult.Requery
		//GLIFV3.x:开始读管道端点初热荷载数据
		while(!f.eof()){
			f.getline(temp.GetBuffer(255),255); temp.ReleaseBuffer();
			if( atof(Mid(temp, 0, 16)) > 0 ){
				iPreINT[1] = atoi(Mid(temp, 0, 16));   //节点号
				iPreINT[2] = atoi(Mid(temp, 16, 10));  //节点类型
				//此处只能是固定架类型
				iPreINT[2] = 100 ; //or 177
				for (j = 1 ;j<= 6;j++) //每行最多6个数据,依次为Fx,Fy,Fz,Mx,My,Mz
					phsVAL[j][1] = atof(Mid(temp, 26 + (j - 1) * 15-1, 15));
				

				rsData->Filter = _variant_t(CString("[jsdbh1] = ") + ltos(iPreINT[1]));
					 if(rsData->BOF && rsData->adoEOF ){
						  rsData->AddNew();
						  rsData->PutCollect("UPxyz",_variant_t((long)giUPxyz));
						  rsData->PutCollect("VolumeID",_variant_t(EDIBgbl::SelVlmID));
						  rsData->PutCollect("Zdjh",_variant_t((long)(ltemp+1)));
						 //rsData->PutCollect("Zdjh",_variant_t((long)iPreINT[1]));
						   rsData->Update();
						  ltemp++;
					 }else{
					 }
					 for (i = 1 ;i< iCount;i++)
					 {
						 if( iJSJDH[i] == iPreINT[1] )break;
					 }
					 rsData->PutCollect("PSAver", _variant_t(strPSAVER));
					 rsData->PutCollect("JSDBH1", _variant_t((long)iPreINT[1]));
					 rsData->PutCollect("Type", _variant_t((long)iPreINT[2]));
					 if( EDIBgbl::PSA_OutDataUnit== "NATIONAL" ){
						rsData->PutCollect("Unit_Force", "N");
						rsData->PutCollect("Unit_Momental", "N.m");
					 }else{
						rsData->PutCollect("Unit_Force", "kgf");
						rsData->PutCollect("Unit_Momental", "kgf.m");
					 }
					 rsData->PutCollect("Unit_Displacement", "mm");
					 rsData->PutCollect("pxr1", _variant_t(phsVAL[1][1]));
					 rsData->PutCollect("pyr1", _variant_t(phsVAL[2][1]));
					 rsData->PutCollect("pgz1", _variant_t(phsVAL[giUPxyz][1]));
					 rsData->PutCollect("pjg1", _variant_t(phsVAL[giUPxyz][1]));
					 rsData->PutCollect("pzr1", _variant_t(phsVAL[3][1]));
					 rsData->PutCollect("mxr1", _variant_t(phsVAL[4][1]));
					 rsData->PutCollect("myr1", _variant_t(phsVAL[5][1]));
					 rsData->PutCollect("mzr1", _variant_t(phsVAL[6][1]));
					 rsData->Filter=_variant_t((long)adFilterNone);
				iC1++;
			}else{
				if( temp.Find(mvsCASE) == -1 ){
					break;
				}
			}
		}
		//结束初热工况4管道端点荷载处理
		f.close();
   
		//管道端点工况1冷态数据在文件前部
		//管道端点工况3/4冷态数据在工况2后
		//GLIFV3.x:开始查找管道端点冷态荷载数据头部
		f.open(SourceDataFileName);
		while(!f.eof()){
			f.getline(strTemp.GetBuffer(255),255); strTemp.ReleaseBuffer();
			if( strTemp.Find(mvsCASEcool) !=-1){ //找到管道端点冷态荷载数据头部
				break;
			}
		}
                     
		//rsResult.Requery
		//Set rsData = rsResult
   
		//GLIFV3.x:开始读管道端点冷态荷载数据
		while(!f.eof()){
			f.getline(temp.GetBuffer(255),255); temp.ReleaseBuffer();
			
			if( atof(Mid(temp, 0, 16)) > 0 ){
				iPreINT[1] = atoi(Mid(temp, 0, 16))  ; //节点号
				iPreINT[2] = atoi(Mid(temp, 16, 10)) ; //节点类型
				//此处只能是固定架类型
				iPreINT[2] = 100 ; //or 177
				for( j = 1 ;j<= 6;j++) //每行最多6个数据,依次为Fx,Fy,Fz,Mx,My,Mz
					phsVAL[j][1] = atof(Mid(temp, 26 + (j - 1) * 15-1, 15));
				rsData->Filter = _variant_t(CString("[jsdbh1] = ") + ltos(iPreINT[1]));
				
					 if( rsData->BOF && rsData->adoEOF ){
						  rsData->AddNew();
						  rsData->PutCollect("UPxyz",_variant_t((long)giUPxyz));
						  rsData->PutCollect("VolumeID",_variant_t(EDIBgbl::SelVlmID));
						  rsData->PutCollect("Zdjh",_variant_t((long)(ltemp+1)));
						 //rsData->PutCollect("Zdjh",_variant_t((long)iPreINT[1]));
						   rsData->Update();
						  ltemp++;
					 }else{
					 }
					 for(i = 1;i<iCount;i++)
					 {
						 if( iJSJDH[i] == iPreINT[1])
							 break;
					 }
					 rsData->PutCollect("PSAver",_variant_t(strPSAVER));
					 rsData->PutCollect("JSDBH1",_variant_t((long)iPreINT[1]));
					 rsData->PutCollect("Type",_variant_t((long)iPreINT[2]));
					 if( EDIBgbl::PSA_OutDataUnit == "NATIONAL" ){
						rsData->PutCollect("Unit_Force", "N");
						rsData->PutCollect("Unit_Momental", "N.m");
					 }else{
						rsData->PutCollect("Unit_Force", "kgf");
						rsData->PutCollect("Unit_Momental", "kgf.m");
					 }
					 rsData->PutCollect("Unit_Displacement", "mm");
					 rsData->PutCollect("pxl1", _variant_t(phsVAL[1][1]));
					 rsData->PutCollect("pyl1", _variant_t(phsVAL[2][1]));
					 rsData->PutCollect("paz0", _variant_t(phsVAL[giUPxyz][1]));
					 rsData->PutCollect("paz1", _variant_t(phsVAL[giUPxyz][1]));
					 rsData->PutCollect("pzl1", _variant_t(phsVAL[3][1]));
					 rsData->PutCollect("mxl1", _variant_t(phsVAL[4][1]));
					 rsData->PutCollect("myl1", _variant_t(phsVAL[5][1]));
					 rsData->PutCollect("mzl1", _variant_t(phsVAL[6][1]));
					 rsData->Filter=_variant_t((long)adFilterNone);
				//管道端点冷态荷载必须与热态荷载在同一个记录，所以记录号不能递增。
				//iC1 = iC1 + 1
			}else{
				if( temp.Find(mvsCASEcool)==-1 ){
					break;
				}
			}
		}
		//结束管道端点冷态荷载处理*/
		f.close();
   
		f.open(SourceDataFileName);
		//GLIFV3.x:开始查找位移数据头部
		while(!f.eof()){
			f.getline(strTemp.GetBuffer(255),255); strTemp.ReleaseBuffer();
				if(strTemp.Find(strCW_DISPLACEMENT) !=-1 ) //找到"CW-DISPLACEMENT (cold/work-status)"
					break;
		}
   
		//找到"CW-DISPLACEMENT (cold/work-status)"
		//GLIFV3.x:开始读位移数据
		while(!f.eof()){
			f.getline(temp.GetBuffer(255),255); temp.ReleaseBuffer();
			//湖南电力院1999年5月前使用的GLIFV3.x版本，iPreINT(1)的字符宽度从第1-10位
			//贵州电力院2001年5月前使用的GLIFV3.1版本，iPreINT(1)的字符宽度从第1-5位，
			//iPreINT(2)的字符宽度从第6-10位，
			if( atof(Mid(temp, 0, 10)) >0)
			{
				if( gbNewGLIFV31 ){
					iPreINT[1] = atoi(Mid(temp,0, 5)) ;     //节点号
					iPreINT[2] = atoi(Mid(temp, 5, 7)) ;     //节点类型
				}else{
					iPreINT[1] = atoi(Mid(temp, 0, 10));     //节点号
					iPreINT[2] = atoi(Mid(temp, 10, 10));    //节点类型
				}
				for(j = 1;j<= 6;j++) //每行最多6个数据,依次为dcx,dcy,dcz,dwx,dwy,dwz,SP-Set,SP-Press
				{
					if( gbNewGLIFV31 ){
						phsVAL[j][1] = atof(Mid(temp, (j <= 3 ? 13 : 35 - 3 * 7) + (j - 1) * 7-1, 7));
					}else{
						phsVAL[j][1] =atof(Mid(temp, (j <= 3 ? 51 : 83 - 4 * 7) + (j - 1) * 7 -1, 7));
					}
				}
				rsData->Filter = _variant_t(CString("[jsdbh1] = ") + ltos(iPreINT[1])+" AND "+CString("[FileNameID]=")+ltos(FileNameID));
				//MsgBox rsData.RecordCount
					 if( rsData->BOF && rsData->adoEOF ){
						  rsData->AddNew();
						  rsData->PutCollect("UPxyz",_variant_t((long)giUPxyz));
						  rsData->PutCollect("VolumeID",_variant_t(EDIBgbl::SelVlmID));
						  rsData->PutCollect("Zdjh",_variant_t((long)(ltemp+1)));
						  rsData->PutCollect("FileNameID",_variant_t(FileNameID));
						 //rsData->PutCollect("Zdjh",_variant_t((long)iPreINT[1]));
						   rsData->Update();
						  ltemp++;
					 }else{
					 }
					 for(i = 1 ;i<iCount;i++)
					 {
						 //if( iJSJDH(i) = 85 ){ Debug.Print i,iPreINT(1), iJSJDH(i), sngDW(i), Tj(i)
						 if( iJSJDH[i] == iPreINT[1] ) break;
					 }
					 rsData->PutCollect("JSDBH1", _variant_t((long)iPreINT[1]));
					 rsData->PutCollect("dj1", _variant_t(sngDW[i]));
					
					 //chengbailian
					 POSITION pos=mElevationList.GetHeadPosition();
					 while(pos !=NULL)
					 {
					     //查找当前支吊点的标高	
						 pNodeUpElevation = mElevationList.GetNext(pos);

						 if(pNodeUpElevation->NodeNum == iPreINT[1])
						 {
							 sngDH1.SetAtGrow(i,pNodeUpElevation->UpElevation); 
							 sngDH1[i]=int(sngDH1[i]*1000+0.5)/1000.0; //精确三位小数
							 break;
						 }
						 
					 }

					 if(vtof(rsData->GetCollect("dh1"))==0.0)//如果原来有则不履盖
						rsData->PutCollect("dh1", _variant_t(sngDH1[i]));
                     //end cbl

					 rsData->PutCollect("t01", _variant_t(Tj[i]));
					 rsData->PutCollect("xl1", _variant_t(phsVAL[1][1]));
					 rsData->PutCollect("yl1", _variant_t(phsVAL[2][1]));
					 rsData->PutCollect("zl1", _variant_t(phsVAL[3][1]));
					 rsData->PutCollect("xr1", _variant_t(phsVAL[4][1]));
					 rsData->PutCollect("yr1", _variant_t(phsVAL[5][1]));
					 rsData->PutCollect("zr1", _variant_t(phsVAL[6][1]));
					 rsData->PutCollect("haz", nil);
					 rsData->Filter=_variant_t((long)adFilterNone);
				iC1++;
				frmStatus.UpdateStatus((float)iC1/(float)iCount,true);
				frmStatus.UpdateWindow();
			}else{
				if( temp.Find(strCW_DISPLACEMENT) ==-1 ){
					break;
				}
			}
			
			if( iCount <= 0 ) iCount = 1;
			//MsgBox iC1 & ":" & iCount
			frmStatus.UpdateStatus((float)iC1/(float)iCount,false);
			frmStatus.UpdateWindow();
			//frmStatus.picstatus.Refresh
			//frmStatus.Refresh
		}
		f.close();
   
		f.open(SourceDataFileName);
		//GLIFV3.x:查找荷载数据头
		while(!f.eof()){
			f.getline(strTemp.GetBuffer(255),255); strTemp.ReleaseBuffer();
			if( strTemp.Find(strSTRUCTURE_LOAD_OF_RESTRAINTS) !=-1 ){ //找到"STRUCTURE LOAD OF RESTRAINTS"
				break;
			}
		}
                     
		//找到"STRUCTURE LOAD OF RESTRAINTS"
		//GLIFV3.x:开始读荷载数据
		_RecordsetPtr rs1;
		rs1.CreateInstance(__uuidof(Recordset));
		strSQL.Format("SELECT G,SEQ FROM %s",strSprTbn);
		rs1->Open((_bstr_t)strSQL, _variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
			adOpenKeyset, adLockOptimistic, adCmdText); 
		while(!f.eof()){
			f.getline(temp.GetBuffer(255),255); temp.ReleaseBuffer();
			if( atof(Mid(temp,0, (gbNewGLIFV31 ? 5 : 16))) > 0 ){
				if( gbNewGLIFV31 ){
					iPreINT[1] = atoi(Mid(temp, 0, 5));      //节点号
					iPreINT[2] = atoi(Mid(temp, 5, 5));      //节点类型
				}else{
					iPreINT[1] = atoi(Mid(temp, 0, 16));   //节点号
					iPreINT[2] = atoi(Mid(temp, 16, 10));  //节点类型
				}
				if( iPreINT[2] < 1000 && iPreINT[2] != (100 + giUPxyz * 10) && iPreINT[2] != (200 + giUPxyz * 10) && iPreINT[2] != (300 + giUPxyz * 10) ){
					//X/Y/Z轴向上，刚吊类型依次为110/120/130(总体坐标系)或210/220/230(单元坐标系)或310/320/330(特定工况下起作用)
					//Debug.Print temp
					//非弹性且非刚性吊架
					//此类支吊架有6行数据,依次为Fx,Fy,Fz,Mx,My,Mz
					//先处理第一行
					i = 1;
					for( j = 1 ;j<=6;j++) //每行最多6个数据,依次为1C-Load(工况1分配荷载),IN-Load(安装荷载),WK-Load(工作荷载),CR-LOAD/S-SCALE(松冷荷载/结构荷载系数),WT-LOAD(水荷载),ST-LOAD(结构荷载)
					{
						if( gbNewGLIFV31 ){
							phsVAL[j][i] = atof(Mid(temp, 11 + (j - 1) * 12 -1, 12));
							mvMultiple+=phsVAL[j][1];
						}else{
							phsVAL[j] [i] = atof(Mid(temp, 26 + (j - 1) * 15 -1, 15));
							mvMultiple+=phsVAL[j][1];
						}
					}
					f.getline(temp.GetBuffer(255),255); temp.ReleaseBuffer();
            
					for(i = 2 ;i<= 6;i++)
					{
						//再读5行
						f.getline(temp.GetBuffer(255),255); temp.ReleaseBuffer();
						//f.getline(temp.GetBuffer(255),255); temp.ReleaseBuffer();
						
						mvMultiple = 0;
						for(j = 1 ;j<= 6;j++) //每行最多6个数据,依次为1C-Load(工况1分配荷载),IN-Load(安装荷载),WK-Load(工作荷载),CR-LOAD/S-SCALE(松冷荷载/结构荷载系数),WT-LOAD(水荷载),ST-LOAD(结构荷载)
						{
							if( gbNewGLIFV31 ){
								phsVAL[j][i] = atof(Mid(temp, 10 + (j - 1) * 12, 12));
								mvMultiple+=phsVAL[j][i];
							}else{
								phsVAL[j][i] =atof(Mid(temp, 26 + (j - 1) * 15 -1, 15));
								mvMultiple+=phsVAL[j][i];
							}
						}
						//如果6个数据不全为0
						//if( mvMultiple <> 0 ){
							//sFx代表了支吊架节点受力的方向
						 //  sFx = Mid(temp, Iif((gbNewGLIFV31, 85, 124), 1)
						  // Exit For
						//}else{
							//湖南电力院1999年5月前使用的GLIFV3.x版本，每个数据行之后空一行
							//贵州电力院2001年5月前使用的GLIFV3.1版本，辽宁院使用的号称GLIFv3.2版本，每个数据行之后不空行
							//f.getline(temp.GetBuffer(255),255); temp.ReleaseBuffer();  //空行
							//Debug.Print temp
						//}
					}
				}else{
					//此类支吊架只有1行数据:弹性或刚性吊架
					for(j = 1 ;j<=6;j++) //每行最多6个数据,依次为1C-Load(工况1分配荷载),IN-Load(安装荷载),WK-Load(工作荷载),CR-LOAD/S-SCALE(松冷荷载/结构荷载系数),WT-LOAD(水荷载),ST-LOAD(结构荷载)
					{
						if( gbNewGLIFV31 ){
							phsVAL[j][1]= atoi(Mid(temp, 11 + (j - 1) * 12-1, 12));
							mvMultiple+=phsVAL[j][1];
						}else{
							phsVAL[j][1] =atoi(Mid(temp, 26 + (j - 1) * 15-1, 15));
							mvMultiple+=phsVAL[j][1];
						}
					}
				}
				rsData->Filter = _variant_t(CString("[jsdbh1] = ") + ltos(iPreINT[1]) +" AND "+CString("[FileNameID]=")+ltos(FileNameID));
				//MsgBox rsData.RecordCount
					 if( rsData->BOF && rsData->adoEOF ){
						  rsData->AddNew();
						  rsData->PutCollect("UPxyz",_variant_t((long)giUPxyz));
						  rsData->PutCollect("VolumeID", _variant_t(EDIBgbl::SelVlmID));
						  rsData->PutCollect("Zdjh",_variant_t((long)(ltemp+1)));
						 //rsData->PutCollect("Zdjh",_variant_t((long)iPreINT[1]));
						   rsData->Update();
						  ltemp++;
					 }else{
					 }
					 for(i = 1 ;i<iCount;i++)
					 {
						 if( iJSJDH[i] == iPreINT[1] ) break;
					 }
					 rsData->PutCollect("PSAver",_variant_t(strPSAVER));
					 if( iPreINT[2] > 1000 && iPreINT[2] < 5000 ){
					 //弹性支吊架并联数
						rsData->PutCollect("ParallelingNum",_variant_t((long)atoi(Mid(ltos(iPreINT[2]), 1, 1))));
					 }
					 rsData->PutCollect("JSDBH1",_variant_t((long)iPreINT[1]));
					 rsData->PutCollect("Type", _variant_t((long)iPreINT[2]));
					 rsData->PutCollect("psaType", _variant_t((long)iPreINT[2]));
					 if( EDIBgbl::PSA_OutDataUnit=="NATIONAL" ){
						rsData->PutCollect("Unit_Force", "N");
						rsData->PutCollect("Unit_Momental", "N.m");
					 }else{
						rsData->PutCollect("Unit_Force", "kgf");
						rsData->PutCollect("Unit_Momental", "kgf.m");
					 }
					 rsData->PutCollect("Unit_Displacement", "mm");
					 if( iPreINT[2] < 1000 && iPreINT[2] != (100 + giUPxyz * 10) && iPreINT[2] != (200 + giUPxyz * 10) && iPreINT[2] != (300 + giUPxyz * 10) )
					 {
						sFx=Mid(ltos(iPreINT[2]),1, 1);     //百位表示限制线位移
						sFx1=Mid(ltos(iPreINT[2]), 2, 1);  //个位表示限制角位移
               
							rsData->PutCollect("paz0", _variant_t(phsVAL[giUPxyz][1]));
							rsData->PutCollect("paz1", _variant_t(phsVAL[giUPxyz][1]));
							rsData->PutCollect("pxl1", _variant_t(phsVAL[1][1]));
							rsData->PutCollect("pyl1", _variant_t(phsVAL[1][2]));
							rsData->PutCollect("pzl1", _variant_t(phsVAL[1][3]));
							rsData->PutCollect("pxr1", _variant_t(phsVAL[3][1]));
							rsData->PutCollect("pyr1", _variant_t(phsVAL[3][2]));
							rsData->PutCollect("pzr1", _variant_t(phsVAL[3][3]));
							rsData->PutCollect("pgz1", _variant_t(phsVAL[3][2]));
							rsData->PutCollect("pjg1", _variant_t(phsVAL[6][2]));
							rsData->PutCollect("mxl1", _variant_t(phsVAL[1][4]));
							rsData->PutCollect("myl1", _variant_t(phsVAL[1][5]));
							rsData->PutCollect("mzl1", _variant_t(phsVAL[1][6]));
							rsData->PutCollect("mxr1", _variant_t(phsVAL[3][4]));
							rsData->PutCollect("myr1", _variant_t(phsVAL[3][5]));
							rsData->PutCollect("mzr1", _variant_t(phsVAL[3][6]));
					 }else{
						//弹簧或刚吊
						rsData->PutCollect("paz0", _variant_t(phsVAL[2][1]));
						rsData->PutCollect("paz1", _variant_t(phsVAL[2][1]));
						rsData->PutCollect("pgz1", _variant_t(phsVAL[3][1]));
						rsData->PutCollect("pjg1", _variant_t(phsVAL[6][1]));
						if(giUPxyz==1)
						{
							rsData->PutCollect("pxl1", _variant_t(phsVAL[2][1]));
							rsData->PutCollect("pxr1", _variant_t(phsVAL[3][1]));
						}
						else if(giUPxyz==2)
						{
							rsData->PutCollect("pyl1", _variant_t(phsVAL[2][1]));
							rsData->PutCollect("pyr1", _variant_t(phsVAL[3][1]));
						}
						else						
						{
							rsData->PutCollect("pzl1", _variant_t(phsVAL[2][1]));
							rsData->PutCollect("pzr1", _variant_t(phsVAL[3][1]));
						}
						float fG,fR;int iSerialNum,iParelleNum,iSEQ;
						if(iPreINT[2]>1000 && iPreINT[2]<5000)
						{
							CString strTmp;
							strTmp.Format("%d",iPreINT[2]);
							iSerialNum=atoi(strTmp.Left(1));
							iParelleNum=atoi(strTmp.Mid(1,1));
							iSEQ=atoi(strTmp.Right(2));
							_variant_t vTmp;
							rs1->Find((_bstr_t)("SEQ=" + ltos(iSEQ)), 0, adSearchForward);
							if(!rs1->adoEOF)
							{
								rs1->get_Collect((_variant_t)"G", &varTmp);
								fG=vtof(varTmp);
								fR=fabs(phsVAL[2][1]) - fabs(phsVAL[3][1]);
								if(EDIBgbl::PSA_OutDataUnit== "NATIONAL")
									fR/=EDIBgbl::kgf2N;
								fR/=iParelleNum;
								fR/=(fG/iSerialNum);
								if(giUPxyz==1)
									rsData->PutCollect("xr1", _variant_t(fR));
								else if(giUPxyz==2)
									rsData->PutCollect("yr1", _variant_t(fR));
								else
									rsData->PutCollect("zr1", _variant_t(fR));
							}
						}
					 }
					 rsData->Update();
					 rsData->Filter=_variant_t((long)adFilterNone);
				iC1++;
			}else{
				if( temp.Find(strSTRUCTURE_LOAD_OF_RESTRAINTS) ==-1 ){
					break;
				}
			}
			
			if( iCount <= 0 ) iCount = 1;
			//MsgBox iC1 & ":" & iCount
			frmStatus.UpdateStatus((float)iC1/(float)iCount,true);
			frmStatus.UpdateWindow();
			//frmStatus.picstatus.Refresh
			//frmStatus.Refresh
		}
		f.close();

		
		//释放内存 cbl
		POSITION posList=mElevationList.GetHeadPosition();
		POSITION preposList;
		CNodeUpElevation *pNodeUpElevationData;
		while(posList !=NULL)
		{
			preposList=posList;
			pNodeUpElevationData = mElevationList.GetNext(posList);
            mElevationList.RemoveAt(preposList);	
			delete pNodeUpElevationData;
		}

		

		if(!rsData->adoEOF || !rsData->BOF)
			rsData->MoveFirst();
		_variant_t vtmp;
		while(!rsData->adoEOF)
		{
			vtmp=rsData->GetCollect("ZDJH");
			rsData->PutCollect("ZDJH",_variant_t((long)(vtoi(vtmp)+1000)));
			rsData->MoveNext();
		}
		_ConnectionPtr pCon;
		strSQL=(char*)(bstr_t)rsData->GetSource();
		pCon=rsData->GetActiveConnection();
		strSQL.Replace("ORDER BY JSDBH1","ORDER BY FileNameID");
		rsData->Close();
		rsData->Open(_variant_t(strSQL),(IDispatch*)pCon,adOpenKeyset,adLockOptimistic,adCmdText);
		//rsData->Sort=_bstr_t("JSDBH1");
		int itmpZdjh=1;
		while(!rsData->adoEOF)
		{
			rsData->PutCollect("ZDJH",_variant_t((long)itmpZdjh));
			itmpZdjh++;
			rsData->MoveNext();
		}
	}
	catch(_com_error e)
	{
		ShowMessage(e.Description());
	}
	catch(CException *e)
	{
		e->Delete();
	}
	catch(...)
	{
	}
}






void ReadResult_GLIF31New(_Recordset* rsResult, CString SourceDataFileName,long maxZdjh,long FileNameID)
{
	  //Remauther:
		//作者:
		//目的:本程序为读*.out文件，可用最后的msgbox语句来测试程序的正确性
		//输入:rsResult数据表内部单位:位移mm,荷载kgf.
		//返回:
   try
	{
		_variant_t nil;
		nil.ChangeType(VT_NULL);
		 CString strOldPhsSPRINGSel; //保存读取应力计算数据之前的弹簧标准
		 float mvMultiple=0;
		 CString sFx, sFx1;
		 CString strTemp;
		 CString temp;
		 int iTemp=0;
		 float phsVAL[7][9]; //此数组代表后面的单精度数据，X维代表整型数据后面的列，
												 //Y维代表行
		 int iPreINT[3]; //此数组代表记录前面的两个整型数
		 _Recordset* rsData;

		LPCTSTR strPIPING_STRESS_OUTUNIT = " + ";   
		LPCTSTR strSTRUCTURE_LOAD_OF_RESTRAINTS = "STRUCTURE LOAD OF RESTRAINTS";
		LPCTSTR strREACTION_ON_ANCHORS_ColdRelease_CASE3 = "REACTION OF PIPING ON ANCHORS (COLD-RELEASE)";
		
		LPCTSTR strREACTION_ON_ANCHORS_ColdInitial_CASE4;
		LPCTSTR strREACTION_ON_ANCHORS_WorkInitial_CASE2;
		LPCTSTR strREACTION_ON_ANCHORS_Cold_CASE1;
		if(YLJSVer.Left(7)!="AutoPSA")
		{
			strREACTION_ON_ANCHORS_ColdInitial_CASE4 = "REACTION OF PINING ON ANCHORS (COLD-INITIAL)";			
			strREACTION_ON_ANCHORS_WorkInitial_CASE2 = "REACTION OF PINING ON ANCHORS (WORK-INITIAL)";
			strREACTION_ON_ANCHORS_Cold_CASE1 = "REACTION OF PINING ON ANCHORS";
		}
		else
		{
			//UEsoft AutoPSA1.0 correct PINING to PIPING
			strREACTION_ON_ANCHORS_ColdInitial_CASE4 = "REACTION OF PIPING ON ANCHORS (COLD-INITIAL)";
			strREACTION_ON_ANCHORS_WorkInitial_CASE2 = "REACTION OF PIPING ON ANCHORS (WORK-INITIAL)";
			strREACTION_ON_ANCHORS_Cold_CASE1 = "REACTION OF PIPING ON ANCHORS";
		}

		LPCTSTR strCOLD_SPRING_TABLE = "COLD SPRING JOINT POINT";
		LPCTSTR strEND_RawData = "10     1     1    1" ; //空格个数与GLIFV1.2不同
		LPCTSTR strPSAVER = "GLIF V31";
   
		LPCTSTR strCW_DISPLACEMENT = "CW-DISPLACEMENT (cold/work-status)";
   
		LPCTSTR strEND_THIS_PIPING = "*THIS PIPING STRUCTURE *CONTAINS*";
		CString strSprTbn;
		_RecordsetPtr tmprs;
		tmprs.CreateInstance(__uuidof(Recordset));
		_variant_t varTmp;
		int fdFlg;
		CString mvsCASE, mvsCASEcool;
		int mviSPR=0;
		long i=0;
		//在显示输入框之前隐藏进度框
		frmStatus.ShowWindow(SW_HIDE);
		CInputBox dlg;
		while(sFx == "" || (sFx.GetAt(0) < '0' || sFx.GetAt(0) > '9')  || ( sFx != "1" && sFx != "3" && sFx != "4"))
		{
			dlg.m_strTitle=GetResStr(IDS_SelThrustQuaGist);
			dlg.m_strWndText=GetResStr(IDS_SelWorkCon);
			dlg.m_strValue="4";
			dlg.DoModal();
			sFx =dlg.m_strValue;
			sFx.TrimLeft();
			sFx.TrimRight();
		}
		i = atoi(sFx);
		if( i == 1 )
		{
			mvsCASEcool = strREACTION_ON_ANCHORS_Cold_CASE1;
		}
		else
		{
			if( i == 3 )
			{
				mvsCASEcool = strREACTION_ON_ANCHORS_ColdRelease_CASE3;
			}
			else
			{
				mvsCASEcool = strREACTION_ON_ANCHORS_ColdInitial_CASE4;
			}
		}
		frmStatus.ShowWindow(SW_SHOW);
		frmStatus.UpdateWindow();
		//截面特性记录:GLIF1.2,NC=6;GLIF3.x,NC=60(弯单元)及61(直单元),成对使用.
		//介质特性记录:GLIF1.2,NC=9;GLIF3.x,NC=90(设计工况).
		const int iNCtj = 90, iNCdw1 = 61, iNCdw = 60;
   
		float sngDWTemp=0, TjTemp=0;
		long iJSJDH_start=0, iJSJDH_end=0;
		CArray<long,long & > iJSJDH;
		CArray<float,float & > sngDW;
		CArray<float ,float & > Tj;
		CArray<float,float &> sngDH1;

		sngDH1.SetSize(100);
		iJSJDH.SetSize(100);
		sngDW.SetSize(100);
		Tj.SetSize(100);
   
		int j=0, iC1=0;
		ifstream f;
		int iFlag=0, iCount=0;
		CString mvS,tempsign;
		bool bCwDisplacement=false, bStrucLoad=false;
        bool sign=false;//chengbailian 2003.11.6 为了适应另外一种文件，90记录是否在10记录之录出现		
		long pos;
		ElevationList mElevationList;
		CNodeUpElevation *pNodeUpElevation;
		int ICC;
		
		//cbl 2004.1.10 增加了每个节点的标高
		//坐标文件名
		CString ElevationFileName=SourceDataFileName.Left(SourceDataFileName.ReverseFind('.'))+".et";//标高文件名

		//生成坐标文件(记录了每个节点的坐标)(链接库中的函数)
		ICC=PSAPointCoordinateWirteFile(SourceDataFileName,ElevationFileName);
	  
		//从文件中读取每个点的标高放入链表mElevationList中
		ElevationFileToList(ElevationFileName,&mElevationList,ICC);
        ::DeleteFile(ElevationFileName); 
	//cbl

		f.open(SourceDataFileName);

		CString strSQL;
		//开始读原始数据
		while(!f.eof())
		{//4第一次读文本文件循环(start)
			f.getline(temp.GetBuffer(255),255); temp.ReleaseBuffer();
			if( temp.Find(strPIPING_STRESS_OUTUNIT)!=-1)
			{//3
				//第一个记录第一个字段是输入单位制，第2个字段是输出单位制(=0，法定单位制)，第3个字段是坐标轴方向
				if( atoi(Trim(Mid(temp,8, 4))) == 0 )
				{//1
					EDIBgbl::PSA_OutDataUnit = "NATIONAL";
				}//1
				else
				{//1
					EDIBgbl::PSA_OutDataUnit = "ENGINEERING";
				}//1
				//坐标轴方向x/y/z轴向上=1/2/3
				giUPxyz = atoi(Mid(temp,12, 6));

				f.getline(temp.GetBuffer(255),255); temp.ReleaseBuffer();
				//GLIFV3.x中,原始数据文件第2个记录第1个数据为弹簧种类,第3个数据为弹簧荷载变化系数
				if(gbNewGLIFV31)
				{//1
					mviSPR = atoi(Trim(Mid(temp, 3, 6)));
					modPHScal::gnSPR_CHZBH = atof(Trim(Mid(temp, 13, 10)));
				}//1
				else
				{//1
					mviSPR = atoi(Trim(Mid(temp, 6, 10)));
					modPHScal::gnSPR_CHZBH = atof(Trim(Mid(temp, 21, 10)));
				}//1
				strOldPhsSPRINGSel = modPHScal::gsPhsSPRINGSel;
				//因为应力计算选择的弹簧标准可能不同,故重新更新规范
				//由于生成规范花费时间较长，所以只有当规范变化时，才需要重新更新
				if( mviSPR == 3 )
				{//2
					strSQL = "SELECT Spring_property FROM phsManuSPRING WHERE StandardDesc=\'";
					strSQL += strSprNWEPDI;
					strSQL += "\'";
					tmprs->Open((_bstr_t)strSQL, _variant_t((IDispatch*)EDIBgbl::dbSORT,true), 
						adOpenKeyset, adLockOptimistic, adCmdText); 
					if(tmprs->BOF && tmprs->adoEOF)
					{//1
					}//1
					else
					{//1
						tmprs->get_Collect((_variant_t)0L, &varTmp);
						strSprTbn=vtos(varTmp);
					}//1
					tmprs->Close();
					if( strOldPhsSPRINGSel != strSprNWEPDI )
					{//1
						modPHScal::gsPhsSPRINGSel = strSprNWEPDI;
					}//1
				}//2
				else if( mviSPR == 4 )
				{//2
					strSQL = "SELECT Spring_property FROM phsManuSPRING WHERE StandardDesc=\'";
					strSQL += strSprGB10182;
					strSQL += "\'";
					tmprs->Open((_bstr_t)strSQL, _variant_t((IDispatch*)EDIBgbl::dbSORT,true), 
						adOpenKeyset, adLockOptimistic, adCmdText); 
					if(tmprs->BOF && tmprs->adoEOF)
					{//1
					}//1
					else
					{//1
						tmprs->get_Collect((_variant_t)0L, &varTmp);
						strSprTbn=vtos(varTmp);
					}//1
					tmprs->Close();
					if( strOldPhsSPRINGSel != strSprGB10182 )
					{//1
						modPHScal::gsPhsSPRINGSel = strSprGB10182;
					}//1
				}//2         
				//荷载变化系数保存到外部
				SetRegValue(_T("Settings"), "SPR_Chzbh", (modPHScal::gnSPR_CHZBH));
				break;
			}//3
		}//4第一次读文本文件循环(end)
   
		iCount = 1;
		while(!f.eof())
		{//9 处理输入数据读文件循环(start)
			iFlag=0;
			do
			{//8
				f.getline(temp.GetBuffer(255),255); temp.ReleaseBuffer();

				if(temp.IsEmpty() )
					continue;
				
				if( temp.Mid(1,1) != "+")
					continue;
				//如果遇到原始数据结束标志则退出循环
				if( Trim(Mid(temp,10)).Find( strEND_RawData)==0) break;
				iTemp = atoi(Mid(temp, 8, 4));
				if( iTemp == iNCtj ) 
				{//1
					TjTemp = atof(Mid(temp, 23, 7));//TjTemp介质温度
					sign=true;
				}//1
				if( (iTemp == iNCdw)  ) 
				{//1
					sngDWTemp = atof(Mid(temp, 13, 7));
					if( iFlag != 0 ) 
						sngDW.SetAtGrow(iCount-1,sngDWTemp);
				}//1
				if( iTemp == 10 )
				{//7 //是节点编号记录(start)
					if(!sign)//chengbailian 2003.11.6
					{//3 //如果10记录出现而90记录还没有出现，先找到90记录读取介质温度，在PSA计算出的文件没有这种情况，而王总的软件计算出的有这种情况
						pos=f.tellg();
						while(!f.eof())
						{//2
							f.getline(tempsign.GetBuffer(255),255);
						    tempsign.ReleaseBuffer();

							if(tempsign.IsEmpty() )
								continue;
							
							if( tempsign.Mid(1,1) != "+")
								continue;
								
							if(atoi(Mid(tempsign, 8, 4))==iNCtj)
							{//1
								TjTemp = atof(Mid(tempsign, 23, 7));
								break;
							}//1

						}//2
						f.seekg(pos);//指针回到原来位置
					}//3
					//计算点始节点号
					iJSJDH_start = atoi(Mid(temp, 13, 10));
					//计算点末节点号
					iJSJDH_end = atoi(Mid(temp, 20, 10));
					if( iFlag == 0 )
					{//5 //是第一个节点记录(start)
						//第一个节点记录只要简单地保存始末节点
						iJSJDH.SetAtGrow(iCount,iJSJDH_start);
						sngDW.SetAtGrow(iCount,sngDWTemp);
						Tj.SetAtGrow(iCount,TjTemp);
						iCount++;
						
                			pos=f.tellg();
			                int iFlg = 0;
							while(!f.eof() && iFlg==0)
							{//4
								f.getline(tempsign.GetBuffer(255),255);
								tempsign.ReleaseBuffer();

								if(tempsign.IsEmpty() )
									continue;
								
								if( tempsign.Mid(1,1) != "+")
									continue;
								if (atoi(Mid(tempsign, 8, 4))==10)
								{//1
									break;
								}//1
								if(atoi(Mid(tempsign, 8, 4))==iNCtj)
								{//3
                                    long Ppos;
                                    float fTjTemp = atof(Mid(tempsign, 23, 7));
									while (!f.eof() && iFlg==0)
									{//2
                                        Ppos = f.tellg();
								        f.getline(tempsign.GetBuffer(255),255);
								        tempsign.ReleaseBuffer();

										if(tempsign.IsEmpty() )
											continue;
										
										if( tempsign.Mid(1,1) != "+")
											continue;
									
										int ipipetype = atoi(Mid(tempsign, 8, 4));
										if (ipipetype == iNCtj)
										{//1
											f.seekg(Ppos);//指针回到原来位置
											break;
										}//1
        								if (ipipetype == 10)
										{//1
											iFlg = 1;
									        break;
										}//1
										if (ipipetype==0 || ipipetype==1 || ipipetype==2 ||ipipetype==3 ||
											ipipetype==4 || ipipetype==5 || ipipetype==6)
										{//1
											TjTemp = fTjTemp;
										}//1
									}//2
								}//3
							}//4
							f.seekg(pos);//指针回到原来位置


						iJSJDH.SetAtGrow(iCount,iJSJDH_end);
						sngDW.SetAtGrow(iCount,sngDWTemp);
						Tj.SetAtGrow(iCount,TjTemp);
						iCount++;
					}//5 //是第一个节点记录(end)
					else
					{//6 //不是第一个节点记录(start)
						for( i = 1 ;i<iCount;i++)
						{//1
							if( iJSJDH_end == iJSJDH[i] ) break;
						}//1
						if( i >= iCount )
						{//5
                            pos=f.tellg();
			                int iFlg = 0;
							while(!f.eof() && iFlg==0)
							{//4
								f.getline(tempsign.GetBuffer(255),255);
								tempsign.ReleaseBuffer();
								
								if(tempsign.IsEmpty() )
									continue;
								
								if( tempsign.Mid(1,1) != "+")
									continue;

								if (atoi(Mid(tempsign, 8, 4))==10)
								{//1
									break;
								}//1
								if(atoi(Mid(tempsign, 8, 4))==iNCtj)
								{//3
                                    long Ppos;
                                    float fTjTemp = atof(Mid(tempsign, 23, 7));
									while (!f.eof() && iFlg==0)
									{//2
                                        Ppos = f.tellg();
								        f.getline(tempsign.GetBuffer(255),255);
								        tempsign.ReleaseBuffer();
										
										if(tempsign.IsEmpty() )
											continue;
										
										if( tempsign.Mid(1,1) != "+")
											continue;

										int ipipetype = atoi(Mid(tempsign, 8, 4));
										if (ipipetype == iNCtj)
										{//1
											f.seekg(Ppos);//指针回到原来位置
											break;
										}//1
        								if (ipipetype == 10)
										{//1
											iFlg = 1;
									        break;
										}//1
										if (ipipetype==0 || ipipetype==1 || ipipetype==2 ||ipipetype==3 ||
											ipipetype==4 || ipipetype==5 || ipipetype==6)
										{//1
											TjTemp = fTjTemp;
										}//1
									}//2
								}//3
							}//4
							f.seekg(pos);//指针回到原来位置

							
							iJSJDH.SetAtGrow(iCount,iJSJDH_end);
							sngDW.SetAtGrow(iCount,sngDWTemp);
							Tj.SetAtGrow(iCount,TjTemp);
							iCount++;
						}//5
					}//6 //不是第一个节点记录(end)
					//设置标志区分是否是第一个节点记录
					if( iFlag == 0 ) iFlag = 1;
				}//7 //是节点编号记录(end)
			}while(Trim(Mid(temp, 10)).Find(strEND_RawData)!=0 && !f.eof());//8
			if( Trim(Mid(temp, 10)).Find(strEND_RawData)==0) break;
		}//9 处理输入数据读文件循环(end)
   
		frmStatus.UpdateStatus(0.02f,true);
		frmStatus.UpdateWindow();
		long ltemp=maxZdjh;
		rsData=rsResult;
		//支吊架点不得包含端点
		//管道端点初热数据在文件前部
		//GLIFV3.x:开始查找管道端点初热荷载数据头部
		f.close();
		f.open(SourceDataFileName);
		//GLIFV3.x:开始查找位移数据头部
		while(!f.eof())
		{
			f.getline(strTemp.GetBuffer(255),255); strTemp.ReleaseBuffer();
			if(strTemp.Find(strCW_DISPLACEMENT) !=-1 )
			{ //找到"CW-DISPLACEMENT (cold/work-status)"
				break;
			}

		}
   
		//找到"CW-DISPLACEMENT (cold/work-status)"
		//GLIFV3.x:开始读位移数据
		fdFlg=0;
		while(!f.eof())
		{
			f.getline(temp.GetBuffer(255),255); temp.ReleaseBuffer();
			//湖南电力院1999年5月前使用的GLIFV3.x版本，iPreINT(1)的字符宽度从第1-10位
			//贵州电力院2001年5月前使用的GLIFV3.1版本，iPreINT(1)的字符宽度从第1-5位，
			//iPreINT(2)的字符宽度从第6-10位，
			if( atof(Mid(temp, 0, 5)) >0 )
			{
				fdFlg=1;
				if( gbNewGLIFV31 )
				{
					iPreINT[1] = atoi(Mid(temp,0, 5)) ;     //节点号
					iPreINT[2] = atoi(Mid(temp, 5, 7)) ;     //节点类型
				}
				else
				{
					iPreINT[1] = atoi(Mid(temp, 0, 10));     //节点号
					iPreINT[2] = atoi(Mid(temp, 10, 10));    //节点类型
				}
				for(j = 1;j<= 6;j++) //每行最多6个数据,依次为dcx,dcy,dcz,dwx,dwy,dwz,SP-Set,SP-Press
				{
					if( gbNewGLIFV31 )
					{
						phsVAL[j][1] = atof(Mid(temp, (j <= 3 ? 13 : 35 - 3 * 7) + (j - 1) * 7-1, 7));
					}
					else
					{
						phsVAL[j][1] =atof(Mid(temp, (j <= 3 ? 51 : 83 - 4 * 7) + (j - 1) * 7 -1, 7));
					}
				}
				rsData->Filter = _variant_t(CString("[jsdbh1] = ") + ltos(iPreINT[1])+" AND "+CString("[FileNameID]=")+ltos(FileNameID));
				//MsgBox rsData.RecordCount
					 if( rsData->BOF && rsData->adoEOF )
					 {
						  rsData->AddNew();
						  rsData->PutCollect("UPxyz",_variant_t((long)giUPxyz));
						  rsData->PutCollect("VolumeID",_variant_t(EDIBgbl::SelVlmID));
						  rsData->PutCollect("Zdjh",_variant_t((long)(ltemp+1)));
						  rsData->PutCollect("FileNameID",_variant_t(FileNameID));
						  rsData->PutCollect("PSACoefofVarLoad",_variant_t(modPHScal::gnSPR_CHZBH));//pfg20051019 //加上应力计算的弹簧荷载变化系数
						   rsData->Update();
						  ltemp++;
					 }
					 else
					 {
					 }
					 for(i = 1 ;i<iCount;i++)
					 {
						 if( iJSJDH[i] == iPreINT[1] ) break;
					 }
					 rsData->PutCollect("JSDBH1", _variant_t((long)iPreINT[1]));
					 rsData->PutCollect("dj1", _variant_t(sngDW[i]));
                     
					 POSITION pos=mElevationList.GetHeadPosition();
					 while(pos !=NULL)
					 {
					     //查找当前支吊点的标高	
						 pNodeUpElevation = mElevationList.GetNext(pos);

						 if(pNodeUpElevation->NodeNum == iPreINT[1])
						 {
							 sngDH1.SetAtGrow(i,pNodeUpElevation->UpElevation); 
							 sngDH1[i]=int(sngDH1[i]*1000+0.5)/1000.0; //精确三位小数
							 break;
						 }
						 
					 }

					 rsData->PutCollect("dh1", _variant_t(sngDH1[i]));
					 rsData->PutCollect("t01", _variant_t(Tj[i]));
					 rsData->PutCollect("xl1", _variant_t(phsVAL[1][1]));
					 rsData->PutCollect("yl1", _variant_t(phsVAL[2][1]));
					 rsData->PutCollect("zl1", _variant_t(phsVAL[3][1]));
					 rsData->PutCollect("xr1", _variant_t(phsVAL[4][1]));
					 rsData->PutCollect("yr1", _variant_t(phsVAL[5][1]));
					 rsData->PutCollect("zr1", _variant_t(phsVAL[6][1]));
					 rsData->PutCollect("haz", nil);
					 rsData->Filter=_variant_t((long)adFilterNone);
				iC1++;
				frmStatus.UpdateStatus((float)iC1/(float)iCount,true);
				frmStatus.UpdateWindow();
			}
			else
			{
				if( fdFlg==1)
				{
					do
					{
						if(temp.Find(strCW_DISPLACEMENT) != -1 )
						{
							break;
						}
						f.getline(temp.GetBuffer(255),255); temp.ReleaseBuffer();
					}while(!f.eof());
					fdFlg=0;
				}
			}
			
			if( iCount <= 0 ) iCount = 1;
			//MsgBox iC1 & ":" & iCount
			frmStatus.UpdateStatus((float)iC1/(float)iCount,false);
			frmStatus.UpdateWindow();
		}
		f.close();
		//////////////////////////////////cbl
		//释放内存
		POSITION posList=mElevationList.GetHeadPosition();
		POSITION preposList;
		CNodeUpElevation *pNodeUpElevationData;
		while(posList !=NULL)
		{
			preposList=posList;
			pNodeUpElevationData = mElevationList.GetNext(posList);
            mElevationList.RemoveAt(preposList);
			delete pNodeUpElevationData;
		}
		/////////////////////////////////////////////
   
		f.open(SourceDataFileName);
		//GLIFV3.x:查找荷载数据头
		while(!f.eof())
		{
			f.getline(strTemp.GetBuffer(255),255); strTemp.ReleaseBuffer();
			if( strTemp.Find(strSTRUCTURE_LOAD_OF_RESTRAINTS) !=-1 )
			{ //找到"STRUCTURE LOAD OF RESTRAINTS"
				break;
			}
		}
                     
		//找到"STRUCTURE LOAD OF RESTRAINTS"
		//GLIFV3.x:开始读荷载数据
		fdFlg=0;
		_RecordsetPtr rs1;
		rs1.CreateInstance(__uuidof(Recordset));

		strSQL.Format("SELECT G,SEQ FROM %s",strSprTbn);
		tmprs->Open((_bstr_t)strSQL, _variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
			adOpenKeyset, adLockOptimistic, adCmdText); 
		while(!f.eof())
		{
			f.getline(temp.GetBuffer(255),255); temp.ReleaseBuffer();
			if( atof(Mid(temp,0, (gbNewGLIFV31 ? 5 : 16))) > 0 )
			{
				fdFlg=1;
				if( gbNewGLIFV31 )
				{
					iPreINT[1] = atoi(Mid(temp, 0, 5));      //节点号
					iPreINT[2] = atoi(Mid(temp, 5, 5));      //节点类型
				}
				else
				{
					iPreINT[1] = atoi(Mid(temp, 0, 16));   //节点号
					iPreINT[2] = atoi(Mid(temp, 16, 10));  //节点类型
				}
				if( iPreINT[2] < 1000 && iPreINT[2] != (100 + giUPxyz * 10) && iPreINT[2] != (200 + giUPxyz * 10) && iPreINT[2] != (300 + giUPxyz * 10) )
				{
					//X/Y/Z轴向上，刚吊类型依次为110/120/130(总体坐标系)或210/220/230(单元坐标系)或310/320/330(特定工况下起作用)
					//Debug.Print temp
					//非弹性且非刚性吊架
					//此类支吊架有6行数据,依次为Fx,Fy,Fz,Mx,My,Mz
					//先处理第一行
					i = 1;
					//如果是
					for( j = 1 ;j<=6;j++) //每行最多6个数据,依次为1C-Load(工况1分配荷载),IN-Load(安装荷载),WK-Load(工作荷载),CR-LOAD/S-SCALE(松冷荷载/结构荷载系数),WT-LOAD(水荷载),ST-LOAD(结构荷载)
					{
						if( gbNewGLIFV31 )
						{
							phsVAL[j][i] = atof(Mid(temp, 11 + (j - 1) * 12 -1, 12));
							mvMultiple+=phsVAL[j][1];
						}
						else
						{
							phsVAL[j] [i] = atof(Mid(temp, 26 + (j - 1) * 15 -1, 15));
							mvMultiple+=phsVAL[j][1];
						}
					}					
            
					for(i = 2 ;i<= 6;i++)
					{
						//再读5行
						f.getline(temp.GetBuffer(255),255); temp.ReleaseBuffer();
						mvMultiple = 0;
						for(j = 1 ;j<= 6;j++) //每行最多6个数据,依次为1C-Load(工况1分配荷载),IN-Load(安装荷载),WK-Load(工作荷载),CR-LOAD/S-SCALE(松冷荷载/结构荷载系数),WT-LOAD(水荷载),ST-LOAD(结构荷载)
						{
							if( gbNewGLIFV31 )
							{
								phsVAL[j][i] = atof(Mid(temp, 10 + (j - 1) * 12, 12));
								mvMultiple+=phsVAL[j][i];
							}
							else
							{
								phsVAL[j][i] =atof(Mid(temp, 26 + (j - 1) * 15 -1, 15));
								mvMultiple+=phsVAL[j][i];
							}
						}
						//如果6个数据不全为0
						//if( mvMultiple <> 0 ){
							//sFx代表了支吊架节点受力的方向
						 //  sFx = Mid(temp, Iif((gbNewGLIFV31, 85, 124), 1)
						  // Exit For
						//}else{
							//湖南电力院1999年5月前使用的GLIFV3.x版本，每个数据行之后空一行
							//贵州电力院2001年5月前使用的GLIFV3.1版本，辽宁院使用的号称GLIFv3.2版本，每个数据行之后不空行
							//f.getline(temp.GetBuffer(255),255); temp.ReleaseBuffer();  //空行
							//Debug.Print temp
						//}
					}
				}
				else
				{
					//此类支吊架只有1行数据:弹性或刚性吊架
					for(j = 1 ;j<=6;j++) //每行最多6个数据,依次为1C-Load(工况1分配荷载),IN-Load(安装荷载),WK-Load(工作荷载),CR-LOAD/S-SCALE(松冷荷载/结构荷载系数),WT-LOAD(水荷载),ST-LOAD(结构荷载)
					{
						if( gbNewGLIFV31 )
						{
							phsVAL[j][1]= atoi(Mid(temp, 11 + (j - 1) * 12-1, 12));
							mvMultiple+=phsVAL[j][1];
						}
						else
						{
							phsVAL[j][1] =atoi(Mid(temp, 26 + (j - 1) * 15-1, 15));
							mvMultiple+=phsVAL[j][1];
						}
					}
				}
				rsData->Filter = _variant_t(CString("[jsdbh1] = ") + ltos(iPreINT[1]) + " AND "+CString("[FileNameID]=")+ltos(FileNameID));
				//MsgBox rsData.RecordCount
					 if( rsData->BOF && rsData->adoEOF )
					 {
						  rsData->AddNew();
						  rsData->PutCollect("UPxyz",_variant_t((long)giUPxyz));
						  rsData->PutCollect("VolumeID", _variant_t(EDIBgbl::SelVlmID));
						  rsData->PutCollect("Zdjh",_variant_t((long)(ltemp+1)));
						  rsData->PutCollect("PSACoefofVarLoad",_variant_t(modPHScal::gnSPR_CHZBH));//pfg20051019 //加上应力计算的弹簧荷载变化系数
						  rsData->Update();
						  ltemp++;
					 }
					 else
					 {
					 }
					 for(i = 1 ;i<iCount;i++)
					 {
						 if( iJSJDH[i] == iPreINT[1] ) break;
					 }
					 rsData->PutCollect("PSAver",_variant_t(strPSAVER));
					 if( iPreINT[2] > 1000 && iPreINT[2] < 5000 )
					 {
					 //弹性支吊架并联数
						rsData->PutCollect("ParallelingNum",_variant_t((long)atoi(Mid(ltos(iPreINT[2]), 1, 1))));
					 }
					 rsData->PutCollect("JSDBH1",_variant_t((long)iPreINT[1]));
					 rsData->PutCollect("Type", _variant_t((long)iPreINT[2]));
					 rsData->PutCollect("psaType", _variant_t((long)iPreINT[2]));
					 if( EDIBgbl::PSA_OutDataUnit=="NATIONAL" )
					 {
						rsData->PutCollect("Unit_Force", "N");
						rsData->PutCollect("Unit_Momental", "N.m");
					 }
					 else
					 {
						rsData->PutCollect("Unit_Force", "kgf");
						rsData->PutCollect("Unit_Momental", "kgf.m");
					 }
					 rsData->PutCollect("Unit_Displacement", "mm");
					 if( iPreINT[2] < 1000 && iPreINT[2] != (100 + giUPxyz * 10) && iPreINT[2] != (200 + giUPxyz * 10) && iPreINT[2] != (300 + giUPxyz * 10) )
					 {
						sFx=Mid(ltos(iPreINT[2]),1, 1);     //百位表示限制线位移
						sFx1=Mid(ltos(iPreINT[2]), 2, 1);  //个位表示限制角位移
					
               
							rsData->PutCollect("paz0", _variant_t(phsVAL[giUPxyz][1]));
							rsData->PutCollect("paz1", _variant_t(phsVAL[giUPxyz][1]));
							rsData->PutCollect("pxl1", _variant_t(phsVAL[2][1]));
							rsData->PutCollect("pyl1", _variant_t(phsVAL[2][2]));
							rsData->PutCollect("pzl1", _variant_t(phsVAL[2][3]));
							rsData->PutCollect("pxr1", _variant_t(phsVAL[3][1]));
							rsData->PutCollect("pyr1", _variant_t(phsVAL[3][2]));
							rsData->PutCollect("pzr1", _variant_t(phsVAL[3][3]));
							rsData->PutCollect("pgz1", _variant_t(phsVAL[3][giUPxyz]));
							rsData->PutCollect("pjg1", _variant_t(phsVAL[6][giUPxyz]));
							rsData->PutCollect("pjgFX1", _variant_t(phsVAL[6][1]));
							rsData->PutCollect("pjgFY1", _variant_t(phsVAL[6][2]));
							rsData->PutCollect("pjgFZ1", _variant_t(phsVAL[6][3]));
							rsData->PutCollect("pjgMX1", _variant_t(phsVAL[6][4]));
							rsData->PutCollect("pjgMY1", _variant_t(phsVAL[6][5]));
							rsData->PutCollect("pjgMZ1", _variant_t(phsVAL[6][6]));
							rsData->PutCollect("mxl1", _variant_t(phsVAL[2][4]));
							rsData->PutCollect("myl1", _variant_t(phsVAL[2][5]));
							rsData->PutCollect("mzl1", _variant_t(phsVAL[2][6]));
							rsData->PutCollect("mxr1", _variant_t(phsVAL[3][4]));
							rsData->PutCollect("myr1", _variant_t(phsVAL[3][5]));
							rsData->PutCollect("mzr1", _variant_t(phsVAL[3][6]));
					 }
					 else
					 {
						//弹簧或刚吊
						rsData->PutCollect("paz0", _variant_t(phsVAL[2][1]));
						rsData->PutCollect("paz1", _variant_t(phsVAL[2][1]));
						rsData->PutCollect("pgz1", _variant_t(phsVAL[3][1]));
						rsData->PutCollect("pjg1", _variant_t(phsVAL[6][1]));
						if(giUPxyz==1)
						{
							rsData->PutCollect("pxl1", _variant_t(phsVAL[2][1]));
							rsData->PutCollect("pxr1", _variant_t(phsVAL[3][1]));
							rsData->PutCollect("pjgFX1", _variant_t(phsVAL[6][1]));
						}
						else if(giUPxyz==2)
						{
							rsData->PutCollect("pyl1", _variant_t(phsVAL[2][1]));
							rsData->PutCollect("pyr1", _variant_t(phsVAL[3][1]));
							rsData->PutCollect("pjgFY1", _variant_t(phsVAL[6][1]));
						}
						else						
						{
							rsData->PutCollect("pzl1", _variant_t(phsVAL[2][1]));
							rsData->PutCollect("pzr1", _variant_t(phsVAL[3][1]));
							rsData->PutCollect("pjgFZ1", _variant_t(phsVAL[6][1]));
						}
						float fG,fR;int iSerialNum,iParelleNum,iSEQ;
						if(iPreINT[2]>1000 && iPreINT[2]<5000)
						{
							CString strTmp;
							strTmp.Format("%d",iPreINT[2]);
							iSerialNum=atoi(strTmp.Left(1));
							iParelleNum=atoi(strTmp.Mid(1,1));
							iSEQ=atoi(strTmp.Right(2));
							_variant_t vTmp;
							rs1->Find((_bstr_t)("SEQ=" + ltos(iSEQ)), 0, adSearchForward);
							if(!rs1->adoEOF)
							{
								rs1->get_Collect((_variant_t)"G", &varTmp);
								fG=vtof(varTmp);
								fR=fabs(phsVAL[2][1]) - fabs(phsVAL[3][1]);
								if(EDIBgbl::PSA_OutDataUnit== "NATIONAL")
									fR/=EDIBgbl::kgf2N;
								fR/=iParelleNum;
								fR/=(fG/iSerialNum);
								if(giUPxyz==1)
									rsData->PutCollect("xr1", _variant_t(fR));
								else if(giUPxyz==2)
									rsData->PutCollect("yr1", _variant_t(fR));
								else
									rsData->PutCollect("zr1", _variant_t(fR));
							}
						}
					 }
					 rsData->Filter=_variant_t((long)adFilterNone);
				iC1++;
			}
			else
			{
				if( fdFlg==1)
				{
					do
					{
						if(temp.Find(strSTRUCTURE_LOAD_OF_RESTRAINTS) != -1 )
						{
							break;
						}
						f.getline(temp.GetBuffer(255),255); temp.ReleaseBuffer();
					}while(!f.eof());
					fdFlg=0;
				}
			}
			
			if( iCount <= 0 ) iCount = 1;
			//MsgBox iC1 & ":" & iCount
			frmStatus.UpdateStatus((float)iC1/(float)iCount,true);
			frmStatus.UpdateWindow();
		}
		f.close();

		if(!rsData->adoEOF || !rsData->BOF)
			rsData->MoveFirst();
		_variant_t vtmp;
		while(!rsData->adoEOF)
		{
			vtmp=rsData->GetCollect("ZDJH");
			rsData->PutCollect("ZDJH",_variant_t((long)(vtoi(vtmp)+1000)));
			rsData->MoveNext();
		}

		_ConnectionPtr pCon;
		strSQL=(char*)(bstr_t)rsData->GetSource();
		pCon=rsData->GetActiveConnection();
		strSQL.Replace("ORDER BY JSDBH1","ORDER BY FileNameID");
		rsData->Close();
		rsData->Open(_variant_t(strSQL),(IDispatch*)pCon,adOpenKeyset,adLockOptimistic,adCmdText);
		int itmpZdjh=1;
		while(!rsData->adoEOF)
		{
			rsData->PutCollect("ZDJH",_variant_t((long)itmpZdjh));
			itmpZdjh++;
			rsData->MoveNext();
		}
	}
	catch(_com_error e)
	{
		ShowMessage(e.Description());
	}
	catch(CException *e)
	{
		e->Delete();
	}
}
/*
void  ReadResult_CAESARII45(_Recordset* rsResult, CString SourceDataFileName,long maxZdjh,long FileNameID)
{
 
	//Zhufang  kuang   add  2005.5.30
	const char* strPSAVER = "CASEARII 4.50";  
	CString ConnectionString="Provider=Microsoft.Jet.OLEDB.4.0;Persist Security Info=False;Data Source=%s;Jet OLEDB:Database Password=%s";
	CString dbConnectionString;
   
	_ConnectionPtr m_pConnSourceDB;			   //源数据库的连接对象
	_RecordsetPtr m_prsCASENUM;					//热态（冷态）的记录集
	_RecordsetPtr m_prsHangers;					//表[OUTPUT_HANGERS]记录集
	_RecordsetPtr m_prsDisplacements;			//表[OUTPUT_DISPLACEMENTS]记录集
	_RecordsetPtr m_prsRestraints;				//表[OUTPUT_RESTRAINTS]记录集
	_RecordsetPtr m_prsJGHZ;						//每个节点最大值
	_RecordsetPtr m_prsJOBNAME;					//不同工程名的记录集
	_RecordsetPtr m_prsBasicData;					//用来获取温度，管径的记录集
   _RecordsetPtr m_prsTempCASENUM;				//用来处理TO_NODE的临时记录集
	m_pConnSourceDB.CreateInstance(__uuidof(Connection));
	m_prsJOBNAME.CreateInstance(__uuidof(Recordset));
	m_prsCASENUM.CreateInstance(__uuidof(Recordset));

	CString m_strSQL;			
	_variant_t tmpvar;		
	long ltemp;
	int node_start;									//FROM_NODE点
	int node_end ;									//END_NODE点
	CString PSA_OutDataFUnit;						//源表的单位
	CString PSA_OutDataMUnit;

	_RecordsetPtr rsData = rsResult;
	CFile m_CAESARIIFile;							//二进制文件对象
	CString m_strCAESARIIFileName;				   //二进制文件名

//处理二进制文件所用的变量
	float buffer[1];
	long loff = 272 ;		//偏移量
	long node_startfromfile;//FROM_NODE 
	long node_endfromfile;  //TO_NODE
	long basepoint = 180;   //基本偏移量
	float tmp1;
	float tmp2;
	bool bReadinDB = true;					//判断是从数据库读取还是从二进制文件中读取,缺省从数据库中读取
//********************************
	CString m_strFilePath;				
	CString m_strTYPE;
	CString m_strJOBNAME_P;				//工程文件名*._P
	CString m_strJOBNAME_A;				//工程文件名*._A
	CString m_strGKname;				//工况名
	CString m_strRGKname;				//热工况数据来源工况名
	CString m_strLGKname;				//冷工况数据来源工况名
	CString* m_pstrTempJobname;
	CString* m_pstrTempGKname;
	float PI=3.1416f;
	int iC1 = 0;
	long iCount = 0;   //节点个数
	int nCount = 0;	   //存储节点个数,得到二进制文件中节点的个数
	int iUPxyz = 0;	   //判断向上的坐标轴，1=x，2=y，3=z；
	ltemp = maxZdjh;
	int n = 0;
	CSelPSAProjectNameDlg* m_selDlg;


	try
	{
		dbConnectionString.Format(ConnectionString,SourceDataFileName,"");
		m_pConnSourceDB->Open(_bstr_t(dbConnectionString),"","",0);

		m_strSQL.Format(_T("SELECT DISTINCT [JOBNAME] FROM [OUTPUT_LOCAL_ELEMENT_FORCES]"));
      //用此方法打开的记录集才能调用GetRecodsetCount()获得记录数
		m_prsJOBNAME->Open(_variant_t(m_strSQL),(IDispatch*)m_pConnSourceDB,adOpenKeyset,adLockOptimistic,adCmdText); 
		//在对话框的列表框中加入工程文件名,组合框加入工况名，并获取用户的选择
		m_selDlg = new CSelPSAProjectNameDlg;
 
		n = m_prsJOBNAME->GetRecordCount();

		m_selDlg->JobNameNum = n;
		m_pstrTempJobname = new CString[n];

		for ( int i=0 ;i<n;i++)
		{
			m_pstrTempJobname[i] = vtos(m_prsJOBNAME->GetCollect("JOBNAME"));
			m_prsJOBNAME->MoveNext();
		}
		m_prsJOBNAME->Close();

		m_strSQL.Empty();
		m_strSQL.Format(_T("SELECT DISTINCT [CASE] FROM [OUTPUT_LOCAL_ELEMENT_FORCES]"));
		m_prsJOBNAME->Open(_variant_t(m_strSQL),(IDispatch*)m_pConnSourceDB,adOpenKeyset,adLockOptimistic,adCmdText);
		
		n = m_prsJOBNAME->GetRecordCount();
		m_selDlg->GKNum = n;
		m_pstrTempGKname = new CString[n];
		
		for (i=0;i<n;i++)
		{
			m_pstrTempGKname[i] = vtos(m_prsJOBNAME->GetCollect("CASE"));
			m_prsJOBNAME->MoveNext();
		}

		m_selDlg->m_pstrGKName		=  m_pstrTempGKname;
		m_selDlg->m_pstrProjectName = m_pstrTempJobname;


		if ( m_selDlg->DoModal() == IDOK)
		{
			m_strJOBNAME_P = m_selDlg->m_strTempProject ;
			m_strGKname    = m_selDlg->m_strTempGK ;
			iUPxyz			= m_selDlg->iUpxyz ;
			m_strRGKname   = m_selDlg->m_strTempRGK ;
			m_strLGKname	= m_selDlg->m_strTempLGK ;
		}
		else
		{
			return;   //按下对话框的取消按钮退出提取程序
		}
		m_prsJOBNAME->Close();
		delete m_selDlg;
		delete [] m_pstrTempGKname;
		delete [] m_pstrTempJobname;


		//显示进度条
		frmStatus.ShowWindow(SW_SHOW);
		frmStatus.m_Label1= GetResStr(IDS_FROMFILE) + SourceDataFileName;
		frmStatus.m_Label2= GetResStr(IDS_TODATABASE) + EDIBgbl::dbPRJDB.GetName() + GetResStr(IDS_InTable) + EDIBgbl::TBNRawData;
		frmStatus.SetWindowText( GetResStr(IDS_DATASWITCH));
		frmStatus.UpdateData(false);
		frmStatus.UpdateStatus(0,true);

		//判断是从二进制文件还是数据库中读取管径，温度
		m_strSQL.Format(_T("SELECT * FROM [INPUT_BASIC_ELEMENT_DATA] "));
		m_prsBasicData = m_pConnSourceDB->Execute(_bstr_t(m_strSQL),NULL,adCmdText);
		if ( m_prsBasicData->BOF && m_prsBasicData->adoEOF )
			bReadinDB = false;
		m_prsBasicData->Close();

		m_strSQL.Format(_T("SELECT * FROM [OUTPUT_LOCAL_ELEMENT_FORCES] WHERE [JOBNAME] = '%s' AND   [CASE] = '%s' "),m_strJOBNAME_P,m_strRGKname);
		m_prsCASENUM->Open(_variant_t(m_strSQL),(IDispatch*)m_pConnSourceDB,adOpenKeyset,adLockOptimistic,adCmdText);
		
		iCount = m_prsCASENUM->GetRecordCount();

		//获取二进制文件名
		m_strCAESARIIFileName = m_strJOBNAME_P;
		m_strFilePath = GetFilePath(SourceDataFileName);
		m_strCAESARIIFileName = m_strFilePath + "\\" + m_strCAESARIIFileName.Left(m_strCAESARIIFileName.GetLength()-3) + "._A";
		//二进制文件扩展名为._A，它存储CAESARII管道应力分析模型原始数据
		m_strJOBNAME_A = m_strJOBNAME_P.Left(m_strJOBNAME_P.GetLength()-3)+"._A";

//读取源表中的单位符号
		tmpvar = m_prsCASENUM->GetCollect("FUNITS");
		PSA_OutDataFUnit = vtos(tmpvar);
		
		tmpvar = m_prsCASENUM->GetCollect("MUNITS");
		PSA_OutDataMUnit = vtos(tmpvar);
		

///****************************开始处理热态数据*******************************

		for (i=0; i <= iCount; i++)
		{
				tmpvar = m_prsCASENUM->GetCollect("FROM_NODE");
				node_start = vtoi(tmpvar);
				if(i == 0)
					node_end = node_start;
	//***********************处理上条记录的TO_NODE点*******************
				if (node_start != node_end ) 
				{//不相等则判断ZA表中有无次点记录，无则加入，有则修改
					rsData->Filter = _variant_t(CString("[jsdbh1] = ") + ltos(node_end)+" AND [FileNameID]="+ltos(FileNameID));
					if ( rsData->BOF && rsData->adoEOF)
					{
						rsData->AddNew();
						rsData->PutCollect("VolumeID",_variant_t(EDIBgbl::SelVlmID));
						rsData->PutCollect("zdjh",_variant_t((long)(ltemp+1)));
						rsData->PutCollect("FileNameID",_variant_t(FileNameID));
						ltemp++;
						rsData->Update();					
					}
					else
					{
					}
					rsData->PutCollect("PSAver", _variant_t(strPSAVER));
					rsData->PutCollect("UPxyz",_variant_t((long)iUPxyz));
					rsData->PutCollect("JSDBH1", _variant_t((long)node_end));

					//写入结构荷载,TO_NODE点		
					m_strSQL.Empty();
					m_strSQL.Format(_T("SELECT DISTINCT [NODE] ,max([RESULTANTF]) AS PJG FROM [OUTPUT_RESTRAINTS] WHERE [NODE] = %d AND [JOBNAME] = '%s' GROUP BY [NODE] "),node_end,m_strJOBNAME_P);
					m_prsJGHZ = m_pConnSourceDB->Execute(_bstr_t(m_strSQL),NULL,adCmdText);
					if(m_prsJGHZ->adoEOF && m_prsJGHZ->BOF)
						;
					else
						rsData->PutCollect("PJG1",m_prsJGHZ->GetCollect("PJG"));		

					rsData->PutCollect("Type", _variant_t(m_strTYPE));
					
					//判断源表的单位制，写入ZA表中			 				 
					if( PSA_OutDataFUnit  == "N." )
						rsData->PutCollect("Unit_Force", "N");
					else
						rsData->PutCollect("Unit_Force", "kgf");

					if (PSA_OutDataMUnit == "N.m.")
						rsData->PutCollect("Unit_Momental", "N.m");
					else
						rsData->PutCollect("Unit_Momental", "kgf.m");
					
					//写入力和力矩					
					m_strSQL.Empty();
					m_strSQL.Format(_T("SELECT * FROM [OUTPUT_LOCAL_ELEMENT_FORCES] WHERE [TO_NODE] = %d AND [CASE] = '%s' "),node_end,m_strRGKname);
					m_prsTempCASENUM = m_pConnSourceDB->Execute(_bstr_t(m_strSQL),NULL,adCmdText);
					rsData->PutCollect("pxr1",_variant_t(0-(vtof(m_prsTempCASENUM->GetCollect("FXT")))));
					rsData->PutCollect("pyr1",_variant_t(0-(vtof(m_prsTempCASENUM->GetCollect("FYT")))));
					rsData->PutCollect("pzr1",_variant_t(0-(vtof(m_prsTempCASENUM->GetCollect("FZT")))));

					rsData->PutCollect("mxr1",_variant_t(0-(vtof(m_prsTempCASENUM->GetCollect("MXT")))));
					rsData->PutCollect("myr1",_variant_t(0-(vtof(m_prsTempCASENUM->GetCollect("MYT")))));
					rsData->PutCollect("mzr1",_variant_t(0-(vtof(m_prsTempCASENUM->GetCollect("MZT")))));

					m_strSQL.Format(_T("SELECT * FROM [OUTPUT_HANGERS] WHERE [NODE] = %d AND [JOBNAME] =  '%s'"),node_end,m_strJOBNAME_P);
					m_prsHangers = m_pConnSourceDB->Execute(_bstr_t(m_strSQL),NULL,adCmdText);
					if ( m_prsHangers->BOF && m_prsHangers->adoEOF )
					{
					}
					else
					{	
					// 弹簧支吊架数据
						CString strSize;
						CString strTemp;
						rsData->PutCollect("pgz1",m_prsHangers->GetCollect("HOT_LOAD"));
						rsData->PutCollect("paz0",m_prsHangers->GetCollect("TH_INSTALL_LOAD"));
						rsData->PutCollect("paz1",m_prsHangers->GetCollect("AC_INSTALL_LOAD"));
						rsData->PutCollect("CoefOfVarLoad",_variant_t(vtof(m_prsHangers->GetCollect("LOAD_VARIATION"))/100));
						tmpvar = m_prsHangers->GetCollect("SIZE");
						strSize.Format(_T("%02d"),vtoi(tmpvar));
					//形成类似GLIF定义的支吊架类型号								
						strTemp = vtos(m_prsHangers->GetCollect("NUMREQ"))+"1"+strSize;
						rsData->PutCollect("TYPE",_variant_t(strTemp));							
					}
					//从输出位移表OUTPUT_DISPLACEMENTS中查找FROM_NODE及工况名CASE相等的记录,使字段DX->字段dxr1,字段DY->字段dyr1,字段DZ->字段dzr1					
					m_strSQL.Format(_T("SELECT * FROM [OUTPUT_DISPLACEMENTS] WHERE [NODE] = %d AND [CASE] = '%s' AND [JOBNAME] =  '%s'"),node_end,m_strRGKname,m_strJOBNAME_P);			
					m_prsDisplacements = m_pConnSourceDB->Execute(_bstr_t(m_strSQL),NULL,adCmdText);
					if ( !(m_prsDisplacements->BOF && m_prsDisplacements->adoEOF) )
					{
						rsData->PutCollect("xr1",m_prsDisplacements->GetCollect("DX"));
						rsData->PutCollect("yr1",m_prsDisplacements->GetCollect("DY"));
						rsData->PutCollect("zr1",m_prsDisplacements->GetCollect("DZ"));

						tmpvar = m_prsDisplacements->GetCollect("DUNITS");
						if (vtos(tmpvar) == "mm")
							rsData->PutCollect("UNIT_Displacement",_variant_t(tmpvar));
						
						tmpvar = m_prsDisplacements->GetCollect("RUNITS");
						if (vtos(tmpvar) == "deg.")
						{
							 tmpvar = m_prsDisplacements->GetCollect("RX");
							 rsData->PutCollect("rxr1",_variant_t(vtof(m_prsDisplacements->GetCollect("RY"))/180*PI));
							 rsData->PutCollect("ryr1",_variant_t(vtof(m_prsDisplacements->GetCollect("RY"))/180*PI));					 
							 rsData->PutCollect("rzr1",_variant_t(vtof(m_prsDisplacements->GetCollect("RZ"))/180*PI));	
						}
						else
						{
							rsData->PutCollect("rxr1",m_prsDisplacements->GetCollect("RX"));
							rsData->PutCollect("ryr1",m_prsDisplacements->GetCollect("RY"));
							rsData->PutCollect("rzr1",m_prsDisplacements->GetCollect("RZ"));
						}
					}
					else
					{
					}
					//从[INPUT_BASIC_ELEMENT_DATA]表中提取温度，管径写入ZA表中	TO_NODE点				

					if ( bReadinDB )
					{
						m_strSQL.Empty();
						m_strSQL.Format(_T("SELECT * FROM [INPUT_BASIC_ELEMENT_DATA] WHERE [JOBNAME] =  '%s' AND [TO_NODE] = %d"),m_strJOBNAME_A,node_end);
						m_prsBasicData = m_pConnSourceDB->Execute(_bstr_t(m_strSQL),NULL,adCmdText);

						if (!(m_prsBasicData->BOF && m_prsBasicData->adoEOF))
						{
							rsData->PutCollect("DJ1",m_prsBasicData->GetCollect("DIAMETER"));
							rsData->PutCollect("T01",m_prsBasicData->GetCollect("TEMP_EXP_C1"));
						}
						else
						{
						}
						m_prsBasicData->Close();
					}
					else
					{
						//从二进制文件中读取
						//读取CAESARII的二进制文件，从中获取节点号，管径，温度，写入ZA表相应节点号的管径，温度字段中
						try
						{
							if ( !FileExists(m_strCAESARIIFileName))
								; 
							else
							{
								m_CAESARIIFile.Open(m_strCAESARIIFileName,CFile::modeRead);
								
								for (int i = 0;i<nCount;i++)
								{
									m_CAESARIIFile.Seek(basepoint+4+(i*loff),CFile::begin);
									m_CAESARIIFile.Read(buffer,4);
									node_endfromfile = (long)buffer[0];
									rsData->Filter = _variant_t(CString("[jsdbh1] = ") + ltos(node_endfromfile) +" AND "+CString("[FileNameID]=")+ltos(FileNameID));;
									if (!(rsData->BOF && rsData->adoEOF))
									{
										m_CAESARIIFile.Seek(basepoint+20+(i*loff),CFile::begin);//读取管径
										m_CAESARIIFile.Read(buffer,4);
										if ( buffer[0] > 0 )
											tmp1 = buffer[0];
										else 
											buffer[0] = tmp1;
										rsData->PutCollect("DJ1",_variant_t(buffer[0]));			
										
										m_CAESARIIFile.Seek(basepoint+36+(i*loff),CFile::begin);//读取温度  
										m_CAESARIIFile.Read(buffer,4);
										if ( buffer[0] > 0 )
											tmp2 = buffer[0];
										else
											buffer[0] = tmp2;
										rsData->PutCollect("T01",_variant_t(buffer[0]));		
									}
									rsData->Filter = "";
								}
								m_CAESARIIFile->Close();
							}

						}catch(CFileException *e)
						{
							e->ReportError ();
						}

					}
					m_prsDisplacements->Close();
					m_prsTempCASENUM->Close();
					m_prsHangers->Close();
					m_prsJGHZ->Close();
				
				}//end if


				
	//	/***********************以下是处理这条记录的FROM_NODE点**********************
				if (i!=iCount)   //最后一次循环时不用处理FROM_NODE点，上一次已经处理了
				{
					rsData->Filter = _variant_t(CString("[jsdbh1] = ") + ltos(node_start)+" AND [FileNameID]="+ltos(FileNameID));
					if ( rsData->BOF && rsData->adoEOF )
					{
						rsData->AddNew();
						// rsData->PutCollect("UPxyz",_variant_t((long)giUPxyz));
						rsData->PutCollect("VolumeID",_variant_t(EDIBgbl::SelVlmID));
						rsData->PutCollect("zdjh",_variant_t((long)(ltemp+1)));
						rsData->PutCollect("FileNameID",_variant_t(FileNameID));
						ltemp++;
						rsData->Update();					
					}
					else{
					}

					rsData->PutCollect("PSAver", _variant_t(strPSAVER));
					rsData->PutCollect("UPxyz",_variant_t((long)iUPxyz));
					rsData->PutCollect("JSDBH1", _variant_t((long)node_start));					
					//写入结构荷载，FROM_NODE点

					m_strSQL.Empty();
					m_strSQL.Format(_T("SELECT DISTINCT [NODE] ,max([RESULTANTF]) AS PJG FROM [OUTPUT_RESTRAINTS] WHERE [NODE] = %d AND [JOBNAME] = '%s' GROUP BY [NODE] "),node_start,m_strJOBNAME_P);
					m_prsJGHZ = m_pConnSourceDB->Execute(_bstr_t(m_strSQL),NULL,adCmdText);
					if(m_prsJGHZ->adoEOF && m_prsJGHZ->BOF)
						;
					else
						rsData->PutCollect("PJG1",m_prsJGHZ->GetCollect("PJG"));					

					rsData->PutCollect("Type", _variant_t(m_strTYPE));


					//从[INPUT_BASIC_ELEMENT_DATA]表中提取温度，管径写入ZA表中
					if ( bReadinDB )
					{
						m_strSQL.Empty();
						m_strSQL.Format(_T("SELECT * FROM [INPUT_BASIC_ELEMENT_DATA] WHERE [JOBNAME] =  '%s' AND [FROM_NODE] = %d"),m_strJOBNAME_A,node_start);
						m_prsBasicData = m_pConnSourceDB->Execute(_bstr_t(m_strSQL),NULL,adCmdText);
						if (!(m_prsBasicData->BOF && m_prsBasicData->adoEOF))
						{
							rsData->PutCollect("DJ1",m_prsBasicData->GetCollect("DIAMETER"));
							rsData->PutCollect("T01",m_prsBasicData->GetCollect("TEMP_EXP_C1"));
						}
						else
						{
						}
						m_prsBasicData->Close();
					}
					else
					{
						//从二进制文件中读取
						//读取CAESARII的二进制文件，从中获取节点号，管径，温度，写入ZA表相应节点号的管径，温度字段中
						try
						{
							if ( !FileExists(m_strCAESARIIFileName))
								;
							else
							{
								m_CAESARIIFile.Open(m_strCAESARIIFileName,CFile::modeRead);
								
								for (int i = 0;i<nCount;i++)
								{
									m_CAESARIIFile.Seek(basepoint+(i*loff),CFile::begin);
									m_CAESARIIFile.Read(buffer,4);
									node_startfromfile = (long)buffer[0];
									rsData->Filter = _variant_t(CString("[jsdbh1] = ") + ltos(node_startfromfile) +" AND "+CString("[FileNameID]=")+ltos(FileNameID));;
									if (!(rsData->BOF && rsData->adoEOF))
									{
										m_CAESARIIFile.Seek(basepoint+20+(i*loff),CFile::begin);//读取管径
										m_CAESARIIFile.Read(buffer,4);
										if ( buffer[0] > 0 )
											tmp1 = buffer[0];
										else 
											buffer[0] = tmp1;
										rsData->PutCollect("DJ1",_variant_t(buffer[0]));			
										
										m_CAESARIIFile.Seek(basepoint+36+(i*loff),CFile::begin);//读取温度  
										m_CAESARIIFile.Read(buffer,4);
										if ( buffer[0] > 0 )
											tmp2 = buffer[0];
										else
											buffer[0] = tmp2;
										rsData->PutCollect("T01",_variant_t(buffer[0]));		
									}
									rsData->Filter = "";
								}
								m_CAESARIIFile->Close();
							}

						}catch(CFileException *e)
						{
							e->ReportError ();
						}

					}
					
					//判断源表的单位制，写入ZA表中			 				 
					if( PSA_OutDataFUnit  == "N." )
						rsData->PutCollect("Unit_Force", "N");
					else
						rsData->PutCollect("Unit_Force", "kgf");
					if (PSA_OutDataMUnit == "N.m.")
						rsData->PutCollect("Unit_Momental", "N.m");
					else
						rsData->PutCollect("Unit_Momental", "kgf.m");


					rsData->PutCollect("pxr1",m_prsCASENUM->GetCollect("FXF"));
					rsData->PutCollect("pyr1",m_prsCASENUM->GetCollect("FYF"));
					rsData->PutCollect("pzr1",m_prsCASENUM->GetCollect("FZF"));
					
					rsData->PutCollect("mxr1",m_prsCASENUM->GetCollect("MXF"));
					rsData->PutCollect("myr1",m_prsCASENUM->GetCollect("MYF"));
					rsData->PutCollect("mzr1",m_prsCASENUM->GetCollect("MZF"));

					//从输出弹簧表OUTPUT_HANGERS中查找NODE=FROM_NODE记录，使pgz1=HOT_LOAD,paz0=TH_INSTALL_LOAD,paz1=AC_INSTALL_LOAD,

					m_strSQL.Empty();			
					m_strSQL.Format(_T("SELECT * FROM [OUTPUT_HANGERS] WHERE [NODE] = %d AND [JOBNAME] =  '%s'"),node_start,m_strJOBNAME_P);

					m_prsHangers = m_pConnSourceDB->Execute(_bstr_t(m_strSQL),NULL,adCmdText);
					if ( m_prsHangers->BOF && m_prsHangers->adoEOF)
					{
					}
					else
					{	
						// 弹簧支吊架数据
						CString strSize;
						CString strTemp;
						rsData->PutCollect("pgz1",m_prsHangers->GetCollect("HOT_LOAD"));
						rsData->PutCollect("paz0",m_prsHangers->GetCollect("TH_INSTALL_LOAD"));
						rsData->PutCollect("paz1",m_prsHangers->GetCollect("AC_INSTALL_LOAD"));
						rsData->PutCollect("CoefOfVarLoad",_variant_t(vtof(m_prsHangers->GetCollect("LOAD_VARIATION"))/100));
						tmpvar = m_prsHangers->GetCollect("SIZE");
						strSize.Format(_T("%02d"),vtoi(tmpvar));
					//形成类似GLIF定义的支吊架类型号								
						strTemp = vtos(m_prsHangers->GetCollect("NUMREQ"))+"1"+strSize;
						rsData->PutCollect("TYPE",_variant_t(strTemp));							
					}

				//从输出位移表OUTPUT_DISPLACEMENTS中查找FROM_NODE及工况名CASE相等的记录,使字段DX->字段dxr1,字段DY->字段dyr1,字段DZ->字段dzr1,

				m_strSQL.Empty();
				m_strSQL.Format(_T("SELECT * FROM [OUTPUT_DISPLACEMENTS] WHERE [NODE] = %d AND [CASE] = '%s' AND [JOBNAME] =  '%s'"),node_start,m_strRGKname,m_strJOBNAME_P);
				m_prsDisplacements = m_pConnSourceDB->Execute(_bstr_t(m_strSQL),NULL,adCmdText);
				if ( !(m_prsDisplacements->BOF && m_prsDisplacements->adoEOF) )
				{
					rsData->PutCollect("xr1",m_prsDisplacements->GetCollect("DX"));
					rsData->PutCollect("yr1",m_prsDisplacements->GetCollect("DY"));
					rsData->PutCollect("zr1",m_prsDisplacements->GetCollect("DZ"));

					tmpvar = m_prsDisplacements->GetCollect("DUNITS");
					if (vtos(tmpvar) == "mm")
						rsData->PutCollect("UNIT_Displacement",_variant_t(tmpvar));
					
					tmpvar = m_prsDisplacements->GetCollect("RUNITS");
					if (vtos(tmpvar) == "deg.")
					{
						 tmpvar = m_prsDisplacements->GetCollect("RX");
						 rsData->PutCollect("rxr1",_variant_t(vtof(m_prsDisplacements->GetCollect("RY"))/180*PI));
						 rsData->PutCollect("ryr1",_variant_t(vtof(m_prsDisplacements->GetCollect("RY"))/180*PI));					 
						 rsData->PutCollect("rzr1",_variant_t(vtof(m_prsDisplacements->GetCollect("RZ"))/180*PI));	
					}
					else
					{
						rsData->PutCollect("rxr1",m_prsDisplacements->GetCollect("RX"));
						rsData->PutCollect("ryr1",m_prsDisplacements->GetCollect("RY"));
						rsData->PutCollect("rzr1",m_prsDisplacements->GetCollect("RZ"));
					}
				}
				else
				{
				}	
 				m_prsDisplacements->Close();
				m_prsHangers->Close();
				m_prsJGHZ->Close();
			}
			//保存本条记录的TO_NODE点，用于与下一条记录的FROM_NODE比较
			node_end = vtoi(m_prsCASENUM->GetCollect("TO_NODE"));  

			if ( i < iCount-1 )
				m_prsCASENUM->MoveNext();
			
			iC1++;
			frmStatus.UpdateStatus((float)iC1/(float)iCount,true);
			frmStatus.UpdateWindow();
			rsData->Filter = "";
			
		   
			nCount++;
		}//  end  for
		m_prsCASENUM->Close();

      m_strSQL.Empty();
		//获取冷态记录集
		m_strSQL.Format(_T("SELECT * FROM [OUTPUT_LOCAL_ELEMENT_FORCES] WHERE [CASE] = '%s' AND [JOBNAME] =  '%s'"),m_strLGKname,m_strJOBNAME_P);
		m_prsCASENUM = m_pConnSourceDB->Execute(_bstr_t(m_strSQL),NULL,adCmdText);
		node_end = -1;
				
//******************************开始进行冷态处理*******************************		
		for (i=0; i<= iCount; i++)
		{
				tmpvar = m_prsCASENUM->GetCollect("FROM_NODE");
				node_start = vtoi(tmpvar);
				if (i==0)
					node_end = node_start;
	//	/***********************处理TO_NODE记录*************************
				if (node_start != node_end ) 
				{
					//不相等则过滤node_end点(一定存在,热态已经处理)加入相应的冷态字段中去
					
					rsData->Filter = _variant_t(CString("[jsdbh1] = ") + ltos(node_end) +" AND "+CString("[FileNameID]=")+ltos(FileNameID));;
										
					//写入力和力矩					
					m_strSQL.Empty();
					m_strSQL.Format(_T("SELECT * FROM [OUTPUT_LOCAL_ELEMENT_FORCES] WHERE [TO_NODE] = %d AND [CASE] = '%s' "),node_end,m_strLGKname);
					m_prsTempCASENUM = m_pConnSourceDB->Execute(_bstr_t(m_strSQL),NULL,adCmdText);
					
					m_strSQL.Empty();
					m_strSQL.Format(_T("SELECT * FROM [OUTPUT_RESTRAINTS] WHERE [NODE] = %d AND [JOBNAME] = '%s' AND [CASE] = '%s' AND [TYPE] = 'Rigid ANC' "),node_end,m_strJOBNAME_P,m_strGKname);	
					m_prsRestraints = m_pConnSourceDB->Execute(_bstr_t(m_strSQL),NULL,adCmdText);
					
					if (m_prsRestraints->adoEOF && m_prsRestraints->BOF)   //不是固定点
					{
						rsData->PutCollect("pxl1",_variant_t(0-(vtof(m_prsTempCASENUM->GetCollect("FXT")))));
						rsData->PutCollect("pyl1",_variant_t(0-(vtof(m_prsTempCASENUM->GetCollect("FYT")))));
						rsData->PutCollect("pzl1",_variant_t(0-(vtof(m_prsTempCASENUM->GetCollect("FZT")))));

						rsData->PutCollect("mxl1",_variant_t(0-(vtof(m_prsTempCASENUM->GetCollect("MXT")))));
						rsData->PutCollect("myl1",_variant_t(0-(vtof(m_prsTempCASENUM->GetCollect("MYT")))));
						rsData->PutCollect("mzl1",_variant_t(0-(vtof(m_prsTempCASENUM->GetCollect("MZT")))));
					}
					else																	//是固定点
					{
						rsData->PutCollect("pxl1",m_prsRestraints->GetCollect("FX"));
						rsData->PutCollect("pyl1",m_prsRestraints->GetCollect("FY"));
						rsData->PutCollect("pzl1",m_prsRestraints->GetCollect("FZ"));

						rsData->PutCollect("mxl1",m_prsRestraints->GetCollect("MX"));
						rsData->PutCollect("myl1",m_prsRestraints->GetCollect("MY"));
						rsData->PutCollect("mzl1",m_prsRestraints->GetCollect("MZ"));
					}

					//从输出位移表OUTPUT_DISPLACEMENTS中查找FROM_NODE及工况名CASE相等的记录,使字段DX->字段dxr1,字段DY->字段dyr1,字段DZ->字段dzr1					
					
					if(m_prsRestraints->adoEOF && m_prsRestraints->BOF)   //如果不是固定点
					{
						m_strSQL.Format(_T("SELECT * FROM [OUTPUT_DISPLACEMENTS] WHERE [NODE] = %d AND [CASE] = '%s' AND [JOBNAME] =  '%s' "),node_end,m_strLGKname,m_strJOBNAME_P);						
						m_prsDisplacements = m_pConnSourceDB->Execute(_bstr_t(m_strSQL),NULL,adCmdText);
					}
					else     //是固定点
					{
						m_strSQL.Format(_T("SELECT * FROM [OUTPUT_DISPLACEMENTS] WHERE [CASE] = '%s' AND [JOBNAME] = '%s' AND [NODE] = %d"),m_strGKname,m_strJOBNAME_P,node_end);
						m_prsDisplacements = m_pConnSourceDB->Execute(_bstr_t(m_strSQL),NULL,adCmdText);
					}
							
					if ( !(m_prsDisplacements->BOF && m_prsDisplacements->adoEOF))
					{
						rsData->PutCollect("xl1",m_prsDisplacements->GetCollect("DX"));
						rsData->PutCollect("yl1",m_prsDisplacements->GetCollect("DY"));
						rsData->PutCollect("zl1",m_prsDisplacements->GetCollect("DZ"));

						tmpvar = m_prsDisplacements->GetCollect("DUNITS");
						if (vtos(tmpvar) == "mm")
							rsData->PutCollect("UNIT_Displacement",_variant_t(tmpvar));
						
						tmpvar = m_prsDisplacements->GetCollect("RUNITS");
						if (vtos(tmpvar) == "deg.")
						{
							 rsData->PutCollect("rxl1",_variant_t(vtof(m_prsDisplacements->GetCollect("RX"))/180*PI));
							 rsData->PutCollect("ryl1",_variant_t(vtof(m_prsDisplacements->GetCollect("RY"))/180*PI));					 
							 rsData->PutCollect("rzl1",_variant_t(vtof(m_prsDisplacements->GetCollect("RZ"))/180*PI));	
						}
						else
						{
							rsData->PutCollect("rxl1",m_prsDisplacements->GetCollect("RX"));
							rsData->PutCollect("ryl1",m_prsDisplacements->GetCollect("RY"));
							rsData->PutCollect("rzl1",m_prsDisplacements->GetCollect("RZ"));
						}
					}
					else
					{
					}
					m_prsDisplacements->Close();
					m_prsTempCASENUM->Close();
					m_prsRestraints->Close();				
					rsData->Filter = "";
				}//end if				
				
//	/*********************处理FROM_NODE点**********************
				if( i != iCount )
				{
					rsData->Filter = _variant_t(CString("[jsdbh1] = ") + ltos(node_start) +" AND "+CString("[FileNameID]=")+ltos(FileNameID));;
					m_strSQL.Empty();
					m_strSQL.Format(_T("SELECT * FROM [OUTPUT_RESTRAINTS] WHERE [NODE] = %d AND [JOBNAME] = '%s' AND [CASE] = '%s' AND [TYPE] = 'Rigid ANC' "),node_start,m_strJOBNAME_P,m_strGKname);	
					m_prsRestraints = m_pConnSourceDB->Execute(_bstr_t(m_strSQL),NULL,adCmdText);
					
					if (m_prsRestraints->adoEOF && m_prsRestraints->BOF) //是固定点 
					{
						rsData->PutCollect("pxl1",m_prsCASENUM->GetCollect("FXF"));
						rsData->PutCollect("pyl1",m_prsCASENUM->GetCollect("FYF"));
						rsData->PutCollect("pzl1",m_prsCASENUM->GetCollect("FZF"));

						rsData->PutCollect("mxl1",m_prsCASENUM->GetCollect("MXF"));
						rsData->PutCollect("myl1",m_prsCASENUM->GetCollect("MYF"));
						rsData->PutCollect("mzl1",m_prsCASENUM->GetCollect("MZF"));
					}
					else																  //不是固定点
					{
						rsData->PutCollect("pxl1",m_prsRestraints->GetCollect("FX"));
						rsData->PutCollect("pyl1",m_prsRestraints->GetCollect("FY"));
						rsData->PutCollect("pzl1",m_prsRestraints->GetCollect("FZ"));

						rsData->PutCollect("mxl1",m_prsRestraints->GetCollect("MX"));
						rsData->PutCollect("myl1",m_prsRestraints->GetCollect("MY"));
						rsData->PutCollect("mzl1",m_prsRestraints->GetCollect("MZ"));
					}
				
				//从输出位移表OUTPUT_DISPLACEMENTS中查找FROM_NODE及工况号CASE相等的记录,使字段DX->字段dxr1,字段DY->字段dyr1,字段DZ->字段dzr1,
				m_strSQL.Empty();
				if(m_prsRestraints->adoEOF && m_prsRestraints->BOF)   //如果不是固定点
				{
					m_strSQL.Format(_T("SELECT * FROM [OUTPUT_DISPLACEMENTS] WHERE [NODE] = %d AND [CASE] = '%s' AND [JOBNAME] =  '%s' "),node_start,m_strLGKname,m_strJOBNAME_P);
					m_prsDisplacements = m_pConnSourceDB->Execute(_bstr_t(m_strSQL),NULL,adCmdText);
				}
				else     //是固定点
				{
					m_strSQL.Format(_T("SELECT * FROM [OUTPUT_DISPLACEMENTS] WHERE [CASE] = '%s' AND [JOBNAME] = '%s' AND [NODE] = %d"),m_strGKname,m_strJOBNAME_P,node_start);
					m_prsDisplacements = m_pConnSourceDB->Execute(_bstr_t(m_strSQL),NULL,adCmdText);
				}

				if ( !(m_prsDisplacements->BOF && m_prsDisplacements->adoEOF))
				{
					rsData->PutCollect("xl1",m_prsDisplacements->GetCollect("DX"));
					rsData->PutCollect("yl1",m_prsDisplacements->GetCollect("DY"));
					rsData->PutCollect("zl1",m_prsDisplacements->GetCollect("DZ"));

					tmpvar = m_prsDisplacements->GetCollect("DUNITS");
					if (vtos(tmpvar) == "mm")
						rsData->PutCollect("UNIT_Displacement",_variant_t(tmpvar));
					
					tmpvar = m_prsDisplacements->GetCollect("RUNITS");
					if (vtos(tmpvar) == "deg.")
					{
						 rsData->PutCollect("rxl1",_variant_t(vtof(m_prsDisplacements->GetCollect("RX"))/180*PI));
						 rsData->PutCollect("ryl1",_variant_t(vtof(m_prsDisplacements->GetCollect("RY"))/180*PI));					 
						 rsData->PutCollect("rzl1",_variant_t(vtof(m_prsDisplacements->GetCollect("RZ"))/180*PI));	
					}
					else
					{
						rsData->PutCollect("rxl1",m_prsDisplacements->GetCollect("RX"));
						rsData->PutCollect("ryl1",m_prsDisplacements->GetCollect("RY"));
						rsData->PutCollect("rzl1",m_prsDisplacements->GetCollect("RZ"));
					}
				}
				else
				{
				}
			m_prsDisplacements->Close();
			m_prsRestraints->Close();
			}	
			//保存这次记录的TO_NODE点，与下一条记录的FROM_NODE比较			
			node_end = vtoi(m_prsCASENUM->GetCollect("TO_NODE"));

			if ( i < iCount-1 )
				m_prsCASENUM->MoveNext();
			
			rsData->Filter = "";
				
		}//  end  for
		m_prsCASENUM = NULL;					
		m_prsHangers = NULL;			
		m_prsDisplacements = NULL;	
		m_prsRestraints = NULL;		
		m_prsJGHZ = NULL;				
		m_prsJOBNAME = NULL;		
		m_prsBasicData = NULL;		
		m_prsTempCASENUM = NULL;		

		m_pConnSourceDB->Close();
		m_pConnSourceDB = NULL;
		
	}//   end   try
	catch(CException *e)
	{
		e->ReportError();
		e->Delete();
	}

}
*/


void  ReadResult_CAESARII42(_Recordset* rsResult, CString SourceDataFileName,long maxZdjh,long FileNameID)
{
	const char* strPSAVER = "CASEARII 4.20";  
	CString ConnectionString="Provider=Microsoft.Jet.OLEDB.4.0;Persist Security Info=False;Data Source=%s;Jet OLEDB:Database Password=%s";
	CString dbConnectionString;
   
	_ConnectionPtr m_pConnSourceDB;			   //源数据库的连接对象
	_RecordsetPtr m_prsCASENUM;					//热态（冷态）的记录集
	_RecordsetPtr m_prsHangers;					//表[OUTPUT_HANGERS]记录集
	_RecordsetPtr m_prsDisplacements;			//表[OUTPUT_DISPLACEMENTS]记录集
	_RecordsetPtr m_prsRestraints;				//表[OUTPUT_RESTRAINTS]记录集
	_RecordsetPtr m_prsJGHZ;						//每个节点最大值
	_RecordsetPtr m_prsJOBNAME;					//不同工程名的记录集
	_RecordsetPtr m_prsBasicData;					//用来获取温度，管径的记录集
   _RecordsetPtr m_prsTempCASENUM;				//用来处理TO_NODE的临时记录集
	m_pConnSourceDB.CreateInstance(__uuidof(Connection));
	m_prsJOBNAME.CreateInstance(__uuidof(Recordset));
	m_prsCASENUM.CreateInstance(__uuidof(Recordset));

	CString m_strSQL;			
	_variant_t tmpvar;		
	long ltemp;
	int node_start;									//FROM_NODE点
	int node_end ;									//END_NODE点
	CString PSA_OutDataFUnit;						//源表的单位
	CString PSA_OutDataMUnit;
	_RecordsetPtr rsData = rsResult;
	CFile m_CAESARIIFile;							//二进制文件对象
	CString m_strCAESARIIFileName;				//二进制文件名
//处理二进制文件所用的变量
	float buffer[1];
	long loff = 272 ;			//偏移量
	long node_startfromfile;//FROM_NODE 
	long node_endfromfile;  //TO_NODE
	long basepoint = 180;   //基本偏移量
	float tmp1;
	float tmp2;
	bool bReadinDB = true;					//判断是从数据库读取还是从二进制文件中读取,缺省从数据库中读取
/*********************************/
	CString m_strFilePath;				
	CString m_strTYPE;
	CString m_strJOBNAME_P;				//工程文件名*._P
	CString m_strJOBNAME_A;				//工程文件名*._A
	CString m_strGKname;					//工况名
	CString m_strRGKname;				//热工况数据来源工况名
	CString m_strLGKname;				//冷工况数据来源工况名
	CString* m_pstrTempJobname;
	CString* m_pstrTempGKname;
	float PI=3.1416f;
	int iC1 = 0;
	long iCount = 0;	//节点个数
	int nCount = 0;	//存储节点个数,得到二进制文件中节点的个数
	int iUPxyz = 0;	//判断向上的坐标轴，1=x，2=y，3=z；
	ltemp = maxZdjh;//最大支吊架号
	int n = 0;
	CSelPSAProjectNameDlg* m_selDlg;
	
	try
	{
		dbConnectionString.Format(ConnectionString,SourceDataFileName,"");
		m_pConnSourceDB->Open(_bstr_t(dbConnectionString),"","",0);

		m_strSQL.Format(_T("SELECT DISTINCT [JOBNAME] FROM [OUTPUT_LOCAL_ELEMENT_FORCES]"));
      //用此方法打开的记录集才能调用GetRecodsetCount()获得记录数
		m_prsJOBNAME->Open(_variant_t(m_strSQL),(IDispatch*)m_pConnSourceDB,adOpenKeyset,adLockOptimistic,adCmdText); 
		//在对话框的列表框中加入工程文件名,组合框加入工况名，并获取用户的选择
		m_selDlg = new CSelPSAProjectNameDlg;
 
		n = m_prsJOBNAME->GetRecordCount();

		m_selDlg->JobNameNum = n;
		m_pstrTempJobname = new CString[n];

		for ( int i=0 ;i<n;i++)
		{
			m_pstrTempJobname[i] = vtos(m_prsJOBNAME->GetCollect("JOBNAME"));
			m_prsJOBNAME->MoveNext();
		}
		m_prsJOBNAME->Close();

		m_strSQL.Empty();
		m_strSQL.Format(_T("SELECT DISTINCT [CASE] FROM [OUTPUT_LOCAL_ELEMENT_FORCES]"));
		m_prsJOBNAME->Open(_variant_t(m_strSQL),(IDispatch*)m_pConnSourceDB,adOpenKeyset,adLockOptimistic,adCmdText);
		
		n = m_prsJOBNAME->GetRecordCount();
		m_selDlg->GKNum = n;
		m_pstrTempGKname = new CString[n];
		
		for (i=0;i<n;i++)
		{
			m_pstrTempGKname[i] = vtos(m_prsJOBNAME->GetCollect("CASE"));
			m_prsJOBNAME->MoveNext();
		}

		m_selDlg->m_pstrGKName		=  m_pstrTempGKname;
		m_selDlg->m_pstrProjectName = m_pstrTempJobname;


		if ( m_selDlg->DoModal() == IDOK)
		{
			m_strJOBNAME_P = m_selDlg->m_strTempProject ;
			m_strGKname    = m_selDlg->m_strTempGK ;
			iUPxyz			= m_selDlg->iUpxyz ;
			m_strRGKname   = m_selDlg->m_strTempRGK ;
			m_strLGKname	= m_selDlg->m_strTempLGK ;
		}
		else
		{
			return;   //按下对话框的取消按钮退出提取程序
		}
		m_prsJOBNAME->Close();
		delete m_selDlg;
		delete [] m_pstrTempGKname;
		delete [] m_pstrTempJobname;


		//显示进度条
		frmStatus.ShowWindow(SW_SHOW);
		frmStatus.m_Label1= GetResStr(IDS_FROMFILE) + SourceDataFileName;
		frmStatus.m_Label2= GetResStr(IDS_TODATABASE) + EDIBgbl::GetDBName(EDIBgbl::dbPRJDB) + GetResStr(IDS_InTable) + EDIBgbl::TBNRawData;
		frmStatus.SetWindowText( GetResStr(IDS_DATASWITCH));
		frmStatus.UpdateData(false);
		frmStatus.UpdateStatus(0,true);

		//判断是从二进制文件还是数据库中读取管径，温度
		m_strSQL.Format(_T("SELECT * FROM [INPUT_BASIC_ELEMENT_DATA] "));
		m_prsBasicData = m_pConnSourceDB->Execute(_bstr_t(m_strSQL),NULL,adCmdText);
		if ( m_prsBasicData->BOF && m_prsBasicData->adoEOF )
			bReadinDB = false;
		m_prsBasicData->Close();

		m_strSQL.Format(_T("SELECT * FROM [OUTPUT_LOCAL_ELEMENT_FORCES] WHERE [JOBNAME] = '%s' AND   [CASE] = '%s' "),m_strJOBNAME_P,m_strRGKname);
		m_prsCASENUM->Open(_variant_t(m_strSQL),(IDispatch*)m_pConnSourceDB,adOpenKeyset,adLockOptimistic,adCmdText);
		
		iCount = m_prsCASENUM->GetRecordCount();

		//获取二进制文件名
		m_strCAESARIIFileName = m_strJOBNAME_P;
		m_strFilePath = GetFilePath(SourceDataFileName);
		m_strCAESARIIFileName = m_strFilePath + "\\" + m_strCAESARIIFileName.Left(m_strCAESARIIFileName.GetLength()-3) + "._A";
		//二进制文件扩展名为._A，它存储CAESARII管道应力分析模型原始数据
		m_strJOBNAME_A = m_strJOBNAME_P.Left(m_strJOBNAME_P.GetLength()-3)+"._A";

//读取源表中的单位符号
		tmpvar = m_prsCASENUM->GetCollect("FUNITS");
		PSA_OutDataFUnit = vtos(tmpvar);
		
		tmpvar = m_prsCASENUM->GetCollect("MUNITS");
		PSA_OutDataMUnit = vtos(tmpvar);
		

/****************************开始处理热态数据********************************/

		for (i=0; i <= iCount; i++)
		{
				tmpvar = m_prsCASENUM->GetCollect("FROM_NODE");
				node_start = vtoi(tmpvar);
				if(i == 0)
					node_end = node_start;
		/***********************处理上条记录的TO_NODE点********************/
				if (node_start != node_end ) 
				{//不相等则判断ZA表中有无次点记录，无则加入，有则修改
					rsData->Filter = _variant_t(CString("[jsdbh1] = ") + ltos(node_end)+" AND [FileNameID]="+ltos(FileNameID));
					if ( rsData->BOF && rsData->adoEOF)
					{
						rsData->AddNew();
						rsData->PutCollect("VolumeID",_variant_t(EDIBgbl::SelVlmID));
						rsData->PutCollect("zdjh",_variant_t((long)(ltemp+1)));
						rsData->PutCollect("FileNameID",_variant_t(FileNameID));
						ltemp++;
						rsData->Update();					
					}
					else
					{
					}
					rsData->PutCollect("PSAver", _variant_t(strPSAVER));
					rsData->PutCollect("UPxyz",_variant_t((long)iUPxyz));
					rsData->PutCollect("JSDBH1", _variant_t((long)node_end));

					//写入结构荷载,TO_NODE点		
					m_strSQL.Empty();
					m_strSQL.Format(_T("SELECT DISTINCT [NODE] ,max([RESULTANTF]) AS PJG FROM [OUTPUT_RESTRAINTS] WHERE [NODE] = %d AND [JOBNAME] = '%s' GROUP BY [NODE] "),node_end,m_strJOBNAME_P);
					m_prsJGHZ = m_pConnSourceDB->Execute(_bstr_t(m_strSQL),NULL,adCmdText);
					if(m_prsJGHZ->adoEOF && m_prsJGHZ->BOF)
						;
					else
						rsData->PutCollect("PJG1",m_prsJGHZ->GetCollect("PJG"));		

					rsData->PutCollect("Type", _variant_t(m_strTYPE));
					rsData->PutCollect("psaType", _variant_t(m_strTYPE));
					//判断源表的单位制，写入ZA表中			 				 
					if( PSA_OutDataFUnit  == "N." )
						rsData->PutCollect("Unit_Force", "N");
					else
						rsData->PutCollect("Unit_Force", "kgf");

					if (PSA_OutDataMUnit == "N.m.")
						rsData->PutCollect("Unit_Momental", "N.m");
					else
						rsData->PutCollect("Unit_Momental", "kgf.m");
					
					//写入力和力矩					
					m_strSQL.Empty();
					m_strSQL.Format(_T("SELECT * FROM [OUTPUT_LOCAL_ELEMENT_FORCES] WHERE [TO_NODE] = %d AND [CASE] = '%s' "),node_end,m_strRGKname);
					m_prsTempCASENUM = m_pConnSourceDB->Execute(_bstr_t(m_strSQL),NULL,adCmdText);
					rsData->PutCollect("pxr1",_variant_t(0-(vtof(m_prsTempCASENUM->GetCollect("FXT")))));
					rsData->PutCollect("pyr1",_variant_t(0-(vtof(m_prsTempCASENUM->GetCollect("FYT")))));
					rsData->PutCollect("pzr1",_variant_t(0-(vtof(m_prsTempCASENUM->GetCollect("FZT")))));

					rsData->PutCollect("mxr1",_variant_t(0-(vtof(m_prsTempCASENUM->GetCollect("MXT")))));
					rsData->PutCollect("myr1",_variant_t(0-(vtof(m_prsTempCASENUM->GetCollect("MYT")))));
					rsData->PutCollect("mzr1",_variant_t(0-(vtof(m_prsTempCASENUM->GetCollect("MZT")))));
					
				
					m_strSQL.Format(_T("SELECT * FROM [OUTPUT_HANGERS] WHERE [NODE] = %d AND [JOBNAME] =  '%s'"),node_end,m_strJOBNAME_P);
					m_prsHangers = m_pConnSourceDB->Execute(_bstr_t(m_strSQL),NULL,adCmdText);
					if ( m_prsHangers->BOF && m_prsHangers->adoEOF )
					{
					}
					else
					{	
					// 弹簧支吊架数据
						CString strSize;
						CString strTemp;
						rsData->PutCollect("pgz1",m_prsHangers->GetCollect("HOT_LOAD"));
						rsData->PutCollect("paz0",m_prsHangers->GetCollect("TH_INSTALL_LOAD"));
						rsData->PutCollect("paz1",m_prsHangers->GetCollect("AC_INSTALL_LOAD"));
						rsData->PutCollect("CoefOfVarLoad",_variant_t(vtof(m_prsHangers->GetCollect("LOAD_VARIATION"))/100));
						tmpvar = m_prsHangers->GetCollect("SIZE");
						strSize.Format(_T("%02d"),vtoi(tmpvar));
					//形成类似GLIF定义的支吊架类型号								
						strTemp = vtos(m_prsHangers->GetCollect("NUMREQ"))+"1"+strSize;
						rsData->PutCollect("TYPE",_variant_t(strTemp));							
						rsData->PutCollect("psaTYPE",_variant_t(strTemp));							
					}
					//从输出位移表OUTPUT_DISPLACEMENTS中查找FROM_NODE及工况名CASE相等的记录,使字段DX->字段dxr1,字段DY->字段dyr1,字段DZ->字段dzr1					
					m_strSQL.Format(_T("SELECT * FROM [OUTPUT_DISPLACEMENTS] WHERE [NODE] = %d AND [CASE] = '%s' AND [JOBNAME] =  '%s'"),node_end,m_strRGKname,m_strJOBNAME_P);			
					m_prsDisplacements = m_pConnSourceDB->Execute(_bstr_t(m_strSQL),NULL,adCmdText);
					if ( !(m_prsDisplacements->BOF && m_prsDisplacements->adoEOF) )
					{
						rsData->PutCollect("xr1",m_prsDisplacements->GetCollect("DX"));
						rsData->PutCollect("yr1",m_prsDisplacements->GetCollect("DY"));
						rsData->PutCollect("zr1",m_prsDisplacements->GetCollect("DZ"));

						tmpvar = m_prsDisplacements->GetCollect("DUNITS");
						if (vtos(tmpvar) == "mm")
							rsData->PutCollect("UNIT_Displacement",_variant_t(tmpvar));
						
						tmpvar = m_prsDisplacements->GetCollect("RUNITS");
						if (vtos(tmpvar) == "deg.")
						{
							 tmpvar = m_prsDisplacements->GetCollect("RX");
							 rsData->PutCollect("rxr1",_variant_t(vtof(m_prsDisplacements->GetCollect("RY"))/180*PI));
							 rsData->PutCollect("ryr1",_variant_t(vtof(m_prsDisplacements->GetCollect("RY"))/180*PI));					 
							 rsData->PutCollect("rzr1",_variant_t(vtof(m_prsDisplacements->GetCollect("RZ"))/180*PI));	
						}
						else
						{
							rsData->PutCollect("rxr1",m_prsDisplacements->GetCollect("RX"));
							rsData->PutCollect("ryr1",m_prsDisplacements->GetCollect("RY"));
							rsData->PutCollect("rzr1",m_prsDisplacements->GetCollect("RZ"));
						}
					}
					else
					{
					}
					//从[INPUT_BASIC_ELEMENT_DATA]表中提取温度，管径写入ZA表中	TO_NODE点				

					if ( bReadinDB )
					{
						m_strSQL.Empty();
						m_strSQL.Format(_T("SELECT * FROM [INPUT_BASIC_ELEMENT_DATA] WHERE [JOBNAME] =  '%s' AND [TO_NODE] = %d"),m_strJOBNAME_A,node_end);
						m_prsBasicData = m_pConnSourceDB->Execute(_bstr_t(m_strSQL),NULL,adCmdText);

						if (!(m_prsBasicData->BOF && m_prsBasicData->adoEOF))
						{
							rsData->PutCollect("DJ1",m_prsBasicData->GetCollect("DIAMETER"));
							rsData->PutCollect("T01",m_prsBasicData->GetCollect("TEMP_EXP_C1"));
						}
						else
						{
						}
						m_prsBasicData->Close();
					}
					else
					{
						//从二进制文件中读取
						//读取CAESARII的二进制文件，从中获取节点号，管径，温度，写入ZA表相应节点号的管径，温度字段中
						try
						{
							if ( !FileExists(m_strCAESARIIFileName))
								; 
							else
							{
								m_CAESARIIFile.Open(m_strCAESARIIFileName,CFile::modeRead);
								
								for (int i = 0;i<nCount;i++)
								{
									m_CAESARIIFile.Seek(basepoint+4+(i*loff),CFile::begin);
									m_CAESARIIFile.Read(buffer,4);
									node_endfromfile = (long)buffer[0];
									rsData->Filter = _variant_t(CString("[jsdbh1] = ") + ltos(node_endfromfile) +" AND "+CString("[FileNameID]=")+ltos(FileNameID));;
									if (!(rsData->BOF && rsData->adoEOF))
									{
										m_CAESARIIFile.Seek(basepoint+20+(i*loff),CFile::begin);//读取管径
										m_CAESARIIFile.Read(buffer,4);
										if ( buffer[0] > 0 )
											tmp1 = buffer[0];
										else 
											buffer[0] = tmp1;
										rsData->PutCollect("DJ1",_variant_t(buffer[0]));			
										
										m_CAESARIIFile.Seek(basepoint+36+(i*loff),CFile::begin);//读取温度  
										m_CAESARIIFile.Read(buffer,4);
										if ( buffer[0] > 0 )
											tmp2 = buffer[0];
										else
											buffer[0] = tmp2;
										rsData->PutCollect("T01",_variant_t(buffer[0]));		
									}
									rsData->Filter = "";
								}
								m_CAESARIIFile.Close();
							}

						}catch(CFileException *e)
						{
							e->ReportError ();
						}

					}
					m_prsDisplacements->Close();
					m_prsTempCASENUM->Close();
					m_prsHangers->Close();
					m_prsJGHZ->Close();
				
				}//end if


				
		/***********************以下是处理这条记录的FROM_NODE点***********************/
				if (i!=iCount)   //最后一次循环时不用处理FROM_NODE点，上一次已经处理了
				{
					rsData->Filter = _variant_t(CString("[jsdbh1] = ") + ltos(node_start)+" AND [FileNameID]="+ltos(FileNameID));
					if ( rsData->BOF && rsData->adoEOF )
					{
						rsData->AddNew();
						// rsData->PutCollect("UPxyz",_variant_t((long)giUPxyz));
						rsData->PutCollect("VolumeID",_variant_t(EDIBgbl::SelVlmID));
						rsData->PutCollect("zdjh",_variant_t((long)(ltemp+1)));
						rsData->PutCollect("FileNameID",_variant_t(FileNameID));
						ltemp++;
						rsData->Update();					
					}
					else{
					}

					rsData->PutCollect("PSAver", _variant_t(strPSAVER));
					rsData->PutCollect("UPxyz",_variant_t((long)iUPxyz));
					rsData->PutCollect("JSDBH1", _variant_t((long)node_start));					
					//写入结构荷载，FROM_NODE点

					m_strSQL.Empty();
					m_strSQL.Format(_T("SELECT DISTINCT [NODE] ,max([RESULTANTF]) AS PJG FROM [OUTPUT_RESTRAINTS] WHERE [NODE] = %d AND [JOBNAME] = '%s' GROUP BY [NODE] "),node_start,m_strJOBNAME_P);
					m_prsJGHZ = m_pConnSourceDB->Execute(_bstr_t(m_strSQL),NULL,adCmdText);
					if(m_prsJGHZ->adoEOF && m_prsJGHZ->BOF)
						;
					else
						rsData->PutCollect("PJG1",m_prsJGHZ->GetCollect("PJG"));					

					rsData->PutCollect("Type", _variant_t(m_strTYPE));
					rsData->PutCollect("psaType", _variant_t(m_strTYPE));

					//从[INPUT_BASIC_ELEMENT_DATA]表中提取温度，管径写入ZA表中
					if ( bReadinDB )
					{
						m_strSQL.Empty();
						m_strSQL.Format(_T("SELECT * FROM [INPUT_BASIC_ELEMENT_DATA] WHERE [JOBNAME] =  '%s' AND [FROM_NODE] = %d"),m_strJOBNAME_A,node_start);
						m_prsBasicData = m_pConnSourceDB->Execute(_bstr_t(m_strSQL),NULL,adCmdText);
						if (!(m_prsBasicData->BOF && m_prsBasicData->adoEOF))
						{
							rsData->PutCollect("DJ1",m_prsBasicData->GetCollect("DIAMETER"));
							rsData->PutCollect("T01",m_prsBasicData->GetCollect("TEMP_EXP_C1"));
						}
						else
						{
						}
						m_prsBasicData->Close();
					}
					else
					{
						//从二进制文件中读取
						//读取CAESARII的二进制文件，从中获取节点号，管径，温度，写入ZA表相应节点号的管径，温度字段中
						try
						{
							if ( !FileExists(m_strCAESARIIFileName))
								;
							else
							{
								m_CAESARIIFile.Open(m_strCAESARIIFileName,CFile::modeRead);
								
								for (int i = 0;i<nCount;i++)
								{
									m_CAESARIIFile.Seek(basepoint+(i*loff),CFile::begin);
									m_CAESARIIFile.Read(buffer,4);
									node_startfromfile = (long)buffer[0];
									rsData->Filter = _variant_t(CString("[jsdbh1] = ") + ltos(node_startfromfile) +" AND "+CString("[FileNameID]=")+ltos(FileNameID));;
									if (!(rsData->BOF && rsData->adoEOF))
									{
										m_CAESARIIFile.Seek(basepoint+20+(i*loff),CFile::begin);//读取管径
										m_CAESARIIFile.Read(buffer,4);
										if ( buffer[0] > 0 )
											tmp1 = buffer[0];
										else 
											buffer[0] = tmp1;
										rsData->PutCollect("DJ1",_variant_t(buffer[0]));			
										
										m_CAESARIIFile.Seek(basepoint+36+(i*loff),CFile::begin);//读取温度  
										m_CAESARIIFile.Read(buffer,4);
										if ( buffer[0] > 0 )
											tmp2 = buffer[0];
										else
											buffer[0] = tmp2;
										rsData->PutCollect("T01",_variant_t(buffer[0]));		
									}
									rsData->Filter = "";
								}
								m_CAESARIIFile.Close();
							}

						}catch(CFileException *e)
						{
							e->ReportError ();
						}

					}
					
					//判断源表的单位制，写入ZA表中			 				 
					if( PSA_OutDataFUnit  == "N." )
						rsData->PutCollect("Unit_Force", "N");
					else
						rsData->PutCollect("Unit_Force", "kgf");
					if (PSA_OutDataMUnit == "N.m.")
						rsData->PutCollect("Unit_Momental", "N.m");
					else
						rsData->PutCollect("Unit_Momental", "kgf.m");


					rsData->PutCollect("pxr1",m_prsCASENUM->GetCollect("FXF"));
					rsData->PutCollect("pyr1",m_prsCASENUM->GetCollect("FYF"));
					rsData->PutCollect("pzr1",m_prsCASENUM->GetCollect("FZF"));
					
					rsData->PutCollect("mxr1",m_prsCASENUM->GetCollect("MXF"));
					rsData->PutCollect("myr1",m_prsCASENUM->GetCollect("MYF"));
					rsData->PutCollect("mzr1",m_prsCASENUM->GetCollect("MZF"));

					//从输出弹簧表OUTPUT_HANGERS中查找NODE=FROM_NODE记录，使pgz1=HOT_LOAD,paz0=TH_INSTALL_LOAD,paz1=AC_INSTALL_LOAD,

					m_strSQL.Empty();			
					m_strSQL.Format(_T("SELECT * FROM [OUTPUT_HANGERS] WHERE [NODE] = %d AND [JOBNAME] =  '%s'"),node_start,m_strJOBNAME_P);

					m_prsHangers = m_pConnSourceDB->Execute(_bstr_t(m_strSQL),NULL,adCmdText);
					if ( m_prsHangers->BOF && m_prsHangers->adoEOF)
					{
					}
					else
					{	
						// 弹簧支吊架数据
						CString strSize;
						CString strTemp;
						rsData->PutCollect("pgz1",m_prsHangers->GetCollect("HOT_LOAD"));
						rsData->PutCollect("paz0",m_prsHangers->GetCollect("TH_INSTALL_LOAD"));
						rsData->PutCollect("paz1",m_prsHangers->GetCollect("AC_INSTALL_LOAD"));
						rsData->PutCollect("CoefOfVarLoad",_variant_t(vtof(m_prsHangers->GetCollect("LOAD_VARIATION"))/100));
						tmpvar = m_prsHangers->GetCollect("SIZE");
						strSize.Format(_T("%02d"),vtoi(tmpvar));
					//形成类似GLIF定义的支吊架类型号								
						strTemp = vtos(m_prsHangers->GetCollect("NUMREQ"))+"1"+strSize;
						rsData->PutCollect("TYPE",_variant_t(strTemp));							
						rsData->PutCollect("psaTYPE",_variant_t(strTemp));							
					}

				//从输出位移表OUTPUT_DISPLACEMENTS中查找FROM_NODE及工况名CASE相等的记录,使字段DX->字段dxr1,字段DY->字段dyr1,字段DZ->字段dzr1,

				m_strSQL.Empty();
				m_strSQL.Format(_T("SELECT * FROM [OUTPUT_DISPLACEMENTS] WHERE [NODE] = %d AND [CASE] = '%s' AND [JOBNAME] =  '%s'"),node_start,m_strRGKname,m_strJOBNAME_P);
				m_prsDisplacements = m_pConnSourceDB->Execute(_bstr_t(m_strSQL),NULL,adCmdText);
				if ( !(m_prsDisplacements->BOF && m_prsDisplacements->adoEOF) )
				{
					rsData->PutCollect("xr1",m_prsDisplacements->GetCollect("DX"));
					rsData->PutCollect("yr1",m_prsDisplacements->GetCollect("DY"));
					rsData->PutCollect("zr1",m_prsDisplacements->GetCollect("DZ"));

					tmpvar = m_prsDisplacements->GetCollect("DUNITS");
					if (vtos(tmpvar) == "mm")
						rsData->PutCollect("UNIT_Displacement",_variant_t(tmpvar));
					
					tmpvar = m_prsDisplacements->GetCollect("RUNITS");
					if (vtos(tmpvar) == "deg.")
					{
						 tmpvar = m_prsDisplacements->GetCollect("RX");
						 rsData->PutCollect("rxr1",_variant_t(vtof(m_prsDisplacements->GetCollect("RY"))/180*PI));
						 rsData->PutCollect("ryr1",_variant_t(vtof(m_prsDisplacements->GetCollect("RY"))/180*PI));					 
						 rsData->PutCollect("rzr1",_variant_t(vtof(m_prsDisplacements->GetCollect("RZ"))/180*PI));	
					}
					else
					{
						rsData->PutCollect("rxr1",m_prsDisplacements->GetCollect("RX"));
						rsData->PutCollect("ryr1",m_prsDisplacements->GetCollect("RY"));
						rsData->PutCollect("rzr1",m_prsDisplacements->GetCollect("RZ"));
					}
				}
				else
				{
				}	
 				m_prsDisplacements->Close();
				m_prsHangers->Close();
				m_prsJGHZ->Close();
			}
			//保存本条记录的TO_NODE点，用于与下一条记录的FROM_NODE比较
			node_end = vtoi(m_prsCASENUM->GetCollect("TO_NODE"));  

			if ( i < iCount-1 )
				m_prsCASENUM->MoveNext();
			
			iC1++;
			frmStatus.UpdateStatus((float)iC1/(float)iCount,true);
			frmStatus.UpdateWindow();
			rsData->Filter = "";
			
		   
			nCount++;
		}//  end  for
		m_prsCASENUM->Close();

      m_strSQL.Empty();
		//获取冷态记录集
		m_strSQL.Format(_T("SELECT * FROM [OUTPUT_LOCAL_ELEMENT_FORCES] WHERE [CASE] = '%s' AND [JOBNAME] =  '%s'"),m_strLGKname,m_strJOBNAME_P);
		m_prsCASENUM = m_pConnSourceDB->Execute(_bstr_t(m_strSQL),NULL,adCmdText);
		node_end = -1;
				
/******************************开始进行冷态处理********************************/		
		for (i=0; i<= iCount; i++)
		{
				tmpvar = m_prsCASENUM->GetCollect("FROM_NODE");
				node_start = vtoi(tmpvar);
				if (i==0)
					node_end = node_start;
		/***********************处理TO_NODE记录**************************/
				if (node_start != node_end ) 
				{
					//不相等则过滤node_end点(一定存在,热态已经处理)加入相应的冷态字段中去
					
					rsData->Filter = _variant_t(CString("[jsdbh1] = ") + ltos(node_end) +" AND "+CString("[FileNameID]=")+ltos(FileNameID));;
										
					//写入力和力矩					
					m_strSQL.Empty();
					m_strSQL.Format(_T("SELECT * FROM [OUTPUT_LOCAL_ELEMENT_FORCES] WHERE [TO_NODE] = %d AND [CASE] = '%s' "),node_end,m_strLGKname);
					m_prsTempCASENUM = m_pConnSourceDB->Execute(_bstr_t(m_strSQL),NULL,adCmdText);
					
					m_strSQL.Empty();
					m_strSQL.Format(_T("SELECT * FROM [OUTPUT_RESTRAINTS] WHERE [NODE] = %d AND [JOBNAME] = '%s' AND [CASE] = '%s' AND [TYPE] = 'Rigid ANC' "),node_end,m_strJOBNAME_P,m_strGKname);	
					m_prsRestraints = m_pConnSourceDB->Execute(_bstr_t(m_strSQL),NULL,adCmdText);
					
					if (m_prsRestraints->adoEOF && m_prsRestraints->BOF)   //不是固定点
					{
						rsData->PutCollect("pxl1",_variant_t(0-(vtof(m_prsTempCASENUM->GetCollect("FXT")))));
						rsData->PutCollect("pyl1",_variant_t(0-(vtof(m_prsTempCASENUM->GetCollect("FYT")))));
						rsData->PutCollect("pzl1",_variant_t(0-(vtof(m_prsTempCASENUM->GetCollect("FZT")))));

						rsData->PutCollect("mxl1",_variant_t(0-(vtof(m_prsTempCASENUM->GetCollect("MXT")))));
						rsData->PutCollect("myl1",_variant_t(0-(vtof(m_prsTempCASENUM->GetCollect("MYT")))));
						rsData->PutCollect("mzl1",_variant_t(0-(vtof(m_prsTempCASENUM->GetCollect("MZT")))));
					}
					else																	//是固定点
					{
						rsData->PutCollect("pxl1",m_prsRestraints->GetCollect("FX"));
						rsData->PutCollect("pyl1",m_prsRestraints->GetCollect("FY"));
						rsData->PutCollect("pzl1",m_prsRestraints->GetCollect("FZ"));

						rsData->PutCollect("mxl1",m_prsRestraints->GetCollect("MX"));
						rsData->PutCollect("myl1",m_prsRestraints->GetCollect("MY"));
						rsData->PutCollect("mzl1",m_prsRestraints->GetCollect("MZ"));
					}

					//从输出位移表OUTPUT_DISPLACEMENTS中查找FROM_NODE及工况名CASE相等的记录,使字段DX->字段dxr1,字段DY->字段dyr1,字段DZ->字段dzr1					
					
					if(m_prsRestraints->adoEOF && m_prsRestraints->BOF)   //如果不是固定点
					{
						m_strSQL.Format(_T("SELECT * FROM [OUTPUT_DISPLACEMENTS] WHERE [NODE] = %d AND [CASE] = '%s' AND [JOBNAME] =  '%s' "),node_end,m_strLGKname,m_strJOBNAME_P);						
						m_prsDisplacements = m_pConnSourceDB->Execute(_bstr_t(m_strSQL),NULL,adCmdText);
					}
					else     //是固定点
					{
						m_strSQL.Format(_T("SELECT * FROM [OUTPUT_DISPLACEMENTS] WHERE [CASE] = '%s' AND [JOBNAME] = '%s' AND [NODE] = %d"),m_strGKname,m_strJOBNAME_P,node_end);
						m_prsDisplacements = m_pConnSourceDB->Execute(_bstr_t(m_strSQL),NULL,adCmdText);
					}
							
					if ( !(m_prsDisplacements->BOF && m_prsDisplacements->adoEOF))
					{
						rsData->PutCollect("xl1",m_prsDisplacements->GetCollect("DX"));
						rsData->PutCollect("yl1",m_prsDisplacements->GetCollect("DY"));
						rsData->PutCollect("zl1",m_prsDisplacements->GetCollect("DZ"));

						tmpvar = m_prsDisplacements->GetCollect("DUNITS");
						if (vtos(tmpvar) == "mm")
							rsData->PutCollect("UNIT_Displacement",_variant_t(tmpvar));
						
						tmpvar = m_prsDisplacements->GetCollect("RUNITS");
						if (vtos(tmpvar) == "deg.")
						{
							 rsData->PutCollect("rxl1",_variant_t(vtof(m_prsDisplacements->GetCollect("RX"))/180*PI));
							 rsData->PutCollect("ryl1",_variant_t(vtof(m_prsDisplacements->GetCollect("RY"))/180*PI));					 
							 rsData->PutCollect("rzl1",_variant_t(vtof(m_prsDisplacements->GetCollect("RZ"))/180*PI));	
						}
						else
						{
							rsData->PutCollect("rxl1",m_prsDisplacements->GetCollect("RX"));
							rsData->PutCollect("ryl1",m_prsDisplacements->GetCollect("RY"));
							rsData->PutCollect("rzl1",m_prsDisplacements->GetCollect("RZ"));
						}
					}
					else
					{
					}
					m_prsDisplacements->Close();
					m_prsTempCASENUM->Close();
					m_prsRestraints->Close();				
					rsData->Filter = "";
				}//end if				
				
	/*********************处理FROM_NODE点***********************/
				if( i != iCount )
				{
					rsData->Filter = _variant_t(CString("[jsdbh1] = ") + ltos(node_start) +" AND "+CString("[FileNameID]=")+ltos(FileNameID));;
					m_strSQL.Empty();
					m_strSQL.Format(_T("SELECT * FROM [OUTPUT_RESTRAINTS] WHERE [NODE] = %d AND [JOBNAME] = '%s' AND [CASE] = '%s' AND [TYPE] = 'Rigid ANC' "),node_start,m_strJOBNAME_P,m_strGKname);	
					m_prsRestraints = m_pConnSourceDB->Execute(_bstr_t(m_strSQL),NULL,adCmdText);
					
					if (m_prsRestraints->adoEOF && m_prsRestraints->BOF) //是固定点 
					{
						rsData->PutCollect("pxl1",m_prsCASENUM->GetCollect("FXF"));
						rsData->PutCollect("pyl1",m_prsCASENUM->GetCollect("FYF"));
						rsData->PutCollect("pzl1",m_prsCASENUM->GetCollect("FZF"));

						rsData->PutCollect("mxl1",m_prsCASENUM->GetCollect("MXF"));
						rsData->PutCollect("myl1",m_prsCASENUM->GetCollect("MYF"));
						rsData->PutCollect("mzl1",m_prsCASENUM->GetCollect("MZF"));
					}
					else																  //不是固定点
					{
						rsData->PutCollect("pxl1",m_prsRestraints->GetCollect("FX"));
						rsData->PutCollect("pyl1",m_prsRestraints->GetCollect("FY"));
						rsData->PutCollect("pzl1",m_prsRestraints->GetCollect("FZ"));

						rsData->PutCollect("mxl1",m_prsRestraints->GetCollect("MX"));
						rsData->PutCollect("myl1",m_prsRestraints->GetCollect("MY"));
						rsData->PutCollect("mzl1",m_prsRestraints->GetCollect("MZ"));
					}
				
				//从输出位移表OUTPUT_DISPLACEMENTS中查找FROM_NODE及工况号CASE相等的记录,使字段DX->字段dxr1,字段DY->字段dyr1,字段DZ->字段dzr1,
				m_strSQL.Empty();
				if(m_prsRestraints->adoEOF && m_prsRestraints->BOF)   //如果不是固定点
				{
					m_strSQL.Format(_T("SELECT * FROM [OUTPUT_DISPLACEMENTS] WHERE [NODE] = %d AND [CASE] = '%s' AND [JOBNAME] =  '%s' "),node_start,m_strLGKname,m_strJOBNAME_P);
					m_prsDisplacements = m_pConnSourceDB->Execute(_bstr_t(m_strSQL),NULL,adCmdText);
				}
				else     //是固定点
				{
					m_strSQL.Format(_T("SELECT * FROM [OUTPUT_DISPLACEMENTS] WHERE [CASE] = '%s' AND [JOBNAME] = '%s' AND [NODE] = %d"),m_strGKname,m_strJOBNAME_P,node_start);
					m_prsDisplacements = m_pConnSourceDB->Execute(_bstr_t(m_strSQL),NULL,adCmdText);
				}

				if ( !(m_prsDisplacements->BOF && m_prsDisplacements->adoEOF))
				{
					rsData->PutCollect("xl1",m_prsDisplacements->GetCollect("DX"));
					rsData->PutCollect("yl1",m_prsDisplacements->GetCollect("DY"));
					rsData->PutCollect("zl1",m_prsDisplacements->GetCollect("DZ"));

					tmpvar = m_prsDisplacements->GetCollect("DUNITS");
					if (vtos(tmpvar) == "mm")
						rsData->PutCollect("UNIT_Displacement",_variant_t(tmpvar));
					
					tmpvar = m_prsDisplacements->GetCollect("RUNITS");
					if (vtos(tmpvar) == "deg.")
					{
						 rsData->PutCollect("rxl1",_variant_t(vtof(m_prsDisplacements->GetCollect("RX"))/180*PI));
						 rsData->PutCollect("ryl1",_variant_t(vtof(m_prsDisplacements->GetCollect("RY"))/180*PI));					 
						 rsData->PutCollect("rzl1",_variant_t(vtof(m_prsDisplacements->GetCollect("RZ"))/180*PI));	
					}
					else
					{
						rsData->PutCollect("rxl1",m_prsDisplacements->GetCollect("RX"));
						rsData->PutCollect("ryl1",m_prsDisplacements->GetCollect("RY"));
						rsData->PutCollect("rzl1",m_prsDisplacements->GetCollect("RZ"));
					}
				}
				else
				{
				}
			m_prsDisplacements->Close();
			m_prsRestraints->Close();
			}	
			//保存这次记录的TO_NODE点，与下一条记录的FROM_NODE比较			
			node_end = vtoi(m_prsCASENUM->GetCollect("TO_NODE"));

			if ( i < iCount-1 )
				m_prsCASENUM->MoveNext();
			
			rsData->Filter = "";
				
		}//  end  for
		m_prsCASENUM = NULL;					
		m_prsHangers = NULL;			
		m_prsDisplacements = NULL;	
		m_prsRestraints = NULL;		
		m_prsJGHZ = NULL;				
		m_prsJOBNAME = NULL;		
		m_prsBasicData = NULL;		
		m_prsTempCASENUM = NULL;		

		m_pConnSourceDB->Close();
		m_pConnSourceDB = NULL;
		
	}//   end   try
	catch(CException *e)
	{
		e->ReportError();
		e->Delete();
	}
}

typedef struct _tagSWEDChildNodeInfo{
	int iStartNode;
	int iNum;
} tagSWEDChildNodeInfo,FAR * LPSWEDChildNodeInfo;

typedef CList<tagSWEDChildNodeInfo,tagSWEDChildNodeInfo&> tagListSWEDChildNodeInfo;

int GetSWEDChildNodeNum(tagListSWEDChildNodeInfo* pLstChildNodeInfo,int iStartNode)
{
	POSITION pos=pLstChildNodeInfo->GetHeadPosition();
	tagSWEDChildNodeInfo ChildNodeInfo;
	while(pos)
	{
		ChildNodeInfo=pLstChildNodeInfo->GetNext(pos);
		if(ChildNodeInfo.iStartNode==iStartNode)
			return ChildNodeInfo.iNum;
	}
	return 0;
}

double GetParam(CString & strText)
{
	LPTSTR lpsz;
	double res=_tcstod(strText,&lpsz);
	strText.Delete(0,lpsz-(LPCTSTR)strText);
	return res;
}

void ReadResult_SWEDPSA(_Recordset* rsResult ,CString SourceDataFileName,long maxZdjh,long FileNameID)
{
   //Remauther:
   //作者:
   //目的:本程序为读西南电力院应力分析程序结果数据文件*.dat文件
   //输入:rsResult数据表内部单位:位移mm,荷载kgf.
   //返回:
   //修改:赵佳
   //修改原因:西南院的应力分析结果文件中中文的处理
	try
	{
		
		long ltemp=maxZdjh;
		CString strOldPhsSPRINGSel;
		CString strTemp, temp ;int iTemp=0;
		CString strSprTbn;
		CString strSQL;
		_RecordsetPtr tmprs;
		tmprs.CreateInstance(__uuidof(Recordset));
		_variant_t varTmp;
		tagListSWEDChildNodeInfo ListChildNodeInfo;
		tagSWEDChildNodeInfo ChildNodeInfo;
		const char* strPIPING_STRESS_OUTUNIT = "UNIT OF OUTPUTDATA IS";
		const char* strSUMMARY_FOR_RESTRAINTS = "SUMMARY FOR RESTRAINTS";
		const char* strCOLD_SPRING_TABLE = "COLD SPRINGS TABLE";
		const char* strEND_RawData = "10       1       1         1";
		const char* strPSAVER = "XNP 2.0";
		const char* strSPRINGType = "SPRING VERSION:";
		const char* strEND_THIS_PIPING = "*THIS PIPING STRUCTURE *CONTAINS*";
		POSITION pos,pos2;

		float sngDWTemp=0, TjTemp=0;
		long i=0;
		int iJSJDH_start=0, iJSJDH_end=0;
		int j=0, iC1=0;
		int iFlag=0, iCount=0;
		int mviSPR=0;
		CString mvS;
		CArray<tagVector,tagVector&> arrVector;
		arrVector.SetSize(500,500);
		int iVectorCount=0;

		//CArray<int,int> arrPhsNode;
		//CArray<int,int> arrPhsType;//
		CList<_PhsInfo,_PhsInfo&> list_PhsInfo;
		CList<float,float> listPhsDW;
		CList<float,float> listPhsTJ;
		CList<SECT,SECT&> listSECT;
		CList<PhsInfo,PhsInfo&> listPhsInfo;
		

		const char* pszZTS = "ZTS=";//支吊架信息
		const char* pszGZ  = "GZ="; //管材
		const char* pszGW  = "GW="; //管温
		const char* pszPNM = "PNM=";//分支点信息

		
		ifstream f;
		frmStatus.ShowWindow(SW_SHOW);
		frmStatus.UpdateStatus(0.0f);
		frmStatus.UpdateWindow();
		//XNP只能用法定单位输出
		EDIBgbl::PSA_OutDataUnit = "NATIONAL";
		//坐标轴方向x/y/z轴向上=1/2/3
		//XNP只能Z轴向上
		giUPxyz = 3;
		//XNP只能选择西北院支吊架手册弹簧种类
		//modPHScal::gnSPR_CHZBH = 0.35f;
		strOldPhsSPRINGSel = modPHScal::gsPhsSPRINGSel;
		//因为应力计算选择的弹簧标准可能不同,故重新更新规范
		//由于生成规范花费时间较长，所以只有当规范变化时，才需要重新更新
		strSQL = "SELECT Spring_property FROM phsManuSPRING WHERE StandardDesc=\'";
		strSQL += strSprNWEPDI;
		strSQL += "\'";
		tmprs->Open((_bstr_t)strSQL, _variant_t((IDispatch*)EDIBgbl::dbSORT,true), 
			adOpenKeyset, adLockOptimistic, adCmdText); 
		if(tmprs->BOF && tmprs->adoEOF)
		{
		}
		else
		{
			tmprs->get_Collect((_variant_t)0L, &varTmp);
			strSprTbn=vtos(varTmp);
		}
		tmprs->Close();
		if( strOldPhsSPRINGSel != strSprNWEPDI )
		{
			modPHScal::gsPhsSPRINGSel = strSprNWEPDI;					
		}

		f.open(SourceDataFileName);
		//开始读原始数据
		temp=_T("");
		while(!f.eof())
		{
			if((i=temp.Find(_T(" IC=")))!=-1)
			{
				int iIC=_ttoi(temp.Mid(i+4));
				if(iIC==0)
					modPHScal::gnSPR_CHZBH=0.35f;
				else if(iIC==1)
					modPHScal::gnSPR_CHZBH=0.25f;
				//荷载变化系数保存到外部
				::SetRegValue(_T("Settings"), "SPR_Chzbh", (modPHScal::gnSPR_CHZBH));
			}
//			int ttt = temp[0];
			if(temp.GetLength()>0 && !(temp[0]>=_T('0')&&temp[0]<=_T('9'))/*_T('A') <= temp[0] && temp[0]<=_T('Z')*/)
			{
				if( temp.Find(pszZTS) != -1 )
				{
					do  //过滤前面的空行
					{
						f.getline(temp.GetBuffer(255),255);
						temp.ReleaseBuffer();
						temp.TrimLeft();
						temp.MakeUpper();
					}while(temp.GetLength()==0 && !f.eof());
//					ttt = temp[0];
					while(temp.GetLength()>0 && /*(temp[0] < _T('A')  ||  _T('Z') < temp[0])*/((temp[0]>=_T('0')&&temp[0]<=_T('9'))||temp[0]==_T('.')) && !f.eof())
					{//支吊点信息 在"ZTS="关键字后
												
						_PhsInfo tmp;
						tmp.iNode = GetParam(temp)*10.0f;
						tmp.lPhsType = GetParam(temp);
						list_PhsInfo.AddTail(tmp);
						GetParam(temp);
						temp.TrimRight();
						if(temp.GetLength()>0)
						{
							tmp.iNode = GetParam(temp)*10.0f;
							tmp.lPhsType = GetParam(temp);
							list_PhsInfo.AddTail(tmp);
						}
						f.getline(temp.GetBuffer(255),255);
						temp.ReleaseBuffer();
						temp.TrimLeft();
						temp.MakeUpper();
					}
				}
				else if(temp.Find("GZ=")!=-1)
				{
					do  //过滤前面的空行
					{
						f.getline(temp.GetBuffer(255),255);
						temp.ReleaseBuffer();
						temp.TrimLeft();
						temp.MakeUpper();
					}while(temp.GetLength()==0 && !f.eof());
					while(temp.GetLength()>0 && /*(temp[0] < _T('A')  ||  _T('Z') < temp[0])*/((temp[0]>=_T('0')&&temp[0]<=_T('9'))||temp[0]==_T('.')) && !f.eof())
					{//管径信息 在"GZ="关键字后
						GetParam(temp);
						listPhsDW.AddTail(GetParam(temp)*10.0f);
						f.getline(temp.GetBuffer(255),255);
						temp.ReleaseBuffer();
						temp.TrimLeft();
						temp.MakeUpper();
					}
				}
				else if(temp.Find("GW=")!=-1)
				{
					do  //过滤前面的空行
					{
						f.getline(temp.GetBuffer(255),255);
						temp.ReleaseBuffer();
						temp.TrimLeft();
						temp.MakeUpper();
					}while(temp.GetLength()==0 && !f.eof());
					while(temp.GetLength()>0 && /*(temp[0] < _T('A')  ||  _T('Z') < temp[0])*/ ((temp[0]>=_T('0')&&temp[0]<=_T('9'))||temp[0]==_T('.')) && !f.eof())
					{//管温信息 在"GW="关键字后

						listPhsTJ.AddTail(GetParam(temp));
						f.getline(temp.GetBuffer(255),255);
						temp.ReleaseBuffer();
						temp.TrimLeft();
						temp.MakeUpper();
					}
				}
				else if(temp.Find(_T("FS="))!=-1)
				{
					do  //过滤前面的空行
					{
						f.getline(temp.GetBuffer(255),255);
						temp.ReleaseBuffer();
						temp.TrimLeft();
						temp.MakeUpper();
					}while(temp.GetLength()==0 && !f.eof());
					while(temp.GetLength()>0 && ((temp[0]>=_T('0')&&temp[0]<=_T('9'))||temp[0]==_T('.'))/*(temp[0] < _T('A')  ||  _T('Z') < temp[0])*/ && !f.eof())
					{//分支信息  在"FS="关键字后
						
						SECT tmpSECT;
						tmpSECT.iDWIndex = GetParam(temp);
						tmpSECT.iTJIndex = GetParam(temp);
						GetParam(temp);
						GetParam(temp);
						tmpSECT.iElementNums = GetParam(temp);
						listSECT.AddTail(tmpSECT);
						f.getline(temp.GetBuffer(255),255);
						temp.ReleaseBuffer();
						temp.TrimLeft();
						temp.MakeUpper();
					}
				}
				else if(temp.Find("PNM=")!=-1)
				{
					do  //过滤前面的空行
					{
						f.getline(temp.GetBuffer(255),255);
						temp.ReleaseBuffer();
						temp.TrimLeft();
						temp.TrimRight();
						temp.MakeUpper();
					}while(temp.GetLength()==0 && !f.eof());
					while(temp.GetLength()>0 && /*(temp[0] < _T('A')  ||  _T('Z') < temp[0])*/((temp[0]>=_T('0')&&temp[0]<=_T('9'))||temp[0]==_T('.')) && !f.eof())
					{//添加单元的信息  在"PNM="关键字后
						while(temp.GetLength()!=0)
						{
							ChildNodeInfo.iStartNode=GetParam(temp);
							ChildNodeInfo.iNum=GetParam(temp);
							ListChildNodeInfo.AddTail(ChildNodeInfo);
						}
						f.getline(temp.GetBuffer(255),255);
						temp.ReleaseBuffer();
						temp.TrimLeft();
						temp.TrimRight();
						temp.MakeUpper();
					}
				}
				else if(temp.Find("ES=")!=-1)
				{
					do  //过滤前面的空行
					{
						f.getline(temp.GetBuffer(255),255);
						temp.ReleaseBuffer();
						temp.TrimLeft();
						temp.TrimRight();
						temp.MakeUpper();
					}while(temp.GetLength()==0 && !f.eof());
					/////z.jia add/////
					int ifind = temp.Find("序号");
					if(ifind!=-1)
						temp = temp.Right(temp.GetLength()-ifind-4);
					/////z.jia add////
					while(temp.GetLength()>0 && /*(temp[0] < _T('A')  ||  _T('Z') < temp[0])*/((temp[0]>=_T('0')&&temp[0]<=_T('9'))||temp[0]==_T('.')) && !f.eof())
					{//添加单元的信息  在"ES="关键字后						
						GetParam(temp);
						tagVector vecTmp;
						vecTmp.x=GetParam(temp);
						vecTmp.y=GetParam(temp);
						vecTmp.z=GetParam(temp);
						arrVector.SetAtGrow(iVectorCount++,vecTmp);
						f.getline(temp.GetBuffer(255),255);
						////z.jia add//
						ifind = temp.Find("序号");
						if(ifind)
							temp = temp.Right(temp.GetLength()-ifind-4);
						//////
						temp.ReleaseBuffer();
						temp.TrimLeft();
						temp.TrimRight();
						temp.MakeUpper();
					}
					break;
				}
				else
				{
					do  //过滤前面的空行
					{
						f.getline(temp.GetBuffer(255),255);
						temp.ReleaseBuffer();
						temp.TrimLeft();
						temp.MakeUpper();
					}while(temp.GetLength()==0 && !f.eof());
					while(temp.GetLength()>0 && /*(temp[0] < _T('A')  ||  _T('Z') < temp[0])*/((temp[0]>=_T('0')&&temp[0]<=_T('9'))||temp[0]==_T('.')) && !f.eof())
					{
						f.getline(temp.GetBuffer(255),255);
						temp.ReleaseBuffer();
						temp.TrimLeft();
						temp.MakeUpper();
					}
				}
			}
			else
			{	
				f.getline(temp.GetBuffer(255),255);
				temp.ReleaseBuffer();
				temp.TrimLeft();
				temp.MakeUpper();
			}
		}
		f.close();
		frmStatus.UpdateStatus(0.02f,true);
		frmStatus.UpdateWindow();

		f.open(SourceDataFileName);
		while(!f.eof())
		{
			f.getline(temp.GetBuffer(255),255);
			temp.ReleaseBuffer();
			if(temp.Find("FULCRUM DISPLACEMENT")!=-1)
			{
				f.getline(temp.GetBuffer(255),255);
				temp.ReleaseBuffer();
				f.getline(temp.GetBuffer(255),255);
				temp.ReleaseBuffer();
				do
				{//非弹吊热态位移

					PhsInfo tmpPhsInfo;

					tmpPhsInfo.iCalNodeNumber = GetParam(temp)*10.0f;
					tmpPhsInfo.fLX = GetParam(temp)*10.0f;
					tmpPhsInfo.fLY = GetParam(temp)*10.0f;
					tmpPhsInfo.fLZ = GetParam(temp)*10.0f;
					tmpPhsInfo.fQX = GetParam(temp);
					tmpPhsInfo.fQY = GetParam(temp);
					tmpPhsInfo.fQZ = GetParam(temp);
					listPhsInfo.AddTail(tmpPhsInfo);
					
					f.getline(temp.GetBuffer(255),255);
					temp.ReleaseBuffer();
					temp.TrimLeft();
				}while(temp.GetLength() >0 && ((_T('0') <= temp[0] && temp[0] <= _T('9')) || temp[0]==_T('.')) &&!f.eof());

				do
				{//非弹吊热态荷载
					if( temp.Find("SUPPORT LOAD")!= -1)
					{
						f.getline(temp.GetBuffer(255),255);
						temp.ReleaseBuffer();
						f.getline(temp.GetBuffer(255),255);
						temp.ReleaseBuffer();
						do
						{
							PhsInfo tmpPhsInfo;

							tmpPhsInfo.iCalNodeNumber = GetParam(temp)*10;
							tmpPhsInfo.fPX = GetParam(temp);        //update cheng
							tmpPhsInfo.fPY = GetParam(temp);
							tmpPhsInfo.fPZ = GetParam(temp);
							tmpPhsInfo.fMX = GetParam(temp);
							tmpPhsInfo.fMY = GetParam(temp);
							tmpPhsInfo.fMZ = GetParam(temp);
							bool bFound=false;
							pos=listPhsInfo.GetHeadPosition();
							while(pos)
							{	
								PhsInfo& infoTmp=listPhsInfo.GetNext(pos);
								if(infoTmp.iCalNodeNumber == tmpPhsInfo.iCalNodeNumber )
								{
									bFound=true;
									infoTmp.fPX=tmpPhsInfo.fPX;    //update cheng
									infoTmp.fPY=tmpPhsInfo.fPY;
									infoTmp.fPZ=tmpPhsInfo.fPZ;
									infoTmp.fMX=tmpPhsInfo.fMX;
									infoTmp.fMY=tmpPhsInfo.fMY;
									infoTmp.fMZ=tmpPhsInfo.fMZ;
								}
							}
							if(!bFound)
								listPhsInfo.AddTail(tmpPhsInfo);
							f.getline(temp.GetBuffer(255),255);
							temp.ReleaseBuffer();
							temp.TrimLeft();
						}while(temp.GetLength() >0 && ((_T('0') <= temp[0] && temp[0] <= _T('9')) || temp[0]==_T('.')) &&!f.eof());
						break;
					}
					f.getline(temp.GetBuffer(255),255);
					temp.ReleaseBuffer();
				}while(!f.eof());

			}
		}
		f.close();

		frmStatus.UpdateStatus(0.1f,true);
		frmStatus.UpdateWindow();

		f.open(SourceDataFileName);
		while(!f.eof())
		{
			f.getline(temp.GetBuffer(255),255);
			temp.ReleaseBuffer();
			if(temp.Find("COLD PULL DISPLACEMENT")!=-1)
			{
				f.getline(temp.GetBuffer(255),255);
				temp.ReleaseBuffer();
				f.getline(temp.GetBuffer(255),255);
				temp.ReleaseBuffer();
				do
				{//所有支吊点冷态位移

					PhsInfo tmpPhsInfo;

					tmpPhsInfo.iCalNodeNumber = GetParam(temp)*10;
					tmpPhsInfo.fCoolLX = GetParam(temp)*10.0f;
					tmpPhsInfo.fCoolLY = GetParam(temp)*10.0f;
					tmpPhsInfo.fCoolLZ = GetParam(temp)*10.0f;
					tmpPhsInfo.fCoolQX = GetParam(temp);
					tmpPhsInfo.fCoolQY = GetParam(temp);
					tmpPhsInfo.fCoolQZ = GetParam(temp);
					bool bFound=false;
					pos=listPhsInfo.GetHeadPosition();
					while(pos)
					{	
						PhsInfo& infoTmp=listPhsInfo.GetNext(pos);
						if(infoTmp.iCalNodeNumber == tmpPhsInfo.iCalNodeNumber )
						{
						    bFound=true;
							infoTmp.fCoolLX = tmpPhsInfo.fCoolLX;
							infoTmp.fCoolLY = tmpPhsInfo.fCoolLY;
							infoTmp.fCoolLZ = tmpPhsInfo.fCoolLZ;
							infoTmp.fCoolQX = tmpPhsInfo.fCoolQX;
							infoTmp.fCoolQY = tmpPhsInfo.fCoolQY;
							infoTmp.fCoolQZ = tmpPhsInfo.fCoolQZ;
						}
					}
					if( !bFound )
						listPhsInfo.AddTail(tmpPhsInfo);

				
					f.getline(temp.GetBuffer(255),255);
					temp.ReleaseBuffer();
					temp.TrimLeft();
				}while(temp.GetLength() >0 && ((_T('0') <= temp[0] && temp[0] <= _T('9')) || temp[0]==_T('.')) &&!f.eof());

				do
				{//所有支吊点冷态荷载
					if( temp.Find("SUPPORT LOAD")!= -1)
					{
						f.getline(temp.GetBuffer(255),255);
						temp.ReleaseBuffer();
						f.getline(temp.GetBuffer(255),255);
						temp.ReleaseBuffer();
						do
						{
							PhsInfo tmpPhsInfo;

							tmpPhsInfo.iCalNodeNumber = GetParam(temp)*10;
							tmpPhsInfo.fCoolPX = GetParam(temp);
							tmpPhsInfo.fCoolPY = GetParam(temp);
							tmpPhsInfo.fCoolPZ = GetParam(temp);
							tmpPhsInfo.fCoolMX = GetParam(temp);
							tmpPhsInfo.fCoolMY = GetParam(temp);
							tmpPhsInfo.fCoolMZ = GetParam(temp);

							bool bFound=false;
							pos=listPhsInfo.GetHeadPosition();
							
							while(pos)
							{	
								PhsInfo& infoTmp=listPhsInfo.GetNext(pos);
								if(infoTmp.iCalNodeNumber == tmpPhsInfo.iCalNodeNumber )
								{
									bFound=true;
									infoTmp.fCoolPX = tmpPhsInfo.fCoolPX;
									infoTmp.fCoolPY = tmpPhsInfo.fCoolPY;
									infoTmp.fCoolPZ = tmpPhsInfo.fCoolPZ;
									infoTmp.fCoolMX = tmpPhsInfo.fCoolMX;
									infoTmp.fCoolMY = tmpPhsInfo.fCoolMY;
									infoTmp.fCoolMZ = tmpPhsInfo.fCoolMZ;
								}
							}
							if( !bFound )
								listPhsInfo.AddTail(tmpPhsInfo);
							
						f.getline(temp.GetBuffer(255),255);
						temp.ReleaseBuffer();
						temp.TrimLeft();
						}while(temp.GetLength() >0 && ((_T('0') <= temp[0] && temp[0] <= _T('9')) || temp[0]==_T('.')) &&!f.eof());
						break;
					}
					f.getline(temp.GetBuffer(255),255);
					temp.ReleaseBuffer();
				}while(!f.eof());
			}
		}
		f.close();
   

		frmStatus.UpdateStatus(0.2f,true);
		frmStatus.UpdateWindow();

		f.open(SourceDataFileName);
		while(!f.eof())
		{
			f.getline(temp.GetBuffer(255),255);
			temp.ReleaseBuffer();
//			if(temp.Find("SPRING TABLE <1>")!=-1)
			if(temp.Find("SPRING TABLE")!=-1 && temp.Find("<1>")!=-1)
			{
				f.getline(temp.GetBuffer(255),255);
				temp.ReleaseBuffer();
				while(temp.Find("HAO")==-1)
				{
					//再寻找HAO字符，这行的下面是数字信息
					f.getline(temp.GetBuffer(255),255);
					temp.ReleaseBuffer();
				}
				do
				{
					PhsInfo tmpPhsInfo;
					tmpPhsInfo.iCalNodeNumber = GetParam(temp)*10.0f;
					int iHA,iBI,iCHM,iCHN;
					float fDAZ,fCAZ;
					iHA=GetParam(temp);
					iBI=GetParam(temp);
					iCHM=GetParam(temp);
					fDAZ=GetParam(temp);
					iCHN=GetParam(temp);
					fCAZ=GetParam(temp);
					if( iHA == 0 )
					{
						tmpPhsInfo.lZDJType = 9999;
					}
					else
					{
						tmpPhsInfo.lZDJType = iHA + iBI*100 + (iCHM*2+iCHN)*1000;
					}
					pos=list_PhsInfo.GetHeadPosition();
					while(pos)
					{//只有弹簧支吊架才转换其类型为AutoPSA的弹性支吊架类型
						tag_PhsInfo& infoTmp=list_PhsInfo.GetNext(pos);
						if(infoTmp.iNode == tmpPhsInfo.iCalNodeNumber )
						{
							if( infoTmp.lPhsType == 5 || infoTmp.lPhsType == 6)
							{
								bool bFound=false;
								pos2=listPhsInfo.GetHeadPosition();
								while(pos2)
								{	
									PhsInfo & infoTmp2=listPhsInfo.GetNext(pos2);
									if(infoTmp2.iCalNodeNumber == tmpPhsInfo.iCalNodeNumber )
									{//已存在的节点，只能修改其类型信息
										bFound=true;
										infoTmp2.lZDJType = tmpPhsInfo.lZDJType;
									}
								}
								if( !bFound )
									listPhsInfo.AddTail(tmpPhsInfo);
							}
						}
					}
				
					f.getline(temp.GetBuffer(255),255);
					temp.ReleaseBuffer();
					temp.TrimLeft();
				}while(temp.GetLength() >0 && ((_T('0') <= temp[0] && temp[0] <= _T('9')) || temp[0]==_T('.')) /*&&!f.eof()*/ && temp.Find("SPRING TABLE")==-1 && temp.Find("<2>")==-1);


				do
				{
//					if(temp.Find("SPRING TABLE <2>")!=-1)
					if(temp.Find("SPRING TABLE")!=-1 && temp.Find("<2>")!=-1)
					{//弹簧支吊架热态数据
						while(temp.Find("PGZ")==-1)
						{
							//再寻找HAO字符，这行的下面是数字信息
							f.getline(temp.GetBuffer(255),255);
							temp.ReleaseBuffer();
						}
						do
						{
							PhsInfo tmpPhsInfo;
							tmpPhsInfo.iCalNodeNumber = GetParam(temp)*10.0f;
							tmpPhsInfo.fPGZ = GetParam(temp);
							tmpPhsInfo.fPAZ = GetParam(temp);
							tmpPhsInfo.fPJG = GetParam(temp);
							tmpPhsInfo.fLX = GetParam(temp)*10.0f;
							tmpPhsInfo.fLY = GetParam(temp)*10.0f;
							tmpPhsInfo.fLZ = GetParam(temp)*10.0f;
							tmpPhsInfo.fPZ=tmpPhsInfo.fPGZ;
							tmpPhsInfo.fCoolPZ=tmpPhsInfo.fPAZ;
							bool bFound=false;
							pos=listPhsInfo.GetHeadPosition();
							while(pos)
							{	
								PhsInfo& infoTmp=listPhsInfo.GetNext(pos);
								if(infoTmp.iCalNodeNumber == tmpPhsInfo.iCalNodeNumber )
								{//已存在的节点，只能修改其类型信息
									bFound=true;
									infoTmp.fPGZ = tmpPhsInfo.fPGZ;
									infoTmp.fPAZ = tmpPhsInfo.fPAZ;
									infoTmp.fPJG = tmpPhsInfo.fPJG;
									infoTmp.fLX = tmpPhsInfo.fLX;
									infoTmp.fLY = tmpPhsInfo.fLY;
									infoTmp.fLZ = tmpPhsInfo.fLZ;
									infoTmp.fPZ = tmpPhsInfo.fPZ;
									infoTmp.fCoolPZ = tmpPhsInfo.fCoolPZ;
								}
							}
							if( !bFound )
								listPhsInfo.AddTail(tmpPhsInfo);
								
						f.getline(temp.GetBuffer(255),255);
						temp.ReleaseBuffer();
						temp.TrimLeft();
						}while(temp.GetLength() >0 && ((_T('0') <= temp[0] && temp[0] <= _T('9')) || temp[0]==_T('.')) &&!f.eof());
					}
					f.getline(temp.GetBuffer(255),255);
					temp.ReleaseBuffer();
					temp.TrimLeft();
				}while(!f.eof());
			}
		}
		f.close();

		frmStatus.UpdateStatus(0.3f,true);
		frmStatus.UpdateWindow();


		f.open(SourceDataFileName);//SourceDataFileName
		while(!f.eof())
		{
			f.getline(temp.GetBuffer(255),255);
			temp.ReleaseBuffer();
			if(temp.Find("JIE GOU SUPPORT LOAD")!=-1)
			{
				f.getline(temp.GetBuffer(255),255);
				temp.ReleaseBuffer();

				f.getline(temp.GetBuffer(255),255);
				temp.ReleaseBuffer();

				do
				{
					temp.TrimLeft();temp.TrimRight();
					PhsInfo tmpPhsInfo;
					tmpPhsInfo.iCalNodeNumber = GetParam(temp)*10.0f;
					GetParam(temp);
					GetParam(temp);
					tmpPhsInfo.fPJG=GetParam(temp);
					bool bFound=false;
					pos=listPhsInfo.GetHeadPosition();
					while(pos)
					{	
						PhsInfo& infoTmp = listPhsInfo.GetNext(pos);
						if(infoTmp.iCalNodeNumber == tmpPhsInfo.iCalNodeNumber )
						{//已存在的节点，只能修改其类型信息
							bFound=true;
							infoTmp.fPJG = tmpPhsInfo.fPJG;
						}
					}
					if(!bFound)
						listPhsInfo.AddTail(tmpPhsInfo);
					f.getline(temp.GetBuffer(255),255);
					temp.ReleaseBuffer();
					temp.TrimLeft();

				}while(temp.GetLength() >0 && ((_T('0') <= temp[0] && temp[0] <= _T('9')) || temp[0]==_T('.')) &&!f.eof());

			}

		}
		f.close();

		frmStatus.UpdateStatus(0.4f,true);
		frmStatus.UpdateWindow();

		f.open(SourceDataFileName);
		while(!f.eof())
		{
			f.getline(temp.GetBuffer(255),255);
			temp.ReleaseBuffer();
			if(temp.Find("WORKING SUPPORT LOAD")!=-1)
			{
				f.getline(temp.GetBuffer(255),255);
				temp.ReleaseBuffer();

				f.getline(temp.GetBuffer(255),255);
				temp.ReleaseBuffer();

				do
				{
					temp.TrimLeft();temp.TrimRight();
					PhsInfo tmpPhsInfo;
					tmpPhsInfo.iCalNodeNumber = GetParam(temp)*10.0f;
					GetParam(temp);
					GetParam(temp);
					tmpPhsInfo.fPGZ=GetParam(temp);
					bool bFound=false;
					pos=listPhsInfo.GetHeadPosition();
					while(pos)
					{	
						PhsInfo& infoTmp=listPhsInfo.GetNext(pos);
						if(infoTmp.iCalNodeNumber == tmpPhsInfo.iCalNodeNumber )
						{//已存在的节点，只能修改其类型信息
							bFound=true;
							infoTmp.fPGZ = tmpPhsInfo.fPGZ;
						}
					}
					if(!bFound)
						listPhsInfo.AddTail(tmpPhsInfo);
					f.getline(temp.GetBuffer(255),255);
					temp.ReleaseBuffer();
					temp.TrimLeft();	
				}while(temp.GetLength() >0 && ((_T('0') <= temp[0] && temp[0] <= _T('9')) || temp[0]==_T('.')) &&!f.eof());

			}

		}
		f.close();
		
		frmStatus.UpdateStatus(0.6f,true);
		frmStatus.UpdateWindow();

		//开始写入数据库
		pos=listSECT.GetHeadPosition();
		int iNodeNo=1;
		int iTrueNodeNo=0;
		int iZdjh=maxZdjh+1;
		CString strFilter;
		float fStatusPos=0.6f;
		int iVectorIndex=0;
		CList<int,int &> listTrueNodeNo;
		while(pos)
		{
			int i,j,k;
			tagSECT &tmpSect=listSECT.GetNext(pos);
			for(i=0;i<tmpSect.iElementNums-1;i++)
			{
				int ChildNum=GetSWEDChildNodeNum(&ListChildNodeInfo,iNodeNo);
				float fDw=0.0f,fTj=0.0f;
				pos2=listPhsDW.GetHeadPosition();
				k=1;
				while(pos2)
				{
					if(k==tmpSect.iDWIndex)
						fDw=listPhsDW.GetNext(pos2);
					else
						listPhsDW.GetNext(pos2);
					k++;
				}
				pos2=listPhsTJ.GetHeadPosition();
				k=1;
				while(pos2)
				{
					if(k==tmpSect.iTJIndex)
						fTj=listPhsTJ.GetNext(pos2);
					else
						listPhsTJ.GetNext(pos2);
					k++;
				}

				for(j=0;j<=ChildNum;j++)
				{
					float fNodeNo=iNodeNo+j*0.1f;
					iTrueNodeNo=(int)(fNodeNo*10.0f);
					if(iTrueNodeNo==560)
					{
						int iDbg=0;
					}
					BOOL bFound=FALSE;
					pos2=list_PhsInfo.GetHeadPosition();
					int iPhsType=0;
					while(pos2)
					{
						_PhsInfo& infoTmp=list_PhsInfo.GetNext(pos2);
						if(infoTmp.iNode==iTrueNodeNo)
						{
							bFound=TRUE;
							iPhsType=infoTmp.lPhsType;
							break;
						}
					}
					TRACE(_T("node=%d\n"),iTrueNodeNo);
					if(bFound)
					{
						listTrueNodeNo.AddTail(iTrueNodeNo);
						pos2=listPhsInfo.GetHeadPosition();
						PhsInfo* pInfo=NULL;
						while(pos2)
						{
							pInfo=&listPhsInfo.GetNext(pos2);
							if(pInfo->iCalNodeNumber==iTrueNodeNo)
								break;
							else pInfo=NULL;
						}
						
                        //strFilter.Format(_T("[jsdbh1]=%d"),iTrueNodeNo);
						strFilter.Format(_T("[jsdbh1]=%d AND [FileNameID]=%ld"),iTrueNodeNo,FileNameID);
						rsResult->Filter=_variant_t(strFilter);
                        
						if(rsResult->BOF || rsResult->adoEOF)
						{

							rsResult->AddNew();
							rsResult->PutCollect(L"FileNameID",_variant_t(FileNameID));
							rsResult->PutCollect(L"VolumeID",_variant_t(EDIBgbl::SelVlmID));
							rsResult->PutCollect(L"Zdjh",_variant_t((long)(iZdjh)));
							rsResult->PutCollect(L"jsdbh1",_variant_t((long)iTrueNodeNo));
							iZdjh++;
						}
						rsResult->PutCollect(L"PSAVer",_variant_t(strPSAVER));
						rsResult->PutCollect(L"UPxyz",_variant_t((long)giUPxyz));
						rsResult->PutCollect(L"DJ1",fDw);
						rsResult->PutCollect(L"T01",fTj);
						if(pInfo)
						{
							if(pInfo->lZDJType<1000)
							{
								switch(iPhsType)
								{
								case 0:
									pInfo->lZDJType=177;
									break;
								case 1:
									pInfo->lZDJType=130;
									break;
								case 2:
									if(fabs(arrVector[iVectorIndex].x-0.0f)>0.01f)
									{
										pInfo->lZDJType=166;
									}
									else if(fabs(arrVector[iVectorIndex].y-0.0f)>0.01f)
									{
										pInfo->lZDJType=155;
									}
									else if(fabs(arrVector[iVectorIndex].z-0.0f)>0.01f)
									{
										pInfo->lZDJType=144;
									}
									break;
								case 3:
								case 7:
									pInfo->lZDJType=130;
									break;
								case -1:
									if(fabs(arrVector[iVectorIndex].x-0.0f)>0.01f)
									{
										pInfo->lZDJType=110;
									}
									else if(fabs(arrVector[iVectorIndex].y-0.0f)>0.01f)
									{
										pInfo->lZDJType=120;
									}
									break;
								case -2:
									if(fabs(arrVector[iVectorIndex].x-0.0f)>0.01f)
									{
										pInfo->lZDJType=120;
									}
									else if(fabs(arrVector[iVectorIndex].y-0.0f)>0.01f)
									{
										pInfo->lZDJType=110;
									}
									break;
								case -3:
									if(fabs(arrVector[iVectorIndex].x-0.0f)>0.01f)
									{
										pInfo->lZDJType=112;
									}
									else if(fabs(arrVector[iVectorIndex].y-0.0f)>0.01f)
									{
										pInfo->lZDJType=121;
									}
									break;
								case -4:
									if(fabs(arrVector[iVectorIndex].x-0.0f)>0.01f)
									{
										pInfo->lZDJType=123;
									}
									else if(fabs(arrVector[iVectorIndex].y-0.0f)>0.01f)
									{
										pInfo->lZDJType=113;
									}
									break;
								case -5:
									pInfo->lZDJType=110;
									break;
								case -6:
									pInfo->lZDJType=120;
									break;
								case -7:
									pInfo->lZDJType=152;
									break;
								case -8:
									pInfo->lZDJType=161;
									break;
								}
							}
							rsResult->PutCollect(L"PJG1",pInfo->fPJG);
							rsResult->PutCollect(L"PGZ1",pInfo->fPGZ);
							rsResult->PutCollect(L"TYPE",(LPCTSTR)ltos(pInfo->lZDJType));
							rsResult->PutCollect(L"psaTYPE",(LPCTSTR)ltos(pInfo->lZDJType));
							rsResult->PutCollect(L"XL1",pInfo->fCoolLX);
							rsResult->PutCollect(L"YL1",pInfo->fCoolLY);
							rsResult->PutCollect(L"ZL1",pInfo->fCoolLZ);
							rsResult->PutCollect(L"XR1",pInfo->fLX);
							rsResult->PutCollect(L"YR1",pInfo->fLY);
							rsResult->PutCollect(L"ZR1",pInfo->fLZ);
							rsResult->PutCollect(L"MXL1",pInfo->fCoolMX);
							rsResult->PutCollect(L"MYL1",pInfo->fCoolMY);
							rsResult->PutCollect(L"MZL1",pInfo->fCoolMZ);
							rsResult->PutCollect(L"MXR1",pInfo->fMX);
							rsResult->PutCollect(L"MYR1",pInfo->fMY);
							rsResult->PutCollect(L"MZR1",pInfo->fMZ);
							rsResult->PutCollect(L"PXL1",pInfo->fCoolPX);
							rsResult->PutCollect(L"PYL1",pInfo->fCoolPY);
							rsResult->PutCollect(L"PZL1",pInfo->fCoolPZ);
							rsResult->PutCollect(L"PXR1",pInfo->fPX);
							rsResult->PutCollect(L"PYR1",pInfo->fPY);
							rsResult->PutCollect(L"PZR1",pInfo->fPZ);
							rsResult->PutCollect(L"RXL1",pInfo->fCoolQX);
							rsResult->PutCollect(L"RYL1",pInfo->fCoolQY);
							rsResult->PutCollect(L"RZL1",pInfo->fCoolQZ);
							rsResult->PutCollect(L"RXR1",pInfo->fQX);
							rsResult->PutCollect(L"RYR1",pInfo->fQY);
							rsResult->PutCollect(L"RZR1",pInfo->fQZ);
							rsResult->PutCollect(L"Paz1",pInfo->fPAZ);
							rsResult->PutCollect("Unit_Force", L"N");
							rsResult->PutCollect("Unit_Momental", L"N.m");
 
						}
						rsResult->Update();
						fStatusPos+=0.02f;
						frmStatus.UpdateStatus(fStatusPos ,true);
						frmStatus.UpdateWindow();
					}
					iVectorIndex++;
				}
				i+=(j-1);
				iNodeNo++;
			}
			iVectorIndex++;
		}
		pos=list_PhsInfo.GetHeadPosition();
		CList<long ,long &> listNoFoundNodeNo;
		while(pos)
		{
			tag_PhsInfo& infoTmp=list_PhsInfo.GetNext(pos);
			pos2=listTrueNodeNo.GetHeadPosition();
			BOOL bFound=FALSE;
			while(pos2)
			{
				int iNodeNo2=listTrueNodeNo.GetNext(pos2);
				if(iNodeNo2==infoTmp.iNode)
				{
					bFound=TRUE;
					break;
				}
			}
			if(!bFound)
			{
				listNoFoundNodeNo.AddTail(infoTmp.iNode);
			}
		}
		CString strNodeSet;
		pos=listNoFoundNodeNo.GetHeadPosition();
		i=0;
		while(pos)
		{
			int iNodeNo2=listNoFoundNodeNo.GetNext(pos);
			if(i==0)
			{
				strTemp.Format(_T(" 节点号 %d "),iNodeNo2);
				i++;
			}
			else
			{
				strTemp.Format(_T(", 节点号 %d "),iNodeNo2);
			}
			strNodeSet+=strTemp;
		}
		if(strNodeSet.GetLength()>0)
		{
			CString strMsg;
			strMsg.Format(_T("错误：有下列支吊架节点号信息未找到 （%s）！"),strNodeSet);
			AfxMessageBox(strMsg);
		}
		frmStatus.UpdateStatus(1.0f ,true);
		frmStatus.UpdateWindow();
		rsResult->Filter=_variant_t(L"");
		rsResult->MoveFirst();
		_variant_t vtmp;
		while(!rsResult->adoEOF)
		{
			vtmp=rsResult->GetCollect("ZDJH");
			rsResult->PutCollect("ZDJH",_variant_t((long)(vtoi(vtmp)+10000)));
			rsResult->MoveNext();
		}
		_ConnectionPtr pCon;
		strSQL=(char*)(bstr_t)rsResult->GetSource();
		pCon=rsResult->GetActiveConnection();
		strSQL.Replace("ORDER BY JSDBH1","ORDER BY FileNameID"); 
		//strSQL.Replace("SElECT * FROM za");
		rsResult->Close();
		rsResult->Open(_variant_t(strSQL),(IDispatch*)pCon,adOpenKeyset,adLockOptimistic,adCmdText);
		//rsData->Sort=_bstr_t("JSDBH1");
		int itmpZdjh=1;
		while(!rsResult->adoEOF)
		{
			rsResult->PutCollect("ZDJH",_variant_t((long)itmpZdjh));
			rsResult->Update();
			itmpZdjh++;
			rsResult->MoveNext();
		}
	}
	       
	catch(_com_error e)
	{
		ShowMessage(e.Description());
	}
	catch(CException *e)
	{
		e->Delete();
	}
	catch(...)
	{
	}
	frmStatus.ShowWindow(SW_HIDE);
}

long GetFileNameId(CString SourceDataFileName,IDispatch *pConID)
{
 		//获得文件ID{
	    long FileNameID1;
		_ConnectionPtr ConDB;
		_RecordsetPtr FilenameIdRd,FilenameId1;
		ConDB.CreateInstance(__uuidof(Connection));
		FilenameIdRd.CreateInstance(__uuidof(Recordset));
		FilenameId1.CreateInstance(__uuidof(Recordset));
		CString FSql,FSql1,ExSql;
		SourceDataFileName.MakeUpper();
        //FilenameId1->Close();
		FSql="SELECT * FROM ZY ORDER BY FileNameID";
		FSql1="SELECT * FROM ZY WHERE FileNameID>=1";
		FilenameId1->Open(_variant_t(FSql1),pConID,adOpenDynamic,adLockOptimistic,adCmdText);	
		ConDB=FilenameId1->GetActiveConnection();
		if(FilenameId1->adoEOF)
		{
			FilenameId1->Close();
			FileNameID1=1;
		}
		else
		{
			FilenameId1->Close();
			FilenameIdRd->Open(_variant_t(FSql),pConID,adOpenDynamic,adLockOptimistic,adCmdText);
		    FilenameIdRd->MoveLast();
			FileNameID1=long(FilenameIdRd->GetCollect("FileNameID"))+1;
		    FilenameIdRd->Close();
		}			
		
		FSql.Format("SELECT * FROM ZY WHERE FileName=\"%s\" AND VolumeID=%ld",SourceDataFileName,EDIBgbl::SelVlmID);
		FilenameIdRd->Open(_variant_t(FSql),pConID,adOpenDynamic,adLockOptimistic,adCmdText);
		if(FilenameIdRd->adoEOF)
		{
			
			ExSql.Format("INSERT INTO [ZY] (FileName,VolumeID,FileNameID) VALUES(\"%s\",%d,%ld)",SourceDataFileName,EDIBgbl::SelVlmID,FileNameID1);
            
			ConDB->Execute(_bstr_t(ExSql),NULL,adCmdText);
            
			//AfxMessageBox("cc");
			//FilenameIdRd->AddNew();
			//FilenameIdRd->PutCollect("FileName",_variant_t(SourceDataFileName));
			//FilenameIdRd->PutCollect("VolumeID",_variant_t(EDIBgbl::SelVlmID));
			//FilenameIdRd->PutCollect("FileNameID",_variant_t(FileNameID1));
		    //FilenameIdRd->Update();
		}
		else	
			FileNameID1=long(FilenameIdRd->GetCollect("FileNameID"));
    FilenameIdRd->Close();
	//AfxMessageBox("bb");
	return FileNameID1;
}

void Delete1()
{   
	_ConnectionPtr ConDB;
	ConDB.CreateInstance(__uuidof(Connection));
	ConDB=::conPRJDB;
	CString strSQL;
	try
	{
            
			strSQL="DELETE * FROM ZY WHERE ZY.FileNameID NOT IN(SELECT DISTINCT FileNameID FROM ZA WHERE NOT ISNULL(ZA.FileNameID))";
            
			ConDB->Execute(_bstr_t(strSQL),NULL,adCmdText);

			//AfxMessageBox("aa");
	}
	catch(CException *e)
	{
		e->ReportError();
		e->Delete();
	}
}












void UpDataZY()
{
	_ConnectionPtr pCon;
	pCon.CreateInstance(__uuidof(Connection));
	pCon=::conPRJDB;
	_RecordsetPtr RD;
	RD.CreateInstance(__uuidof(Recordset));
	CString strUPdata;
	CString strZASQL = "SELECT DISTINCT [ZA].[FileNameID] AS [KEY],[ZY].[VolumeID],[ZA].[VolumeID] FROM ZA,ZY WHERE [ZY].[FileNameID] = [ZA].[FileNameID]";
	try
	{
		RD->Open(_variant_t(strZASQL),(IDispatch *)pCon,adOpenDynamic,adLockOptimistic,adCmdText);
		if(!RD->adoEOF && !RD->BOF)
		{
			RD->MoveFirst();
			CString strZAVolumeID;
			CString strZYVolumeID;
			CString strFileNameID;
			while(!RD->adoEOF)
			{
				//ZA，ZY表中相同的FileNameID对应不同的VolumeID时改ZY表的VolumeID
				strZAVolumeID = vtos(RD->GetCollect("ZA.VolumeID"));
				strZYVolumeID = vtos(RD->GetCollect("ZY.VolumeID"));
				strFileNameID = vtos(RD->GetCollect("KEY"));
				if(strZYVolumeID.CompareNoCase(strZAVolumeID))//不相同
				{
					strUPdata.Format("UPDATE ZY SET [VolumeID] = %s WHERE [FileNameID] = %s ",strZAVolumeID,strFileNameID);
					pCon->Execute(_bstr_t(strUPdata),NULL,adCmdText);
				}	
				RD->MoveNext();
			}
		}
	}
	catch(_com_error & e)
	{
		AfxMessageBox(e.Description());
		RD->Close();
	}
	RD->Close();
}

void ReadPSAData(_Recordset* rsResult ,CString SourceDataFileName,long maxZdjh,long FileNameID)
{
	CString strShareDbPath; //数据库路径
	CString strSql;
	strShareDbPath = GetShareDbPath();
	_ConnectionPtr pRefInfoCon;
	try
	{
		pRefInfoCon.CreateInstance(__uuidof(Connection));
		pRefInfoCon->Open(_bstr_t(::dbConnectionString4+strShareDbPath+_T("ShareRefInfo.mdb")),
			_T(""),_T(""),0);
		WritePHSDataToTable(pRefInfoCon,rsResult,SourceDataFileName,maxZdjh,FileNameID,EDIBgbl::SelVlmID,modPHScal::gsngRatioOfPjg2Pgz);

	}
	catch(CException *e)
	{
		e->ReportError();
	    e->Delete();
	}
	if ( pRefInfoCon->GetState() == adStateOpen)
		pRefInfoCon->Close();

}