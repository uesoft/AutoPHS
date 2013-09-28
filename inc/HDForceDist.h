// HDForceDist.h: interface for the HDForceDist class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HDFORCEDIST_H__18A134E0_04C9_4945_A546_3E11BD9C2E5F__INCLUDED_)
#define AFX_HDFORCEDIST_H__18A134E0_04C9_4945_A546_3E11BD9C2E5F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ConstantSpringDataBase.h"

class HDForceDist : public ConstantSpringDataBase 
{
public:
	HDForceDist();
	virtual ~HDForceDist();
public:
	int GetDH(); //弹簧编号
	double GetMinCapacity(); //得到最小的容量值
	double GetMaxCapacity(); //得到最大的容量值
	double GetMinDist(); //当前记录的最小位移
	double GetMaxDist(); //当前记录的最大位移
	double GetCapacity(); //得到容量值
public:
	const CString& GetCapacityField()
	{
		return m_strCapacityField;
	}
private:
	static CString m_strCapacityField;
	static CString m_strMinDistField;
	static CString m_strMaxDistField;
	static CString m_strDHField;
};

#endif // !defined(AFX_HDFORCEDIST_H__18A134E0_04C9_4945_A546_3E11BD9C2E5F__INCLUDED_)
