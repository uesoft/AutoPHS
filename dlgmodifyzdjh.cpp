// DlgModifyZdjh.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "DlgModifyZdjh.h"
#include "EDIBgbl.h"
#include "user.h"
#include "FrmTxsr.h"
#include "PhsData.h"
#include "EDIBDB.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgModifyZdjh dialog
typedef struct _tagListSort{
	BOOL bAsc;
	int iColIndex;
}tagListSort,FAR * LPListSort;
int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, 
   LPARAM lParamSort);

CDlgModifyZdjh::CDlgModifyZdjh(long iVlmID,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgModifyZdjh::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgModifyZdjh)
	m_strLab1 = _T("");
	m_strLab2 = _T("");
	//}}AFX_DATA_INIT
	m_iVlmID=iVlmID;
	m_iCurSortCol=-1;
}


void CDlgModifyZdjh::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgModifyZdjh)
	DDX_Control(pDX, IDC_LIST2, m_list2);
	DDX_Control(pDX, IDC_LIST1, m_list1);
	DDX_Text(pDX, IDC_LAB1, m_strLab1);
	DDX_Text(pDX, IDC_LAB2, m_strLab2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgModifyZdjh, CDialog)
	//{{AFX_MSG_MAP(CDlgModifyZdjh)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_DELETEALL, OnDeleteall)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, OnItemchangedList1)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST2, OnItemchangedList2)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST1, OnColumnclickList1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST2, OnDblclkList2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgModifyZdjh message handlers

BOOL CDlgModifyZdjh::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetWindowCenter(this->GetSafeHwnd());
	m_list1.SetExtendedStyle(m_list1.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP  );
	m_list2.SetExtendedStyle(m_list1.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP);
	m_ImgList.Create(IDB_LISTCTRL_HEAD,8,1,RGB(0,255,0));
	m_list1.GetHeaderCtrl()->SetImageList(&m_ImgList);
	LVCOLUMN col;
	memset(&col,0,sizeof(col));
	col.mask=LVCF_TEXT | LVCF_WIDTH | LVCF_IMAGE|LVCF_FMT|LVCF_SUBITEM   ;
	col.fmt=LVCFMT_LEFT;
	col.cx=90;
	col.pszText=_T("支吊架号");
	col.iImage=0;
	col.iSubItem=0;
	m_list1.InsertColumn(0,&col);

	col.cx=70;
	col.pszText=_T("节点号");
	col.iImage=-1;
	col.iSubItem++;
	m_list1.InsertColumn(1,&col);

	col.pszText=_T("管径");
	col.iSubItem++;
	m_list1.InsertColumn(2,&col);

	col.pszText=_T("温度");
	col.iSubItem++;
	m_list1.InsertColumn(3,&col);


	col.mask=LVCF_TEXT | LVCF_WIDTH|LVCF_FMT|LVCF_SUBITEM ;
	col.fmt=LVCFMT_LEFT ;
	col.cx=70;
	col.pszText=_T("支吊架号");
	col.iSubItem=0;
	m_list2.InsertColumn(0,&col);

	col.cx=60;
	col.pszText=_T("节点号");
	col.iSubItem++;
	m_list2.InsertColumn(1,&col);

	col.pszText=_T("管径");
	col.iSubItem++;
	m_list2.InsertColumn(2,&col);

	col.pszText=_T("温度");
	col.iSubItem++;
	m_list2.InsertColumn(3,&col);

	m_list1SortType[0]=iAsc;
	m_list1SortType[1]=iNone;
	m_list1SortType[2]=iNone;
	m_list1SortType[3]=iNone;
	LoadZdj();

	GetDlgItem(IDC_DELETEALL)->EnableWindow(FALSE);
	GetDlgItem(IDC_DELETE)->EnableWindow(FALSE);

	if(m_list1.GetItemCount()>0)
	{
		SetListCurSel(&m_list1,0);
	}
	else
	{
		GetDlgItem(IDC_ADD)->EnableWindow(FALSE);
	}
	UpdateItemLab();
	m_iCurSortCol=0;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgModifyZdjh::LoadZdj()
{
	CString strSQL;
	try
	{
		strSQL.Format(_T("SELECT ZDJH,JSDBH1,DJ1,T01 FROM [ZA] WHERE VolumeID=%d ORDER BY ZDJH"),
			m_iVlmID);
		_RecordsetPtr rs=conPRJDB->Execute(((LPCTSTR)strSQL),NULL,adCmdText);
		LVITEM item;
		item.mask=LVIF_TEXT;
		item.iItem=0;
		for(;!rs->adoEOF;rs->MoveNext())
		{
			LPListData pListData=new tagListData;
			item.iSubItem=0;
			CString strText=vtos(rs->GetCollect(0L));
			pListData->iZdjh=vtoi(rs->GetCollect(0L));
			item.pszText=(LPTSTR)(LPCTSTR)strText;
			int ix=m_list1.InsertItem(&item);

			m_list1.SetItemData(ix,(DWORD)(LPVOID)pListData);
		
			strText=vtos(rs->GetCollect(1L));
			pListData->iNodeNO=vtoi(rs->GetCollect(1L));
			item.pszText=(LPTSTR)(LPCTSTR)strText;
			item.iSubItem=1;
			m_list1.SetItem(&item);
		
			strText=vtos(rs->GetCollect(2L));
			pListData->fDw=vtof(rs->GetCollect(2L));
			item.pszText=(LPTSTR)(LPCTSTR)strText;
			item.iSubItem=2;
			m_list1.SetItem(&item);
		
			strText=vtos(rs->GetCollect(3L));
			pListData->fTj=vtof(rs->GetCollect(3L));
			item.pszText=(LPTSTR)(LPCTSTR)strText;
			item.iSubItem=3;
			m_list1.SetItem(&item);
			
			item.iItem++;
		}
		rs->Close();
	}
	catch(_com_error & e)
	{
		AfxMessageBox(e.Description());
	}
}



