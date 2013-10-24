// SelectXYZdlg.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "SelectXYZdlg.h"
#include "edibgbl.h"
#include "frmtxsr.h"
#include "phsdata.h"
#include "edibdb.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectXYZdlg dialog


CSelectXYZdlg::CSelectXYZdlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectXYZdlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectXYZdlg)
	m_strDX = _T("");
	m_strNB = _T("");
	m_strUP = _T("");
	//}}AFX_DATA_INIT
}


void CSelectXYZdlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectXYZdlg)
	DDX_Control(pDX, IDC_COMBOup, m_up);
	DDX_Control(pDX, IDC_COMBOnb, m_nb);
	DDX_Control(pDX, IDC_COMBOdx, m_dx);
	DDX_CBString(pDX, IDC_COMBOdx, m_strDX);
	DDX_CBString(pDX, IDC_COMBOnb, m_strNB);
	DDX_CBString(pDX, IDC_COMBOup, m_strUP);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectXYZdlg, CDialog)
	//{{AFX_MSG_MAP(CSelectXYZdlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectXYZdlg message handlers

void CSelectXYZdlg::OnOK()
{
	m_dx.GetWindowText(m_strDX);
	m_strDX.TrimLeft();
	m_strDX.TrimRight();
	m_nb.GetWindowText(m_strNB);
	m_strNB.TrimLeft();
	m_strNB.TrimRight();
	m_up.GetWindowText(m_strUP);
	m_strUP.TrimLeft();
	m_strUP.TrimRight();
	CDialog::OnOK();
}

BOOL CSelectXYZdlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_dx.SetCurSel(0);
	m_nb.SetCurSel(0);
	m_up.SetCurSel(0);
	return true;
}

