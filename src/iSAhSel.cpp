// iSAhSel.cpp: implementation of the CiSAhSel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "autophs.h"
#include "iSAhSel.h"
#include "crudeSA.h"
#include "user.h"
#include "InputZA.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CiSAhSel::CiSAhSel()
{

}

CiSAhSel::~CiSAhSel()
{

}

//Cphs.cpp(3465,3472)
CString CiSAhSel::GetiSAhSQL(CString &Ptype)
{
	crudeSA SA;
	InputZA za;
	CString SQLx=_T("");
	SQLx.Format(_T("SELECT * FROM  %s  WHERE  + %s %s >=  %g  AND trim(%s)=\'%s\' ORDER BY %s,%s"),
		SA.GettbnSATableName(),
		m_tmpSQL,
		m_SAfDPmax,
		m_tmpSelPJG / m_iNumPart * (za.GetifLongVertPipe() ? m_iNumPart : 1),
		SA.GetCustomIDField(),
		Ptype,
		SA.GetGDW1Field(),
		m_SAfDPmax);
	return SQLx;
}
