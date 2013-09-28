// PreCalPag.cpp : implementation file
//
 
#include "stdafx.h"
#include "autophs.h"
#include "PreCalPag.h"
#include "modPHScal.h"
#include "EDIBgbl.h"
#include "modRegistry.h"
#include "user.h"  
#include "ViewTxsr.h"

#ifdef _DEBUG   
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreCalPag property page


IMPLEMENT_DYNCREATE(CPreCalPag, CPropertyPage)

CPreCalPag::CPreCalPag() : CPropertyPage(CPreCalPag::IDD)
{
	//{{AFX_DATA_INIT(CPreCalPag)
	m_sComboSpringOrder = (_T(""));
	m_bCheckSPRAutoLugDia = TRUE;
	m_bCheckConnectHoleDimMatch = TRUE;
	m_bCheckLimitLugMinDia = FALSE;
	m_bComboAutoApplyChzbh = TRUE;
	m_bCheckAutoPASA = TRUE;
	m_bCheckCalSAbyConservativenessMethod = TRUE;
	m_bCheckCalPARTbyMaxLoad = FALSE;
	m_bCheckCalSAbyPJG = TRUE;
	m_bCheckAddPartWeight2Pmax = TRUE;
	m_bCheckWorksWithSABoltsNuts = TRUE;
	m_bCheckWorksWithBoltsNuts = TRUE;
	m_bCheckCSPRCanSpecialDesign = TRUE;
	m_bCheckAutoAlignCSPRtoSA = TRUE;
	m_bCheckCalCSPRLugDiaByPjg = FALSE;
	m_bCheckAutoApplyCSPRLugDia = TRUE;
	m_bCheckOutputSA = TRUE;
	m_bCheckSumAllVolume = FALSE;
	m_sComboSPR_CHzbh = (_T(""));
	m_sComboUseSPRminDist = (_T(""));
	m_sComboDW_Delta = (_T(""));
	m_sComboPjg2Pgz = (_T(""));
	m_sComboConnectTJ = (_T(""));
	m_bRepZdjType = FALSE;
	m_sComboRodLenRoundVal = (_T(""));
	m_iNumCS = -1;
	m_bAutoSPRRodDiaDecrease = FALSE;
	m_bRodDiaByMax = TRUE;
	m_strMinRodLen = (_T(""));
	m_bHotStatus2Zero = FALSE;
	m_bSumRodByRoundSteel = FALSE;
	m_sComboRodInsertLenRoundVal = (_T(""));
	m_bCheckAddPartWeight2CSPRPgz = FALSE;
	m_bCheckAddPartWeight2SPRPgz = FALSE;//2007.07.30
	m_IsPAForceZero= FALSE;//2007.09.25
	m_bCheckMinMaxDispByActualDisp = FALSE;
	m_bCheckDHT = FALSE;
	m_strDHTGG = _T("");
	m_bSPRBHLOADBYInstallLoad = FALSE;
	m_bSPRBHLOADUNITBYNEWTON = FALSE;
	m_bPipeClampBolt = TRUE;
	m_FrictionFree = 0.3f;
	m_bCalcCSPRHeight_BySelDisp = FALSE;
	m_bMaxPgzPaz = FALSE;//2007.09.03
	m_bCheckAddPartWeight2SPRPgz = FALSE;
	m_bMaxPgzPaz = FALSE;
	m_IsPAForceZero = FALSE;
	m_dZJOverValue = 30.0;
	//}}AFX_DATA_INIT
}

CPreCalPag::~CPreCalPag()
{
}

