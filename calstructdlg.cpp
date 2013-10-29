// CalStructDlg.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "CalStructDlg.h"
#include "user.h"
#include "basDirectory.h"
#include "CalStructPag.h"
#include "modPHScal.h"
#include <math.h>
#include "ModEncrypt.h"
#include "GridComboBoxCell.h"
#include "EDIBgbl.h"
#include "StatusBarThread.h"
#include "DlgCalStructRes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCalStructDlg dialog


CCalStructDlg::CCalStructDlg(CWnd* pParent /*=NULL*/)
	: CXDialog(CCalStructDlg::IDD, pParent)
	, m_bAllowInitVar(TRUE)
	, m_iComCount(0)
	, m_pstrType(NULL)
	, m_pnLen(NULL)
	, m_iTmpComCount(0)
	, m_iTabSel(-1)
{
	//{{AFX_DATA_INIT(CCalStructDlg)
	m_strOut = (_T(""));
	m_strStatusIsOk = (_T(""));
	//}}AFX_DATA_INIT
	m_iOldListIndex=-1;
	m_lStructID=0;
	m_lClassID=-1;
	m_lStructIDRes=0;
}


void CCalStructDlg::DoDataExchange(CDataExchange* pDX)
{
	CXDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCalStructDlg)
	DDX_Control(pDX, IDC_EDIT_OK, m_editOk);
	DDX_Control(pDX, IDC_COMBO_TMP, m_comboTmp);
	DDX_Control(pDX, IDC_COMBO_COM_MATERIAL, m_comboComMaterial);
	DDX_Control(pDX, IDC_TAB1, m_tabCal);
	DDX_Control(pDX, IDC_EDIT_OUT, m_edtOut);
	DDX_Control(pDX, IDC_LIST_IMG, m_lstImg);
	DDX_Text(pDX, IDC_EDIT_OUT, m_strOut);
	DDX_Control(pDX, IDC_DATAGRID_ALLDATA, m_gridVar);
	DDX_Control(pDX, IDC_DATAGRID_MAINDIM, m_gridMainDim);
	DDX_Text(pDX, IDC_EDIT_OK, m_strStatusIsOk);
	//}}AFX_DATA_MAP
	DDX_GridControl(pDX, IDC_GRID_PJG, m_PjgGrid);
	DDX_GridControl(pDX, IDC_COM_GRID, m_ComGrid);
}


BEGIN_MESSAGE_MAP(CCalStructDlg, CXDialog)
	//{{AFX_MSG_MAP(CCalStructDlg)
	ON_LBN_SELCHANGE(IDC_LIST_IMG, OnSelChangeListImg)
	ON_BN_CLICKED(IDC_BTN_CAL, OnBtnCal)
	ON_WM_DESTROY()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, OnSelchangeTab1)
	ON_CBN_SELCHANGE(IDC_COMBO_COM_MATERIAL, OnSelchangeComboComMaterial)
	ON_CBN_SELENDOK(IDC_COMBO_TMP, OnSelendokComboTmp)
	ON_CBN_EDITUPDATE(IDC_COMBO_TMP, OnEditupdateComboTmp)
	ON_BN_CLICKED(IDC_BUTTON_LOOP_CAL, OnButtonLoopCal)
	ON_CBN_SELENDOK(IDC_COMBO_COM_MATERIAL, OnSelendokComboComMaterial)
	ON_BN_CLICKED(IDC_BUTTON_SHOW_RES, OnButtonShowRes)
	ON_BN_CLICKED(IDC_BTN_DEFAULTFORMULA, OnBtnDefaultformula)
	//}}AFX_MSG_MAP
	ON_NOTIFY(GVN_ENDLABELEDIT,IDC_COM_GRID,OnEndEditInComGridCell)
	ON_NOTIFY(GVN_ENDLABELEDIT,IDC_GRID_PJG,OnEndEditInPjgGridCell)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCalStructDlg message handlers

BOOL CCalStructDlg::OnInitDialog() 
{
	CXDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CRect rect,rcWnd;
	::SystemParametersInfo(SPI_GETWORKAREA,0,(LPVOID)&rect,0);
	this->GetWindowRect(&rcWnd);
	BOOL bCW=FALSE;
	if(rcWnd.Width()>rect.Width())
	{
		rcWnd.left=0;
		rcWnd.right=rect.right;
		rcWnd.bottom+=::GetSystemMetrics(SM_CYHSCROLL);
		bCW=TRUE;
	}
	if(rcWnd.Height()>rect.Height())
	{
		rcWnd.top=0;
		rcWnd.bottom=rect.bottom;
		if(!bCW)
		{
			rcWnd.right+=::GetSystemMetrics(SM_CXVSCROLL);
			if(rcWnd.Width()>rect.Width())
			{
				rcWnd.left=0;
				rcWnd.right=rect.right;
			}
		}
	}
	this->MoveWindow(&rcWnd);

	m_lstImg.SetBmpSize(296,158);
	try
	{
		m_gridVar.SetForeColor(0xff0000);
		m_gridVar.SetBackColor(0xffc0c0);
		m_gridMainDim.SetForeColor(0xff0000);
		m_gridMainDim.SetBackColor(0xffc0c0);
		m_connMaterial.CreateInstance(__uuidof(Connection));
		m_connSort.CreateInstance(__uuidof(Connection));
		m_connSASCal.CreateInstance(__uuidof(Connection));
		m_connMaterial->Open(_bstr_t(::dbConnectionString + basDirectory::ProjectDBDir + (_T("Material.mdb"))),(_T("")),(_T("")),adConnectUnspecified);//20071102
		m_connSort->Open(_bstr_t(::dbConnectionString + basDirectory::ProjectDBDir + (_T("Sort.mdb"))),(_T("")),(_T("")),adConnectUnspecified);
		m_connSASCal->Open(_bstr_t(::dbConnectionString + basDirectory::ProjectDBDir + (_T("SAStructureCal.mdb"))),(_T("")),(_T("")),adConnectUnspecified);//20071103
		m_ComGrid.SetTextBkColor(::GetSysColor(COLOR_INFOBK));
		m_PjgGrid.SetTextBkColor(::GetSysColor(COLOR_INFOBK));
		m_editOk.SetBkColor(::GetSysColor(COLOR_3DFACE));
		LoadComMaterial();
		LoadImgList();
		LoadTmp();
		OnSelChangeListImg();
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.Description());
	}
	::SetWindowCenter(GetSafeHwnd());
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCalStructDlg::OnSelChangeListImg() 
{
	// TODO: Add your control notification handler code here
	int ix=m_lstImg.GetCurSel();
	if(ix==m_iOldListIndex)
		return;
	m_iOldListIndex=ix;
	DeleteAllTab();
	m_strOut=(_T(""));
	int i;
	try
	{
		_RecordsetPtr rs;
		CString strSQL;
		CString strTemp;
		try
		{
			m_rsVariable->Update();
		}
		catch(CException *e)
		{
		}
		try
		{
			m_rsMainDim->Update();
		}
		catch(CException *e)
		{
		}
		m_gridVar.SetRefDataSource(NULL);
		m_rsVariable=NULL;
		m_rsVariable.CreateInstance(__uuidof(Recordset));

		m_gridMainDim.SetRefDataSource(NULL);
		m_rsMainDim=NULL;
		m_rsMainDim.CreateInstance(__uuidof(Recordset));
		
		m_lStructID=m_lstImg.GetItemData(ix);
		GetDlgItem(IDC_BUTTON_SHOW_RES)->EnableWindow(m_lStructIDRes==m_lStructID);
		strSQL.Format((_T("SELECT * FROM [SAStruct] WHERE [StructID]=%d")),m_lStructID);
		rs=m_connSASCal->Execute(_bstr_t(strSQL),NULL,adCmdText);//20071103 "m_connSort" 改为 "m_connSASCal"
		m_lClassID=-1;
		if(!rs->adoEOF)
		{
			m_lClassID=vtoi(rs->GetCollect((_T("ClassID"))));
		}
		rs->Close();
		CalComLen();
		strSQL.Format((_T("SELECT [StructID],[VarName],[VarValue],[Caption],[bMainDim],[VarNameInImage] FROM [SAStructVariable] WHERE [StructID]=%d AND bMainDim<>-1")),m_lStructID);		
		m_rsVariable->CursorLocation=adUseClient;
		m_rsVariable->Open(_variant_t(strSQL),(IDispatch*)m_connSASCal,adOpenStatic,adLockOptimistic,adCmdText);//20071103 "m_connSort" 改为 "m_connSASCal"

		m_gridVar.SetRefDataSource(m_rsVariable->GetDataSource());
		m_gridVar.GetColumns().GetItem(_variant_t(0L)).SetVisible(FALSE);
		m_gridVar.GetColumns().GetItem(_variant_t(4L)).SetVisible(FALSE);
		m_gridVar.GetColumns().GetItem(_variant_t(1L)).SetCaption(GetResStr(IDS_VARIABLE_NAME));
		m_gridVar.GetColumns().GetItem(_variant_t(2L)).SetCaption(GetResStr(IDS_VARIABLE_VALUE));
		m_gridVar.GetColumns().GetItem(_variant_t(3L)).SetCaption(GetResStr(IDS_DESC));
		m_gridVar.GetColumns().GetItem(_variant_t(5L)).SetCaption(GetResStr(IDS_VARNAME_IN_IMAGE));
		m_gridVar.GetColumns().GetItem(_variant_t(1L)).SetWidth(60.0f);
		m_gridVar.GetColumns().GetItem(_variant_t(2L)).SetWidth(60.0f);
		m_gridVar.GetColumns().GetItem(_variant_t(3L)).SetWidth(240.0f);
		m_gridVar.GetColumns().GetItem(_variant_t(5L)).SetWidth(90.0f);

		strSQL.Format((_T("SELECT [StructID],[VarName],[VarValue],[Caption],[bMainDim],[VarNameInImage] FROM [SAStructVariable] WHERE [StructID]=%d AND bMainDim is not null AND bMainDim <> 0")),m_lStructID);		
		m_rsMainDim->CursorLocation=adUseClient;
		m_rsMainDim->Open(_variant_t(strSQL),(IDispatch*)m_connSASCal,adOpenStatic,adLockOptimistic,adCmdText);//20071103 "m_connSort" 改为 "m_connSASCal"

		m_gridMainDim.SetRefDataSource(m_rsMainDim->GetDataSource());
		m_gridMainDim.GetColumns().GetItem(_variant_t(0L)).SetVisible(FALSE);
		m_gridMainDim.GetColumns().GetItem(_variant_t(4L)).SetVisible(FALSE);
		m_gridMainDim.GetColumns().GetItem(_variant_t(1L)).SetCaption(GetResStr(IDS_VARIABLE_NAME));
		m_gridMainDim.GetColumns().GetItem(_variant_t(2L)).SetCaption(GetResStr(IDS_VARIABLE_VALUE));
		m_gridMainDim.GetColumns().GetItem(_variant_t(3L)).SetCaption(GetResStr(IDS_DESC));
		m_gridMainDim.GetColumns().GetItem(_variant_t(5L)).SetCaption(GetResStr(IDS_VARNAME_IN_IMAGE));
		m_gridMainDim.GetColumns().GetItem(_variant_t(1L)).SetWidth(60.0f);
		m_gridMainDim.GetColumns().GetItem(_variant_t(2L)).SetWidth(60.0f);
		m_gridMainDim.GetColumns().GetItem(_variant_t(3L)).SetWidth(240.0f);
		m_gridMainDim.GetColumns().GetItem(_variant_t(5L)).SetWidth(90.0f);
		CString strFmt=m_gridMainDim.GetColumns().GetItem(_variant_t(3L)).GetNumberFormat();

		strSQL.Format((_T("SELECT [SAStructComponent].[ComponentID],[SAStructFormula].[Result] FROM [SAStructComponent],[SAStructFormula] WHERE [SAStructComponent].[StructID] = %d AND [SAStructComponent].[ComponentID]=[SAStructFormula].[ComponentID] ORDER BY [SAStructComponent].[Number]")),
			m_lStructID);
		rs=m_connSASCal->Execute(_bstr_t(strSQL),NULL,adCmdText);//20071103 "m_connSort" 改为 "m_connSASCal"
		CCalStructPag* pPag;
		for(;!rs->adoEOF;rs->MoveNext())
		{
			pPag=new CCalStructPag(m_connSASCal,m_lStructID,vtoi(rs->GetCollect(0L)),vtos(rs->GetCollect(1L)));//20071103 "m_connSort" 改为 "m_connSASCal"
			pPag->Create(IDD_PAGE_CALSTR,&m_tabCal);
			m_tabCal.AddWnd(pPag);
			m_wndTabs.AddTail(pPag);
		}
		LoadComGrid();
		LoadPjgList();

		for(i=1;i<=m_iComCount;i++)
		{
			InitVar(i,i==1,TRUE,TRUE);
		}
		OnBtnCal();
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.Description());
	}
	GetDlgItem(IDC_BTN_CAL)->EnableWindow(m_tabCal.GetItemCount()>0);
	UpdateData(FALSE);
}

void CCalStructDlg::LoadImgList()
{
	this->m_lstImg.ResetContent();
	try
	{
		_RecordsetPtr rs;
		CString strSQL,strFileName;
		strSQL=(_T("SELECT * FROM [SAStruct] ORDER BY [StructID]"));
		rs=m_connSASCal->Execute(_bstr_t(strSQL),NULL,adCmdText);//20071103 "connSort" 改为 "m_connSASCal"
		for(;!rs->adoEOF;rs->MoveNext())
		{
			strFileName=vtos(rs->GetCollect((_T("BmpName"))));
			int ix=m_lstImg.AddBitmapFile(basDirectory::TemplateDir + strFileName);
			m_lstImg.SetItemData(ix,(DWORD)vtoi(rs->GetCollect((_T("StructID")))));
		}
		if(m_lstImg.GetCount()>0)
		{
			m_lstImg.SetCurSel(0);
		}
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.Description());
	}
}

void CCalStructDlg::DeleteAllTab()
{
	int count=m_tabCal.GetItemCount();
	for(int i=count-1;i>=0;i--)
	{
		m_tabCal.RemoveWnd(i);
	}
	m_wndTabs.RemoveAll();
}

