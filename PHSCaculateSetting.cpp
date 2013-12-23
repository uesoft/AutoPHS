// PHSCaculateSetting.cpp: implementation of the PHSCaculateSetting class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PHSCaculateSetting.h"
#include <afxdao.h>
#include "modPHScal.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
 
#pragma warning (disable:4800)

extern void		SetRegValue(LPCTSTR pszKey, LPCTSTR pszName,const int vValue);
extern void		SetRegValue(LPCTSTR pszKey, LPCTSTR pszName,const CString vValue);
extern void		SetRegValue(LPCTSTR pszKey, LPCTSTR pszName,const float vValue);
extern void		SetRegValue(LPCTSTR pszKey, LPCTSTR pszName,const bool vValue);
extern void		SetRegValue(LPCTSTR pszKey, LPCTSTR pszName, const double vValue);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PHSCaculateSetting theCaculateSetting;

PHSCaculateSetting& GetCurCaculateSetting()
{
	return theCaculateSetting;
}

PHSCaculateSetting::PHSCaculateSetting()
{
	this->m_dbRubCoefficient = 0.0;
}

PHSCaculateSetting::~PHSCaculateSetting()
{
//	this->SaveSetting();
}

BOOL PHSCaculateSetting::InitSetting()
{
	return TRUE;
}

BOOL PHSCaculateSetting::SaveSetting()
{
    SetRegValue(_T("Settings"),	_T("CalcCSPRHeight_BySelDisp"),modPHScal::gbCalcCSPRHeight_BySelDisp);
	SetRegValue(_T("Settings"), _T("MinRodLen"), modPHScal::giMinRodLen);
	SetRegValue(_T("Settings"), _T("UseSpringMinDist"), modPHScal::gUseSpringMinDist);
	SetRegValue(_T("Settings"), _T("SPR_Chzbh"), modPHScal::gnSPR_CHZBH);
	SetRegValue(_T("Settings"), _T("Pjg2Pgz"), modPHScal::gsngRatioOfPjg2Pgz);
	SetRegValue(_T("Settings"), _T("ConnectTJ"), modPHScal::gnConnectTJ);
	SetRegValue(_T("Settings"), _T("RodDiaByMax"), modPHScal::gbRodDiaByMax);
	SetRegValue(_T("Settings"), _T("PipeClampBolt"), modPHScal::gbPipeClampBolt);
	SetRegValue(_T("Settings"), _T("RodLenRoundVal"), modPHScal::gnRodLenRoundVal);
	SetRegValue(_T("Settings"), _T("RodInsertLenRoundVal"), modPHScal::gnRodInsertLenRoundVal);
	SetRegValue(_T("Settings"), _T("ConnectHoleDimMatch"), modPHScal::gbConnectHoleDimMatch);
	SetRegValue(_T("Settings"), _T("SPRAutoLugDia"), modPHScal::gbSPRAutoLugDia);
	SetRegValue(_T("Settings"), _T("AutoApplyChzbh"), modPHScal::gbAutoApplyChzbh);
	SetRegValue(_T("Settings"), _T("AutoApplyCSPRLugDia"), modPHScal::gbAutoApplyCSPRLugDia);
	SetRegValue(_T("Settings"), _T("OutputSA"), modPHScal::gbOutputSA);
	SetRegValue(_T("Settings"), _T("AutoPASA"), modPHScal::gbAutoPASA);
	SetRegValue(_T("Settings"),	_T("HintRepZdjType"),modPHScal::bHintRepZdjType);
	SetRegValue(_T("Settings"),	_T("NumCSVal"),modPHScal::iNumCSVal);
	SetRegValue(_T("Settings"),	_T("AutoSPRRodDiaDecrease"),modPHScal::gbAutoSPRRodDiaDecrease);
	SetRegValue(_T("Settings"),	_T("SPRBHLOADBYInstallLoad"),modPHScal::gbSPRBHLOADBYInstallLoad);
	SetRegValue(_T("Settings"),	_T("SPRBHLOADUNITBYNEWTON"),modPHScal::gbSPRBHLOADUNITBYNEWTON);
	SetRegValue(_T("Settings"),	_T("HotStatus2Zero"),gbHotStatus2Zero);
	SetRegValue(_T("Settings"), _T("SumRodByRoundSteel"), modPHScal::gbSumRodByRoundSteel);
	SetRegValue(_T("Settings"), _T("DHT"), modPHScal::g_bDHT);
	SetRegValue(_T("Settings"), _T("DHTGG"), modPHScal::g_strDHTGG);
	SetRegValue(_T("Settings"), _T("DW_delta"), modPHScal::gnDW_delta);
	SetRegValue(_T("Settings"), _T("Friction-free"), this->m_dbRubCoefficient);
	SetRegValue(_T("Settings"), _T("LimitLugMinDia"), modPHScal::gbLimitLugMinDia);
	SetRegValue(_T("Settings"), _T("CSPRCanSpecialDesign"), modPHScal::gbCSPRCanSpecialDesign);
	SetRegValue(_T("Settings"), _T("AutoAlignCSPRtoSA"), modPHScal::gbAutoAlignCSPRtoSA);
	SetRegValue(_T("Settings"), _T("CalCSPRLugDiaByPjg"), modPHScal::gbCalCSPRLugDiaByPjg);
	SetRegValue(_T("Settings"), _T("CalSAbyConservativenessMethod"), modPHScal::gbCalSAbyConservativenessMethod);
	SetRegValue(_T("Settings"), _T("CalSAbyPJG"), modPHScal::gbCalSAbyPJG);
	SetRegValue(_T("Settings"), _T("AddPartWeight2PMax"), modPHScal::gbAddPartWeight2PMax);
	SetRegValue(_T("Settings"), _T("AddPartWeight2SPRPgz"), modPHScal::gbAddPartWeight2SPRPgz);//2007.07.30
	SetRegValue(_T("Settings"),_T("PAForceZero"),modPHScal::gbPAForceZero);//2007.09.25
	SetRegValue(_T("Settings"), _T("MaxPgzPaz"), modPHScal::gbMaxPgzPaz);//2007.09.03

	SetRegValue(_T("Settings"), _T("AddPartWeight2CSPRPgz"), modPHScal::gbAddPartWeight2CSPRPgz);
	SetRegValue(_T("Settings"), _T("MinMaxDispByActualDisp"), modPHScal::gbMinMaxDispByActualDisp);
	SetRegValue(_T("Settings"), _T("SumAllVolume"), modPHScal::gbSumAllVolume);
	SetRegValue(_T("Settings"), _T("WorksWithSABoltsNuts"), modPHScal::gbWorksWithSABoltsNuts);
	SetRegValue(_T("Settings"), _T("WorksWithBoltsNuts"), modPHScal::gbWorksWithBoltsNuts);
	SetRegValue(_T("Settings"), _T("strSpringOrderSQL"), modPHScal::gstrSpringOrderSQL);
	SetRegValue(_T("Settings"), _T("SpringOrder"), modPHScal::giSpringOrder);
	SetRegValue(_T("Settings"), _T("strSpringOrder"), modPHScal::gstrSpringOrder);

	
	return TRUE;
}

