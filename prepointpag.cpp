// PrePointPag.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "PrePointPag.h"
#include "modPHScal.h"
#include "user.h"
#include "modRegistry.h"
#include "PreDrawPag.h"
#include "EDIBgbl.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrePointPag property page

IMPLEMENT_DYNCREATE(CPrePointPag, CPropertyPage)

CPrePointPag::CPrePointPag() : CPropertyPage(CPrePointPag::IDD)
{
	//{{AFX_DATA_INIT(CPrePointPag)
	m_bCheckAutoTZ = FALSE;
	//}}AFX_DATA_INIT
	m_posFld[0].FldName="Vx_pt0x";
	m_posFld[1].FldName="VZ_pt0x";
	m_posFld[2].FldName="VX_pt0y";
	m_posFld[3].FldName="Pt1x";
	m_posFld[4].FldName="Pt1y";
	m_posFld[5].FldName="Pt2x";
	m_posFld[6].FldName="Pt2y";
	m_posFld[7].FldName="Pt3x";
	m_posFld[8].FldName="Pt3y";
	/*m_posFld[0].nID=IDC_POS1;
	m_posFld[1].nID=IDC_POS2;
	m_posFld[2].nID=IDC_POS3;
	m_posFld[3].nID=IDC_POS4;
	m_posFld[4].nID=IDC_POS5;
	m_posFld[5].nID=IDC_POS6;
	m_posFld[6].nID=IDC_POS7;
	m_posFld[7].nID=IDC_POS8;
	m_posFld[7].nID=IDC_POS9;*/
	for(int i=0;i<9;i++)
		m_posFld[i].FldValue=0;
	bSetDefault=false;
	bSetBPOK=false;
	bSetPrevious=false;
	bGetPos=true;
	iSetBPbegan=1;
	oldix=0;
	oldA3A4 = -1;
}

CPrePointPag::~CPrePointPag()
{
}

void CPrePointPag::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrePointPag)
	DDX_Control(pDX, IDC_POS9, m_TextPt3y);
	DDX_Control(pDX, IDC_POS8, m_TextPt3x);
	DDX_Control(pDX, IDC_POS7, m_TextPt2y);
	DDX_Control(pDX, IDC_POS6, m_TextPt2x);
	DDX_Control(pDX, IDC_POS5, m_TextPt1y);
	DDX_Control(pDX, IDC_POS4, m_TextPt1x);
	DDX_Control(pDX, IDC_POS3, m_TextVX_pt0y);
	DDX_Control(pDX, IDC_POS2, m_TextVZ_pt0x);
	DDX_Control(pDX, IDC_POS1, m_TextVX_pt0x);
	DDX_Check(pDX, IDC_AUTOADJ, m_bCheckAutoTZ);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_POS1, m_posFld[0].FldValue);
	DDX_Text(pDX, IDC_POS2, m_posFld[1].FldValue);
	DDX_Text(pDX, IDC_POS3, m_posFld[2].FldValue);
	DDX_Text(pDX, IDC_POS4, m_posFld[3].FldValue);
	DDX_Text(pDX, IDC_POS5, m_posFld[4].FldValue);
	DDX_Text(pDX, IDC_POS6, m_posFld[5].FldValue);
	DDX_Text(pDX, IDC_POS7, m_posFld[6].FldValue);
	DDX_Text(pDX, IDC_POS8, m_posFld[7].FldValue);
	DDX_Text(pDX, IDC_POS9, m_posFld[8].FldValue);
}


BEGIN_MESSAGE_MAP(CPrePointPag, CPropertyPage)
	//{{AFX_MSG_MAP(CPrePointPag)
	ON_BN_CLICKED(IDC_AUTOADJ, OnAutoadj)
	ON_BN_CLICKED(IDC_DEFAULT, OnDefault)
	ON_BN_CLICKED(IDC_PREVIOUS, OnPrevious)
	ON_BN_CLICKED(IDOK1, OnOk1)
	ON_BN_CLICKED(IDCANCEL1, OnCancel1)
	ON_BN_CLICKED(IDC_OPTIONZ1, OnOptionz1)
	ON_BN_CLICKED(IDC_OPTIONZ2, OnOptionz1)
	ON_BN_CLICKED(IDC_OPTIONZ3, OnOptionz1)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrePointPag message handlers

