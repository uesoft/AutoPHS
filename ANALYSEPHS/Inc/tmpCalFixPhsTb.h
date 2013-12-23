// tmpCalFixPhsTb.h: interface for the tmpCalFixPhsTb class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TMPCALFIXPHSTB_H__2C9D5403_1D8C_402C_9A37_23F28B8A606A__INCLUDED_)
#define AFX_TMPCALFIXPHSTB_H__2C9D5403_1D8C_402C_9A37_23F28B8A606A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//Cphs.cpp(3651,4390)
//SQLx = _T("SELECT * FROM tmpCalFixPhs");
class tmpCalFixPhsTb  
{
public:
	tmpCalFixPhsTb();
	virtual ~tmpCalFixPhsTb();
public:
	BOOL SetData( CDaoRecordset* pRs,CDaoRecordset* pOtherRs );


};

#endif // !defined(AFX_TMPCALFIXPHSTB_H__2C9D5403_1D8C_402C_9A37_23F28B8A606A__INCLUDED_)
