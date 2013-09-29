// ClipCtrl.h: interface for the CClipCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIPCTRL_H__B750D0C4_A54B_4B10_876C_AC3C146D420F__INCLUDED_)
#define AFX_CLIPCTRL_H__B750D0C4_A54B_4B10_876C_AC3C146D420F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

typedef std::vector<VARIANT> RecordVal;
typedef std::vector<int> SelectedCol;

class CClipCtrl  
{
public:
	CClipCtrl();
	virtual ~CClipCtrl();

public:
	bool m_bClipFlag; //If any data existed, set this to true, else false

    int m_nRow; //count of selected row
	int m_nCol; //count of selected col

    RecordVal m_vRecord; //save for record in table
	SelectedCol m_nSelectedCol; //Index of Col in table
public:
	
	VARIANT GetRecord(int nRow, int nCol); //Return m_vRecord value, nRow and nCol pointed location of record 
	void Initial(int nrow, int ncol); //Initial Class CClipCtrl, assigned space for record nrow X nrol    
    void SetClipFlag(bool bFlag); //Set m_bClipFlag value
	void SetRecord(int nRow, int nCol, VARIANT vRecord); //Set m_vRecord value, nRow and nCol pointed location of record 
	void Clear(); //Set member zero, clear m_vRecord and m_nCol
};

#endif // !defined(AFX_CLIPCTRL_H__B750D0C4_A54B_4B10_876C_AC3C146D420F__INCLUDED_)