void CPrePointPag::SetCtrlValue()
{
	m_bCheckAutoTZ = modPHScal::AutoOrdinateDrawing;
//	m_bCheckBOM =modPHScal::gbDraw_BOM;
	UpdateData(false);
}

BOOL CPrePointPag::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	this->CheckRadioButton(IDC_OPTIONZ1,IDC_OPTIONZ3,IDC_OPTIONZ1);
	//UpdateData(false);
	SetCtrlValue();
	if(this->m_bCheckAutoTZ)
	{
		m_TextVX_pt0x.EnableWindow(true);
     m_TextVZ_pt0x.EnableWindow(true);
     m_TextVX_pt0y.EnableWindow(true);
	}
	bGetPos=true;
	this->SetBasePoint();
	bGetPos=false;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPrePointPag::SaveIniValue()
{
	UpdateData();
	modPHScal::AutoOrdinateDrawing = m_bCheckAutoTZ;
	bSetBPOK=true;
	this->SetBasePoint();
	bSetBPOK=false;
}

void CPrePointPag::OnAutoadj() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if(this->m_bCheckAutoTZ)
	{
		m_TextVX_pt0x.EnableWindow(true);
		m_TextVZ_pt0x.EnableWindow(true);
		m_TextVX_pt0y.EnableWindow(true);
	}
	else 
	{
		m_TextVX_pt0x.EnableWindow();
		m_TextVZ_pt0x.EnableWindow();
		m_TextVX_pt0y.EnableWindow();
	}
}

BOOL CPrePointPag::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	this->OnOptionz1();
	return CPropertyPage::OnSetActive();
}

