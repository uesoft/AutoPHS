// FrmListBox.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "FrmListBox.h"
#include "Cphs.h"
#include "modPHScal.h"
#include "FrmPhsSamp.h"
#include "EDIBgbl.h"
#include "user.h"
#include "PhsData.h"
//#include "CshowBMP.h"
#include "FrmBmpSe.h"
#include "FrmSelSpecification.h"
#include <math.h>
#include "FrmTxsr.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFrmListBox dialog

CFrmListBox FrmListBox;
bool bLockAct=false;
CFrmListBox::CFrmListBox(CWnd* pParent /*=NULL*/)
	: CDialog(CFrmListBox::IDD, pParent)
{
	CheckCondition[5] = TRUE;
	CheckCondition[4] = FALSE;
	CheckCondition[0] = FALSE;
	CheckCondition[1] = FALSE;
	CheckCondition[2] = FALSE;
	CheckCondition[3] = FALSE;
	CheckID[0]=IDC_CHECK_PJG;
	CheckID[1]=IDC_CHECK_TJ;
	CheckID[2]=IDC_CHECK_GDW1;
	CheckID[3]=IDC_CHECK_CNT;
	CheckID[4]=IDC_CHECK_DW;
	CheckID[5]=IDC_CHECK_ONLYPAH;
	//{{AFX_DATA_INIT(CFrmListBox)
	m_bBmpShow = TRUE;
	m_bPA = FALSE;
	//}}AFX_DATA_INIT
	m_OldIxZDG=-1;
	m_OldIxEDIT=-1;
	m_bRefImg=true;
}

/*iChkBoxPJG=0,
   iChkBoxTJ=1,
   iChkBoxGDW1=2,
   iChkBoxCNT=3,
   iChkBoxDW=4,
   iChkBoxOnlyPAh=5 //只选择吊架管部*/
void CFrmListBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_ONLYPAH, CheckCondition[iChkBoxOnlyPAh]);
	DDX_Check(pDX, IDC_CHECK_DW, CheckCondition[iChkBoxDW]);
	DDX_Check(pDX, IDC_CHECK_PJG, CheckCondition[iChkBoxPJG]);
	DDX_Check(pDX, IDC_CHECK_TJ, CheckCondition[iChkBoxTJ]);
	DDX_Check(pDX, IDC_CHECK_GDW1, CheckCondition[iChkBoxGDW1]);
	DDX_Check(pDX, IDC_CHECK_CNT, CheckCondition[iChkBoxCNT]);
	//{{AFX_DATA_MAP(CFrmListBox)
	DDX_Control(pDX, IDC_CHECK_PA, m_CheckPA);
	DDX_Control(pDX, IDC_LIST2, m_ListZDG);
	DDX_Control(pDX, IDC_LIST1, m_ListPhsStruEDIT);
	DDX_Check(pDX, IDC_CHECK_BMP, m_bBmpShow);
	DDX_Check(pDX, IDC_CHECK_PA, m_bPA);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFrmListBox, CDialog)
	//{{AFX_MSG_MAP(CFrmListBox)
	ON_WM_ACTIVATE()
	ON_BN_CLICKED(IDC_CMD_ADD, OnCmdAdd)
	ON_BN_CLICKED(IDC_CMD_APPLY, OnCmdApply)
	ON_BN_CLICKED(IDC_CMD_DEL, OnCmdDel)
	ON_BN_CLICKED(IDC_CMD_SAVE, OnCmdSave)
	ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeList1)
	ON_LBN_SELCHANGE(IDC_LIST2, OnSelchangeList2)
	ON_BN_CLICKED(IDC_CHECK_PA, OnCheckPA)
	ON_BN_CLICKED(IDC_CHECK_BMP, OnCheckBmp)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	ON_LBN_DBLCLK(IDC_LIST2, OnDblclkListZDG)
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_CHECK_DW, OnCheck6)
	ON_BN_CLICKED(IDC_CHECK_PJG, OnCheck6)
	ON_BN_CLICKED(IDC_CHECK_TJ, OnCheck6)
	ON_BN_CLICKED(IDC_CHECK_GDW1, OnCheck6)
	ON_BN_CLICKED(IDC_CHECK_CNT, OnCheck6)
	ON_BN_CLICKED(IDC_CHECK_ONLYPAH, OnCheckOnlypah)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFrmListBox message handlers

