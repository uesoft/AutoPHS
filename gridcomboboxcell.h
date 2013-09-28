// GridComboBoxCell.h: interface for the CGridComboBoxCell class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRIDCOMBOBOXCELL_H__558CC278_47AE_45C1_AF31_C905A86E27E7__INCLUDED_)
#define AFX_GRIDCOMBOBOXCELL_H__558CC278_47AE_45C1_AF31_C905A86E27E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GRIDCTRL_SRC\GridCell.h"

class CInPlaceList;
class CGridComboBoxCell : public CGridCell  
{
	friend class CInPlaceList;
	DECLARE_DYNCREATE(CGridComboBoxCell)
public:
	int FindStringExact(LPCTSTR lpszText);
	void SetCurSel(int iIndex);
	int GetCurSel();
	DWORD m_dwStyle;
	CStringArray m_Items;
	CDWordArray	m_ItemDatas;
	virtual void EndEdit();
	virtual BOOL Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);
	CGridComboBoxCell();
	virtual ~CGridComboBoxCell();

protected:
	int m_iCurSel;
};

#endif // !defined(AFX_GRIDCOMBOBOXCELL_H__558CC278_47AE_45C1_AF31_C905A86E27E7__INCLUDED_)
