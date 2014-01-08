 // SelTemplate.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "modPHScal.h"
#include "user.h"
#include "FrmTxsr.h"
#include <math.h>
#include "EDIBgbl.h"
#include "ModEncrypt.h"
#include "cphs.h"
#include "frmselspecification.h"
#include "modregistry.h"
#include "SelTemplate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelTemplate dialog

LPCTSTR theSortName[]={_T("SampleID"),_T("Frequence"),_T("iNumOfPart"),_T("SampleName"),_T("PA")};

CSelTemplate::CSelTemplate(CWnd* pParent /*=NULL*/)
	: CDialog(CSelTemplate::IDD, pParent)
	, m_dwChkCondition(0)
	, m_itmpPreviousIndex(0)
	, m_bRefImg(TRUE)
	, m_iOldIxEDIT(-1)
	, m_iOldIxZDG(-1)
	, m_bSaveChange(TRUE)
	, m_iLastSampleID(0)
	, m_iCurSampleID(0)
	, m_iSaveID(-1)
{
	m_bLoadPA=FALSE;
	m_bLoadSA=FALSE;
	m_bIsSelPA=FALSE;
	m_bIsSelSA=FALSE;
	m_bSetFavorite=FALSE;
	m_bSetDefaultFavorite=FALSE;

	m_CheckCondition[5] = FALSE;
	m_CheckCondition[4] = FALSE;
	m_CheckCondition[0] = FALSE;
	m_CheckCondition[1] = FALSE;
	m_CheckCondition[2] = FALSE;
	m_CheckCondition[3] = FALSE;

	m_CheckID[0]=IDC_CHECK_PJG;
	m_CheckID[1]=IDC_CHECK_TJ;
	m_CheckID[2]=IDC_CHECK_PLACE;
	m_CheckID[3]=IDC_CHECK_CNT;
	m_CheckID[4]=IDC_CHECK_DW;
	m_CheckID[5]=IDC_CHECK_ONLYPAH;
	//{{AFX_DATA_INIT(CSelTemplate)
	m_iSortType = 0;
	m_bDesc = FALSE;
	m_bRePA = FALSE;
	m_bShowImg = FALSE;
	m_bTmp = FALSE;
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
	m_iDefaultTemplateStatus = -1;
	m_isfrist=0;
	m_bRodNum = FALSE;
	//}}AFX_DATA_INIT
}


void CSelTemplate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_CHECK_ONLYPAH, m_CheckCondition[iChkBoxOnlyPAh]);
	DDX_Check(pDX, IDC_CHECK_DW, m_CheckCondition[iChkBoxDW]);
	DDX_Check(pDX, IDC_CHECK_PJG, m_CheckCondition[iChkBoxPJG]);
	DDX_Check(pDX, IDC_CHECK_TJ, m_CheckCondition[iChkBoxTJ]);
	DDX_Check(pDX, IDC_CHECK_PLACE, m_CheckCondition[iChkBoxGDW1]);
	DDX_Check(pDX, IDC_CHECK_CNT, m_CheckCondition[iChkBoxCNT]);
	//{{AFX_DATA_MAP(CSelTemplate)
	DDX_Control(pDX, IDC_LISTCTRL_STRUCT, m_listctrlStruct);
	DDX_Control(pDX, IDC_BUTTON_SET_DEFAULT_FAVORITE, m_btnDefaultFavoriteTemplate);
	DDX_Control(pDX, IDC_BTN_FAVORITE_TEMPLATE, m_btnFavoriteTemplate);
	DDX_Control(pDX, IDC_LIST2, m_lstPart);
	DDX_Control(pDX, IDC_LIST_STRUCT, m_lstStruct);
	DDX_Radio(pDX, IDC_RADIO7, m_iSortType);
	DDX_Check(pDX, IDC_CHECK2, m_bDesc);
	DDX_Check(pDX, IDC_CHECK_PA, m_bRePA);
	DDX_Check(pDX, IDC_CHECK_BMP, m_bShowImg);
	DDX_Check(pDX, IDC_CHECK_TJ, m_bTmp);
	DDX_Control(pDX, IDC_COMBO_SA, m_comboSA);
	DDX_Control(pDX, IDC_COMBO_PA, m_comboPA);
	DDX_CBStringExact(pDX, IDC_COMBO_RODNUM, m_nRodNum);
	DDX_CBStringExact(pDX, IDC_COMBO_SPRNUM, m_nSprNum);
	DDX_Check(pDX, IDC_CHECK_FILTER, m_bFilter);
	DDX_Check(pDX, IDC_CHECK_SH2, m_bSh);
	DDX_Check(pDX, IDC_CHECK_SABEAM2, m_bSABEAM);
	DDX_Check(pDX, IDC_CHECK_SACANT2, m_bSACANTil);
	DDX_Check(pDX, IDC_CHECK_SALBRACE2, m_bSALBRACE);
	DDX_Check(pDX, IDC_CHECK_DH2, m_bDH);
	DDX_CBStringExact(pDX, IDC_COMBO_CSPRNUM, m_nCSPRNum);
	DDX_Check(pDX, IDC_CHECK_CURENTSA2, m_bCurrentSA);
	DDX_Check(pDX, IDC_CHECK_CURRENTPA, m_bCurrentPA);
	DDX_Check(pDX, IDC_CHECK_SAGCEMENT, m_bSAGCEMENT);
	DDX_Check(pDX, IDC_CHECK_CURRENTZDJHPA, m_bCurrentZdjhPA);
	DDX_Check(pDX, IDC_CHECK_CURRENTZDJHSA2, m_bCurrentZdjhSA);
	DDX_Check(pDX, IDC_CHECK_CURRENTZDJHAVPA, m_bCurrentZdjhAvPA);
	DDX_Check(pDX, IDC_CHECK_CURRENTZDJHAVSA, m_bCurrentZdjhAvSA);
	DDX_Check(pDX, IDC_CHECK_AUTOSPRNUM2, m_bAutoSprNum);
	DDX_Radio(pDX, IDC_RADIO_DEFAULT_TEMPLATE, m_iDefaultTemplateStatus);
	DDX_Check(pDX, IDC_CHECK11, m_bRodNum);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelTemplate, CDialog)
	//{{AFX_MSG_MAP(CSelTemplate)
	ON_BN_CLICKED(IDC_RADIO7, OnRadio1)
	ON_BN_CLICKED(IDC_CHECK2, OnCheckDesc)
	ON_BN_CLICKED(IDC_CHECK_FILTER, OnCheckFilter)
	ON_BN_CLICKED(IDC_CHECK_CURRENTZDJHAVPA, OnCheckCurrentzdjhavpa)
	ON_BN_CLICKED(IDC_CHECK_CURRENTPA, OnCheckCurrentPA)
	ON_BN_CLICKED(IDC_CHECK_CURRENTZDJHPA, OnCheckCurrentZdjhPA)
	ON_BN_CLICKED(IDC_CHECK_DH2, OnCheckDh)
	ON_BN_CLICKED(IDC_CHECK_SH2, OnCheckSh)
	ON_CBN_SELCHANGE(IDC_COMBO_PA, OnSelchangeComboPA)
	ON_CBN_SELCHANGE(IDC_COMBO_SA, OnSelchangeComboSA)
	ON_BN_CLICKED(IDC_CHECK_CURRENTZDJHAVSA, OnCheckCurrentzdjhavSA)
	ON_BN_CLICKED(IDC_CHECK_CURENTSA2, OnCheckCurentSA)
	ON_BN_CLICKED(IDC_CHECK_CURRENTZDJHSA2, OnCheckCurrentzdjhSA)
	ON_BN_CLICKED(IDC_CHECK_SABEAM2, OnCheckSabeam)
	ON_BN_CLICKED(IDC_CHECK_SACANT2, OnCheckSacant)
	ON_BN_CLICKED(IDC_CHECK_SALBRACE2, OnCheckSalbrace)
	ON_BN_CLICKED(IDC_CHECK_SAGCEMENT, OnCheckSagcement)
	ON_BN_CLICKED(IDC_CHECK_AUTOSPRNUM2, OnCheckAutosprnum)
	ON_CBN_SELCHANGE(IDC_COMBO_SPRNUM, OnSelchangeComboSprnum)
	ON_CBN_SELCHANGE(IDC_COMBO_CSPRNUM, OnSelchangeComboCsprnum)
	ON_LBN_SELCHANGE(IDC_LIST_STRUCT, OnSelchangeListStruct)
	ON_LBN_SELCHANGE(IDC_LIST2, OnSelchangeListPart)
	ON_BN_CLICKED(IDC_CMD_ADD, OnCmdAdd)
	ON_BN_CLICKED(IDC_CMD_DEL, OnCmdDel)
	ON_BN_CLICKED(IDC_CMD_SAVE, OnCmdSave)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CHECK_BMP, OnCheckBmp)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	ON_BN_CLICKED(IDC_CHECK_PA, OnCheckPa)
	ON_BN_CLICKED(IDC_CHECK_ONLYPAH, OnCheckOnlypah)
	ON_LBN_DBLCLK(IDC_LIST_NAME, OnDblclkListName)
	ON_BN_CLICKED(IDC_BTN_FAVORITE_TEMPLATE, OnBtnFavoriteTemplate)
	ON_BN_CLICKED(IDC_RADIO_FAVORITE_TEMPLATE, OnRadioFavoriteTemplate)
	ON_BN_CLICKED(IDC_RADIO_DEFAULT_TEMPLATE, OnRadioDefaultTemplate)
	ON_BN_CLICKED(IDC_RADIO_ALL_TEMPLATE, OnRadioAllTemplate)
	ON_BN_CLICKED(IDC_BUTTON_SET_DEFAULT_FAVORITE, OnButtonSetDefaultFavorite)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LISTCTRL_STRUCT, OnItemchangedListctrlStruct)
	ON_BN_CLICKED(IDC_BUTTON_DEL_TEMPLATE, OnButtonDelTemplate)
	ON_BN_CLICKED(IDC_CMD_CANCEL, OnCmdCancel)
	ON_BN_CLICKED(IDC_BUTTON5, OnButton5)
	ON_BN_CLICKED(IDC_CHECK11, OnCheck11)
	ON_BN_CLICKED(IDC_RADIO8, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO9, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO10, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO11, OnRadio1)
	ON_LBN_DBLCLK(IDC_LIST2, OnCmdAdd)
	ON_LBN_DBLCLK(IDC_LIST_STRUCT, OnCmdDel)
	ON_CBN_SELCHANGE(IDC_COMBO_RODNUM, OnSelchangeComboRodnum)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelTemplate message handlers
