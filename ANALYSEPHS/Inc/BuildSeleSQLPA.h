// BuildSeleSQLPA.h: interface for the BuildSeleSQLPA class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUILDSELESQLPA_H__2CF82A0B_8D93_470F_9112_331CD20064EC__INCLUDED_)
#define AFX_BUILDSELESQLPA_H__2CF82A0B_8D93_470F_9112_331CD20064EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "AnalysePAPara.h"
#include "AnalyseHangPara.h"
#include "AnalysePHSPara.h"

class BuildSeleSQLPA  
{
public:
	AnalysePAPara * m_PAPara;
	AnalyseHangPara *m_Para;
	AnalysePHSPara *m_PHSPara;
public:
//	CString GetiPAD4LA(CString &Ptype);
//	CString GetiPAsSQL(CString &Ptype);
//	CString GetiPAfixZ1(CString &Ptype);
//	CString GetiPAhSQL(CString& Ptype);
	BuildSeleSQLPA();
	virtual ~BuildSeleSQLPA();

};

#endif // !defined(AFX_BUILDSELESQLPA_H__2CF82A0B_8D93_470F_9112_331CD20064EC__INCLUDED_)
