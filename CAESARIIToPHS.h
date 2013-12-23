// CAESARIIToPHS.h: interface for the CAESARIIToPHS class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAESARIITOPHS_H__6113F5D2_DC33_41DC_8068_C064AD3D7E69__INCLUDED_)
#define AFX_CAESARIITOPHS_H__6113F5D2_DC33_41DC_8068_C064AD3D7E69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CAESARIIToPHS  
{
public:
	CAESARIIToPHS();
	virtual ~CAESARIIToPHS();
	void  ReadResult_CAESARII45(_Recordset* rsResult, CString SourceDataFileName,long maxZdjh,long FileNameID,char * strPSAVER);
private:
	void ImportHangerRestraints(_RecordsetPtr &rsData,_RecordsetPtr &rsHR,long &ltemp,long &FileNameID,char *strPSAVER,long &node,int flag);//pfg20050615
	void importUnitsForces(_RecordsetPtr &rsData,long node,CString m_strJOBNAME_P,CString PSA_OutDataFUnit,CString PSA_OutDataMUnit,CString m_strRGKname,CString m_strLGKname);//pfg20050622
	void ImportDisplacements(_RecordsetPtr rsData,long node, CString strLGKname,CString strJOBNAME_P,int flag);
	void ImportDiameter(_RecordsetPtr &rsData,long node,CString strJOBNAME_A);
	//导入弹簧数据
	void ImportHanger(_RecordsetPtr &rsData,long node,CString strJOBNAME_P);
	void ConversionTypeCaesarToPhs(_RecordsetPtr &rsData,long node,CString strJOBNAME_P,CString strGKname,long &iType);
	//单位换算
	double UnitsToUeUnits(_RecordsetPtr rsData,CString SourceUnits,CString UeUnits,double &dOffset);

	BOOL IsHanger( int iNode,const CString& strFileName  ); //是否是弹簧(变力，恒力)
	double GetMaxLoad( int iNode,const CString& strFileName ); //得到支吊点最大的荷载值
	BOOL GetHangerLoad( int iNode,const CString& strFileName, double& dWorkLoad,double& dInsLoad ); //得到弹簧的工作荷载和安装荷载

	BOOL GetHangerSeriesNum(const CString& strHangerType,int& iRtSeriesNum); //根据弹簧类型得到弹簧串联数

	_ConnectionPtr m_pConnSourceDB;			   //源数据库的连接对象
	BOOL m_IsCaesarError; 
	CString m_strUNITFORCE;//内部使用N单位
	CString m_strUNITMOMENTAL;//内部使用N.m单位
//	BOOL m_bCheckRigPHSMaxLoad; //刚性支吊架是否用最大荷载值工况提取荷载
	int m_iSpringLoadType; //弹簧荷载提取工况方式(0:按上述选择的冷/热态提取荷载值 1按CAESARII弹簧表中的荷载值提取)
	int m_iUPxyz ; //向上轴(1:X 2:Y 3:Z)
};

#endif // !defined(AFX_CAESARIITOPHS_H__6113F5D2_DC33_41DC_8068_C064AD3D7E69__INCLUDED_)
