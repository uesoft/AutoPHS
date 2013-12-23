// FrmPhsSamp.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "FrmPhsSamp.h"
#include "modPHScal.h"
#include "FrmListBox.h"
#include "PhsData.h"
#include "Cphs.h"
#include "user.h"
#include "basDirectory.h"
#include "FrmTxsr.h"
#include "ModEncrypt.h"
#include "FrmListBox.h"
#include "FrmBmpSe.H"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFrmPhsSamp dialog
CFrmPhsSamp FrmPhsSamp;

CFrmPhsSamp::CFrmPhsSamp(CWnd* pParent /*=NULL*/)
	: CDialog(CFrmPhsSamp::IDD, pParent)
{
	m_bLoadPA=FALSE;
	m_bLoadSA=FALSE;
	m_bIsSelPA=false;
	m_bIsSelSA=false;
	//{{AFX_DATA_INIT(CFrmPhsSamp)
	m_bOrderByDesc = FALSE;
	m_nRodNum = _T("2");
	m_nSprNum = _T("1");
	m_bFilter = FALSE;
	m_bSh = FALSE;
	m_bSABEAM = FALSE;
	m_bSACANTil = FALSE;
	m_bSALBRACE = FALSE;
	m_bDH = FALSE;
	m_nCSPRNum = _T("0");
	m_bCurrentSA = FALSE;
	m_bCurrentPA = FALSE;
	m_bSAGCEMENT = FALSE;
	m_bCurrentZdjhPA = FALSE;
	m_bCurrentZdjhSA = FALSE;
	m_bCurrentZdjhAvPA = TRUE;
	m_bCurrentZdjhAvSA = TRUE;
	m_bAutoSprNum = TRUE;
	//}}AFX_DATA_INIT
	m_ListID=NULL;
	m_ListIDCount=0;
	m_strListPA=NULL;
}


void CFrmPhsSamp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFrmPhsSamp)
	DDX_Control(pDX, IDC_COMBO_SA, m_comboSA);
	DDX_Control(pDX, IDC_COMBO_PA, m_comboPA);
	DDX_Control(pDX, IDC_FRM_SEL, m_FrameSelPhs);
	DDX_Control(pDX, IDC_LIST1, m_ListSelPhs);
	DDX_Check(pDX, IDC_CHECK1, m_bOrderByDesc);
	DDX_CBStringExact(pDX, IDC_COMBO_RODNUM, m_nRodNum);
	DDX_CBStringExact(pDX, IDC_COMBO_SPRNUM, m_nSprNum);
	DDX_Check(pDX, IDC_CHECK_FILTER, m_bFilter);
	DDX_Check(pDX, IDC_CHECK_SH, m_bSh);
	DDX_Check(pDX, IDC_CHECK_SABEAM, m_bSABEAM);
	DDX_Check(pDX, IDC_CHECK_SACANT, m_bSACANTil);
	DDX_Check(pDX, IDC_CHECK_SALBRACE, m_bSALBRACE);
	DDX_Check(pDX, IDC_CHECK_DH, m_bDH);
	DDX_CBStringExact(pDX, IDC_COMBO_CSPRNUM, m_nCSPRNum);
	DDX_Check(pDX, IDC_CHECK_CURENTSA, m_bCurrentSA);
	DDX_Check(pDX, IDC_CHECK_CURRENTPA, m_bCurrentPA);
	DDX_Check(pDX, IDC_CHECK_SAGCEMENT, m_bSAGCEMENT);
	DDX_Check(pDX, IDC_CHECK_CURRENTZDJHPA, m_bCurrentZdjhPA);
	DDX_Check(pDX, IDC_CHECK_CURRENTZDJHSA, m_bCurrentZdjhSA);
	DDX_Check(pDX, IDC_CHECK_CURRENTZDJHAVPA, m_bCurrentZdjhAvPA);
	DDX_Check(pDX, IDC_CHECK_CURRENTZDJHAVSA, m_bCurrentZdjhAvSA);
	DDX_Check(pDX, IDC_CHECK_AUTOSPRNUM, m_bAutoSprNum);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFrmPhsSamp, CDialog)
	//{{AFX_MSG_MAP(CFrmPhsSamp)
	ON_WM_ACTIVATE()
	ON_BN_CLICKED(IDC_RADIO1, OnOptSortIndex)
	ON_COMMAND(ID_OLE_SIZE, OnOleSize)
	ON_BN_CLICKED(IDC_RADIO2, OnOptSortFREQ)
	ON_BN_CLICKED(IDC_RADIO3, OnOptSortiNumOfPart)
	ON_BN_CLICKED(IDC_RADIO5, OnOptSortName)
	ON_BN_CLICKED(IDC_RADIO4, OnOptSortPAID)
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_CHECK1, OnByDesc)
	ON_COMMAND(ID_BY_ROOT, OnmnuiSALimited)
	ON_COMMAND(ID_BY_PIPEROOT, OnmnuiPASALimited)
	ON_COMMAND(ID_SEL_AVAIL, OnmnuIAvailableSamp)
	ON_COMMAND(ID_SEL_ALL, OnmnuIALLSamp)
	ON_COMMAND(ID_SEL1, OnmnuIALLSPRSamp)
	ON_COMMAND(ID_SEL2, OnmnuIALLCSPRSamp)
	ON_COMMAND(ID_SEL3, OnmnuIALLNotSPRCSPRSamp)
	ON_COMMAND(ID_CHECKID, OnmnuICheckID)
	ON_COMMAND(ID_CHECKDUP, OnmnuICHECKALLindex)
	ON_COMMAND(ID_MODIFYNAME, OnmnuIEDITName)
	ON_BN_CLICKED(IDC_FRM_SEL, OnFrmSel)
	ON_LBN_SELCHANGE(IDC_LIST1, OnSelChangeListSelPhs)
	ON_WM_RBUTTONUP()
	ON_WM_SHOWWINDOW()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CHECK_FILTER, OnCheckFilter)
	ON_CBN_SELCHANGE(IDC_COMBO_SPRNUM, OnSelchangeComboSprnum)
	ON_CBN_SELCHANGE(IDC_COMBO_CSPRNUM, OnSelchangeComboCsprnum)
	ON_BN_CLICKED(IDC_CHECK_CURRENTPA, OnCheckCurrentPA)
	ON_BN_CLICKED(IDC_CHECK_CURENTSA, OnCheckCurrentSA)
	ON_BN_CLICKED(IDC_CHECK_CURRENTZDJHPA, OnCheckCurrentZdjhPA)
	ON_BN_CLICKED(IDC_CHECK_CURRENTZDJHSA, OnCheckCurrentZdjhSA)
	ON_BN_CLICKED(IDC_CHECK_CURRENTZDJHAVPA, OnCheckCurrentZdjhAvPA)
	ON_BN_CLICKED(IDC_CHECK_CURRENTZDJHAVSA, OnCheckCurrentZdjhAvSA)
	ON_BN_CLICKED(IDC_CHECK_AUTOSPRNUM, OnCheckAutosprnum)
	ON_CBN_DROPDOWN(IDC_COMBO_PA, OnDropdownComboPa)
	ON_CBN_DROPDOWN(IDC_COMBO_SA, OnDropdownComboSa)
	ON_CBN_SELCHANGE(IDC_COMBO_PA, OnSelchangeComboPa)
	ON_CBN_SELCHANGE(IDC_COMBO_SA, OnSelchangeComboSa)
	ON_BN_CLICKED(IDC_CHECK_DH, OnCheckDh)
	ON_BN_CLICKED(IDC_CHECK_SH, OnCheckSh)
	ON_BN_CLICKED(IDC_CHECK_SABEAM, OnCheckSabeam)
	ON_BN_CLICKED(IDC_CHECK_SACANT, OnCheckSacant)
	ON_BN_CLICKED(IDC_CHECK_SALBRACE, OnCheckSalbrace)
	ON_BN_CLICKED(IDC_CHECK_SAGCEMENT, OnCheckSagcement)
	ON_CBN_SELCHANGE(IDC_COMBO_RODNUM, OnCheckFilter)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFrmPhsSamp message handlers

