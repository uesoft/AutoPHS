// DlgAddMaterial.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "DlgAddMaterial.h"
#include "EDIBgbl.h"
#include "user.h"
#include "modPHScal.h"
#include "FrmTxsr.h"
//#include _T("ADORecordset.h")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgAddMaterial dialog


CDlgAddMaterial::CDlgAddMaterial(BOOL bIsSA,_RecordsetPtr pRs,int nNode,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAddMaterial::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAddMaterial)
	m_fLength = 1000.0f;
	m_iNum = 1;
	m_fWeight = 0.0f;
	m_fPerMeterWeight = 0.0f;
	m_fTotalWeight = 0.0f;
	m_fWidth = 100.0f;
	m_fThickness = 10.0f;
	m_strMaterial = (_T(""));
	m_strInpCustomID = (_T(""));
	//}}AFX_DATA_INIT
	m_pstrDescID=NULL;
	m_piClassIndex=NULL;
	m_pstrClassTbn=NULL;
	m_pstrDescCustomID=NULL;
	m_pfBHWeight=NULL;
	m_pstrBHMaterial=NULL;
	m_iClassIndex=0;

	m_bIsSA=bIsSA;
	m_pRs=pRs;
	m_nNode=nNode;
}


void CDlgAddMaterial::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAddMaterial)
	DDX_Control(pDX, IDC_COMBO4, m_comboMaterial);
	DDX_Control(pDX, IDC_EDIT7, m_editThickness);
	DDX_Control(pDX, IDC_EDIT6, m_editWidth);
	DDX_Control(pDX, IDC_COMBO2, m_comboDesc);
	DDX_Control(pDX, IDC_COMBO1, m_comboClass);
	DDX_Text(pDX, IDC_EDIT1, m_fLength);
	DDX_Text(pDX, IDC_EDIT2, m_iNum);
	DDX_Text(pDX, IDC_EDIT3, m_fWeight);
	DDX_Text(pDX, IDC_EDIT5, m_fPerMeterWeight);
	DDX_Text(pDX, IDC_EDIT4, m_fTotalWeight);
	DDX_Text(pDX, IDC_EDIT6, m_fWidth);
	DDX_Text(pDX, IDC_EDIT7, m_fThickness);
	DDX_Control(pDX, IDC_COMBO3, m_comboBH);
	DDX_Text(pDX, IDC_EDIT8, m_strInpCustomID);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAddMaterial, CDialog)
	//{{AFX_MSG_MAP(CDlgAddMaterial)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeClass)
	ON_CBN_SELCHANGE(IDC_COMBO2, OnSelchangeDesc)
	ON_CBN_SELCHANGE(IDC_COMBO3, OnSelchangeBH)
	ON_EN_CHANGE(IDC_EDIT3, OnChangeWeigth)
	ON_EN_CHANGE(IDC_EDIT2, OnChangeNum)
	ON_EN_CHANGE(IDC_EDIT1, OnChangeLength)
	ON_EN_CHANGE(IDC_EDIT6, OnChangeWidth)
	ON_EN_CHANGE(IDC_EDIT7, OnChangeThickness)
	ON_BN_CLICKED(ID_BTN_ADD, OnBtnAdd)
	ON_CBN_EDITCHANGE(IDC_COMBO2, OnEditchangeDesc)
	ON_CBN_EDITCHANGE(IDC_COMBO3, OnEditchangeBH)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAddMaterial message handlers

BOOL CDlgAddMaterial::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if(m_bIsSA)
	{
		GetDlgItem(IDCANCEL)->SetWindowText(GetResStr(IDS_CANCEL));
		GetDlgItem(ID_BTN_ADD)->SetWindowText(GetResStr(IDS_CONFIRM));
	}
	else
	{
		GetDlgItem(IDCANCEL)->SetWindowText(GetResStr(IDS_CLOSE));
		GetDlgItem(ID_BTN_ADD)->SetWindowText(GetResStr(IDS_APPEND));
	}
	SetWindowCenter(this->GetSafeHwnd());
	LoadListClass();
	if(m_comboClass.GetCount()>0)
		m_comboClass.SetCurSel(0);
	LoadListDesc();
	if(m_comboDesc.GetCount()>0)
		m_comboDesc.SetCurSel(0);
	LoadListBH();
	if(m_comboBH.GetCount()>0)
		m_comboBH.SetCurSel(0);
	LoadMaterialValue();
	LoadListMaterial();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgAddMaterial::LoadListClass()
{
	m_comboClass.ResetContent();
	if(m_piClassIndex)
	{
		delete [] m_piClassIndex;
		m_piClassIndex=NULL;
	}
	if(m_pstrClassTbn)
	{
		delete [] m_pstrClassTbn;
		m_pstrClassTbn=NULL;
	}
	_RecordsetPtr rs;
	rs.CreateInstance(__uuidof(_Recordset));
	try
	{
		CString SQLx;
		//SQLx=_T("SELECT [Index],[IndexName],[tbn] FROM ClassTypeDesc WHERE Index = 3 OR Index=4  OR Index=5  OR Index=2  OR Index=1 OR Index=0 OR Index=6 ORDER BY seq ");
		//不要根部
		if(m_bIsSA)
		{
			if(m_nNode==1)
				SQLx=_T("SELECT [Index],[IndexName],[tbn] FROM ClassTypeDesc WHERE Index = 3 ORDER BY seq ");
			else
				SQLx=_T("SELECT [Index],[IndexName],[tbn] FROM ClassTypeDesc WHERE Index = 3 OR Index=4  OR Index=5 OR Index=6 ORDER BY seq ");
		}
		else
		{
			SQLx=_T("SELECT [Index],[IndexName],[tbn] FROM ClassTypeDesc WHERE Index = 3 OR Index=4  OR Index=5  OR Index=2  OR Index=1 OR Index=6 ORDER BY seq ");
		}
// 		rs.m_pDatabase=&EDIBgbl::dbSORT;
// 		rs.Open(dbOpenSnapshot,SQLx);
		rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
			adOpenForwardOnly, adLockReadOnly, adCmdText); 
		int i;
		int count=rs->RecordCount;
		m_piClassIndex=new long[count];
		m_pstrClassTbn=new CString[count];
		i=0;
		CString sTmp;
		COleVariant vTmp;
		while(!rs->adoEOF)
		{
			rs->get_Collect((_variant_t)0L, &vTmp);
			m_piClassIndex[i]=vtoi(vTmp);
			
			rs->get_Collect((_variant_t)1L, &vTmp);
			m_comboClass.AddString(vtos(vTmp));

			rs->get_Collect((_variant_t)2L, &vTmp);
			m_pstrClassTbn[i]=vtos(vTmp);
			rs->MoveNext();
			i++;
		}
	}
	catch(CException *e)
	{
		e->ReportError();
		e->Delete();
	}	
}

