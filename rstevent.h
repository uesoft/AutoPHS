// RstEvent.h: interface for the CRstEvent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RSTEVENT_H__5FA3FC81_035E_11D6_BBA7_00D009619765__INCLUDED_)
#define AFX_RSTEVENT_H__5FA3FC81_035E_11D6_BBA7_00D009619765__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "DEBUG\msado15.tlh"

class CRstEvent : public RecordsetEventsVt  
{
public:
	STDMETHODIMP/*inline HRESULT*/ MoveComplete ( enum EventReasonEnum adReason, struct Error * pError, enum EventStatusEnum * adStatus, struct _Recordset * pRecordset );
	STDMETHODIMP/*inline HRESULT*/ WillMove ( enum EventReasonEnum adReason, enum EventStatusEnum * adStatus, struct _Recordset * pRecordset );
};

#endif // !defined(AFX_RSTEVENT_H__5FA3FC81_035E_11D6_BBA7_00D009619765__INCLUDED_)