BOOL PHSCaculateSetting::IsDesignBaseMaxWorkLoadOrAssembleLoad()
{
	return FALSE;
}

void PHSCaculateSetting::SetDesignBaseMaxWorkLoadOrAssembleLoad(BOOL bState)
{
}

//弹簧载荷变化系数
DOUBLE PHSCaculateSetting::GetSpringLoadChangeCoefficient()
{
	return modPHScal::gnSPR_CHZBH;
}

void PHSCaculateSetting::SetSpringLoadChangeCoefficient(DOUBLE dbCoefficient)
{
	modPHScal::gnSPR_CHZBH = static_cast<float>(dbCoefficient);
}

//结构载荷和工作载荷之比
DOUBLE PHSCaculateSetting::GetStructureLoadWorkLoadRatio()
{
	return modPHScal::gsngRatioOfPjg2Pgz;
}

void PHSCaculateSetting::SetStructureLoadWorkLoadRatio(DOUBLE dbRatio)
{
	modPHScal::gsngRatioOfPjg2Pgz = static_cast<float>(dbRatio);
}

//必须使用弹簧的最小热位移值
DOUBLE PHSCaculateSetting::GetMaxHotDisplacementOfRigiditySupport()
{
	return modPHScal::gUseSpringMinDist;
}

void PHSCaculateSetting::SetMaxHotDisplacementOfRigiditySupport(DOUBLE dbMaxDis)
{
	modPHScal::gUseSpringMinDist = static_cast<float>(dbMaxDis);
}

//冬季空气调节室外计算温度(C,GBJ19)
DOUBLE PHSCaculateSetting::GetCaculateTemperature()
{
	return modPHScal::gnConnectTJ;
}

void PHSCaculateSetting::SetCaculateTemperature(DOUBLE dbTemp)
{
	modPHScal::gnConnectTJ = static_cast<float>(dbTemp);
}

