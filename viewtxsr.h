//{{AFX_INCLUDES()
#include "adodc.h"
//#include "datacombo.h"
//}}AFX_INCLUDES
#if !defined(AFX_VIEWTXSR_H__CB8E20F7_6BCF_11D6_8751_00D009619765__INCLUDED_)
#define AFX_VIEWTXSR_H__CB8E20F7_6BCF_11D6_8751_00D009619765__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FrmTxsr.h : header file
//
#include "_recordset.h"
#include "MyStatic.h"
/////////////////////////////////////////////////////////////////////////////
// CViewTxsr dialog
#include "TxsrPag1.h"
#include "TxsrPag2.h"
#include "TxsrPag3.h"
#include "PagTxsrOption.h"
#include "TxsrPagItem.h"
#include "Xtabctrl.h"
class CRstEvent;
class CFrmTxsr;

extern CTxsrPag1  *pTestWnd;

class CViewTxsr : public CFormView
{
// Construction
	friend class CFrmTxsr;
protected:
	DECLARE_DYNCREATE(CViewTxsr)
		
	CViewTxsr();   // standard constructor
	virtual  ~CViewTxsr();
public:

	//The follow Function and variable added by ³ÂÓî, for record copy, paste and Undo 
	//The data is recorded to Table Clip(¸´ÖÆ°å) and Undo(³·Ïú²Ù×÷) in AllPrjDB.mdb 
	short m_UndoCount; //Record Undo times of current recordset m_UndoCount
	                   //Initial in Function InitRs() by Function SetUndoCount()
	long GetFirstRow(VARIANT vBookmark);
	void UndoDelete(); //Invoked by UndoPaste to clear the records which were 
	                   //restore to Recordset m_ActiveRs
	void UndoPaste(void); //Restore the latest recorded Record to Recordset 
	                      //m_ActiveRs
	void SetUndoCount(void); //Get current Undo times to m_UndoCount,the times of
	                         //current Undo saved in fields Times in Table Undo
	void InitialUndo(void); //InitialUndo clear Table Undo
	void UndoCopy(void); //Save Recordset m_ActiveRs to Table Undo
	void EditCopy(void); //Save selected record to Clip
	void EditPaste(void); 
	//The End By ³ÂÓî
	void EditDel(void);