BOOL CFrmPhsSamp::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CString sTmp;
	int i;
	m_popMenu.LoadMenu(IDR_PHSSAMP);
	CRect rc;
	if(user::GetPos("FrmPhsSamp",rc))
		this->SetWindowPos(NULL,rc.left,rc.top,0,0,SWP_NOSIZE|SWP_NOZORDER);
	// TODO: Add extra initialization here
	//AutoRedraw = true;
   //Dim k As Integer
   //Dim rs As Recordset
   //LoadCaption
   //ScaleMode = vbPixels
   //GetPos Me
   //gbStopOLE = False

   //类初始化时自动对空表加入记录
   //然后调用下面的过程修改名称,出错!
   //因为其中用到数据控件data1.recordset,而此时它尚未初始化
   //If rs.EOF And rs.BOF Then
      //phsStructureName为空则调用过程
      //Cavphs->ChangeNameInphsStructureName   //注释掉加快速度
   //End If
   
   //计算值用于选择可用型式
   //i = Cavphs->CheckDuplicateIndex  //注释掉加快速度
   //i = Cavphs->CheckDuplicateREFRecord //注释掉加快速度
   SelectItemMenu = mnuIALLSamp;
   SelectItemOpt = optSortFREQ;
   
   //Call OptionSortIndex_Click(optSortIndex)
   this->UpdateRecordset();
   this->CheckRadioButton(IDC_RADIO1,IDC_RADIO5,optSortIndex);
   this->OnOptSortIndex();
   if (modPHScal::iSelSampleID <= 0)
      modPHScal::iSelSampleID = 1;
	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFrmPhsSamp::OnOleSize() 
{
	// TODO: Add your command handler code here

}



void CFrmPhsSamp::OnOptSortIndex() 
{
	// TODO: Add your control notification handler code here
	SortFieldName = "SampleID";
	SelectItemOpt=0;
	//UpdateRecordset();
	mnuDWGM_Click(SelectItemMenu);
}

void CFrmPhsSamp::OnOptSortFREQ() 
{
	// TODO: Add your control notification handler code here
	SortFieldName = "Frequence";
	SelectItemOpt=1;
	//UpdateRecordset();
	mnuDWGM_Click(SelectItemMenu);
}

void CFrmPhsSamp::OnOptSortiNumOfPart() 
{

	SortFieldName = "iNumOfPart";
	SelectItemOpt=2;
	//UpdateRecordset();
	mnuDWGM_Click(SelectItemMenu);
}

void CFrmPhsSamp::OnOptSortName() 
{
	// TODO: Add your control notification handler code here
	SortFieldName = "SampleName";
	SelectItemOpt=3;
	//UpdateRecordset();
	mnuDWGM_Click(SelectItemMenu);
}

void CFrmPhsSamp::OnOptSortPAID() 
{
	// TODO: Add your control notification handler code here
	SortFieldName = "PA";
	SelectItemOpt=4;
	//UpdateRecordset();
	mnuDWGM_Click(SelectItemMenu);
}

void CFrmPhsSamp::mnuDWGM_Click(int Index)
{
  //Screen.MousePointer = vbHourglass
   //int i;
   //switch(Index)
   //{
   /*case mnuiPALimited:
         modPHScal::PreCalCrudeData( FrmTxsr.m_pViewTxsr->m_ActiveRs);
         Cavphs->PA = modPHScal::dn;
         SelectItemMenu = Index;
         Cavphs->iLIST = Index;
         Cavphs->SortFieldName = SortFieldName;
          Data1 = Cavphs->AvailableSampleIDrsForphsSamp(strOrderByDesc);
		 break;
   case mnuiSALimited:
         modPHScal::PreCalCrudeData( FrmTxsr.m_pViewTxsr->m_ActiveRs);
         Cavphs->SA = modPHScal::gn;
         SelectItemMenu = Index;
         Cavphs->iLIST = Index;
         Cavphs->SortFieldName = SortFieldName;
          Data1 = Cavphs->AvailableSampleIDrsForphsSamp(strOrderByDesc);
		 break;
   case mnuiPASALimited:
         modPHScal::PreCalCrudeData( FrmTxsr.m_pViewTxsr->m_ActiveRs);
         Cavphs->PA = modPHScal::dn;
         Cavphs->SA = modPHScal::gn;
         SelectItemMenu = Index;
         Cavphs->iLIST = Index;
         Cavphs->SortFieldName = SortFieldName;
          Data1 = Cavphs->AvailableSampleIDrsForphsSamp(strOrderByDesc);
		 break;
   case mnuIAvailableSamp:
         SelectItemMenu = Index;
         Cavphs->iLIST = Index;
         Cavphs->SortFieldName = SortFieldName;
          Data1 = Cavphs->AvailableSampleIDrsForphsSamp(strOrderByDesc);
		 break;
   case mnuIALLSamp:
   case mnuIALLSPRSamp:
   case mnuIALLCSPRSamp:
   case mnuIALLNotSPRCSPRSamp:
         SelectItemMenu = Index;
         Cavphs->iLIST = Index;
         Cavphs->SortFieldName = SortFieldName;
         Data1 = Cavphs->AvailableSampleIDrsForphsSamp(strOrderByDesc);
		 break;
   case mnuICHECKALLindex:
         Cavphs->CheckDuplicateIndex();
         this->SendMessage(WM_CLOSE);
		 //Unload frmStatus
		 break;
   case mnuIEDITName:
         Cavphs->ChangeNameInphsStructureName();
         this->SendMessage(WM_CLOSE);
		 //Unload frmStatus
		 break;
   case mnuICheckID:
         Cavphs->CheckAllCustomIDandIDinPhsStructureREF();
         this->SendMessage(WM_CLOSE);
		 //Unload frmStatus
		 break;
   }*/
	      Cavphs->iLIST = Index;
         Cavphs->SortFieldName = SortFieldName;
         Data1 = Cavphs->AvailableSampleIDrsForphsSamp(strOrderByDesc,this->m_strFilter);
#ifdef _DEBUG 
		 int iCount=Data1->GetRecordCount();
#endif
   /*if(Index>=0 && Index<=7)
		this->m_popMenu.GetSubMenu(1)->CheckMenuRadioItem(0,7,Index,MF_BYPOSITION);
	*/
	this->m_ListSelPhs.ResetContent();
	if(!Data1->FindFirst("SampleID="+ ltos(modPHScal::iSelSampleID)) && !(Data1->IsEOF() && Data1->IsBOF()))
		Data1->MoveFirst();
   //this->Data1_Reposition();
   LoadListSelPhs();
}

void CFrmPhsSamp::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
/*	CPoint pos=point;
	this->ClientToScreen(&pos);
	m_popMenu.GetSubMenu(1)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,pos.x,pos.y,this);
	CDialog::OnLButtonUp(nFlags, point);*/
}

void CFrmPhsSamp::OnByDesc() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if(this->m_bOrderByDesc)
		strOrderByDesc = " DESC";
	else
		strOrderByDesc = "";
	UpdateRecordset();
	//mnuDWGM_Click(SelectItemMenu);
}