//查找管径扩大范围系数
DOUBLE PHSCaculateSetting::GetPipeDiameterEnlargeCoefficient()
{
	return modPHScal::gnDW_delta;
}

void PHSCaculateSetting::SetPipeDiameterEnlargeCoefficient(DOUBLE dbCoefficient)
{
	modPHScal::gnDW_delta = static_cast<float>(dbCoefficient);
}

//拉杆长度圆整值
DOUBLE PHSCaculateSetting::GetRodLengthRoundValue()
{
	return modPHScal::gnRodLenRoundVal;
}

void PHSCaculateSetting::SetRodLengthRoundValue(DOUBLE dbValue)
{
	modPHScal::gnRodLenRoundVal = static_cast<int>(dbValue);
	if (modPHScal::gnRodLenRoundVal<1)
	{
		modPHScal::gnRodLenRoundVal=100;
	}
}

//非标支吊架的的拉杆最小长度
DOUBLE PHSCaculateSetting::GetMinRodLengthOfUnregularSupport()
{
	return modPHScal::giMinRodLen;
}

void PHSCaculateSetting::SetMinRodLengthOfUnregularSupport(DOUBLE dbLength)
{
	modPHScal::giMinRodLen = static_cast<int>(dbLength);
}

//拉杆插入长度圆整值
DOUBLE PHSCaculateSetting::GetRodInsertLengthRoundValue()
{
	return modPHScal::gnRodInsertLenRoundVal;
}
 
void PHSCaculateSetting::SetRodInsertLengthRoundValue(DOUBLE dbValue)
{
	modPHScal::gnRodInsertLenRoundVal = static_cast<int>(dbValue);
	if (modPHScal::gnRodInsertLenRoundVal<1)
	{
		modPHScal::gnRodInsertLenRoundVal=1;
	}
}

//弹簧规格中荷载单位为牛顿N
BOOL PHSCaculateSetting::IsNewtonUnitOfSpringLoad()
{
	return modPHScal::gbSPRBHLOADUNITBYNEWTON;
}

void PHSCaculateSetting::SetUnitOfSpringLoadIsNewton(BOOL bState)
{
	modPHScal::gbSPRBHLOADUNITBYNEWTON = bState;
}

//摩擦系数
DOUBLE PHSCaculateSetting::GetRubCoefficient()
{
	return this->m_dbRubCoefficient;
}

void PHSCaculateSetting::SetRubCoefficient(DOUBLE dbCoefficient)
{
	this->m_dbRubCoefficient = dbCoefficient;
}

//弹簧规格中荷载用安装荷载
BOOL PHSCaculateSetting::IsBaseAssembleLoadDesignSpring()
{
	return modPHScal::gbSPRBHLOADBYInstallLoad;
}

void PHSCaculateSetting::SetBaseAssembleLoadDesignSpring(BOOL bState)
{
	modPHScal::gbSPRBHLOADBYInstallLoad = bState;
}

//热态吊零
BOOL PHSCaculateSetting::IsZeroLoadOfHot()
{
	return gbHotStatus2Zero;
}

void PHSCaculateSetting::SetLoadOfHotZero(BOOL bState)
{
	gbHotStatus2Zero = bState;
}

//弹簧拉杆孔自动缩小
BOOL PHSCaculateSetting::IsRodHoleOfSpringReduce()
{
	return modPHScal::gbAutoSPRRodDiaDecrease;
}

void PHSCaculateSetting::SetRodHoleOfSpringReduce(BOOL bState)
{
	modPHScal::gbAutoSPRRodDiaDecrease = bState;
}

//弹簧拉杆孔自动扩大
BOOL PHSCaculateSetting::IsRodHoleOfSpringEnlarge()
{
	return modPHScal::gbSPRAutoLugDia;
}

void PHSCaculateSetting::SetRodHoleOfSpringEnglarge(BOOL bState)
{
	modPHScal::gbSPRAutoLugDia = bState;
}

//提示是否替换应力分析程序计算出的弹簧号
BOOL PHSCaculateSetting::IsClewToChangeSpringNo()
{
	return modPHScal::bHintRepZdjType;
}

void PHSCaculateSetting::SetClewToChangeSpringNo(BOOL bState)
{
	modPHScal::bHintRepZdjType = bState;
}

//自适应优化选择弹簧
BOOL PHSCaculateSetting::IsAutoOptimizeSpring()
{
	return modPHScal::gbAutoApplyChzbh;
}

