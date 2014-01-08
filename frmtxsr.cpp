// FrmTxsr.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "FrmTxsr.h"
#include "modPHScal.h"
#include "user.h"
#include "modRegistry.h"
#include "edibgbl.h"
#include "MainFrm.h"
#include "edibacad.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFrmTxsr
CFrmTxsr	FrmTxsr;

IMPLEMENT_DYNCREATE(CFrmTxsr, CFrameWnd)

CFrmTxsr::CFrmTxsr()
{
	m_pViewTxsr=NULL;
	m_IsFirst=false;//pfg20050216
}

CFrmTxsr::~CFrmTxsr()
{
	if(m_pViewTxsr)
		delete m_pViewTxsr;
}


BEGIN_MESSAGE_MAP(CFrmTxsr, CFrameWnd)
	//{{AFX_MSG_MAP(CFrmTxsr)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_ACTIVATE()
	ON_WM_DESTROY()
	ON_WM_GETMINMAXINFO()
	ON_WM_MOVING()
	ON_WM_SIZING()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFrmTxsr message handlers

BOOL CFrmTxsr::Create(CWnd* pParentWnd) 
{
	// TODO: Add your specialized code here and/or call the base class
	CString lpszClassName=::AfxRegisterWndClass( 0,::LoadCursor(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_ARROW)),
									(HBRUSH)COLOR_WINDOW,NULL);
	CRect rect;
	//if(!user::GetPos(_T(_T("FrmTxsr")),rect))
		rect=rectDefault;
	return CFrameWnd::Create(lpszClassName, NULL, 
		/*WS_OVERLAPPEDWINDOW*/WS_POPUPWINDOW|WS_CAPTION|WS_SYSMENU|WS_SIZEBOX|WS_MAXIMIZEBOX|WS_MINIMIZEBOX, rectDefault, pParentWnd);
}

int CFrmTxsr::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	CCreateContext Context;
	Context.m_pCurrentFrame=this;
	Context.m_pCurrentDoc=NULL;
	Context.m_pLastView=NULL;
	Context.m_pNewDocTemplate=NULL;
	Context.m_pNewViewClass=RUNTIME_CLASS(CViewTxsr);
	m_pViewTxsr=(CViewTxsr*)this->CreateView(&Context);
	if(m_pViewTxsr==NULL)
		return -1;
	((CView*)m_pViewTxsr)->OnInitialUpdate();
	HICON icon=::LoadIcon(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	
	SetIcon(icon,TRUE);
	CRect rcx,rc1;
	AfxGetApp()->m_pMainWnd->GetWindowRect(&rcx);
	int cx,cy;
	::SystemParametersInfo(SPI_GETWORKAREA,0,(LPVOID)&rc1,0);
	cx=rc1.Width();
	cy=rc1.Height()-rcx.Height();
	this->SetWindowPos(NULL,0,rcx.Height(),cx,cy,SWP_NOZORDER|SWP_NOACTIVATE);
	return 0;
}

void CFrmTxsr::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	ShowWindow(SW_HIDE);
	//CFrameWnd::OnClose();
}

