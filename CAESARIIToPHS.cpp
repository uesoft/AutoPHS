// CAESARIIToPHS.cpp: implementation of the CAESARIIToPHS class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CAESARIIToPHS.h"
#include "SelPSAProjectNameDlg.h"
#include "user.h"
#include "frmStatus.h"
#include "EDIBgbl.h"
#include "UnitsTransformDlg.h"
#include "modphscal.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAESARIIToPHS::CAESARIIToPHS()
{
	m_IsCaesarError = FALSE;
	m_iSpringLoadType = 0;
//	m_bCheckRigPHSMaxLoad = FALSE;
	m_iUPxyz = 1;
}

CAESARIIToPHS::~CAESARIIToPHS()
{
	if(m_pConnSourceDB->State == adStateOpen)
		m_pConnSourceDB->Close();
}


//pfg20050614 把CASEARII 4.50 中的数据导入到 AutoPhs中去
void  CAESARIIToPHS::ReadResult_CAESARII45(_Recordset* rsResult, CString SourceDataFileName,long maxZdjh,long FileNameID,char * strPSAVER)
{
	//char* strPSAVER = "CASEARII 4.50";  
	CString ConnectionString="Provider=Microsoft.Jet.OLEDB.4.0;Persist Security Info=False;Data Source=%s;Jet OLEDB:Database Password=%s";
	CString dbConnectionString;
   
	_RecordsetPtr m_prsCASENUM;					//热态（冷态）的记录集
	_RecordsetPtr m_prsHangers;					//表[OUTPUT_HANGERS]记录集
	_RecordsetPtr m_prsDisplacements;			//表[OUTPUT_DISPLACEMENTS]记录集
	_RecordsetPtr m_prsRestraints;				//表[OUTPUT_RESTRAINTS]记录集
	_RecordsetPtr m_prsJGHZ;						//每个节点最大值
	_RecordsetPtr m_prsJOBNAME;					//不同工程名的记录集
	_RecordsetPtr prsInputHangers;             //表INPUT_HANGERS 记录集	pfg20050614
	_RecordsetPtr prsInputRestraints;          //表INPUT_RESTRAINTS记录集 pfg20050614
	_RecordsetPtr m_prsBasicData;					//用来获取温度，管径的记录集
   _RecordsetPtr m_prsTempCASENUM;				//用来处理TO_NODE的临时记录集
	m_pConnSourceDB.CreateInstance(__uuidof(Connection));
	m_prsJOBNAME.CreateInstance(__uuidof(Recordset));
	m_prsCASENUM.CreateInstance(__uuidof(Recordset));
	prsInputHangers.CreateInstance(__uuidof(Recordset));
	prsInputRestraints.CreateInstance(__uuidof(Recordset));

	CString m_strSQL;
	CString strFind;
	_variant_t tmpvar;		
	long ltemp;
	long node;
	CString PSA_OutDataFUnit;						//源表的单位
	CString PSA_OutDataMUnit;
	_RecordsetPtr rsData = rsResult;
	CFile m_CAESARIIFile;							//二进制文件对象
	CString m_strCAESARIIFileName;				//二进制文件名
//处理二进制文件所用的变量
	long loff = 272 ;			//偏移量
	long basepoint = 180;   //基本偏移量
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
	float PI=3.1416f;
	int iC1 = 0;
	long iCount = 0;	//节点个数
	int nCount = 0;	//存储节点个数,得到二进制文件中节点的个数
	ltemp = maxZdjh;
	int iJobCount=0;//工程的个数
	CSelPSAProjectNameDlg* m_selDlg;
	long iType=0;//支吊架类型pfg20050624

//	int IsCaesarError=0;//pfg20050627 
	try
	{
		dbConnectionString.Format(ConnectionString,SourceDataFileName,"");
		m_pConnSourceDB->Open(_bstr_t(dbConnectionString),"","",0);

		m_strSQL.Format(_T("SELECT DISTINCT [JOBNAME] FROM [OUTPUT_LOCAL_ELEMENT_FORCES]"));
      //用此方法打开的记录集才能调用GetRecodsetCount()获得记录数
		m_prsJOBNAME->Open(_variant_t(m_strSQL),(IDispatch*)m_pConnSourceDB,adOpenStatic,adLockOptimistic,adCmdText); 
		//在对话框的列表框中加入工程文件名,组合框加入工况名，并获取用户的选择
		m_selDlg = new CSelPSAProjectNameDlg;
 
		 iJobCount= m_prsJOBNAME->GetRecordCount();
		//pfg20050705当工程名称没有时程序应该返回(start)
		if(iJobCount==0)
		{
			AfxMessageBox("数据源为空!");
			return;
		}
		//pfg20050705 当工程名称没有时程序应该回(end)
		m_selDlg->JobNameNum =iJobCount;
		m_pstrTempJobname = new CString[iJobCount];

		int i=0 ;
		for ( i=0 ;i<iJobCount;i++)
		{
			m_pstrTempJobname[i] = vtos(m_prsJOBNAME->GetCollect("JOBNAME"));
			m_prsJOBNAME->MoveNext();
		}
		m_prsJOBNAME->Close();

		m_strSQL.Empty();
		m_strSQL.Format(_T("SELECT DISTINCT [CASE],[JOBNAME] FROM [OUTPUT_LOCAL_ELEMENT_FORCES]"));
		m_prsJOBNAME->Open(_variant_t(m_strSQL),(IDispatch*)m_pConnSourceDB,adOpenStatic,adLockOptimistic,adCmdText);
		
		int ii = m_prsJOBNAME->GetRecordCount();
		m_selDlg->GKNum = ii;
		//pfg20050705这是所有工程的工况,在对话框类中要进行过滤(start)
		m_selDlg->m_pRsJOBNAME=m_prsJOBNAME;
		//pfg20050705这是所有工程的工况,在对话框类中要进行过滤(end)
		m_selDlg->m_pstrProjectName = m_pstrTempJobname;
		if ( m_selDlg->DoModal() == IDOK)
		{
			m_strJOBNAME_P = m_selDlg->m_strTempProject ;
			m_strGKname    = m_selDlg->m_strTempGK ;
			m_iUPxyz			= m_selDlg->iUpxyz ;
			m_strRGKname   = m_selDlg->m_strTempRGK ;
			m_strLGKname	= m_selDlg->m_strTempLGK ;
			m_strUNITFORCE=m_selDlg->m_ForceUnits;//pfg20050630
			m_strUNITMOMENTAL=(m_strUNITFORCE=="N")?("N.m"):("kgf.m");//pfg20050630
		//	m_bCheckRigPHSMaxLoad = m_selDlg->m_bCheckRigPHSMaxLoad;
			m_iSpringLoadType = m_selDlg->m_iSpringLoadType;
		}
		else
		{
			return;   //按下对话框的取消按钮退出提取程序
		}
		m_prsJOBNAME->Close();
		delete m_selDlg;
		delete [] m_pstrTempJobname;


		//显示进度条
		frmStatus.ShowWindow(SW_SHOW);
		frmStatus.m_Label1= GetResStr(IDS_FROMFILE) + SourceDataFileName;
		frmStatus.m_Label2= GetResStr(IDS_TODATABASE) + (LPTSTR)(LPCTSTR)EDIBgbl::dbPRJDB->DefaultDatabase + GetResStr(IDS_InTable) + EDIBgbl::TBNRawData;
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
		m_prsCASENUM->Open(_variant_t(m_strSQL),(IDispatch*)m_pConnSourceDB,adOpenStatic,adLockOptimistic,adCmdText);
		
		iCount = m_prsCASENUM->GetRecordCount();
		if(iCount==0)//pfg20050704没有记录返回
		{
			CString strError;
			strError.Format("提示:工程='%s',工况='%s'在表(OUTPUT_LOCAL_ELEMENT_FORCES)中没有记录!",m_strJOBNAME_P,m_strRGKname);
			AfxMessageBox(strError);
			return;
		}
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

		//取吊架记录集
		m_strSQL.Format("select * from input_hangers where jobname='%s'",m_strJOBNAME_A);
		try
		{
			prsInputHangers->Open(_variant_t(m_strSQL),(IDispatch*)m_pConnSourceDB,adOpenStatic,adLockOptimistic,adCmdText);
			if(prsInputHangers->GetRecordCount()>0)
			{
				prsInputHangers->MoveFirst();
			}
		}
		catch(_com_error e)
		{
			CString strErrorMsg;
			strErrorMsg.Format(_T("%s: %d, %s"), __FILE__, __LINE__, e.Description());
			AfxMessageBox(strErrorMsg);
		}
		//取支架数据
		m_strSQL.Format("select * from input_restraints where jobname='%s'",m_strJOBNAME_A);
		try
		{
			prsInputRestraints->Open(_variant_t(m_strSQL),(IDispatch*)m_pConnSourceDB,adOpenStatic,adLockOptimistic,adCmdText);
			if(prsInputRestraints->GetRecordCount()>0)
			{
				prsInputRestraints->MoveFirst();
			}
		}
		catch(_com_error e)
		{
			CString strErrorMsg;
			strErrorMsg.Format(_T("%s: %d, %s"), __FILE__, __LINE__, e.Description());
			AfxMessageBox(strErrorMsg);
		}
		//20050624pfg(start)
		int n=0,m=0;
		n=prsInputHangers->GetRecordCount();
		n+=prsInputRestraints->GetRecordCount();
		if(n==0)//pfg20050624 当支吊架的个数为零时返回
		{
			AfxMessageBox("表:INPUT_HANGERS和INPUT_RESTRAINTS为空!");
			return;
		}
		//20050624pfg(end)
		
		for(i=0;i<2;i++)
		{
			if(i==0)
			{
				//处理吊架
				for(;!prsInputHangers->adoEOF;prsInputHangers->MoveNext())
				{
					if(m_IsCaesarError) break;//pfg20050627当单位转换出错、终止导入数据
					//导入吊架数据
					ImportHangerRestraints(rsData,prsInputHangers,ltemp,FileNameID,strPSAVER,node,0);
					if(m_IsCaesarError) break;//pfg20050627当单位转换出错、终止导入数据
					
					//导入各方向的推力
					importUnitsForces(rsData,node,m_strJOBNAME_P,PSA_OutDataFUnit,PSA_OutDataMUnit,m_strRGKname,m_strLGKname);
					if(m_IsCaesarError) break;//pfg20050627当单位转换出错、终止导入数据
					
					//导入热态线位移、角位移
					ImportDisplacements(rsData,node,m_strRGKname,m_strJOBNAME_P,1);
					if(m_IsCaesarError) break;//pfg20050627当单位转换出错、终止导入数据
					
					//导入冷态线位移、角位移
					ImportDisplacements(rsData,node,m_strLGKname,m_strJOBNAME_P,0);
					
					//导入管道直径等
					ImportDiameter(rsData,node,m_strJOBNAME_A);
					if(m_IsCaesarError) break;//pfg20050627当单位转换出错、终止导入数据
					
					//导入弹簧数据(包括弹簧类型转换)
					ImportHanger(rsData,node,m_strJOBNAME_P);
					if(m_IsCaesarError) break;//pfg20050627当单位转换出错、终止导入数据
					
					//确定支吊架类型pfg20050622(转换刚性支吊架和恒力弹簧类型)
				//	ConversionTypeCaesarToPhs(rsData,node,m_strJOBNAME_P,m_strRGKname,iType);
				//	ConversionTypeCaesarToPhs(rsData,node,m_strJOBNAME_P,m_strLGKname,iType);
					//pfg20050624(start)
					m++;
					frmStatus.UpdateStatus((float)m/(float)n,true);
					frmStatus.UpdateWindow();
					//pfg20050624(end)
					rsData->Filter="";
					rsData->Update();//pfg20050630
				}
				rsData->Filter="";//pfg20050627
			}   //处理吊架结束
			else
			{
				//处理支架(start)
				for(;!prsInputRestraints->adoEOF;prsInputRestraints->MoveNext())
				{
					if(m_IsCaesarError) break;//pfg20050627当单位转换出错、终止导入数据
					//导入吊架数据
				    ImportHangerRestraints(rsData,prsInputRestraints,ltemp,FileNameID,strPSAVER,node,1);
					if ( node < 1e-6 ) //5.2版本中所有的约束信息会导入INPUT_RESTRAINTS表中(同一约束点会有四条记录，所以会有NODE_NUM为-1情况)
						continue; 
					if(m_IsCaesarError) break;//pfg20050627当单位转换出错、终止导入数据
					
					//确定支吊架类型pfg20050622
					ConversionTypeCaesarToPhs(rsData,node,m_strJOBNAME_P,m_strRGKname,iType);
			//		ConversionTypeCaesarToPhs(rsData,node,m_strJOBNAME_P,m_strLGKname,iType);
					
					if(iType==177)//pfg20050624当是固定支架时、它的冷态采用另一种工况
					{
						//导入各方向的推力
						importUnitsForces(rsData,node,m_strJOBNAME_P,PSA_OutDataFUnit,PSA_OutDataMUnit,m_strRGKname,m_strGKname);
						if(m_IsCaesarError) break;//pfg20050627当单位转换出错、终止导入数据
						
						//导入冷态线位移、角位移
						ImportDisplacements(rsData,node,m_strGKname,m_strJOBNAME_P,0);
						if(m_IsCaesarError) break;//pfg20050627当单位转换出错、终止导入数据
					}
					else
					{
						//导入各方向的推力
						importUnitsForces(rsData,node,m_strJOBNAME_P,PSA_OutDataFUnit,PSA_OutDataMUnit,m_strRGKname,m_strLGKname);
						if(m_IsCaesarError) break;//pfg20050627当单位转换出错、终止导入数据
						
						//导入冷态线位移、角位移
						ImportDisplacements(rsData,node,m_strLGKname,m_strJOBNAME_P,0);
						if(m_IsCaesarError) break;//pfg20050627当单位转换出错、终止导入数据
					}

					//导入热态线位移、角位移
					ImportDisplacements(rsData,node,m_strRGKname,m_strJOBNAME_P,1);
					if(m_IsCaesarError) break;//pfg20050627当单位转换出错、终止导入数据
					
					//导入管道直径等
					ImportDiameter(rsData,node,m_strJOBNAME_A);
					if(m_IsCaesarError) break;//pfg20050627当单位转换出错、终止导入数据
					
					//导入弹簧数据
				//	ImportHanger(rsData,node,m_strJOBNAME_P);
					if(m_IsCaesarError) break;//pfg20050627当单位转换出错、终止导入数据
					//pfg20050624(start)
					m++;
					frmStatus.UpdateStatus((float)m/(float)n,true);
					frmStatus.UpdateWindow();
					//pfg20050624(end)
					rsData->Filter="";
					rsData->Update();//pfg20050630
				}
				rsData->Filter="";//pfg20050627
			}   //处理支架结束(end)
			//rsData->Update();//pfg20050630
		}
	}catch(_com_error e)
	{
		CString strErrorMsg;
		strErrorMsg.Format(_T("%s: %d, %s"), __FILE__, __LINE__, e.Description());
		AfxMessageBox(strErrorMsg);
	}
}


