// FrmStatus.cpp : implementation file
//

#include "stdafx.h"
#include "modPHScal.h"
#include "autophs.h"
#include "FrmStatus.h"
#include "PHSStart.h"
#include "user.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFrmStatus dialog
CFrmStatus frmStatus=NULL;
HANDLE hStatusStopEvent=NULL;
HANDLE hStatusCreateEvent=NULL;
CFrmStatus::CFrmStatus(CWnd* pParent /*=NULL*/)
	: CDialog(CFrmStatus::IDD, pParent)
	, m_bThread(FALSE)
{
	//{{AFX_DATA_INIT(CFrmStatus)
	m_Label2 = _T(""); 
	m_Label1 = _T("");
	//}}AFX_DATA_INIT
	//::InitializeCriticalSection(&m_crUpd);
	//::InitializeCriticalSection(&m_crStat);
}


void CFrmStatus::DoDataExchange(CDataExchange* pDX)
{
	//::EnterCriticalSection(&m_crUpd);
	CDialog::DoDataExchange(pDX);	
		
	//{{AFX_DATA_MAP(CFrmStatus)
	DDX_Control(pDX, IDC_LABEL2, m_ctrLabel2);
	DDX_Control(pDX, IDC_LABEL1, m_ctrLabel1);
	DDX_Control(pDX, IDC_STOP, m_btnStop);
	DDX_Control(pDX, IDC_PROGRESS, m_picStatus);
	DDX_Text(pDX, IDC_LABEL2, m_Label2);
	DDX_Text(pDX, IDC_LABEL1, m_Label1);	
	//}}AFX_DATA_MAP
	//:/:LeaveCriticalSection(&m_crUpd);
}


BEGIN_MESSAGE_MAP(CFrmStatus, CDialog)
	//{{AFX_MSG_MAP(CFrmStatus)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_STOP, OnStop)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_CLOSEWINDOW1,OnCloseWindow1)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFrmStatus message handlers

void CFrmStatus::UpdateStatus(float sngPercent, bool fBorderCase)
{
	//::EnterCriticalSection(&m_crStat);
	long pos=(long)(sngPercent*100.0+0.5);
	/*DWORD sy=::GetWindowLong(m_picStatus.m_hWnd,GWL_STYLE);
	if(fBorderCase)
		sy|=PBS_SMOOTH;
	else
		sy &= ~PBS_SMOOTH;
	::SetWindowLong(m_picStatus.m_hWnd,GWL_STYLE,sy);*/
	if(pos != m_nPos)
	{
		if(pos<=0)
		{
			if(!fBorderCase)
				pos=1;
			else
				pos=0;
		}
		else if(pos>=100)
		{
			if(!fBorderCase)
				pos=99;
			else 
				pos=100;
		}
	
		m_nPos = pos;
	
		CString s;
		s.Format(_T("%d"),pos);
		s+=_T("%");
		m_picStatus.SetText(s);
	//if(m_bThread)
	//	::Message(m_picStatus.GetSafeHwnd(),PBM_SETPOS, pos, 0L);
	//else
		m_picStatus.SetPos(pos);
	}
	//::LeaveCriticalSection(&m_crStat);
}

BOOL CFrmStatus::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	//if (modPHScal::gbCalAllPhs) GetDlgItem(IDC_STOP)
	//this->m_picStatus.SetTextFont("ËÎÌו",-12,TRUE);
	m_picStatus.SetRange(0,100);
	m_picStatus.SetStep(1);
	if(FrmSplash!=NULL && IsWindow(FrmSplash->m_hWnd))
   {
	   CRect rc,rc1;
	   FrmSplash->GetWindowRect(&rc);
	   GetWindowRect(&rc1);
	   int H=rc1.Height();
	   int W=rc1.Width();
	   int SW=::GetSystemMetrics(SM_CXSCREEN);
	   rc1.left=(SW-W)/2;
	   rc1.right=rc1.left+W;
	   rc1.top=rc.bottom+1;
	   rc1.bottom=rc1.top+H;
	   MoveWindow(&rc1);
   }
	frmStatus.GetWindowRect(&m_wndRc);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFrmStatus::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	//this->m_picStatus.SetTextFont("ËÎÌו",-12,TRUE);
	// TODO: Add your message handler code here
	//MoveWindow(&m_wndRc);
	if(FrmSplash==NULL && bShow==TRUE)
	{
		::SetWindowCenter(this->m_hWnd);
	}
}

void CFrmStatus::OnStop() 
{
	::SetEvent(hStatusStopEvent);
	//modPHScal::gbStopCalAllPHS=true;
}

CFrmStatus::~CFrmStatus()
{
	//DeleteCriticalSection(&m_crUpd);
	//DeleteCriticalSection(&m_crStat);
}

void CFrmStatus::OnCancel() 
{
	// TODO: Add extra cleanup here
	
}

LRESULT CFrmStatus::OnCloseWindow1(/*UINT msg,*/ WPARAM wParam, LPARAM lParam)
{
	DestroyWindow();
	return TRUE;
}

void CFrmStatus::OnOK()
{

}
