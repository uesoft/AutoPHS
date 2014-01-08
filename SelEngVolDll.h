// SelEngVolDll.h: interface for the CSelEngVolDll class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SELENGVOLDLL_H__196ECA30_613B_49FD_822A_E0FB9DB7AEDA__INCLUDED_)
#define AFX_SELENGVOLDLL_H__196ECA30_613B_49FD_822A_E0FB9DB7AEDA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class AFX_EXT_CLASS CSelEngVolDll  
{
public:
	char* m_SelPrjName;//工程名称
	char* m_SelVlmName;//卷册名称
	char* m_SelVlmCODE;//卷册代号
	long m_SelVlmID;//卷的ID
	long m_SelHyID;//行业的ID
	char* m_SelPrjID;//工程的ID
	long m_SelDsgnID;//设计阶段的ID
	long m_SelSpecID;//专业的ID
	BOOL ShowEngVolDlg(LPTSTR cWorkPrjPathName,LPTSTR cAllProjDBPathName,LPTSTR cSortPathName );
	CSelEngVolDll();
	virtual ~CSelEngVolDll();
private:
	CString m_strSelPrjID;//pfg2007.08.20
	CString m_strSelVlmCODE;
	CString m_strSelPrjName;
	CString m_strSelVlmName;
	
	CString m_workprjPathName;//数据库workprj的全路径名
	CString m_AllPrjDBPathName;//数据库AllPrjDB的全路径名
	CString m_sortPathName;//数据库sort的全路径名	

};

#endif // !defined(AFX_SELENGVOLDLL_H__196ECA30_613B_49FD_822A_E0FB9DB7AEDA__INCLUDED_)
