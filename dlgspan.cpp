// DlgSpan.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "EDIBgbl.h"
#include "DlgSpan.h"
#include "user.h"
#include "modPHScal.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSpan dialog

CDlgSpan gDlgSpan;
CDlgSpan::CDlgSpan(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSpan::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSpan)
	m_strLMaxR = (_T(""));
	m_strLMaxS = (_T(""));
	m_strMD = (_T(""));
	m_strIsoWeight = (_T("10"));
	m_strQ = (_T(""));
	m_strTemp = (_T(""));
	m_strLMaxRNew = (_T(""));
	m_strLMaxSNew = (_T(""));
	m_strQp = (_T(""));
	m_bHighCrSteel = FALSE;
	m_bPG40 = FALSE;
	//}}AFX_DATA_INIT
	m_iDWItemIndex=-1;
	m_iMediaItemIndex=-1;
	m_iMaterialItemIndex=-1;
	m_iMediaHotIndex=-1;
	m_iDWHotIndex=-1;
	m_iMaterialHotIndex=-1;
	
}


void CDlgSpan::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSpan)
	DDX_Control(pDX, IDC_COMBO_TEMP, m_comboTemp);
	DDX_Control(pDX, IDC_LIST_MEDIA, m_lstMedia);
	DDX_Control(pDX, IDC_LIST_MATERIAL, m_lstMaterial);
	DDX_Control(pDX, IDC_LIST_DW, m_lstDW);
	DDX_Text(pDX, IDC_STATIC_LMAX_R, m_strLMaxR);
	DDX_Text(pDX, IDC_STATIC_LMAX_S, m_strLMaxS);
	DDX_Text(pDX, IDC_EDIT_MEDIADENSITY, m_strMD);
	DDX_Text(pDX, IDC_EDIT_ISOWEIGHT, m_strIsoWeight);
	DDX_Text(pDX, IDC_EDIT_Q, m_strQ);
	DDX_CBStringExact(pDX, IDC_COMBO_TEMP, m_strTemp);
	DDX_Text(pDX, IDC_STATIC_LMAX_RNEW, m_strLMaxRNew);
	DDX_Text(pDX, IDC_STATIC_LMAX_SNEW, m_strLMaxSNew);
	DDX_Text(pDX, IDC_EDIT_Qp, m_strQp);
	DDX_Check(pDX, IDC_CHECK_HIGH_CR, m_bHighCrSteel);
	DDX_Check(pDX, IDC_CHECK_PG40, m_bPG40);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSpan, CDialog)
	//{{AFX_MSG_MAP(CDlgSpan)
	ON_NOTIFY(LVN_HOTTRACK,IDC_LIST_MEDIA, OnHottrackListMedia)
	ON_NOTIFY(LVN_HOTTRACK,IDC_LIST_MATERIAL, OnHottrackListMaterial)
	ON_NOTIFY(LVN_HOTTRACK,IDC_LIST_DW, OnHottrackListDw)
	ON_WM_ACTIVATE()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DW, OnItemchangedListDw)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_MATERIAL, OnItemchangedListMaterial)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_MEDIA, OnItemchangedListMedia)
	ON_EN_CHANGE(IDC_EDIT_MEDIADENSITY, OnChangeEditMediadensity)
	ON_EN_CHANGE(IDC_EDIT_ISOWEIGHT, OnChangeEditIsoweight)
	ON_CBN_EDITCHANGE(IDC_COMBO_TEMP, OnEditchangeComboTemp)
	ON_CBN_SELCHANGE(IDC_COMBO_TEMP, OnSelchangeComboTemp)
	ON_BN_CLICKED(IDC_CHECK_HIGH_CR, OnCheckHighCr)
	ON_BN_CLICKED(IDC_CHECK_PG40, OnCheckPg40)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSpan message handlers

