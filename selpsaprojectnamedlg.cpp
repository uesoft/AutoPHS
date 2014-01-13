// SelPSAProjectNameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "SelPSAProjectNameDlg.h"
#include "ReadPSAResult.h"
#include "user.h"
#include "UnitsTransformDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelPSAProjectNameDlg dialog


CSelPSAProjectNameDlg::CSelPSAProjectNameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelPSAProjectNameDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelPSAProjectNameDlg)
//	m_bCheckRigPHSMaxLoad = FALSE;
	m_iSpringLoadType = 0;
	//}}AFX_DATA_INIT
}


void CSelPSAProjectNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelPSAProjectNameDlg)
	DDX_Control(pDX, IDC_LIST3, m_listLGKname);
	DDX_Control(pDX, IDC_LIST2, m_listRGKname);
	DDX_Control(pDX, IDC_LIST1, m_listGKname);
	DDX_Control(pDX, ID_X, m_check);
	DDX_Control(pDX, IDC_LIST_PROJECTNAME, m_listPSAprojectname);
	//DDX_Check(pDX, IDC_CHECK_RIGPHSMAXLOAD, m_bCheckRigPHSMaxLoad);
	DDX_Radio(pDX, IDC_RADIO_SPRINGMAXLOAD, m_iSpringLoadType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelPSAProjectNameDlg, CDialog)
	//{{AFX_MSG_MAP(CSelPSAProjectNameDlg)
	ON_BN_CLICKED(ID_X, OnX)
	ON_BN_CLICKED(ID_Y, OnY)
	ON_BN_CLICKED(ID_Z, OnZ)
	ON_BN_CLICKED(IDCANCLE, OnCancle)
	ON_BN_CLICKED(IDC_RADIO_kgf, OnRADIOkgf)
	ON_BN_CLICKED(IDC_RADIO_N, OnRadioN)
	ON_LBN_SELCHANGE(IDC_LIST_PROJECTNAME, OnSelchangeListProjectname)
	ON_BN_CLICKED(IDC_DrowUnitsBUTTON, OnDrowUnitsBUTTON)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelPSAProjectNameDlg message handlers

void CSelPSAProjectNameDlg::OnOK() 
{
	UpdateData( TRUE );
	// TODO: Add extra validation here
	m_listPSAprojectname.GetText(m_listPSAprojectname.GetCurSel(),m_strTempProject);
	m_listGKname.GetText(m_listGKname.GetCurSel(),m_strTempGK);
	
	m_listRGKname.GetText(m_listRGKname.GetCurSel(),m_strTempRGK);
	m_listLGKname.GetText(m_listLGKname.GetCurSel(),m_strTempLGK);
	

	CDialog::OnOK();
}

BOOL CSelPSAProjectNameDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	try
	{
		int i;
		CString tmpR = "(OPE)";
		CString tmpL = "(SUS)";
		CString strSQL;
		CString strTmp;
		iR=0;
		iL=0;
		//PFG20050705对记录集进行过滤,使它只显示当前工程的工况,默认第一条(start)
		m_pRsJOBNAME->Filter="";
		strSQL.Format("JOBNAME ='%s'",m_pstrProjectName[0]);
		m_pRsJOBNAME->Filter=_variant_t(strSQL);
		GKNum=m_pRsJOBNAME->GetRecordCount();
		if(GKNum>0) m_pRsJOBNAME->MoveFirst();
		//pfg20050705对记录集进行过滤,使它只显示当前工程的工况,默认第一条(end)
		for(i = 0 ; i<JobNameNum ;i++)
			m_listPSAprojectname.AddString(m_pstrProjectName[i]);
		for(i = 0 ; !m_pRsJOBNAME->adoEOF;i++)
		{
			strTmp=vtos(m_pRsJOBNAME->GetCollect("CASE"));
			m_listGKname.AddString(strTmp);
			m_listRGKname.AddString(strTmp);
			m_listLGKname.AddString(strTmp);
			if (strTmp.Find((LPCTSTR)tmpR)!=-1)
				iR = i;
			if (strTmp.Find((LPCTSTR)tmpL)!=-1)
				iL = i;
			m_pRsJOBNAME->MoveNext();
		}
		m_listPSAprojectname.SetCurSel(0);
		m_listGKname.SetCurSel(iL);//pfg20050627
		m_listRGKname.SetCurSel(iR);
		m_listLGKname.SetCurSel(iL);
		this->CheckRadioButton(ID_X,ID_Z,ID_Y);
		this->CheckRadioButton(IDC_RADIO_kgf,IDC_RADIO_N,IDC_RADIO_N);//PFG20050630
		m_ForceUnits="N";
		iUpxyz = 2;
		m_pRsJOBNAME->Filter="";
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.Description());
		return false;
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSelPSAProjectNameDlg::OnX() 
{
	// TODO: Add your control notification handler code here
	iUpxyz = 1;
}

