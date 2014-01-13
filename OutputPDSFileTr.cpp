// OutputPDSFileTr.cpp: implementation of the OutputPDSFileTr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OutputPDSFileTr.h"
#include "user.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

OutputPDSFileTr::OutputPDSFileTr()
{
	m_pRs.CreateInstance( __uuidof(Recordset) );
}

OutputPDSFileTr::~OutputPDSFileTr()
{
	if ( m_pRs->GetState() == adStateOpen )
		m_pRs->Close();
}

//iVolumeID:卷册号; mPHSNumberArray:输出的所有支吊架号  mGNumArray:输出所有支吊架的根部数量
//strFileName:第一个支吊架的输出路径及名字
void OutputPDSFileTr::OutPutText( int iVolumeID,CArray<int,int>& mPHSNumberArray,CArray<int,int>& mGNumArray,const CString& strFileName )
{
	//文件路径
	CString strFilePath,strError;
	int iIndex = strFileName.ReverseFind( _T('\\') );
	if ( iIndex == -1 )
		return;
	strFilePath = strFileName.Left( iIndex+1 );

	_RecordsetPtr pRs,pOutCfgRs,pDataRs; //记录集 用来查询表的记录
	pRs.CreateInstance( __uuidof(Recordset) );//记录集的初始化
	pOutCfgRs.CreateInstance( __uuidof(Recordset) );//
	pDataRs.CreateInstance( __uuidof(Recordset) );

	CString strFieldName,strOutName,strTableName,strMDBName;
	CString strPreTableNane,strPreMdbName;
	
	CString strSQL;
	int iSize = mPHSNumberArray.GetSize();//数组的个数
	int iPHSNumber,iGNum;
	CString strCustomID,strBH,strOutValue,strStrand;
	try
	{
		//多个支吊架
		for ( int i=0; i<iSize; i++ )
		{
			iPHSNumber = mPHSNumberArray.GetAt(i);
			//文件名
			CStdioFile stdFile;
			CString strPHSFileName;
			if ( i== 0 )
				strPHSFileName = strFileName;
			else
			{
				strPHSFileName.Format( _T("%d"),iPHSNumber );
				strPHSFileName = strFilePath + strPHSFileName + _T(".txt");
			}
			BOOL bRt = stdFile.Open( strPHSFileName, CStdioFile::modeCreate|CStdioFile::modeWrite );
			if ( !bRt )
			{
				strError.Format("打开文件%s失败",strPHSFileName);
				AfxMessageBox( strError );
				continue;
			}

			
			iGNum = mGNumArray.GetAt(i);
			strSQL.Format( _T("SELECT * FROM ZB where VolumeID=%d AND ZDJH = %d AND IsSAPart = FALSE ORDER BY recno "),iVolumeID, iPHSNumber );
			//打开记录集 第一个参数：sql语句 第二个参数：数据库连接 
			pRs->Open( _variant_t(strSQL),(IDispatch*)::conPRJDB ,adOpenKeyset,adLockOptimistic,adCmdText  );
			
			//得到ID号
			CString strID;
			int iParalleledNum = 0;
			if ( !pRs->adoEOF )
			{
				strID = vtos( pRs->GetCollect( _variant_t("ID") ) );
				strCustomID = vtos( pRs->GetCollect( _variant_t("CustomID") ) );
				if ( strID == strCustomID )
					strStrand = _T("-XB");
				else
				    strStrand = _T("-HD");
				iParalleledNum = GetParalleledNum( strID );
				/*文件分开，不需要隔开了
				if ( i != 0 )
					stdFile.WriteString(_T("\n"));
					*/

				//得到支吊类型，是支吊还是吊架,管部、根部标高
				CString strType;
				double dPB=0.0, dRB = 0.0,dDW = 0.0;
				GetZDJInfo( iVolumeID,iPHSNumber,strType,dPB,dRB,dDW );
				if ( iParalleledNum == 2 )
					strOutValue.Format( _T("ZDJINFO%s,%d,SLG%s(1)PB=%g(2)RB=%g(3)DW=%g\n"),strStrand,iPHSNumber,strType,dPB,dRB,dDW );
				else
					strOutValue.Format( _T("ZDJINFO%s,%d,DLG%s(1)PB=%g(2)RB=%g(3)DW=%g\n"),strStrand,iPHSNumber,strType,dPB,dRB,dDW );
				stdFile.WriteString(strOutValue);
			}


			CString strParllText; //关联的支吊架另外的文本
			strOutValue.Empty();
			BOOL bFirst = TRUE;
			//开始输出要输出的每个零件
			while ( !pRs->adoEOF )
			{
				strCustomID = vtos( pRs->GetCollect( _variant_t("CustomID") ) );
				strBH = vtos( pRs->GetCollect( _variant_t("BH") ) );
				strOutValue = GetCustomIDOutText( iVolumeID,iPHSNumber,strBH,strCustomID,pRs );
				
				pRs->MoveNext();

				if ( iParalleledNum == 2) //双杆且是管部
				{
					if ( bFirst ) //管部
					{
						strOutValue = _T("ZDJELE") + strOutValue;
					}
					else ////如果是双杆,记录好文本,管部不需要
					{
						if ( pRs->adoEOF )
						{
							if ( iGNum == 2 ) //根部数量
							{
								strParllText = strParllText + _T("ZDJELER") + strOutValue;
							}
						}
						else
						{
							strParllText = strParllText +  _T("ZDJELER") + strOutValue;
						}
						strOutValue = _T("ZDJELEL") + strOutValue;
					}	
				}
				else //单杆支吊架
				{
					strOutValue = _T("ZDJELEL") + strOutValue;
				}
				stdFile.WriteString(strOutValue);
				bFirst = FALSE;

			}
			pRs->Close();
			if ( iParalleledNum == 2 ) //双杆
			{
				stdFile.WriteString(strParllText);
			}
			stdFile.Close();

		}
	}
	catch (_com_error e)
	{
		AfxMessageBox( e.Description() );
	}
	if ( pRs->GetState() == adStateOpen )
		pRs->Close();
	if ( pOutCfgRs->GetState() == adStateOpen )
		pOutCfgRs->Close();
	if ( pDataRs->GetState() == adStateOpen )
		pDataRs->Close();
	AfxMessageBox( _T("导出文件成功(以支吊架的编号为文件名)") );
	
}