void CFrmTxsr::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CFrameWnd::OnActivate(nState, pWndOther, bMinimized);
	
	if(m_pViewTxsr==NULL || !IsWindow(m_pViewTxsr->m_hWnd))
		return;
	// TODO: Add your message handler code here
	static UINT oldState=0;
	try
	{
		if(!m_pViewTxsr->m_InitWnd || m_pViewTxsr->m_bIsUpd || m_pViewTxsr->m_bIsAddNew)
			return;

		//变量 bIn , bOut 防止窗口多次执行激活事件,防止死循环
		static bool bIn=false;
		static bool bOut=false;
		m_IsFirst=false;//pfg20050224
		if(nState!=WA_INACTIVE)			//窗口进入激活状态
		{	
		//	if(!bIn)//pfg20050216原代码
			if(!m_IsFirst)//pfg20050216
			{//2
				m_IsFirst=true;//pfg20050216
				bIn=true;
				if(!m_pViewTxsr->m_ActiveRs->adoEOF && !m_pViewTxsr->m_ActiveRs->BOF)
				{//1
					//记录集为空时允许执行以下操作！，否则会产生致命错误！
					//if(m_pViewTxsr->m_ActiveRs->Status!=adRecOK)
					//	m_pViewTxsr->m_ActiveRs->Update();
				}//1
				m_pViewTxsr->m_bIsActiveing=true;
				if(m_pViewTxsr->m_ActiveRs->adoEOF && !m_pViewTxsr->m_ActiveRs->BOF) m_pViewTxsr->m_ActiveRs->MovePrevious();
				if(m_pViewTxsr->m_ActiveRs->BOF && !m_pViewTxsr->m_ActiveRs->adoEOF) m_pViewTxsr->m_ActiveRs->MoveNext();
				m_pViewTxsr->m_Databill.SetEnabled(true);
				m_pViewTxsr->m_bIsActiveing=false;
				//if(m_pViewTxsr->m_ActiveRs->EditMode==adEditAdd)
				//	m_pViewTxsr->m_ActiveRs->MoveLast();
				SetWindowText(GetResStr(IDS_MsgBox_60628) + EDIBgbl::Cbtype[EDIBgbl::SelBillType].MnuCaption + _T("    ") + EDIBgbl::TBNSelPrjSpec + EDIBgbl::Btype[EDIBgbl::SelBillType] + _T(" (") + EDIBgbl::SelJcdm + _T(")"));
				m_pViewTxsr->m_bActive=true;
				//if(m_pViewTxsr->m_ActiveRs!=NULL)
				//m_pViewTxsr->m_ActiveRs->Requery(0);
				m_pViewTxsr->RefreshBoundData();
				m_pViewTxsr->DatabillReposition();
				//m_pViewTxsr->LoadGDWItem2ComboGDW1();//pfg20050217在m_pViewTxsr->DatabillReposition()已经调用
				m_pViewTxsr->InitBornSA();
				m_pViewTxsr->LoadDiameterItem2ComboDW();//装入管径数据到组合框
				m_pViewTxsr->LoadPAMaterialItem2ComboDCL1();//装入管部材料到组合框
			//LoadGDWItem2ComboGDW1 //放到data1_reposition事件中


				m_pViewTxsr->LoadStrFLDItem2MyComboBox( m_pViewTxsr->m_comboZDJH1, _T("zdjh"));//装入支吊架号
				m_pViewTxsr->LoadStrFLDItem2MyComboBox( m_pViewTxsr->m_comboJSJDH, _T("jsdbh1"));//装入计算节点号
				if(m_pViewTxsr->m_ActiveRs->GetCollect(_variant_t(_T("JSDBH1"))).vt!=VT_NULL)
					m_pViewTxsr->m_csJSDBH1.Format(_T("%d"),(long)m_pViewTxsr->m_ActiveRs->GetCollect(_variant_t(_T("JSDBH1"))));
				else
					m_pViewTxsr->m_csJSDBH1=_T("");
				int ix;
				ix=m_pViewTxsr->m_comboJSJDH.FindStringExact(-1,m_pViewTxsr->m_csJSDBH1);
				if(ix==-1 && m_pViewTxsr->m_csJSDBH1!=_T(""))
				{//1
					ix=0;
					m_pViewTxsr->m_comboJSJDH.InsertString(0,m_pViewTxsr->m_csJSDBH1);
				}//1
 				m_pViewTxsr->m_comboJSJDH.SetCurSel(ix);
				if(m_pViewTxsr->m_ActiveRs->GetCollect(_variant_t(_T("ZDJH"))).vt!=VT_NULL)
					m_pViewTxsr->m_csZDJH.Format(_T("%d"),(long)m_pViewTxsr->m_ActiveRs->GetCollect(_variant_t(_T("ZDJH"))));
				else
					m_pViewTxsr->m_csZDJH=_T("");
				ix=m_pViewTxsr->m_comboZDJH1.FindStringExact(-1,m_pViewTxsr->m_csZDJH);
				if(ix==-1 && m_pViewTxsr->m_csZDJH!=_T(""))
				{//1
					ix=0;
					m_pViewTxsr->m_comboZDJH1.InsertString(0,m_pViewTxsr->m_csZDJH);
				}//1
				m_pViewTxsr->m_comboZDJH1.SetCurSel(ix);
				m_pViewTxsr->LoadTJItem2ComboT01();

				modPHScal::GetSaveAsDwgFileName(modPHScal::zdjh,m_pViewTxsr->m_ActiveRs);//为了获得图形文件名EDIBAcad::g_strCustomPicNoFileName

				m_pViewTxsr->m_PagItem->m_txtCLB1= EDIBAcad::g_strCustomPicNoFileName;
				m_pViewTxsr->m_PagItem->m_LblPrjName = EDIBgbl::SelPrjName;
				m_pViewTxsr->m_PagItem->UpdateData(FALSE);
				
				m_pViewTxsr->UpdateData(false);
				bIn=false;
			}//2
			else
			{
				bIn=false;
				m_pViewTxsr->m_Databill.SetEnabled(true);//pfg20050218
				m_pViewTxsr->m_bActive=true;//pfg20050218

			}
		}
		else						////窗口退出激活状态
		{
			if(!bOut)
			{
				bOut=true;
				m_pViewTxsr->UpdateBoundData();
				::SetRegValue(_T("Settings"), _T("phsScheduleDwgNo"),modPHScal::gsPhsScheduleDwgNo);
				::SetRegValue(_T("Settings"), _T("phsArrangementDwgNo"),modPHScal::gsPhsArrangementDwgNo);
				
				m_pViewTxsr->m_bActive=false;
				if(!m_pViewTxsr->m_ActiveRs->adoEOF && !m_pViewTxsr->m_ActiveRs->BOF)
				{
					try
					{
						m_pViewTxsr->m_ActiveRs->Update();
					}
					catch(CException *e)
					{
						m_pViewTxsr->m_ActiveRs->CancelUpdate();
					}
				}
				m_pViewTxsr->m_Databill.SetEnabled(false);//pfg20050217原代码
				bOut=false;
			}
			else
				bOut=false;
		}
	}
	catch(_com_error e)
	{
		//ShowMessage(e.Description());
	}
	oldState=nState;
	m_pViewTxsr->m_bIsActiveing=false;
}