void CFrmPhsSamp::OnmnuiPALimited() 
{
	// TODO: Add your command handler code here
	mnuDWGM_Click(mnuiPALimited);
}

void CFrmPhsSamp::OnmnuiSALimited() 
{
	// TODO: Add your command handler code here
	mnuDWGM_Click(mnuiSALimited);
}

void CFrmPhsSamp::OnmnuiPASALimited() 
{
	// TODO: Add your command handler code here
	mnuDWGM_Click(mnuiPASALimited);
}

void CFrmPhsSamp::OnmnuIAvailableSamp() 
{
	// TODO: Add your command handler code here
	mnuDWGM_Click(mnuIAvailableSamp);
}

void CFrmPhsSamp::OnmnuIALLSamp() 
{
	// TODO: Add your command handler code here
	mnuDWGM_Click(mnuIALLSamp);
}

void CFrmPhsSamp::OnmnuIALLSPRSamp() 
{
	// TODO: Add your command handler code here
	mnuDWGM_Click(mnuIALLSPRSamp);
}

void CFrmPhsSamp::OnmnuIALLCSPRSamp() 
{
	// TODO: Add your command handler code here
	mnuDWGM_Click(mnuIALLCSPRSamp);
}

void CFrmPhsSamp::OnmnuIALLNotSPRCSPRSamp() 
{
	// TODO: Add your command handler code here
	mnuDWGM_Click(mnuIALLNotSPRCSPRSamp);
}

void CFrmPhsSamp::OnmnuICHECKALLindex() 
{
	// TODO: Add your command handler code here
	mnuDWGM_Click(mnuICHECKALLindex);
}

void CFrmPhsSamp::OnmnuIEDITName() 
{
	// TODO: Add your command handler code here
	mnuDWGM_Click(mnuIEDITName);
}

void CFrmPhsSamp::OnmnuICheckID() 
{
	// TODO: Add your command handler code here
	//mnuDWGM_Click(mnuICheckID);
}

void CFrmPhsSamp::Data1_Reposition()
{
//样本图使用频度
	try{
	COleVariant v;
	Data1->GetFieldValue("SampleID",v);
   modPHScal::iSelSampleID = vtoi(v);
   //FrameSelPhs.Caption = "选择支吊架组装模板" & modPHScal::iSelSampleID & " (第" & (Data1->Recordset.AbsolutePosition + 1) & "项 共" & Data1->Recordset.RecordCount & "项 使用频度" & Data1->Recordset.Fields("FREQUENCE") & ")"
   CString sTmp,sTmp2;
   sTmp.Format(GetResStr(IDS_SelSampleIDNoXSumXFRQx),"\%d","\%d","\%d","\%d");
   Data1->GetFieldValue("FREQUENCE",v);
   sTmp2.Format(sTmp,modPHScal::iSelSampleID,Data1->GetAbsolutePosition()+1,Data1->GetRecordCount(),vtoi(v));
   this->m_FrameSelPhs.SetWindowText(sTmp2);
   Data1->GetFieldValue("SampleName",v);
   if(v.vt!=VT_NULL)
	   sBmpName = basDirectory::TemplateDir + vtos(v);
   if(sBmpName.Find(".dwg",0)==-1)
	   sBmpName+=".dwg";
   //MsgBox sBmpName
   //将图形显示在OLE1控件中
   /*If Not gbStopOLE Then
      If mbCreateEmbed Then
         If FileExists(sBmpName) Then
            mObjAcadDoc.Open sBmpName
         Else
            mObjAcadDoc.New sBmpName
         End If
      Else
         If FileExists(sBmpName) Then
            //然后建立连接AutoCAD，这应该启动AutoCAD的第二个实例。
            OLE1.CreateEmbed sBmpName
            OLE1.object.Application.Visible = False
            Set mObjAcadApp = OLE1.object.Application
            Set mObjAcadDoc = OLE1.object.Application.ActiveDocument
            mbCreateEmbed = True
         End If
      End If
   Else
      OLE1.Enabled = False
   End If*/
   CDaoRecordset rs;
   rs.m_pDatabase=&EDIBgbl::dbPRJ;
   sTmp="SELECT CustomID FROM phsStructureREF WHERE SampleID=";
   sTmp2.Format("%d",modPHScal::iSelSampleID);
   sTmp+=sTmp2;
   sTmp+=" ORDER BY SEQ";
   rs.Open(dbOpenSnapshot,sTmp);
   if(IsWindow(FrmListBox.m_hWnd))
   {
	   FrmListBox.m_ListPhsStruEDIT.ResetContent();
	   while(!rs.IsEOF())
	   {
		   rs.GetFieldValue("CustomID",v);
		   if(vtos(v)!="")
			   FrmListBox.m_ListPhsStruEDIT.AddString(vtos(v));
		   rs.MoveNext();
	   }
   }
   rs.Close();
   
   
   Cavphs->SourceObj = &FrmListBox.m_ListPhsStruEDIT;
	}
	catch(::CDaoException * e)
	{
		//e->ReportError();
		e->Delete();
	}
}

void CFrmPhsSamp::OnFrmSel() 
{
	// TODO: Add your control notification handler code here
	
}

void CFrmPhsSamp::LoadListSelPhs()
{
	try{
		if(Data1==NULL || !Data1->IsOpen())
			return;
		COleVariant v;
		COleVariant book;
		this->m_ListSelPhs.ResetContent();
		long ix=0;
		if(m_ListID!=NULL)
		{
			delete []/*m_ListIDCount]*/ m_ListID;
			m_ListID=NULL;
			m_ListIDCount=0;
		}
		if(Data1->IsBOF() && Data1->IsEOF())
			return;
		Data1->GetFieldValue("SampleID",v);
		long id=vtoi(v);
		book=Data1->GetBookmark();
		Data1->MoveLast();
		Data1->MoveFirst();
		this->m_ListIDCount=Data1->GetRecordCount();
		this->m_ListID=new long [m_ListIDCount];
		int i=0;
		while(!Data1->IsEOF())
		{
			Data1->GetFieldValue("SampleName",v);
			if(vtos(v)!="")
				this->m_ListSelPhs.AddString(vtos(v));
			else
				this->m_ListSelPhs.AddString(" ");
			Data1->GetFieldValue("SampleID",v);
			m_ListID[i]=vtoi(v);
			if(m_ListID[i]==id)
				ix=i;
			i++;
			Data1->MoveNext();
		}
		Data1->SetBookmark(book);
		if(this->m_ListSelPhs.GetCount()>0)
			this->m_ListSelPhs.SetCurSel(ix);
		else
			this->m_ListSelPhs.SetCurSel(-1);
		/*Data2.GetFieldValue("SampleID",v);
		CString sTmp;
		if(v.vt!=VT_NULL)
		{
			sTmp="SampleID=";
			sTmp+=v.pcVal;
			if(Data1->FindFirst(sTmp))
			{
				Data1->GetFieldValue("SampleName",v);
				if(v.vt!=VT_NULL && CString(v.pbVal)!="")
				{
					int ix=this->m_ListSelPhs.FindStringExact(-1,v.pcVal);
					if(ix>=0)
						this->m_ListSelPhs.SetCurSel(ix);
					else
						this->m_ListSelPhs.SetCurSel(-1);
				}
			}
		}*/
		Data1_Reposition();
	}
	catch(::CDaoException * e)
	{
		e->ReportError();
		e->Delete();
	}
		//Data1->SetBookmark(book);
}

