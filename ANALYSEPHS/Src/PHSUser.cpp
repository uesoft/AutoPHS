// PHSUser.cpp: implementation of the PHSUser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\INC\PHSUser.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PHSUser::PHSUser()
{

}

PHSUser::~PHSUser()
{

}

bool  vtob(	VARIANT &v)
{
	switch(v.vt)
	{
	case VT_BOOL:
		return (bool)V_BOOL(&v);
	case VT_I2:
		return (bool)V_I2(&v);
	case VT_I4:
		return (bool)V_I4(&v);
	case VT_R4:
		return (bool)V_R4(&v);
	case VT_R8:
		return (bool)V_R8(&v);
	}
	return false;
}

float vtof(_variant_t &v)
{
	float ret=0;
	CString tmps;
	switch(v.vt)
	{
	case VT_R4:
		ret = V_R4(&v);
		break;
	case VT_R8:
		ret=(float)V_R8(&v);
		break;
	case VT_I2:
		ret= (float)V_I2(&v);
		break;
	case VT_I4:
		ret= (float)V_I4(&v);
		break;
	case VT_BSTR:
		tmps=(char*)_bstr_t(&v);
		ret=atof((LPCSTR)tmps);
		break;

	}
	return ret;
}
float vtof(COleVariant &v)
{
	float ret=0;
	CString tmps;
	switch(v.vt)
	{
	case VT_R4:
		ret = V_R4(&v);
		break;
	case VT_R8:
		ret=(float)V_R8(&v);
		break;
	case VT_I2:
		ret= (float)V_I2(&v);
		break;
	case VT_I4:
		ret= (float)V_I4(&v);
		break;
	case VT_BSTR:
		tmps=V_BSTRT(&v);
		ret=atof((LPCSTR)tmps);
		break;

	}
	return ret;
}

CString  ftos(float v)
{
	CString s;
	s.Format("%g",v);
	return s;
}

CString vtos(_variant_t& v)
{
	CString ret;
	switch(v.vt)
	{
	case VT_NULL:
	case VT_EMPTY:
		ret="";
		break;
	case VT_BSTR:
		ret=V_BSTR(&v);
		break;
	case VT_R4:
		ret.Format("%g", (double)V_R4(&v));
		break;
	case VT_R8:
		ret.Format("%g",V_R8(&v));
		break;
	case VT_I2:
		ret.Format("%d",(int)V_I2(&v));
		break;
	case VT_I4:
		ret.Format("%d",(int)V_I4(&v));
		break;
	case VT_BOOL:
		ret=(V_BOOL(&v) ? "True" : "False");
		break;
	}
	ret.TrimLeft();ret.TrimRight();
	return ret;
}
CString vtos(COleVariant& v)
{
	CString ret;
	switch(v.vt)
	{
	case VT_NULL:
	case VT_EMPTY:
		ret="";
		break;
	case VT_BSTR:
		ret=V_BSTRT(&v);
		break;
	case VT_R4:
		ret.Format("%g", (double)V_R4(&v));
		break;
	case VT_R8:
		ret.Format("%g",V_R8(&v));
		break;
	case VT_I2:
		ret.Format("%d",(int)V_I2(&v));
		break;
	case VT_I4:
		ret.Format("%d",(int)V_I4(&v));
		break;
	case VT_BOOL:
		ret=(V_BOOL(&v) ? "True" : "False");
		break;
	}
	ret.TrimLeft();ret.TrimRight();
	return ret;
}
