///////////////////////////////////////////////
// 字体设置对话框类实现文件
// PreStylePag.cpp
// 设计&编程：罗超
// 日     期：2003/10/27

#include "stdafx.h"
#include "math.h"
#include "user.h"
#include "basDirectory.h"

#include "PreStylePag.h"
#include "autophs.h"
#include "NewStyleDlg.h"
#include "modPHScal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CAutoPHSApp theApp;
/////////////////////////////////////////////////////////////////////////////
// CPreStylePag property page

IMPLEMENT_DYNCREATE(CPreStylePag, CPropertyPage)

CPreStylePag::CPreStylePag() : CPropertyPage(CPreStylePag::IDD)
{
	//{{AFX_DATA_INIT(CPreStylePag)
	m_blBackward = FALSE;
	m_blUp2Down = FALSE;
	m_blVertical = FALSE;
	m_dHeight = 0.0;
	m_dAngle = 0.0;
	m_dScaleFactor = 0.0;
	m_nBigFonts = 0;
	m_nShxFonts = 0;
	m_nStyle = 0;
	m_nTable = 0;
	m_nAlign = 0;
	m_strMsg = _T("");
	//}}AFX_DATA_INIT
	ActiveTimes = 0;
}

CPreStylePag::~CPreStylePag()
{
}

void CPreStylePag::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreStylePag)
	DDX_Check(pDX, IDC_CHECK_BACKWARD, m_blBackward);
	DDX_Check(pDX, IDC_CHECK_UP2DOWN, m_blUp2Down);
	DDX_Check(pDX, IDC_CHECK_VERTICAL, m_blVertical);
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_dHeight);
	DDV_MinMaxDouble(pDX, m_dHeight, 0., 100000000.);
	DDX_Text(pDX, IDC_EDIT_OBLIQUEANGLE, m_dAngle);
	DDV_MinMaxDouble(pDX, m_dAngle, -85., 85.);
	DDX_Text(pDX, IDC_EDIT_SCALEFACTOR, m_dScaleFactor);
	DDV_MinMaxDouble(pDX, m_dScaleFactor, 0., 100000000.);
	DDX_CBIndex(pDX, IDC_COMBO_BIGFONTS, m_nBigFonts);
	DDX_CBIndex(pDX, IDC_COMBO_SHXFONTS, m_nShxFonts);
	DDX_CBIndex(pDX, IDC_COMBO_STYLE, m_nStyle);
	DDX_CBIndex(pDX, IDC_COMBO_TABLE, m_nTable);
	DDX_CBIndex(pDX, IDC_COMBO_ALIGN, m_nAlign);
	DDX_Text(pDX, IDC_STATIC_MSG, m_strMsg);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPreStylePag, CPropertyPage)
	//{{AFX_MSG_MAP(CPreStylePag)
	ON_CBN_SELCHANGE(IDC_COMBO_TABLE, OnSelchangeComboTable)
	ON_CBN_SELCHANGE(IDC_COMBO_STYLE, OnSelchangeComboStyle)
	ON_BN_CLICKED(IDC_BTN_NEWSTYLE, OnBtnNewstyle)
	ON_BN_CLICKED(IDC_BTN_DELETESTYLE, OnBtnDeletestyle)
	ON_BN_CLICKED(IDC_BTN_DEFAULT, OnBtnDefault)
	ON_BN_CLICKED(IDC_BTN_SAVE, OnBtnSave)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreStylePag message handlers

void CPreStylePag::SaveIniValue(void)
{
    CString   strStyle, strShx, strBig;
    CComboBox *lpStyle, *lpShx, *lpBig;
    int       nShx, nBig, iGenFlag;
    BOOL      blIsFailed;

    UpdateTableData(TRUE);
	
    lpStyle = (CComboBox *)GetDlgItem(IDC_COMBO_STYLE);
    lpShx = (CComboBox *)GetDlgItem(IDC_COMBO_SHXFONTS);
    lpBig = (CComboBox *)GetDlgItem(IDC_COMBO_BIGFONTS);
    nShx = lpShx->GetCount();
    nBig = lpBig->GetCount();


    //保存当前样式值
	if(lpStyle->GetCount()>0)//pfg20050330
	{
	 lpStyle->GetLBText(m_nStyle, strStyle);
	}
    lpShx->GetLBText(m_nShxFonts, strShx);
    lpBig->GetLBText(m_nBigFonts, strBig);
    iGenFlag = 0;
    if(m_blBackward == TRUE) iGenFlag += 2;
    if(m_blUp2Down == TRUE)   iGenFlag += 4;
    ModifyTextStyleProperty(strStyle, strShx, strBig,
        m_dHeight, m_dScaleFactor, m_dAngle, iGenFlag);

    //保存所有表格的当前设置
    pCurTable = &dim_style;
    SaveTable2Registry();

    pCurTable = &ljmx_style;
    SaveTable2Registry();

    pCurTable = &load_style;
    SaveTable2Registry();

    pCurTable = &Comments_style;
    SaveTable2Registry();

    pCurTable = &BH_style;
    SaveTable2Registry();

    pCurTable = &BG_style;
    SaveTable2Registry();

    pCurTable = &other_style;
    SaveTable2Registry();

    //将对话框中的控件变量值保存到modPHScal类成员变量中

    blIsFailed = FALSE;
    if(nShx <= 0) blIsFailed = TRUE;
    if(nBig <= 0) blIsFailed = TRUE;
    if(dim_style.nShxFonts >= nShx) blIsFailed = TRUE;
    if(dim_style.nBigFonts >= nBig) blIsFailed = TRUE;
    if(dim_style.nShxFonts <  0) blIsFailed = TRUE;
    if(dim_style.nBigFonts <  0) blIsFailed = TRUE;

    if(blIsFailed)
    {
        strShx = _T("txt.shx");
        strBig = _T("gbcbig.shx");
    }
    else
    {
        lpShx->GetLBText(dim_style.nShxFonts, strShx);
        if(dim_style.nBigFonts == 0)
        {
            strBig = " ";
        }
        else
        {
            lpBig->GetLBText(dim_style.nBigFonts, strBig);
        }
    }
    modPHScal::strSHXFontDim  = strShx;
    modPHScal::strBigFontDim  = strBig;

    blIsFailed = FALSE;
    if(nShx <= 0) blIsFailed = TRUE;
    if(nBig <= 0) blIsFailed = TRUE;
    if(ljmx_style.nShxFonts >= nShx) blIsFailed = TRUE;
    if(ljmx_style.nBigFonts >= nBig) blIsFailed = TRUE;
    if(ljmx_style.nShxFonts <  0) blIsFailed = TRUE;
    if(ljmx_style.nBigFonts <  0) blIsFailed = TRUE;

    if(blIsFailed)
    {
        strShx = _T("txt.shx");
        strBig = _T("gbcbig.shx");
    }
    else
    {
        lpShx->GetLBText(ljmx_style.nShxFonts, strShx);
        if(ljmx_style.nBigFonts == 0)
        {
            strBig = " ";
        }
        else
        {
            lpBig->GetLBText(ljmx_style.nBigFonts, strBig);
        }
    }
    modPHScal::strSHXFontLjmx = strShx;
    modPHScal::strBigFontLjmx = strBig;

    blIsFailed = FALSE;
    if(nShx <= 0) blIsFailed = TRUE;
    if(nBig <= 0) blIsFailed = TRUE;
    if(load_style.nShxFonts >= nShx) blIsFailed = TRUE;
    if(load_style.nBigFonts >= nBig) blIsFailed = TRUE;
    if(load_style.nShxFonts <  0) blIsFailed = TRUE;
    if(load_style.nBigFonts <  0) blIsFailed = TRUE;

    if(blIsFailed)
    {
        strShx = _T("txt.shx");
        strBig = _T("gbcbig.shx");
    }
    else
    {
        lpShx->GetLBText(load_style.nShxFonts, strShx);
        if(load_style.nBigFonts == 0)
        {
            strBig = " ";
        }
        else
        {
            lpBig->GetLBText(load_style.nBigFonts, strBig);
        }
    }
    modPHScal::strSHXFontLoad = strShx;
    modPHScal::strBigFontLoad = strBig;

    modPHScal::fFWDim  = dim_style.dScaleFactor;
    modPHScal::fFWLjmx = ljmx_style.dScaleFactor;
    modPHScal::fFWLoad = load_style.dScaleFactor;

    modPHScal::iAlignDim  = dim_style.nAlign;
    modPHScal::iAlignLjmx = ljmx_style.nAlign;
    modPHScal::iAlignLoad = load_style.nAlign;

    //将对话框中的控件变量值保存到注册表相应字段中
	SetRegValue(_T("Settings"), _T("CADBigFontDim"),  modPHScal::strBigFontDim);
	SetRegValue(_T("Settings"), _T("CADBigFontLjmx"), modPHScal::strBigFontLjmx);
	SetRegValue(_T("Settings"), _T("CADBigFontLoad"), modPHScal::strBigFontLoad);
	
	SetRegValue(_T("Settings"), _T("CADFontWidthFactorDim"),  modPHScal::fFWDim);
	SetRegValue(_T("Settings"), _T("CADFontWidthFactorLjmx"), modPHScal::fFWLjmx);
	SetRegValue(_T("Settings"), _T("CADFontWidthFactorLoad"), modPHScal::fFWLoad);

	SetRegValue(_T("Settings"), _T("CADSHXFontDim"),  modPHScal::strSHXFontDim);
	SetRegValue(_T("Settings"), _T("CADSHXFontLjmx"), modPHScal::strSHXFontLjmx);
	SetRegValue(_T("Settings"), _T("CADSHXFontLoad"), modPHScal::strSHXFontLoad);

	SetRegValue(_T("Settings"), _T("CADAlignDim"),  modPHScal::iAlignDim);
	SetRegValue(_T("Settings"), _T("CADAlignLjmx"), modPHScal::iAlignLjmx);
    SetRegValue(_T("Settings"), _T("CADAlignLoad"), modPHScal::iAlignLoad);
}



