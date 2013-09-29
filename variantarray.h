// VariantArray.h: interface for the CVariantArray class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VARIANTARRAY_H__503BB086_8CC6_11D5_AE95_00D00961973B__INCLUDED_)
#define AFX_VARIANTARRAY_H__503BB086_8CC6_11D5_AE95_00D00961973B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CVariantArray : public COleSafeArray  
{
public:
	void SetAt(int index,double dVar);
	void SetData(long Count,...);
	CVariantArray(int size=0);
	virtual ~CVariantArray();
	double operator [](int inex);
	CVariantArray & operator =(_variant_t &newVar);
private:
//	double* m_vData;
//	int m_size;
};

#endif // !defined(AFX_VARIANTARRAY_H__503BB086_8CC6_11D5_AE95_00D00961973B__INCLUDED_)
