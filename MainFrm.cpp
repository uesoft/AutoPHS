// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "AutoPHS.h"
#include "DlgDBOper.h"   //LFX 2005.2.26加
#include "MainFrm.h"
#include <math.h>
#include "PhsData.h"
#include "FrmStatus.h"
#include "PhsStart.h"
#include "modPHScal.h"
#include "EDIBgbl.h"
#include "EDIBAcad.h"
#include "ModEncrypt.h"
#include "basDirectory.h"
#include "FrmTxsr.h"
#include "Cphs.h" 
#include "PSDstart.h"
#include "FrmSelSpecification.h"
#include "FrmSelPDSV.h"
#include "user.h"
#include "FrmAddSArecord.h"
#include "FrmPreferences.h"
#include "FrmDatabaseIn.h"
//#include "FrmDataFormat.h"
#include "EDIBDB.h"
#include "READPSARESULT.h"
#include "FrmTK.h"
#include "FrmFolderLocation.h"
#include "InputBox.h"
#include "StatusBarThread.h"
#include "FrmDataEDIT.h"
#include "DlgAddMaterial.h"
#include "DlgSpan.h"
#include "DlgEditDB.h"
#include "PDMSCore.h"
#include "CalStructDlg.h"
#include "SelTemplate.h"
#include "DlgModifyZdjh.h"
#include <AFXCONV.h>
#include "FileDialogEx.h"
#include "DlgExportVolume.h"
#include "DlgBackupUndo.h"
#include "user.h"
#include "modregistry.h"
#include "TBFormatDlg.h"
#include "SelEngVolDll.h"
#include "modSelPdSV.h"
#include "DlgDataFormatDll.h"
#include "ChangMaterilName.h"
#include "ExportPaint.h"
#include "ImportPlantVision.h"
#include "SelectXYZdlg.h";
#include "Guide.h"
#include "OutputPDSFileTr.h"
#include "SelBookmarks.h"

#define REG_CALL(CALL,MSG) \
	if(ERROR_SUCCESS != CALL) \
{\
	AfxMessageBox(MSG);\
}
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ID_TIME1 100

BOOL gStartAcad();
bool bIsCom = false;//用COM画为true; arx画为false; 1/12   
//_declspec(dllexport) extern CDlgDBOper dlgDBOper;
/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_COMMAND(ID_INT_TABINT, OnIntTabint)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_COMMAND(ID_FILE_NEWDWG, OnFileNewDwg)
	ON_WM_SIZING()
	ON_COMMAND(ID_INT_IMGINT, OnIntImgint)
	ON_WM_SHOWWINDOW()
	ON_WM_ACTIVATE()
	ON_COMMAND(ID_INT_SELPRO, OnSelPDSV)
	ON_COMMAND(ID_INT_TEM, OnSampleManage)
	ON_COMMAND(ID_INT_ADDDATA, OnInputSArec)
	ON_COMMAND(ID_COU_ONEZDJ, OnCalcZdjh)
	ON_COMMAND(ID_DRAW_ZDJ, OnDrawZdj)
	ON_COMMAND(ID_FILE_STARTACAD, OnStartacad)
	ON_COMMAND(ID_FILE_SELCRI, OnSelPreferences)
	ON_COMMAND(ID_FILE_OPT, OnFileOpt)
	ON_COMMAND(ID_INT_IMPDATA, OnIntImpdata)
	ON_COMMAND(ID_FOR_SEL, OnSetTableFormat)
	ON_COMMAND(ID_SUM_ACAD, OnSumAcad)
	ON_COMMAND(ID_SUM_EXC, OnSumExc)
	ON_COMMAND(ID_SUM_TXT, OnSumTxt)
	ON_COMMAND(ID_SUM_ACAD2, OnSumAcad2)
	ON_COMMAND(ID_INT_NUM, OnIntPipeana)
	ON_COMMAND(ID_FILE_SELDIR, OnFileSelDir)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVEAS, OnFileSaveAs)
	ON_COMMAND(ID_FILE_CLOSEACAD, OnFileCloseAcad)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEWDWG, OnUpdateFileNewOpen)
	ON_COMMAND(ID_COU_ALLZDJ, OnCalAllzdj)
	ON_COMMAND(ID_FILE_CLOEXC, OnFileCloseExcel)
	ON_COMMAND(ID_DRAW_ZDJTAB18, OnDrawZdjTab180)
	ON_COMMAND(ID_DRAW_ZDJTAB27, OnDrawZdjTab270)
	ON_COMMAND(ID_ZDJ_DRAWEP, OnZdjDrawEP)
	ON_COMMAND(ID_ZDJ_DRAWPC, OnZdjDrawPC)
	ON_COMMAND(ID_DRAW_ZDJDIR, OnDrawZdjTML)
	ON_COMMAND(ID_ERASE_ALL, OnEraseAll)
	ON_COMMAND(ID_ERASE_LAYER, OnEraseLayer)
	ON_COMMAND(ID_CAD_CEL, OnCadCel)
	ON_COMMAND(ID_CAD_MENU, OnCadMenu)	
	ON_COMMAND(ID_DELETE_LAYER_TAG, OnDeleteLayerTag)
	ON_COMMAND(ID_COU_SHOWTAB, OnCouShowtab)
	ON_COMMAND(ID_DRAW_CHANGEDIM, OnDrawChangeDim)
	ON_COMMAND(ID_REC_FILE1, OnRecFile1)
	ON_COMMAND(ID_REC_FILE2, OnRecFile2)
	ON_COMMAND(ID_REC_FILE3, OnRecFile3)
	ON_COMMAND(ID_REC_FILE4, OnRecFile4)
	ON_COMMAND(ID_REC_FILE5, OnRecFile5)
	ON_COMMAND(ID_REC_FILE6, OnRecFile6)
	ON_COMMAND(ID_REC_FILE7, OnRecFile7)
	ON_COMMAND(ID_REC_FILE8, OnRecFile8)
	ON_UPDATE_COMMAND_UI(ID_SPEC_PA, OnUpdateSpecPa)
	ON_UPDATE_COMMAND_UI(ID_SPEC_CSPR, OnUpdateSpecCspr)
	ON_UPDATE_COMMAND_UI(ID_SPEC_BOLTS, OnUpdateSpecBolts)
	ON_UPDATE_COMMAND_UI(ID_SPEC_PART, OnUpdateSpecPart)
	ON_UPDATE_COMMAND_UI(ID_SPEC_SA, OnUpdateSpecSa)
	ON_UPDATE_COMMAND_UI(ID_SPEC_SPR, OnUpdateSpecSpr)
	ON_COMMAND(ID_CAL_ADDMATERIAL, OnCalAddmaterial)
	ON_COMMAND(ID_CAL_ALLOWMAXSPAN, OnCalAllowMaxSpan)
	ON_COMMAND(ID_DRAW_ZDJTABL, OnDrawZdjtabl)
	ON_COMMAND(ID_DRAW_ZDJTABL_EXCE, OnDrawZdjtablExce)
	ON_COMMAND(ID_SUM_EXCEL, OnSumExcel)
	ON_COMMAND(ID_DRAW_ZDJDIR_EXECL, OnDrawZdjdirExecl)
	ON_COMMAND(ID_FILE_OPEN_DB, OnFileOpenDb)
	ON_COMMAND(ID_TOOL_WBLOCK, OnToolWblock)
	ON_COMMAND(ID_TOOL_INSERTUSERBLOCK, OnToolInsertUserBlock)
	ON_UPDATE_COMMAND_UI(ID_TOOL_WBLOCK, OnUpdateToolWblock)
	ON_UPDATE_COMMAND_UI(ID_TOOL_INSERTUSERBLOCK, OnUpdateToolInsertuserblock)
	ON_COMMAND(ID_PDMSTOPHS, OnPdmstophs)
	ON_WM_GETMINMAXINFO()
	ON_WM_MOVING()
	ON_COMMAND(ID_PHSTOPDMS, OnPhstToPdms)
	ON_UPDATE_COMMAND_UI(ID_PDMSTOPHS, OnUpdatePdmsToPhs)
	ON_UPDATE_COMMAND_UI(ID_PHSTOPDMS, OnUpdatePhsToPdms)
	ON_COMMAND(ID_CAL_STRUCT, OnCalStruct)
	ON_WM_CLOSE()
	ON_UPDATE_COMMAND_UI(ID_INT_IMPDATA, OnUpdateIntImpdata)
	ON_COMMAND(ID_MODIFY_ZDJH, OnModifyZdjh)
	ON_COMMAND(ID_GUIDE_ENTER,OnGuideEnter)
	ON_COMMAND(ID_ZDJ_XN, OnZdjXn)
	ON_COMMAND(ID_FILE_BACKUP_DB, OnFileBackupDB)
	ON_COMMAND(ID_FILE_RESTORE_DB, OnFileRestoreDB)
	ON_COMMAND(ID_AUTO_UPDATE, OnAutoUpdate)
	ON_MESSAGE(WM_QUIT_CUS,OnQuitCus)
	ON_COMMAND(ID_DRAW_ZDJ_ARX, OnDrawZdjArx)
	ON_UPDATE_COMMAND_UI(ID_DRAW_ZDJ, OnUpdateDrawZdj)
	ON_COMMAND(ID_ZDJ_DRAWEP_HSY, OnZdjDrawepHsy)
	ON_COMMAND(ID_MENUITEM_CHANGMATERIAL, OnMenuitemChangmaterial)
	ON_COMMAND(ID_ZDJ_DRAWEP_QGY, OnZdjDrawepQgy)
	ON_COMMAND(ID_ZDJ_HBYJ, OnZdjHbyj)
	ON_COMMAND(ID_PHS_TO_AUTOIPED, OnPhsToAutoiped)
	ON_COMMAND(ID_COMMDBOPER, OnCommdboper)
	ON_COMMAND(IDC_ZDJ_ADRAWBJHY, OnZdjAdrawbjhy)
	ON_COMMAND(IDC_ZDJ_BDRAWBJHY, OnZdjBdrawbjhy)
	ON_COMMAND(IDC_MENU_ImportAutoPHS, OnMENUImportAutoPHS)
	ON_COMMAND(IDM_PLANTVISION_TO_PHS, OnPlantvisionToPhs)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVEAS, OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateFileNewOpen)
	ON_UPDATE_COMMAND_UI(ID_FILE_STARTACAD, OnUpdateFileNewOpen)
	ON_COMMAND(ID_MENUITEM_OUTPUTPDS, OnMenuitemOutputpds)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_UPD_OPTION , OnUpdOption)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	m_nRecFileNum=0;
	m_bIsExitMsg=true;
	
}

//CMainFrame::~CMainFrame()
//{
//}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass( 0,::LoadCursor(::AfxGetInstanceHandle(),IDC_ARROW),
													(HBRUSH)COLOR_WINDOW,::LoadIcon(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME)));
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::OnSetFocus(CWnd* pOldWnd) 
{
	CFrameWnd::OnSetFocus(pOldWnd);
	
	// TODO: Add your message handler code here
	
}

void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	//CRect rc1;
	//::SystemParametersInfo(SPI_GETWORKAREA,0,(LPVOID)&rc1,0);
	//this->SetWindowPos(NULL,0,0,rc1.Width(),GetMenuHeight(),SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
}

LRESULT CMainFrame::OnQuitCus(WPARAM wParam,LPARAM lParam)
{
	exit(1);
}
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{

	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	

	return 0;
}

void CMainFrame::OnIntTabint() 
{
	// TODO: Add your command handler code here
	
	if(FrmPhsData.m_hWnd==NULL)
	{
		FrmPhsData.Create(IDD_PHSDATA,NULL); 
	}
	FrmPhsData.ShowWindow(SW_SHOWNORMAL );
	FrmPhsData.SetWindowPos(&wndTop,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
}



void CMainFrame::OnDestroy() 
{
	SetRegValue(_T("Settings"),_T("MainWndHandle"),CString());
	ModEncrypt::EncryptLogout();
	if(IsWindow(FrmPhsData.m_hWnd))
		FrmPhsData.DestroyWindow();
	if(IsWindow(frmStatus.m_hWnd))
			frmStatus.DestroyWindow();
	if(IsWindow(FrmDataEDIT.m_hWnd))
		FrmDataEDIT.DestroyWindow();
	if(IsWindow(frmSelPDSV.m_hWnd))
		frmSelPDSV.DestroyWindow();
	// TODO: Add your message handler code here
	if(Cavphs != NULL)
	{
		delete Cavphs;
		Cavphs = NULL;
	}

//  delete Cavphs;
	if(IsWindow(FrmTxsr.m_hWnd))
		FrmTxsr.DestroyWindow();
	EDIBgbl::CloseAll_ConnectionPtr();
	/*try{
		if(modPHScal::dbZDJcrude.IsOpen())
		{
			modPHScal::dbZDJcrude->Close();
		}
	}
	catch(_com_error& e)
	{
		ShowMessage((char*)e.Description());
	}*/
	CFrameWnd::OnDestroy();
}

void CMainFrame::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	
	CFrameWnd::OnTimer(nIDEvent);
}

void CMainFrame::SetMenuEnabled()
{

}

void CMainFrame::OnFileNewDwg() 
{
	// TODO: Add your command handler code here
	this->DDEFileCmd(0);
}

void CMainFrame::OnSizing(UINT fwSide, LPRECT pRect) 
{
	CFrameWnd::OnSizing(fwSide, pRect);
	// TODO: Add your message handler code here
	CRect rc;
	GetWindowRect(&rc);
	pRect->left=rc.left;
	pRect->top=rc.top;
	pRect->bottom=rc.bottom;
	pRect->right=rc.right;
}
void CMainFrame::OnIntImgint() 
{
	// TODO: Add your command handler code here
	FrmTxsr.m_IsFirst=false;//pfg20050216
	if(!IsWindow(FrmTxsr.m_hWnd))
	{
		FrmTxsr.Create(this);
	}
	if(IsWindow(FrmTxsr.m_hWnd))
	{
		FrmTxsr.ShowWindow(SW_SHOWNORMAL);
		FrmTxsr.SetWindowPos(&wndTop,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	}
}

void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CFrameWnd::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	CRect rc1;
	::SystemParametersInfo(SPI_GETWORKAREA,0,(LPVOID)&rc1,0);
	this->SetWindowPos(NULL,0,0,rc1.Width(),GetMenuHeight(),SWP_NOZORDER|SWP_NOACTIVATE);
}

void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CFrameWnd::OnActivate(nState, pWndOther, bMinimized);
	
}

