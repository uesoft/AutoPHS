// FrmDataEdit.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "FrmDataEdit.h"
#include "FrmTxsr.h"
#include "EDIBAcad.h"
#include "EDIBgbl.h"
#include "EDIBDB.h"
#include "user.h"
#include "Columns.h"
#include "modPhscal.h"
#include "Column.h"
#include "inputbox.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFrmDataEdit dialog

CFrmDataEdit FrmDataEDIT;
CFrmDataEdit::CFrmDataEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CFrmDataEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFrmDataEdit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bAllowUpd=false;
}


void CFrmDataEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFrmDataEdit)
	DDX_Control(pDX, IDC_DATAGRID1, m_DBGrid1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFrmDataEdit, CDialog)
	//{{AFX_MSG_MAP(CFrmDataEdit)
	ON_WM_ACTIVATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFrmDataEdit message handlers

BOOL CFrmDataEdit::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	this->SetWindowText(GetResStr(IDS_FrmDataEDIT_FrmDataEDIT));
	CRect rc;
	if(user::GetPos("FrmDataEdit",rc))
	{
		rc.right = 1024;
		rc.bottom = 1024;
		MoveWindow(&rc);
	}
	this->GetClientRect(&rc);
	m_DBGrid1.MoveWindow(&rc);

	m_DBGrid1.SetScrollBars(SB_BOTH);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFrmDataEdit::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if(IsWindow(m_DBGrid1.m_hWnd))
	{
		CRect rc;
		this->GetClientRect(&rc);
		m_DBGrid1.MoveWindow(&rc);
	}
}



