// MessageBoxEx.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "MessageBoxEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMessageBoxEx dialog


CMessageBoxEx::CMessageBoxEx(CWnd* pParent /*=NULL*/)
	: CDialog(CMessageBoxEx::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMessageBoxEx)
	m_strPrompt = _T("");
	//}}AFX_DATA_INIT
}


void CMessageBoxEx::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMessageBoxEx)
	DDX_Text(pDX, IDC_STATIC_PROMPT, m_strPrompt);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMessageBoxEx, CDialog)
	//{{AFX_MSG_MAP(CMessageBoxEx)
	ON_BN_CLICKED(IDALLYES, OnAllYes)
	ON_BN_CLICKED(IDALLNO, OnAllNo)
	ON_BN_CLICKED(IDYES, OnYes)
	ON_BN_CLICKED(IDNO, OnNo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMessageBoxEx message handlers

void CMessageBoxEx::OnAllYes() 
{
	// TODO: Add your control notification handler code here
	EndDialog(IDALLYES);
}

void CMessageBoxEx::OnAllNo() 
{
	// TODO: Add your control notification handler code here
	EndDialog(IDALLNO);
}

BOOL CMessageBoxEx::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if(m_strTitle=="")
		m_strTitle=::AfxGetApp()->m_pszAppName;
	this->SetWindowText(m_strTitle);
	m_strPrompt="    " + m_strPrompt;
	UpdateData(false);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMessageBoxEx::OnYes() 
{
	// TODO: Add your control notification handler code here
	EndDialog(IDYES);
}

void CMessageBoxEx::OnNo() 
{
	// TODO: Add your control notification handler code here
	EndDialog(IDNO);
}