//DEL void CSelTemplate::UpdateRS()
//DEL {
//DEL 	GetFilterStr();
//DEL 	OpenTemplateRs();
//DEL 	if(m_rsTemplateName.IsEOF() && m_rsTemplateName.IsBOF())
//DEL 	{
//DEL 		m_bCurrentPA=FALSE;
//DEL 		m_bCurrentSA=FALSE;
//DEL 		CheckDlgButton(IDC_CHECK_CURENTSA2,FALSE);
//DEL 		CheckDlgButton(IDC_CHECK_CURRENTPA,FALSE);
//DEL 		GetDlgItem(IDC_CHECK_CURENTSA2)->EnableWindow(FALSE);
//DEL 		GetDlgItem(IDC_CHECK_CURRENTPA)->EnableWindow(FALSE);
//DEL 		
//DEL 	}
//DEL 	else
//DEL 	{
//DEL 		UpdateData();
//DEL 		GetDlgItem(IDC_CHECK_CURENTSA2)->EnableWindow(m_bFilter && m_bCurrentZdjhAvSA);
//DEL 		GetDlgItem(IDC_CHECK_CURRENTPA)->EnableWindow(m_bFilter && m_bCurrentZdjhAvPA);
//DEL 		
//DEL 	}
//DEL }

void CSelTemplate::UpdateRecordset()
{
	::CWaitCursor curWait;
	GetFilterStr();
	OpenTemplateRs();

	if(m_rsTemplateName->adoEOF && m_rsTemplateName->BOF)
	{
		m_bCurrentPA=FALSE;
		m_bCurrentSA=FALSE;
		CheckDlgButton(IDC_CHECK_CURENTSA2,FALSE);
		CheckDlgButton(IDC_CHECK_CURRENTPA,FALSE);
		GetDlgItem(IDC_CHECK_CURENTSA2)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CURRENTPA)->EnableWindow(FALSE);
		
	}
	else
	{
		UpdateData();
		GetDlgItem(IDC_CHECK_CURENTSA2)->EnableWindow(m_bFilter && m_bCurrentZdjhAvSA);
		GetDlgItem(IDC_CHECK_CURRENTPA)->EnableWindow(m_bFilter && m_bCurrentZdjhAvPA);
		
	}
   //CString strTmp;
   //CString strTmp2;

   //_variant_t vTmp;
   if(!m_bIsSelPA)
   {
	   LoadListPA();
	   if(m_comboPA.GetCount()>0)
			m_comboPA.SelectString(-1,GetResStr(IDS_ALL));
	   else
			m_comboPA.AddString(GetResStr(IDS_ALL));
   }
   if(!m_bIsSelSA)
   {
		LoadListSA();
	   if(m_comboSA.GetCount()>0)
			m_comboSA.SelectString(-1,GetResStr(IDS_ALL));
	   else
			m_comboSA.AddString(GetResStr(IDS_ALL));
   }
}

void CSelTemplate::GetFilterStr()
{
	UpdateData();
	try
	{
		CString sTmp,strTmp;
		_variant_t vTmp;
		
		//不选择过滤器时，所有选项框、单选纽均不可用
		//列表中当前根部IDC_CHECK_CURENTSA2
		m_isfrist=m_isfrist++;
//		if(m_isfrist==1)
//        GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
//		else
//        GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CURENTSA2)->EnableWindow(m_bFilter && m_bCurrentZdjhAvSA);
		GetDlgItem(IDC_CHECK_CURRENTPA)->EnableWindow(m_bFilter && m_bCurrentZdjhAvPA);
		GetDlgItem(IDC_CHECK_CURRENTZDJHSA2)->EnableWindow(m_bFilter && m_bCurrentZdjhAvSA);
		GetDlgItem(IDC_CHECK_CURRENTZDJHPA)->EnableWindow(m_bFilter && m_bCurrentZdjhAvPA);
		GetDlgItem(IDC_CHECK_CURRENTZDJHAVSA)->EnableWindow(m_bFilter);
		GetDlgItem(IDC_CHECK_CURRENTZDJHAVPA)->EnableWindow(m_bFilter);
		GetDlgItem(IDC_CHECK_AUTOSPRNUM2)->EnableWindow(m_bFilter);
		GetDlgItem(IDC_CHECK_DH2)->EnableWindow(m_bFilter);
		GetDlgItem(IDC_CHECK_SH2)->EnableWindow(m_bFilter);
		GetDlgItem(IDC_CHECK_SABEAM2)->EnableWindow(m_bFilter);
		GetDlgItem(IDC_CHECK_SACANT2)->EnableWindow(m_bFilter);
		GetDlgItem(IDC_CHECK_SALBRACE2)->EnableWindow(m_bFilter);
		GetDlgItem(IDC_COMBO_RODNUM)->EnableWindow(m_bFilter);
		GetDlgItem(IDC_COMBO_SPRNUM)->EnableWindow(m_bFilter);
		GetDlgItem(IDC_COMBO_CSPRNUM)->EnableWindow(m_bFilter);
		GetDlgItem(IDC_CHECK_SAGCEMENT)->EnableWindow(m_bFilter);
		GetDlgItem(IDC_COMBO_PA)->EnableWindow(m_bFilter);
		GetDlgItem(IDC_COMBO_SA)->EnableWindow(m_bFilter);
       GetDlgItem(IDC_CHECK11)->EnableWindow(m_bFilter);

		//过滤器不选择时，过滤字符串置空，以便显示全部模板
		m_strFilter="";
		if(this->m_bFilter==FALSE)
		{	
			if( m_iDefaultTemplateStatus==0 )//默认常用模板
			{
				m_strFilter=" bAllowUse is not null AND bAllowUse <>0 AND [DefaultFavoriteTemplate]=-1";
			}
			else if( m_iDefaultTemplateStatus==1 )//常用模板
			{
				m_strFilter = " bAllowUse is not null AND bAllowUse <>0 AND [Favorite]=-1";
			}
			else//显示全部模板
			{
				m_strFilter=" bAllowUse is not null AND bAllowUse <>0 ";
			}
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
						m_nCSPRNum=GetResStr(IDS_Great0);
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
								m_nSprNum=GetResStr(IDS_Great0);
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
								m_nSprNum=GetResStr(IDS_Great0);
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
		GetDlgItem(IDC_CHECK_DH2)->EnableWindow(/*!m_bCurrentPA &&*/ !m_bCurrentZdjhPA || m_bAutoSprNum/*&& !m_bCurrentZdjhAvPA*/);
		GetDlgItem(IDC_CHECK_SH2)->EnableWindow(/*!m_bCurrentPA &&*/ !m_bCurrentZdjhPA || m_bAutoSprNum/*&& !m_bCurrentZdjhAvPA*/);
		
		if(m_bCurrentPA)
		{
			//按模板列表框中当前管部选择
			m_rsTemplateName->get_Collect((_variant_t)"PA", &vTmp);
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
				strSelPA=GetResStr(IDS_ALL);
				int ix=m_comboPA.GetCurSel();
				if(ix!=-1)
					m_comboPA.GetLBText(ix,strSelPA);
				if (strSelPA!=GetResStr(IDS_ALL) && m_bIsSelPA)
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
					m_bIsSelPA=FALSE;
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
				m_strFilter += "( PA IN (Select ID FROM PictureClipData IN \'" + EDIBgbl::GetDBName(EDIBgbl::dbPRJ) + 
					"\' WHERE CustomID IN ( Select CustomID FROM [" + modPHScal::tbnPA + "] IN \"\" [\; DATABASE=" 
									+ EDIBgbl::GetDBName(modPHScal::dbZDJcrude) + " ;PWD=" + ModEncrypt::gstrDBZdjCrudePassWord + "]WHERE (Pmax >=" 
									+ ftos(tmppjg) + " AND PictureClipData.ClassID<>" + ltos(iPAfixZ1) + " AND PictureClipData.ClassID<>" + ltos(iPAfixZ2) 
									+") AND (Dw >= " + ftos(modPHScal::dj* (1 - modPHScal::gnDW_delta * 0.01))
									+ " AND Dw <= " + ftos(modPHScal::dj* (1 + modPHScal::gnDW_delta * 0.01)) 
									+ " AND Dw>0 OR Dw IS NULL OR Dw=0) AND tj >= " + ftos(modPHScal::t0) 
									+ " )))";
			}
			else 
			{
				//2007.09.30(start)
				if( modPHScal::gbPAForceZero )
				{
					m_strFilter += "( PA IN (Select ID FROM PictureClipData IN \'" + EDIBgbl::GetDBName(EDIBgbl::dbPRJ) + 
						"\' WHERE CustomID IN ( Select CustomID FROM [" + modPHScal::tbnPA + "] IN \"\" [\; DATABASE=" 
										+ EDIBgbl::GetDBName(modPHScal::dbZDJcrude) + " ;PWD=" + ModEncrypt::gstrDBZdjCrudePassWord + "]WHERE ( ( Pmax IS NULL OR Pmax >=" 
										+ ftos(tmppjg) + " ) AND PictureClipData.ClassID<>" + ltos(iPAfixZ1) + " AND PictureClipData.ClassID<>" + ltos(iPAfixZ2) 
										+") AND (Dw >= " + ftos(modPHScal::dj* (1 - modPHScal::gnDW_delta * 0.01))
										+ " AND Dw <= " + ftos(modPHScal::dj* (1 + modPHScal::gnDW_delta * 0.01)) 
										+ " AND Dw>0 OR Dw IS NULL OR Dw=0) AND tj >= " + ftos(modPHScal::t0) 
										+ " )) OR PA IN (\'Z1\',\'Z2\',\'Z2A\') )";
				}
				else
				{
					m_strFilter += "( PA IN (Select ID FROM PictureClipData IN \'" + EDIBgbl::GetDBName(EDIBgbl::dbPRJ) + 
						"\' WHERE CustomID IN ( Select CustomID FROM [" + modPHScal::tbnPA + "] IN \"\" [\; DATABASE=" 
										+ EDIBgbl::GetDBName(modPHScal::dbZDJcrude) + " ;PWD=" + ModEncrypt::gstrDBZdjCrudePassWord + "]WHERE (Pmax >=" 
										+ ftos(tmppjg) + " AND PictureClipData.ClassID<>" + ltos(iPAfixZ1) + " AND PictureClipData.ClassID<>" + ltos(iPAfixZ2) 
										+") AND (Dw >= " + ftos(modPHScal::dj* (1 - modPHScal::gnDW_delta * 0.01))
										+ " AND Dw <= " + ftos(modPHScal::dj* (1 + modPHScal::gnDW_delta * 0.01)) 
										+ " AND Dw>0 OR Dw IS NULL OR Dw=0) AND tj >= " + ftos(modPHScal::t0) 
										+ " )) OR PA IN (\'Z1\',\'Z2\',\'Z2A\') )";
				}
				//2007.09.30(end)
			}
		}
		//不按当前支吊架管部选择，双吊、单吊两个按钮可任意组合选择模板
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
				m_strFilter+=" ( PA IN (SELECT ID FROM PictureClipData IN \'" + EDIBgbl::GetDBName(EDIBgbl::dbPRJ) + "\' WHERE " 
								+ strTmp + ")) ";
			}
		}
		
		//简支、悬臂、三角架、混凝土梁四组复选框可组合
		GetDlgItem(IDC_CHECK_SABEAM2)->EnableWindow(/*!m_bCurrentSA && */!m_bCurrentZdjhSA /*&& !m_bCurrentZdjhAvSA*/);
		GetDlgItem(IDC_CHECK_SACANT2)->EnableWindow(/*!m_bCurrentSA && */!m_bCurrentZdjhSA /*&& !m_bCurrentZdjhAvSA*/);
		GetDlgItem(IDC_CHECK_SALBRACE2)->EnableWindow(/*!m_bCurrentSA && */!m_bCurrentZdjhSA /*&& !m_bCurrentZdjhAvSA*/);
		GetDlgItem(IDC_CHECK_SAGCEMENT)->EnableWindow(/*!m_bCurrentSA &&*/ !m_bCurrentZdjhSA /*&& !m_bCurrentZdjhAvSA*/);
		
		if(m_bCurrentSA)
		{
			//按模板列表框中当前根部选择
			m_rsTemplateName->get_Collect((_variant_t)"SA", &vTmp);
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
				strSelSA=GetResStr(IDS_ALL);
				int ix=m_comboSA.GetCurSel();
				if(ix!=-1)
					m_comboSA.GetLBText(ix,strSelSA);
				if (strSelSA!=GetResStr(IDS_ALL) && m_bIsSelSA)
				{
					//选择了某种根部型式,根部型式组合框中不是"全部"字符串
					
					if(m_strFilter!="")
						m_strFilter += " AND ";
					m_strFilter+=" SA = \'" + modPHScal::sFindID(strSelSA) + "\'";
				}
				else
				{
					//没有选择某种根部型式,根部型式组合框中是"全部"字符串
					m_bIsSelSA=FALSE;
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
			m_strFilter += " ( SA IN ( Select ID FROM PictureClipData IN \'" + EDIBgbl::GetDBName(EDIBgbl::dbPRJ) + "\'"
								+ " WHERE EXISTS ( Select CustomID FROM [" + modPHScal::tbnSA + "] IN \"\" [; DATABASE=" 
								+ EDIBgbl::GetDBName(modPHScal::dbZDJcrude) + " ;PWD=" + ModEncrypt::gstrDBZdjCrudePassWord + "] WHERE PictureClipData.CustomID = CustomID AND (PictureClipData.ClassID= " + ltos(iG100) + " OR PictureClipData.ClassID = " + ltos(iSALbraceFixG47) + " OR PictureClipData.ClassID = " + ltos(iSALbraceFixG48) + " OR PMAXH >=" 
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
				m_strFilter+="( SA IN (SELECT ID FROM PictureClipData IN \'" + EDIBgbl::GetDBName(EDIBgbl::dbPRJ) + "\' WHERE " 
								+ strTmp + ") )";
			}
		}

		//当选择弹簧串联数“大于 0”，联锁恒力弹簧串联数=0
		CString iSprNum,iRodNum, iCSprNum;
		if(m_nSprNum.Find(GetResStr(IDS_Great))!=-1)
			iSprNum = " iNumSpr > 0 ";
		else if(m_nSprNum.Find(_T(">= 0"))!=-1)
			iSprNum = " iNumSpr >= 0 ";
		else
			iSprNum.Format(" iNumSpr = %d ", atoi(m_nSprNum));
		//当选择恒力弹簧串联数“大于 0”，联锁弹簧串联数=0
		if(m_nCSPRNum.Find(GetResStr(IDS_Great))!=-1)
			iCSprNum = " iNumCSpr > 0 ";
		else if(m_nCSPRNum.Find(_T(">= 0"))!=-1)
			iCSprNum = " iNumCSpr >= 0 ";
		else
			iCSprNum.Format(" iNumCSpr = %d ", atoi(m_nCSPRNum));
		//选择拉杆串联数“大于 0”，过滤出所有吊架模板
		if(m_nRodNum.Find(GetResStr(IDS_Great))!=-1)
			iRodNum = " iNumRod > 0 ";
		else
			iRodNum.Format(" iNumRod = %d ", atoi(m_nRodNum));

		if(m_strFilter != "") m_strFilter += " AND ";

		
		if( m_iDefaultTemplateStatus==0 )//默认常用模板
		{
			m_strFilter += " " + iSprNum + " AND " + iCSprNum + " AND " + iRodNum + " AND bAllowUse is not null AND bAllowUse <>0 AND [DefaultFavoriteTemplate]=-1";
		}
		else if( m_iDefaultTemplateStatus==1 )//常用模板
		{
			m_strFilter += " " + iSprNum + " AND " + iCSprNum + " AND " + iRodNum + " AND bAllowUse is not null AND bAllowUse <>0 AND [Favorite]=-1";
		}
		else//显示全部模板 
		{
			m_strFilter += " " + iSprNum + " AND " + iCSprNum + " AND " + iRodNum + " AND bAllowUse is not null AND bAllowUse <>0 ";
		}

		UpdateData(FALSE);
	}
	catch(CException * e)
	{
		e->ReportError();
		e->Delete();
	}
}