//-----------------------------------------------------------
// Function Name:OnActivate
// Parameter(s) :UINT nState
//				:CWnd *pWndOther
//				:BOOL bMinimized
// Return       :无
// Memo         :此消息响应函数解释请查看MSDN
// Author		:不知道是哪个写的
// Mofify		:2005-02-26 13:56 WangJia 增加了自适应窗体大小的功能
//-----------------------------------------------------------
void CFrmDataEdit::OnActivate(UINT nState, CWnd *pWndOther, BOOL bMinimized)
{
	/*static bool b=false;
	if(b)
	{
		b=false;
		return;
	}*/
	if(pWndOther==NULL) 
	{
		CDialog::OnActivate(nState,pWndOther,bMinimized);
		return;
	}
	if(GetWindowLong(pWndOther->GetSafeHwnd(),GWL_HINSTANCE)!=(long)::AfxGetInstanceHandle())
	{
		CDialog::OnActivate(nState,pWndOther,bMinimized);
		return;
	}
	CDialog::OnActivate(nState,pWndOther,bMinimized);
	if( (m_bAllowUpd || nState==WA_CLICKACTIVE) && EDIBgbl::gbDisplayBOM)
	{
		//long c=m_Data1->RecordCount;
		/*m_Data1->Requery(-1);*/
		//b=true;
		try
		{
			if(m_Data1!=NULL && m_Data1->State!=adStateClosed)
			{
				try
				{
					m_Data1->Update();
				}
				catch(CException *e)
				{
				//	m_Data1->CancelUpdate();//pfg20050227原代码
				}
			}
			m_DBGrid1.SetRefDataSource(NULL);
			EDIBAcad::DisplayDataZB();
			m_Data1->Filter=_variant_t("ClassID="+ltos(iROD));
			m_iRodNum=m_Data1->GetRecordCount();
			m_Data1->Filter="";
			m_DBGrid1.SetRefDataSource(m_Data1->GetDataSource());
			EDIBDB::RefreshGrid(m_DBGrid1,m_Data1);
			EDIBgbl::SetDBGridColumnCaptionAndWidth(m_DBGrid1,"tzx");
			m_DBGrid1.GetColumns().GetItem(_variant_t("bUserAdd")).SetVisible(FALSE);
			m_DBGrid1.GetColumns().GetItem(_variant_t("Index")).SetVisible(FALSE);
			m_DBGrid1.GetColumns().GetItem(_variant_t("L1")).SetVisible(FALSE);
			m_DBGrid1.GetColumns().GetItem(_variant_t("sizeH")).SetVisible(FALSE);
			m_DBGrid1.GetColumns().GetItem(_variant_t("ClassID")).SetVisible(FALSE);
			m_DBGrid1.GetColumns().GetItem(_variant_t("zdjh")).SetVisible(FALSE);
			m_DBGrid1.GetColumns().GetItem(_variant_t("nth")).SetVisible(FALSE);
			m_DBGrid1.Refresh();
	//EDIBgbl::SetDBGridColumnCaptionAndWidth(m_DBGrid1, "tzx");
		//b=false;

		}
		catch(_com_error e)
		{
			AfxMessageBox(e.Description());
		}
		m_bAllowUpd=false;

		// 根据DataGrid大小调整窗口大小
		int nWidth = 0, nHeight = 0;
		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(Recordset));
		try
		{
			_variant_t tmpvar;
			COleVariant v;
			CString strSQL;
			strSQL.Format( "SELECT * FROM tzx WHERE trim(LocalCaption)<>\'\'");
// 			rs.m_pDatabase = &EDIBgbl::dbTable;//20071019 "dbSORT" 改为 "dbTable"
// 			rs.Open( dbOpenSnapshot, strSQL);
			rs->Open((_bstr_t)strSQL,_variant_t((IDispatch*)EDIBgbl::dbTable,true), 
				adOpenKeyset, adLockOptimistic, adCmdText); 
		
			_variant_t ix;
			ix.ChangeType(VT_I4);
			CString sTmp;
			for( int i = 0; i < m_DBGrid1.GetColumns().GetCount(); i++)
			{
				ix.intVal = i;
				sTmp = m_DBGrid1.GetColumns().GetItem(ix).GetDataField();
				sTmp.MakeUpper();

				if( sTmp == _T("bUserAdd") || 
					sTmp == _T("Index") || 
					sTmp == _T("L1") || 
					sTmp == _T("sizeH") || 
					sTmp == _T("ClassID") || 
					sTmp == _T("zdjh") || 
					sTmp == _T("nth") )
					continue;

				VARIANT vTmp;
				if( rs->Find((_bstr_t)(CString(_T("Trim(FieldName)=\'")) + sTmp + _T("\'")), 0, adSearchForward, vTmp))
				{
					rs->get_Collect((_variant_t)( _T("width")), &v);
					if( v.vt == VT_NULL )
						continue;

					nWidth += vtoi(v);
				}
			}
			nWidth = float(nWidth)/15.5;	
		}
		catch(CException *e)
		{
			e->ReportError();
			e->Delete();
		}
          
        nWidth += 10;
		nHeight = m_Data1->GetRecordCount() * 21;
		
		nHeight = (nHeight < 100) ? 100 : nHeight;
        
		if( m_Data1->GetRecordCount() == 10 )
			nHeight = m_Data1->GetRecordCount() * 23;

		if( nWidth > ::GetSystemMetrics(SM_CXFULLSCREEN) )
			nWidth = ::GetSystemMetrics(SM_CXFULLSCREEN);

		if( nHeight > ::GetSystemMetrics(SM_CYFULLSCREEN) )
			nHeight = ::GetSystemMetrics(SM_CYFULLSCREEN);		
		
		::SetWindowPos( this->GetSafeHwnd(),
				HWND_TOP,
				0, 0, nWidth, nHeight,
				SWP_NOMOVE);
	}
}

