// TxsrPagItem.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "TxsrPagItem.h"
#include "FrmTxsr.h"
#include "EDIBAcad.h"
#include "modPHScal.h"
#include "user.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTxsrPagItem property page


CTxsrPagItem::CTxsrPagItem(CWnd* pParentWnd ) : CDialog(CTxsrPagItem::IDD,pParentWnd)
{
	//{{AFX_DATA_INIT(CTxsrPagItem)
	m_PSAver = _T("");
	m_SPRrigid = _T("");
	m_SPRWorks = _T("");
	m_conSPRworks = _T("");
	m_LblPrjName = _T("");
	m_txtCLB1 = _T("");
	m_ConstantSPRNo = _T("");
	//}}AFX_DATA_INIT
}

CTxsrPagItem::~CTxsrPagItem()
{
}

void CTxsrPagItem::DoDataExchange(CDataExchange* pDX)
{
	FrmTxsr.m_pViewTxsr->m_bIsUpd=true;
	CDialog::DoDataExchange(pDX);
	
	//{{AFX_DATA_MAP(CTxsrPagItem)
	DDX_Text(pDX, IDC_LBL_PSAVER, m_PSAver);
	DDX_Text(pDX, IDC_LBL_SPRRIGID, m_SPRrigid);
	DDX_Text(pDX, IDC_LBL_SPRWORKS, m_SPRWorks);
	DDX_Text(pDX, IDC_LBL_CONSPRWORKS2, m_conSPRworks);
	DDX_Text(pDX, IDC_LABPRJ_NAME, m_LblPrjName);
	DDX_Text(pDX, IDC_EDIT1, m_txtCLB1);
	DDX_Text(pDX, IDC_CONSTANTSPRN0, m_ConstantSPRNo);
	//}}AFX_DATA_MAP
	FrmTxsr.m_pViewTxsr->m_bIsUpd=false;
	if( EDIBAcad::g_bCustomPicNo )
	{
		GetDlgItem(IDC_EDIT1)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_EDIT1)->EnableWindow(FALSE);
	}
}


BEGIN_MESSAGE_MAP(CTxsrPagItem, CDialog)
	//{{AFX_MSG_MAP(CTxsrPagItem)
	ON_EN_CHANGE(IDC_EDIT1, OnChangeEDITDwgName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTxsrPagItem message handlers

void CTxsrPagItem::OnOK() 
{
	// TODO: Add extra validation here
	
	//CDialog::OnOK();
}

void CTxsrPagItem::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	//CDialog::OnCancel();
}

void CTxsrPagItem::OnChangeEDITDwgName() 
{
	if( EDIBAcad::g_bCustomPicNo )
	{
		UpdateData();
		m_txtCLB1.TrimLeft(); m_txtCLB1.TrimRight(); 
		if( m_txtCLB1.IsEmpty() )
		{
			FrmTxsr.m_pViewTxsr->m_ActiveRs->PutCollect("CustomDwgName",_variant_t(modPHScal::zdjh));
			m_txtCLB1 = ltos(modPHScal::zdjh);
		}
		else
		{
			FrmTxsr.m_pViewTxsr->m_ActiveRs->PutCollect("CustomDwgName",_variant_t(m_txtCLB1));
		}	
	
		EDIBAcad::g_strCustomPicNoFileName = m_txtCLB1;
		UpdateData(FALSE);
	}
}
