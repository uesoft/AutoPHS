#if !defined(AFX_DBCOMBOBOX_H__0236474C_81AB_11D5_8752_00D009619765__INCLUDED_)
#define AFX_DBCOMBOBOX_H__0236474C_81AB_11D5_8752_00D009619765__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DBComboBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDBComboBox window
#define ON_SELDATACHANGE WM_USER+100
class CDBComboBox : public CComboBox
{
// Construction
public:
	CDBComboBox();

// Attributes
public:
	FARPROC SelChange;
	_RecordsetPtr m_RowRs;
	_RecordsetPtr m_Rs;
	CString m_Field;
	CString m_BoundField;
	CString m_ListField;
	BOOL m_bDoSelchange;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDBComboBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	void RefLst();
	void LoadList();
	virtual ~CDBComboBox();

	// Generated message map functions
public:
	void OnSelchange();
//	_variant_t * m_LstID;
	//{{AFX_MSG(CDBComboBox)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	bool m_fis;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DBCOMBOBOX_H__0236474C_81AB_11D5_8752_00D009619765__INCLUDED_)