int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, 
   LPARAM lParamSort)
{
	LPListSort pSort=(LPListSort)(LPVOID)lParamSort;
	CDlgModifyZdjh::LPListData pData1,pData2;
	pData1=(CDlgModifyZdjh::LPListData)lParam1;
	pData2=(CDlgModifyZdjh::LPListData)lParam2;
	if(pSort && pData1 && pData2)
	{
		int res=0;
		switch(pSort->iColIndex)
		{
		case 0:
			if(pData1->iZdjh > pData2->iZdjh)
				res= 1; 
			else if(pData1->iZdjh < pData2->iZdjh)
				res =-1;
			else
				res= 0;
			break;
		case 1:
			if(pData1->iNodeNO > pData2->iNodeNO)
				res= 1;
			else if(pData1->iNodeNO < pData2->iNodeNO)
				res= -1;
			else
				res= 0;
			break;
		case 2:
			if(pData1->fDw > pData2->fDw)
				res= 1;
			else if(pData1->fDw < pData2->fDw)
				res= -1;
			else
				res= 0;
			break;
		case 3:
			if(pData1->fTj > pData2->fTj)
				res= 1;
			else if(pData1->fTj < pData2->fTj)
				res= -1;
			else
				res= 0;
			break;

		}
		if(!pSort->bAsc) res=-res;
		return res;
	}
	return 0;
}