void CMainFrame::OnSelPDSV() 
{
	// TODO: Add your command handler code here
		modPHScal::SelInputMethod = 2;
	   	CSelEngVolDll dlg;
		LPTSTR lpstrAllPrjDBPathName;
		LPTSTR lpstrsortPathName;
		LPTSTR lpstrworkprjPathName;
		//20071105 " basDirectory::DBShareDir+"AllPrjDB.mdb" " 改为 "basDirectory::DBShareDir"
		CString strAllPrjDBPathName = basDirectory::DBShareDir + _T("AllPrjDB.mdb");//20071025 "ProjectDBDir" 改为 "DBShareDir"
		//20071105 " basDirectory::DBShareDir+"DrawingSize.mdb" " 改为 " basDirectory::DBShareDir "
		CString strsortPathName = basDirectory::DBShareDir + _T("DrawingSize.mdb");//20071025 "Sort.mdb" 改为 "DrawingSize.mdb"
		CString strworkprjPathName = basDirectory::ProjectDir+"workprj.mdb"  ;

     	lpstrAllPrjDBPathName=(LPTSTR)(LPCTSTR)(strAllPrjDBPathName);
		lpstrsortPathName=(LPTSTR) (LPCTSTR) (strsortPathName);
		lpstrworkprjPathName=(LPTSTR) (LPCTSTR) (strworkprjPathName);
		if(dlg.ShowEngVolDlg(lpstrworkprjPathName,lpstrAllPrjDBPathName,lpstrsortPathName)==false)
		{
			return;
		}
		EDIBgbl::SelSpecID=dlg.m_SelSpecID;
		EDIBgbl::SelPrjID=dlg.m_SelPrjID;
		EDIBgbl::SelDsgnID=dlg.m_SelDsgnID;
		EDIBgbl::SelHyID=dlg.m_SelHyID;
		EDIBgbl::SelVlmID=dlg.m_SelVlmID;
		EDIBgbl::SelJcdm=dlg.m_SelVlmCODE;
		CMenu* men=this->GetMenu();
		men->EnableMenuItem(0,MF_ENABLED);


		//取卷中的有关信息
		_RecordsetPtr selSVPtr;
	    selSVPtr.CreateInstance(__uuidof(Recordset));
		CString strSQL;
		CString str;
		_variant_t key;

		str.Format("%ld",(EDIBgbl::SelVlmID));
		strSQL=("SELECT *   FROM [Volume] WHERE volumeid="+str+"");
	    try
		{
			selSVPtr->Open(_bstr_t(strSQL),(IDispatch*)conPRJDB,adOpenKeyset,adLockOptimistic,adCmdText);
			key=selSVPtr->GetCollect("jcdm");
			if(key.vt==VT_EMPTY)
			{
				EDIBgbl::SelJcdm="";	
			}
			else
			{
				EDIBgbl::SelJcdm=key.bstrVal;
			}
			key=selSVPtr->GetCollect("jcmc");
			if(key.vt==VT_EMPTY)
			{
				EDIBgbl::SelVlmName="";
			}
			else
			{
				EDIBgbl::SelVlmName=key.bstrVal;
			}
		}
		catch(_com_error e)
		{
			AfxMessageBox(e.ErrorMessage());
			return;
		}
		//取工程中的有关信息
		strSQL="select * from Engin where enginid='"+EDIBgbl::SelPrjID+"'";
    	try
		{
			if(selSVPtr->State==adStateOpen)
			{selSVPtr->Close();}
			selSVPtr->Open(_bstr_t(strSQL),(IDispatch*)conPRJDB,adOpenKeyset,adLockOptimistic,adCmdText);
			key=selSVPtr->GetCollect("gcmc");
			if(key.vt==VT_EMPTY)
			{
				EDIBgbl::SelPrjName="";
			}
			else
			{
				EDIBgbl::SelPrjName=key.bstrVal;
			}
			key=selSVPtr->GetCollect("e_gcmc");
			if(key.vt==VT_EMPTY||key.vt==VT_NULL)
			{
				EDIBgbl::SelPrjEnglishName="";
			}
			else
			{
				EDIBgbl::SelPrjEnglishName=key.bstrVal;
			}
			key=selSVPtr->GetCollect("UnitNum");
			EDIBgbl::giUnitNum=(key.vt==VT_EMPTY)?0:key.iVal;
		}
		catch(_com_error e)
		{
			AfxMessageBox(e.ErrorMessage());
			return;
		}
		if(selSVPtr->State==adStateOpen)
		{
			selSVPtr->Close();
		}
		//取行业中的相关信息,设计阶段,专业的相关信息

		_RecordsetPtr DataCurrWork;
		DataCurrWork.CreateInstance(__uuidof(Recordset));

		_RecordsetPtr DataCategory;//行业
		DataCategory.CreateInstance(__uuidof(Recordset));
		_RecordsetPtr DataSpe;//专业
		DataSpe.CreateInstance(__uuidof(Recordset));
		_RecordsetPtr DataDsgn;//设计阶段
		DataDsgn.CreateInstance(__uuidof(Recordset));
		CString str_sjhyid;
		str_sjhyid.Format("%ld",(EDIBgbl::SelHyID));
		_variant_t tmpVar;
		CString str_zyid;
		str_zyid.Format("%ld",(EDIBgbl::SelSpecID));
		try
		{
			//行业
			CString SQLx = _T("SELECT * FROM DrawSize where sjhyid="+str_sjhyid+"");
			DataCategory->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbDSize,true), 
				adOpenKeyset, adLockOptimistic, adCmdText); 
			
			DataCategory->get_Collect((_variant_t)(_T("SJHYIndex")), &tmpVar);
			EDIBDB::SJHYIndex=vtoi(tmpVar);
			//专业
			DataSpe->Open((_bstr_t)(_T("select * from speciality where zyid="+str_zyid+"")),_variant_t((IDispatch*)EDIBgbl::dbDSize,true), 
				adOpenKeyset, adLockOptimistic, adCmdText); 
			DataSpe->get_Collect((_variant_t)_T("ZYMC"),&tmpVar);
			EDIBgbl::SelSpecName = vtos(tmpVar);
			DataSpe->get_Collect((_variant_t)_T("zydm"),&tmpVar);
			EDIBgbl::strSelSpec=vtos(tmpVar);
			//设计阶段
			CString strDsgn;
			strDsgn.Format("%ld",(EDIBgbl::SelDsgnID));
			DataDsgn->Open((_bstr_t)(_T("select * from designstage where sjjdid="+strDsgn+"")),_variant_t((IDispatch*)EDIBgbl::dbDSize,true), 
				adOpenKeyset, adLockOptimistic, adCmdText); 
			DataDsgn->get_Collect((_variant_t)_T("sjjdmc"),&tmpVar);
			EDIBgbl::SelDsgnName =vtos(tmpVar);
			DataDsgn->get_Collect((_variant_t)_T("sjjddm"),&tmpVar);
			EDIBgbl::strSelDsgn=vtos(tmpVar);
			
			//保存当的结果
			DataCurrWork->Open((_bstr_t)_T("Select * From CurrentWork"),_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
				adOpenKeyset, adLockOptimistic, adCmdText); 
			
			_RecordsetPtr m_Rs;
			m_Rs.CreateInstance(__uuidof(Recordset));
			m_Rs=DataCurrWork;
			DataCategory->get_Collect((_variant_t)_T("sjhyid"),&tmpVar);
			m_Rs->put_Collect((_variant_t)_T("sjhyindex"),tmpVar);

			DataCategory->get_Collect((_variant_t)_T("sjhy"),&tmpVar);
			m_Rs->put_Collect((_variant_t)_T("sjhy"),tmpVar);

			DataDsgn->get_Collect((_variant_t)_T("sjjddm"),&tmpVar);
			m_Rs->put_Collect((_variant_t)_T("sjjddm"),tmpVar);

			DataDsgn->get_Collect((_variant_t)_T("sjjdmc"),&tmpVar);
			m_Rs->put_Collect((_variant_t)_T("sjjdmc"),tmpVar);

			DataSpe->get_Collect((_variant_t)_T("zydm"),&tmpVar);
			m_Rs->put_Collect((_variant_t)_T("zydm"),tmpVar);

			DataSpe->get_Collect((_variant_t)_T("zymc"),&tmpVar);
			m_Rs->put_Collect((_variant_t)_T("zymc"),tmpVar);
			m_Rs->Update();

		}
		catch(_com_error e)
		{
			AfxMessageBox(e.ErrorMessage());
			return;
		}


		if( EDIBgbl::SelHyID < 0 ) EDIBgbl::SelHyID = modSelPDSV::defHyID;
		if( EDIBDB::SJHYIndex < 0 ) EDIBDB::SJHYIndex = modSelPDSV::defSJHYIndex;
		if( EDIBgbl::CompanyID == _T("") ) EDIBgbl::CompanyID = _T("43");

		SetRegValue(_T("RecentWork"), user::gstrAppName + _T("_PrjID"), EDIBgbl::SelPrjID);
		SetRegValue(_T("RecentWork"), user::gstrAppName + _T("_DsgnID"), EDIBgbl::SelDsgnID);
		SetRegValue(_T("RecentWork"), user::gstrAppName + _T("_SpecID"), EDIBgbl::SelSpecID);
		SetRegValue(_T("RecentWork"), user::gstrAppName + _T("_HyID"), EDIBgbl::SelHyID);
		SetRegValue(_T("RecentWork"), user::gstrAppName + _T("_VlmID"), EDIBgbl::SelVlmID);
		SetRegValue(_T("RecentWork"), user::gstrAppName + _T("_Dsgn"), EDIBgbl::strSelDsgn);
		SetRegValue(_T("RecentWork"), user::gstrAppName + _T("_Spec"), EDIBgbl::strSelSpec);
		SetRegValue(_T("RecentWork"), user::gstrAppName + _T("_BillID"), EDIBgbl::SelBillType);
		SetRegValue(_T("RecentWork"), _T("SJHY"), EDIBDB::SJHY);
		SetRegValue(_T("RecentWork"), _T("SJHYIndex"), EDIBDB::SJHYIndex);

		EDIBgbl::InitDBTBN();
		FrmPhsData.ShowWindow(SW_SHOW);
		AfxGetApp()->EndWaitCursor();
		CString ss=EDIBgbl::gsTitle+(EDIBgbl::TBNSelPrjSpec == _T("") ? _T("") : (_T("    ") + EDIBgbl::TBNSelPrjSpec + EDIBgbl::Btype[EDIBgbl::SelBillType] + _T(" (") + EDIBgbl::SelJcdm + _T(")")));
		AfxGetApp()->m_pMainWnd->SetWindowText(ss);
		FrmPhsData.SetWindowText(EDIBgbl::Cbtype[EDIBgbl::SelBillType].MnuCaption + "    " + EDIBgbl::TBNSelPrjSpec + EDIBgbl::Btype[EDIBgbl::SelBillType] + " (" + EDIBgbl::SelJcdm + ")");//pfg20050303
}

void CMainFrame::OnSampleManage() 
{
	// TODO: Add your command handler code here
	if (EDIBgbl::SelBillType != EDIBgbl::TZA)
		return;
   //选择模板之前必须处理数据，以便获得管径、荷载、温度等必要数据。
// 	try
// 	{
		if(FrmTxsr.m_pViewTxsr->m_ActiveRs!=NULL && FrmTxsr.m_pViewTxsr->m_ActiveRs->State!=adStateClosed )
		{
			if(FrmTxsr.m_pViewTxsr->m_ActiveRs->adoEOF && FrmTxsr.m_pViewTxsr->m_ActiveRs->BOF)
				return;
			if( !FrmTxsr.m_pViewTxsr->m_ActiveRs->adoEOF && !FrmTxsr.m_pViewTxsr->m_ActiveRs->BOF)
				FrmTxsr.m_pViewTxsr->m_ActiveRs->Update();
			modPHScal::zdjh=vtoi(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("zdjh")));
			modPHScal::iSelSampleID=vtoi(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("iSelSampleID")));
			if(modPHScal::iSelSampleID<=0)
			{
				modPHScal::iSelSampleID=1;
				FrmTxsr.m_pViewTxsr->m_ActiveRs->PutCollect(_T("iSelSampleID"),_variant_t((long)1));
				FrmTxsr.m_pViewTxsr->m_ActiveRs->Update();
			}
			try
			{
				modPHScal::PreCalCrudeData(FrmPhsData.m_DataBillRs);
			}
			catch(_com_error e)
			{
				CString strMsg;
				strMsg.Format(_T("%d: %s"), __LINE__, e.Description());
				ShowMessage(strMsg);
			}

			CSelTemplate dlg;
			dlg.DoModal();
		}
// 	}
// 	catch(_com_error e)
// 	{
// 		CString strMsg;
// 		strMsg.Format(_T("%d: %s"), __LINE__, e.Description());
// 		ShowMessage(strMsg);
// 	}
}

void CMainFrame::OnInputSArec() 
{
	// TODO: Add your command handler code here
	CFrmAddSArecord addDlg;
	addDlg.DoModal();
	FrmTxsr.m_pViewTxsr->m_PagOption->LoadSAG100();
}


void CMainFrame::OnCalcZdjh() 
{

	EDIBgbl::IsCalc=true;//正在计算
	FrmTxsr.m_IsFirst=false;
	if(EDIBgbl::SelBillType != EDIBgbl::TZA)
	{
		MessageBox(GetResStr(IDS_PleaseSelectTZA));
		return;
	}
    if(!ModEncrypt::gbLegalUser)
	{
		CTime tm=CTime::GetCurrentTime();
        if(tm.GetSecond()% 12 !=0)
		{
            //非法用户，只有1/8的时间可以计算
			MessageBox(GetResStr(IDS_DemoVersionOutDate));
			return;
		}
	}
	try
	{
		if (FrmTxsr.m_pViewTxsr->m_ActiveRs->BOF || FrmTxsr.m_pViewTxsr->m_ActiveRs->adoEOF) return;
		//第k个开始连续计算n个支吊架
		AfxGetApp()->BeginWaitCursor();
		if(FrmTxsr.m_pViewTxsr->m_bActive)
			FrmTxsr.m_pViewTxsr->UpdateBoundData();
		frmStatus.ShowWindow(SW_SHOW);
		CString sTmp;
		modPHScal::zdjh=vtoi(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("zdjh")));
		sTmp.Format(_T("%d"),modPHScal::zdjh);
		frmStatus.m_Label1.Format(GetResStr(IDS_CalculatingNoXphs),sTmp);// _T("正在计算") & zdjh & _T("号支吊架")
		modPHScal::iSelSampleID=vtoi(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("iSelSampleID")));
		sTmp.Format(_T("%d"),modPHScal::iSelSampleID);
		frmStatus.m_Label2.Format(GetResStr(IDS_ApplyingSampleIDxPhs),sTmp);//_T("正在使用") & iSelSampleID & _T("号样本支吊架组合结构")
		frmStatus.SetWindowText(GetResStr(IDS_PhsCalculation));//' _T("计算支吊架")
		frmStatus.UpdateData(false);
		frmStatus.UpdateStatus(0.02,true);
		frmStatus.UpdateWindow();
		modPHScal::VB_Cal(FrmTxsr.m_pViewTxsr->m_ActiveRs,modPHScal::zdjh,frmStatus);
		frmStatus.UpdateWindow();
		AfxGetApp()->EndWaitCursor();
		frmStatus.ShowWindow(SW_HIDE);
		AfxGetApp()->EndWaitCursor();
	}
	catch(CException *e)
	{
		e->Delete();
		AfxGetApp()->EndWaitCursor();
		frmStatus.ShowWindow(SW_HIDE);
	}
	catch(...)
	{
		AfxGetApp()->EndWaitCursor();
		frmStatus.ShowWindow(SW_HIDE);
	}

	if( FrmDataEDIT.m_hWnd == this->GetActiveWindow()->m_hWnd )
	{
		FrmDataEDIT.CloseWindow ();
		FrmDataEDIT.DestroyWindow();

		EDIBgbl::gbDisplayBOM = true;
		FrmDataEDIT.m_bAllowUpd=true;
		if(!IsWindow(FrmDataEDIT.m_hWnd))
		{
			FrmDataEDIT.Create(IDD_DATAEDIT,AfxGetApp()->m_pMainWnd);
			FrmDataEDIT.UpdateWindow();
		}
		::SetWindowPos(FrmDataEDIT.GetSafeHwnd(),HWND_TOP,0,0,0,0,/*SWP_NOACTIVATE|*/SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
	}
}



//-----------------------------------------------------------
// Function Name:OnDrawZdj
// Parameter(s) :void
// Return       :void
// Memo         :
// Author       :WangJia
// Modify       :在画图前后修改注册表中的值
//-----------------------------------------------------------
void CMainFrame::OnDrawZdj() 
{

	CString szGUID = _T("{5EFC7975-14BC-11CF-9B2B-00AA00573819}");
	HANDLE hSem = CreateSemaphore( NULL, 1, 1, szGUID); 
	//////////////////////////////////////////////////////////////////////////
	

	static long n=0;
	try
	{
		CString sTmp,sTmp2;
		modPHScal::zdjh=(long)FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("zdjh"));
		if(vtob(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("bCalSuccess")))==false)
		{
			CString strMsg;
			strMsg.Format(IDS_NO_DRAWPHS,modPHScal::zdjh);
			ShowMessage(strMsg);
			::CloseHandle(hSem); 
			return;
		}

		modPHScal::iSelSampleID=(long)FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("iSelSampleID"));
		if(!EDIBAcad::StartAcad())
		{
			::CloseHandle(hSem); 
			return;
		}
		
		EDIBAcad::DisplayAcadTop();

		//如果当前打开的图形不是(PhsBlkDir & _T("phs.dwg")),需要重新打开
		//因为后面的操作需要大量的图形块准备好

		basDirectory::PhsBlkDir.MakeUpper();
		EDIBAcad::GetActiveAcadDoc();
		
		modPHScal::SetBasePoint();

		EDIBAcad::EraseAllEntities();

		modPHScal::DrawPhs(FrmTxsr.m_pViewTxsr->m_ActiveRs);
		


		if(modPHScal::gbAutoSaveDrawing)
		{
			sTmp=vtos(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("GN1")));
			sTmp.MakeUpper();
			if(sTmp!=_T("G100"))
				DDEFileCmd(SaveDwg);
		}
		EDIBAcad::SetAcadTop();
	}
	catch(COleException* e)
	{
		e->ReportError();
		e->Delete();
	}
	catch(CException *e)
	{
		e->ReportError();
		e->Delete();
	}
	catch(_com_error e)
	{		
		MessageBox(e.Description());
	}
	//////////////////////////////////////////////////////////////////////////
	::CloseHandle(hSem); 
}

long CMainFrame::GetMenuHeight()
{
	CRect rc1,rc2;
	this->GetWindowRect(&rc1);
	this->GetClientRect(&rc2);
	return rc1.Height()-rc2.Height() + ::GetSystemMetrics(SM_CYBORDER);
}

void CMainFrame::InitPos()
{
	CRect rc1; 
	::SystemParametersInfo(SPI_GETWORKAREA,0,(LPVOID)&rc1,0);
	this->SetWindowPos(NULL,0,0,rc1.Width(),GetMenuHeight(),SWP_NOZORDER|SWP_NOACTIVATE);
}

