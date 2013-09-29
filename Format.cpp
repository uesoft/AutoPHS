// Format.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "Format.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFormat

IMPLEMENT_DYNCREATE(CFormat, CCmdTarget)

CFormat::CFormat()
{
	EnableAutomation();
	
	// To keep the application running as long as an OLE automation 
	//	object is active, the constructor calls AfxOleLockApp.
	
	AfxOleLockApp();
}

CFormat::~CFormat()
{
	// To terminate the application when all objects created with
	// 	with OLE automation, the destructor calls AfxOleUnlockApp.
	
	AfxOleUnlockApp();
}


void CFormat::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CFormat, CCmdTarget)
	//{{AFX_MSG_MAP(CFormat)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CFormat, CCmdTarget)
	//{{AFX_DISPATCH_MAP(CFormat)
	DISP_FUNCTION(CFormat, "FormatSelTabFormat", FormatSelTabFormat, VT_EMPTY, VTS_NONE)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IFormat to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {1C135DEF-8734-4ED4-A90C-F02340F8FCAD}
static const IID IID_IFormat =
{ 0x1c135def, 0x8734, 0x4ed4, { 0xa9, 0xc, 0xf0, 0x23, 0x40, 0xf8, 0xfc, 0xad } };

BEGIN_INTERFACE_MAP(CFormat, CCmdTarget)
	INTERFACE_PART(CFormat, IID_IFormat, Dispatch)
END_INTERFACE_MAP()

// {A2771F21-15E0-4ADB-944A-F570DED25C82}
IMPLEMENT_OLECREATE(CFormat, "AutoPHS.Format", 0xa2771f21, 0x15e0, 0x4adb, 0x94, 0x4a, 0xf5, 0x70, 0xde, 0xd2, 0x5c, 0x82)

/////////////////////////////////////////////////////////////////////////////
// CFormat message handlers

void CFormat::FormatSelTabFormat() 
{
	// TODO: Add your dispatch handler code here
	((CMainFrame*)AfxGetApp()->m_pMainWnd)->OnSetTableFormat ();
}
