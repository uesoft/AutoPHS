#if !defined(AFX_MIMAGE_H__03DB3055_76CC_11D5_8752_00D009619765__INCLUDED_)
#define AFX_MIMAGE_H__03DB3055_76CC_11D5_8752_00D009619765__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MImage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMImage window
#define IMG_CLICKED WM_USER+100
#define IMG_W  78
#define IMG_H  104
class CMImage : public CStatic
{
// Construction
public:
	CMImage();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMImage)
	public:
	virtual BOOL Create(CWnd* pParentWnd, UINT nID);
	//}}AFX_VIRTUAL

// Implementation
public:
	bool Dstinvert(bool bDst=true);
	bool m_bDst;
	BOOL GetVisible();
	BOOL m_Visible;
	void SetVisible(BOOL newValue);
	void SetLeft(int left);
	void SetTop(int top);
	int GetLeft();
	int GetTop();
	void SetWidth(int Width);
	void SetHeight(int Height);
	int GetWidth();
	int GetHeight();
	void SetImage(const _TCHAR* imgName);
	virtual ~CMImage();
	int m_Index;
	// Generated message map functions
protected:
	//{{AFX_MSG(CMImage)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MIMAGE_H__03DB3055_76CC_11D5_8752_00D009619765__INCLUDED_)
