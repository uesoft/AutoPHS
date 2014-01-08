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
		if(m_RowRs==NULL || m_RowRs->State != adStateOpen)
			return;
		if(m_ListField==_T(""))
			return;
		//Çå¿ÕÏîÄ¿
		this->ResetContent();
		if(m_RowRs->adoEOF && m_RowRs->BOF)
			return;
		m_RowRs->MoveFirst();
		_variant_t v;
		CString sTmp;
		while(!m_RowRs->adoEOF)
		{
			m_RowRs->get_Collect((_variant_t)m_ListField, &v);
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
	catch(CException *e)
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
		
		if(m_RowRs==NULL || m_RowRs->State != adStateOpen)
			return;
		if(m_Rs==NULL || m_Rs->State != adStateOpen)
			return;
		if(m_BoundField==_T("") || m_Field==_T(""))
			return;
		_variant_t v;
		//m_Rs->get_Collect((_variant_t)m_Field,v);
		int i=0;
		if(m_RowRs->BOF && m_RowRs->adoEOF)
		{
			this->SetCurSel(-1);
			return;
		}
		if(m_RowRs->BOF)
			m_RowRs->MoveFirst();
		else if(m_RowRs->adoEOF)
			m_RowRs->MoveLast();
		_variant_t v1;
		m_RowRs->get_Collect((_variant_t)m_ListField, &v1);
		i=this->FindStringExact(-1,vtos(v1));
		this->SetCurSel(i);
	}
	catch(CException *e)
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
		if(m_RowRs==NULL || m_RowRs->State != adStateOpen)
			return;
		if(m_Rs==NULL || m_Rs->State != adStateOpen)
			return;
		if(m_BoundField==_T("") || m_Field==_T(""))
			return;
		if(m_Rs->adoEOF || m_Rs->BOF)
			return;
		int ix=this->GetCurSel();
		if(ix<0)
			return;
		m_RowRs->MoveFirst();
		int i=0;
		_variant_t v;
		while(!m_RowRs->adoEOF)
		{
			if(i==ix)
			{
				m_RowRs->get_Collect((_variant_t)m_BoundField, &v);
//				m_Rs->Edit();
				m_Rs->put_Collect((_variant_t)m_Field, v);
				m_RowRs->get_Collect((_variant_t)m_ListField, &v);
				m_Rs->put_Collect((_variant_t)m_ListField, v);
				m_Rs->Update();
				break;
			}
			i++;
			m_RowRs->MoveNext();
		}
	}
	catch(CException *e)
	{
		//e->ReportError();
		e->Delete();
	}
}