CCalStructPag* CCalStructDlg::GetPag(int index)
{
	POSITION pos=m_wndTabs.GetHeadPosition();
	for(int i=0;pos && i<=index;i++)
	{
		CCalStructPag* pPag=m_wndTabs.GetNext(pos);
		if(i==index)
			return pPag;
	}
	return NULL;
}

void CCalStructDlg::OnBtnCal() 
{
	// TODO: Add your control notification handler code here
	CWaitCursor wait;
	int i,ix;
	int iComNo;
	CCalStructPag *pPag=NULL;
	ix=m_tabCal.GetCurSel();
	if(ix<0) return;
	pPag=GetPag(ix);
	CString strSQL;
	pPag->UpdateData();
	iComNo=pPag->m_iComNo;
	CalInfo theCalInfo;
	StrCom	theStrCom;
	ComPt	theComPt;

	PSSteelInfo pS;
	CString strTemp;
	CGridCell* pCell=(CGridCell*)m_ComGrid.GetCell(iComNo,1);
	strTemp=pCell->GetText();
	if(strTemp==(_T("")))
		return;
	ix=((CGridComboBoxCell*)pCell)->GetCurSel();
	if(ix==-1)
		return;
	pS=(PSSteelInfo)(LPVOID)((CGridComboBoxCell*)pCell)->m_ItemDatas[ix];
	if(pS==NULL)
		return;

	theComPt.strID=pS->szID;
	theComPt.strCustomID=pS->szCustomID;
	theComPt.strClgg=m_ComGrid.GetItemText(iComNo,2);
	if(theComPt.strClgg==(_T("")))
		return;

	CString strMaterial;
	i=m_comboComMaterial.GetCurSel();
	if(i==-1)
		return;
	m_comboComMaterial.GetLBText(i,theComPt.strMaterial);

	m_comboTmp.GetWindowText(strTemp);
	LPTSTR lpszTemp;
	theComPt.fTmp=(float)_tcstod(strTemp,&lpszTemp);
	if(lpszTemp==(LPCTSTR)strTemp)
		return;//温度值无效
	theComPt.nLen=_ttoi(m_ComGrid.GetItemText(iComNo,3));

	theStrCom.lComNo=iComNo;

	theStrCom.lStructID=this->m_lStructID;

	theCalInfo.strResult=pPag->m_strResult;
	theCalInfo.strWhere=pPag->m_strWhere;
	theCalInfo.strFormula=pPag->m_strFormula;
	theCalInfo.pComPt=&theComPt;
	theCalInfo.pStrCom=&theStrCom;
	
	theStrCom.lStructID=this->m_lStructID;
	float fResult;
	DWORD res = CalStruct(&fResult,&theCalInfo);
	CString strMsg;
	switch(res)
	{
	case CAL_RESULT_NULL:
		m_strOut=(_T(""));
		m_strStatusIsOk.LoadString(IDS_INVALID);
		m_editOk.SetTextColor(RGB(255,0,0));
		break;
	case CAL_ERROR:
		m_strOut=(_T(""));
		m_strStatusIsOk.LoadString(IDS_CAL_ERROR);
		m_editOk.SetTextColor(RGB(255,0,0));
		break;
	case CAL_REGULAR:
		m_strOut.Format((_T("%g")),fResult);
		m_strStatusIsOk.Format(IDS_REGULAR,theCalInfo.strResult,GetOpt(theCalInfo.strCPT),theCalInfo.fWhereVal);
		m_editOk.SetTextColor(RGB(0,0,255));
		//MessageBox(strMsg,NULL,MB_OK|MB_ICONINFORMATION);
		break;
	case CAL_NOT_REGULAR:
		m_strOut.Format((_T("%g")),fResult);
		m_strStatusIsOk.Format(IDS_NOT_REGULAR,theCalInfo.strResult,GetOpt(theCalInfo.strCPT),theCalInfo.fWhereVal);
		//MessageBox(strMsg,NULL,MB_OK|MB_ICONERROR);
		m_editOk.SetTextColor(RGB(255,0,0));
		break;	
	}
	UpdateData(FALSE);
}

BEGIN_EVENTSINK_MAP(CCalStructDlg, CXDialog)
    //{{AFX_EVENTSINK_MAP(CCalStructDlg)
	ON_EVENT(CCalStructDlg, IDC_DATAGRID_ALLDATA, 209/* OnBeforeUpdate */, OnBeforeUpdateDatagrid1, VTS_PI2)
	ON_EVENT(CCalStructDlg, IDC_DATAGRID_MAINDIM, 209/* OnBeforeUpdate */, OnBeforeUpdateDatagridMainDim, VTS_PI2)
	ON_EVENT(CCalStructDlg, IDC_DATAGRID_MAINDIM, 201 /* AfterColUpdate */, OnAfterColUpdateDatagridMaindim, VTS_I2)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CCalStructDlg::OnBeforeUpdateDatagrid1(short FAR* Cancel) 
{
	// TODO: Add your control notification handler code here
	m_gridVar.GetColumns().GetItem(_variant_t(0L)).SetValue(_variant_t(m_lStructID));

}

void CCalStructDlg::LoadComMaterial()
{
	m_comboComMaterial.ResetContent();
	CString strSQL;
	try
	{
		strSQL=(_T("SELECT DISTINCT [Material] FROM [MechanicalOfMaterialALPHAt]"));
		_RecordsetPtr rs;
		rs=m_connMaterial->Execute((LPCTSTR)strSQL,NULL,adCmdText);//20071102 "m_connSort" 改为 "m_connMaterial"
		for(;!rs->adoEOF;rs->MoveNext())
		{
			m_comboComMaterial.AddString(vtos(rs->GetCollect(0L)));
		}
		if(m_comboComMaterial.GetCount()>0)
			m_comboComMaterial.SetCurSel(0);
		int ix;
		if((ix=m_comboComMaterial.FindStringExact(0,(_T("Q235-A"))))!=-1)
			m_comboComMaterial.SetCurSel(ix);
		rs->Close();
	}
	catch(_com_error & e)
	{
		AfxMessageBox(e.Description());
	}
}

void CCalStructDlg::DeleteAllComType()
{
	int i;
	BOOL bDelSet=FALSE;
	for(i=1;i<=m_iComCount;i++)
	{
		if(m_ComTypeInfo[i-1].m_astrTypes.GetSize()>0 )
		{
			CGridComboBoxCell* pCell=(CGridComboBoxCell*)m_ComGrid.GetCell(i,1);
			if(pCell)
			{
				for(int i1=0;i1<pCell->m_ItemDatas.GetSize();i1++)
				{
					PSSteelInfo pSSteelInfo=(PSSteelInfo)(LPVOID)(DWORD)pCell->m_ItemDatas[i1];
					if(pSSteelInfo) delete pSSteelInfo;
					pCell->m_ItemDatas[i1]=0;
				}
			}
		}
		else if(bDelSet==FALSE)
		{
			
			CGridComboBoxCell* pCell=(CGridComboBoxCell*)m_ComGrid.GetCell(i,1);
			if(pCell)
			{
				for(int i1=0;i1<pCell->m_ItemDatas.GetSize();i1++)
				{
					PSSteelInfo pSSteelInfo=(PSSteelInfo)(LPVOID)(DWORD)pCell->m_ItemDatas[i1];
					if(pSSteelInfo) delete pSSteelInfo;
					pCell->m_ItemDatas[i1]=0;
				}
			}
			bDelSet=TRUE;
		}
	}
	m_iComCount=0;
}

//DEL void CCalStructDlg::OnSelchangeComboCom() 
//DEL {
//DEL 	// TODO: Add your control notification handler code here
//DEL 	int ix=m_comboCom.GetCurSel();
//DEL 	if(ix==-1)
//DEL 		return;
//DEL 	m_comboComType.SetCurSel(m_pComInfo[ix].iType);
//DEL 	m_comboComMaterial.SetCurSel(m_pComInfo[ix].iMaterial);
//DEL 	LoadComGg();
//DEL 	m_comboComGg.SetCurSel(m_pComInfo[ix].iGg);
//DEL 	m_nLen=m_pComInfo[ix].nLen;
//DEL 	UpdateData(FALSE);
//DEL 	InitVar();
//DEL }

//DEL void CCalStructDlg::OnSelchangeComboComType() 
//DEL {
//DEL 	// TODO: Add your control notification handler code here
//DEL 	int ix=m_comboComType.GetCurSel();
//DEL 	if(ix==-1)
//DEL 		return;
//DEL 	LoadComGg();
//DEL 	int iCom=m_comboCom.GetCurSel();
//DEL 	if(iCom==-1)
//DEL 		return;
//DEL 	m_pComInfo[iCom].iType=ix;
//DEL 	m_pComInfo[iCom].iGg=0;
//DEL 	m_comboComGg.SetCurSel(0);
//DEL 	InitVar();
//DEL }

void CCalStructDlg::InitVar(int iComNo,BOOL bCurComNo,BOOL bOther,BOOL bUpdMaterial,BOOL bUpdCom)
{
	//根椐选择的零件填充零件数据.
	CWaitCursor wait;
	UpdateData(TRUE);
	if(!m_bAllowInitVar)
		return;
	int i,ix;
	PSSteelInfo pS;
	CString strTemp;
	CGridCell* pCell=(CGridCell*)m_ComGrid.GetCell(iComNo,1);
	strTemp=pCell->GetText();
	if(strTemp==(_T("")))
		return;
	ix=((CGridComboBoxCell*)pCell)->GetCurSel();
	if(ix==-1)
		return;
	pS=(PSSteelInfo)(LPVOID)((CGridComboBoxCell*)pCell)->m_ItemDatas[ix];
	if(pS==NULL)
		return;
	ComPt theComPt;
	StrCom theStrCom;
	theComPt.strID=pS->szID;
	theComPt.strCustomID=pS->szCustomID;
	theComPt.strClgg=m_ComGrid.GetItemText(iComNo,2);
	if(theComPt.strClgg==(_T("")))
		return;

//	int iCom;
	CString strMaterial;
	i=m_comboComMaterial.GetCurSel();
	if(i==-1)
		return;
	m_comboComMaterial.GetLBText(i,theComPt.strMaterial);

	m_comboTmp.GetWindowText(strTemp);
	LPTSTR lpszTemp;
	theComPt.fTmp=(float)_tcstod(strTemp,&lpszTemp);
	if(lpszTemp==(LPCTSTR)strTemp)
		return;//温度值无效
	theComPt.nLen=_ttoi(m_ComGrid.GetItemText(iComNo,3));

	theStrCom.lComNo=iComNo;

	theStrCom.lStructID=this->m_lStructID;
	CString strVarSQL;
	CString strVarSQLMainDim;
	try
	{
		try
		{
			m_rsVariable->Update();
		}
		catch(CException *e)
		{
		}
		try
		{
			m_rsMainDim->Update();
		}
		catch(CException *e)
		{
		}
		m_gridVar.SetRefDataSource(NULL);
		strVarSQL=(LPCTSTR)(_bstr_t)m_rsVariable->GetSource();
		m_rsVariable=NULL;
		m_gridMainDim.SetRefDataSource(NULL);
		strVarSQLMainDim=(LPCTSTR)(_bstr_t)m_rsMainDim->GetSource();
		m_rsMainDim=NULL;
		//m_connSort->Close();
	}
	catch(_com_error & e)
	{
		AfxMessageBox(e.Description());
	}
	InitVar(&theComPt,&theStrCom,bCurComNo,bOther,bUpdMaterial,bUpdCom);
	try
	{
		
		EDIBgbl::dbSACal->Execute((_T("UPDATE [SAStructVariable] SET [VarValue]=[VarValue]")), NULL, adCmdText);
		m_rsVariable.CreateInstance(__uuidof(Recordset));
		m_rsVariable->CursorLocation=adUseClient;
		m_rsVariable->Open(_variant_t((LPCTSTR)strVarSQL),(IDispatch*)m_connSASCal,adOpenStatic,adLockOptimistic,adCmdText);//20071103 "m_connSort" 改为 "m_connSASCal"
		m_gridVar.SetRefDataSource(m_rsVariable->GetDataSource());
		m_gridVar.GetColumns().GetItem(_variant_t(0L)).SetVisible(FALSE);
		m_gridVar.GetColumns().GetItem(_variant_t(4L)).SetVisible(FALSE);
		m_gridVar.GetColumns().GetItem(_variant_t(1L)).SetCaption(GetResStr(IDS_VARIABLE_NAME));
		m_gridVar.GetColumns().GetItem(_variant_t(2L)).SetCaption(GetResStr(IDS_VARIABLE_VALUE));
		m_gridVar.GetColumns().GetItem(_variant_t(3L)).SetCaption(GetResStr(IDS_DESC));
		m_gridVar.GetColumns().GetItem(_variant_t(5L)).SetCaption(GetResStr(IDS_VARNAME_IN_IMAGE));
		m_gridVar.GetColumns().GetItem(_variant_t(1L)).SetWidth(60.0f);
		m_gridVar.GetColumns().GetItem(_variant_t(2L)).SetWidth(60.0f);
		m_gridVar.GetColumns().GetItem(_variant_t(3L)).SetWidth(240.0f);
		m_gridVar.GetColumns().GetItem(_variant_t(5L)).SetWidth(90.0f);

		m_rsMainDim.CreateInstance(__uuidof(Recordset));
		m_rsMainDim->CursorLocation=adUseClient;
		m_rsMainDim->Open(_variant_t((LPCTSTR)strVarSQLMainDim),(IDispatch*)m_connSort,adOpenStatic,adLockOptimistic,adCmdText);
		m_gridMainDim.SetRefDataSource(m_rsMainDim->GetDataSource());
		m_gridMainDim.GetColumns().GetItem(_variant_t(0L)).SetVisible(FALSE);
		m_gridMainDim.GetColumns().GetItem(_variant_t(4L)).SetVisible(FALSE);
		m_gridMainDim.GetColumns().GetItem(_variant_t(1L)).SetCaption(GetResStr(IDS_VARIABLE_NAME));
		m_gridMainDim.GetColumns().GetItem(_variant_t(2L)).SetCaption(GetResStr(IDS_VARIABLE_VALUE));
		m_gridMainDim.GetColumns().GetItem(_variant_t(3L)).SetCaption(GetResStr(IDS_DESC));
		m_gridMainDim.GetColumns().GetItem(_variant_t(5L)).SetCaption(GetResStr(IDS_VARNAME_IN_IMAGE));
		m_gridMainDim.GetColumns().GetItem(_variant_t(1L)).SetWidth(60.0f);
		m_gridMainDim.GetColumns().GetItem(_variant_t(2L)).SetWidth(60.0f);
		m_gridMainDim.GetColumns().GetItem(_variant_t(3L)).SetWidth(240.0f);
		m_gridMainDim.GetColumns().GetItem(_variant_t(5L)).SetWidth(90.0f);
	}
	catch(_com_error & e)
	{
		ReportError(e.Description(),__FILE__,__LINE__);
	}
}


