// VariantArray.cpp: implementation of the CVariantArray class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "autophs.h"
#include "VariantArray.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVariantArray::CVariantArray(int size)
{
	if(size==0)
		return;
	DWORD Em[1];
	Em[0]=size;
	this->Create(VT_R8,1,(unsigned long*)Em);
	long ix;
	double data=0;
	for(ix=0;ix<size;ix++)
		this->PutElement(&ix,&data);
}

CVariantArray::~CVariantArray()
{
	//this->DestroyData();
	//this->Destroy();
}

//DEL double * CVariantArray::m_ix()
//DEL {
//DEL 
//DEL }

void CVariantArray::SetData(long Count, ...)
{
	va_list argList;
	va_start(argList,Count);
	long i;
	double data;
	long ub;
	long c=0;
	this->GetLBound(1,&i);
	this->GetUBound(1,&ub);
	for(;i<=ub && c<Count;i++)
	{
		data=va_arg(argList,double);
		if(data<-100000 || data>100000)
			data=0.0;
		this->PutElement(&i,&data);
		c++;
	}
	va_end(argList);
}

double CVariantArray::operator [](int inex)
{
	double ret;
	long ix=inex;
	this->GetElement(&ix,&ret);
	return ret;
}

void CVariantArray::SetAt(int index,double dVar)
{
	long ix=index;
	double data=dVar;
	if(data<-100000 || data>100000)
		data=0.0;
	long lb,ub;
	this->GetLBound(1,&lb);
	this->GetUBound(1,&ub);
	if(ix<lb || ix>ub)
		return;
	this->PutElement(&ix,&data);
}

CVariantArray& CVariantArray::operator =(_variant_t &newVar)
{
	COleSafeArray tmparr(newVar.parray,VT_R8);
	COleSafeArray::operator =(tmparr);
	return *this;
}