BEGIN_EVENTSINK_MAP(CFrmDataEdit, CDialog)
    //{{AFX_EVENTSINK_MAP(CFrmDataEdit)
	ON_EVENT(CFrmDataEdit, IDC_DATAGRID1, 213 /* ColResize */, OnColResizeDatagrid1, VTS_I2 VTS_PI2)
	ON_EVENT(CFrmDataEdit, IDC_DATAGRID1, 215 /* Error */, OnErrorDatagrid1, VTS_I2 VTS_PI2)
	ON_EVENT(CFrmDataEdit, IDC_DATAGRID1, 207 /* BeforeDelete */, OnBeforeDeleteDatagrid1, VTS_PI2)
	ON_EVENT(CFrmDataEdit, IDC_DATAGRID1, 202 /* AfterDelete */, OnAfterDeleteDatagrid1, VTS_NONE)
	ON_EVENT(CFrmDataEdit, IDC_DATAGRID1, -606 /* MouseMove */, OnMouseMoveDatagrid1, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	ON_EVENT(CFrmDataEdit, IDC_DATAGRID1, 204 /* AfterUpdate */, OnAfterUpdateDatagrid1, VTS_NONE)
	ON_EVENT(CFrmDataEdit, IDC_DATAGRID1, 209 /* BeforeUpdate */, OnBeforeUpdateDatagrid1, VTS_PI2)
	ON_EVENT(CFrmDataEdit, IDC_DATAGRID1, 218 /* RowColChange */, OnRowColChangeDatagrid1, VTS_PVARIANT VTS_I2)
	ON_EVENT(CFrmDataEdit, IDC_DATAGRID1, 205 /* BeforeColEdit */, OnBeforeColEditDatagrid1, VTS_I2 VTS_I2 VTS_PI2)
	ON_EVENT(CFrmDataEdit, IDC_DATAGRID1, 200 /* AfterColEdit */, OnAfterColEditDatagrid1, VTS_I2)
	ON_EVENT(CFrmDataEdit, IDC_DATAGRID1, 201 /* AfterColUpdate */, OnAfterColUpdateDatagrid1, VTS_I2)
	ON_EVENT(CFrmDataEdit, IDC_DATAGRID1, 211 /* Change */, OnChangeDatagrid1, VTS_NONE)
	ON_EVENT(CFrmDataEdit, IDC_DATAGRID1, 206 /* BeforeColUpdate */, OnBeforeColUpdateDatagrid1, VTS_I2 VTS_PVARIANT VTS_PI2)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CFrmDataEdit::OnColResizeDatagrid1(short ColIndex, short FAR* Cancel) 
{
	EDIBgbl::SaveDBGridColumnCaptionAndWidth( m_DBGrid1, ColIndex, "tzx");		
}

//-----------------------------------------------------------
// Function Name:OnDestroy
// Parameter(s) :无
// Return       :无
// Memo         :销毁窗体时调用
// Author       :不知道谁写的
// Modify       :2005-02-26 15:36 WangJia 把位置保存放到这里来了
//-----------------------------------------------------------
void CFrmDataEdit::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	try
	{
		if(m_Data1)
		{
 			if(m_Data1->State!=adStateClosed)
 				m_Data1->Close();
		}
	}
	catch(CException *e)
	{
		e->Delete();
	}

	m_Data1=NULL;
	user::SavePos( this, "FrmDataEdit");
}


void CFrmDataEdit::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	//user::SavePos( this, "FrmDataEdit");
	CDialog::OnClose();
}

void CFrmDataEdit::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	//if(!bShow)
	//user::SavePos(this,"FrmDataEdit");
	// TODO: Add your message handler code here
}

void CFrmDataEdit::OnErrorDatagrid1(short DataError, short FAR* Response) 
{
	// TODO: Add your control notification handler code here
	//取消删除操作时不显示提示框
	if(DataError==7011)
	{
		*Response=0;
	}
}