//吊架和支架数据的导入pfg20050615
//flag=0 为吊架; flag=1 为支架
//记录集是结果集 对应ZA表
//rsHR记录集是数据源集(弹簧、吊架)
//ltemp 支吊架号
void CAESARIIToPHS::ImportHangerRestraints(_RecordsetPtr rsData,_RecordsetPtr rsHR,long &ltemp,long &FileNameID,char *strPSAVER,long &node,int flag)
{
	_variant_t tmpvar; 
	long lTmp;
	try
	{
		if(flag)
		{
			tmpvar=rsHR->GetCollect(_T("node_num"));
			lTmp=vtoi(rsHR->GetCollect(_T("RES_TYPEID")));//pfg20050624
		}
		else
		{
			tmpvar=rsHR->GetCollect(_T("node"));
		}
		if(tmpvar.vt==VT_EMPTY||tmpvar.vt==VT_NULL)
		{
			return;
		}
		else
		{
			node=vtoi(tmpvar);
		}
		if ( node < 1e-6 ) //5.2版本中所有的约束信息会导入INPUT_RESTRAINTS表中(同一约束点会有四条记录，所以会有NODE_NUM为-1情况)
			return;
		if(rsData->GetRecordCount()>0)
		{
			rsData->MoveFirst();
		}
		
		rsData->Filter = _variant_t(CString("[jsdbh1] = ") + ltos(node)+" AND [FileNameID]="+ltos(FileNameID));
		
		if (rsData->adoEOF&&rsData->BOF)
		{
			rsData->AddNew();
			rsData->PutCollect("VolumeID",_variant_t(EDIBgbl::SelVlmID));
			rsData->PutCollect("zdjh",_variant_t((long)(ltemp+1)));
			rsData->PutCollect("FileNameID",_variant_t(FileNameID));
			ltemp++;
			rsData->Update();					
		}
		rsData->PutCollect("PSAver", _variant_t(strPSAVER));
		rsData->PutCollect("UPxyz",_variant_t((long)m_iUPxyz));
		rsData->PutCollect("JSDBH1", _variant_t((long)node));
		rsData->Update();
	}
	catch(_com_error e)
	{
		CString strErrorMsg;
		strErrorMsg.Format(_T("%s: %d, %s"), __FILE__, __LINE__, e.Description());
		AfxMessageBox(strErrorMsg);
	}
}

