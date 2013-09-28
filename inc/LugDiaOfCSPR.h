// LugDiaOfCSPR.h: interface for the LugDiaOfCSPR class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LUGDIAOFCSPR_H__EA04D393_C27B_4500_B4D4_E726872AA332__INCLUDED_)
#define AFX_LUGDIAOFCSPR_H__EA04D393_C27B_4500_B4D4_E726872AA332__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ConstantSpringDataBase.h"

class LugDiaOfCSPR : public ConstantSpringDataBase
{
public:
	LugDiaOfCSPR();
	virtual ~LugDiaOfCSPR();
	double GetT();
	int GetFiJ(); //得以恒力弹簧松紧螺母直径
	int GetFiK(); //得到弹簧吊板孔直径
public:
	const CString& GetCustomIDField()
	{
		return m_strCustomIDField;
	}
	const CString& GetPmaxKgfField()
	{
		return m_strPmaxKgfField;
	}
	const CString& GetFiJField() 
	{
		return m_strFiJField;
	}
	const CString& GetminDHField()
	{
		return m_strminDHField;
	}
	const CString& GetmaxDHField()
	{
		return m_strmaxDHField;
	}
	const CString& GetPmaxField()
	{
		return m_strPmaxField;
	}

private:
	static CString m_strCustomIDField; //恒力弹簧型式字段名
	static CString m_strPmaxKgfField; //恒力弹簧工作荷载字段名
	static CString m_strFiJField; //恒力弹簧松紧螺母直径字段名
	static CString m_strminDHField; //恒力弹簧最小规格号字段名
	static CString m_strmaxDHField; //恒力弹簧最大规格号字段名
	static CString m_strPmaxField;  //恒力弹簧工作荷(N)字段名
	static CString m_strFiKField; //恒力弹簧的吊板孔直径
	static CString m_strTField; 
};

#endif // !defined(AFX_LUGDIAOFCSPR_H__EA04D393_C27B_4500_B4D4_E726872AA332__INCLUDED_)
