// PHSUser.h: interface for the PHSUser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PHSUSER_H__FBB75B94_127C_46C9_93E0_103838C63EC7__INCLUDED_)
#define AFX_PHSUSER_H__FBB75B94_127C_46C9_93E0_103838C63EC7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class PHSUser  
{
public:
	PHSUser();
	virtual ~PHSUser();

};
bool  vtob(	VARIANT &v);
float vtof(_variant_t &v);
float vtof(COleVariant &v);
CString  ftos(float v);
CString vtos(_variant_t& v);
CString vtos(COleVariant& v);

#endif // !defined(AFX_PHSUSER_H__FBB75B94_127C_46C9_93E0_103838C63EC7__INCLUDED_)
