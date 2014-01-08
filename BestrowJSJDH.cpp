// BestrowJSJDH.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "BestrowJSJDH.h"
#include "edibgbl.h"
#include "user.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// BestrowJSJDH dialog



BestrowJSJDH::BestrowJSJDH(CWnd* pParent /*=NULL*/)
	: CDialog(BestrowJSJDH::IDD, pParent)
{
	//{{AFX_DATA_INIT(BestrowJSJDH)
	m_AddOrUpdate = -1;
	//}}AFX_DATA_INIT
}


void BestrowJSJDH::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(BestrowJSJDH)
	DDX_Radio(pDX, IDC_RADIO1, m_AddOrUpdate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(BestrowJSJDH, CDialog)
	//{{AFX_MSG_MAP(BestrowJSJDH)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// BestrowJSJDH message handlers

//缩短文件名
CString CutFileName(CString FileName,int Length)
{
	CString FileName1;
	int FindChr=0,FindStrat=0,FindEnd,FileNameLength;
	
	//FileName=BestrowJSJDH::WriteFileName;
	FileName1=FileName;
	FileNameLength=FileName.GetLength();
	if(FileNameLength>Length)
	{
		while(FindChr<Length/2)
		{   FindStrat=+FindChr;
			FindChr=BestrowJSJDH::WriteFileName.Find("\\",FindChr+1);
		} 
        FindEnd=FindStrat;
		while(FileNameLength-FindEnd>Length/2)
		{
			FindEnd=FileName.Find("\\",FindEnd+1);
		}
		
        //FindLength=BestrowJSJDH::WriteFileName.Find("\\",FindLength);
		FileName1=FileName.Left(FindStrat+1);
		FileName1+="~~";
		FileName1+=FileName.Mid(FindEnd);
	}
	return FileName1;

}

CString BestrowJSJDH::WriteFileName=" ";
CString JSJDH_Dlg_FileName;
IDispatch* JSJDH_Dlg_pCon;
BOOL BestrowJSJDH::OnInitDialog() 
{
	
	CDialog::OnInitDialog();
    CString FileName;
    FileName=CutFileName(BestrowJSJDH::WriteFileName,50);
	CEdit *m_Edit=(CEdit *)GetDlgItem(IDC_EDIT1);
	m_Edit->SetWindowText(FileName);
    
	if(m_pGridCtrl==NULL)  // 引用GridCtrl 类
		m_pGridCtrl=new CGridCtrl;
    CRect rect1,rect2,rect3;
    
	this->GetWindowRect(rect1);   
    CButton *m_Bt=(CButton *)GetDlgItem(IDOK);
	m_Bt->GetWindowRect(rect2);
	rect3.left=rect1.left;
	rect3.right=rect1.right-10;
	rect3.top=rect1.bottom/4;
	rect3.bottom=(LONG)(3*rect1.bottom/4.5);
	m_pGridCtrl->Create(rect3,this,100);
	
	m_AddOrUpdate=0;
	SetWindowCenter(this->m_hWnd);
	UpdateData(false);
	_RecordsetPtr rscount;
	CString Sql,SqlCount;
	int count;
	rs.CreateInstance(__uuidof(Recordset));
	rscount.CreateInstance(__uuidof(Recordset));
	SqlCount.Format("SELECT count(*) FROM ZY WHERE VolumeID=%d",EDIBgbl::SelVlmID);
	rscount->Open(_variant_t(SqlCount),JSJDH_Dlg_pCon,adOpenDynamic,adLockOptimistic,adCmdText);
	Sql.Format("SELECT * FROM ZY WHERE VolumeID=%d",EDIBgbl::SelVlmID);
	rs->Open(_variant_t(Sql),JSJDH_Dlg_pCon,adOpenDynamic,adLockOptimistic,adCmdText);

    m_pGridCtrl->SetRedraw(false);
	m_pGridCtrl->SetRedraw(true);
    m_pGridCtrl->SetColumnCount(2);
	m_pGridCtrl->SetRowCount(long(rscount->GetCollect(_variant_t(long(0))))+1);
	m_pGridCtrl->SetFixedRowCount(1);
	m_pGridCtrl->SetFixedColumnCount(1);
	m_pGridCtrl->SetFocusCell(-1,-1);
	m_pGridCtrl->SetSelectedRange(-1,-1,-1,-1);
	
	rscount->Close();
	m_pGridCtrl->SetColumnWidth(0,100);
     
    m_pGridCtrl->SetColumnWidth(1,rect3.right-100-10);
	GV_ITEM Item;
	Item.mask=GVIF_TEXT|GVIF_FORMAT;
	Item.nFormat = DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX;
	Item.row=0;
	Item.col=0;
	Item.strText.Format("管系文件序号");
	m_pGridCtrl->SetItem(&Item);
	Item.row=0;
	Item.col=1;
	Item.strText.Format("管系对应的应力分析结果数据文件");
	
	m_pGridCtrl->SetItem(&Item);
	count=1;
	while(!rs->adoEOF)
	{
		Item.col=0;
		Item.row=count;
        Item.strText.Format("%d",count);
		m_pGridCtrl->SetItem(&Item);
        Item.col=1;
		Item.strText.Format(vtos(rs->GetCollect("FileName")));
		m_pGridCtrl->SetItem(&Item);
		rs->MoveNext();
		count++;
	}
	
    m_pGridCtrl->EnableWindow(false);
    
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void BestrowJSJDH::OnOK() 
{
	if(m_AddOrUpdate==1)
	{
		CCellRange CRange=m_pGridCtrl->GetSelectedCellRange();
		JSJDH_Dlg_FileName=m_pGridCtrl->GetItemText(CRange.GetMinRow(),1);
        rs->Filter=_variant_t(CString("FileName='"+JSJDH_Dlg_FileName+"'"));
		rs->PutCollect("FileName",_variant_t(WriteFileName));
		rs->Update();
		rs->Close();
        JSJDH_Dlg_FileName=WriteFileName;
	}
	else
        JSJDH_Dlg_FileName=" ";
	CDialog::OnOK();
}

void BestrowJSJDH::OnRadio1() 
{
	m_pGridCtrl->SetFocusCell(-1,-1);
	m_pGridCtrl->SetSelectedRange(-1,-1,-1,-1);
	
	m_pGridCtrl->EnableWindow(false);

	UpdateData(true);
}

void BestrowJSJDH::OnRadio2() 
{	
  	CString FileName;
	m_pGridCtrl->EnableWindow(true);
	UpdateData(true);
   	   for(int i=1;i<m_pGridCtrl->GetRowCount();i++)
				{
		            FileName=m_pGridCtrl->GetItemText(i,1);
					if(FileName==JSJDH_Dlg_FileName)
					{
			         
						m_pGridCtrl->SetScrollPos(SB_VERT,(i-6)*m_pGridCtrl->GetRowHeight(i));
						m_pGridCtrl->SetFocusCell(i,0);
					    m_pGridCtrl->SetSelectedRange(i,0,i,1,true);
                        
						break;
					}
				}
}

BOOL BestrowJSJDH::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if(m_pGridCtrl && IsWindow(m_pGridCtrl->m_hWnd))
		if(m_pGridCtrl->OnCmdMsg(nID,nCode,pExtra,pHandlerInfo))
		   return true;
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void BestrowJSJDH::OnClose() 
{
    rs->Close();
	JSJDH_Dlg_FileName=" ";
	CDialog::OnClose();
}
