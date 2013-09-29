// PreDrawPag.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "PreDrawPag.h"
#include "modPHScal.h"
#include "user.h"
#include "EDIBgbl.h"
#include "modRegistry.h"
#include "CadFontPag.h"
#include "EDIBAcad.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreDrawPag property page

IMPLEMENT_DYNCREATE(CPreDrawPag, CPropertyPage)

CPreDrawPag::CPreDrawPag() : CPropertyPage(CPreDrawPag::IDD)
{
	//{{AFX_DATA_INIT(CPreDrawPag)
	m_OrientationPicture=false;//pfg20050426
	m_bCheckDrawNoPre = TRUE;
	m_bCheckOSFlag = TRUE;
	m_bCheckDraw_BOM = TRUE;
	m_bCheckDrawOverWrite = TRUE;
	m_bCheckAnnotateCSPR = TRUE;
	m_bCheckNotPROMPT_ACAD_SHIFT = FALSE;
	m_bCheckNotPROMPT_Catlog_Overwrite = FALSE;
	m_bCheckAutoSaveDrawing = FALSE;
	m_sComboSEQyOffset = _T("");
	m_sTextDrawNoStartNumber = _T("");
	m_strDwgVersion = _T(""); 
	m_stextDWGPRE = _T("");
	m_sLabelStartPN = GetResStr(IDS_PHSFirstChartMark);
	m_bNoFenQu = FALSE;
	m_bTBhq = TRUE;
	m_bBomSprFactory = FALSE;
	m_bSumBomSprFactory = FALSE;
	m_bDimLocation = FALSE;
	m_bCheckDraw_SumWeight = FALSE;
	m_GroupWare = 0;
	m_strGroupWareName = _T("");
	m_bCheckTagUsingCircle = FALSE;
	m_bCheckCustomPicNo = FALSE;
	m_bCheckLJMX_BJBW = FALSE;
	m_bCheckBGForBJBW = FALSE;
	m_bCheckDWForBJBW = FALSE;
	m_fCircleDiameter = 0.0f;
	m_bCheckDLOADBJBW = FALSE;
	m_bCheckDrawNameWithoutNo = FALSE;
	m_fSeqTxtHeight = 0.0f;
	m_bNoWaiKuang = FALSE;
	m_bNotDrawWeight = FALSE;
	m_bInsertLength = FALSE;
	m_bColdHeatState = FALSE;
	m_bInstallElevation = FALSE;
	m_bHzabs = TRUE;
	m_bPrintSA_Value = FALSE;
	m_fOvertopDim = 2.0f;
	m_fDistanceOffset = 0.0f;
	m_bDimNumber = FALSE;
	m_fSetTableWidth = 0.0f;
	m_fSetTKWidth = 0.3f;
	m_IsACAD = 0;
	m_bDrawWeldSymbol = FALSE;
	m_bAutoWeldViewInsertPoint = FALSE;
	m_bDrawWeldViewDim = FALSE;
	m_bWeldAutoDirectAndPos = FALSE;
	m_MaxLGLong=110;//拉杆最大绘制长度
	m_MaxLGLong = 0.0;
	m_bSymbol =	FALSE;
	//}}AFX_DATA_INIT
	m_iOldTabFontSel=-1;
}

CPreDrawPag::~CPreDrawPag()
{
}