void CPreStylePag::SetCtrlValue(void)
{
    CString strShx, strBig;
    CComboBox *lpShx, *lpBig;

    lpShx = (CComboBox *)GetDlgItem(IDC_COMBO_SHXFONTS);
    lpBig = (CComboBox *)GetDlgItem(IDC_COMBO_BIGFONTS);

    //用modPHScal类成员变量的值初始化对话框中的控件变量

    strShx  = modPHScal::strSHXFontDim;
    strBig  = modPHScal::strBigFontDim;
    dim_style.nShxFonts = lpShx->FindString(0, strShx);
    dim_style.nBigFonts = lpBig->FindString(0, strBig);
    if(dim_style.nShxFonts == -1)
    {
        lpShx->AddString(strShx);
        dim_style.nShxFonts = lpShx->GetCount() -1;
    }
    if(dim_style.nBigFonts == -1)
    {
        lpBig->AddString(strBig);
        dim_style.nBigFonts = lpBig->GetCount() -1;
    }

    strShx = modPHScal::strSHXFontLjmx;
    strBig = modPHScal::strBigFontLjmx;
    ljmx_style.nShxFonts = lpShx->FindString(0, strShx);
    ljmx_style.nBigFonts = lpBig->FindString(0, strBig);
    if(ljmx_style.nShxFonts == -1)
    {
        lpShx->AddString(strShx);
        ljmx_style.nShxFonts = lpShx->GetCount() -1;
    }
    if(ljmx_style.nBigFonts == -1)
    {
        lpBig->AddString(strBig);
        ljmx_style.nBigFonts = lpBig->GetCount() -1;
    }
    
    strShx = modPHScal::strSHXFontLoad;
    strBig = modPHScal::strBigFontLoad;
    load_style.nShxFonts = lpShx->FindString(0, strShx);
    load_style.nBigFonts = lpBig->FindString(0, strBig);
    if(load_style.nShxFonts == -1)
    {
        lpShx->AddString(strShx);
        load_style.nShxFonts = lpShx->GetCount() -1;
    }
    if(load_style.nBigFonts == -1)
    {
        lpBig->AddString(strBig);
        load_style.nBigFonts = lpBig->GetCount() -1;
    }

    dim_style.dScaleFactor  = modPHScal::fFWDim;
    ljmx_style.dScaleFactor = modPHScal::fFWLjmx;
    load_style.dScaleFactor = modPHScal::fFWLoad;

    dim_style.nAlign  = modPHScal::iAlignDim;
    ljmx_style.nAlign = modPHScal::iAlignLjmx;
    load_style.nAlign = modPHScal::iAlignLoad;
}

BOOL CPreStylePag::UpdateTableData(BOOL bSaveAndValidate)
{
    //保存或返回当前表数据
	double dheight;
    if(bSaveAndValidate == TRUE)//保存：将界面数据保存到内存变量
    {
        if(UpdateData(TRUE) == FALSE) return FALSE;
		if(m_dHeight<3)//pfg20050330
		{
			dheight=3;//pfg20050330
		}
		else
		{
			dheight=m_dHeight;//pfg20050330
		}
		SetRegValue(_T("Settings"), _T("标注尺寸-HEIGHT"), dheight);//PFG20050329
        pCurTable->blBackward   = m_blBackward   ;
        pCurTable->blUp2Down    = m_blUp2Down    ;
        pCurTable->blVertical   = m_blVertical   ;

        pCurTable->dHeight      = SetPrecision(m_dHeight, -2)      ;
        pCurTable->dAngle       = SetPrecision(m_dAngle, -2)       ;
        pCurTable->dScaleFactor = SetPrecision(m_dScaleFactor, -2) ;

        m_dHeight               = pCurTable->dHeight      ;
        m_dAngle                = pCurTable->dAngle       ;
        m_dScaleFactor          = pCurTable->dScaleFactor ;

        pCurTable->nBigFonts    = m_nBigFonts    ;
        pCurTable->nShxFonts    = m_nShxFonts    ;
        pCurTable->nStyle       = m_nStyle       ;
        //pCurTable->nTable     只读属性         ;
        pCurTable->nAlign       = m_nAlign       ;

        UpdateData(FALSE);
		return TRUE;
    }
    else // == FALSE，返回：将数据显示到界面上
    {
        m_blBackward   = pCurTable->blBackward   ;
        m_blUp2Down    = pCurTable->blUp2Down    ;
        m_blVertical   = pCurTable->blVertical   ;

        m_dHeight      = SetPrecision(pCurTable->dHeight, -2)      ;
        m_dAngle       = SetPrecision(pCurTable->dAngle, -2)       ;
        m_dScaleFactor = SetPrecision(pCurTable->dScaleFactor, -2) ;

        pCurTable->dHeight      = m_dHeight      ;
        pCurTable->dAngle       = m_dAngle       ;
        pCurTable->dScaleFactor = m_dScaleFactor ;

        m_nBigFonts    = pCurTable->nBigFonts    ;
        m_nShxFonts    = pCurTable->nShxFonts    ;
        m_nStyle       = pCurTable->nStyle       ;
        m_nTable       = pCurTable->nTable       ;
        m_nAlign       = pCurTable->nAlign       ;

        return UpdateData(FALSE);
    }
}

