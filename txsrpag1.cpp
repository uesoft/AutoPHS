// TxsrPag1.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "TxsrPag1.h"
#include "FrmTxsr.h"
#include "modPHScal.h"
#include "edibgbl.h"
#include "user.h"
#include "fstream.h"
#include "basDirectory.h"
#include "modregistry.h"
#include "DlgOutDescOpt.h"
#include "PreCalPag.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//#include "d:\PROGRAM FILES\MICROSOFT VISUAL STUDIO\VC98\MFC\SRC\DLGFLOAT.CPP"
/////////////////////////////////////////////////////////////////////////////
// CTxsrPag1 dialog

CTxsrPag1::CTxsrPag1(CWnd* pParentWnd )
	: CDialog(CTxsrPag1::IDD,pParentWnd)
{
	//{{AFX_DATA_INIT(CTxsrPag1)
	m_fHAZ = 0.0f;
	m_fPAZ1 = 0.0f;
	m_fPAZ0 = 0.0f;
	m_fPGZ1 = 0.0f;
	m_fPJG1 = 0.0f;
	m_labPaz = _T("");
	m_labPaz0 = _T("");
	m_labPgz = _T("");
	m_labPjg = _T("");
	m_strUserDesc = _T("");
	m_bsys = FALSE;
	m_buser = FALSE;
	m_bcustom = FALSE;
	//}}AFX_DATA_INIT
	m_bOnlySys = false;
	m_bOnlyUser = false;
	m_bCustomDesc = false;
}


void CTxsrPag1::DoDataExchange(CDataExchange* pDX)
{
	//目的：FrmTxsr.m_pViewTxsr->m_bIsUpd=true 防止输入框为空时多次执行 FrmTxsr.m_pViewTxsr->OnActive() 而进入本循环体，产生致命错误

	FrmTxsr.m_pViewTxsr->m_bIsUpd=true;
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TXSR_PAG1_EDIT1, modPHScal::gsPhsArrangementDwgNo);
	DDX_Text(pDX, IDC_TXSR_PAG1_EDIT2, modPHScal::gsPhsScheduleDwgNo);
	//{{AFX_DATA_MAP(CTxsrPag1)
	DDX_Control(pDX, IDC_BTN_USER_OUTOPTION, m_btnCustomDesc);
	DDX_Control(pDX, IDC_CHECK_CUSTOMDESC, m_checkCustomDesc);
	DDX_Control(pDX, IDC_CHECK_ONLYUSER, m_btnOnlyUser);
	DDX_Control(pDX, IDC_CHECK_ONLYSYS, m_btnOnlySys);
	DDX_Control(pDX, IDC_EDIT_USERDESC, m_editUserDesc);
	DDX_Text(pDX, IDC_TXSR_PAG1_EDIT3, m_fHAZ);
	DDX_Text(pDX, IDC_TXSR_PAG1_EDIT4, m_fPAZ1);
	DDX_Text(pDX, IDC_TXSR_PAG1_EDIT5, m_fPAZ0);
	DDX_Text(pDX, IDC_TXSR_PAG1_EDIT6, m_fPGZ1);
	DDX_Text(pDX, IDC_TXSR_PAG1_EDIT7, m_fPJG1);
	DDX_Text(pDX, IDC_LABEL_PAZ, m_labPaz);
	DDX_Text(pDX, IDC_LABEL_PAZ0, m_labPaz0);
	DDX_Text(pDX, IDC_LABEL_PGZ, m_labPgz);
	DDX_Text(pDX, IDC_LABEL_PJG, m_labPjg);
	DDX_Text(pDX, IDC_EDIT_USERDESC, m_strUserDesc);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_CHECK_ONLYSYS, EDIBgbl::g_bOnlySysDesc);
	DDX_Check(pDX, IDC_CHECK_ONLYUSER, EDIBgbl::g_bOnlyUserDesc);
	DDX_Check(pDX, IDC_CHECK_CUSTOMDESC, EDIBgbl::g_bCustomDesc);
	FrmTxsr.m_pViewTxsr->m_bIsUpd=false;
}


