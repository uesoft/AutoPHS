// PHSCaculateSetting.h: interface for the PHSCaculateSetting class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PHSCACULATESETTING_H__DE1AB612_DF0C_474E_A95B_1A440A17C1B0__INCLUDED_)
#define AFX_PHSCACULATESETTING_H__DE1AB612_DF0C_474E_A95B_1A440A17C1B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PHSSetting.h"

class __declspec( dllexport ) PHSCaculateSetting : public PHSSetting  
{
public:
	PHSCaculateSetting(); 
	virtual ~PHSCaculateSetting();

public:
	virtual BOOL InitSetting();
	virtual BOOL SaveSetting();

public:
	BOOL IsDesignBaseMaxWorkLoadOrAssembleLoad();
	void SetDesignBaseMaxWorkLoadOrAssembleLoad(BOOL bState);

	//弹簧载荷变化系数
	DOUBLE GetSpringLoadChangeCoefficient();
	void SetSpringLoadChangeCoefficient(DOUBLE dbCoefficient);
	
	//结构载荷和工作载荷之比
	DOUBLE GetStructureLoadWorkLoadRatio();
	void SetStructureLoadWorkLoadRatio(DOUBLE dbRatio);
	
	//必须使用弹簧的最小热位移值
	DOUBLE GetMaxHotDisplacementOfRigiditySupport();
	void SetMaxHotDisplacementOfRigiditySupport(DOUBLE dbMaxDis);
	
	//冬季空气调节室外计算温度(C,GBJ19)
	DOUBLE GetCaculateTemperature();
	void SetCaculateTemperature(DOUBLE dbTemp);

	//查找管径扩大范围系数
	DOUBLE GetPipeDiameterEnlargeCoefficient();
	void SetPipeDiameterEnlargeCoefficient(DOUBLE dbCoefficient);
	
	//拉杆长度圆整值
	DOUBLE GetRodLengthRoundValue();
	void SetRodLengthRoundValue(DOUBLE dbValue);
	
	//非标支吊架的的拉杆最小长度
	DOUBLE GetMinRodLengthOfUnregularSupport();
	void SetMinRodLengthOfUnregularSupport(DOUBLE dbLength);
	
	//拉杆插入长度圆整值
	DOUBLE GetRodInsertLengthRoundValue();
	void SetRodInsertLengthRoundValue(DOUBLE dbValue);
	
	//弹簧规格中荷载单位为牛顿N
	BOOL IsNewtonUnitOfSpringLoad();
	void SetUnitOfSpringLoadIsNewton(BOOL bState);
	DOUBLE GetRubCoefficient();
	void SetRubCoefficient(DOUBLE dbCoefficient);

	//弹簧规格中荷载用安装荷载
	BOOL IsBaseAssembleLoadDesignSpring();
	void SetBaseAssembleLoadDesignSpring(BOOL bState);

	//热态吊零
	BOOL IsZeroLoadOfHot();
	void SetLoadOfHotZero(BOOL bState);
	
	//弹簧拉杆孔自动缩小
	BOOL IsRodHoleOfSpringReduce();
	void SetRodHoleOfSpringReduce(BOOL bState);
	
	//弹簧拉杆孔自动扩大
	BOOL IsRodHoleOfSpringEnlarge();
	void SetRodHoleOfSpringEnglarge(BOOL bState);
	
	//提示是否替换应力分析程序计算出的弹簧号
	BOOL IsClewToChangeSpringNo();
	void SetClewToChangeSpringNo(BOOL bState);

	//自适应优化选择弹簧
	BOOL IsAutoOptimizeSpring();
	void SetAutoOptimizeSpring(BOOL bState);

	//连接孔、杆尺寸自动检查
	BOOL IsAutoCheckHoleAndRodSize();
	void SetAutoCheckHoleAndRodSize(BOOL bState);
	
	//限制吊杆直径
	BOOL IsRestrictDiameterOfRod();
	void SetRestirctDiameterOfRod(BOOL bState);