//得到支吊架类型 返回值 strType 1:是支架 2:吊架  dPB:管部标高  dRB:根部标高
BOOL OutputPDSFileTr::GetZDJInfo( int iVolumeID,int iPHSNumber,CString& strType,double& dPB,double& dRB,double& dDW ) 
{
	BOOL bRtVlaue = FALSE;
	CString strSQL;
	_RecordsetPtr pRs; //记录集 用来查询表的记录
	pRs.CreateInstance( __uuidof(Recordset) );//记录集的初始化

	try
	{
		strSQL.Format( _T("SELECT * FROM ZA where VolumeID=%d AND ZDJH = %d"),iVolumeID, iPHSNumber );
		//打开记录集 第一个参数：sql语句 第二个参数：数据库连接 
		pRs->Open( _variant_t(strSQL),(IDispatch*)::conPRJDB ,adOpenKeyset,adLockOptimistic,adCmdText  );
		if ( !pRs->adoEOF )
		{
			dPB = vtof( pRs->GetCollect( _variant_t( _T("DH1") )) ); //管部
			dRB = vtof( pRs->GetCollect( _variant_t( _T("GH1") )) ); //根部
			dDW = vtof( pRs->GetCollect( _variant_t( _T("DJ1") )) ); //外径
			if ( dRB > dPB ) //吊架
				strType = _T("DJ");
			else
				strType = _T("ZJ");
			bRtVlaue = TRUE;
		}
	}
	catch (_com_error e)
	{
		AfxMessageBox( e.Description() );
	}
	if ( pRs->GetState() == adStateOpen )
		pRs->Close();
	return bRtVlaue;

}

int OutputPDSFileTr::GetParalleledNum( const CString& strID ) //得到支吊架的并联数 
{
	CString strSql;
	strSql.Format( _T("SELECT * FROM CrudePAidenCN Where ID = '%s'"),strID );
	_RecordsetPtr pRs; //记录集 用来查询表的记录
	pRs.CreateInstance( __uuidof(Recordset) );//记录集的初始化
	int iRt =0 ;
	try
	{
		pRs->Open( _variant_t(strSql),(IDispatch*)::conZDJCrude ,adOpenKeyset,adLockOptimistic,adCmdText );
		if ( !pRs->adoEOF )
		{
			iRt = vtoi( pRs->GetCollect( _variant_t("ParalleledNum") ) );
		}
	}
	catch (_com_error e)
	{
		AfxMessageBox( e.Description() );
	}
	if ( pRs->GetState() == adStateOpen )
		pRs->Close();
	return iRt;
}