void CMainFrame::DDEFileCmd(DWORD cmd)
{
	try
	{
		CString strTmp,strTmp2;
		bool bExist=false;
		switch(cmd)
		{
		case NewDwg:
			EDIBAcad::StartAcad();
			break;
		case OpenDwg:
			//打开当前支吊架图形，便于观察和打印绘制。如果图形不存在，打开phs.dwg。
			modPHScal::gsDwgFN = modPHScal::GetSaveAsDwgFileName(modPHScal::zdjh);
			if( modPHScal::gsDwgFN != _T("") )
			{
				//获取文件名成功，不为空
				if( FileExists(modPHScal::gsDwgFN))
				{
					//文件存在
					EDIBAcad::StartAcad(modPHScal::gsDwgFN);
					modPHScal::giZDJHinDwg =(long) EDIBAcad::objAcadDoc.Invoke(_T("GetVariable"),1,&_variant_t(_T("USERI5")));
				}
				else
				{
					//文件不存在
					EDIBAcad::StartAcad();
				}
			}
			else
			{
				modPHScal::gsDwgFN = user::SelFileDwg(GetResStr(IDS_MsgBox_60573), basDirectory::ProjectDWGDir);
				if( modPHScal::gsDwgFN != _T("") )
				{
					EDIBAcad::StartAcad(modPHScal::gsDwgFN);
					modPHScal::giZDJHinDwg = (long)EDIBAcad::objAcadDoc.Invoke(_T("GetVariable"),1,&_variant_t(_T("USERI5")));
				}
				else
					EDIBAcad::StartAcad();
			}
			break;
		case SaveDwg:
		case SaveAsDwg:
			if( modPHScal::gsDwgFN != _T("") )
			{
				//pfg20050331
				if((cmd==2)&&FileExists(modPHScal::gsDwgFN))
				{
					if(AfxMessageBox("警告：\n"+modPHScal::gsDwgFN+"文件已经存在，是否覆盖!",1)!=IDOK)
					{
						return;
					}
					else
					{
						bExist=true;
					}
				}
				//必须首先把手工修改的图形保存到m.dxf文件中，因为写块时会丢失m_phs0层
				EDIBAcad::AddEntities2SSetInLayers(acSelectionSetAll,1, _T("m_phs0"));

				if(!DirExists(basDirectory::ProjectDWGDir)) 
					if(!CreateDirectory(basDirectory::ProjectDWGDir,NULL))
						return;
				EDIBAcad::objAcadDoc=EDIBAcad::objAcadApp.GetPropertyByName(_T("ActiveDocument")).pdispVal;
				strTmp=vtos(EDIBAcad::objAcadDoc.Invoke(_T("GetVariable"),1,&_variant_t(_T("DWGPREFIX"))));
				strTmp2=vtos(EDIBAcad::objAcadDoc.Invoke(_T("GetVariable"),1,&_variant_t(_T("DWGNAME"))));
				user::AddDirSep(strTmp);
				strTmp.MakeUpper();
				strTmp2.MakeUpper();
				modPHScal::gsDwgFN.MakeUpper();
				if(bExist&&modPHScal::gsDwgFN.Find(strTmp2)==-1)
				{
					try
					{

						CMObject docs,doc;
						docs = EDIBAcad::objAcadApp.GetPropertyByName ( _T ( "Documents" ) ).pdispVal;
						long count = docs.GetPropertyByName( _T("Count") );
						for ( long n = 0; n < count; n++ )
						{
							doc=docs.Invoke("Item",1,&_variant_t(n)).pdispVal;
							CString str=vtos((_variant_t)doc.GetPropertyByName("FullName"));
							if ( str.CollateNoCase (modPHScal::gsDwgFN) == 0 )
							{
								doc.Invoke(_T("Close"),0,&_variant_t()); 
								break;
							}

						}
					}
					catch(CException *e)
					{
						e->Delete();
					}
					CFile::Remove(modPHScal::gsDwgFN);
				}
				EDIBAcad::objAcadDoc.Invoke(_T("SetVariable"),2,&_variant_t(_T("DWGNAME")),&_variant_t(modPHScal::gsDwgFN));
				
				if(strTmp+strTmp2 == modPHScal::gsDwgFN)
				{
					EDIBAcad::objAcadDoc.Invoke0(_T("Save"));
				}
				else
				{//2004.08.11由杨武辉改造完成以后，
					//存盘支吊架图形变得简单了。added by ligb on 2004.08.11
			
					EDIBAcad::objAcadDoc.Invoke(_T("SaveAs"),1, &_variant_t(modPHScal::gsDwgFN));
					AddRecentFiles(modPHScal::gsDwgFN);
					LoadRecentFiles();
				}
			}
			break;
		case SaveAsSampleDwg:
			{
				CString sSampleDwg,sTmp;
				sTmp.Format(_T("%4d"),modPHScal::iSelSampleID);
				sSampleDwg = basDirectory::TemplateDir + sTmp + _T(".dwg");
				EDIBAcad::AddEntities2SSetInLayers(acSelectionSetAll,5, _T("bom"), _T("dim"), _T("phs"), _T("tag"), _T("tk"));
			    if (FileExists(sSampleDwg) )
				{
					if(MessageBox(GetResStr(IDS_MsgBox_60574) + sSampleDwg + GetResStr(IDS_MsgBox_60575), NULL,MB_ICONQUESTION|MB_DEFBUTTON1|MB_YESNO|MB_SYSTEMMODAL)==IDYES)
						EDIBAcad::objAcadDoc.Invoke(_T("Wblock"),2,&_variant_t(sSampleDwg), &_variant_t(EDIBAcad::sset.p));
				}
				else
					EDIBAcad::objAcadDoc.Invoke(_T("Wblock"),2,&_variant_t(sSampleDwg), &_variant_t(EDIBAcad::sset.p));
			}
			break;
		case OutPutPhsBlock:
			EDIBAcad::WblockFromPhsToPhsBlkDir(basDirectory::ProjectDir);
		}
	}
	catch(CException *e)
	{
		e->Delete();
	}
}

void CMainFrame::OnStartacad() 
{
	// TODO: Add your command handler code here
	EDIBAcad::StartAcad();
}

void CMainFrame::OnSelPreferences() 
{
	// TODO: Add your command handler code here
	ShowModalSelSpecification(FALSE);
	//EDIBgbl::InitPhsStructureREFandName();
}

void CMainFrame::OnFileOpt() 
{
	// TODO: Add your command handler code here
	CFrmPreferences dd;
	dd.DoModal();
}

void CMainFrame::OnIntImpdata() 
{
	// TODO: Add your command handler code here
	CFrmDatabaseIn dd;
	dd.DoModal();
}

void CMainFrame::OnSetTableFormat() 
{
	// TODO: Add your command handler code here
	CDlgDataFormatDll dlgFormat;
	//显示对话框之前，必须对其指定数据库的全路径名。
	LPTSTR lpstrDB;
	CString strDB;
	strDB=basDirectory::DBShareDir;//20071016"sort.mdb"改为"TableFormat.mdb" //20071024 "ProjectDBDir" 改为 "DBShareDir"
	lpstrDB=(LPTSTR) (LPCTSTR) strDB;
	dlgFormat.ShowFormatDlg(lpstrDB);	
}

void CMainFrame::SumCL(int Index)
{
	AfxGetApp()->BeginWaitCursor();
	USES_CONVERSION;
	try
	{
		if(!FrmTxsr.m_pViewTxsr->m_ActiveRs->adoEOF && !FrmTxsr.m_pViewTxsr->m_ActiveRs->BOF)
			//表格窗口失去焦点，更新当前支吊架号
			modPHScal::zdjh=vtoi(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("Zdjh")));
		static bool bExcelStart =false;   //Excel已经启动
		double X =0;long i=0;
		CString strTbn, strXls;
		_ConnectionPtr db;
		db.CreateInstance(__uuidof(Connection));
		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(Recordset));
		_RecordsetPtr rs1;
		CString BOM_Format_phs;
		if( !ModEncrypt::gbLegalUser )
		{
			ShowMessage(GetResStr(IDS_IllegalUserOrDemoVerCantOutPutBom));
			return;
		}
		CString strTmp;
		//判断是否所有支吊架计算完成。
		try
		{
			if(!FrmTxsr.m_pViewTxsr->m_ActiveRs->adoEOF && !FrmTxsr.m_pViewTxsr->m_ActiveRs->BOF)
				FrmTxsr.m_pViewTxsr->m_ActiveRs->Update();
		}
		catch(CException *e)
		{
			e->Delete();
		}

		CString SQLx = _T("SELECT count(*) FROM [") + EDIBgbl::Btype[EDIBgbl::TZA] + _T("] WHERE ( bCalSuccess=0  OR bCalSuccess IS NULL )");
		
		if( modPHScal::gbSumAllVolume )
		{
			SQLx +=_T(" AND VolumeID IN ( SELECT VolumeID FROM Volume WHERE EnginID=\'") + EDIBgbl::SelPrjID + _T("\' AND ZYID = ") + ltos(EDIBgbl::SelSpecID) + _T(" AND SJJDID = ") + ltos(EDIBgbl::SelDsgnID) + _T(" AND SJHYID = ") + ltos(EDIBgbl::SelHyID) +_T(") ");
		}
		else
		{
			SQLx +=_T(" AND VolumeID =") + ltos(EDIBgbl::SelVlmID);
		}

		long i1,i2;
		rs1.CreateInstance(__uuidof(Recordset));

		rs1->Open(_variant_t(SQLx),(IDispatch*)::conPRJDB,adOpenForwardOnly,adLockReadOnly,adCmdText);
		i1=vtoi(rs1->GetCollect(_variant_t((long)0)));
		rs1->Close();

		SQLx = _T("SELECT count(*) FROM [") + EDIBgbl::Btype[EDIBgbl::TZA] + _T("] WHERE ");
		if(modPHScal::gbSumAllVolume )
		{
			SQLx +=_T(" VolumeID IN ( SELECT VolumeID FROM Volume WHERE EnginID=\'") + EDIBgbl::SelPrjID + _T("\' AND ZYID = ") + ltos(EDIBgbl::SelSpecID) + _T(" AND SJJDID = ") + ltos(EDIBgbl::SelDsgnID) + _T(" AND SJHYID = ") + ltos(EDIBgbl::SelHyID) +_T(") ");
		}
		else
		{
			SQLx +=_T(" VolumeID =") + ltos(EDIBgbl::SelVlmID);
		}
		rs1->Open(_variant_t(SQLx),(IDispatch*)::conPRJDB,adOpenForwardOnly,adLockReadOnly,adCmdText);
		i2=vtoi(rs1->GetCollect(_variant_t((long)0)));
		rs1->Close();
		rs1=NULL;
		strTmp.Format(GetResStr(IDS_SomePhsNotCalSuccess),ltos(i2),ltos(i2-i1),ltos(i1));
		if( IDNO == ShowMessage(strTmp,MB_DEFBUTTON1|MB_ICONQUESTION|MB_YESNO) )
			return;
		CMObject objTmp;
		CCadPoint p0,p1;
		//生成所有或某个卷册的材料汇总表
		Cavphs->GetphsSumBom();
		CString strTmp2;
		BOOL flag = true;
		switch(Index)
		{
		case 0:
		case 1:
				//如果当前打开的图形不是(PhsBlkDir + _T("phs.dwg")),需要重新打开
				//因为后面的操作需要大量的图形块准备好
			if(EDIBAcad::objAcadApp.GetActiveObject(EDIBAcad::gstrApplicatonName)!=S_OK)
					flag = EDIBAcad::StartAcad();
				else
				{
					EDIBAcad::gsACADVER=vtos(EDIBAcad::objAcadApp.GetPropertyByName("Version"));
					if(EDIBAcad::gsACADVER.Left(2)==_T("14"))
					{
						EDIBAcad::gbACADR14 = true;
					}
					else 
					{
						EDIBAcad::gbACADR14 = false;
					}
				}

				if(flag == FALSE)
					break;
				EDIBAcad::GetActiveAcadDoc();
				BOM_Format_phs = _T("ACAD");
				//下句接受材料汇总表起始图号，但程序目前实际未采用该值
				EDIBgbl::gStartPage = modPHScal::iBOMStartNo();
				EDIBDB::DrawID = EDIBAcad::GetDrawIDAndName(modPHScal::zdjh + modPHScal::glDrawNoStartNumber - 1, EDIBDB::DrawName);
				
				ShowMessage(GetResStr(IDS_PleasePickApointAsBOMInsertPointINAutoCAD));

				EDIBAcad::SetAcadTop();

				//传递正确的格式参数
				BOM_Format_phs = _T(";");
				EDIBDB::OutPutTable(_T("tmpPhsBOM1"), EDIBgbl::GetDBName(EDIBgbl::dbPRJ), _T(""), EDIBgbl::dbPRJ, _T("tmp2"), BOM_Format_phs);
				//对tmpPhsBOM1表编号
				SQLx = _T("SELECT sum(CLzz) as SumWeight FROM tmpPhsBOM1");
				rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
					adOpenKeyset, adLockOptimistic, adCmdText); 
				if( rs->adoEOF || rs->BOF ){
					rs->Close();
				}else {
					EDIBgbl::gsngSumBomWeight = vtof(GetFields(rs,_T("SumWeight")));
					SQLx = _T("SELECT * FROM tmpPhsBOM1");
					rs->Close();
					rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
						adOpenKeyset, adLockOptimistic, adCmdText); 
					i=0;

					//============汇总电焊条==============
					if( modPHScal::g_bDHT )
					{
						rs->AddNew();
						rs->put_Collect((_variant_t)"CLmc",STR_VAR("电焊条"));
						rs->put_Collect((_variant_t)"CLgg",STR_VAR(modPHScal::g_strDHTGG));
						rs->Update();
						rs->Close();
						rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
							adOpenKeyset, adLockOptimistic, adCmdText); 
					}
					//====================================

					while(!rs->adoEOF)
					{
						i++;
						rs->put_Collect((_variant_t)_T("SEQ"),_variant_t((long)i)); 
						rs->Update();
						rs->MoveNext();
					}
					rs->Close();
					SQLx = _T("SELECT * FROM TmpPhsBOM1");
					rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
						adOpenKeyset, adLockOptimistic, adCmdText); 
					if(i>0)
					{
						if( EDIBAcad::gbACADR14 || !bIsCom ) //使用动态库画图  zsy 12/30
						{
							rs->Close();
						}else
						{
							objTmp=EDIBAcad::objAcadDoc.GetPropertyByName(_T("Utility")).pdispVal;
							p0 = objTmp.Invoke(_T("GetPoint"),2,(LPVARIANT)p1,&_variant_t(GetResStr(IDS_PleasePickApointAsBOMInsertPointINAutoCAD)));
						}              //12/30
						EDIBAcad::DrawTableACAD(p0, (Index == 0 ? EDIBgbl::TCL : EDIBgbl::TCLA4), atan(1.0) * 0, rs, 1.0f);

					}

				}
				rs->Close();
				EDIBAcad::SetAcadNoTop();
				break;
		case 2:
			{
				EDIBDB::CloseExcel();
				BOM_Format_phs = _T("Excel 5.0;");
				strXls = basDirectory::ProjectDir + _T("phsBom") + EDIBgbl::SelPrjID + _T(".xls");
				
				if( FileExists(strXls) )
				{
					::DeleteFile( strXls );
				}

				if( !FileExists(strXls) )
				{
					if( FileExists(basDirectory::TemplateDir+_T("phsBomF4511.xls")) )
					{
						CopyFile(basDirectory::TemplateDir+_T("phsBomF4511.xls"),strXls,TRUE);
					}
					else
					{
						CString strInfo;
						strInfo.Format(IDS_NOTEXIST_EXCELFILE,basDirectory::TemplateDir,_T("phsBomF4511.xls"));
						AfxMessageBox(strInfo);

						AfxGetApp()->EndWaitCursor();
						return;
					}
				}

				_RecordsetPtr rs1;
				rs1.CreateInstance(__uuidof(Recordset));
				CString strPaintPathName, strTblName;
				SQLx = _T("SELECT BomIndex, BomName , BomTable FROM BOMname ORDER BY BomIndex");
				rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPHScode,true), 
					adOpenKeyset, adLockOptimistic, adCmdText); 
				if( rs->adoEOF || rs->BOF )
				{
				}
				else
				{
					int iBomIndex;
					while(!rs->adoEOF)
					{
						if( FileExists(strXls) )
						{
							db->Open((_bstr_t)strXls, "", "", adModeUnknown);
							strTbn = vtos(rs->GetCollect((_variant_t)"BomName"));
							if( EDIBgbl::tdfExists(db, strTbn) )
							{
								db->Execute((_bstr_t)(_T("DROP TABLE ") + strTbn), NULL, adCmdText);
							}
							db->Close();
						}
						i = 0;
						SQLx = _T("SELECT sum(CLzz) as SumWeight FROM tmp") + vtos(GetFields(rs,_T("BomTable")));
						rs1->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
							adOpenKeyset, adLockOptimistic, adCmdText); 
						if( rs1->adoEOF || rs1->BOF )
						{
							rs1->Close();
						}
						else
						{
							EDIBgbl::gsngSumBomWeight = vtof(GetFields(rs1,_T("SumWeight")));//, 0, rs1.Fields(_T("SumWeight")))
							SQLx = _T("SELECT SEQ FROM tmp") + vtos(GetFields(rs,_T("BomTable")));
							
							rs1->Close();
							rs1->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
								adOpenKeyset, adLockOptimistic, adCmdText); 
							if( rs1->adoEOF || rs1->BOF )
							{
							}
							else
							{
								while(!rs1->adoEOF)
								{
									i++;
									rs1->put_Collect((_variant_t)_T("SEQ"),_variant_t((long)i)); 
									rs1->Update();
									rs1->MoveNext();
								}
							}
							rs1->Close();
							//获得表类型,与保温油漆软件AutoIPED接口时,
							//只有3(管部汇总表),4(型钢零件汇总表用于下料),6(连接件汇总表用于制作),8(附件汇总表),9(螺栓螺母汇总表).须要处理.
							iBomIndex = vtoi(rs->GetCollect((_variant_t)"BomIndex"));

							if( !EDIBgbl::strExportPaint.IsEmpty() )
							{
								//重新写入到一个Excel文件中.
								switch( iBomIndex )
								{
								case    3:
								case	4:
								case	6:
								case	8:
								case	9:
									//strTblName = "paint";
								//	strPaintPathName = basDirectory::ProjectDir + strTblName + ".xls";
									EDIBDB::CreateTableToAutoIPED(strPaintPathName, strTblName, strTbn, EDIBgbl::gsngSumBomWeight);
									break;
								}
								
							}
						}
						EDIBDB::OutPutTable(strTbn, strXls, BOM_Format_phs, EDIBgbl::dbPRJ, _T("tmp") + vtos(GetFields(rs,_T("BomTable"))), BOM_Format_phs);						
						rs->MoveNext();
					}
				}
				if( EDIBgbl::strExportPaint.IsEmpty() ) //导出数据到AutoIPED也调用该函数,但不要显示下面对话框.
				{
						//如果Excel未启动，才调用这过程。以避免错误。
					if( ShowMessage(GetResStr(IDS_DoYouWantViewBomXls),MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON1)==IDYES)
					{
						EDIBDB::StartEXCEL(strXls);
					}
				}
				else
				{	//记住该Excel文件的路径,进行导出.之后再清空.
					EDIBgbl::strExportPaint = strPaintPathName;
				}
			}
				break;
		case 3:
				BOM_Format_phs = _T("Text;");
				if( FileExists(basDirectory::ProjectDBDir + _T("phs.txt")) ){
					DeleteFile(basDirectory::ProjectDBDir + _T("phs.txt"));
				}
				EDIBDB::OutPutTable(_T("Phs.txt"), basDirectory::ProjectDBDir, BOM_Format_phs, EDIBgbl::dbPRJ, _T("tmp2"), BOM_Format_phs);
				::WinExec(T2A((LPTSTR)(LPCTSTR)(user::gstrWinDir+_T("notepad.exe  ")+basDirectory::ProjectDBDir+_T("phs.txt"))),SW_SHOW);
				break;
		case 4:
				//生成正规的Excel汇总表
				BOM_Format_phs = _T(";");
				EDIBDB::DrawID = EDIBAcad::GetDrawIDAndName(modPHScal::zdjh + modPHScal::glDrawNoStartNumber - 1, EDIBDB::DrawName);
				EDIBDB::OutPutTable(_T("tmpPhsBOM1"), EDIBgbl::GetDBName(EDIBgbl::dbPRJ), _T(""), EDIBgbl::dbPRJ, _T("tmp2"), BOM_Format_phs);
				//对tmpPhsBOM1表编号
				SQLx = _T("SELECT sum(CLzz) as SumWeight FROM tmpPhsBOM1");
				rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
					adOpenKeyset, adLockOptimistic, adCmdText); 
				if( rs->adoEOF || rs->BOF ){
					rs->Close();
				}else {
					EDIBgbl::gsngSumBomWeight = vtof(GetFields(rs,_T("SumWeight")));
					SQLx = _T("SELECT * FROM tmpPhsBOM1");
					rs->Close();
					rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
						adOpenKeyset, adLockOptimistic, adCmdText); 
					i=0;
					while(!rs->adoEOF)
					{
						i++;
						rs->put_Collect((_variant_t)_T("SEQ"),_variant_t((long)i)); 
						rs->Update();
						rs->MoveNext();
					}
					rs->Close();
					SQLx = _T("SELECT * FROM TmpPhsBOM1");
					rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
						adOpenKeyset, adLockOptimistic, adCmdText); 
					if(i>0)
						EDIBAcad::DrawTableExcel(EDIBgbl::TCLE,rs);

				}
				  //Excel用到rs记录集，当Excel没有关闭之前，关闭rs则Excel不能显示rs记录集
				break;
		}
	}
	catch(CException *e)
	{
		e->ReportError();
		e->Delete();
	}
	catch(_com_error e)
	{
		ShowMessage(e.Description());
	}
	AfxGetApp()->EndWaitCursor();
}