void CFrmPhsSamp::OnSelChangeListSelPhs() 
{
	// TODO: Add your control notification handler code here
	try
	{
	    if(Data1==NULL || !Data1->IsOpen())
			return;
		CString sTmp;
		int ix=this->m_ListSelPhs.GetCurSel();
		if(ix<0 || ix > m_ListIDCount-1 || m_ListID==NULL)
			return;
		sTmp.Format("%d",m_ListID[ix]);
		Data1->FindFirst(_T("SampleID="+sTmp));
		this->Data1_Reposition();
	}
	catch(::CDaoException * e)
	{
		//e->ReportError();
		e->Delete();
	}
}

CFrmPhsSamp::~CFrmPhsSamp()
{
	if(m_ListID!=NULL)
	{
		delete []/*m_ListIDCount]*/ m_ListID;
	}
}

void CFrmPhsSamp::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
		CPoint p=point;
	ClientToScreen(&p);
	m_popMenu.GetSubMenu(1)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,p.x,p.y,this);

	CDialog::OnRButtonUp(nFlags, point);
}

void CFrmPhsSamp::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	if(bShow==FALSE)
	{
		//SetWindowPos(&wndNoTopMost,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
		//user::SavePos(this,"FrmPhsSamp");
	}
}

void CFrmPhsSamp::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	user::SavePos(this,"FrmPhsSamp");
	CDialog::OnClose();
	//FrmListBox.ShowWindow(SW_HIDE);
}