void CSelTemplate::OpenTemplateRs()
{
//可用于frmphsSamp窗体的数据控件选择数据
	CString strSQL;
	CString strTemp;
	UpdateData();
	m_strSortFieldName=theSortName[this->m_iSortType];

	if(m_rsTemplateName != NULL)
		m_rsTemplateName->Close();
// 	m_rsTemplateName.m_strFilter=_T("");
// 	m_rsTemplateName.m_strSort=_T("");
// 	m_rsTemplateName.m_pDatabase=&EDIBgbl::dbPRJ;

	strSQL=_T("SELECT * FROM phsStructureName ");

	this->m_strFilter.TrimLeft();
	this->m_strFilter.TrimRight();
	if(m_strFilter!=_T(""))
		strSQL +=_T(" WHERE ") + m_strFilter ;

	m_strSortFieldName.TrimLeft();
	m_strSortFieldName.TrimRight();
	if(m_strSortFieldName !=_T(""))
	{
		strSQL+=CString(_T(" ORDER BY ")) + _T( "  [") + m_strSortFieldName + _T("] ");
		if(this->m_bDesc)
			strSQL += _T(" DESC ");
	}
	m_rsTemplateName.CreateInstance(__uuidof(Recordset));
	m_rsTemplateName->Open((_bstr_t)strSQL, _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
		adOpenKeyset, adLockOptimistic, adCmdText); 
	if (m_iSaveID == -1)
		strTemp.Format(_T("SampleID=%d"),modPHScal::iSelSampleID);
	else
	{
		strTemp.Format(_T("SampleID=%d"),m_iSaveID);
		m_iSaveID = -1;
	}

	_variant_t vTmp;
	m_rsTemplateName->Find((_bstr_t)(strTemp), 0, adSearchBackward);
	if(!m_rsTemplateName->adoEOF && !m_rsTemplateName->BOF)
		m_rsTemplateName->MoveFirst();

	m_listctrlStruct.m_prsNAME = m_rsTemplateName;

	LoadListName();
	DataReposition();
}

void CSelTemplate::LoadListName()
{
/* ==================================
修改：于明辉
*/

	try{
		m_listctrlStruct.SetRedraw(FALSE);
		m_listctrlStruct.DeleteAllItems();
		m_listctrlStruct.DeleteAllColumns();
		_variant_t v;
		_variant_t book;

		m_rsTemplateName->get_Collect((_variant_t)_T("SampleID"), &v);
		long id=vtoi(v);		
		long iSelItem = m_rsTemplateName->AbsolutePosition;

		m_rsTemplateName->MoveLast();
		long count = m_rsTemplateName->RecordCount;
		//			m_rsTemplateName->MoveFirst();
//		int iSelItem=0;
		
		_RecordsetPtr rsRef;
		rsRef.CreateInstance(__uuidof(Recordset));
		CString strSQL = _T("SELECT max(MaxColNum) AS [MaxCount] FROM [SELECT Count(*) AS MaxColNum FROM PhsStructureREF GROUP BY PhsStructureREF.SampleID]. AS [ColNumTable]");
		// 计算连接件的最大数目，确定ListCtrl中的列数。

		rsRef->Open((_bstr_t)strSQL, _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
			adOpenKeyset, adLockOptimistic, adCmdText); 
		rsRef->get_Collect((_variant_t)_T("MaxCount"), &v);
		rsRef->Close();
		int nMaxCol = vtoi(v);
		//		m_nFields = nMaxCol;
		CString s;
		m_listctrlStruct.SetRedraw(FALSE);
		m_listctrlStruct.InsertColumn(0, _T("模板号"), LVCFMT_LEFT, 50);
		m_listctrlStruct.InsertColumn(1, _T("名称"), LVCFMT_LEFT, 200);
		for (int i = 1; i <= nMaxCol; i++)
		{
			s.Format("零件 %d", i);
			m_listctrlStruct.InsertColumn(i + 1,s, LVCFMT_LEFT, 40);
		}
		m_listctrlStruct.SetItemCount(count);
		m_listctrlStruct.SetRedraw();	
		
		//if(this->m_ListTmpName.GetCount()>0)
		//	this->m_ListTmpName.SetCurSel(iSelItem);
		if(m_listctrlStruct.GetItemCount()>0)
			this->m_listctrlStruct.SetItemState(iSelItem,0xffff,LVIS_SELECTED);
		
	}
	catch(CException *e)
	{
		e->ReportError();
		e->Delete();
	}

	
/*
//		m_listctrlStruct.SetRedraw(FALSE);
		m_listctrlStruct.DeleteAllItems();
		m_listctrlStruct.DeleteAllColumns();
		try{
			if(!m_rsTemplateName.IsOpen())
				return;
			if(m_rsTemplateName.IsBOF() && m_rsTemplateName.IsEOF())
				return;
			_variant_t v;
			_variant_t book;
			m_rsTemplateName.GetFieldValue(_T("SampleID"),v);
			long id=vtoi(v);
			book=m_rsTemplateName.GetBookmark();
			m_rsTemplateName.MoveLast();
			m_rsTemplateName.MoveFirst();
			int iSelItem=0;
			
			LVITEM item;
			item.mask=LVIF_TEXT;
			item.iItem=0;
			item.iSubItem=0;
			
			LVCOLUMN column;
			column.mask=LVCF_TEXT|LVCF_WIDTH;
			column.pszText=_T("");
			column.cx=200;
			m_listctrlStruct.InsertColumn(0,&column);
			
			column.cx=40;
			
			int iColCount=1;
			CString strSQL;
			strSQL=_T("SELECT SampleID, CustomID FROM PhsStructureREF ORDER BY SampleID,SEQ");
			CDaoRecordset rsRef(&EDIBgbl::dbPRJ);
			rsRef.Open(dbOpenSnapshot,strSQL);
					int count = rsRef.GetRecordCount();
			for(;!m_rsTemplateName.IsEOF();m_rsTemplateName.MoveNext())
			{
				int iItem;
				item.iSubItem=0;
				CString strTemp;
				m_rsTemplateName.GetFieldValue(_T("SampleName"),v);
				strTemp=vtos(v);
				item.pszText=(LPTSTR)(LPCTSTR)strTemp;
				iItem=m_listctrlStruct.InsertItem(&item);
				item.iSubItem++;
				
				m_rsTemplateName.GetFieldValue(_T("SampleID"),v);
				
				int iSID=vtoi(v);
				m_listctrlStruct.SetItemData(iItem,(DWORD)(long)iSID);
				
				if(iSID==id)
					iSelItem=iItem;
				
				CString strFind;
				strFind.Format(_T("SampleID=%d"),iSID);
				if(rsRef.FindFirst(strFind))
				{
					int count = rsRef.GetRecordCount();
					while(!rsRef.IsEOF())
					{
						rsRef.GetFieldValue(_T("SampleID"),v);
						int iSempID=vtoi(v);
						if(iSempID!=iSID)
							break;
						rsRef.GetFieldValue(_T("CustomID"),v);
						CString strPar=vtos(v);
						if(iColCount<=item.iSubItem)
						{
							m_listctrlStruct.InsertColumn(item.iSubItem,&column);
							iColCount=item.iSubItem+1;
						}
						item.pszText=(LPTSTR)(LPCTSTR)strPar;
						this->m_listctrlStruct.SetItem(&item);
						item.iSubItem++;
						rsRef.MoveNext();
					}
				}
				item.iItem++;
			}
			m_rsTemplateName.SetBookmark(book);
			//if(this->m_ListTmpName.GetCount()>0)
			//	this->m_ListTmpName.SetCurSel(iSelItem);
			if(m_listctrlStruct.GetItemCount()>0)
				this->m_listctrlStruct.SetItemState(iSelItem,0xffff,LVIS_SELECTED);
			
		}
		catch(::CDaoException * e)
		{
			e->ReportError();
			e->Delete();
		}
*/
}

