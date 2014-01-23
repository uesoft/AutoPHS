// PagTxsrOption.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "PagTxsrOption.h"
#include "FrmTxsr.h"
#include "modPHScal.h"
#include "user.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPagTxsrOption property page

CPagTxsrOption::CPagTxsrOption(CWnd* pParentWnd ) : CDialog(CPagTxsrOption::IDD,pParentWnd)
{
	//{{AFX_DATA_INIT(CPagTxsrOption)
	m_iNum = 0;
	m_csDCL1 = _T("");
	m_bifLongVertPipe = FALSE;
	m_bNotSumSA = FALSE;
	m_fPSLength = 0.0f;
	m_fPSThickness = 0.0f;
	m_fPSWidth = 0.0f;
	m_csSAG100No = _T("");
	//}}AFX_DATA_INIT
}

CPagTxsrOption::~CPagTxsrOption()
{
}

void CPagTxsrOption::DoDataExchange(CDataExchange* pDX)
{
	FrmTxsr.m_pViewTxsr->m_bIsUpd=true;
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPagTxsrOption)
	DDX_Control(pDX, IDC_COMBO1, m_comboSAG100);
	DDX_Control(pDX, IDC_BTN_SANUM, m_CmdSAnum);
	DDX_Control(pDX, IDC_COMBO2, m_comboDCL1);
	DDX_Text(pDX, IDC_EDIT_NUM, m_iNum);
	DDX_CBString(pDX, IDC_COMBO2, m_csDCL1);
	DDX_Check(pDX, IDC_CHECK2, m_bifLongVertPipe);
	DDX_Check(pDX, IDC_CHECK1, m_bNotSumSA);
	DDX_Text(pDX, IDC_EDIT_PSLENGTH, m_fPSLength);
	DDX_Text(pDX, IDC_EDIT_PSTHICK, m_fPSThickness);
	DDX_Text(pDX, IDC_EDIT_PSWIDTH, m_fPSWidth);
	DDX_CBString(pDX, IDC_COMBO1, m_csSAG100No);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT_PSTHICK, (float)modPHScal::gfPSThickness);
	DDX_Text(pDX, IDC_EDIT_PSWIDTH, (float)modPHScal::gfPSWidth);
	DDX_Text(pDX, IDC_EDIT_PSLENGTH, (float)modPHScal::gfPSLength);
	FrmTxsr.m_pViewTxsr->m_bIsUpd=false;
}


BEGIN_MESSAGE_MAP(CPagTxsrOption, CDialog)
	//{{AFX_MSG_MAP(CPagTxsrOption)
	ON_BN_CLICKED(IDC_UNIT_KGF, OnUnitKgf)
	ON_BN_CLICKED(IDC_UNIT_N, OnUnitN)
	ON_BN_CLICKED(IDC_PZ_G, OnPzG)
	ON_BN_CLICKED(IDC_PZ_D, OnPzD)
	ON_BN_CLICKED(IDC_BTN_SANUM, OnBtnSanum)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPagTxsrOption message handlers



void CPagTxsrOption::OnUnitKgf() 
{
	// TODO: Add your control notification handler code here
	//CheckRadioButton(IDC_UNIT_KGF,IDC_UNIT_N,IDC_UNIT_KGF);
	FrmTxsr.m_pViewTxsr->UpdateBoundData();
	modPHScal::Unit_Force="kgf";
	modPHScal::Unit_Momental="kgf.m";
	FrmTxsr.m_pViewTxsr->ConvertUnit();
}

void CPagTxsrOption::OnUnitN() 
{
	// TODO: Add your control notification handler code here
	//CheckRadioButton(IDC_UNIT_KGF,IDC_UNIT_N,IDC_UNIT_N);
	FrmTxsr.m_pViewTxsr->UpdateBoundData();
	modPHScal::Unit_Force="N";
	modPHScal::Unit_Momental="N.m";
	FrmTxsr.m_pViewTxsr->ConvertUnit();
}
void CPagTxsrOption::OnPzG() 
{
	// TODO: Add your control notification handler code here
	CheckRadioButton(IDC_PZ_G,IDC_PZ_D,IDC_PZ_G);
	try
	{
		FrmTxsr.m_pViewTxsr->m_ActiveRs->PutCollect("dg1",_variant_t("G"));
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.Description());
	}
}
void CPagTxsrOption::OnPzD() 
{
	// TODO: Add your control notification handler code here
	CheckRadioButton(IDC_PZ_G,IDC_PZ_D,IDC_PZ_D);
	try
	{
		FrmTxsr.m_pViewTxsr->m_ActiveRs->PutCollect("dg1",_variant_t("D"));
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.Description());
	}
}