BOOL CFrmListBox::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CRect rc;
	if(user::GetPos("FrmListBox",rc))
		this->SetWindowPos(NULL,rc.left,rc.top,0,0,SWP_NOSIZE|SWP_NOZORDER);
	// TODO: Add extra initialization here
	   //LoadCaption
   //Debug.Print Me.AutoRedraw
   //无论使用hide再show还是AutoRedraw=true,也不论它们是否同时使用,
   //窗体的显示都存在杂乱现象,而无法立刻显示出完整的窗体
   //Hide
   //AutoRedraw = True
   //GetPos Me
   //InitDlg();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFrmListBox::OnCmdAdd() 
{
	// TODO: Add your control notification handler code here
  /* Dim iOldSelSampleID As Long
   Screen.MousePointer = vbHourglass
   Dim i As Integer, k As Integer, X
   Dim rs As Recordset*/
         //添加之前，立刻复位条件检查框，加快运行速度
         /*CheckCondition[iChkBoxPJG]= vbUnchecked
         CheckCondition[iChkBoxGDW1] = vbUnchecked
         CheckCondition[iChkBoxTJ] = vbUnchecked
         CheckCondition[iChkBoxCNT]= vbUnchecked*/
			UpdateData();
         this->GetDlgItem(IDC_CMD_SAVE)->EnableWindow(true);
         this->GetDlgItem(IDC_CMD_APPLY)->EnableWindow(false);
		 if(m_ListZDG.GetCurSel()<0)
			 m_ListZDG.SetCurSel(0);
		 int ix=m_ListZDG.GetCurSel();
		 CString sTmp;
		 m_ListZDG.GetText(ix,sTmp);
		if(m_ListPhsStruEDIT.GetCurSel()<0)
			 m_ListPhsStruEDIT.AddString(sTmp);
		else
		{
			ix=m_ListPhsStruEDIT.GetCurSel();
			ix++;
			m_ListPhsStruEDIT.InsertString(ix,sTmp);
            m_ListPhsStruEDIT.SetCurSel(ix);
		}
		OnSelchangeList1();
}

void CFrmListBox::OnCmdApply() 
{
	// TODO: Add your control notification handler code here
	try
	{
			this->GetDlgItem(IDC_CMD_SAVE)->EnableWindow(false);
         if (m_ListPhsStruEDIT.GetCount() > 0)
			{
            //检查匹配
            Cavphs->SourceObj = &m_ListPhsStruEDIT;
            Cavphs->ResultObj = &m_ListZDG;
            int i = Cavphs->CheckMatchPhs();
            if( i == -1 )
				{
               //匹配则入库
               //iSelSampleID = Cavphs->CheckDuplicateREFRecordWhenAppend()
               //改名称
               //Cavphs->ChangeNameInphsStructureName
               //更新
               //FrmphsSamp.Form_Load
               //With FrmPhsSamp!Data1.Recordset
               //With Cavphs->AvailableSampleIDrsForphsSamp
                  //MsgBoxFrmPhsSamp.Data1->RecordCount
                  //.FindFirst "SampleID=" & iSelSampleID
					CString sTmp,sTmp2;
					sTmp.Format("%d",modPHScal::iSelSampleID);
						COleVariant v;
                  if (FrmPhsSamp.Data1->FindFirst("SampleID=" +sTmp))
						{
                     //dbSORT.Execute "UPDATE PhsStructureName SET FREQUENCE=FREQUENCE+1 WHERE SampleID=" & iSelSampleID
                     //FrmPhsSamp!Data1.Recordset.Requery
                     //FrmPhsSamp!Data1.Refresh
                     //下面的代码可以马上反映改变，而上面的不行。或者虽反映，但是记录指针发生了变化。
                    
						  FrmPhsSamp.Data1->GetFieldValue("FREQUENCE",v);
							  FrmPhsSamp.Data1->Edit();
							  int vi=vtoi(v);
							  v.lVal=vi+1;
								FrmPhsSamp.Data1->SetFieldValue("FREQUENCE",v);
								FrmPhsSamp.Data1->Update();
						  if (EDIBgbl::SelBillType == EDIBgbl::TZA)
						  {
                     //如果是支吊架原始数据表
                        //MsgBox "支吊架号" & zdjh & " 组装模板号" & iSelSampleID
                        //MsgBox ResolveResString(iUE_ZDJHxAndSampleIDx, "|1", zdjh, "|2", iSelSampleID)
                        sTmp.Format(GetResStr(IDS_ZDJHxAndSampleIDx),"\%d","\%d");
								sTmp2.Format(sTmp,modPHScal::zdjh,modPHScal::iSelSampleID);
							  MessageBox(sTmp2);
							  FrmTxsr.m_pViewTxsr->m_ActiveRs->PutCollect("iSelSampleID",_variant_t((long)modPHScal::iSelSampleID));
							  /*CString strDn,strGn;
							  m_ListPhsStruEDIT.GetText(0,strDn);
							  m_ListPhsStruEDIT.GetText(m_ListPhsStruEDIT.GetCount()-1,strGn);
								FrmTxsr.m_pViewTxsr->m_ActiveRs->PutCollect("dn1",_variant_t(strDn));
								FrmTxsr.m_pViewTxsr->m_ActiveRs->PutCollect("gn1",_variant_t(strGn));*/

							  /*if( EDIBgbl::pbFrmLoadedTxsr )
							  {
                           //Set rs = frmtxsr!Data1.Recordset
                           //frmtxsr.Data1.Edit();
								  frmtxsr.
                           frmtxsr.Data1.SetFieldValue("iSelSampleID",COleVariant((long)modPHScal::iSelSampleID));
                           frmtxsr.Data1.Update();
                        else if (EDIBgbl::pbFrmLoadedphsData )
								{
                           Set rs = PhsData!Databill.Recordset
                           rs.Edit
                           rs.Fields("iSelSampleID") = iSelSampleID
                           rs.Update
                        End If*/
                        //rs.FindFirst "val(Trim(ZDJH))=" & Zdjh
                         Cavphs->SourceObj = &m_ListPhsStruEDIT;
                        i = Cavphs->SavephsStructTorsTmpREF();
						  }
						}
				}
			}
			}
	catch(CException *e)
	{
		e->Delete();
	}
	catch(...)
	{
	}
}

