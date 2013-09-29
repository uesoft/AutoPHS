// IPAHSel.h: interface for the IPAHSel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPAHSEL_H__B0BBD8E9_8972_41D2_83AC_DD3B3BBD8DD9__INCLUDED_)
#define AFX_IPAHSEL_H__B0BBD8E9_8972_41D2_83AC_DD3B3BBD8DD9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CompSel.h"

class SupportCompData;
class ParaBase;
class IPAHSel : public CompSel  
{
public:
	IPAHSel();
	virtual ~IPAHSel();
	virtual void GetSearchComp(SupportCompData *pObj,ParaBase* pPara);
};

#endif // !defined(AFX_IPAHSEL_H__B0BBD8E9_8972_41D2_83AC_DD3B3BBD8DD9__INCLUDED_)
