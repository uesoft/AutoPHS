// DlgDataFormatDll.h: interface for the CDlgDataFormatDll class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLGDATAFORMATDLL_H__3A15C278_B79C_4549_BC5B_52379BA42AB7__INCLUDED_)
#define AFX_DLGDATAFORMATDLL_H__3A15C278_B79C_4549_BC5B_52379BA42AB7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// __declspec(dllexport) 
class  AFX_EXT_CLASS CDlgDataFormatDll  
{
public:
	//数据库的全路径名。(sotr.mdb);
	//CString  m_strDBPath;
	//弹出格式对话框。(调用之前必须设置数据库的全路径名)
	void ShowFormatDlg( LPTSTR m_cDBPath );
	CDlgDataFormatDll();
	virtual ~CDlgDataFormatDll();
};

#endif // !defined(AFX_DLGDATAFORMATDLL_H__3A15C278_B79C_4549_BC5B_52379BA42AB7__INCLUDED_)
