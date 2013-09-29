// LugDiaOfCSPR.cpp: implementation of the LugDiaOfCSPR class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LugDiaOfCSPR.h"
#include "user.h"
#include "modPHScal.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CString LugDiaOfCSPR::m_strPmaxKgfField = _T("PmaxKgf");
CString LugDiaOfCSPR::m_strFiJField = _T("FiJ");
CString LugDiaOfCSPR::m_strCustomIDField = _T("CustomID"); //恒力弹簧型式字段名
CString LugDiaOfCSPR::m_strminDHField = _T("minDH"); //恒力弹簧最小规格号字段名
CString LugDiaOfCSPR::m_strmaxDHField = _T("maxDH"); //恒力弹簧最大规格号字段名
CString LugDiaOfCSPR::m_strPmaxField = _T("Pmax"); //恒力弹簧工作荷(N)字段名
CString LugDiaOfCSPR::m_strFiKField = _T("FiK"); //恒力弹簧的吊板孔直径
CString LugDiaOfCSPR::m_strTField = _T("T");

LugDiaOfCSPR::LugDiaOfCSPR()
{
	m_pRs.m_pDatabase = &modPHScal::dbZDJcrude;
}

LugDiaOfCSPR::~LugDiaOfCSPR()
{

}

//得以恒力弹簧松紧螺母直径
int LugDiaOfCSPR::GetFiJ() 
{
	return vtoi(m_pRs.GetFieldValue(m_strFiJField));
}

//恒力弹簧的吊板孔直径
int LugDiaOfCSPR::GetFiK() 
{
	return vtoi(m_pRs.GetFieldValue(m_strFiKField));
}

double LugDiaOfCSPR::GetT()
{
	return vtof(m_pRs.GetFieldValue(m_strTField));
}



