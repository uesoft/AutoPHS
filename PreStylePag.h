#if !defined(AFX_PRESTYLEPAG_H__D45C1BD9_9C2D_4021_9EB5_DB00DB0CC545__INCLUDED_)
#define AFX_PRESTYLEPAG_H__D45C1BD9_9C2D_4021_9EB5_DB00DB0CC545__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
///////////////////////////////////////////////
// 字体设置对话框类声明文件
// PreStylePag.h
// 设计&编程：罗超
// 日     期：2003/10/27

#include "resource.h"
#include "EDIBAcad.h"
#include "modregistry.h"
/////////////////////////////////////////////////////////////////////////////
// CPreStylePag dialog
//设置浮点数精度
double SetPrecision(double value, int prec);
class CPreStylePag : public CPropertyPage
{
	DECLARE_DYNCREATE(CPreStylePag)

// Construction
public:
	SHORT GetActiveTimes();
	CPreStylePag();
	~CPreStylePag();
    typedef struct tagStyle
    {
        BOOL	blBackward;
        BOOL	blUp2Down;
        BOOL	blVertical;
        double	dHeight;
        double	dAngle;
        double	dScaleFactor;
        int		nBigFonts;
        int		nShxFonts;
        int		nStyle;
        int		nTable;
        int     nAlign;
    }WHK_STYLE;

    //数据保存结构
    //零件明细、尺寸、载荷、注释、编号、标高、其它文字
    WHK_STYLE ljmx_style, dim_style, load_style;
    WHK_STYLE Comments_style, BH_style, BG_style, other_style;
    WHK_STYLE *pCurTable;
    BOOL      m_blAcadIsOpened;

    void SaveIniValue(void);
    void SetCtrlValue(void);
    void DoDefaultSetting(void);
    
    void LoadFontFilesCombo(void);      //搜索可用的字体文件和大字体文件
    void QueryAcadTextStyleCombo(void); //查询ACAD中已有的可用的文字样式
    void LoadTableCombo(void);          //导入表格初始数据
    void LoadHistorySetting(void);
    BOOL LoadTableHistory(CString strTable);
    void SaveTable2Registry();
    CString GetCADFontDir(void);        //获得ACAD的字体文件目录
    BOOL DeleteTextStyle(CString strTSName);
    BOOL FindAcad(BOOL blCrackFind = FALSE);
    BOOL CreateTextStyle(
                       CString strTSName,
                       CString strShxFile,
                       CString strBigFile,
                       double  dHeight,
                       double  dWidthFactor,
                       double  dAngleByDegree,
                       short   iGenerationFlag);
    BOOL ModifyTextStyleProperty(
                       CString strTSName,
                       CString strShxFile,
                       CString strBigFile,
                       double  dHeight,
                       double  dWidthFactor,
                       double  dAngleByDegree,
                       short   iGenerationFlag);
    BOOL GetTextStyleProperty(
                       CString  strTSName,
                       CString &strShxFile,
                       CString &strBigFile,
                       double  &dHeight,
                       double  &dWidthFactor,
                       double  &dAngleByDegree,
                       short   &iGenerationFlag);
    BOOL UpdateTableData(BOOL bSaveAndValidate);

// Dialog Data
	//{{AFX_DATA(CPreStylePag)
	enum { IDD = IDD_PRE_PAG4 };
	BOOL	m_blBackward;
	BOOL	m_blUp2Down;
	BOOL	m_blVertical;
	double	m_dHeight;
	double	m_dAngle;
	double	m_dScaleFactor;
	int		m_nBigFonts;
	int		m_nShxFonts;
	int		m_nStyle;
	int		m_nTable;
	int		m_nAlign;
	CString	m_strMsg;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPreStylePag)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPreStylePag)
	afx_msg void OnSelchangeComboTable();
	afx_msg void OnSelchangeComboStyle();
	afx_msg void OnBtnNewstyle();
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnDeletestyle();
	afx_msg void OnBtnDefault();
	afx_msg void OnBtnSave();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	SHORT ActiveTimes;
};

/////////////////////////////////////////// */
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRESTYLEPAG_H__D45C1BD9_9C2D_4021_9EB5_DB00DB0CC545__INCLUDED_)
