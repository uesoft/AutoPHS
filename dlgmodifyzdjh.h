#if !defined(AFX_DLGMODIFYZDJH_H__682322F2_D864_47D0_B5BD_D8DF985AA5B3__INCLUDED_)
#define AFX_DLGMODIFYZDJH_H__682322F2_D864_47D0_B5BD_D8DF985AA5B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgModifyZdjh.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgModifyZdjh dialog
class CDlgModifyZdjh : public CDialog
{
// Construction
public:
	void ArrangeList2();
	static int GetListCurSel(CListCtrl* pList);
	static void SetListCurSel(CListCtrl* pList,int iSel);
	typedef struct _tagListData{
		int iZdjh,iNodeNO;
		float fDw,fTj;
	}tagListData,FAR * LPListData;
	void LoadZdj();
	long m_iVlmID;
	CImageList m_ImgList;
	CDlgModifyZdjh(long iVlmID,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgModifyZdjh)
	enum { IDD = IDD_MODIFYZDJH_FORM };
	CListCtrl	m_list2;
	CListCtrl	m_list1;
	CString	m_strLab1;
	CString	m_strLab2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgModifyZdjh)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateItemLab();
	void SortList1(int iSortType,int iCol);
	int m_iCurSortCol;
	enum enumSortType{ iNone=0,iAsc,iDesc};	
	enumSortType m_list1SortType[4];
	// Generated message map functions
	//{{AFX_MSG(CDlgModifyZdjh)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnDeleteall();
	afx_msg void OnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedList2(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	afx_msg void OnColumnclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkList2(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMODIFYZDJH_H__682322F2_D864_47D0_B5BD_D8DF985AA5B3__INCLUDED_)