//导入支架、吊架中的单位，各个方向的推力(冷态、热态)
//rsData为结果数据库	m_strLGKname为冷态、m_strRGKname为热态
//m_strJOBNAME_P 工程名称
void CAESARIIToPHS::importUnitsForces(_RecordsetPtr rsData,long node,CString m_strJOBNAME_P,CString PSA_OutDataFUnit,CString PSA_OutDataMUnit,CString m_strRGKname,CString m_strLGKname)
{
	CString strSQL;
	CString strMessage;
	_RecordsetPtr prsOutputRestraints;
	int i=0;
	int IsXYZ=0;//是否在x,y,z方向已经导入了数据，因为约束类型很多，对于同一方向有多
				//个约束时，需要提示用户我们还没有对这一种情况进行处理 当百位为1
				//表示x方向已经导入了数据，依此类推
	int IsmXYZ=0;//表示力矩，含义与IsXYZ类似
	double tmpVar;
	double dFx=1.0;//力的转换因子
	double dFOffset=0.0;//力的偏移量
	double dMx=1.0;//力矩的转换因子
	double dMOffset=0.0;//力矩的偏移量
	CString strFUnits;//力的单位；
	CString strMUnits;//力矩的单位

	double dICCWorkLoad = 0.0,dICCIncLoad = 0.0; //向上轴热态、冷态荷载

	strFUnits=PSA_OutDataFUnit;
	strMUnits=PSA_OutDataMUnit;
	CString strTmp;
	prsOutputRestraints.CreateInstance(__uuidof(Recordset));
	try
	{
		//转换后力的单位:kgf;力矩的单位:kgf.m写入ZA表中(start)			 				 
		rsData->PutCollect("Unit_Force",_variant_t(m_strUNITFORCE));
		rsData->PutCollect("Unit_Momental",_variant_t(m_strUNITMOMENTAL));
		//转换后力的单位:kgf;力矩的单位:kgf.m写入ZA表中(end)

		//导入热态的荷载、力矩(start)
		strSQL.Format(_T("SELECT * FROM [OUTPUT_RESTRAINTS] WHERE [NODE] = %d AND [CASE] = '%s' and JOBNAME='%s' "),node,m_strRGKname,m_strJOBNAME_P);
		prsOutputRestraints = m_pConnSourceDB->Execute(_bstr_t(strSQL),NULL,adCmdText);
		if(!(prsOutputRestraints->BOF&&prsOutputRestraints->adoEOF))
		{
			//pfg20050622(start)
			strTmp=vtos(prsOutputRestraints->GetCollect(_T("FUNITS")));
			strFUnits=(strTmp!="")?strTmp:strFUnits;
			//单位换算
			dFx=UnitsToUeUnits(rsData,strFUnits,m_strUNITFORCE,dFOffset);
			if(m_IsCaesarError) return;//pfg20050629当单位转换出错、终止导入数据
			strTmp=vtos(prsOutputRestraints->GetCollect(_T("MUNITS")));
			strMUnits=(strTmp!="")?strTmp:strMUnits;
			dMx=UnitsToUeUnits(rsData,strMUnits,m_strUNITMOMENTAL,dMOffset);
			if(m_IsCaesarError) return;//pfg20050629当单位转换出错、终止导入数据
			//pfg20050622(end)
			prsOutputRestraints->MoveFirst();
			while(!prsOutputRestraints->adoEOF)
			{
				tmpVar=vtof(prsOutputRestraints->GetCollect("FX"));
				if( (tmpVar!=0) && (IsXYZ/100==0) || i==0 )
				{//当它是第一次导入或者以前在这一方向没有荷载
					rsData->PutCollect("pxr1",_variant_t(tmpVar*dFx+dFOffset));
					IsXYZ=(tmpVar!=0)?(IsXYZ+100):IsXYZ;
					if ( m_iUPxyz == 1 ) //X轴向上
					{
						dICCWorkLoad = tmpVar*dFx+dFOffset;
					}
				}
				else if((tmpVar!=0)&&(IsXYZ/100))
				{
					strMessage.Format("警告:节点号为%d的支吊点在X方向上有几个荷载(请阅读使用说明)!",node);
					AfxMessageBox(strMessage);
				}

				tmpVar=vtof(prsOutputRestraints->GetCollect("FY"));
				if(((tmpVar!=0)&&(((IsXYZ%100)/10)==0))||(i==0))
				{//当它是第一次导入或者以前在这一方向没有荷载
					rsData->PutCollect("pyr1",_variant_t(tmpVar*dFx+dFOffset));
					IsXYZ=(tmpVar!=0)?(IsXYZ+10):IsXYZ;
					if ( m_iUPxyz == 2 ) //Y轴向上
					{
						dICCWorkLoad = tmpVar*dFx+dFOffset;
					}
				}
				else if((tmpVar!=0)&&((IsXYZ%100)/10))
				{
					strMessage.Format("警告:节点号为%d的支吊点在Y方向上有几个荷载(请阅读使用说明)!",node);
					AfxMessageBox(strMessage);
				}
				tmpVar=vtof(prsOutputRestraints->GetCollect("FZ"));
				if(((tmpVar!=0)&&(IsXYZ%10==0))||(i==0))
				{//当它是第一次导入或者以前在这一方向没有荷载
					rsData->PutCollect("pzr1",_variant_t(tmpVar*dFx+dFOffset));
					IsXYZ=(tmpVar!=0)?(IsXYZ+1):IsXYZ;
					if ( m_iUPxyz == 3 ) //Z轴向上
					{
						dICCWorkLoad = tmpVar*dFx+dFOffset;
					}
				}
				else if((tmpVar!=0)&&(IsXYZ%10))
				{
					strMessage.Format("警告:节点号为%d的支吊点在Z方向上有几个荷载(请阅读使用说明)!",node);
					AfxMessageBox(strMessage);
				}
				tmpVar=vtof(prsOutputRestraints->GetCollect("MX"));
				if((tmpVar!=0)&&(IsmXYZ/100==0)||i==0)
				{//当它是第一次导入或者以前在这一方向没有荷载
					rsData->PutCollect("mxr1",_variant_t(tmpVar*dMx+dMOffset));
					IsmXYZ=(tmpVar!=0)?(IsmXYZ+100):IsmXYZ;
				}
				else if((tmpVar!=0)&&(IsmXYZ/100))
				{
					strMessage.Format("警告:节点号为%d的支吊点在X方向上有几个力矩(请阅读使用说明)!",node);
					AfxMessageBox(strMessage);
				}
				tmpVar=vtof(prsOutputRestraints->GetCollect("MY"));
				if(((tmpVar!=0)&&(((IsmXYZ%100)/10)==0))||(i==0))
				{//当它是第一次导入或者以前在这一方向没有荷载
					rsData->PutCollect("myr1",_variant_t(tmpVar*dMx+dMOffset));
					IsmXYZ=(tmpVar!=0)?(IsmXYZ+10):IsmXYZ;
				}
				else if((tmpVar!=0)&&((IsmXYZ%100)/10))
				{
					strMessage.Format("警告:节点号为%d的支吊点在Y方向上有几个力矩(请阅读使用说明)!",node);
					AfxMessageBox(strMessage);
				}
				tmpVar=vtof(prsOutputRestraints->GetCollect("MZ"));
				if(((tmpVar!=0)&&(IsmXYZ%10==0))||(i==0))
				{//当它是第一次导入或者以前在这一方向没有荷载
					rsData->PutCollect("mzr1",_variant_t(tmpVar*dMx+dMOffset));
					IsmXYZ=(tmpVar!=0)?(IsmXYZ+1):IsmXYZ;
				}
				else if((tmpVar!=0)&&(IsmXYZ%10))
				{
					strMessage.Format("警告:节点号为%d的支吊点在Z方向上有几个力矩(请阅读使用说明)!",node);
					AfxMessageBox(strMessage);
				}
				i++;
				prsOutputRestraints->MoveNext();
			}
		}
		if(prsOutputRestraints->State==adStateOpen)
		{
			prsOutputRestraints->Close();
		}
		//导入热态的荷载、力矩(end)

		//导入冷态的荷载、力矩(start)
		IsXYZ=0;
		IsmXYZ=0;
		i=0;
		strSQL.Format(_T("SELECT * FROM [OUTPUT_RESTRAINTS] WHERE [NODE] = %d AND [CASE] = '%s' and JOBNAME='%s' "),node,m_strLGKname,m_strJOBNAME_P);
		prsOutputRestraints = m_pConnSourceDB->Execute(_bstr_t(strSQL),NULL,adCmdText);
		if(!(prsOutputRestraints->BOF&&prsOutputRestraints->adoEOF))
		{
			//pfg20050622(start)
			strTmp=vtos(prsOutputRestraints->GetCollect(_T("FUNITS")));
			strFUnits=(strTmp!="")?strTmp:strFUnits;
			//单位换算
			dFx=UnitsToUeUnits(rsData,strFUnits,m_strUNITFORCE,dFOffset);
			if(m_IsCaesarError) return;//pfg20050629当单位转换出错、终止导入数据
			strTmp=vtos(prsOutputRestraints->GetCollect(_T("MUNITS")));
			strMUnits=(strTmp!="")?strTmp:strMUnits;
			dMx=UnitsToUeUnits(rsData,strMUnits,m_strUNITMOMENTAL,dMOffset);
			if(m_IsCaesarError) return;//pfg20050629当单位转换出错、终止导入数据
			//pfg20050622(end)
			prsOutputRestraints->MoveFirst();
			while(!prsOutputRestraints->adoEOF)
			{
				tmpVar=vtof(prsOutputRestraints->GetCollect("FX"));
				if((tmpVar!=0)&&(IsXYZ/100==0)||i==0)
				{//当它是第一次导入或者以前在这一方向没有荷载
					rsData->PutCollect("pxl1",_variant_t(tmpVar*dFx+dFOffset));
					IsXYZ=(tmpVar!=0)?(IsXYZ+100):IsXYZ;

					if ( m_iUPxyz == 1 ) //X轴向上
					{
						dICCIncLoad = tmpVar*dFx+dFOffset;
					}
				}
				else if((tmpVar!=0)&&(IsXYZ/100))
				{
					strMessage.Format("警告:节点号为%d的支吊点在X方向上有几个荷载(请阅读使用说明)!",node);
					AfxMessageBox(strMessage);
				}

				tmpVar=vtof(prsOutputRestraints->GetCollect("FY"));
				if(((tmpVar!=0)&&(((IsXYZ%100)/10)==0))||(i==0))
				{//当它是第一次导入或者以前在这一方向没有荷载
					rsData->PutCollect("pyl1",_variant_t(tmpVar*dFx+dFOffset));
					IsXYZ=(tmpVar!=0)?(IsXYZ+10):IsXYZ;
					if ( m_iUPxyz == 2 ) //Y轴向上
					{
						dICCIncLoad = tmpVar*dFx+dFOffset;
					}
				}
				else if((tmpVar!=0)&&((IsXYZ%100)/10))
				{
					strMessage.Format("警告:节点号为%d的支吊点在Y方向上有几个荷载(请阅读使用说明)!",node);
					AfxMessageBox(strMessage);
				}
				tmpVar=vtof(prsOutputRestraints->GetCollect("FZ"));
				if(((tmpVar!=0)&&(IsXYZ%10==0))||(i==0))
				{//当它是第一次导入或者以前在这一方向没有荷载
					rsData->PutCollect("pzl1",_variant_t(tmpVar*dFx+dFOffset));
					IsXYZ=(tmpVar!=0)?(IsXYZ+1):IsXYZ;
					if ( m_iUPxyz == 3 ) //Z轴向上
					{
						dICCIncLoad = tmpVar*dFx+dFOffset;
					}
				}
				else if((tmpVar!=0)&&(IsXYZ%10))
				{
					strMessage.Format("警告:节点号为%d的支吊点在Z方向上有几个荷载(请阅读使用说明)!",node);
					AfxMessageBox(strMessage);
				}
				tmpVar=vtof(prsOutputRestraints->GetCollect("MX"));
				if((tmpVar!=0)&&(IsmXYZ/100==0)||i==0)
				{//当它是第一次导入或者以前在这一方向没有荷载
					rsData->PutCollect("mxl1",_variant_t(tmpVar*dMx+dMOffset));
					IsmXYZ=(tmpVar!=0)?(IsmXYZ+100):IsmXYZ;
				}
				else if((tmpVar!=0)&&(IsmXYZ/100))
				{
					strMessage.Format("警告:节点号为%d的支吊点在X方向上有几个力矩(请阅读使用说明)!",node);
					AfxMessageBox(strMessage);
				}
				tmpVar=vtof(prsOutputRestraints->GetCollect("MY"));
				if(((tmpVar!=0)&&(((IsmXYZ%100)/10)==0))||(i==0))
				{//当它是第一次导入或者以前在这一方向没有荷载
					rsData->PutCollect("myl1",_variant_t(tmpVar*dMx+dMOffset));
					IsmXYZ=(tmpVar!=0)?(IsmXYZ+10):IsmXYZ;
				}
				else if((tmpVar!=0)&&((IsmXYZ%100)/10))
				{
					strMessage.Format("警告:节点号为%d的支吊点在Y方向上有几个力矩(请阅读使用说明)!",node);
					AfxMessageBox(strMessage);
				}
				tmpVar=vtof(prsOutputRestraints->GetCollect("MZ"));
				if(((tmpVar!=0)&&(IsmXYZ%10==0))||(i==0))
				{//当它是第一次导入或者以前在这一方向没有荷载
					rsData->PutCollect("mzl1",_variant_t(tmpVar*dMx+dMOffset));
					IsmXYZ=(tmpVar!=0)?(IsmXYZ+1):IsmXYZ;
				}
				else if((tmpVar!=0)&&(IsmXYZ%10))
				{
					strMessage.Format("警告:节点号为%d的支吊点在Z方向上有几力矩(请阅读使用说明)!",node);
					AfxMessageBox(strMessage);
				}
				i++;
				prsOutputRestraints->MoveNext();
			}
		}
		if(prsOutputRestraints->State==adStateOpen)
		{
			prsOutputRestraints->Close();
		}

		//先按默认的值设置工作荷载和安装荷载
		rsData->PutCollect("PGZ1",_variant_t( dICCWorkLoad) ); //工作荷载
		rsData->PutCollect("paz0", _variant_t( dICCIncLoad ) ); //安装荷载
		rsData->PutCollect("paz1", _variant_t( dICCIncLoad ) ); 

		double dMaxLoad = 0.0;
		//导入冷态的荷载、力矩(end)
		if ( IsHanger( node,m_strJOBNAME_P ) )
		{
			if ( m_iSpringLoadType == 1 ) //弹簧荷载提取工况方式(0:按上述选择的冷/热态提取荷载值 1按CAESARII弹簧表中的荷载值提取)
			{
				double dWorkLoad = 0.0,dInsLoad = 0.0;
				if ( GetHangerLoad( node,m_strJOBNAME_P,dWorkLoad,dInsLoad ) ) //得到弹簧荷载
				{
					rsData->PutCollect("PGZ1",_variant_t( dWorkLoad*dFx+dFOffset) );
					rsData->PutCollect("paz0", _variant_t( dInsLoad*dFx+dFOffset )); //冷态
					rsData->PutCollect("paz1", _variant_t( dInsLoad*dFx+dFOffset ));

					//改变向上轴的荷载值
					if ( m_iUPxyz == 1 )
					{
						rsData->PutCollect("pxl1", _variant_t( dInsLoad*dFx+dFOffset ));//冷态推力
						rsData->PutCollect("pxr1", _variant_t( dWorkLoad*dFx+dFOffset ));//热态推力
					}
					else if ( m_iUPxyz == 2 )
					{
						rsData->PutCollect("pyl1", _variant_t( dInsLoad*dFx+dFOffset ));//冷态推力
						rsData->PutCollect("pyr1", _variant_t( dWorkLoad*dFx+dFOffset ));//热态推力
					}
					else if ( m_iUPxyz == 3 )
					{
						rsData->PutCollect("pzl1", _variant_t( dInsLoad*dFx+dFOffset ));//冷态推力
						rsData->PutCollect("pzr1", _variant_t( dWorkLoad*dFx+dFOffset ));//热态推力
					}
				}
			}
		}
		
	}
	catch(_com_error e)
	{
		CString strErrorMsg;
		strErrorMsg.Format(_T("%s: %d, %s"), __FILE__, __LINE__, e.Description());
		AfxMessageBox(strErrorMsg);
	}
}

