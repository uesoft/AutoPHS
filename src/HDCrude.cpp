// HDCrude.cpp: implementation of the HDCrude class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HDCrude.h"
#include "modPHScal.h"
#include "user.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CString HDCrude::m_strFjminField = _T("Fjmin");
CString HDCrude::m_strDHField = _T("DH");
CString HDCrude::m_strBHField = _T("BH");
CString HDCrude::m_strfBminField = _T("fBmin");
CString HDCrude::m_strfBmaxField = _T("fBmax");
CString HDCrude::m_strCustomIDField = _T("CustomID");
CString HDCrude::m_strWeightField = _T("Weight");
CString HDCrude::m_strSizeCField = _T("SizeC");
CString HDCrude::m_strSizeHField = _T("SizeH");
CString HDCrude::m_strTField = _T("T");
CString HDCrude::m_strPPField = _T("PP");
HDCrude::HDCrude()
{
	m_pRs.m_pDatabase = &modPHScal::dbZDJcrude;
}

HDCrude::~HDCrude()
{

}

int HDCrude::GetDH()
{
	return vtoi(m_pRs.GetFieldValue(m_strDHField));
}

CString HDCrude::GetBH()
{
	return vtos(m_pRs.GetFieldValue(m_strBHField));
}

double HDCrude::GetSizeC()
{
	return vtof(m_pRs.GetFieldValue(m_strSizeCField));
}
//得到最小螺栓孔直径
int HDCrude::GetFjmin() 
{
	return vtoi(m_pRs.GetFieldValue(m_strFjminField));
}

//得到组件质量
double HDCrude::GetWeight() 
{
	return vtoi(m_pRs.GetFieldValue(m_strWeightField));
}

//得到型式
CString HDCrude::GetCustomID()
{
	return vtos(m_pRs.GetFieldValue(m_strCustomIDField));
}

double HDCrude::GetSizeH()
{
	return vtof(m_pRs.GetFieldValue(m_strSizeHField));
}
double HDCrude::GetT()
{
	return vtof( m_pRs.GetFieldValue(m_strTField) );
}

double HDCrude::GetPP()
{
	return vtof( m_pRs.GetFieldValue(m_strPPField) );
}

double HDCrude::GetfBmin()
{
	return vtof( m_pRs.GetFieldValue(m_strfBminField) );
}
double HDCrude::GetfBmax()
{
	return vtof( m_pRs.GetFieldValue(m_strfBmaxField) );
}



