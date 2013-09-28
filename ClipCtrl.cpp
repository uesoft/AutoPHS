// ClipCtrl.cpp: implementation of the CClipCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ClipCtrl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CClipCtrl::CClipCtrl()
{
	m_bClipFlag = false;
    m_nRow = -1; 
	m_nCol   = -1;


}

CClipCtrl::~CClipCtrl()
{
}

void CClipCtrl::Clear()
{
	m_bClipFlag = false;
    m_nRow      = -1; 
	m_nCol      = -1;
	
    while(m_vRecord.size ())
	{
		switch(m_vRecord[m_vRecord.size() - 1].vt)
		{
		case VT_EMPTY:
			m_vRecord.pop_back ();
			break;
		case VT_NULL:
			m_vRecord.pop_back ();
			break;
		case VT_BSTR:
			free(m_vRecord[m_vRecord.size() - 1].bstrVal);
		    m_vRecord.pop_back ();
			break;
		case VT_R4:
			m_vRecord.pop_back();
			break;
		default:
			m_vRecord.pop_back ();
			break;
		}
	}
	
	while(m_nSelectedCol.size ())
	{
		m_nSelectedCol.pop_back ();
	}
}

VARIANT CClipCtrl::GetRecord(int nRow, int nCol)
{
	return(m_vRecord[(nRow - 1) * m_nCol + nCol -1 ]); 
}

void CClipCtrl::Initial(int nRow, int nCol)
{
	int i;

	Clear();
    
	m_nRow = nRow; 
	m_nCol = nCol;
	
	for(i = 0; i < nRow * nCol; i ++)
	{
		VARIANT vRecord;
		vRecord.vt = VT_NULL;
		m_vRecord.push_back (vRecord);
	}
}

void CClipCtrl::SetClipFlag(bool bFlag)
{
	m_bClipFlag = bFlag;
}

void CClipCtrl::SetRecord(int nRow, int nCol, VARIANT vRecord)
{	
	switch(vRecord.vt)
    {
	case VT_EMPTY:
		break;
	case VT_BSTR: 
		m_vRecord[(nRow - 1) * m_nCol + nCol - 1].vt = VT_BSTR;
		wchar_t *pstr;
		pstr = (wchar_t *)malloc(sizeof(wchar_t) * 512);
        m_vRecord[(nRow - 1) * m_nCol + nCol - 1].bstrVal = pstr;
		wcscpy(m_vRecord[(nRow - 1) * m_nCol + nCol - 1].bstrVal, vRecord.bstrVal );
		break;
    case VT_I2:
		m_vRecord[(nRow - 1) * m_nCol + nCol - 1] = vRecord;
		break;
	case VT_R4:
		m_vRecord[(nRow - 1) * m_nCol + nCol - 1].vt = vRecord.vt;
		m_vRecord[(nRow - 1) * m_nCol + nCol - 1].fltVal = vRecord.fltVal;
		break;
	case VT_NULL:
		m_vRecord[(nRow - 1) * m_nCol + nCol - 1] = vRecord;
		break;
	case VT_BOOL:
        m_vRecord[(nRow - 1) * m_nCol + nCol - 1].vt = vRecord.vt;
		m_vRecord[(nRow - 1) * m_nCol + nCol - 1].boolVal = vRecord.boolVal;
		break;
	case VT_I4:
		m_vRecord[(nRow - 1) * m_nCol + nCol - 1].vt = vRecord.vt;
		m_vRecord[(nRow - 1) * m_nCol + nCol - 1].lVal = vRecord.lVal;
        break;

	default:
		break;
	}
}