//导入支吊架的线位移、角位移
//rsData 结果表(ZA)记录集、strLGkname 工况名称、strJDBNAME_P 工程名称
//flag =1 热态 、flag=0为冷态
void CAESARIIToPHS::ImportDisplacements(_RecordsetPtr rsData,long node, CString strLGKname,CString strJOBNAME_P,int flag)
{
	double x= 1.0;//单位换算因子
	double dLOffset=0.0;//长度的偏移
	double dROffset=0.0;//角度的偏移
	CString strSQL,strError;
	CString strUnits;
	_RecordsetPtr prsDisplacements;
	prsDisplacements.CreateInstance(__uuidof(Recordset));
	try
	{
		if(flag)
		{
			//热态
			strSQL.Format(_T("SELECT * FROM [OUTPUT_DISPLACEMENTS] WHERE [NODE] = %d AND [CASE] = '%s' AND [JOBNAME] =  '%s' "),node,strLGKname,strJOBNAME_P);						
			prsDisplacements = m_pConnSourceDB->Execute(_bstr_t(strSQL),NULL,adCmdText);
			if ( prsDisplacements->adoEOF && prsDisplacements->BOF )
			{
				strError.Format( _T("OUTPUT_DISPLACEMENTS表中没有点号为%d用工况号为%s的数据"),node,strLGKname );
				AfxMessageBox( strError );
				prsDisplacements->Close();
				return;
			}
			
			//长度单位换算
			strUnits=vtos(prsDisplacements->GetCollect(_T("DUNITS")));
			strUnits.TrimLeft();
			strUnits.TrimRight();
			strUnits.MakeLower();
			if (strUnits == "mm")
			{
				x=1.0;
			}
			else
			{
				x=UnitsToUeUnits(rsData,strUnits,"mm",dLOffset);
				if(m_IsCaesarError) return;//pfg20050629当单位转换出错、终止导入数据

			}
			//热态线位移
			rsData->PutCollect("xr1",_variant_t(vtof(prsDisplacements->GetCollect("dX"))*x+dLOffset));
			rsData->PutCollect("yr1",_variant_t(vtof(prsDisplacements->GetCollect("dY"))*x+dLOffset));					 
			rsData->PutCollect("zr1",_variant_t(vtof(prsDisplacements->GetCollect("dZ"))*x+dLOffset));	
			rsData->PutCollect("UNIT_Displacement",_T("mm"));

			//角度单位换算
			strUnits=vtos(prsDisplacements->GetCollect(_T("RUNITS")));
			strUnits.TrimLeft();
			strUnits.TrimRight();
			if(strUnits=="弧度")
			{
				x=1.0;
			}
			else
			{
				x=UnitsToUeUnits(rsData,strUnits,"弧度",dROffset);
				if(m_IsCaesarError) return;//pfg20050629当单位转换出错、终止导入数据

			}
			//热态角位移
			rsData->PutCollect("rxR1",_variant_t(vtof(prsDisplacements->GetCollect("RX"))*x+dROffset));
			rsData->PutCollect("ryR1",_variant_t(vtof(prsDisplacements->GetCollect("RY"))*x+dROffset));					 
			rsData->PutCollect("rzR1",_variant_t(vtof(prsDisplacements->GetCollect("RZ"))*x+dROffset));	
		}
		else
		{
			//冷态
			strSQL.Format(_T("SELECT * FROM [OUTPUT_DISPLACEMENTS] WHERE [NODE] = %d AND [CASE] = '%s' AND [JOBNAME] =  '%s' "),node,strLGKname,strJOBNAME_P);						
			prsDisplacements = m_pConnSourceDB->Execute(_bstr_t(strSQL),NULL,adCmdText);

			if ( prsDisplacements->adoEOF && prsDisplacements->BOF )
			{
				strError.Format( _T("OUTPUT_DISPLACEMENTS表中没有点号为%d用工况号为%s的数据"),node,strLGKname );
				AfxMessageBox( strError );
				prsDisplacements->Close();
				return;
			}
			
			//长度单位换算
			strUnits=vtos(prsDisplacements->GetCollect(_T("DUNITS")));
			strUnits.TrimLeft();
			strUnits.TrimRight();
			strUnits.MakeLower();
			if (strUnits == "mm")
			{
				x=1.0;
			}
			else
			{
				x=UnitsToUeUnits(rsData,strUnits,"mm",dLOffset);
				if(m_IsCaesarError) return;//pfg20050629当单位转换出错、终止导入数据

			}
			//冷态线位移
			rsData->PutCollect("xL1",_variant_t(vtof(prsDisplacements->GetCollect("dX"))*x+dLOffset));
			rsData->PutCollect("yL1",_variant_t(vtof(prsDisplacements->GetCollect("dY"))*x+dLOffset));					 
			rsData->PutCollect("zL1",_variant_t(vtof(prsDisplacements->GetCollect("dZ"))*x+dLOffset));	

			//角度单位换算
			strUnits=vtos(prsDisplacements->GetCollect(_T("RUNITS")));
			strUnits.TrimLeft();
			strUnits.TrimRight();
			if(strUnits=="弧度")
			{
				x=1.0;
			}
			else
			{
				x=UnitsToUeUnits(rsData,strUnits,"弧度",dROffset);
				if(m_IsCaesarError) return;//pfg20050629当单位转换出错、终止导入数据

			}
			//冷态角位移
			rsData->PutCollect("rxL1",_variant_t(vtof(prsDisplacements->GetCollect("RX"))*x+dROffset));
			rsData->PutCollect("ryL1",_variant_t(vtof(prsDisplacements->GetCollect("RY"))*x+dROffset));					 
			rsData->PutCollect("rzL1",_variant_t(vtof(prsDisplacements->GetCollect("RZ"))*x+dROffset));	
		}
	}
	catch(_com_error e)
	{
		CString strErrorMsg;
		strErrorMsg.Format(_T("%s: %d, %s"), __FILE__, __LINE__, e.Description());
		AfxMessageBox(strErrorMsg);
	}
}