void CPreDrawPag::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreDrawPag)
	DDX_Control(pDX, IDC_TAB1, m_tabFont);
	DDX_Control(pDX, IDC_LABLE_PN, m_LabelStartPN);
	DDX_Control(pDX, IDC_MINOFFSET, m_ComboSEQyOffset);
	DDX_Check(pDX, IDC_ORIENTATION_PICTURE, m_OrientationPicture);//pfg20050426
	DDX_Check(pDX, IDC_DRAWPRENO, m_bCheckDrawNoPre);
	DDX_Check(pDX, IDC_OSFLAG, m_bCheckOSFlag);
	DDX_Check(pDX, IDC_DRAW_BOM, m_bCheckDraw_BOM);
	DDX_Check(pDX, IDC_OVERWRITE, m_bCheckDrawOverWrite);
	DDX_Check(pDX, IDC_ANNOTATE, m_bCheckAnnotateCSPR);
	DDX_Check(pDX, IDC_PROMPT_SHIFT, m_bCheckNotPROMPT_ACAD_SHIFT);
	DDX_Check(pDX, IDC_PROMPT_CATALOG, m_bCheckNotPROMPT_Catlog_Overwrite);
	DDX_Check(pDX, IDC_PROMPT_AUTOSAVE, m_bCheckAutoSaveDrawing);
	DDX_CBStringExact(pDX, IDC_MINOFFSET, m_sComboSEQyOffset);
	DDX_Text(pDX, IDC_STARTNO, m_sTextDrawNoStartNumber);
	DDX_Text(pDX, IDC_DWGVERSION, m_strDwgVersion);
	DDX_Text(pDX, IDC_PRECHAR, m_stextDWGPRE);
	DDX_Text(pDX, IDC_LABLE_PN, m_sLabelStartPN);
	DDX_Check(pDX, IDC_NO_FENQU, m_bNoFenQu);
	DDX_Check(pDX, IDC_NO_TBHQ, m_bTBhq);
	DDX_Check(pDX, IDC_BOM_SPRINGFACTORY, m_bBomSprFactory);
	DDX_Check(pDX, IDC_SUMBOM_SPRINGFACTORY, m_bSumBomSprFactory);
	DDX_Check(pDX, IDC_DIMENSION, m_bDimLocation);
	DDX_Check(pDX, IDC_DRAW_BOM_SUMWEIHT, m_bCheckDraw_SumWeight);
	DDX_Radio(pDX, IDC_GROUPWAREX, m_GroupWare);
	DDX_Text(pDX, IDC_GROUPWARE_EDIT, m_strGroupWareName);
	DDX_Check(pDX, IDC_DRAW_TAG_CIRCLE, m_bCheckTagUsingCircle);
	DDX_Check(pDX, IDC_CUSTOM_PICTURE_NO, m_bCheckCustomPicNo);
	DDX_Check(pDX, IDC_LJMX_BJBW, m_bCheckLJMX_BJBW);
	DDX_Check(pDX, IDC_BG_BJBW, m_bCheckBGForBJBW);
	DDX_Check(pDX, IDC_DW_BJBW, m_bCheckDWForBJBW);
	DDX_Text(pDX, IDC_EDIT_CIRCLE_DIAMETER, m_fCircleDiameter);
	DDX_Check(pDX, IDC_DLOAD_BJBW, m_bCheckDLOADBJBW);
	DDX_Check(pDX, IDC_DRAWINGNAME_WITHOUT_NO, m_bCheckDrawNameWithoutNo);
	DDX_Text(pDX, IDC_EDIT_SEQ_TXTHEIGHT, m_fSeqTxtHeight);
	DDX_Check(pDX, IDC_NO_WAIKUANG, m_bNoWaiKuang);
	DDX_Check(pDX, IDC_NOTDRAWWEIHT, m_bNotDrawWeight);
	DDX_Check(pDX, IDC_INSERT_LENGTH, m_bInsertLength);
	DDX_Check(pDX, IDC_COLDHEATSTATE, m_bColdHeatState);
	DDX_Check(pDX, IDC_INSTALL_ELEVATION, m_bInstallElevation);
	DDX_Check(pDX, IDC_hzabs, m_bHzabs);
	DDX_Check(pDX, IDC_PRINTSA_VALUE, m_bPrintSA_Value);
	DDX_Control(pDX, IDC_GROUPWARE_EDIT, m_GroupWareEdit);
	DDX_Text(pDX, IDC_EDIT_OVERTOP_DIM, m_fOvertopDim);
	DDX_Text(pDX, IDC_EDIT_DISTANCEOFFSET, m_fDistanceOffset);
	DDX_Check(pDX, IDC_CHECK_DIM_NUMBER, m_bDimNumber);
	DDX_Text(pDX, IDC_EDIT_TABLE_WIDTH, m_fSetTableWidth);
	DDX_Text(pDX, IDC_EDIT_TK_WIDTH, m_fSetTKWidth);
	DDX_Radio(pDX, IDC_RCAD, m_IsACAD);
	DDX_Check(pDX, IDC_CHK_DRAWWELDSYMBOL, m_bDrawWeldSymbol);
	DDX_Check(pDX, IDC_CHK_AUTOWELDVIEW_INSERTPOINT, m_bAutoWeldViewInsertPoint);
	DDX_Check(pDX, IDC_CHK_DRAWWELDVIEWDIM, m_bDrawWeldViewDim);
	DDX_Check(pDX, IDC_CHK_AUTO_DIRECTANDPOS, m_bWeldAutoDirectAndPos);
	DDX_Text(pDX, IDC_EDIT1, m_MaxLGLong);
	DDX_Check(pDX, IDC_DIASYMBOL, m_bSymbol);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPreDrawPag, CPropertyPage)
	//{{AFX_MSG_MAP(CPreDrawPag)
	ON_BN_CLICKED(IDC_DRAWSIZE_A3, OnDrawsizeA3)
	ON_BN_CLICKED(IDC_DRAWSIZE_A4, OnDrawsizeA4)
	ON_BN_CLICKED(IDC_DRAWSIZE_AUTO, OnDrawsizeAuto)
	ON_BN_CLICKED(IDC_PHSCOLOR0, OnPhscolor0)
	ON_BN_CLICKED(IDC_PHSCOLOR1, OnPhscolor1)
	ON_BN_CLICKED(IDC_PHSCOLOR2, OnPhscolor2)
	ON_BN_CLICKED(IDC_RADIO230, OnOutinfo0)
	ON_BN_CLICKED(IDC_RADIO231, OnOutinfo1)
	ON_BN_CLICKED(IDC_RADIO233, OnOutinfo2)
	ON_BN_CLICKED(IDC_DWGPRE0, OnDwgpre0)
	ON_BN_CLICKED(IDC_DWGPRE1, OnDwgpre1)
	ON_BN_CLICKED(IDC_DRAWPRENO, OnDrawpreno)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_DRAW_TAG_CIRCLE, OnDrawTagCircle)
	ON_EN_CHANGE(IDC_EDIT_CIRCLE_DIAMETER, OnChangeCircleDiameter)
	ON_CBN_SELCHANGE(IDC_MINOFFSET, OnSelchangeMinOffset)
	ON_CBN_KILLFOCUS(IDC_MINOFFSET, OnKillfocusMinOffset)
	ON_BN_CLICKED(IDC_LJMX_BJBW, OnLjmxBjbw)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_GROUPWARE_RADIO3, OnGroupWareRadio3)
	ON_BN_CLICKED(IDC_GROUPWARE_RADIO2, OnGroupWareRadio2)
	ON_BN_CLICKED(IDC_GROUPWAREX, OnGroupWarex)
	ON_BN_CLICKED(IDC_RCAD, OnRcad)
	ON_BN_CLICKED(IDC_RMICROSTATION, OnRmicrostation)
	ON_BN_CLICKED(IDC_RADIO232, OnOutinfo3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreDrawPag message handlers

void CPreDrawPag::OnDrawsizeA3() 
{
	// TODO: Add your control notification handler code here
	this->CheckRadioButton(IDC_DRAWSIZE_A3,IDC_DRAWSIZE_AUTO,IDC_DRAWSIZE_A3);
}

void CPreDrawPag::OnDrawsizeA4() 
{
	// TODO: Add your control notification handler code here
	this->CheckRadioButton(IDC_DRAWSIZE_A3,IDC_DRAWSIZE_AUTO,IDC_DRAWSIZE_A4);
}

void CPreDrawPag::OnDrawsizeAuto() 
{
	// TODO: Add your control notification handler code here
	this->CheckRadioButton(IDC_DRAWSIZE_A3,IDC_DRAWSIZE_AUTO,IDC_DRAWSIZE_AUTO);
}

void CPreDrawPag::OnPhscolor0() 
{
	// TODO: Add your control notification handler code here
	this->CheckRadioButton(IDC_PHSCOLOR0,IDC_PHSCOLOR2,IDC_PHSCOLOR0);
	
}

void CPreDrawPag::OnPhscolor1() 
{
	// TODO: Add your control notification handler code here
	this->CheckRadioButton(IDC_PHSCOLOR0,IDC_PHSCOLOR2,IDC_PHSCOLOR1);
	
}

void CPreDrawPag::OnPhscolor2() 
{
	// TODO: Add your control notification handler code here
		this->CheckRadioButton(IDC_PHSCOLOR0,IDC_PHSCOLOR2,IDC_PHSCOLOR2);

}

void CPreDrawPag::OnOutinfo0() 
{
	// TODO: Add your control notification handler code here
	this->CheckRadioButton(IDC_RADIO230,IDC_RADIO232,IDC_RADIO230);
	
}

void CPreDrawPag::OnOutinfo1() 
{
	// TODO: Add your control notification handler code here
	this->CheckRadioButton(IDC_RADIO230,IDC_RADIO232,IDC_RADIO231);//PFG20050518
	
}

void CPreDrawPag::OnOutinfo2() 
{
	// TODO: Add your control notification handler code here
	this->CheckRadioButton(IDC_RADIO230,IDC_RADIO232,IDC_RADIO233);
	
}

void CPreDrawPag::OnDwgpre0() 
{
	// TODO: Add your control notification handler code here
	this->CheckRadioButton(IDC_DWGPRE0,IDC_DWGPRE1,IDC_DWGPRE0);
	GetDlgItem(IDC_FRM_DWGPRE)->ShowWindow(SW_HIDE);	
	GetDlgItem(IDC_PRECHAR)->ShowWindow(SW_HIDE);
}

void CPreDrawPag::OnDwgpre1() 
{
	// TODO: Add your control notification handler code here
	this->CheckRadioButton(IDC_DWGPRE0,IDC_DWGPRE1,IDC_DWGPRE1);
	GetDlgItem(IDC_FRM_DWGPRE)->ShowWindow(SW_SHOW);	
	GetDlgItem(IDC_PRECHAR)->ShowWindow(SW_SHOW);
}

void CPreDrawPag::SetCtrlValue()
{
	//lfx 2005.6.30 加 焊缝选项
	m_bDrawWeldSymbol = modPHScal::gbDrawWeldSymbol;
	m_bAutoWeldViewInsertPoint = modPHScal::gbAutoWeldViewInsertPoint;
	m_bDrawWeldViewDim = modPHScal::gbDrawWeldViewDim;
	m_bWeldAutoDirectAndPos = modPHScal::gbWeldAutoDirectAndPos;
	//END lfx 2005.6.30

	m_bNotDrawWeight = EDIBAcad::g_bNotDrawSingleWeight;
	m_bInsertLength = EDIBAcad::g_bInsertLength;
	m_bPrintSA_Value = EDIBAcad::g_bPrintSA_Value;
	m_bInstallElevation = EDIBAcad::g_bInstallElevation;
	m_bColdHeatState = EDIBAcad::g_bColdHeatState;
	m_bHzabs = EDIBAcad::g_bHzabs;
	m_bSymbol = EDIBgbl::bSymbol;
	m_bTBhq = modPHScal::gbTBhq;
	m_bNoWaiKuang = modPHScal::gbNoWaiKuang;
	m_OrientationPicture=modPHScal::gbOrientationPicture;//pfg20050531
	m_bCheckOSFlag = EDIBgbl::gbTbOS;
	m_sComboSEQyOffset = ltos(modPHScal::gnSEQyOffset);
	m_bCheckNotPROMPT_ACAD_SHIFT = modPHScal::gbNotPrompt_ACAD_SHIFT;
	m_IsACAD = modPHScal::gIsACAD;
	m_bCheckNotPROMPT_Catlog_Overwrite = modPHScal::gbNotPrompt_Catlog_OVERWRITE;
	CheckRadioButton(IDC_DRAWSIZE_A3,IDC_DRAWSIZE_AUTO,IDC_DRAWSIZE_A3+modPHScal::giDrawSIZE_A3A4);
	CheckRadioButton(IDC_RCAD,IDC_RMICROSTATION,IDC_RCAD+modPHScal::gIsACAD);
	CheckRadioButton(IDC_RADIO230,IDC_RADIO232,IDC_RADIO230+modPHScal::giPhsOutINFOTableType);//PFG20050523
	if(modPHScal::gsPhsColor == "white")
		OnPhscolor0();
	else if( modPHScal::gsPhsColor == "Green")
		OnPhscolor1();
	else if(modPHScal::gsPhsColor == "Auto")
		OnPhscolor2();
	else
		OnPhscolor0();
	m_bCheckDraw_BOM = modPHScal::gbDraw_BOM;
	m_bCheckAnnotateCSPR = modPHScal::gbAnnotateCSPR;
	
	m_bCheckDrawOverWrite =modPHScal::gbDrawOVERWRITE;
	m_bNoFenQu=modPHScal::gbNoFenQu;
	
	m_bCheckDrawNoPre = modPHScal::gbDrawNoPreMethod;
	m_bCheckAutoSaveDrawing = modPHScal::gbAutoSaveDrawing;
	m_sTextDrawNoStartNumber.Format(_T("%02d"),modPHScal::glDrawNoStartNumber);
	
	this->CheckRadioButton(IDC_DWGPRE0,IDC_DWGPRE1,IDC_DWGPRE0+modPHScal::glDWGPreMethod);
	if(modPHScal::glDWGPreMethod==0)
	{
		GetDlgItem(IDC_FRM_DWGPRE)->ShowWindow(SW_HIDE);	
		GetDlgItem(IDC_PRECHAR)->ShowWindow(SW_HIDE);
	}
	else if(modPHScal::glDWGPreMethod==1)
	{
		GetDlgItem(IDC_FRM_DWGPRE)->ShowWindow(SW_SHOW);	
		GetDlgItem(IDC_PRECHAR)->ShowWindow(SW_SHOW);
	}
	m_stextDWGPRE = modPHScal::gsDWGPre;
	
	m_bCheckDraw_SumWeight = EDIBAcad::g_bDrawSumWeight;
	
	//m_bCheckGroupWare = EDIBAcad::g_bGroupWare;
	m_GroupWare = EDIBAcad::g_bGroupWare;
	m_strDwgVersion = EDIBgbl::strDwgVersion;   //Add by lfx 置初始图号 2005.4.1
	m_MaxLGLong=EDIBgbl::MaxLGLong;//pfg 20050922
	//cbl
	//材料表与零件明细表中的“组件”写成,自定义名字文本框是否显示出来
	
	if(m_GroupWare == 2)
	{	
		m_GroupWareEdit.ShowWindow(SW_SHOW);
	}
	else
	{
		m_GroupWareEdit.ShowWindow(SW_HIDE);
	}
	//

	m_strGroupWareName = EDIBAcad::g_strGroupWareName;

	m_bCheckCustomPicNo = EDIBAcad::g_bCustomPicNo;
	m_bCheckLJMX_BJBW = EDIBAcad::g_bLJMX_BJBW;
	m_bCheckBGForBJBW = EDIBAcad::g_bBGForBJBW;
	m_bCheckDWForBJBW = EDIBAcad::g_bDWForBJBW;

	m_bSumBomSprFactory = EDIBAcad::g_bSumBomSprFactory;
	m_bBomSprFactory = EDIBAcad::g_bBomSprFactory;
	m_bDimLocation = EDIBAcad::g_bLocationDimension;
	m_bCheckDLOADBJBW = EDIBAcad::g_bDLOADBJBW;
	m_bCheckDrawNameWithoutNo = EDIBAcad::g_bDrawNameWithoutNo;
	
	m_fCircleDiameter = modPHScal::gfSeqCircleDiameter;
	m_fSeqTxtHeight = modPHScal::gfSeqTxtHeight;
	
	m_bCheckTagUsingCircle = EDIBAcad::g_bTagUsingCircle;
	if( m_bCheckTagUsingCircle )
	{
		GetDlgItem(IDC_EDIT_CIRCLE_DIAMETER)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_EDIT_CIRCLE_DIAMETER)->EnableWindow(FALSE);
	}

	m_fOvertopDim = EDIBAcad::g_fOvertopDim;
	m_fDistanceOffset = EDIBAcad::g_fDistanceOffset;


	m_fSetTKWidth = EDIBAcad::g_fSetTKWidth;
	m_fSetTableWidth = EDIBAcad::g_fSetTableWidth;

	m_bDimNumber = EDIBAcad::g_bDimNumber;


	UpdateData(false);
}

