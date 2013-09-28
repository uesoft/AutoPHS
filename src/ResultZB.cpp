// ResultZB.cpp: implementation of the ResultZB class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ResultZB.h"
#include "EDIBgbl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CString ResultZB::m_strCustomIDField = _T("VolumeID"); //字段名
CString ResultZB::m_strZdjhField = _T("ZDJH"); //
CString ResultZB::m_strNthField = _T("nth");
CString ResultZB::m_strRecnoField = _T("recno");
CString ResultZB::m_strPreviousPartInsertLenField = _T("PreviousPartInsertLen");
CString ResultZB::m_strBHformatField = _T("BHformat");
CString ResultZB::m_strCLggField = _T("CLgg");
CString ResultZB::m_strVolumeIDField = _T("VolumeID"); //卷册索引号
CString ResultZB::m_strClassIDField = _T("ClassID"); //部件类别号
CString ResultZB::m_strPjsField = _T("Pjs"); //选择部件的计算荷载
CString ResultZB::m_strFxField = _T("fx"); //部件图形块旋转方向
CString ResultZB::m_strRotationField = _T("Rotation"); //部件图形块旋转角度
CString ResultZB::m_strSizeDimField = _T("SizeDim"); //部件尺寸标注长度
CString ResultZB::m_strSizeHField = _T("SizeH"); //部件的实际高度或长度
CString ResultZB::m_strCLnumField = _T("CLnum"); //部件数量
CString ResultZB::m_strSEQField = _T("SEQ"); //部件编号
CString ResultZB::m_strCLmcField = _T("CLmc"); //部件名称
CString ResultZB::m_strWeightField = _T("Weight"); //部件重量
CString ResultZB::m_strCldzField = _T("CLdz"); //部件单重
CString ResultZB::m_strClzzField = _T("CLzz"); //部件总重
CString ResultZB::m_strIndexField = _T("Index"); //部件类别索引号
CString ResultZB::m_strIDField = _T("ID"); //部件内部标志
CString ResultZB::m_strCLclField = _T("CLcl"); //部件材料名称
CString ResultZB::m_strTField = _T("T");
CString ResultZB::m_strAllowedMinLugExtField = _T("AllowedMinLugExt"); //连接件两端的最小必需插入长度
CString ResultZB::m_strAllowedMaxLugExtField = _T("AllowedMaxLugExt"); //连接件两端的最大必需插入长度
CString ResultZB::m_strClbzField = _T("CLbz"); //部件备注
CString ResultZB::m_strGDW1 = _T("GDW1"); //根部定位值
CString ResultZB::m_strSpringNo = _T("SpringNo"); //弹簧号
CString ResultZB::m_strLugDia = _T("LugDia"); //拉杆直径
CString ResultZB::m_strHeatDisp = _T("HeatDisp"); //弹簧或恒力弹簧组件的热位移
ResultZB::ResultZB()
{
	m_pRs.m_pDatabase=&EDIBgbl::dbPRJDB;
}

ResultZB::~ResultZB()
{
	
}

void ResultZB::SetNth(int iNth)
{
	m_pRs.Edit();
	m_pRs.SetFieldValue( m_strNthField, COleVariant((short)iNth) );
}

void ResultZB::SetPreviousPartInsertLen(double dbValue)
{
	m_pRs.Edit();
	m_pRs.SetFieldValue( m_strPreviousPartInsertLenField, COleVariant(dbValue) );
}

void ResultZB::SetBHformat( const CString& strValue )
{
	m_pRs.Edit();
	m_pRs.SetFieldValue( m_strBHformatField, COleVariant(strValue) );
}

void ResultZB::SetCLgg( const CString& strValue )
{
	m_pRs.Edit();
	m_pRs.SetFieldValue( m_strCLggField, COleVariant(strValue) );
}