void CMainFrame::OnSumAcad() 
{
	// TODO: Add your command handler code here
	SumCL(0);
}

void CMainFrame::OnSumExc() 
{
	// TODO: Add your command handler code here

	SumCL(2);
}

void CMainFrame::OnSumTxt() 
{
	// TODO: Add your command handler code here
	SumCL(3);
}

void CMainFrame::OnSumAcad2() 
{
	// TODO: Add your command handler code here
	SumCL(1);
}

void CMainFrame::OnIntPipeana() 
{
	// TODO: Add your command handler code here
	try
	{
		if(EDIBgbl::SelBillType!=EDIBgbl::TZA)
			return;
		FrmPhsData.ShowWindow(SW_HIDE);
		FrmTxsr.ShowWindow(SW_HIDE);
		ReadYljsResult(FrmTxsr.m_pViewTxsr->m_ActiveRs);
		FrmPhsData.ShowWindow(SW_SHOWNORMAL);
	}
	catch(CException *e)
	{
		e->Delete();
	}
	catch(...)
	{
	}
}

void CMainFrame::OnFileSelDir() 
{
	// TODO: Add your command handler code here
	CFrmFolderLocation dlg;
	dlg.DoModal();
}

void CMainFrame::OnFileOpen() 
{
	// TODO: Add your command handler code here
	this->DDEFileCmd(1);
}

void CMainFrame::OnFileSave() 
{
	// TODO: Add your command handler code here
	DDEFileCmd(2);
}

void CMainFrame::OnFileSaveAs() 
{
	// TODO: Add your command handler code here
	DDEFileCmd(3);
}

void CMainFrame::OnFileCloseAcad() 
{
	// TODO: Add your command handler code here
	if(ShowMessage(GetResStr(IDS_ReallyCloseACAD), MB_DEFBUTTON1|MB_ICONQUESTION|MB_YESNO) == IDYES)
	{
		if(EDIBAcad::objAcadApp.GetActiveObject(EDIBAcad::gstrApplicatonName) == S_OK)
			EDIBAcad::objAcadApp.Invoke0(_T("Quit"));
		modPHScal::gsDwgFN = _T("");
	}
}

void CMainFrame::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(modPHScal::gsDwgFN==_T(""))
	{
		pCmdUI->Enable(FALSE);
	}
	else
	{
		pCmdUI->Enable(::ModEncrypt::gbLegalUser);
	}
}

void CMainFrame::OnUpdateFileNewOpen(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(::ModEncrypt::gbLegalUser);
}

void CMainFrame::OnCalAllzdj() 
{
	if(EDIBgbl::SelBillType != EDIBgbl::TZA)
	{
		MessageBox(GetResStr(IDS_PleaseSelectTZA));
		return;
	}
    if(!ModEncrypt::gbLegalUser)
	{
		CTime tm=CTime::GetCurrentTime();
        if(tm.GetSecond()% 12 !=0)
		{
            //非法用户，只有1/8的时间可以计算
			MessageBox(GetResStr(IDS_DemoVersionOutDate));
			return;
		}
	}
	CStatusBarThread* thr=NULL;
	try
	{
		int k,n,i=0;//第k个开始连续计算n个支吊架
		CString sTmp;
		if(FrmTxsr.m_pViewTxsr->m_bActive)
			FrmTxsr.m_pViewTxsr->UpdateBoundData();
		
		CInputBox dlg;
		dlg.m_strTitle=GetResStr(IDS_FIRST_PHS);
		dlg.m_strWndText=GetResStr(IDS_INPUT_FROM_K_PHS);
		dlg.m_strValue.Format(_T("%d"),(long)FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("zdjh")));
		if( dlg.DoModal() == IDCANCEL )
			return;
		while(dlg.m_strValue==_T("") || dlg.m_strValue.GetAt(0) < _T('0') || dlg.m_strValue.GetAt(0) > _T('9'))
		{
			if( dlg.DoModal() == IDCANCEL )
				return;
		}
		k=_ttoi(dlg.m_strValue);
		dlg.m_strTitle.LoadString(IDS_CON_COUNT_PHS_AMOUT);
		//_T("请输入从第%s个支吊架开始，连续计算的支吊架个数n")
		//dlg.m_strWndText=_T("请输入从第") + ltos(k) + _T("个支吊架开始，连续计算的支吊架个数n");
		dlg.m_strWndText.Format(IDS_INPUT_FROM_K_PHS_CON_COUNT_PHS_AMOUT,k);
		dlg.m_strValue.Format(_T("%d"),FrmTxsr.m_pViewTxsr->m_ActiveRs->RecordCount- k+1);
		if( dlg.DoModal() == IDCANCEL )
			return;
		while(dlg.m_strValue==_T("") || dlg.m_strValue.GetAt(0) < _T('0') || dlg.m_strValue.GetAt(0) > _T('9'))
		{
			if( dlg.DoModal() == IDCANCEL )
				return;	
		}
		n=_ttoi(dlg.m_strValue);
		FrmTxsr.m_pViewTxsr->m_ActiveRs->MoveFirst();
		FrmTxsr.m_pViewTxsr->m_ActiveRs->Find(_bstr_t(CString(_T("zdjh="))+ltos(k)),0,adSearchForward);
		//FrmTxsr.m_pViewTxsr->m_ActiveRs->Find (_T("zdjh=") + ltos(k));        
		modPHScal::gbCalAllPhs=true;
		modPHScal::gbStopCalAllPHS = false;
		hStatusStopEvent=::CreateEvent(NULL,FALSE,FALSE,NULL);
		hStatusCreateEvent=::CreateEvent(NULL,FALSE,FALSE,NULL);
		thr=(CStatusBarThread*)::AfxBeginThread(RUNTIME_CLASS(CStatusBarThread));
		::WaitForSingleObject(hStatusCreateEvent,INFINITE);
		sTmp.Format(_T("%d"),modPHScal::zdjh);
		CString sl1,sl2;
		sl1.Format(GetResStr(IDS_CalculatingNoXphs),sTmp); // _T("正在计算") & zdjh & _T("号支吊架")
		sl2.Format(GetResStr(IDS_ApplyingSampleIDxPhs), ltos(modPHScal::iSelSampleID)); //_T("正在使用") & iSelSampleID & _T("号样本支吊架组合结构")
		thr->m_StatusBar.SetWindowText(GetResStr(IDS_AllPhsCalculation)); // _T("计算全部支吊架")
		thr->m_StatusBar.m_ctrLabel1.SetWindowText(sl1);
		thr->m_StatusBar.m_ctrLabel2.SetWindowText(sl2);		
		//thr->m_StatusBar.UpdateData(false);
		thr->m_StatusBar.UpdateStatus(0.0,true);
//		::AfxBeginThread((AFX_THREADPROC)CalAllZdjh,this);
		//EnableWindow(FALSE);
		while (!FrmTxsr.m_pViewTxsr->m_ActiveRs->adoEOF && ( i < n))
		{
			i++;
			
			modPHScal::zdjh=vtoi(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("zdjh")));
			sTmp.Format (_T("%d"),modPHScal::zdjh);
			sl1.Format (GetResStr(IDS_CalculatingNoXphs),sTmp); // _T("正在计算") & zdjh & _T("号支吊架")
			modPHScal::iSelSampleID=vtoi(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("iSelSampleID")));
			sl2.Format (GetResStr(IDS_ApplyingSampleIDxPhs),  ltos(modPHScal::iSelSampleID)); //_T("正在使用") & iSelSampleID & _T("号样本支吊架组合结构")
			thr->m_StatusBar.SetWindowText(GetResStr(IDS_AllPhsCalculation)); // _T("计算全部支吊架")
			thr->m_StatusBar.m_ctrLabel1.SetWindowText(sl1);
			thr->m_StatusBar.m_ctrLabel2.SetWindowText(sl2);
			//thr->m_StatusBar.UpdateData(false);
			thr->m_StatusBar.UpdateStatus(i / n,true);
			if(::WaitForSingleObject(hStatusStopEvent,0)==WAIT_OBJECT_0)
			{
				modPHScal::gbStopCalAllPHS=true;
				break;
			}
			modPHScal::VB_Cal(FrmTxsr.m_pViewTxsr->m_ActiveRs ,modPHScal::zdjh,thr->m_StatusBar);
			FrmTxsr.m_pViewTxsr->m_ActiveRs->MoveNext();      
			//SetWindowTop Me.hwnd
		}
		if(FrmTxsr.m_pViewTxsr->m_ActiveRs->adoEOF )
		{
			FrmTxsr.m_pViewTxsr->m_ActiveRs->MoveLast();
			modPHScal::zdjh=vtoi(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("zdjh")));
			modPHScal::iSelSampleID=vtoi(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("iSelSampleID")));
		}
		//EnableWindow(TRUE);
		modPHScal::gbCalAllPhs = false;
		ShowMessage(GetResStr(IDS_AllPhsCalculationFinished)); //_T("所有支吊架计算完毕!")         
	}
	catch(CException *e)
	{
		e->Delete();
	}
	catch(...)
	{
	}
	if(thr!=NULL)
	{
		//::SendMessage(thr->m_StatusBar.GetSafeHwnd(),WM_CLOSEWINDOW1,0,0);
		thr->EndThread();
		::CloseHandle(hStatusCreateEvent);
		::CloseHandle(hStatusStopEvent);
		hStatusStopEvent=NULL;
		hStatusCreateEvent=NULL;
	}
}
LRESULT CALLBACK CalAllZdjhProc()
{
	return 0;
}

void CMainFrame::OnFileCloseExcel() 
{
	// TODO: Add your command handler code here
	EDIBDB::CloseExcel();
}

void CMainFrame::OnDrawZdjTab180() 
{
	// TODO: Add your command handler code here
	DrawZdjTab(iZDJDRAW180mm);
}


