// FrmSelSpecification.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "FrmSelSpecification.h"
#include "modPHScal.h"
#include "ModEncrypt.h"
#include "ModRegistry.h"
#include "basDirectory.h"
#include "user.h"
#include "EDIBgbl.h"
#include "mainfrm.h"
#include "DlgDBOper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  

//_declspec(dllexport) extern CDlgDBOper dlgDBOper;

#define LIGHT_COUNT 5
/////////////////////////////////////////////////////////////////////////////
// CFrmSelSpecification dialog

//CFrmSelSpecification FrmSelSpecification;
CFrmSelSpecification::CFrmSelSpecification(BOOL bShowExitMenuItem,CWnd* pParent /*=NULL*/)
	: CDialog(CFrmSelSpecification::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFrmSelSpecification)
	m_Label2 = _T("");
	m_iSelIndex = 0;
	//}}AFX_DATA_INIT
	m_bSPECchanged=false;
	m_bShowExitMenuItem=bShowExitMenuItem;
}


void CFrmSelSpecification::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFrmSelSpecification)
	DDX_Control(pDX, IDC_LIST2, m_List2);
	DDX_Control(pDX, IDC_LIST1, m_List1);
	DDX_Control(pDX, IDC_LAB2, m_CLab2);
	DDX_Text(pDX, IDC_LAB2, m_Label2);
	DDX_Radio(pDX, IDC_RADIO1, m_iSelIndex);
	//}}AFX_DATA_MAP
	
}


BEGIN_MESSAGE_MAP(CFrmSelSpecification, CDialog)
	//{{AFX_MSG_MAP(CFrmSelSpecification)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_RADIO3, OnRadio3)
	ON_BN_CLICKED(IDC_RADIO4, OnRadio4)
	ON_BN_CLICKED(IDC_RADIO5, OnRadio5)
	ON_BN_CLICKED(IDC_RADIO6, OnRadio6)
	ON_LBN_SELCHANGE(IDC_LIST2, OnSelchangeList2)
	ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeList1)
	ON_WM_TIMER()
	ON_COMMAND(ID_IMPSPEC, OnImpSpec)
	ON_COMMAND(ID_SELEXIT, OnSelexit)
	ON_WM_CONTEXTMENU()
	ON_BN_CLICKED(ID_DROPRECORD, OnDroprecord)
	ON_WM_CANCELMODE()
	ON_COMMAND(IDC_OPENPHSTABLE, OnOpenphstable)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFrmSelSpecification message handlers