BEGIN_MESSAGE_MAP(CTxsrPag1, CDialog)
	//{{AFX_MSG_MAP(CTxsrPag1)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_CHECK_ONLYSYS, OnCheckOnlysys)
	ON_BN_CLICKED(IDC_CHECK_ONLYUSER, OnCheckOnlyuser)
	ON_EN_CHANGE(IDC_EDIT_USERDESC, OnChangeEditUserdesc)
	ON_EN_KILLFOCUS(IDC_EDIT_USERDESC, OnKillfocusEditUserdesc)
	ON_BN_CLICKED(IDC_BTN_USER_OUTOPTION, OnBtnUserOutoption)
	ON_BN_CLICKED(IDC_CHECK_CUSTOMDESC, OnCheckCustomdesc)
	ON_MESSAGE( WM_SENDMESSAGE, OnChangeMessage )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTxsrPag1 message handlers

 HBRUSH CTxsrPag1::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
 {
 	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
 	
 	// TODO: Change any attributes of the DC here
 	
 	// TODO: Return a different brush if the default is not desired
 	//return hbr;
 	/*static CBrush br;
 	LOGBRUSH logbr;
 	logbr.lbStyle=BS_SOLID;
 	if(nCtlColor==CTLCOLOR_DLG || nCtlColor==CTLCOLOR_STATIC)
 	{
 		logbr.lbColor=0x0000FFFF;
 		pDC->SetBkColor(0x0000FFFF);
 		logbr.lbStyle=BS_SOLID;
 	}
 	else 
 		return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
 	br.DeleteObject();
 	br.CreateBrushIndirect(&logbr);
 	return (HBRUSH)br;*/
	return hbr;
 }

LRESULT CTxsrPag1::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	/*if(message==WM_COMMAND && (wParam==2 || wParam==1))
	{
		return TRUE;
	}*/	
	return CDialog::WindowProc(message, wParam, lParam);
}

void CTxsrPag1::OnOK() 
{
	// TODO: Add extra validation here
	
	//CDialog::OnOK();
}

void CTxsrPag1::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	//CDialog::OnCancel();
}

void CTxsrPag1::OnCheckOnlysys() 
{

	if( m_btnOnlySys.GetCheck()==1 )
	{
		m_bCustomDesc = EDIBgbl::g_bCustomDesc=false;
		if ( EDIBgbl::g_bOnlyUserDesc )
		{
			m_editUserDesc.EnableWindow(TRUE);
		}
		else 
		{
			m_editUserDesc.EnableWindow(FALSE);
		}
	}
	else
	{
		if ( EDIBgbl::g_bOnlyUserDesc )
			m_editUserDesc.EnableWindow(TRUE);
		else 
			m_editUserDesc.EnableWindow(FALSE);
	}
	UpdateData();
	SetRegValue(_T("Settings"),_T("OnlySysDesc"),EDIBgbl::g_bOnlySysDesc);
}


BOOL CTxsrPag1::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CString strLine;

	user::AddDirSep( basDirectory::ProjectDir);
	ifstream ifile(basDirectory::ProjectDir+"UserDesc.txt");

    if( ifile.is_open() )
	{
		while( !ifile.eof() )
		{
			ifile.getline(strLine.GetBuffer(512),512);strLine.ReleaseBuffer();
			m_strUserDesc += strLine+"\r\n";
		}
		ifile.close();
	}

	EDIBgbl::g_bOnlyUserDesc = GetRegKey(_T("Settings"),_T("OnlyUserDesc"),FALSE);

	EDIBgbl::g_bOnlySysDesc = GetRegKey(_T("Settings"),_T("OnlySysDesc"),TRUE);

	EDIBgbl::g_bCustomDesc = GetRegKey(_T("Settings"),_T("CustomDesc"),FALSE);

	if( !EDIBgbl::g_bCustomDesc )
		m_btnCustomDesc.EnableWindow(FALSE);
	if( EDIBgbl::g_bCustomDesc )
	{
		//如果选择了“自定义注释”则应该读出其自定义设置		
		EDIBgbl::g_bPhsManual=  GetRegKey(_T("Settings"),_T("Custom_PhsManual"), EDIBgbl::g_bPhsManual);
		EDIBgbl::g_bPhsPicture= GetRegKey(_T("Settings"),_T("Custom_PhsPicture"),EDIBgbl::g_bPhsPicture);
		EDIBgbl::g_bPhsSetNum=  GetRegKey(_T("Settings"),_T("Custom_PhsSetNum"), EDIBgbl::g_bPhsSetNum);
		EDIBgbl::g_bUserDesc=   GetRegKey(_T("Settings"),_T("Custom_UserDesc"),  EDIBgbl::g_bUserDesc);	

		EDIBgbl::g_bOnlySysDesc=false;
		m_btnOnlySys.SetCheck(0);
		EDIBgbl::g_bOnlyUserDesc=false;
		m_btnOnlyUser.SetCheck(0);

		m_btnCustomDesc.EnableWindow(TRUE);
		m_btnOnlySys.EnableWindow(FALSE);
		m_btnOnlyUser.EnableWindow(FALSE);
		m_editUserDesc.EnableWindow(FALSE);
	}
	if( EDIBgbl::g_bOnlyUserDesc && EDIBgbl::g_bCustomDesc==FALSE )
		m_btnOnlyUser.SetCheck(1);
	if( EDIBgbl::g_bOnlySysDesc && EDIBgbl::g_bCustomDesc==FALSE )
		m_btnOnlySys.SetCheck(1);

	if( EDIBgbl::g_bCustomDesc )
		m_editUserDesc.EnableWindow(FALSE);
	else if ( EDIBgbl::g_bOnlyUserDesc && EDIBgbl::g_bCustomDesc==FALSE )
		m_editUserDesc.EnableWindow(TRUE);
	else
		m_editUserDesc.EnableWindow(FALSE);


	if(gbHotStatus2Zero) //选中 "热态吊零"
	{
		GetDlgItem(IDC_TXSR_PAG1_EDIT4)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_TXSR_PAG1_EDIT6)->EnableWindow(FALSE);
	}

	UpdateData(FALSE);
	return TRUE; 
}

