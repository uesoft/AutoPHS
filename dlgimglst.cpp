// DlgImgLst.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "DlgImgLst.h"
#include "modPHScal.h"
#include "basDirectory.h"
#include "math.h"
#include "user.h"
#include "DlgImgScroll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgImgLst dialog


CDlgImgLst::CDlgImgLst(CListBox* pListZDG,CListBox* pListBox)
: m_pListZDG(pListZDG)
, m_pListBox(pListBox)
{
	m_imgCount=0;
	m_ImgIx=-1;
	//{{AFX_DATA_INIT(CDlgImgLst)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


//DEL void CDlgImgLst::DoDataExchange(CDataExchange* pDX)
//DEL {
//DEL 	CDialog::DoDataExchange(pDX);
//DEL 	//{{AFX_DATA_MAP(CDlgImgLst)
//DEL 		// NOTE: the ClassWizard will add DDX and DDV calls here
//DEL 	//}}AFX_DATA_MAP
//DEL }


BEGIN_MESSAGE_MAP(CDlgImgLst, CWnd)
	//{{AFX_MSG_MAP(CDlgImgLst)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgImgLst message handlers

CDlgImgLst::~CDlgImgLst()
{
}

void CDlgImgLst::LoadAllImage()
{
 // On Error Resume Next
  // Dim iR As Integer, IC As Integer, i As Integer, t As Integer, FN As String, tmp As String
	try{
		m_imgCount=m_pListZDG->GetCount();
		m_Image.SetCount(m_imgCount);
		if(m_imgCount<1)
			return;
		CString sTmp,lstText;
		int i ,j;
		for(i=0; i<m_imgCount;i++)
		{
			if(!IsWindow(m_Image[i]->m_hWnd))
				m_Image[i]->Create(this,IMG_ID+i);
			m_Image[i]->m_Index=i;
			m_Image[i]->Dstinvert(false);
			m_pListZDG->GetText(i,lstText);
			lstText.TrimLeft();lstText.TrimRight();
			sTmp=vtos(modPHScal::sFindFLD("CustomID", "BmpName",lstText));
			sTmp=basDirectory::TemplateDir+sTmp+".bmp";
			m_Image[i]->SetImage((LPCTSTR)sTmp);
			//m_Image[i].SetWidth(IMG_W);
			//m_Image[i].SetHeight(IMG_H);
		}
		int Rows=(int)sqrt(m_imgCount);
		int Cols=(int)(m_imgCount/Rows);
		if(m_imgCount%Rows > 0)
			Cols++;
		int k=0;
		CRect rc;
		rc.left=0;
		rc.top=0;
		rc.right=Cols*IMG_W;
		rc.bottom=Rows*IMG_H;
		this->MoveWindow(&rc);
		this->UpdateWindow();
		for(i=0;i<Rows;i++)
			for(j=0;j<Cols;j++)
			{
				if(k>m_imgCount-1)
					break;
				m_Image[k]->SetLeft(j*IMG_W);
				m_Image[k]->SetTop(i*IMG_H);
				m_Image[k]->ShowWindow(SW_SHOW);
				m_Image[k]->Invalidate();
				k++;
			}
		k=m_pListZDG->GetCurSel();
		CDlgImgScroll * pWnd=(CDlgImgScroll*)this->GetParent();
		if(pWnd!=NULL)
		{
			pWnd->m_nScrWidth=rc.Width();
			pWnd->m_nScrHeight=rc.Height();
			pWnd->m_nScrColWidth=IMG_W;
			pWnd->m_nScrLineHeight=IMG_H;
			pWnd->SetScrollRange(SB_VERT,0,rc.Height(),FALSE);
			pWnd->SetScrollRange(SB_HORZ,0,rc.Width(),FALSE);
		}
		if(k>=0)
		{
			m_ImgIx=k;
			m_Image[k]->SendMessage(WM_LBUTTONUP);
		}

	}
	catch(...)
	{
		//e->ReportError();
		e->Delete();
	}
  /*int  i = 0;
  int iR,t,IC;
  CString tmp,tmp2,FN;
  m_imgCount=0;
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
   }*/
}

LRESULT CDlgImgLst::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(message==IMG_CLICKED)
		OnImgClick(wParam);
	/*if(message==WM_COMMAND && (wParam==2 || wParam==1))
	{
		return TRUE;
	}*/
	return CWnd::WindowProc(message, wParam, lParam);
}

void CDlgImgLst::OnImgClick(int Index)
{
	if(m_ImgIx!=Index)
	{
		if(Index>=0 && Index<m_imgCount)
		{
			if(m_ImgIx>=0 && m_ImgIx<m_imgCount)
			{
				m_Image[m_ImgIx]->Dstinvert(false);
			}

		}
		m_ImgIx=Index;
	}
	int c=m_pListBox->GetCount();
	if(Index>=0 && Index<c)
		m_pListBox->SetCurSel(Index);
	else
		m_pListBox->SetCurSel(-1);
	c=m_pListZDG->GetCount();
	if(Index>=0 && Index<c)
		m_pListZDG->SetCurSel(Index);
	else
		m_pListZDG->SetCurSel(-1);
	
	CWnd* pWnd=this->GetParent();
	if(pWnd!=NULL)
		pWnd->SendMessage(IMG_CLICKED,Index);
}


//DEL HBRUSH CDlgImgLst::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
//DEL {
//DEL 	//HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
//DEL 	
//DEL 	// TODO: Change any attributes of the DC here
//DEL 	if(nCtlColor==CTLCOLOR_DLG)
//DEL 	{
//DEL 	m_bkbr.DeleteObject();
//DEL 	m_bkbr.CreateSolidBrush(GetSysColor(COLOR_INFOBK));
//DEL 	// TODO: Return a different brush if the default is not desired
//DEL 	return (HBRUSH)m_bkbr;
//DEL 	}
//DEL 	return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
//DEL }

BOOL CDlgImgLst::Create(CWnd* pParentWnd) 
{
	// TODO: Add your specialized code here and/or call the base class
	CString lpszClassName=::AfxRegisterWndClass( 0,::LoadCursor(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_ARROW)),
									(HBRUSH)COLOR_GRAYTEXT,NULL);
	return CWnd::CreateEx( 0,lpszClassName, _T(""), WS_CHILD,
								CW_USEDEFAULT,CW_USEDEFAULT,
								CW_USEDEFAULT,CW_USEDEFAULT,
								pParentWnd->GetSafeHwnd() , NULL);
}