//导入管道外径、介质温度
void CAESARIIToPHS::ImportDiameter(_RecordsetPtr rsData,long node,CString strJOBNAME_A)
{
	CString strSQL;
	_RecordsetPtr pRs;
	double dDiameterx=1.0;//直径的换算因子
	double dDOffset=0.0;//直径偏移量
	double dT01x=1.0;//温度的换算因子
	double dTOffset=0.0;//温度偏移量
	CString str;
	pRs.CreateInstance(__uuidof(Recordset));
	try
	{
		strSQL.Format(_T("select * from input_units where jobname='%s'"),strJOBNAME_A);
		pRs=m_pConnSourceDB->Execute(_bstr_t(strSQL),NULL,adCmdText);
		str=vtos(pRs->GetCollect(_T("LENGTH")));
		str.TrimLeft();
		str.TrimRight();
		str.MakeUpper();
		if(!(str=="MM"))
		{
			dDiameterx=UnitsToUeUnits(rsData,str,"MM",dDOffset);
			if(m_IsCaesarError) return;//pfg20050629当单位转换出错、终止导入数据
			
		}
		str=vtos(pRs->GetCollect(_T("TEMP")));
		str.TrimLeft();
		str.TrimRight();
		str.MakeUpper();
		//pfg20050630
		dT01x=UnitsToUeUnits(rsData,str,"C",dTOffset);
		if(m_IsCaesarError) return;//pfg20050629当单位转换出错、终止导入数据

//chenbl 2010.05.15
		
	//找出单元记录
		//先找起点号
		pRs->Close();
		strSQL.Format(_T("SELECT * FROM [INPUT_BASIC_ELEMENT_DATA] WHERE [JOBNAME] =  '%s' AND [FROM_NODE] = %d"),strJOBNAME_A,node);
		pRs->Open(_variant_t(strSQL),(IDispatch*)m_pConnSourceDB,adOpenStatic,adLockOptimistic,adCmdText);
		if ( pRs->BOF && pRs->adoEOF ) //没有找到记录
		{
			pRs->Close();
			//找末点
			strSQL.Format(_T("SELECT * FROM [INPUT_BASIC_ELEMENT_DATA] WHERE [JOBNAME] =  '%s' AND [TO_NODE] = %d"),strJOBNAME_A,node);
			pRs->Open(_variant_t(strSQL),(IDispatch*)m_pConnSourceDB,adOpenStatic,adLockOptimistic,adCmdText);
			if ( pRs->BOF && pRs->adoEOF ) //没有找到记录
			{
				pRs->Close();
				strSQL.Format(_T("SELECT * FROM [INPUT_BASIC_ELEMENT_DATA],[INPUT_BENDS] WHERE [INPUT_BENDS].[JOBNAME] =  '%s' AND  ( [INPUT_BENDS].NODE1 = %d OR [INPUT_BENDS].NODE2 = %d OR [INPUT_BENDS].NODE3 = %d ) AND [INPUT_BASIC_ELEMENT_DATA].BEND_PTR = [INPUT_BENDS].BEND_PTR"),
					strJOBNAME_A,node,node,node);
				pRs->Open(_variant_t(strSQL),(IDispatch*)m_pConnSourceDB,adOpenStatic,adLockOptimistic,adCmdText);
				if ( pRs->BOF && pRs->adoEOF ) //没有找到记录
				{
					pRs->Close();
					return;
				}
			}
		}
		rsData->PutCollect("DJ1",_variant_t(vtof(pRs->GetCollect("DIAMETER"))*dDiameterx+dDOffset));
		rsData->PutCollect("T01",_variant_t(vtof(pRs->GetCollect("TEMP_EXP_C1"))*dT01x+dTOffset));
		
		if(pRs->State==adStateOpen)
		{
			pRs->Close();
		}
	}
	catch(_com_error e)
	{
		CString strErrorMsg;
		strErrorMsg.Format(_T("%s: %d, %s"), __FILE__, __LINE__, e.Description());
		AfxMessageBox(strErrorMsg);
	}
}