void CCalStructDlg::OnDestroy() 
{
	DeleteAllComType();
	CXDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

void CCalStructDlg::OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	if(m_tabCal.GetCurSel() != m_iTabSel)
	{
		m_iTabSel=m_tabCal.GetCurSel();
		CCalStructPag* pPag=(CCalStructPag*)m_tabCal.GetActiveWnd();
		InitVar(pPag->m_iComNo,TRUE,TRUE,FALSE);
		OnBtnCal();
	}
	if(pResult)
		*pResult = 0;
}

void CCalStructDlg::OnSelchangeComboComMaterial() 
{
	// TODO: Add your control notification handler code here
	for(int i=1;i<=m_iComCount;i++)
	{
		InitVar(i,FALSE,FALSE,TRUE,FALSE);
	}
	OnBtnCal();
}

void CCalStructDlg::LoadTmp()
{
	m_comboTmp.ResetContent();
	try
	{
		_RecordsetPtr rs;
		CString strSQL;
		strSQL=(_T("SELECT DISTINCT [t] FROM [MechanicalofMaterialALPHAt] ORDER BY [t]"));
		rs=m_connMaterial->Execute((LPCTSTR)strSQL,NULL,adCmdText);//20071102 "m_connSort" 改为 "m_connMaterial"
		for(;!rs->adoEOF;rs->MoveNext())
		{
			m_comboTmp.AddString(vtos(rs->GetCollect(0L)));
		}
		if(m_comboTmp.GetCount() > 0)
			m_comboTmp.SetCurSel(0);
	}
	catch(_com_error & e)
	{
		AfxMessageBox(e.Description());
	}
}

void CCalStructDlg::OnSelendokComboTmp() 
{
	// TODO: Add your control notification handler code here
	for(int i=1;i<=m_iComCount;i++)
	{
		InitVar(i,FALSE,FALSE,TRUE,FALSE);
	}
	OnBtnCal();
}

void CCalStructDlg::OnEditupdateComboTmp() 
{
	// TODO: Add your control notification handler code here
	
	InitVar(1,FALSE,FALSE,TRUE);
}