void CSelTemplate::LoadListPA()
{
	CString strSQL;
	_RecordsetPtr rs;
	rs.CreateInstance(__uuidof(Recordset));
	m_bLoadPA=TRUE;
	try
	{
		m_comboPA.ResetContent();
		m_comboPA.AddString(GetResStr(IDS_ALL));
		strSQL="SELECT DISTINCT [PA] FROM [phsStructureName]";
		if(m_strFilter!="")
			strSQL+=" WHERE " + m_strFilter;

		_variant_t vTmp;
		rs->Open((_bstr_t)strSQL, _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
			adOpenKeyset, adLockOptimistic, adCmdText); 
		while(!rs->adoEOF)
		{
			rs->get_Collect((_variant_t)0L, &vTmp);
			CString sTmp=modPHScal::sFindCustomID(vtos(vTmp));
			if (sTmp!="")
				m_comboPA.AddString(sTmp);
			rs->MoveNext();
		}
	}
	catch(CException *e)
	{
		e->ReportError();
		e->Delete();
	}
	m_bLoadPA=FALSE;
}

void CSelTemplate::LoadListSA()
{
	CString strSQL;
	_RecordsetPtr rs;
	rs.CreateInstance(__uuidof(Recordset));
	m_bLoadSA=TRUE;
	try
	{
		m_comboSA.ResetContent();
		m_comboSA.AddString(GetResStr(IDS_ALL));
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
		_variant_t vTmp;
		rs->Open((_bstr_t)strSQL, _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
			adOpenKeyset, adLockOptimistic, adCmdText); 
		while(!rs->adoEOF)
		{
			rs->get_Collect((_variant_t)0L, &vTmp);
			CString sTmp=modPHScal::sFindCustomID(vtos(vTmp));
			if (sTmp!="")
				m_comboSA.AddString(sTmp);
			rs->MoveNext();
		}
	}
	catch(CException *e)
	{
		e->ReportError();
		e->Delete();
	}
	m_bLoadSA=FALSE;
}

void CSelTemplate::OnRadio1() 
{
	// TODO: Add your control notification handler code here
	this->OpenTemplateRs();
}

void CSelTemplate::OnCheckDesc() 
{
	// TODO: Add your control notification handler code here
	this->OpenTemplateRs();
}

void CSelTemplate::OnCheckFilter() 
{
	// TODO: Add your control notification handler code here
	UpdateRecordset();
}

void CSelTemplate::OnCheckCurrentzdjhavpa() 
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
		UpdateData(FALSE);
	}
	else
	{
		GetDlgItem(IDC_CHECK_CURRENTPA)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CURRENTZDJHPA)->EnableWindow(TRUE);
	}
	m_bIsSelPA=FALSE;
	UpdateRecordset();
}

void CSelTemplate::OnCheckCurrentPA() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if(m_bCurrentPA)
	{
		m_bCurrentZdjhPA=FALSE;
		//m_bCurrentZdjhAvPA=FALSE;
		UpdateData(FALSE);
	}
	m_bIsSelPA=FALSE;
	UpdateRecordset();
}

void CSelTemplate::OnCheckCurrentZdjhPA() 
{
	// TODO: Add your control notification handler code here
	m_bIsSelPA=FALSE;
	UpdateRecordset();
}

void CSelTemplate::OnCheckDh() 
{
	// TODO: Add your control notification handler code here
	m_bIsSelPA=FALSE;
	UpdateRecordset();
}

void CSelTemplate::OnCheckSh() 
{
	// TODO: Add your control notification handler code here
	m_bIsSelPA=FALSE;
	UpdateRecordset();
}

void CSelTemplate::OnSelchangeComboPA() 
{
	// TODO: Add your control notification handler code here
	if(!m_bLoadPA)
	{
		m_bIsSelPA=TRUE;
		UpdateRecordset();
	}
}

void CSelTemplate::OnSelchangeComboSA() 
{
	// TODO: Add your control notification handler code here
	if(!m_bLoadSA)
	{
		m_bIsSelSA=TRUE;
		UpdateRecordset();
	}
}

void CSelTemplate::OnCheckCurrentzdjhavSA() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if(!m_bCurrentZdjhAvSA)
	{
		m_bCurrentSA=FALSE;
		m_bCurrentZdjhSA=FALSE;
		CheckDlgButton(IDC_CHECK_CURENTSA2,FALSE);
		CheckDlgButton(IDC_CHECK_CURRENTZDJHSA2,FALSE);
		GetDlgItem(IDC_CHECK_CURENTSA2)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CURRENTZDJHSA2)->EnableWindow(FALSE);
		UpdateData(FALSE);
	}
	else
	{
		GetDlgItem(IDC_CHECK_CURENTSA2)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CURRENTZDJHSA2)->EnableWindow(TRUE);
	}
	m_bIsSelSA=FALSE;
	UpdateRecordset();
}

void CSelTemplate::OnCheckCurentSA() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if(m_bCurrentSA)
	{
		m_bCurrentZdjhSA=FALSE;
		//m_bCurrentZdjhAvSA=FALSE;
		UpdateData(FALSE);
	}
	m_bIsSelSA=FALSE;
	UpdateRecordset();
}

void CSelTemplate::OnCheckCurrentzdjhSA() 
{
	// TODO: Add your control notification handler code here
	m_bIsSelSA=FALSE;
	UpdateRecordset();
}

void CSelTemplate::OnCheckSabeam() 
{
	// TODO: Add your control notification handler code here
	m_bIsSelSA=FALSE;
	UpdateRecordset();
}

void CSelTemplate::OnCheckSacant() 
{
	// TODO: Add your control notification handler code here
	m_bIsSelSA=FALSE;
	UpdateRecordset();
}

void CSelTemplate::OnCheckSalbrace() 
{
	// TODO: Add your control notification handler code here
	m_bIsSelSA=FALSE;
	UpdateRecordset();
}

void CSelTemplate::OnCheckSagcement() 
{
	// TODO: Add your control notification handler code here
	m_bIsSelSA=FALSE;
	UpdateRecordset();
}

void CSelTemplate::OnCheckAutosprnum() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if(m_bAutoSprNum)
	{
	
	}
	UpdateRecordset();
}

void CSelTemplate::OnSelchangeComboSprnum() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if(m_nSprNum != "0" )
	{
		if(m_nSprNum == ">= 0")
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
	UpdateData(FALSE);
	UpdateRecordset();
}

void CSelTemplate::OnSelchangeComboCsprnum() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if(m_nCSPRNum != "0" && m_nCSPRNum != ">= 0")
	{
		m_nSprNum="0";
	}
	m_bAutoSprNum=FALSE;
	UpdateData(FALSE);
	UpdateRecordset();
}

BOOL CSelTemplate::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	int i;
	m_iLastSampleID=modPHScal::iSelSampleID;
	m_frmBmpSe=new CBmpSeWnd(this);
	this->m_frmBmpSe->Create(IDD_BMPSE,AfxGetMainWnd());
	GetDlgItem(IDC_CHECK_PA)->EnableWindow(FALSE);
	for(i=0 ;i<6;i++)
		GetDlgItem(m_CheckID[i])->EnableWindow(FALSE);
	UpdateData();
	m_bRodNum=true;
	UpdateData(FALSE);
    int a = modPHScal::SumLugNum();
	((CComboBox*)GetDlgItem(IDC_COMBO_RODNUM))->SetCurSel(a);

    UpdateRecordset();

	//m_btnFavoriteTemplate.ShowWindow(SW_HIDE);    //LFX 2005.3.25注
	m_btnFavoriteTemplate.ShowWindow(SW_SHOW); 
	((CButton*)GetDlgItem(IDC_RADIO_ALL_TEMPLATE))->SetCheck(1);

	m_bIsOpenDefaultFavoriteTemplate = GetRegKey("Status","IsOpenDefaultFavorite",0);  //LFX 2005.3.25 注
	if( m_bIsOpenDefaultFavoriteTemplate )
		m_btnDefaultFavoriteTemplate.ShowWindow(SW_SHOW);
	else
		m_btnDefaultFavoriteTemplate.ShowWindow(SW_HIDE);

	m_listctrlStruct.SetExtendedStyle(m_listctrlStruct.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES |LVS_AUTOARRANGE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSelTemplate::DataReposition()
{
	//样本图使用频度
	CString strSQL,strTemp;
	UpdateData();
	try{
		_variant_t v;
		
		m_lstStruct.ResetContent();   //LFX 2005.3.25	加   
		m_lstPart.ResetContent();  //LFX 2005.3.25	加

		//记录集为空是因为在现有过滤串条件下没有符合的模板可供选择，所以在这直接返回没问题
		if( m_rsTemplateName->BOF && m_rsTemplateName->adoEOF )
		{
			 //LFX 2005.3.25 加   显示正确的当前信息
			strTemp.Format(IDS_SelSampleIDNoXSumXFRQx,0,0,0,0);    
			GetDlgItem(IDC_STATIC_CAPTION)->SetWindowText(strTemp); 
			return;   //原
			//END //LFX 2005.3.25 加
		}

		m_rsTemplateName->get_Collect((_variant_t)"SampleID", &v);
		//if(vtoi(v) == m_iCurSampleID)  LFX 注掉
		if (false)    //LFX  不执行下面语句，避免结构框不刷新
		{
			m_rsTemplateName->get_Collect((_variant_t)"FREQUENCE", &v);   //LFX 2005.3.25   加
			strTemp.Format(IDS_SelSampleIDNoXSumXFRQx,m_iCurSampleID,m_rsTemplateName->AbsolutePosition+1,m_rsTemplateName->RecordCount,vtoi(v)); //LFX 2005.3.25 加
			return;   //原
		}
		this->m_iCurSampleID = vtoi(v);
		//FrameSelPhs.Caption = "选择支吊架组装模板" & modPHScal::iSelSampleID & " (第" & (m_rsTemplateName.Recordset.AbsolutePosition + 1) & "项 共" & m_rsTemplateName.Recordset.RecordCount & "项 使用频度" & m_rsTemplateName.Recordset.Fields("FREQUENCE") & ")"
		m_rsTemplateName->get_Collect((_variant_t)"FREQUENCE", &v);
		strTemp.Format(IDS_SelSampleIDNoXSumXFRQx,m_iCurSampleID,m_rsTemplateName->AbsolutePosition+1,m_rsTemplateName->RecordCount,vtoi(v));
		GetDlgItem(IDC_STATIC_CAPTION)->SetWindowText(strTemp);
		m_rsTemplateName->get_Collect((_variant_t)"SampleName", &v);
		//if(v.vt!=VT_NULL)
		//	sBmpName = basDirectory::TemplateDir + vtos(v);
		//if(sBmpName.Find(".dwg",0)==-1)
		//	sBmpName+=".dwg";
		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(Recordset));
		strSQL.Format(_T("SELECT CustomID FROM phsStructureREF WHERE SampleID=%d ORDER BY SEQ"),
			this->m_iCurSampleID);
		rs->Open((_bstr_t)strSQL, _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
			adOpenKeyset, adLockOptimistic, adCmdText); 
		m_lstStruct.ResetContent();
		while(!rs->adoEOF)
		{
			rs->get_Collect((_variant_t)"CustomID", &v);
			if(vtos(v)!="")
				m_lstStruct.AddString(vtos(v));
			rs->MoveNext();
		}
		this->m_bSaveChange=TRUE;
		this->GetDlgItem(IDC_CMD_SAVE)->EnableWindow(FALSE);
		rs->Close();
		Cavphs->SourceObj = &m_lstStruct;
	}
	catch(CException *e)
	{
		e->ReportError();
		e->Delete();
	}
	UpdateData(FALSE);
}