void CSelPSAProjectNameDlg::OnY() 
{
	// TODO: Add your control notification handler code here
	iUpxyz = 2;
}

void CSelPSAProjectNameDlg::OnZ() 
{
	// TODO: Add your control notification handler code here
	iUpxyz =3;
}

void CSelPSAProjectNameDlg::OnCancle() 
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}

void CSelPSAProjectNameDlg::OnRADIOkgf() 
{
	m_ForceUnits="kgf";		
}

void CSelPSAProjectNameDlg::OnRadioN() 
{
	m_ForceUnits="N";	
}

//当选用不同的工程时所显示的工况也要相应的改变pfg20050704
void CSelPSAProjectNameDlg::OnSelchangeListProjectname() 
{
	CString tmpR = "(OPE)";
	CString tmpL = "(SUS)";
	iL=0;
	iR=0;
	CString strTmp;
	try
	{
		m_pRsJOBNAME->Filter="";
		int i;
		//清空工况所对应的控件,进行重写
		m_listGKname.ResetContent();
		m_listRGKname.ResetContent();
		m_listLGKname.ResetContent();
		m_listPSAprojectname.GetText(m_listPSAprojectname.GetCurSel(),m_strTempProject);
		strTmp.Format("jobname='%s'",m_strTempProject);
		//对记录集进行过滤,使它只显示当前工程的工况
		m_pRsJOBNAME->Filter=_variant_t(strTmp);
		GKNum=m_pRsJOBNAME->GetRecordCount();
		if(GKNum>0) m_pRsJOBNAME->MoveFirst();
		for(i = 0 ; !m_pRsJOBNAME->adoEOF;i++)
		{
			strTmp=vtos(m_pRsJOBNAME->GetCollect("CASE"));
			m_listGKname.AddString(strTmp);
			m_listRGKname.AddString(strTmp);
			m_listLGKname.AddString(strTmp);
			if (strTmp.Find((LPCTSTR)tmpR)!=-1)
				iR = i;
			if (strTmp.Find((LPCTSTR)tmpL)!=-1)
				iL = i;
			m_pRsJOBNAME->MoveNext();
		}
		m_listGKname.SetCurSel(iL);//pfg20050627
		m_listRGKname.SetCurSel(iR);
		m_listLGKname.SetCurSel(iL);
		m_pRsJOBNAME->Filter="";
	}
	catch(_com_error e)
	{
	}
}

void CSelPSAProjectNameDlg::OnDrowUnitsBUTTON() 
{
	CString strSQL("");
	_RecordsetPtr pRs=NULL;
	strSQL=_T("select * from UnitsToUeUnits");
	pRs.CreateInstance(__uuidof(Recordset));
	try
	{
		pRs->Open(_variant_t(strSQL),(IDispatch*)conPRJDB,adOpenKeyset,adLockOptimistic,adCmdText);
		CUnitsTransformDlg dlg;
		dlg.m_pRsUnits=pRs;
		dlg.DoModal();
	}
	catch(_com_error e)
	{
	}
}
