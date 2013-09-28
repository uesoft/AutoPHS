// VirtualListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "AutoPHS.h"
#include "user.h"
#include "VirtualListCtrl.h"
#include "EDIBgbl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVirtualListCtrl

CVirtualListCtrl::CVirtualListCtrl()
{
	try
	{
		m_prsREF = new CDaoRecordset(&EDIBgbl::dbPRJ);
		CString strSQL=_T("SELECT [SampleID], [CustomID] FROM PhsStructureREF");
		m_prsREF->m_strSort = _T("[SampleID], [SEQ]");
		m_prsREF->m_strFilter = _T("");
		m_prsREF->Open(dbOpenSnapshot, strSQL);
	}
	catch(CDaoException *ep)
	{
		ep->ReportError();
		ep->Delete();
	}
	m_lRefRecords = 0;
}

CVirtualListCtrl::~CVirtualListCtrl()
{
	m_prsREF->Close();
	delete m_prsREF;
}


BEGIN_MESSAGE_MAP(CVirtualListCtrl, CMyListCtrl)
	//{{AFX_MSG_MAP(CVirtualListCtrl)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVirtualListCtrl message handlers

void CVirtualListCtrl::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	long index = pDispInfo->item.iItem;
	
	
	TCHAR szValue[MAX_PATH];
	COleVariant varValue;		
	long subItem = 	pDispInfo->item.iSubItem;
	
	if(pDispInfo->item.mask & LVIF_TEXT)
	{
		CString str;		
		try
		{
			if(subItem >= 2)	
			{
				if (subItem < m_lRefRecords + 2)
				{
					m_prsREF->SetAbsolutePosition(subItem - 2);
					m_prsREF->GetFieldValue(_T("CustomID"), varValue);
					str = vtos(varValue);
				}
				else
					str = _T("");
			}
			else
				if (subItem ==0)
				{
					m_prsNAME->SetAbsolutePosition(index);//Set the file to desired index
					m_prsNAME->GetFieldValue(_T("SampleID"), varValue);
					str = vtos(varValue);
					CString strFilter;
					strFilter.Format(_T("SampleID = %s"), str);
					m_prsREF->m_strFilter = strFilter;
					if (m_prsREF->CanRestart())
						m_prsREF->Requery();
					m_lRefRecords = m_prsREF->GetRecordCount();
				}
				else 
					if (subItem == 1)
					{
						m_prsNAME->SetAbsolutePosition(index);
						m_prsNAME->GetFieldValue(_T("SampleName"), varValue);
						str = vtos(varValue);
					}
					
					//			SetItemText(index, 0, str);
					
		}
		catch(CDaoException* ep)
		{
			ep->ReportError();
			ep->Delete();
			return;
		}
		
		strcpy(szValue, str.GetBuffer(str.GetLength()));
		str.ReleaseBuffer();
		
		lstrcpyn(pDispInfo->item.pszText, szValue, pDispInfo->item.cchTextMax);//set item text
		
		//		m_nIndex = index;
	}		
	*pResult = 0;
}

BOOL CVirtualListCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class

	return CMyListCtrl::PreCreateWindow(cs);
}

//======================================================================
//该类是在程序设计后期加入的，因原来程序中多处用到了GetItemData()函数，
//为了与原程序的兼容而重载该函数，实现相同功能
DWORD CVirtualListCtrl::GetItemData(int nItem) const
{
	try
	{
		m_prsNAME->SetAbsolutePosition(nItem);
		COleVariant v;
		m_prsNAME->GetFieldValue(_T("SampleID"), v);
		return (DWORD)vtoi(v);
	}
	catch(CException *e)
	{
		e->Delete();
	}
	catch(...)
	{
		return 0;
	}


}

//DEL int CVirtualListCtrl::DelAllSelectedItem()
//DEL {
//DEL 	return 0;
//DEL }
