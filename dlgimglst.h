#if !defined(AFX_DLGIMGLST_H__7AD465A0_80DF_11D5_8752_00D009619765__INCLUDED_)
#define AFX_DLGIMGLST_H__7AD465A0_80DF_11D5_8752_00D009619765__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgImgLst.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgImgLst dialog
#include "MImage.h"
#include "Cphs.h"

#define IMG_ID 4000
class CImgList : public CTypedPtrList <CObList,CMImage* >
{
private:
	long m_Count;

public:
	CImgList(){
		m_Count=0;
		
	}
	virtual ~CImgList(){
		CMImage* mTmp;
		POSITION po;
		po=GetHeadPosition();
		while(po!=NULL)
		{
			mTmp=GetNext(po);
			delete mTmp;
		}
		RemoveAll();
	}
   	void SetCount(long nCount)
	{
		CMImage* mTmp;
		/*po=GetHeadPosition();
		while(po!=NULL)
		{
			mTmp=GetNext(po);
			mTmp->DestroyWindow();
			delete mTmp;
		}
		RemoveAll();
		for(int i= 0 ;i<nCount;i++)
		{
			mTmp=new CMImage;
			AddTail(mTmp);
		}*/
		m_Count=this->GetCount();
		if(m_Count < nCount)
		{
			int C=nCount-m_Count;
			for(int i=0;i<C;i++)
			{
				mTmp=new CMImage;
				AddTail(mTmp);
			}
		}
		else if(m_Count > nCount)
		{
			int C=m_Count-nCount;
			for(int i=0;i<C;i++)
			{
				mTmp=GetTail();
				if(IsWindow(mTmp->m_hWnd))
				{
					mTmp->DestroyWindow();
				}
				delete mTmp;
				RemoveTail();
			}
		}
	}
	CMImage* operator [] (int nIndex ){
		POSITION po;
		po=FindIndex(nIndex);
		return GetAt(po);
	}
};
class CDlgImgLst : public CWnd
{
// Construction
public:
	int m_ImgIx;
	void OnImgClick(int Index);
	void LoadAllImage();
	virtual  ~CDlgImgLst();
	CDlgImgLst(CListBox* pListZDG,CListBox* pListBox);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgImgLst)
	//enum { IDD = IDD_IMG_LST };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	int m_imgCount;
	CImgList m_Image;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgImgLst)
	public:
	virtual BOOL Create(CWnd* pParentWnd=NULL);
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	CListBox* m_pListBox;
	CListBox *m_pListZDG;
	CBrush m_bkbr;
	// Generated message map functions
	//{{AFX_MSG(CDlgImgLst)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGIMGLST_H__7AD465A0_80DF_11D5_8752_00D009619765__INCLUDED_)
