// DlgCalStructRes.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "DlgCalStructRes.h"
#include "EDIBgbl.h"
#include "user.h"
#include "basDirectory.h"
#include "Columns.h"
#include "Column.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgCalStructRes dialog


CDlgCalStructRes::CDlgCalStructRes(int iComCount,CString* pstrType,UINT * pnLen,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCalStructRes::IDD, pParent)
	, m_igridRow(-1)
{
	//{{AFX_DATA_INIT(CDlgCalStructRes)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pstrType=pstrType;
	m_pnLen=pnLen;
	m_iComCount=iComCount;
	m_size.cx=0;
	m_size.cy=0;
	m_bCreated=FALSE;
}


void CDlgCalStructRes::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCalStructRes)
	DDX_Control(pDX, IDC_DATAGRID1, m_grid);
	DDX_Control(pDX, IDC_DATAGRID3, m_grid1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgCalStructRes, CDialog)
	//{{AFX_MSG_MAP(CDlgCalStructRes)
	ON_WM_SIZE()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgCalStructRes message handlers

void CDlgCalStructRes::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	if(!m_bCreated)
		return;
	// TODO: Add your message handler code here
	int dx=cx-m_size.cx,dy=cy-m_size.cy;
	CRect rect;
	m_grid.GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.right+=dx;
	rect.bottom+=dy;
	m_grid.MoveWindow(&rect);
	GetDlgItem(IDC_STATIC2)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.OffsetRect(0,dy);
	GetDlgItem(IDC_STATIC2)->MoveWindow(&rect);
	m_grid1.GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.OffsetRect(0,dy);
	rect.right+=dx;
	m_grid1.MoveWindow(&rect);
	m_size.cx=cx;
	m_size.cy=cy;
}

BOOL CDlgCalStructRes::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_bCreated=TRUE;
	try
	{
		if(m_iComCount<=0)
		{
			CString str;
			str.LoadString(IDS_NORESULT);
			this->SetWindowText(str);
			return TRUE;
		}
		m_pConnPrj.CreateInstance(__uuidof(Connection));
		m_pConnPrj->Open(_bstr_t(::dbConnectionString + basDirectory::ProjectDir + _T("Workprj.mdb")),_T(""),_T(""),adModeUnknown);
		m_rs.CreateInstance(__uuidof(Recordset));
		m_rs->CursorLocation=adUseClient;
		CString strSQL,strFDs,strTemp;
		int i;
		for(i=1;i<=m_iComCount;i++)
		{
			if(i!=1)
				strFDs+=_T(",");
			strTemp.Format(IDS_FDFMT_CALRES,m_pstrType[i-1],i,i,i,m_pnLen[i-1],i);
			strFDs+=strTemp;
		}
		strSQL.Format(IDS_SQL_CALRES,strFDs);
		//AfxMessageBox(strSQL);
		m_rs->Open(_variant_t(strSQL),(IDispatch*)m_pConnPrj,adOpenKeyset,adLockReadOnly,adCmdText);
		m_grid.SetRefDataSource(m_rs->GetDataSource());
		m_grid.GetColumns().GetItem(_variant_t(0L)).SetVisible(FALSE);
		if(!m_rs->adoEOF)
		{
			OnRowColChangeDatagrid1(NULL,0);
		}
		//m_grid.GetColumns().GetItem(_variant_t("ºÏ¸ñ·ñ")).SetRefDataFormat(
	}
	catch(_com_error & e)
	{
		AfxMessageBox(e.Description());
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_EVENTSINK_MAP(CDlgCalStructRes, CDialog)
    //{{AFX_EVENTSINK_MAP(CDlgCalStructRes)
	ON_EVENT(CDlgCalStructRes, IDC_DATAGRID1, 218 /* RowColChange */, OnRowColChangeDatagrid1, VTS_PVARIANT VTS_I2)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CDlgCalStructRes::OnRowColChangeDatagrid1(VARIANT FAR* LastRow, short LastCol) 
{
	// TODO: Add your control notification handler code here
	int iRow=m_grid.GetRow();
	if(iRow==m_igridRow)
		return;
	CString strSQL;
	try
	{
		m_rs1=NULL;
		m_rs1.CreateInstance(__uuidof(Recordset));

		strSQL.Format(IDS_SQL2_CALRES,
			vtoi(m_rs->GetCollect(/*_variant_t(GetResStr(IDS_SELECTMODE))*/_T("SelType"))));
		m_rs1.CreateInstance(__uuidof(Recordset));
		m_rs1->Open(_variant_t(strSQL),(IDispatch*)m_pConnPrj,adOpenKeyset,adLockReadOnly,adCmdText);
		m_grid1.SetRefDataSource(m_rs1->GetDataSource());
	}
	catch(_com_error & e)
	{
		AfxMessageBox(e.Description());
	}
}

int CDlgCalStructRes::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add yur specialized creation code here
	CRect rect;
	GetClientRect(&rect);
	m_size.cx=rect.Width();
	m_size.cy=rect.Height();
	return 0;
}