//导入弹簧数据(变力弹簧)
void CAESARIIToPHS::ImportHanger(_RecordsetPtr rsData,long node,CString strJOBNAME_P)
{
	CString strSQL;
	_RecordsetPtr pRs;
	double dLoadx=1.0;//力的单位转换因子
	double dFOffset=0.0;//力的偏移量
	_variant_t tmpvar;
	CString str;
	CString strSize;
	CString strPHSType;
	try
	{
		strSQL.Format(_T("SELECT * FROM [OUTPUT_HANGERS] WHERE [NODE] = %d AND [JOBNAME] =  '%s'" ),
			node,strJOBNAME_P);
		pRs = m_pConnSourceDB->Execute(_bstr_t(strSQL),NULL,adCmdText);
		if(!(pRs->BOF&&pRs->adoEOF))
		{
			pRs->MoveFirst();
			//力的单位转换
		/*
			str=vtos(pRs->GetCollect(_T("LOAD_UNITS")));
			str.TrimLeft();
			str.TrimRight();
			str.MakeLower();
			dLoadx=UnitsToUeUnits(rsData,str,m_strUNITFORCE,dFOffset);
			if(m_IsCaesarError) return;//pfg20050629当单位转换出错、终止导入数据
		*/	
			//pgz1工作荷载已经在表:OUTPUT_RESTRAINTS中导入了
			//	rsData->PutCollect("pgz1",_variant_t(vtof(pRs->GetCollect("HOT_LOAD"))*dLoadx));
		//	rsData->PutCollect("paz0",_variant_t(vtof(pRs->GetCollect("TH_INSTALL_LOAD"))*dLoadx+dFOffset));
		//	rsData->PutCollect("paz1",_variant_t(vtof(pRs->GetCollect("AC_INSTALL_LOAD"))*dLoadx+dFOffset));
			
			double dLoadVariation = 0.0;
			dLoadVariation = vtof( pRs->GetCollect("LOAD_VARIATION") );
			if ( dLoadVariation < 1E-6 )
			{
				strPHSType = _T("9999"); //恒力弹簧
			}
			else //变力弹簧
			{
				rsData->PutCollect("CoefOfVarLoad",_variant_t( dLoadVariation /100 ) );
				tmpvar = pRs->GetCollect("SIZE");
				strSize.Format(_T("%02d"),vtoi(tmpvar));
				
				//并联数
				int iParalNum = vtoi( pRs->GetCollect( _T("NUMREQ") ) );
				CString strHangerType = vtos( pRs->GetCollect( _T("FIGNUM") ) );
				//串联数
				int iSeriesNum = 0;
				if ( !GetHangerSeriesNum(strHangerType,iSeriesNum) ) //得到弹簧不成功
				{
					CString strError;
					strError.Format( _T("无法通过弹簧类型%s获得弹簧的串联数,串联数默认为1"),strHangerType );
					AfxMessageBox( strError );
					iSeriesNum = 1;
				}
				
				//形成类似GLIF定义的支吊架类型号								
				strPHSType.Format( _T("%d%d%s"),iSeriesNum,iParalNum,strSize );
				
			}
			rsData->PutCollect("TYPE",_variant_t(strPHSType));
			rsData->PutCollect("psaTYPE",_variant_t(strPHSType));
		}
	}
	catch(_com_error e)
	{
		CString strErrorMsg;
		strErrorMsg.Format(_T("%s: %d, %s"), __FILE__, __LINE__, e.Description());
		AfxMessageBox(strErrorMsg);
	}
	if ( pRs->State == adStateOpen )
	{
		pRs->Close();
	}
}

