// ComboGenDlg.cpp : implementation file
//

#include "stdafx.h"
#include "modPHScal.h"
#include "user.h"
#include "FrmTxsr.h"
#include <math.h>
#include "EDIBgbl.h"
#include "ModEncrypt.h"
#include "cphs.h"
#include "frmselspecification.h"
#include "modregistry.h"
#include "autophs.h"
#include "ComboGenDlg.h"
#include "PhsData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CComboGenDlg dialog


CComboGenDlg::CComboGenDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CComboGenDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CComboGenDlg)
	m_ComboBoxGenVal = _T("");
	//}}AFX_DATA_INIT
}


void CComboGenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CComboGenDlg)
	DDX_Control(pDX, IDC_COMBO, m_ComboBoxGen);
	DDX_CBString(pDX, IDC_COMBO, m_ComboBoxGenVal);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CComboGenDlg, CDialog)
	//{{AFX_MSG_MAP(CComboGenDlg)
	ON_CBN_SELCHANGE(IDC_COMBO, OnSelchangeCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComboGenDlg message handlers

BOOL CComboGenDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	/*m_ComboBoxGen.AddString("hello");
	m_ComboBoxGen.AddString("you");
	m_ComboBoxGen.AddString("bye");*/
	m_bIsVisible=true;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CComboGenDlg::SetVisibleState(BOOL flag)
{
	m_bIsVisible=flag;
}

void CComboGenDlg::OnSelchangeCombo() 
{
	// TODO: Add your control notification handler code here
	UpdateValue(true);
}

void CComboGenDlg::MoveComboBox()
{
	CRect Rect;
	this->GetClientRect(Rect);
	m_ComboBoxGen.MoveWindow(Rect);
}

void CComboGenDlg::UpdateValue(BOOL flag)//flag为false时把值读进combobox，为true时把值写进datagrid
{
	VARIANT Val,Bookmark;
	Val.vt=VT_I2;
	Val.iVal=m_nCol;
	ASSERT(FrmTxsr.m_pViewTxsr->m_ActiveRs != NULL);
	Bookmark=FrmTxsr.m_pViewTxsr->m_ActiveRs->Bookmark;
	FrmTxsr.m_pViewTxsr->m_ActiveRs->Bookmark=((CPhsData *)GetParent())->m_DBGbill.RowBookmark(m_nRow);
	if (flag)
	{
		UpdateData(true);
		((CPhsData *)GetParent())->m_DBGbill.GetColumns().GetItem(Val).SetText(m_ComboBoxGenVal);
	}
	else
	{
		m_ComboBoxGenVal=((CPhsData *)GetParent())->m_DBGbill.GetColumns().GetItem(Val).GetText();
		//m_ComboBoxGen.AddString(m_ComboBoxGenVal);
		UpdateData(false);
	}
	FrmTxsr.m_pViewTxsr->m_ActiveRs->Bookmark=Bookmark;
}

void CComboGenDlg::LoadComboBox(int i)
{
	m_strFilter.Empty();

/**********************************************************************/
	if (EDIBgbl::SelBillType != EDIBgbl::TZA)
		return;
   //选择模板之前必须处理数据，以便获得管径、荷载、温度等必要数据。
	try
	{
		if(FrmTxsr.m_pViewTxsr->m_ActiveRs!=NULL && FrmTxsr.m_pViewTxsr->m_ActiveRs->State!=adStateClosed)
		{
			FrmTxsr.m_pViewTxsr->m_ActiveRs->Update();
			if(FrmTxsr.m_pViewTxsr->m_ActiveRs->adoEOF || FrmTxsr.m_pViewTxsr->m_ActiveRs->BOF)
				return;
			modPHScal::zdjh=vtoi(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("zdjh")));
			modPHScal::iSelSampleID=vtoi(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("iSelSampleID")));
			if(modPHScal::iSelSampleID<=0)
			{
				modPHScal::iSelSampleID=1;
				FrmTxsr.m_pViewTxsr->m_ActiveRs->PutCollect(_T("iSelSampleID"),_variant_t((long)1));
				FrmTxsr.m_pViewTxsr->m_ActiveRs->Update();
			}
			modPHScal::PreCalCrudeData(FrmPhsData.m_DataBillRs);
		}
	}
	catch(_com_error e)
	{
		ShowMessage(e.Description());
	}


