// CompSel.h: interface for the CCompSel class.
//
//////////////////////////////////////////////////////////////////////
#include "SupportCompData.h"
#if !defined(AFX_COMPSEL_H__AFA063F0_94BD_4FAA_9BF2_32993B30F789__INCLUDED_)
#define AFX_COMPSEL_H__AFA063F0_94BD_4FAA_9BF2_32993B30F789__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CompSel  
{
public:
	CompSel();
	virtual ~CompSel();
protected :
	virtual void GetSearchComp( SupportCompData *pObj) = 0;

};

#endif // !defined(AFX_COMPSEL_H__AFA063F0_94BD_4FAA_9BF2_32993B30F789__INCLUDED_)
