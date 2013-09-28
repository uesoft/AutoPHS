// DataTableBase.cpp: implementation of the CDataTableBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DataTableBase.h"
#include "user.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DataTableBase::DataTableBase()
{

}

DataTableBase::~DataTableBase()
{
	if( m_pRs.IsOpen() )
	{
		m_pRs.Close();
	}
}

void DataTableBase::Initialize(const CString &strSql)
{
	if( m_pRs.IsOpen() )
	{
		m_pRs.Close();
	}
	m_pRs.Open(dbOpenDynaset,strSql);
}


BOOL DataTableBase::IsNull()
{
	if( !m_pRs.IsOpen() )
	{
		return TRUE;
	}
	else if( m_pRs.IsEOF() && m_pRs.IsBOF() )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
	return FALSE;
}


void DataTableBase::FindFirst(const CString& strSql)
{
	m_pRs.FindFirst(strSql);
}

void DataTableBase::MovePrev()
{
	m_pRs.MovePrev();
}

BOOL DataTableBase::IsBOF()
{
	return m_pRs.IsBOF();
}

void DataTableBase::MoveNext()
{
	m_pRs.MoveNext();
}

CString DataTableBase::GetFieldValue(CString strFieldName)
{
	return vtos( m_pRs.GetFieldValue(strFieldName) );
}

void DataTableBase::Update()
{
	m_pRs.Update();
}