void CPreCalPag::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreCalPag)
	DDX_Control(pDX, IDC_SET_RodInsertLenRoundVal, m_ComboRodInsertLenRoundVal);
	DDX_Control(pDX, IDC_SPRINGORDER, m_ComboSpringOrder);
	DDX_Control(pDX, IDC_SET_RodLenRoundVal, m_ComboRodLenRoundVal);
	DDX_Control(pDX, IDC_SET_COMBO4, m_ComboConnectTJ);
	DDX_Control(pDX, IDC_SET_COMBO3, m_ComboPjg2Pgz);
	DDX_Control(pDX, IDC_SET_COMBO5, m_ComboDW_Delta);
	DDX_Control(pDX, IDC_SET_COMBO2, m_ComboUseSPRminDist);
	DDX_Control(pDX, IDC_SET_COMBO1, m_ComboSPR_CHzbh);
	DDX_LBString(pDX, IDC_SPRINGORDER, m_sComboSpringOrder);
	DDX_Check(pDX, IDC_LUGSPRAUTODIA, m_bCheckSPRAutoLugDia);
	DDX_Check(pDX, IDC_DIMMATCH, m_bCheckConnectHoleDimMatch);
	DDX_Check(pDX, IDC_LIMITDIA, m_bCheckLimitLugMinDia);
	DDX_Check(pDX, IDC_OPTIMIZESEL, m_bComboAutoApplyChzbh);
	DDX_Check(pDX, IDC_AUTOPASA, m_bCheckAutoPASA);
	DDX_Check(pDX, IDC_CONSEVATIVESEL, m_bCheckCalSAbyConservativenessMethod);
	DDX_Check(pDX, IDC_SABYPJG, m_bCheckCalSAbyPJG);
	DDX_Check(pDX, IDC_ADDPARTWEIGHT, m_bCheckAddPartWeight2Pmax);
	DDX_Check(pDX, IDC_WORKWITHSA, m_bCheckWorksWithSABoltsNuts);
	DDX_Check(pDX, IDC_WORKWITH, m_bCheckWorksWithBoltsNuts);
	DDX_Check(pDX, IDC_SPECIALDESIGN, m_bCheckCSPRCanSpecialDesign);
	DDX_Check(pDX, IDC_ALIGNCSPRTOSA, m_bCheckAutoAlignCSPRtoSA);
	DDX_Check(pDX, IDC_CALLUGBYPJG, m_bCheckCalCSPRLugDiaByPjg);
	DDX_Check(pDX, IDC_LUG2CSPR, m_bCheckAutoApplyCSPRLugDia);
	DDX_Check(pDX, IDC_OUTPUTSA, m_bCheckOutputSA);
	DDX_Check(pDX, IDC_SUMALL, m_bCheckSumAllVolume);
	DDX_CBStringExact(pDX, IDC_SET_COMBO1, m_sComboSPR_CHzbh);
	DDX_CBStringExact(pDX, IDC_SET_COMBO2, m_sComboUseSPRminDist);
	DDX_CBStringExact(pDX, IDC_SET_COMBO5, m_sComboDW_Delta);
	DDX_CBStringExact(pDX, IDC_SET_COMBO3, m_sComboPjg2Pgz);
	DDX_CBStringExact(pDX, IDC_SET_COMBO4, m_sComboConnectTJ);
	DDX_Check(pDX, IDC_CHECK_REPTYPE, m_bRepZdjType);
	DDX_CBStringExact(pDX, IDC_SET_RodLenRoundVal, m_sComboRodLenRoundVal);
	DDX_Check(pDX, IDC_LUGSPRAUTODIA_DECREASE, m_bAutoSPRRodDiaDecrease);
	DDX_Check(pDX, IDC_RODDIABYMAX, m_bRodDiaByMax);
	DDX_CBStringExact(pDX, IDC_COMBO_MINRODLEN, m_strMinRodLen);
	DDX_Check(pDX, IDC_CHECK_HotStatus2Zero, m_bHotStatus2Zero);
	DDX_Check(pDX, IDC_SumRodByRoundSteel, m_bSumRodByRoundSteel);
	DDX_CBStringExact(pDX, IDC_SET_RodInsertLenRoundVal, m_sComboRodInsertLenRoundVal);
	DDX_Check(pDX, IDC_ADDPARTWEIGHT2CSPR_PGZ, m_bCheckAddPartWeight2CSPRPgz);
	DDX_Check(pDX, IDC_ADDPARTWEIGHT2SPR_PGZ, m_bCheckAddPartWeight2SPRPgz);//2007.07.30
	DDX_Check(pDX, IDC_CHECK_MINMAXDISP_BYACTUALDISP, m_bCheckMinMaxDispByActualDisp);
	DDX_Check(pDX, IDC_CHECK_HZDHT, m_bCheckDHT);
	DDX_Text(pDX, IDC_EDIT_DHTGG, m_strDHTGG);
	DDX_Check(pDX, IDC_SPRBHLOADBYInstallLoad, m_bSPRBHLOADBYInstallLoad);
	DDX_Check(pDX, IDC_SPRBHLOAD_UNIT_BYNEWTON, m_bSPRBHLOADUNITBYNEWTON);
	DDX_Check(pDX, IDC_PIPE_CLAMP_BOLTDIA_DECREASE, m_bPipeClampBolt);
	DDX_Text(pDX, IDC_FRICTIONFREE, m_FrictionFree);
	DDX_Check(pDX, IDC_CALCCSPRHEIGHT_BYSELDISP, m_bCalcCSPRHeight_BySelDisp);
	DDX_Check(pDX, IDC_MaxPgzPaz, m_bMaxPgzPaz);
	DDX_Check(pDX, IDC_PAForceCHECK, m_IsPAForceZero);
	DDX_Text(pDX, IDC_EDIT_ZJOverValue, m_dZJOverValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPreCalPag, CPropertyPage)
	//{{AFX_MSG_MAP(CPreCalPag)
	ON_BN_CLICKED(IDC_RADIO_SCS, OnRadioScs)
	ON_BN_CLICKED(IDC_RADIO_DCS, OnRadioDcs)
	ON_BN_CLICKED(IDC_RADIO_ACS, OnRadioAcs)
	ON_BN_CLICKED(IDC_CHECK_HZDHT, OnCheckHzDHT)
	ON_WM_LBUTTONDOWN()
	ON_EN_KILLFOCUS(IDC_FRICTIONFREE, OnKillfocusFrictionfree)
	ON_BN_CLICKED(IDC_CALCCSPRHEIGHT_BYSELDISP, OnCalccsprheightByseldisp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreCalPag message handlers

void CPreCalPag::SetComboSpringOrder(bool bSaveAction)
{
	//目的:设置弹簧选择规则的组合框
		//只有在此处作，因为数据库初始化已经完成
	CDaoRecordset rs(&EDIBgbl::dbPHScode);//20071022 "dbSORT" 改为 "dbPHScode"
	COleVariant vTmp;
	rs.Open(dbOpenSnapshot,(_T("SELECT * FROM SelSpringRule ORDER BY seq")));
	if(bSaveAction)
		modPHScal::gstrSpringOrder = (_T(""));
	if( bSaveAction) modPHScal::giSpringOrder = m_ComboSpringOrder.GetCurSel();
	if(!bSaveAction) m_ComboSpringOrder.ResetContent();

	if( rs.IsEOF() && rs.IsBOF());
	else
	{
		while(!rs.IsEOF())
		{
			if(!bSaveAction)
			{
				rs.GetFieldValue(_T("LocalRuleDescription"),vTmp);
				m_ComboSpringOrder.AddString(vtos(vTmp));
			}
			else
			{
				rs.GetFieldValue(_T("seq"),vTmp);
				if (modPHScal::giSpringOrder == vtoi(vTmp))
				{
					rs.GetFieldValue(_T("LocalRuleDescription"),vTmp);
					modPHScal::gstrSpringOrder =vtos(vTmp);
					rs.GetFieldValue(_T("RuleSQL"),vTmp);
					modPHScal::gstrSpringOrderSQL=vtos(vTmp);
					if(vtos(vTmp)==_T(""))
					{
						rs.GetFieldValue(_T("defRuleSQL"),vTmp);
						modPHScal::gstrSpringOrderSQL = vtos(vTmp);
						if(vtos(vTmp)==_T(""))
							MessageBox(GetResStr(IDS_NullDefRuleSQLfieldInSelSpringRuleInSortMdb));
					}
						break;
				}
			}
			rs.MoveNext();
		}
		if(!bSaveAction)
		{
			m_ComboSpringOrder.SetCurSel(modPHScal::giSpringOrder);
			m_ComboSpringOrder.GetText(modPHScal::giSpringOrder,m_sComboSpringOrder);
			modPHScal::gstrSpringOrder=m_sComboSpringOrder;
		}
		else
			m_ComboSpringOrder.GetText(modPHScal::giSpringOrder,modPHScal::gstrSpringOrder);
	}
}

void CPreCalPag::SetCtrlValue()
{
	m_bRepZdjType=modPHScal::bHintRepZdjType;
	m_sComboUseSPRminDist = ftos(modPHScal::gUseSpringMinDist);
	m_sComboSPR_CHzbh = ftos(modPHScal::gnSPR_CHZBH);
	m_sComboPjg2Pgz = ftos(modPHScal::gsngRatioOfPjg2Pgz);
	m_sComboConnectTJ = ftos(modPHScal::gnConnectTJ);
	if (modPHScal::gnRodLenRoundVal<1)
	{
		modPHScal::gnRodLenRoundVal=100;
		SetRegValue((_T("Settings")), _T("RodLenRoundVal"), modPHScal::gnRodLenRoundVal);
	}
	if (modPHScal::gnRodInsertLenRoundVal<1)
	{
		modPHScal::gnRodInsertLenRoundVal=1;
		SetRegValue((_T("Settings")), _T("RodInsertLenRoundVal"), modPHScal::gnRodInsertLenRoundVal); 
	}
	m_sComboRodLenRoundVal = ltos(modPHScal::gnRodLenRoundVal);
	m_sComboRodInsertLenRoundVal = ltos(modPHScal::gnRodInsertLenRoundVal);
	//int indexRodLen=m_ComboRodLenRoundVal.FindStringExact(-1,m_sComboRodLenRoundVal);
	//m_ComboRodLenRoundVal.SetCurSel(indexRodLen);
	
   m_bCheckConnectHoleDimMatch = modPHScal::gbConnectHoleDimMatch;
   m_bCheckSPRAutoLugDia = modPHScal::gbSPRAutoLugDia;
   //CheckCSPRAutoLugDia = IIf(gbCSPRAutoLugDia = 0, 0, 1)
   m_bCheckAutoApplyCSPRLugDia = modPHScal::gbAutoApplyCSPRLugDia;
   
   m_bComboAutoApplyChzbh =modPHScal::gbAutoApplyChzbh;
   m_bCheckOutputSA = modPHScal::gbOutputSA;
   m_bCheckAutoPASA = modPHScal::gbAutoPASA;
   m_bCheckCSPRCanSpecialDesign = modPHScal::gbCSPRCanSpecialDesign;
   m_bCheckLimitLugMinDia = modPHScal::gbLimitLugMinDia;
	m_bRodDiaByMax=modPHScal::gbRodDiaByMax;
	m_bPipeClampBolt = modPHScal::gbPipeClampBolt;
   
   m_bCheckAutoAlignCSPRtoSA = modPHScal::gbAutoAlignCSPRtoSA;
   m_bCheckSumAllVolume = modPHScal::gbSumAllVolume;
   m_bSumRodByRoundSteel=modPHScal::gbSumRodByRoundSteel;
   
   m_bCheckCalSAbyConservativenessMethod = modPHScal::gbCalSAbyConservativenessMethod;
   m_bCheckCalSAbyPJG = modPHScal::gbCalSAbyPJG;
   m_bCheckCalCSPRLugDiaByPjg = modPHScal::gbCalCSPRLugDiaByPjg;
   
   m_bCheckCalSAbyConservativenessMethod = modPHScal::gbCalSAbyConservativenessMethod;
   m_bCheckCalPARTbyMaxLoad = modPHScal::gbCalPARTbyMaxLoad;
   m_bCheckAddPartWeight2Pmax = modPHScal::gbAddPartWeight2PMax;
   m_bCheckAddPartWeight2CSPRPgz = modPHScal::gbAddPartWeight2CSPRPgz;
   m_bCheckAddPartWeight2SPRPgz = modPHScal::gbAddPartWeight2SPRPgz;//2007.07.30
   m_IsPAForceZero = modPHScal::gbPAForceZero;//2007.09.25
   m_bMaxPgzPaz = modPHScal::gbMaxPgzPaz;//2007.09.03
   m_bCheckMinMaxDispByActualDisp = modPHScal::gbMinMaxDispByActualDisp;
   m_bCalcCSPRHeight_BySelDisp = modPHScal::gbCalcCSPRHeight_BySelDisp;

   //m_sComboSpringOrder = modPHScal::gstrSpringOrder;
   m_bCheckWorksWithBoltsNuts =modPHScal::gbWorksWithBoltsNuts;
   m_bCheckWorksWithSABoltsNuts =modPHScal::gbWorksWithSABoltsNuts;
	m_bAutoSPRRodDiaDecrease=modPHScal::gbAutoSPRRodDiaDecrease;
	m_bSPRBHLOADBYInstallLoad=modPHScal::gbSPRBHLOADBYInstallLoad;
	m_bSPRBHLOADUNITBYNEWTON=modPHScal::gbSPRBHLOADUNITBYNEWTON;
	m_bHotStatus2Zero=gbHotStatus2Zero;
	m_dZJOverValue = modPHScal::gdZJOverValue;
	SetRegValue((_T("Settings")), _T("gdZJOverValue"), modPHScal::gdZJOverValue); 
	m_sComboDW_Delta = ftos(modPHScal::gnDW_delta);

	m_strMinRodLen.Format(_T("%d"),modPHScal::giMinRodLen);
	m_iNumCS=modPHScal::iNumCSVal;
	CheckRadioButton(IDC_RADIO_ACS,IDC_RADIO_DCS,IDC_RADIO_ACS+m_iNumCS);

	m_bCheckDHT = modPHScal::g_bDHT;
	m_strDHTGG = modPHScal::g_strDHTGG;
	if( m_bCheckDHT )
		GetDlgItem(IDC_EDIT_DHTGG)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_EDIT_DHTGG)->EnableWindow(FALSE);

	UpdateData(false);
}