void CFrmListBox::OnCmdDel() 
{
	// TODO: Add your control notification handler code here
	this->GetDlgItem(IDC_CMD_SAVE)->EnableWindow(true);
         this->GetDlgItem(IDC_CMD_APPLY)->EnableWindow(false);

         if( m_ListPhsStruEDIT.GetCount()>0 && m_ListPhsStruEDIT.GetCurSel() >= 0 )
		 {
			 int ix=m_ListPhsStruEDIT.GetCurSel();
            m_ListPhsStruEDIT.DeleteString(ix);
			if(ix>=1)
			{
				ix--;
				m_ListPhsStruEDIT.SetCurSel(ix);
			}
			else if(ix < m_ListPhsStruEDIT.GetCount()-1)
			{
				m_ListPhsStruEDIT.SetCurSel(ix);
			}

			OnSelchangeList1();
		 }
}

void CFrmListBox::OnCmdSave() 
{
	// TODO: Add your control notification handler code here
	AfxGetApp()->BeginWaitCursor();
	 if (m_ListPhsStruEDIT.GetCount() > 0 )
	 {
            //检查匹配
            Cavphs->SourceObj = &m_ListPhsStruEDIT;
            Cavphs->ResultObj = &m_ListZDG;
         int i = Cavphs->CheckMatchPhs();
         if( i == -1)
			{
               //匹配则入库
               //同时设置保存按钮为灰色状态，防止再次点击。
               this->GetDlgItem(IDC_CMD_SAVE)->EnableWindow(false);
					this->GetDlgItem(IDC_CMD_APPLY)->EnableWindow(true);
					modPHScal::iSelSampleID = Cavphs->CheckDuplicateREFRecordWhenAppend();
               m_OldSelSampleID = modPHScal::iSelSampleID;
               //改名称
               CString X = Cavphs->GetPhsAssembleName(m_OldSelSampleID);
               //下面2句容易造成级联事件，导致死机。
               //新增加的模板应该出现在模板窗体中，为此过滤选择项=mnuIALLSamp
               FrmPhsSamp.SelectItemMenu = mnuIALLSamp;
               FrmPhsSamp.Data1->Requery();
					FrmPhsSamp.Data1->FindFirst("SampleID=" + ltos(m_OldSelSampleID));
					FrmPhsSamp.LoadListSelPhs();
               modPHScal::iSelSampleID = m_OldSelSampleID;
			}
         else
            this->GetDlgItem(IDC_CMD_APPLY)->EnableWindow(false);
	 }
	 AfxGetApp()->EndWaitCursor();
}

