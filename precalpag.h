#if !defined(AFX_PRECALPAG_H__0888E9C5_9DF0_11D5_AE95_00D00961973B__INCLUDED_)
#define AFX_PRECALPAG_H__0888E9C5_9DF0_11D5_AE95_00D00961973B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PreCalPag.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPreCalPag dialog
#define WM_SENDMESSAGE WM_USER+1
class CPreCalPag : public CPropertyPage
{
	DECLARE_DYNCREATE(CPreCalPag)

// Construction
public:
	void DoDefaultSetting();
	void SaveIniValue();
	void SetCtrlValue();
	void SetComboSpringOrder(bool bSaveAction);
	CPreCalPag();
	virtual ~CPreCalPag();

// Dialog Data
	//{{AFX_DATA(CPreCalPag)
	enum { IDD = IDD_PRE_PAG1 };
	CComboBox	m_ComboRodInsertLenRoundVal;
	CListBox	m_ComboSpringOrder;
	CComboBox	m_ComboRodLenRoundVal;
	CComboBox	m_ComboConnectTJ;
	CComboBox	m_ComboPjg2Pgz;
	CComboBox	m_ComboDW_Delta;
	CComboBox	m_ComboUseSPRminDist;
	CComboBox	m_ComboSPR_CHzbh;
	CString	m_sComboSpringOrder;
	BOOL	m_bCheckSPRAutoLugDia;
	BOOL	m_bCheckConnectHoleDimMatch;
	BOOL	m_bCheckLimitLugMinDia;
	BOOL	m_bComboAutoApplyChzbh;
	BOOL	m_bCheckAutoPASA;
	BOOL	m_bCheckCalSAbyConservativenessMethod;
	BOOL	m_bCheckCalPARTbyMaxLoad;
	BOOL	m_bCheckCalSAbyPJG;
	BOOL	m_bCheckAddPartWeight2Pmax;
	BOOL	m_bCheckWorksWithSABoltsNuts;
	BOOL	m_bCheckWorksWithBoltsNuts;
	BOOL	m_bCheckCSPRCanSpecialDesign;
	BOOL	m_bCheckAutoAlignCSPRtoSA;
	BOOL	m_bCheckCalCSPRLugDiaByPjg;
	BOOL	m_bCheckAutoApplyCSPRLugDia;
	BOOL	m_bCheckOutputSA;
	BOOL	m_bCheckSumAllVolume;
	CString	m_sComboSPR_CHzbh;
	CString	m_sComboUseSPRminDist;
	CString	m_sComboDW_Delta;
	CString	m_sComboPjg2Pgz;
	CString	m_sComboConnectTJ;
	BOOL	m_bRepZdjType;
	CString	m_sComboRodLenRoundVal;
	int		m_iNumCS;
	BOOL	m_bAutoSPRRodDiaDecrease;
	BOOL	m_bRodDiaByMax;
	CString	m_strMinRodLen;
	BOOL	m_bHotStatus2Zero;
	BOOL	m_bSumRodByRoundSteel;
	CString	m_sComboRodInsertLenRoundVal;
	BOOL	m_bCheckAddPartWeight2CSPRPgz;
	BOOL	m_bCheckAddPartWeight2SPRPgz;
	BOOL	m_bCheckMinMaxDispByActualDisp;
	BOOL	m_bCheckDHT;
	CString	m_strDHTGG;
	BOOL	m_bSPRBHLOADBYInstallLoad;
	BOOL	m_bSPRBHLOADUNITBYNEWTON;
	BOOL	m_bPipeClampBolt;
	float	m_FrictionFree;
	BOOL	m_bCalcCSPRHeight_BySelDisp;
	BOOL	m_bMaxPgzPaz;
	BOOL	m_IsPAForceZero;
	double  m_dZJOverValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPreCalPag)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPreCalPag)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioScs();
	afx_msg void OnRadioDcs();
	afx_msg void OnRadioAcs();
	afx_msg void OnCheckHzDHT();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKillfocusFrictionfree();
	afx_msg void OnCalccsprheightByseldisp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void InitFromReg();

	void UpdateCBString(CDataExchange * pDX,UINT nID,CString strData);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRECALPAG_H__0888E9C5_9DF0_11D5_AE95_00D00961973B__INCLUDED_)
