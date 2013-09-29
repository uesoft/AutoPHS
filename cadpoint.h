// CadPoint.h: interface for the CCadPoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CADPOINT_H__A8DD17C4_88CE_11D5_874F_FF2DA9F5B235__INCLUDED_)
#define AFX_CADPOINT_H__A8DD17C4_88CE_11D5_874F_FF2DA9F5B235__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCadPoint : public COleSafeArray  
{
public:
	CCadPoint();
	CCadPoint(double x,double y, double z=0.0);
	CCadPoint(float x,float y, float z=0.0);
	virtual ~CCadPoint();

public:
	void SetZ(double v);
	void SetY(double v);
	void SetX(double v);
	void SetZ(float v);
	void SetY(float v);
	void SetX(float v);
	void SetPoint(double x,double  y, double z=0.0);
	void SetPoint(float x,float y, float z=0.0);
	double GetZ();
	double GetY();
	double GetX();

	CCadPoint& operator =(_variant_t &newVar);
	CCadPoint& operator =(CCadPoint &newVar);
	double operator [](int inex);
};

#endif // !defined(AFX_CADPOINT_H__A8DD17C4_88CE_11D5_874F_FF2DA9F5B235__INCLUDED_)