void CFrmListBox::OnSelchangeList1() 
{
	// TODO: Add your control notification handler code here
	AfxGetApp()->BeginWaitCursor();
	UpdateData();
	 if(m_ListPhsStruEDIT.GetCurSel()<=0 )
	 {
			GetDlgItem(IDC_CHECK_PA)->EnableWindow(true);
		 /*if(m_bPA)
		 {
			//GetDlgItem(IDC_CHECK_ONLYPAH)->EnableWindow(true);
			//for(int i=0;i<6;i++)
			//	GetDlgItem(CheckID[i])->EnableWindow(true);
		 }*/
	 }
	 else
	 {
			GetDlgItem(IDC_CHECK_PA)->EnableWindow(false);
			//GetDlgItem(IDC_CHECK_ONLYPAH)->EnableWindow(false);
			for(int i=0 ;i<6;i++)
			{
				CheckCondition[i]=false;
				GetDlgItem(CheckID[i])->EnableWindow(false);
			}
			UpdateData(false);
	 }
	CalChkCondition();
//      CSBmp.bRestore = true;
    ListZDGInit();
//    if (tmpPreviousIndex = m_ListPhsStruEDIT.GetCurSel())
      //如果索引未改变，则恢复原来的图像
//      CSBmp.bRestore = true;
//    else
//      CSBmp.bRestore = false;
    
//    if( tmpPreviousIndex < 0 || tmpPreviousIndex != m_ListPhsStruEDIT.GetCurSel())
//      CSBmp.PreviousIndex = 0;
    
    if (tmpPreviousIndex < 0 || tmpPreviousIndex != m_ListPhsStruEDIT.GetCurSel())// Then
        tmpPreviousIndex = m_ListPhsStruEDIT.GetCurSel();
		
    UpdateData(false);
    Bmpshow();
	 AfxGetApp()->EndWaitCursor();
    //blResizable = False
    //Screen.MousePointer = vbDefault
}

void CFrmListBox::Bmpshow()
{
//Dim incX, incY, k
   if (! m_bBmpShow )
   {
	   if(IsWindow(frmBmpSe))
		   frmBmpSe.ShowWindow(SW_HIDE);
        return;
   }
   int ix=this->m_ListPhsStruEDIT.GetCurSel();
   frmBmpSe.ShowWindow(SW_SHOW);
   if(ix!=m_OldIxEDIT || m_bRefImg)
   {
	   m_OldIxEDIT=ix;
		frmBmpSe.InitList();
   }
	ix=this->m_ListZDG.GetCurSel();
	if(ix!=m_OldIxZDG || m_bRefImg)
	{
		m_OldIxZDG=ix;
		if(ix>=0 && ix<frmBmpSe.m_pDlgImgLst->m_pDlgImgLst->m_imgCount)
			frmBmpSe.m_pDlgImgLst->m_pDlgImgLst->m_Image[ix]->SendMessage(WM_LBUTTONUP);
	}
	m_bRefImg=false;
   /*CSBmp.ListBoxCustomID = &m_ListZDG;
   CSBmp.VisibRows (1);
   CSBmp.VisibCols (5);
   CSBmp.BmpRows (10);
   CSBmp.BmpCols(10);
   frmBmpSe.SetWindowText(GetResStr(IDS_PhsComponentSelect));
   CSBmp.bList1Visible = true;
   CSBmp.RsObj = FrmTxsr.m_pViewTxsr->m_ActiveRs;
   CSBmp.EntryFrm ="FrmListBox";
   frmBmpSe.ShowWindow(SW_HIDE);
   frmBmpSe.ShowWindow(SW_SHOW);
   //如果改变了选择,则更新图形及列表框
   //frmBmpSe.Form_Load
   if (m_ListZDG.GetCurSel() < 0 && m_ListZDG.GetCount() >= 1)
       m_ListZDG.SetCurSel(0);
   frmBmpSe.m_List1.SetCurSel(m_ListZDG.GetCurSel());
   frmBmpSe.OnSelchangeList1();
   CSBmp.PreviousIndex = m_ListZDG.GetCurSel() - 500;*/
}