DWORD CCalStructDlg::CalStruct(float * pResult,PCalInfo pCalInfo,BOOL bMakeTab,int iTabNo)
{
	CString strSQL;
	//删除临时表
	DWORD res;
	CString strTmpTabName;
	strTmpTabName.Format(_T("tmpCalSAStruct%d"),iTabNo);
	if(bMakeTab)
	{
		try
		{
			//临时表必须放在本地数据库workprj.mdb,不能放在可能共享的数据库sort.mdb中。
			//否则共享时将会混乱其他用户。
			strSQL.Format((_T("DROP TABLE [%s]")),strTmpTabName);
			EDIBgbl::dbPRJ->Execute((_bstr_t)strSQL, NULL, adCmdText);
		}
		catch(CException *e)
		{
		}
	}
	try
	{
		_RecordsetPtr rsVar;
		_RecordsetPtr rsLreal;
		COleVariant vTemp;
		
		HRESULT hr = S_OK;
		hr = rsVar.CreateInstance(__uuidof(Recordset));
//		rsVar.m_pDatabase=&EDIBgbl::dbSORT;
		strSQL.Format((_T("SELECT [VarName],[VarValue] FROM [SAStructVariable] WHERE [StructID]=%d")),pCalInfo->pStrCom->lStructID);
		rsVar->Open((_bstr_t)strSQL,_variant_t((IDispatch*)EDIBgbl::dbSORT,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 

		strSQL.Format(_T("CREATE TABLE [%s] ("),strTmpTabName);
		CString strTemp,strTemp2,strFields;
		strTemp.Format(_T("[%s] Double,"),pCalInfo->strResult);
		strSQL+=strTemp;
		int i=0;
		CString strVarName;
		for(;!rsVar->adoEOF;rsVar->MoveNext(),i++)
		{
			rsVar->get_Collect((_variant_t)0L,vTemp);
			strVarName=vtos(vTemp);
			if(i==0)
			{
				strTemp.Format((_T(" [%s] Double")),strVarName);
				strTemp2.Format((_T(" [%s] ")),strVarName);
			}
			else
			{
				strTemp.Format((_T(", [%s] Double")),strVarName);
				strTemp2.Format((_T(" , [%s] ")),strVarName);
			}
			strSQL+=strTemp;
			strFields+=strTemp2;
		}

		//构件 1,2,3,4 的 φw 由程序计算
		if(1 <= m_lStructID && m_lStructID <= 4)
		{
			if(strSQL.Find(GetResStr(IDS_FD_FI_W))==-1 && strSQL.Find(GetResStr(IDS_FD_FI_W1))==-1)
			{
				strTemp.Format((_T(",%s Double")),GetResStr(IDS_FD_FI_W1));
				strSQL +=strTemp;
			}
		}
		BOOL bCalFi=FALSE;
		if(pCalInfo->strFormula.Find(GetResStr(IDS_FI))!=-1)
			bCalFi=TRUE;
		if(bCalFi && strSQL.Find(GetResStr(IDS_FD_FI))==-1 )
		{
			strTemp.Format((_T(",%s Double")),GetResStr(IDS_FD_FI));
			strSQL +=strTemp;
		}
		

		strSQL+=_T(")");
		if(bMakeTab)
			EDIBgbl::dbPRJ->Execute((_bstr_t)strSQL, NULL, adCmdText);

		if(!bMakeTab)
		{
			strSQL.Format((_T("DELETE FROM [%s]")),strTmpTabName);
			EDIBgbl::dbPRJ->Execute((_bstr_t)strSQL, NULL, adCmdText);
		}
		strSQL.Format((_T("INSERT INTO [%s] ( %s ) VALUES (")),strTmpTabName,strFields);
		
		rsVar->MoveFirst();
		float fVarValue;
		for(i=0;!rsVar->adoEOF;rsVar->MoveNext(),i++)
		{
			rsVar->get_Collect((_variant_t)1L,vTemp);
			fVarValue=vtof(vTemp);
			if(i==0)
				strTemp.Format((_T(" %f")),fVarValue);
			else
				strTemp.Format((_T(", %f")),fVarValue);
			strSQL+=strTemp;
		}
		strSQL+=_T(")");

		EDIBgbl::dbPRJ->Execute((_bstr_t)strSQL, NULL, adCmdText);

		//以上已经获得所有原始数据的值，但是l1/l2/l3...的值尚未计算出来
		
		//此处不有计算 l1,l2,l3,.... , 在用户改变 Grid 的数据时已经计算了.


		CString strFind;
		//构件 1,2,3,4 的 φw 由程序计算
		if(1 <= m_lStructID && m_lStructID <= 4)
		{
			double dFiw=0.0;
			double h=0.0,b=0.0,t=0.0,xs=0.0,l=0.0;

			rsVar->MoveFirst();
			if(rsVar->Find((_bstr_t)(_T("[VarName]=\'h\'")), 0, adSearchForward, vTemp))
			{
				rsVar->get_Collect((_variant_t)1L,vTemp);
				h=vtod(vTemp);
			}

			if(rsVar->Find((_bstr_t)(_T("[VarName]=\'b\'")), 0, adSearchForward, vTemp))
			{
				rsVar->get_Collect((_variant_t)1L,vTemp);
				b=vtod(vTemp);
			}

			if(rsVar->Find((_bstr_t)(_T("[VarName]=\'t\'")), 0, adSearchForward, vTemp))
			{
				rsVar->get_Collect((_variant_t)1L,vTemp);
				t=vtod(vTemp);
			}
			
			strFind.Format((_T("[VarName]=\'%s\'")),GetResStr(IDS_XS));
			if(rsVar->Find((_bstr_t)strFind, 0, adSearchForward, vTemp))
			{
				rsVar->get_Collect((_variant_t)1L,vTemp);
				xs=vtod(vTemp);
			}

			if(rsVar->Find((_bstr_t)(_T("[VarName]=\'l\'")), 0, adSearchForward, vTemp))
			{
				rsVar->get_Collect((_variant_t)1L,vTemp);
				l=vtod(vTemp);
			}
			else if(rsVar->Find((_bstr_t)(_T("[VarName]=\'l1\'")), 0, adSearchForward, vTemp))
			{
				rsVar->get_Collect((_variant_t)1L,vTemp);
				l=vtod(vTemp);
			}

			if(m_lClassID==iSAbeam)
			{
				//简支
				dFiw=570.0 * b * t / l / h * 2400.0 / xs;
			}
			else if(m_lClassID==iSACantilever)
			{
				//悬臂,l:cm
				if( l   <= 50.0)
				{
					dFiw=1.0;
				}
				else if( l <= 100.0)
				{
					dFiw=0.9;
				}
			}
			strSQL.Format((_T("UPDATE [%s] SET %s=%g ")),strTmpTabName,GetResStr(IDS_FD_FI_W),dFiw);
			EDIBgbl::dbPRJ->Execute((_bstr_t)strSQL, NULL, adCmdText);
			strSQL.Format("UPDATE [SAStructVariable] SET [VarValue]=%f WHERE StructID=%d AND VarName=\'%s\'",dFiw,pCalInfo->pStrCom->lStructID,GetResStr(IDS_FD_FI_W));
			EDIBgbl::dbSORT->Execute((_bstr_t)strSQL, NULL, adCmdText);
			strSQL.Format("UPDATE [SAStructVariable] SET [VarValue]=%f WHERE StructID=%d AND VarName=\'%s%d\'",dFiw,pCalInfo->pStrCom->lStructID,GetResStr(IDS_FD_FI_W),pCalInfo->pStrCom->lComNo);
			EDIBgbl::dbSORT->Execute((_bstr_t)strSQL, NULL, adCmdText);
		}

		if(bCalFi)
		{
			//计算φ值
			double l=0.0,Ix=0.0,Iy=0.0,F=0.0,Jmin=0.0f,Lamda=0.0,Fi=0.0,u=0.0;
			strTemp.Format((_T("[VarName]=\'l%d\'")),pCalInfo->pStrCom->lComNo);
			if(rsVar->Find((_bstr_t)strTemp, 0, adSearchForward, vTemp))
			{
				rsVar->get_Collect((_variant_t)1L,vTemp);
				l=vtod(vTemp);
			}

			strTemp.Format((_T("[VarName]=\'Ix%d\'")),pCalInfo->pStrCom->lComNo);
			if(rsVar->Find((_bstr_t)strTemp, 0, adSearchForward, vTemp))
			{
				rsVar->get_Collect((_variant_t)1L,vTemp);
				Ix=vtod(vTemp);
			}
			strTemp.Format((_T("[VarName]=\'Iy%d\'")),pCalInfo->pStrCom->lComNo);
			if(rsVar->Find((_bstr_t)strTemp, 0, adSearchForward, vTemp))
			{
				rsVar->get_Collect((_variant_t)1L,vTemp);
				Iy=vtod(vTemp);
			}
			strTemp.Format((_T("[VarName]=\'F%d\'")),pCalInfo->pStrCom->lComNo);
			if(rsVar->Find((_bstr_t)strTemp, 0, adSearchForward, vTemp))
			{
				rsVar->get_Collect((_variant_t)1L,vTemp);
				F=vtod(vTemp);
			}
			strTemp.Format((_T("[VarName]=\'%s%d\'")),GetResStr(IDS_U),pCalInfo->pStrCom->lComNo);
			if(rsVar->Find((_bstr_t)strTemp, 0, adSearchForward, vTemp))
			{
				rsVar->get_Collect((_variant_t)1L,vTemp);
				u=vtod(vTemp);
			}
			Jmin=(Ix<=Iy ? Ix : Iy);

			Lamda=l*u/sqrt(Jmin/F);//可能会有"除零"错误
		
			Fi=this->GetFiOfLamda(Lamda,pCalInfo->pComPt->strMaterial);
			strSQL.Format((_T("UPDATE [%s] SET [%s]=%g ")),strTmpTabName,GetResStr(IDS_FI),Fi);

			EDIBgbl::dbPRJ->Execute((_bstr_t)strSQL, NULL, adCmdText);
			strSQL.Format("UPDATE [SAStructVariable] SET [VarValue]=%f WHERE StructID=%d AND VarName=\'%s\'",Fi,pCalInfo->pStrCom->lStructID,GetResStr(IDS_FI));
			EDIBgbl::dbSORT->Execute((_bstr_t)strSQL, NULL, adCmdText);
			strSQL.Format("UPDATE [SAStructVariable] SET [VarValue]=%f WHERE StructID=%d AND VarName=\'%s%d\'",Fi,pCalInfo->pStrCom->lStructID,GetResStr(IDS_FI),pCalInfo->pStrCom->lComNo);
			EDIBgbl::dbSORT->Execute((_bstr_t)strSQL, NULL, adCmdText);
		}

		rsVar->Close();
		strSQL.Format((_T("UPDATE [%s] SET [%s]=%s")),strTmpTabName,pCalInfo->strResult,pCalInfo->strFormula);
		EDIBgbl::dbPRJ->Execute((_bstr_t)strSQL, NULL, adCmdText);
		strSQL.Format((_T("SELECT [%s] FROM [%s]")),pCalInfo->strResult,strTmpTabName);

		_RecordsetPtr rs;
		hr = rs.CreateInstance(__uuidof(Recordset));
//		rs.m_pDatabase=&EDIBgbl::dbPRJ;
//		rs.Open(dbOpenSnapshot,strSQL);
		rs->Open((_bstr_t)strSQL,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		
		if(rs->adoEOF)
			res = CAL_RESULT_NULL;
		else
		{
			rs->get_Collect((_variant_t)0L,vTemp);
			*pResult=vtof(vTemp);
			rs->Close();
			strSQL.Format((_T("SELECT [%s] FROM [%s] WHERE %s%s")),
				pCalInfo->strResult,
				strTmpTabName,
				pCalInfo->strResult,pCalInfo->strWhere);
			rs->Open((_bstr_t)strSQL,_variant_t((IDispatch*)EDIBgbl::dbSORT,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 
			CString strMsg;
			if(rs->adoEOF)
			{
				res = CAL_NOT_REGULAR;
			}
			else
			{
				res = CAL_REGULAR;
			}
		}
		rs->Close();
		CString strWVal;
		if((i=pCalInfo->strWhere.Find((_T('='))))!=-1)
		{
			strWVal=pCalInfo->strWhere.Mid(i+1);
			pCalInfo->strCPT=pCalInfo->strWhere.Left(i+1);
		}
		else
		{
			strWVal=pCalInfo->strWhere.Mid(1);
			pCalInfo->strCPT=pCalInfo->strWhere.Left(1);
		}
		strSQL.Format((_T("SELECT %s FROM [%s]")),strWVal,strTmpTabName);
		rs->Open((_bstr_t)strSQL,_variant_t((IDispatch*)EDIBgbl::dbSORT,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		pCalInfo->fWhereVal=0.0f;
		if(!rs->adoEOF)
		{
			rs->get_Collect((_variant_t)0L,vTemp);
			pCalInfo->fWhereVal=vtof(vTemp);
		}
		rs->Close();
	}
	catch(CException *e)
	{
		ReportError( "Exception", __FILE__, __LINE__ );
	}
	/*catch(_com_error e)
	{
		AfxMessageBox(e.Description());
		res=CAL_ERROR;
	}*/
	return res;
}

BOOL CCalStructDlg::InitVar(PComPt pComPt, PStrCom pStrCom,BOOL	 bCurComNo,BOOL bOther,BOOL bUpdMaterial,BOOL bUpdCom)
{
	//根椐选择的零件填充零件数据.
	CString strSQL;
	CString strTemp;
	const TCHAR szSQLFmt[]=_T("UPDATE [SAStructVariable] SET [VarValue]=%f WHERE [StructID]=%d AND [VarName]=\'%s\'");
	const TCHAR szError1[]=_T("找不到指定零件的特性数据!");
	CList < VarInfo , VarInfo& > lstVarInfo;
	VarInfo sVarInfo;
	_RecordsetPtr pRs;
	try
	{
		_RecordsetPtr rs;
		COleVariant vTemp;
		if(bUpdCom)
		{
			//要更新构件数据
//			rs.m_pDatabase=(&modPHScal::dbZDJcrude);
			if(_tcscmp(pComPt->strID,(_T("CS")))==0)
			{
				//槽钢
				strSQL.Format((_T("SELECT * FROM [SSteelPropertyCS] WHERE [BH]=\'%s\'")),pComPt->strClgg);
//				rs.Open(dbOpenSnapshot,strSQL);
				rs->Open((_bstr_t)strSQL,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
					adOpenDynamic, adLockReadOnly, adCmdText); 
				if(rs->adoEOF)
					throw szError1;
				
				rs->get_Collect((_variant_t)(_T("h")),vTemp);
				sVarInfo.fVal=vtof(vTemp)/10.0f;
				_tcscpy(sVarInfo.szName,(_T("h")));
				lstVarInfo.AddTail(sVarInfo);

				
				rs->get_Collect((_variant_t)(_T("b")),vTemp);
				sVarInfo.fVal=vtof(vTemp)/10.0f;
				_tcscpy(sVarInfo.szName,(_T("b")));
				lstVarInfo.AddTail(sVarInfo);

				rs->get_Collect((_variant_t)(_T("d")),vTemp);
				sVarInfo.fVal=vtof(vTemp)/10.0f;
				_tcscpy(sVarInfo.szName,(_T("d")));
				lstVarInfo.AddTail(sVarInfo);

				rs->get_Collect((_variant_t)(_T("t")),vTemp);
				sVarInfo.fVal=vtof(vTemp)/10.0f;
				_tcscpy(sVarInfo.szName,(_T("t")));
				lstVarInfo.AddTail(sVarInfo);

				rs->get_Collect((_variant_t)(_T("Wx")),vTemp);
				sVarInfo.fVal=vtof(vTemp);
				_tcscpy(sVarInfo.szName,(_T("Wx")));
				lstVarInfo.AddTail(sVarInfo);

				rs->get_Collect((_variant_t)(_T("Wy")),vTemp);
				sVarInfo.fVal=vtof(vTemp);
				_tcscpy(sVarInfo.szName,(_T("Wy")));
				lstVarInfo.AddTail(sVarInfo);

				rs->get_Collect((_variant_t)(_T("Ix")),vTemp);
				sVarInfo.fVal=vtof(vTemp);
				_tcscpy(sVarInfo.szName,(_T("Ix")));
				lstVarInfo.AddTail(sVarInfo);

				rs->get_Collect((_variant_t)(_T("Iy")),vTemp);
				sVarInfo.fVal=vtof(vTemp);
				_tcscpy(sVarInfo.szName,(_T("Iy")));
				lstVarInfo.AddTail(sVarInfo);

				rs->get_Collect((_variant_t)(_T("s")),vTemp);
				sVarInfo.fVal=vtof(vTemp);
				_tcscpy(sVarInfo.szName,(_T("F")));
				lstVarInfo.AddTail(sVarInfo);

			}
			else if(_tcscmp(pComPt->strID,(_T("LSS")))==0)
			{
				//角钢
				pComPt->strClgg.MakeLower();
				CString strBH1=pComPt->strClgg.Mid(pComPt->strClgg.Find(_T("x"))+1);
				int ib=_ttoi(pComPt->strClgg.Mid(1));
				strSQL.Format((_T("SELECT * FROM [SSteelPropertyLS] WHERE [BH]=\'%s\' AND [b]=%d ")),strBH1,ib);
				rs->Open((_bstr_t)strSQL,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
					adOpenDynamic, adLockReadOnly, adCmdText); 
				if(rs->adoEOF)
					throw szError1;

				rs->get_Collect((_variant_t)(_T("b")),vTemp);
				sVarInfo.fVal=vtof(vTemp)/10.0f;
				_tcscpy(sVarInfo.szName,(_T("b")));
				lstVarInfo.AddTail(sVarInfo);

				rs->get_Collect((_variant_t)(_T("d")),vTemp);
				sVarInfo.fVal=vtof(vTemp)/10.0f;
				_tcscpy(sVarInfo.szName,(_T("d")));
				lstVarInfo.AddTail(sVarInfo);


				rs->get_Collect((_variant_t)(_T("Wx")),vTemp);
				sVarInfo.fVal=vtof(vTemp);
				_tcscpy(sVarInfo.szName,(_T("Wx")));
				lstVarInfo.AddTail(sVarInfo);

				rs->get_Collect((_variant_t)(_T("Ix")),vTemp);
				sVarInfo.fVal=vtof(vTemp);
				_tcscpy(sVarInfo.szName,(_T("Ix")));
				lstVarInfo.AddTail(sVarInfo);

				rs->get_Collect((_variant_t)(_T("s")),vTemp);
				sVarInfo.fVal=vtof(vTemp);
				_tcscpy(sVarInfo.szName,(_T("F")));
				lstVarInfo.AddTail(sVarInfo);

			}
			else if(_tcscmp(pComPt->strID,(_T("HS")))==0)
			{
				//工字钢
				strSQL.Format((_T("SELECT * FROM [SSteelPropertyHS] WHERE [BH]=\'%s\'")),pComPt->strClgg);
				rs->Open((_bstr_t)strSQL,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
					adOpenDynamic, adLockReadOnly, adCmdText); 
				if(rs->adoEOF)
					throw szError1;
				
				rs->get_Collect((_variant_t)(_T("h")),vTemp);
				sVarInfo.fVal=vtof(vTemp)/10.0f;
				_tcscpy(sVarInfo.szName,(_T("h")));
				lstVarInfo.AddTail(sVarInfo);

				rs->get_Collect((_variant_t)(_T("b")),vTemp);
				sVarInfo.fVal=vtof(vTemp)/10.0f;
				_tcscpy(sVarInfo.szName,(_T("b")));
				lstVarInfo.AddTail(sVarInfo);

				rs->get_Collect((_variant_t)(_T("d")),vTemp);
				sVarInfo.fVal=vtof(vTemp)/10.0f;
				_tcscpy(sVarInfo.szName,(_T("d")));
				lstVarInfo.AddTail(sVarInfo);

				rs->get_Collect((_variant_t)(_T("t")),vTemp);
				sVarInfo.fVal=vtof(vTemp)/10.0f;
				_tcscpy(sVarInfo.szName,(_T("t")));
				lstVarInfo.AddTail(sVarInfo);

				rs->get_Collect((_variant_t)(_T("Wx")),vTemp);
				sVarInfo.fVal=vtof(vTemp);
				_tcscpy(sVarInfo.szName,(_T("Wx")));
				lstVarInfo.AddTail(sVarInfo);

				rs->get_Collect((_variant_t)(_T("Wy")),vTemp);
				sVarInfo.fVal=vtof(vTemp);
				_tcscpy(sVarInfo.szName,(_T("Wy")));
				lstVarInfo.AddTail(sVarInfo);

				rs->get_Collect((_variant_t)(_T("Ix")),vTemp);
				sVarInfo.fVal=vtof(vTemp);
				_tcscpy(sVarInfo.szName,(_T("Ix")));
				lstVarInfo.AddTail(sVarInfo);

				rs->get_Collect((_variant_t)(_T("Iy")),vTemp);
				sVarInfo.fVal=vtof(vTemp);
				_tcscpy(sVarInfo.szName,(_T("Iy")));
				lstVarInfo.AddTail(sVarInfo);

				rs->get_Collect((_variant_t)(_T("s")),vTemp);
				sVarInfo.fVal=vtof(vTemp);
				_tcscpy(sVarInfo.szName,(_T("F")));
				lstVarInfo.AddTail(sVarInfo);
			}
			else
			{
				//其它类型
				strSQL.Format((_T("SELECT * FROM [%s] WHERE [CustomID] IN (SELECT [CustomID] FROM [%s] WHERE [ID]=\'%s\') AND [BH]=\'%s\'")),modPHScal::tbnBoltsNuts,modPHScal::tbnBoltsNutsID,pComPt->strID,pComPt->strClgg);
				rs->Open((_bstr_t)strSQL,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
					adOpenDynamic, adLockReadOnly, adCmdText); 
				if(rs->adoEOF)
					throw szError1;
				rs->get_Collect((_variant_t)(_T("size2")),vTemp);
				sVarInfo.fVal=vtof(vTemp)/10.0f;
				_tcscpy(sVarInfo.szName,(_T("d0")));
				lstVarInfo.AddTail(sVarInfo);
			}
			rs->Close();
		}
		
		if(bUpdMaterial)
		{
			//要更新材料数据
			//填充材料数据.
//			rs.m_pDatabase=&EDIBgbl::dbMaterial;//20071102 "dbSORT" 改为 "dbMaterial"
			BOOL bEOF=FALSE;
			//	α
			float fTmpTemp;

			//	E 钢材在 20度时的弹性模数(公斤力/厘米2)
			strSQL.Format((_T("SELECT  [t],[Et] FROM [MechanicalofMaterialEt] WHERE [Material] ='%s' AND [t]>=%f ORDER BY [t]")),
				pComPt->strMaterial,20.0f);	
			rs->Open((_bstr_t)strSQL,_variant_t((IDispatch*)EDIBgbl::dbMaterial,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 
			_tcscpy(sVarInfo.szName,(_T("E")));
			if(!rs->adoEOF)
			{
				rs->get_Collect((_variant_t)0L,vTemp);
				fTmpTemp=fabs(vtof(vTemp)-20.0f);
				rs->get_Collect((_variant_t)1L,vTemp);
				sVarInfo.fVal=vtof(vTemp) * 100000.0f/EDIBgbl::kgf2N;
				bEOF=FALSE;
			}
			else bEOF=TRUE;
			rs->Close();

			strSQL.Format((_T("SELECT TOP 1 [t],[Et] FROM [MechanicalofMaterialEt] WHERE [Material] ='%s' AND [t]<%f ORDER BY [t] DESC")),
				pComPt->strMaterial,20.0f);
			rs->Open((_bstr_t)strSQL,_variant_t((IDispatch*)EDIBgbl::dbSORT,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 
			if(!rs->adoEOF && !bEOF)
			{
				rs->get_Collect((_variant_t)0L,vTemp);
				if(fTmpTemp > fabs(vtof(vTemp)-20.0f))
				{
					rs->get_Collect((_variant_t)1L,vTemp);
					sVarInfo.fVal=vtof(vTemp) * 100000.0f/EDIBgbl::kgf2N;
				}
				lstVarInfo.AddTail(sVarInfo);
			}
			else if(!rs->adoEOF)
			{
				rs->get_Collect((_variant_t)1L,vTemp);
				sVarInfo.fVal=vtof(vTemp) * 100000.0f/EDIBgbl::kgf2N;
				lstVarInfo.AddTail(sVarInfo);
			}
			else if(!bEOF)
				lstVarInfo.AddTail(sVarInfo);
			rs->Close();


			//	σtj 钢材在计算温度下基本许用应力(公斤力/厘米2)
			strSQL.Format((_T("SELECT TOP 1 [t],[Sigma] FROM [MechanicalofMaterialSIGMAt] WHERE [Material] ='%s' AND [t]>=%f ORDER BY [t]")),
				pComPt->strMaterial,pComPt->fTmp);//,pComPt->strMaterial,pComPt->fTmp);	
			rs->Open((_bstr_t)strSQL,_variant_t((IDispatch*)EDIBgbl::dbSORT,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 
			_tcscpy(sVarInfo.szName,GetResStr(IDS_X_TJ));
			if(!rs->adoEOF)
			{
				rs->get_Collect((_variant_t)0L,vTemp);
				fTmpTemp=fabs(vtof(vTemp)-pComPt->fTmp);
				rs->get_Collect((_variant_t)1L,vTemp);
				sVarInfo.fVal=vtof(vTemp) * 100.0f / EDIBgbl::kgf2N;
				bEOF=FALSE;
			}
			else bEOF=TRUE;
			rs->Close();

			strSQL.Format((_T("SELECT TOP 1 [t],[Sigma] FROM [MechanicalofMaterialSIGMAt] WHERE [Material] ='%s' AND [t]<%f ORDER BY [t] DESC")),
				pComPt->strMaterial,pComPt->fTmp);//,pComPt->strMaterial,pComPt->fTmp);	
			rs->Open((_bstr_t)strSQL,_variant_t((IDispatch*)EDIBgbl::dbSORT,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 
			if(!rs->adoEOF && !bEOF)
			{
				rs->get_Collect((_variant_t)0L,vTemp);
				if(fTmpTemp > fabs(vtof(vTemp)-pComPt->fTmp))
				{
					rs->get_Collect((_variant_t)1L,vTemp);
					sVarInfo.fVal=vtof(vTemp) * 100.0f / EDIBgbl::kgf2N;
				}
				lstVarInfo.AddTail(sVarInfo);
			}
			else if(!rs->adoEOF)
			{
				rs->get_Collect((_variant_t)1L,vTemp);
				sVarInfo.fVal=vtof(vTemp) * 100.0f / EDIBgbl::kgf2N;
				lstVarInfo.AddTail(sVarInfo);
			}
			else if(!bEOF)
				lstVarInfo.AddTail(sVarInfo);

			rs->Close();

			//σs 钢材屈服极限最小值(公斤力/厘米2)
			strSQL.Format((_T("SELECT TOP 1 [SIGMAs] FROM [MechanicalofMaterialSIGMAsb] WHERE [Material] ='%s'")),
				pComPt->strMaterial);	
			rs->Open((_bstr_t)strSQL,_variant_t((IDispatch*)EDIBgbl::dbSORT,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 
			if(rs->adoEOF)
				throw szError1;
			_tcscpy(sVarInfo.szName,GetResStr(IDS_XS));
			rs->get_Collect((_variant_t)0L,vTemp);
			sVarInfo.fVal=vtof(vTemp) * 100.0f / EDIBgbl::kgf2N;
			lstVarInfo.AddTail(sVarInfo);
			rs->Close();
		}

		POSITION pos;
		strSQL.Format((_T("SELECT  [VarValue],[VarName] FROM [SAStructVariable] WHERE [StructID]=%d ")),
			pStrCom->lStructID);
//		rs.m_pDatabase=&EDIBgbl::dbSACal;//20071103 "dbSORT" 改为 "dbSACal"
		rs->Open((_bstr_t)strSQL,_variant_t((IDispatch*)EDIBgbl::dbSACal,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		CString strFind;
		for(pos=lstVarInfo.GetHeadPosition();pos!=0;)
		{
			VarInfo& varInfo=lstVarInfo.GetNext(pos);
			if(_tcscmp(varInfo.szName,_T("l"))!=0 || pStrCom->lComNo==1)
			{
				if(bCurComNo || bUpdMaterial)
				{
					strFind.Format((_T("VarName=\'%s\'")),varInfo.szName);
					VARIANT vTmp;
					if(rs->Find((_bstr_t)strFind, 0, adSearchForward, vTmp))
					{
//						rs.Edit();
						VARIANT var;
						var.vt = VT_R4;
						var.fltVal = varInfo.fVal;
						rs->put_Collect((_variant_t)0L, var);
						rs->Update();
					}
				}
			}
			if(bOther)
			{
				//如果要更新带尾数的记录,如 Ix1,Iy1,Ix2,Iy2,...
				strFind.Format((_T("VarName=\'%s%d\'")),varInfo.szName,pStrCom->lComNo);
				VARIANT vTmp;
				if(rs->Find((_bstr_t)strFind, 0, adSearchForward, vTmp))
				{
//					rs.Edit();
					VARIANT var;
					var.vt = VT_R4;
					var.fltVal = varInfo.fVal;
					rs->put_Collect((_variant_t)0L, var);
					rs->Update();
				}
			}
		}
		rs->Close();
		return TRUE;
	}
	/*catch(_com_error & e)
	{
		AfxMessageBox(e.Description());
		return FALSE;
	}*/
	catch(_TCHAR e[])
	{
		AfxMessageBox(e);
		return FALSE;
	}
	catch(CException *e)
	{
		ReportError("Exception",__FILE__,__LINE__);
	}
	return TRUE;
}

void CCalStructDlg::LoopSelCom(long lStructID)
{

	//循环选择构件形式
	CWaitCursor wait;
	CString strSQL;
	CStatusBarThread* thr=NULL;
	try
	{
		int i,ix;
		CString strMaterial;
		CString strTemp;
		float fTmp;
		_TCHAR * ptTemp;
		COleVariant vTemp;

		i=m_comboComMaterial.GetCurSel();
		if(i==-1)
			return;
		m_comboComMaterial.GetLBText(i,strMaterial);
		m_comboTmp.GetWindowText(strTemp);
		fTmp=(float)_tcstod(strTemp,&ptTemp);
		if(ptTemp==(LPCTSTR)strTemp)
			return;

		CString strIDs;
		for(i=0;i<this->m_iComCount;i++)
		{
			CGridComboBoxCell* pCell=(CGridComboBoxCell*)m_ComGrid.GetCell(i+1,1);
			ix=pCell->GetCurSel();
			if(ix==-1)
				return;
			PSSteelInfo pS=(PSSteelInfo)(LPVOID)pCell->m_ItemDatas[ix];
			if(pS==NULL) return;
			if(i!=0)
				strIDs+=(_T(","));
			strIDs+=(_T("\'"));
			strIDs+=pS->szID;
			strIDs+=(_T("\'"));
		}

		strSQL=(_T("DELETE FROM [SSteelIDBH]"));
		EDIBgbl::dbPRJ->Execute((_bstr_t)strSQL, NULL, adCmdText);
		strSQL.Format(_T("INSERT INTO [SSteelIDBH] SELECT DISTINCT [tb1].[ID],[tb2].[BH],[Weight] As [Weg] FROM [%s] AS [tb1],[%s] AS [tb2]  IN \"\" [;DATABASE=%s;PWD=%s] ")
			_T(" WHERE ")
			_T(" [tb1].[ID] IN (%s) AND ")
			_T(" [tb1].[CustomID]=[tb2].[CustomID] AND ")
			_T(" ( ")
			_T(" ([tb1].[ID]=\'CS\' AND [tb2].[BH] IN (SELECT [BH] FROM [SSteelPropertyCS] IN \"\" [;DATABASE=%s;PWD=%s] )) OR ")
			_T(" ([tb1].[ID]=\'HS\' AND [tb2].[BH] IN (SELECT [BH] FROM [SSteelPropertyHS] IN \"\" [;DATABASE=%s;PWD=%s] )) OR ")
			_T(" ( ")
			_T(" [tb1].[ID]=\'LSS\' AND INSTR(RIGHT([tb2].[BH],LEN([tb2].[BH]) - INSTR([tb2].[BH],\'x\')),\'x\' ) = 0 ")
			_T(" AND EXISTS ")
			_T(" ( ")
			_T(" SELECT * FROM [SSteelPropertyLS]  IN \"\" [;DATABASE=%s;PWD=%s] WHERE ")
			_T(" [b]= VAL(MID([tb2].[BH],2,INSTR([tb2].[BH],\'x\')-2)) AND ")
			_T(" [BH]=RIGHT([tb2].[BH],LEN([tb2].[BH]) - INSTR([tb2].[BH],\'x\')))))"),
			modPHScal::tbnSectionSteelID,modPHScal::tbnSectionSteel,
			basDirectory::ProjectDBDir + (_T("ZdjCrude.mdb")),ModEncrypt::gstrDBZdjCrudePassWord,
			strIDs,
			basDirectory::ProjectDBDir + (_T("ZdjCrude.mdb")),ModEncrypt::gstrDBZdjCrudePassWord,
			basDirectory::ProjectDBDir + (_T("ZdjCrude.mdb")),ModEncrypt::gstrDBZdjCrudePassWord,
			basDirectory::ProjectDBDir + (_T("ZdjCrude.mdb")),ModEncrypt::gstrDBZdjCrudePassWord);
		EDIBgbl::dbPRJ->Execute((LPCTSTR)strSQL, NULL, adCmdText);

		for(i=0;i<this->m_iComCount;i++)
		{
			if(m_ComTypeInfo[i].m_astrTypes.GetSize()!=0)
			{

				strIDs=(_T(""));
				for(int k=0;k<m_ComTypeInfo[i].m_astrTypes.GetSize();k++)
				{
					if(k!=0)
						strIDs+=(_T(","));
					strIDs+=(_T("\'"));
					strIDs+=m_ComTypeInfo[i].m_astrTypes[k];
					strIDs+=(_T("\'"));
				}
				strSQL.Format(_T("INSERT INTO [SSteelIDBH] ([ID],[BH],[Weg]) SELECT [tb1].[ID],[tb2].[BH],[tb2].[Weight] FROM ")
					_T(" [%s] [tb1] ,[%s] [tb2] IN \"\" [;DATABASE=%s;PWD=%s] ")
					_T(" WHERE [tb1].[ID] IN (%s) AND [tb1].[CustomID]=[tb2].[CustomID] "),
					modPHScal::tbnBoltsNutsID,modPHScal::tbnBoltsNuts,
					basDirectory::ProjectDBDir + (_T("ZdjCrude.mdb")),ModEncrypt::gstrDBZdjCrudePassWord,
					strIDs);
				EDIBgbl::dbPRJ->Execute((LPCTSTR)strSQL, NULL, adCmdText);
			}
		}
		strSQL=(_T("DELETE FROM [CalSelList]"));
		EDIBgbl::dbPRJ->Execute((_bstr_t)strSQL, NULL, adCmdText);
		strSQL=(_T("DELETE FROM [CalSelResult]"));
		EDIBgbl::dbPRJ->Execute((_bstr_t)strSQL, NULL, adCmdText);

		
		_RecordsetPtr rs;
		HRESULT hr = S_OK;
		hr = rs.CreateInstance(__uuidof(Recordset));
		CString strFDs,strTbs,strWheres,strWeights;
		for(i=0;i<this->m_iComCount;i++)
		{
			CGridComboBoxCell* pCell=(CGridComboBoxCell*)m_ComGrid.GetCell(i+1,1);
			ix=pCell->GetCurSel();
			if(ix==-1)
				return;
			PSSteelInfo pS=(PSSteelInfo)(LPVOID)pCell->m_ItemDatas[ix];
			if(pS==NULL) return;
			int iLength=_ttoi(m_ComGrid.GetItemText(i+1,3));
			if(i!=0)
			{
				strFDs+=(_T(","));
				strTbs+=(_T(","));
				strWheres+=(_T(" AND "));
				strWeights+=(_T(" + "));
			}
			strTemp.Format((_T("[tb%d].[ID] As [ID%d],[tb%d].[BH] AS [BH%d] , %d As [Length%d] ,[tb%d].[Weg] * %f As [Weight%d]")),i+1,i+1,i+1,i+1,iLength,i+1,i+1,(float)iLength/1000.0f , i+1 );
			strFDs+=strTemp;
			strTemp.Format((_T("[SSteelIDBH] [tb%d]")),i+1);
			strTbs+=strTemp;
			strTemp.Format((_T(" [tb%d].[ID]=\'%s\'")),i+1,pS->szID);
			strWheres+=strTemp;
			strTemp.Format((_T("[Weight%d]")),i+1);
			strWeights+=strTemp;
		}
		int iSelID=1;
		strSQL.Format((_T("INSERT INTO [CalSelList] SELECT DISTINCT %s FROM %s WHERE %s ")),strFDs,strTbs,strWheres);
		EDIBgbl::dbPRJ->Execute((_bstr_t)strSQL, NULL, adCmdText);
		strSQL.Format((_T("UPDATE [CalSelList] SET [Weight]=%s")),strWeights);
		EDIBgbl::dbPRJ->Execute((_bstr_t)strSQL, NULL, adCmdText);

		strSQL=(_T("SELECT * FROM [CalSelList] ORDER BY [Weight]"));
		_RecordsetPtr rs1;
		hr = rs1.CreateInstance(__uuidof(Recordset));
//		rs1.m_pDatabase=&EDIBgbl::dbPRJ;
		rs1->Open((_bstr_t)strSQL,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		rs1->MoveLast();
		rs1->MoveFirst();
		int c=rs1->GetRecordCount();

		strSQL.Format((_T("SELECT [Number],[Result],[Formula],[Where] FROM [SAStructComponent] , [SAStructFormula] WHERE [SAStructComponent].[StructID]=%d ")
			_T(" AND [SAStructComponent].[ComponentID]=[SAStructFormula].[ComponentID] ORDER BY [Number]")),
			this->m_lStructID);
		_RecordsetPtr rsf;
//		rsf.m_pDatabase=&EDIBgbl::dbSORT;
		hr = rsf.CreateInstance(__uuidof(Recordset));
		rsf->Open((_bstr_t)strSQL,_variant_t((IDispatch*)EDIBgbl::dbSORT,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		
		strSQL=(_T("SELECT * FROM [CalSelResult]"));
		_RecordsetPtr rs2;
//		rs2.m_pDatabase=&EDIBgbl::dbPRJ;
		hr = rs2.CreateInstance(__uuidof(Recordset));
		rs2->Open((_bstr_t)strSQL,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 

		i=0;
		hStatusStopEvent=::CreateEvent(NULL,FALSE,FALSE,NULL);
		hStatusCreateEvent=::CreateEvent(NULL,FALSE,FALSE,NULL);
		thr=(CStatusBarThread*)::AfxBeginThread(RUNTIME_CLASS(CStatusBarThread));
		::WaitForSingleObject(hStatusCreateEvent,INFINITE);
		thr->m_StatusBar.UpdateStatus(0.0f);

		StrCom	theStrCom;
		ComPt	theComPt;
		CalInfo theCalInfo;

		theStrCom.lStructID=this->m_lStructID;
		theCalInfo.pComPt=&theComPt;
		theCalInfo.pStrCom=&theStrCom;
		theComPt.fTmp=fTmp;
		theComPt.strMaterial=strMaterial;

		//更新材料数据
		for(int k=0;k<m_iComCount;k++)
		{
			theStrCom.lComNo=k+1;
			InitVar(&theComPt,&theStrCom,FALSE,FALSE,TRUE,FALSE);
		}

		BOOL bMakeTab=TRUE;
		for(i=0;!rs1->adoEOF;rs1->MoveNext(),i++)
		{
			if(::WaitForSingleObject(hStatusStopEvent,0)==WAIT_OBJECT_0)
			{
				goto OutCal;
			}
//			rs1.Edit();
			rs1->put_Collect((_variant_t)(_T("SelID")),COleVariant((long)(i+1)));
			rs1->Update();
			DWORD dwTime1=::GetTickCount();

			//更新每个构件自已的数据
			for(int i1=0;i1<this->m_iComCount;i1++)
			{
				theStrCom.lComNo=i1+1;
				strTemp.Format((_T("Length%d")),i1+1);
				rs1->get_Collect((_variant_t)strTemp,vTemp);
				theComPt.nLen=vtoi(vTemp);
				strTemp.Format((_T("BH%d")),i1+1);
				rs1->get_Collect((_variant_t)strTemp,vTemp);
				theComPt.strClgg=vtos(vTemp);
				strTemp.Format((_T("ID%d")),i1+1);
				rs1->get_Collect((_variant_t)strTemp,vTemp);
				theComPt.strID=vtos(vTemp);
				InitVar(&theComPt,&theStrCom,FALSE,TRUE,FALSE);
			}
			int iTabNo;
			BOOL bRegular=TRUE;
			theStrCom.lComNo=-1;
			for(iTabNo=1,rsf->MoveFirst();!rsf->adoEOF;rsf->MoveNext(),iTabNo++)
			{
				if(::WaitForSingleObject(hStatusStopEvent,0)==WAIT_OBJECT_0)
				{
					goto OutCal;
				}
				rsf->get_Collect((_variant_t)0L,vTemp);
				int iComNo=vtoi(vTemp);

				//更新当前计算的构件的数据,如果上一次更新的构件是当前构件则不用更新
				if(theStrCom.lComNo!=iComNo)
				{
					theStrCom.lComNo=iComNo;
					strTemp.Format((_T("Length%d")),iComNo);
					rs1->get_Collect((_variant_t)strTemp,vTemp);
					theComPt.nLen=vtoi(vTemp);
					strTemp.Format((_T("BH%d")),iComNo);
					rs1->get_Collect((_variant_t)strTemp,vTemp);
					theComPt.strClgg=vtos(vTemp);
					strTemp.Format((_T("ID%d")),iComNo);
					rs1->get_Collect((_variant_t)strTemp,vTemp);
					theComPt.strID=vtos(vTemp);
					InitVar(&theComPt,&theStrCom,TRUE,FALSE,FALSE);
				}
				rsf->get_Collect((_variant_t)2L,vTemp);
				theCalInfo.strFormula=vtos(vTemp);
				rsf->get_Collect((_variant_t)1L,vTemp);
				theCalInfo.strResult=vtos(vTemp);
				rsf->get_Collect((_variant_t)3L,vTemp);
				theCalInfo.strWhere=vtos(vTemp);
				float fResult=0.0f;
				DWORD code=CalStruct(&fResult,&theCalInfo,bMakeTab,iTabNo);
				if(bRegular)
					bRegular=(code==CAL_REGULAR);
				strSQL.Format((_T("INSERT INTO [CalSelResult] ([SelID],[ComNo],[ResultName],[Result],[Regular]) ")
					_T("VALUES(%d,%d,\'%s\',%0.3f,%d)")),
					i+1,theStrCom.lComNo,theCalInfo.strResult,fResult,(BOOL)(code==CAL_REGULAR));
				EDIBgbl::dbPRJ->Execute((_bstr_t)strSQL, NULL, adCmdText);
			}
			bMakeTab=FALSE;
//			rs1.Edit();
			VARIANT var;
			var.vt = VT_INT;
			var.intVal = bRegular;
			rs1->put_Collect((_variant_t)(_T("Regular")), var);
			rs1->Update();
			thr->m_StatusBar.UpdateStatus((float)i/(float)c);
		}
OutCal:
		rs1->Close();
		rs2->Close();
		rsf->Close();
		if(thr!=NULL)
		{
			thr->EndThread();
			::CloseHandle(hStatusStopEvent);
			::CloseHandle(hStatusCreateEvent);
			hStatusStopEvent=NULL;
			hStatusCreateEvent=NULL;
			thr=NULL;
		}

		EDIBgbl::dbPRJ->Execute(_T("UPDATE [CalSelResult] SET [Regular]=[Regular]"), NULL, adCmdText);
		delete [] m_pstrType;
		delete [] m_pnLen;

		m_iTmpComCount=m_iComCount;
		m_pstrType=new CString [ m_iTmpComCount];
		m_pnLen=new UINT[m_iTmpComCount];
		for(i=0;i<this->m_iTmpComCount;i++)
		{
			CGridComboBoxCell* pCell=(CGridComboBoxCell*)m_ComGrid.GetCell(i+1,1);
			m_pstrType[i]=pCell->GetText();
			pCell=(CGridComboBoxCell*)m_ComGrid.GetCell(i+1,3);
			m_pnLen[i]=_ttoi(pCell->GetText());
		}
		m_lStructIDRes=m_lStructID;
		GetDlgItem(IDC_BUTTON_SHOW_RES)->EnableWindow(TRUE);
		CDlgCalStructRes dlg(m_iTmpComCount,m_pstrType,m_pnLen);
		dlg.DoModal();
	}
	catch(_com_error * e)
	{
	}
	if(thr!=NULL)
	{
		thr->EndThread();
		::CloseHandle(hStatusStopEvent);
		::CloseHandle(hStatusCreateEvent);
		hStatusStopEvent=NULL;
		hStatusCreateEvent=NULL;
		thr=NULL;
	}
}

void CCalStructDlg::OnButtonLoopCal() 
{
	// TODO: Add your control notification handler code here
	CString strVarSQL,strVarSQLMainDim;
	try
	{
		m_gridVar.SetRefDataSource(NULL);
		strVarSQL=(LPCTSTR)(_bstr_t)m_rsVariable->GetSource();
		m_rsVariable=NULL;
		m_gridMainDim.SetRefDataSource(NULL);
		strVarSQLMainDim=(LPCTSTR)(_bstr_t)m_rsMainDim->GetSource();
		m_rsMainDim=NULL;
		m_connSort->Close();
	}
	catch(_com_error & e)
	{
		AfxMessageBox(e.Description());
	}
	LoopSelCom(this->m_lStructID);
	try
	{
		m_connSort->Open(_bstr_t(::dbConnectionString + basDirectory::ProjectDBDir + (_T("Sort.mdb"))),(_T("")),(_T("")),adConnectUnspecified);
		m_rsVariable.CreateInstance(__uuidof(Recordset));
		m_rsVariable->CursorLocation=adUseClient;
		m_rsVariable->Open(_variant_t((LPCTSTR)strVarSQL),(IDispatch*)m_connSort,adOpenStatic,adLockOptimistic,adCmdText);
		m_gridVar.SetRefDataSource(m_rsVariable->GetDataSource());
		m_gridVar.GetColumns().GetItem(_variant_t(0L)).SetVisible(FALSE);
		m_gridVar.GetColumns().GetItem(_variant_t(4L)).SetVisible(FALSE);
		m_gridVar.GetColumns().GetItem(_variant_t(1L)).SetCaption(GetResStr(IDS_VARIABLE_NAME));
		m_gridVar.GetColumns().GetItem(_variant_t(2L)).SetCaption(GetResStr(IDS_VARIABLE_VALUE));
		m_gridVar.GetColumns().GetItem(_variant_t(3L)).SetCaption(GetResStr(IDS_DESC));
		m_gridVar.GetColumns().GetItem(_variant_t(5L)).SetCaption(GetResStr(IDS_VARNAME_IN_IMAGE));
		m_gridVar.GetColumns().GetItem(_variant_t(1L)).SetWidth(60.0f);
		m_gridVar.GetColumns().GetItem(_variant_t(2L)).SetWidth(60.0f);
		m_gridVar.GetColumns().GetItem(_variant_t(3L)).SetWidth(240.0f);
		m_gridVar.GetColumns().GetItem(_variant_t(5L)).SetWidth(90.0f);

		m_rsMainDim.CreateInstance(__uuidof(Recordset));
		m_rsMainDim->CursorLocation=adUseClient;
		m_rsMainDim->Open(_variant_t((LPCTSTR)strVarSQLMainDim),(IDispatch*)m_connSort,adOpenStatic,adLockOptimistic,adCmdText);
		m_gridMainDim.SetRefDataSource(m_rsMainDim->GetDataSource());
		m_gridMainDim.GetColumns().GetItem(_variant_t(0L)).SetVisible(FALSE);
		m_gridMainDim.GetColumns().GetItem(_variant_t(4L)).SetVisible(FALSE);
		m_gridMainDim.GetColumns().GetItem(_variant_t(1L)).SetCaption(GetResStr(IDS_VARIABLE_NAME));
		m_gridMainDim.GetColumns().GetItem(_variant_t(2L)).SetCaption(GetResStr(IDS_VARIABLE_VALUE));
		m_gridMainDim.GetColumns().GetItem(_variant_t(3L)).SetCaption(GetResStr(IDS_DESC));
		m_gridMainDim.GetColumns().GetItem(_variant_t(5L)).SetCaption(GetResStr(IDS_VARNAME_IN_IMAGE));
		m_gridMainDim.GetColumns().GetItem(_variant_t(1L)).SetWidth(60.0f);
		m_gridMainDim.GetColumns().GetItem(_variant_t(2L)).SetWidth(60.0f);
		m_gridMainDim.GetColumns().GetItem(_variant_t(3L)).SetWidth(240.0f);
		m_gridMainDim.GetColumns().GetItem(_variant_t(5L)).SetWidth(90.0f);
	}
	catch(_com_error & e)
	{
		AfxMessageBox(e.Description());
	}
}

void CCalStructDlg::LoadComGrid()
{
	CString strSQL;
	int i,i1,j;//,c;
	for(i=0;i<m_iComCount;i++)
	{
		for(j=0;j<3;j++)
		{
			m_astrGridSel[i][j]=m_ComGrid.GetItemText(i+1,j+1);
		}
	}

	DeleteAllComType();
	m_ComGrid.DeleteAllItems();
	this->m_iComCount=0;
	m_ComGrid.SetColumnCount(4);
	m_ComGrid.SetFixedRowCount(1);
	m_ComGrid.SetFixedColumnCount(1);
	GV_ITEM item;
	item.mask = GVIF_TEXT;
	item.row=0;
	item.col=0;
	item.strText=(_T(""));
	m_ComGrid.SetItem(&item);
	item.col=1;
	item.strText=(_T("类别"));
	m_ComGrid.SetItem(&item);
	item.col=2;
	item.strText=(_T("规格"));
	m_ComGrid.SetItem(&item);
	m_ComGrid.SetColumnWidth(2,40);
	item.col=3;
	item.strText=_T("长度(cm)");
	m_ComGrid.SetItem(&item);
	try
	{
		strSQL.Format(_T("SELECT [Number],[Name],[Types] FROM [SAStructComponent] WHERE [StructID]=%d "),
			m_lStructID);
		_RecordsetPtr rs;
//		rs.m_pDatabase=&EDIBgbl::dbSORT;
		rs.CreateInstance(__uuidof(Recordset));
		rs->Open((_bstr_t)strSQL,_variant_t((IDispatch*)EDIBgbl::dbSORT,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		CString strTemp;
		COleVariant vTemp,vTemp1;
		
		//加入构件,未填充数据
		for(i=0;!rs->adoEOF;rs->MoveNext(),i++)
		{
			rs->get_Collect((_variant_t)0L,vTemp);
			rs->get_Collect((_variant_t)1L,vTemp1);
			strTemp.Format((_T("构件 %d %s")),vtoi(vTemp),vtos(vTemp1));
			m_ComGrid.InsertRow(strTemp);
			rs->get_Collect((_variant_t)2L,vTemp);
			this->ParseTypes(m_ComTypeInfo[i].m_astrTypes,vtos(vTemp));
			m_ComGrid.SetCellType(i+1,1,RUNTIME_CLASS(CGridComboBoxCell));
			m_ComGrid.SetCellType(i+1,2,RUNTIME_CLASS(CGridComboBoxCell));
			m_ComGrid.SetItemState(i+1,3,m_ComGrid.GetItemState(i+1,3)|GVIS_READONLY);

		}
		rs->Close();

		m_iComCount=i;
		if(m_iComCount>0 )
		{
			//如果有构件

			PSSteelInfo pSSteelInfo;
			strSQL.Format(_T("SELECT [Description],[CustomID],[ID] FROM [%s] WHERE ID IN (\'CS\',\'LSS\',\'HS\')"),
				modPHScal::tbnSectionSteelID);
//			rs.m_pDatabase=&modPHScal::dbZDJcrude;
			rs->Open((_bstr_t)strSQL,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 

			//加入构件的类型信息
			for(i1=0;!rs->adoEOF;rs->MoveNext(),i1++)
			{
				pSSteelInfo=new SSteelInfo;
				rs->get_Collect((_variant_t)1L,vTemp);
				_tcscpy(pSSteelInfo->szCustomID,vtos(vTemp));
				rs->get_Collect((_variant_t)2L,vTemp);
				_tcscpy(pSSteelInfo->szID,vtos(vTemp));
				rs->get_Collect((_variant_t)0L,vTemp);
				strTemp=vtos(vTemp);
				for(i=1;i<=m_iComCount;i++)
				{
					if(m_ComTypeInfo[i-1].m_astrTypes.GetSize()==0)
					{
						//[Types] 字段中无数据
						CGridComboBoxCell* pCell=(CGridComboBoxCell*)m_ComGrid.GetCell(i,1);
						pCell->m_Items.Add(strTemp);
						pCell->m_ItemDatas.Add((DWORD)(LPVOID)pSSteelInfo);
					}
				}
			}
			rs->Close();

			for(i=1;i<=m_iComCount;i++)
			{
				if(m_ComTypeInfo[i-1].m_astrTypes.GetSize()!=0)
				{	
					//[Types] 字段中有数据
					CGridComboBoxCell* pCell;//=(CGridComboBoxCell*)m_ComGrid.GetCell(i,3);
					pCell=(CGridComboBoxCell*)m_ComGrid.GetCell(i,1);

					//形成类型ID字符串,在SQL 中的 'IN (XXX,XXX,XXX...)' 字句中用
					strTemp=(_T(""));
					for(i1=0;i1<m_ComTypeInfo[i-1].m_astrTypes.GetSize();i1++)
					{
						if(i1!=0)
							strTemp+=(_T(","));
						strTemp+=(_T("\'"));
						strTemp+=m_ComTypeInfo[i-1].m_astrTypes[i1];
						strTemp+=(_T("\'"));
					}

					strSQL.Format((_T("SELECT [Description],[CustomID],[ID] FROM [%s] WHERE ID IN (%s)")),
						modPHScal::tbnBoltsNutsID,strTemp);
//					rs.m_pDatabase=&modPHScal::dbZDJcrude;
					rs->Open((_bstr_t)strSQL,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
						adOpenDynamic, adLockReadOnly, adCmdText); 
					for(;!rs->adoEOF;rs->MoveNext())
					{
						pSSteelInfo=new SSteelInfo;
						rs->get_Collect((_variant_t)1L,vTemp);
						_tcscpy(pSSteelInfo->szCustomID,vtos(vTemp));
						rs->get_Collect((_variant_t)2L,vTemp);
						_tcscpy(pSSteelInfo->szID,vtos(vTemp));
						rs->get_Collect((_variant_t)0L,vTemp);
						strTemp=vtos(vTemp);
						pCell->m_Items.Add(strTemp);
						pCell->m_ItemDatas.Add((DWORD)(LPVOID)pSSteelInfo);
					}
				}
			}

			for(i=1;i<=m_iComCount;i++)
			{
				CGridComboBoxCell* pCell=(CGridComboBoxCell*)m_ComGrid.GetCell(i,1);
				if(m_astrGridSel[i-1][0].IsEmpty() || pCell->FindStringExact(m_astrGridSel[i-1][0])==-1)
				{
					pCell->SetCurSel(0);
					m_astrGridSel[i-1][0]=pCell->m_Items[0];
				}
				else
				{
					pCell->SetText(m_astrGridSel[i-1][0]);
				}
				LoadComGridGg(i);

				pCell=(CGridComboBoxCell*)m_ComGrid.GetCell(i,2);
				if(m_astrGridSel[i-1][1].IsEmpty() || pCell->FindStringExact(m_astrGridSel[i-1][1])==-1)
				{
					m_astrGridSel[i-1][1]=pCell->GetText();
				}
				else
				{
					pCell->SetText(m_astrGridSel[i-1][1]);
				}

			}
			//更新长度值 
			UpdateComLen();
		}
	}
	catch(_com_error * e)
	{
	}
	/*catch(_com_error & e)
	{
		AfxMessageBox(e.Description());
	}*/
}

void CCalStructDlg::OnEndEditInComGridCell(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult=TRUE;
	NM_GRIDVIEW* pnmgv=(NM_GRIDVIEW*)pNMHDR;
	if(pnmgv->iRow>=1 && pnmgv->iColumn==1)
	{
		LoadComGridGg(pnmgv->iRow);
	}
	if(pnmgv->iRow>=1 && pnmgv->iRow<=m_iComCount && pnmgv->iColumn>=1 && pnmgv->iColumn<=3)
	{
		CCalStructPag * pPag=(CCalStructPag*)m_tabCal.GetActiveWnd();
		if(pPag!=NULL)
		{
			BOOL bCurNo=(pnmgv->iRow == pPag->m_iComNo);
			InitVar(pnmgv->iRow,bCurNo,TRUE,FALSE);
			OnBtnCal();
		}
	}
}

void CCalStructDlg::LoadComGridGg(int iRow)
{
	CGridComboBoxCell* pCell=(CGridComboBoxCell*)m_ComGrid.GetCell(iRow,1);
	CString strTemp;
	strTemp=pCell->GetText();
	int ix;
	ix=pCell->GetCurSel();
	if(ix==-1)
		return;
	CString strCustomID;
	PSSteelInfo pS=(PSSteelInfo)(LPVOID)(DWORD)pCell->m_ItemDatas[ix];
	if(pS==NULL)
		return;
	strCustomID=pS->szCustomID;
	CString strSQL;
	pCell=(CGridComboBoxCell*)m_ComGrid.GetCell(iRow,2);
	CString strOldGg=pCell->GetText();
	pCell->SetText((_T("")));
	pCell->m_Items.RemoveAll();
	pCell->m_ItemDatas.RemoveAll();
	try
	{
		if(m_ComTypeInfo[iRow-1].m_astrTypes.GetSize()<=0)
		{
			CString strTbn;
			if(_tcscmp(pS->szID,(_T("CS")))==0)
				strTbn=(_T(" AND BH IN (SELECT [BH] FROM [SSteelPropertyCS])"));
			else if(_tcscmp(pS->szID,(_T("LSS")))==0)
				strTbn.Format((_T(" AND INSTR(RIGHT([BH],LEN([BH]) - INSTR([BH],\'x\')),\'x\' ) = 0 ")
				_T(" AND EXISTS (SELECT * FROM [SSteelPropertyLS] WHERE ")
				_T(" [b]= VAL(MID([%s].[BH],2,INSTR([%s].[BH],\'x\')-2)) AND ")
				_T(" [BH]=RIGHT([%s].[BH],LEN([%s].[BH]) - INSTR([%s].[BH],\'x\')) ")
				_T(" )")),
				modPHScal::tbnSectionSteel,modPHScal::tbnSectionSteel,modPHScal::tbnSectionSteel,
				modPHScal::tbnSectionSteel,modPHScal::tbnSectionSteel);
			else if(_tcscmp(pS->szID,(_T("HS")))==0)
				strTbn=(_T(" AND BH IN (SELECT [BH] FROM [SSteelPropertyHS])"));
			strSQL.Format((_T("SELECT [BH] FROM [%s] WHERE [CustomID]=\'%s\' %s")),
				modPHScal::tbnSectionSteel,strCustomID,strTbn);
		}
		else
		{
			strSQL.Format((_T("SELECT [BH] FROM [%s] WHERE [CustomID]=\'%s\'")),
				modPHScal::tbnBoltsNuts,pS->szCustomID);
		}
		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(Recordset));
//		rs.m_pDatabase=&modPHScal::dbZDJcrude;
		rs->Open((_bstr_t)strSQL,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		COleVariant vTemp;
		for(;!rs->adoEOF;rs->MoveNext())
		{
			rs->get_Collect((_variant_t)0L,vTemp);
			strTemp=vtos(vTemp);
			pCell->m_Items.Add(strTemp);
			if(strTemp==strOldGg)
				pCell->SetText(strTemp);
		}
		if(pCell->m_Items.GetSize()>0 && _tcscmp(pCell->GetText(),(_T("")))==0 )
		{
			pCell->SetText(pCell->m_Items[0]);
		}
		CRect rect;
		m_ComGrid.GetCellRect(iRow,2,&rect);
		m_ComGrid.InvalidateRect(&rect);
		rs->Close();
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.Description());
	}
}

void CCalStructDlg::OnSelendokComboComMaterial() 
{
	// TODO: Add your control notification handler code here
	
}



CCalStructDlg::~CCalStructDlg()
{
	delete [] m_pstrType;
	delete [] m_pnLen;
}

void CCalStructDlg::OnButtonShowRes() 
{
	// TODO: Add your control notification handler code here
	if(m_lStructIDRes==m_lStructID)
	{
		CDlgCalStructRes dlg(m_iTmpComCount,m_pstrType,m_pnLen);
		dlg.DoModal();
	}
}

void CCalStructDlg::LoadPjgList()
{

	int i;
	CString strVarName;
	int c=m_PjgGrid.GetRowCount();
	if(c>1)
	{
		for(i=1;i<c;i++)
		{
			strVarName=m_PjgGrid.GetItemText(i,0);
			strVarName.MakeLower();
			m_mapPjg[strVarName]=m_PjgGrid.GetItemText(i,1);
		}
	}
	m_PjgGrid.DeleteAllItems();
	m_PjgGrid.SetColumnCount(2);
	m_PjgGrid.SetFixedColumnCount(1);
	m_PjgGrid.SetRowCount(1);
	m_PjgGrid.SetFixedRowCount(1);
	try
	{
		GV_ITEM item;
		item.mask = GVIF_TEXT;
		item.row=0;
		item.col=1;
		item.strText=(_T("荷载(kgf))"));
		m_PjgGrid.SetItem(&item);

		m_PjgGrid.SetColumnWidth(0,40);
		CString strSQL;
		strSQL.Format((_T("SELECT DISTINCT [VarName] FROM [SAStructVariable] WHERE [StructID]=%d AND [VarName] LIKE \'Pjg*\' ORDER BY [VarName]")),
			this->m_lStructID);
		_RecordsetPtr rs;
//		rs.m_pDatabase=&EDIBgbl::dbSORT;
//		rs.Open(dbOpenSnapshot,strSQL,dbForwardOnly);
		rs->Open((_bstr_t)strSQL,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
			adOpenForwardOnly, adLockReadOnly, adCmdText); 
		COleVariant vTemp;
		for(i=1;!rs->adoEOF;rs->MoveNext(),i++)
		{
			rs->get_Collect((_variant_t)0L,vTemp);
			item.row=i;
			item.col=0;
			item.strText=vtos(vTemp);
			strVarName=vtos(vTemp);
			m_PjgGrid.SetRowCount(i+1);
			m_PjgGrid.SetItem(&item);
			strVarName.MakeLower();
			if(m_mapPjg[strVarName].IsEmpty())
				m_mapPjg[strVarName]=(_T("1000"));
			m_PjgGrid.SetItemText(i,1,m_mapPjg[strVarName]);
		}
		rs->Close();
		UpdatePjg(NULL);
	}
	catch(_com_error * e)
	{
	}
}

void CCalStructDlg::UpdatePjg(LPCTSTR lpszPjg)
{
	CString strSQL;
	if(lpszPjg==NULL)
		strSQL.Format((_T("SELECT [VarName],[VarValue] FROM [SAStructVariable] WHERE [StructID]=%d AND [VarName] LIKE \'Pjg*\' ORDER BY [VarName]")),
			this->m_lStructID);
	else
		strSQL.Format((_T("SELECT [VarName],[VarValue] FROM [SAStructVariable] WHERE [StructID]=%d AND [VarName] = \'%s\' ORDER BY [VarName]")),
			this->m_lStructID,lpszPjg);
	CString strVarSQL,strVarSQLMainDim;
	try
	{
		m_gridVar.SetRefDataSource(NULL);
		strVarSQL=(LPCTSTR)(_bstr_t)m_rsVariable->GetSource();
		m_rsVariable=NULL;
		m_gridMainDim.SetRefDataSource(NULL);
		strVarSQLMainDim=(LPCTSTR)(_bstr_t)m_rsMainDim->GetSource();
		m_rsMainDim=NULL;
		//m_connSort->Close();
	}
	catch(_com_error & e)
	{
		AfxMessageBox(e.Description());
	}
	try
	{
		_RecordsetPtr rs;
//		rs.m_pDatabase=&EDIBgbl::dbSORT;
		rs->Open((_bstr_t)strSQL,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		COleVariant vTemp;
		for(;!rs->adoEOF;rs->MoveNext())
		{
			rs->get_Collect((_variant_t)0L,vTemp);
			CString strVarName=vtos(vTemp);
			strVarName.MakeLower();
			double dValue=_tcstod(m_mapPjg[strVarName],NULL);
//			rs.Edit();
			VARIANT var;
			var.vt = VT_R8;
			var.dblVal = dValue;
			rs->put_Collect((_variant_t)1L, var);
			rs->Update();
		}
		rs->Close();
		
	}
	catch(_com_error * e)
	{
	}
	try
	{
		EDIBgbl::dbSACal->Execute((_T("UPDATE [SAStructVariable] SET [VarValue]=[VarValue]")), NULL, adCmdText);
		//m_connSort->Open(_bstr_t(::dbConnectionString + basDirectory::ProjectDBDir + (_T("Sort.mdb"))),(_T("")),(_T("")),adConnectUnspecified);
		m_rsVariable.CreateInstance(__uuidof(Recordset));
		m_rsVariable->CursorLocation=adUseClient;
		m_rsVariable->Open(_variant_t((LPCTSTR)strVarSQL),(IDispatch*)m_connSort,adOpenStatic,adLockOptimistic,adCmdText);
		m_rsVariable->Requery(-1);
		m_gridVar.SetRefDataSource(m_rsVariable->GetDataSource());
		m_gridVar.GetColumns().GetItem(_variant_t(0L)).SetVisible(FALSE);
		m_gridVar.GetColumns().GetItem(_variant_t(4L)).SetVisible(FALSE);
		m_gridVar.GetColumns().GetItem(_variant_t(1L)).SetCaption(GetResStr(IDS_VARIABLE_NAME));
		m_gridVar.GetColumns().GetItem(_variant_t(2L)).SetCaption(GetResStr(IDS_VARIABLE_VALUE));
		m_gridVar.GetColumns().GetItem(_variant_t(3L)).SetCaption(GetResStr(IDS_DESC));
		m_gridVar.GetColumns().GetItem(_variant_t(5L)).SetCaption(GetResStr(IDS_VARNAME_IN_IMAGE));
		m_gridVar.GetColumns().GetItem(_variant_t(1L)).SetWidth(60.0f);
		m_gridVar.GetColumns().GetItem(_variant_t(2L)).SetWidth(60.0f);
		m_gridVar.GetColumns().GetItem(_variant_t(3L)).SetWidth(240.0f);
		m_gridVar.GetColumns().GetItem(_variant_t(5L)).SetWidth(90.0f);

		m_rsMainDim.CreateInstance(__uuidof(Recordset));
		m_rsMainDim->CursorLocation=adUseClient;
		m_rsMainDim->Open(_variant_t((LPCTSTR)strVarSQLMainDim),(IDispatch*)m_connSort,adOpenStatic,adLockOptimistic,adCmdText);
		m_rsMainDim->Requery(-1);
		m_gridMainDim.SetRefDataSource(m_rsMainDim->GetDataSource());
		m_gridMainDim.GetColumns().GetItem(_variant_t(0L)).SetVisible(FALSE);
		m_gridMainDim.GetColumns().GetItem(_variant_t(4L)).SetVisible(FALSE);
		m_gridMainDim.GetColumns().GetItem(_variant_t(1L)).SetCaption(GetResStr(IDS_VARIABLE_NAME));
		m_gridMainDim.GetColumns().GetItem(_variant_t(2L)).SetCaption(GetResStr(IDS_VARIABLE_VALUE));
		m_gridMainDim.GetColumns().GetItem(_variant_t(3L)).SetCaption(GetResStr(IDS_DESC));
		m_gridMainDim.GetColumns().GetItem(_variant_t(5L)).SetCaption(GetResStr(IDS_VARNAME_IN_IMAGE));
		m_gridMainDim.GetColumns().GetItem(_variant_t(1L)).SetWidth(60.0f);
		m_gridMainDim.GetColumns().GetItem(_variant_t(2L)).SetWidth(60.0f);
		m_gridMainDim.GetColumns().GetItem(_variant_t(3L)).SetWidth(240.0f);
		m_gridMainDim.GetColumns().GetItem(_variant_t(5L)).SetWidth(90.0f);
	}
	catch(_com_error & e)
	{
		AfxMessageBox(e.Description());
	}
}

void CCalStructDlg::OnEndEditInPjgGridCell(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult=TRUE;
	NM_GRIDVIEW* pnmgv=(NM_GRIDVIEW*)pNMHDR;
	CString strVal=m_PjgGrid.GetItemText(pnmgv->iRow,1);
	CString strName=m_PjgGrid.GetItemText(pnmgv->iRow,0);
	strName.MakeLower();
	m_mapPjg[strName]=strVal;
	UpdatePjg(strName);
	OnBtnCal();
}

BOOL CCalStructDlg::ParseTypes(CStringArray &strArray, LPCTSTR lpszTypes)
{
	strArray.RemoveAll();
	LPCTSTR  p1,p2;
	p1=lpszTypes;
	_TCHAR szTemp[64];
	while(*p1!=(_T('\0')) && (p2=_tcschr(p1,(_T(';'))))!=NULL)
	{
		memcpy(szTemp,p1,(p2-p1)*sizeof(_TCHAR));
		szTemp[p2-p1]=(_T('\0'));
		strArray.Add(szTemp);
		p1=p2+1;
	}
	if(*p1!=(_T('\0')))
		strArray.Add(p1);
	return TRUE;
}

double CCalStructDlg::GetFiOfLamda(double dLamda, LPCTSTR lpszMaterial)
{
	CString strSQL;
	try
	{
		double lret=0.0,hret=-1.0;
		double lla=0.0,hla=-1.0;
		double ret;
		COleVariant vTemp;
		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(Recordset));
//		rs.m_pDatabase=&EDIBgbl::dbSACal;//20071103 "dbSORT" 改为 "dbSACal"

		strSQL.Format((_T("SELECT [Lamda],[Fi] FROM [SteadyDecreaseCoef] WHERE [Material]=\'%s\' ORDER BY [Lamda] ")),lpszMaterial);
		rs->Open((_bstr_t)strSQL,_variant_t((IDispatch*)EDIBgbl::dbSACal,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		if(rs->adoEOF && rs->BOF)
			;//no this material
		else
		{
			rs->get_Collect((_variant_t)0L,vTemp);
			lla=vtod(vTemp);
			rs->get_Collect((_variant_t)1L,vTemp);
			lret=vtod(vTemp);
			if(dLamda<lla)
			{
				//细长比<最小值
				ret=lret;
			}
			else
			{
				//细长比>=最小值
				VARIANT vTmp;
				if(!rs->Find((_bstr_t)("Lamda>"+ftos(dLamda)), 0, adSearchForward, vTmp))
				{
					//细长比>最大值
					rs->MoveLast();
					rs->get_Collect((_variant_t)0L,vTemp);
					hla=vtod(vTemp);
					rs->get_Collect((_variant_t)1L,vTemp);
					hret=vtod(vTemp);
					ret=hret;
				}
				else
				{
					//细长比>=最小值 and 细长比<=最大值
					rs->get_Collect((_variant_t)0L,vTemp);
					hla=vtod(vTemp);
					rs->get_Collect((_variant_t)1L,vTemp);
					hret=vtod(vTemp);

					rs->MovePrevious();
					if(rs->BOF && rs->adoEOF)
						ret=hret;
					else
					{
						rs->get_Collect((_variant_t)0L,vTemp);
						lla=vtod(vTemp);
						rs->get_Collect((_variant_t)1L,vTemp);
						lret=vtod(vTemp);
						ret= (dLamda-lla)/(hla-lla) * (hret-lret) + lret;
					}
				}
			}
		}
		rs->Close();
		return ret;
	}
	catch(_com_error * e)
	{
	}
	return 0;
}

CString CCalStructDlg::GetOpt(LPCTSTR lpszOpt)
{
	if(_tcscmp(lpszOpt,(_T(">=")))==0)
		return GetResStr(IDS_DY_DY);
	if(_tcscmp(lpszOpt,(_T("<=")))==0)
		return GetResStr(IDS_XY_DY);
	if(_tcscmp(lpszOpt,(_T(">")))==0)
		return GetResStr(IDS_DAY);
	if(_tcscmp(lpszOpt,(_T("<")))==0)
		return GetResStr(IDS_XIAY);
	if(_tcscmp(lpszOpt,(_T("=")))==0)
		return GetResStr(IDS_DNYU);
	return lpszOpt;
}

void CCalStructDlg::OnBeforeUpdateDatagridMainDim(short *Cancel)
{
	m_gridMainDim.GetColumns().GetItem(_variant_t(0L)).SetValue(_variant_t(m_lStructID));
	m_gridMainDim.GetColumns().GetItem(_variant_t(4L)).SetValue(_variant_t(true));
	
}

void CCalStructDlg::OnAfterColUpdateDatagridMaindim(short ColIndex) 
{
	// TODO: Add your control notification handler code here
	if(ColIndex==2)
	{
		if(!this->m_rsMainDim->adoEOF && !this->m_rsMainDim->BOF)
		{
			CWaitCursor wait;
			CString strVarSQL;
			try
			{
				m_gridVar.SetRefDataSource(NULL);
				strVarSQL=(LPCTSTR)(_bstr_t)m_rsVariable->GetSource();
				m_rsVariable=NULL;
			}
			catch(_com_error & e)
			{
				AfxMessageBox(e.Description());
			}
			CalComLen(this->m_rsMainDim);
			UpdateComLen();
			try
			{
				m_rsVariable.CreateInstance(__uuidof(Recordset));
				m_rsVariable->CursorLocation=adUseClient;
				m_rsVariable->Open(_variant_t((LPCTSTR)strVarSQL),(IDispatch*)m_connSort,adOpenStatic,adLockOptimistic,adCmdText);
				m_gridVar.SetRefDataSource(m_rsVariable->GetDataSource());
				m_gridVar.GetColumns().GetItem(_variant_t(0L)).SetVisible(FALSE);
				m_gridVar.GetColumns().GetItem(_variant_t(4L)).SetVisible(FALSE);
				m_gridVar.GetColumns().GetItem(_variant_t(1L)).SetCaption(GetResStr(IDS_VARIABLE_NAME));
				m_gridVar.GetColumns().GetItem(_variant_t(2L)).SetCaption(GetResStr(IDS_VARIABLE_VALUE));
				m_gridVar.GetColumns().GetItem(_variant_t(3L)).SetCaption(GetResStr(IDS_DESC));
				m_gridVar.GetColumns().GetItem(_variant_t(5L)).SetCaption(GetResStr(IDS_VARNAME_IN_IMAGE));
				m_gridVar.GetColumns().GetItem(_variant_t(1L)).SetWidth(60.0f);
				m_gridVar.GetColumns().GetItem(_variant_t(2L)).SetWidth(60.0f);
				m_gridVar.GetColumns().GetItem(_variant_t(3L)).SetWidth(240.0f);
				m_gridVar.GetColumns().GetItem(_variant_t(5L)).SetWidth(90.0f);
			}
			catch(_com_error & e)
			{
				ReportError(e.Description(),__FILE__,__LINE__);
			}
		}
	}
}

void CCalStructDlg::CalComLen(_RecordsetPtr rstheVar)
{
	CString strSQL;
	try
	{
		strSQL=_T("DROP TABLE [tmpCalComponentLength]");
		try
		{
			EDIBgbl::dbPRJ->Execute((_bstr_t)strSQL, NULL, adCmdText);
		}
		catch(CException *e)
		{
		}
		_RecordsetPtr rsVar;
		_variant_t vTemp;
		if(rstheVar==NULL)
		{
			rsVar.CreateInstance(__uuidof(Recordset));
			strSQL.Format((_T("SELECT [VarName],[VarValue] FROM [SAStructVariable] WHERE [StructID]=%d")),m_lStructID);
			rsVar->Open((LPCTSTR)strSQL,(IDispatch*)this->m_connSort,adOpenForwardOnly,adLockReadOnly,adCmdText);
		}
		else
		{
			rsVar=rstheVar->Clone(adLockReadOnly);
			if(!(rsVar->BOF && rsVar->adoEOF))
				rsVar->MoveFirst();
		}

		strSQL=_T("CREATE TABLE [tmpCalComponentLength] (");
		CString strTemp,strTemp2,strTemp3,strFields,strValues;
		int i=0;
		CString strVarName;
		double dVarValue;
		for(;!rsVar->adoEOF;rsVar->MoveNext(),i++)
		{
			vTemp=rsVar->GetCollect(L"VarName");
			strVarName=vtos(vTemp);
			vTemp=rsVar->GetCollect(L"VarValue");
			dVarValue=vtod(vTemp);
			if(i==0)
			{
				strTemp.Format((_T(" [%s] Double")),strVarName);
				strTemp2.Format((_T(" [%s] ")),strVarName);
				strTemp3.Format(_T(" %f "),dVarValue);
			}
			else
			{
				strTemp.Format((_T(", [%s] Double")),strVarName);
				strTemp2.Format((_T(" , [%s] ")),strVarName);
				strTemp3.Format(_T(" , %f "),dVarValue);
			}
			strSQL+=strTemp;
			strFields+=strTemp2;
			strValues+=strTemp3;
		}
		rsVar->Close();
		strSQL+=_T(")");
		
		EDIBgbl::dbPRJ->Execute((_bstr_t)strSQL, NULL, adCmdText);

		strSQL.Format((_T("INSERT INTO [tmpCalComponentLength] ( %s ) VALUES (%s)")),strFields,strValues);
		
		EDIBgbl::dbPRJ->Execute((_bstr_t)strSQL, NULL, adCmdText);
		strSQL.Format(_T("SELECT [Lreal_formula],[Number] FROM [SAStructComponent] WHERE [StructID]=%d ORDER BY [Number]"),
			this->m_lStructID);
		_RecordsetPtr rsLiFml;
		rsLiFml=this->m_connSASCal->Execute(_bstr_t(strSQL),NULL,adCmdText);

		CString strLiFml;
//		rsTmp.m_pDatabase = &EDIBgbl::dbPRJ;
		_RecordsetPtr rsTmp;
		HRESULT hr = S_OK;
		hr = rsTmp.CreateInstance(__uuidof(Recordset));
		
		for(;!rsLiFml->adoEOF;rsLiFml->MoveNext())
		{
			strLiFml=vtos(rsLiFml->GetCollect(0L));
			if( !strLiFml.IsEmpty() )
			{
				//根据l1/l2/l3的计算公式，利用JET计算其值
				strSQL.Format("SELECT (%s) As  Li FROM [tmpCalComponentLength]",strLiFml);
				rsTmp->Open((_bstr_t)strSQL,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
					adOpenDynamic, adLockReadOnly, adCmdText); 
				if( rsTmp->adoEOF )
					;
				else
				{
					COleVariant varTmp;
					rsTmp->get_Collect((_variant_t)0L,varTmp);
					double dLi=vtod(varTmp);
					strSQL.Format("UPDATE [SAStructVariable] SET [VarValue]=%f WHERE StructID=%d AND VarName=\'l%d\'",dLi,m_lStructID,vtoi(rsLiFml->GetCollect(1L)));
					this->m_connSASCal->Execute((LPCTSTR)strSQL,NULL,adCmdText);//20071103 "m_connSort" 改为 "m_connSASCal"
				}
				rsTmp->Close();
			}
		}
		rsLiFml->Close();
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.Description());
	}
}

void CCalStructDlg::UpdateComLen()
{
	try
	{
		int c=m_ComGrid.GetRowCount()-1;
		int i;
		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(Recordset));
		for(i=1;i<=c;i++)
		{
			CString strSQL;
			strSQL.Format(_T("SELECT [VarValue] FROM [SAStructVariable] WHERE [StructID]=%d AND [VarName]=\'l%d\'"),this->m_lStructID,i);
			double l;
			rs=this->m_connSASCal->Execute((LPCTSTR)strSQL,NULL,adCmdText);//20071103 "m_connSort" 改为 "m_connSASCal"
			CString strTemp;
			if(!rs->adoEOF)
			{
				l=vtod(rs->GetCollect(0L));
				strTemp.Format(_T("%g"),l);
			}
			m_ComGrid.SetItemText(i,3,strTemp);
		}
		m_ComGrid.Invalidate();
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.Description());
	}
}

void CCalStructDlg::OnBtnDefaultformula() 
{
	//恢复默认的公式
	try
	{
		_RecordsetPtr rsDefaultFormula(__uuidof(Recordset));
		
		CWnd* pActiveWnd = m_tabCal.GetActiveWnd();
		CString strSQL;
		strSQL.Format(_T("SELECT [SAStructFormula].[DefaultFormula],[SAStructFormula].[DefaultCaption],[SAStructFormula].[DefaultWhere] FROM [SAStructFormula] WHERE [SAStructFormula].[ComponentID] = %d AND [SAStructFormula].[Result]=\'%s\'"),
			((CCalStructPag*)pActiveWnd)->m_lComponentID,((CCalStructPag*)pActiveWnd)->m_strResult);
		
		rsDefaultFormula = m_connSASCal->Execute(_bstr_t(strSQL),NULL,adCmdText);//20071103 "m_connSort" 改为 "m_connSASCal"

		((CCalStructPag*)pActiveWnd)->m_strFormula = vtos(rsDefaultFormula->GetCollect("DefaultFormula"));
		((CCalStructPag*)pActiveWnd)->m_strCaption = vtos(rsDefaultFormula->GetCollect("DefaultCaption"));
		((CCalStructPag*)pActiveWnd)->m_strWhere = vtos(rsDefaultFormula->GetCollect("DefaultWhere"));
	
		((CCalStructPag*)pActiveWnd)->UpdateData(FALSE);
	}
	catch(_com_error &e)
	{
		ReportError(e.Description(),__FILE__,__LINE__);
	}
}
