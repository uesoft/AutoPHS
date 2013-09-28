// Connect.cpp: implementation of the Connect class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Connect.h"
#include "EDIBgbl.h"
#include "user.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CString Connect::m_strTableName = _T("connect");
CString Connect::m_strCNTBField = _T("CNTB"); //CNTB头部件的字段名
CString Connect::m_strCNTEField = _T("CNTE"); //CNTE尾部件的字段名
CString Connect::m_strCNTBextlenFLDField = _T("CNTBextlenFLD");//头部件伸入下一部件的长度
Connect::Connect()
{
	m_pRs.m_pDatabase = &EDIBgbl::dbPRJ;
}

Connect::~Connect()
{

}

//得到头部件伸入下一部件的长度
double Connect::GetCNTBextlenFLD() 
{
	return vtof( m_pRs.GetFieldValue(m_strCNTBextlenFLDField) );
}
