// CrudePA.cpp: implementation of the CCrudePA class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CrudePA.h"
#include "user.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CString CrudePA::m_strCustomIDField=_T("CustomID");
CString CrudePA::m_strDWField=_T("DW");
CString CrudePA::m_strPMAXField=_T("PMAX");
CString CrudePA::m_strTJField=_T("TJ");
CString CrudePA::m_strWeightField=_T("Weight");
CString CrudePA::m_strTableName=_T("crudePA");

CrudePA::CrudePA(_ConnectionPtr DataMdb)
{
//	m_pRs.m_pDatabase = &DataMdb;
	m_pRs->put_ActiveConnection(DataMdb);
}

CrudePA::~CrudePA()
{

}

double CrudePA::GetDW()
{
	return vtod(m_pRs.GetFieldValue(m_strDWField));
}

CString CrudePA::GetCustomID()
{
	return vtos(m_pRs.GetFieldValue(m_strCustomIDField));
}

double CrudePA::GetPMAX()
{
	return vtod(m_pRs.GetFieldValue(m_strPMAXField));
}

double CrudePA::GetTJ()
{
	return vtod(m_pRs.GetFieldValue(m_strTJField));
}

float CrudePA::GetWeight()
{
	return vtof(m_pRs.GetFieldValue(m_strWeightField));
}