void CPreStylePag::OnSelchangeComboTable() 
{
    int nOldTable;
    int nOldStyle;
    CComboBox *lpStyle, *lpShx, *lpBig;
    CString    strStyleName, strShxFile, strBigFile;
    short      iGenerationFlag;

    nOldTable = m_nTable;
    nOldStyle = m_nStyle;

    UpdateTableData(TRUE);
	m_dHeight=0;//pfg20050330
    //选择没有更改
    if(nOldTable == m_nTable) return ;

    //用户选择了不同的表
    //保存当前样式的属性值
    lpStyle = (CComboBox *)GetDlgItem(IDC_COMBO_STYLE);
    lpShx   = (CComboBox *)GetDlgItem(IDC_COMBO_SHXFONTS);
    lpBig   = (CComboBox *)GetDlgItem(IDC_COMBO_BIGFONTS);
	if(lpStyle->GetCount()>0)//pfg20050330
	{
		lpStyle->GetLBText(nOldStyle, strStyleName);
	}
    lpShx  ->GetLBText(m_nShxFonts, strShxFile);
    lpBig  ->GetLBText(m_nBigFonts, strBigFile);

    iGenerationFlag = 0;
    if(m_blBackward == TRUE) iGenerationFlag += 2;
    if(m_blUp2Down  == TRUE) iGenerationFlag += 4;

    //保存更改被更改的样式属性
    ModifyTextStyleProperty(strStyleName, strShxFile, strBigFile,
        m_dHeight, m_dScaleFactor, m_dAngle, iGenerationFlag);

    switch(m_nTable)
    {
    case 0://选择了 零件明细表
        pCurTable = &ljmx_style;
        break;
    case 1://选择了 标注尺寸
        pCurTable = &dim_style;
		m_dHeight=GetRegKey(_T("Settings"), _T("标注尺寸-HEIGHT"),3);//pfg20050330
        break;
    case 2://选择了 荷载位移表
        pCurTable = &load_style;
        break;
    case 3://选择了 注释
        pCurTable = &Comments_style;
        break;
    case 4://选择了 编号
        pCurTable = &BH_style;
        break;
    case 5://选择了 标高
        pCurTable = &BG_style;
        break;
    case 6://选择了 其它文字
        pCurTable = &other_style;
        break;
    default://错误选择，给出测试提示信息
#ifdef _DEBUG
        MessageBox("字体设置标签-[选择表格]出现错误：用户的选择可能出现非法值，请查实！！！", "！！！错误", 0);
#endif
        //将错误表格数据恢复
        m_nTable = nOldTable;
        break;
    }
    if(m_nTable != nOldTable)
    {
        //导入新表格的样式数据 
		if(lpStyle->GetCount()>0)//pfg20050330
		{
			lpStyle->GetLBText(pCurTable->nStyle, strStyleName);
		}
        GetTextStyleProperty(strStyleName, strShxFile, strBigFile,
            pCurTable->dHeight, pCurTable->dScaleFactor,
            pCurTable->dAngle, iGenerationFlag);
        
        pCurTable->nShxFonts = lpShx->FindString(0, strShxFile);
        if(pCurTable->nShxFonts == CB_ERR)
        {
            lpShx->AddString(strShxFile);
            pCurTable->nShxFonts = lpShx->GetCount() - 1;
        }

        pCurTable->nBigFonts = lpBig->FindString(0, strBigFile);
        if(pCurTable->nBigFonts == CB_ERR) pCurTable->nBigFonts = 0;
        
        if(iGenerationFlag & 0x02) pCurTable->blBackward = TRUE;
        else                       pCurTable->blBackward = FALSE;
        if(iGenerationFlag & 0x04) pCurTable->blUp2Down  = TRUE;
        else                       pCurTable->blUp2Down  = FALSE;
    }
    UpdateTableData(FALSE);
}

void CPreStylePag::OnSelchangeComboStyle() 
{
    CComboBox *lpStyle, *lpShx, *lpBig;
    CString    strStyleName, strShxFile, strBigFile;
    CString    tmpstr;
    short      iGenerationFlag;
    int        oldStyle, nIndex;

    //获取旧样式
    oldStyle = m_nStyle;

    //保存新数据
    UpdateTableData(TRUE);

    //用户的选择没有更改
    if(oldStyle == m_nStyle) return ;

    //用户选择了不同的style
    lpStyle = (CComboBox *)GetDlgItem(IDC_COMBO_STYLE);
    lpShx   = (CComboBox *)GetDlgItem(IDC_COMBO_SHXFONTS);
    lpBig   = (CComboBox *)GetDlgItem(IDC_COMBO_BIGFONTS);

    lpStyle->GetLBText(oldStyle, strStyleName);
    lpShx  ->GetLBText(m_nShxFonts, strShxFile);
    lpBig  ->GetLBText(m_nBigFonts, strBigFile);

    iGenerationFlag = 0;
    if(m_blBackward == TRUE) iGenerationFlag += 2;
    if(m_blUp2Down  == TRUE) iGenerationFlag += 4;

    //保存更改被更改的样式属性
    ModifyTextStyleProperty(strStyleName, strShxFile, strBigFile,
        m_dHeight, m_dScaleFactor, m_dAngle, iGenerationFlag);

    //读取新style的数据
    lpStyle->GetLBText(m_nStyle, strStyleName);
    GetTextStyleProperty(strStyleName, strShxFile, strBigFile,
        pCurTable->dHeight, pCurTable->dScaleFactor,
        pCurTable->dAngle, iGenerationFlag);

    pCurTable->nShxFonts = lpShx->FindString(0, strShxFile);
    if(pCurTable->nShxFonts == CB_ERR)
    {
        lpShx->AddString(strShxFile);
        pCurTable->nShxFonts = lpShx->GetCount() - 1;
    }
    pCurTable->nBigFonts = lpBig->FindString(0, strBigFile);
    if(pCurTable->nBigFonts == CB_ERR) pCurTable->nBigFonts = 0;

    if(iGenerationFlag & 0x02) pCurTable->blBackward = TRUE;
    else                       pCurTable->blBackward = FALSE;
    if(iGenerationFlag & 0x04) pCurTable->blUp2Down  = TRUE;
    else                       pCurTable->blUp2Down  = FALSE;

    //更新界面显示
    UpdateTableData(FALSE);
}

BOOL CPreStylePag::CreateTextStyle(
                       CString strTSName,
                       CString strShxFile,
                       CString strBigFile,
                       double  dHeight,
                       double  dWidthFactor,
                       double  dAngleByDegree,
                       short   iGenerationFlag)
{
    //局部变量定义和初始化
    CMObject obj, newobj, tsobj;
    long      i, nCount;
    CString  tmpTSName, strtmp;

    strTSName.TrimLeft();
    strTSName.TrimRight();
    if(strTSName.GetLength() <= 0) return FALSE;

    strShxFile.TrimLeft();
    strShxFile.TrimRight();
    if(strShxFile.GetLength() <= 0) return FALSE;

    strBigFile.TrimLeft();
    strBigFile.TrimRight();
    if(strBigFile.GetLength() <= 0) return FALSE;
    if(strBigFile == "不使用大字体") strBigFile = " ";

    if(dHeight < 0.0) dHeight = 0.0;
    if(dWidthFactor < 0.0) dWidthFactor = 1.0;
    if(dAngleByDegree > 85.0 || dAngleByDegree < -85.0) dAngleByDegree = 0.0;
    iGenerationFlag &= 0x06;
    tmpTSName = strTSName;
    tmpTSName.MakeUpper();

    try
    {
        if(FindAcad() == FALSE)
        {
            return FALSE;
        }
        
        //SETUP-1: 获取ACAD文本样式列表
        obj = EDIBAcad::objAcadDoc.GetPropertyByName(
                                      _T("TextStyles")).pdispVal;
        
        //SETUP-2: 如果指定样式对象已经存在，则返回

		nCount = (long)obj.GetPropertyByName(_T("Count"));
		for( i = 0; i < nCount; i++)
		{
			tsobj = obj.Invoke(_T("Item"), 1, &_variant_t(i)).pdispVal;
			strtmp = vtos(tsobj.GetPropertyByName(_T("Name")));
			strtmp.TrimLeft();
            strtmp.TrimRight();
            strtmp.MakeUpper();
			if(strtmp == tmpTSName)
			{
                //目标样式已存在
                return FALSE;
			}
		}

        //SETUP-3: 添加新的文本样式
        obj.Invoke(_T("Add"),
                   1,
                   &_variant_t(strTSName));
        
        //SETUP-4: 获取新样式对象
        newobj = obj.Invoke(_T("Item"),
                            1,
                            &_variant_t(strTSName)).pdispVal;
        
        //SETUP-5: 设定新样式的属性值
        if(newobj.p)//成功获取新对象
        {
            newobj.PutPropertyByName(_T("FontFile"),           //设定字体文件名
                   &_variant_t(strShxFile));

            newobj.PutPropertyByName(_T("BigFontFile"),        //设定大字体文件名
                   &_variant_t(strBigFile));

            newobj.PutPropertyByName(_T("Height"),             //设定字体高度
                   &_variant_t(dHeight));
            newobj.PutPropertyByName(_T("Width"),              //设定宽度比例因子***
                   &_variant_t(dWidthFactor));
            newobj.PutPropertyByName(_T("ObliqueAngle"),       //设定字体倾斜角度
                   &_variant_t(dAngleByDegree));
            newobj.PutPropertyByName(_T("TextGenerationFlag"), //设定字体生成字段
                   &_variant_t(iGenerationFlag));
            
            return TRUE;
        }
    }
	catch(CException *e)
	{
		e->Delete();
	}
    return FALSE;
}

