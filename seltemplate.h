#if !defined(AFX_SELTEMPLATE_H__0252FF33_19FF_4951_A78C_181C1808BB4D__INCLUDED_)
#define AFX_SELTEMPLATE_H__0252FF33_19FF_4951_A78C_181C1808BB4D__INCLUDED_

#include "FrmBmpSe.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelTemplate.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelTemplate dialog
#include "BmpSeWnd.h"
#include "MyListCtrl.h"
#include "VirtualListCtrl.h"

class CSelTemplate : public CDialog
{
	// Construction
public:
	int m_isfrist;
	void initOpenTemplateRs();
	void initUpdateRecordset();
	void DeleteItem(POSITION pos);
	BOOL IsDefaultFavoriteTemplate(long lSampleID);
	BOOL IsFavoriteTemplate(long lSampleID);
	void SetFavoriteTemplate();
	BOOL m_bSetFavorite;
	enum ChkConditionIndex{
		iChkBoxPJG=0,
			iChkBoxTJ=1,
			iChkBoxGDW1=2,
			iChkBoxCNT=3,
			iChkBoxDW=4,
			iChkBoxOnlyPAh=5 //只选择吊架管部
	};
	CSelTemplate(CWnd* pParent = NULL);   // standard constructor

	// Dialog Data
	//{{AFX_DATA(CSelTemplate)
	enum { IDD = IDD_SEL_TEMPLATE };
	CVirtualListCtrl	m_listctrlStruct;
	CButton	m_btnDefaultFavoriteTemplate;
	CButton	m_btnFavoriteTemplate;
	CListBox	m_lstPart;
	CListBox	m_lstStruct;
	int		m_iSortType;
	BOOL	m_bDesc;
	BOOL	m_bRePA;
	BOOL	m_bShowImg;
	BOOL	m_bTmp;
	CComboBox	m_comboSA;
	CComboBox	m_comboPA;
	CString	m_nRodNum;
	CString	m_nSprNum;
	BOOL	m_bFilter;
	BOOL	m_bSh;
	BOOL	m_bSABEAM;
	BOOL	m_bSACANTil;
	BOOL	m_bSALBRACE;
	BOOL	m_bDH;
	CString	m_nCSPRNum;
	BOOL	m_bCurrentSA;
	BOOL	m_bCurrentPA;
	BOOL	m_bSAGCEMENT;
	BOOL	m_bCurrentZdjhPA;
	BOOL	m_bCurrentZdjhSA;
	BOOL	m_bCurrentZdjhAvPA;
	BOOL	m_bCurrentZdjhAvSA;
	BOOL	m_bAutoSprNum;
	int		m_iDefaultTemplateStatus;
	BOOL	m_bRodNum;
	//}}AFX_DATA
	BOOL m_CheckCondition[6];
	BOOL m_bLoadPA;
	BOOL m_bLoadSA;
	BOOL m_bIsSelPA;
	BOOL m_bIsSelSA;
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelTemplate)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL

		// Implementation
	protected:
		long m_iSaveID;
		UINT m_CheckID[6];
		BOOL m_bSaveChange;
		int m_iOldIxZDG;
		BOOL m_bRefImg;
		int m_iOldIxEDIT;
		CBmpSeWnd * m_frmBmpSe;
		void Bmpshow();
		int m_itmpPreviousIndex;
		DWORD m_dwChkCondition;
		DWORD CalChkCondition();
		void ListZDGInit();
		int m_iCurSampleID;
		int m_iLastSampleID;
		void DataReposition();
		void LoadListSA();
		void LoadListPA();
		void LoadListName();
		CString m_strSortFieldName;
		_RecordsetPtr m_rsTemplateName;
		void OpenTemplateRs();
		void UpdateRecordset();
public://sorry
		CString m_strFilter;
		void GetFilterStr();
		

		// Generated message map functions
		//{{AFX_MSG(CSelTemplate)
		afx_msg void OnRadio1();
		afx_msg void OnCheckDesc();
		afx_msg void OnCheckFilter();
		afx_msg void OnCheckCurrentzdjhavpa();
		afx_msg void OnCheckCurrentPA();
		afx_msg void OnCheckCurrentZdjhPA();
		afx_msg void OnCheckDh();
		afx_msg void OnCheckSh();
		afx_msg void OnSelchangeComboPA();
		afx_msg void OnSelchangeComboSA();
		afx_msg void OnCheckCurrentzdjhavSA();
		afx_msg void OnCheckCurentSA();
		afx_msg void OnCheckCurrentzdjhSA();
		afx_msg void OnCheckSabeam();
		afx_msg void OnCheckSacant();
		afx_msg void OnCheckSalbrace();
		afx_msg void OnCheckSagcement();
		afx_msg void OnCheckAutosprnum();
		afx_msg void OnSelchangeComboSprnum();
		afx_msg void OnSelchangeComboCsprnum();
		virtual BOOL OnInitDialog();
		afx_msg void OnSelchangeListStruct();
		afx_msg void OnSelchangeListPart();
		afx_msg void OnCmdAdd();
		afx_msg void OnCmdDel();
		afx_msg void OnCmdSave();
		virtual void OnOK();
		afx_msg void OnDestroy();
		afx_msg void OnCheckBmp();
		afx_msg void OnButton3();
		afx_msg void OnCheckPa();
		afx_msg void OnCheckOnlypah();
		afx_msg void OnDblclkListName();
		afx_msg void OnBtnFavoriteTemplate();
	afx_msg void OnRadioFavoriteTemplate();
	afx_msg void OnRadioDefaultTemplate();
	afx_msg void OnRadioAllTemplate();
	afx_msg void OnButtonSetDefaultFavorite();
	afx_msg void OnItemchangedListctrlStruct(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonDelTemplate();
	afx_msg void OnCmdCancel();
	afx_msg void OnButton5();
	afx_msg void OnCheck11();
	afx_msg void OnSelchangeComboRodnum();
	//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
private:
	int m_iSavex;
	int iSaveTmpID;
	BOOL m_bIsOpenDefaultFavoriteTemplate;
	bool m_bSetDefaultFavorite;
	void SetDefaultFavoriteTemplate();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELTEMPLATE_H__0252FF33_19FF_4951_A78C_181C1808BB4D__INCLUDED_)