void CMainFrame::DrawZdjTab(int index)
{
	try
	{
		bool Found=false;
	//	bool bIsCom = false; //画图时用COM画为true; arx画为false; 1/12   zsy
		CCadPoint p0, p1;
		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(Recordset));
		_RecordsetPtr rs1;
		rs1.CreateInstance(__uuidof(Recordset));
		int i=0;
		int k=0;
		static int n=0;
		if(index!=iZDJDRAWEXECL && index!=iZDJDRAWCatalogueExcel)
		{
		EDIBAcad::StartAcad();
		EDIBAcad::DisplayAcadTop();
		EDIBAcad::SetAcadTop();
		EDIBAcad::GetActiveAcadDoc();
		}
		//如果当前打开的图形不是(PhsBlkDir & _T("phs.dwg")),需要重新打开
		//因为后面的操作需要大量的图形块准备好
		CString sTmp,sTmp1;
		_variant_t vTmp;
		CMObject objTmp;
		CCadPoint pTmp;
		CString SQLx;
		_variant_t varTmp;
		_variant_t varNull;
		varNull.ChangeType(VT_NULL);
		if(index!=iZDJDRAWTag && index!=iZDJDRAWbom)
		{
			//判断是否所有支吊架计算完成。
			try
			{
				if(!FrmTxsr.m_pViewTxsr->m_ActiveRs->adoEOF && !FrmTxsr.m_pViewTxsr->m_ActiveRs->BOF)
					FrmTxsr.m_pViewTxsr->m_ActiveRs->Update();
			}
	catch(CException *e)
	{
		e->Delete();
	}
			CString SQLx = _T("SELECT count(*) FROM [") + EDIBgbl::Btype[EDIBgbl::TZA] + _T("] WHERE ( bCalSuccess=0  OR bCalSuccess IS NULL )");
			
			if(modPHScal::gbSumAllVolume )
			{
				SQLx +=_T(" AND VolumeID IN ( SELECT VolumeID FROM Volume WHERE EnginID=\'") + EDIBgbl::SelPrjID + _T("\' AND ZYID = ") + ltos(EDIBgbl::SelSpecID) + _T(" AND SJJDID = ") + ltos(EDIBgbl::SelDsgnID) + _T(" AND SJHYID = ") + ltos(EDIBgbl::SelHyID) +_T(") ");
			}
			else
			{
				SQLx +=_T(" AND VolumeID =") + ltos(EDIBgbl::SelVlmID);
			}

			long i1,i2;
			rs1.CreateInstance(__uuidof(Recordset));

			rs1->Open(_variant_t(SQLx),(IDispatch*)::conPRJDB,adOpenForwardOnly,adLockReadOnly,adCmdText);
			i1=vtoi(rs1->GetCollect(_variant_t((long)0)));
			rs1->Close();

			SQLx = _T("SELECT count(*) FROM [") + EDIBgbl::Btype[EDIBgbl::TZA] + _T("] WHERE ");
			if(modPHScal::gbSumAllVolume )
			{
				SQLx +=_T(" VolumeID IN ( SELECT VolumeID FROM Volume WHERE EnginID=\'") + EDIBgbl::SelPrjID + _T("\' AND ZYID = ") + ltos(EDIBgbl::SelSpecID) + _T(" AND SJJDID = ") + ltos(EDIBgbl::SelDsgnID) + _T(" AND SJHYID = ") + ltos(EDIBgbl::SelHyID) +_T(") ");
			}
			else
			{
				SQLx +=_T(" VolumeID =") + ltos(EDIBgbl::SelVlmID);
			}
			rs1->Open(_variant_t(SQLx),(IDispatch*)::conPRJDB,adOpenForwardOnly,adLockReadOnly,adCmdText);
			i2=vtoi(rs1->GetCollect(_variant_t((long)0)));
			rs1->Close();
			rs1=NULL;
			CString strTmp;
			strTmp.Format(GetResStr(IDS_SomePhsNotCalSuccess),ltos(i2),ltos(i2-i1),ltos(i1));
			if( IDNO == ShowMessage(strTmp,MB_DEFBUTTON1|MB_ICONQUESTION|MB_YESNO) )
				return;
		}

		switch(index)
		{
		case iZDJDRAWTag:
			//EDIBAcad::DrawTagByPickEntity();
			break;
		case iZDJDRAWbom:
			modPHScal::SetBasePoint();
			Cavphs->GetphsSEQ(FrmTxsr.m_pViewTxsr->m_ActiveRs);
			EDIBDB::MakeTmp2ToBom();
			p0.SetPoint(modPHScal::pt2x,modPHScal::pt2y);
			EDIBAcad::DeleteAllEntitiesInLayers(1,_T("bom"));
			SQLx = _T("SELECT * FROM TMP2");
			rs->Open(_variant_t(SQLx),(IDispatch*)EDIBgbl::dbPRJ,adOpenKeyset, adLockOptimistic,adCmdText);
			
			EDIBAcad::DrawTableACAD(p0, EDIBgbl::TLJ, atan(1.0) * 0.0, rs,0,1.0f,_T("Bom"),_T("Bom"),_T("%g"),modPHScal::iAlignLjmx);
			sTmp.Format(GetResStr(IDS_xDrawingFinished),EDIBgbl::Cbtype[EDIBgbl::TCL].MnuCaption);

			ShowMessage(sTmp);
			break;
		case iZDJDRAWbomBJBW:
			modPHScal::SetBasePoint();
			Cavphs->GetphsSEQ(FrmTxsr.m_pViewTxsr->m_ActiveRs);
			EDIBDB::MakeTmp2ToBom();
			p0.SetPoint(modPHScal::pt2x,modPHScal::pt2y);
			EDIBAcad::DeleteAllEntitiesInLayers(1,_T("bom"));
			SQLx = _T("SELECT * FROM TMP2");
			rs->Open(_variant_t(SQLx),(IDispatch*)EDIBgbl::dbPRJ,adOpenKeyset, adLockOptimistic,adCmdText);
			
			EDIBAcad::DrawTableACAD(p0, EDIBgbl::TLJ_BJBW, atan(1.0) * 0.0, rs,0,1.0f,_T("Bom"),_T("Bom"),_T("%g"),modPHScal::iAlignLjmx);
			sTmp.Format(GetResStr(IDS_xDrawingFinished),EDIBgbl::Cbtype[EDIBgbl::TCL].MnuCaption);

			ShowMessage(sTmp);
			break;
		case iZDJDRAWload:
			modPHScal::SetBasePoint();
			modPHScal::PhsDisplacementLoadINFOMake();
			break;
		case iZDJDRAW270mm:
				//每次计算一个支吊架时生成当前当前支吊架一览表，计算完成后总的一览表就生成了。
				//每次计算一个支吊架时生成当前当前支吊架明细一览表，计算完成后总的明细一览表就生成了。
			SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZD] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" ORDER BY zdjh");
			rs->Open(_variant_t(SQLx),(IDispatch*)EDIBgbl::dbPRJDB,adOpenKeyset, adLockOptimistic,adCmdText);
			if(!rs->BOF && !rs->adoEOF)
			{//zsy 12/17      改为用ARX画图
					ShowMessage(GetResStr(IDS_PleasePickApointAsZDJYLBInsertPointINAutoCAD));
					if( EDIBAcad::gbACADR14 || !bIsCom ) //使用动态库画图
					{
						rs->Close();
					}else
					{
						objTmp=EDIBAcad::objAcadDoc.GetPropertyByName(_T("Utility")).pdispVal;
						p1=objTmp.Invoke(_T("GetPoint"),2,(LPVARIANT)p0,&_variant_t(GetResStr(IDS_PleasePickApointAsBOMInsertPointINAutoCAD)));
						p0 = p1;
					}
					EDIBAcad::DrawTableACAD(p0, EDIBgbl::TZD, atan(1.0) * 0.0, rs, 0,1.0f, _T("YLB"));
			}
		//	sTmp.Format(GetResStr(IDS_xDrawingFinished),EDIBgbl::Cbtype[EDIBgbl::TZD].MnuCaption);
		//	ShowMessage(sTmp);
			break;
		case iZDJDRAW180mm:
				//每次计算一个支吊架时生成当前支吊架一览表，计算完成后总的一览表就生成了。
				//每次计算一个支吊架时生成当前支吊架明细一览表，计算完成后总的明细一览表就生成了。
			SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZD] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" ORDER BY zdjh");
			rs->Open(_variant_t(SQLx),(IDispatch*)EDIBgbl::dbPRJDB,adOpenKeyset, adLockOptimistic,adCmdText);
			if(!rs->BOF && !rs->adoEOF)
			{//zsy 12/17      改为用ARX画图
				ShowMessage(GetResStr(IDS_PleasePickApointAsZDJYLBInsertPointINAutoCAD));
				if( EDIBAcad::gbACADR14 || !bIsCom ) //使用动态库画图
				{
					rs->Close();
				}else
				{
					objTmp=EDIBAcad::objAcadDoc.GetPropertyByName(_T("Utility")).pdispVal;
					p1=objTmp.Invoke(_T("GetPoint"),2,(LPVARIANT)p0,&_variant_t(GetResStr(IDS_PleasePickApointAsBOMInsertPointINAutoCAD)));
					p0 = p1;
				}
				EDIBAcad::DrawTableACAD(p0, EDIBgbl::TZD180, atan(1.0) * 0.0, rs, 0 , 1.0f, _T("YLB"));
			}
		//	sTmp.Format(GetResStr(IDS_xDrawingFinished),EDIBgbl::Cbtype[EDIBgbl::TZD180].MnuCaption);
		//	ShowMessage(sTmp);
			break;	
		case iZDJDRAW270L:
			SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZD] + _T("] WHERE ZDJH IN ( SELECT ZDJH FROM [")
				+ EDIBgbl::Btype[EDIBgbl::TZA] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) 
				+ _T(" AND TYPE LIKE \'????\' ) AND VolumeID =")+ ltos(EDIBgbl::SelVlmID) + _T(" ORDER BY zdjh");
			rs->Open(_variant_t(SQLx),(IDispatch*)EDIBgbl::dbPRJDB,adOpenKeyset, adLockOptimistic,adCmdText);
			if(!rs->BOF && !rs->adoEOF)
			{//zsy 12/17      改为用ARX画图
				ShowMessage(GetResStr(IDS_PleasePickApointAsZDJYLBInsertPointINAutoCAD));
				if( EDIBAcad::gbACADR14 || !bIsCom ) //使用动态库画图
				{
					rs->Close();
				}else
				{
					objTmp=EDIBAcad::objAcadDoc.GetPropertyByName(_T("Utility")).pdispVal;
					p1=objTmp.Invoke(_T("GetPoint"),2,(LPVARIANT)p0,&_variant_t(GetResStr(IDS_PleasePickApointAsBOMInsertPointINAutoCAD)));
					p0 = p1;
				}
				EDIBAcad::DrawTableACAD(p0, EDIBgbl::TZD270L, atan(1.0) * 0.0, rs, 0 , 1.0f, _T("YLB"));
			}
		//	sTmp.Format(GetResStr(IDS_xDrawingFinished),EDIBgbl::Cbtype[EDIBgbl::TZD].MnuCaption);
		//	ShowMessage(sTmp);
			break;
		case iZDJDRAWEXECL:
			SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZD] + _T("] WHERE ZDJH IN ( SELECT ZDJH FROM [")
				+ EDIBgbl::Btype[EDIBgbl::TZA] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) 
				+ _T(" AND TYPE LIKE \'????\' ) AND VolumeID =")+ ltos(EDIBgbl::SelVlmID) + _T(" ORDER BY zdjh");			
			rs->Open(_variant_t(SQLx),(IDispatch*)EDIBgbl::dbPRJDB,adOpenKeyset, adLockOptimistic,adCmdText);
			if(!rs->BOF && !rs->adoEOF)
			{
				EDIBAcad::DrawTableExcel( EDIBgbl::TZDExcel, rs);
			}
			sTmp.Format(GetResStr(IDS_xDrawingFinished),EDIBgbl::Cbtype[EDIBgbl::TZDExcel].MnuCaption);
			ShowMessage(sTmp);
			break;
		case iZDJDRAW705mm:
				//判断是否所有支吊架计算完成。
			SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZD] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" ORDER BY zdjh");
			rs->Open(_variant_t(SQLx),(IDispatch*)EDIBgbl::dbPRJDB,adOpenKeyset, adLockOptimistic,adCmdText);
			if(!rs->BOF && !rs->adoEOF)
			{//zsy 12/17      改为用ARX画图
					ShowMessage(GetResStr(IDS_PleasePickApointAsZDJYLBInsertPointINAutoCAD));
					if( EDIBAcad::gbACADR14 || !bIsCom ) //使用动态库画图
					{
						rs->Close();
					}else
					{

						objTmp=EDIBAcad::objAcadDoc.GetPropertyByName(_T("Utility")).pdispVal;
						p1=objTmp.Invoke(_T("GetPoint"),2,(LPVARIANT)p0,&_variant_t(GetResStr(IDS_PleasePickApointAsBOMInsertPointINAutoCAD)));
						p0 = p1;
					}
					EDIBAcad::DrawTableACAD(p0, EDIBgbl::TZD800, atan(1.0) * 0.0, rs, 0 , 1.0f, _T("YLB"));
			}
		//	sTmp.Format(GetResStr(IDS_xDrawingFinished),EDIBgbl::Cbtype[EDIBgbl::TZD800].MnuCaption);
		//	ShowMessage(sTmp);
			break;

		case iZDJDRAW705mmHSY:			////核四院支吊架明细一览表
				//判断是否所有支吊架计算完成。
			SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZD] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" ORDER BY zdjh");
			rs->Open(_variant_t(SQLx),(IDispatch*)EDIBgbl::dbPRJDB,adOpenKeyset, adLockOptimistic,adCmdText);
			if(!rs->BOF && !rs->adoEOF)
			{//zsy 12/17      改为用ARX画图
					ShowMessage(GetResStr(IDS_PleasePickApointAsZDJYLBInsertPointINAutoCAD));
					if( EDIBAcad::gbACADR14 || !bIsCom ) //使用动态库画图
					{
						rs->Close();
					}else
					{
						objTmp=EDIBAcad::objAcadDoc.GetPropertyByName(_T("Utility")).pdispVal;
						p1=objTmp.Invoke(_T("GetPoint"),2,(LPVARIANT)p0,&_variant_t(GetResStr(IDS_PleasePickApointAsBOMInsertPointINAutoCAD)));
						p0 = p1;
					}
					EDIBAcad::DrawTableACAD(p0, EDIBgbl::TZD800HSY, atan(1.0) * 0.0, rs, 0 , 1.0f, _T("YLB"));
			}
		//	sTmp.Format(GetResStr(IDS_xDrawingFinished),EDIBgbl::Cbtype[EDIBgbl::TZD800].MnuCaption);
		//	ShowMessage(sTmp);
			break;

		case iZDJDRAW705mmQGY:			////支吊架明细一览表
				//判断是否所有支吊架计算完成。
			SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZD] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" ORDER BY zdjh");
			rs->Open(_variant_t(SQLx),(IDispatch*)EDIBgbl::dbPRJDB,adOpenKeyset, adLockOptimistic,adCmdText);
			if(!rs->BOF && !rs->adoEOF)
			{//zsy 12/17      改为用ARX画图
				ShowMessage(GetResStr(IDS_PleasePickApointAsZDJYLBInsertPointINAutoCAD));
				if( EDIBAcad::gbACADR14 || !bIsCom ) //使用动态库画图
				{
					rs->Close();
				}else
				{
					objTmp=EDIBAcad::objAcadDoc.GetPropertyByName(_T("Utility")).pdispVal;
					p1=objTmp.Invoke(_T("GetPoint"),2,(LPVARIANT)p0,&_variant_t(GetResStr(IDS_PleasePickApointAsBOMInsertPointINAutoCAD)));
					p0 = p1;
				}
				EDIBAcad::DrawTableACAD(p0, EDIBgbl::TZD800QGY, atan(1.0) * 0.0, rs, 0 , 1.0f, _T("YLB"));
			}
		//	sTmp.Format(GetResStr(IDS_xDrawingFinished),EDIBgbl::Cbtype[EDIBgbl::TZD800].MnuCaption);
		//	ShowMessage(sTmp);
			break;

		case iZDJDRAW270pc:   //巴陵石化设计院支吊架表
				//每次计算一个支吊架时生成当前当前支吊架一览表，计算完成后总的一览表就生成了。
				//每次计算一个支吊架时生成当前当前支吊架明细一览表，计算完成后总的明细一览表就生成了。
			SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZD] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" ORDER BY zdjh");
			rs->Open(_variant_t(SQLx),(IDispatch*)EDIBgbl::dbPRJDB,adOpenKeyset, adLockOptimistic,adCmdText);
			if(!rs->BOF && !rs->adoEOF)
			{//zsy 12/17      改为用ARX画图
					ShowMessage(GetResStr(IDS_PleasePickApointAsZDJYLBInsertPointINAutoCAD));
					if( EDIBAcad::gbACADR14 || !bIsCom ) //使用动态库画图
					{
						rs->Close();
					}else
					{
						objTmp=EDIBAcad::objAcadDoc.GetPropertyByName(_T("Utility")).pdispVal;
						p1=objTmp.Invoke(_T("GetPoint"),2,(LPVARIANT)p0,&_variant_t(GetResStr(IDS_PleasePickApointAsBOMInsertPointINAutoCAD)));
						p0 = p1;
					}
					EDIBAcad::DrawTableACAD(p0, EDIBgbl::TZP, atan(1.0) * 0.0, rs, 0 , 1.0f, _T("YLB"));
			}
		//	sTmp.Format(GetResStr(IDS_xDrawingFinished),EDIBgbl::Cbtype[EDIBgbl::TZP].MnuCaption);
		//	ShowMessage(sTmp);
			break;	
		case iZDJDRAWCatalogue:
		//	EDIBAcad::EraseAllEntities();
			EDIBDB::DrawID = EDIBAcad::GetDrawIDAndName(1, EDIBDB::DrawName);

				//绘制目录
				//每次计算一个支吊架时生成当前当前支吊架图纸目录信息，计算完成后总的目录就生成了。
			SQLx=_T("DELETE FROM [ML] WHERE VolumeID IS NULL AND ZDJH IS NULL");
			EDIBgbl::dbPRJDB->Execute((_bstr_t)SQLx, NULL, adCmdText);
			SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TML] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" ORDER BY SEQ");
			rs->Open(_variant_t(SQLx),(IDispatch*)EDIBgbl::dbPRJDB,adOpenKeyset, adLockOptimistic,adCmdText);
			if(!rs->BOF && !rs->adoEOF)
			{
					//如果第一个图纸的序号>=1，在前面添加空的图纸目录,以便用户手工修改
				_variant_t vTmp;
				rs->Find((_bstr_t)(_T("ZDJH <> NULL")), 0, adSearchForward);
				if(rs->adoEOF)
					return;
				rs->get_Collect((_variant_t)_T("SEQ"), &varTmp);
				k = vtoi(varTmp);
				for( i = 1 ;i< k ;i++)
				{
					rs->Find((_bstr_t)(_T("SEQ=") + ltos(i)), 0, adSearchForward);
					if(rs->adoEOF)
					{
						rs->AddNew();
						rs->put_Collect((_variant_t)_T("SEQ"),_variant_t((long)i));
						rs->put_Collect((_variant_t)_T("Pages"),_variant_t((long)1));
						rs->put_Collect((_variant_t)_T("DrawNo"),STR_VAR(_T("1")));
						rs->put_Collect((_variant_t)_T("DrawNa"),STR_VAR(_T("1")));
						rs->put_Collect((_variant_t)_T("Zdjh"),varNull);
						rs->put_Collect((_variant_t)_T("VolumeID"),varNull);
						rs->Update();
					}
				}
				rs->Close();
				//zsy 12/17      改为用ARX画图
				SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TML] + _T("] WHERE VolumeID IS NULL OR VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T("  ORDER BY SEQ");
				rs->Open(_variant_t(SQLx),(IDispatch*)EDIBgbl::dbPRJDB,adOpenKeyset, adLockOptimistic,adCmdText);
				ShowMessage(GetResStr(IDS_PleasePickApointAsZDJYLBInsertPointINAutoCAD));
				if( EDIBAcad::gbACADR14 || !bIsCom ) //使用动态库画图
				{
					rs->Close();
				}else
				{
					objTmp=EDIBAcad::objAcadDoc.GetPropertyByName(_T("Utility")).pdispVal;
					p1=objTmp.Invoke(_T("GetPoint"),2,(LPVARIANT)p0,&_variant_t(GetResStr(IDS_PleasePickApointAsBOMInsertPointINAutoCAD)));
					p0 = p1;
				}
				EDIBAcad::DrawTableACAD(p0, EDIBgbl::TML, atan(1.0) * 0.0, rs, 0 , 1.0f, _T("ML"));
			}
		//	sTmp.Format(GetResStr(IDS_xDrawingFinished),EDIBgbl::Cbtype[EDIBgbl::TML].MnuCaption);
		//	ShowMessage(sTmp);
			break;
		case iZDJDRAWCatalogueExcel:
			EDIBDB::DrawID = EDIBAcad::GetDrawIDAndName(1, EDIBDB::DrawName);
				//绘制目录
				//每次计算一个支吊架时生成当前当前支吊架图纸目录信息，计算完成后总的目录就生成了。
			SQLx=_T("DELETE FROM [ML] WHERE VolumeID IS NULL AND ZDJH IS NULL");
			EDIBgbl::dbPRJDB->Execute((_bstr_t)SQLx, NULL, adCmdText);

			SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TML] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" ORDER BY SEQ");
			rs->Open(_variant_t(SQLx),(IDispatch*)EDIBgbl::dbPRJDB,adOpenKeyset, adLockOptimistic,adCmdText);
			if(!rs->BOF && !rs->adoEOF)
			{
					//如果第一个图纸的序号>=1，在前面添加空的图纸目录,以便用户手工修改
				_variant_t vTmp;
				rs->Find((_bstr_t)(_T("ZDJH <> NULL")), 0, adSearchForward);
				if(rs->adoEOF)
					return;
				rs->get_Collect((_variant_t)_T("SEQ"), &varTmp);
				k = vtoi(varTmp);
				for( i = 1 ;i< k ;i++)
				{
					_variant_t vTmp;
					rs->Find((_bstr_t)(_T("SEQ=") + ltos(i)), 0, adSearchForward);
					if(rs->adoEOF)
					{
						rs->AddNew();
						rs->put_Collect((_variant_t)_T("SEQ"),_variant_t((long)i));
						rs->put_Collect((_variant_t)_T("Pages"),_variant_t((long)1));
						rs->put_Collect((_variant_t)_T("DrawNo"),STR_VAR(_T("1")));
						rs->put_Collect((_variant_t)_T("DrawNa"),STR_VAR(_T("1")));
						rs->put_Collect((_variant_t)_T("Zdjh"),varNull);
						rs->put_Collect((_variant_t)_T("VolumeID"),varNull);
						rs->Update();
					}
				}
				rs->Close();
				SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TML] + _T("] WHERE VolumeID IS NULL OR VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T("  ORDER BY SEQ");
				rs->Open(_variant_t(SQLx),(IDispatch*)EDIBgbl::dbPRJDB,adOpenKeyset, adLockOptimistic,adCmdText);
				EDIBAcad::DrawTableExcel(EDIBgbl::TMLE, rs);
			}
			sTmp.Format(GetResStr(IDS_xDrawingFinished),EDIBgbl::Cbtype[EDIBgbl::TML].MnuCaption);
			ShowMessage(sTmp);
			break;
		case iZDJDRAWm_phs :  //开始手工修改
			EDIBAcad::MakeNewLayer(_T("M_phs0"), acRed);
			break;
		case iZDJDRAWXN180mm://绘制西南电力院的支吊架一览表
			//Z8表
			SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZD] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" ORDER BY zdjh");
			rs->Open(_variant_t(SQLx),(IDispatch*)EDIBgbl::dbPRJDB,adOpenKeyset, adLockOptimistic,adCmdText);
			if(!rs->BOF && !rs->adoEOF)
			{//zsy 12/17      改为用ARX画图
				ShowMessage(GetResStr(IDS_PleasePickApointAsZDJYLBInsertPointINAutoCAD));
				if( EDIBAcad::gbACADR14 || !bIsCom ) //使用动态库画图
				{
					rs->Close();
				}else
				{
					objTmp=EDIBAcad::objAcadDoc.GetPropertyByName(_T("Utility")).pdispVal;
					p1=objTmp.Invoke(_T("GetPoint"),2,(LPVARIANT)p0,&_variant_t(GetResStr(IDS_PleasePickApointAsBOMInsertPointINAutoCAD)));
					p0 = p1;
				}
				EDIBAcad::DrawTableACAD(p0, EDIBgbl::TZDSWED180, atan(1.0) * 0.0, rs, 0 , 1.0f, _T("YLB"));
			}
		//	sTmp.Format(GetResStr(IDS_xDrawingFinished),EDIBgbl::Cbtype[EDIBgbl::TZDSWED180].MnuCaption);
		//	ShowMessage(sTmp);
			break;
		case iZDJDRAW705mmHBYZ: //华北冶建
			//z9
						SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZD] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" ORDER BY zdjh");
			rs->Open(_variant_t(SQLx),(IDispatch*)EDIBgbl::dbPRJDB,adOpenKeyset, adLockOptimistic,adCmdText);
			if(!rs->BOF && !rs->adoEOF)
			{//zsy 12/17      改为用ARX画图
				ShowMessage(GetResStr(IDS_PleasePickApointAsZDJYLBInsertPointINAutoCAD));
				if( EDIBAcad::gbACADR14 || !bIsCom ) //使用动态库画图
				{
					rs->Close();
				}else
				{
					objTmp=EDIBAcad::objAcadDoc.GetPropertyByName(_T("Utility")).pdispVal;
					p1=objTmp.Invoke(_T("GetPoint"),2,(LPVARIANT)p0,&_variant_t(GetResStr(IDS_PleasePickApointAsBOMInsertPointINAutoCAD)));
					p0 = p1;
				}
				EDIBAcad::DrawTableACAD(p0, EDIBgbl::TZD800HBYJ, atan(1.0) * 0.0, rs, 0 , 1.0f, _T("YLB"));
			}
		//	sTmp.Format(GetResStr(IDS_xDrawingFinished),EDIBgbl::Cbtype[EDIBgbl::TZD800HBYJ].MnuCaption);
		//	ShowMessage(sTmp);
			break;
		case iZDJDRAWloadBJBW://北京巴威支吊荷载位移表
			modPHScal::SetBasePoint();
			modPHScal::PhsDisplacementLoadINFOMake("hz",0,EDIBgbl::TzDLOADNEW);
			break;
		//pfg20050601//北京华宇
		case iZDJDRAWBJHY01:
				//判断是否所有支吊架计算完成。
			SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZD] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" ORDER BY zdjh");
			rs->Open(_variant_t(SQLx),(IDispatch*)EDIBgbl::dbPRJDB,adOpenKeyset, adLockOptimistic,adCmdText);
			if(!rs->BOF && !rs->adoEOF)
			{//zsy 12/17      改为用ARX画图
					ShowMessage(GetResStr(IDS_PleasePickApointAsZDJYLBInsertPointINAutoCAD));
					if( EDIBAcad::gbACADR14 || !bIsCom ) //使用动态库画图
					{
						rs->Close();
					}else
					{

						objTmp=EDIBAcad::objAcadDoc.GetPropertyByName(_T("Utility")).pdispVal;
						p1=objTmp.Invoke(_T("GetPoint"),2,(LPVARIANT)p0,&_variant_t(GetResStr(IDS_PleasePickApointAsBOMInsertPointINAutoCAD)));
						p0 = p1;
					}
					EDIBAcad::DrawTableACAD(p0, 36, atan(1.0) * 0.0, rs, 0 , 1.0f, _T("YLB"));
			}
			break;

		case iZDJDRAWBJHY02:
				//判断是否所有支吊架计算完成。
			SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZD] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" ORDER BY zdjh");
			rs->Open(_variant_t(SQLx),(IDispatch*)EDIBgbl::dbPRJDB,adOpenKeyset, adLockOptimistic,adCmdText);
			if(!rs->BOF && !rs->adoEOF)
			{//zsy 12/17      改为用ARX画图
					ShowMessage(GetResStr(IDS_PleasePickApointAsZDJYLBInsertPointINAutoCAD));
					if( EDIBAcad::gbACADR14 || !bIsCom ) //使用动态库画图
					{
						rs->Close();
					}else
					{

						objTmp=EDIBAcad::objAcadDoc.GetPropertyByName(_T("Utility")).pdispVal;
						p1=objTmp.Invoke(_T("GetPoint"),2,(LPVARIANT)p0,&_variant_t(GetResStr(IDS_PleasePickApointAsBOMInsertPointINAutoCAD)));
						p0 = p1;
					}
					EDIBAcad::DrawTableACAD(p0, 37, atan(1.0) * 0.0, rs, 0 , 1.0f, _T("YLB"));
			}
			break;
		//pfg20050601
		}
	}
	catch(_com_error e)
	{
		ShowMessage(e.Description());
	}
	catch(CException *e)
	{
		e->Delete();
	}
}

