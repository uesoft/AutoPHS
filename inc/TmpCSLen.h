// TmpCSLen.h: interface for the TmpCSLen class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TMPCSLEN_H__E1F6D90F_0A92_43F3_BDB0_B6B6CF719E43__INCLUDED_)
#define AFX_TMPCSLEN_H__E1F6D90F_0A92_43F3_BDB0_B6B6CF719E43__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ConstantSpringDataBase.h"

class TmpCSLen : public ConstantSpringDataBase
{
public:
	TmpCSLen();
	virtual ~TmpCSLen();
	void SetAllBH(const CString& strBH);
	void SetAllLugDiameter(int iDia);
	void SetAllSumDist(double dbValue);
	void SetAllSizeC(double dbValue);
	void SetAllHeatDist(double dbValue);
	void SetAllUnitForce(const CString& strValue);
	void SetAllDistFX(const CString& strValue);
	void SetAllSelOpLoad(double dbValue);
	void SetAllOpLoad(double dbValue);
	void SetAllInstallLoad(double dbValue);
public:
	const CString& GetTableName()
	{
		return m_strTableName;
	}
	const CString& GetBHField()
	{
		return m_strBHField;
	}
	const CString& GetLugDiameterField()
	{
		return m_strLugDiameterField;
	}
	const CString& GetSumDistField()
	{
		return m_strSumDistField;
	}
	const CString& GetSizeCFieldField()
	{
		return m_strSizeCField;
	}
	const CString& GetHeatDistField()
	{
		return m_strHeatDistField;
	}
	const CString& GetUnitForceField()
	{
		return m_strUnitForceField;
	}
	const CString& GetDistFXField()
	{
		return m_strDistFXField;
	}
	const CString& GetSelOpLoadField()
	{
		return m_strSelOpLoadField;
	}
	const CString& GetOpLoadField()
	{
		return m_strOpLoadField;
	}
	const CString& GetInstallLoadField()
	{
		return m_strInstallLoadField;
	}

private:
	static CString m_strTableName; 
	static CString m_strBHField; //BH×Ö¶ÎÃû
	static CString m_strLugDiameterField; //
	static CString m_strSumDistField;
	static CString m_strSizeCField;
	static CString m_strHeatDistField;
	static CString m_strUnitForceField;
	static CString m_strDistFXField;
	static CString m_strSelOpLoadField;
	static CString m_strOpLoadField;
	static CString m_strInstallLoadField;
};

#endif // !defined(AFX_TMPCSLEN_H__E1F6D90F_0A92_43F3_BDB0_B6B6CF719E43__INCLUDED_)