void CPagTxsrOption::OnBtnSanum() 
{
	// TODO: Add your control notification handler code here
	try
	{
		long iTmp;
		FrmTxsr.m_pViewTxsr->UpdateBoundData();
		if(vtoi(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect("Gnum")) <= 0);
			//glNumSA = 2
		else
			modPHScal::glNumSA = vtoi(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect("Gnum"));

   
		//点击时，根部数量从0循环2
		iTmp = modPHScal::glNumSA;
		if( iTmp >= 2 )
			modPHScal::glNumSA = 0;
		else
			modPHScal::glNumSA = modPHScal::glNumSA + 1;
		
		modPHScal::CalSpringParallelNum();
		//if( modPHScal::gintParallelNum == 1);
			//并联数=1，根部数量必然<=1
			//If glNumSA >= 1 Then
				//glNumSA = 1
			//Else
				//根部数量<=0是不合理的，需要修正这种情况。
				//不一定。如根部材料不统计时的情况
			//End If
		//else
		//{
		//	If glNumSA <= 0 Then
	//         glNumSA = 2
		//	Else
		//		If glNumSA >= 2 Then
	 //           glNumSA = 2
		//		Else
		//		End If
		//	End If
	//	End If
		modPHScal::UpdateTZB4zdjh("Gnum", _variant_t((long)modPHScal::glNumSA));
		FrmTxsr.m_pViewTxsr->DisPlaySANum();
		//gsPartType = trim(data1.recordsetm_ActiveRs->PutCollect("IDgn1"))
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
}



//DEL void CPagTxsrOption::OnChangeEDITSAG100No() 
//DEL {
//DEL 	// TODO: If this is a RICHEDIT control, the control will not
//DEL 	// send this notification unless you override the CDialog::OnInitDialog()
//DEL 	// function and call CRichEditCtrl().SetEventMask()
//DEL 	// with the ENM_CHANGE flag ORed into the mask.
//DEL 	
//DEL 	// TODO: Add your control notification handler code here
//DEL 	CString strTmp;
//DEL 	((CEdit*)GetDlgItem(IDC_EDIT_SAG100NO))->GetWindowText(strTmp);
//DEL 	modPHScal::giSAG100No=atof(strTmp);
//DEL 	if(modPHScal::giSAG100No<=0)
//DEL 	{
//DEL 		modPHScal::giSAG100No=1;
//DEL 		strTmp=1;
//DEL 		((CEdit*)GetDlgItem(IDC_EDIT_SAG100NO))->SetWindowText(strTmp);
//DEL 	}
//DEL }





void CPagTxsrOption::LoadSAG100()
{
	try
	{
		m_comboSAG100.ResetContent();
		CString strSQL;
		_variant_t vTmp;
		strSQL.Format(_T("SELECT [DH] FROM [%s] WHERE [CustomID]=\'G100\'"),modPHScal::tbnSA);
		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(Recordset));
		rs->Open((_bstr_t)strSQL,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
			adOpenStatic, adLockOptimistic, adCmdText); 
		while(!rs->adoEOF)
		{
			rs->get_Collect((_variant_t)0L,&vTmp);
			m_comboSAG100.AddString(vtos(vTmp));
			rs->MoveNext();
		}
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}
}

BOOL CPagTxsrOption::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	LoadSAG100();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPagTxsrOption::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	//CDialog::OnCancel();
}

void CPagTxsrOption::OnOK() 
{
	// TODO: Add extra validation here
	
	//CDialog::OnOK();
}
