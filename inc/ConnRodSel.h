// ConnRodSel.h: interface for the ConnRodSel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONNRODSEL_H__F224A86A_A6D9_49BA_B0A2_DAF7BAEBBECF__INCLUDED_)
#define AFX_CONNRODSEL_H__F224A86A_A6D9_49BA_B0A2_DAF7BAEBBECF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "HELESel.h"

class ConnRodSel : public HELESel
{
public:
	ConnRodSel();
	virtual ~ConnRodSel();
private:
	virtual void GetSearchComp( SupportCompData *pObj);

};

#endif // !defined(AFX_CONNRODSEL_H__F224A86A_A6D9_49BA_B0A2_DAF7BAEBBECF__INCLUDED_)