BOOL CFrmSelSpecification::OnInitDialog() 
{
	CDialog::OnInitDialog();
	::SetWindowCenter(m_hWnd);
	CRect rc;
	GetWindowRect(&rc);
   m_Label2=GetResStr(IDS_PressRightButtonOnMouseBeginImputProductDotMdb);
   SetTimer(100,3000,NULL);
   modPHScal::GetRecentWorkSPEC();
     
   m_List2.AddString(modPHScal::gsPhsPASel);
   m_List2.AddString(modPHScal::gsPhsPARTSel);
   m_List2.AddString(modPHScal::gsPhsSASel);
   m_List2.AddString(modPHScal::gsPhsSPRINGSel);
   m_List2.AddString(modPHScal::gsPhsConstantSpringSel);
   m_List2.AddString(modPHScal::gsPhsBoltsNutsSel);
   OnRadio1();
	m_mnu.LoadMenu(MEN_SELSPE);
	if(!m_bShowExitMenuItem)
		m_mnu.DeleteMenu(ID_SELEXIT,MF_BYCOMMAND);
	UpdateData(FALSE);
	for(int i=0;i<6;i++)
	{
		m_List2.GetText(i,m_aSpec[i]);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CFrmSelSpecification::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	
	// TODO: Return a different brush if the default is not desired
	if(pWnd!=NULL && pWnd->m_hWnd==m_CLab2.m_hWnd)
	{
		pDC->SetTextColor(0xff0000);
	}
	CWnd* pwnd=this->GetDlgItem(IDC_RADIO1+m_iSelIndex);
	if(pwnd && pwnd->m_hWnd==pWnd->m_hWnd)
		pDC->SetTextColor(0x0000ff);
	return hbr;
}

void CFrmSelSpecification::Option1_Click(int Index)
{
	_ConnectionPtr db;
	db.CreateInstance(__uuidof(_Connection));
	_RecordsetPtr rs1;
	rs1.CreateInstance(__uuidof(_Recordset));
	_RecordsetPtr rs;
	rs.CreateInstance(__uuidof(_Recordset));
	CString strSQL;
	CString sTmp;
	COleVariant v,v1;
	try
	{
		long i,ix;
// 		db.Open(basDirectory::ProjectDBDir+_T("zdjcrude.mdb"),false,false,_T(";pwd=") + ModEncrypt::gstrDBZdjCrudePassWord);
		db->Open(_bstr_t(basDirectory::ProjectDBDir+_T("zdjcrude.mdb")),_T(""), (_bstr_t)ModEncrypt::gstrDBZdjCrudePassWord,adConnectUnspecified);
		m_iSelIndex = Index;
		m_List2.SetCurSel(m_iSelIndex);
		for(i=0;i<6;i++)
		{
			CWnd* pwnd=this->GetDlgItem(IDC_RADIO1+i);
			if(pwnd)
				pwnd->Invalidate();
		}
		m_List1.ResetContent();
		//下面要判断是否所有的原始数据表都存在，不存在的要排除，不能写到可用列表框，防止用户误点击。
// 		rs1.m_pDatabase=&EDIBgbl::dbSORT;
		strSQL.Format(_T("SELECT * FROM PhsManu WHERE seq=%d"),Index);
// 		rs1.Open(dbOpenSnapshot,strSQL);
		rs1->MoveFirst();
		rs1->get_Collect((_variant_t)_T("SQL"),&v);
// 		rs.m_pDatabase=&EDIBgbl::dbSORT;
// 		rs.Open(dbOpenSnapshot,vtos(v));
		rs->Open((_bstr_t)v.bstrVal, _variant_t((IDispatch*)EDIBgbl::dbSORT,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		bool bFound=false;
		while(!rs->adoEOF)
		{
			//假设全部表都存在
			bFound = true;
			rs1->get_Collect((_variant_t)_T("CrudeDataTableNum"),v);
			for(i=1;i<=vtoi(v);i++)
			{
				rs->get_Collect((_variant_t)i, &v);
				if(vtos(v) != _T("") )
				{
					if(!EDIBgbl::tdfExists(db,vtos(v)))
					{
						bFound = false;
						//删除这个记录，防止它被写入到数据表，导致用户误点击。
						rs1->get_Collect((_variant_t)_T("TableName"),&v);
						rs->get_Collect((_variant_t)_T("Standard"),&v1);
						strSQL.Format(_T("DELETE  FROM [%s] WHERE trim(standard)=\'%s\'"),vtos(v),vtos(v1));
						EDIBgbl::dbSORT->Execute((_bstr_t)strSQL, NULL, adCmdText);
						break;
					 }
				}
			}
			if( bFound )
			{
				rs->get_Collect((_variant_t)_T("standard"),&v);
				ix=m_List1.AddString( vtos(v));
				m_List1.SetVirtual(ix);
			}
			rs->MoveNext();
		}
		rs->Close();
		rs1->get_Collect((_variant_t)_T("TableName"),v);
		m_tableName=vtos(v);//pfg20050223
		strSQL.Format(_T("SELECT * FROM [%s]"),
			vtos(v));
//		rs.Open(dbOpenDynaset,strSQL);
		rs->Open((_bstr_t)strSQL, _variant_t((IDispatch*)EDIBgbl::dbSORT,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		int iRecCount = rs->RecordCount;
		for(i=0;i<LIGHT_COUNT && !rs->adoEOF;i++,rs->MoveNext())
		{
			rs->get_Collect((_variant_t)_T("standard"),&v);
			ix=m_List1.FindStringExact(-1,vtos(v));
			if(ix!=-1)
				//m_List1.SetVirtual(ix,FALSE);
				m_List1.SetVirtual(ix,FALSE);
		}
		rs->Close();
		rs1->Close();
		db->Close();
	}
	catch(CException *e)
	{
		e->ReportError();
		e->Delete();
	}
}

void CFrmSelSpecification::OnRadio1() 
{
	// TODO: Add your control notification handler code here
	Option1_Click(0);
}

void CFrmSelSpecification::OnRadio2() 
{
	// TODO: Add your control notification handler code here
	Option1_Click(1);
	
}

void CFrmSelSpecification::OnRadio3() 
{
	// TODO: Add your control notification handler code here
	Option1_Click(2);
}

void CFrmSelSpecification::OnRadio4() 
{
	// TODO: Add your control notification handler code here
	Option1_Click(3);
}

void CFrmSelSpecification::OnRadio5() 
{
	// TODO: Add your control notification handler code here
	Option1_Click(4);
}

void CFrmSelSpecification::OnRadio6() 
{
	// TODO: Add your control notification handler code here
	Option1_Click(5);
}

void CFrmSelSpecification::OnSelchangeList2() 
{
	// TODO: Add your control notification handler code here

	m_iSelIndex=m_List2.GetCurSel();
	this->CheckRadioButton(IDC_RADIO1,IDC_RADIO6,IDC_RADIO1+m_iSelIndex);
	for(int i=0;i<6;i++)
	{
		CWnd* pwnd=this->GetDlgItem(IDC_RADIO1+i);
		if(pwnd)
			pwnd->Invalidate();
	}
	Option1_Click(m_iSelIndex);
}

void CFrmSelSpecification::OnSelchangeList1() 
{
	// TODO: Add your control notification handler code here
	CString strText;
	int ix=m_List1.GetCurSel();
	if(ix!=-1)
	{
		m_List1.GetText(ix,strText);
		m_iSelIndex=m_List2.GetCurSel();
		m_List2.DeleteString(m_iSelIndex);
		m_List2.InsertString(m_iSelIndex,strText);
		m_List2.SetCurSel(m_iSelIndex);
	}
}

void CFrmSelSpecification::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	static UINT bv=SW_HIDE;
	
	if(nIDEvent==100 && IsWindow(m_CLab2.m_hWnd))
		m_CLab2.ShowWindow(bv);
	bv=(bv==SW_HIDE ? bv=SW_SHOW : bv=SW_HIDE);
	CDialog::OnTimer(nIDEvent);
}



int inline ShowModalSelSpecification(BOOL bShowExitMenuItem)
{
	CFrmSelSpecification frm(bShowExitMenuItem);
	return frm.DoModal();
}


void CFrmSelSpecification::OnImpSpec() 
{
	// TODO: Add your command handler code here
	//防止窗体遮住文件选择对话框
	CString strFN =_T("");
	//strFN = SelFile(gObjMainFrm!CommonDialog1, _T("选择产品数据库"), _T("数据库 (*.mdb)|*.mdb|所有文件 (*.*)|*.*"), 1, TemplateDir)
	strFN = user::SelFile(GetResStr(IDS_SelectProductMdb), GetResStr(IDS_DatabaseMdbAllFiles), 1,modPHScal::gsSelectProductMdb);
	modPHScal::gsSelectProductMdb=GetFilePath(strFN);
	SetRegValue(_T("Settings"),_T("SelectProductMdb"),modPHScal::gsSelectProductMdb);
	if(strFN == _T("") || !FileExists(strFN)) return;
	modPHScal::ImportDataFromZdjCrudeXXXX( strFN, true, m_bSPECchanged);
   //重新显示
   
   Option1_Click(m_iSelIndex);
   //SetWindowTop Me.hwnd
	
}


void CFrmSelSpecification::OnSelexit() 
{
	// TODO: Add your command handler code here
	//设置下面开关为真，不检查规范表是否存在，导致容易错误。
	//m_bExit=true;
	EndDialog(ID_SELEXIT);
}



void CFrmSelSpecification::OnOK() 
{
	// TODO: Add extra validation here
	m_List2.GetText(0,modPHScal::gsPhsPASel);//optIPA
 	m_List2.GetText(1,modPHScal::gsPhsPARTSel);//optIPART
 	m_List2.GetText(2,modPHScal::gsPhsSASel);//optISA
 	m_List2.GetText(3,modPHScal::gsPhsSPRINGSel);//optISPRING
 	m_List2.GetText(4,modPHScal::gsPhsConstantSpringSel);//optICSPRING
 	m_List2.GetText(5,modPHScal::gsPhsBoltsNutsSel);//optIBoltsNuts
	this->ShowWindow(SW_HIDE);
 	if(! modPHScal::blnSelphsSPEC(m_bSPECchanged))
 	{
 		MessageBox(GetResStr(IDS_PleaseReselectSpecification));
		this->ShowWindow(SW_SHOW);
 		return ;
 	}
	AfxGetApp()->m_pMainWnd->ShowWindow(SW_HIDE);	
	AfxGetApp()->m_pMainWnd->ShowWindow(SW_RESTORE);
	modPHScal::SaveRecentWorkSPEC();
	SetUseCount();
	CDialog::OnOK();
}

void CFrmSelSpecification::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
	//CPoint p=point;
	//ClientToScreen(&p);
	m_mnu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x,point.y,this);
	//TrackPopupMenuEx(m_mnu.GetSubMenu(0)->m_hMenu,TPM_HORIZONTAL|TPM_VERTICAL,p.x,p.y,m_hWnd,NULL);
	//m_mnu.TrackPopupMenu(TPM_RIGHTALIGN|TPM_LEFTBUTTON,p.x,p.y,this);
}

void CFrmSelSpecification::SetUseCount()
{
	try
	{
//		rs1(&EDIBgbl::dbSORT),rs2(&EDIBgbl::dbSORT);
		_RecordsetPtr rs1;
		rs1.CreateInstance(__uuidof(_Recordset));
		_RecordsetPtr rs2;
		rs2.CreateInstance(__uuidof(_Recordset));
		CString strSQL;
		int i;
		COleVariant v;
		for(i=0;i<6;i++)
		{
			CString strTemp;
			m_List2.GetText(i,strTemp);
			strSQL.Format(_T("SELECT * FROM PhsManu WHERE SEQ=%d"),i);
// 			rs1.Open(dbOpenSnapshot,strSQL);
			rs1->Open((_bstr_t)strSQL, _variant_t((IDispatch*)EDIBgbl::dbSORT,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 
			rs1->get_Collect((_variant_t)_T("TableName"),&v);
			rs1->Close();
			strSQL.Format(_T("SELECT * FROM [%s] WHERE Standard=\'%s\'"),
				vtos(v),strTemp);
// 			rs2.Open(dbOpenDynaset,strSQL);
			rs2->Open((_bstr_t)strSQL, _variant_t((IDispatch*)EDIBgbl::dbSORT,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 
			rs2->get_Collect((_variant_t)_T("UseCount"),&v);
			LONG UseCount=vtoi(v);
			UseCount++;
//			rs2.Edit();
			if(UseCount==1 || m_aSpec[i]!=strTemp)
				rs2->put_Collect((_variant_t)_T("UseCount"),(_variant_t)UseCount);
			rs2->Update();
			rs2->Close();
		}
	}
	catch(CException *e)
	{
		e->ReportError();
		e->Delete();
	}
}

//删除选择产品规范：//pfg20050222
void CFrmSelSpecification::OnDroprecord() 

{	// TODO: Add your control notification handler code here
	int iCurSel;
	int icount=0;
	CString strSQL;
	CString str;
	icount=m_List1.GetCount();
	if(icount<=0)
	{
		AfxMessageBox("没有产品规范!");
		return;
	}
	iCurSel=m_List1.GetCurSel();
	if(iCurSel==LB_ERR)
	{
		iCurSel=0;
	}
	m_List1.GetText(iCurSel,str);
	if(!modPHScal::gsPhsPASel.Compare(str)||!modPHScal::gsPhsPARTSel.Compare(str)||!modPHScal::gsPhsSASel.Compare(str))
	{
		AfxMessageBox("该标准正在使用,不能删除,请重新选择!");
		return;
	}
	if(!modPHScal::gsPhsSPRINGSel.Compare(str)||!modPHScal::gsPhsConstantSpringSel.Compare(str)||!modPHScal::gsPhsBoltsNutsSel.Compare(str))
	{
		AfxMessageBox("该标准正在使用,不能删除,请重新选择!");
		return;
	}
	
	strSQL="delete from "+m_tableName+" where standard='"+str+"'";
	try
	{
		EDIBgbl::dbSORT->Execute((_bstr_t)strSQL, NULL, adCmdText);
	}
	catch(_com_error e)
	{
		AfxMessageBox("删除失败!");
//		db->Close();
		return;
	}
	m_List1.DeleteString(iCurSel);
	icount=m_List1.GetCount();
	if(iCurSel>=icount)
	{
		iCurSel=iCurSel-1;
	}
	m_List1.SetCurSel(iCurSel);
	m_List1.GetText(iCurSel,str);
	int m_iSelIndex=0;
	m_iSelIndex=m_List2.GetCurSel();
	m_List2.DeleteString(m_iSelIndex);
	m_List2.InsertString(m_iSelIndex,str);
	m_List2.SetCurSel(m_iSelIndex);
}

void CFrmSelSpecification::OnOpenphstable() 
{
	//LFX 2005.3.3加  用以打开当前的标准或规范
	// TODO: Add your command handler code here
	CString strType;
	CString strStandard;
	//if (::IsWindow(dlgDBOper.m_hWnd))
	//{
		//dlgDBOper.CloseDlg();
		//dlgDBOper.OnClose();
	//}
	CDlgDBOper dlgDBOper;
	dlgDBOper.m_strDBName = basDirectory::ProjectDBDir+_T("Zdjcrude.mdb");
	//dlgDBOper.m_pCallWnd = this;
	//dlgDBOper.ShowWindow(SW_SHOWNORMAL);
	CWnd* pWnd=this->GetDlgItem(IDC_RADIO1+m_iSelIndex);
	pWnd->GetWindowText(strType);
	dlgDBOper.m_strTableType = strType;
	m_List2.GetText(m_List2.GetCurSel(),strStandard);
	dlgDBOper.m_strTableStandard = strStandard;
	dlgDBOper.m_bAutoOpen = true;
	dlgDBOper.DoModal();

	
}