	bool m_bIsActiveing;
	void LoadtbnPAItem2ComBox(CComboBox & combo, CString strFLD);
	void LoadGDWItem2ComboGDW1();
	void LoadPAMaterialItem2ComboDCL1();
	void LoadDiameterItem2ComboDW();
//	CRstEvent * m_pRStEvent;
	void AddNewRec();
	bool m_bAllowUpd;
	long GetMaxZdjh();
	bool m_bChangeZDJH;
	bool m_bIsAddNew;
	bool m_bIsUpd;
	bool m_bIsInit;
	void SelSPR();
	bool m_bIsInitHint;
	CToolTipCtrl m_ctrlHint;
	void PhsCalSelection(int index);
	enum mnuPhsCalSelectionIndex{
		iPhsCalSelectionALL=0,
		iPhsCalSelectionNotSuccess=1,
		iPhsCalSelectionSuccess=2,
		iPhsCalSelectionSPR=3,
		iPhsCalSelectionCSPR=4,
		iPhsCalSelectionNotSPRAndCSPR=5,
		iPhsCalSelectionSPRAndNotSuccess=6,
		iPhsCalSelectionSPRAndSuccess=7,
		iPhsCalSelectionCSPRAndNotSuccess=8,
		iPhsCalSelectionCSPRAndSuccess=9,
	};
	CMenu m_popMenu;
	void RefreshOptData();
	void ConvertUnit();
	bool m_bActive;
	BOOL m_bUpd;
	CString m_SQL;
	void InitRs();
	void CloseRs();
	bool m_InitWnd;
	void ChangeImg(CStatic &con, LPCSTR nID,BOOL bt=TRUE);
	int m_iCSnum;
	void ChangeImg(CStatic& con,UINT nID,BOOL bt=TRUE);
//	_RecordsetPtr m_dataLstComboDW;
	void UpdateBoundData();
	void RefreshBoundData();
	void DisPlaySANum();
	_RecordsetPtr m_ActiveRs;
	void SetActivePage(int ix);
/*	CPropertyPage m_pag1;
	CPropertyPage m_pag2;
	CPropertyPage m_pag3;
	CPropertyPage m_pag4;
	CPropertyPage m_pag5;
	CPropertySheet m_PagSheet;*/
	//C_Recordset rs;
	CTxsrPag1* m_OptPag4;
	CTxsrPag2* m_OptPag5;
	CTxsrPag3* m_OptPag6;
	CPagTxsrOption* m_PagOption;
	CTxsrPagItem* m_PagItem;
// Dialog Data
	//{{AFX_DATA(CViewTxsr)
	enum { IDD = IDD_TXSR };
	CComboBox	m_ComBoBornSA;
	CXTabCtrl	m_tabTxsr;
	CEdit	m_edtZdjh;
	CStatic	m_wndBorand;
	CStatic	m_LabelMsgbox;
	CMyStatic	m_ImageViewG0;
	CMyStatic	m_ImageViewG1;
	CMyStatic	m_Image1;
	CMyStatic	m_ImageViewD0;
	CMyStatic	m_ImageViewD1;
	CMyStatic	m_ImageXY1;
	CMyStatic	m_ImageXY0;
	CComboBox	m_comboDW;
	CComboBox	m_comboJSJDH;
	CComboBox	m_comboZDJH1;
	CButton	m_cmdSelPhs;
	CComboBox	m_comboDwB;
	CComboBox	m_comboGDW1;
	CComboBox	m_comboParallelingNum;
	CComboBox	m_comboT01;
	CListBox	m_LstPhsStructure;
	CStatic	m_Pic;
	CAdodc	m_Databill;
	CString	m_txtXZU1;
	CString	m_txtXZU2;
	CString	m_txtXZU3;
	CString	m_txtXZU4;
	CString	m_txtXZU5;
	CString	m_txtXZU6;
	CString	m_txtXZU7;
	CString	m_txtXZU8;
	CString	m_txtZZU1;
	CString	m_txtZZU2;
	CString	m_txtZZU3;
	CString	m_txtZZU4;
	CString	m_txtZZU5;
	CString	m_txtZZU6;
	CString	m_txtZZU7;
	CString	m_txtZZU8;
	CString	m_txtType;
	float	m_fDZA1;
	float	m_fGH1;
	float	m_fDH1;
	float	m_fDXA1;
	float	m_fLC1;
	float	m_fB01;
	float	m_fA01;
	float	m_fCoefOfVarLoad;
	long	m_lSelSampleID;
	float	m_fSATotalLength;
	CString	m_csPhsStructure;
	CString	m_csParallelingNum;
	CString	m_csT01;
	CString	m_csGDW1;
	CString	m_csDJ1;
	CString	m_csDwB;
	CString	m_csJSDBH1;
	CString	m_csZDJH;
	long	m_lngZDJH;
	CString	m_strBornSA;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewTxsr)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void OnInitialUpdate();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	void LoadTJItem2ComboT01();
	void ShowPictureGenbuFX();
	void ShowPicturePASA(int Index);
	void DatabillReposition();
	void LoadStrFLDItem2MyComboBox(CComboBox& comBox,CString strFLD);
	void SetOptionPZ();

	// Generated message map functions
	//{{AFX_MSG(CViewTxsr)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnWillMoveDatabill(long adReason, long FAR* adStatus, LPDISPATCH pRecordset);
	afx_msg void OnMoveCompleteDatabill(long adReason, LPDISPATCH pError, long FAR* adStatus, LPDISPATCH pRecordset);
	afx_msg void OnButton5();
	afx_msg void OnImgViewg1();
	afx_msg void OnImgViewd1();
	afx_msg void OnImgImg1();
	afx_msg void OnSelchangeList1();
	afx_msg void OnSelchangeZdjh();
	afx_msg void OnImgXy0();
	afx_msg void OnBtnAdd();
	afx_msg void OnDestroy();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSelchangeJSJDH();
	afx_msg void OnGetPrevoiusPhsData();
	afx_msg void OnSelectionAll();
	afx_msg void OnCalSelectionNotSuccess();
	afx_msg void OnSelectionSuccess();
	afx_msg void OnSelectionSPR();
	afx_msg void OnSelectionCSPR();
	afx_msg void OnSelectionNotSPRAndCSPR();
	afx_msg void OnChangeEdtType();
	afx_msg void OnUpdateEdtType();
	afx_msg void OnSetfocusEdtType();
	afx_msg void OnGetZdjhdata();
	afx_msg void OnSelectionSPRandNotCal();
	afx_msg void OnSelectionSPRandCal();
	afx_msg void OnSelectionCSPRandNotCal();
	afx_msg void OnSelectionCSPRandCal();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnClose();
	afx_msg void OnButtonSaveJSJDH();
	afx_msg void OnKillfocusEdtSsid();
	afx_msg void OnKillfocusEdtType();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	afx_msg void OnPage2EditChange();
	afx_msg void OnPage1EditChange();
	DECLARE_MESSAGE_MAP()
private:
	void GetBornSA(CString & m_strBornSA);
	void InitFromReg();
	float m_FrictionFree;
public:
	CString m_strBornSAEngChina;
	CString m_strBornSAEng;
	void InitBornSA();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRMTXSR_H__CB8E20F7_6BCF_11D6_8751_00D009619765__INCLUDED_)
