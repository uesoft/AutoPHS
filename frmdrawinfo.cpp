// FrmDrawInfo.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "FrmDrawInfo.h"
#include "EDIBDB.h"
#include "EDIBAcad.h"
#include "EDIBgbl.h"
#include "basDirectory.h"
#include "user.h"
#include "modPHScal.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFrmDrawInfo dialog


CFrmDrawInfo::CFrmDrawInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CFrmDrawInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFrmDrawInfo)
	m_strPrjName = _T("");
	m_strDrawName = _T("");
	m_strDrawNo = _T("");
	m_strScal = _T("");
	//}}AFX_DATA_INIT
}


void CFrmDrawInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFrmDrawInfo)
	DDX_Text(pDX, IDC_PRJNAME, m_strPrjName);
	DDX_Text(pDX, IDC_DRAWNAME, m_strDrawName);
	DDX_Text(pDX, IDC_EDIT_DRAWNO, m_strDrawNo);
	DDX_Text(pDX, IDC_EDIT_SCALE, m_strScal);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFrmDrawInfo, CDialog)
	//{{AFX_MSG_MAP(CFrmDrawInfo)
	ON_EN_CHANGE(IDC_EDIT_SCALE, OnChangeEditScale)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFrmDrawInfo message handlers

BOOL CFrmDrawInfo::OnInitDialog() 
{
	CDialog::OnInitDialog();
	::SetWindowCenter(this->m_hWnd);
	// TODO: Add extra initialization here
	//if(EDIBDB::PrjName == "" || EDIBDB::DrawName == "" || EDIBDB::DrawID == "" || EDIBDB::DwgScale == 0)
   //   EDIBDB::DrawInfoGet();
   EDIBDB::GetTBsize();
 
   if(EDIBDB::PrjName == "")	EDIBDB::PrjName = GetResStr(IDS_XT_ELECTRICITY_BFACTORY);
   if(EDIBDB::DrawName == "")  EDIBDB::DrawName = GetResStr(IDS_THR_WING_PIPING_INSTALL_CHART);
   if(EDIBDB::DrawID == "" )	EDIBDB::DrawID = EDIBDB::DrawIDinitNo;
   if(EDIBDB::DwgScale == 0)	 EDIBDB::DwgScale = 100;
   m_strPrjName= EDIBDB::PrjName;
   m_strDrawName= EDIBDB::DrawName;
	//m_ctrDrawID.SetMask(EDIBDB::DrawIDMask);
	m_strDrawNo=EDIBDB::DrawID;
	//OnKeyPressDrawID(13);
	m_strScal.Format(_T("1:%4d"),EDIBDB::DwgScale);

	//m_ctrScale.SetText(ss);
	UpdateData(false);
   //MaskEdBoxDrawID_KeyPress 13
   //MaskEdBoxScale.Text = "1:" & Format(EDIBDB::DwgScale, "0000")
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_EVENTSINK_MAP(CFrmDrawInfo, CDialog)
    //{{AFX_EVENTSINK_MAP(CFrmDrawInfo)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()



void CFrmDrawInfo::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();
	int i=m_strScal.Find(_T(":"));
	EDIBDB::DwgScale=_ttoi(m_strScal.Mid(i+1));
   if(EDIBDB::DwgScale == 0)	 EDIBDB::DwgScale = 100;

	EDIBAcad::StartAcad();
	EDIBAcad::MakeNewLayer("TK");
	
	EDIBAcad::GetActiveAcadDoc();
	EDIBAcad::SetACADCurrentStyle("hz");
//    SJHY = "电力"
	EDIBDB::SJHY = GetResStr(IDS_MsgBox_60579);
	CCadPoint p0;
	EDIBAcad::AcAtt[0].Name = "PROJECT_NAME";
	EDIBAcad::AcAtt[0].Text = m_strPrjName;
	EDIBAcad::AcAtt[1].Name = "DRAWING_NAME";
	EDIBAcad::AcAtt[1].Text = m_strDrawName;
	EDIBAcad::AcAtt[1].iWidth=0;
	EDIBAcad::AcAtt[2].Name = "STAGE";
	EDIBAcad::AcAtt[2].Text = EDIBgbl::SelDsgnName;
	EDIBAcad::AcAtt[2].iWidth=0;
	EDIBAcad::AcAtt[3].Name = "DRAWING_NO";
	EDIBAcad::AcAtt[3].Text = m_strDrawNo;
	EDIBAcad::AcAtt[4].Name = "SCALE";
	EDIBAcad::AcAtt[4].Text = "/";
	EDIBAcad::AcAtt[4].iWidth=0;
	EDIBAcad::AcAtt[5].Name = "DATE";
	EDIBAcad::AcAtt[5].Text = CTime::GetCurrentTime().Format("%Y.%m.%d");
	EDIBAcad::AcAtt[6].Name = "JC_NAME";
	EDIBAcad::AcAtt[6].Text = EDIBgbl::SelVlmName;
	EDIBAcad::AcAtt[6].iWidth=0;
	//由于有些表头属性误写为SPECILITY,SPECIALITY
   EDIBAcad::AcAtt[7].Name = "SPECIALITY";
   EDIBAcad::AcAtt[7].Text = EDIBgbl::SelSpecName;
	EDIBAcad::AcAtt[7].iWidth=0;
   EDIBAcad::AcAtt[8].Name = "SPECILITY";
   EDIBAcad::AcAtt[8].Text = EDIBgbl::SelSpecName;
	EDIBAcad::AcAtt[8].iWidth=0;

	if(EDIBgbl::DrawNoSel<=1)
	{
		EDIBAcad::AcAtt[0].iWidth=EDIBAcad::iA0PrjNameWidth;
		EDIBAcad::AcAtt[3].iWidth=EDIBAcad::iA0DrawNoWidth;
		EDIBAcad::AcAtt[1].iWidth=EDIBAcad::iA0DrawNameWidth;
		EDIBAcad::AcAtt[6].iWidth=EDIBAcad::iA0VlmNameWidth;
		EDIBAcad::AcAtt[5].iWidth=EDIBAcad::iA0DateWidth;
	}
	else
	{
		EDIBAcad::AcAtt[0].iWidth=EDIBAcad::iA2PrjNameWidth;
		EDIBAcad::AcAtt[3].iWidth=EDIBAcad::iA2DrawNoWidth;
		EDIBAcad::AcAtt[1].iWidth=EDIBAcad::iA2DrawNameWidth;
		EDIBAcad::AcAtt[6].iWidth=EDIBAcad::iA2VlmNameWidth;
		EDIBAcad::AcAtt[5].iWidth=EDIBAcad::iA2DateWidth;
	}
	EDIBAcad::drawTK(EDIBgbl::DrawNoSel, EDIBgbl::inc, EDIBgbl::DrawType, EDIBgbl::Tbhq, EDIBgbl::gbTbOS, EDIBgbl::TKsubzone, basDirectory::BlkDir, p0, EDIBAcad::AcAtt,9,modPHScal::gbNoWaiKuang);

	CDialog::OnOK();
}

void CFrmDrawInfo::OnChangeEditScale() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}