void CFrmDataEdit::OnBeforeDeleteDatagrid1(short FAR* Cancel) 
{
	// TODO: Add your control notification handler code here
	try
	{
		//当用户执行删除操作时，不允许删除程序计算的材料，提示是否要删除用户添加的非标材料
		if(m_Data1->adoEOF || m_Data1->BOF)
			return;
		if(vtob(m_Data1->GetCollect("bUserAdd")))
		{
			if(MessageBox(GetResStr(IDS_DO_YOU_DEL_NON_SIGN_STUFF),NULL,MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2)!=IDYES)
			{
				*Cancel=1;
			}
			else
			{
				//连同 ZG 表中的数据一起删除
				CString strSQL;
				strSQL.Format(_T("DELETE FROM [ZG] WHERE VolumeID=%d AND zdjh=%d AND nth=%d AND recno=%d"),
					EDIBgbl::SelVlmID,vtoi(m_Data1->GetCollect("zdjh")),vtoi(m_Data1->GetCollect("nth")),
					vtoi(m_Data1->GetCollect("recno")));
				EDIBgbl::dbPRJDB->Execute((_bstr_t)strSQL, NULL, adCmdText);

			}
		}
		else
		{
			*Cancel=1;
		}
	}
	catch(_com_error e)
	{
		*Cancel=1;
		AfxMessageBox(e.Description());
	}
}

void CFrmDataEdit::OnAfterDeleteDatagrid1() 
{
	// TODO: Add your control notification handler code here
	
	/*if(m_Data1->get_RecordCount()==0)
		FrmTxsr.m_ActiveRs->PutCollect("bCalSuccess",0);
	else
		FrmTxsr.m_ActiveRs->PutCollect("bCalSuccess",-1);*/
}

void CFrmDataEdit::OnMouseMoveDatagrid1(short Button, short Shift, long X, long Y) 
{
	// TODO: Add your control notification handler code here
	
}

void CFrmDataEdit::OnAfterUpdateDatagrid1() 
{
}

void CFrmDataEdit::OnBeforeUpdateDatagrid1(short FAR* Cancel) 
{
	// TODO: Add your control notification handler code here
	try
	{
		if(!IsWindow(m_DBGrid1.GetSafeHwnd()))
			return;
		int iCol=m_DBGrid1.GetCol();
		if(iCol<3)
		{
			m_DBGrid1.GetColumns().GetItem(_variant_t((long)iCol)).SetText(m_strCols[iCol]);
			//*Cancel=1;
			return;
		}
		if(iCol==3)
		{
			if(vtoi(m_Data1->GetCollect("ClassID"))!=iROD)
				return;
			if(m_iRodNum<=1)
			{
				m_DBGrid1.GetColumns().GetItem(_variant_t((long)iCol)).SetText(m_strCols[iCol]);
				//*Cancel=1;
				return;
			}
			CString strN=m_strCols[iCol];//vtos(m_Data1->GetCollect("CLgg"));
			if(strN=="")
				return;
			int ix=strN.GetLength()-1;
			while(strN[ix]<'0' || strN[ix]>'9') ix--;
			while(strN[ix]>='0' && strN[ix]<='9') ix--;
			CString strN1=m_DBGrid1.GetColumns().GetItem(_variant_t((long)3)).GetText();
			if(strN1=="" || strN1.GetLength()<ix+2)
			{
				m_DBGrid1.GetColumns().GetItem(_variant_t((long)iCol)).SetText(m_strCols[iCol]);
				//*Cancel=1;
				return;
			}
			if(strN1.Left(ix+1) != strN.Left(ix+1))
			{
				m_DBGrid1.GetColumns().GetItem(_variant_t((long)iCol)).SetText(m_strCols[iCol]);
				//*Cancel=1;
				return;
			}
			if(_ttol(strN1.Mid(ix+1))<=0)
			{
				m_DBGrid1.GetColumns().GetItem(_variant_t((long)iCol)).SetText(m_strCols[iCol]);
				//*Cancel=1;
				return;
			}
			m_DBGrid1.GetColumns().GetItem(_variant_t((long)iCol)).SetText(strN1.Left(ix+1)+ltos(_ttol(strN1.Mid(ix+1))));
			m_DBGrid1.GetColumns().GetItem(_variant_t("L1")).SetText(strN1.Mid(ix+1));
		}
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.Description());
	}
}