	//自选管部根部
	BOOL IsAutoChooseSAAndPA();
	void SetAutoChooseSAAndPA(BOOL bState);

	//恒吊可以特殊设计
	BOOL IsSpecialDesignConstSpring();
	void SetSpecialDesignConstSpring(BOOL bState);

	//吊杆直径一致
	BOOL IsSameDiameterOfRod();
	void SetSameDiamterOfRod(BOOL bState);

	//管夹连接螺栓可换小
	BOOL IsChangeBoltOfClamp();
	void SetChangeBoltOfClamp(BOOL bState);

	//自动调整恒吊方向与根部方向一致
	BOOL IsSameDirectionOfConstSpringAndSA();
	void SetSameDirectionOfConstSpringAndSA(BOOL bState);

	//按结构荷载选择恒力弹簧拉杆直径
	BOOL IsBaseStructureLoadDesignRodOfCSpring();
	void SetBaseStructureLoadDesignRodOfCSpring(BOOL bState);

	//按保守方式选择双吊点单根部型钢
	BOOL IsGuardDesignProfileBar();
	void SetGuardDesignProfileBar(BOOL bState);

	//拉杆直径自动适应恒吊拉杆直径
	BOOL IsFitRodDiameterOfCSpring();
	void SetFitRodDiameterOfCSpring(BOOL bState);

	//按结构荷载选择根部
	BOOL IsBaseStructureLoadDesignSA();
	void SetBaseStructureLoadDesignSA(BOOL bState);

	//输出根部整体零件表
	BOOL IsOutputPartTableOfSA();
	void SetOutputPartTableOfSA(BOOL bState);

	//累加零部件重量到计算荷载
	BOOL IsAccumulateWeightOfPart();
	void SetAccumulateWeightOfPart(BOOL bState);

	//累加零部件重量到恒吊工作荷载
	BOOL IsAccumulateWeightOfPartForCSpring();
	void SetAccumulateWeightOfPartForCSpring(BOOL bState);

	//拉杆汇总为圆钢
	BOOL IsRodAsRoundSteel();
	void SetRodAsRoundSteel(BOOL bState);

	//采用实际热位移校核恒吊总位移最小最大值
	BOOL IsActualHotDisplacementCheckCSpring();
	void SetActualHotDisplacementCheckCSpring(BOOL bState);

	//汇总所有卷册
	BOOL IsCollectAllVolumes();
	void SetCollectAllVolumes(BOOL bState);

	//按选型热位移计算恒吊安装高度
	BOOL IsBaseHotDispCaculateCSpringHeight();
	void SetBaseHotDispCaculateCSpringHeight(BOOL bState);

	//汇总电焊条
	BOOL IsCollectWelding();
	void SetCollectWelding(BOOL bState);

	//电焊条规格
	CString GetCollectWeldingSpec();
	void SetCollectWeldingSpec(const CString& strSpec);

	//厂家自带与根部连接的螺栓螺母
	BOOL HasBoltNutForSAManufacturer();
	void SetHasboltNutForSAManufacturer(BOOL bState);

	//厂家自带与连接件连接的螺栓螺母
	BOOL HasBoltNutForConnectManufacturer();
	void SetHasBoltNutForConnectManufacturer(BOOL bState);

	INT GetSpringDesignRegularNum();
	void SetSpringDesignRegularNum(INT nNum);

	//弹簧顺序规则名
	CString GetSpringDesignRegularName();
	void SetSpringDesignRegularName(const CString& strRegularName);

	//弹簧顺序规则
	CString GetSpringDesignRegular();
	void SetSpringDesignRegular(const CString& strRegular);

	////根部型钢数量
	INT GetProfileBarNumOfSA();
	void SetProfileBarNumOfSA(INT nNum);

private:
	DOUBLE m_dbRubCoefficient;
};

__declspec( dllexport ) PHSCaculateSetting& GetCurCaculateSetting();
#endif // !defined(AFX_PHSCACULATESETTING_H__DE1AB612_DF0C_474E_A95B_1A440A17C1B0__INCLUDED_)
