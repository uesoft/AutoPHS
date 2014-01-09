// DlgSARecord.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "DlgSARecord.h"
#include "modPHScal.h"
#include "EDIBgbl.h"
#include "user.h"
#include "DlgAddMaterial.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSARecord dialog


CDlgSARecord::CDlgSARecord(_RecordsetPtr pRs,BOOL bAddNew,int nDH,CString strCustomID,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSARecord::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSARecord)
	m_strName = (_T(""));
	m_strNum = (_T(""));
	m_strLength = (_T(""));
	m_strCustomID = strCustomID;
	m_fPMax = 0.0f;
	m_strSIZEH = (_T(""));
	m_strSIZE2 = (_T(""));
	m_strM = (_T(""));
	m_strL1 = (_T(""));
	m_strCHDIST = (_T(""));
	m_strGDW1 = (_T(""));
	m_strC = (_T(""));
	m_strA = (_T(""));
	m_strTJ = (_T(""));
	m_strPMAXSS100 = (_T(""));
	m_strPMAXSS150 = (_T(""));
	m_strPMAXSF = (_T(""));
	m_strWEIGTH = (_T(""));
	//}}AFX_DATA_INIT
	m_pRs=pRs;
	m_bAddNew=bAddNew;
	m_nDH=nDH;
}


 void CDlgSARecord::DoDataExchange(CDataExchange* pDX)
 {
 	CDialog::DoDataExchange(pDX);
 	//{{AFX_DATA_MAP(CDlgSARecord)
 	DDX_Control(pDX, IDC_LIST1, m_lstMaterial);
 	DDX_Text(pDX, IDC_EDIT1, m_strName);
 	DDX_Text(pDX, IDC_EDIT_NUM, m_strNum);
 	DDX_Text(pDX, IDC_EDIT_Len, m_strLength);
 	DDX_Text(pDX, IDC_EDIT4, m_strCustomID);
 	DDX_Text(pDX, IDC_EDIT5, m_fPMax);
	DDX_Text(pDX, IDC_EDIT2, m_strSIZEH);
	DDX_Text(pDX, IDC_EDIT3, m_strSIZE2);
	DDX_Text(pDX, IDC_EDIT6, m_strM);
	DDX_Text(pDX, IDC_EDIT7, m_strL1);
	DDX_Text(pDX, IDC_EDIT9, m_strCHDIST);
	DDX_Text(pDX, IDC_EDIT8, m_strGDW1);
	DDX_Text(pDX, IDC_EDIT10, m_strC);
	DDX_Text(pDX, IDC_EDIT12, m_strA);
	DDX_Text(pDX, IDC_EDIT11, m_strTJ);
	DDX_Text(pDX, IDC_EDIT33, m_strPMAXSS100);
	DDX_Text(pDX, IDC_EDIT34, m_strPMAXSS150);
	DDX_Text(pDX, IDC_EDIT35, m_strPMAXSF);
	DDX_Text(pDX, IDC_EDIT36, m_strWEIGTH);
	//}}AFX_DATA_MAP
 }


BEGIN_MESSAGE_MAP(CDlgSARecord, CDialog)
	//{{AFX_MSG_MAP(CDlgSARecord)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, OnItemchangedList1)
	ON_BN_CLICKED(IDC_BTN_ADD, OnAddMaterial)
	ON_BN_CLICKED(IDC_BTN_REMOVE, OnBtnRemove)
	ON_EN_CHANGE(IDC_EDIT_NUM, OnChangeEditNum)
	ON_EN_CHANGE(IDC_EDIT_Len, OnChangeEditLen)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST1, OnEndlabeleditList1)
	ON_EN_CHANGE(IDC_EDIT1, OnChangeEdit1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSARecord message handlers

BOOL CDlgSARecord::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	((CEdit*)GetDlgItem(IDC_EDIT4))->SetReadOnly(TRUE);
	m_lstMaterial.SetExtendedStyle(m_lstMaterial.GetExtendedStyle()|LVS_EX_HEADERDRAGDROP|LVS_EX_FULLROWSELECT
		|LVS_EX_GRIDLINES|LVS_EX_ONECLICKACTIVATE);//|LVS_EX_UNDERLINEHOTLVS_EX_TRACKSELECT);//|);
	
	m_lstMaterial.AddColumn(GetResStr(IDS_PARTNAME),0,m_lstMaterial.GetStringWidth(GetResStr(IDS_PARTNAME))+20);

	m_lstMaterial.AddColumn(GetResStr(IDS_PARTAMOUNT),1,m_lstMaterial.GetStringWidth(GetResStr(IDS_PARTAMOUNT))+20);
	
	m_lstMaterial.AddColumn(GetResStr(IDS_PARTLENGTH),2);
	if(!m_bAddNew)
	{
		RefData();
		LoadList();
	}
	InitLab();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSARecord::OnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	RefEditData();
	*pResult = 0;
}

