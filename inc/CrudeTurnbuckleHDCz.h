// CrudeTurnbuckleHDCz.h: interface for the CrudeTurnbuckleHDCz class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRUDETURNBUCKLEHDCZ_H__3560ECB4_5425_47C9_BA67_CCBB95A4B909__INCLUDED_)
#define AFX_CRUDETURNBUCKLEHDCZ_H__3560ECB4_5425_47C9_BA67_CCBB95A4B909__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ConstantSpringDataBase.h"

class CrudeTurnbuckleHDCz : public ConstantSpringDataBase
{
public:
	CrudeTurnbuckleHDCz();
	virtual ~CrudeTurnbuckleHDCz();
	double GetSizeH();
	double GetSize4();
	double GetSize5();
public:
	const CString& GetCustomIDField()
	{
		return m_strCustomIDField;
	}
	const CString& GetminDHField()
	{
		return m_strminDHField;
	}
	const CString& GetmaxDHField()
	{
		return m_strmaxDHField;
	}
	const CString& GetfBminField()
	{
		return m_strfBminField;
	}
	const CString& GetfBmaxDHField()
	{
		return m_strfBmaxField;
	}
private:
	static CString m_strCustomIDField;
	static CString m_strminDHField;
	static CString m_strmaxDHField;
	static CString m_strfBminField;
	static CString m_strfBmaxField;
	static CString m_strSizeHField;
	static CString m_strSize4Field;
	static CString m_strSize5Field;
	
};

#endif // !defined(AFX_CRUDETURNBUCKLEHDCZ_H__3560ECB4_5425_47C9_BA67_CCBB95A4B909__INCLUDED_)