void CSelTemplate::OnSelchangeListStruct() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	int i;
	if(m_lstStruct.GetCurSel()<=0 )
	{
		GetDlgItem(IDC_CHECK_PA)->EnableWindow(TRUE);
	 }
	 else
	 {
			m_bRePA=FALSE;
			for(i=0 ;i<6;i++)
			{
				m_CheckCondition[i]=FALSE;
			}
			UpdateData(FALSE);
			GetDlgItem(IDC_CHECK_PA)->EnableWindow(FALSE);
			for(i=0 ;i<6;i++)
				GetDlgItem(m_CheckID[i])->EnableWindow(FALSE);
	 }
    ListZDGInit();
    if (m_itmpPreviousIndex < 0 || 
		m_itmpPreviousIndex != m_lstStruct.GetCurSel())
        m_itmpPreviousIndex = m_lstStruct.GetCurSel();
    UpdateData(FALSE);
    Bmpshow();
}

void CSelTemplate::ListZDGInit()
{
	CWaitCursor wait;
	int i,k;
	UpdateData();
	m_dwChkCondition=CalChkCondition();
	Cavphs->iSAPAConnect = 10;
	m_lstPart.ShowWindow(SW_SHOW);
	CString sTmp;
	if( m_lstStruct.GetCurSel() < 0 )
	{
		Cavphs->ResultObj = &m_lstPart;
		Cavphs->SourceObj = &m_lstStruct;
		Cavphs->iChkCondition = m_dwChkCondition;
		Cavphs->iSAPAConnect = iPA;
		Cavphs->Cntb = "";
		i = Cavphs->SetPhsTypeToListBox();
		if( m_dwChkCondition != 0 ) i = Cavphs->SetPhsCheckedTypeToListBox();
		if(m_lstStruct.GetCount()>0)
		{
			m_lstStruct.GetText(0,sTmp);
			k=m_lstPart.FindStringExact(-1,sTmp);
			if(k>=0)
				m_lstPart.SetCurSel(k);
		}
		//k = CheckListBoxItem(&m_lstPart, sTmp)
		//提示改换规范再试
		if( m_lstPart.GetCount() <= 0 ){
			sTmp.Format(GetResStr(IDS_PleaseChangeOtherSPECretry),modPHScal::gsPhsPASel);

			if( MessageBox(sTmp,NULL,MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON1|MB_TOPMOST)==IDYES)
			{
				//FrmSelSpecification.DoModal();
				ShowModalSelSpecification(FALSE);
			}
		}
	}
	else if( m_lstStruct.GetCurSel() >= 0 )
	{
		m_lstStruct.GetText(0,sTmp);
		Cavphs->Cntb = sTmp;
		if(!Cavphs->bFirstPartIsPA() || (m_lstStruct.GetCurSel() == 0 && Cavphs->bFirstPartIsPA() && m_bRePA==TRUE) )
		{
			Cavphs->ResultObj = &m_lstPart;
			Cavphs->SourceObj = &m_lstStruct;
			Cavphs->iChkCondition = m_dwChkCondition;
			Cavphs->iSAPAConnect = iPA;
			Cavphs->Cntb = "";
			i = Cavphs->SetPhsTypeToListBox();
			if(!i)
			{
				Cavphs->ResultObj->ResetContent();
			}//增加了上述条件语句。pfg2006.12.29
			if( m_dwChkCondition != 0 && i) i = Cavphs->SetPhsCheckedTypeToListBox();//增加"&& i" pfg2006.12.29
			int index = m_lstStruct.GetCurSel()+1;
			int count = m_lstStruct.GetCount();
			if( index < count )
				m_lstStruct.GetText(index,sTmp);
			else
				sTmp = _T("");
			
			//k = CheckListBoxItem(&m_lstPart, sTmp);
			k=m_lstPart.FindStringExact(-1,sTmp);
			if(k>=0)
				m_lstPart.SetCurSel(k);
			//k = CheckListBoxItem(&m_lstPart, sTmp)
			//提示改换规范再试
			if( m_lstPart.GetCount() <= 0 )
			{
				sTmp.Format(GetResStr(IDS_PleaseChangeOtherSPECretry),modPHScal::gsPhsPASel);
				if( MessageBox(sTmp,NULL,MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON1|MB_TOPMOST)==IDYES)
					ShowModalSelSpecification(FALSE);
			}
		}
		else
		{
			if( m_lstStruct.GetCurSel() >= 1 )
			{
				int iTemp=m_lstStruct.GetCurSel();
				m_lstStruct.GetText(iTemp-1,sTmp);
				Cavphs->CntbP =sTmp;
			}
			else
				Cavphs->CntbP = "";
			int ix=m_lstStruct.GetCurSel();
			CString sTmp;
			m_lstStruct.GetText(ix,sTmp);
			Cavphs->Cntb =sTmp;
			Cavphs->ResultObj = &m_lstPart;
			Cavphs->SourceObj = &m_lstStruct;
			Cavphs->iChkCondition = m_dwChkCondition;
			Cavphs->SetPhsTypeToListBox();
			if( m_dwChkCondition != 0 )
				Cavphs->SetPhsCheckedTypeToListBox();
			if(m_lstStruct.GetCurSel() < (m_lstStruct.GetCount()-1))
				m_lstStruct.GetText(m_lstStruct.GetCurSel()+1,sTmp);
			else sTmp="";
			//k = CheckListBoxItem(&m_lstPart, sTmp);
			k=m_lstPart.FindStringExact(-1,sTmp);
			if(k>=0)
				m_lstPart.SetCurSel(k);
			//}
		}
	}
	this->GetDlgItem(IDC_CMD_DEL)->EnableWindow(m_lstStruct.GetCount() > 0);
	this->GetDlgItem(IDC_CMD_ADD)->EnableWindow( m_lstPart.GetCount() > 0);
}

DWORD CSelTemplate::CalChkCondition()
{
   DWORD res = 0;
   for(int i=0;i<=5;i++)
   {
		if(m_CheckCondition[i])
			res |= 0x1 << i;
	}
   return res;
}

void CSelTemplate::Bmpshow()
{
	if (! m_bShowImg )
   {
		m_frmBmpSe->ShowWindow(SW_HIDE);
        return;
   }
   int ix=this->m_lstStruct.GetCurSel();
   m_frmBmpSe->ShowWindow(SW_SHOW);
   ::SetWindowPos(m_frmBmpSe->GetSafeHwnd(),HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
   if(ix!=m_iOldIxEDIT || m_bRefImg)
   {
		m_iOldIxEDIT=ix;
		m_frmBmpSe->InitList();
   }
	ix=this->m_lstPart.GetCurSel();
	if(ix!=m_iOldIxZDG || m_bRefImg)
	{
		m_iOldIxZDG=ix;
		if(ix>=0 && ix<m_frmBmpSe->m_pDlgImgLst->m_pDlgImgLst->m_imgCount)
			m_frmBmpSe->m_pDlgImgLst->m_pDlgImgLst->m_Image[ix]->SendMessage(WM_LBUTTONUP);
	}
	m_bRefImg=FALSE;
}

void CSelTemplate::OnSelchangeListPart() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if(m_bShowImg)
	{
		int ix=this->m_lstPart.GetCurSel();
		if(ix!=m_iOldIxZDG)
		{
			m_iOldIxZDG=ix;
			if(ix>=0 && ix<m_frmBmpSe->m_pDlgImgLst->m_pDlgImgLst->m_imgCount)
				m_frmBmpSe->m_pDlgImgLst->m_pDlgImgLst->m_Image[ix]->SendMessage(WM_LBUTTONUP);
		}
    UpdateData(FALSE);
    Bmpshow();
	}
}

void CSelTemplate::OnCmdAdd() 
{
	//LFX 2005.3.25 加  防止在列表框中没有记录的情况下出现的异常情况
	if (m_lstPart.GetCount() < 1 || m_lstStruct.GetCount() < 1)
	{
		return;
	}
	//END LFX 2005.3.25 加  
	UpdateData();
	GetDlgItem(IDC_CMD_SAVE)->EnableWindow(TRUE);
	m_bSaveChange=FALSE;

	if(m_lstPart.GetCurSel()<0)      
		 m_lstPart.SetCurSel(0);
	int ix=m_lstPart.GetCurSel();
	CString sTmp;
	m_lstPart.GetText(ix,sTmp);
	if(m_lstStruct.GetCurSel()<0)
		m_lstStruct.AddString(sTmp);
	else
	{
		ix=m_lstStruct.GetCurSel();
		ix++;
		m_lstStruct.InsertString(ix,sTmp);
		m_lstStruct.SetCurSel(ix);
	}
	OnSelchangeListStruct();
}

