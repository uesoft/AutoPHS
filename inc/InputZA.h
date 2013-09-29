// InputZA.h: interface for the InputZA class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPUTZA_H__A8A80A74_308B_4ACE_B3CF_E9D15FA3DFC5__INCLUDED_)
#define AFX_INPUTZA_H__A8A80A74_308B_4ACE_B3CF_E9D15FA3DFC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DataTableBase.h"

class InputZA : public DataTableBase 
{
public:
	InputZA();
	virtual ~InputZA();
	bool GetifLongVertPipe();

	const CString& GetifLongVertPipeField()
	{
		return m_ifLongVertPipe;
	}

protected:
	static CString m_ifLongVertPipe;//ifLongVertPipe,Cphs.cpp(3480)
};

#endif // !defined(AFX_INPUTZA_H__A8A80A74_308B_4ACE_B3CF_E9D15FA3DFC5__INCLUDED_)