BOOL CPreStylePag::ModifyTextStyleProperty(
                       CString strTSName,
                       CString strShxFile,
                       CString strBigFile,
                       double  dHeight,
                       double  dWidthFactor,
                       double  dAngleByDegree,
                       short   iGenerationFlag)
{
    CMObject obj, tsobj;
    long     i, nCount;
    CString  strtmp;
//    HRESULT  retCode;

    //进行参数检查和初始化
    strTSName.TrimLeft();
    strTSName.TrimRight();
    strTSName.MakeUpper();
    if(strTSName.GetLength() <= 0) return FALSE;

    strShxFile.TrimLeft();
    strShxFile.TrimRight();
    if(strShxFile.GetLength() <= 0) return FALSE;

    strBigFile.TrimLeft();
    strBigFile.TrimRight();
    if(strBigFile.GetLength() <= 0) return FALSE;
    if(strBigFile == "不使用大字体") strBigFile = "";

    if(dHeight < 0.0) return FALSE;
    if(dWidthFactor < 0.0) return FALSE;
    if(dAngleByDegree > 85.0 || dAngleByDegree < -85.0) return FALSE;
    iGenerationFlag &= 0x06;
    if(dAngleByDegree < 0) dAngleByDegree += 360;
    dAngleByDegree *= 3.14159265359;
    dAngleByDegree /= 180.0;

    try
    {
        if(FindAcad() == FALSE)
        {
            return FALSE;
        }
        
        //SETUP-1: 获取ACAD文本样式列表
        obj = EDIBAcad::objAcadDoc.GetPropertyByName(
                                      _T("TextStyles")).pdispVal;
        
        //SETUP-2: 获取样式对象

		nCount = (long)obj.GetPropertyByName(_T("Count"));
		for( i = 0; i < nCount; i++)
		{
			tsobj = obj.Invoke(_T("Item"), 1, &_variant_t(i)).pdispVal;
			strtmp = vtos(tsobj.GetPropertyByName(_T("Name")));
			strtmp.TrimLeft();
            strtmp.TrimRight();
			strtmp.MakeUpper();
			if(strtmp == strTSName)
			{
                //更改目标样式的属性值
                tsobj.PutPropertyByName(_T("FontFile"),           //设定字体文件名
                                       &_variant_t(strShxFile));
                
                tsobj.PutPropertyByName(_T("BigFontFile"),        //设定大字体文件名
                                       &_variant_t(strBigFile));
                
                tsobj.PutPropertyByName(_T("Height"),             //设定字体高度
                                       &_variant_t(dHeight));
                tsobj.PutPropertyByName(_T("Width"),              //设定宽度比例因子***
                                       &_variant_t(dWidthFactor));
                tsobj.PutPropertyByName(_T("ObliqueAngle"),       //设定字体倾斜角度
                                       &_variant_t(dAngleByDegree));
                tsobj.PutPropertyByName(_T("TextGenerationFlag"), //设定字体生成字段
                                       &_variant_t(iGenerationFlag));
                return TRUE;
			}
		}
    }
	catch(CException *e)
	{
		e->Delete();
	}
    return FALSE;
}

BOOL CPreStylePag::GetTextStyleProperty(
                       CString  strTSName,
                       CString &strShxFile,
                       CString &strBigFile,
                       double  &dHeight,
                       double  &dWidthFactor,
                       double  &dAngleByDegree,
                       short   &iGenerationFlag)
{
    CMObject obj, tsobj;
    long     i, nCount;
    CString  strtmp;

    //进行参数检查和初始化
    strTSName.TrimLeft();
    strTSName.TrimRight();
    strTSName.MakeUpper();
    if(strTSName.GetLength() <= 0) return FALSE;

    try
    {
        if(FindAcad() == FALSE)
        {
            return FALSE;
        }
        
        //SETUP-1: 获取ACAD文本样式列表
        obj = EDIBAcad::objAcadDoc.GetPropertyByName(
                                      _T("TextStyles")).pdispVal;
        
        //SETUP-2: 获取样式对象

		nCount = (long)obj.GetPropertyByName(_T("Count"));
		for( i = 0; i < nCount; i++)
		{
			tsobj = obj.Invoke(_T("Item"), 1, &_variant_t(i)).pdispVal;
			strtmp = vtos(tsobj.GetPropertyByName(_T("Name")));
			strtmp.TrimLeft();
            strtmp.TrimRight();
			strtmp.MakeUpper();
			if(strtmp == strTSName)
			{
                //获得目标样式的属性值
                strShxFile      = vtos(tsobj.GetPropertyByName(
                                 _T("FontFile")));   //获得字体文件名
                strBigFile      = vtos(tsobj.GetPropertyByName(
                                 _T("BigFontFile")));//获得大字体文件名
                
                dHeight         = vtod(tsobj.GetPropertyByName(
                                 _T("Height")));     //获得字体高度

                dWidthFactor    = vtod(tsobj.GetPropertyByName(
                                 _T("Width")));      //获得宽度比例因子***

                dAngleByDegree  = vtod(tsobj.GetPropertyByName(
                                 _T("ObliqueAngle")));        //获得字体倾斜角度
                dAngleByDegree  *= 180.0;
                dAngleByDegree  /= 3.14159265359;
                if(dAngleByDegree > 270.0) dAngleByDegree -= 360;
                
                iGenerationFlag = vtoi(tsobj.GetPropertyByName(
                                 _T("TextGenerationFlag")));  //获得字体生成字段
                iGenerationFlag &= 0x06;

                return TRUE;
			}
		}
    }
	catch(CException *e)
	{
		e->Delete();
	}
    return FALSE;
}

BOOL CPreStylePag::DeleteTextStyle(CString strTSName)
{
    CMObject obj, tsobj;
    long     i, nCount;
    CString  strtmp;

    //进行参数检查和初始化
    strTSName.TrimLeft();
    strTSName.TrimRight();
    strTSName.MakeUpper();
    if(strTSName.GetLength() <= 0) return FALSE;

    try
    {
        if(FindAcad() == FALSE)
        {
            return FALSE;
        }
        
        //SETUP-1: 获取ACAD文本样式列表
        obj = EDIBAcad::objAcadDoc.GetPropertyByName(
                                      _T("TextStyles")).pdispVal;
        
        //SETUP-2: 获取样式对象
		nCount = (long)obj.GetPropertyByName(_T("Count"));
		for( i = 0; i < nCount; i++)
		{
			tsobj = obj.Invoke(_T("Item"), 1, &_variant_t(i)).pdispVal;
			strtmp = vtos(tsobj.GetPropertyByName(_T("Name")));
			strtmp.TrimLeft();
            strtmp.TrimRight();
			strtmp.MakeUpper();
			if(strtmp == strTSName)
			{
                //删除目标样式
                tsobj.Invoke0(_T("Delete"));
                return TRUE;
			}
		}
    }
	catch(CException *e)
	{
		e->Delete();
	}
    return FALSE;
}

