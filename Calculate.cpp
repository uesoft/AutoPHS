// Calculate.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "Calculate.h"
#include "mainfrm.h"
#include "frmstatus.h"
#include "modPHScal.h"
#include "EDIBgbl.h"
#include "user.h"
#include "FrmTxsr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCalculate

IMPLEMENT_DYNCREATE(CCalculate, CCmdTarget)

CCalculate::CCalculate()
{
	EnableAutomation();
	
	// To keep the application running as long as an OLE automation 
	//	object is active, the constructor calls AfxOleLockApp.
	
	AfxOleLockApp();
}

CCalculate::~CCalculate()
{
	// To terminate the application when all objects created with
	// 	with OLE automation, the destructor calls AfxOleUnlockApp.
	
	AfxOleUnlockApp();
}


void CCalculate::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CCalculate, CCmdTarget)
	//{{AFX_MSG_MAP(CCalculate)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CCalculate, CCmdTarget)
	//{{AFX_DISPATCH_MAP(CCalculate)
	DISP_FUNCTION(CCalculate, "CalculateAllZdj", CalculateAllZdj, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CCalculate, "CalculateOneZdj", CalculateOneZdj, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CCalculate, "CalculateShowTab", CalculateShowTab, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CCalculate, "VBCal", VBCal, VT_EMPTY, VTS_I4 VTS_I4)
	DISP_FUNCTION(CCalculate, "AutoCalPhs", AutoCalPhs, VT_BOOL, VTS_VARIANT VTS_I2 VTS_I2)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_ICalculate to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {6164984F-787B-43EA-8582-D588F89B8F08}
static const IID IID_ICalculate =
{ 0x6164984f, 0x787b, 0x43ea, { 0x85, 0x82, 0xd5, 0x88, 0xf8, 0x9b, 0x8f, 0x8 } };

BEGIN_INTERFACE_MAP(CCalculate, CCmdTarget)
	INTERFACE_PART(CCalculate, IID_ICalculate, Dispatch)
END_INTERFACE_MAP()

// {1785B9A0-BA3D-4785-AF92-18A5498EF340}
IMPLEMENT_OLECREATE(CCalculate, "AutoPHS.Calculate", 0x1785b9a0, 0xba3d, 0x4785, 0xaf, 0x92, 0x18, 0xa5, 0x49, 0x8e, 0xf3, 0x40)

/////////////////////////////////////////////////////////////////////////////
// CCalculate message handlers

void CCalculate::CalculateAllZdj() 
{
	// TODO: Add your dispatch handler code here
	((CMainFrame*)AfxGetApp()->m_pMainWnd)->OnCalcZdjh();
}

void CCalculate::CalculateOneZdj() 
{
	// TODO: Add your dispatch handler code here
	((CMainFrame*)AfxGetApp()->m_pMainWnd)->OnCalAllzdj ();
}

void CCalculate::CalculateShowTab() 
{
	// TODO: Add your dispatch handler code here
	((CMainFrame*)AfxGetApp()->m_pMainWnd)->OnCouShowtab ();
}