BOOL CDlgSpan::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	LOGFONT fg;
	CFont* tmpFnt=GetDlgItem(IDC_STATIC_LMAX_R)->GetFont();
	tmpFnt->GetLogFont(&fg);
	fg.lfWeight=FW_BOLD;
	fg.lfHeight=-14;
	m_outFont.CreateFontIndirect(&fg);
	GetDlgItem(IDC_STATIC_LMAX_R)->SetFont(&m_outFont);
	GetDlgItem(IDC_STATIC_LMAX_S)->SetFont(&m_outFont);
	GetDlgItem(IDC_STATIC_LMAX_RNEW)->SetFont(&m_outFont);
	GetDlgItem(IDC_STATIC_LMAX_SNEW)->SetFont(&m_outFont);
	m_iDWItemIndex=-1;
	m_iMediaItemIndex=-1;
	m_iMaterialItemIndex=-1;
	
	
	m_lstDW.SetExtendedStyle(m_lstDW.GetExtendedStyle()|LVS_EX_HEADERDRAGDROP|LVS_EX_FULLROWSELECT
		|LVS_EX_GRIDLINES|LVS_EX_ONECLICKACTIVATE|LVS_EX_UNDERLINEHOT);//LVS_EX_TRACKSELECT);
	
	m_lstMaterial.SetExtendedStyle(m_lstMaterial.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_ONECLICKACTIVATE|LVS_EX_UNDERLINEHOT);//LVS_EX_TRACKSELECT);
	m_lstMedia.SetExtendedStyle(m_lstMedia.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_ONECLICKACTIVATE|LVS_EX_UNDERLINEHOT);//LVS_EX_TRACKSELECT);

	m_lstDW.AddColumn( _T("BH"),0,70);
	m_lstDW.AddColumn(_T("DW"),1,40);
	m_lstDW.AddColumn(_T("S"),40);
	m_lstMedia.InsertColumn( 0,_T("S"),LVCFMT_LEFT,80);
	m_lstMaterial.InsertColumn(0,_T("S"),LVCFMT_LEFT,80);
	LoadListDW();
	LoadListMedia();
	LoadListMaterial();
	LoadTemp();
	Cal();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSpan::LoadListDW()
{
	try
	{
		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(Recordset));
		COleVariant vTmp;
//		rs.m_pDatabase=&EDIBgbl::dbMaterial;//20071102 "dbSORT" 改为 "Material"
		CString strSQL;
		strSQL=_T("SELECT * FROM [PipeDiameter] ORDER BY [DW],[S]");
//		rs.Open(dbOpenSnapshot,strSQL);
		rs->Open((_bstr_t)strSQL,_variant_t((IDispatch*)EDIBgbl::dbMaterial,true), 
			adOpenKeyset, adLockOptimistic, adCmdText); 
		int i=0;
		while(!rs->adoEOF)
		{
			rs->get_Collect((_variant_t)(_T("BH")), &vTmp);
			m_lstDW.AddItem(i,0,vtos(vTmp));
			rs->get_Collect((_variant_t)(_T("DW")), &vTmp);
			m_lstDW.AddItem(i,1,vtos(vTmp));
			rs->get_Collect((_variant_t)(_T("S")), &vTmp);
			m_lstDW.AddItem(i,2,vtos(vTmp));
			rs->MoveNext();
			i++;
		}
		rs->Close();
		if(m_lstDW.GetItemCount()>0)
		{
			m_lstDW.SelectItem(0);
			m_iDWItemIndex=0;
			m_iDWHotIndex=0;
		}
	}
	catch(CException *e)
	{
		e->ReportError();
		e->Delete();
	}

}

void CDlgSpan::LoadListMedia()
{
	try
	{
		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(Recordset));
		COleVariant vTmp;
//		rs.m_pDatabase=&EDIBgbl::dbMaterial;//20071102 "dbSORT" 改为 "dbMaterial"
		CString strSQL;
		strSQL=_T("SELECT [Media],[Density] FROM [MediaDensity] ORDER BY [SEQ]");
//		rs.Open(dbOpenSnapshot,strSQL);
		rs->Open((_bstr_t)strSQL,_variant_t((IDispatch*)EDIBgbl::dbMaterial,true), 
			adOpenKeyset, adLockOptimistic, adCmdText); 
		int i=0,ix;
		float * pflt;
		while(!rs->adoEOF)
		{
			rs->get_Collect((_variant_t)0L, &vTmp);
			ix=m_lstMedia.InsertItem(i,vtos(vTmp));
			rs->get_Collect((_variant_t)1L, &vTmp);
			pflt=new float;
			*pflt=vtof(vTmp);
			m_lstMedia.SetItemData(ix,(DWORD)(LPVOID)pflt);
			rs->MoveNext();
			i++;
		}
		rs->Close();
		if(m_lstMedia.GetItemCount()>0)
		{
			m_lstMedia.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
			m_iMediaItemIndex=0;
			m_iMediaHotIndex=0;
		}
	}
	catch(CException *e)
	{
		e->ReportError();
		e->Delete();
	}
}

