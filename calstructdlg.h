//{{AFX_INCLUDES()
#include "datagrid.h"
//}}AFX_INCLUDES
#if !defined(AFX_CALSTRUCTDLG_H__48B8ABD8_ABC2_4A6D_9043_F9CC0F8E3097__INCLUDED_)
#define AFX_CALSTRUCTDLG_H__48B8ABD8_ABC2_4A6D_9043_F9CC0F8E3097__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CalStructDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCalStructDlg dialog
#include "XImgListBOx.h"
#include "XEdit.h"
#include "XTabCtrl.h"
#include <afxtempl.h>
class CCalStructPag;
#include "datagrid.h"
#include "Columns.h"
#include "Column.h"

#define CAL_REGULAR			0
#define CAL_RESULT_NULL		1
#define CAL_NOT_REGULAR		2
#define	CAL_ERROR			3
#define CAL_INITVAR_ERROR	4

#include "GridCtrl_src/GridCtrl.h"
#include "XDialog.h"
class CCalStructDlg : public CXDialog
{
// Construction
public:
	void UpdateComLen();
	virtual ~CCalStructDlg();
	typedef struct __ComInfo{
		int iType;
		int iGg;
		int iMaterial;
		UINT nLen;
		float fTmp;
	} _ComInfo,* PComInfo;

	typedef struct _ComTypeInfo{
		CStringArray m_astrTypes;
	}ComTypeInfo, * PComTypeInfo;

	typedef struct _SSteelInfo{
		_TCHAR szID[8];
		_TCHAR szCustomID[8];
	}SSteelInfo,* PSSteelInfo;
	
	typedef struct _VarInfo{
		_TCHAR szName[8];
		float	fVal;
	}VarInfo, * PVarInfo;

	typedef struct _ComPt{
		float fTmp;						//温度
		CString strID;					//零件ID
		CString strCustomID;			//零件CustomID
		CString strMaterial;			//材料
		CString	strClgg;				//规格
		UINT	nLen;					//长度
	}ComPt,* PComPt;

	typedef struct _StrCom{
		long lStructID;					//结构ID
		long lComNo;					//构件号
	}StrCom,* PStrCom;

	typedef struct _CalInfo{
		CString strResult;				//计算结果
		CString strWhere;				//条件
		CString strFormula;				//公式
		PStrCom pStrCom;				//结构,构件
		PComPt pComPt;					//构件特性数据
		CString strCPT;					//比较运算符
		float fWhereVal;			//比较值
	}CalInfo,* PCalInfo;
	

	CCalStructDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCalStructDlg)
	enum { IDD = IDD_DIALOG_CALSTR };
	CXEdit	m_editOk;
	CComboBox	m_comboTmp;
	CComboBox	m_comboComMaterial;
	CXTabCtrl	m_tabCal;
	CXEdit	m_edtOut;
	CXImgListBox	m_lstImg;
	CString	m_strOut;
	CDataGrid	m_gridVar;
	CDataGrid	m_gridMainDim;
	CString	m_strStatusIsOk;
	//}}AFX_DATA
	CGridCtrl m_ComGrid;
	CGridCtrl m_PjgGrid;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCalStructDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void CalComLen(_RecordsetPtr rstheVar=NULL);
	CString GetOpt(LPCTSTR lpszOpt);
	long m_lStructIDRes;
	double GetFiOfLamda(double dLamda,LPCTSTR lpszMaterial);
	ComTypeInfo m_ComTypeInfo[5];
	BOOL ParseTypes(CStringArray& strArray,LPCTSTR lpszTypes);
	afx_msg void OnEndEditInPjgGridCell(NMHDR *pNMHDR, LRESULT *pResult);
	void UpdatePjg(LPCTSTR lpszPjg=NULL);
	void LoadPjgList();
	CMapStringToString m_mapPjg;
	_RecordsetPtr m_rsVariable;
	_RecordsetPtr m_rsMainDim;
	int m_iTabSel;
	CString m_astrGridSel[5][3];
	int m_iComCount;
	void LoadComGrid();
	void LoadTmp();
	int m_iTmpComCount;
	UINT* m_pnLen;
	CString* m_pstrType;
	void LoadComGridGg(int iRow);
	afx_msg void OnEndEditInComGridCell(NMHDR* pNMHDR, LRESULT* pResult);
	void LoopSelCom(long lStructID);
	BOOL InitVar(PComPt pComPt,PStrCom pStrCom,BOOL	 bCurComNo=TRUE,BOOL bOther=TRUE,BOOL bUpdMaterial=TRUE,BOOL bUpdCom=TRUE);
	DWORD CalStruct(float * pResult,PCalInfo pCalInfo,BOOL bMakeTab=TRUE,int iTabNo=1);
	void InitVar(int iComNo,BOOL bCurComNo=TRUE,BOOL bOther=TRUE,BOOL bUpdMaterial=TRUE,BOOL bUpdCom=TRUE);
	BOOL m_bAllowInitVar;
	void DeleteAllComType();
	void LoadComMaterial();
	long m_lClassID;
	long m_lStructID;
	CCalStructPag* GetPag(int index);
	CList <CCalStructPag*,CCalStructPag*> m_wndTabs;
	int m_iOldListIndex;
	void DeleteAllTab();
	void LoadImgList();
	_ConnectionPtr m_connSASCal;//20071103
	_ConnectionPtr m_connMaterial;//20071102
	_ConnectionPtr m_connSort;

	// Generated message map functions
	//{{AFX_MSG(CCalStructDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChangeListImg();
	afx_msg void OnBtnCal();
	afx_msg void OnBeforeUpdateDatagrid1(short FAR* Cancel);
	afx_msg void OnBeforeUpdateDatagridMainDim(short FAR* Cancel);
	afx_msg void OnDestroy();
	afx_msg void OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeComboComMaterial();
	afx_msg void OnSelendokComboTmp();
	afx_msg void OnEditupdateComboTmp();
	afx_msg void OnButtonLoopCal();
	afx_msg void OnSelendokComboComMaterial();
	afx_msg void OnButtonShowRes();
	afx_msg void OnAfterColUpdateDatagridMaindim(short ColIndex);
	afx_msg void OnBtnDefaultformula();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CALSTRUCTDLG_H__48B8ABD8_ABC2_4A6D_9043_F9CC0F8E3097__INCLUDED_)
