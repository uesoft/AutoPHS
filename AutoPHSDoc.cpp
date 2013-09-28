// AutoPHSDoc.cpp : implementation of the CAutoPHSDoc class
//

#include "stdafx.h"
#include "AutoPHS.h"

#include "AutoPHSDoc.h"
#include "mainfrm.h"
#include "calculate.h"
#include "datainput.h"
#include "files.h"
#include "format.h"
#include "sumstuff.h"
#include "modPHScal.h"
#include "PSDstart.h"
#include "PhsStart.h"
#include "modregistry.h"
#include "modSelPDSV.h"
#include "SplashThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
 
/////////////////////////////////////////////////////////////////////////////
// CAutoPHSDoc
extern int m_AppTime;
extern CLSID clsid;
IMPLEMENT_DYNCREATE(CAutoPHSDoc, CDocument)

BEGIN_MESSAGE_MAP(CAutoPHSDoc, CDocument)
	//{{AFX_MSG_MAP(CAutoPHSDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CAutoPHSDoc, CDocument)
	//{{AFX_DISPATCH_MAP(CAutoPHSDoc)
	DISP_PROPERTY_EX(CAutoPHSDoc, "Calculate", GetCalculate, SetCalculate, VT_DISPATCH)
	DISP_PROPERTY_EX(CAutoPHSDoc, "DataInput", GetDataInput, SetDataInput, VT_DISPATCH)
	DISP_PROPERTY_EX(CAutoPHSDoc, "Files", GetFiles, SetFiles, VT_DISPATCH)
	DISP_PROPERTY_EX(CAutoPHSDoc, "Format", GetFormat, SetFormat, VT_DISPATCH)
	DISP_PROPERTY_EX(CAutoPHSDoc, "SumStuff", GetSumStuff, SetSumStuff, VT_DISPATCH)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IAutoPHS to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {602FEC24-7E1D-4A05-BBE2-838CED8ACC3B}
static const IID IID_IAutoPHS =
{ 0x602fec24, 0x7e1d, 0x4a05, { 0xbb, 0xe2, 0x83, 0x8c, 0xed, 0x8a, 0xcc, 0x3b } };

BEGIN_INTERFACE_MAP(CAutoPHSDoc, CDocument)
	INTERFACE_PART(CAutoPHSDoc, IID_IAutoPHS, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoPHSDoc construction/destruction

CAutoPHSDoc::CAutoPHSDoc()
{
	// TODO: add one-time construction code here

	EnableAutomation();

	AfxOleLockApp();

	IUnknown* pUnk;
	GetIDispatch(FALSE)->QueryInterface(IID_IUnknown,(void**)&pUnk);
	GetIDispatch(FALSE)->Release();
	::RegisterActiveObject(pUnk,clsid,ACTIVEOBJECT_WEAK,&m_ulID);

	if(m_AppTime != 1)
	{
		modPHScal::bInstalledAndFirstRun = GetRegKey("Status","InstalledAndFirstRun",TRUE);
		InitDefVar();
		if(!PSDstart::Start())
			return ;
	
		CMainFrame* pWnd=new CMainFrame;
		ASSERT(pWnd != NULL);
		AfxGetApp()->m_pMainWnd = (CWnd*)pWnd;
		if(!pWnd->LoadFrame(IDR_MAINFRAME,
			WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
			NULL))
			return ;

		pWnd->ShowWindow(SW_SHOW);
		pWnd->UpdateWindow();

		hSplashWndCreate=::CreateEvent(NULL,FALSE,FALSE,NULL);
		::AfxBeginThread(RUNTIME_CLASS(CSplashThread));
		::WaitForSingleObject(hSplashWndCreate,INFINITE);

		if(!pWnd->Start())
		{
			delete pWnd;
//			m_pMainWnd=NULL;
			FrmSplash->SendMessage(WM_DESTROY);
			return ;
		}
		
		SetRegValue("Status","InstalledAndFirstRun",FALSE);
		m_hAccelData=LoadAccelerators(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDA_DATA));
		FrmSplash->SendMessage(WM_DESTROY);
		CloseHandle(hSplashWndCreate);
		hSplashWndCreate=NULL;
	}
}

CAutoPHSDoc::~CAutoPHSDoc()
{
	AfxOleUnlockApp();
}

BOOL CAutoPHSDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CAutoPHSDoc serialization

void CAutoPHSDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAutoPHSDoc diagnostics

#ifdef _DEBUG
void CAutoPHSDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAutoPHSDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAutoPHSDoc commands

void CAutoPHSDoc::OnFinalRelease() 
{
	// TODO: Add your specialized code here and/or call the base class
	::RevokeActiveObject(m_ulID,NULL);
	CDocument::OnFinalRelease();
}

LPDISPATCH CAutoPHSDoc::GetCalculate() 
{
	// TODO: Add your property handler here

	CCalculate *pDI = new CCalculate;
	LPDISPATCH lpResult = pDI->GetIDispatch (FALSE);

	return lpResult;
}

void CAutoPHSDoc::SetCalculate(LPDISPATCH newValue) 
{
	// TODO: Add your property handler here

}

LPDISPATCH CAutoPHSDoc::GetDataInput() 
{
	// TODO: Add your property handler here

	CDataInput *pDI = new CDataInput;
	LPDISPATCH lpResult = pDI->GetIDispatch (FALSE);

	return lpResult;
}

void CAutoPHSDoc::SetDataInput(LPDISPATCH newValue) 
{
	// TODO: Add your property handler here

}

LPDISPATCH CAutoPHSDoc::GetFiles() 
{
	// TODO: Add your property handler here
	CFiles *pDI = new CFiles;
	LPDISPATCH lpResult = pDI->GetIDispatch (FALSE);

	return lpResult;
}

void CAutoPHSDoc::SetFiles(LPDISPATCH newValue) 
{
	// TODO: Add your property handler here

}

LPDISPATCH CAutoPHSDoc::GetFormat() 
{
	// TODO: Add your property handler here

	CFormat *pDI = new CFormat;
	LPDISPATCH lpResult = pDI->GetIDispatch (FALSE);

	return lpResult;
}

void CAutoPHSDoc::SetFormat(LPDISPATCH newValue) 
{
	// TODO: Add your property handler here

}

LPDISPATCH CAutoPHSDoc::GetSumStuff() 
{
	// TODO: Add your property handler here

	CSumStuff *pDI = new CSumStuff;
	LPDISPATCH lpResult = pDI->GetIDispatch (FALSE);

	return lpResult;
}

void CAutoPHSDoc::SetSumStuff(LPDISPATCH newValue) 
{
	// TODO: Add your property handler here

}
