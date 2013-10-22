// DlgEditDB.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "DlgEditDB.h"
#include "InputBox.h"
#include "_recordset.h"
#include "ModEncrypt.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgEditDB dialog
_TCHAR dbConnectionString[]=_T("Provider=Microsoft.Jet.OLEDB.4.0;Persist Security Info=False;Data Source=%s;Jet OLEDB:Database Password=%s");

CDlgEditDB::CDlgEditDB(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEditDB::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgEditDB)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgEditDB::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgEditDB)
	DDX_Control(pDX, IDC_LIST1, m_lstTableName);
	DDX_Control(pDX, IDC_DATAGRID1, m_dbGrid);
	DDX_Control(pDX, IDC_ADODC1, m_Adodc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgEditDB, CDialog)
	//{{AFX_MSG_MAP(CDlgEditDB)
	ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeList1)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgEditDB message handlers

BOOL CDlgEditDB::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if(ListTableName()==FALSE)
	{
		this->EndDialog(IDCANCEL);
		return FALSE;
	}
	if(m_lstTableName.GetCount()>0)
	{
		m_lstTableName.SetCurSel(0);
		OnSelchangeList1();
	}
 CListBox * list=(CListBox *)GetDlgItem(IDC_LIST1);

    CDC * dc=list->GetDC();

    char string[256];
    int max_len=0;

    for(int i=0;i<list->GetCount();i++)
    {
    list->GetText(i,string);

    CSize size=dc->GetTextExtent(string,strlen(string));

    if(size.cx>max_len)
        max_len=size.cx;
    }
    ReleaseDC(dc);

    list->SendMessage(LB_SETHORIZONTALEXTENT,max_len,0);


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgEditDB::OpenTable()
{
	try
	{
		CString strCon;
		strCon.Format(dbConnectionString,m_strDBName,m_strPassword);
		m_dbGrid.SetRefDataSource(NULL);
		m_Adodc.SetConnectionString(strCon);
		m_Adodc.SetCommandType(1);
		m_Adodc.SetRecordSource(m_strSQL);
		m_Adodc.Refresh();
		m_dbGrid.SetRefDataSource(m_Adodc.GetRecordset().GetDataSource());
	}
	catch(CException *e)
	{
		e->Delete();
	}
}

BOOL CDlgEditDB::ListTableName()
{
	m_lstTableName.ResetContent();
	_ConnectionPtr db;
	db.CreateInstance(__uuidof(_Connection));
	CInputBox inpBox;
	inpBox.m_bIsProtected=TRUE;
	BOOL bIsRet=FALSE;
	CString strConnect;
	int errNum=0;
	_ConnectionPtr pCon;
	inpBox.m_strWndText.LoadString(IDS_NEEDPASSWORD);
	inpBox.m_strTitle.LoadString(IDS_PLEASEINPUTDBPASSWORD);
	try
	{
		pCon.CreateInstance(__uuidof(Connection));
		CString strCon;

		while(true)
		{
			try
			{
				strCon.Format(dbConnectionString,m_strDBName,m_strPassword);
				pCon->Open(_bstr_t(strCon),_T(""),_T(""),-1);
			}
			catch(_com_error e)
			{
				if(e.WCode()==0)
				{
					errNum++;
					if(errNum==1)
					{
						//strConnect.Format(_T(";pwd=%s"),::ModEncrypt::gstrDBZdjCrudePassWord);
						m_strPassword=ModEncrypt::gstrDBZdjCrudePassWord;
						continue;
					}
					else
					{
						if(inpBox.DoModal()!=IDOK)
						{
							bIsRet=TRUE;
							break;
						}
						m_strPassword=inpBox.m_strValue;
						//strConnect.Format(_T(";pwd=%s"),inpBox.m_strValue);
						continue;
					}
				}
				else
				{
					bIsRet=TRUE;
					break;
				}
			}
			break;
		}
		if(bIsRet) return FALSE;
		pCon->Close();
		pCon=NULL;
// 		db.Open(m_strDBName,FALSE,TRUE,_T(";pwd=") + m_strPassword);
		CString ConnectionString="Provider=Microsoft.Jet.OLEDB.4.0;Persist Security Info=False;Data Source=" + m_strDBName;
		db->Open((_bstr_t)ConnectionString, "", (_bstr_t)m_strPassword, adConnectUnspecified);
/*
		int i,c;
		c=db.GetTableDefCount();
		for(i=0;i<c;i++)
		{
			db.GetTableDefInfo(i,tbInfo);
			if(!(tbInfo.m_lAttributes & dbSystemObject) && !(tbInfo.m_lAttributes & dbHiddenObject))
			{
				m_lstTableName.AddString(tbInfo.m_strName);
			}
		}
*/
		_bstr_t bt;
		_RecordsetPtr rs;
		rs = db->OpenSchema(adSchemaTables);
		while (!rs->adoEOF)
		{
			bt = rs->Fields->GetItem(_T("TABLE_TYPE"))->Value;
			if (!strcmp((char*)bt, "TABLE"))
			{
				bt = rs->Fields->GetItem(_T("TABLE_NAME"))->Value;
				m_lstTableName.AddString((char*)bt);
			}
			rs->MoveNext();
		}
		return TRUE;
	}
	catch(CException *e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.Description());
		return FALSE;
	}
}