void CFrmTxsr::OnDestroy() 
{
	//m_pViewTxsr=NULL;
	//user::SavePos(this,_T(_T("FrmTxsr")));
	CFrameWnd::OnDestroy();
	
	// TODO: Add your message handler code here
}

void CFrmTxsr::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	//CFrameWnd::PostNcDestroy();
}

void CFrmTxsr::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	// TODO: Add your message handler code here and/or call default
	CRect rect,rc1;
	AfxGetApp()->m_pMainWnd->GetWindowRect(&rect);
	::SystemParametersInfo(SPI_GETWORKAREA,0,(LPVOID)&rc1,0);
	lpMMI->ptMaxSize.y=rc1.Height()-rect.Height();
	lpMMI->ptMaxPosition.y=rect.Height();
	lpMMI->ptMaxPosition.x=0;
	lpMMI->ptMaxSize.x=rc1.Width();
}

void CFrmTxsr::OnMoving(UINT fwSide, LPRECT pRect) 
{
	CFrameWnd::OnMoving(fwSide, pRect);
	
	// TODO: Add your message handler code here
	if(pRect->top < ((CMainFrame*)AfxGetMainWnd())->GetMenuHeight())
	{
		CRect rc;
		GetWindowRect(&rc);
		pRect->top=((CMainFrame*)AfxGetMainWnd())->GetMenuHeight();
		pRect->bottom=pRect->top+rc.Height();
	}
}

void CFrmTxsr::OnSizing(UINT fwSide, LPRECT pRect) 
{
	CFrameWnd::OnSizing(fwSide, pRect);
	
	// TODO: Add your message handler code here
	if(pRect->top < ((CMainFrame*)AfxGetMainWnd())->GetMenuHeight())
		pRect->top=((CMainFrame*)AfxGetMainWnd())->GetMenuHeight();

}