void CFrmListBox::ListZDGInit()
{
	int i,k;
	UpdateData();
	CalChkCondition();
   Cavphs->iSAPAConnect = 10;
   m_ListZDG.ShowWindow(SW_SHOW);
   CString sTmp;
   if( m_ListPhsStruEDIT.GetCurSel() <= 0 ){
      //结构框中无内容，则需要重新选择管部。
      //m_bPA=true;
	   m_CheckPA.EnableWindow();
	   this->GetDlgItem(IDC_CHECK_ONLYPAH)->EnableWindow();
   }
   else
   {
      m_bPA=false;
	  CheckDlgButton(IDC_CHECK_PA,BST_UNCHECKED);
      m_CheckPA.EnableWindow(false);
	   this->GetDlgItem(IDC_CHECK_ONLYPAH)->EnableWindow(false);
   }
   if( m_ListPhsStruEDIT.GetCurSel() < 0 )
   {
      Cavphs->ResultObj = &m_ListZDG;
      Cavphs->SourceObj = &m_ListPhsStruEDIT;
      Cavphs->iChkCondition = iChkCondition;
      Cavphs->iSAPAConnect = iPA;
      Cavphs->Cntb = "";
      i = Cavphs->SetPhsTypeToListBox();
      if( iChkCondition != 0 ) i = Cavphs->SetPhsCheckedTypeToListBox();
	  m_ListPhsStruEDIT.GetText(0,sTmp);
	  k=m_ListZDG.FindStringExact(-1,sTmp);
	  if(k>=0)
		  m_ListZDG.SetCurSel(k);
      //k = CheckListBoxItem(&m_ListZDG, sTmp)
      //提示改换规范再试
      if( m_ListZDG.GetCount() <= 0 ){
		  sTmp.Format(GetResStr(IDS_PleaseChangeOtherSPECretry),modPHScal::gsPhsPASel);

		  if( MessageBox(sTmp,NULL,MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON1|MB_TOPMOST)==IDYES)
			{
			  //FrmSelSpecification.DoModal();
			  ShowModalSelSpecification(FALSE);
			}
	  }
	}
  else if( m_ListPhsStruEDIT.GetCurSel() >= 0 )
  {
	  m_ListPhsStruEDIT.GetText(0,sTmp);
	Cavphs->Cntb = sTmp;
	if(!Cavphs->bFirstPartIsPA() || (m_ListPhsStruEDIT.GetCurSel() == 0 && Cavphs->bFirstPartIsPA() && m_bPA==true) )
	{
     Cavphs->ResultObj = &m_ListZDG;
     Cavphs->SourceObj = &m_ListPhsStruEDIT;
     Cavphs->iChkCondition = iChkCondition;
     Cavphs->iSAPAConnect = iPA;
     Cavphs->Cntb = "";
     i = Cavphs->SetPhsTypeToListBox();
     if( iChkCondition != 0 ) i = Cavphs->SetPhsCheckedTypeToListBox();
			m_ListPhsStruEDIT.GetText(m_ListPhsStruEDIT.GetCurSel()+1,sTmp);
		 //k = CheckListBoxItem(&m_ListZDG, sTmp);
			 k=m_ListZDG.FindStringExact(-1,sTmp);
			 if(k>=0)
				m_ListZDG.SetCurSel(k);
     //k = CheckListBoxItem(&m_ListZDG, sTmp)
	  //提示改换规范再试
	   if( m_ListZDG.GetCount() <= 0 )
	   {
		  sTmp.Format(GetResStr(IDS_PleaseChangeOtherSPECretry),modPHScal::gsPhsPASel);
			  if( MessageBox(sTmp,NULL,MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON1|MB_TOPMOST)==IDYES)
				  //FrmSelSpecification.DoModal();
				  ShowModalSelSpecification(FALSE);
     
	   }
	}
	  else
	  {
		 if( m_ListPhsStruEDIT.GetCurSel() >= 1 )
		 {
			 m_ListPhsStruEDIT.GetText(m_ListPhsStruEDIT.GetCurSel()-1,sTmp);
			Cavphs->CntbP =sTmp;
		 }else
			Cavphs->CntbP = "";
		//UpdateData();
		 int ix=m_ListPhsStruEDIT.GetCurSel();
		 CString sTmp;
		 m_ListPhsStruEDIT.GetText(ix,sTmp);
		 Cavphs->Cntb =sTmp;
		 Cavphs->ResultObj = &m_ListZDG;
		 Cavphs->SourceObj = &m_ListPhsStruEDIT;
		 Cavphs->iChkCondition = iChkCondition;
		 Cavphs->SetPhsTypeToListBox();
		 if( iChkCondition != 0 )
			 Cavphs->SetPhsCheckedTypeToListBox();
		m_ListPhsStruEDIT.GetText(m_ListPhsStruEDIT.GetCurSel()+1,sTmp);
		 //k = CheckListBoxItem(&m_ListZDG, sTmp);
		k=m_ListZDG.FindStringExact(-1,sTmp);
		if(k>=0)
			m_ListZDG.SetCurSel(k);
		 //}
	  }
}
if( m_ListPhsStruEDIT.GetCount() > 0 )
	this->GetDlgItem(IDC_CMD_DEL)->EnableWindow(true);
else
  this->GetDlgItem(IDC_CMD_DEL)->EnableWindow(false);

if( m_ListZDG.GetCount() > 0 )
  this->GetDlgItem(IDC_CMD_ADD)->EnableWindow(true);
else
  this->GetDlgItem(IDC_CMD_ADD)->EnableWindow(false);
//	UpdateData(false);
}

