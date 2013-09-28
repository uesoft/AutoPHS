// AutoPHS.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "AutoPHS.h"

#include "MainFrm.h"
#include "AutoPHSDoc.h"
#include "AutoPHSView.h"
#include "user.h"
#include "HyperLink.h"
#include "Htmlhelp.h"
#include "modPHScal.h"
#include "PSDstart.h"
#include "PhsStart.h"
#include "modregistry.h"
#include "modSelPDSV.h"
#include "SplashThread.h"

#include "EDIBgbl.h"
#include "PreCalPag.h"
#include "PreDrawPag.h"
#include "PrePointPag.h"
#include "PreStylePag.h"
#include "FrmPreferences.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if(WINVER < 0x0500)
	#define AW_HOR_POSITIVE             0x00000001
	#define AW_HOR_NEGATIVE             0x00000002
	#define AW_VER_POSITIVE             0x00000004
	#define AW_VER_NEGATIVE             0x00000008
	#define AW_CENTER                   0x00000010
	#define AW_HIDE                     0x00010000
	#define AW_ACTIVATE                 0x00020000
	#define AW_SLIDE                    0x00040000
	#define AW_BLEND                    0x00080000
#endif
typedef BOOL (WINAPI * __PROCAnimateWindow) (
  HWND hwnd,     // handle to the window to animate
  DWORD dwTime,  // duration of animation
  DWORD dwFlags  // animation type
);

/////////////////////////////////////////////////////////////////////////////
// CAutoPHSApp

BEGIN_MESSAGE_MAP(CAutoPHSApp, CWinApp)
	//{{AFX_MSG_MAP(CAutoPHSApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_CONTEXT_HELP, OnContextHelp)
	ON_COMMAND(ID_HELP_FINDER, OnHelpFinder)
	ON_COMMAND(ID_HELP_INDEX, OnHelpIndex)
	ON_COMMAND(ID_HELP, OnHelp)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoPHSApp construction

CAutoPHSApp::CAutoPHSApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

CAutoPHSApp::~CAutoPHSApp ()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAutoPHSApp object

CAutoPHSApp theApp;
int m_AppTime = 0;
// This identifier was generated to be statistically unique for your app.
// You may change it if you prefer to choose a specific identifier.

// {E8B98C98-5DD5-41B1-A0D6-766045C87464}
CLSID clsid =
{ 0xe8b98c98, 0x5dd5, 0x41b1, { 0xa0, 0xd6, 0x76, 0x60, 0x45, 0xc8, 0x74, 0x64 } };

/////////////////////////////////////////////////////////////////////////////
// CAutoPHSApp initialization

BOOL CAutoPHSApp::InitInstance()
{
	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDS_OLEINITIALIZE_FAIL);
		return FALSE;
	}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CAutoPHSDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CAutoPHSView));
	AddDocTemplate(pDocTemplate);

	// Connect the COleTemplateServer to the document template.
	//  The COleTemplateServer creates new documents on behalf
	//  of requesting OLE containers by using information
	//  specified in the document template.
	m_server.ConnectTemplate(clsid, pDocTemplate, TRUE);
		// Note: SDI applications register server objects only if /Embedding
		//   or /Automation is present on the command line.

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Check to see if launched as OLE server
	if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated )
	{
		// Register all OLE server (factories) as running.  This enables the
		//  OLE libraries to create objects from other applications.
		COleTemplateServer::RegisterAll();

		// Application was run with /Embedding or /Automation.  Don't show the
		//  main window in this case.
		return TRUE;
	}

	// When a server application is launched stand-alone, it is a good idea
	//  to update the system registry in case it has been damaged.
	m_server.UpdateRegistry(OAT_DISPATCH_OBJECT);
	COleObjectFactory::UpdateRegistryAll();

	m_AppTime = 1;
	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
	{
		return FALSE;
	}

	CreateMutex(NULL,TRUE,this->m_pszAppName);
	if(::GetLastError()==ERROR_ALREADY_EXISTS)
	{
		CString sMsg;
		sMsg.Format(IDS_YOU_HAVE_RUN_THANK_YOU,this->m_pszAppName,this->m_pszAppName);
        CString str;
		str.LoadString(IDS_WARNING);
		//ligb on 2005.08.12允许运行多个实例，以便使用多个规范标准比较设计支吊架
		//MessageBox(NULL,sMsg,str,MB_OK|MB_TOPMOST);
		//return FALSE;
	}

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_HIDE);
	m_pMainWnd->UpdateWindow();

	modPHScal::bInstalledAndFirstRun = GetRegKey("Status","InstalledAndFirstRun",TRUE);
	InitDefVar();
	if(!PSDstart::Start())
		return FALSE;

	CMainFrame* pWnd=new CMainFrame;
	ASSERT(pWnd != NULL);
	this->m_pMainWnd=(CWnd*)pWnd;
	if(!pWnd->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL))
		return FALSE;

	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	hSplashWndCreate=::CreateEvent(NULL,FALSE,FALSE,NULL);
	::AfxBeginThread(RUNTIME_CLASS(CSplashThread));
	::WaitForSingleObject(hSplashWndCreate,INFINITE);

	if(!pWnd->Start())
	{
		delete pWnd;
		m_pMainWnd=NULL;
		FrmSplash->SendMessage(WM_DESTROY);
		return FALSE;
	}
	
	SetRegValue("Status","InstalledAndFirstRun",FALSE);
	m_hAccelData=LoadAccelerators(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDA_DATA));
	FrmSplash->SendMessage(WM_DESTROY);
	CloseHandle(hSplashWndCreate);
	hSplashWndCreate=NULL;