void CSelTemplate::OnCmdDel() 
{
	// TODO: Add your control notification handler code here

	//LFX 2005.3.25 加  防止在列表框中没有记录的情况下出现的异常情况
	if (m_lstStruct.GetCount() < 1)
	{
		return;
	}
	//END LFX 2005.3.25 加 

	this->GetDlgItem(IDC_CMD_SAVE)->EnableWindow(true);
	//this->GetDlgItem(IDC_CMD_APPLY)->EnableWindow(FALSE);
	m_bSaveChange=FALSE;
	if( m_lstStruct.GetCount()>0 && m_lstStruct.GetCurSel() >= 0 )
	{
		int ix=m_lstStruct.GetCurSel();
		m_lstStruct.DeleteString(ix);
		if(ix>=1)
		{
			ix--;
			m_lstStruct.SetCurSel(ix);
		}
		else if(ix < m_lstStruct.GetCount()-1)
		{
			m_lstStruct.SetCurSel(ix);
		}

		OnSelchangeListStruct();
	}
}

void CSelTemplate::OnCmdSave() 
{
try{	// TODO: Add your control notification handler code here
	if (m_lstStruct.GetCount() > 0 )
	{
		//检查匹配
		Cavphs->SourceObj = &m_lstStruct;
		Cavphs->ResultObj = &m_lstPart;
		int i = Cavphs->CheckMatchPhs();
		if( i == -1)
		{
			//匹配则入库
			//同时设置保存按钮为灰色状态，防止再次点击。
			GetDlgItem(IDC_CMD_SAVE)->EnableWindow(FALSE);

			// LFX 2005.3.24 改
			int iTemp; 
			iTemp = m_iDefaultTemplateStatus;
			//m_iLastSampleID = Cavphs->CheckDuplicateREFRecordWhenAppend();   //原
			m_iLastSampleID = Cavphs->CheckDuplicateREFRecordWhenAppend(&iTemp);
			if (iTemp != m_iDefaultTemplateStatus)   
			{
				m_iDefaultTemplateStatus = iTemp; 
				UpdateData(false);   
				UpdateRecordset();   
			}
			//END  LFX 2005.3.24 改


			//改名称
			CString X = Cavphs->GetPhsAssembleName(m_iLastSampleID);
			//下面2句容易造成级联事件，导致死机。
			m_rsTemplateName->Requery(adExecuteRecord);
			_variant_t vTmp;
			m_iSavex = m_rsTemplateName->Find((_bstr_t)("SampleID=" + ltos(m_iLastSampleID)), 0, adSearchBackward);
			LoadListName();
			DataReposition();
			m_bSaveChange=TRUE;

			m_btnFavoriteTemplate.ShowWindow(SW_SHOW);
			GetDlgItem(IDC_CMD_CANCEL)->EnableWindow(TRUE);
//			int iSavex=this->m_listctrlStruct.GetSelectedItem();
			if(m_iSavex == 0)//m_iSavex==-1||m_iSavex==0)
			{ //  GetDlgItem(IDC_CHECK_FILTER)->EnableWindow(FALSE);
				//	AfxMessageBox(GetResStr(IDS_PleSelecteFirstSaveAgain),MB_ICONWARNING|MB_OK);
				//        GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_CURENTSA2)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_CURRENTPA)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_CURRENTZDJHSA2)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_CURRENTZDJHPA)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_CURRENTZDJHAVSA)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_CURRENTZDJHAVPA)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_AUTOSPRNUM2)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_DH2)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_SH2)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_SABEAM2)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_SACANT2)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_SALBRACE2)->EnableWindow(FALSE);
				GetDlgItem(IDC_COMBO_RODNUM)->EnableWindow(FALSE);
				GetDlgItem(IDC_COMBO_SPRNUM)->EnableWindow(FALSE);
				GetDlgItem(IDC_COMBO_CSPRNUM)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_SAGCEMENT)->EnableWindow(FALSE);
				GetDlgItem(IDC_COMBO_PA)->EnableWindow(FALSE);
				GetDlgItem(IDC_COMBO_SA)->EnableWindow(FALSE);
                GetDlgItem(IDC_CHECK11)->EnableWindow(FALSE);

//				GetDlgItem(IDC_CMD_CANCEL)->EnableWindow(FALSE);

				CheckDlgButton(IDC_CHECK_FILTER,FALSE);
				m_iSaveID = m_iLastSampleID;
				//m_bFilter = FALSE;
				/* 	GetDlgItem(IDC_LISTCTRL_STRUCT)->*///UpdateData(FALSE);
				UpdateRecordset();
//				return;
				//		LoadListName();	
				//		GetDlgItem(IDC_CHECK_FILTER)->SetCheck(FALSE);
				
			}
//			else
				iSaveTmpID=m_iLastSampleID;//m_listctrlStruct.GetItemData(iSavex);

			if( IsFavoriteTemplate(m_iLastSampleID) )
			{//显示“不常用”
				m_btnFavoriteTemplate.SetWindowText(GetResStr(IDS_SET_NOT_FAVORITE));
			}
			else
			{//显示“常用”
				m_btnFavoriteTemplate.SetWindowText(GetResStr(IDS_SET_FAVORITE));
			}

			if( m_bIsOpenDefaultFavoriteTemplate )
				m_btnDefaultFavoriteTemplate.ShowWindow(SW_SHOW);
			else
				m_btnDefaultFavoriteTemplate.ShowWindow(SW_SHOW);

			if( IsDefaultFavoriteTemplate(m_iLastSampleID) )
			{//显示“不常用”
				m_btnDefaultFavoriteTemplate.SetWindowText(GetResStr(IDS_SET_NOT_DEFAULT_FAVORITE));
			}
			else
			{//显示“常用”
				m_btnDefaultFavoriteTemplate.SetWindowText(GetResStr(IDS_SET_DEFAULT_FAVORITE));
			}
		}

	}
}
	catch(_com_error & e)
	{
		AfxMessageBox(e.Description());
		return;
	}
}

void CSelTemplate::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();
	if(m_bSaveChange==FALSE)
	{
		AfxMessageBox(GetResStr(IDS_PleSaveFirstConfirmAgain),MB_ICONWARNING|MB_OK);
		return;
	}

	//应用：
	try
	{
		_variant_t v;
		this->m_rsTemplateName->get_Collect((_variant_t)"FREQUENCE", &v);
		int vi=vtoi(v);
		v.lVal=vi+1;
		this->m_rsTemplateName->put_Collect((_variant_t)"FREQUENCE",v);
		this->m_rsTemplateName->Update();
		//如果是支吊架原始数据表
		//MsgBox "支吊架号" & zdjh & " 组装模板号" & iSelSampleID
		//MsgBox ResolveResString(iUE_ZDJHxAndSampleIDx, "|1", zdjh, "|2", iSelSampleID)
		CString strTemp;
		long intSampleID=vtoi(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect("iSelSampleID"));
		if(intSampleID != this->m_iCurSampleID )
		{		//当标准支吊架的模板改变时，删除支吊架原来模板结构，以便重建支吊架的结构，
		
			_RecordsetPtr rsTmpZB;
			rsTmpZB.CreateInstance(__uuidof(Recordset));
			strTemp.Format("SELECT DISTINCT nth FROM ZB WHERE VolumeID = %d AND ZDJH = %d AND bUserAdd <> -1 ",EDIBgbl::SelVlmID ,modPHScal::zdjh );
			rsTmpZB->Open((_bstr_t)strTemp, _variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
				adOpenKeyset, adLockOptimistic, adCmdText); 
			int iCount;
			if(rsTmpZB->BOF && rsTmpZB->adoEOF)
				iCount=0;
			else
			{
				rsTmpZB->MoveLast();
				iCount=rsTmpZB->RecordCount;
			}
			rsTmpZB->Close();
			

			if(iCount <= 1)
			{
				strTemp.Format("DELETE * FROM ZB WHERE VolumeID = %d AND ZDJH = %d AND bUserAdd <> -1 ",EDIBgbl::SelVlmID ,modPHScal::zdjh );
				EDIBgbl::dbPRJDB->Execute((_bstr_t)strTemp, NULL, adCmdText);

				strTemp.Format("DELETE * FROM Z1 WHERE VolumeID = %d AND ZDJH = %d ",EDIBgbl::SelVlmID ,modPHScal::zdjh );
				//EDIBgbl::dbPRJDB->Execute((_bstr_t)strTemp, NULL, adCmdText);
			}
			else
			{
				if(MessageBox("确认要删除非标准的支吊架吗？","警告",MB_OKCANCEL|MB_ICONEXCLAMATION )==IDOK)
				{
					strTemp.Format("DELETE * FROM ZB WHERE VolumeID = %d AND ZDJH = %d AND bUserAdd <> -1 ",EDIBgbl::SelVlmID ,modPHScal::zdjh );
					EDIBgbl::dbPRJDB->Execute((_bstr_t)strTemp, NULL, adCmdText);
				}
			}
			
		}
		modPHScal::iSelSampleID=this->m_iCurSampleID;
		strTemp.Format(IDS_ZDJHxAndSampleIDx,modPHScal::zdjh,modPHScal::iSelSampleID);
		AfxMessageBox(strTemp,MB_OK|MB_ICONINFORMATION);
		FrmTxsr.m_pViewTxsr->m_ActiveRs->PutCollect("iSelSampleID",_variant_t((long)modPHScal::iSelSampleID));
		FrmTxsr.m_pViewTxsr->m_ActiveRs->Update();
	}
	catch(CException* e)
	{
		e->ReportError();
		e->Delete();
		return;
	}
	catch(_com_error & e)
	{
		AfxMessageBox(e.Description());
		return;
	}
	CDialog::OnOK();
}