void CDlgAddMaterial::LoadListDesc()
{
	m_comboDesc.ResetContent();
	if(m_pstrDescCustomID)
	{
		delete [] m_pstrDescCustomID;
		m_pstrDescCustomID=NULL;
	}
	if(m_pstrDescID)
	{
		delete [] m_pstrDescID;
		m_pstrDescID=NULL;
	}
	CString strClass,strTbn;
	long iClassIndex;
	int ix=m_comboClass.GetCurSel();
	if(ix==-1)
		return;
	m_comboClass.GetLBText(ix,strClass);
	iClassIndex=m_piClassIndex[ix];
	switch(iClassIndex)
	{
	case 3: //型钢
		strTbn=modPHScal::tbnSectionSteelID;
		break;
	case 4: //附件
		strTbn=modPHScal::tbnAttachmentID;
		break;
	case 5: //螺栓螺母
		strTbn=modPHScal::tbnBoltsNutsID;
		break;
	case 2: //连接件
		strTbn=modPHScal::tbnPARTID;
		break;
	case 1://管部
		strTbn=modPHScal::tbnPAID;
		break;
	case 0://根部
		strTbn=modPHScal::tbnSAID;
		break;
	case 6://其它
		strTbn=modPHScal::tbnAttachmentID;
		
		break;
	}
	if(iClassIndex==iOtherPART)
	{
		GetDlgItem(IDC_EDIT1)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT8)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LAB_CUSTOMID)->ShowWindow(SW_SHOW);
		//((CEdit*)GetDlgItem(IDC_EDIT2))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT3))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT8))->SetReadOnly(TRUE);
		GetDlgItem(IDC_EDIT4)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT5)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT6)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT7)->EnableWindow(FALSE);
		::ModifyControlStyle(m_comboDesc.m_hWnd,CBS_DROPDOWNLIST,CBS_DROPDOWN);
		::ModifyControlStyle(m_comboBH.m_hWnd,CBS_DROPDOWNLIST,CBS_DROPDOWN);
		//m_comboDesc.ModifyStyle(CBS_DROPDOWNLIST,CBS_DROPDOWN);
	}
	else
	{
		
		GetDlgItem(IDC_EDIT1)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT4)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT5)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT6)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT7)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT8)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LAB_CUSTOMID)->ShowWindow(SW_HIDE);
		//((CEdit*)GetDlgItem(IDC_EDIT2))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT3))->SetReadOnly(FALSE);
		::ModifyControlStyle(m_comboDesc.m_hWnd,CBS_DROPDOWN,CBS_DROPDOWNLIST);
		::ModifyControlStyle(m_comboBH.m_hWnd,CBS_DROPDOWN,CBS_DROPDOWNLIST);
		//m_comboDesc.ModifyStyle(CBS_DROPDOWN,CBS_DROPDOWNLIST);
	}
	m_strDescTbn=strTbn;
	_RecordsetPtr rs;
	rs.CreateInstance(__uuidof(_Recordset));
	try
	{
		CString SQLx;
		if(iClassIndex==iOtherPART)
			SQLx=_T("SELECT [CustomID],[Description],[ID] FROM ") + strTbn + _T(" WHERE ClassID>=900 ORDER BY seq ");
		else
			SQLx=_T("SELECT [CustomID],[Description],[ID] FROM ") + strTbn + _T(" WHERE ClassID<900 ORDER BY seq ");
		
// 		rs.m_pDatabase=&modPHScal::dbZDJcrude;
// 		rs.Open(dbOpenSnapshot,SQLx);
		rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
			adOpenForwardOnly, adLockReadOnly, adCmdText); 
		int count,i;
		if(!rs->BOF && !rs->adoEOF)
		{
			rs->MoveLast();
			rs->MoveFirst();
		}
		count=rs->RecordCount;
		m_pstrDescCustomID=new CString[count];
		m_pstrDescID=new CString[count];
		i=0;
		while(!rs->adoEOF)
		{
			COleVariant vTmp;
			rs->get_Collect((_variant_t)0L, &vTmp);
			m_pstrDescCustomID[i]=vtos(vTmp);

			rs->get_Collect((_variant_t)1L, &vTmp);
			m_comboDesc.AddString(vtos(vTmp));
			rs->get_Collect((_variant_t)2L, &vTmp);
			m_pstrDescID[i]=vtos(vTmp);
			rs->MoveNext();
			i++;
		}
		rs->Close();
	}
	catch(CException *e)
	{
		e->ReportError();
		e->Delete();
	}
}

