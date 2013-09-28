// ConstantSpring.h: interface for the ConstantSpring class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONSTANTSPRING_H__70A14B70_748C_4B97_AC22_E6AFEFD3E64B__INCLUDED_)
#define AFX_CONSTANTSPRING_H__70A14B70_748C_4B97_AC22_E6AFEFD3E64B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class ConstantSpring  
{
public:
	ConstantSpring();
	virtual ~ConstantSpring();
public:
	static double GetPGZ(double dbPgz,CString sPartID,int FirstCal);
	static double Getyr(double dbyr);
	static CString GetSelectLugSql(CString strPrtID,int iSEQofSPR,float tmpSelPJG4CSPRFiJ,float fWeight,CString strCustomID,int iFjmin);
	static void SetAforeCompDia(int iFiJ,int iSEQofSPR,int iFirstCal);
};

#endif // !defined(AFX_CONSTANTSPRING_H__70A14B70_748C_4B97_AC22_E6AFEFD3E64B__INCLUDED_)
