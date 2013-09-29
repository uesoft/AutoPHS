// AnalyseSAPara.h: interface for the AnalyseSAPara class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANALYSESAPARA_H__688CCFFA_E49E_4A83_97A9_8C14568516E5__INCLUDED_)
#define AFX_ANALYSESAPARA_H__688CCFFA_E49E_4A83_97A9_8C14568516E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//仅保存根部的参数.
class AnalyseSAPara  
{
public:
	void SettbnSA(CString &tbnSA);
	void Setgdw(float gdw);
	
	void SetglNumSA(int glNumSA);
	void SetWidthB(float WidthB);
	void SetLspan(float Lspan);
	void SetgiSAG100No(int giSAG100No);
	
	int GetgiSAG100No();
	float GetLspan();
	float GetWidthB();
	int GetglNumSA();
	float Getgdw();
	CString GettbnSA();

	
	CString m_tbnSA;//modPHScal::tbnSA
	float m_gdw;//modPHScal::gdw
	int m_glNumSA;//modPHScal::glNumSA 根部的数量	
	float m_WidthB;//modPHScal::WidthB根部柱子宽度B值
	float m_Lspan;//modPHScal::Lspan根部L1值=跨距
	int m_giSAG100No;//modPHScal::giSAG100No

	AnalyseSAPara();
	virtual ~AnalyseSAPara();

};

#endif // !defined(AFX_ANALYSESAPARA_H__688CCFFA_E49E_4A83_97A9_8C14568516E5__INCLUDED_)
