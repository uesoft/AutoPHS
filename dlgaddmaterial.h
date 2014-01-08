#if !defined(AFX_DLGADDMATERIAL_H__BBA4FDA6_F22D_11D5_B3DE_0040B800239A__INCLUDED_)
#define AFX_DLGADDMATERIAL_H__BBA4FDA6_F22D_11D5_B3DE_0040B800239A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAddMaterial.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgAddMaterial dialog
struct stMaterial
{
	CString m_strName;
	CString m_strNum;
	CString m_strLength;
};
class CDlgAddMaterial : public CDialog
{
// Construction
public:
	stMaterial m_stMaterial;
	int GetMaxSEQ(CString tbn,_ConnectionPtr & db);
	void LoadListMaterial();
	CString* m_pstrDescID;
	void GetLWT(CString strBH,int & L,int& W,int &T);
	void UpdateTmpCSLen();
	CString m_strDescTbn;
	virtual  ~CDlgAddMaterial();
	CDlgAddMaterial(BOOL bIsSA=FALSE,_RecordsetPtr pRs=NULL,int nNode=0,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgAddMaterial)
	enum { IDD = IDD_DIALOG_ADDMATERIAL };
	CComboBox	m_comboMaterial;
	CEdit	m_editThickness;
	CEdit	m_editWidth;
	CComboBox	m_comboDesc;
	CComboBox	m_comboClass;
	float	m_fLength;
	long	m_iNum;
	float	m_fWeight;
	float	m_fPerMeterWeight;
	float	m_fTotalWeight;
	float	m_fWidth;
	float	m_fThickness;
	CComboBox		m_comboBH;
	CString	m_strMaterial;
	CString	m_strInpCustomID;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAddMaterial)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	int m_nNode;
	_RecordsetPtr m_pRs;
	BOOL m_bIsSA;
	CString m_strCustomID;
	CString m_strDesc;
	CString m_strBH;
	void LoadMaterialValue();
	void LoadListBH();
	void LoadListDesc();
	void LoadListClass();

	// Generated message map functions
	//{{AFX_MSG(CDlgAddMaterial)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeClass();
	afx_msg void OnSelchangeDesc();
	afx_msg void OnSelchangeBH();
	afx_msg void OnChangeWeigth();
	afx_msg void OnChangeNum();
	afx_msg void OnChangeLength();
	afx_msg void OnChangeWidth();
	afx_msg void OnChangeThickness();
	afx_msg void OnBtnAdd();
	afx_msg void OnEditchangeDesc();
	afx_msg void OnEditchangeBH();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int m_iClassIndex;
	CString * m_pstrBHMaterial;
	float * m_pfBHWeight;
	CString m_strBHTbn;
	CString* m_pstrDescCustomID;
	CString* m_pstrClassTbn;
	long* m_piClassIndex;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGADDMATERIAL_H__BBA4FDA6_F22D_11D5_B3DE_0040B800239A__INCLUDED_)