BOOL CPreDrawPag::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	OnDrawpreno();
	this->OnDrawsizeA3();
	this->OnPhscolor0();
	this->OnDwgpre1();
	this->OnOutinfo1();
	SetCtrlValue();
    //这项功能被取消
	//SetTabFont();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPreDrawPag::OnDrawpreno() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	static CFont font;
	LOGFONT lf;
	lf.lfHeight=8;
	//font.DeleteObject();

	if(m_bCheckDrawNoPre == 1)
	{
		//       LabelStartPN.Caption = "支吊架起始图号" 
		m_sLabelStartPN= GetResStr(IDS_MsgBox_60570) + EDIBgbl::TBNSelPrjVlm;
		//strcpy(lf.lfFaceName,"arial");
		//font.CreateFontIndirect(&lf);
		//m_LabelStartPN.SetFont(&font);
	}
	else
	{
		//       LabelStartPN.Caption = "支吊架起始图号" 
		m_sLabelStartPN= GetResStr(IDS_MsgBox_60570) + EDIBgbl::TBNSelPrjVlm;
		//strcpy(lf.lfFaceName,"arial");
		//font.CreateFontIndirect(&lf);
		//m_LabelStartPN.SetFont(&font);
	}
	UpdateData(false);
}

void CPreDrawPag::SaveIniValue()
{
	UpdateData();
    //这项功能被取消
	//m_fontLjmx->UpdateData();
	//m_fontDim->UpdateData();
	//m_fontLoad->UpdateData();
	modPHScal::gbTBhq=m_bTBhq ;
	modPHScal::gbNoWaiKuang=m_bNoWaiKuang;
	modPHScal::gbOrientationPicture=m_OrientationPicture;//pfg20050531
	EDIBgbl::gbTbOS = m_bCheckOSFlag;
	modPHScal::gbDraw_BOM = m_bCheckDraw_BOM;
	modPHScal::gbNotPrompt_ACAD_SHIFT = m_bCheckNotPROMPT_ACAD_SHIFT;
	modPHScal::gbNotPrompt_Catlog_OVERWRITE = m_bCheckNotPROMPT_Catlog_Overwrite;
	modPHScal::giDrawSIZE_A3A4=this->GetCheckedRadioButton(IDC_DRAWSIZE_A3,IDC_DRAWSIZE_AUTO);
	if(modPHScal::giDrawSIZE_A3A4!=0) modPHScal::giDrawSIZE_A3A4-=IDC_DRAWSIZE_A3;
	modPHScal::gIsACAD=this->GetCheckedRadioButton(IDC_RCAD,IDC_RMICROSTATION);
	if(modPHScal::gIsACAD!=0) modPHScal::gIsACAD-=IDC_RCAD;
	long ix=this->GetCheckedRadioButton(IDC_PHSCOLOR0,IDC_PHSCOLOR2);
	if(ix!=0) ix-=IDC_PHSCOLOR0;
	switch(ix)
	{
		case 0: modPHScal::gsPhsColor="White"; break;
		case 1: modPHScal::gsPhsColor="Green"; break;
		case 2: modPHScal::gsPhsColor="Auto"; break;
	}
	modPHScal::giPhsOutINFOTableType=GetCheckedRadioButton(IDC_RADIO230,IDC_RADIO232);
	if(modPHScal::giPhsOutINFOTableType!=0)modPHScal::giPhsOutINFOTableType -= IDC_RADIO230;
	modPHScal::gbAnnotateCSPR = m_bCheckAnnotateCSPR;
	modPHScal::gbDrawOVERWRITE = m_bCheckDrawOverWrite;

	modPHScal::gbDrawNoPreMethod = m_bCheckDrawNoPre;
	modPHScal::gbAutoSaveDrawing = m_bCheckAutoSaveDrawing;
	modPHScal::glDrawNoStartNumber = _ttoi(m_sTextDrawNoStartNumber);
	modPHScal::glDWGPreMethod = this->GetCheckedRadioButton(IDC_DWGPRE0,IDC_DWGPRE1);
	if(modPHScal::glDWGPreMethod!=0) modPHScal::glDWGPreMethod -=IDC_DWGPRE0;
	modPHScal::gsDWGPre = Trim(m_stextDWGPRE);
	modPHScal::gbNoFenQu=m_bNoFenQu;

	EDIBgbl::strDwgVersion = m_strDwgVersion;   //LFX 2005.4.1 加 保存图形版本号
	EDIBgbl::MaxLGLong=m_MaxLGLong;//pfg20050922
	modPHScal::gnSEQyOffset = _ttoi(m_sComboSEQyOffset);
	modPHScal::gfSeqTxtHeight = m_fSeqTxtHeight;
	
	modPHScal::gbDrawWeldSymbol = m_bDrawWeldSymbol; //lfx 2005.6.30
	modPHScal::gbAutoWeldViewInsertPoint = m_bAutoWeldViewInsertPoint;//lfx 2005.6.30
	modPHScal::gbDrawWeldViewDim = m_bDrawWeldViewDim;//lfx 2005.6.30
	modPHScal::gbWeldAutoDirectAndPos = m_bWeldAutoDirectAndPos; //LFX 2005.7.05

//	if( m_fCircleDiameter <= 10.0 )
		modPHScal::gfSeqCircleDiameter = m_fCircleDiameter;
//	else 
//		modPHScal::gfSeqCircleDiameter = m_fCircleDiameter = 6.0;

	if(EDIBAcad::g_bTagUsingCircle)
	{
		if(modPHScal::gnSEQyOffset<=modPHScal::gfSeqCircleDiameter)
			modPHScal::gnSEQyOffset=modPHScal::gfSeqCircleDiameter+2;
		else if(modPHScal::gnSEQyOffset>modPHScal::gfSeqCircleDiameter+6 )
			modPHScal::gnSEQyOffset=modPHScal::gfSeqCircleDiameter+6;
	}
	else
	{
		if(modPHScal::gnSEQyOffset<6 )
			modPHScal::gnSEQyOffset=6;
		else if(modPHScal::gnSEQyOffset>20 )
			modPHScal::gnSEQyOffset=10;
	}

	EDIBAcad::g_fOvertopDim = m_fOvertopDim;
	EDIBAcad::g_fDistanceOffset = m_fDistanceOffset;

	EDIBAcad::g_fSetTKWidth = m_fSetTKWidth;
	EDIBAcad::g_fSetTableWidth = m_fSetTableWidth;

	m_sComboSEQyOffset.Format("%d",modPHScal::gnSEQyOffset);

	//LFX 2005.6.30 加  焊缝绘制选项
	SetRegValue(_T("Settings"), _T("DrawWeldSymbol"), (m_bDrawWeldSymbol));
	SetRegValue(_T("Settings"), _T("AutoWeldViewInsertPoint"), (m_bAutoWeldViewInsertPoint));
	SetRegValue(_T("Settings"), _T("DrawWeldViewDim"), (m_bDrawWeldViewDim));
	SetRegValue(_T("Settings"), _T("WELDAutoDirectAndPos"), (m_bWeldAutoDirectAndPos));
	//END LFX 2005.6.30 加  焊缝绘制选项


	SetRegValue(_T("Settings"), _T("SEQyOffset"), m_sComboSEQyOffset);
	SetRegValue(_T("Settings"), _T("TbOS"), (m_bCheckOSFlag));
	SetRegValue(_T("Settings"), _T("NotPrompt_ACAD_SHIFT"), (m_bCheckNotPROMPT_ACAD_SHIFT));
	SetRegValue(_T("Settings"), _T("IsACAD"), (m_IsACAD));
	SetRegValue(_T("Settings"), _T("NotPrompt_Catlog_OVERWRITE"), (m_bCheckNotPROMPT_Catlog_Overwrite));
	SetRegValue(_T("Settings"), _T("bNoFenQu"), (m_bNoFenQu));
	SetRegValue(_T("Settings"), _T("bTBhq"), (m_bTBhq));
	SetRegValue(_T("Settings"), _T("bNoWaiKuang"), (m_bNoWaiKuang));
	
	SetRegValue(_T("Settings"), _T("DistanceOffset"), (m_fDistanceOffset));
	SetRegValue(_T("Settings"), _T("OvertopDim"), (m_fOvertopDim));

	SetRegValue(_T("Settings"), _T("SetTKWidth"),    (m_fSetTKWidth));
	SetRegValue(_T("Settings"), _T("SetTableWidth"), (m_fSetTableWidth));

	//SetRegValue(_T("Settings"),_T("CADBigFontLjmx"),modPHScal::strBigFontLjmx);
	//SetRegValue(_T("Settings"),_T("CADBigFontDim"),modPHScal::strBigFontDim);
	//SetRegValue(_T("Settings"),_T("CADBigFontLoad"),modPHScal::strBigFontLoad);
	
	//SetRegValue(_T("Settings"),_T("CADFontWidthFactorDim"),modPHScal::fFWDim);
	//SetRegValue(_T("Settings"),_T("CADFontWidthFactorLjmx"),modPHScal::fFWLjmx);
	//SetRegValue(_T("Settings"),_T("CADFontWidthFactorLoad"),modPHScal::fFWLoad);

	//SetRegValue(_T("Settings"),_T("CADSHXFontLjmx"),modPHScal::strSHXFontLjmx);
	//SetRegValue(_T("Settings"),_T("CADSHXFontDim"),modPHScal::strSHXFontDim);
	//SetRegValue(_T("Settings"),_T("CADSHXFontLoad"),modPHScal::strSHXFontLoad);

	//SetRegValue(_T("Settings"),_T("CADAlignLjmx"),modPHScal::iAlignLjmx);
	//SetRegValue(_T("Settings"),_T("CADAlignLoad"),modPHScal::iAlignLoad);
	//SetRegValue(_T("Settings"),_T("CADAlignDim"),modPHScal::iAlignDim);

	EDIBAcad::g_bSumBomSprFactory = m_bSumBomSprFactory;
	EDIBAcad::g_bBomSprFactory = m_bBomSprFactory;
	EDIBAcad::g_bLocationDimension = m_bDimLocation;
	

	SetRegValue(_T("Settings"),_T("SumBomSprFactory"),m_bSumBomSprFactory);
	SetRegValue(_T("Settings"),_T("BomSprFactory"),m_bBomSprFactory);
	SetRegValue(_T("Settings"),_T("LocationDimension"),m_bDimLocation);

	EDIBAcad::g_bDrawSumWeight = m_bCheckDraw_SumWeight ;
	SetRegValue(_T("Settings"),_T("Draw_SumWeight"),m_bCheckDraw_SumWeight);
	SetRegValue(_T("Settings"),_T("OrientationPicture"),m_OrientationPicture);//pfg20050426

//	EDIBAcad::g_bGroupWare = m_bCheckGroupWare ;
	EDIBAcad::g_bGroupWare = m_GroupWare ;
	EDIBAcad::g_strGroupWareName = m_strGroupWareName;

	SetRegValue(_T("Settings"),_T("GroupWare"),m_GroupWare);
	SetRegValue((_T("Settings")), _T("strGroupWareName"),m_strGroupWareName);

	EDIBAcad::g_bTagUsingCircle = m_bCheckTagUsingCircle ;
	SetRegValue(_T("Settings"),_T("TagUsingCircle"),m_bCheckTagUsingCircle);

	EDIBAcad::g_bCustomPicNo = m_bCheckCustomPicNo ;
	SetRegValue(_T("Settings"),_T("CustomPicNo"),m_bCheckCustomPicNo);

	EDIBAcad::g_bLJMX_BJBW = m_bCheckLJMX_BJBW ;
	SetRegValue(_T("Settings"),_T("LJMX_BJBW"),m_bCheckLJMX_BJBW);

	EDIBAcad::g_bBGForBJBW = m_bCheckBGForBJBW ;
	SetRegValue(_T("Settings"),_T("BGForBJBW"),m_bCheckBGForBJBW);

	EDIBAcad::g_bDWForBJBW = m_bCheckDWForBJBW ;
	SetRegValue(_T("Settings"),_T("DWForBJBW"),m_bCheckDWForBJBW);

	EDIBAcad::g_bDLOADBJBW = m_bCheckDLOADBJBW;
	SetRegValue(_T("Settings"),_T("DLOADBJBW"),m_bCheckDLOADBJBW);

	EDIBAcad::g_bDrawNameWithoutNo = m_bCheckDrawNameWithoutNo;
	SetRegValue(_T("Settings"),_T("DrawNameWithoutNo"),m_bCheckDrawNameWithoutNo);

	SetRegValue(_T("Settings"),_T("CircleDiameter"),m_fCircleDiameter);
	SetRegValue(_T("Settings"),_T("SeqTxtHeight"),m_fSeqTxtHeight);

	EDIBAcad::g_bNotDrawSingleWeight = m_bNotDrawWeight;
	SetRegValue(_T("Settings"),_T("NotDrawSingleWeight"),m_bNotDrawWeight);

	EDIBAcad::g_bInsertLength = m_bInsertLength;
	SetRegValue(_T("Settings"),_T("InsertLength"),m_bInsertLength);

	EDIBAcad::g_bPrintSA_Value = m_bPrintSA_Value;
	SetRegValue(_T("Settings"),_T("PrintSA_Value"),m_bPrintSA_Value);

	EDIBAcad::g_bInstallElevation = m_bInstallElevation;
	SetRegValue(_T("Settings"),_T("InstallElevation"),m_bInstallElevation);

	EDIBAcad::g_bColdHeatState =  m_bColdHeatState;
	SetRegValue(_T("Settings"),_T("ColdHeatState"),m_bColdHeatState);

	EDIBAcad::g_bHzabs =  m_bHzabs;
	SetRegValue(_T("Settings"),_T("HzAbs"),m_bHzabs);

	EDIBAcad::g_bDimNumber = m_bDimNumber;
	SetRegValue(_T("Settings"),_T("DimNumber"),m_bDimNumber);

	EDIBgbl::bSymbol = m_bSymbol;
	SetRegValue(_T("Settings"),_T("DiaSymbol"),m_bSymbol);

}

