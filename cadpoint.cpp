// CadPoint.cpp: implementation of the CCadPoint class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AutoPHS.h"
#include "CadPoint.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCadPoint::CCadPoint()
{
	DWORD Em[1];
	Em[0]=3;
	this->Create(VT_R8,1,(unsigned long*)&Em);
	long ix;
	double data[3];
	data[0]=0;data[1]=0;data[2]=0;
	for(ix=0;ix<3;ix++)
		this->PutElement(&ix,&data[ix]);
}

CCadPoint::~CCadPoint()
{
	//this->DestroyData();
	//this->Destroy();
}

CCadPoint::CCadPoint(float x, float y, float z)
{
	DWORD Em[1];
	Em[0]=3;
	this->Create(VT_R8,1,(unsigned long*)&Em);
	long ix;
	double data[3];
	data[0]=(double)x;data[1]=(double)y;data[2]=(double)z;
	for(ix=0;ix<3;ix++)
		this->PutElement(&ix,&data[ix]);
	    

}

void CCadPoint::SetPoint(float x, float y, float z)
{
	long ix;
	double data[3];
	data[0]=(double)x;data[1]=(double)y;data[2]=(double)z;
	for(ix=0;ix<3;ix++)
	{
		if(data[ix]<-100000 || data[ix]>100000)
			data[ix]=0.0;
		this->PutElement(&ix,&data[ix]);
	}
}

double CCadPoint::GetX()
{
	long ix=0;
	double ret;
	this->GetElement(&ix,&ret);
	return ret;
}

double CCadPoint::GetY()
{
	long ix=1;
	double ret;
	this->GetElement(&ix,&ret);
	return ret;
	
}

double CCadPoint::GetZ()
{
	long ix=2;
	double ret;
	this->GetElement(&ix,&ret);
	return ret;

}

CCadPoint& CCadPoint::operator =(CCadPoint &newVar)
{
	SetPoint(newVar.GetX(),newVar.GetY(),newVar.GetZ());
	return *this;
}
//DEL _variant_t& CCadPoint::operator =(_variant_t & newVar)
//DEL {
//DEL 	this->
//DEL }

double CCadPoint::operator [](int inex)
{
	double ret=0;
	long ix=inex;
	if(inex>=0 && inex<3)
		this->GetElement(&ix,&ret);
	return ret;
}

void CCadPoint::SetX(float v)
{
	long ix=0;
	double data=(double)v;
	if(data<-100000 || data>100000)
		data=0.0;
	this->PutElement(&ix,&data);
}

void CCadPoint::SetY(float v)
{
	long ix=1;
	double data=(double)v;
	if(data<-100000 || data>100000)
		data=0.0;
	this->PutElement(&ix,&data);
}

void CCadPoint::SetZ(float v)
{
	long ix=2;
	double data=(double)v;
	if(data<-100000 || data>100000)
		data=0.0;
	this->PutElement(&ix,&data);
}

CCadPoint& CCadPoint::operator =(_variant_t &newVar)
{
	COleSafeArray tmparr(newVar.parray,VT_R8);
	COleSafeArray::operator =(tmparr);
	return *this;
}

CCadPoint::CCadPoint(double x, double y, double z)
{
	DWORD Em[1];
	Em[0]=3;
	this->Create(VT_R8,1,(unsigned long*)&Em);
	long ix;
	double data[3];
	data[0]=x;data[1]=y;data[2]=z;
	for(ix=0;ix<3;ix++)
		this->PutElement(&ix,&data[ix]);

}

void CCadPoint::SetPoint(double x, double y, double z)
{
	long ix;
	double data[3];
	data[0]=x;data[1]=y;data[2]=z;
	for(ix=0;ix<3;ix++)
	{
		if(data[ix]<-100000 || data[ix]>100000)
			data[ix]=0.0;
		this->PutElement(&ix,&data[ix]);
	}
}

void CCadPoint::SetX(double v)
{
	long ix=0;
	double data=v;
	if(data<-100000 || data>100000)
		data=0.0;
	this->PutElement(&ix,&data);
}

void CCadPoint::SetY(double v)
{
	long ix=1;
	double data=v;
	if(data<-100000 || data>100000)
		data=0.0;
	this->PutElement(&ix,&data);
}

void CCadPoint::SetZ(double v)
{
	long ix=2;
	double data=v;
	if(data<-100000 || data>100000)
		data=0.0;
	this->PutElement(&ix,&data);
//	_ConnectionPtr ptr;
	
}
