// Files.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "Files.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFiles

IMPLEMENT_DYNCREATE(CFiles, CCmdTarget)

CFiles::CFiles()
{
	EnableAutomation();
	
	// To keep the application running as long as an OLE automation 
	//	object is active, the constructor calls AfxOleLockApp.
	
	AfxOleLockApp();
}

CFiles::~CFiles()
{
	// To terminate the application when all objects created with
	// 	with OLE automation, the destructor calls AfxOleUnlockApp.
	
	AfxOleUnlockApp();
}


void CFiles::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CFiles, CCmdTarget)
	//{{AFX_MSG_MAP(CFiles)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CFiles, CCmdTarget)
	//{{AFX_DISPATCH_MAP(CFiles)
	DISP_FUNCTION(CFiles, "FilesOpt", FilesOpt, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CFiles, "FilesSelDir", FilesSelDir, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CFiles, "FilesSelPreferences", FilesSelPreferences, VT_EMPTY, VTS_NONE)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IFiles to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {0DDEC804-71A7-4801-BBF0-74E40ADB461B}
static const IID IID_IFiles =
{ 0xddec804, 0x71a7, 0x4801, { 0xbb, 0xf0, 0x74, 0xe4, 0xa, 0xdb, 0x46, 0x1b } };

BEGIN_INTERFACE_MAP(CFiles, CCmdTarget)
	INTERFACE_PART(CFiles, IID_IFiles, Dispatch)
END_INTERFACE_MAP()

// {13C3058E-6D52-419A-A3D9-BDC917B3300C}
IMPLEMENT_OLECREATE(CFiles, "AutoPHS.Files", 0x13c3058e, 0x6d52, 0x419a, 0xa3, 0xd9, 0xbd, 0xc9, 0x17, 0xb3, 0x30, 0xc)

/////////////////////////////////////////////////////////////////////////////
// CFiles message handlers

void CFiles::FilesOpt() 
{
	// TODO: Add your dispatch handler code here
	((CMainFrame*)AfxGetApp()->m_pMainWnd)->OnFileOpt ();
}

void CFiles::FilesSelDir() 
{
	// TODO: Add your dispatch handler code here
	((CMainFrame*)AfxGetApp()->m_pMainWnd)->OnFileSelDir ();
}

void CFiles::FilesSelPreferences() 
{
	// TODO: Add your dispatch handler code here
	((CMainFrame*)AfxGetApp()->m_pMainWnd)->OnSelPreferences ();
}