// 初始化选项列表中的数据，让其与注册表中的变量数据一致
// 让其在安装的第一次运行。

	if(modPHScal::bInstalledAndFirstRun)
	{
		int nResult=0;
		CPreCalPag 	m_wndCalPag;
		m_wndCalPag.Create (IDD_PRE_PAG1);
		m_wndCalPag.DoDefaultSetting();//Modified by Shuli Luo
		m_wndCalPag.SaveIniValue();
		m_wndCalPag.EndDialog (nResult);
		
		CPreDrawPag	m_wndDrawPag;
		m_wndDrawPag.Create (IDD_PRE_PAG2);	
		m_wndDrawPag.DoDefaultSetting();//Modified by Shuli Luo
		m_wndDrawPag.SaveIniValue();
		m_wndDrawPag.EndDialog(nResult);
		
		SetRegValue(_T("Settings"), _T("CSPRAutoLugDia"), modPHScal::gbCSPRAutoLugDia);
		SetRegValue(_T("Settings"), _T("AutoTZ"), modPHScal::AutoOrdinateDrawing);
		
		SetRegValue(_T("Settings"), _T("CalSAbyConservativenessMethod"), (modPHScal::gbCalSAbyConservativenessMethod));
		SetRegValue(_T("Settings"), _T("CalSAbyPJG"), (modPHScal::gbCalSAbyPJG));
		SetRegValue(_T("Settings"), _T("CalCSPRLugDiaByPjg"), (modPHScal::gbCalCSPRLugDiaByPjg));
		
		SetRegValue(_T("Settings"), _T("CalPARTbyMaxLoad"), (modPHScal::gbCalPARTbyMaxLoad));
		SetRegValue(_T("Settings"), _T("AddPartWeight2PMax"), (modPHScal::gbAddPartWeight2PMax));
		SetRegValue(_T("Settings"), _T("AddPartWeight2CSPRPgz"), (modPHScal::gbAddPartWeight2CSPRPgz));
		SetRegValue(_T("Settings"), _T("AddPartWeight2SPRPgz"),  (modPHScal::gbAddPartWeight2SPRPgz));//2007.07.30
		SetRegValue(_T("Settings"),_T("PAForceZero"),(modPHScal::gbPAForceZero));//2007.09.25
		SetRegValue(_T("Settings"), _T("MaxPgzPaz"),  (modPHScal::gbMaxPgzPaz));//2007.09.03

		SetRegValue(_T("Settings"), _T("MinMaxDispByActualDisp"), (modPHScal::gbMinMaxDispByActualDisp));
		SetRegValue(_T("Settings"), _T("CalcCSPRHeight_BySelDisp"), (modPHScal::gbCalcCSPRHeight_BySelDisp));
		
		
		SetRegValue(_T("Settings"), _T("PhsOutINFOTableType"), (modPHScal::giPhsOutINFOTableType));
		SetRegValue(_T("Settings"), _T("DrawSIZE_A3A4"), (modPHScal::giDrawSIZE_A3A4));
		SetRegValue(_T("Settings"), _T("IsACAD"), (modPHScal::gIsACAD));
		SetRegValue(_T("Settings"), _T("SpringOrder"), (modPHScal::giSpringOrder));
		SetRegValue(_T("Settings"), _T("strSpringOrder"), modPHScal::gstrSpringOrder);
		SetRegValue(_T("Settings"), _T("strSpringOrderSQL"), modPHScal::gstrSpringOrderSQL);
		SetRegValue(_T("Settings"), _T("PhsColor"), modPHScal::gsPhsColor);
		SetRegValue(_T("Settings"), _T("Draw_BOM"), (modPHScal::gbDraw_BOM));
		SetRegValue(_T("Settings"), _T("AnnotateCSPR"), (modPHScal::gbAnnotateCSPR));
		SetRegValue(_T("Settings"), _T("Draw_OVERWRITE"), (modPHScal::gbDrawOVERWRITE));
		SetRegValue(_T("Settings"), _T("WorksWithBoltsNuts"), (modPHScal::gbWorksWithBoltsNuts));
		SetRegValue(_T("Settings"), _T("WorksWithSABoltsNuts"), (modPHScal::gbWorksWithSABoltsNuts));
		SetRegValue(_T("Settings"), _T("DW_delta"), (modPHScal::gnDW_delta));
		
		SetRegValue(_T("Settings"), _T("ZDJH"), (EDIBgbl::glCurRecordNo));
		SetRegValue(_T("Settings"), _T("DrawNoPreMethod"), (modPHScal::gbDrawNoPreMethod));
		SetRegValue(_T("Settings"), _T("AutoSaveDrawing"), (modPHScal::gbAutoSaveDrawing));
		SetRegValue(_T("Settings"), _T("DrawNoStartNumber"), (modPHScal::glDrawNoStartNumber));
		SetRegValue(_T("Settings"), _T("DwgVersion"), (EDIBgbl::strDwgVersion));
		SetRegValue(_T("Settings"), _T("DwgMaxLGLong"), (EDIBgbl::MaxLGLong));//pfg20050922

		SetRegValue(_T("Settings"), _T("DWGPreMethod"), (modPHScal::glDWGPreMethod));
		SetRegValue(_T("Settings"), _T("DWGPre"), modPHScal::gsDWGPre);
		SetRegValue(_T("Settings"), _T("CSPRCanSpecialDesign"), (modPHScal::gbCSPRCanSpecialDesign));
		SetRegValue(_T("Settings"), _T("LimitLugMinDia"), (modPHScal::gbLimitLugMinDia));
		
		SetRegValue(_T("Settings"), _T("AutoAlignCSPRtoSA"), (modPHScal::gbAutoAlignCSPRtoSA));
		SetRegValue(_T("Settings"), _T("SumAllVolume"), (modPHScal::gbSumAllVolume));
	}
	/////////////////////////////////////////
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CHyperLink myURL;
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CHyperLink	m_address;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_STATIC_address, m_address);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// App command to run the dialog
void CAutoPHSApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CAutoPHSApp message handlers


int CAutoPHSApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CWinApp::ExitInstance();
}

void CAutoPHSApp::OnContextHelp() 
{
	// TODO: Add your command handler code here
	
}

void CAutoPHSApp::OnHelpFinder() 
{
	// TODO: Add your command handler code here
	::HtmlHelp(this->m_pMainWnd->m_hWnd,this->m_pszHelpFilePath,HH_HELP_FINDER,NULL);
}

void CAutoPHSApp::OnHelpIndex() 
{
	// TODO: Add your command handler code here
	::HtmlHelp(this->m_pMainWnd->m_hWnd,this->m_pszHelpFilePath,HH_DISPLAY_INDEX,NULL);
}

void CAutoPHSApp::OnHelp() 
{
	// TODO: Add your command handler code here
	::HtmlHelp(this->m_pMainWnd->m_hWnd,this->m_pszHelpFilePath,HH_DISPLAY_TOC,NULL);
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	myURL.SetURL("http://www.uesoft.com");
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/*HBRUSH CAboutDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	if(pDC!=NULL && nCtlColor==CTLCOLOR_STATIC && pWnd!=NULL && pWnd->m_hWnd==m_ctrURL.m_hWnd)
	{
		  pDC->SetTextColor(0xff0000);
	}
	// TODO: Return a different brush if the default is not desired
	return hbr;
}*/