//确定支架、吊架类型(约束)
//rsData 结果记录集(ZA表)
//node 当前的支吊节点
//strJOBNAME_A 工程名称,strLGKname
//返回：成功；支吊架的类型，如果没有返回0 ，请再更换另一种工况重试 
//注意：弹簧支吊架只简单分为可变弹簧和恒力弹簧
//支架也只处理以下几种:X、Y、Z、XY、XZ、YZ、XYZ
//iType:支吊架类型
void CAESARIIToPHS::ConversionTypeCaesarToPhs(_RecordsetPtr rsData,long node,CString strJOBNAME_P,CString strGKname,long &iType)
{
	_RecordsetPtr pRs;
	pRs.CreateInstance(__uuidof(Recordset));
	int i=0;
	iType=0;//支吊架类型:AutoPSA和AutoPHS的约束类型
	CString strType;//CAESARII的约束类型
	CString strLineType;//线位移的约束类型
	CString strRadType;//角位移的约束类型
	short iDECType=0;//线位移(十位)的约束类型
	short iNUMType=0;//角位移(个位)的约束类型
	CString strSQL;	
	strLineType=_T("");
	try
	{
		strSQL.Format(_T("SELECT * FROM [OUTPUT_RESTRAINTS] WHERE [NODE] = %d AND [CASE] = '%s' and JOBNAME='%s' "),node,strGKname,strJOBNAME_P);
		pRs->Open(_variant_t(strSQL),(IDispatch*)m_pConnSourceDB,adOpenStatic,adLockOptimistic,adCmdText);
		
		while ( !pRs->adoEOF )
		{
			strType = vtos( pRs->GetCollect( _T("TYPE") ) );
			strType.MakeUpper();
			//线位移约束
			if(strType=="RIGID X" || strType=="RIGID +X" || strType=="RIGID -X")
			{
				strLineType+="x";	
			}
			else if(strType=="RIGID Y" || strType=="RIGID +Y" || strType=="RIGID -Y")
			{
				strLineType+="y";
			}
			else if(strType=="RIGID Z" || strType=="RIGID +Z" || strType=="RIGID -Z")
			{
				strLineType+="z";
			}
			
			//角位移约束
			if(strType=="RIGID RX" || strType=="RIGID +RX" || strType=="RIGID -RX")
			{
				strRadType+="x";	
			}
			else if(strType=="RIGID RY" || strType=="RIGID +RY" || strType=="RIGID -RY")
			{
				strRadType+="y";
			}
			else if(strType=="RIGID RZ" || strType=="RIGID +RZ" || strType=="RIGID -RZ")
			{
				strRadType+="z";
			}
			else if(strType=="RIGID ANC")
			{
				strLineType = _T("xyz");
				strRadType+="xyz";
			} 
			else if ( strType == _T("GUIDE") )
			{
				iType = 260; //导向
				break;
			}
			pRs->MoveNext();
		}
		if( iType == 0)
		{//判断多方向约束的支吊架类型(start)
			//先确定约束类型十位上的数子，它由线位移确定
			//pfg20050630
			strLineType.TrimLeft();
			strLineType.TrimRight();
			if(!strLineType.IsEmpty())
			{
				//pfg20050630
				if(strLineType.Find("x")!=-1 && strLineType.Find("y")!=-1 && strLineType.Find("z")!=-1)
				{
					iDECType=7;
				}
				else if(strLineType.Find("y")!=-1 && strLineType.Find("z")!=-1 )
				{
					iDECType=6;
				}
				else if(strLineType.Find("x")!=-1 && strLineType.Find("z")!=-1 )
				{
					iDECType=5;
				}
				else if(strLineType.Find("x")!=-1 && strLineType.Find("y")!=-1 )
				{
					iDECType=4;
				}
				else if(strLineType.Find("z")!=-1 )
				{
					iDECType=3;
				}
				else if(strLineType.Find("y")!=-1 )
				{
					iDECType=2;
				}
				else if(strLineType.Find("x")!=-1 )
				{
					iDECType=1;
				}
			}
			//再确定约束类型个位上的数子，它由角位移确定
			//pfg20050630
			strRadType.TrimLeft();
			strRadType.TrimRight();
			if(!strRadType.IsEmpty())
			{
				//pfg20050630
				if(strRadType.Find("x")!=-1 && strRadType.Find("y")!=-1 && strRadType.Find("z")!=-1)
				{
					iNUMType=7;
				}
				else if(strRadType.Find("y")!=-1 && strRadType.Find("z")!=-1 )
				{
					iNUMType=6;
				}
				else if(strRadType.Find("x")!=-1 && strRadType.Find("z")!=-1 )
				{
					iNUMType=5;
				}
				else if(strRadType.Find("x")!=-1 && strRadType.Find("y")!=-1 )
				{
					iNUMType=4;
				}
				else if(strRadType.Find("z")!=-1 )
				{
					iNUMType=3;
				}
				else if(strRadType.Find("y")!=-1 )
				{
					iNUMType=2;
				}
				else if(strRadType.Find("x")!=-1 )
				{
					iNUMType=1;
				}
			}
			//转换成AutoPSA/AutoPHS约束类型iType
			iType=100+10*iDECType+iNUMType;
		}

		if(iType!=0)
		{
			rsData->PutCollect("TYPE",_variant_t(iType));
			rsData->PutCollect("psaTYPE",_variant_t(iType));
		}
	}
	catch(_com_error e)
	{
		CString strErrorMsg;
		strErrorMsg.Format(_T("%s: %d, %s"), __FILE__, __LINE__, e.Description());
		AfxMessageBox(strErrorMsg);
	}
}