void CFrmPhsSamp::GetFilterStr()
{
	UpdateData();
	try
	{
		CString sTmp,strTmp;
		COleVariant vTmp;
		
		//不选择过滤器时，所有选项框、单选纽均不可用
		//列表中当前根部IDC_CHECK_CURENTSA
		GetDlgItem(IDC_CHECK_CURENTSA)->EnableWindow(m_bFilter && m_bCurrentZdjhAvSA);
		GetDlgItem(IDC_CHECK_CURRENTPA)->EnableWindow(m_bFilter && m_bCurrentZdjhAvPA);
		GetDlgItem(IDC_CHECK_CURRENTZDJHSA)->EnableWindow(m_bFilter && m_bCurrentZdjhAvSA);
		GetDlgItem(IDC_CHECK_CURRENTZDJHPA)->EnableWindow(m_bFilter && m_bCurrentZdjhAvPA);
		GetDlgItem(IDC_CHECK_CURRENTZDJHAVSA)->EnableWindow(m_bFilter);
		GetDlgItem(IDC_CHECK_CURRENTZDJHAVPA)->EnableWindow(m_bFilter);
		GetDlgItem(IDC_CHECK_AUTOSPRNUM)->EnableWindow(m_bFilter);
		GetDlgItem(IDC_CHECK_DH)->EnableWindow(m_bFilter);
		GetDlgItem(IDC_CHECK_SH)->EnableWindow(m_bFilter);
		GetDlgItem(IDC_CHECK_SABEAM)->EnableWindow(m_bFilter);
		GetDlgItem(IDC_CHECK_SACANT)->EnableWindow(m_bFilter);
		GetDlgItem(IDC_CHECK_SALBRACE)->EnableWindow(m_bFilter);
		GetDlgItem(IDC_COMBO_RODNUM)->EnableWindow(m_bFilter);
		GetDlgItem(IDC_COMBO_SPRNUM)->EnableWindow(m_bFilter);
		GetDlgItem(IDC_COMBO_CSPRNUM)->EnableWindow(m_bFilter);
		GetDlgItem(IDC_CHECK_SAGCEMENT)->EnableWindow(m_bFilter);
		GetDlgItem(IDC_COMBO_PA)->EnableWindow(m_bFilter);
		GetDlgItem(IDC_COMBO_SA)->EnableWindow(m_bFilter);

		//过滤器不选择时，过滤字符串置空，以便显示全部模板
		m_strFilter="";
		if(this->m_bFilter==FALSE)
		{	
			m_strFilter=" bAllowUse is not null AND bAllowUse <>0 ";
			return;
		}
		
		//自适应热位移
		if(m_bAutoSprNum)
		{
			if(modPHScal::bPAIsHanger() && m_nRodNum=="0")
			{
				((CComboBox*)GetDlgItem(IDC_COMBO_RODNUM))->SetCurSel(((CComboBox*)GetDlgItem(IDC_COMBO_RODNUM))->FindString(-1,"2"));
				((CComboBox*)GetDlgItem(IDC_COMBO_RODNUM))->GetLBText(((CComboBox*)GetDlgItem(IDC_COMBO_RODNUM))->GetCurSel(),m_nRodNum);
			}
			modPHScal::ZdjType=vtos(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect("Type"));
			::giUPxyz=vtoi(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect("UPXYZ"));
			if(giUPxyz ==0 ) giUPxyz=2;
			if(giUPxyz==1)
				modPHScal::yr=vtof(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect("xr1"));
			if(giUPxyz==2)
				modPHScal::yr=vtof(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect("yr1"));
			if(giUPxyz==3)
				modPHScal::yr=vtof(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect("zr1"));
			if(fabs(modPHScal::yr) > modPHScal::gUseSpringMinDist)
			{
				if(modPHScal::ZdjType=="9999" || modPHScal::ZdjType=="5000")
				{
					m_nSprNum="0";
					if(m_nCSPRNum=="0")
						m_nCSPRNum="大于 0";
				}
				else
				{
					if(modPHScal::ZdjType.GetLength() ==3)
					{
						//如果是吊架,不作判断,因为计算的ZdjType可能并不正确.
						/*if(!modPHScal::bPAIsHanger())
						{
							m_nSprNum="大于 0";
							m_nCSPRNum="0";
						}*/
					}
					else
					{
						if(modPHScal::ZdjType.GetLength() ==4)
						{
							if(modPHScal::ZdjType.GetAt(1)=='1')
							{
								m_bDH=FALSE;
								m_bSh=TRUE;
							}
							else
							{
								m_bDH=TRUE;
								m_bSh=FALSE;
							}
							long iTmp=(modPHScal::ZdjType.GetAt(0) -'0') / 2 ;
							iTmp += ((modPHScal::ZdjType.GetAt(0) -'0') % 2 == 0 ? 0 : 1);
							
							if(iTmp > 2 ) 
							{
								m_nSprNum="大于 0";
								m_nCSPRNum="0";
							}
							else
							{
								m_nCSPRNum="0";
								m_nSprNum.Format("%d",iTmp);
							}
						}
						else
						{
							if(m_nSprNum=="0")
								m_nSprNum="大于 0";
							m_nCSPRNum="0";
						}
					}
				}
			}
			else
			{
				//非弹簧支吊架
				m_nSprNum="0";
				m_nCSPRNum="0";
			}
			//支架,拉杆数=0
			if(!modPHScal::bPAIsHanger())
				m_nRodNum="0";
		}

		//双吊、单吊纽可组合选择，表示过滤出单双吊
		GetDlgItem(IDC_CHECK_DH)->EnableWindow(/*!m_bCurrentPA &&*/ !m_bCurrentZdjhPA || m_bAutoSprNum/*&& !m_bCurrentZdjhAvPA*/);
		GetDlgItem(IDC_CHECK_SH)->EnableWindow(/*!m_bCurrentPA &&*/ !m_bCurrentZdjhPA || m_bAutoSprNum/*&& !m_bCurrentZdjhAvPA*/);
		
		if(m_bCurrentPA)
		{
			//按模板列表框中当前管部选择
			Data1->GetFieldValue("PA",vTmp);
			m_strFilter=" PA = \'" + vtos(vTmp) + "\'";
			m_comboPA.EnableWindow(FALSE);
		}
		else
		{
			//不按模板列表框中当前管部选择			
			if(m_bCurrentZdjhPA)
			{
				//按当前支吊架管部选择
				m_comboPA.EnableWindow(FALSE);
				if(m_strFilter!="")
					m_strFilter += " AND ";
				m_strFilter+=" PA = \'" + vtos(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect("IDdn1")) + "\'";
			}
			else
			{
				//不按当前支吊架管部选择
				//是否选择了某种管部型式
				m_comboPA.EnableWindow(TRUE);
				CString strSelPA;
				strSelPA="全部";
				int ix=m_comboPA.GetCurSel();
				if(ix!=-1)
					m_comboPA.GetLBText(ix,strSelPA);
				if (strSelPA!="全部" && m_bIsSelPA)
				{
					//选择了某种管部型式,管部型式组合框中不是"全部"字符串
					if(m_strFilter!="")
						m_strFilter += " AND ";
					m_strFilter+=" PA = \'" + modPHScal::sFindID(strSelPA) + "\'";
					
				}
				else
				{
					//没有选择某种管部型式,管部型式组合框中是"全部"字符串
					//复位组合框，重新装载
					m_bIsSelPA=false;
				}
			}
		}

		//按当前支吊架可用管部选择
		sTmp=vtos(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect("Unit_Force"));
		sTmp.MakeUpper();
		modPHScal::pjg=vtof(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect("pjg1"));
		if(sTmp=="N")
			modPHScal::pjg /= EDIBgbl::kgf2N;
		float tmppjg=fabs(modPHScal::pjg);
		modPHScal::t0=vtof(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect("T01"));
		modPHScal::dj=vtof(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect("dj1"));
		if(m_bCurrentZdjhAvPA)
		{
			if(m_strFilter!="")
				m_strFilter += " AND ";
			//烟风煤粉管道矩形管径=null
			if(modPHScal::bPAIsHanger())
			{
				m_strFilter += "( PA IN (Select ID FROM PictureClipData IN \'" + EDIBgbl::dbPRJ.GetName() + 
					"\' WHERE CustomID IN ( Select CustomID FROM [" + modPHScal::tbnPA + "] IN \"\" [\; DATABASE=" 
									+ modPHScal::dbZDJcrude.GetName() + " ;PWD=" + ModEncrypt::gstrDBZdjCrudePassWord + "]WHERE (Pmax >=" 
									+ ftos(tmppjg) + " AND PictureClipData.ClassID<>" + ltos(iPAfixZ1) + " AND PictureClipData.ClassID<>" + ltos(iPAfixZ2) 
									+") AND (Dw >= " + ftos(modPHScal::dj* (1 - modPHScal::gnDW_delta * 0.01))
									+ " AND Dw <= " + ftos(modPHScal::dj* (1 + modPHScal::gnDW_delta * 0.01)) 
									+ " AND Dw>0 OR Dw IS NULL OR Dw=0) AND tj >= " + ftos(modPHScal::t0) 
									+ " )))";
			}
			else
			{
				m_strFilter += "( PA IN (Select ID FROM PictureClipData IN \'" + EDIBgbl::dbPRJ.GetName() + 
					"\' WHERE CustomID IN ( Select CustomID FROM [" + modPHScal::tbnPA + "] IN \"\" [\; DATABASE=" 
									+ modPHScal::dbZDJcrude.GetName() + " ;PWD=" + ModEncrypt::gstrDBZdjCrudePassWord + "]WHERE (Pmax >=" 
									+ ftos(tmppjg) + " AND PictureClipData.ClassID<>" + ltos(iPAfixZ1) + " AND PictureClipData.ClassID<>" + ltos(iPAfixZ2) 
									+") AND (Dw >= " + ftos(modPHScal::dj* (1 - modPHScal::gnDW_delta * 0.01))
									+ " AND Dw <= " + ftos(modPHScal::dj* (1 + modPHScal::gnDW_delta * 0.01)) 
									+ " AND Dw>0 OR Dw IS NULL OR Dw=0) AND tj >= " + ftos(modPHScal::t0) 
									+ " )) OR PA IN (\'Z1\',\'Z2\',\'Z2A\') )";
			}
								//+" )))";
								//+ ") OR PictureClipData.ClassID=" + ltos(iPAfixZ1) + " OR PictureClipData.ClassID=" + ltos(iPAfixZ2) + ")) ";
			/*m_strFilter += "( PA IN (Select ID FROM PictureClipData IN \'" + EDIBgbl::dbPRJ.GetName() + 
				"\' WHERE (ClassID<>" + ltos(iPAfixZ1) + " AND ClassID<>" + ltos(iPAfixZ2) 
							    + " AND CustomID IN ( Select CustomID FROM [" + modPHScal::tbnPA + "] IN \"\" [\; DATABASE=" 
								+ modPHScal::dbZDJcrude.GetName() + " ;PWD=" + ModEncrypt::gstrDBZdjCrudePassWord + "]WHERE Pmax >=" 
								+ ftos(tmppjg)
								+ " AND (Dw >= " + ftos(modPHScal::dj* (1 - modPHScal::gnDW_delta * 0.01))
								+ " AND Dw <= " + ftos(modPHScal::dj* (1 + modPHScal::gnDW_delta * 0.01)) 
								+ " AND Dw>0 OR Dw IS NULL OR Dw=0) AND tj >= " + ftos(modPHScal::t0) + "))"
								+ " OR ((PictureClipData.ClassID=" + ltos(iPAfixZ1) + " OR PictureClipData.ClassID=" + ltos(iPAfixZ2)
								+ " ))))";*/
								/*+ " ) AND CustomID IN ( Select CustomID FROM [" + modPHScal::tbnPAfix + "] IN \"\" [\; DATABASE=" 
								+ modPHScal::dbZDJcrude.GetName() + " ;PWD=" + ModEncrypt::gstrDBZdjCrudePassWord + "]WHERE " 
								+ "(Dw >= " + ftos(modPHScal::dj* (1 - modPHScal::gnDW_delta * 0.01))
								+ " AND Dw <= " + ftos(modPHScal::dj* (1 + modPHScal::gnDW_delta * 0.01)) 
								+ " AND Dw>0 OR Dw IS NULL OR Dw=0) AND tj >= " + ftos(modPHScal::t0) + ")))) ";*/
			/*m_strFilter += "( PA IN (Select ID FROM PictureClipData IN \'" + EDIBgbl::dbPRJ.GetName() + 
				"\' WHERE CustomID IN ( Select CustomID FROM [" + modPHScal::tbnPA + "] IN \"\" [\; DATABASE=" 
								+ modPHScal::dbZDJcrude.GetName() + " ;PWD=" + ModEncrypt::gstrDBZdjCrudePassWord + "] WHERE  PictureClipData.CustomID=CustomID AND (Pmax >=" 
								+ ftos(tmppjg)
								+ " OR PictureClipData.ClassID=" + ltos(iPAfixZ1) + " OR PictureClipData.ClassID=" + ltos(iPAfixZ2) 
								+ ") AND (Dw >= " + ftos(modPHScal::dj* (1 - modPHScal::gnDW_delta * 0.01))
								+ " AND Dw <= " + ftos(modPHScal::dj* (1 + modPHScal::gnDW_delta * 0.01)) 
								+ " AND Dw>0 OR Dw IS NULL OR Dw=0) AND tj >= " + ftos(modPHScal::t0) + "))) ";
			*///AfxMessageBox(m_strFilter);
		}
		//不按当前支吊架管部选择，双吊、单吊2按钮可任意组合选择模板
		if(!m_bCurrentZdjhPA || m_bAutoSprNum)
		{
			strTmp="";
			if(m_bDH)
			{  
				if(m_bSh)
					strTmp="( ParalleledNum = 2 OR ParalleledNum = 3  OR ParalleledNum = 1)";
				else
					strTmp="( ParalleledNum = 2 OR ParalleledNum = 3 )";
			}
			else
			{
				if(m_bSh)
					strTmp="( ParalleledNum = 1)";
				else
					strTmp="";
			}
			if(m_nRodNum=="0")
			{
				//strTmp="( ParalleledNum IS NULL OR ParalleledNum = 2 OR ParalleledNum = 3  OR ParalleledNum = 1)";
				strTmp="";
			}
			if(strTmp!="")
			{
				if(m_strFilter!="")
					m_strFilter += " AND ";
				m_strFilter+=" ( PA IN (SELECT ID FROM PictureClipData IN \'" + EDIBgbl::dbPRJ.GetName() + "\' WHERE " 
								+ strTmp + ")) ";
			}
		}
		//简支、悬臂、三角架、混凝土梁四组复选框可组合
		GetDlgItem(IDC_CHECK_SABEAM)->EnableWindow(/*!m_bCurrentSA && */!m_bCurrentZdjhSA /*&& !m_bCurrentZdjhAvSA*/);
		GetDlgItem(IDC_CHECK_SACANT)->EnableWindow(/*!m_bCurrentSA && */!m_bCurrentZdjhSA /*&& !m_bCurrentZdjhAvSA*/);
		GetDlgItem(IDC_CHECK_SALBRACE)->EnableWindow(/*!m_bCurrentSA && */!m_bCurrentZdjhSA /*&& !m_bCurrentZdjhAvSA*/);
		GetDlgItem(IDC_CHECK_SAGCEMENT)->EnableWindow(/*!m_bCurrentSA &&*/ !m_bCurrentZdjhSA /*&& !m_bCurrentZdjhAvSA*/);
		
		if(m_bCurrentSA)
		{
			//按模板列表框中当前根部选择
			Data1->GetFieldValue("SA",vTmp);
			if(m_strFilter!="")
				m_strFilter += " AND ";
			m_strFilter += " ( SA = \'" + vtos(vTmp) + "\' )";
			m_comboSA.EnableWindow(FALSE);
		}
		else
		{
			//不按模板列表框中当前根部选择			
			if(m_bCurrentZdjhSA)
			{
				//按当前支吊架根部选择
				m_comboSA.EnableWindow(FALSE);
				if(m_strFilter!="")
					m_strFilter += " AND ";
				m_strFilter+=" ( SA = \'" + vtos(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect("IDgn1")) + "\' )";
			}
			else
			{
				//不按当前支吊架根部选择
				//是否选择了某种根部型式
				m_comboSA.EnableWindow(TRUE);
				CString strSelSA;
				strSelSA="全部";
				int ix=m_comboSA.GetCurSel();
				if(ix!=-1)
					m_comboSA.GetLBText(ix,strSelSA);
				if (strSelSA!="全部" && m_bIsSelSA)
				{
					//选择了某种根部型式,根部型式组合框中不是"全部"字符串
					
					if(m_strFilter!="")
						m_strFilter += " AND ";
					m_strFilter+=" SA = \'" + modPHScal::sFindID(strSelSA) + "\'";
				}
				else
				{
					//没有选择某种根部型式,根部型式组合框中是"全部"字符串
					m_bIsSelSA=false;
				}
			}
		}

		//按当前支吊架可用根部选择
		if(m_bCurrentZdjhAvSA)
		{
			modPHScal::glNumSA=vtoi(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect("Gnum"));
			modPHScal::gdw1=vtof(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect("gdw1"));
			if(m_strFilter!="")
				m_strFilter += " AND ";
			int Gnum;			
			Gnum= (modPHScal::glNumSA!=0 ? modPHScal::glNumSA : 1);
			m_strFilter += " ( SA IN ( Select ID FROM PictureClipData IN \'" + EDIBgbl::dbPRJ.GetName() + "\'"
								+ " WHERE EXISTS ( Select CustomID FROM [" + modPHScal::tbnSA + "] IN \"\" [; DATABASE=" 
								+ modPHScal::dbZDJcrude.GetName() + " ;PWD=" + ModEncrypt::gstrDBZdjCrudePassWord + "] WHERE PictureClipData.CustomID = CustomID AND (PictureClipData.ClassID= " + ltos(iG100) + " OR PictureClipData.ClassID = " + ltos(iSALbraceFixG47) + " OR PictureClipData.ClassID = " + ltos(iSALbraceFixG48) + " OR PMAXH >=" 
								+ ftos(tmppjg / Gnum * (vtob(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect("ifLongVertPipe")) ? Gnum : 1))
								+ " AND (( PictureClipData.ClassID = "
								+ ltos(iSACantilever) + " OR PictureClipData.ClassID = " + ltos(iSALbrace) + " OR PictureClipData.ClassID = " + ltos(iG51) + " OR PictureClipData.ClassID = " + ltos(iG56) + " OR PictureClipData.ClassID = " + ltos(iG57)  + ") AND GDW1 >="
								+ ftos(modPHScal::gdw1) + " OR ( PictureClipData.ClassID =" + ltos(iSAbeam) +" OR PictureClipData.ClassID =" + ltos(iG52_55) + " ) AND GDW1 >="
								+ " IIF("+ ftos(modPHScal::gdw1+(modPHScal::glNumSA==1 ? modPHScal::DblHangerRodDist : 0)/2) +" > L1/2 , L1 - " + ftos(modPHScal::gdw1+(modPHScal::glNumSA == 1 ? modPHScal::DblHangerRodDist : 0) / 2)
								+ " , "+ ftos(modPHScal::gdw1+(modPHScal::glNumSA == 1 ? modPHScal::DblHangerRodDist : 0) / 2) + ") AND L1> " + ftos(modPHScal::Lspan)
								+ "  OR PictureClipData.ClassID= " + ltos(iGCement) + ")) )))";
			//AfxMessageBox(m_strFilter);
		}
		//不按当前支吊架根部选择，4种根部类型均可组合过滤
		if(!m_bCurrentZdjhSA)
		{
			strTmp="";
			if(m_bSABEAM)
			{ 
				strTmp.Format(" ClassID = %d " , iSAbeam);
			}
			if(m_bSACANTil)
			{
				if(strTmp=="")
					strTmp.Format(" ClassID = %d " , iSACantilever);
				else
					strTmp +=" OR ClassID = " + ltos(iSACantilever);
			}
			if(m_bSALBRACE)
			{
				if(strTmp=="")
					strTmp.Format(" ClassID = %d " , iSALbrace);
				else
					strTmp +=" OR ClassID = " + ltos(iSALbrace);
			}
			if(m_bSAGCEMENT)
			{
				if(strTmp=="")
					strTmp.Format(" ClassID = %d " , iGCement);
				else
					strTmp +=" OR ClassID = " + ltos(iGCement);
			}
			if(strTmp!="")
			{
				if(m_strFilter != "") m_strFilter += " AND ";
				m_strFilter+="( SA IN (SELECT ID FROM PictureClipData IN \'" + EDIBgbl::dbPRJ.GetName() + "\' WHERE " 
								+ strTmp + ") )";
			}
		}

		

		//当选择弹簧串联数“大于 0”，联锁恒力弹簧串联数=0
		CString iSprNum,iRodNum, iCSprNum;
		if(m_nSprNum.Find(_T("大于"))!=-1)
			iSprNum = " iNumSpr > 0 ";
		else if(m_nSprNum.Find(_T(">= 0"))!=-1)
			iSprNum = " iNumSpr >= 0 ";
		else
			iSprNum.Format(" iNumSpr = %d ", atoi(m_nSprNum));
		//当选择恒力弹簧串联数“大于 0”，联锁弹簧串联数=0
		if(m_nCSPRNum.Find(_T("大于"))!=-1)
			iCSprNum = " iNumCSpr > 0 ";
		else if(m_nCSPRNum.Find(_T(">= 0"))!=-1)
			iCSprNum = " iNumCSpr >= 0 ";
		else
			iCSprNum.Format(" iNumCSpr = %d ", atoi(m_nCSPRNum));
		//选择拉杆串联数“大于 0”，过滤出所有吊架模板
		if(m_nRodNum.Find(_T("大于"))!=-1)
			iRodNum = " iNumRod > 0 ";
		else
			iRodNum.Format(" iNumRod = %d ", atoi(m_nRodNum));

		if(m_strFilter != "") m_strFilter += " AND ";

		m_strFilter += " " + iSprNum + " AND " + iCSprNum + " AND " + iRodNum + " AND bAllowUse is not null AND bAllowUse <>0 ";
		//MessageBox(m_strFilter);
		UpdateData(false);
	}
#ifdef _DEBUG
	catch(CDaoException * e)
	{
		e->ReportError();
		e->Delete();
	}
#endif
	catch(CException *e)
	{
		e->Delete();
	}
	catch(...)
	{
	}
}
void CFrmPhsSamp::OnActivate(UINT nState, CWnd *pWndOther, BOOL bMinimized)
{
	if(!bLockAct && nState==WA_CLICKACTIVE)
	{
		bLockAct=true;
		if(FrmListBox.m_bBmpShow)
			::frmBmpSe.SetWindowPos(this,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
		::FrmListBox.SetWindowPos(this,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);	
		bLockAct=false;
		//当从图形输入窗口点击本窗口列表框切换到本窗口时，当出现消息框时下句易产生致命错误
		//UpdateRecordset();
	}
	CDialog::OnActivate(nState,pWndOther,bMinimized);
}
void CFrmPhsSamp::UpdateRecordset()
{
	::CWaitCursor curWait;
	GetFilterStr();
	mnuDWGM_Click(SelectItemMenu);
	//	this->m_popMenu.GetSubMenu(1)->CheckMenuRadioItem(0,7,Index,MF_BYPOSITION);
	if(Data1->IsEOF() && Data1->IsBOF())
	{
		m_bCurrentPA=FALSE;
		m_bCurrentSA=FALSE;
		CheckDlgButton(IDC_CHECK_CURENTSA,FALSE);
		CheckDlgButton(IDC_CHECK_CURRENTPA,FALSE);
		GetDlgItem(IDC_CHECK_CURENTSA)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CURRENTPA)->EnableWindow(FALSE);
	}
	else
	{
		UpdateData();
		GetDlgItem(IDC_CHECK_CURENTSA)->EnableWindow(m_bFilter && m_bCurrentZdjhAvSA);
		GetDlgItem(IDC_CHECK_CURRENTPA)->EnableWindow(m_bFilter && m_bCurrentZdjhAvPA);
		//if(!Data1->FindFirst("SampleID="+ ltos(modPHScal::iSelSampleID)))
		//	Data1->MoveFirst();
	}

   //this->Data1_Reposition();
   CString strTmp;
   CString strTmp2;

   COleVariant vTmp;
   /*iTmp=m_comboPA.GetCurSel();
   if(iTmp!=-1)
   {
	   m_comboPA.GetLBText(iTmp,strTmp);
	   if(!Data1.IsEOF() && !Data1.IsBOF())
	   {
		   Data1.GetFieldValue("PA",vTmp);
		   strTmp2=modPHScal::sFindCustomID(vtos(vTmp));
		   if(strTmp2!=strTmp)
			   m_comboPA.SelectString(-1,_T("全部"));
	   }
	   else
			m_comboPA.SelectString(-1,_T("全部"));

   }
   else*/
   if(!m_bIsSelPA)
   {
	   LoadListPA();
	   if(m_comboPA.GetCount()>0)
			m_comboPA.SelectString(-1,_T("全部"));
	   else
			m_comboPA.AddString(_T("全部"));
   }
   /*iTmp=m_comboSA.GetCurSel();
   if(iTmp!=-1)
   {
	   m_comboSA.GetLBText(iTmp,strTmp);
	   if(!Data1.IsEOF() && !Data1.IsBOF())
	   {
		   Data1.GetFieldValue("SA",vTmp);
		   strTmp2=modPHScal::sFindCustomID(vtos(vTmp));
		   if(strTmp2!=strTmp)
			   m_comboSA.SelectString(-1,_T("全部"));
	   }
	   else
			m_comboSA.SelectString(-1,_T("全部"));

   }
   else*/
   if(!m_bIsSelSA)
   {
		LoadListSA();
	   if(m_comboSA.GetCount()>0)
			m_comboSA.SelectString(-1,_T("全部"));
	   else
			m_comboSA.AddString(_T("全部"));
   }

}

