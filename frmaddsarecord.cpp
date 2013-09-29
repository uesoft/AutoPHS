// FrmAddSArecord1.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "FrmAddSArecord.h"
#include "modPHScal.h"
#include "phsdata.h"
#include "basDirectory.h"
#include "FrmTxsr.h"
#include "user.h"
#include "modEncrypt.h"
#include "_recordset.h"
#include "EDIBDB.h"
#include "DlgSARecord.h"
#include "Fields.h"
#include "Field.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFrmAddSArecord dialog


CFrmAddSArecord::CFrmAddSArecord(CWnd* pParent /*=NULL*/)
	: CDialog(CFrmAddSArecord::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFrmAddSArecord)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

}


void CFrmAddSArecord::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFrmAddSArecord)
	DDX_Control(pDX, IDC_LIST1, m_List1);
	DDX_Control(pDX, IDC_DATAGRID1, m_DBGrid1);
	DDX_Control(pDX, IDC_ADODC1, m_Data1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFrmAddSArecord, CDialog)
	//{{AFX_MSG_MAP(CFrmAddSArecord)
	ON_WM_SIZE()
	ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeList1)
	ON_WM_SHOWWINDOW()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_EDIT, OnBtnEdit)
	ON_BN_CLICKED(IDC_BTN_ADD, OnBtnAdd)
	ON_BN_CLICKED(IDC_BTN_REMOVE, OnBtnRemove)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFrmAddSArecord message handlers

BOOL CFrmAddSArecord::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	try
	{
		CString SQL=_T("SELECT DISTINCT CustomID FROM [") + modPHScal::tbnSA + _T("] ORDER BY CustomID");
		CString ConStr=CString(dbConnectionString)+
			basDirectory::ProjectDBDir+
			_T("zdjcrude.mdb;Jet OLEDB:Database Password=")+ModEncrypt::gstrDBZdjCrudePassWord;
		CDaoRecordset rs(&modPHScal::dbZDJcrude);
		rs.Open(dbOpenSnapshot,SQL);
		rs.MoveFirst();
		COleVariant v;
		_variant_t vTmp;
		int i=0;
		CString sTmp,sTmp2;
		vTmp=FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("gn1"));
		sTmp2=vtos(vTmp);
		sTmp2.MakeUpper();
		while(!rs.IsEOF())
		{
			rs.GetFieldValue(_T("CustomID"),v);
			sTmp=vtos(v);
			sTmp.MakeUpper();
			m_List1.AddString(sTmp);
			if(sTmp==sTmp2)
				m_List1.SetCurSel(i);
			i++;
			rs.MoveNext();
		}
		rs.Close();
		if(m_List1.FindStringExact(-1,_T("G100"))==-1)
			m_List1.AddString(_T("G100"));
		m_DBGrid1.SetAllowDelete(FALSE);
		m_DBGrid1.SetAllowAddNew(FALSE);
		m_DBGrid1.SetAllowUpdate(FALSE);
		m_List1.SetCurSel(0);
		m_Data1.SetCursorLocation(2); //User Server Cursors
		m_Data1.SetCursorType(3);
		m_Data1.SetCommandType(1);
		m_Data1.SetConnectionString(ConStr);
		this->OnSelchangeList1();
		GetClientRect(&m_oldRect);
		RefWndRect();
		CRect rc;
		if(user::GetPos(_T("FrmAddSArecord"),rc))
			this->MoveWindow(&rc);
	}
	catch(_com_error& e)
	{
		ShowMessage(e.Description());
	}
	catch(::CDaoException * e)
	{
		e->ReportError();
		e->Delete();
	}
   /*Left = 0
   Width = Screen.Width
   CenterForm Me
   List1.Top = 0
   ScaleHeight = List1.Height
   DBGrid1.Height = List1.Height
   DBGrid1.Left = List1.Left + List1.Width
   DBGrid1.Width = ScaleWidth - List1.Width
   Height = Height - ScaleHeight + List1.Height
   Data1.Visible = False*/
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFrmAddSArecord::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	RefWndRect();

}

void CFrmAddSArecord::OnSelchangeList1() 
{
	// TODO: Add your control notification handler code here
	try
	{
		CString sTmp;
		int i=m_List1.GetCurSel();
		if(i!=-1)
		{
			m_List1.GetText(i,sTmp);
			m_strCustomID=sTmp;
			if(sTmp==_T("G100"))
			{
				GetDlgItem(IDC_BTN_REMOVE)->EnableWindow(TRUE);
				//GetDlgItem(IDC_BTN_EDIT)->EnableWindow(TRUE);
			}
			else
			{
				//GetDlgItem(IDC_BTN_EDIT)->EnableWindow(FALSE);
				GetDlgItem(IDC_BTN_REMOVE)->EnableWindow(FALSE);
			}
			m_DBGrid1.SetRefDataSource(NULL);
			CString SQL = _T("SELECT * FROM [") + modPHScal::tbnSA + _T("] WHERE trim(CustomID)=\'") + sTmp + _T("\' ORDER BY dh");
			m_Data1.SetRecordSource(SQL);
			m_Data1.Refresh();
			m_DBGrid1.SetRefDataSource(m_Data1.GetRecordset().GetDataSource());
			m_DBGrid1.Refresh();
			m_DBGrid1.ReBind();
			EDIBDB::RefreshGrid(m_DBGrid1,m_Data1.GetRecordset().m_lpDispatch);
			if(!m_Data1.GetRecordset().GetEof() || !m_Data1.GetRecordset().GetBof())
				m_Data1.GetRecordset().MoveLast();
		}
		else
		{
			m_strCustomID=_T("");
			GetDlgItem(IDC_BTN_REMOVE)->EnableWindow(FALSE);
			GetDlgItem(IDC_BTN_EDIT)->EnableWindow(FALSE);
		}
	}
	catch(_com_error& e)
	{
		ShowMessage(e.Description());
	}
}