void CPreDrawPag::SetBasePoint()
{

}

void CPreDrawPag::SetTabFont()
{
    //这项功能已经取消
	//m_fontLjmx=new CCadFontPag(&modPHScal::strSHXFontLjmx,&modPHScal::strBigFontLjmx,&modPHScal::fFWLjmx,&modPHScal::iAlignLjmx);
	//m_fontDim=new CCadFontPag(&modPHScal::strSHXFontDim,&modPHScal::strBigFontDim,&modPHScal::fFWDim,&modPHScal::iAlignDim);
	//m_fontLoad = new CCadFontPag(&modPHScal::strSHXFontLoad,&modPHScal::strBigFontLoad,&modPHScal::fFWLoad,&modPHScal::iAlignLoad);
	//m_fontLjmx->Create(IDD_CADFONT_PAG,&m_tabFont);
	//m_fontDim->Create(IDD_CADFONT_PAG,&m_tabFont);
	//m_fontLoad->Create(IDD_CADFONT_PAG,&m_tabFont);
	//m_fontLjmx->ShowWindow(SW_HIDE);
	//m_fontDim->ShowWindow(SW_HIDE);
	//m_fontLoad->ShowWindow(SW_HIDE);
	//m_tabFont.AddWnd(m_fontLjmx,GetResStr(IDS_PARTBrightTable));
	//m_tabFont.AddWnd(m_fontDim,GetResStr(IDS_SIZE));
	//m_tabFont.AddWnd(m_fontLoad,GetResStr(IDS_LoadDisplacementTable));
	//m_iOldTabFontSel=0;
}