void ResultZB::SetRecno(int iValue)
{
	m_pRs.Edit();
	m_pRs.SetFieldValue( m_strRecnoField, COleVariant((long)iValue) );
}
void ResultZB::SetVolumeID(int iValue)
{
	m_pRs.Edit();
	m_pRs.SetFieldValue( m_strVolumeIDField, COleVariant((long)iValue) );
}
void ResultZB::SetZdjh(int iValue) //支吊架号
{
	m_pRs.Edit();
	m_pRs.SetFieldValue( m_strZdjhField, COleVariant((long)iValue) );
}
void ResultZB::SetCustomID(CString strValue)//部件代号
{
	m_pRs.Edit();
	m_pRs.SetFieldValue( m_strCustomIDField, COleVariant(strValue) );
}
void ResultZB::SetClassID(int iValue) //部件类别号
{
	m_pRs.Edit();
	m_pRs.SetFieldValue( m_strClassIDField, COleVariant((long)iValue) );
}
void ResultZB::SetPjs(double dbValue) //选择部件时采用的计算荷载值，这个值可以累加了前面的重量 kgf
{
	m_pRs.Edit();
	m_pRs.SetFieldValue( m_strPjsField, COleVariant(dbValue) );
}
void ResultZB::SetFx(int iValue)//零件左视图第一路方向
{
	m_pRs.Edit();
	m_pRs.SetFieldValue( m_strFxField, COleVariant((long)iValue) );
}
void ResultZB::SetRotation(int iValue)//零件y坐标方向
{
	m_pRs.Edit();
	m_pRs.SetFieldValue( m_strRotationField, COleVariant((long)iValue) );
}
void ResultZB::SetSizeDim(int iValue)//得到部件的实际高度和长度
{
	m_pRs.Edit();
	m_pRs.SetFieldValue( m_strSizeDimField, COleVariant((long)iValue) );
}
void ResultZB::SetSizeH(double dbValue)//部件的实际高度或长度
{
	m_pRs.Edit();
	m_pRs.SetFieldValue( m_strSizeHField, COleVariant(dbValue) );
}
void ResultZB::SetCLnum(int iValue)
{
	m_pRs.Edit();
	m_pRs.SetFieldValue( m_strCLnumField, COleVariant((long)iValue) );
}
void ResultZB::SetSEQ(int iValue)
{
	m_pRs.Edit();
	m_pRs.SetFieldValue( m_strSEQField, COleVariant((long)iValue) );
}
void ResultZB::SetCLmc(CString strValue)
{
	m_pRs.Edit();
	m_pRs.SetFieldValue( m_strCLmcField, COleVariant(strValue) );
}
void ResultZB::SetWeight(double dbValue)
{
	m_pRs.Edit();
	m_pRs.SetFieldValue( m_strWeightField, COleVariant(dbValue) );
}
void ResultZB::SetCldz(double dbValue) //部件单重
{
	m_pRs.Edit();
	m_pRs.SetFieldValue( m_strCldzField, COleVariant(dbValue) );
}
void ResultZB::SetClzz(double dbValue) //部件总重
{
	m_pRs.Edit();
	m_pRs.SetFieldValue( m_strClzzField, COleVariant(dbValue) );
}
void ResultZB::SetIndex( int iValue)
{
	m_pRs.Edit();
	m_pRs.SetFieldValue( m_strIndexField, COleVariant((long)iValue) );
}
void ResultZB::SetID( CString strValue ) //部件内部标志
{
	m_pRs.Edit();
	m_pRs.SetFieldValue( m_strIDField, COleVariant(strValue) );
}
void ResultZB::SetCLcl(CString strValue)//部件材料名称
{
	m_pRs.Edit();
	m_pRs.SetFieldValue( m_strCLclField, COleVariant(strValue) );
}
void ResultZB::SetT(int iValue)
{
	m_pRs.Edit();
	m_pRs.SetFieldValue( m_strTField, COleVariant((long)iValue) );
}
void ResultZB::SetAllowedMinLugExt(double dbValue)
{
	m_pRs.Edit();
	m_pRs.SetFieldValue( m_strAllowedMinLugExtField, COleVariant(dbValue) );
}
void ResultZB::SetAllowedMaxLugExt(double dbValue)
{
	m_pRs.Edit();
	m_pRs.SetFieldValue( m_strAllowedMaxLugExtField, COleVariant(dbValue) );
}
void ResultZB::SetClbz(CString strValue)
{
	m_pRs.Edit();
	m_pRs.SetFieldValue( m_strClbzField, COleVariant(strValue) );
}
void ResultZB::SetGDW1(double dbValue)
{
	m_pRs.Edit();
	m_pRs.SetFieldValue( m_strGDW1, COleVariant(dbValue) );
}
void ResultZB::SetSpringNo(int iValue)
{
	m_pRs.Edit();
	m_pRs.SetFieldValue( m_strSpringNo, COleVariant((long)iValue) );
}
void ResultZB::SetLugDia(int iValue)
{
	m_pRs.Edit();
	m_pRs.SetFieldValue( m_strLugDia, COleVariant((long)iValue) );
}
void ResultZB::SetHeatDisp(double dbValue)
{
	m_pRs.Edit();
	m_pRs.SetFieldValue( m_strHeatDisp, COleVariant(dbValue) );
}