void CFrmListBox::CalChkCondition()
{
   iChkCondition = 0;
   int tmp=0;
   for(int i=0;i<=5;i++)
   {
		if(!CheckCondition[i])
			tmp=0;
		else tmp=1;
		//if(!this->GetDlgItem(CheckID[i])->IsWindowEnabled())
		//	tmp=2;
      iChkCondition = iChkCondition + tmp * (int)pow(2 , i);
	}
  
}

void CFrmListBox::OnSelchangeList2() 
{
	// TODO: Add your control notification handler code here
	//CSBmp.ListIndex (m_ListZDG.GetCurSel());
	UpdateData();
	if(m_bBmpShow)
	{
		int ix=this->m_ListZDG.GetCurSel();
		if(ix!=m_OldIxZDG)
		{
			m_OldIxZDG=ix;
			if(ix>=0 && ix<frmBmpSe.m_pDlgImgLst->m_pDlgImgLst->m_imgCount)
				frmBmpSe.m_pDlgImgLst->m_pDlgImgLst->m_Image[ix]->SendMessage(WM_LBUTTONUP);
		}
	}
}

void CFrmListBox::OnCheckPA() 
{
	// TODO: Add your control notification handler code here
	UpdateData(); 
	if(m_bPA == true)
	{
		for(int i=0;i<6;i++)
			GetDlgItem(CheckID[i])->EnableWindow(true);
      CheckCondition[iChkBoxOnlyPAh]=true;
		CheckCondition[iChkBoxDW]=true;
		CheckCondition[iChkBoxPJG]=true;
		CheckCondition[iChkBoxTJ]=true;
		CheckCondition[iChkBoxGDW1]=false;
		CheckCondition[iChkBoxCNT]=false;
	}
	else{
      //GetDlgItem(CheckID[iChkBoxOnlyPAh])->EnableWindow(false);
      CheckCondition[iChkBoxOnlyPAh]=false;
      CheckCondition[iChkBoxOnlyPAh]=false;
		CheckCondition[iChkBoxDW]=false;
		CheckCondition[iChkBoxPJG]=false;
		CheckCondition[iChkBoxTJ]=false;
		CheckCondition[iChkBoxGDW1]=false;
		CheckCondition[iChkBoxCNT]=false;
		for(int i=0;i<6;i++)
			GetDlgItem(CheckID[i])->EnableWindow(false);
	}
	UpdateData(false);
   //如果frmListBox窗体中的ListPhsStruEDIT.ListIndex发生改变，
   //则重新设置CSBmp.PreviousIndex=0，以防止错误恢复图片。
   tmpPreviousIndex = -1;
	m_OldIxEDIT=-1;
	m_OldIxZDG=-1;
	m_bRefImg=true;
   OnSelchangeList1();
   UpdateData(false);
}