BOOL CPreCalPag::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetComboSpringOrder(false);
	SetCtrlValue();
	InitFromReg();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPreCalPag::SaveIniValue()
{
	UpdateData();
	modPHScal::gbSumRodByRoundSteel=m_bSumRodByRoundSteel;
	modPHScal::bHintRepZdjType=m_bRepZdjType;
	modPHScal::gUseSpringMinDist = _tcstod(m_sComboUseSPRminDist,NULL);
	modPHScal::gnSPR_CHZBH = _tcstod(m_sComboSPR_CHzbh,NULL);
	
	modPHScal::gsngRatioOfPjg2Pgz = _tcstod(m_sComboPjg2Pgz,NULL);
	modPHScal::gnConnectTJ = _tcstod(m_sComboConnectTJ,NULL);
	
	modPHScal::gbConnectHoleDimMatch = m_bCheckConnectHoleDimMatch;
	modPHScal::gbSPRAutoLugDia = m_bCheckSPRAutoLugDia;
	modPHScal::gbAutoSPRRodDiaDecrease=m_bAutoSPRRodDiaDecrease;
	modPHScal::gbSPRBHLOADBYInstallLoad=m_bSPRBHLOADBYInstallLoad;
	modPHScal::gbSPRBHLOADUNITBYNEWTON=m_bSPRBHLOADUNITBYNEWTON;
	gbHotStatus2Zero=m_bHotStatus2Zero;
	//gbCSPRAutoLugDia = m_bCheckCSPRAutoLugDia
	modPHScal::gbAutoApplyCSPRLugDia = m_bCheckAutoApplyCSPRLugDia;
	
	
	modPHScal::gbOutputSA = m_bCheckOutputSA;
	modPHScal::gbAutoPASA = m_bCheckAutoPASA;
	modPHScal::gbAutoApplyChzbh = m_bComboAutoApplyChzbh;
	modPHScal::gbCalSAbyConservativenessMethod = m_bCheckCalSAbyConservativenessMethod;
	modPHScal::gbCalSAbyPJG = m_bCheckCalSAbyPJG;
	modPHScal::gbCalCSPRLugDiaByPjg = m_bCheckCalCSPRLugDiaByPjg;

	modPHScal::gbCalPARTbyMaxLoad = m_bCheckCalPARTbyMaxLoad;
	modPHScal::gbAddPartWeight2PMax = m_bCheckAddPartWeight2Pmax;
	modPHScal::gbAddPartWeight2CSPRPgz = m_bCheckAddPartWeight2CSPRPgz;
	modPHScal::gbAddPartWeight2SPRPgz = m_bCheckAddPartWeight2SPRPgz;//2007.07.30
	modPHScal::gbPAForceZero=m_IsPAForceZero;//2007.09.25
	modPHScal::gbMaxPgzPaz=m_bMaxPgzPaz;//2007.09.03
	modPHScal::gbMinMaxDispByActualDisp = m_bCheckMinMaxDispByActualDisp;
	modPHScal::gbCalcCSPRHeight_BySelDisp = m_bCalcCSPRHeight_BySelDisp;
    SetRegValue((_T("Settings")),_T("CalcCSPRHeight_BySelDisp"),(m_bCalcCSPRHeight_BySelDisp));

	modPHScal::gbCSPRCanSpecialDesign = m_bCheckCSPRCanSpecialDesign;
	modPHScal::gbLimitLugMinDia = m_bCheckLimitLugMinDia;

	modPHScal::gbAutoAlignCSPRtoSA = m_bCheckAutoAlignCSPRtoSA;
	modPHScal::gbSumAllVolume = m_bCheckSumAllVolume;
	modPHScal::gdZJOverValue = m_dZJOverValue;
	

	modPHScal::giSpringOrder = m_ComboSpringOrder.GetCurSel();
	SetComboSpringOrder(true);

	
	modPHScal::gbWorksWithBoltsNuts = m_bCheckWorksWithBoltsNuts;
	modPHScal::gbWorksWithSABoltsNuts = m_bCheckWorksWithSABoltsNuts;
	modPHScal::gnDW_delta = _tcstod(m_sComboDW_Delta,NULL);
	modPHScal::gbRodDiaByMax=m_bRodDiaByMax;
	modPHScal::gbPipeClampBolt = m_bPipeClampBolt;
	modPHScal::giMinRodLen=_ttoi(m_strMinRodLen);

     SetRegValue((_T("Settings")), _T("MinRodLen"), m_strMinRodLen);
     SetRegValue((_T("Settings")), _T("UseSpringMinDist"), m_sComboUseSPRminDist);
     SetRegValue((_T("Settings")), _T("SPR_Chzbh"), m_sComboSPR_CHzbh);
     SetRegValue((_T("Settings")), _T("Pjg2Pgz"), m_sComboPjg2Pgz);
     SetRegValue((_T("Settings")), _T("ConnectTJ"), m_sComboConnectTJ);
     SetRegValue((_T("Settings")), _T("RodDiaByMax"), m_bRodDiaByMax);
	 SetRegValue((_T("Settings")), _T("PipeClampBolt"), m_bPipeClampBolt);
	 modPHScal::gnRodLenRoundVal=_ttoi(m_sComboRodLenRoundVal);
	 modPHScal::gnRodInsertLenRoundVal=_ttoi(m_sComboRodInsertLenRoundVal);
	if (modPHScal::gnRodLenRoundVal<1)
	{
		modPHScal::gnRodLenRoundVal=100;
	}
	if (modPHScal::gnRodInsertLenRoundVal<1)
	{
		modPHScal::gnRodInsertLenRoundVal=1;
	}
	 SetRegValue((_T("Settings")), _T("RodLenRoundVal"), (modPHScal::gnRodLenRoundVal));
	 SetRegValue((_T("Settings")), _T("RodInsertLenRoundVal"), (modPHScal::gnRodInsertLenRoundVal));
     SetRegValue((_T("Settings")), _T("ConnectHoleDimMatch"), m_bCheckConnectHoleDimMatch);
     SetRegValue((_T("Settings")), _T("SPRAutoLugDia"), m_bCheckSPRAutoLugDia);
     SetRegValue((_T("Settings")), _T("AutoApplyChzbh"), (m_bComboAutoApplyChzbh));
     SetRegValue((_T("Settings")), _T("AutoApplyCSPRLugDia"), (m_bCheckAutoApplyCSPRLugDia));
     SetRegValue((_T("Settings")), _T("OutputSA"), (m_bCheckOutputSA));
     SetRegValue((_T("Settings")), _T("AutoPASA"), (m_bCheckAutoPASA));
	 SetRegValue((_T("Settings")),_T("HintRepZdjType"),(m_bRepZdjType));

	 modPHScal::iNumCSVal=this->m_iNumCS;
	 SetRegValue((_T("Settings")),_T("NumCSVal"),(modPHScal::iNumCSVal));
	 SetRegValue((_T("Settings")),_T("AutoSPRRodDiaDecrease"),(modPHScal::gbAutoSPRRodDiaDecrease));
	 SetRegValue((_T("Settings")),_T("SPRBHLOADBYInstallLoad"),(modPHScal::gbSPRBHLOADBYInstallLoad));
	 SetRegValue((_T("Settings")),_T("SPRBHLOADUNITBYNEWTON"),(modPHScal::gbSPRBHLOADUNITBYNEWTON));
	 SetRegValue((_T("Settings")),_T("HotStatus2Zero"),(gbHotStatus2Zero));
	 SetRegValue((_T("Settings")), _T("SumRodByRoundSteel"), m_bSumRodByRoundSteel);

	 modPHScal::g_bDHT = m_bCheckDHT;
	 SetRegValue((_T("Settings")), _T("DHT"), m_bCheckDHT);
	 modPHScal::g_strDHTGG = m_strDHTGG;
	 SetRegValue((_T("Settings")), _T("DHTGG"), m_strDHTGG);


	 pTestWnd->SendMessage(WM_SENDMESSAGE,m_bHotStatus2Zero);

}

