// SHDlg.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "SHDlg.h"
#include "PhsData.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSHDlg dialog


CSHDlg::CSHDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSHDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSHDlg)
	//}}AFX_DATA_INIT
}


void CSHDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSHDlg)
	DDX_Control(pDX, IDC_LIST, m_CheckListBox);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSHDlg, CDialog)
	//{{AFX_MSG_MAP(CSHDlg)
	ON_BN_CLICKED(IDC_BTN_DISPLAY, OnBtnDisplay)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSHDlg message handlers
BOOL CSHDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	((CPhsData *)GetParent())->UpdateVector(bIsVisible);
	AddStrings();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSHDlg::AddStrings()
{
	CColumns Columns;
	CColumn Column;
	VARIANT Var;
	short nColCount;
	Columns=((CPhsData *)GetParent())->m_DBGbill.GetColumns();
	nColCount=Columns.GetCount();
	for (short i=0;i<nColCount;i++)
	{
		Var.vt=VT_I2;
		Var.iVal=i;
		Column=Columns.GetItem(Var);
		CString Str;
		Str=Column.GetCaption();
		m_CheckListBox.AddString(Str);
		//m_CheckListBox.SetCheck(i,1);
		m_CheckListBox.SetCheck(i,bIsVisible[i]);
	}
}

void CSHDlg::StrToInt(CString Str, short& result)
{
	int len;
	int i;
	result=0;
	len=Str.GetLength();
	for (i=0;i<len;i++)
	{
		result*=10;
		result+=(Str.GetAt(i)-'0');
	}
}

void CSHDlg::OnBtnDisplay() 
{
	// TODO: Add your control notification handler code here
	int Count=m_CheckListBox.GetCount();
	VARIANT Var;
	CColumns Columns;
	CColumn Column;
	for (short i=0;i<Count;i++)
	{
		if(m_CheckListBox.GetCheck(i))
		{
			Var.vt=VT_I2;
			Var.iVal=i;
			Columns=((CPhsData *)GetParent())->m_DBGbill.GetColumns();
			Column=Columns.GetItem(Var);
			Column.SetVisible(true);
		}
		else
		{
			Var.vt=VT_I2;
			Var.iVal=i;
			Columns=((CPhsData *)GetParent())->m_DBGbill.GetColumns();
			Column=Columns.GetItem(Var);
			Column.SetVisible(false);
		}
	}
	((CPhsData *)GetParent())->UpdateVector(bIsVisible);
}