void CFrmPhsSamp::OnCheckFilter() 
{
	// TODO: Add your control notification handler code here
	UpdateRecordset();
}

void CFrmPhsSamp::OnSelchangeComboSprnum() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if(m_nSprNum != "0" )
	{
		if(m_nSprNum = ">= 0")
		{
		}			
		else
		{
			m_nCSPRNum="0";
		}
	}
	else
	{
	}
				
	m_bAutoSprNum=FALSE;
	UpdateData(false);
	UpdateRecordset();
}

void CFrmPhsSamp::OnSelchangeComboCsprnum() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if(m_nCSPRNum != "0" && m_nCSPRNum != ">= 0")
	{
		m_nSprNum="0";
	}
	m_bAutoSprNum=FALSE;
	UpdateData(false);
	UpdateRecordset();
}

void CFrmPhsSamp::OnCheckCurrentPA() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if(m_bCurrentPA)
	{
		m_bCurrentZdjhPA=FALSE;
		//m_bCurrentZdjhAvPA=FALSE;
		UpdateData(false);
	}
	m_bIsSelPA=false;
	UpdateRecordset();
}

void CFrmPhsSamp::OnCheckCurrentSA() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if(m_bCurrentSA)
	{
		m_bCurrentZdjhSA=FALSE;
		//m_bCurrentZdjhAvSA=FALSE;
		UpdateData(false);
	}
	m_bIsSelSA=false;
	UpdateRecordset();
}