void CDlgAddMaterial::LoadListBH()
{
	m_comboBH.ResetContent();
	if(m_pfBHWeight)
	{
		delete [] m_pfBHWeight;
		m_pfBHWeight=NULL;
	}
	if(m_pstrBHMaterial)
	{
		delete [] m_pstrBHMaterial;
		m_pstrBHMaterial=NULL;
	}
	int ix=m_comboDesc.GetCurSel();
	if(ix==-1)
		return;
	CString strCustomID=m_pstrDescCustomID[ix];
	CString strID=m_pstrDescID[ix];
	m_editWidth.EnableWindow(FALSE);
	m_editThickness.EnableWindow(FALSE);
	ix=m_comboClass.GetCurSel();
	if(ix==-1)
		return;
	CString strTbn;

	int iClassIndex=m_piClassIndex[ix];
	switch(iClassIndex)
	{
	case iSectionSteel://3: //型钢
		strTbn=modPHScal::tbnSectionSteel;
		break;
	case iAttachment://4: //附件
		strTbn=modPHScal::tbnAttachment;
		break;
	case iBoltsNuts://5: //螺栓螺母
		strTbn=modPHScal::tbnBoltsNuts;
		break;
	case iConnectPART://2: //连接件
		strTbn=modPHScal::tbnPART;
		break;
	case iPA://1://管部
		strTbn=modPHScal::tbnPA;
		break;
	case iSA://0://根部
		strTbn=modPHScal::tbnSA;
		break;
	case iOtherPART://6://其它
		strTbn=modPHScal::tbnAttachment;
		break;
	}
	if(iClassIndex==iOtherPART)//6
	{
		::ModifyControlStyle(m_comboBH.m_hWnd,CBS_DROPDOWNLIST,CBS_DROPDOWN);
		//m_comboDesc.ModifyStyle(CBS_DROPDOWNLIST,CBS_DROPDOWN);
	}
	else
	{
		::ModifyControlStyle(m_comboBH.m_hWnd,CBS_DROPDOWN,CBS_DROPDOWNLIST);
		//m_comboDesc.ModifyStyle(CBS_DROPDOWN,CBS_DROPDOWNLIST);
	}
	m_iClassIndex=iClassIndex;
	m_strBHTbn=strTbn;
	if(strID==_T("PS"))
	{//钢板
		m_editWidth.EnableWindow(TRUE);
		m_editThickness.EnableWindow(TRUE);
		//m_comboBH.EnableWindow(FALSE);
		//return;
	}
	else 
	{//
		m_editWidth.EnableWindow(FALSE);
		m_editThickness.EnableWindow(FALSE);
		//m_comboBH.EnableWindow(TRUE);
	}
	_RecordsetPtr rs;
	rs.CreateInstance(__uuidof(_Recordset));
	try
	{
		CString SQLx;
		if(iClassIndex==iSA)
			SQLx=_T("SELECT [BH],[WEIGHT] FROM ") + strTbn + _T(" WHERE CustomID=\'") + strCustomID + _T("\'");
		else
		{
			if(iClassIndex==iOtherPART)
			{
				SQLx=_T("SELECT [BH],[WEIGHT],[Material] FROM ") + strTbn + _T(" WHERE CustomID=\'") + strCustomID + _T("\' AND CustomID IN (SELECT CustomID FROM PictureClipData IN \'") + (LPTSTR)(LPCTSTR)EDIBgbl::dbPRJ->DefaultDatabase + _T("\' WHERE ClassID>=900)");
			}
			else
			{
				SQLx=_T("SELECT [BH],[WEIGHT],[Material] FROM [") + strTbn + _T("] AS tmpTb1 WHERE CustomID=\'") + strCustomID + _T("\' AND CustomID IN (SELECT CustomID FROM PictureClipData IN \'") + (LPTSTR)(LPCTSTR)EDIBgbl::dbPRJ->DefaultDatabase + _T("\' WHERE ClassID<900)");
			}			
		}

// 		rs.m_pDatabase=&modPHScal::dbZDJcrude;
// 		rs.Open(dbOpenSnapshot,SQLx);
		rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
			adOpenForwardOnly, adLockReadOnly, adCmdText); 
		int count,i;
		if(!rs->BOF && !rs->adoEOF)
		{
			rs->MoveLast();
			rs->MoveFirst();
		}
		count=rs->RecordCount;
		m_pfBHWeight=new float[count];
		if(iClassIndex!=iSA)
			m_pstrBHMaterial=new CString[count];
		i=0;
		while(!rs->adoEOF)
		{
			CString sTmp;
			COleVariant vTmp;
			rs->get_Collect((_variant_t)0L, &vTmp);
			m_comboBH.AddString(vtos(vTmp));

			rs->get_Collect((_variant_t)1L, &vTmp);
			m_pfBHWeight[i]=vtof(vTmp);

			if(iClassIndex!=iSA)
			{
				rs->get_Collect((_variant_t)2L, &vTmp);
				m_pstrBHMaterial[i]=vtos(vTmp);
			}
			rs->MoveNext();
			i++;
		}
	}
	catch(CException *e)
	{
		e->ReportError();
		e->Delete();
	}	
}

