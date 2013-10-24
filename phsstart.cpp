// PhsStart.cpp : implementation file
//

#include "stdafx.h"
#include "AutoPHS.h"
#include "PhsStart.h"
#include "user.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPhsStart dialog

CPhsStart* FrmSplash=NULL;
CPhsStart::CPhsStart(CWnd* pParent /*=NULL*/)
	: CDialog(CPhsStart::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPhsStart)
	m_label1 = _T("");
	m_LabelRegister = _T("");
	m_LabelVersion = _T("");
	//}}AFX_DATA_INIT
}


void CPhsStart::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPhsStart)
	//DDX_Control(pDX, IDC_PIC, m_Pic);
	DDX_Text(pDX, IDC_LAB1, m_label1);
	DDX_Text(pDX, IDC_LABREG, m_LabelRegister);
//	DDX_Text(pDX, IDC_LABVER, m_LabelVersion); //chenbl 2013.05.02 统一改成图片显示版本号
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPhsStart, CDialog)
	//{{AFX_MSG_MAP(CPhsStart)
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
	ON_WM_ACTIVATEAPP()
END_MESSAGE_MAP()


void CPhsStart::OnDestroy() 
{

	CDialog::OnDestroy();
	// TODO: Add your message handler code here
	if(m_bm.GetSafeHandle()!=NULL)
		m_bm.DeleteObject();
	PostNcDestroy();
}

BOOL CPhsStart::OnInitDialog() 
{
	CDialog::OnInitDialog();
	// TODO: Add extra initialization here
	m_label1.LoadString(IDS_AutoPHSRegisterUser);
	CRect rc1,rc2;
	m_bm.LoadBitmap(IDB_START);
	BITMAP bm;
	m_bm.GetBitmap(&bm);
//	CRect rc1;
	int dx,dy;
	GetWindowRect(&rc1);
	GetClientRect(&rc2);
	dx=bm.bmWidth-rc2.Width();
	dy=bm.bmHeight-rc2.Height();
	rc1.right+=dx;
	rc1.bottom+=dy;
	MoveWindow(&rc1);

	CWnd* pWnd;
	//pWnd=GetTopWindow();
	pWnd=GetDlgItem(IDC_LAB1);
	pWnd->SetWindowPos(NULL,9,153,0,0,SWP_NOSIZE|SWP_NOZORDER);
	pWnd=GetDlgItem(IDC_LABREG);
	pWnd->SetWindowPos(NULL,18,171,0,0,SWP_NOSIZE|SWP_NOZORDER);
	pWnd=GetDlgItem(IDC_LABVER);
	pWnd->SetWindowPos(NULL,368,120,0,0,SWP_NOSIZE|SWP_NOZORDER);

	::SetWindowCenter(m_hWnd);
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CPhsStart::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{		
	// TODO: Change any attributes of the DC here
	static CBrush br;
	
	LOGBRUSH logbr;
	logbr.lbStyle=BS_SOLID;
	if(nCtlColor==CTLCOLOR_STATIC)
	{
		logbr.lbStyle=BS_NULL;
		pDC->SetBkMode(TRANSPARENT);
		br.DeleteObject();
		br.CreateBrushIndirect(&logbr);//COLOR_BACKGROUND
		if(pWnd!=NULL && (pWnd->m_hWnd==GetDlgItem(IDC_LABREG)->m_hWnd || pWnd->m_hWnd==GetDlgItem(IDC_LABVER)->m_hWnd))
		{
			if(pDC!=NULL)
				pDC->SetTextColor(GetSysColor(COLOR_BACKGROUND));
		}
		return (HBRUSH)br;
	}
	// TODO: Return a different brush if the default is not desired
	return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}

void CPhsStart::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CDC sdc;
	sdc.CreateCompatibleDC(&dc);
	CBitmap* oldbm=sdc.SelectObject(&m_bm);
	CRect rc;
	GetClientRect(&rc);
	BITMAP bm;
	m_bm.GetBitmap(&bm);
	//dc.StretchBlt(0,0,rc.Width(),rc.Height(),&sdc,
	//	0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
	dc.BitBlt(0,0,bm.bmWidth,bm.bmHeight,&sdc,0,0,SRCCOPY);
	sdc.SelectObject(oldbm);
	sdc.DeleteDC();
	// Do not call CDialog::OnPaint() for painting messages
}

void CPhsStart::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	FrmSplash=NULL;
	delete this;
	//CDialog::PostNcDestroy();
}



void CPhsStart::OnActivateApp(BOOL bActive, DWORD hTask/*HTASK hTask*/)
{
#ifdef _AUTOPHS2004
	CDialog::OnActivateApp(bActive,hTask);
#else

#if _MSC_VER > 1200

	CDialog::OnActivateApp(bActive, hTask);
#else
	CDialog::OnActivateApp(bActive, (HTASK)hTask);
#endif
#endif
}

BOOL CPhsStart::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
	CWaitCursor wait;
	return TRUE;
	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}
