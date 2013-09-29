// SALbraceSel.cpp: implementation of the SALbraceSel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SALbraceSel.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SALbraceSel::SALbraceSel()
{

}

SALbraceSel::~SALbraceSel()
{

}

//Cphs.cpp(3476,3483)
bool SALbraceSel::SelSALbrace(CString &Ptype)
{
	int flag=0;
	try
	{
		SQLx.Format(_T("SELECT * FROM  %s  WHERE GDW1>= %g AND %s >=%g AND trim(CustomID)=\'%s\' %s ORDER BY GDW1,%s"),
			modPHScal::tbnSA,
			modPHScal::gdw,
			SAfDPmax ,
			tmpSelPJG / iNumPart * (vtob(rsza->GetCollect(_T("ifLongVertPipe"))) ? iNumPart : 1),
			Ptype[i],
			strSelNumCS,
			SAfDPmax);
	}
	catch(_com_error e)
	{
	}
	return flag;
}
