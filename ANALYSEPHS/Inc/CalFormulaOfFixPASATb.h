// CalFormulaOfFixPASATb.h: interface for the CalFormulaOfFixPASATb class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CALFORMULAOFFIXPASATB_H__8BBEE884_B46E_4A6A_8ACA_9FA9C7FC4E2E__INCLUDED_)
#define AFX_CALFORMULAOFFIXPASATB_H__8BBEE884_B46E_4A6A_8ACA_9FA9C7FC4E2E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//Cphs.cpp(3639,4390)
//打开固定支架计算公式表，备计算
//SQLx = _T("SELECT * FROM CalFormulaOfFixPASA WHERE CustomID=\'") + Ptype[i] + _T("\' ORDER BY CustomID,SEQ");
class CalFormulaOfFixPASATb  
{
public:
	CalFormulaOfFixPASATb();
	virtual ~CalFormulaOfFixPASATb();
public:
	CString Wyh1sa;
	CString Wzh1sa;
	CString Wyh2sa;
	CString Wzh2sa;
	CString SIGMA1saMax;
	CString TAO1saMax;
	CString SIGMA2saMax;
	CString TAO2saMax;
	CString lamda1;
	CString lamda2;
	CString SIGMA1sa;
	CString TAOx1sa;
	CString TAOy1sa;
	CString TAOz1sa;
	CString TAO1sa;
	CString SIGMA2sa;
	CString TAOx2sa;
	CString TAOy2sa;
	CString TAOz2sa;
	CString TAO2sa;
	
};

#endif // !defined(AFX_CALFORMULAOFFIXPASATB_H__8BBEE884_B46E_4A6A_8ACA_9FA9C7FC4E2E__INCLUDED_)
