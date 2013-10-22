// FrmBmpSe.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "BmpSeWnd.h"
//#include "CShowBMP.h"
#include "MImage.h"
#include "user.h"
#include "basDirectory.h"
#include "modPHScal.h"
#include "SelTemplate.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CBmpSeWnd dialog


CBmpSeWnd::CBmpSeWnd(CSelTemplate* pSelTemplate,CWnd* pParent /*=NULL*/)
	: CDialog(CBmpSeWnd::IDD, pParent)
	, m_pSelTemplate(pSelTemplate)
{
	//{{AFX_DATA_INIT(CBmpSeWnd)
	m_csList1 = _T("");
	//}}AFX_DATA_INIT
//	m_Image=NULL;
//	m_imgCount=0;
	m_bDisplayFrmData=false;
	m_pDlgImgLst= new CDlgImgScroll(&pSelTemplate->m_lstPart,&m_List1);
}

void CBmpSeWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBmpSeWnd)
	DDX_Control(pDX, IDC_LIST1, m_List1);
	DDX_LBString(pDX, IDC_LIST1, m_csList1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBmpSeWnd, CDialog)
	//{{AFX_MSG_MAP(CBmpSeWnd)
	ON_WM_SHOWWINDOW()
	ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeList1)
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_COMMAND(ID_MEN_DISPRESULT, OnMenDispresult)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBmpSeWnd message handlers

BOOL CBmpSeWnd::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CRect rcc;
	if(user::GetPos("frmBmpSe",rcc))
		this->MoveWindow(&rcc);
	modPHScal::blResizable = true;
	m_pDlgImgLst->Create(this);
	m_pDlgImgLst->ShowWindow(SW_SHOW);
	m_pDlgImgLst->UpdateWindow();
	CRect rc,rc2,rc3;
	this->GetClientRect(&rc);
	m_List1.GetWindowRect(&rc2);
	this->ScreenToClient(&rc2);
	rc3=rc2;
	rc3.top=0;
	rc3.bottom=rc3.top+rc.Height();
	m_List1.MoveWindow(&rc3);
	rc2.left=rc3.right+1;
	rc2.top=0;
	rc2.bottom=rc.Height();
	rc2.right=rc.Width();
	m_pDlgImgLst->MoveWindow(&rc2);
	SetWindowText(GetResStr(IDS_PhsComponentSelect));
	m_popMenu.LoadMenu(IDR_MENU_BMPSEL);
	m_List1.SetBkColor(GetSysColor(COLOR_INFOBK));
	/*LoadAllImage();
   //LoadCaption
   //GetPos Me
   //KeyPreview = True //窗体先接受键盘输入
   OldIndex = -1 ; //为了防止多次进入image1_mouseMove过程
   modPHScal::gsOldPartType = "G";  //为了防止多次进入SelGenbuGuanbu过程
   incX = 0;
   incY = 0;
   
   RefDlgSize();
   //Load frmData to ready display selected data
   CSBmp.CalScrollValue();
   m_popMenu.LoadMenu(IDR_MENU_BMPSEL);
   m_popMenu.GetSubMenu(0)->CheckMenuItem(ID_MEN_DISPRESULT,CSBmp.bDisplayFrmData ? MF_CHECKED : MF_UNCHECKED);
   
   /*if( CSBmp.bDisplayFrmData )
      FrmData.ShowWindow(SW_SHOW);*/
   
   modPHScal::blResizable = false;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CBmpSeWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(message==IMG_CLICKED)
	{
		OnImgClick(wParam);
		return TRUE;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void CBmpSeWnd::OnImgClick(int Index)
{
  /* if (OldIndex == Index )
	   return;
   m_Image[OldIndex].m_bRev=false;
   if(IsWindow(m_Image[OldIndex].m_hWnd))
		m_Image[OldIndex].Invalidate();
   //保存老索引,防止在同一图形多次进入
   OldIndex = Index;
   m_List1.SetCurSel(Index);
   CSBmp.ListIndex(Index);
   CSBmp.CalScrollValue();
   CSBmp.MovePos();
   int i;
  
   /*if (CSBmp.bDisplayFrmData )
   {
      FrmData.ShowWindow(SW_SHOW);
      i = Cavphs->SetPhsTypeToListBox()
      i = Cavphs->SetPhsCheckedTypeToListBox()
      if( IsNull(Cavphs->phsAvailableTypeRs) )
         Set FrmData.rs = Cavphs->phsAvailableTypeRs
	
	}*/
}

void CBmpSeWnd::LoadAllImage()
{
  
  /*int  i = 0;
  int iR,t,IC;
  CString tmp,tmp2,FN;
  m_imgCount=0;
  if(m_Image!=NULL)
  {
	  delete [m_imgCount] m_Image;
	  m_Image=NULL;
	  m_imgCount=0;
  }
  if(CSBmp.BmpRows()*CSBmp.BmpCols()<=0)
  {
	  m_imgCount=1;
	  m_Image=new CMImage[1];
	  m_Image[0].Create(this,ID_IMG);
	  return;
  }
	m_imgCount=CSBmp.BmpRows()*CSBmp.BmpCols();
  m_Image=new CMImage [CSBmp.BmpRows()*CSBmp.BmpCols()];
   for (iR = 0 ;iR<CSBmp.BmpRows();iR++)
   {
	   for (IC = 0 ;IC<CSBmp.BmpCols();IC++)
	   {
		   m_Image[i].Create(this,ID_IMG+i);
		   m_Image[i].m_Index=i;
         if (i < CSBmp.BmpNums() )
            m_Image[i].SetVisible(true);
         else
			 m_Image[i].SetVisible(false);
         
         if (i < CSBmp.BmpNums() )
		 {
            //frmBmpSe!Image1(i).Picture = LoadPicture()
			 if(CSBmp.ListBoxCustomID!=NULL)
			 {
			 CSBmp.ListBoxCustomID->GetText(i,tmp);
			 tmp.TrimLeft();tmp.TrimRight();
			 tmp2 = modPHScal::sFindFLD("CustomID", "BmpName", tmp).pcVal;
				 FN = basDirectory::TemplateDir + tmp + ".bmp";
				m_Image[i].SetImage((char*)(LPCSTR)FN);
			 }
		 }
         i = i + 1;
	   }
   }
*/
}

void CBmpSeWnd::SetHeight(int Height)
{
	CRect rc;
	this->GetWindowRect(&rc);
	rc.bottom=rc.top+Height;
	MoveWindow(&rc);
}

void CBmpSeWnd::SetWidth(int Width)
{
	CRect rc;
	this->GetWindowRect(&rc);
	rc.right=rc.left+Width;
	MoveWindow(&rc);
}

CBmpSeWnd::~CBmpSeWnd()
{
	delete m_pDlgImgLst;
}

void CBmpSeWnd::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	if(bShow)
	{
		//user::SavePos(this,"frmBmpSe");
		//SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
		
	}
	else
	{
		//user::SavePos(this,"FrmListBox");
		//SetWindowPos(&wndNoTopMost,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
	}
	// TODO: Add your message handler code here
	   //Screen.MousePointer = vbDefault
	modPHScal::blResizable = true;
   //计算最大可见列数
/*	CRect rc,rc1,rc2;
	GetWindowRect(&rc);
	m_VScroll.GetWindowRect(&rc1);
	this->ScreenToClient(&rc1);
	m_List1.GetWindowRect(&rc2);
	this->ScreenToClient(&rc2);
	int ScrW=::GetSystemMetrics(SM_CXSCREEN);
   CSBmp.VisibCols((ScrW - (rc.left + rc2.Width() + 10 + rc1.Width())) / m_Image[0].GetWidth());
   CSBmp.SetCtrlSizesInFrmBmpSe();*/
   modPHScal::blResizable = false;
}