CDlgAddMaterial::~CDlgAddMaterial()
{
	if(m_piClassIndex)
	{
		delete [] m_piClassIndex;
		m_piClassIndex=NULL;
	}
	if(m_pstrClassTbn)
	{
		delete [] m_pstrClassTbn;
		m_pstrClassTbn=NULL;
	}
	if(m_pstrDescCustomID)
	{
		delete [] m_pstrDescCustomID;
		m_pstrDescCustomID=NULL;
	}
	if(m_pfBHWeight)
	{
		delete [] m_pfBHWeight;
		m_pfBHWeight=NULL;
	}
	if(m_pstrBHMaterial)
	{
		delete [] m_pstrBHMaterial;
		m_pstrBHMaterial=NULL;
	}
	if(m_pstrDescID)
	{
		delete [] m_pstrDescID;
		m_pstrDescID=NULL;
	}
}

void CDlgAddMaterial::OnSelchangeClass() 
{
	// TODO: Add your control notification handler code here

	LoadListDesc();
	if(m_comboDesc.GetCount()>0)
		m_comboDesc.SetCurSel(0);
	LoadListBH();
	if(m_comboBH.GetCount()>0)
		m_comboBH.SetCurSel(0);
	LoadMaterialValue();
}

void CDlgAddMaterial::OnSelchangeDesc() 
{
	// TODO: Add your control notification handler code here
	//((CEdit*)GetDlgItem(IDC_EDIT2))->SetReadOnly(TRUE);
	int ix=m_comboClass.GetCurSel();
	if(ix==-1)
		return;
	if(m_piClassIndex[ix]==6)
	{
		((CEdit*)GetDlgItem(IDC_EDIT3))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT8))->SetReadOnly(TRUE);
	}
	LoadListBH();
	if(m_comboBH.GetCount()>0)
		m_comboBH.SetCurSel(0);
	LoadMaterialValue();
}

void CDlgAddMaterial::OnSelchangeBH() 
{
	// TODO: Add your control notification handler code here
	//((CEdit*)GetDlgItem(IDC_EDIT2))->SetReadOnly(TRUE);
	int ix=m_comboClass.GetCurSel();
	if(ix==-1)
		return;
	if(m_piClassIndex[ix]==iOtherPART)
	{
		((CEdit*)GetDlgItem(IDC_EDIT3))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT8))->SetReadOnly(TRUE);
	}
	LoadMaterialValue();
}

void CDlgAddMaterial::LoadMaterialValue()
{
	m_fPerMeterWeight=0;
	int ix=m_comboClass.GetCurSel();
	CString strCustomID;
	CString strID;
	if(ix==-1)
		return;
	int iClassIndex=this->m_piClassIndex[ix];
	if(iClassIndex==iOtherPART)//iOtherPart=6
	{
		ix=m_comboBH.GetCurSel();
		if(ix==-1)
		{
			return;
		}
		this->m_comboMaterial.SetWindowText(m_pstrBHMaterial[ix]);
		this->m_fPerMeterWeight=m_pfBHWeight[ix];
		ix=m_comboDesc.GetCurSel();
		if(ix!=-1)
		{
			this->m_strInpCustomID=this->m_pstrDescCustomID[ix];
		}
		UpdateData(FALSE);
		return;
	}
	ix=m_comboBH.GetCurSel();
	if(ix==-1)
	{
		UpdateData(FALSE);
		return;
	}
	m_fPerMeterWeight=m_pfBHWeight[ix];
	ix=m_comboDesc.GetCurSel();
	if(ix==-1)
		return;
	CString strBH;
	strCustomID=m_pstrDescCustomID[ix];
	strID=m_pstrDescID[ix];
	ix=m_comboBH.GetCurSel();
	if(ix==-1)
		return;
	this->m_comboMaterial.SetWindowText(m_pstrBHMaterial[ix]);
	if(strID==_T("PS"))
	{
		//钢板
		m_comboBH.GetLBText(ix,strBH);
		strBH.TrimLeft();strBH.TrimRight();
		int L,W,T;
		GetLWT(strBH,L,W,T);
		this->m_fLength=(float)L;
		this->m_fWidth=(float)W;
		this->m_fThickness=(float)T;
	}
	UpdateData(FALSE);
	OnChangeLength();
}

void CDlgAddMaterial::OnChangeWeigth() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData();
	m_fTotalWeight=m_iNum*m_fWeight;
	UpdateData(FALSE);
}

void CDlgAddMaterial::OnChangeNum() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData();
	m_fTotalWeight=m_iNum*m_fWeight;
	UpdateData(FALSE);
}

void CDlgAddMaterial::OnChangeLength() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	int ix=m_comboClass.GetCurSel();
	if(ix==-1)
		return;
	int iClassIndex=m_piClassIndex[ix];
	ix=m_comboDesc.GetCurSel();
	if(ix==-1)
		return;
	CString strCustomID=m_pstrDescCustomID[ix];
	CString strID=m_pstrDescID[ix];
	UpdateData();
	if(iClassIndex==iSectionSteel)
	{
		//型钢
		m_fWeight=m_fPerMeterWeight;
		m_fTotalWeight=m_iNum*m_fPerMeterWeight*m_fLength/1000;
	}
	else 
	{//非型钢
		if(strID==_T("L1") || strID==_T("L2") || strID==_T("F9") || strID==_T("F10") )
		{
			//拉杆、单头/双头螺杆
			m_fWeight=m_fPerMeterWeight;
			m_fTotalWeight=m_iNum*m_fPerMeterWeight*m_fLength/1000;
		}
		else if(strID==_T("PS"))
		{
			//钢板
			m_fWeight=m_fLength*m_fWidth*m_fThickness*7850E-9;
			m_fTotalWeight=m_iNum*m_fWeight;
		}
		else
		{
			//其它零件
			m_fWeight=m_fPerMeterWeight;
			m_fTotalWeight=m_iNum*m_fWeight;
		}
	}
	
	UpdateData(FALSE);
}

