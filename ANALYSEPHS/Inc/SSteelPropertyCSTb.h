// SSteelPropertyCSTb.h: interface for the SSteelPropertyCSTb class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SSTEELPROPERTYCSTB_H__76E35D3B_EBFE_49FB_A176_8839C6F5DB33__INCLUDED_)
#define AFX_SSTEELPROPERTYCSTB_H__76E35D3B_EBFE_49FB_A176_8839C6F5DB33__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//Cphs.cpp(3658,4390)
//打开槽钢特性表，备查
//SQLx = _T("SELECT * FROM SSteelPropertyCS ORDER BY ID");
class SSteelPropertyCSTb  
{
public:
	SSteelPropertyCSTb();
	virtual ~SSteelPropertyCSTb();
public:
	//从数据库中获取数据
	BOOL GetData( CDaoRecordset* pRs );
public:
	float J1y;
	float J1z;
	float W1y;
	float W1z;
	float F1;
	float h1;
	float b1;
	float J2y;
	float J2z;
	float W2y;
	float W2z;
	float F2;
	float h2;
	float b2;
};

#endif // !defined(AFX_SSTEELPROPERTYCSTB_H__76E35D3B_EBFE_49FB_A176_8839C6F5DB33__INCLUDED_)
