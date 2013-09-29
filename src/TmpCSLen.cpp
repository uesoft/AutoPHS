// TmpCSLen.cpp: implementation of the TmpCSLen class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TmpCSLen.h"
#include "EDIBgbl.h"

CString TmpCSLen::m_strTableName = _T("TmpCSLen");
CString TmpCSLen::m_strBHField = _T("BH");
CString TmpCSLen::m_strLugDiameterField = _T("LugDiameter"); //
CString TmpCSLen::m_strSumDistField = _T("SumDist");
CString TmpCSLen::m_strSizeCField = _T("sizeC");
CString TmpCSLen::m_strHeatDistField = _T("HeatDist");;
CString TmpCSLen::m_strUnitForceField = _T("UnitForce");;
CString TmpCSLen::m_strDistFXField = _T("DistFX");;
CString TmpCSLen::m_strSelOpLoadField = _T("SelOpLoad");;
CString TmpCSLen::m_strOpLoadField = _T("OpLoad");;
CString TmpCSLen::m_strInstallLoadField = _T("InstallLoad");;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TmpCSLen::TmpCSLen()
{
	m_pRs.m_pDatabase = &EDIBgbl::dbPRJ;
}

TmpCSLen::~TmpCSLen()
{
	
}

void TmpCSLen::SetAllBH(const CString& strBH)
{
	CString strSql;
	strSql.Format( _T("UPDATE %s SET %s=\'%s\'"),m_strTableName,m_strBHField,strBH );
	EDIBgbl::dbPRJ.Execute(strSql);
}
void TmpCSLen::SetAllLugDiameter(int iDia)
{
	CString strSql;
	strSql.Format( _T("UPDATE %s SET %s=%d"),m_strTableName,m_strLugDiameterField,iDia );
	EDIBgbl::dbPRJ.Execute(strSql);
}
void TmpCSLen::SetAllSumDist(double dbValue)
{
	CString strSql;
	strSql.Format( _T("UPDATE %s SET %s=%g"),m_strTableName,m_strSumDistField,dbValue );
	EDIBgbl::dbPRJ.Execute(strSql);
}
void TmpCSLen::SetAllSizeC(double dbValue)
{
	CString strSql;
	strSql.Format( _T("UPDATE %s SET %s=%g"),m_strTableName,m_strSizeCField,dbValue );
	EDIBgbl::dbPRJ.Execute(strSql);
}
void TmpCSLen::SetAllHeatDist(double dbValue)
{
	CString strSql;
	strSql.Format( _T("UPDATE %s SET %s=%g"),m_strTableName,m_strHeatDistField,dbValue );
	EDIBgbl::dbPRJ.Execute(strSql);
}
void TmpCSLen::SetAllUnitForce(const CString& strValue)
{
	CString strSql;
	strSql.Format( _T("UPDATE %s SET %s=\'%s\'"),m_strTableName,m_strUnitForceField,strValue );
	EDIBgbl::dbPRJ.Execute(strSql);
}
void TmpCSLen::SetAllDistFX(const CString& strValue)
{
	CString strSql;
	strSql.Format( _T("UPDATE %s SET %s=\'%s\'"),m_strTableName,m_strDistFXField,strValue );
	EDIBgbl::dbPRJ.Execute(strSql);
}
void TmpCSLen::SetAllSelOpLoad(double dbValue)
{
	CString strSql;
	strSql.Format( _T("UPDATE %s SET %s=%g"),m_strTableName,m_strSelOpLoadField,dbValue );
	EDIBgbl::dbPRJ.Execute(strSql);
}
void TmpCSLen::SetAllOpLoad(double dbValue)
{
	CString strSql;
	strSql.Format( _T("UPDATE %s SET %s=%g"),m_strTableName,m_strOpLoadField,dbValue );
	EDIBgbl::dbPRJ.Execute(strSql);
}
void TmpCSLen::SetAllInstallLoad(double dbValue)
{
	CString strSql;
	strSql.Format( _T("UPDATE %s SET %s=%g"),m_strTableName,m_strInstallLoadField,dbValue );
	EDIBgbl::dbPRJ.Execute(strSql);
}