void CDlgModifyZdjh::OnDestroy() 
{
	int i,c;
	c=m_list1.GetItemCount();
	LPListData pListData;
	for(i=0;i<c;i++)
	{
		pListData=(LPListData)m_list1.GetItemData(i);
		if(pListData)
			delete pListData;
	}
	c=m_list2.GetItemCount();
	for(i=0;i<c;i++)
	{
		pListData=(LPListData)m_list2.GetItemData(i);
		if(pListData)
			delete pListData;
	}
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

void CDlgModifyZdjh::OnAdd() 
{
	// TODO: Add your control notification handler code here
	POSITION pos=m_list1.GetFirstSelectedItemPosition();
	TCHAR szText[64];
	LVITEM item2,item1;
	item1.mask=item2.mask=LVIF_TEXT;
	item2.iItem=m_list2.GetItemCount();
	item1.pszText=item2.pszText=szText;
	item1.cchTextMax=sizeof(szText)/sizeof(szText[0]);
	int *arrItem=new int[m_list1.GetSelectedCount()];
	int iArrItemCount=0;
	int iFirstSelItem=-1;
	while(pos)
	{
		int iSelItem=m_list1.GetNextSelectedItem(pos);
		arrItem[iArrItemCount++]=iSelItem;
		if(iFirstSelItem==-1) iFirstSelItem=iSelItem;
		LPListData pData=(LPListData)m_list1.GetItemData(iSelItem);
		item1.iItem=iSelItem;

		item1.iSubItem=0;
		m_list1.GetItem(&item1);
		item2.iSubItem=0;
		int ix=m_list2.InsertItem(&item2);
		m_list2.SetItemData(ix,(DWORD)pData);

		item1.iSubItem++;
		m_list1.GetItem(&item1);
		item2.iSubItem++;
		m_list2.SetItem(&item2);

		item1.iSubItem++;
		m_list1.GetItem(&item1);
		item2.iSubItem++;
		m_list2.SetItem(&item2);

		item1.iSubItem++;
		m_list1.GetItem(&item1);
		item2.iSubItem++;
		m_list2.SetItem(&item2);

		item2.iItem++;

	}
	for(int i=iArrItemCount-1;i>=0;i--)
	{
		m_list1.DeleteItem(arrItem[i]);
	}
	delete [] arrItem;
	GetDlgItem(IDC_DELETE)->EnableWindow(TRUE);
	GetDlgItem(IDC_DELETEALL)->EnableWindow(TRUE);
	if(m_list1.GetItemCount()<=0)
	{
		GetDlgItem(IDC_ADD)->EnableWindow(FALSE);
	}
	else
	{
		if(iFirstSelItem<m_list1.GetItemCount())
			SetListCurSel(&m_list1,iFirstSelItem);
		else
			SetListCurSel(&m_list1,m_list1.GetItemCount()-1);
	}
	if(GetListCurSel(&m_list2)==-1)
	{
		SetListCurSel(&m_list2,0);
	}
	ArrangeList2();
	UpdateItemLab();
}

void CDlgModifyZdjh::OnDelete() 
{
	// TODO: Add your control notification handler code here
	POSITION pos=m_list2.GetFirstSelectedItemPosition();
	TCHAR szText[64];
	LVITEM item2,item1;
	item1.mask=item2.mask=LVIF_TEXT;
	item1.iItem=m_list1.GetItemCount();
	item1.pszText=item2.pszText=szText;
	item2.cchTextMax=sizeof(szText)/sizeof(szText[0]);
	int *arrItem=new int[m_list2.GetSelectedCount()];
	int iArrItemCount=0;
	int iFirstSelItem=-1;
	while(pos)
	{
		int iSelItem=m_list2.GetNextSelectedItem(pos);
		arrItem[iArrItemCount++]=iSelItem;
		if(iFirstSelItem==-1)iFirstSelItem=iSelItem;
		LPListData pData=(LPListData)m_list2.GetItemData(iSelItem);
		item2.iItem=iSelItem;

		item2.iSubItem=0;
		m_list2.GetItem(&item2);
		_stprintf(szText,_T("%d"),pData->iZdjh);
		item1.iSubItem=0;
		int ix=m_list1.InsertItem(&item1);
		m_list1.SetItemData(ix,(DWORD)pData);

		item2.iSubItem++;
		m_list2.GetItem(&item2);
		item1.iSubItem++;
		m_list1.SetItem(&item1);

		item2.iSubItem++;
		m_list2.GetItem(&item2);
		item1.iSubItem++;
		m_list1.SetItem(&item1);

		item2.iSubItem++;
		m_list2.GetItem(&item2);
		item1.iSubItem++;
		m_list1.SetItem(&item1);

		item1.iItem++;

	}	
	for(int i=iArrItemCount-1;i>=0;i--)
	{
		m_list2.DeleteItem(arrItem[i]);
	}
	delete [] arrItem;
	SortList1(m_list1SortType[m_iCurSortCol],m_iCurSortCol);

	GetDlgItem(IDC_ADD)->EnableWindow(TRUE);
	if(m_list2.GetItemCount()<=0) 
	{
		GetDlgItem(IDC_DELETEALL)->EnableWindow(FALSE);
		GetDlgItem(IDC_DELETE)->EnableWindow(FALSE);
	}
	else
	{
		if(iFirstSelItem<m_list2.GetItemCount())
			SetListCurSel(&m_list2,iFirstSelItem);
		else
			SetListCurSel(&m_list2,m_list2.GetItemCount()-1);
	}
	if(GetListCurSel(&m_list1)==-1)
	{
		SetListCurSel(&m_list1,0);
	}
	ArrangeList2();
	UpdateItemLab();
}

void CDlgModifyZdjh::OnDeleteall() 
{
	// TODO: Add your control notification handler code hereint
	int i,c;
	c=m_list2.GetItemCount();
	TCHAR szText[64];
	LVITEM item2,item1;
	item1.mask=item2.mask=LVIF_TEXT;
	item1.iItem=m_list1.GetItemCount();
	item1.pszText=item2.pszText=szText;
	item2.cchTextMax=sizeof(szText)/sizeof(szText[0]);
	for(i=0;i<c;i++)
	{
		LPListData pData=(LPListData)m_list2.GetItemData(i);
		item2.iItem=i;

		item2.iSubItem=0;
		m_list2.GetItem(&item2);
		item1.iSubItem=0;
		_stprintf(szText,_T("%d"),pData->iZdjh);
		int ix=m_list1.InsertItem(&item1);
		m_list1.SetItemData(ix,(DWORD)pData);

		item2.iSubItem++;
		m_list2.GetItem(&item2);
		item1.iSubItem++;
		m_list1.SetItem(&item1);

		item2.iSubItem++;
		m_list2.GetItem(&item2);
		item1.iSubItem++;
		m_list1.SetItem(&item1);

		item2.iSubItem++;
		m_list2.GetItem(&item2);
		item1.iSubItem++;
		m_list1.SetItem(&item1);

		item1.iItem++;
	}	
	m_list2.DeleteAllItems();

	SortList1(m_list1SortType[m_iCurSortCol],m_iCurSortCol);

	GetDlgItem(IDC_DELETEALL)->EnableWindow(FALSE);
	GetDlgItem(IDC_DELETE)->EnableWindow(FALSE);
	GetDlgItem(IDC_ADD)->EnableWindow(TRUE);

	if(GetListCurSel(&m_list1)==-1)
	{
		SetListCurSel(&m_list1,0);
	}
	UpdateItemLab();
}

void CDlgModifyZdjh::SortList1(int iSortType, int iCol)
{
	tagListSort listsort;
	if(iSortType==iAsc)
		listsort.bAsc=TRUE;
	else
		listsort.bAsc=FALSE;
	listsort.iColIndex=iCol;
	m_list1SortType[iCol]=(enumSortType)iSortType;
	int i;
	for(i=0;i<4;i++)
	{
		if(i!=iCol)
		{
			m_list1SortType[i]=iNone;
		}
		LVCOLUMN col;
		col.mask=LVCF_IMAGE ;
		if(m_list1SortType[i]==iNone)
			col.iImage=-1;
		else if(m_list1SortType[i]==iAsc)
			col.iImage=0;
		else
			col.iImage=1;
		m_list1.SetColumn(i,&col);
	}
	m_list1.SortItems(CompareFunc,(DWORD)(LPVOID)&listsort);
	m_iCurSortCol=iCol;
	this->UpdateItemLab();
}

void CDlgModifyZdjh::UpdateItemLab()
{
	int iSel=-1;
	int c=m_list1.GetItemCount();
	iSel=GetListCurSel(&m_list1);
	
	m_strLab1.Format(IDS_ITEM_DESCRIBE,iSel+1,c);

	iSel=-1;
	c=m_list2.GetItemCount();
	iSel=GetListCurSel(&m_list2);
	m_strLab2.Format(IDS_ITEM_DESCRIBE,iSel+1,c);
	UpdateData(FALSE);
}

void CDlgModifyZdjh::SetListCurSel(CListCtrl *pList, int iSel)
{
	LVITEM item;
	item.mask=LVIF_STATE;
	item.iItem=0;
	int c=pList->GetItemCount();
	item.iSubItem=0;
	item.stateMask=LVIS_SELECTED ;
	for(;item.iItem<c;item.iItem++)
	{
		if(item.iItem!=iSel)
			item.state=0;
		else
			item.state=LVIS_SELECTED ;
		pList->SetItem(&item);
	}
}

int CDlgModifyZdjh::GetListCurSel(CListCtrl *pList)
{
	return pList->GetNextItem(-1,LVNI_SELECTED);
	
}

void CDlgModifyZdjh::OnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	UpdateItemLab();
	*pResult = 0;
}