/**********************************************************************/
	//按当前支吊架可用管部选择
	//if(m_strFilter!="")
	//	m_strFilter += " AND ";
	//m_strFilter+=" PA = \'" + vtos(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect("IDdn1")) + "\'";

	CString sTmp=vtos(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect("Unit_Force"));
	sTmp.MakeUpper();
	modPHScal::pjg=vtof(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect("pjg1"));
	if(sTmp=="N")
		modPHScal::pjg /= EDIBgbl::kgf2N;
	float tmppjg=fabs(modPHScal::pjg);
	modPHScal::t0=vtof(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect("T01"));
	modPHScal::dj=vtof(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect("dj1"));
	
	if(m_strFilter!="")
		m_strFilter += " AND ";
	//烟风煤粉管道矩形管径=null
	if(modPHScal::bPAIsHanger())
	{
		m_strFilter += "( PA IN (Select ID FROM PictureClipData IN \'" + EDIBgbl::dbPRJ->DefaultDatabase + 
			"\' WHERE CustomID IN ( Select CustomID FROM [" + modPHScal::tbnPA + "] IN \"\" [; DATABASE=" 
			+ (LPTSTR)(LPCTSTR)modPHScal::dbZDJcrude->DefaultDatabase + " ;PWD=" + ModEncrypt::gstrDBZdjCrudePassWord + "]WHERE (Pmax >=" 
			+ ftos(tmppjg) + " AND PictureClipData.ClassID<>" + ltos(iPAfixZ1) + " AND PictureClipData.ClassID<>" + ltos(iPAfixZ2) 
			+") AND (Dw >= " + ftos(modPHScal::dj* (1 - modPHScal::gnDW_delta * 0.01))
			+ " AND Dw <= " + ftos(modPHScal::dj* (1 + modPHScal::gnDW_delta * 0.01)) 
			+ " AND Dw>0 OR Dw IS NULL OR Dw=0) AND tj >= " + ftos(modPHScal::t0) 
			+ " )))";
	}
	else
	{
		m_strFilter += "( PA IN (Select ID FROM PictureClipData IN \'" + EDIBgbl::dbPRJ->DefaultDatabase + 
			"\' WHERE CustomID IN ( Select CustomID FROM [" + modPHScal::tbnPA + "] IN \"\" [; DATABASE=" 
			+ (LPTSTR)(LPCTSTR)modPHScal::dbZDJcrude->DefaultDatabase + " ;PWD=" + ModEncrypt::gstrDBZdjCrudePassWord + "]WHERE (Pmax >=" 
			+ ftos(tmppjg) + " AND PictureClipData.ClassID<>" + ltos(iPAfixZ1) + " AND PictureClipData.ClassID<>" + ltos(iPAfixZ2) 
			+") AND (Dw >= " + ftos(modPHScal::dj* (1 - modPHScal::gnDW_delta * 0.01))
			+ " AND Dw <= " + ftos(modPHScal::dj* (1 + modPHScal::gnDW_delta * 0.01)) 
			+ " AND Dw>0 OR Dw IS NULL OR Dw=0) AND tj >= " + ftos(modPHScal::t0) 
			+ " )) OR PA IN (\'Z1\',\'Z2\',\'Z2A\') )";
	}