void CDlgAddMaterial::OnChangeWidth() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	OnChangeLength();
}

void CDlgAddMaterial::OnChangeThickness() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	OnChangeLength();
}

void CDlgAddMaterial::OnBtnAdd() 
{
	// TODO: Add your control notification handler code here
	CString sTmp;
	if(m_bIsSA)
		sTmp.Format(IDS_YESNOADDSAAttachements);
	else
		sTmp.Format(IDS_YESNOADDMATERIAL);
		
	if(m_bIsSA || MessageBox(sTmp,NULL,MB_YESNO)==IDYES)
	{
//		 rsTZB,rsDesc,rsTmp,rsBH;
		_RecordsetPtr rsTZB;
		rsTZB.CreateInstance(__uuidof(_Recordset));
		_RecordsetPtr rsDesc;
		rsDesc.CreateInstance(__uuidof(_Recordset));
		_RecordsetPtr rsTmp;
		rsTmp.CreateInstance(__uuidof(_Recordset));
		_RecordsetPtr rsBH;
		rsBH.CreateInstance(__uuidof(_Recordset));
		CString strSQL;
		try
		{
			COleVariant vTmp;
			CString strTmp,strClgg,strBHFormat;
			UpdateData();
			CString strDesc,strBH,strMaterial;
			int ix=m_comboMaterial.GetCurSel();
			m_comboMaterial.GetWindowText(strMaterial);
			//m_comboMaterial.GetLBText(ix,strMaterial);
			if(strMaterial==_T(""))
			{
				strMaterial=_T("Q235-A");
			}
			ix=m_comboClass.GetCurSel();
			CString strCustomID;
			CString strID;
			if(ix==-1)
				return;
			int iClassIndex=this->m_piClassIndex[ix];
			//将其它类零部件写进原始表tbn和结果表ZB
			if(iClassIndex==iOtherPART)
			{
				int iClassID=900;//其它类(index=iOtherPART=6)用户自定义零件,其ClassID从900开始
				m_comboDesc.GetWindowText(strDesc);
// 				rsDesc.m_pDatabase=&modPHScal::dbZDJcrude;
				m_strInpCustomID.TrimLeft();m_strInpCustomID.TrimRight();
				strSQL.Format(_T("Select * FROM %s WHERE Description=\'%s\' AND ClassID>=900"),m_strDescTbn,strDesc);
// 				rsDesc.Open(AFX_DAO_USE_DEFAULT_TYPE,strSQL);
				rsDesc->Open((_bstr_t)strSQL, _variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
					adOpenForwardOnly, adLockReadOnly, adCmdText); 
				if(rsDesc->adoEOF && rsDesc->BOF)
				{
					strSQL.Format(_T("Select * FROM %s WHERE ID LIKE \'UD*\' AND ClassID>=900 ORDER BY ID "),m_strDescTbn);
					rsDesc->Close();
//					rsDesc.Open(AFX_DAO_USE_DEFAULT_TYPE,strSQL);
					rsDesc->Open((_bstr_t)strSQL, _variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
						adOpenForwardOnly, adLockReadOnly, adCmdText); 
					if(rsDesc->BOF && rsDesc->adoEOF)
						strID=_T("UD0000");
					else
					{
						rsDesc->MoveLast();
						rsDesc->get_Collect((_variant_t)_T("ID"),vTmp);
						strID=vtos(vTmp);
						ix=0;
						while(strID[ix]<_T('0') || strID[ix]>_T('9')) ix++;
						ix=_ttoi(strID.Mid(ix));
						strID.Format(_T("UD%04d"),ix+1);
					}
					if(m_strInpCustomID==_T(""))
						m_strInpCustomID=strID;
// 					rsTmp.m_pDatabase=&EDIBgbl::dbPRJ;
					strSQL.Format(_T("SELECT * FROM PictureClipData WHERE ID=\'%s\'"),strID);
// 					rsTmp.Open(AFX_DAO_USE_DEFAULT_TYPE,strSQL);
					rsTmp->Open((_bstr_t)strSQL, _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
						adOpenForwardOnly, adLockReadOnly, adCmdText); 
					if(rsTmp->adoEOF && rsTmp->BOF)
					{
						rsTmp->AddNew();
						rsTmp->put_Collect((_variant_t)_T("ID"),STR_VAR(strID));
						rsTmp->put_Collect((_variant_t)_T("CustomID"),STR_VAR(m_strInpCustomID));
						rsTmp->put_Collect((_variant_t)_T("Index"),COleVariant((long)6));
						rsTmp->put_Collect((_variant_t)_T("ClassID"),COleVariant((long)iClassID));
						rsTmp->put_Collect((_variant_t)_T("SEQ"),COleVariant((long)(GetMaxSEQ(_T("PictureClipData"),EDIBgbl::dbPRJ)+1)));
						rsTmp->put_Collect((_variant_t)_T("Description"),STR_VAR(strDesc));
						rsTmp->put_Collect((_variant_t)_T("ChineseDescription"),STR_VAR(strDesc));
						rsTmp->Update();
					}
					rsTmp->Close();

					rsDesc->AddNew();
					rsDesc->put_Collect((_variant_t)_T("ID"),STR_VAR(strID));
					rsDesc->put_Collect((_variant_t)_T("CustomID"),STR_VAR(m_strInpCustomID));
					rsDesc->put_Collect((_variant_t)_T("Index"),COleVariant((long)6));
					rsDesc->put_Collect((_variant_t)_T("Description"),STR_VAR(strDesc));
					rsDesc->put_Collect((_variant_t)_T("ChineseDescription"),STR_VAR(strDesc));
					rsDesc->put_Collect((_variant_t)_T("ClassID"),COleVariant((long)iClassID));
					rsDesc->put_Collect((_variant_t)_T("SEQ"),COleVariant((long)(GetMaxSEQ(m_strDescTbn,modPHScal::dbZDJcrude)+1)));
					rsDesc->Update();
					this->LoadListDesc();
					m_comboDesc.SetCurSel(m_comboDesc.GetCount()-1);
				}
				else
				{
					rsDesc->get_Collect((_variant_t)_T("ID"),vTmp);
					strID=vtos(vTmp);
					rsDesc->get_Collect((_variant_t)_T("ClassID"),vTmp);
					iClassID=vtoi(vTmp);
				}
				rsDesc->Close();
				strCustomID=m_strInpCustomID;
				m_comboBH.GetWindowText(strBH);
				strSQL=_T("SELECT * FROM ") + m_strBHTbn + _T(" WHERE CustomID=\'") + strCustomID + _T("\' AND BH=\'") + strBH +_T("\'");
// 				rsBH.m_pDatabase=&modPHScal::dbZDJcrude;
// 				rsBH.Open(AFX_DAO_USE_DEFAULT_TYPE,strSQL);
				rsBH->Open((_bstr_t)strSQL, _variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
					adOpenForwardOnly, adLockReadOnly, adCmdText); 
				if(rsBH->BOF && rsBH->adoEOF)
				{
					rsBH->AddNew();
					rsBH->put_Collect((_variant_t)_T("BH"),STR_VAR(strBH));
					rsBH->put_Collect((_variant_t)_T("CustomID"),STR_VAR(strCustomID));
					rsBH->put_Collect((_variant_t)_T("Material"),STR_VAR(strMaterial));
					rsBH->put_Collect((_variant_t)_T("Weight"),COleVariant((float)m_fWeight));
					rsBH->Update();
				}
				rsBH->Close();
				
				this->LoadListBH();
				m_comboBH.SetCurSel(m_comboBH.FindStringExact(-1,strBH));
			}
			else
			{
				ix=m_comboDesc.GetCurSel();
				if(ix==-1)
				{
					return;
				}
				m_comboDesc.GetLBText(ix,strDesc);
				strCustomID=m_pstrDescCustomID[ix];
				strID=m_pstrDescID[ix];
				if(strID==_T("PS"))
				{
					CString _strBH;
					strBH=ftos(m_fThickness) +_T("x")+ftos(m_fWidth)+_T("x")+ftos(m_fLength);
					_strBH=_T("-") +ftos(m_fLength) +_T("x")+ftos(m_fWidth)+_T("x")+ftos(m_fThickness);

					strSQL=_T("SELECT * FROM ") + m_strBHTbn + _T(" WHERE (BH=\'") + strBH + _T("\' OR BH=\'") + _strBH +_T("\') AND CustomID=\'")
						+ strCustomID + _T("\'");
// 					rsBH.m_pDatabase=&modPHScal::dbZDJcrude;
// 					rsBH.Open(AFX_DAO_USE_DEFAULT_TYPE,strSQL);
					rsBH->Open((_bstr_t)strSQL, _variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
						adOpenForwardOnly, adLockReadOnly, adCmdText); 
					if(rsBH->BOF && rsBH->adoEOF)
					{
						rsBH->AddNew();
						rsBH->put_Collect((_variant_t)_T("BH"),STR_VAR(strBH));
						rsBH->put_Collect((_variant_t)_T("CustomID"),STR_VAR(strCustomID));
						rsBH->put_Collect((_variant_t)_T("Material"),STR_VAR(strMaterial));
						rsBH->put_Collect((_variant_t)_T("Weight"),COleVariant((float)m_fWeight));						
						rsBH->Update();
						
						LoadListBH();
					}
					else
					{
						rsBH->get_Collect((_variant_t)_T("BH"),vTmp);
						strBH=vtos(vTmp);
						rsBH->get_Collect((_variant_t)_T("Material"),vTmp);
						strMaterial=vtos(vTmp);
					}
				}
				else
				{
					ix=m_comboBH.GetCurSel();
					if(ix==-1)
					{
						return;
					}
					m_comboBH.GetLBText(ix,strBH);
					if(m_iClassIndex!=0)
						strMaterial=m_pstrBHMaterial[ix];
				}
				m_strBH=strBH;
				strSQL.Format(_T("Select * FROM %s WHERE ID=\'%s\' AND Description=\'%s\' AND ClassID<900"),m_strDescTbn,strID,strDesc);
// 				rsDesc.m_pDatabase=&modPHScal::dbZDJcrude;
// 				rsDesc.Open(dbOpenSnapshot,strSQL);
				rsDesc->Open((_bstr_t)strSQL, _variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
					adOpenForwardOnly, adLockReadOnly, adCmdText); 
				rsDesc->get_Collect((_variant_t)_T("BHFormat"),vTmp);
				strBHFormat=vtos(vTmp);

				UpdateTmpCSLen();

				strSQL.Format(_T("Select %s FROM tmpCSLen"),strBHFormat);
//				rsTmp.m_pDatabase=&EDIBgbl::dbPRJ;
				try
				{
// 					rsTmp.Open(dbOpenSnapshot,strSQL);
					rsTmp->Open((_bstr_t)strSQL, _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
						adOpenForwardOnly, adLockReadOnly, adCmdText); 
					rsTmp->get_Collect((_variant_t)0L, &vTmp);
					strClgg=vtos(vTmp);
					rsTmp->Close();
				}
				catch(CException *e)
				{
					e->Delete();
					strClgg=strBH;
				}
			}
			if(!m_bIsSA)
			{
				//如果不是在“添加根部的附件”状态
// 				rsTZB.m_pDatabase=&EDIBgbl::dbPRJDB;
				modPHScal::zdjh=vtoi(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("ZDJH")));
				strSQL.Format(_T("SELECT * FROM %s WHERE VolumeID=%d AND ZDJH=%d ORDER BY recno"),
					EDIBgbl::Btype[EDIBgbl::TZB],
					EDIBgbl::SelVlmID,
					modPHScal::zdjh);
// 				rsTZB.Open(AFX_DAO_USE_DEFAULT_TYPE,strSQL);
				rsTZB->Open((_bstr_t)strSQL, _variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
					adOpenForwardOnly, adLockReadOnly, adCmdText); 
				int maxrecno=0;
				if(!rsTZB->BOF && !rsTZB->adoEOF)
				{
					//获得现有零件的最大记录号
					rsTZB->MoveLast();
					rsTZB->get_Collect((_variant_t)_T("recno"),vTmp);
					maxrecno=vtoi(vTmp);
				}

				rsTZB->AddNew();
				rsTZB->put_Collect((_variant_t)_T("VolumeID"),COleVariant((long)EDIBgbl::SelVlmID));
				rsTZB->put_Collect((_variant_t)_T("Zdjh"),COleVariant((long)modPHScal::zdjh));
				rsTZB->put_Collect((_variant_t)_T("nth"),COleVariant((long)1));

				rsTZB->put_Collect((_variant_t)_T("bUserAdd"),COleVariant((short)1));
				//用户添加的材料将不能被作为根部附件处理。因为可能添加管部等
				//这样就不必修改GetPhsSumBom函数。
				rsTZB->put_Collect((_variant_t)_T("IsSAPart"),COleVariant((long)-1));
				rsTZB->put_Collect((_variant_t)_T("Index"),COleVariant((short)iClassIndex));
				//添加的零件recno=最大记录号+1
				rsTZB->put_Collect((_variant_t)_T("recno"),COleVariant((long)(maxrecno+1)));
				rsTZB->put_Collect((_variant_t)_T("ClassID"),modPHScal::sFindFLD(_T("CustomID"),_T("ClassID"),strCustomID));
				rsTZB->put_Collect((_variant_t)_T("CustomID"),STR_VAR(strCustomID));
				rsTZB->put_Collect((_variant_t)_T("ID"),STR_VAR(strID));
				rsTZB->put_Collect((_variant_t)_T("CLmc"),STR_VAR(strDesc));
				rsTZB->put_Collect((_variant_t)_T("clcl"),STR_VAR(strMaterial));
				rsTZB->put_Collect((_variant_t)_T("CLnum"),COleVariant((long)m_iNum));
				rsTZB->put_Collect((_variant_t)_T("Cldz"),COleVariant((float)m_fWeight));
				if(iClassIndex==iOtherPART)
				{
					rsTZB->put_Collect((_variant_t)_T("Clgg"),STR_VAR(strBH));
//					rsTZB->put_Collect((_variant_t)_T("BH"),STR_VAR(strBH));
				}
				else
				{
					rsTZB->put_Collect((_variant_t)_T("Clgg"),STR_VAR(strClgg));
//					rsTZB->put_Collect((_variant_t)_T("BH"),STR_VAR(strClgg));
					rsTZB->put_Collect((_variant_t)_T("L1"),COleVariant((float)m_fLength));
					rsTZB->put_Collect((_variant_t)_T("CLzz"),COleVariant((float)(m_iNum*m_fWeight)));
				}
				rsTZB->Update();
				//添加成功！
				sTmp.Format(IDS_AddMaterialSuccess);
				MessageBox(sTmp);
			}
			else
			{
				//如果是在“添加根部的附件”状态
				m_stMaterial.m_strName=strBH;
				m_stMaterial.m_strNum.Format(_T("%d"),m_iNum);
				m_stMaterial.m_strLength.Format(_T("%g"),m_fLength);
				this->EndDialog(IDOK);
			}
		}
		catch(CException *e)
		{
			e->ReportError();
			e->Delete();
		}
		catch(_com_error e)
		{
			AfxMessageBox(e.Description());
		}
	}
}
void CDlgAddMaterial::UpdateTmpCSLen()
{
	CString strSQL;
	try
	{
		strSQL.Format(_T("UPDATE tmpCSLen Set [BH]=\'%s\'"),m_strBH);
		EDIBgbl::dbPRJ->Execute((_bstr_t)strSQL,NULL,adCmdText);
		strSQL.Format(_T("UPDATE tmpCSLen Set [L1]=%g"),m_fLength);
		EDIBgbl::dbPRJ->Execute((_bstr_t)strSQL,NULL,adCmdText);
		if(!FrmTxsr.m_pViewTxsr->m_ActiveRs->adoEOF && ! FrmTxsr.m_pViewTxsr->m_ActiveRs->BOF)
		{
			modPHScal::gdw1=vtof(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("gdw1")));
			strSQL.Format(_T("UPDATE tmpCSLen Set [GDW1]=%g"),modPHScal::gdw1);
		}
		else
		{
			strSQL.Format(_T("UPDATE tmpCSLen Set [GDW1]=%g"),0.0);
		}

		EDIBgbl::dbPRJ->Execute((_bstr_t)strSQL,NULL,adCmdText);
	}
	catch(CException *e)
	{
		e->ReportError();
		e->Delete();
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.Description());
	}
}