void CFrmDataEdit::OnRowColChangeDatagrid1(VARIANT FAR* LastRow, short LastCol) 
{
	// TODO: Add your control notification handler code here
	try
	{
		if(!IsWindow(m_DBGrid1.GetSafeHwnd()))
			return;
		int iCol=m_DBGrid1.GetCol();
		if(iCol==-1) return;
		for(int i=0;i<m_DBGrid1.GetColumns().GetCount();i++)
			m_strCols[i]=m_DBGrid1.GetColumns().GetItem(_variant_t((long)iCol)).GetText();
	}
	catch(CException *e)
	{
		e->Delete();
	}
}

void CFrmDataEdit::OnBeforeColEditDatagrid1(short ColIndex, short KeyAscii, short FAR* Cancel) 
{
	// TODO: Add your control notification handler code here
	try
	{
		int iRow=m_DBGrid1.GetRow();
	}
	catch(CException *e)
	{
		e->Delete();
	}
}

void CFrmDataEdit::OnAfterColEditDatagrid1(short ColIndex) 
{
	// TODO: Add your control notification handler code here
	try
	{
		int i=ColIndex;
	}
	catch(CException *e)
	{
		e->Delete();
	}
}

void CFrmDataEdit::OnAfterColUpdateDatagrid1(short ColIndex) 
{
	// TODO: Add your control notification handler code here
	
}

void CFrmDataEdit::OnChangeDatagrid1() 
{
	// TODO: Add your control notification handler code here
	
}