void CPreStylePag::OnBtnNewstyle() 
{
    //局部变量定义和初始化
    CMObject   obj, newobj;
    CString    newTextStyleName, tmpStr;
    CComboBox *lpStyle, *lpShxFonts, *lpBigFonts;

    lpStyle    = (CComboBox *)GetDlgItem(IDC_COMBO_STYLE);
    lpShxFonts = (CComboBox *)GetDlgItem(IDC_COMBO_SHXFONTS);
    lpBigFonts = (CComboBox *)GetDlgItem(IDC_COMBO_BIGFONTS);

    try
    {
        if(FindAcad() == FALSE)
        {
            return;
        }
        
        //获取新字体样式的名字
        CNewStyleDlg dlg;
        if(dlg.DoModal() != IDOK) return ;
        newTextStyleName = dlg.m_strStyleName;
        if(lpStyle->FindString(0, newTextStyleName) != -1)
        {
            MessageBox("文字样式 [" + newTextStyleName + "] 已存在，不许重复创建！");
            return ;
        }
        
        //创建新的字体样式
        //SETUP-1: 获取ACAD文本样式列表
        obj = EDIBAcad::objAcadDoc.GetPropertyByName(
                                      _T("TextStyles")).pdispVal;
        
        //SETUP-2: 添加新的文本样式
        obj.Invoke(_T("Add"),
                   1,
                   &_variant_t(newTextStyleName));
        
        //SETUP-3: 获取新样式对象
        newobj = obj.Invoke(_T("Item"),
                            1,
                            &_variant_t(newTextStyleName)).pdispVal;
        
        //SETUP-4: 设定新样式的属性值
        if(newobj.p)//成功获取新对象
        {
            lpShxFonts->GetLBText(0, tmpStr);
            newobj.PutPropertyByName(_T("FontFile"),           //设定字体文件名
                                    &_variant_t(tmpStr));

            tmpStr = " ";
            newobj.PutPropertyByName(_T("BigFontFile"),        //设定大字体文件名
                                    &_variant_t(tmpStr));

            newobj.PutPropertyByName(_T("Height"),             //设定字体高度
                                    &_variant_t(double(20.0)));
            newobj.PutPropertyByName(_T("Width"),              //设定宽度比例因子***
                                    &_variant_t(double(1.0)));
            newobj.PutPropertyByName(_T("ObliqueAngle"),       //设定字体倾斜角度
                                    &_variant_t(double(0.0)));
            newobj.PutPropertyByName(_T("TextGenerationFlag"), //设定字体生成字段
                   &_variant_t(short(0)));                     //无任何特殊标志
                   //&_variant_t(short(2)));                   //反向标志
                   //&_variant_t(short(4)));                   //颠倒标志
            
            //SETUP-5: 将新创建的字体样式设定为系统的当前字体样式
            EDIBAcad::objAcadDoc.Invoke(_T("ActiveTextStyle"),
                                        1,
                                        &_variant_t(newobj.p));

            //SETUP-6: 更新显示
            lpStyle->AddString(newTextStyleName);

            pCurTable->nStyle = lpStyle->GetCount() - 1;            
            pCurTable->blBackward = FALSE;
            pCurTable->blUp2Down = FALSE;
            pCurTable->blVertical = FALSE;
            pCurTable->dHeight = 0.0;
            pCurTable->dAngle = 0.0;
            pCurTable->dScaleFactor = 1.0;
            pCurTable->nBigFonts = 0;
            pCurTable->nShxFonts = 0;
            pCurTable->nAlign = 0;

            UpdateTableData(FALSE);
        }
        else
        {
            MessageBox("创建字体样式失败，请您在ACAD中创建所需的字体样式后再做后续相关操作");
        }
    }
	catch(CException *e)
	{
		e->Delete();
	}
}

void CPreStylePag::OnBtnDeletestyle() 
{
    //局部变量定义和初始化
    CMObject   obj;
    CString    strTSName, strShxFile, strBigFile, tmpstr;
    CComboBox *lpStyle, *lpShx, *lpBig;
    int        nCount, nIndex;
    short      iGenerationFlag;

    lpStyle = (CComboBox *)GetDlgItem(IDC_COMBO_STYLE);
    lpShx   = (CComboBox *)GetDlgItem(IDC_COMBO_SHXFONTS);
    lpBig   = (CComboBox *)GetDlgItem(IDC_COMBO_BIGFONTS);

    UpdateTableData(TRUE);
    nCount = lpStyle->GetCount();
    if(nCount <= 0)return ;

    if(m_nStyle < 0)
    {
        pCurTable->nStyle = 0;
        UpdateTableData(FALSE);
        return ;
    }
    else if(m_nStyle >= nCount)
    {
        pCurTable->nStyle--;
        UpdateTableData(FALSE);
        return ;
    }
    if(nCount == 1)
    {
        MessageBox("系统必须保留一种文字样式，删除失败！",
            "错误操作", MB_ICONSTOP);
        return ;
    }
    lpStyle->GetLBText(m_nStyle, strTSName);
    if(MessageBox("请确定: 您真的要删除文字样式 [" + strTSName + "] 吗？",
        "确认删除", MB_YESNO | MB_ICONWARNING) == IDNO)
    {
        return ;
    }

    if(DeleteTextStyle(strTSName) == FALSE) return ;
    lpStyle->DeleteString(m_nStyle);

    //此处判断表示：
    //1、若用户删除的是列表最后一项样式，则新样式自动向上滚动
    //2、若用户删除的不是是列表最后的项，则自动向下滚动
    //3、根据前面的选择，用户不能删除列表中唯一的项
    if(nCount == m_nStyle + 1) m_nStyle--;

    lpStyle->GetLBText(m_nStyle, strTSName);
    GetTextStyleProperty(strTSName, strShxFile, strBigFile,
               pCurTable->dHeight, pCurTable->dScaleFactor,
               pCurTable->dAngle, iGenerationFlag);

    pCurTable->nStyle = m_nStyle;
    pCurTable->nShxFonts = lpShx->FindString(0, strShxFile);
    if(pCurTable->nShxFonts == CB_ERR)
    {
        lpShx->AddString(strShxFile);
        pCurTable->nShxFonts = lpShx->GetCount() - 1;
    }
    pCurTable->nBigFonts = lpBig->FindString(0, strBigFile);
    if(pCurTable->nBigFonts == CB_ERR) pCurTable->nBigFonts = 0;

    if(iGenerationFlag & 0x02) pCurTable->blBackward = TRUE;
    else                       pCurTable->blBackward = FALSE;
    if(iGenerationFlag & 0x04) pCurTable->blUp2Down  = TRUE;
    else                       pCurTable->blUp2Down  = FALSE;

    UpdateTableData(FALSE);
}

