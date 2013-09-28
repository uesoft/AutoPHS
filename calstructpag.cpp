// CalStructPag.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "CalStructPag.h"
#include "user.h"
#include "Columns.h"
#include "Column.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCalStructPag dialog


CCalStructPag::CCalStructPag(_ConnectionPtr connSort,long lStructID,long lComponentID,CString strResult,CWnd* pParent /*=NULL*/)
	: CDialog(CCalStructPag::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCalStructPag)
	m_strFormula = _T("");
	m_strWhere = _T("");
	m_strCaption = _T("");
	//}}AFX_DATA_INIT
	m_iComNo=0;
	m_connSort=connSort;
	m_lComponentID=lComponentID;
	m_strResult=strResult;
	m_lStructID=lStructID;
	m_brBk.CreateSolidBrush(::GetSysColor(COLOR_INFOBK));
}


void CCalStructPag::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCalStructPag)
	DDX_Text(pDX, IDC_EDIT_FORMULA, m_strFormula);
	DDX_Text(pDX, IDC_EDIT_WHERE, m_strWhere);
	DDX_Text(pDX, IDC_EDIT_CAPTION, m_strCaption);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCalStructPag, CDialog)
	//{{AFX_MSG_MAP(CCalStructPag)
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCalStructPag message handlers

BOOL CCalStructPag::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	try
	{
		
		_RecordsetPtr rs;
		CString strSQL;
		strSQL.Format(_T("SELECT [SAStructComponent].[Number],[SAStructFormula].[Formula],[SAStructFormula].[Where],[SAStructFormula].[Caption] FROM [SAStructComponent],[SAStructFormula] WHERE [SAStructComponent].[ComponentID] = %d AND [SAStructComponent].[ComponentID]=[SAStructFormula].[ComponentID] AND [SAStructFormula].[Result]=\'%s\' "),
			m_lComponentID,m_strResult);
		rs=m_connSort->Execute(_bstr_t(strSQL),NULL,adCmdText);
		if(!rs->adoEOF)
		{
			CString strTemp;
			strTemp.Format(IDS_COMPONENT_de,vtoi(rs->GetCollect(0L)),m_strResult);
			this->SetWindowText(strTemp);
			this->m_strFormula=vtos(rs->GetCollect(1L));
			this->m_strWhere=vtos(rs->GetCollect(2L));
			this->m_strCaption=vtos(rs->GetCollect(3L));
			m_iComNo=vtoi(rs->GetCollect(0L));
			UpdateData(FALSE);
		}
		rs->Close();
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.Description());
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCalStructPag::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CDialog::PostNcDestroy();
	delete this;
}

HBRUSH CCalStructPag::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
/*	if(nCtlColor==CTLCOLOR_DLG)
	{
		return (HBRUSH)m_brBk;
	}*/
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

BEGIN_EVENTSINK_MAP(CCalStructPag, CDialog)
    //{{AFX_EVENTSINK_MAP(CCalStructPag)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()



void CCalStructPag::OnOK() 
{
	// TODO: Add extra validation here
	
//	CDialog::OnOK();
}

void CCalStructPag::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	//CDialog::OnCancel();
}



void CCalStructPag::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	/*CWnd* pWnd=GetDlgItem(IDC_EDIT_FORMULA);
	if(IsWindow(pWnd->GetSafeHwnd()))
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		this->ScreenToClient(&rect);
		rect.bottom=cy-5;
		rect.right=cx-5;
		pWnd->SetWindowPos(NULL,0,0,rect.Width(),rect.Height(),SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);
	}*/
}

void CCalStructPag::OnDestroy() 
{
	UpdateData();
	CString strSQL;
	strSQL.Format(_T("UPDATE [SAStructFormula] SET [Formula]=\'%s\',[Where]=\'%s\',[Caption]=\'%s\' WHERE [ComponentID]=%d AND [Result]=\'%s\'"),
		m_strFormula,m_strWhere,m_strCaption,m_lComponentID,m_strResult);
	strSQL.Replace(_T("\'\'"),_T("NULL"));
	try
	{
		m_connSort->Execute(_bstr_t(strSQL),NULL,adCmdText);
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.Description());
	}

	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here

}