void CFrmPhsSamp::OnCheckCurrentZdjhPA() 
{
	// TODO: Add your control notification handler code here
	/*UpdateData();
	if(m_bCurrentZdjhPA)
	{
		m_bCurrentPA=FALSE;
		m_bCurrentZdjhAvPA=FALSE;
		UpdateData(false);
	}*/
	m_bIsSelPA=false;
	UpdateRecordset();
}

void CFrmPhsSamp::OnCheckCurrentZdjhSA() 
{
	// TODO: Add your control notification handler code here
	/*UpdateData();
	if(m_bCurrentZdjhSA)
	{
		m_bCurrentSA=FALSE;
		m_bCurrentZdjhAvSA=FALSE;
		UpdateData(false);
	}*/
	m_bIsSelSA=false;
	UpdateRecordset();
}

void CFrmPhsSamp::OnCheckCurrentZdjhAvPA() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if(!m_bCurrentZdjhAvPA)
	{
		m_bCurrentPA=FALSE;
		m_bCurrentZdjhPA=FALSE;
		CheckDlgButton(IDC_CHECK_CURRENTPA,FALSE);
		CheckDlgButton(IDC_CHECK_CURRENTZDJHPA,FALSE);
		GetDlgItem(IDC_CHECK_CURRENTPA)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CURRENTZDJHPA)->EnableWindow(FALSE);
		UpdateData(false);
	}
	else
	{
		GetDlgItem(IDC_CHECK_CURRENTPA)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CURRENTZDJHPA)->EnableWindow(TRUE);
	}
	m_bIsSelPA=false;
	UpdateRecordset();
}

