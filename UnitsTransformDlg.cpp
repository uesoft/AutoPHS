// UnitsTransformDlg.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "UnitsTransformDlg.h"
#include "user.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUnitsTransformDlg dialog


CUnitsTransformDlg::CUnitsTransformDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUnitsTransformDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUnitsTransformDlg)
	m_dGene = 1.0;
	m_dOffset = 0.0;
	m_strSourceUnits = _T("");
	m_strDestinationUnits = _T("");
	//}}AFX_DATA_INIT
}


void CUnitsTransformDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUnitsTransformDlg)
	DDX_Control(pDX, IDOK, m_conIdokBut);
	DDX_Control(pDX, IDC_LIST_CONVERSION, m_ListConversion);
	DDX_Text(pDX, IDC_EDIT_GENE, m_dGene);
	DDX_Text(pDX, IDC_EDIT_OFFSET, m_dOffset);
	DDX_Text(pDX, IDC_EDIT_AFTER, m_strSourceUnits);
	DDX_Text(pDX, IDC_EDIT_FORWARD, m_strDestinationUnits);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUnitsTransformDlg, CDialog)
	//{{AFX_MSG_MAP(CUnitsTransformDlg)
	ON_BN_CLICKED(ID_SAVE, OnSave)
	ON_NOTIFY(NM_CLICK, IDC_LIST_CONVERSION, OnClickListConversion)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUnitsTransformDlg message handlers

void CUnitsTransformDlg::OnSave() 
{
	IDispatch *pCon;
	UpdateData(true);
	_variant_t strSQL;
	CString strTmp;
	try
	{
		m_pRsUnits->Filter=_variant_t("Units='"+m_strSourceUnits+"' and UeUnits='"+m_strDestinationUnits+"'");
		if(m_pRsUnits->adoEOF)
		{
			m_pRsUnits->AddNew();
			m_pRsUnits->PutCollect(_T("Ueunits"),_variant_t(m_strDestinationUnits));
			m_pRsUnits->PutCollect(_T("units"),_variant_t(m_strSourceUnits));
		}
		m_pRsUnits->PutCollect(_T("value"),_variant_t(m_dGene));
		m_pRsUnits->PutCollect(_T("offset"),_variant_t(m_dOffset));
		m_pRsUnits->Update();
		m_pRsUnits->Filter="";
		
		if(m_pRsUnits->State==adStateOpen)
		{
			m_pRsUnits->Close();
		}
		//刷新单位转换对应关系记录集(start)
		pCon=m_pRsUnits->GetActiveConnection();
		//清空原来记经显示的内容
		m_ListConversion.DeleteAllItems();
		strSQL=m_pRsUnits->GetSource();
		m_pRsUnits->Open(_variant_t(strSQL),(IDispatch*)pCon,adOpenStatic,adLockOptimistic,adCmdText);
		if(m_pRsUnits&&m_pRsUnits->State==adStateOpen)
		{
			if((!m_pRsUnits->adoEOF)&&(!m_pRsUnits->BOF))
			{
				m_pRsUnits->MoveFirst();
				int i=0;//用来记录当前表项的序号
				while(!m_pRsUnits->adoEOF)
				{
					m_ListConversion.InsertItem(i,vtos(m_pRsUnits->GetCollect("Units")));
					m_ListConversion.SetItemText(i,1,vtos(m_pRsUnits->GetCollect("UeUnits")));
					strTmp.Format("%f",vtof(m_pRsUnits->GetCollect("Value")));
					m_ListConversion.SetItemText(i,2,strTmp);
					strTmp.Format("%f",vtof(m_pRsUnits->GetCollect("offset")));
					m_ListConversion.SetItemText(i,3,strTmp);
					m_pRsUnits->MoveNext();
					i++;
				}
			}
		}
		//刷新单位转换对应关系记录集(end)
	}
	catch(_com_error e)
	{
		CString strErrorMsg;
		strErrorMsg.Format(_T("%s: %d, %s"), __FILE__, __LINE__, e.Description());
		AfxMessageBox(strErrorMsg);
	}
	m_conIdokBut.EnableWindow(true);//保存数据后按钮可用
	UpdateData(false);
}

void CUnitsTransformDlg::OnOK() 
{
	CDialog::OnOK();
}

void CUnitsTransformDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CUnitsTransformDlg::OnClickListConversion(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateData(TRUE);
	int i=m_ListConversion.GetSelectionMark();
	if(i>=0)
	{
		m_ListConversion.SetHotItem(i);
		m_strSourceUnits=m_ListConversion.GetItemText(i,0);
		m_strDestinationUnits=m_ListConversion.GetItemText(i,1);
		m_dOffset=atof(m_ListConversion.GetItemText(i,3));
		m_dGene=atof(m_ListConversion.GetItemText(i,2));
	}
	UpdateData(false);
}

BOOL CUnitsTransformDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	//确定列名
	CString strTmp;
	m_conIdokBut.EnableWindow(false);//当第一次出现对话框时，OK按钮不能用必须先按“保存”
	m_ListConversion.InsertColumn(0,"转换前的单位");
	m_ListConversion.InsertColumn(1,"转换后的单位");
	m_ListConversion.InsertColumn(2,"转换单位因子");
	m_ListConversion.InsertColumn(3,"偏移值");
	RECT rect;
	//取得List_Control对象的宽度并进行分配
	m_ListConversion.GetWindowRect(&rect);
	int iwidth=rect.right-rect.left;
	m_ListConversion.SetColumnWidth(0,(iwidth/100)*28);
	m_ListConversion.SetColumnWidth(1,(iwidth/100)*28);
	m_ListConversion.SetColumnWidth(2,(iwidth/100)*28);
	m_ListConversion.SetColumnWidth(3,(iwidth/100)*28);
	//为CListCTR添加网格
	DWORD styles = m_ListConversion.GetExtendedStyle();
	m_ListConversion.SetExtendedStyle(styles|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	try
	{
		if(m_pRsUnits&&m_pRsUnits->State==adStateOpen)
		{
			if((!m_pRsUnits->adoEOF)&&(!m_pRsUnits->BOF))
			{
				m_pRsUnits->MoveFirst();
//				char buf[20];
				int i=0;//用来记录当前表项的序号
				while(!m_pRsUnits->adoEOF)
				{
					m_ListConversion.InsertItem(i,vtos(m_pRsUnits->GetCollect("Units")));
					m_ListConversion.SetItemText(i,1,vtos(m_pRsUnits->GetCollect("UeUnits")));
					strTmp.Format("%f",vtof(m_pRsUnits->GetCollect("Value")));
					m_ListConversion.SetItemText(i,2,strTmp);
					strTmp.Format("%f",vtof(m_pRsUnits->GetCollect("offset")));
					m_ListConversion.SetItemText(i,3,strTmp);
					m_pRsUnits->MoveNext();
					i++;
				}

			}
		}
	}
	catch(_com_error e)
	{
		CString strErrorMsg;
		strErrorMsg.Format(_T("%s: %d, %s"), __FILE__, __LINE__, e.Description());
		AfxMessageBox(strErrorMsg);
		return false;
	}
	return true;
}