void CPreCalPag::UpdateCBString(CDataExchange *pDX, UINT nID, CString strData)
{
	//if(pDx->m_bSaveAndValidate
}

void CPreCalPag::OnRadioScs() 
{
	// TODO: Add your control notification handler code here
	m_iNumCS=1;
	//UpdateData(FALSE);
}

void CPreCalPag::OnRadioDcs() 
{
	// TODO: Add your control notification handler code here
	m_iNumCS=2;
	//UpdateData(FALSE);
}

void CPreCalPag::OnRadioAcs() 
{
	// TODO: Add your control notification handler code here
	m_iNumCS=0;
	//UpdateData(FALSE);
}

void CPreCalPag::DoDefaultSetting()
{

	m_sComboUseSPRminDist = "0";
	m_sComboSPR_CHzbh = "0.35";
	m_sComboPjg2Pgz = "1.4";
	m_sComboConnectTJ = "200";
	m_sComboRodLenRoundVal = "100";
	m_sComboRodInsertLenRoundVal = "1";
	m_sComboDW_Delta = "7";

	m_bSumRodByRoundSteel= FALSE;
	m_bCheckConnectHoleDimMatch = TRUE;
	m_bCheckSPRAutoLugDia = TRUE;
	m_bCheckAutoApplyCSPRLugDia = TRUE;
	m_bRepZdjType=FALSE;
	m_bComboAutoApplyChzbh = FALSE;
	m_bCheckOutputSA = TRUE;
	m_bCheckCSPRCanSpecialDesign = TRUE;
	m_bCheckLimitLugMinDia = TRUE;
	m_bRodDiaByMax=TRUE;
	m_bPipeClampBolt = TRUE;
	m_bCheckAutoAlignCSPRtoSA = TRUE;
	m_bCheckSumAllVolume = FALSE;
	m_bCheckCalSAbyConservativenessMethod = TRUE;
	m_bCheckCalSAbyPJG = FALSE;
	m_bCheckCalCSPRLugDiaByPjg = TRUE;
	m_bCheckCalSAbyConservativenessMethod = TRUE;
	m_bCheckCalPARTbyMaxLoad = FALSE;
	m_bCheckAddPartWeight2Pmax = TRUE;
	m_bCheckAddPartWeight2CSPRPgz = TRUE;
	m_bCheckAddPartWeight2SPRPgz=FALSE;//2007.07.30
	m_IsPAForceZero=FALSE;//2007.09.25
	m_bMaxPgzPaz=FALSE;//2007.09.03	
	
	m_bCheckMinMaxDispByActualDisp = TRUE;
	m_bCalcCSPRHeight_BySelDisp = FALSE;
	
	m_bCheckWorksWithBoltsNuts = TRUE;
	m_bCheckWorksWithSABoltsNuts = TRUE;
	m_bAutoSPRRodDiaDecrease= FALSE;
	m_bSPRBHLOADBYInstallLoad= FALSE;
	m_bSPRBHLOADUNITBYNEWTON = TRUE;

	m_bHotStatus2Zero=TRUE;
	m_iNumCS=0;	CheckRadioButton(IDC_RADIO_ACS,IDC_RADIO_DCS,IDC_RADIO_ACS+m_iNumCS);
	m_sComboSpringOrder = _T("自动");

	m_bCheckDHT = FALSE;
	GetDlgItem(IDC_EDIT_DHTGG)->EnableWindow(FALSE);

	UpdateData(FALSE);
}