void CTxsrPag1::OnChangeEditUserdesc() 
{
    UpdateData();
}

void CTxsrPag1::OnKillfocusEditUserdesc() 
{
	UpdateData(FALSE);
	user::AddDirSep( basDirectory::ProjectDir);
	ofstream ofile(basDirectory::ProjectDir+"UserDesc.txt");
	ofile<<m_strUserDesc;
	ofile.close();
}

void CTxsrPag1::OnBtnUserOutoption() 
{
	CDlgOutDescOption dlgOption;
	if( dlgOption.DoModal() == IDOK )
	{
		//保存选项
		//清除‘只系统’和‘只用户’两个状态
	}

}

void CTxsrPag1::OnCheckCustomdesc() 
{
	if( m_checkCustomDesc.GetCheck()==1 )
	{
		m_btnCustomDesc.EnableWindow(TRUE);
		m_editUserDesc.EnableWindow(FALSE);
		EDIBgbl::g_bOnlySysDesc=m_bOnlySys = false;
		m_btnOnlySys.SetCheck(0);
		EDIBgbl::g_bOnlyUserDesc=m_bOnlyUser=false;
		m_btnOnlyUser.SetCheck(0);

		m_btnOnlySys.EnableWindow(FALSE);
		m_btnOnlyUser.EnableWindow(FALSE);
	}
	else
	{
		m_btnCustomDesc.EnableWindow(FALSE);
		m_btnOnlySys.EnableWindow(TRUE);
		m_btnOnlyUser.EnableWindow(TRUE);
		if( EDIBgbl::g_bOnlyUserDesc )
			m_editUserDesc.EnableWindow(TRUE);
		else
			m_editUserDesc.EnableWindow(FALSE);
	}
	UpdateData();
	SetRegValue(_T("Settings"),_T("CustomDesc"),EDIBgbl::g_bCustomDesc);
}

void CTxsrPag1::OnCheckOnlyuser() 
{
	if( m_btnOnlyUser.GetCheck()==1 )
	{
		m_bCustomDesc = EDIBgbl::g_bCustomDesc=false;
		if( m_checkCustomDesc.GetCheck() == 1 )
		{
			m_editUserDesc.EnableWindow(FALSE);
		}
		else
		{
			m_editUserDesc.EnableWindow(TRUE);
		}
	}
	else
	{
		m_editUserDesc.EnableWindow(FALSE);
	}
	UpdateData();
	SetRegValue(_T("Settings"),_T("OnlyUserDesc"),EDIBgbl::g_bOnlyUserDesc);
}


LRESULT CTxsrPag1::OnChangeMessage(WPARAM wParam,LPARAM lParam)
{
	if( wParam == 1 ) //选中 "热态吊零"
	{
		GetDlgItem(IDC_TXSR_PAG1_EDIT4)->EnableWindow(FALSE);
		GetDlgItem(IDC_TXSR_PAG1_EDIT6)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_TXSR_PAG1_EDIT4)->EnableWindow(TRUE);
		GetDlgItem(IDC_TXSR_PAG1_EDIT6)->EnableWindow(FALSE);
	}
	return 1;
}	



BOOL CTxsrPag1::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	BOOL bRet;
	CWnd *pParent=NULL;

	pParent=GetParent();

	bRet=CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
	if(!bRet)
	{
		if(pParent)
		{
			return pParent->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
		}
	}

	return bRet;
}