void CDlgAddMaterial::GetLWT(CString strBH, int &L, int &W, int &T)
{
	L=0;W=0;T=0;
	int i;
	if(strBH.GetLength()>0)
	{
		i=0;
		strBH.MakeUpper();
		if(strBH.Left(3)==_T("1/2"))
			strBH=strBH.Mid(4);
		if(strBH[0]==_T('-'))
			strBH=strBH.Mid(1);
		while(strBH[i]<_T('0') || strBH[i]>_T('9')) 
		{
			i++;
		}
		if(strBH[i]!=0)
		{
			strBH=strBH.Mid(i);
			L=_ttoi(strBH);
			i=strBH.Find(_T('X'));
			if(i!=-1)
			{
				strBH=strBH.Mid(i+1);
				W=_ttoi(strBH);
				i=strBH.Find(_T('X'));
				if(i!=-1)
				{
					strBH=strBH.Mid(i+1);
					T=_ttoi(strBH);
				}
			}
		}
	}
	int iTmp;
	if(L<W)
	{
		iTmp=W;
		W=L;
		L=iTmp;
	}
	if(L<T)
	{
		iTmp=T;
		T=L;
		L=iTmp;
	}
	if(W<T)
	{
		iTmp=T;
		T=W;
		W=iTmp;
	}
}

