// PCLASel.h: interface for the CPCLASel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PCLASEL_H__43D4256E_5AD7_4FED_83DE_37C5B87AAA9D__INCLUDED_)
#define AFX_PCLASEL_H__43D4256E_5AD7_4FED_83DE_37C5B87AAA9D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CompSel.h"
#include "SelParameter.h"

class PipePara;
class PCLASel : public CompSel  
{
public:
	PCLASel();
	virtual ~PCLASel();
public:
	virtual void GetSearchComp(SupportCompData *pObj);

protected:
	PipePara* m_pPipePara; //管道道参，有温度，管径
	double m_dbtmpSelPJG; //选型时用的荷载值
	int m_iNumPart; //部件的数量
	double m_dbDWdelta;//查找管径范围扩大,0~7(%)
	CString m_strCustomID; //对应Ptype(i)
};

#endif // !defined(AFX_PCLASEL_H__43D4256E_5AD7_4FED_83DE_37C5B87AAA9D__INCLUDED_)