void CMainFrame::OnDrawZdjTab270() 
{
	// TODO: Add your command handler code here
	DrawZdjTab(iZDJDRAW270mm);
}

void CMainFrame::OnZdjDrawEP() 
{
	// TODO: Add your command handler code here
	DrawZdjTab(iZDJDRAW705mm);
}

void CMainFrame::OnZdjDrawPC() 
{
	// TODO: Add your command handler code here
	DrawZdjTab(iZDJDRAW270pc);
}

void CMainFrame::OnDrawZdjTML() 
{
	// TODO: Add your command handler code here
	DrawZdjTab(iZDJDRAWCatalogue);	
}

void CMainFrame::OnEraseAll() 
{
	// TODO: Add your command handler code here
	EDIBAcad::EraseAllEntities();
}

void CMainFrame::OnEraseLayer() 
{
	EDIBAcad::DeleteAllEntitiesInLayers();
}

void CMainFrame::OnCadCel() 
{
	EDIBAcad::SetAcadNoTop();	
	//SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOSIZE);
	//SendMessage(SW_SHOW);
}

void CMainFrame::OnCadMenu() 
{
	EDIBAcad::DisplayAcadTop();	
}

void CMainFrame::OnDeleteLayerTag() 
{
	EDIBAcad::DeleteAllEntitiesInLayers(1,_T("TAG"));
}

void CMainFrame::OnCouShowtab() 
{
	EDIBgbl::gbDisplayBOM = true;
   //EDIBAcad::DisplayDataZB();
	FrmDataEDIT.m_bAllowUpd=true;
	if(!IsWindow(FrmDataEDIT.m_hWnd))
	{
		FrmDataEDIT.Create(IDD_DATAEDIT,AfxGetApp()->m_pMainWnd);
		FrmDataEDIT.UpdateWindow();
	}
	//if(!FrmDataEDIT.IsWindowVisible())
	::SetWindowPos(FrmDataEDIT.GetSafeHwnd(),HWND_TOP,0,0,0,0,/*SWP_NOACTIVATE|*/SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
}

void CMainFrame::CalAllZdjh(LPVOID pParam)
{		

}

void CMainFrame::OnDrawChangeDim() 
{
	// TODO: Add your command handler code here
	if(FAILED(EDIBAcad::objAcadApp.GetActiveObject(EDIBAcad::gstrApplicatonName)))
		return;
	EDIBAcad::DrawphsDataEdit();
}

void CMainFrame::LoadRecentFiles()
{
	int i,c;
	CString stmp,sfile;
	CMenu* menu=GetMenu()->GetSubMenu(0);
	CMenu* menu1;
	if(menu==NULL)
		return;
	c=menu->GetMenuItemCount();
	for(i=0;i<c;i++)
	{
		menu1=menu->GetSubMenu(i);
		if(menu1)
		{
			menu=menu1;
			break;
		}
	}
	if(menu1==NULL)
		return;
	c=menu->GetMenuItemCount();
	for(i=c;i>0;i--)
	{
		menu->DeleteMenu(i-1,MF_BYPOSITION);
	}
	m_nRecFileNum=0;
	for(i=0; i<8;i++)
	{
		stmp.Format(_T("%d"),i+1);
		if((sfile =::GetRegKey(_T("RecentFiles"),stmp,CString(_T("")))) != _T(""))
		{
			sfile=CString(_T("&"))+stmp+_T(" ") +sfile;
			menu->InsertMenu(i, MF_BYPOSITION | MF_STRING , ID_REC_FILE1+i , sfile);
			m_nRecFileNum++;
		}
	}
	if(m_nRecFileNum==0)
		menu->InsertMenu(0,MF_BYPOSITION|MF_STRING|MF_DISABLED|MF_GRAYED,ID_REC_FILE1-1,GetResStr(IDS_NO_LATELY_USE_FILE));
}

void CMainFrame::AddRecentFiles(CString FileName)
{
	CString stmp,sfile;
	int i,k;
	FileName.MakeLower();
	for(k=1;k<=8;k++)
	{
		stmp.Format(_T("%d"),k);
		sfile=GetRegKey(_T("RecentFiles"),stmp,CString(_T("")));
		sfile.MakeLower();
		if(sfile==FileName)
			break;
	}
	if(k>8) k=8;
	for(i=k;i>1;i--)
	{
		stmp.Format(_T("%d"),i-1);
		sfile=GetRegKey(_T("RecentFiles"),stmp,CString(_T("")));
		stmp.Format(_T("%d"),i);
		::SetRegValue(_T("RecentFiles"),stmp,sfile);
	}
	::SetRegValue(_T("RecentFiles"),_T("1"),FileName);

}

void CMainFrame::OnRecFile1() 
{
	// TODO: Add your command handler code here
	mnuRecFile_Click(ID_REC_FILE1);
}

void CMainFrame::OnRecFile2() 
{
	// TODO: Add your command handler code here
	mnuRecFile_Click(ID_REC_FILE2);
	
}

void CMainFrame::OnRecFile3() 
{
	// TODO: Add your command handler code here
	mnuRecFile_Click(ID_REC_FILE3);
	
}

void CMainFrame::OnRecFile4() 
{
	// TODO: Add your command handler code here
	mnuRecFile_Click(ID_REC_FILE4);
	
}

void CMainFrame::OnRecFile5() 
{
	// TODO: Add your command handler code here
	mnuRecFile_Click(ID_REC_FILE5);
	
}

void CMainFrame::OnRecFile6() 
{
	// TODO: Add your command handler code here
	mnuRecFile_Click(ID_REC_FILE6);
	
}

void CMainFrame::OnRecFile7() 
{
	// TODO: Add your command handler code here
	mnuRecFile_Click(ID_REC_FILE7);
	
}

void CMainFrame::OnRecFile8() 
{
	// TODO: Add your command handler code here
	mnuRecFile_Click(ID_REC_FILE8);
	
}

void CMainFrame::mnuRecFile_Click(int nID)
{
   if(!ModEncrypt::gbLegalUser)
	{
		ShowMessage(GetResStr(IDS_IllegalUserOrDemoVerCantStartACAD));
		return;
	}
	int i,c;
	CMenu* menu=GetMenu()->GetSubMenu(0);
	CMenu* menu1;
	if(menu==NULL)
		return;
	c=menu->GetMenuItemCount();
	for(i=0;i<c;i++)
	{
		menu1=menu->GetSubMenu(i);
		if(menu1)
		{
			menu=menu1;
			break;
		}
	}
	if(menu1==NULL)
		return;
	CString sfile;
	menu->GetMenuString(nID,sfile,MF_BYCOMMAND);
	modPHScal::gsDwgFN =sfile.Mid(3);
   if(modPHScal::gsDwgFN != _T(""))
	{
		EDIBAcad::StartAcad(modPHScal::gsDwgFN);
		AddRecentFiles(modPHScal::gsDwgFN);
		LoadRecentFiles();
	}
}

void CMainFrame::OnUpdateSpecPa(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetText(modPHScal::gsPhsPASel);
	pCmdUI->Enable();
}

void CMainFrame::OnUpdateSpecCspr(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetText(modPHScal::gsPhsConstantSpringSel);
	pCmdUI->Enable();
}

void CMainFrame::OnUpdateSpecBolts(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetText(modPHScal::gsPhsBoltsNutsSel);
	pCmdUI->Enable();
}

void CMainFrame::OnUpdateSpecPart(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetText(modPHScal::gsPhsPARTSel);
	pCmdUI->Enable();
}

void CMainFrame::OnUpdateSpecSa(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetText(modPHScal::gsPhsSASel);
	pCmdUI->Enable();
}

void CMainFrame::OnUpdateSpecSpr(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetText(modPHScal::gsPhsSPRINGSel);
	pCmdUI->Enable();
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class

	
	return CFrameWnd::PreTranslateMessage(pMsg);
}



void CMainFrame::OnCalAddmaterial() 
{
	// TODO: Add your command handler code here
	CDlgAddMaterial dlg;
	dlg.DoModal();
}

void CMainFrame::OnCalAllowMaxSpan() 
{
	// TODO: Add your command handler code here
	if(!IsWindow(gDlgSpan.GetSafeHwnd()))
		gDlgSpan.Create(IDD_DIALOG_SPAN,this);
	gDlgSpan.ShowWindow(SW_SHOW);
	gDlgSpan.UpdateWindow();
}


void CMainFrame::OnDrawZdjtabl() 
{
	// TODO: Add your command handler code here
	DrawZdjTab(iZDJDRAW270L);
}

void CMainFrame::OnDrawZdjtablExce() 
{
	// TODO: Add your command handler code here
	DrawZdjTab(iZDJDRAWEXECL);
}

void CMainFrame::OnSumExcel() 
{
	// TODO: Add your command handler code here
	SumCL(4);	
}

void CMainFrame::OnDrawZdjdirExecl() 
{
	// TODO: Add your command handler code here
	DrawZdjTab(iZDJDRAWCatalogueExcel);
}

void CMainFrame::OnFileOpenDb() 
{
	// TODO: Add your command handler code here
	CFileDialogEx fileDlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,_T("Database Files(*.mdb)|*.mdb|All Files(*.*)|*.*||"));
	if(fileDlg.DoModal()==IDOK)
	{
		CDlgEditDB dlg;
		dlg.m_strDBName=fileDlg.GetPathName();
		dlg.DoModal();
	}
}

