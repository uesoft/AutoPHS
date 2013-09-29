// ConstantSpring.cpp: implementation of the ConstantSpring class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ConstantSpring.h"
#include <math.h>
#include "modphscal.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ConstantSpring::ConstantSpring()
{

}

ConstantSpring::~ConstantSpring()
{

}

/*
根据原始荷载得到用来选型的荷载
*/
double ConstantSpring::GetPGZ(double dbPgz,CString sPartID,int FirstCal)
{
	double TmpPgz = 0;
	if ( sPartID == _T("LHG") && FirstCal >=2 )
	{
		TmpPgz = (fabs(dbPgz)+modPHScal::g_fLHGWeight) / modPHScal::gintParallelNum;
	}
	else
	{
		TmpPgz = fabs(dbPgz) / modPHScal::gintParallelNum;
	}
	return TmpPgz;
}

//根据原始位移得到用来选型的位移
double ConstantSpring::Getyr(double dbyr)
{
	double Tmpyr;
	Tmpyr = fabs(dbyr);         //yr 单位:mm
	if( Tmpyr * modPHScal::gUseConstantSpringPercentSurplusDist < modPHScal::gUseConstantSpringMinSurplusDist )
	{
		Tmpyr =Tmpyr + modPHScal::gUseConstantSpringMinSurplusDist;
	}
	else
	{
		Tmpyr = Tmpyr * (1 + modPHScal::gUseConstantSpringPercentSurplusDist);
	}
	return Tmpyr;
}



/*得到选择拉杆的SQL语句
strPrtID:ID号;iSEQofSPR:??;fWeight:当前恒力弹簧重量; tmpSelPJG4CSPRFiJ:根据原始数据计算出的荷载
strCustomID:当前恒力弹簧号;iFjmin:弹簧螺栓孔直径
*/
CString ConstantSpring::GetSelectLugSql(CString strPrtID,int iSEQofSPR,float tmpSelPJG4CSPRFiJ,float fWeight,CString strCustomID,int iFjmin)
{
	//其它恒力弹簧，循环2次才能找出最大拉杆直径
	CString sTmp,SQLx;
	
	if(modPHScal::gbByForce) //是否用荷载选拉杆
	{
		sTmp.Format(_T(" PmaxKgf>=%g "),
			(strPrtID == _T("LHG") ? tmpSelPJG4CSPRFiJ +fWeight * modPHScal::gintParallelNum : tmpSelPJG4CSPRFiJ) / modPHScal::gintParallelNum);
	}
	else
	{
		sTmp.Format(_T(" fBmin<=%g AND fBmax>=%g"),
			modPHScal::sSprInfo[iSEQofSPR].CheckDisp,
			modPHScal::sSprInfo[iSEQofSPR].CheckDisp);
	}
	SQLx.Format( _T("SELECT * FROM [%s] WHERE %s AND FiJ>=%d AND minDH<=%d AND maxDH>=%d  AND trim(CustomID)=\'%s\' %s"),
		modPHScal::tbnLugDiaOfCSPR,					
		sTmp,
		(iFjmin <= 0 ? 10 : iFjmin),
		modPHScal::sSprInfo[iSEQofSPR].DH,
		modPHScal::sSprInfo[iSEQofSPR].DH,
		strCustomID,
		(modPHScal::gbByForce ? _T(" ORDER BY Pmax,FiJ") : _T(" ORDER BY FiJ")));
	
	return SQLx;
}

/*
第一次计算时，之前的所有拉杆直径约束值=当前恒吊的松紧螺母直径
iFjg:当前恒吊的松紧螺母直径;iSEQofSPR:当前弹簧的索引号(如第一个:0;第二个:1) iFirstCal:当前计算的次数
*/
void ConstantSpring::SetAforeCompDia(int iFiJ,int iSEQofSPR,int iFirstCal)
{
	int k=0;
	if( iSEQofSPR + 1 > modPHScal::giWholeSprNum )
	{
		return ;
	}
	if( iFirstCal > 1 )
	{
		//第一次计算，之前的所有拉杆直径约束值=当前恒吊的松紧螺母直径
		//第二次计算，根据拉杆直径选择恒吊的松紧螺母直径，这时拉杆直径必然满足了恒吊松紧螺母的最小直径。
		return ;
	}
	if( iSEQofSPR > 0 )
	{
		k = modPHScal::giCSPRindex[iSEQofSPR - 1] + 1;
	}
	for(;k<= modPHScal::giCSPRindex[iSEQofSPR];k++)
	{
		modPHScal::PtypeDiameter[k] = iFiJ;
		if(modPHScal::gbRodDiaByMax) //拉杆直径最小限制
		{
			if(modPHScal::iMaxRodDia>modPHScal::PtypeDiameter[k])
				modPHScal::PtypeDiameter[k]=modPHScal::iMaxRodDia;
			else
				modPHScal::iMaxRodDia=modPHScal::PtypeDiameter[k];
		}
	}
}
