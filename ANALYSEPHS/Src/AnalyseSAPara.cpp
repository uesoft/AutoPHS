// AnalyseSAPara.cpp: implementation of the AnalyseSAPara class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\INC\AnalyseSAPara.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AnalyseSAPara::AnalyseSAPara()
{
	m_tbnSA=_T("");//modPHScal::tbnSA
	m_gdw=0.0;//modPHScal::gdw
	m_glNumSA=0;//modPHScal::glNumSA 根部的数量	
	m_WidthB=0.0;//modPHScal::WidthB根部柱子宽度B值
	m_Lspan=0.0;//modPHScal::Lspan根部L1值=跨距
	m_giSAG100No=0;//modPHScal::giSAG100No
}

AnalyseSAPara::~AnalyseSAPara()
{

}

CString AnalyseSAPara::GettbnSA()
{
	return m_tbnSA;
}

float AnalyseSAPara::Getgdw()
{
	return m_gdw;
}

int AnalyseSAPara::GetglNumSA()
{
	return m_glNumSA;
}

float AnalyseSAPara::GetWidthB()
{
	return m_WidthB;
}

float AnalyseSAPara::GetLspan()
{
	return m_Lspan;
}

int AnalyseSAPara::GetgiSAG100No()
{
	return m_giSAG100No;
}

void AnalyseSAPara::SettbnSA(CString &tbnSA)
{

	m_tbnSA=tbnSA;
}

void AnalyseSAPara::Setgdw(float gdw)
{
	m_gdw=gdw;
}

void AnalyseSAPara::SetglNumSA(int glNumSA)
{
	m_glNumSA=glNumSA;
}

void AnalyseSAPara::SetWidthB(float WidthB)
{
	m_WidthB=WidthB;
}

void AnalyseSAPara::SetLspan(float Lspan)
{
	m_Lspan=Lspan;
}

void AnalyseSAPara::SetgiSAG100No(int giSAG100No)
{
	m_giSAG100No=giSAG100No;
}