void CFrmAddSArecord::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	/*if(bShow)
	{
		SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
	}
	else
		SetWindowPos(&wndNoTopMost,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
*/
}

void CFrmAddSArecord::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	user::SavePos(this,_T("FrmAddSArecord"));
	CDialog::OnClose();
}

void CFrmAddSArecord::RefWndRect()
{
	CRect rc1,rc2,rc3,rc4;
	if(IsWindow(m_List1.m_hWnd) && IsWindow(m_DBGrid1.m_hWnd))
	{
		m_List1.GetWindowRect(&rc1);
		this->ScreenToClient(&rc1);
		this->GetClientRect(&rc2);
		rc3.top=0;rc3.left=0;
		rc3.right=rc1.Width();
		rc3.bottom=rc2.Height();
		m_List1.MoveWindow(&rc3);

		m_DBGrid1.GetWindowRect(&rc1);
		this->ScreenToClient(&rc1);
		m_List1.GetWindowRect(&rc2);
		this->ScreenToClient(&rc2);
		this->GetClientRect(&rc3);
		rc4.top=0;rc4.left=rc2.right+1;
		rc4.right=rc3.right;
		rc4.bottom=rc3.Height()-30;
		m_DBGrid1.MoveWindow(&rc4);
		
		int cx=rc3.Width() - m_oldRect.Width();
		int cy=rc3.Height() - m_oldRect.Height();
		m_oldRect=rc3;

		GetDlgItem(IDOK)->GetWindowRect(&rc1);
		ScreenToClient(&rc1);
		rc1.left+=cx;rc1.right+=cx;
		rc1.top+=cy;rc1.bottom+=cy;
		GetDlgItem(IDOK)->MoveWindow(&rc1);

		GetDlgItem(IDC_BTN_ADD)->GetWindowRect(&rc1);
		ScreenToClient(&rc1);
		rc1.left+=cx;rc1.right+=cx;
		rc1.top+=cy;rc1.bottom+=cy;
		GetDlgItem(IDC_BTN_ADD)->MoveWindow(&rc1);

		GetDlgItem(IDC_BTN_EDIT)->GetWindowRect(&rc1);
		ScreenToClient(&rc1);
		rc1.left+=cx;rc1.right+=cx;
		rc1.top+=cy;rc1.bottom+=cy;
		GetDlgItem(IDC_BTN_EDIT)->MoveWindow(&rc1);

		GetDlgItem(IDC_BTN_REMOVE)->GetWindowRect(&rc1);
		ScreenToClient(&rc1);
		rc1.left+=cx;rc1.right+=cx;
		rc1.top+=cy;rc1.bottom+=cy;
		GetDlgItem(IDC_BTN_REMOVE)->MoveWindow(&rc1);
		
	}
}

void CFrmAddSArecord::OnBtnEdit() 
{
	
	// TODO: Add your control notification handler code here
	try
	{
		/*if(m_strCustomID!=_T("G100"))
			return;*/
		int nDH=1;
		if(!m_Data1.GetRecordset().GetEof() && !m_Data1.GetRecordset().GetBof())
		{
			m_strCustomID=vtos((_variant_t)m_Data1.GetRecordset().GetFields().GetItem(_variant_t(_T("CustomID"))).GetValue());
		
			CDlgSARecord dlg(this->m_Data1.GetRecordset().m_lpDispatch,FALSE,0,m_strCustomID);
			if(dlg.DoModal()==IDOK)
				this->OnSelchangeList1();
			//else
			//	AfxMessageBox(_T(_T("没有修改记录！")));
		}
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.Description());
	}
}

void CFrmAddSArecord::OnBtnAdd() 
{
	// TODO: Add your control notification handler code here
	try
	{
		int nDH=1;
		if(!m_Data1.GetRecordset().GetEof() || !m_Data1.GetRecordset().GetBof())
		{
			m_strCustomID=vtos((_variant_t)m_Data1.GetRecordset().GetFields().GetItem(_variant_t(_T("CustomID"))).GetValue());
			m_Data1.GetRecordset().MoveLast();
			nDH=vtoi((_variant_t)m_Data1.GetRecordset().GetFields().GetItem(_variant_t(_T("DH"))).GetValue());
			nDH++;
		}
		else
			m_List1.GetText(m_List1.GetCurSel(),m_strCustomID);
		CDlgSARecord dlg(this->m_Data1.GetRecordset().m_lpDispatch,TRUE,nDH,m_strCustomID);
		if(dlg.DoModal()!=IDOK)
			AfxMessageBox(	IDS_NOAPPENTNOTE);
		else
		{
			OnSelchangeList1();
		}
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.Description());
	}
}

void CFrmAddSArecord::OnBtnRemove() 
{
	// TODO: Add your control notification handler code here
	try
	{
		if(!m_Data1.GetRecordset().GetEof() && !m_Data1.GetRecordset().GetBof())
		{
			m_strCustomID=vtos((_variant_t)m_Data1.GetRecordset().GetFields().GetItem(_variant_t(_T("CustomID"))).GetValue());
			if(m_strCustomID!=_T("G100"))
				return;
			if(MessageBox(GetResStr(IDS_DEL_THIS_NOTE_YESNO),NULL,MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2)==IDYES)
			{
				m_Data1.GetRecordset().Delete(1);//adAffectCurrent
				//OnSelchangeList1();
			}
		}
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.Description());
	}
}

void CFrmAddSArecord::LoadList()
{
	
}