#include "user.h"
void ReadResult_Excel(_RecordsetPtr pRs)
{
	CString strSQL=_T("");
	CString x=_T("");
	CString y=_T("");
	CString z=_T("");
	_RecordsetPtr pRsZA1=NULL;
	CSelectXYZdlg dlg;
	if(dlg.DoModal()!=IDOK)
	{
		return;
	}
	z=(dlg.m_strUP=="东")?"E":"W";
	x=(dlg.m_strDX=="北")?"N":"S";
	y=(dlg.m_strNB=="竖直向上")?"UP":"DN";
	_variant_t key;
	CString strTmp=_T("");
	CString strSource=_T("");
	CString strLeft=_T("");
	CString strRight=_T("");
	float yr1=0.0;//竖直向位移
	float pyr1=0.0;//竖直向工作载荷
	float pxr1=0.0;//南北向工作载荷
	float pzr1=0.0;//东西向工作载荷
	float phydyl=0.0;//竖直向水压试验载荷
	float pjg1=0.0;//结构荷载
	float pgz1=0.0;//工作荷载
	float pyl1=0.0;//冷态载荷
	float PoccXL=0.0;//x轴上偶然荷载
	float PoccYL=0.0;//y轴上的偶然荷载
	float PoccZL=0.0;//Z轴上的偶然荷载
	float Pocc=0.0;
	pRsZA1.CreateInstance(__uuidof(Recordset));
	strSQL="select * from za1";
	try
	{
		pRsZA1->Open(_bstr_t(strSQL),(IDispatch*)conPRJDB,adOpenStatic,adLockOptimistic,adCmdText);
		if(pRsZA1->GetRecordCount()>0)
		{
			pRsZA1->MoveFirst();
		}
		else
		{
			AfxMessageBox("导入数据失败!");
			return;
		}
		for(;!pRsZA1->adoEOF;pRsZA1->MoveNext())
		{
			strSQL.Format("VolumeID=%d and ZDJH=%d",EDIBgbl::SelVlmID,vtoi(pRsZA1->GetCollect("zdjh")));
			pRs->Filter="";
			pRs->Filter=_variant_t(strSQL);
			if(pRs->adoEOF&&pRs->BOF)
			{
				pRs->AddNew();
			}
			pRs->PutCollect(_T("zdjh"),pRsZA1->GetCollect("zdjh"));
			pRs->PutCollect(_T("VolumeID"),_variant_t(EDIBgbl::SelVlmID));

			key=pRsZA1->GetCollect(_T("xr1"));//南北向热位移
			if(!(key.vt==VT_NULL||key.vt==VT_EMPTY))
			{
				strSource=vtos(key);
				strRight=strSource.Right(1);
				strSource+=" ";
				strLeft=strSource.Left(strSource.Find(_T(" "),0));
				if(strRight==x)
				{
					pRs->PutCollect(_T("xr1"),_variant_t(atof(strLeft)));	
				}
				else
				{
					pRs->PutCollect(_T("xr1"),_variant_t(-atof(strLeft)));	
				}
			}

			key=pRsZA1->GetCollect(_T("yr1"));//竖直向位移
			if(!(key.vt==VT_NULL||key.vt==VT_EMPTY))
			{
				strSource=vtos(key);
				strRight=strSource.Right(2);
				strSource+=" ";
				strLeft=strSource.Left(strSource.Find(_T(" "),0));
				if(strRight==y)
				{
					pRs->PutCollect(_T("yr1"),_variant_t(atof(strLeft)));	
				}
				else
				{
					pRs->PutCollect(_T("yr1"),_variant_t(-atof(strLeft)));	
				}
				yr1=atof(strLeft);//pfg20051205
			}
			else
			{
				yr1=0.0;
			}

			key=pRsZA1->GetCollect(_T("zr1"));//东西向热位移
			if(!(key.vt==VT_NULL||key.vt==VT_EMPTY))
			{
				strSource=vtos(key);
				strRight=strSource.Right(1);
				strSource+=" ";
				strLeft=strSource.Left(strSource.Find(_T(" "),0));
				if(strRight==z)
				{
					pRs->PutCollect(_T("zr1"),_variant_t(atof(strLeft)));	
				}
				else
				{
					pRs->PutCollect(_T("zr1"),_variant_t(-atof(strLeft)));	
				}
			}

			pyr1=vtof(pRsZA1->GetCollect("pyr1"));
			pxr1=vtof(pRsZA1->GetCollect("pxr1"));
			pzr1=vtof(pRsZA1->GetCollect("pzr1"));
			pgz1=sqrt(pyr1*pyr1+pxr1*pxr1+pzr1*pzr1);
			phydyl=vtof(pRsZA1->GetCollect("PhydYL"));//竖直向水压试验载荷
			//pgz1=(phydyl>pgz1)?phydyl:pgz1;
			pyl1=vtof(pRsZA1->GetCollect("pyl1"));
			
			PoccXL=vtof(pRsZA1->GetCollect("PoccXL"));
			PoccYL=vtof(pRsZA1->GetCollect("PoccYL"));
			PoccZL=vtof(pRsZA1->GetCollect("PoccZL"));
			Pocc=sqrt(PoccXL*PoccXL+PoccYL*PoccYL+PoccZL*PoccZL);
			if(yr1!=0)
			{//弹簧的工作荷载取三个方向的矢量和(start)
				pRs->PutCollect(_T("pgz1"),_variant_t(pgz1));//工作载荷
			}//弹簧的工作荷载取三个方向的矢量和(end)
			else
			{//刚性支吊架的结构荷载Pdesign = Max(Pope  , Psus)*1.4 + Pocc*1.2 + Phyd-Psus (start)
				pjg1=(pgz1>pyl1)?pgz1:pyl1;
				pjg1=pjg1*1.4+Pocc*1.2+phydyl-pyl1;
				pRs->PutCollect(_T("pjg1"),_variant_t(pjg1));
			}//刚性支吊架的结构荷载Pdesign = Max(Pope  , Psus)*1.4 + Pocc*1.2 + Phyd-Psus (end)
			pRs->PutCollect(_T("pyr1"),vtof(pRsZA1->GetCollect("pyr1")));//竖直向工作载荷
			pRs->PutCollect(_T("pxr1"),vtof(pRsZA1->GetCollect("pxr1")));//南北向工作载荷
			pRs->PutCollect(_T("pzr1"),vtof(pRsZA1->GetCollect("pzr1")));//东西向工作载荷
			strSource=vtos(pRsZA1->GetCollect(_T("gh1")));//根部标高（梁为顶标高）
			if(strSource!="")
			{
				strSource+=" ";
				strLeft=strSource.Left(strSource.Find(_T(" "),0));
				pRs->PutCollect(_T("gh1"),_variant_t(atof(strLeft)));
			}

			strSource=vtos(pRsZA1->GetCollect(_T("dh1")));//管部标高
			if(strSource!="")
			{
				strSource+=" ";
				strLeft=strSource.Left(strSource.Find(_T(" "),0));
				pRs->PutCollect(_T("dh1"),_variant_t(atof(strLeft)));
			}
			pRs->PutCollect(_T("T01"),vtof(pRsZA1->GetCollect("T01")));//设计温度
			pRs->PutCollect(_T("dj1"),vtof(pRsZA1->GetCollect("dj1")));//管道工程直径
			pRs->PutCollect(_T("UPxyz"),_variant_t(long(2)));//向上的坐标方向为y方向
			pRs->PutCollect(_T("UNIT_FORCE"),pRsZA1->GetCollect("UNIT_FORCE"));
			key=pRsZA1->GetCollect(_T("dxa1"));//管部南北向位置(start)
			if(!(key.vt==VT_NULL||key.vt==VT_EMPTY))
			{
				strSource=vtos(key);
				strSource+=" ";
				strLeft=strSource.Left(strSource.Find(_T(" "),0));
				strLeft=(strLeft=="")?"0.0":strLeft;
				if(strSource.Find(_T("S"))!=-1)
				{
					pRs->PutCollect(_T("dxa1"),_variant_t(-atof(strLeft)));
				}
				else
				{
					pRs->PutCollect(_T("dxa1"),_variant_t(strLeft));
				}
				strRight=strSource.Right(strSource.GetLength()-strSource.Find(_T("/"),0)-1);
				if(strSource.Find(_T("/"),0)!=-1)
				{
					pRs->PutCollect(_T("XZU1"),_variant_t(strRight));
				}
			}//管部南北向位置(end)

			//管部东西向位置(start)
			key=pRsZA1->GetCollect(_T("dza1"));
			if(!(key.vt==VT_NULL||key.vt==VT_EMPTY))
			{
				strSource=vtos(key);
				strSource+=" ";
				strLeft=strSource.Left(strSource.Find(_T(" "),0));
				strLeft=(strLeft=="")?"0.0":strLeft;
				if(strSource.Find(_T("W"))!=-1)
				{
					pRs->PutCollect(_T("dza1"),_variant_t(-atof(strLeft)));
				}
				else
				{
					pRs->PutCollect(_T("dza1"),_variant_t(strLeft));
				}
				strRight=strSource.Right(strSource.GetLength()-strSource.Find(_T("/"),0)-1);
				if(strSource.Find(_T("/"),0)!=-1)
				{
					pRs->PutCollect(_T("ZZU1"),_variant_t(strRight));
				}
			}
			//管部东西向位置(end)

			//管道方向
			strSource=vtos(pRsZA1->GetCollect("DF1"));
			if(strSource.Find(_T("N"),0)!=-1)
			{
				pRs->PutCollect(_T("DF1"),_variant_t("X"));
			}
			else if(strSource.Find(_T("E"),0)!=-1)
			{
				pRs->PutCollect(_T("DF1"),_variant_t("Z"));
			}

			//根部方向
			strSource=vtos(pRsZA1->GetCollect("GF1"));
			if(strSource.Find(_T("N"),0)!=-1)
			{
				pRs->PutCollect(_T("GF1"),_variant_t("PX"));
			}
			else if(strSource.Find(_T("E-W"),0)!=-1)
			{
				pRs->PutCollect(_T("GF1"),_variant_t("PZ"));
			}
			pRs->Update();
		}
		AfxMessageBox("导入支架原始数据成功!");
		pRs->Filter="";
		FrmTxsr.m_pViewTxsr->m_bAllowUpd=false;
		FrmTxsr.m_pViewTxsr->m_Databill.SetRefRecordset(pRs);
		FrmTxsr.m_pViewTxsr->m_Databill.SetEnabled(TRUE);
		FrmPhsData.m_DBGbill.SetRefDataSource(pRs);
		EDIBDB::SetColumnsProperty(FrmPhsData.m_DBGbill, EDIBgbl::SelBillType);
		FrmTxsr.m_pViewTxsr->m_bAllowUpd=true;
	}
	catch(_com_error e)
	{
		pRs->Filter="";
		AfxMessageBox(e.Description());
	}
}