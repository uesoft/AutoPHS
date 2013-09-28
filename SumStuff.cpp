// SumStuff.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "SumStuff.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSumStuff

IMPLEMENT_DYNCREATE(CSumStuff, CCmdTarget)

CSumStuff::CSumStuff()
{
	EnableAutomation();
	
	// To keep the application running as long as an OLE automation 
	//	object is active, the constructor calls AfxOleLockApp.
	
	AfxOleLockApp();
}

CSumStuff::~CSumStuff()
{
	// To terminate the application when all objects created with
	// 	with OLE automation, the destructor calls AfxOleUnlockApp.
	
	AfxOleUnlockApp();
}


void CSumStuff::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CSumStuff, CCmdTarget)
	//{{AFX_MSG_MAP(CSumStuff)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CSumStuff, CCmdTarget)
	//{{AFX_DISPATCH_MAP(CSumStuff)
	DISP_FUNCTION(CSumStuff, "SumStuffACAD", SumStuffACAD, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CSumStuff, "SumStuffACAD2", SumStuffACAD2, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CSumStuff, "SumStuffExcel", SumStuffExcel, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CSumStuff, "SumStuffExcel2", SumStuffExcel2, VT_EMPTY, VTS_NONE)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_ISumStuff to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {FEE10B95-2479-466E-B2CA-3AC00CCC9105}
static const IID IID_ISumStuff =
{ 0xfee10b95, 0x2479, 0x466e, { 0xb2, 0xca, 0x3a, 0xc0, 0xc, 0xcc, 0x91, 0x5 } };

BEGIN_INTERFACE_MAP(CSumStuff, CCmdTarget)
	INTERFACE_PART(CSumStuff, IID_ISumStuff, Dispatch)
END_INTERFACE_MAP()

// {A8F7CAA0-5D06-43FA-8ECE-9C55CD9FE98C}
IMPLEMENT_OLECREATE(CSumStuff, "AutoPHS.SumStuff", 0xa8f7caa0, 0x5d06, 0x43fa, 0x8e, 0xce, 0x9c, 0x55, 0xcd, 0x9f, 0xe9, 0x8c)

/////////////////////////////////////////////////////////////////////////////
// CSumStuff message handlers

void CSumStuff::SumStuffACAD() 
{
	// TODO: Add your dispatch handler code here
	((CMainFrame*)AfxGetApp()->m_pMainWnd)->OnSumAcad ();
}

void CSumStuff::SumStuffACAD2() 
{
	// TODO: Add your dispatch handler code here
	((CMainFrame*)AfxGetApp()->m_pMainWnd)->OnSumAcad2 ();
}

void CSumStuff::SumStuffExcel() 
{
	// TODO: Add your dispatch handler code here
	((CMainFrame*)AfxGetApp()->m_pMainWnd)->OnSumAcad2 ();
}

void CSumStuff::SumStuffExcel2() 
{
	// TODO: Add your dispatch handler code here
	((CMainFrame*)AfxGetApp()->m_pMainWnd)->OnSumExcel ();
}
