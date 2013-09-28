// InputSeleParaSA.cpp: implementation of the InputSeleParaSA class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\INC\AnalyseHangPara.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AnalyseHangPara::AnalyseHangPara()
{
 	m_strSelNumCS=_T("");//选择根部主槽钢数量的过滤字符串
	m_Ptype=NULL;   //不含附件的组件结构CustomID数组
	m_mvBWidth=0.0;     //固定支架根部主槽钢间距,cm,G47=根部表chdist字段值，G48=TZA表A01字段值
	m_tmpSelPJG=0.0;//累加零件重量后的计算荷载
	m_tmpSQL=_T("");          //当前的连接匹配查询字段信息
	m_tbn1=_T("");
	m_SAfDPmax=_T("");

	//类Cphs.cpp的数据成员(start)
	m_iNumPart=0;//iNumPart 
	//类Cphs.cpp的数据成员(end)
}

AnalyseHangPara::~AnalyseHangPara()
{

}

CString AnalyseHangPara::GetstrSelNumCS()
{
	return m_strSelNumCS;
}

CString *AnalyseHangPara::GetPtype()
{
	return m_Ptype;
}

float AnalyseHangPara::GetmvBWidth()
{
	return m_mvBWidth;
}

float AnalyseHangPara::GettmpSelPJG()
{
	return m_tmpSelPJG;
}

CString AnalyseHangPara::GettmpSQL()
{
	return m_tmpSQL;
}

CString AnalyseHangPara::Gettbn1()
{
	return m_tbn1;
}

CString AnalyseHangPara::GetSAfDPmax()
{
	return m_SAfDPmax;
}

int AnalyseHangPara::GetiNumPart()
{
	return m_iNumPart;
}

void AnalyseHangPara::SetstrSelNumCS(const CString &strSelNumCS)
{
	m_strSelNumCS=strSelNumCS;
}

void AnalyseHangPara::SetPtype(CString* Ptype)
{
	m_Ptype=Ptype;
}

void AnalyseHangPara::SetmvBWidth(float mvBWidth)
{
	m_mvBWidth=mvBWidth;
}

void AnalyseHangPara::SettmpSelPJG(float tmpSelPJG)
{
	m_tmpSelPJG=tmpSelPJG;
}

void AnalyseHangPara::SettmpSQL(const CString &tmpSQL)
{
	m_tmpSQL=tmpSQL;
}

void AnalyseHangPara::Settbn1(const CString &tbn1)
{
	m_tbn1=tbn1;
}

void AnalyseHangPara::SetSAfDPmax(const CString &SAfDPmax)
{
	m_SAfDPmax=SAfDPmax;
}

void AnalyseHangPara::SetiNumPart(int iNumPart)
{
	m_iNumPart=iNumPart;
}
