// InputBox.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "InputBox.h"
#include "user.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInputBox dialog


CInputBox::CInputBox(CWnd* pParent /*=NULL*/)
	: CDialog(CInputBox::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInputBox)
	m_strValue = _T("");
	m_strTitle = _T("");
	//}}AFX_DATA_INIT
	m_bIsProtected=FALSE;
}


void CInputBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputBox)
	DDX_Text(pDX, IDC_STRVALUE, m_strValue);
	DDX_Text(pDX, IDC_TITLE, m_strTitle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInputBox, CDialog)
	//{{AFX_MSG_MAP(CInputBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputBox message handlers

BOOL CInputBox::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if(m_bIsProtected)
	{
		((CEdit*)GetDlgItem(IDC_STRVALUE))->SetPasswordChar('*');
	}
	if(m_strWndText=="")
	{
		m_strWndText=AfxGetApp()->m_pszAppName;
	}
	this->SetWindowText(m_strWndText);
	SetWindowCenter(this->m_hWnd);
	if(AfxGetApp()->m_pMainWnd!=NULL)
	{
		HICON hIcon=AfxGetApp()->m_pMainWnd->GetIcon(FALSE);
		this->SetIcon(hIcon,FALSE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