void CDlgModifyZdjh::OnItemchangedList2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	UpdateItemLab();
	*pResult = 0;
}

void CDlgModifyZdjh::OnOK() 
{
	// TODO: Add extra validation here
	CWaitCursor wait;
	if(m_list1.GetItemCount()>0)
	{
		MessageBox(_T("错误：请把左边列表框中所有元素按照所需顺序加入右边的列表框中！"),NULL,MB_OK|MB_ICONINFORMATION);
		return;
	}
	_variant_t vSQL;
	BOOL bErr=FALSE; 
	try
	{
		vSQL=FrmTxsr.m_pViewTxsr->m_ActiveRs->GetSource();
		if(FrmTxsr.m_pViewTxsr->m_ActiveRs->State!=adStateClosed)
		{
			if(!FrmTxsr.m_pViewTxsr->m_ActiveRs->adoEOF && !FrmTxsr.m_pViewTxsr->m_ActiveRs->BOF)
			{
				try
				{
					FrmTxsr.m_pViewTxsr->m_ActiveRs->Update();
				}
				catch(CException *e)
				{
					FrmTxsr.m_pViewTxsr->m_ActiveRs->CancelUpdate();
				}
			}
			try
			{
				FrmTxsr.m_pViewTxsr->m_ActiveRs->Close();
			}
			catch(CException *e)
			{
				e->Delete();
			}				
		}
		::FrmPhsData.m_DBGbill.SetRefDataSource(NULL);
		//FrmTxsr.m_pViewTxsr->m_Databill.SetRefRecordset(NULL);
	}
	catch(_com_error & e)
	{
		bErr=TRUE;
		AfxMessageBox(e.Description());
	}
	if(!bErr)
	{
		BOOL bStartTrans=FALSE;
		try
		{
			::conPRJDB->BeginTrans();
			bStartTrans=TRUE;
			_CommandPtr pCmd;
			pCmd.CreateInstance(__uuidof(Command));
			pCmd->ActiveConnection=conPRJDB;
			pCmd->CommandType=adCmdText;
			CString strSQL;
			int iBase=10000;
			strSQL.Format(_T("UPDATE [ZA] SET [ZDJH]=[ZDJH]+%d WHERE VolumeID=%d"),iBase,m_iVlmID);
			pCmd->CommandText=(LPCTSTR)strSQL;
			pCmd->Execute(NULL,NULL,adCmdText);
			int i,c;
			c=m_list2.GetItemCount();
			for(i=0;i<c;i++)
			{
				LPListData pData=(LPListData)m_list2.GetItemData(i);
				strSQL.Format(_T("UPDATE [ZA] SET [ZDJH]=%d WHERE VolumeID=%d AND ZDJH=%d"),i+1,m_iVlmID,pData->iZdjh+iBase);
				pCmd->CommandText=(LPCTSTR)strSQL;
				pCmd->Execute(NULL,NULL,adCmdText);
			}
			pCmd=NULL;
			conPRJDB->CommitTrans();
			bStartTrans=FALSE;
		}
		catch(_com_error & e)
		{
			if(bStartTrans) conPRJDB->RollbackTrans();
			AfxMessageBox(e.Description());
		}
	}
	try
	{
		FrmTxsr.m_pViewTxsr->m_ActiveRs->Open(vSQL,(IDispatch*)conPRJDB,adOpenKeyset,adLockOptimistic,adCmdText);
		FrmTxsr.m_pViewTxsr->m_bAllowUpd=false;
		FrmTxsr.m_pViewTxsr->m_Databill.SetRefRecordset(FrmTxsr.m_pViewTxsr->m_ActiveRs);
		FrmTxsr.m_pViewTxsr->m_Databill.SetEnabled(TRUE);
		FrmPhsData.m_DBGbill.SetRefDataSource(FrmTxsr.m_pViewTxsr->m_ActiveRs);
		EDIBDB::SetColumnsProperty(FrmPhsData.m_DBGbill, EDIBgbl::SelBillType);
		FrmTxsr.m_pViewTxsr->m_bAllowUpd=true;
	}
	catch(_com_error & e)
	{
		AfxMessageBox(e.Description());
	}
	CDialog::OnOK();
}

void CDlgModifyZdjh::ArrangeList2()
{
	int i,c;
	c=m_list2.GetItemCount();
	CString strText;
	for(i=0;i<c;i++)
	{
		LVITEM item;
		item.mask=LVIF_TEXT;
		strText.Format(_T("%d"),i+1);
		item.pszText=(LPTSTR)(LPCTSTR)strText;
		item.iItem=i;
		item.iSubItem=0;
		m_list2.SetItem(&item);
	}
}

void CDlgModifyZdjh::OnColumnclickList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	int colindex=pNMListView->iSubItem;
	if(colindex!=-1 )
	{
		int iOldSortType=m_list1SortType[colindex];
		int iSortType=iAsc;
		if(iOldSortType==iAsc)
			iSortType=iDesc;
		SortList1(iSortType,colindex);
	}
	*pResult = 0;
}

void CDlgModifyZdjh::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	if(GetDlgItem(IDC_ADD)->IsWindowEnabled())
		OnAdd();
	*pResult = 0;
}

void CDlgModifyZdjh::OnDblclkList2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	if(GetDlgItem(IDC_DELETE)->IsWindowEnabled())
		OnDelete();
	*pResult = 0;
}