void CMainFrame::OnToolWblock() 
{
	// TODO: Add your command handler code here
	try
	{
		EDIBAcad::objAcadDoc.Invoke( _T("SendCommand"),1,&_variant_t(_T("phsw\n")));
	}
	catch(...)
	{
	}
}

void CMainFrame::OnToolInsertUserBlock() 
{
	// TODO: Add your command handler code here
	try
	{
		EDIBAcad::objAcadDoc.Invoke(_T("SendCommand"),1,&_variant_t(_T("phsr\n")));
	}
	catch(CException *e)
	{
		e->Delete();
	}
}

void CMainFrame::OnUpdateToolWblock(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(EDIBAcad::gbACADR14)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable();
}

void CMainFrame::OnUpdateToolInsertuserblock(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(EDIBAcad::gbACADR14)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable();
}

LRESULT CMainFrame::OnUpdOption(WPARAM wParam, LPARAM lParam)
{
	modPHScal::ReadAutoPHSiniValue();
	return TRUE;
}

void CMainFrame::OnPdmstophs() 
{
	// TODO: Add your command handler code here
	ReadPDMS();
}

void CMainFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	// TODO: Add your message handler code here and/or call default
	CRect rc1;
	::SystemParametersInfo(SPI_GETWORKAREA,0,(LPVOID)&rc1,0);
	lpMMI->ptMaxPosition.x=0;
	lpMMI->ptMaxPosition.y=0;
	lpMMI->ptMaxSize.x=rc1.right;
	lpMMI->ptMaxSize.y=this->GetMenuHeight();

}

void CMainFrame::OnMoving(UINT fwSide, LPRECT pRect) 
{
	//CFrameWnd::OnMoving(fwSide, pRect);
	
	// TODO: Add your message handler code here
	CRect rc1;
	::SystemParametersInfo(SPI_GETWORKAREA,0,(LPVOID)&rc1,0);
	//pRect->left=pRect->top=0;
	//pRect->right=rc1.right;
	pRect->top=0;
	pRect->bottom=this->GetMenuHeight();
}

void CMainFrame::OnPhstToPdms() 
{
	// TODO: Add your command handler code here
	CFileDialogEx filedlg(FALSE,NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
	GetResStr(IDS_PDMSFILES_FILTER),this);
	if(filedlg.DoModal()==IDOK)
	{
		::PHSToPDMS(filedlg.GetPathName());
	}
}

void CMainFrame::OnUpdatePdmsToPhs(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnUpdatePhsToPdms(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(TRUE);
	
}

void CMainFrame::OnCalStruct() 
{
	// TODO: Add your command handler code here
	CCalStructDlg dlg;
	dlg.DoModal();
}

void CMainFrame::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	CString sTmp;
	sTmp.Format(GetResStr(IDS_ReallyExitThisForm),EDIBgbl::gsTitle);
	if(ShowMessage(sTmp,MB_YESNO|MB_ICONQUESTION) != IDYES)
			return;
	CFrameWnd::OnClose();
}

BOOL CMainFrame::Start()
{
	EDIBgbl::gsTitle = _T("优易管道支吊架设计");
	EDIBgbl::gsTitle+=_T("(") + ModEncrypt::gstrLegalCompanyName + _T(")");
	SetWindowText(EDIBgbl::gsTitle);

	CString strTemp;
	strTemp.Format(_T("%d"),this->GetSafeHwnd());
	SetRegValue(_T("Settings"),_T("MainWndHandle"),strTemp);
	gObjMainFrm=_T("AutoPHSD");
	CFileStatus filstat;
	CRect rc,rc1;
	::SystemParametersInfo(SPI_GETWORKAREA,0,(LPVOID)&rc1,0);
	int screenW=GetSystemMetrics(SM_CXSCREEN);
	this->SetWindowPos(NULL,0,0,rc1.Width(),GetMenuHeight(),SWP_NOACTIVATE|SWP_NOZORDER);
	ShowWindow(SW_SHOW);

	Cavphs=new Cphs;
	modPHScal::ReadAutoPHSiniValue();

	if(! ModEncrypt::gbLegalUser)
	{
		CMenu* menu=this->GetMenu();
		menu->EnableMenuItem(5,MF_DISABLED|MF_GRAYED|MF_BYPOSITION);
		menu->EnableMenuItem(6,MF_DISABLED|MF_GRAYED|MF_BYPOSITION);
	}
	if( FileExists( basDirectory::InstallDir + _T("AutoPHS.chm")) )
	{
		AfxGetApp()->m_pszHelpFilePath = _tcsdup(basDirectory::InstallDir+_T("AutoPHS.chm"));
	}
	else if ( FileExists( basDirectory::InstallDir + _T("PHS\\Help\\AutoPHS.chm") ) )
	{
		AfxGetApp()->m_pszHelpFilePath = _tcsdup( basDirectory::InstallDir + _T("PHS\\Help\\AutoPHS.chm") );
	}
	else
	{
		AfxGetApp()->m_pszHelpFilePath = _tcsdup( user::GetAppPath() + _T("\\AutoPHS.chm") );
	}

	if(!IsWindow(frmStatus.m_hWnd))
		frmStatus.Create(IDD_STATUS,this);
	if(FrmSplash!=NULL && IsWindow(FrmSplash->m_hWnd))
    {
	   FrmSplash->GetWindowRect(&rc);
	   frmStatus.GetWindowRect(&rc1);
	   int H=rc1.Height();
	   int W=rc1.Width();
	   int SW=::GetSystemMetrics(SM_CXSCREEN);
	   rc1.left=(SW-W)/2;
	   rc1.right=rc1.left+W;
	   rc1.top=rc.bottom+1;
	   rc1.bottom=rc1.top+H;
	   frmStatus.MoveWindow(&rc1);
    }

	Timer1Interval=10;	//仅在计算时打开
	
	::AfxGetApp()->LoadCursor(IDC_WAIT);
	SetMenuEnabled();
	
	HRESULT hr = S_OK;
	if (modPHScal::dbZDJcrude == NULL)
	{
		hr = modPHScal::dbZDJcrude.CreateInstance(__uuidof(Connection));
	}
	CString strConnect;
	strConnect.Format(_T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s;Jet OLEDB:Database Password=%s"),
		basDirectory::ProjectDBDir+_T("zdjcrude.mdb"), ModEncrypt::gstrDBZdjCrudePassWord);
	//以下两句打开了所有的数据库连接
	hr = modPHScal::dbZDJcrude->Open((_bstr_t)strConnect, "", "", adModeUnknown);
	EDIBgbl::InitCurrWork();

	modPHScal::InitZdjTxName();

	if(!modPHScal::blnSelAndSaveSPEC() )
	{
		if(ShowModalSelSpecification()==ID_SELEXIT)
		{
			m_bIsExitMsg=false;
			return FALSE;
		}
	}

	if(IsWindow(frmStatus.m_hWnd) && ::frmStatus.IsWindowVisible()) frmStatus.ShowWindow(SW_HIDE);
   
	try
	{
	  conPRJDB.CreateInstance(__uuidof(Connection));
	  conPRJDB->Open(_bstr_t(::dbConnectionString+basDirectory::DBShareDir+_T("AllPrjDB.mdb")), //20071025 "ProjectDBDir" 改为 "DBShareDir"
		  _T(""),_T(""),0);

	  conPHSConfig.CreateInstance(__uuidof(Connection)); //数据库连接初始化
	  //open 第一个参数：连接的参数和数据库路径
	  conPHSConfig->Open(_bstr_t(::dbConnectionString4+basDirectory::TemplateDir+_T("PHSConfig.mdb")), //20071025 "ProjectDBDir" 改为 "DBShareDir"
			  _T(""),_T(""),0);

	  conZDJCrude.CreateInstance(__uuidof(Connection)); //数据库连接初始化
	  //open 第一个参数：连接的参数和数据库路径
	  conZDJCrude->Open(_bstr_t(::dbConnectionString4+basDirectory::ProjectDBDir+_T("ZdjCrude.mdb")), //20071025 "ProjectDBDir" 改为 "DBShareDir"
		  _T(""),_T(""),0);

	}
	catch( _com_error e)
	{
		AfxMessageBox( e.Description() );
	}


	   
	  LoadRecentFiles();
	

	if(!IsWindow(FrmTxsr.m_hWnd))
	{	
		FrmTxsr.Create(this);
	}
	FrmTxsr.ShowWindow(SW_HIDE);

	OnIntTabint();
	CString ss=EDIBgbl::gsTitle+(EDIBgbl::TBNSelPrjSpec == _T("") ? _T("") : (_T("    ") + EDIBgbl::TBNSelPrjSpec + EDIBgbl::Btype[EDIBgbl::SelBillType] + _T(" (") + EDIBgbl::SelJcdm + _T(")")));
	SetWindowText(ss);
	return TRUE;
}

void CMainFrame::OnUpdateIntImpdata(CCmdUI* pCmdUI) 
{
    pCmdUI->Enable(FALSE);
}

void CMainFrame::OnModifyZdjh() 
{
	// TODO: Add your command handler code here
	CDlgModifyZdjh dlg(EDIBgbl::SelVlmID,this);
	if(dlg.DoModal()==IDOK)
	{
	}
}

//cheng

void CMainFrame::OnGuideEnter()
{
	modPHScal::SelInputMethod = 2;
	Guide GuideEnter;
	GuideEnter.DoModal();
    	
}


void CMainFrame::OnZdjXn() 
{	
	//西南院支吊架明细一览表(180mm,A4)
	DrawZdjTab(iZDJDRAWXN180mm );
}

void CMainFrame::OnExportCurrentVolume() 
{
	CDlgExportVolume exportDlg;
	if( exportDlg.DoModal()==IDOK )
	{		
		
	}	
}

void CMainFrame::OnImportVolume() 
{
	CString strSQL;
	_RecordsetPtr rsRefresh(__uuidof(Recordset));
	::FrmPhsData.m_DBGbill.SetRefDataSource(NULL);
	FrmTxsr.m_pViewTxsr->m_Databill.SetRefRecordset(NULL);

	//import...
	//_bstr_t(dbConnectionString + m_strPrjDBDir + "AllPrjDB.mdb"),


//	rsRefresh->Open(strSQL,pCon,adOpenKeyset,adLockOptimistic,adCmdText);
	FrmTxsr.m_pViewTxsr->m_bAllowUpd=false;
	FrmTxsr.m_pViewTxsr->m_Databill.SetRefRecordset(rsRefresh);
	FrmTxsr.m_pViewTxsr->m_Databill.SetEnabled(TRUE);
	FrmPhsData.m_DBGbill.SetRefDataSource(rsRefresh);
	EDIBDB::SetColumnsProperty(FrmPhsData.m_DBGbill, EDIBgbl::SelBillType);
	FrmTxsr.m_pViewTxsr->m_bAllowUpd=true;
}

void CMainFrame::OnFileBackupDB() 
{
	CTime time;
	CString strFmtTime;
	CString strVersion;

	CString strAppInstallPath;
	DWORD dwVersion=user::GetVersion();
	strVersion.Format(_T("v%d.%d"),MAJOR(dwVersion),MINOR(dwVersion));
	time = time.GetCurrentTime();
	strFmtTime.Format("%d.%d.%d_%d.%d.%d_%s",time.GetYear(),time.GetMonth(),time.GetDay(),
		time.GetHour(),time.GetMinute(),time.GetSecond(),strVersion);
	strAppInstallPath = GetRegKey(_T("Directory"), _T("EDInBox_InsDir"), CString(_T("")));



	//Create Directory for backup
	//////////////////////////////////////////////////////////////////
	CString strBackupPath;
	strBackupPath = strAppInstallPath+"BackupDB";	CreateDirectory(strBackupPath,NULL);	
	strBackupPath = strBackupPath + "\\" + strFmtTime;
	if( !CreateDirectory(strBackupPath,NULL) )
	{
		AfxMessageBox("备份失败，可能是硬盘空间不足");
		return;
	}
	//////////////////////////////////////////////////////////////////



	//Backup File...
	//////////////////////////////////////////////////////////////////
	CString strPrjDBDir = GetRegKey(_T("Directory"),_T("EDInBox_PrjDBDir"),CString(""));
	CopyFile(strPrjDBDir+"AllPrjDB.mdb",strBackupPath+"\\AllPrjDB.mdb",FALSE);
	CopyFile(strPrjDBDir+"Sort.mdb",strBackupPath+"\\Sort.mdb",FALSE);
	//////////////////////////////////////////////////////////////////
}

void CMainFrame::OnFileRestoreDB()
{
	CDlgBackupUndo dlg;
	dlg.DoModal();
}
//Modified by Shuli Luo
//Modify Date : 2003-12-26
void CMainFrame::OnAutoUpdate() 
{
	// TODO: Add your command handler code here
	CString  strRegPath = "SOFTWARE\\长沙优易软件开发有限公司\\AutoUpdate";
	CString  strListFile;
	HKEY hKey;
	BYTE szUpdateExeFile[MAX_PATH];
	CString strUpdateExeFile;
	DWORD dwType,dwLen;
	dwType = REG_SZ;
	dwLen = MAX_PATH;
	memset(szUpdateExeFile,0,MAX_PATH);
	REG_CALL(RegOpenKeyEx(HKEY_LOCAL_MACHINE,strRegPath,0,KEY_ALL_ACCESS,&hKey),"读取注册表失败,请检查您的自动更新组件是否安装");
	REG_CALL(RegQueryValueEx(hKey,"Shell",NULL,&dwType,szUpdateExeFile,&dwLen),"读取注册表失败,请检查您的自动更新组件是否安装");
	REG_CALL(RegCloseKey(hKey),"读取注册表失败,请检查您的自动更新组件是否安装");
	GetModuleFileName(NULL,strListFile.GetBuffer(MAX_PATH),MAX_PATH);
	strListFile.ReleaseBuffer();
	strListFile = strListFile.Left(strListFile.ReverseFind('\\')) + "\\AutoPHS.AUO";
	strUpdateExeFile = szUpdateExeFile;
	//AfxMessageBox("Let's Go");
	ShellExecute(NULL,"open",(LPCSTR)szUpdateExeFile,

		strListFile,strUpdateExeFile.Left(strUpdateExeFile.ReverseFind('\\')),SW_SHOW);
}
//Modify Ended