void CPreDrawPag::OnDestroy() 
{
	CPropertyPage::OnDestroy();
	
	// TODO: Add your message handler code here
    //功能被取消
	//delete m_fontLjmx;
	//delete m_fontDim;
	//delete m_fontLoad;
}

void CPreDrawPag::DoDefaultSetting()
{
	m_bDimNumber = TRUE;
	m_bNotDrawWeight = TRUE;
	m_bInsertLength  = TRUE;
	m_bPrintSA_Value = TRUE;
	m_bInstallElevation = TRUE;
	m_bColdHeatState = TRUE;
	m_bHzabs = FALSE;
	m_bTBhq = TRUE;
	m_fSeqTxtHeight = 3.0f;
	m_bNoWaiKuang = FALSE;
	m_OrientationPicture=false;//pfg20050531
	m_bCheckOSFlag = TRUE;
	if(EDIBAcad::g_bTagUsingCircle)
	{
		//用圆圈画，圆圈直径d>=6
		if(modPHScal::gnSEQyOffset<modPHScal::gfSeqCircleDiameter)
		{
			modPHScal::gnSEQyOffset=modPHScal::gfSeqCircleDiameter;
			m_sComboSEQyOffset = ftos(modPHScal::gnSEQyOffset);
		}
	}
	else
		m_sComboSEQyOffset = "8";
	m_bCheckNotPROMPT_ACAD_SHIFT = TRUE;
	m_IsACAD = 0;
	m_bCheckNotPROMPT_Catlog_Overwrite = TRUE;
	m_bCheckDraw_BOM = TRUE;
	m_bCheckAnnotateCSPR = FALSE;
	m_bCheckDrawOverWrite = TRUE;
	m_bNoFenQu= FALSE;
	m_bCheckDrawNoPre = TRUE;
	m_bCheckAutoSaveDrawing = FALSE;
	m_bCheckDraw_SumWeight = TRUE;
//	m_bCheckGroupWare = FALSE;
	m_GroupWare =0;
	m_bCheckTagUsingCircle = FALSE;
	m_bCheckCustomPicNo = FALSE;
	m_bCheckLJMX_BJBW = FALSE;
	m_bCheckBGForBJBW = FALSE;
	m_bCheckDWForBJBW = FALSE;
	m_bCheckDLOADBJBW = FALSE;
	GetDlgItem(IDC_EDIT_CIRCLE_DIAMETER)->EnableWindow(FALSE);
	m_strDwgVersion.Empty();  //Add by LFX 2005.4.1
	m_MaxLGLong=110;//pfg20050922
	CheckRadioButton(IDC_DRAWSIZE_A3,IDC_DRAWSIZE_AUTO,IDC_DRAWSIZE_AUTO);//A4
	CheckRadioButton(IDC_RADIO230,IDC_RADIO233,IDC_RADIO230+1);//位移荷载表
	CheckRadioButton(IDC_DWGPRE0,IDC_DWGPRE1,IDC_DWGPRE0);//卷册号带-号
	GetDlgItem(IDC_FRM_DWGPRE)->ShowWindow(SW_HIDE);	
	GetDlgItem(IDC_PRECHAR)->ShowWindow(SW_HIDE);

	OnPhscolor0();//white

    //这项功能已经取消
	//m_fontDim->DoDefaultSetting();
	//m_fontLjmx->DoDefaultSetting();
	//m_fontLoad->DoDefaultSetting();

	m_bDimLocation = TRUE;
	m_bBomSprFactory = FALSE;
	m_bSumBomSprFactory = FALSE;

	m_bCheckDrawNameWithoutNo = FALSE;


	m_fOvertopDim = 2.0;
	m_fDistanceOffset = 0.0;

	m_fSetTKWidth = 0.3;
	m_fSetTableWidth = 0.0;
	
	//lfx 2005.6.30 加
	m_bDrawWeldSymbol = TRUE;   //默认绘制焊缝符号
	m_bAutoWeldViewInsertPoint = TRUE;
	m_bDrawWeldViewDim = TRUE;
	m_bWeldAutoDirectAndPos = TRUE;
	//END LFX 2005.6.30

	UpdateData(FALSE);
}

