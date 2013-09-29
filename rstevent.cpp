// RstEvent.cpp: implementation of the CRstEvent class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "autophs.h"
#include "RstEvent.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*CRstEvent::CRstEvent()
{

}

CRstEvent::~CRstEvent()
{

}*/

STDMETHODIMP/*inline HRESULT*/ CRstEvent::MoveComplete ( enum EventReasonEnum adReason, struct Error * pError, enum EventStatusEnum * adStatus, struct _Recordset * pRecordset )
{
	return S_OK;
}
STDMETHODIMP/*inline HRESULT*/CRstEvent::WillMove ( enum EventReasonEnum adReason, enum EventStatusEnum * adStatus, struct _Recordset * pRecordset )
{
	return S_OK;
}
