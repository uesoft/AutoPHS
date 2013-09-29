// SAbeamSel.cpp: implementation of the SAbeamSel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SAbeamSel.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SAbeamSel::SAbeamSel()
{

}

SAbeamSel::~SAbeamSel()
{

}

//Cphs.cpp(3500,3534)
//	case iSAbeam: 简支梁
//	case iG52_55: 板肋螺栓简支吊
bool SAbeamSel::SelSAbeam(CString &Ptype)
{
	int flag=0;
	try
	{
		//简支梁G31~G39和G52~G55排序方式应为ORDER BY L1,GDW1,%s或者ORDER BY DH
		//其它根部排序方式应为ORDER BY GDW1,L1,%s或者ORDER BY DH
		
		//在2006.06.09彭范庚发现根部选型有部分型号偏大以前，
		//根部选型排序方式为ORDER BY GDW1,L1,%s
		if( iNumPart == 2 )
		{
			//两个根部梁承担所有荷载
			SQLx.Format(_T("SELECT * FROM %s WHERE GDW1 >= IIF(%g > L1/2 , L1-%g , %g) AND L1>= %g AND %s >=%g AND trim(CustomID)=\'%s\' %s ORDER BY DH"),
				modPHScal::tbnSA,
				modPHScal::gdw+(modPHScal::glNumSA==1 ? modPHScal::DblHangerRodDist : 0)/2,
				modPHScal::gdw+(modPHScal::glNumSA==1 ? modPHScal::DblHangerRodDist : 0) / 2,
				modPHScal::gdw+(modPHScal::glNumSA==1 ? modPHScal::DblHangerRodDist : 0) / 2 ,
				modPHScal::Lspan,
				SAfDPmax,
				tmpSelPJG / iNumPart * (vtob(rsza->GetCollect(_T("ifLongVertPipe"))) ? iNumPart : 1 ),
				Ptype[i],
				strSelNumCS);
		}
		else
		{
			//一个根部梁承担所有荷载
			//假定荷载作用在一点，这样选择出来的根部偏于安全。
			SQLx.Format(_T("SELECT * FROM %s WHERE GDW1>=IIF(%g > L1/2 , L1-%g , %g) AND L1>=%g AND %s>=%g AND trim(CustomID)=\'%s\' %s ORDER BY DH"),
				modPHScal::tbnSA,
				modPHScal::gdw+(modPHScal::glNumSA==1 ? modPHScal::DblHangerRodDist : 0)/2,
				modPHScal::gdw+(modPHScal::glNumSA==1 ? modPHScal::DblHangerRodDist : 0)/2,
				modPHScal::gdw+(modPHScal::glNumSA==1 ? modPHScal::DblHangerRodDist : 0)/2,
				modPHScal::Lspan,
				SAfDPmax,
				tmpSelPJG / iNumPart * (vtob(rsza->GetCollect(_T("ifLongVertPipe"))) ? iNumPart : 1),
				Ptype[i],
				strSelNumCS);
		}
	}
	catch(_com_error e)
	{
	}
	return flag;
}
