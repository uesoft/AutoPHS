// Connect.h: interface for the Connect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONNECT_H__3E2A11AD_EAB4_44AF_9F1E_609D4CE556C3__INCLUDED_)
#define AFX_CONNECT_H__3E2A11AD_EAB4_44AF_9F1E_609D4CE556C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ConstantSpringDataBase.h"

class Connect : public ConstantSpringDataBase
{
public:
	Connect();
	virtual ~Connect();
	double GetCNTBextlenFLD(); //得到头部件伸入下一部件的长度
public:
	const CString& GetTableName()
	{
		return m_strTableName;
	}
	const CString& GetCNTBField()
	{
		return m_strCNTBField;
	}
	const CString& GetCNTEField()
	{
		return m_strCNTEField;
	}
	const CString& GetCNTBextlenFLDField()
	{
		return m_strCNTBextlenFLDField;
	}
private:
	static CString m_strTableName; //表名
	static CString m_strCNTBField; //CNTB头部件的字段名
	static CString m_strCNTEField; //CNTE尾部件的字段名
	static CString m_strCNTBextlenFLDField;//头部件伸入下一部件的长度字段名
};

#endif // !defined(AFX_CONNECT_H__3E2A11AD_EAB4_44AF_9F1E_609D4CE556C3__INCLUDED_)
