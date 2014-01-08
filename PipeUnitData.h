// PipeUnitData1.h: interface for the CPipeUnitData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PIPEUNITDATA1_H__7EF2C07D_7609_4A8C_A1B5_962F3BE33462__INCLUDED_)
#define AFX_PIPEUNITDATA1_H__7EF2C07D_7609_4A8C_A1B5_962F3BE33462__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OtherStruct.h"

//单元特性数据类
class AFX_EXT_CLS CPipeUnitFeature
{
public:
	CPipeUnitFeature();
	virtual ~CPipeUnitFeature(){};
	CAverageForce m_AverageForce;//均布力
	COffSetsData m_OffSetsData;//偏移数据
	MediumFeature m_MediumFeature;//介质特性
	CWind m_Wind; //风
	PipeFeature m_PipeFeature; //单元特性
	MaterialFeature m_MaterialFeature; //材料特性
	SoilModels m_SoilModels;//埋地土壤模型
	ProjectionLength m_ProjectionLength;//投影长
};

class AFX_EXT_CLS CPipeNodeFeature
{
public:
	CPipeNodeFeature(){};
	virtual ~CPipeNodeFeature(){};
	CHanger m_Hanger; //弹簧
	RestrantSel m_RestrantSel; //约束
	Displacement m_Displacement;//位移
	CForcmnt m_Forcmnt; //力和力矩
	ThreeOrSifsOpen m_ThreeOrSifsOpen;//三通或应力加强
	
	int m_nNodeNumTo; //终点
	int m_nNodeNumFrom; //起点
	CNodeName m_NodeName; //节点名称
};

class AFX_EXT_CLS CPipeUnitData : public CPipeUnitFeature,public CPipeNodeFeature
{
public:
	CPipeUnitData();
	virtual ~CPipeUnitData();
	BOOL m_bModified;//判别单元数据在上一次保存以来是否改变过
	Coordinates m_Coordinate1; //起点坐标(交点坐标,当带弯管时!=真正管道上的坐标)
	Coordinates m_Coordinate2; //末点坐标(交点坐标)
	Coordinates m_TrueCoordinate1; //点在管道上的真实坐标
	Coordinates m_TrueCoordinate2; //点在管道上的真实坐标
	
	Coordinates m_ResultResult1; //管道产生位移时的坐标(真实坐标)
	Coordinates m_ResultResult2; //管道产生位移时的坐标(真实坐标)
	
	AssistPosition m_AssistPosition;//单元的辅助数据的位置
	StraightUnit m_StraightUnit; //直单元
	RigidUnit m_RigidUnit;//刚性件
	ReducerUnit m_ReducerUnit;//大小头
	
	NoPipeStrongUnit m_NoPipeStrongUnit;//补偿器
	
	int m_nUnitType;//单元类型 0:直单元; 1:刚性件 2:大小头 3:补偿器)
	BYTE m_bNOCOLOR;
	
	double GetSumWeight();
	double GetBendWeight();
	double GetStraightNoBendWeight();
	HighDisplay m_BeginHighData;//加亮数据
	HighDisplay m_EndHighData;
	HighDisplay m_CenterHighDataB;
	HighDisplay m_CenterHighDataE;
	BOOL m_bHigh;//加亮逻辑

	//PDMS->CII接口另加
	CString m_strFromPDMSCompName; //起点的端口的PDMS的管件名称
	CString m_strToPDMSCompName; //起点的末端的PDMS的管件名称
	int m_iNodeFromPort; //起点的端口号
	int m_iNodeToPort; //末点的端口号
};


class AFX_EXT_CLS CPipeGlobalData
{
public:
	CPipeGlobalData();
	virtual ~CPipeGlobalData();
	CUnits m_Units;
	CUnitsScale m_UnitsScale;
	OutUnitsScale m_OutUnitsScale; //所用输出文件到输出文件的比例
	int m_ncheckInUnit;
	int m_ncheckOutUnit;
	int m_iICC;//向上轴(0:X;1:Y;2:Z)
	Coordinates m_OriginalCoordinate; //原点坐标
	double	m_dbDesignConditionEnvironmentTemp; //环境温度
	int m_dbRigidityType; //支吊架刚度方式
	int m_dbRigidityTimes;//刚性件刚度为直管刚度的倍数
	double m_dbOccLoadk;//偶然荷载系数
	BOOL m_bSoid; //计算时是否考虑土壤数据
	
	CString m_strGversion; //应用程序主版本
	CString m_strRversion; //应用程序副版本
	
	CInitHangerAttribute m_InitHangerAtt; //弹簧属性默认值
	
	BOOL m_bStrongWeight;//计算时是否考虑补尝器的重量 2007.05.24 Modified by lyf
	BOOL m_bLoadWeightratio;//计算时是否使用荷重比g's:默认FALSE(不使用);TRUE为使用
	BOOL m_bAutoSettingDef;//是否自动设置默认值:默认TRUE为自动;FALSE(不自动)

	BOOL m_bIsInsWeight; //保温是否导出为重量,CII5.3中用到,从PDMS或AutoPDMS生成CII时用到
};


#endif // !defined(AFX_PIPEUNITDATA1_H__7EF2C07D_7609_4A8C_A1B5_962F3BE33462__INCLUDED_)