void CDlgSpan::LoadListMaterial()
{
	try
	{
		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(Recordset));
		COleVariant vTmp;
//		rs.m_pDatabase=&EDIBgbl::dbMaterial;//20071102 "dbSORT" 改为 "dbMaterial"
		CString strSQL;
		strSQL=_T("SELECT DISTINCT [Material] FROM [MechanicalOfMaterialEt] WHERE Material IN ( SELECT DISTINCT Material FROM [MechanicalOfMaterialSIGMAt] ) ");
//		rs.Open(dbOpenSnapshot,strSQL);
		rs->Open((_bstr_t)strSQL,_variant_t((IDispatch*)EDIBgbl::dbMaterial,true), 
			adOpenKeyset, adLockOptimistic, adCmdText); 
		int i=0;
		while(!rs->adoEOF)
		{
			rs->get_Collect((_variant_t)0L, &vTmp);
			m_lstMaterial.InsertItem(i,vtos(vTmp));
			rs->MoveNext();
			i++;
		}
		rs->Close();
		if(m_lstMaterial.GetItemCount()>0)
		{
			m_lstMaterial.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
			m_iMaterialItemIndex=0;
			m_iMaterialHotIndex=0;
		}
	}
	catch(CException *e)
	{
		e->ReportError();
		e->Delete();
	}
}

void CDlgSpan::FreeListMediaData()
{
	int i,c;
	c=m_lstMedia.GetItemCount();
	float * pflt;
	for(i=0;i<c;i++)
	{
		pflt=(float*)(void*)m_lstMedia.GetItemData(i);
		delete pflt;
		m_lstMedia.SetItemData(i,0);
	}
}

void CDlgSpan::OnDestroy() 
{
	m_strLMaxR = (_T(""));
	m_strLMaxS = (_T(""));
	m_strMD = (_T(""));
	m_strLMaxRNew = (_T(""));
	m_strLMaxSNew = (_T(""));
	m_strQp = (_T(""));
	m_bHighCrSteel = FALSE;
	m_bPG40 = FALSE;
	m_iDWItemIndex=-1;
	m_iMediaItemIndex=-1;
	m_iMaterialItemIndex=-1;
	m_iMediaHotIndex=-1;
	m_iDWHotIndex=-1;
	m_iMaterialHotIndex=-1;
	FreeListMediaData();
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	m_outFont.DeleteObject();
}

void CDlgSpan::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnClose();
	this->DestroyWindow();
}

void CDlgSpan::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
	if(IsWindow(this->GetSafeHwnd()))
		this->DestroyWindow();
}

void CDlgSpan::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
	if(IsWindow(this->GetSafeHwnd()))
		this->DestroyWindow();
}

void CDlgSpan::OnActivate(UINT nState, CWnd *pWndOther, BOOL bMinimized)
{
	if(pWndOther==NULL) return;
	if(GetWindowLong(pWndOther->GetSafeHwnd(),GWL_HINSTANCE)!=(long)::AfxGetInstanceHandle())
		return;
	if(nState==WA_INACTIVE)
		this->PostMessage(WM_CLOSE);
}

void CDlgSpan::OnItemchangedListDw(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	int ix=pNMListView->iItem;
	if(ix!=m_iDWItemIndex)
	{
		m_iDWItemIndex=ix;
		m_iDWHotIndex=ix;
		Cal();
	}
	*pResult = 0;
}

void CDlgSpan::OnItemchangedListMaterial(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	int ix=pNMListView->iItem;
	if(ix!=m_iMaterialItemIndex)
	{
		m_iMaterialItemIndex=ix;
		m_iMaterialHotIndex=ix;
		Cal();
	}
	*pResult = 0;
}

void CDlgSpan::OnItemchangedListMedia(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	int ix=pNMListView->iItem;
	//int ix=m_lstMedia.GetNextItem(-1,LVNI_SELECTED);
	if(ix!=m_iMediaItemIndex)
	{
		m_iMediaItemIndex=ix;
		m_iMediaHotIndex=ix;
		float *pflt=(float*)(void*)m_lstMedia.GetItemData(m_iMediaItemIndex);
		if(pflt!=NULL)
			m_strMD.Format(_T("%g"),*pflt);
		else m_strMD=_T("");
		UpdateData(FALSE);
		Cal();
		if(ix==0)
		{
			((CEdit*)GetDlgItem(IDC_EDIT_MEDIADENSITY))->SetReadOnly(FALSE);
		}
		else
			((CEdit*)GetDlgItem(IDC_EDIT_MEDIADENSITY))->SetReadOnly(TRUE);
	}
	*pResult = 0;
}

