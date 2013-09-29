// SysVar.h: interface for the CSysVar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYSVAR_H__C2E7E598_64C9_401B_AD25_9F90B9D65C9A__INCLUDED_)
#define AFX_SYSVAR_H__C2E7E598_64C9_401B_AD25_9F90B9D65C9A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSysVar  
{
public:
	CSysVar();
	virtual ~CSysVar();
	static const int sInt;
	static const char* ssChar;
	static CString sstr;

};

#endif // !defined(AFX_SYSVAR_H__C2E7E598_64C9_401B_AD25_9F90B9D65C9A__INCLUDED_)