void CPrePointPag::SetBasePoint()
{
/*	try
	{
		long bFirst;
		CPreDrawPag* pag=(CPreDrawPag*)(((CPropertySheet*)this->GetParent())->GetPage(1));
		pag->UpdateData();
		this->UpdateData();
		bFirst = 1;
		CDaoRecordset rs;
		rs.m_pDatabase=&EDIBgbl::dbSORT;
		if(pag->GetCheckedRadioButton(IDC_DRAWSIZE_A3,IDC_DRAWSIZE_AUTO)==IDC_DRAWSIZE_A3)
			modPHScal::Ax = "A3";
		else modPHScal::Ax = "A4";

		//modPHScal::Ax = CString("A") + IIf(OptionDrawSIZE_A3A4(0), "3", "4")
		int ix=this->GetCheckedRadioButton(IDC_OPTIONZ1,IDC_OPTIONZ3);
		if(ix!=0) ix-=IDC_OPTIONZ1;
		if(!bGetPos)
			ix=oldix;
		CString FDp;
		if(ix==0) FDp="1_";
		else 
		{
			if(ix==1) FDp="2_";
			else FDp="S_";
		}
		if(pag->m_bCheckDraw_BOM)
			FDp+="bom";
		CString SQLx = "SELECT * FROM phsDrawPos";
		rs.Open(dbOpenDynaset,SQLx);
		CString sTmp;
		FDp.MakeUpper();
		_variant_t vTmp;
		for(int i=0;i<9;i++)
		{
			sTmp=m_posFld[i].FldName;
			sTmp.MakeUpper();
			SQLx = "ucase(trim(name))=\'" + FDp + sTmp + "\'";
				//MsgBox SQLx
			if(rs.FindFirst(SQLx))
			{
					if(bGetPos)
					{
						rs.GetFieldValue(modPHScal::Ax+"pos",vTmp);
						if(vTmp.vt==VT_NULL)
							m_posFld[i].FldValue=10;
						else m_posFld[i].FldValue=vtoi(vTmp);
					}
					else if( bSetPrevious)
					{
						rs.Edit();
						rs.GetFieldValue(modPHScal::Ax+"old",vTmp);
						rs.SetFieldValue(modPHScal::Ax+"pos",vTmp);
						rs.Update();
						if(vTmp.vt==VT_NULL)
							m_posFld[i].FldValue=10;
						else m_posFld[i].FldValue=vtoi(vTmp);
					}
					else if(bSetDefault)
					{
						rs.Edit();
						rs.GetFieldValue(modPHScal::Ax+"Default",vTmp);
						rs.SetFieldValue(modPHScal::Ax+"pos",vTmp);
						rs.Update();
						if(vTmp.vt==VT_NULL)
							m_posFld[i].FldValue=10;
						else m_posFld[i].FldValue=vtoi(vTmp);
					}
					else if (bSetBPOK )
					{
						vTmp=_variant_t((long)m_posFld[i].FldValue);
						rs.Edit();
						rs.SetFieldValue(modPHScal::Ax+"pos",vTmp);
						rs.Update();
					}
					else
					{
						rs.GetFieldValue(modPHScal::Ax+"pos",vTmp);
						if(iSetBPbegan == 1 )
						{
							rs.Edit();
							rs.SetFieldValue(modPHScal::Ax+"old",vTmp);
							rs.Update();
						}
						if(vTmp.vt==VT_NULL)
							m_posFld[i].FldValue=10;
						else m_posFld[i].FldValue=vtoi(vTmp);
					}
			}
		}

		bSetDefault = false;
		bSetBPOK = false;
		bSetPrevious = false;
		iSetBPbegan = iSetBPbegan + 1;
	}
	catch(CDaoException *e)
	{
		//e->ReportError();
		char ss[256];
		e->GetErrorMessage(ss,255);
		MessageBox(ss);
		e->Delete();
	}

*/

	try
	{
		long bFirst;
		CPreDrawPag* pag=(CPreDrawPag*)(((CPropertySheet*)this->GetParent())->GetPage(1));
		pag->UpdateData();
		this->UpdateData();
		bFirst = 1;
		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(Recordset));

		if(pag->GetCheckedRadioButton(IDC_DRAWSIZE_A3,IDC_DRAWSIZE_AUTO)==IDC_DRAWSIZE_A3)
			modPHScal::Ax = "A3";
		else modPHScal::Ax = "A4";

		long curA3A4; //当前绘图选项的图幅尺寸
		if((curA3A4 = pag->GetCheckedRadioButton(IDC_DRAWSIZE_A3, IDC_DRAWSIZE_AUTO)) == IDC_DRAWSIZE_A3)
			modPHScal::Ax = "A3";
		else 
			modPHScal::Ax = "A4";
		if (curA3A4 != 0)	curA3A4 -= IDC_DRAWSIZE_A3;

		CString strCurAx;	
		if (oldA3A4 <= -1)
		{
			oldA3A4 = curA3A4;
			strCurAx = modPHScal::Ax;
		}
		else
		{
			if (oldA3A4 == 0)
				strCurAx = "A3";
			else
				strCurAx = "A4";
		}
		oldA3A4 = curA3A4;

		int ix=this->GetCheckedRadioButton(IDC_OPTIONZ1,IDC_OPTIONZ3);
		if(ix!=0) ix-=IDC_OPTIONZ1;
		if(!bGetPos)
			ix=oldix;
		CString FDp;
		if(ix==0) FDp="1_";
		else 
		{
			if(ix==1) FDp="2_";
			else FDp="S_";
		}
		if(pag->m_bCheckDraw_BOM)
			FDp+="bom";
		CString SQLx = "SELECT * FROM phsDrawPos";
		rs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPHScode,true), 
			adOpenDynamic, adLockOptimistic, adCmdText); 
		if (!rs->adoEOF)
		{
		CString sTmp;
		FDp.MakeUpper();
		_variant_t vTmp;
			HRESULT hr = S_OK;
			rs->MoveFirst();
		for(int i=0;i<9;i++)
		{
			sTmp=m_posFld[i].FldName;
			sTmp.MakeUpper();
			SQLx = "ucase(trim(name))=\'" + FDp + sTmp + "\'";
				hr = rs->Find((_bstr_t)(SQLx), 0, adSearchForward);
				if(!rs->adoEOF)
			{
					if(bGetPos)
					{
						rs->get_Collect((_variant_t)(strCurAx + "pos"), &vTmp);
						if(vTmp.vt==VT_NULL)
							m_posFld[i].FldValue=10;
						else m_posFld[i].FldValue=vtoi(vTmp);
					}
					else if( bSetPrevious)
					{
						rs->get_Collect((_variant_t)(strCurAx + "old"), &vTmp);
						rs->put_Collect((_variant_t)(strCurAx + "pos"), vTmp);
						rs->Update();
						if(vTmp.vt==VT_NULL)
							m_posFld[i].FldValue=10;
						else m_posFld[i].FldValue=vtoi(vTmp);
					}
					else if(bSetDefault)
					{
						rs->get_Collect((_variant_t)(strCurAx + "Default"), &vTmp);
						rs->put_Collect((_variant_t)(strCurAx + "pos"), vTmp);
						rs->Update();
						if(vTmp.vt==VT_NULL)
							m_posFld[i].FldValue=10;
						else m_posFld[i].FldValue=vtoi(vTmp);
					}
					else if (bSetBPOK )
					{
						vTmp=_variant_t((long)m_posFld[i].FldValue);
						rs->put_Collect((_variant_t)(strCurAx + "pos"), vTmp);
						rs->Update();
					}
					else
					{
						rs->get_Collect((_variant_t)(strCurAx + "pos"), &vTmp);
						if(iSetBPbegan == 1 )
						{
								rs->put_Collect((_variant_t)(strCurAx + "old"), vTmp);
								rs->Update();
						}
						if(vTmp.vt==VT_NULL)
							m_posFld[i].FldValue=10;
						else m_posFld[i].FldValue=vtoi(vTmp);
					}
			}
		}

		bSetDefault = false;
		bSetBPOK = false;
		bSetPrevious = false;
		iSetBPbegan = iSetBPbegan + 1;
	}
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}

	this->UpdateData(false);
}