void CDlgAddMaterial::LoadListMaterial()
{
	try
	{
		int ix=m_comboClass.GetCurSel();
		if(ix==-1)
			return;
		int iClassIndex=m_piClassIndex[ix];

		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(_Recordset));
		COleVariant vTmp;
		CString SQLx;
		if(iClassIndex==iPA)
			//管部材料直接从原始表中提取
			SQLx=_T("SELECT DISTINCT Material FROM ") + modPHScal::tbnPA;
		else
			//非管部材料从材料选择规范表提取
			SQLx.Format(_T("SELECT * FROM SpecificationOfMaterial WHERE ClassIndex=%d AND ID=\'default\' AND tmin<=%g AND %g<tmax ORDER BY tmin,tmax,SEQ"),modPHScal::giClassIndex,modPHScal::gnConnectTJ,modPHScal::gnConnectTJ);
		this->m_comboMaterial.ResetContent();
// 		rs.m_pDatabase=&EDIBgbl::dbPHScode;//20071101 "dbSORT" 改为 "dbPHScode"
// 		rs.Open(dbOpenSnapshot,SQLx);
		rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPHScode,true), 
			adOpenForwardOnly, adLockReadOnly, adCmdText); 
		while(!rs->adoEOF)
		{
			rs->get_Collect((_variant_t)_T("Material"), &vTmp);
			m_comboMaterial.AddString(vtos(vTmp));
			rs->MoveNext();
		}
		if(m_comboMaterial.GetCount()>0)
			m_comboMaterial.SetCurSel(0);
	}
	catch(CException *e)
	{
		e->ReportError();
		e->Delete();
	}
}