void CFrmDataEdit::OnBeforeColUpdateDatagrid1(short ColIndex, VARIANT FAR* OldValue, short FAR* Cancel) 
{
	// TODO: Add your control notification handler code here
	try
	{
		if(!IsWindow(m_DBGrid1.GetSafeHwnd()))
			return;		
		
		int iFDL1=0,iFDCLgg=0,iFDNum=0,iFDSEQ=0,iFDbUserAdd=0,i=0,iL=0,ix=0;
		bool bFound=false;
		COleVariant v;
		CString sTmp;
		int iFDCount=m_DBGrid1.GetColumns().GetCount();
		while((ix<iFDCount) && (m_DBGrid1.GetColumns().GetItem(_variant_t((long)ix)).GetDataField()!="CLnum")) ix++;
		iFDNum=ix;	
		
		ix=0;
		while((ix<iFDCount) && (m_DBGrid1.GetColumns().GetItem(_variant_t((long)ix)).GetDataField()!="bUserAdd")) ix++;
		iFDbUserAdd=ix;		
				
		ix=0;
		while((ix<iFDCount) && (m_DBGrid1.GetColumns().GetItem(_variant_t((long)ix)).GetDataField()!="SEQ")) ix++;
		iFDSEQ=ix;		
				
		if(vtob(m_Data1->GetCollect("bUserAdd")) && (ColIndex==iFDSEQ || ColIndex==iFDNum))
		{
			//用户添加材料允许改数量和编号
			*Cancel=0;
			return;
		}
		//获得当前列字段名
		CString strFD=m_DBGrid1.GetColumns().GetItem(_variant_t((long)ColIndex)).GetDataField();

		if(strFD=="sizeDIM")
		{
			//编辑的是尺寸列,并且要2根以上拉杆
			if(vtoi(m_Data1->GetCollect("ClassID"))==iROD && m_iRodNum>1)
			{
				CInputBox dlg;
				/*
				static bool bFlag;
				static float SumDeltaL1;								
				if(!bFlag)
				{
					//第一次修改的拉杆长度增量
					DeltaL1=atof(NewValue)-vtof(_variant_t(OldValue));
					SumDeltaL1=DeltaL1;
				}
				else
				{
					//第一次以后修改的拉杆					
					SumDeltaL1=-SumDeltaL1/(m_iRodNum-1);
					NewValue=ftos(vtof(_variant_t(OldValue))+SumDeltaL1);
					m_DBGrid1.GetColumns().GetItem(_variant_t((long)ColIndex)).SetValue(_variant_t(NewValue));
				}
				if(!bFlag) bFlag=true;
				*/

				CString L1=vtos(m_Data1->GetCollect("L1"));
				/*
				while(L1=="")
				{
					dlg.DoModal();
					L1=dlg.m_strValue;
				}
				*/
				CString NewValue=m_DBGrid1.GetColumns().GetItem(_variant_t((long)ColIndex)).GetText();
				CString strCLgg=vtos(m_Data1->GetCollect("CLgg"));
				float DeltaL1=_tcstod(NewValue,NULL)-vtof(_variant_t(OldValue));
				CString nL1=ftos(_tcstod(L1,NULL)+DeltaL1);
				//代换规格中的长度值
				strCLgg.Replace(L1,nL1);
				//获得L1字段值，它代表拉杆长度
				ix=0;
				while((ix<iFDCount) && (m_DBGrid1.GetColumns().GetItem(_variant_t((long)ix)).GetDataField()!="L1")) ix++;
				m_DBGrid1.GetColumns().GetItem(_variant_t((long)ix)).SetText(nL1);
				iFDL1=ix;
				//更新sizeH字段值，它代表拉杆长度
				ix=0;
				while((ix<iFDCount) && (m_DBGrid1.GetColumns().GetItem(_variant_t((long)ix)).GetDataField()!="sizeH")) ix++;
				m_DBGrid1.GetColumns().GetItem(_variant_t((long)ix)).SetText(nL1);
				//不能使用下列语句，它破坏缓冲
				//m_Data1->PutCollect("L1",_variant_t(nL1));

				ix=0;
				while((ix<iFDCount) && (m_DBGrid1.GetColumns().GetItem(_variant_t((long)ix)).GetDataField()!="CLgg")) ix++;
				m_DBGrid1.GetColumns().GetItem(_variant_t((long)ix)).SetText(strCLgg);
				iFDCLgg=ix;
				//更新CLdz/CLzz字段值，它代表拉杆重量				
				ix=0;
				while((ix<iFDCount) && (m_DBGrid1.GetColumns().GetItem(_variant_t((long)ix)).GetDataField()!="CLdz")) ix++;
				float fCLdz;
				v=m_DBGrid1.GetColumns().GetItem(_variant_t((long)ix)).GetValue();
				sTmp=m_DBGrid1.GetColumns().GetItem(_variant_t((long)ix)).GetText();
				fCLdz=vtof(v)*_tcstod(nL1,NULL)/_tcstod(L1,NULL);
				fCLdz=_tcstod(sTmp,NULL)*_tcstod(nL1,NULL)/_tcstod(L1,NULL);
				m_DBGrid1.GetColumns().GetItem(_variant_t((long)ix)).SetValue(_variant_t(fCLdz));
				//更新CLdz/CLzz字段值，它代表拉杆重量
				ix=0;
				while((ix<iFDCount) && (m_DBGrid1.GetColumns().GetItem(_variant_t((long)ix)).GetDataField()!="CLzz")) ix++;
				float fCLzz;
				v=m_DBGrid1.GetColumns().GetItem(_variant_t((long)ix)).GetValue();
				sTmp=m_DBGrid1.GetColumns().GetItem(_variant_t((long)ix)).GetText();
				fCLzz=vtof(v)*_tcstod(nL1,NULL)/_tcstod(L1,NULL);
				fCLzz=_tcstod(sTmp,NULL)*_tcstod(nL1,NULL)/_tcstod(L1,NULL);
				m_DBGrid1.GetColumns().GetItem(_variant_t((long)ix)).SetValue(_variant_t(fCLzz));

				*Cancel=0;
			}
			else
			{
				*Cancel=1;
			}
		}
		else
		{
			*Cancel=1;
		}
		return;
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.Description());		
	}	
	return;
}