BOOL CPreStylePag::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	//检查ACAD是否已经启动
	//查询ACAD中已有的可用的文字样式
	//pfg20050330
	CComboBox *lpStyle;
	lpStyle = (CComboBox *)GetDlgItem(IDC_COMBO_STYLE);
	if(lpStyle->GetCount()<1)
	{
	    lpStyle->AddString("DIM");
	}
	//pfg20050330
	/*
	FindAcad(TRUE);

	//搜索可用的字体文件和大字体文件
	LoadFontFilesCombo();

	//查询ACAD中已有的可用的文字样式
	QueryAcadTextStyleCombo();

	//设定表格初始数据
	LoadTableCombo();

	//从modPHScal获得控件值
	SetCtrlValue();

	//从注册表导入表格历史数据
	LoadHistorySetting();

	//设置使用信息
	m_strMsg.LoadString(IDS_PRESTYLEPAGEMSG);

	//更新对话框显示
	UpdateTableData(FALSE);*/

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPreStylePag::LoadFontFilesCombo()
{
    //搜索可用的字体文件和大字体文件
    CString strFontDir, strFile;
    WIN32_FIND_DATA fd;
    HANDLE fh;
    CComboBox *lpBigCombo, *lpShxCombo;

    strFontDir = GetCADFontDir();
    lpBigCombo = (CComboBox *)GetDlgItem(IDC_COMBO_BIGFONTS);
    lpShxCombo = (CComboBox *)GetDlgItem(IDC_COMBO_SHXFONTS);
    memset(&fd, 0, sizeof(fd));
    if(strFontDir.IsEmpty() == TRUE) return;

    strFile = strFontDir + (_T("\\*.shx"));
    
    fh = FindFirstFile(strFile, &fd);
    if(fh != INVALID_HANDLE_VALUE)
    {
        do
        {
            strFile = strFontDir + (_T("\\")) + fd.cFileName;
            strFile.MakeLower();
            if(strFile.GetLength() > 4 && strFile.Right(4) == _T(".shx"))
            {
                try
                {
                    CFile file(strFile, CFile::modeRead);
                    CString strVal;
                    char data[30];
                    int  iCount = file.Read(data, 29);
                    data[iCount]=0;
                    strVal = data;
                    strVal.MakeLower();
                    if(strVal.Find((_T("bigfont"))) != -1)
                    {
                        strFile = strFile.Mid(strFile.ReverseFind((_T('\\'))) + 1);
                        //strFile = strFile.Left(strFile.ReverseFind((_T('.'))));
                        lpBigCombo->AddString(strFile);
                    }
                    else if(strVal.Find((_T("unifont"))) != -1)
                    {
                        strFile = strFile.Mid(strFile.ReverseFind((_T('\\'))) + 1);
                        //strFile = strFile.Left(strFile.ReverseFind((_T('.'))));
                        lpShxCombo->AddString(strFile);
                    }
                    file.Close();
                }
                catch(CFileException * e)
                {
                    if(e->m_cause==11)
                    {
                        //由于 AutoCAD 当前使用该字体，而发生共享违例
                        strFile = strFile.Mid(strFile.ReverseFind((_T('\\'))) + 1);
                        if(::CopyFile(e->m_strFileName,
                            basDirectory::TemplateDir +
                            strFile, FALSE))
                        {
                            try
                            {
                                CFile file(basDirectory::TemplateDir + strFile,
                                    CFile::modeRead);
                                CString strVal;
                                char data[30];
                                int iCount = file.Read(data, 29);
                                data[iCount]=0;
                                strVal = data;
                                strVal.MakeLower();
                                if(strVal.Find(_T("bigfont")) != -1)
                                {
                                    //strFile = strFile.Left(strFile.ReverseFind((_T('.'))));
                                    lpBigCombo->AddString(strFile);
                                }
                                else if(strVal.Find((_T("unifont"))) != -1)
                                {
                                    //strFile = strFile.Left(strFile.ReverseFind((_T('.'))));
                                    lpShxCombo->AddString(strFile);
                                }
                                file.Close();
                            }
                            catch(CFileException * e)
                            {
#ifdef _DEBUG
                                e->ReportError();
#endif
                                e->Delete();
                            }
                        }
                    }
                    else
                    {
#ifdef _DEBUG
                        e->ReportError();
#endif
                    }
                    e->Delete();
                }
            }
        }while(FindNextFile(fh, &fd));
        FindClose(fh);
    }
}

void CPreStylePag::QueryAcadTextStyleCombo()
{
    //查询ACAD中已有的可用的文字样式
    CMObject obj, sobj;
    CString  strStyleName;
    CComboBox *lpStyle;
    long i, nCount;
	_variant_t v;
	try
	{
        lpStyle = (CComboBox *)GetDlgItem(IDC_COMBO_STYLE);
		v = EDIBAcad::objAcadDoc.GetPropertyByName(_T("TextStyles"));
        obj = v.pdispVal;
		nCount =(long)obj.GetPropertyByName(_T("Count"));

		for(i = 0; i < nCount; i++)
		{
			sobj = obj.Invoke(_T("Item"), 1, &_variant_t(i)).pdispVal;
			strStyleName = vtos(sobj.GetPropertyByName(_T("Name")));
			strStyleName.TrimLeft();
            strStyleName.TrimRight();
            if(strStyleName.GetLength() <= 0) continue;

            lpStyle->AddString(strStyleName);
		}
	}
	catch(CException *e)
	{
		e->Delete();
	}
}

void CPreStylePag::LoadTableCombo()
{
    //导入表格初始数据
    memset(&ljmx_style, NULL, sizeof(WHK_STYLE));
    memset(&dim_style,  NULL, sizeof(WHK_STYLE));
    memset(&load_style, NULL, sizeof(WHK_STYLE));

    memset(&Comments_style, NULL, sizeof(WHK_STYLE));
    memset(&BH_style,       NULL, sizeof(WHK_STYLE));
    memset(&BG_style,       NULL, sizeof(WHK_STYLE));
    memset(&other_style,    NULL, sizeof(WHK_STYLE));

    ljmx_style.nTable = 0;
    dim_style.nTable  = 1;
    load_style.nTable = 2;

    Comments_style.nTable = 3;
    BH_style.nTable       = 4;
    BG_style.nTable       = 5;
    other_style.nTable    = 6;

    pCurTable = &ljmx_style;
}

CString CPreStylePag::GetCADFontDir()
{
	HKEY hKey;
	CString strSubKey=(_T("Software\\Autodesk\\AutoCAD\\R14.0"));
	if(ERROR_SUCCESS!=RegOpenKey(HKEY_LOCAL_MACHINE,strSubKey,&hKey))
	{
		strSubKey=(_T("Software\\Autodesk\\AutoCAD\\R14.01"));
		if(ERROR_SUCCESS!=RegOpenKey(HKEY_LOCAL_MACHINE,strSubKey,&hKey))
		{
			strSubKey=(_T("Software\\Autodesk\\AutoCAD\\R15.0"));
			if(ERROR_SUCCESS!=RegOpenKey(HKEY_LOCAL_MACHINE,strSubKey,&hKey))
			{
				return (_T(""));
			}
		}
	}
	_TCHAR szValue[256];
	long len=256*sizeof(_TCHAR);
	ULONG type=REG_SZ;
	if(ERROR_SUCCESS!=RegQueryValueEx(hKey,(_T("CurVer")),NULL,&type,(unsigned char*)szValue,(unsigned long*)&len))
	{
		RegCloseKey(hKey);
		return (_T(""));
	}
	HKEY hKey2;
	if(ERROR_SUCCESS!=RegOpenKey(hKey,szValue,&hKey2))
	{
		RegCloseKey(hKey);
		return (_T(""));
	}
	RegCloseKey(hKey);
	len=256*sizeof(_TCHAR);
	if(ERROR_SUCCESS!=RegQueryValueEx(hKey2,(_T("AcadLocation")),NULL,&type,(unsigned char*)szValue,(unsigned long*)&len))
	{
		RegCloseKey(hKey2);
		return (_T(""));
	}
	RegCloseKey(hKey2);
	CString strRet=szValue;
	user::AddDirSep(strRet);
	strRet+=(_T("FONTS"));
	return strRet;
}

BOOL CPreStylePag::FindAcad(BOOL blCrackFind)
{
    if(blCrackFind == TRUE)
    {
        m_blAcadIsOpened = EDIBAcad::StartAcad();
        if(m_blAcadIsOpened == TRUE)
        {
            EDIBAcad::SetAcadNoTop();
            ::SetForegroundWindow(theApp.m_pMainWnd->m_hWnd);
        }
    }
    else
    {
        if(m_blAcadIsOpened == FALSE)
        {
            if(EDIBAcad::FindAcad() == NULL)
            {
                m_blAcadIsOpened = EDIBAcad::StartAcad(); 
            }
            else m_blAcadIsOpened = TRUE;
            EDIBAcad::SetAcadNoTop();  
            ::SetForegroundWindow(theApp.m_pMainWnd->m_hWnd);
        }
    }
    return m_blAcadIsOpened;
}