//得到传入部件的输出文本
//iPHSNumber:支中架号,  strBH:零部件的标号值  strCustomID:标部件的代号
//pZBRs: 当前部件的连接记录,因为有些值需要从ZB表当前部件记录中取
CString OutputPDSFileTr::GetCustomIDOutText( int iVolumeID,int iPHSNumber,const CString& strBH, const CString& strCustomID,_RecordsetPtr pZBRs ) 
{
	CString strSQL;
	_RecordsetPtr pOutCfgRs;
	pOutCfgRs.CreateInstance( __uuidof(Recordset) );
	///	strSQL.Format(_T("select BH,SIZE2 from ZB inner join crudePA on ZB.CustomID=crudePA.CustomID"););
	strSQL.Format( _T("SELECT * FROM OutPutPDSCfg WHERE CustomID = '%s' ORDER BY OutSEQ"),strCustomID );
	pOutCfgRs->Open( _variant_t(strSQL),(IDispatch*)::conPHSConfig ,adOpenKeyset,adLockOptimistic,adCmdText );
	int iIndex = 0;
	CString strText,strTmp;
	strText.Format( _T(",%d"),iPHSNumber );
				
	CString strFieldName,strOutName,strTableName,strMDBName,strOutValue;
	CString strValue1,strValue2,strMdb1,strMdb2,strTableName1,strTableName2,strFieldName1,strFieldName2;
	CString strMidKey1,strMidKey2;
	int iMid = 0;
	if ( pOutCfgRs->adoEOF )
	{
		CString strError;
		strError.Format( _T("OutPutPDScfg表中没有配置%s的输出信息，请检查"),strCustomID );
		AfxMessageBox( strError );
	}
	while( !pOutCfgRs->adoEOF )
	{
		iIndex ++;
		strFieldName = vtos( pOutCfgRs->GetCollect( _variant_t("FieldName") ) );
		strOutName = vtos( pOutCfgRs->GetCollect( _variant_t("OutName") ) );
		strTableName = vtos( pOutCfgRs->GetCollect( _variant_t("FieldTableName") ) );
		strMDBName = vtos( pOutCfgRs->GetCollect( _variant_t("FieldMDBName") ) );
					
		//处理不同表中求值
		if ( GetStrAtCountEx( strMDBName ) > 1 )
		{
			strTableName1 = GetStrAtEx(strTableName,1);
			strFieldName1 = GetStrAtEx(strFieldName,1);
			strMdb1 = GetStrAtEx(strMDBName,1);
			strValue1 = GetValue( iVolumeID,iPHSNumber,strBH,strTableName1,strFieldName1,strMdb1,pZBRs );
						
			strTableName2 = GetStrAtEx(strTableName,2);
			strFieldName2 = GetStrAtEx(strFieldName,2);
			strMdb2 = GetStrAtEx(strMDBName,2);
			strValue2 = GetValue( iVolumeID,iPHSNumber,strBH,strTableName2,strFieldName2,strMdb2,pZBRs);
						
			strOutValue.Format( _T("%g"),atof(strValue1) - atof(strValue2) );
		}
		else
		{
			//处理从值中取子字符值
			if ( strFieldName.Find(_T("输出型号")) != -1 )
			{
				strOutValue = strCustomID;
			}
			else if ( strFieldName.Find(_T("单字符分析")) != -1 )
			{
				strFieldName1 = GetStrAtEx( strFieldName,1 );
				strOutValue = GetValue( iVolumeID,iPHSNumber,strBH,strTableName,strFieldName1,strMDBName,pZBRs );
				strMidKey1 =  GetStrAtEx( strFieldName,3 ); //第一个分隔字符
				strMidKey2 =  GetStrAtEx( strFieldName,4 ); //第二个分隔字符
				iMid = atoi( strMidKey2 );
				strOutValue.Replace( strMidKey1,_T(",") );
				strOutValue = GetStrAtEx( strOutValue,iMid ); //取第几个字符值
			}
			else if ( strFieldName.Find(_T("双字符分析")) != -1 )
			{
				strFieldName1 = GetStrAtEx( strFieldName,1 );
				strOutValue = GetValue( iVolumeID,iPHSNumber,strBH,strTableName,strFieldName1,strMDBName,pZBRs );
				strMidKey1 =  GetStrAtEx( strFieldName,3 ); //第一个分隔字符
				strMidKey2 =  GetStrAtEx( strFieldName,4 ); //第二个分隔字符
				strOutValue.Replace( strMidKey1,_T(",") );
				if( !strMidKey2.IsEmpty() )
					strOutValue.Replace( strMidKey2,_T(",") );
				strOutValue = GetStrAtEx( strOutValue,2 );
			}
			else
				strOutValue = GetValue( iVolumeID,iPHSNumber,strBH,strTableName,strFieldName,strMDBName,pZBRs );
		}
		
		if ( strOutName == _T("bh") ) //标号
		{
			strOutValue.TrimLeft();
			strOutValue.TrimRight();
			strOutValue.Replace( _T(" "),_T("-") );
			strOutValue.Replace( _T("型"),_T("") );
		}
		strTmp.Format( _T("(%d)%s=%s"),iIndex,strOutName,strOutValue );
		strText = strText + strTmp;
					
		pOutCfgRs->MoveNext();
	}
	pOutCfgRs->Close();

	if ( !strText.IsEmpty() )
		strText = strText + _T("\n");
	return strText;
	

}


