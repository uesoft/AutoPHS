// AnalysePAPara.h: interface for the AnalysePAPara class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANALYSEPAPARA_H__18593474_FA28_44DD_AE90_8B45974385E4__INCLUDED_)
#define AFX_ANALYSEPAPARA_H__18593474_FA28_44DD_AE90_8B45974385E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class AnalysePAPara  
{
public:
	void SetPAfixH(float PAfixH);
	void SetgnDW_delta(float gnDW_delta);
	void Setdj(float dj);
	void SettbnPA(CString & tbnPA);
	void SetgbPAForceZero(float  gbPAForceZero);

	bool GetgbPAForceZero();
	CString GettbnPA();
	float Getdj();
	float GetgnDW_delta();
	float GetPAfixH();

public:
	
	AnalysePAPara();
	virtual ~AnalysePAPara();
	
private:
	float m_PAfixH;//modPHScal::PAfixH固定支架管部高度H,cm
	float	m_gnDW_delta;//modPHScal::gnDW_delta查找管径范围扩大,0~7(%)
	float	m_dj;//modPHScal::dj管外径
	CString	m_tbnPA;//modPHScal::tbnPA管部表名称
	bool m_gbPAForceZero;//modPHScal::gbPAForceZero当管部的许用荷载为零时不考虑荷载.
};

#endif // !defined(AFX_ANALYSEPAPARA_H__18593474_FA28_44DD_AE90_8B45974385E4__INCLUDED_)