void CBmpSeWnd::OnSelchangeList1() 
{
	// TODO: Add your control notification handler code here
	int ix=m_List1.GetCurSel();
//	 CSBmp.ListIndex(ix);
	 if(m_pDlgImgLst->m_pDlgImgLst->m_imgCount > ix)
		m_pDlgImgLst->m_pDlgImgLst->m_Image[ix]->SendMessage(WM_LBUTTONUP);
}

BEGIN_EVENTSINK_MAP(CBmpSeWnd, CDialog)
    //{{AFX_EVENTSINK_MAP(CBmpSeWnd)
	ON_EVENT(CBmpSeWnd, IDC_VSCROLLBAR, 2 /* Change */, OnChangeVscrollbar, VTS_NONE)
	ON_EVENT(CBmpSeWnd, IDC_HSCROLLBAR, 2 /* Change */, OnChangeHscrollbar, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CBmpSeWnd::OnChangeVscrollbar() 
{
	// TODO: Add your control notification handler code here
//	CSBmp.MovePos();
}

void CBmpSeWnd::OnChangeHscrollbar() 
{
	// TODO: Add your control notification handler code here
//	CSBmp.MovePos();
}

void CBmpSeWnd::RefDlgSize()
{
/*	LoadAllImage();
if(CSBmp.bList1Visible)
	   m_List1.ShowWindow(SW_SHOW);
   else
	   m_List1.ShowWindow(SW_HIDE);
   if (CSBmp.bList1Visible )
   {
      CSBmp.ListBoxDescription = &m_List1;
      CSBmp.LoadItemToListBoxDescription();
   }
   m_List1.SetWindowPos(NULL,0,0,0,0,SWP_NOSIZE|SWP_NOZORDER);
	m_HScroll.SetMin(0);
	m_HScroll.SetMax(CSBmp.BmpCols()-CSBmp.VisibCols());
	m_HScroll.SetLargeChange(1);
	m_HScroll.SetSmallChange(1);
	CRect rc,rc2;
	m_List1.GetWindowRect(&rc2);
	rc.left=CSBmp.bList1Visible ? rc2.Width()+10 : 0;
	rc.top=m_Image[0].GetHeight()*CSBmp.VisibRows();
	rc.bottom=rc.top+15;
	rc.right=rc.left+m_Image[0].GetWidth()*CSBmp.VisibCols()+15;
	m_HScroll.MoveWindow(&rc);


	m_VScroll.SetMin(0);
	m_VScroll.SetMax(CSBmp.BmpRows()-CSBmp.VisibRows());
   m_VScroll.SetLargeChange(1);
   m_VScroll.SetSmallChange(1);

   rc.left=(CSBmp.bList1Visible ? rc2.Width()+10 : 0)+m_Image[0].GetWidth() * CSBmp.VisibCols();
   rc.top=0;
   rc.bottom=rc.top+m_Image[0].GetHeight()* CSBmp.VisibRows();
   rc.right=rc.left+15;
   m_VScroll.MoveWindow(&rc);
   m_HScroll.GetWindowRect(&rc);
   this->ScreenToClient(&rc);
   SetHeight(rc.bottom);
   SetWidth(rc.right);
   GetClientRect(&rc);
   m_List1.GetWindowRect(&rc2);
   CRect rc3;
   rc3.right=rc2.Width();
   rc3.left=0;
   rc3.bottom=rc.Height();
   rc3.top=0;
   m_List1.MoveWindow(&rc3);*/
}

//DEL int CBmpSeWnd::incY()
//DEL {
//DEL 
//DEL }

void CBmpSeWnd::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CPoint p=point;
	ClientToScreen(&p);
	m_popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,p.x,p.y,this);
	CDialog::OnRButtonUp(nFlags, point);
}