void PHSCaculateSetting::SetAutoOptimizeSpring(BOOL bState)
{
	modPHScal::gbAutoApplyChzbh = bState;
}

//连接孔、杆尺寸自动检查
BOOL PHSCaculateSetting::IsAutoCheckHoleAndRodSize()
{
	return modPHScal::gbConnectHoleDimMatch;
}

void PHSCaculateSetting::SetAutoCheckHoleAndRodSize(BOOL bState)
{
	modPHScal::gbConnectHoleDimMatch = bState;
}

//限制吊杆直径
BOOL PHSCaculateSetting::IsRestrictDiameterOfRod()
{
	return modPHScal::gbLimitLugMinDia;
}

void PHSCaculateSetting::SetRestirctDiameterOfRod(BOOL bState)
{
	modPHScal::gbLimitLugMinDia = bState;
}

//自选管部根部
BOOL PHSCaculateSetting::IsAutoChooseSAAndPA()
{
	return modPHScal::gbAutoPASA;
}

void PHSCaculateSetting::SetAutoChooseSAAndPA(BOOL bState)
{
	modPHScal::gbAutoPASA = bState;
}

//恒吊可以特殊设计
BOOL PHSCaculateSetting::IsSpecialDesignConstSpring()
{
	return modPHScal::gbCSPRCanSpecialDesign;
}

void PHSCaculateSetting::SetSpecialDesignConstSpring(BOOL bState)
{
	modPHScal::gbCSPRCanSpecialDesign = bState;
}

//吊杆直径一致
BOOL PHSCaculateSetting::IsSameDiameterOfRod()
{
	return modPHScal::gbRodDiaByMax;
}

void PHSCaculateSetting::SetSameDiamterOfRod(BOOL bState)
{
	modPHScal::gbRodDiaByMax = bState;
}

//管夹连接螺栓可换小
BOOL PHSCaculateSetting::IsChangeBoltOfClamp()
{
	return modPHScal::gbPipeClampBolt;
}

void PHSCaculateSetting::SetChangeBoltOfClamp(BOOL bState)
{
	modPHScal::gbPipeClampBolt = bState;
}

//自动调整恒吊方向与根部方向一致
BOOL PHSCaculateSetting::IsSameDirectionOfConstSpringAndSA()
{
	return modPHScal::gbAutoAlignCSPRtoSA;
}

void PHSCaculateSetting::SetSameDirectionOfConstSpringAndSA(BOOL bState)
{
	modPHScal::gbAutoAlignCSPRtoSA = bState;
}

//按结构荷载选择恒力弹簧拉杆直径
BOOL PHSCaculateSetting::IsBaseStructureLoadDesignRodOfCSpring()
{
	return modPHScal::gbCalCSPRLugDiaByPjg;
}

void PHSCaculateSetting::SetBaseStructureLoadDesignRodOfCSpring(BOOL bState)
{
	modPHScal::gbCalCSPRLugDiaByPjg = bState;
}

//按保守方式选择双吊点单根部型钢
BOOL PHSCaculateSetting::IsGuardDesignProfileBar()
{
	return modPHScal::gbCalSAbyConservativenessMethod;
}

void PHSCaculateSetting::SetGuardDesignProfileBar(BOOL bState)
{
	modPHScal::gbCalSAbyConservativenessMethod = bState;
}

//按结构荷载选择根部
BOOL PHSCaculateSetting::IsBaseStructureLoadDesignSA()
{
	return modPHScal::gbCalSAbyPJG;
}

void PHSCaculateSetting::SetBaseStructureLoadDesignSA(BOOL bState)
{
	modPHScal::gbCalSAbyPJG = bState;
}

//输出根部整体零件表
BOOL PHSCaculateSetting::IsOutputPartTableOfSA()
{
	return modPHScal::gbOutputSA;
}

void PHSCaculateSetting::SetOutputPartTableOfSA(BOOL bState)
{
	modPHScal::gbOutputSA = bState;
}

//累加零部件重量到计算荷载
BOOL PHSCaculateSetting::IsAccumulateWeightOfPart()
{
	return modPHScal::gbAddPartWeight2PMax;
}

void PHSCaculateSetting::SetAccumulateWeightOfPart(BOOL bState)
{
	modPHScal::gbAddPartWeight2PMax = bState;
}

//累加零部件重量到恒吊工作荷载
BOOL PHSCaculateSetting::IsAccumulateWeightOfPartForCSpring()
{
	return modPHScal::gbAddPartWeight2CSPRPgz;
}

