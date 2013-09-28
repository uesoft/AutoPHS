// TxsrPag2.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "TxsrPag2.h"
#include "FrmTxsr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTxsrPag2 dialog

CTxsrPag2::CTxsrPag2(CWnd* pParentWnd )
	: CDialog(CTxsrPag2::IDD,pParentWnd)
{
	//{{AFX_DATA_INIT(CTxsrPag2)
	m_fDIlineDispX = 0.0f;
	m_fDIlineDispY = 0.0f;
	m_fDIlineDispZ = 0.0f;
	m_fDIFx = 0.0f;	
	m_fDIFz = 0.0f;
	m_fDImX = 0.0f;
	m_fDImY = 0.0f;
	m_fDImZ = 0.0f;
	m_fDIRotDispX = 0.0f;
	m_fDIRotDispY = 0.0f;
	m_fDIRotDispZ = 0.0f;
	m_labDx = _T("");
	m_labDy = _T("");
	m_labDz = _T("");
	m_labFx = _T("");
	m_labFz = _T("");
	m_labMx = _T("");
	m_labMy = _T("");
	m_labMz = _T("");
	m_labFy = _T("");	
	m_fDIFy = 0.0f;
	//}}AFX_DATA_INIT
}


void CTxsrPag2::DoDataExchange(CDataExchange* pDX)
{
	
	//目的：FrmTxsr.m_pViewTxsr->m_bIsUpd=true 防止输入框为空时多次执行 FrmTxsr.m_pViewTxsr->OnActive() 而进入本循环体，产生致命错误
	FrmTxsr.m_pViewTxsr->m_bIsUpd=true;
	
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTxsrPag2)
	DDX_Text(pDX, IDC_EDIT1, m_fDIlineDispX);
	DDX_Text(pDX, IDC_EDIT10, m_fDIlineDispY);
	DDX_Text(pDX, IDC_EDIT2, m_fDIlineDispZ);
	DDX_Text(pDX, IDC_EDIT3, m_fDIFx);
	DDX_Text(pDX, IDC_EDIT4, m_fDIFz);
	DDX_Text(pDX, IDC_EDIT5, m_fDImX);
	DDX_Text(pDX, IDC_EDIT6, m_fDImY);
	DDX_Text(pDX, IDC_EDIT7, m_fDImZ);
	DDX_Text(pDX, IDC_EDIT8, m_fDIRotDispX);
	DDX_Text(pDX, IDC_EDIT9, m_fDIRotDispY);
	DDX_Text(pDX, IDC_EDIT12, m_fDIRotDispZ);
	DDX_Text(pDX, IDC_LABEL_DXR, m_labDx);
	DDX_Text(pDX, IDC_LABEL_DYR, m_labDy);
	DDX_Text(pDX, IDC_LABEL_DZR, m_labDz);
	DDX_Text(pDX, IDC_LABEL_FXR, m_labFx);
	DDX_Text(pDX, IDC_LABEL_FZR, m_labFz);
	DDX_Text(pDX, IDC_LABEL_MXR, m_labMx);
	DDX_Text(pDX, IDC_LABEL_MYR, m_labMy);
	DDX_Text(pDX, IDC_LABEL_MZR, m_labMz);
	DDX_Text(pDX, IDC_LABEL_FYR, m_labFy);
	DDX_Text(pDX, IDC_EDIT13, m_fDIFy);
	//}}AFX_DATA_MAP
	FrmTxsr.m_pViewTxsr->m_bIsUpd=false;
}


BEGIN_MESSAGE_MAP(CTxsrPag2, CDialog)
	//{{AFX_MSG_MAP(CTxsrPag2)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTxsrPag2 message handlers



HBRUSH CTxsrPag2::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	
	// TODO: Return a different brush if the default is not desired
	//return hbr;
	/*static CBrush br;
	LOGBRUSH logbr;
	logbr.lbStyle=BS_SOLID;
	if(nCtlColor==CTLCOLOR_DLG || nCtlColor==CTLCOLOR_STATIC)
	{
		logbr.lbColor=0x000080FF;
		pDC->SetBkColor(0x000080FF);
		logbr.lbStyle=BS_SOLID;
	}
	else 
		return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	br.DeleteObject();
	br.CreateBrushIndirect(&logbr);
	return (HBRUSH)br;*/
	return hbr;
}

LRESULT CTxsrPag2::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	/*if(message==WM_COMMAND && (wParam==2 || wParam==1))
	{
		return TRUE;
	}*/
	return CDialog::WindowProc(message, wParam, lParam);
}

void CTxsrPag2::OnOK() 
{
	// TODO: Add extra validation here
	
	//CDialog::OnOK();
}

void CTxsrPag2::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	//CDialog::OnCancel();
}



BOOL CTxsrPag2::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	BOOL bRet;
	CWnd *pParent=NULL;

	pParent=GetParent();

	bRet=CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
	if(!bRet)
	{
		if(pParent)
		{
			return pParent->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
		}
	}

	return bRet;
}