/*
CString OutputPDSFileTr::GetStrandStr( const CString& strCustomID) //得到数据库标准字符
{
	_RecordsetPtr pOutCfgRs;
	pOutCfgRs.CreateInstance( __uuidof(Recordset) );
	strSQL.Format( _T("SELECT * FROM OutPutPDSCfg WHERE CustomID = '%s' ORDER BY OutSEQ"),strCustomID );
	try
	{
		pOutCfgRs->Open( _variant_t(strSQL),(IDispatch*)::conPHSConfig ,adOpenKeyset,adLockOptimistic,adCmdText );
	}
	catch( _com_error e )
	{
		AfxMessageBox( e.Description() );
	}
	if ( pOutCfgRs->GetState() == adStateOpen )
		pOutCfgRs->Close();


}
*/


//得到传入零部件号传入表传入字段的值
CString OutputPDSFileTr::GetValue( int iVolumeID,int iPHSNumber, const CString& strBH,const CString& strTableName,const CString& strFieldName,const CString& strmdbName,_RecordsetPtr pZBRs )
{
	//重新连接数据库
	CString strRt;
	if ( strmdbName == _T("AllPrjDB.mdb") && strTableName == _T("ZB"))
	{
		strRt = vtos( pZBRs->GetCollect( _variant_t( strFieldName ) ) ); //ZB表中的记录,现在配置文件中只有ZB表
		return strRt;
	}
	else if ( strmdbName == _T("AllPrjDB.mdb") && strTableName == _T("ZA") )
	{
		if ( m_pRs->GetState() == adStateOpen )
			m_pRs->Close();
		CString strSql;
		strSql.Format( _T("SELECT * FROM %s WHERE VolumeID = %d AND ZDJH = %d"),strTableName,iVolumeID,iPHSNumber );
		m_pRs->Open( _variant_t(strSql),(IDispatch*)::conPRJDB ,adOpenKeyset,adLockOptimistic,adCmdText );

		m_strBH = strBH;
		m_strTableName = strTableName;
		m_strFieldName = strFieldName;
		m_strMdbName = strmdbName;
	}
	else if ( strBH != m_strBH || strTableName != m_strTableName || strFieldName != m_strFieldName || strmdbName != m_strMdbName )
	{
		if ( m_pRs->GetState() == adStateOpen )
			m_pRs->Close();
		CString strSql;
		strSql.Format( _T("SELECT * FROM %s WHERE BH = '%s'"),strTableName,strBH );
		m_pRs->Open( _variant_t(strSql),(IDispatch*)::conZDJCrude ,adOpenKeyset,adLockOptimistic,adCmdText );
		
		m_strBH = strBH;
		m_strTableName = strTableName;
		m_strFieldName = strFieldName;
		m_strMdbName = strmdbName;
	}
	
	if ( !m_pRs->adoEOF )
		strRt = vtos( m_pRs->GetCollect( _variant_t( strFieldName ) ) );
	else
	{
		CString strError;
		strError.Format( _T("在表%s中没有找表BH=%s的记录，请查看配置是否正确"),strTableName,strBH );
		AfxMessageBox( strError );
	}

	return strRt;
}