void CMainFrame::OnDrawZdjArx() 
{
	// TODO: Add your command handler code here
	try
	{
		CString sTmp,sTmp2;
		modPHScal::zdjh=(long)FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("zdjh"));
		if(vtob(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("bCalSuccess")))==false)
		{
			CString strMsg;
			strMsg.Format(IDS_NO_DRAWPHS,modPHScal::zdjh);
			ShowMessage(strMsg);
			return;
		}

		modPHScal::iSelSampleID=(long)FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("iSelSampleID"));
		//pfg20050328
		_variant_t key;
		_RecordsetPtr mlSet;
		CString strZdjh;
		CString strVolumeID;
		CString strSQL;
		CString strPhsName;
		strZdjh.Format("%ld",modPHScal::zdjh);
		strVolumeID.Format("%ld",EDIBgbl::SelVlmID);
		try
		{
			mlSet.CreateInstance(__uuidof(Recordset));
			strSQL="select * from ml where Volumeid="+strVolumeID+" and zdjh="+strZdjh+"";
			mlSet->Open(_variant_t(strSQL),(IDispatch*)conPRJDB,/*adOpenDynamic*/adOpenKeyset,adLockOptimistic,adCmdText);
			key=mlSet->GetCollect(_T("DRAWNA"));
			if(!(key.vt==VT_NULL||key.vt==VT_EMPTY))
			{
				strPhsName=key.bstrVal;
		    	SetRegValue(_T("Settings"),_T("PhsCurZDJName"), strPhsName);
			}
		}catch(_com_error e)
		{
		//	AfxMessageBox(e.Description());
		}
		if(mlSet->State==adStateOpen)
		{
			mlSet->Close();
		}
		//pfg20050328
		SetRegValue(_T("Settings"),_T("PhsCurZdjhNo"),modPHScal::zdjh);
		SetRegValue(_T("Settings"),_T("DrawFlag"),bool(true));

		gStartAcad();
		EDIBAcad::DisplayAcadTop();
		EDIBAcad::objAcadDoc=EDIBAcad::objAcadApp.GetPropertyByName(_T("ActiveDocument")).pdispVal;

		EDIBAcad::objAcadDoc.Invoke(_T("SendCommand"),1,&_variant_t(_T("drawzdj\n")));


		SetRegValue(_T("Settings"),_T("DrawFlag"),bool(false));
		EDIBAcad::MakeNewLayer(_T("DIM"));
		modPHScal::gsPhsColor.MakeUpper();
		if(modPHScal::gsPhsColor==_T("WHITE"))
		{
			EDIBAcad::MakeNewLayer(_T("Phs"), acWhite);
		}
		else
		{
			if(modPHScal::gsPhsColor== _T("GREEN") )
			{
				EDIBAcad::MakeNewLayer(_T("phs"), acGreen);
			}
			else
			{
				EDIBAcad::MakeNewLayer(_T("phs"));
			}
		}
		EDIBAcad::SetDimVariables();
		return;	
	}
	catch(COleException* e)
	{
		e->ReportError();
		e->Delete();
	}
	catch(_com_error e)
	{		
		MessageBox(e.Description());
	}
	catch(CException *e)
	{
		e->Delete();
	}
}

BOOL gStartAcad()
{
 	CString tmpsz;
	_variant_t VarName;
	_variant_t tmpvar;
	CString strTmp,strTmp2;
	static int StartCount=0;
	AfxGetApp()->BeginWaitCursor();
	try
	{
		//11/10 PFG
		if(::FileExists(basDirectory::InstallDir+_T("phs2000.mnc")))
		{
			CFile::Remove(basDirectory::InstallDir+_T("phs2000.mnc"));	
		}
		//11/10 PFG
		if(FAILED(EDIBAcad::objAcadApp.GetActiveObject(EDIBAcad::gstrApplicatonName)))
		{
			StartCount=0;
			if(FAILED(EDIBAcad::objAcadApp.CreateObject(EDIBAcad::gstrApplicatonName)))
			{
				ShowMessage( GetResStr(IDS_NotStartAcad));
				return FALSE;
			}
		}
		tmpvar=EDIBAcad::objAcadApp.GetPropertyByName(_T("Path"));
		basDirectory::AcadwinDir=vtos(tmpvar);

		user::AddDirSep(basDirectory::AcadwinDir);
		user::AddDirSep(basDirectory::TemplateDir);

		if(!::FileExists(basDirectory::AcadwinDir+_T("fonts\\hzfs.shx")))
		{
			if(!CopyFile(basDirectory::TemplateDir + _T("hzfs.shx"),basDirectory::AcadwinDir + _T("fonts\\hzfs.shx"),FALSE))
			{
				if(FileExists(basDirectory::AcadwinDir +_T("fonts\\hztxt.shx")))
					CopyFile(basDirectory::AcadwinDir +_T("fonts\\hztxt.shx"),basDirectory::AcadwinDir+_T("fonts\\hzfs.shx"),FALSE);
				else
					ShowMessage(GetResStr(IDS_AT) + basDirectory::AcadwinDir +GetResStr(IDS_CANNOTSHOW));

			}		
		}
		if(!::FileExists(basDirectory::AcadwinDir+_T("fonts\\hztxt.shx")))
		{
			if(CopyFile(basDirectory::TemplateDir + _T("hzfs.shx"),basDirectory::AcadwinDir + _T("fonts\\hztxt.shx"),FALSE))
			{
			}
		
		}
		tmpvar=EDIBAcad::objAcadApp.GetPropertyByName(_T("Version"));
		EDIBAcad::gsACADVER=vtos(tmpvar);
		if(EDIBAcad::gsACADVER.Left(2)==_T("14"))
		{
			EDIBAcad::gbACADR14=true;
			CMObject ObjPreferences;
			ObjPreferences=EDIBAcad::objAcadApp.GetPropertyByName(_T("Preferences")).pdispVal;
			_variant_t varStr;
			varStr.SetString("<<Unnamed Profile>>");
			ObjPreferences.PutPropertyByName(_T("ActiveProfile"),&varStr);

			EDIBAcad::gbACADR2000 = false;
		}
		else 
		{
			EDIBAcad::gbACADR14 = false;
			if(EDIBAcad::gsACADVER.Left(2)==_T("15"))
			{
				EDIBAcad::gbACADR2000 = true;
			}
			else
			{
				EDIBAcad::gbACADR2000 = false;
			}
		
		}

		EDIBAcad::MoSpace=EDIBAcad::objAcadDoc.GetPropertyByName(_T("ModelSpace")).pdispVal;
		EDIBAcad::PaSpace=EDIBAcad::objAcadDoc.GetPropertyByName(_T("PaperSpace")).pdispVal;
		//Set OSNAP mode for duration of the VB program
		EDIBAcad::sysVarName = _T("OSMODE");
		VarName=_variant_t(EDIBAcad::sysVarName);
		tmpvar=EDIBAcad::objAcadDoc.Invoke(_T("GetVariable"),1,&VarName);
		//sysVarData =(char*)_bstr_t(tmpvar);
		EDIBAcad::osMode = tmpvar.operator long();//VT_I2
		tmpvar.ChangeType(VT_I2);
		tmpvar.intVal=0;
		EDIBAcad::objAcadDoc.Invoke(_T("SetVariable"),2,&VarName,&tmpvar);

	   //设置尺寸变量
		
		EDIBAcad::SetACADCurrentStyle(_T("Dim"),modPHScal::strBigFontDim,modPHScal::strSHXFontDim,modPHScal::fFWDim);
		EDIBAcad::SetACADCurrentStyle(_T("BOM"),modPHScal::strBigFontLjmx,modPHScal::strSHXFontLjmx,modPHScal::fFWLjmx);
		EDIBAcad::SetACADCurrentStyle(_T("LOAD"),modPHScal::strBigFontLoad,modPHScal::strSHXFontLoad,modPHScal::fFWLoad);
		EDIBAcad::SetACADCurrentStyle(_T("hz"));
		EDIBAcad::objAcadDoc.Invoke(_T("SetVariable"),2,&_variant_t(_T("DIMTXSTY")),&_variant_t(_T("Dim")));
	   tmpvar.ChangeType(VT_R8);
	   tmpvar.dblVal=EDIBAcad::g_fDistanceOffset;
	   VarName=_variant_t(_T("dimexo"));
	   EDIBAcad::objAcadDoc.Invoke(_T("SetVariable"),2,&VarName,&tmpvar);
	   //超过尺寸线的部分


	
	   tmpvar.dblVal=EDIBAcad::g_fOvertopDim;
	   VarName=_variant_t(_T("dimexe"));
	   EDIBAcad::objAcadApp.Invoke(_T("SetVariable"),2,&VarName,&tmpvar);
	   tmpvar.ChangeType(VT_BOOL);
	   tmpvar.boolVal=TRUE;
	   EDIBAcad::objAcadApp.PutPropertyByName(_T("Visible"),&tmpvar);
		//if(StartCount==0)
		//	ShowMessage( GetResStr(IDS_AcadStartFinished),MB_SYSTEMMODAL|MB_OK);// vbAfxMessageBox(SetForeground + vbSystemModal*/
		StartCount++;

		EDIBAcad::LoadMyArx("PHS.arx");
	//	EDIBAcad::LoadMyArx("sm.arx");
	}
	catch(CException *e)
	{
		e->Delete();
	}
	catch(...)
	{
		CString str;
		str.LoadString(IDS_AUTOCAD_INITIALIZE_ERROR);
		ShowMessage(str);
		return FALSE;
	}
	return TRUE;
}

void CMainFrame::OnUpdateDrawZdj(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	_variant_t tmpvar;
	CString str("");
	try
	{
		if(SUCCEEDED(EDIBAcad::objAcadApp.GetActiveObject(EDIBAcad::gstrApplicatonName)))
		{
			tmpvar=EDIBAcad::objAcadApp.GetPropertyByName(_T("Version"));
			str = vtos(tmpvar);
			
			if(str.Left(2)==_T("14"))
			{
				pCmdUI->Enable(false);
			}
			else
			{
				pCmdUI->Enable(true);
			}
			return;
		}

		if(modPHScal::gIsACAD==0)//使用AutoCAD
		{
			CString  data_Set;
			data_Set = "SOFTWARE\\Autodesk\\AutoCAD\\";
			HKEY hKEY;
			long ret0 = RegOpenKeyEx(HKEY_LOCAL_MACHINE,data_Set,0,KEY_READ,&hKEY);
			if(ret0 != ERROR_SUCCESS)
			{
				return ;
			}
			
			LPBYTE owner_Get=new BYTE[MAX_PATH];
			DWORD type_1=REG_SZ;			//定义数据类型
			DWORD cbData_1=MAX_PATH;				//定义数据长度
			
			long ret1=::RegQueryValueEx(hKEY,"CurVer",NULL,&type_1,owner_Get,&cbData_1);
			if(ret1!=ERROR_SUCCESS)
			{
			//查找当前版本信息时.在2004版或以上.CAD不建立"CurVer"项的值.		ZSY	
			//如果查找失败,可能是2004版或以上的.或者计算机没安装CAD.

			//	AfxMessageBox("错误：无法查询有关的注册表信息");
			//	return ;
			}
			str = owner_Get;
			delete []owner_Get;
			::RegCloseKey(hKEY);

			if( str.Find("R14") != -1)
			{
				pCmdUI->Enable(false);

			}
			else
			{
				pCmdUI->Enable(true);
			}
		}
		else
		{
			pCmdUI->Enable(true);
		}
	}
	catch(CException *e)
	{
		e->Delete();
	}
	catch(...)
	{
		AfxMessageBox("Error OnUpdateDrawZDj");
	}
	
}

void CMainFrame::OnZdjDrawepHsy() 
{
	// TODO: Add your command handler code here
	DrawZdjTab(iZDJDRAW705mmHSY);	
}

void CMainFrame::OnMenuitemChangmaterial() 
{
	CChangMaterilName dlgMaterilName;
	dlgMaterilName.DoModal();

	// TODO: Add your command handler code here
	
}

void CMainFrame::OnZdjDrawepQgy() 
{
	// TODO: Add your command handler code here
	DrawZdjTab(iZDJDRAW705mmQGY);	
}

void CMainFrame::OnZdjHbyj() 
{
	// TODO: Add your command handler code here
		DrawZdjTab(iZDJDRAW705mmHBYZ );
}
//
//从支吊架中导出已经计算后的数据到AutoIPED(保温油漆)
void CMainFrame::OnPhsToAutoiped() 
{
	EDIBgbl::strExportPaint = "c:\\prjdb4.0\\"; //又作为状态变量,

	CExportPaint Paint;
	if(	!Paint.GetIPEDPath() )
	{
		AfxMessageBox("当前机器没有安装AutoIPED软件!");
	}
	else
	{
		SumCL( 2 );
		Paint.StartupAutoIPED();
	}

	EDIBgbl::strExportPaint = "";

}


// LFX 加  2005.2.26
void CMainFrame::OnCommdboper() 
{
	// TODO: Add your command handler code here
	CDlgDBOper dlgDBOper;
	dlgDBOper.DoModal();
	//if(dlgDBOper.m_hWnd==NULL)
	//{
		//dlgDBOper.Create(IDD_DLG_DBOPER,NULL); 
	//	dlgDBOper.DoModal();
	//}
	//dlgDBOper.ShowWindow(SW_SHOWNORMAL );
	//dlgDBOper.SetWindowPos(&wndTop,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
}

//END LFX 加  2005.2.26

//pfg20050601
void CMainFrame::OnZdjAdrawbjhy() 
{
	DrawZdjTab(iZDJDRAWBJHY01);	
	
}

void CMainFrame::OnZdjBdrawbjhy() 
{
	DrawZdjTab(iZDJDRAWBJHY02);	
	
}

void CMainFrame::OnMENUImportAutoPHS() 
{
	
}

void CMainFrame::OnPlantvisionToPhs() 
{
	CImportPlantVision pv;
	if ( IDOK == pv.DoModal() )
	{
		ReadResult_Excel(FrmTxsr.m_pViewTxsr->m_ActiveRs);
	}
}

void CMainFrame::OnMenuitemOutputpds() 
{
	VARIANT vBookMark;
	vBookMark.vt = VT_I2;
	int nCountRow = 0,iVolID = 0,iPHSNumber = 0,iGNum = 0;
	CArray<int,int> mPHSArray;
	CArray<int,int> mGNumArray;
	
	CString strFirstPHSName;

	nCountRow = FrmPhsData.m_DBGbill.GetSelBookmarks().GetCount();
	if ( nCountRow == 0 )
	{
		AfxMessageBox( _T("请选择要导出的支吊架记录") );
		return;
	}
	for(int i = 0; i < nCountRow; i ++)
	{
		vBookMark.iVal = i;
		FrmPhsData.m_ActiveRs->Bookmark = FrmPhsData.m_DBGbill.GetSelBookmarks().GetItem(vBookMark);
		iVolID = vtoi( FrmPhsData.m_ActiveRs->GetCollect( _variant_t( (long)0 ) ) );
		iPHSNumber = vtoi( FrmPhsData.m_ActiveRs->GetCollect( _variant_t( (long)1 ) ) );
		if ( i== 0 )
		{
			strFirstPHSName.Format( _T("%d"),iPHSNumber );
		}
		iGNum = vtoi( FrmPhsData.m_ActiveRs->GetCollect( _variant_t( (long)13 ) ) );
		mPHSArray.Add( iPHSNumber );
		mGNumArray.Add( iGNum );
	}




	OutputPDSFileTr mOutputPDSFileTr;
	CFileDialogEx filedlg(FALSE,NULL, strFirstPHSName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
		GetResStr(IDS_PDSFILES_FILTER),this);
	if(filedlg.DoModal()==IDOK)
	{
		CString strFileName = filedlg.GetPathName();
		CString strSub = strFileName.Right(4) ;
		strSub.MakeUpper();
		if ( strSub != _T(".TXT") )
			strFileName = strFileName + _T(".txt");
		mOutputPDSFileTr.OutPutText( iVolID,mPHSArray,mGNumArray,strFileName );
	}
	// TODO: Add your command handler code here	
}
