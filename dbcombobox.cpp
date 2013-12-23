// DBComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "DBComboBox.h"
#include "user.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDBComboBox

CDBComboBox::CDBComboBox()
{
	m_fis=true;
	SelChange=NULL;
	m_bDoSelchange=TRUE;
}

CDBComboBox::~CDBComboBox()
{
}


BEGIN_MESSAGE_MAP(CDBComboBox, CComboBox)
	//{{AFX_MSG_MAP(CDBComboBox)
	//ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDBComboBox message handlers

void CDBComboBox::LoadList()
{
	m_fis=true;
	try
	{
		if(m_RowRs==NULL || !m_RowRs->IsOpen())
			return;
		if(m_ListField==_T(""))
			return;
		//Çå¿ÕÏîÄ¿
		this->ResetContent();
		if(m_RowRs->IsEOF () && m_RowRs->IsBOF())
			return;
		m_RowRs->MoveFirst();
		COleVariant v;
		CString sTmp;
		while(!m_RowRs->IsEOF())
		{
			m_RowRs->GetFieldValue(m_ListField,v);
			if(v.vt==VT_BSTR)
				sTmp=vtos(v);
			else
				sTmp.Format(_T("%d"),vtoi(v));
			if(sTmp==_T("0"))
				sTmp.Format(_T("%g"),vtof(v));
			this->AddString(sTmp);
			m_RowRs->MoveNext();
		}
	}
	catch(::CDaoException * e)
	{
		//e->ReportError();
		e->Delete();
	}
	m_fis=false;
}

void CDBComboBox::RefLst()
{
	m_fis=true;
	try
	{
		
		if(m_RowRs==NULL || !m_RowRs->IsOpen())
			return;
		if(m_Rs==NULL || !m_Rs->IsOpen())
			return;
		if(m_BoundField==_T("") || m_Field==_T(""))
			return;
		COleVariant v;
		//m_Rs->GetFieldValue(m_Field,v);
		int i=0;
		if(m_RowRs->IsBOF() && m_RowRs->IsEOF())
		{
			this->SetCurSel(-1);
			return;
		}
		if(m_RowRs->IsBOF())
			m_RowRs->MoveFirst();
		else if(m_RowRs->IsEOF())
			m_RowRs->MoveLast();
		COleVariant v1;
		m_RowRs->GetFieldValue(m_ListField,v1);
		i=this->FindStringExact(-1,vtos(v1));
		this->SetCurSel(i);
	}
	catch(::CDaoException * e)
	{
		e->ReportError();
		e->Delete();
	}
	m_fis=false;
}

void CDBComboBox::OnSelchange() 
{
	// TODO: Add your control notification handler code here
	try
	{
		if(m_fis)
		{
			m_fis=false;
			return;
		}
		if(m_RowRs==NULL || !m_RowRs->IsOpen())
			return;
		if(m_Rs==NULL || !m_Rs->IsOpen())
			return;
		if(m_BoundField==_T("") || m_Field==_T(""))
			return;
		if(m_Rs->IsEOF() || m_Rs->IsBOF())
			return;
		int ix=this->GetCurSel();
		if(ix<0)
			return;
		m_RowRs->MoveFirst();
		int i=0;
		COleVariant v;
		while(!m_RowRs->IsEOF())
		{
			if(i==ix)
			{
				m_RowRs->GetFieldValue(m_BoundField,v);
				m_Rs->Edit();
				m_Rs->SetFieldValue(m_Field,v);
				m_RowRs->GetFieldValue(m_ListField,v);
				m_Rs->SetFieldValue(m_ListField,v);
				m_Rs->Update();
				break;
			}
			i++;
			m_RowRs->MoveNext();
		}
	}
	catch(::CDaoException * e)
	{
		//e->ReportError();
		e->Delete();
	}
}