void CPreStylePag::OnBtnDefault() 
{
    CComboBox *lpStyle, *lpShx, *lpBig;
    CString    strTSName, strShxFile, strBigFile;
    short      iGenerationFlag;

    lpStyle = (CComboBox *)GetDlgItem(IDC_COMBO_STYLE);
    lpBig   = (CComboBox *)GetDlgItem(IDC_COMBO_BIGFONTS);
    lpShx   = (CComboBox *)GetDlgItem(IDC_COMBO_SHXFONTS);
    //当前表格采用缺省的参数设置
    switch(pCurTable->nTable)
    {
    case 0://ljmx
        pCurTable->nStyle = lpStyle->FindString(0, _T("BOM"));
        strTSName = "BOM";
        break;
    case 1://dim
        pCurTable->nStyle = lpStyle->FindString(0, _T("DIM"));
        strTSName = "DIM";
        break;
    case 2://load
        pCurTable->nStyle = lpStyle->FindString(0, _T("LOAD"));
        strTSName = "LOAD";
        break;
    case 3://Comments
        pCurTable->nStyle = lpStyle->FindString(0, _T("BOM"));
        strTSName = "BOM";
        break;
    case 4://BH
        pCurTable->nStyle = lpStyle->FindString(0, _T("BOM"));
        strTSName = "BOM";
        break;
    case 5://BG
        pCurTable->nStyle = lpStyle->FindString(0, _T("BOM"));
        strTSName = "BOM";
        break;
    case 6://Other
        pCurTable->nStyle = lpStyle->FindString(0, _T("BOM"));
        strTSName = "BOM";
        break;
    default: return ;
    }
    //样式名

    if(pCurTable->nStyle == -1)
    {
        CreateTextStyle(strTSName, _T("txt.shx"), _T("hzfs.shx"),
            0.0, 0.7, 0.0, 0);
        lpStyle->AddString(strTSName);
        pCurTable->nStyle = lpStyle->GetCount() - 1;
        pCurTable->blBackward = FALSE;
        pCurTable->blUp2Down = FALSE;
        pCurTable->dAngle = 0.0;
        pCurTable->dHeight = 0.0;
        pCurTable->dScaleFactor = 0.7;
        pCurTable->nAlign = 0;
        pCurTable->nBigFonts = lpBig->FindString(0, _T("hzfs.shx"));
        if(pCurTable->nBigFonts == -1)
        {
            lpBig->AddString(_T("hzfs.shx"));
            pCurTable->nBigFonts = lpBig->GetCount() - 1;
        }
        pCurTable->nShxFonts = lpShx->FindString(0, _T("txt.shx"));
        if(pCurTable->nShxFonts == -1)
        {
            lpShx->AddString(_T("txt.shx"));
            pCurTable->nShxFonts = lpShx->GetCount() - 1;
        }

        UpdateTableData(FALSE);
        return ;
    }

    //将缺省样式的属性值重新设置为系统定义的值
    strShxFile = _T("txt.shx");
    strBigFile = _T("hzfs.shx");
    pCurTable->dHeight = 0.0;
    pCurTable->dScaleFactor = 0.7;
    pCurTable->dAngle = 0.0;
    iGenerationFlag = 0;
    ModifyTextStyleProperty(strTSName, strShxFile, strBigFile,
               pCurTable->dHeight, pCurTable->dScaleFactor,
               pCurTable->dAngle, iGenerationFlag);
    
    pCurTable->nShxFonts = lpShx->FindString(0, strShxFile);
    if(pCurTable->nShxFonts == CB_ERR)
    {
        lpShx->AddString(strShxFile);
        pCurTable->nShxFonts = lpShx->GetCount() - 1;
    }
    pCurTable->nBigFonts = lpBig->FindString(0, strBigFile);
    if(pCurTable->nBigFonts == CB_ERR)
    {
        lpShx->AddString(strBigFile);
        pCurTable->nShxFonts = lpShx->GetCount() - 1;
    }

    pCurTable->blBackward = FALSE;
    pCurTable->blUp2Down  = FALSE;
    pCurTable->nAlign = 0;

    UpdateTableData(FALSE);
}

void CPreStylePag::OnBtnSave() 
{
    UpdateTableData(TRUE);
    SaveTable2Registry();
}

void CPreStylePag::SaveTable2Registry() 
{
    CComboBox *lpTable, *lpStyle, *lpShx, *lpBig;
    CString   strTable, strValue;

    lpTable = (CComboBox *)GetDlgItem(IDC_COMBO_TABLE);
    lpStyle = (CComboBox *)GetDlgItem(IDC_COMBO_STYLE);
    lpShx   = (CComboBox *)GetDlgItem(IDC_COMBO_SHXFONTS);
    lpBig   = (CComboBox *)GetDlgItem(IDC_COMBO_BIGFONTS);

    //保存当前表格的参数设置
    lpTable->GetLBText(pCurTable->nTable, strTable);

    //保存当前样式名
	if(lpStyle->GetCount()>0)//pfg20050330
	{
    lpStyle->GetLBText(pCurTable->nStyle, strValue);
	}
	else//pfg20050330
	{
		strValue="";//pfg20050330
	}
    SetRegValue(_T("Settings"), strTable + _T("-STYLENAME"), strValue);

    //保存当前样式属性：当重新导入的时候，若系统中没有相应
    //    的样式时，则自动创建，若系统已经有相应的样式，则
    //    不更改相应样式的属性值，这时表格参数被重置。

    //保存当前字体文件名
    lpShx->GetLBText(pCurTable->nShxFonts, strValue);
    SetRegValue(_T("Settings"), strTable + _T("-SHXFILENAME"), strValue);
    //保存当前大字体文件名
    lpBig->GetLBText(pCurTable->nBigFonts, strValue);
    SetRegValue(_T("Settings"), strTable + _T("-BIGFILENAME"), strValue);
    //高度
	//pfg20050330
	strTable.TrimLeft();
	strTable.TrimRight();
	if(strTable.Compare("标注尺寸"))
	{
		SetRegValue(_T("Settings"), strTable + _T("-HEIGHT"), double(pCurTable->dHeight));//pfg20050330原代码
	}
	//pfg20050330
    //宽度
    SetRegValue(_T("Settings"), strTable + _T("-WIDTH"), double(pCurTable->dScaleFactor));
    //倾斜角度
    SetRegValue(_T("Settings"), strTable + _T("-OBLIQUEANGLE"), double(pCurTable->dAngle));
    //颠倒
    SetRegValue(_T("Settings"), strTable + _T("-BLUP2DOWN"), pCurTable->blUp2Down);
    //反向
    SetRegValue(_T("Settings"), strTable + _T("-BLBACKWARD"), pCurTable->blBackward);
    //垂直
    SetRegValue(_T("Settings"), strTable + _T("-BLVERTICAL"), pCurTable->blVertical);
    //文字对齐方式
    SetRegValue(_T("Settings"), strTable + _T("-ALIGN"), int(pCurTable->nAlign));
}

void CPreStylePag::LoadHistorySetting()
{
    //导入表格历史数据，一般只在初始化时或出现异
    //常错误无法恢复时调用本函数
    LoadTableHistory(_T("其它文字"));
    LoadTableHistory(_T("标高"));
    LoadTableHistory(_T("编号"));
    LoadTableHistory(_T("注释"));

    LoadTableHistory(_T("荷载位移表"));
    LoadTableHistory(_T("标注尺寸"));
    LoadTableHistory(_T("零件明细表"));
}