void CDlgSARecord::RefEditData()
{
	UpdateData();
	int ix=m_lstMaterial.GetSelectedItem();
	if(ix!=-1)
	{
		m_strName=m_lstMaterial.GetItemText(ix,0);
		m_strNum=m_lstMaterial.GetItemText(ix,1);
		m_strLength=m_lstMaterial.GetItemText(ix,2);
		UpdateData(FALSE);
	}
	else
	{
		ClearEditData();
	}
}

void CDlgSARecord::ClearEditData()
{
	m_strName=_T("");
	m_strNum=_T("");
	m_strLength=_T("");
	UpdateData(FALSE);
}	

void CDlgSARecord::OnAddMaterial() 
{
	// TODO: Add your control notification handler code here
	m_nNode=m_lstMaterial.GetItemCount()+1;
	if(m_nNode>12)
		//根部最多允许12种附件
		return;
	CDlgAddMaterial dlg(TRUE,m_pRs,m_nNode);
	if(dlg.DoModal()==IDOK)
	{
		m_nNode++;
		int ix=m_lstMaterial.AddItem(m_nNode-2,0,dlg.m_stMaterial.m_strName);
		m_lstMaterial.SetItemData(ix,m_nNode-1);
		m_lstMaterial.AddItem(m_nNode-2,1,dlg.m_stMaterial.m_strNum);
		m_lstMaterial.AddItem(m_nNode-2,2,dlg.m_stMaterial.m_strLength);

		if(m_nNode>12)
			//根部最多允许12种附件
			GetDlgItem(IDC_BTN_ADD)->EnableWindow(FALSE);
		m_lstMaterial.SelectItem(m_nNode-2);
		RefEditData();
	}
	else
	{CString str;
	str.LoadString(IDS_NOAPPENDPART);
		AfxMessageBox(str);
	}
}

//DEL void CDlgSARecord::OnChangeCustomID() 
//DEL {
//DEL 	// TODO: If this is a RICHEDIT control, the control will not
//DEL 	// send this notification unless you override the CDialog::OnInitDialog()
//DEL 	// function and call CRichEditCtrl().SetEventMask()
//DEL 	// with the ENM_CHANGE flag ORed into the mask.
//DEL 	
//DEL 	// TODO: Add your control notification handler code here
//DEL 	static b=true;
//DEL 	if(b)
//DEL 	{
//DEL 		b=false;
//DEL 		CString sTmp;
//DEL 		GetDlgItem(IDC_EDIT4)->GetWindowText(sTmp);
//DEL 		sTmp.MakeUpper();
//DEL 		if(sTmp.GetLength()<3 || sTmp[0]!=_T('G') || sTmp[1]<_T('1') || sTmp[1]>_T('9') || atoi(sTmp.Mid(1))<100)
//DEL 		{
//DEL 		}	
//DEL 		else
//DEL 		{
//DEL 			m_strCustomID.Format(_T("G%d"),atoi(sTmp.Mid(1)));
//DEL 		}
//DEL 		m_strCustomID=_T("G100");
//DEL 		GetDlgItem(IDC_EDIT4)->SetWindowText(m_strCustomID);
//DEL 		((CEdit*)GetDlgItem(IDC_EDIT4))->SetSel(m_strCustomID.GetLength(),m_strCustomID.GetLength());
//DEL 		b=true;
//DEL 	}
//DEL 	else b=true;
//DEL }

