// SelZdjhDlg.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "SelZdjhDlg.h"
#include "Columns.h"
#include "Column.h"
#include "user.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelZdjhDlg dialog


CSelZdjhDlg::CSelZdjhDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelZdjhDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelZdjhDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_rs=NULL;
}


void CSelZdjhDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelZdjhDlg)
	DDX_Control(pDX, IDC_DATAGRID, m_dbGrid);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelZdjhDlg, CDialog)
	//{{AFX_MSG_MAP(CSelZdjhDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelZdjhDlg message handlers



BOOL CSelZdjhDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	try
	{
		m_dbGrid.SetForeColor(0xff0000);
		m_dbGrid.SetBackColor(0xffc0c0);
		if(m_rs!=NULL)
		{
			m_dbGrid.SetRefDataSource(m_rs->GetDataSource());
			int C=m_dbGrid.GetColumns().GetCount();
			int i=0;
			for(i=C-1;i>2;i--)
			{
				m_dbGrid.GetColumns().Remove(_variant_t((long)i));
			}
			m_dbGrid.GetColumns().Remove(_variant_t((long)0));
			m_dbGrid.GetColumns().GetItem(_variant_t((long)0)).SetDataField("ZDJH");
			m_dbGrid.GetColumns().GetItem(_variant_t((long)0)).SetCaption(GetResStr(IDS_PHSMARK));
			//m_dbGrid.GetColumns().GetItem(_variant_t((long)0)).SetWidth(80);
			m_dbGrid.GetColumns().GetItem(_variant_t((long)1)).SetDataField("JSDBH1");
			m_dbGrid.GetColumns().GetItem(_variant_t((long)1)).SetCaption(GetResStr(IDS_CountNodeMark));
		}
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}
	catch(...)
	{
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