void CPrePointPag::OnDefault() 
{
	DoDefaultSetting();
}

void CPrePointPag::OnPrevious() 
{
	// TODO: Add your control notification handler code here
	bSetPrevious=true;
	this->SetBasePoint();
	bSetPrevious=false;
}



void CPrePointPag::OnOk1() 
{
	// TODO: Add your control notification handler code here
	bSetBPOK=true;
	this->SetBasePoint();
	bSetBPOK=false;
	//modPHScal::SetBasePoint();
	//((CPropertySheet*)this->GetParent())->SetActivePage(1);
}

void CPrePointPag::OnCancel1() 
{
	// TODO: Add your control notification handler code here
	SetBasePoint();
	((CPropertySheet*)this->GetParent())->SetActivePage(1);
}

void CPrePointPag::OnOptionz1() 
{
	// TODO: Add your control notification handler code here
	OnOk1();
	bGetPos=true;
	this->SetBasePoint();
	bGetPos=false;
	oldix=this->GetCheckedRadioButton(IDC_OPTIONZ1,IDC_OPTIONZ3);
	if(oldix!=0) oldix-=IDC_OPTIONZ1;
}

void CPrePointPag::DoDefaultSetting()
{
	bSetDefault = true;
	SetBasePoint();
	bSetDefault=false;

	m_bCheckAutoTZ = TRUE;
	m_TextVX_pt0x.EnableWindow(true);
	m_TextVZ_pt0x.EnableWindow(true);
	m_TextVX_pt0y.EnableWindow(true);

	UpdateData(FALSE);

}

BOOL CPrePointPag::UpdatePoint()
{
	OnOptionz1();
	
	return TRUE;
}

void CPrePointPag::OnLButtonDown(UINT nFlags,CPoint point)
{//chengbailian 2003.12.12 当窗口很大时,在分辨率改变小时,窗口不能全部显示出来,需要移动窗体
	HWND h=::GetActiveWindow();//得到主窗口名柄
	::SendMessage(h,WM_NCLBUTTONDOWN, HTCAPTION, NULL);//发送移动消息
}