void PHSCaculateSetting::SetAccumulateWeightOfPartForCSpring(BOOL bState)
{
	modPHScal::gbAddPartWeight2CSPRPgz = bState;
}

//拉杆汇总为圆钢
BOOL PHSCaculateSetting::IsRodAsRoundSteel()
{
	return modPHScal::gbSumRodByRoundSteel;
}

void PHSCaculateSetting::SetRodAsRoundSteel(BOOL bState)
{
	modPHScal::gbSumRodByRoundSteel = bState;
}

//采用实际热位移校核恒吊总位移最小最大值
BOOL PHSCaculateSetting::IsActualHotDisplacementCheckCSpring()
{
	return modPHScal::gbMinMaxDispByActualDisp;
}

void PHSCaculateSetting::SetActualHotDisplacementCheckCSpring(BOOL bState)
{
	modPHScal::gbMinMaxDispByActualDisp = bState;
}

//汇总所有卷册
BOOL PHSCaculateSetting::IsCollectAllVolumes()
{
	return modPHScal::gbSumAllVolume;
}

void PHSCaculateSetting::SetCollectAllVolumes(BOOL bState)
{
	modPHScal::gbSumAllVolume = bState;
}

//按选型热位移计算恒吊安装高度
BOOL PHSCaculateSetting::IsBaseHotDispCaculateCSpringHeight()
{
	return modPHScal::gbCalcCSPRHeight_BySelDisp;
}

void PHSCaculateSetting::SetBaseHotDispCaculateCSpringHeight(BOOL bState)
{
	modPHScal::gbCalcCSPRHeight_BySelDisp = bState;
}

//汇总电焊条
BOOL PHSCaculateSetting::IsCollectWelding()
{
	return modPHScal::g_bDHT;
}

void PHSCaculateSetting::SetCollectWelding(BOOL bState)
{
	modPHScal::g_bDHT = bState;
}

//电焊条规格
CString PHSCaculateSetting::GetCollectWeldingSpec()
{
	return modPHScal::g_strDHTGG;
}

void PHSCaculateSetting::SetCollectWeldingSpec(const CString& strSpec)
{
	modPHScal::g_strDHTGG = strSpec;
}

//厂家自带与根部连接的螺栓螺母
BOOL PHSCaculateSetting::HasBoltNutForSAManufacturer()
{
	return modPHScal::gbWorksWithSABoltsNuts;
}

void PHSCaculateSetting::SetHasboltNutForSAManufacturer(BOOL bState)
{
	modPHScal::gbWorksWithSABoltsNuts = bState;
}

//厂家自带与连接件连接的螺栓螺母
BOOL PHSCaculateSetting::HasBoltNutForConnectManufacturer()
{
	return modPHScal::gbWorksWithBoltsNuts;
}

void PHSCaculateSetting::SetHasBoltNutForConnectManufacturer(BOOL bState)
{
	modPHScal::gbWorksWithBoltsNuts = bState;
}

CString PHSCaculateSetting::GetSpringDesignRegular()
{
	return modPHScal::gstrSpringOrderSQL;
}

void PHSCaculateSetting::SetSpringDesignRegular(const CString& strRegular)
{
	modPHScal::gstrSpringOrderSQL = strRegular;
}

//根部型钢数量
INT PHSCaculateSetting::GetProfileBarNumOfSA()
{
	return modPHScal::iNumCSVal;
}

void PHSCaculateSetting::SetProfileBarNumOfSA(INT nNum)
{
	modPHScal::iNumCSVal = nNum;
}

//拉杆直径自动适应恒吊拉杆直径
BOOL PHSCaculateSetting::IsFitRodDiameterOfCSpring()
{
	return modPHScal::gbAutoApplyCSPRLugDia;
}

void PHSCaculateSetting::SetFitRodDiameterOfCSpring(BOOL bState)
{
	modPHScal::gbAutoApplyCSPRLugDia = bState;
}

//弹簧顺序规则名
CString PHSCaculateSetting::GetSpringDesignRegularName()
{
	return modPHScal::gstrSpringOrder;
}

void PHSCaculateSetting::SetSpringDesignRegularName(const CString& strRegularName)
{
	modPHScal::gstrSpringOrder = strRegularName;
}

INT PHSCaculateSetting::GetSpringDesignRegularNum()
{
	return modPHScal::giSpringOrder;
}

void PHSCaculateSetting::SetSpringDesignRegularNum(INT nNum)
{
	modPHScal::giSpringOrder = nNum;
}