// OutputPDSFileTr.h: interface for the OutputPDSFileTr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OUTPUTPDSFILETR_H__9F0E9F0C_2D8B_49F1_B2D7_D5E9058B493B__INCLUDED_)
#define AFX_OUTPUTPDSFILETR_H__9F0E9F0C_2D8B_49F1_B2D7_D5E9058B493B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class OutputPDSFileTr  
{
public:
	OutputPDSFileTr();
	virtual ~OutputPDSFileTr();
	//输出支吊架的数据到PDS文本文件格式   mPHSNumberArray:要输出支吊架的编号
	void OutPutText(int iVolumeID, CArray<int,int>& mPHSNumberArray,CArray<int,int>& mGNumArray,const CString& strFileName );
private:
	int GetParalleledNum( const CString& strID ); //得到支吊架的并联数 
	//得到支吊架类型 返回值 strType 1:是支架 2:吊架  dPB:管部标高  dRB:根部标高
	BOOL GetZDJInfo( int iVolumeID,int iPHSNumber,CString& strType,double& dPB,double& dRB,double& dDW ); 
	CString GetCustomIDOutText( int iVolumeID,int iPHSNumber,const CString& strBH, const CString& strCustomID,_RecordsetPtr pZBRs ); //得到传入部件的输出文本
	CString GetValue( int iVolumeID,int iPHSNumber,const CString& strBH,const CString& strTableName,const CString& strFieldName,const CString& strmdbName,_RecordsetPtr pZBRs );

//	CString GetStrandStr( const CString& strCustomID); //得到数据库标准字符
	
	CString m_strBH;
	CString m_strTableName;
	CString m_strFieldName;
	CString m_strMdbName;
	_RecordsetPtr m_pRs;
};

#endif // !defined(AFX_OUTPUTPDSFILETR_H__9F0E9F0C_2D8B_49F1_B2D7_D5E9058B493B__INCLUDED_)