void CDlgSARecord::LoadList()
{
	try
	{
		m_lstMaterial.DeleteAllItems();
		if(m_pRs!=NULL && m_pRs->State==adStateOpen && !m_pRs->adoEOF && !m_pRs->BOF )
		{
			CString strFdName,strFdNum,strFdL;
			int i;
			_variant_t vTmp;
			CString strTmp;
			int ix;
			for(i=1;i<=12;i++)
			{
				strFdName.Format(_T("P%d"),i);
				strFdNum.Format(_T("PNUM%d"),i);
				strFdL.Format(_T("PL%d"),i);
				strTmp=vtos(m_pRs->GetCollect(_variant_t(strFdName)));
				if(strTmp!=_T(""))
				{
					ix=m_lstMaterial.AddItem(i-1,0,strTmp);
					m_lstMaterial.SetItemData(ix,i);
					strTmp=vtos(m_pRs->GetCollect(_variant_t(strFdNum)));
					m_lstMaterial.AddItem(i-1,1,strTmp);
					strTmp=vtos(m_pRs->GetCollect(_variant_t(strFdL)));
					m_lstMaterial.AddItem(i-1,2,strTmp);
				}
			}
			if(i>1)
			{
				m_lstMaterial.SelectItem(0);
				RefEditData();
			}
			m_nNode=i;
			UpdateData(FALSE);
		}
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.Description());
	}
}

