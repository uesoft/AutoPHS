// crudeSA.h: interface for the crudeSA class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRUDESA_H__AE0E5B53_FACD_40C5_A16E_5B4B72F14E37__INCLUDED_)
#define AFX_CRUDESA_H__AE0E5B53_FACD_40C5_A16E_5B4B72F14E37__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class crudeSA  
{
public:
	crudeSA();
	virtual ~crudeSA();
public:
	
const CString& GetCustomIDField()
	{
		return m_CustomID;
	}

const CString& GetGDW1Field()
	{
		return m_GDW1;
	}

const CString& GettbnSATableName()
	{
		return m_tbnSA;
	}

private:
	static CString m_CustomID;//Cphs(3466)CustomID
	static CString m_GDW1;//Cphs(3466)GDW1
	static CString m_tbnSA;//Cphs(3467)modPHScal::tbnSA
};

#endif // !defined(AFX_CRUDESA_H__AE0E5B53_FACD_40C5_A16E_5B4B72F14E37__INCLUDED_)
