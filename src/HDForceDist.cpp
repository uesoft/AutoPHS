// HDForceDist.cpp: implementation of the HDForceDist class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HDForceDist.h"
#include "modPHScal.h"
#include "user.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*
HDForceDist对应恒力弹簧特性表
*/

CString HDForceDist::m_strCapacityField = _T("Capacity");
CString HDForceDist::m_strMinDistField = _T("MinDist");
CString HDForceDist::m_strMaxDistField = _T("MaxDist");
CString HDForceDist::m_strDHField = _T("DH");
HDForceDist::HDForceDist()
{
	m_pRs.m_pDatabase = &modPHScal::dbZDJcrude;
}

HDForceDist::~HDForceDist()
{

}




double HDForceDist::GetMinCapacity()
{
	m_pRs.MoveFirst();
	return vtof( m_pRs.GetFieldValue(m_strCapacityField) );	
}

double HDForceDist::GetMaxCapacity()
{
	m_pRs.MoveLast();
	return vtof( m_pRs.GetFieldValue(m_strCapacityField) );	
}

//当前记录的最小位移
double HDForceDist::GetMinDist()
{
	return vtof( m_pRs.GetFieldValue(m_strMinDistField) );
}

//当前记录的最大位移
double HDForceDist::GetMaxDist() 
{
	return vtof( m_pRs.GetFieldValue(m_strMaxDistField) );
}

double HDForceDist::GetCapacity()
{
	return vtof( m_pRs.GetFieldValue(m_strCapacityField) );
}

//弹簧编号
int HDForceDist::GetDH()
{
	return vtof( m_pRs.GetFieldValue(m_strDHField) );
}



