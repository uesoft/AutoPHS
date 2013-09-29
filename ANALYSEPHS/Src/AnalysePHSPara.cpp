// SelePartPHSData.cpp: implementation of the SelePartPHSData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\INC\AnalysePHSPara.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AnalysePHSPara::AnalysePHSPara()
{
}

AnalysePHSPara::~AnalysePHSPara()
{

}

float AnalysePHSPara::GetDblHangerRodDist()//modPHScal::DblHangerRodDist双吊拉杆间距
{
	return m_DblHangerRodDist;
}

long AnalysePHSPara::GetglClassID()//modPHScal::glClassID
{
	return m_glClassID;
}

float AnalysePHSPara::Gett0()//modPHScal::t0介质温度,C
{
	return m_t0;
}

void AnalysePHSPara::SetDblHangerRodDist(float DblHangerRodDist)//modPHScal::DblHangerRodDist双吊拉杆间距
{
	m_DblHangerRodDist=DblHangerRodDist;
}

void AnalysePHSPara::SetglClassID(long glClassID)//modPHScal::glClassID
{
	m_glClassID=glClassID;
}

void AnalysePHSPara::Sett0(float t0)//modPHScal::t0介质温度,C
{
	m_t0=t0;
}