void CDlgSpan::OnHottrackListMedia(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LISTVIEW *pNMListView=(NM_LISTVIEW*)pNMHDR;
	int ix=pNMListView->iItem;
	//int ix=m_lstMedia.GetNextItem(-1,LVNI_SELECTED);
	*pResult=0;
	if(ix!=m_iMediaHotIndex)
	{
		float *pflt=0;
		if(ix!=-1)
		{
			m_iMediaHotIndex=ix;
			pflt=(float*)(void*)m_lstMedia.GetItemData(ix);
		}
		else if(m_iMediaItemIndex!=-1)
		{
			m_iMediaHotIndex=m_iMediaItemIndex;
			pflt=(float*)(void*)m_lstMedia.GetItemData(m_iMediaItemIndex);
		}
		else return;
		if(pflt!=NULL)
			m_strMD.Format(_T("%g"),*pflt);
		else m_strMD=_T("");
		UpdateData(FALSE);
		Cal();
	}
}
void CDlgSpan::OnHottrackListMaterial(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LISTVIEW *pNMListView=(NM_LISTVIEW*)pNMHDR;
	int ix=pNMListView->iItem;
	if(ix!=m_iMaterialHotIndex)
	{
		if(ix!=-1)
			m_iMaterialHotIndex=ix;
		else
			m_iMaterialHotIndex=m_iMaterialItemIndex;
		CDlgSpan::LoadTemp();
		Cal();
	}
	*pResult=0;
}
void CDlgSpan::OnHottrackListDw(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LISTVIEW *pNMListView=(NM_LISTVIEW*)pNMHDR;
	int ix=pNMListView->iItem;
	if(ix!=m_iDWHotIndex)
	{
		if(ix!=-1)
			m_iDWHotIndex=ix;
		else
			m_iDWHotIndex=m_iDWItemIndex;
		Cal();
	}
	*pResult=0;
}

void CDlgSpan::Cal()
{
	if(m_iMaterialHotIndex==-1 || m_iDWHotIndex==-1)
		return;	
	CString strMaterial;
	strMaterial=m_lstMaterial.GetItemText(m_iMaterialHotIndex,0);
	float fTemp;
	fTemp=_tcstod(m_strTemp,NULL);
	CString strSQL;
	float fS,fDw,fQp,fQw,fQi,fDensity=7850.0,fMediaDensity,fQ;
	float PI=3.1416;
	fMediaDensity=_tcstod(this->m_strMD,NULL);
	fS=_tcstod(m_lstDW.GetItemText(m_iDWHotIndex,2),NULL);
	fDw=_tcstod(m_lstDW.GetItemText(m_iDWHotIndex,1),NULL);
	//fQp:unit=kgf/m;
	//fQw:unit=kgf/m;
	//fQi:unit=kgf/m
	fQp=PI/4 * ( fDw*fDw-(fDw-2*fS)*(fDw-2*fS))*fDensity*1.0e-6;
	fQw=PI/4 * (fDw-2*fS) * (fDw-2*fS)* fMediaDensity * 1.0e-6;
	fQi=_tcstod(this->m_strIsoWeight,NULL);
	fQ=fQp+fQw+fQi;
	m_strQ.Format(_T("%g"),fQ);
	m_strQp.Format(_T("%g"),fQp);
	float fI;
	fI=PI/64 * (pow(fDw,4) - pow(fDw-2*fS,4));
	//单位mm4->cm4
	fI=fI/10000.0;

	float fW;

	fW=PI/32*pow(fDw,3)*(1-pow((fDw-2*fS)/fDw,4));
	//单位mm3->cm3
	fW=fW/1000.0;

	float fSigmat,fSigma100,fSigma20;
	float fEt,fE100,fE20;
	//fEt:unit=kN/mm2
	modPHScal::StressOfMaterial(strMaterial,fTemp,fEt,1,fDensity);
	//fSigmat:unit=kgf/cm2
	modPHScal::StressOfMaterial(strMaterial,fTemp,fSigmat,0);
	
	float LmaxROld,LmaxRNew;
	float LmaxSOld,LmaxSNew;
	//新管规,fQ:unit=kN/m
	LmaxSNew=0.4336*sqrt(fW/(fQ*EDIBgbl::kgf2N/1000));
	LmaxRNew=0.2118*pow((float)(fEt*fI/(fQ*EDIBgbl::kgf2N/1000)),(float)(1.0/4.0));

	float fPHI;//高铬钢0.7碳素钢0.9
	if(m_bHighCrSteel)
		fPHI=0.7;
	else
		fPHI=0.9;

	if(m_bPG40)
		LmaxSOld=2*sqrt(fW*fPHI*(fSigmat/100)/fQ);
	else
		LmaxSOld=2.24*sqrt(fW*fPHI*(fSigmat/100)/fQ);//Pg<=40
	
	LmaxROld=0.0241*pow((float)(fEt*1000/EDIBgbl::kgf2N*100)*fI/fQ,(float)(1.0/3.0));
	//m_iGS=this->GetCheckedRadioButton(IDC_RADIO_NEW,IDC_RADIO_OLD)-IDC_RADIO_NEW;

	m_strLMaxR.Format(_T("%.2fm"),LmaxROld);
	m_strLMaxS.Format(_T("%.2fm"),LmaxSOld);
	m_strLMaxRNew.Format(_T("%.2fm"),LmaxRNew);
	m_strLMaxSNew.Format(_T("%.2fm"),LmaxSNew);

	UpdateData(FALSE);
}

