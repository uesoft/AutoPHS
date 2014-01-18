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
		m_prsREF.CreateInstance(__uuidof(Recordset));
//		m_prsREF->CursorLocation = adUseClient;
		CString strSQL=_T("SELECT [SampleID], [CustomID] FROM PhsStructureREF ORDER BY [SampleID], [SEQ]");
// 		m_prsREF->Sort = "[SampleID]";
// 		m_prsREF->Filter = _variant_t("");
		m_prsREF->Open((_bstr_t)strSQL, _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
			adOpenKeyset, adLockOptimistic, adCmdText); 
	}
	catch(CException * ep)
	{
		ep->ReportError();
		ep->Delete();
	}
	m_lRefRecords = 0;
}

CVirtualListCtrl::~CVirtualListCtrl()
{
	if (m_prsREF != NULL)
	{
		m_prsREF->Close();
		m_prsREF.Release();
	}
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
	
// 	FILE* fp = NULL;
// 	fp = fopen("testado.txt", "a+");
	TCHAR szValue[MAX_PATH];
	_variant_t varValue;		
	long subItem = 	pDispInfo->item.iSubItem;
	
// 	fprintf(fp, "%d %d", index, subItem);

	if(pDispInfo->item.mask & LVIF_TEXT)
	{
		CString str;		
		try
		{
			if(subItem >= 2)	
			{
				if (subItem < m_lRefRecords + 2)
				{
					m_prsREF->AbsolutePosition = (PositionEnum)(subItem-1);
					m_prsREF->get_Collect((_variant_t)(_T("CustomID")), &varValue);
					str = vtos(varValue);
// 					fprintf(fp, "m_prsREF CustomID: %s", str);
				}
				else
				{
					str = _T("");
//					fprintf(fp, "m_prsREF %s", str);
				}
			}
			else
			if (subItem ==0)
			{
				m_prsNAME->AbsolutePosition = (PositionEnum)(index+1);//Set the file to desired index
				m_prsNAME->get_Collect((_variant_t)_T("SampleID"), &varValue);
				str = vtos(varValue);
				CString strFilter;
				strFilter.Format(_T("SampleID = %s"), str);
 				m_prsREF->Filter = (_variant_t)strFilter;
 				m_prsREF->Requery(adOptionUnspecified);
				m_lRefRecords = m_prsREF->GetRecordCount();
// 				fprintf(fp, "m_prsNAME SampleID: %s", str);
			}
			else 
			if (subItem == 1)
			{
				if (index == 0)
				{
					m_prsNAME->AbsolutePosition = (PositionEnum)1;
				} else 
					m_prsNAME->AbsolutePosition = (PositionEnum)index;						

				m_prsNAME->get_Collect((_variant_t)_T("SampleName"), &varValue);
				str = vtos(varValue);
// 				fprintf(fp, "m_prsNAME SampleName: %s", str);
			}
			
			//			SetItemText(index, 0, str);
					
		}
		catch(_com_error e)
		{
// 			fprintf(fp, " _com_error\n");
// 			fclose(fp);
			return;
		}
		
		strcpy(szValue, str.GetBuffer(str.GetLength()));
		str.ReleaseBuffer();
		
		lstrcpyn(pDispInfo->item.pszText, szValue, pDispInfo->item.cchTextMax);//set item text

// 		fprintf(fp, "\n");
// 		fclose(fp);
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
		m_prsNAME->AbsolutePosition = (PositionEnum)nItem;
		_variant_t v;
		m_prsNAME->get_Collect((_variant_t)_T("SampleID"), &v);
		return (DWORD)vtoi(v);
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
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