int CDlgAddMaterial::GetMaxSEQ(CString tbn, _ConnectionPtr &db)
{
	try
	{
//		 rs(&db);
		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(_Recordset));
		CString strSQL;
		strSQL.Format(_T("SELECT MAX(SEQ) FROM %s"),tbn);
// 		rs.Open(dbOpenSnapshot,strSQL);
		rs->Open((_bstr_t)strSQL, _variant_t((IDispatch*)db,true), 
			adOpenForwardOnly, adLockReadOnly, adCmdText); 
		if(rs->adoEOF && rs->BOF)
			return 0;
		COleVariant vTmp;
		rs->get_Collect((_variant_t)0L, &vTmp);
		return vtoi(vTmp);
	}
	catch(CException *e)
	{
		e->ReportError();
		e->Delete();
		return 0;
	}
}

void CDlgAddMaterial::OnEditchangeDesc() 
{
	// TODO: Add your control notification handler code here
	CString strTmp;
	m_comboDesc.GetWindowText(strTmp);
	if(m_comboDesc.FindStringExact(-1,strTmp)!=-1)
	{
		//((CEdit*)GetDlgItem(IDC_EDIT2))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT3))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT8))->SetReadOnly(TRUE);
	}
	else
	{
		//((CEdit*)GetDlgItem(IDC_EDIT2))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT3))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT8))->SetReadOnly(FALSE);
	}
	LoadListBH();
	if(m_comboBH.GetCount()>0)
		m_comboBH.SetCurSel(0);
	LoadMaterialValue();
}

void CDlgAddMaterial::OnEditchangeBH() 
{
	// TODO: Add your control notification handler code here
	CString strTmp;
	m_comboBH.GetWindowText(strTmp);
	if(m_comboBH.FindStringExact(-1,strTmp)!=-1)
	{
		//((CEdit*)GetDlgItem(IDC_EDIT2))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT3))->SetReadOnly(TRUE);

	}
	else
	{
		//((CEdit*)GetDlgItem(IDC_EDIT2))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT3))->SetReadOnly(FALSE);
	}
}

LRESULT CDlgAddMaterial::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(message==WM_COMMAND && (/*wParam==2 || */wParam==1))
	{
		return TRUE;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}
