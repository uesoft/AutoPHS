// FrmTK.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "FrmTK.h"
#include "user.h"
#include "EDIBgbl.h"
#include "CadPoint.h"
#include "EDIBacad.h"
#include "EDIBDB.h"
#include "basDirectory.h"
#include "FrmDrawInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFrmTK dialog


CFrmTK::CFrmTK(CWnd* pParent /*=NULL*/)
	: CDialog(CFrmTK::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFrmTK)
	m_bSigned = TRUE;
	m_bOSflag = TRUE;
	m_bSubzone = TRUE;
	m_strTkExtRatio = _T("");
	//}}AFX_DATA_INIT
}


void CFrmTK::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFrmTK)
	DDX_Control(pDX, IDC_SPIN1, m_Spin1);
	DDX_Control(pDX, IDC_COMBO1, m_ComboTkSize);
	DDX_Check(pDX, IDC_SIGNED, m_bSigned);
	DDX_Check(pDX, IDC_OSFLAG, m_bOSflag);
	DDX_Check(pDX, IDC_SUBZONE, m_bSubzone);
	DDX_Text(pDX, IDC_TKEXTRATIO, m_strTkExtRatio);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFrmTK, CDialog)
	//{{AFX_MSG_MAP(CFrmTK)
	ON_BN_CLICKED(IDC_SIGNED, OnSigned)
	ON_BN_CLICKED(IDC_OSFLAG, OnOsflag)
	ON_BN_CLICKED(IDC_SUBZONE, OnSubzone)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
	ON_EN_CHANGE(IDC_TKEXTRATIO, OnChangeTkextratio)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, OnDeltaposSpin1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFrmTK message handlers

BOOL CFrmTK::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CRect rc;
	if(user::GetPos("FrmTK",rc))
		this->SetWindowPos(NULL,rc.left,rc.top,0,0,SWP_NOSIZE|SWP_NOZORDER);
	else
		SetWindowCenter(this->m_hWnd);
	LoadItem2TKsize();
	this->CheckRadioButton(IDC_RADIO1,IDC_RADIO2,IDC_RADIO1);
	EDIBgbl::DrawType=0;
	EDIBgbl::Tbhq=this->m_bSigned;
	EDIBgbl::gbTbOS=this->m_bOSflag;
	EDIBgbl::TKsubzone=this->m_bSubzone;
	EDIBgbl::MaxExtRatio=4;
	m_strTkExtRatio.Format(_T("%d/%d"),EDIBgbl::inc,EDIBgbl::MaxExtRatio);
	m_Spin1.SetBase(10);
	m_Spin1.SetRange(0,EDIBgbl::MaxExtRatio*2);
	//m_Spin1.SetBuddy(GetDlgItem(IDC_TKEXTRATIO));
	m_Spin1.SetPos(EDIBgbl::inc);
	UpdateData(false);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFrmTK::LoadItem2TKsize()
{
	int i;
	CString stmp;
	for(i = 0 ; i<= 4;i++)
	{
		stmp.Format(_T("A%d"),i);
      m_ComboTkSize.AddString(stmp);
	}
	EDIBgbl::DrawNoSel = 1;
	m_ComboTkSize.SetCurSel(EDIBgbl::DrawNoSel);
}



void CFrmTK::OnSigned() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	EDIBgbl::Tbhq=this->m_bSigned;
}

void CFrmTK::OnOsflag() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	EDIBgbl::gbTbOS=this->m_bOSflag;
}

void CFrmTK::OnSubzone() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	EDIBgbl::TKsubzone=this->m_bSubzone;
}

void CFrmTK::OnRadio1() 
{
	// TODO: Add your control notification handler code here
	EDIBgbl::DrawType=0;
}

void CFrmTK::OnRadio2() 
{
	// TODO: Add your control notification handler code here
	EDIBgbl::DrawType=1;
}

void CFrmTK::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	user::SavePos(this,"FrmTK");
}

void CFrmTK::OnSelchangeCombo1() 
{
	// TODO: Add your control notification handler code here
	int ix=m_ComboTkSize.GetCurSel();
	if(ix==4)
	{
		this->m_bSigned=FALSE;
		this->CheckRadioButton(IDC_RADIO1,IDC_RADIO2,IDC_RADIO2);
		EDIBgbl::DrawType=1;
		EDIBgbl::Tbhq=false;
		UpdateData(false);
		CheckDlgButton(IDC_SIGNED,BST_UNCHECKED);
		GetDlgItem(IDC_SIGNED)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO1)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_SIGNED)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO1)->EnableWindow(TRUE);
	}
	EDIBgbl::DrawNoSel=ix;
	if(ix % 2 ==0)
	{
		EDIBgbl::MaxExtRatio=8;
	}
	else
		EDIBgbl::MaxExtRatio=4;
	EDIBgbl::inc=0;
	m_strTkExtRatio.Format(_T("%d/%d"),EDIBgbl::inc,EDIBgbl::MaxExtRatio);
	m_Spin1.SetRange(0,EDIBgbl::MaxExtRatio*2);
	m_Spin1.SetPos(0);
	UpdateData(false);
}




void CFrmTK::OnChangeTkextratio() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	CString sTmp;int i;
	sTmp=this->m_strTkExtRatio;
   i = sTmp.Find(_T("/"));
   if(i!=-1)
      //取出加长倍数
		EDIBgbl::inc = _ttoi(sTmp.Left(i));
	else
		EDIBgbl::inc=0;
}





void CFrmTK::OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult) 
{
//	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	try
	{
		int pos=m_Spin1.GetPos();
		//WORD Err=*(((WORD*)&pos)+1);
		WORD ix=(WORD)pos;
		EDIBgbl::inc=ix;
		this->m_strTkExtRatio.Format(_T("%d/%d"),ix,EDIBgbl::MaxExtRatio);
		UpdateData(false);
		*pResult = 0;
	}
	catch(CException *e)
	{
		e->Delete();
	}
	return;
}





void CFrmTK::OnOK() 
{
	// TODO: Add extra validation here
	try
	{
		CFrmDrawInfo dlg;
		dlg.DoModal();
		
		CDialog::OnOK();
	}
	catch(CException *e)
	{
		e->Delete();
	}
}