void CDlgSARecord::OnOK() 
{
	// TODO: Add extra validation here
	//目的:取得列表控件ListCtrl中的根部附件数，与根部记录比较之后，
	//如果有任意一项不同，则加入根部数据表。
	int count=m_lstMaterial.GetItemCount();
	int i,ix;
	CString strFd,strVal;
	CString SQLx;
	UpdateData();
	bool bAllowAlter=true;
	try
	{
		if(m_pRs!=NULL && m_pRs->State==adStateOpen && (m_bAddNew || (!m_pRs->adoEOF && !m_pRs->BOF)) )
		{
			SQLx=_T("SELECT CustomID FROM [") + modPHScal::tbnSA + _T("] WHERE CustomID=\'") + m_strCustomID + _T("\' ");
			_RecordsetPtr rs;	
			rs.CreateInstance(__uuidof(Recordset));

			if(count<=0)			
				bAllowAlter=false;

			for(i=0;i<count;i++)
			{
				ix=m_lstMaterial.GetItemData(i);
				strVal=m_lstMaterial.GetItemText(i,0);
				strFd.Format(_T("P%d"),ix);	
				SQLx+=(_T(" AND ") + strFd + _T("=\'") + strVal + _T("\' "));
				strVal=m_lstMaterial.GetItemText(i,1);
				strFd.Format(_T("PNUM%d"),ix);
				SQLx+=(_T(" AND ") + strFd + _T("=") + strVal );
				strVal=m_lstMaterial.GetItemText(i,2);
				strFd.Format(_T("PL%d"),ix);
				SQLx+=(_T(" AND ") + strFd + _T("=\'") + strVal + _T("\' "));
			}
			//以下从Px(x=count)到P12都是空字段
			for(i=count;i<12;i++)
			{
				//if i=0,then ix=1,因为最小编号附件从P1开始
				ix=i+1;
				strFd.Format(_T("P%d"),ix);	
				SQLx+=(_T(" AND ") + strFd + _T(" is NULL "));
				strFd.Format(_T("PNUM%d"),ix);
				SQLx+=(_T(" AND ") + strFd + _T(" is NULL ") );
				strVal=m_lstMaterial.GetItemText(i,2);
				strFd.Format(_T("PL%d"),ix);
				SQLx+=(_T(" AND ") + strFd + _T(" is NULL ") );
			}
			SQLx+=(_T(" AND PmaxH=") + ftos(m_fPMax));
			if(rs->State != adOpenStatic)
				rs->Close();
			rs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
				adOpenKeyset, adLockOptimistic, adCmdText); 
			if(!rs->BOF || !rs->adoEOF)
			{
				//有记录,不可加入
				if(m_bAddNew)
					bAllowAlter=false;
			}

			if(bAllowAlter)
			{
				if(m_bAddNew)
				{
					m_pRs->AddNew();
					m_pRs->PutCollect(_variant_t(_T("CustomID")),_variant_t(m_strCustomID));
				}
				for(i=0;i<count;i++)
				{
					ix=m_lstMaterial.GetItemData(i);
					strVal=m_lstMaterial.GetItemText(i,0);
					strFd.Format(_T("P%d"),ix);	
					m_pRs->PutCollect(_variant_t(strFd),_variant_t(strVal));
					strVal=m_lstMaterial.GetItemText(i,1);
					strFd.Format(_T("PNUM%d"),ix);
					m_pRs->PutCollect(_variant_t(strFd),_variant_t(strVal));
					strVal=m_lstMaterial.GetItemText(i,2);
					strFd.Format(_T("PL%d"),ix);
					m_pRs->PutCollect(_variant_t(strFd),_variant_t(strVal));				
				}
				m_pRs->PutCollect(_variant_t(_T("PMAXH")),_variant_t((float)m_fPMax));
				_variant_t vNull;
				vNull.ChangeType(VT_NULL);
				m_pRs->PutCollect(_T("SIZEH"),_variant_t(_tcstod(m_strSIZEH,NULL)));
				m_pRs->PutCollect(_T("SIZE2"),m_strSIZE2==_T("") ? vNull : _variant_t(_tcstod(m_strSIZE2,NULL)));
				m_pRs->PutCollect(_T("TJ"),m_strTJ==_T("") ? vNull : _variant_t(_ttol(m_strTJ)));
				m_pRs->PutCollect(_T("C"),m_strC==_T("") ? vNull : _variant_t(_tcstod(m_strC,NULL)));
				m_pRs->PutCollect(_T("A"),m_strA==_T("") ? vNull : _variant_t(_tcstod(m_strA,NULL)));
				m_pRs->PutCollect(_T("WEIGHT"),m_strWEIGTH==_T("") ? vNull : _variant_t(_tcstod(m_strWEIGTH,NULL)));
				m_pRs->PutCollect(_T("PMAXSF"),m_strPMAXSF==_T("") ? vNull : _variant_t(_tcstod(m_strPMAXSF,NULL)));
				m_pRs->PutCollect(_T("PMAXSS100"),m_strPMAXSS100==_T("") ? vNull : _variant_t(_tcstod(m_strPMAXSS100,NULL)));
				m_pRs->PutCollect(_T("PMAXSS150"),m_strPMAXSS150==_T("") ? vNull : _variant_t(_tcstod(m_strPMAXSS150,NULL)));
				m_pRs->PutCollect(_T("L1"),m_strL1==_T("") ? vNull : _variant_t(_tcstod(m_strL1,NULL)));
				m_pRs->PutCollect(_T("M"),m_strM==_T("") ? vNull : _variant_t(_tcstod(m_strM,NULL)));
				m_pRs->PutCollect(_T("GDW1"),m_strGDW1==_T("") ? vNull : _variant_t(_tcstod(m_strGDW1,NULL)));
				m_pRs->PutCollect(_T("CHDIST"),m_strCHDIST==_T("") ? vNull : _variant_t(_tcstod(m_strCHDIST,NULL)));
				if(m_bAddNew)
				{
					m_pRs->PutCollect(_variant_t(_T("DH")),_variant_t((long)m_nDH));
					strVal.Format(_T("%s.%02d"),m_strCustomID,m_nDH);
					m_pRs->PutCollect(_T("BH"),_variant_t(strVal));
				}
				m_pRs->Update();
			}			
		}
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.Description());
	}
	CDialog::OnOK();
}

void CDlgSARecord::OnBtnRemove() 
{
	// TODO: Add your control notification handler code here
	int count=m_lstMaterial.GetItemCount();
	if(count==0)
	{
		m_nNode=1;
		return;
	}
	int ix=m_lstMaterial.GetSelectedItem();
	if(ix==0 && count>1)
		return;
	m_lstMaterial.DeleteItem(ix);
	m_nNode--;
	count=m_lstMaterial.GetItemCount();
	if(count> ix+1)
		m_lstMaterial.SelectItem(ix);
	else if(count>ix-1)
		m_lstMaterial.SelectItem(ix-1);
	RefEditData();
	GetDlgItem(IDC_BTN_ADD)->EnableWindow(TRUE);
}

