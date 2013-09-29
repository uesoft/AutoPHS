// CrudeTurnbuckleHDCz.cpp: implementation of the CrudeTurnbuckleHDCz class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CrudeTurnbuckleHDCz.h"
#include "user.h"
#include "modPHScal.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CString CrudeTurnbuckleHDCz::m_strCustomIDField = _T("CustomID");
CString CrudeTurnbuckleHDCz::m_strminDHField = _T("minDH");
CString CrudeTurnbuckleHDCz::m_strmaxDHField = _T("maxDH");
CString CrudeTurnbuckleHDCz::m_strfBminField  = _T("fBmin");
CString CrudeTurnbuckleHDCz::m_strfBmaxField  = _T("fBmax");
CString CrudeTurnbuckleHDCz::m_strSizeHField = _T("sizeH");
CString CrudeTurnbuckleHDCz::m_strSize4Field = _T("size4");
CString CrudeTurnbuckleHDCz::m_strSize5Field = _T("size5");

CrudeTurnbuckleHDCz::CrudeTurnbuckleHDCz()
{
	m_pRs.m_pDatabase = &modPHScal::dbZDJcrude;
}

CrudeTurnbuckleHDCz::~CrudeTurnbuckleHDCz()
{

}

double CrudeTurnbuckleHDCz::GetSizeH()
{
	return vtof( m_pRs.GetFieldValue(m_strSizeHField) );
}
double CrudeTurnbuckleHDCz::GetSize4()
{
	return vtof( m_pRs.GetFieldValue(m_strSize4Field) );
}
double CrudeTurnbuckleHDCz::GetSize5()
{
	return vtof( m_pRs.GetFieldValue(m_strSize5Field) );
}