void CFrmPhsSamp::OnCheckCurrentZdjhAvSA() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if(!m_bCurrentZdjhAvSA)
	{
		m_bCurrentSA=FALSE;
		m_bCurrentZdjhSA=FALSE;
		CheckDlgButton(IDC_CHECK_CURENTSA,FALSE);
		CheckDlgButton(IDC_CHECK_CURRENTZDJHSA,FALSE);
		GetDlgItem(IDC_CHECK_CURENTSA)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CURRENTZDJHSA)->EnableWindow(FALSE);
		UpdateData(false);
	}
	else
	{
		GetDlgItem(IDC_CHECK_CURENTSA)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CURRENTZDJHSA)->EnableWindow(TRUE);
	}
	m_bIsSelSA=false;
	UpdateRecordset();
}

void CFrmPhsSamp::OnCheckAutosprnum() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if(m_bAutoSprNum)
	{
	
	}
	UpdateRecordset();
}

void CFrmPhsSamp::LoadListPA()
{
	CString strSQL;
	CDaoRecordset rs;
	m_bLoadPA=true;
	try
	{
		m_comboPA.ResetContent();
		m_comboPA.AddString(_T("全部"));
		/*strSQL="Select [CustomID] FROM [PictureClipData] WHERE [Index]=" + ltos(iPA);
		if(this->m_bDH )
		{
			if(this->m_bSh)
			{
				strSQL+=" AND ([ParalleledNum] =1 OR [ParalleledNum]=2) ";
			}
			else
			{
				strSQL+=" AND ([ParalleledNum] =2 ) ";
			}
		}
		else
		{
			if(this->m_bSh)
			{
				strSQL+=" AND ([ParalleledNum] =1 ) ";
			}
			else
			{
			}
		}*/
		strSQL="SELECT DISTINCT [PA] FROM [phsStructureName]";
		if(m_strFilter!="")
			strSQL+=" WHERE " + m_strFilter;

		COleVariant vTmp;
		//rs.m_pDatabase=&EDIBgbl::dbSORT;
		rs.m_pDatabase=&EDIBgbl::dbPRJ;
		rs.Open(dbOpenSnapshot,strSQL,dbForwardOnly);
		while(!rs.IsEOF())
		{
			rs.GetFieldValue(0,vTmp);
			CString sTmp=modPHScal::sFindCustomID(vtos(vTmp));
			if (sTmp!="")
				m_comboPA.AddString(sTmp);
			rs.MoveNext();
		}
	}
	catch(CDaoException *e)
	{
		e->ReportError();
		e->Delete();
	}
	m_bLoadPA=false;
}

void CFrmPhsSamp::LoadListSA()
{
	CString strSQL;
	CDaoRecordset rs;
	m_bLoadSA=true;
	try
	{
		m_comboSA.ResetContent();
		m_comboSA.AddString(_T("全部"));
		/*strSQL="Select [CustomID]  FROM [PictureClipData] WHERE [Index]=" + ltos(iSA);
		CString strTmp;
		if(m_bSABEAM)
		{ 
			strTmp.Format(" ClassID = %d " , iSAbeam);
		}
		if(m_bSACANTil)
		{
			if(strTmp=="")
				strTmp.Format(" ClassID = %d " , iSACantilever);
			else
				strTmp +=" OR ClassID = " + ltos(iSACantilever);
		}
		if(m_bSALBRACE)
		{
			if(strTmp=="")
				strTmp.Format(" ClassID = %d " , iSALbrace);
			else
				strTmp +=" OR ClassID = " + ltos(iSALbrace);
		}
		if(m_bSAGCEMENT)
		{
			if(strTmp=="")
				strTmp.Format(" ClassID = %d " , iGCement);
			else
				strTmp +=" OR ClassID = " + ltos(iGCement);
		}
		if(strTmp !="")
		{
			strSQL +=" AND (" + strTmp + ")";
		}*/
		strSQL="SELECT DISTINCT [SA] FROM [phsStructureName] ";
		if(m_strFilter!="")
			strSQL+=" WHERE " + m_strFilter;
		COleVariant vTmp;
		//rs.m_pDatabase=&EDIBgbl::dbSORT;
		rs.m_pDatabase=&EDIBgbl::dbPRJ;
		rs.Open(dbOpenSnapshot,strSQL,dbForwardOnly);
		while(!rs.IsEOF())
		{
			rs.GetFieldValue(0,vTmp);
			CString sTmp=modPHScal::sFindCustomID(vtos(vTmp));
			if (sTmp!="")
				m_comboSA.AddString(sTmp);
			rs.MoveNext();
		}
	}
	catch(CDaoException *e)
	{
		e->ReportError();
		e->Delete();
	}
	m_bLoadSA=false;
}

void CFrmPhsSamp::OnDropdownComboPa() 
{
	// TODO: Add your control notification handler code here
	/*CString strSel;
	int ix;
	m_bLoadPA=true;
	ix=m_comboPA.GetCurSel();
	if(ix!=-1)
		m_comboPA.GetLBText(ix,strSel);
	LoadListPA();
	if(ix==-1)
		m_comboPA.SetCurSel(0);
	else
	{
		m_comboPA.SelectString(-1,strSel);
	}
	m_bLoadPA=false;*/
}

void CFrmPhsSamp::OnDropdownComboSa() 
{
	// TODO: Add your control notification handler code here
	/*CString strSel;
	int ix;
	m_bLoadSA=true;
	ix=m_comboSA.GetCurSel();
	if(ix!=-1)
		m_comboSA.GetLBText(ix,strSel);
	LoadListSA();
	if(ix==-1)
		m_comboSA.SetCurSel(0);
	else
	{
		m_comboSA.SelectString(-1,strSel);
	}
	m_bLoadSA=false;*/
	
}

void CFrmPhsSamp::OnSelchangeComboPa() 
{
	// TODO: Add your control notification handler code here
	if(!m_bLoadPA)
	{
		m_bIsSelPA=true;
		UpdateRecordset();
	}
}

void CFrmPhsSamp::OnSelchangeComboSa() 
{
	// TODO: Add your control notification handler code here
	if(!m_bLoadSA)
	{
		m_bIsSelSA=true;
		UpdateRecordset();
	}
}

void CFrmPhsSamp::OnCheckDh() 
{
	// TODO: Add your control notification handler code here
	m_bIsSelPA=false;
	UpdateRecordset();
	
}

void CFrmPhsSamp::OnCheckSh() 
{
	// TODO: Add your control notification handler code here
	m_bIsSelPA=false;
	UpdateRecordset();
}

void CFrmPhsSamp::OnCheckSabeam() 
{
	// TODO: Add your control notification handler code here
	m_bIsSelSA=false;
	UpdateRecordset();
	
}

void CFrmPhsSamp::OnCheckSacant() 
{
	// TODO: Add your control notification handler code here
	m_bIsSelSA=false;
	UpdateRecordset();
}

void CFrmPhsSamp::OnCheckSalbrace() 
{
	// TODO: Add your control notification handler code here
	m_bIsSelSA=false;
	UpdateRecordset();
}

void CFrmPhsSamp::OnCheckSagcement() 
{
	// TODO: Add your control notification handler code here
	m_bIsSelSA=false;
	UpdateRecordset();
}


