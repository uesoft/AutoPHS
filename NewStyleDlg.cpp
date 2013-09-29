// NewStyleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NewStyleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewStyleDlg dialog


CNewStyleDlg::CNewStyleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewStyleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewStyleDlg)
	m_strStyleName = _T("UE_Text_style");
	//}}AFX_DATA_INIT
}


void CNewStyleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewStyleDlg)
	DDX_Text(pDX, IDC_EDIT_STYLENAME, m_strStyleName);
	DDV_MaxChars(pDX, m_strStyleName, 32);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewStyleDlg, CDialog)
	//{{AFX_MSG_MAP(CNewStyleDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewStyleDlg message handlers

void CNewStyleDlg::OnOK() 
{
    UpdateData(TRUE);
    if(m_strStyleName.GetLength() <= 0)
    {
        MessageBox("请输入有效的文字样式名，支持中文");
        return ;
    }
	CDialog::OnOK();
}

void CNewStyleDlg::OnCancel() 
{
    m_strStyleName = "";
	CDialog::OnCancel();
}
