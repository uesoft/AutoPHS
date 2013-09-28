// DlgOutDescOpt.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "DlgOutDescOpt.h"

#include "EDIBgbl.h"
#include "modRegistry.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgOutDescOption dialog


CDlgOutDescOption::CDlgOutDescOption(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgOutDescOption::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgOutDescOption)
	//}}AFX_DATA_INIT
}


void CDlgOutDescOption::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgOutDescOption)
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_CHECK_PHSPICTURE, EDIBgbl::g_bPhsPicture);
	DDX_Check(pDX, IDC_CHECK_PHSSETNUM, EDIBgbl::g_bPhsSetNum);
	DDX_Check(pDX, IDC_CHECK_PHSMANUAL, EDIBgbl::g_bPhsManual);
///	DDX_Check(pDX, IDC_CHECK_SYSDESC, EDIBgbl::g_bSysDesc);
	DDX_Check(pDX, IDC_CHECK_USERCUSTOM, EDIBgbl::g_bUserDesc);
}


BEGIN_MESSAGE_MAP(CDlgOutDescOption, CDialog)
	//{{AFX_MSG_MAP(CDlgOutDescOption)
	ON_BN_CLICKED(IDC_CHECK_PHSMANUAL, OnCheckPhsmanual)
	ON_BN_CLICKED(IDC_CHECK_PHSSETNUM, OnCheckPhssetnum)
	ON_BN_CLICKED(IDC_CHECK_USERCUSTOM, OnCheckUsercustom)
	ON_BN_CLICKED(IDC_CHECK_PHSPICTURE, OnCheckPhspicture)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgOutDescOption message handlers

void CDlgOutDescOption::OnCheckPhsmanual() 
{
	UpdateData();	
}

void CDlgOutDescOption::OnCheckPhssetnum() 
{
	UpdateData();	
}

void CDlgOutDescOption::OnCheckUsercustom() 
{
	UpdateData();
}

void CDlgOutDescOption::OnOK() 
{
	UpdateData();

    SetRegValue(_T("Settings"),	_T("Custom_PhsManual"), EDIBgbl::g_bPhsManual);
	SetRegValue(_T("Settings"),	_T("Custom_PhsPicture"),EDIBgbl::g_bPhsPicture);
	SetRegValue(_T("Settings"),	_T("Custom_PhsSetNum"), EDIBgbl::g_bPhsSetNum);
//	SetRegValue(_T("Settings"),	_T("Custom_SysDesc"),   EDIBgbl::g_bSysDesc);
	SetRegValue(_T("Settings"),	_T("Custom_UserDesc"),  EDIBgbl::g_bUserDesc);


	CDialog::OnOK();
}

BOOL CDlgOutDescOption::OnInitDialog() 
{
	CDialog::OnInitDialog();

    EDIBgbl::g_bPhsManual=  GetRegKey(_T("Settings"),_T("Custom_PhsManual"), EDIBgbl::g_bPhsManual);
	EDIBgbl::g_bPhsPicture= GetRegKey(_T("Settings"),_T("Custom_PhsPicture"),EDIBgbl::g_bPhsPicture);
	EDIBgbl::g_bPhsSetNum=  GetRegKey(_T("Settings"),_T("Custom_PhsSetNum"), EDIBgbl::g_bPhsSetNum);
//	EDIBgbl::g_bSysDesc=    GetRegKey(_T("Settings"),_T("Custom_SysDesc"),   EDIBgbl::g_bSysDesc);
	EDIBgbl::g_bUserDesc=   GetRegKey(_T("Settings"),_T("Custom_UserDesc"),  EDIBgbl::g_bUserDesc);	

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgOutDescOption::OnCheckPhspicture() 
{
	UpdateData();	
}