void CPreDrawPag::OnDrawTagCircle() 
{
	UpdateData(TRUE);

	if( m_bCheckTagUsingCircle )
	{
		GetDlgItem(IDC_EDIT_CIRCLE_DIAMETER)->EnableWindow(TRUE);
		VerifyCirDiaOrMinOffset();
	}
	else
	{
		GetDlgItem(IDC_EDIT_CIRCLE_DIAMETER)->EnableWindow(FALSE);
	}
}

void CPreDrawPag::OnChangeCircleDiameter() 
{
	VerifyCirDiaOrMinOffset();
}

void CPreDrawPag::OnSelchangeMinOffset() 
{
	
	VerifyCirDiaOrMinOffset(FALSE);

}

void CPreDrawPag::OnKillfocusMinOffset() 
{
	
	VerifyCirDiaOrMinOffset(FALSE);

}

void CPreDrawPag::VerifyCirDiaOrMinOffset(BOOL bVerifyCirDia)
{
	UpdateData(TRUE);
	if( bVerifyCirDia )
	{
		if( m_fCircleDiameter > 10 )
			m_fCircleDiameter = 6;

		float fSEQyOffset = _ttoi(m_sComboSEQyOffset);
		if( m_fCircleDiameter > fSEQyOffset )
		{
			m_sComboSEQyOffset = ltos( m_fCircleDiameter );
		}
	}
	else
	{
		int iSel = m_ComboSEQyOffset.GetCurSel();
		if( iSel < 0 )
			m_ComboSEQyOffset.GetWindowText(m_sComboSEQyOffset);
		else
			m_ComboSEQyOffset.GetLBText(iSel,m_sComboSEQyOffset);
		float fSEQyOffset = _ttoi(m_sComboSEQyOffset);

		if( fSEQyOffset > 40 )
			fSEQyOffset = 10;
		if( fSEQyOffset < 5 )
			fSEQyOffset = 5;

		if( m_bCheckTagUsingCircle )
		{
			if( fSEQyOffset < m_fCircleDiameter )
			{
				 fSEQyOffset = m_fCircleDiameter;
			}
		}		
		m_sComboSEQyOffset = ltos(fSEQyOffset);
	}
	UpdateData(FALSE);
}

