// ResultZB.h: interface for the ResultZB class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESULTZB_H__BF11B6F9_49C0_4F6B_94D7_561EFDD7BF64__INCLUDED_)
#define AFX_RESULTZB_H__BF11B6F9_49C0_4F6B_94D7_561EFDD7BF64__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ConstantSpringDataBase.h"

class ResultZB : public ConstantSpringDataBase
{
public:
	ResultZB();
	virtual ~ResultZB();
public:
	void SetNth( int iNth );
	void SetPreviousPartInsertLen( double dbValue );
	void SetBHformat( const CString& strValue );
	void SetCLgg( const CString& strValue );
	void SetRecno(int iValue);
	void SetVolumeID(int iValue);
	void SetZdjh(int iValue);
	void SetCustomID(CString strValue);
	void SetClassID(int iValue);
	void SetPjs(double dbValue); //选择部件时采用的计算荷载值，这个值可以累加了前面的重量 kgf
	void SetFx(int iValue);//零件左视图第一路方向
	void SetRotation(int iValue);//零件y坐标方向
	void SetSizeDim(int iValue);//得到部件的实际高度和长度
	void SetSizeH(double dbValue);//
	void SetCLnum(int iValue);
	void SetSEQ(int iValue);
	void SetCLmc(CString strValue);
	void SetWeight(double dbValue);
	void SetCldz(double dbValue);
	void SetClzz(double dbValue);
	void SetIndex( int iValue);
	void SetID( CString strValue );
	void SetCLcl(CString strValue);
	void SetT(int iValue);
	void SetAllowedMinLugExt(double dbValue);
	void SetAllowedMaxLugExt(double dbValue);
	void SetClbz(CString strValue);
	void SetGDW1(double dbValue);
	void SetSpringNo(int iValue);
	void SetLugDia(int iValue);
	void SetHeatDisp(double dbValue);
public:
	const CString& GetCustomIDField()
	{
		return m_strCustomIDField;
	}
	const CString& GetZdjhField()
	{
		return m_strZdjhField;
	}
	const CString& GetNthField()
	{
		return m_strNthField;
	}
	const CString& GetRecnoField()
	{
		return m_strRecnoField;
	}
private:
	static CString m_strNthField;
	static CString m_strRecnoField;
	static CString m_strPreviousPartInsertLenField; //字段名
	static CString m_strBHformatField;
	static CString m_strCLggField; //规格
	static CString m_strVolumeIDField; //卷册索引号
	static CString m_strZdjhField; //支吊架号
	static CString m_strCustomIDField;//部件代号
	static CString m_strClassIDField; //部件类别号
	static CString m_strPjsField; //选择部件的计算荷载
	static CString m_strFxField; //部件图形块旋转方向
	static CString m_strRotationField; //部件图形块旋转角度
	static CString m_strSizeDimField; //部件尺寸标注长度
	static CString m_strSizeHField; //部件的实际高度或长度
	static CString m_strCLnumField; //部件数量
	static CString m_strSEQField; //部件编号
	static CString m_strCLmcField; //部件名称
	static CString m_strWeightField; //部件重量
	static CString m_strCldzField; //部件单重
	static CString m_strClzzField; //部件总重
	static CString m_strIndexField; //部件类别索引号
	static CString m_strIDField; //部件内部标志
	static CString m_strCLclField; //部件材料名称
	static CString m_strTField;
	static CString m_strAllowedMinLugExtField; //连接件两端的最小必需插入长度
	static CString m_strAllowedMaxLugExtField; //连接件两端的最大必需插入长度
	static CString m_strClbzField; //部件备注
	static CString m_strGDW1; //根部定位值
	static CString m_strSpringNo; //弹簧号
	static CString m_strLugDia; //拉杆直径
	static CString m_strHeatDisp; //弹簧或恒力弹簧组件的热位移
};

#endif // !defined(AFX_RESULTZB_H__BF11B6F9_49C0_4F6B_94D7_561EFDD7BF64__INCLUDED_)
