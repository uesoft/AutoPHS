#if !defined(AFX_XIMGLISTBOX_H__BE2D25BA_CB7E_4A23_83A2_B2EF5CB4A0A9__INCLUDED_)
#define AFX_XIMGLISTBOX_H__BE2D25BA_CB7E_4A23_83A2_B2EF5CB4A0A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// XImgListBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CXImgListBox window
#include <afxtempl.h>
class CXImgListBox : public CListBox
{
// Construction
public:
	CXImgListBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXImgListBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetAutoImgSize(BOOL bAutoImgSize);
	void DeleteBmp(int iItem);
	int AddBitmapFile(LPCTSTR lpszFileName);
	int AddBitmapID(UINT nID);
	HBITMAP GetItemBmp(int iItem);
	int AddBitmap(HBITMAP hBm);
	void SetBmpSize(DWORD cx,DWORD cy);
	virtual ~CXImgListBox();

	// Generated message map functions
protected:
	BOOL m_bAutoImgSize;
	CSize m_BmpSize;
	CList < HBITMAP,HBITMAP& > m_lBitmaps;
	//{{AFX_MSG(CXImgListBox)
	afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct);
	afx_msg int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XIMGLISTBOX_H__BE2D25BA_CB7E_4A23_83A2_B2EF5CB4A0A9__INCLUDED_)