void CDlgSARecord::OnChangeEditNum() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	long ix;
	CString strTmp;
	((CEdit*)GetDlgItem(IDC_EDIT_NUM))->GetWindowText(strTmp);
	ix=m_lstMaterial.GetSelectedItem();
	if(ix==-1)
		return;
	else
		m_lstMaterial.SetItemText(ix,1,strTmp);
}

void CDlgSARecord::OnChangeEditLen() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	long ix;
	CString strTmp;
	((CEdit*)GetDlgItem(IDC_EDIT_Len))->GetWindowText(strTmp);
	ix=m_lstMaterial.GetSelectedItem();
	if(ix==-1)
		return;
	else
		m_lstMaterial.SetItemText(ix,2,strTmp);	
}

void CDlgSARecord::RefData()
{
	try
	{
		if(m_pRs!=NULL && m_pRs->State==adStateOpen && !m_pRs->adoEOF && !m_pRs->BOF)
		{
			m_strCustomID=vtos(m_pRs->GetCollect(_T("CustomID")));
			m_fPMax=vtof(m_pRs->GetCollect(_T("PMAXH")));

			m_strSIZEH = vtos(m_pRs->GetCollect(_T("SIZEH")));
			m_strSIZE2 = vtos(m_pRs->GetCollect(_T("SIZE2")));
			m_strM = vtos(m_pRs->GetCollect(_T("M")));
			m_strL1 = vtos(m_pRs->GetCollect(_T("L1")));
			m_strCHDIST = vtos(m_pRs->GetCollect(_T("CHDIST")));
			m_strGDW1 = vtos(m_pRs->GetCollect(_T("GDW1")));
			m_strC = vtos(m_pRs->GetCollect(_T("C")));
			m_strA = vtos(m_pRs->GetCollect(_T("A")));
			m_strTJ = vtos(m_pRs->GetCollect(_T("TJ")));
			m_strPMAXSS100 = vtos(m_pRs->GetCollect(_T("PMAXSS100")));
			m_strPMAXSS150 = vtos(m_pRs->GetCollect(_T("PMAXSS150")));
			m_strPMAXSF = vtos(m_pRs->GetCollect(_T("PMAXSF")));
			m_strWEIGTH = vtos(m_pRs->GetCollect(_T("WEIGHT")));
			UpdateData(FALSE);
		}
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.Description());
	}
}

