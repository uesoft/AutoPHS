// SAG51Sel.cpp: implementation of the SAG51Sel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SAG51Sel.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SAG51Sel::SAG51Sel()
{

}

SAG51Sel::~SAG51Sel()
{

}

//	case iG51: Cphs.cpp(3488,3496),±§ÁºÐü±Ûµõ
//	case iG56: ¼ÐÖùÐü±ÛÖ§¼Ü
//	case iG57: ¼ÐÖùÈý½ÇÖ§¼Ü
bool SAG51Sel::SelSAG51(CString &Ptype)
{
	int flag=0;
	try
	{
		SQLx.Format(_T("SELECT * FROM %s WHERE GDW1>= %g AND size2>=%g AND %s >=%g AND trim(CustomID)=\'%s\' %s ORDER BY GDW1,size2,%s"),
			modPHScal::tbnSA,
			modPHScal::gdw,
			modPHScal::gdw/modPHScal::WidthB,
			SAfDPmax,
			tmpSelPJG / iNumPart * (vtob(rsza->GetCollect(_T("ifLongVertPipe"))) ? iNumPart : 1),
			Ptype[i],
			strSelNumCS,
			SAfDPmax);
	}
	catch(_com_error e)
	{
	}
	retrun flag;
}