//DEL void CSelTemplate::OnSelchangeListName() 
//DEL {
//DEL 	// TODO: Add your control notification handler code here
//DEL 	try
//DEL 	{
//DEL 	    if(!m_rsTemplateName.IsOpen())
//DEL 			return;
//DEL 
//DEL 		CString sTmp;
//DEL 		int ix=m_ListTmpName.GetCurSel();
//DEL 		CString strCurItem;
//DEL 		m_ListTmpName.GetText(ix,strCurItem);
//DEL 		if(ix<0 )
//DEL 			return;
//DEL 
//DEL 		sTmp.Format(_T("SampleID=%d"),m_ListTmpName.GetItemData(ix));
//DEL 
//DEL 		this->m_rsTemplateName.FindFirst(sTmp);
//DEL 
//DEL 		this->DataReposition();
//DEL 
//DEL 
//DEL 		//在点到相应模板时确认其是否“常用模板”
//DEL 		m_btnFavoriteTemplate.ShowWindow(SW_SHOW);
//DEL 		if( IsFavoriteTemplate(m_ListTmpName.GetItemData(ix)) )
//DEL 		{
//DEL 			m_btnFavoriteTemplate.SetWindowText(GetResStr(IDS_SET_NOT_FAVORITE));
//DEL 			m_bSetFavorite = FALSE;
//DEL 		}
//DEL 		else
//DEL 		{
//DEL 			m_btnFavoriteTemplate.SetWindowText(GetResStr(IDS_SET_FAVORITE));
//DEL 			m_bSetFavorite = TRUE;
//DEL 		}
//DEL 
//DEL 		//在点到相应模板时确认其是否“默认常用模板”
//DEL 		if( m_bIsOpenDefaultFavoriteTemplate )
//DEL 			m_btnDefaultFavoriteTemplate.ShowWindow(SW_SHOW);
//DEL 		else
//DEL 			m_btnDefaultFavoriteTemplate.ShowWindow(SW_HIDE);
//DEL 
//DEL 		if( IsDefaultFavoriteTemplate(m_ListTmpName.GetItemData(ix)) ) 
//DEL 		{
//DEL 			m_btnDefaultFavoriteTemplate.SetWindowText(GetResStr(IDS_SET_NOT_DEFAULT_FAVORITE));
//DEL 			m_bSetDefaultFavorite = FALSE;
//DEL 		}
//DEL 		else
//DEL 		{
//DEL 			m_btnDefaultFavoriteTemplate.SetWindowText(GetResStr(IDS_SET_DEFAULT_FAVORITE));
//DEL 			m_bSetDefaultFavorite = TRUE;
//DEL 		}
//DEL 
//DEL 		//将对应模板名称的所有支吊架组装结构都显示在IDC_LISTCTRL_STRUCT列表中
//DEL 		m_listctrlStruct.DeleteAllItems();
//DEL 		m_listctrlStruct.DeleteAllColumns();
//DEL 
//DEL 		CDaoRecordset rsPhsSturctureName,rsStructureREF;
//DEL 		rsPhsSturctureName.m_pDatabase = &EDIBgbl::dbSORT;
//DEL 		rsStructureREF.m_pDatabase = &EDIBgbl::dbSORT;
//DEL 		CString strSQL;
//DEL 		strSQL.Format("SELECT [SampleID] FROM PhsStructureName WHERE [SampleName]=\'%s\' ORDER BY [SampleID]",strCurItem);
//DEL 		rsPhsSturctureName.Open(dbOpenDynaset,strSQL);
//DEL 		if( rsPhsSturctureName.IsEOF() || rsPhsSturctureName.IsBOF() )
//DEL 			return;
//DEL 		for(int iCol=0; !rsPhsSturctureName.IsEOF(); rsPhsSturctureName.MoveNext(),iCol++)
//DEL 		{ 
//DEL 			strSQL.Format("SELECT [CustomID] FROM PhsStructureREF WHERE [SampleID]=%d ORDER BY SEQ",vtoi(rsPhsSturctureName.GetFieldValue("SampleID")));
//DEL 			if( rsStructureREF.IsOpen() )
//DEL 				rsStructureREF.Close();
//DEL 			rsStructureREF.Open(dbOpenDynaset,strSQL);
//DEL 			
//DEL 			//以下就往准备好IDC_LISTCTRL_STRUCT中加入支吊架组装结构
//DEL 			CString strColumn;			
//DEL 			strColumn.Format("支吊架结构%d",iCol);
//DEL 			m_listctrlStruct.AddColumn(strColumn,iCol,m_listctrlStruct.GetStringWidth(strColumn.GetBuffer(strColumn.GetLength()))+20);
//DEL 			for(int i=0; !rsStructureREF.IsEOF(); rsStructureREF.MoveNext(),i++)
//DEL 			{
//DEL 				m_listctrlStruct.AddItem(i,iCol,vtos(rsStructureREF.GetFieldValue("CustomID")));
//DEL 			}
//DEL 		}
//DEL 	}
//DEL 	catch(::CDaoException * e)
//DEL 	{
//DEL 		e->ReportError();
//DEL 		e->Delete();
//DEL 	}
//DEL }

void CSelTemplate::OnDestroy() 
{
	this->m_frmBmpSe->DestroyWindow();
	delete this->m_frmBmpSe;
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
}

void CSelTemplate::OnCheckBmp() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	Bmpshow();
}

void CSelTemplate::OnButton3() 
{
	// TODO: Add your control notification handler code here
	m_bRefImg=TRUE;
	UpdateData();
	OnSelchangeListStruct();
}

void CSelTemplate::OnCheckPa() 
{
	// TODO: Add your control notification handler code here
	UpdateData(); 
	if(m_bRePA == true)
	{
		for(int i=0;i<6;i++)
			GetDlgItem(m_CheckID[i])->EnableWindow(TRUE);
		m_CheckCondition[iChkBoxOnlyPAh]=TRUE;
		m_CheckCondition[iChkBoxDW]=TRUE;
		m_CheckCondition[iChkBoxPJG]=TRUE;
		m_CheckCondition[iChkBoxTJ]=TRUE;
		m_CheckCondition[iChkBoxGDW1]=FALSE;
		m_CheckCondition[iChkBoxCNT]=FALSE;
	}
	else{
		//GetDlgItem(CheckID[iChkBoxOnlyPAh])->EnableWindow(FALSE);
		m_CheckCondition[iChkBoxOnlyPAh]=FALSE;
		m_CheckCondition[iChkBoxOnlyPAh]=FALSE;
		m_CheckCondition[iChkBoxDW]=FALSE;
		m_CheckCondition[iChkBoxPJG]=FALSE;
		m_CheckCondition[iChkBoxTJ]=FALSE;
		m_CheckCondition[iChkBoxGDW1]=FALSE;
		m_CheckCondition[iChkBoxCNT]=FALSE;
		for(int i=0;i<6;i++)
			GetDlgItem(m_CheckID[i])->EnableWindow(FALSE);
	}
	UpdateData(FALSE);
	//如果frmListBox窗体中的ListPhsStruEDIT.ListIndex发生改变，
	//则重新设置CSBmp.PreviousIndex=0，以防止错误恢复图片。
	m_itmpPreviousIndex = -1;
	m_iOldIxEDIT=-1;
	m_iOldIxZDG=-1;
	m_bRefImg=TRUE;
	OnSelchangeListStruct();
	UpdateData(FALSE);
}

void CSelTemplate::OnCheckOnlypah() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	m_bRefImg=TRUE;
	OnSelchangeListStruct();
}

void CSelTemplate::OnDblclkListName() 
{
	SetFavoriteTemplate();
}

void CSelTemplate::OnBtnFavoriteTemplate() 
{
	SetFavoriteTemplate();
}

void CSelTemplate::SetFavoriteTemplate()
{
    //当相应模板为非常用模板时则可将其置为“常用”状态
	//否则可将其置为“不常用”状态

	CString strSQL;
	int ix=this->m_listctrlStruct.GetSelectedItem();
	if(ix==-1) return;
	int iTmpID=m_listctrlStruct.GetItemData(ix);
	if( m_bSetFavorite )
	{//将其置为“常用”
		strSQL.Format("UPDATE PhsStructureName SET [Favorite]=-1 WHERE [SampleID]=%d",
			iTmpID);
		m_bSetFavorite = FALSE;
		m_btnFavoriteTemplate.SetWindowText(GetResStr(IDS_SET_NOT_FAVORITE));
	}
	else
	{//将其置为“不常用”
		strSQL.Format("UPDATE PhsStructureName SET [Favorite]=0 WHERE [SampleID]=%d",
			iTmpID);
		m_bSetFavorite = TRUE;
		m_btnFavoriteTemplate.SetWindowText(GetResStr(IDS_SET_FAVORITE));
	}

	EDIBgbl::dbPRJ->Execute((_bstr_t)strSQL, NULL, adCmdText);
	//同时更新sort.mdb，保持同步
	EDIBgbl::dbPHScode->Execute((_bstr_t)strSQL, NULL, adCmdText);//20071101 dbSORT 改为 "dbPHScode"
}

void CSelTemplate::OnRadioFavoriteTemplate() 
{
	//选择常用模板

	UpdateData();
	UpdateRecordset();
}

void CSelTemplate::OnRadioDefaultTemplate() 
{
	//选择默认常用模板

	UpdateData();
	UpdateRecordset();
}

void CSelTemplate::OnRadioAllTemplate() 
{
	//选择所有模板

	UpdateData();
	UpdateRecordset();
}

void CSelTemplate::OnButtonSetDefaultFavorite() 
{
	// TODO: Add your control notification handler code here
	SetDefaultFavoriteTemplate();
}

void CSelTemplate::SetDefaultFavoriteTemplate()
{
    //当相应模板为非默认常用模板时则可将其置为“默认常用”状态
	//否则可将其置为“非默认常用”状态

	CString strSQL;
	int ix=this->m_listctrlStruct.GetSelectedItem();
	if(ix==-1) return;
	int iTmpID=m_listctrlStruct.GetItemData(ix);

	if( m_bSetDefaultFavorite )
	{//将其置为“默认常用”
		strSQL.Format("UPDATE PhsStructureName SET [DefaultFavoriteTemplate]=-1 WHERE [SampleID]=%d",
			iTmpID);
		m_bSetDefaultFavorite = FALSE;
		m_btnDefaultFavoriteTemplate.SetWindowText(GetResStr(IDS_SET_NOT_DEFAULT_FAVORITE));
	}
	else
	{//将其置为“非默认常用”
		strSQL.Format("UPDATE PhsStructureName SET [DefaultFavoriteTemplate]=0 WHERE [SampleID]=%d",
			iTmpID);
		m_bSetDefaultFavorite = TRUE;
		m_btnDefaultFavoriteTemplate.SetWindowText(GetResStr(IDS_SET_DEFAULT_FAVORITE));
	}

	EDIBgbl::dbPRJ->Execute((_bstr_t)strSQL, NULL, adCmdText);
	//同时更新sort.mdb，保持同步
	EDIBgbl::dbPHScode->Execute((_bstr_t)strSQL, NULL, adCmdText);//20071101 "dbSORT"改为 "dbPHScode"
}

BOOL CSelTemplate::IsFavoriteTemplate(long lSampleID)
{
	CString strSQL;
	strSQL.Format("SELECT * FROM PhsStructureName WHERE [Favorite]=-1 AND [SampleID]=%d",
		lSampleID);
	BOOL bFavorite = FALSE;

	try
	{
		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(Recordset));
		rs->Open((_bstr_t)strSQL, _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
			adOpenKeyset, adLockOptimistic, adCmdText); 
		if( rs->BOF || rs->adoEOF )
		{
			bFavorite = FALSE;
		}
		else
		{
			bFavorite = TRUE;
		}
		rs->Close();
	}catch( CException& e )
	{
		e.ReportError();
		e.Delete();
	}
	return bFavorite;
}

