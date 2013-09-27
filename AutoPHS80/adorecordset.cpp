// ADORecordset.cpp: implementation of the CADORecordset class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "autophs.h"
#include "ADORecordset.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CADORecordset::CADORecordset()
{
	m_pSet=NULL;
	m_ActiveConnection=NULL;
}

CADORecordset::~CADORecordset()
{
	if(m_pSet!=NULL)
	{
		if(m_pSet->State != adStateClosed)
		{
			Close();
		}
		m_pSet=NULL;
	}
}

CADORecordset::CADORecordset(_ConnectionPtr pCon)
{
	m_ActiveConnection=pCon;
	m_pSet=NULL;
}

void CADORecordset::Open(CString strCmd, int CursorType, int LockType, int CmdType)
{
	if(m_pSet==NULL)
		m_pSet.CreateInstance(__uuidof(Recordset));
	Close();
	m_pSet->Open(_variant_t(strCmd),(IDispatch*)m_ActiveConnection,(CursorTypeEnum)CursorType,(adoLockTypeEnum)LockType,CmdType);
}

void CADORecordset::Update()
{
	if(m_pSet!=NULL)
	{
		if(m_pSet->State != adStateClosed)
		{
			m_pSet->Update();
		}
	}
}

void CADORecordset::Edit()
{

}

void CADORecordset::Close()
{
	if(m_pSet!=NULL)
	{
		if(m_pSet->State != adStateClosed)
		{
			try
			{

//				m_pSet->Update();
			}
			catch(...)
			{
				m_pSet->CancelUpdate();
			}
			m_pSet->Close();
		}
	}
}



void  CADORecordset::SetFieldValue(LPCTSTR FldName, _variant_t &var)
{
	m_pSet->Fields->GetItem(_variant_t(FldName))->Value=var;
}

void  CADORecordset::SetFieldValue(int index, _variant_t &var)
{
	m_pSet->Fields->GetItem(_variant_t((long)index))->Value=var;
}

void  CADORecordset::GetFieldValue(LPCTSTR FldName, _variant_t &retVar)
{
	retVar=m_pSet->Fields->GetItem(_variant_t(FldName))->Value;
}

void  CADORecordset::GetFieldValue(int index, _variant_t &retVar)
{
	retVar=m_pSet->Fields->GetItem(_variant_t((long)index))->Value;
}

BOOL  CADORecordset::IsEOF()
{
	return (BOOL)m_pSet->adoEOF;
}

BOOL  CADORecordset::IsBOF()
{
	return (BOOL)m_pSet->BOF;
}

int  CADORecordset::GetRecordCount()
{
	return m_pSet->GetRecordCount();
}

int  CADORecordset::GetFieldCount()
{
	return m_pSet->Fields->Count;
}

void  CADORecordset::AddNew()
{
	m_pSet->AddNew();
}

void  CADORecordset::MoveLast()
{
	m_pSet->MoveLast();
}

void  CADORecordset::MoveFirst()
{
	m_pSet->MoveFirst();
}

BOOL CADORecordset::FindFirst(CString strFind)
{
	try
	{
		m_pSet->MoveFirst();
		m_pSet->Find(_bstr_t(strFind),0,adSearchForward);
		if(m_pSet->adoEOF)
			return FALSE;
		return TRUE;
	}

	catch(CException *e)
	{
		e->Delete();
		return FALSE;
	}
	catch(...)
	{
		return FALSE;
	}
}

void  CADORecordset::MoveNext()
{
	m_pSet->MoveNext();
}

void  CADORecordset::MovePrev()
{
	m_pSet->MovePrevious();
}

BOOL CADORecordset::IsOpen()
{
	return (m_pSet !=NULL && m_pSet->State==adStateOpen);
}

void CADORecordset::SetCursorLocation(int val)
{
	try
	{
		if(m_pSet==NULL)
			m_pSet.CreateInstance(__uuidof(Recordset));
		m_pSet->CursorLocation=(CursorLocationEnum)val;
	}
	catch(CException *e)
	{
		e->Delete();
	}
	catch(...)
	{
	}
}
