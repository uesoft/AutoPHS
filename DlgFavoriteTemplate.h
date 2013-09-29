#if !defined(AFX_DLGFAVORITETEMPLATE_H__0BDE0559_1323_418D_A3BE_1EBAAEFAC3DD__INCLUDED_)
#define AFX_DLGFAVORITETEMPLATE_H__0BDE0559_1323_418D_A3BE_1EBAAEFAC3DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgFavoriteTemplate.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgFavoriteTemplate dialog

class CDlgFavoriteTemplate : public CDialog
{
// Construction
public:
	int m_iCurSel;
	void Delete(int index);
	long m_lCurSelSampleID;
	void InitFavoriteList();
	long m_lSampleID;
	CDaoRecordset m_rsFavorite;
	CDlgFavoriteTemplate(CWnd* pParent ,long lSampleID);
	CDlgFavoriteTemplate(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgFavoriteTemplate)
	enum { IDD = IDD_FAVORITE_TEMPLATE };
		// NOTE: the ClassWizard will add data members here
	CListBox m_lstFavorite;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgFavoriteTemplate)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgFavoriteTemplate)
	virtual BOOL OnInitDialog();
	afx_msg void OnDelete();
	virtual void OnOK();
	afx_msg void OnSelchangeListFavorite();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGFAVORITETEMPLATE_H__0BDE0559_1323_418D_A3BE_1EBAAEFAC3DD__INCLUDED_)