void CPreCalPag::OnCheckHzDHT() 
{
	UpdateData();
	if( m_bCheckDHT )
		GetDlgItem(IDC_EDIT_DHTGG)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_EDIT_DHTGG)->EnableWindow(FALSE);
}


void CPreCalPag::OnLButtonDown(UINT nFlags,CPoint point)
{//chengbailian 2003.12.12 当窗口很大时,在分辨率改变小时,窗口不能全部显示出来,需要移动窗体
	HWND h=::GetActiveWindow();//得到主窗口名柄
	::SendMessage(h,WM_NCLBUTTONDOWN, HTCAPTION, NULL);//发送移动消息
}



void CPreCalPag::InitFromReg()
{
	m_FrictionFree=  GetRegKey(_T("Settings"),_T("Friction-free"), m_FrictionFree);
	SetRegValue("Settings","Friction-free",m_FrictionFree);
	
// 	HKEY hKey;
// 	long dwErrorCode;
// 	DWORD dwDisposition;
// 	DWORD dwSize;
// 	dwErrorCode=RegCreateKeyEx(HKEY_LOCAL_MACHINE,
// 				   _T("SOFTWARE\\长沙优易软件开发有限公司\\AutoPHS\\7.0\\Settings"),//pfg20050609
// 				   0,
// 				   "",
// 				   REG_OPTION_NON_VOLATILE,
// 				   KEY_ALL_ACCESS,
// 				   NULL,
// 				   &hKey,
// 				   &dwDisposition);
// 
// 	if(dwErrorCode!=ERROR_SUCCESS)
// 	{
// 		return;
// 	}
// 
// 	dwSize=sizeof(m_FrictionFree);
// 	dwErrorCode=RegQueryValueEx(hKey,_T("Friction-free"),NULL,NULL,(BYTE*)&this->m_FrictionFree,&dwSize);
// 
// 	if(dwErrorCode!=ERROR_SUCCESS)
// 	{
// 		dwErrorCode=::RegSetValueEx(hKey,_T("Friction-free"),0,REG_BINARY,(BYTE*)&m_FrictionFree,sizeof(m_FrictionFree));
// 	}
// 
// 	CloseHandle(hKey);
	this->UpdateData(FALSE);
}