void CDlgSARecord::InitLab()
{
	try
	{
		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(Recordset));
		CString strDesc=_T("Description");
		CString strSQL;
		strSQL.Format(_T("Select %s ,FDName FROM FieldNameSizeVar WHERE ID=\'%s\'"),
			strDesc,modPHScal::sFindID(m_strCustomID));
		rs->Open((_bstr_t)strSQL,_variant_t((IDispatch*)EDIBgbl::dbPHScode,true), 
			adOpenKeyset, adLockOptimistic, adCmdText); 
		if(rs->BOF || rs->adoEOF) return;
		_variant_t vTmp;
		HRESULT hr = S_OK;
		CString strFind;
		strFind = _T("ucase(FDName)=\'SIZEH\'");
		hr = rs->Find((_bstr_t)strFind, 0, adSearchForward);
		if( !rs->adoEOF)
		{
			rs->get_Collect((_variant_t)0L,&vTmp);
			GetDlgItem(IDC_LAB_SIZEH)->SetWindowText(vtos(vTmp));
		}
		strFind = _T("ucase(FDName)=\'SIZE2\'");
		hr = rs->Find((_bstr_t)strFind, 0, adSearchForward);
		if( !rs->adoEOF)
		{
			rs->get_Collect((_variant_t)0L,&vTmp);
			GetDlgItem(IDC_LAB_SIZE2)->SetWindowText(vtos(vTmp));
		}
		strFind = _T("ucase(FDName)=\'M\'");
		hr = rs->Find((_bstr_t)strFind, 0, adSearchForward);
		if( !rs->adoEOF)
		{
			rs->get_Collect((_variant_t)0L,&vTmp);
			GetDlgItem(IDC_LAB_M)->SetWindowText(vtos(vTmp));
		}
		strFind = _T("ucase(FDName)=\'L1\'");
		hr = rs->Find((_bstr_t)strFind, 0, adSearchForward);
		if( !rs->adoEOF)
		{
			rs->get_Collect((_variant_t)0L,&vTmp);
			GetDlgItem(IDC_LAB_L1)->SetWindowText(vtos(vTmp));
		}
		strFind = _T("ucase(FDName)=\'GDW1\'");
		hr = rs->Find((_bstr_t)strFind, 0, adSearchForward);
		if( !rs->adoEOF)
		{
			rs->get_Collect((_variant_t)0L,&vTmp);
			GetDlgItem(IDC_LAB_GDW1)->SetWindowText(vtos(vTmp));
		}
		strFind = _T("ucase(FDName)=\'CHDIST\'");
		hr = rs->Find((_bstr_t)strFind, 0, adSearchForward);
		if( !rs->adoEOF)
		{
			rs->get_Collect((_variant_t)0L,&vTmp);
			GetDlgItem(IDC_LAB_CHDIST)->SetWindowText(vtos(vTmp));
		}
		strFind = _T("ucase(FDName)=\'C\'");
		hr = rs->Find((_bstr_t)strFind, 0, adSearchForward);
		if( !rs->adoEOF)
		{
			rs->get_Collect((_variant_t)0L,&vTmp);
			GetDlgItem(IDC_LAB_C)->SetWindowText(vtos(vTmp));
		}
		strFind = _T("ucase(FDName)=\'A\'");
		hr = rs->Find((_bstr_t)strFind, 0, adSearchForward);
		if( !rs->adoEOF)
		{
			rs->get_Collect((_variant_t)0L,&vTmp);
			GetDlgItem(IDC_LAB_A)->SetWindowText(vtos(vTmp));
		}
		strFind = _T("ucase(FDName)=\'TJ\'");
		hr = rs->Find((_bstr_t)strFind, 0, adSearchForward);
		if( !rs->adoEOF)
		{
			rs->get_Collect((_variant_t)0L,&vTmp);
			GetDlgItem(IDC_LAB_TJ)->SetWindowText(vtos(vTmp));
		}
		strFind = _T("(FDName)=\'PMAXSS100\'");
		hr = rs->Find((_bstr_t)strFind, 0, adSearchForward);
		if( !rs->adoEOF)
		{
			rs->get_Collect((_variant_t)0L,&vTmp);
			GetDlgItem(IDC_LAB_PMAXSS100)->SetWindowText(vtos(vTmp));
		}
		strFind = _T("(FDName)=\'PMAXSS150\'");
		hr = rs->Find((_bstr_t)strFind, 0, adSearchForward);
		if( !rs->adoEOF)
		{
			rs->get_Collect((_variant_t)0L,&vTmp);
			GetDlgItem(IDC_LAB_PMAXSS150)->SetWindowText(vtos(vTmp));
		}
		strFind = _T("(FDName)=\'PMAXSF\'");
		hr = rs->Find((_bstr_t)strFind, 0, adSearchForward);
		if( !rs->adoEOF)
		{
			rs->get_Collect((_variant_t)0L,&vTmp);
			GetDlgItem(IDC_LAB_PMAXSF)->SetWindowText(vtos(vTmp));
		}
		strFind = _T("(FDName)=\'WEIGHT\'");
		hr = rs->Find((_bstr_t)strFind, 0, adSearchForward);
		if( !rs->adoEOF)
		{
			rs->get_Collect((_variant_t)0L,&vTmp);
			GetDlgItem(IDC_LAB_WEIGTH)->SetWindowText(vtos(vTmp));
		}
		rs->Close();
	}
	catch(CException *e)
	{
		e->ReportError();
		e->Delete();
	}
}

void CDlgSARecord::OnEndlabeleditList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	 NMLVDISPINFO *pDispInfo = (NMLVDISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	//if(pDispInfo->
	*pResult = 1;
}

void CDlgSARecord::OnChangeEdit1() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	long ix;
	CString strTmp;
	((CEdit*)GetDlgItem(IDC_EDIT1))->GetWindowText(strTmp);
	ix=m_lstMaterial.GetSelectedItem();
	if(ix==-1)
		return;
	else
		m_lstMaterial.SetItemText(ix,0,strTmp);
}
