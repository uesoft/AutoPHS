// PHSSetting.h: interface for the PHSSetting class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PHSSETTING_H__9CC857C0_BCB5_464C_921C_3FD8824428CC__INCLUDED_)
#define AFX_PHSSETTING_H__9CC857C0_BCB5_464C_921C_3FD8824428CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class __declspec( dllexport ) PHSSetting  
{
public:
	PHSSetting();
	virtual ~PHSSetting();

public:
	virtual BOOL InitSetting() = 0;
	virtual BOOL SaveSetting() = 0;
};

#endif // !defined(AFX_PHSSETTING_H__9CC857C0_BCB5_464C_921C_3FD8824428CC__INCLUDED_)