void CPreCalPag::OnKillfocusFrictionfree() 
{
// 	HKEY hKey;
// 	long dwErrorCode;
// 	DWORD dwDisposition;
// 	DWORD dwSize;
// 
	this->UpdateData();	
	SetRegValue("Settings","Friction-free",m_FrictionFree);
// 
// 	dwErrorCode=RegCreateKeyEx(HKEY_LOCAL_MACHINE,
// 				   _T("SOFTWARE\\长沙优易软件开发有限公司\\AutoPHS\\7.0\\Settings"),
// 				   0,
// 				   "",
// 				   REG_OPTION_NON_VOLATILE,
// 				   KEY_ALL_ACCESS,
// 				   NULL,
// 				   &hKey,
// 				   &dwDisposition);
// 
// 	if(dwErrorCode!=ERROR_SUCCESS)
// 	{
// 		return;
// 	}
// 
// 	dwSize=sizeof(m_FrictionFree);
// 	dwErrorCode=::RegSetValueEx(hKey,_T("Friction-free"),0,REG_BINARY,(BYTE*)&m_FrictionFree,sizeof(m_FrictionFree));
// 
// 	CloseHandle(hKey);	
}

void CPreCalPag::OnCalccsprheightByseldisp() 
{
	UpdateData(true);
	if(m_bCalcCSPRHeight_BySelDisp) 
		m_bCheckMinMaxDispByActualDisp = false;
	else
		 m_bCheckMinMaxDispByActualDisp = true;

	UpdateData(false);
}
