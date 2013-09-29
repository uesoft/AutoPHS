#if !defined(AFX_PREDRAWPAG_H__0888E9C6_9DF0_11D5_AE95_00D00961973B__INCLUDED_)
#define AFX_PREDRAWPAG_H__0888E9C6_9DF0_11D5_AE95_00D00961973B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PreDrawPag.h : header file
//
#include "XTabCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CPreDrawPag dialog
class CCadFontPag;

class CPreDrawPag : public CPropertyPage
{
	DECLARE_DYNCREATE(CPreDrawPag)

// Construction
public:
	void DoDefaultSetting();
	void SetTabFont();
	void SetBasePoint();
	void SaveIniValue();
	void SetCtrlValue();
	CPreDrawPag();
	virtual ~CPreDrawPag();
	CCadFontPag* m_fontLjmx;
	CCadFontPag* m_fontDim;
	CCadFontPag* m_fontLoad;
// Dialog Data
	//{{AFX_DATA(CPreDrawPag)
	enum { IDD = IDD_PRE_PAG2 };
	CXTabCtrl	m_tabFont;
	CStatic	m_LabelStartPN;
	CComboBox	m_ComboSEQyOffset;
	BOOL    m_OrientationPicture;//pfg20050426
	BOOL	m_bCheckDrawNoPre;
	BOOL	m_bCheckOSFlag;
	BOOL	m_bCheckDraw_BOM;
	BOOL	m_bCheckDrawOverWrite;
	BOOL	m_bCheckAnnotateCSPR;
	BOOL	m_bCheckNotPROMPT_ACAD_SHIFT;
	BOOL	m_bCheckNotPROMPT_Catlog_Overwrite;
	BOOL	m_bCheckAutoSaveDrawing;
	CString	m_sComboSEQyOffset;
	CString	m_sTextDrawNoStartNumber;
	CString	m_strDwgVersion;
	CString	m_stextDWGPRE;
	CString	m_sLabelStartPN;
	BOOL	m_bNoFenQu;
	BOOL m_bTBhq;
	BOOL	m_bBomSprFactory;
	BOOL	m_bSumBomSprFactory;
	BOOL	m_bDimLocation;
	BOOL	m_bCheckDraw_SumWeight;
	int		m_GroupWare;
	CString m_strGroupWareName;
	BOOL	m_bCheckTagUsingCircle;
	BOOL	m_bCheckCustomPicNo;
	BOOL	m_bCheckLJMX_BJBW;
	BOOL	m_bCheckBGForBJBW;
	BOOL	m_bCheckDWForBJBW;
	float	m_fCircleDiameter;
	BOOL	m_bCheckDLOADBJBW;
	BOOL	m_bCheckDrawNameWithoutNo;
	float	m_fSeqTxtHeight;
	BOOL	m_bNoWaiKuang;
	BOOL	m_bNotDrawWeight;
	BOOL	m_bInsertLength;
	BOOL	m_bColdHeatState;
	BOOL	m_bInstallElevation;
	BOOL	m_bHzabs;
	BOOL	m_bPrintSA_Value;
	CEdit	m_GroupWareEdit;
	float	m_fOvertopDim;
	float	m_fDistanceOffset;
	BOOL	m_bDimNumber;
	float	m_fSetTableWidth;
	float	m_fSetTKWidth;
	int		m_IsACAD;
	BOOL	m_bDrawWeldSymbol;
	BOOL	m_bAutoWeldViewInsertPoint;
	BOOL	m_bDrawWeldViewDim;
	BOOL	m_bWeldAutoDirectAndPos;
	double	m_MaxLGLong;
	BOOL	m_bSymbol;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPreDrawPag)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int m_iOldTabFontSel;
	// Generated message map functions
	//{{AFX_MSG(CPreDrawPag)
	afx_msg void OnDrawsizeA3();
	afx_msg void OnDrawsizeA4();
	afx_msg void OnDrawsizeAuto();
	afx_msg void OnPhscolor0();
	afx_msg void OnPhscolor1();
	afx_msg void OnPhscolor2();
	afx_msg void OnOutinfo0();
	afx_msg void OnOutinfo1();
	afx_msg void OnOutinfo2();
	afx_msg void OnDwgpre0();
	afx_msg void OnDwgpre1();
	virtual BOOL OnInitDialog();
	afx_msg void OnDrawpreno();
	afx_msg void OnDestroy();
	afx_msg void OnDrawTagCircle();
	afx_msg void OnChangeCircleDiameter();
	afx_msg void OnSelchangeMinOffset();
	afx_msg void OnKillfocusMinOffset();
	afx_msg void OnLjmxBjbw();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnGroupWareRadio3();
	afx_msg void OnGroupWareRadio2();
	afx_msg void OnGroupWarex();
	afx_msg void OnRcad();
	afx_msg void OnRmicrostation();
	afx_msg void OnOutinfo3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void VerifyCirDiaOrMinOffset(BOOL bVerifyCirDia=TRUE);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREDRAWPAG_H__0888E9C6_9DF0_11D5_AE95_00D00961973B__INCLUDED_)
