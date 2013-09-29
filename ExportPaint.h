// ExportPaint.h: interface for the CExportPaint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXPORTPAINT_H__0A13B517_585A_4898_924C_F7E01645428A__INCLUDED_)
#define AFX_EXPORTPAINT_H__0A13B517_585A_4898_924C_F7E01645428A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CExportPaint  
{
public:
	bool StartupAutoIPED();

    CString m_strDbPath, //项目数据库路径
			m_strWorkPath,//工作路径
			m_strInsPath;//程序路径

	CString m_strVlmCODE;  //卷册代号
	CString m_strPrjID;		//工程代号

	BOOL SelEngVol();
	CExportPaint();
	virtual ~CExportPaint();
	////得到IPED注册表信息
	BOOL GetIPEDPath();

protected:
	void SetCodePath(CString& strPath);
};

#endif // !defined(AFX_EXPORTPAINT_H__0A13B517_585A_4898_924C_F7E01645428A__INCLUDED_)