void CPreDrawPag::OnLjmxBjbw() 
{
    UpdateData(TRUE);	

	if( m_bCheckLJMX_BJBW )
		m_GroupWare = 1;
	UpdateData(FALSE);
}


void CPreDrawPag::OnLButtonDown(UINT nFlags,CPoint point)
{//chengbailian 2003.12.12 当窗口很大时,在分辨率改变小时,窗口不能全部显示出来,需要移动窗体
	HWND h=::GetActiveWindow();//得到主窗口名柄
	::SendMessage(h,WM_NCLBUTTONDOWN, HTCAPTION, NULL);//发送移动消息
}

void CPreDrawPag::OnGroupWareRadio3()
{
	m_GroupWareEdit.ShowWindow(SW_SHOW);
}

void CPreDrawPag::OnGroupWareRadio2()
{
	m_GroupWareEdit.ShowWindow(SW_HIDE);
}

void CPreDrawPag::OnGroupWarex()
{
	m_GroupWareEdit.ShowWindow(SW_HIDE);

}




void CPreDrawPag::OnRcad() 
{
	// TODO: Add your control notification handler code here
	::CheckRadioButton(this->GetSafeHwnd(),IDC_RCAD,IDC_RMICROSTATION,IDC_RCAD);
}

void CPreDrawPag::OnRmicrostation() 
{
	// TODO: Add your control notification handler code here
	::CheckRadioButton(this->GetSafeHwnd(),IDC_RCAD,IDC_RMICROSTATION,IDC_RMICROSTATION);
}

void CPreDrawPag::OnOutinfo3() //PFG20050518
{
	this->CheckRadioButton(IDC_RADIO230,IDC_RADIO232,IDC_RADIO232);
	
}