//单位换算
//SourceUnits 被转换的单位
//UeUnits 转换后的单位
//返回单位换算因子
//dOffset 偏移值
double CAESARIIToPHS::UnitsToUeUnits(_RecordsetPtr rsData,CString SourceUnits,CString UeUnits,double &dOffset)
{
	double x;//单位换算因子
	CString strSQL;                                            
	_RecordsetPtr pRs;
	_ConnectionPtr pCon;
	pCon.CreateInstance(__uuidof(Connection));
	pRs.CreateInstance(__uuidof(Recordset));
	strSQL="select * from UnitsToUeUnits ";//删除了pfg20050629where Units='"+SourceUnits+"' and UeUnits='"+UeUnits+"'
	try
	{
		pCon=rsData->GetActiveConnection();
		pRs->Open(_variant_t(strSQL),(IDispatch*)pCon,adOpenStatic,adLockOptimistic,adCmdText);
		pRs->Filter=_variant_t("Units='"+SourceUnits+"' and UeUnits='"+UeUnits+"'");//pfg20050629
		if(pRs->GetRecordCount()>0)
		{
			x=vtof(pRs->GetCollect(_T("value")));
			dOffset=vtof(pRs->GetCollect(_T("offset")));//pfg20050701
			return x;
		}
		else if(pRs->GetRecordCount()==0)
		{
			//pfg20050629弹出增加单位转换关系对话框(statr)
			CUnitsTransformDlg dlg;
			pRs->Filter="";
			dlg.m_pRsUnits=pRs;
			dlg.m_strDestinationUnits=UeUnits;
			dlg.m_strSourceUnits=SourceUnits;
			if((dlg.DoModal())==IDOK)
			{
				pRs=dlg.m_pRsUnits;//pfg20050630
				pRs->Filter=_variant_t("Units='"+SourceUnits+"' and UeUnits='"+UeUnits+"'");//pfg20050629
				if(pRs->GetRecordCount()>0)
				{
					dOffset=vtof(pRs->GetCollect("Offset"));//pfg20050701
					x=vtof(pRs->GetCollect(_T("value")));
					return x;
				}
				else
					return 1.0;
			}
			else
			{
				m_IsCaesarError=TRUE;//增加了一个条件pfg20050627				
			}
			//pfg20050629弹出增加单位转换关系对话框(end)
		}
	}
	catch(_com_error e)
	{
		if(AfxMessageBox("错误:当前单位不能转换,请更换单位后重试(仔细阅读使用说明)!",MB_YESNO)==IDYES)
		{
			m_IsCaesarError=TRUE;//增加了一个条件pfg20050627
		}
	}
	return 1.0;
}

BOOL CAESARIIToPHS::IsHanger( int iNode,const CString& strFileName ) //是否是弹簧
{
	_RecordsetPtr pHangerRs;
	pHangerRs.CreateInstance( __uuidof(Recordset) );

	CString strSQL;
	strSQL.Format(_T("SELECT * FROM OUTPUT_HANGERS WHERE [NODE] = %d AND [JOBNAME] = '%s'"),iNode,strFileName);
	pHangerRs->Open(_variant_t(strSQL),(IDispatch *)m_pConnSourceDB,adOpenDynamic,adLockOptimistic,adCmdText);
	if(pHangerRs->adoEOF && pHangerRs->BOF)
	{
		pHangerRs->Close();
		return FALSE;
	}
	pHangerRs->Close();
	return TRUE;
}

double CAESARIIToPHS::GetMaxLoad( int iNode,const CString& strFileName ) //得到支吊点最大的荷载值
{
	double dRtMaxLoad = 0.0;
	_RecordsetPtr pMaxLoadRs;
	pMaxLoadRs.CreateInstance( __uuidof(Recordset) );
	//导入工作荷载(start)
	CString strSQL;
	strSQL.Format(_T("SELECT DISTINCT [NODE] ,max([RESULTANTF]) AS PJG FROM [OUTPUT_RESTRAINTS] WHERE [NODE] = %d AND [JOBNAME] = '%s' GROUP BY [NODE] "),iNode,strFileName);
	pMaxLoadRs->Open(_variant_t(strSQL),(IDispatch *)m_pConnSourceDB,adOpenDynamic,adLockOptimistic,adCmdText);
	//prsOutputRestraints = gpconCaesar->Execute(_bstr_t(strSQL),NULL,adCmdText);
	if(pMaxLoadRs->adoEOF && pMaxLoadRs->BOF)
	{
	}
	else
	{	
		dRtMaxLoad = vtof( pMaxLoadRs->GetCollect("PJG") );
	}
	if(pMaxLoadRs->State==adStateOpen)
	{
		pMaxLoadRs->Close();
	}
	return dRtMaxLoad;
}

//得到弹簧的工作荷载和安装荷载
//没有经过单位转换的荷载值
BOOL CAESARIIToPHS::GetHangerLoad( int iNode,const CString& strFileName, double& dWorkLoad,double& dInsLoad ) //得到弹簧的工作荷载和安装荷载
{
	_RecordsetPtr pHangerRs;
	pHangerRs.CreateInstance( __uuidof(Recordset) );
	
	try
	{
		CString strSQL;
		strSQL.Format(_T("SELECT * FROM OUTPUT_HANGERS WHERE [NODE] = %d AND [JOBNAME] = '%s'"),iNode,strFileName);
		pHangerRs->Open(_variant_t(strSQL),(IDispatch *)m_pConnSourceDB,adOpenDynamic,adLockOptimistic,adCmdText);
		if(pHangerRs->adoEOF && pHangerRs->BOF)
		{
			pHangerRs->Close();
			return FALSE;
		}
		else
		{
			dWorkLoad = vtoi ( pHangerRs->GetCollect( _T("NUMREQ") ) ) * vtof( pHangerRs->GetCollect( _T("HOT_LOAD") ) ); //工作荷载
			dInsLoad = vtoi ( pHangerRs->GetCollect( _T("NUMREQ") ) ) * vtof( pHangerRs->GetCollect( _T("TH_INSTALL_LOAD") ) ); //安装荷载
		}
	}
	catch ( _com_error e )
	{
		CString strErrorMsg;
		strErrorMsg.Format(_T("%s: %d, %s"), __FILE__, __LINE__, e.Description());
		AfxMessageBox(strErrorMsg);
	}
	if(pHangerRs->State==adStateOpen)
	{
		pHangerRs->Close();
	}
	return TRUE;
}

//根据弹簧类型得到弹簧串联数,类型规则参考Spring.mdb数据库
BOOL CAESARIIToPHS::GetHangerSeriesNum(const CString& strHangerType,int& iRtSeriesNum ) 
{
	
	int iLen = strHangerType.GetLength();
	if ( strHangerType.GetLength() < 2 )
		return FALSE;
	CString strSubType = strHangerType.Left(2);
	strSubType.MakeUpper();
	if ( strSubType == _T("ZH") ) //火电厂汽水管道支吊架设计手册弹簧D-ZD83 对应CAESARII中 21索引号 格式("ZH" & SeriesNum & Format(SPRSpecNo,"00"))
	{
		if ( iLen < 3 )
			return FALSE;
		iRtSeriesNum = atoi( strHangerType.Mid( 2,1 ) ) ;
		return TRUE;
	}
	else if ( strSubType == _T("TH") ) //华东电力设计院汽水管道支吊架设计手册弹簧D-ZD83 对应CAESARII中 21索引号 ("TH1-A" & SeriesNum & Format(SPRSpecNo,"00"))
	{
		if ( iLen < 5 )
			return FALSE;
		iRtSeriesNum = atoi( strHangerType.Mid( 5,1 ) ) ;
		return TRUE;
	}
	else if ( strSubType == _T("VS") || strSubType == _T("TD") )  //江阴石化设备厂弹簧厂,大连弹簧厂弹簧,国家标准/机械行业标准弹簧GB10182-88
	{
		//"VS" & Format(SeriesNum*30,"00") & "D" & Format(SprNo,"00"),"TD" & (SeriesNum*30) & "D" & Format(SPRSpecNo,"00"),"TD" & (SeriesNum*30) & "D" & Format(SPRSpecNo,"00")
		int iTmpDisp = 0;
		int iEndIndex = strHangerType.Find( _T("D"),2 );
		if ( iEndIndex == -1 )
		{
			iTmpDisp = atoi( strHangerType.Mid( 2,iLen-2 ) );
		}
		else
		{
			iTmpDisp = atoi ( strHangerType.Mid( 2,iEndIndex-2 ) );
		}
		iRtSeriesNum = iTmpDisp / 30;
		return TRUE;
	}
	
	return FALSE;
}