void CFrmListBox::OnCheckBmp() 
{
/*	UpdateData();
    if (tmpPreviousIndex == m_ListPhsStruEDIT.GetCurSel())
      //如果索引未改变，则恢复原来的图像
      CSBmp.bRestore = true;
    else
      CSBmp.bRestore = false;
    
    if (tmpPreviousIndex < 0 || tmpPreviousIndex != m_ListPhsStruEDIT.GetCurSel())
      CSBmp.PreviousIndex = 0;
    */
	UpdateData();
   Bmpshow();
   //UpdateData(false);
}
void CFrmListBox::OnActivate(UINT nState, CWnd *pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState,pWndOther,bMinimized);
	if(!bLockAct && nState==WA_CLICKACTIVE)
	{
		bLockAct=true;
		if(m_bBmpShow)
			::frmBmpSe.SetWindowPos(this,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
		//	frmBmpSe.ShowWindow(SW_SHOWNOACTIVATE);
		//FrmPhsSamp.ShowWindow(SW_SHOWNORMAL);
		::FrmPhsSamp.SetWindowPos(this,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
		//ShowWindow(SW_SHOWNORMAL);
		//SetWindowPos(&wndTop,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
		//this->FlashWindow(FALSE);
		bLockAct=false;
	}
	m_bRefImg=true;
}
void CFrmListBox::OnButton3() 
{
	// TODO: Add your control notification handler code here
	m_bRefImg=true;
	OnSelchangeList1();
}

void CFrmListBox::OnDblclkListZDG() 
{
	// TODO: Add your control notification handler code here
	OnCmdAdd();
}

void CFrmListBox::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	if(IsWindow(frmBmpSe.m_hWnd))
		frmBmpSe.ShowWindow(SW_HIDE);
	if(IsWindow(FrmPhsSamp.m_hWnd))
		FrmPhsSamp.ShowWindow(SW_HIDE);
	user::SavePos(this,"FrmListBox");
	CDialog::OnClose();
}

void CFrmListBox::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	if(bShow==FALSE)
	{		
		//SetWindowPos(&wndNoTopMost,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);

		if(IsWindow(frmBmpSe.m_hWnd))
			frmBmpSe.ShowWindow(SW_HIDE);
		if(IsWindow(FrmPhsSamp.m_hWnd))
			FrmPhsSamp.ShowWindow(SW_HIDE);
		//user::SavePos(this,"FrmListBox");
	}

}

void CFrmListBox::OnCheck6() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	m_bRefImg=true;
	CalChkCondition();
}

void CFrmListBox::InitDlg()
{
	m_bBmpShow =true;
   m_ListZDG.ShowWindow(SW_HIDE);
	this->GetDlgItem(IDC_CHECK_ONLYPAH)->EnableWindow(false);
   //CheckCondition[iChkBoxOnlyPAh].Enabled = False
   //CheckCondition[iChkBoxOnlyPAh]=true;
	for(int i=0 ;i<6;i++)
			{
				CheckCondition[i]=false;
				GetDlgItem(CheckID[i])->EnableWindow(false);
			}
   //此处设置状态无用，应该在ListZDGinit中设置。
   //初始保存状态为False
	this->GetDlgItem(IDC_CMD_SAVE)->EnableWindow(false);
   //Show
   //如果frmListBox窗体中的ListPhsStruEDIT.ListIndex发生改变，
   //则重新设置CSBmp.PreviousIndex=0，以防止错误恢复图片。
   tmpPreviousIndex = -1;
   Cavphs->InitListRs();
//	ListZDGInit();

   UpdateData(false);
}

void CFrmListBox::OnCheckOnlypah() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	m_bRefImg=true;
	/*if(m_bPA==false)
	{
		CheckCondition[iChkBoxDW]=CheckCondition[iChkBoxOnlyPAh];
		CheckCondition[iChkBoxPJG]=CheckCondition[iChkBoxOnlyPAh];
		CheckCondition[iChkBoxTJ]=CheckCondition[iChkBoxOnlyPAh];
		if(CheckCondition[iChkBoxOnlyPAh])
		{
			CheckCondition[iChkBoxGDW1]=false;
			CheckCondition[iChkBoxCNT]=false;
		}
	}
	UpdateData(false);*/
	OnSelchangeList1();
}