void CDlgSpan::LoadTemp()
{
	try
	{
		if(m_iMaterialHotIndex==-1 || m_iDWHotIndex==-1)
			return;
		UpdateData();
		CString strMaterial;
		strMaterial=m_lstMaterial.GetItemText(m_iMaterialHotIndex,0);

		m_comboTemp.ResetContent();
		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(Recordset));
		COleVariant vTmp;
		CString strSQL;
		strSQL=_T("SELECT DISTINCT [t] FROM [MechanicalOfMaterialEt] WHERE material=\'") + strMaterial + _T("\'ORDER BY [t]"); 
// 		rs.m_pDatabase=&EDIBgbl::dbMaterial;//20071102 "dbSORT" 改为 ""
// 		rs.Open(dbOpenSnapshot,strSQL);
		rs->Open((_bstr_t)strSQL,_variant_t((IDispatch*)EDIBgbl::dbMaterial,true), 
			adOpenKeyset, adLockOptimistic, adCmdText); 
		while(!rs->adoEOF)
		{
			rs->get_Collect((_variant_t)0L, &vTmp);
			m_comboTemp.AddString(vtos(vTmp));
			rs->MoveNext();
		}
		rs->Close();
		if(m_comboTemp.GetCount()>0)
			m_comboTemp.SetCurSel(0);
	}
	catch(CException *e)
	{
		e->ReportError();
		e->Delete();
	}
}

void CDlgSpan::OnChangeEditMediadensity() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	((CEdit*)GetDlgItem(IDC_EDIT_MEDIADENSITY))->GetWindowText(m_strMD);
	if(m_iMediaItemIndex==0)
	{
		float * pflt=(float*)(void*)m_lstMedia.GetItemData(0);
		*pflt=_tcstod(m_strMD,NULL);
	}
	Cal();
}

void CDlgSpan::OnChangeEditIsoweight() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	((CEdit*)GetDlgItem(IDC_EDIT_ISOWEIGHT))->GetWindowText(m_strIsoWeight);
	Cal();
}

void CDlgSpan::OnEditchangeComboTemp() 
{
	// TODO: Add your control notification handler code here
	m_comboTemp.GetWindowText(m_strTemp);
	//m_comboTemp.SetWindowText(m_strTemp);
	Cal();
}

void CDlgSpan::OnSelchangeComboTemp() 
{
	// TODO: Add your control notification handler code here
	CString strTmp;
	m_comboTemp.GetLBText(m_comboTemp.GetCurSel(),m_strTemp);
	m_comboTemp.SetWindowText(m_strTemp);
	Cal();
}

void CDlgSpan::OnCheckHighCr() 
{
	// TODO: Add your control notification handler code here
	m_bHighCrSteel=(!m_bHighCrSteel);
 	Cal();	
}

void CDlgSpan::OnCheckPg40() 
{
	// TODO: Add your control notification handler code here	
 	m_bPG40=(!m_bPG40);
	UpdateData(false);
 	Cal();	
}
