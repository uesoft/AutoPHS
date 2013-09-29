// PictureClipData.cpp: implementation of the PictureClipData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PictureClipData.h"
#include "EDIBgbl.h"
#include "user.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CString PictureClipData::m_strTableName = _T("PictureClipData");
CString PictureClipData::m_strCustomIDField = _T("CustomID");
CString PictureClipData::m_strBHFormatField = _T("BHFormat");	//×Ö¶ÎÃû
CString PictureClipData::m_strDescriptionField = _T("Description"); //ÃèÊö
CString PictureClipData::m_strIDField = _T("ID");
PictureClipData::PictureClipData()
{
	m_pRs.m_pDatabase = &EDIBgbl::dbPRJ;
}

PictureClipData::~PictureClipData()
{

}

CString PictureClipData::GetBHFormat()
{
	return vtos( m_pRs.GetFieldValue(m_strBHFormatField) );
}

CString PictureClipData::GetDescription()
{
	return vtos( m_pRs.GetFieldValue(m_strDescriptionField) );
}
CString PictureClipData::GetID()
{
	return vtos( m_pRs.GetFieldValue(m_strIDField) );
}