/**********************************************************************/
	//按当前支吊架可用根部选择
	//if(m_strFilter!="")
	//	m_strFilter += " AND ";
	//m_strFilter+=" ( SA = \'" + vtos(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect("IDgn1")) + "\' )";

	modPHScal::glNumSA=vtoi(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect("Gnum"));
	modPHScal::gdw1=vtof(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect("gdw1"));
	if(modPHScal::gdw1==0.0) {modPHScal::gdw1;}//pfg128
	if(m_strFilter!="")
		m_strFilter += " AND ";
	int Gnum;			
	Gnum= (modPHScal::glNumSA!=0 ? modPHScal::glNumSA : 1);
	m_strFilter += " ( SA IN ( Select ID FROM PictureClipData IN \'" + EDIBgbl::dbPRJ->DefaultDatabase + "\'"
								+ " WHERE EXISTS ( Select CustomID FROM [" + modPHScal::tbnSA + "] IN \"\" [; DATABASE=" 
								+ (LPTSTR)(LPCTSTR)modPHScal::dbZDJcrude->DefaultDatabase + " ;PWD=" + ModEncrypt::gstrDBZdjCrudePassWord + "] WHERE PictureClipData.CustomID = CustomID AND (PictureClipData.ClassID= " + ltos(iG100) + " OR PictureClipData.ClassID = " + ltos(iSALbraceFixG47) + " OR PictureClipData.ClassID = " + ltos(iSALbraceFixG48) + " OR PMAXH >=" 
								+ ftos(tmppjg / Gnum * (vtob(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect("ifLongVertPipe")) ? Gnum : 1))
								+ " AND (( PictureClipData.ClassID = "
								+ ltos(iSACantilever) + " OR PictureClipData.ClassID = " + ltos(iSALbrace) + " OR PictureClipData.ClassID = " + ltos(iG51) + " OR PictureClipData.ClassID = " + ltos(iG56) + " OR PictureClipData.ClassID = " + ltos(iG57)  + ") AND GDW1 >="
								+ ftos(modPHScal::gdw1) + " OR ( PictureClipData.ClassID =" + ltos(iSAbeam) +" OR PictureClipData.ClassID =" + ltos(iG52_55) + " ) AND GDW1 >="
								+ " IIF("+ ftos(modPHScal::gdw1+(modPHScal::glNumSA==1 ? modPHScal::DblHangerRodDist : 0)/2) +" > L1/2 , L1 - " + ftos(modPHScal::gdw1+(modPHScal::glNumSA == 1 ? modPHScal::DblHangerRodDist : 0) / 2)
								+ " , "+ ftos(modPHScal::gdw1+(modPHScal::glNumSA == 1 ? modPHScal::DblHangerRodDist : 0) / 2) + ") AND L1> " + ftos(modPHScal::Lspan)
								+ "  OR PictureClipData.ClassID= " + ltos(iGCement) + ")) )))";
	
	CString strSQL;
	CComPtr<_Recordset> rs;
	HRESULT hr = S_OK;
	hr = rs.CoCreateInstance(__uuidof(Recordset));
	try
	{
		m_ComboBoxGen.ResetContent();
		UpdateValue(false);
		if (i==1)
			strSQL="SELECT DISTINCT [PA] FROM [phsStructureName]";
		else if (i==2)
			strSQL="SELECT DISTINCT [SA] FROM [phsStructureName]";
		else return;
		if(m_strFilter!="")
			strSQL+=" WHERE " + m_strFilter;
		COleVariant vTmp;
//		rs.m_pDatabase=&EDIBgbl::dbPRJ;
		rs->Open((_bstr_t)strSQL,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
			adOpenForwardOnly, adLockReadOnly, adCmdText); 
		while(!rs->adoEOF)
		{
			rs->get_Collect((_variant_t)0L,vTmp);
			CString sTmp=modPHScal::sFindCustomID(vtos(vTmp));
			if (sTmp!="")
				m_ComboBoxGen.AddString(sTmp);
			rs->MoveNext();
		}
	}
	catch(_com_error *e)
	{
	}
}