void CCalculate::VBCal(long zdjh, long nth) 
{
	// TODO: Add your dispatch handler code here
	/*	if( !PhsDrawUser::InitDB() )
		return;

	_RecordsetPtr rsZ1(__uuidof(Recordset));
	CRString strSQL;

	try 
	{
		strSQL.Format("SELECT nth FROM Z1 WHERE [VolumeID]=%d AND [ZDJH]=%d",
			PhsDrawUser::m_iVlmID,PhsDrawUser::m_iZdjh);
		rsZ1->CursorLocation = adUseClient;
		rsZ1->Open(_variant_t(strSQL),(IDispatch*)PhsDrawUser::m_pConnPRJDB,adOpenStatic,adLockOptimistic,adCmdText);
		if( rsZ1->adoEOF && rsZ1->BOF )
			return;

		if( (long)rsZ1->RecordCount > 1 )//根据路数确定使用标准支吊架选型还是非标支吊架选型
		{
			rsZ1->Close();
			PhsNonStandardAutoCalc();
		}
		else
		{
			long lnth = vtoi(rsZ1->GetCollect("nth"));
			rsZ1->Close();
			PhsStandardAutoCalc( lnth );
		}
	}
	catch(_com_error& e)
	{
		ReportError(e.Description(), __FILE__, __LINE__);
	}

	PhsDrawUser::UnInitDB();
*/

/*
	VARIANT var1,var2;
	VariantInit(&var1);
	VariantInit(&var2);
	_RecordsetPtr rs;
	rs.CreateInstance(__uuidof(Recordset));

	CFrmStatus *frm;
	var1 = varRs;
//	VariantCopy(&var1,&varRs);

	HRESULT hr = VariantChangeType(&var2,&var1,0,VT_UNKNOWN);
	if(FAILED(hr))
		return ;
	rs = (_RecordsetPtr) var1.punkVal;
//	if(rs->adoEOF && rs->BOF)
//	{
		AfxMessageBox("rs");
//	}
	
//	hr = VariantChangeType(&var2,&varFrm,0,VT_UNKNOWN);
//	frm = (CFrmStatus*)var2.punkVal;
*/


	CFrmStatus FrmProgress;
	if(!IsWindow(FrmProgress.m_hWnd))
    	FrmProgress.Create(IDD_STATUS,NULL);
	FrmProgress.CenterWindow();  
	FrmProgress.ShowWindow(SW_SHOW);
	
    CString sTmp;
	sTmp.Format("%d",zdjh);

	FrmProgress.m_Label1.Format(GetResStr(IDS_CalculatingNoXphs),sTmp);// "正在计算" & zdjh & "号支吊架"
	FrmProgress.SetWindowText(GetResStr(IDS_PhsCalculation));// "计算支吊架"
	FrmProgress.UpdateData(false);
	FrmProgress.UpdateStatus(0.02f,true);
	FrmProgress.UpdateWindow();


	_RecordsetPtr rs;
	rs.CreateInstance(__uuidof(Recordset));
	 

	CString strSQL; 

	strSQL.Format("SELECT * FROM [ZA] WHERE [VolumeID]=%d AND [ZDJH]=%d",EDIBgbl::SelVlmID,zdjh);
	rs->Open(_variant_t(strSQL),(IDispatch*)conPRJDB,adOpenStatic,adLockOptimistic,adCmdText);
	if( rs->adoEOF && rs->BOF )
		AfxMessageBox("recordset empty");

	modPHScal::VB_Cal (rs,zdjh,FrmProgress,nth);

	FrmProgress.UpdateWindow();
	FrmProgress.ShowWindow(SW_HIDE);
	FrmProgress.DestroyWindow();

}

BOOL CCalculate::AutoCalPhs(const VARIANT FAR& varRs, short zdjh, short nth) 
{
	// TODO: Add your dispatch handler code here
	CFrmStatus FrmProgress;
	if(!IsWindow(FrmProgress.m_hWnd))
    	FrmProgress.Create(IDD_STATUS,NULL);
	FrmProgress.CenterWindow();  
	FrmProgress.ShowWindow(SW_SHOW);

	modPHScal::zdjh=zdjh;
	modPHScal::iSelSampleID=vtoi(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("iSelSampleID")));

	CString sTmp;
	sTmp.Format(_T("%d"),zdjh);
	frmStatus.m_Label1.Format(GetResStr(IDS_CalculatingNoXphs),sTmp);// _T("正在计算") & zdjh & _T("号支吊架")
	frmStatus.SetWindowText(GetResStr(IDS_PhsCalculation)); //' _T("计算支吊架")
	frmStatus.UpdateData(false);
	frmStatus.UpdateStatus(0.02f,true);
	frmStatus.UpdateWindow();

	_RecordsetPtr rs;
	rs.CreateInstance(__uuidof(Recordset));

	if(varRs.vt == VT_DISPATCH && varRs.pdispVal != NULL)
	{
		rs = _RecordsetPtr (varRs.pdispVal );
	}
	else
	{
		AfxMessageBox("数据库错误，退出计算!");
		return FALSE;
	}

	modPHScal::VB_Cal (rs,zdjh,FrmProgress,nth);

	FrmProgress.UpdateWindow();
	FrmProgress.ShowWindow(SW_HIDE);
	FrmProgress.DestroyWindow();

	return TRUE;
}
