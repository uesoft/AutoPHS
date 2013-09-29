// InputZA.cpp: implementation of the InputZA class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "autophs.h"
#include "InputZA.h"
#include "user.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CString m_ifLongVertPipe=_T("ifLongVertPipe");

InputZA::InputZA()
{

}

InputZA::~InputZA()
{

}
bool InputZA::GetifLongVertPipe()
{
	return	vtob(m_pRs.GetFieldValue(m_ifLongVertPipe));
}