BOOL CPreStylePag::LoadTableHistory(CString strTable)
{
    //导入表格历史数据
    
    CComboBox *lpTable, *lpStyle, *lpShx, *lpBig;
    CString   strValue, strShx, strBig;
    double    dHeight, dScaleFactor, dAngle;
    BOOL      blUp2Down, blBackward;
    short     iGenFlag;
    int       retCode;

    lpTable = (CComboBox *)GetDlgItem(IDC_COMBO_TABLE);
    lpStyle = (CComboBox *)GetDlgItem(IDC_COMBO_STYLE);
    lpShx   = (CComboBox *)GetDlgItem(IDC_COMBO_SHXFONTS);
    lpBig   = (CComboBox *)GetDlgItem(IDC_COMBO_BIGFONTS);

    if(strTable == _T("零件明细表"))
    {
        pCurTable = &ljmx_style;
    }
    else if(strTable == _T("标注尺寸"))
    {
        pCurTable = &dim_style;
    }
    else if(strTable == _T("荷载位移表"))
    {
        pCurTable = &load_style;
    }
    else if(strTable == _T("注释"))
    {
        pCurTable = &Comments_style;
    }
    else if(strTable == _T("编号"))
    {
        pCurTable = &BH_style;
    }
    else if(strTable == _T("标高"))
    {
        pCurTable = &BG_style;
    }
    else if(strTable == _T("其它文字"))
    {
        pCurTable = &other_style;
    }
    else
    {
        return FALSE;
    }

    //样式名
    strValue = GetRegKey(_T("Settings"), strTable + _T("-STYLENAME"), strValue);
    retCode = lpStyle->FindString(0, strValue);
    
    //SHX 字体
    strShx = GetRegKey(_T("Settings"), strTable + _T("-SHXFILENAME"), strShx);
    //BIG 字体
    strBig = GetRegKey(_T("Settings"), strTable + _T("-BIGFILENAME"), strBig);
    //高度
    dHeight = GetRegKey(_T("Settings"), strTable + _T("-HEIGHT"), float(0.0));
    //宽度比例
    dScaleFactor = GetRegKey(_T("Settings"), strTable + _T("-WIDTH"), float(0.0));
    //倾斜角度
    dAngle = GetRegKey(_T("Settings"), strTable + _T("-OBLIQUEANGLE"), float(0.0));
    //颠倒
    blUp2Down = GetRegKey(_T("Settings"), strTable + _T("-BLUP2DOWN"), FALSE);
    //反向
    blBackward = GetRegKey(_T("Settings"), strTable + _T("-BLBACKWARD"), FALSE);
    
    iGenFlag = 0;
    if(blBackward == TRUE) iGenFlag += 2;
    if(blUp2Down == TRUE)  iGenFlag += 4;
    
    if(retCode == -1 && strValue != "")//样式列表中找不到样式名
    {
        //自动创建样式
        CreateTextStyle(strValue, strShx, strBig, 
            dHeight, dScaleFactor, dAngle, iGenFlag);

        lpStyle->AddString(strValue);

        pCurTable->nStyle = lpStyle->GetCount() - 1;
        pCurTable->nShxFonts = lpShx->FindString(0, strShx);
        if(pCurTable->nShxFonts == -1)
        {
            lpShx->AddString(strShx);
            pCurTable->nShxFonts = lpShx->GetCount() - 1;
        }
        pCurTable->nBigFonts = lpBig->FindString(0, strBig);
        if(pCurTable->nBigFonts == -1)
        {
            pCurTable->nBigFonts = 0;
        }
        pCurTable->dHeight = dHeight;
        pCurTable->dScaleFactor = dScaleFactor;
        pCurTable->dAngle = dAngle;
        pCurTable->blUp2Down = blUp2Down;
        pCurTable->blBackward = blBackward;
    }
    else if(retCode != -1)//找到样式名
    {
        pCurTable->nStyle = retCode;
        //重置给零件明细表的相应属性
        ModifyTextStyleProperty(strValue, strShx, strBig, 
            dHeight, dScaleFactor, dAngle, iGenFlag);

        pCurTable->nShxFonts = lpShx->FindString(0, strShx);
        if(pCurTable->nShxFonts == -1)
        {
            lpShx->AddString(strShx);
            pCurTable->nShxFonts = lpShx->GetCount() - 1;
        }
        pCurTable->nBigFonts = lpBig->FindString(0, strBig);
        if(pCurTable->nBigFonts == -1)
        {
            pCurTable->nBigFonts = 0;
        }
        pCurTable->dHeight = dHeight;
        pCurTable->dScaleFactor = dScaleFactor;
        pCurTable->dAngle = dAngle;
        pCurTable->blUp2Down = blUp2Down;
        pCurTable->blBackward = blBackward;
    }

    //导入对齐属性
    pCurTable->nAlign = GetRegKey(_T("Settings"), strTable + _T("-ALIGN"), 0);

    //更新显示
    UpdateTableData(FALSE);
    return TRUE;
}

void CPreStylePag::DoDefaultSetting(void)
{
    pCurTable = &other_style;
    OnBtnDefault();
    pCurTable = &BG_style;
    OnBtnDefault();
    pCurTable = &BH_style;
    OnBtnDefault();
    pCurTable = &Comments_style;
    OnBtnDefault();
    pCurTable = &load_style;
    OnBtnDefault();
    pCurTable = &dim_style;
    OnBtnDefault();
    pCurTable = &ljmx_style;
    OnBtnDefault();
}

double SetPrecision(double value, int prec)
{
    double nCmp = pow(10.0, prec);
    double dRet = 0.0;
    double dtmp1 = 0.0;
    double dtmp2 = 0.0;
    double dtmp3 = 0.0;

    if(value < nCmp * 0.5) return 0.0;
    if(value >= nCmp * 0.5 && value < nCmp * 1.5) return nCmp;

    if(prec == 0.0)
    {
        modf(value, &dRet);
        return dRet;
    }
    else if(prec < 0.0) // 精确到小数位
    {
        dtmp1 = modf(value, &dRet);
        dtmp1 *= pow(10.0, 0 - prec);
        dtmp2 = modf(dtmp1, &dtmp3);
        if(dtmp2 >= 0.5)
        {
            dtmp3 += 1.0;
        }
        dtmp3 /= pow(10.0, 0 - prec);
        return dRet + dtmp3;
    }
    else //精确到整数位
    {
        modf(value, &dRet);
        dRet /= pow(10.0, prec);
        dtmp2 = modf(dRet, &dtmp1);
        if(dtmp2 >= 0.5)
        {
            dtmp1 += 1.0;
        }
        dtmp1 *= pow(10.0, prec);
        return dtmp1;
    }
}

void CPreStylePag::OnLButtonDown(UINT nFlags,CPoint point)
{//chengbailian 2003.12.12 当窗口很大时,在分辨率改变小时,窗口不能全部显示出来,需要移动窗体
	HWND h=::GetActiveWindow();//得到主窗口名柄
	::SendMessage(h,WM_NCLBUTTONDOWN, HTCAPTION, NULL);//发送移动消息
}







BOOL CPreStylePag::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	if(ActiveTimes != 0)
	{
		FindAcad(TRUE);

		//搜索可用的字体文件和大字体文件
		LoadFontFilesCombo();

		//查询ACAD中已有的可用的文字样式
		QueryAcadTextStyleCombo();

		//设定表格初始数据
		LoadTableCombo();

		//从modPHScal获得控件值
		SetCtrlValue();

		//从注册表导入表格历史数据
		LoadHistorySetting();

		//设置使用信息
		m_strMsg.LoadString(IDS_PRESTYLEPAGEMSG);

		//更新对话框显示
		UpdateTableData(FALSE);
	}
	ActiveTimes = ActiveTimes + 1;
	return CPropertyPage::OnSetActive();
}

SHORT CPreStylePag::GetActiveTimes()
{
	return ActiveTimes;
}