BOOL CSelTemplate::IsDefaultFavoriteTemplate(long lSampleID)
{
	CString strSQL;
	strSQL.Format("SELECT * FROM PhsStructureName WHERE [DefaultFavoriteTemplate]=-1 AND [SampleID]=%d",
		lSampleID);

	BOOL bDefaultFavorite = FALSE;
	try 
	{
		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(Recordset));
		rs->Open((_bstr_t)strSQL, _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
			adOpenKeyset, adLockOptimistic, adCmdText); 
		if( rs->BOF || rs->adoEOF )
		{
			m_btnDefaultFavoriteTemplate.SetWindowText(GetResStr(IDS_SET_NOT_DEFAULT_FAVORITE));
			bDefaultFavorite = FALSE;
		}
		else
		{
			m_btnDefaultFavoriteTemplate.SetWindowText(GetResStr(IDS_SET_DEFAULT_FAVORITE));
			bDefaultFavorite = TRUE;
		}
		rs->Close();
	}catch( CException &e )
	{
		e.ReportError();
		e.Delete();
	}
	return bDefaultFavorite;
}

void CSelTemplate::OnItemchangedListctrlStruct(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	try
	{
	    if(m_rsTemplateName->State != adStateOpen)
			return;

		CString sTmp;
		int ix=this->m_listctrlStruct.GetSelectedItem();
		if(ix<0 )
			return;
		CString strCurItem;
		LVITEM  item;
		item.mask=LVIF_TEXT;
		item.pszText=strCurItem.GetBuffer(256);
		item.iItem=ix;
		item.iSubItem=0;
		m_listctrlStruct.GetItem(&item);
		strCurItem.ReleaseBuffer();

		//m_ListTmpName.GetText(ix,strCurItem);
		int iTmpID=m_listctrlStruct.GetItemData(ix);

		sTmp.Format(_T("SampleID=%d"),iTmpID);

		_variant_t vTmp;
		this->m_rsTemplateName->Find((_bstr_t)(sTmp), 0, adSearchBackward);

		this->DataReposition();
		

		//在点到相应模板时确认其是否“常用模板”
		m_btnFavoriteTemplate.ShowWindow(SW_SHOW);
		if( IsFavoriteTemplate(iTmpID) )
		{
			m_btnFavoriteTemplate.SetWindowText(GetResStr(IDS_SET_NOT_FAVORITE));
			m_bSetFavorite = FALSE;
		}
		else
		{
			m_btnFavoriteTemplate.SetWindowText(GetResStr(IDS_SET_FAVORITE));
			m_bSetFavorite = TRUE;
		}

		//在点到相应模板时确认其是否“默认常用模板”
		if( m_bIsOpenDefaultFavoriteTemplate )
			m_btnDefaultFavoriteTemplate.ShowWindow(SW_SHOW);
		else
			m_btnDefaultFavoriteTemplate.ShowWindow(SW_HIDE);

		if( IsDefaultFavoriteTemplate(iTmpID) ) 
		{
			m_btnDefaultFavoriteTemplate.SetWindowText(GetResStr(IDS_SET_NOT_DEFAULT_FAVORITE));
			m_bSetDefaultFavorite = FALSE;
		}
		else
		{
			m_btnDefaultFavoriteTemplate.SetWindowText(GetResStr(IDS_SET_DEFAULT_FAVORITE));
			m_bSetDefaultFavorite = TRUE;
		}

		//将对应模板名称的所有支吊架组装结构都显示在IDC_LISTCTRL_STRUCT列表中
	}
	catch(CException *e)
	{
		e->ReportError();
		e->Delete();
	}
	*pResult = 0;
}


void CSelTemplate::OnButtonDelTemplate() 
{
// TODO: Add your control notification handler code here
     if(m_listctrlStruct.GetSelectedCount()<=0)
	       MessageBox("请选择需要删除的模板");
     else
	 {
           if(AfxMessageBox("请确认是否删除",MB_YESNO)==IDYES)
		   {
				m_listctrlStruct.SetRedraw(FALSE);
	            POSITION pos; 
	            pos = m_listctrlStruct.GetFirstSelectedItemPosition();
                if(pos)
				{
		          DeleteItem(pos);
	            }
//				SetDlgItemText(IDC_STATIC_CAPTION,"选择支吊架组装模板号");
//				for(int i=m_lstStruct.GetCount()-1;i>=0;i--)
//					m_lstStruct.DeleteString(i);
				UpdateRecordset();			
//				m_rsTemplateName.Requery();
				m_listctrlStruct.SetRedraw();	
					

				/*	POINT pt;
					m_listctrlStruct.GetItemPosition(0,&pt);


				   //m_listctrlStruct.SetItemPosition(0,pt);
				   LVHITTESTINFO pInfo;
			   
					pInfo.iItem = 0;
					pInfo.pt = pt;

				   m_listctrlStruct.SubItemHitTest(&pInfo);
				   //m_listctrlStruct.HitTest(&pInfo);
					m_listctrlStruct.Update(0);*/
		   }  

	}

}


void CSelTemplate::DeleteItem(POSITION pos)
{
    int nItemPos;
	nItemPos = m_listctrlStruct.GetNextSelectedItem(pos);
	if(pos)
	{
        DeleteItem(pos);
	}
	int iTmpID=m_listctrlStruct.GetItemData(nItemPos);

	CString strSQL;
	strSQL.Format(_T("DELETE FROM PhsStructureNAME WHERE [SampleID] = %d"), iTmpID);
	EDIBgbl::dbPRJ->Execute((_bstr_t)strSQL, NULL, adCmdText);
	EDIBgbl::dbPHScode->Execute((_bstr_t)strSQL, NULL, adCmdText);//20071101 "dbSORT" 改为 "dbPHScode"
	strSQL.Format(_T("DELETE FROM PhsStructureREF WHERE [SampleID] = %d"), iTmpID);
	EDIBgbl::dbPRJ->Execute((_bstr_t)strSQL, NULL, adCmdText);
	EDIBgbl::dbPHScode->Execute((_bstr_t)strSQL, NULL, adCmdText);//20071101 "dbSORT" 改为 "dbPHScode"
	m_listctrlStruct.DeleteItem(nItemPos);

}

void CSelTemplate::OnCmdCancel() 
{
	// TODO: Add your control notification handler code here
		// TODO: Add your control notification handler code here
	CString str;
	str.Format("是否取消保存组装模板号=%d",iSaveTmpID);
   if(AfxMessageBox(str,MB_YESNO)==IDYES)
   {
	   CString strSQL;
	  strSQL.Format(_T("DELETE FROM PhsStructureNAME WHERE SampleID = %d"),iSaveTmpID);
	   EDIBgbl::dbPRJ->Execute((_bstr_t)strSQL, NULL, adCmdText);
    	EDIBgbl::dbPHScode->Execute((_bstr_t)strSQL, NULL, adCmdText);//20071101 "dbSORT" 改为 "dbPHScode"
	   strSQL.Format(_T("DELETE FROM PhsStructureREF WHERE [SampleID] = %d"), iSaveTmpID);
	   EDIBgbl::dbPRJ->Execute((_bstr_t)strSQL, NULL, adCmdText);
    	EDIBgbl::dbPHScode->Execute((_bstr_t)strSQL, NULL, adCmdText);//20071101 "dbSORT" 改为 "dbPHScode"
		m_iSaveID = -1;

			UpdateRecordset();

		GetDlgItem(IDC_CMD_CANCEL)->EnableWindow(FALSE);
   }

	
}



void CSelTemplate::initUpdateRecordset()
{
	::CWaitCursor curWait;
	GetFilterStr();
    initOpenTemplateRs();
	if(m_rsTemplateName->adoEOF && m_rsTemplateName->BOF)
	{
		m_bCurrentPA=FALSE;
		m_bCurrentSA=FALSE;
		CheckDlgButton(IDC_CHECK_CURENTSA2,FALSE);
		CheckDlgButton(IDC_CHECK_CURRENTPA,FALSE);
		GetDlgItem(IDC_CHECK_CURENTSA2)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CURRENTPA)->EnableWindow(FALSE);
		
	}
	else
	{
		UpdateData();
		GetDlgItem(IDC_CHECK_CURENTSA2)->EnableWindow(m_bFilter && m_bCurrentZdjhAvSA);
		GetDlgItem(IDC_CHECK_CURRENTPA)->EnableWindow(m_bFilter && m_bCurrentZdjhAvPA);
		
	}
   CString strTmp;
   CString strTmp2;

   _variant_t vTmp;
   if(!m_bIsSelPA)
   {
	   LoadListPA();
	   if(m_comboPA.GetCount()>0)
			m_comboPA.SelectString(-1,GetResStr(IDS_ALL));
	   else
			m_comboPA.AddString(GetResStr(IDS_ALL));
   }
   if(!m_bIsSelSA)
   {
		LoadListSA();
	   if(m_comboSA.GetCount()>0)
			m_comboSA.SelectString(-1,GetResStr(IDS_ALL));
	   else
			m_comboSA.AddString(GetResStr(IDS_ALL));
   }
}

void CSelTemplate::initOpenTemplateRs()
{	CString strSQL;
	CString strTemp;
	UpdateData();
	m_strSortFieldName=theSortName[this->m_iSortType];

	if(m_rsTemplateName->State == adStateOpen)
		m_rsTemplateName->Close();
// 	m_rsTemplateName.m_strFilter=_T("");
// 	m_rsTemplateName.m_strSort=_T("");
// 	m_rsTemplateName.m_pDatabase=&EDIBgbl::dbPRJ;

	strSQL=_T("SELECT * FROM phsStructureName ");

	this->m_strFilter.TrimLeft();
	this->m_strFilter.TrimRight();
	if(m_strFilter!=_T(""))
		strSQL +=_T(" WHERE ") + m_strFilter ;

	m_strSortFieldName.TrimLeft();
	m_strSortFieldName.TrimRight();
	if(m_strSortFieldName !=_T(""))
	{
		strSQL+=CString(_T(" ORDER BY ")) + _T( "  [") + m_strSortFieldName + _T("] ");
		if(this->m_bDesc)
			strSQL += _T(" DESC ");
	}
	m_rsTemplateName->Open((_bstr_t)strSQL,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
		adOpenKeyset, adLockOptimistic, adCmdText); 
	strTemp.Format(_T("SampleID=%d"),modPHScal::iSelSampleID);
	_variant_t vTmp;
	m_rsTemplateName->Find((_bstr_t)(strTemp), 0, adSearchBackward);
	if(!m_rsTemplateName->adoEOF && !m_rsTemplateName->BOF)
		m_rsTemplateName->MoveFirst();
	LoadListName();
	DataReposition();

}

void CSelTemplate::OnButton5() 
{

}


void CSelTemplate::OnCheck11() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if(m_bRodNum)
		((CComboBox*)GetDlgItem(IDC_COMBO_RODNUM))->SetCurSel(modPHScal::SumLugNum());
}

void CSelTemplate::OnSelchangeComboRodnum() 
{
	UpdateData();
	if(m_bRodNum)
		m_bRodNum=false;
	UpdateData(FALSE);
	UpdateRecordset();	
}
