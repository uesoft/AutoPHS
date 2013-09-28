// DataInput.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "DataInput.h"
#include "mainfrm.h"
#include "FrmTxsr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDataInput

IMPLEMENT_DYNCREATE(CDataInput, CCmdTarget)

CDataInput::CDataInput()
{
	EnableAutomation();
	
	// To keep the application running as long as an OLE automation 
	//	object is active, the constructor calls AfxOleLockApp.
	
	AfxOleLockApp();
}

CDataInput::~CDataInput()
{
	// To terminate the application when all objects created with
	// 	with OLE automation, the destructor calls AfxOleUnlockApp.
	
	AfxOleUnlockApp();
}


void CDataInput::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CDataInput, CCmdTarget)
	//{{AFX_MSG_MAP(CDataInput)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CDataInput, CCmdTarget)
	//{{AFX_DISPATCH_MAP(CDataInput)
	DISP_FUNCTION(CDataInput, "DataInputImg", DataInputImg, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CDataInput, "DataInputModifyZdjh", DataInputModifyZdjh, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CDataInput, "DataInputPipeana", DataInputPipeana, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CDataInput, "DataInputSArec", DataInputSArec, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CDataInput, "DataInputSelPro", DataInputSelPro, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CDataInput, "DataInputTab", DataInputTab, VT_EMPTY, VTS_NONE)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IDataInput to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {082570C2-6B2B-416A-816B-A4DB4F6A5A25}
static const IID IID_IDataInput =
{ 0x82570c2, 0x6b2b, 0x416a, { 0x81, 0x6b, 0xa4, 0xdb, 0x4f, 0x6a, 0x5a, 0x25 } };

BEGIN_INTERFACE_MAP(CDataInput, CCmdTarget)
	INTERFACE_PART(CDataInput, IID_IDataInput, Dispatch)
END_INTERFACE_MAP()

// {8207EBBA-90F6-4CAD-B658-7455CF74FB0D}
IMPLEMENT_OLECREATE(CDataInput, "AutoPHS.DataInput", 0x8207ebba, 0x90f6, 0x4cad, 0xb6, 0x58, 0x74, 0x55, 0xcf, 0x74, 0xfb, 0xd)

/////////////////////////////////////////////////////////////////////////////
// CDataInput message handlers

void CDataInput::DataInputSArec() 
{
	// TODO: Add your dispatch handler code here
	((CMainFrame*)AfxGetApp()->m_pMainWnd)->OnInputSArec ();

}

void CDataInput::DataInputImg() 
{
	// TODO: Add your dispatch handler code here
//	CFrameWnd *pWnd = (CFrameWnd*)AfxGetMainWnd();

//	if(!IsWindow(FrmTxsr.m_hWnd))
//	{
//		FrmTxsr.Create(pWnd);
//	}

//	FrmTxsr.ActivateFrame (SW_SHOW);	
	((CMainFrame*)AfxGetApp()->m_pMainWnd)->OnIntImgint ();

}

void CDataInput::DataInputModifyZdjh() 
{
	// TODO: Add your dispatch handler code here
	((CMainFrame*)AfxGetApp()->m_pMainWnd)->OnModifyZdjh ();

}

void CDataInput::DataInputPipeana() 
{
	// TODO: Add your dispatch handler code here
	((CMainFrame*)AfxGetApp()->m_pMainWnd)->OnIntPipeana();

}

void CDataInput::DataInputSelPro() 
{
	// TODO: Add your dispatch handler code here
	((CMainFrame*)AfxGetApp()->m_pMainWnd)->OnSelPDSV();

}

void CDataInput::DataInputTab() 
{
	// TODO: Add your dispatch handler code here
	((CMainFrame*)AfxGetApp()->m_pMainWnd)->OnIntTabint ();

}