//DEL void CDlgEditDB::OnSelcancelList1() 
//DEL {
//DEL 	// TODO: Add your control notification handler code here
//DEL 
//DEL }

void CDlgEditDB::OnSelchangeList1() 
{
	// TODO: Add your control notification handler code here
	int ix=m_lstTableName.GetCurSel();
	if(ix!=-1)
	{
		CString TableName;
		CString strSQL;
		m_lstTableName.GetText(ix,TableName);
		CString strWndTitle;
		strWndTitle.Format(IDS_SEE_DATABASE,TableName);
		this->SetWindowText(strWndTitle);
		strSQL.Format(_T("SELECT * FROM [%s]"),TableName);
		if(strSQL != m_strSQL)
		{
			m_strSQL=strSQL;
			OpenTable();
		}
	}
}

BEGIN_EVENTSINK_MAP(CDlgEditDB, CDialog)
    //{{AFX_EVENTSINK_MAP(CDlgEditDB)
	ON_EVENT(CDlgEditDB, IDC_ADODC1, 201 /* MoveComplete */, OnMoveCompleteAdodc1, VTS_I4 VTS_DISPATCH VTS_PI4 VTS_DISPATCH)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CDlgEditDB::OnMoveCompleteAdodc1(long adReason, LPDISPATCH pError, long FAR* adStatus, LPDISPATCH pRecordset) 
{
	// TODO: Add your control notification handler code here
	try
	{
		CString sTitle;
		sTitle.Format(_T("%d/%d"),m_Adodc.GetRecordset().GetAbsolutePosition(),m_Adodc.GetRecordset().GetRecordCount());
		m_Adodc.SetCaption(sTitle);
	}
	catch(CException *e)
	{
		e->Delete();
	}
}

void CDlgEditDB::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	CRect rc1,rc2,rc3,rc4;
	if(IsWindow(m_lstTableName.GetSafeHwnd()) && IsWindow(m_dbGrid.GetSafeHwnd()) && IsWindow(m_Adodc.GetSafeHwnd()))
	{
		this->GetClientRect(&rc1);
		m_lstTableName.GetWindowRect(&rc2);
		this->ScreenToClient(&rc2);
		int W,H;
		W=rc2.Width();
		rc2.left=0;rc2.right=W;
		rc2.top=0;rc2.bottom=rc1.Height()-1;
		m_lstTableName.MoveWindow(&rc2);
		
		
		m_Adodc.GetWindowRect(&rc4);
		this->ScreenToClient(&rc4);
		rc3.left=rc2.right+1;
		rc3.top=0;
		rc3.bottom=rc1.Height()-rc4.Height()-1;
		rc3.right=rc1.Width()-1;
		m_dbGrid.MoveWindow(&rc3);

		H=rc4.Height();
		rc4.top=rc3.bottom+1;
		rc4.left=rc2.right+1;
		rc4.bottom =rc4.top+H;
		rc4.right=rc1.Width()-1;
		::MoveWindow(m_Adodc.GetSafeHwnd(),rc4.left,rc4.top,rc4.Width(),rc4.Height(),TRUE);
	}
}