void CBmpSeWnd::InitList()
{
	CString temp;

	//if(!IsWindow(FrmListBox.m_hWnd))
	//	return;
	//ListBoxCustomID->GetText(0,temp);
    m_List1.ResetContent();
    //CBitmap bitmap;
	//bitmap.LoadBitmap("D:\\lgb\\vc\autoPHS2002\\autophs1.bmp");
	for(int i=0;i<this->m_pSelTemplate->m_lstPart.GetCount();i++)
	{
		m_pSelTemplate->m_lstPart.GetText(i,temp);
		if(temp!="")
			m_List1.AddString(vtos(modPHScal::sFindFLD("CustomID","Description",temp)));
	}

	m_pDlgImgLst->m_pDlgImgLst->LoadAllImage();
	//CRect rc1,rc2,rc3,rc4;
	//int height,width;

}

void CBmpSeWnd::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	CRect rc;
	CRect rc1,rc2,rc3;
//	int height,width;
	/*if(IsWindow(m_pDlgImgLst->m_hWnd) && IsWindow(m_pDlgImgLst->m_pDlgImgLst->m_hWnd))
	{
		GetWindowRect(&rc1);
		m_pDlgImgLst->GetClientRect(&rc2);
		m_pDlgImgLst->m_pDlgImgLst->GetWindowRect(&rc3);
		if(rc2.Height()>rc3.Height())
		{
			height=rc1.Height()-(rc2.Height() - rc3.Height());
		}
		if(rc2.Width()>rc3.Width())
		{
			width=rc1.Width()- (rc2.Width() - rc3.Width());
		}
		this->SetWindowPos(NULL,0,0,width,height,SWP_NOMOVE | SWP_NOZORDER);
	}
*/
	if(IsWindow(m_List1.m_hWnd) && m_List1.IsWindowVisible())
	{
		m_List1.GetWindowRect(&rc);
		this->ScreenToClient(&rc);
		rc2=rc;
		rc2.top=0;
		this->GetClientRect(&rc);
		rc2.bottom=rc.Height();
		m_List1.MoveWindow(&rc2);
	}
	if(IsWindow(m_pDlgImgLst->m_hWnd))
	{
		this->GetClientRect(&rc2);
		if(IsWindow(m_List1.m_hWnd) && m_List1.IsWindowVisible())
		{
			m_List1.GetWindowRect(&rc);
			this->ScreenToClient(&rc);
			rc3.left=rc.right+1;
		}
		else
			rc3.left=0;
		rc3.top=0;
		rc3.right=rc2.Width();
		rc3.bottom=rc2.Height();
		m_pDlgImgLst->MoveWindow(&rc3);
	}
}

void CBmpSeWnd::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	m_pSelTemplate->m_bShowImg=FALSE;
	m_pSelTemplate->UpdateData(false);
	user::SavePos(this,"frmBmpSe");
	CDialog::OnClose();
}

void CBmpSeWnd::OnMenDispresult() 
{
	// TODO: Add your command handler code here
	m_popMenu.GetSubMenu(0)->CheckMenuItem(ID_MEN_DISPRESULT, (m_bDisplayFrmData ? MF_UNCHECKED : MF_CHECKED));
	m_bDisplayFrmData=!m_bDisplayFrmData;
}
