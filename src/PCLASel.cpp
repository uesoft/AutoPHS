// PCLASel.cpp: implementation of the CPCLASel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PCLASel.h"
#include "CrudePA.h"
#include "modPHScal.h"
#include "SelParameter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PCLASel::PCLASel():
m_pPipePara(NULL),m_dbtmpSelPJG(0.0)
{

}

PCLASel::~PCLASel()
{

}
/*
功能:从数据库中查找合适的部件，适用于(iPAXZ1_XZ3、iPAh)(iPAs、iPASS、iPAGS的第一步选择)
*/
void PCLASel::GetSearchComp(SupportCompData *pObj)
{
	CrudePA *pCrudePA = static_cast<CrudePA *> (pObj);

	CString strSql,strTemp;
	for (int k = 0 ;k<= (int)m_dbDWdelta;k++)
	{
		double dbDw = m_pPipePara->GetDj() * (1 - k * 0.01);
		double dbDw1 = m_pPipePara->GetDj() * (1 + k * 0.01);
		//直径条件
		strSql.Format(_T("SELECT * FROM [%s]  WHERE %s >= %g AND [%s]<= %g"),
			pCrudePA->GetTableName(),pCrudePA->GetDWField(),dbDw,
			pCrudePA->GetDWField(),dbDw1);
		
		strTemp.Format(_T("AND %s >= %g"),pCrudePA->GetPmaxField(),(float)(m_dbtmpSelPJG/m_iNumPart));
		strSql += strTemp;
		
		//温度类型
		strTemp.Format( _T(" AND %s>=%g AND trim(%s)=\'%s\'"),
			pCrudePA->GetTJField(),m_pPipePara->GetT0(),
			pCrudePA->GetCustomIDField(),m_strCustomID );
		strSql += strTemp;

		//排序
		strTemp.Format(_T("ORDER BY %s,%s,%s,%s"),
			pCrudePA->GetTJField(),pCrudePA->GetDWField(),
			pCrudePA->GetPmaxField(),pCrudePA->GetWeightField() );

		strSql += strTemp;
		pCrudePA->Initialize(strSql);
		
	}
}