//#include "StdAfx.h"
#ifndef OTHERSTRUCT_H
#define OTHERSTRUCT_H
//坐标
#ifdef _AFXEXTCLS
#define AFX_EXT_CLS       AFX_CLASS_EXPORT
#else
#define AFX_EXT_CLS       AFX_CLASS_IMPORT
#endif
#include <math.h>
class AFX_EXT_CLS Coordinates
{
public:
	Coordinates();
	Coordinates(double x0, double y0, double z0){X=x0; Y=y0; Z=z0;}
	virtual ~Coordinates(){};
	BOOL operator==(const Coordinates& unit2);
	double X;
	double Y;
	double Z;
};

class AFX_EXT_CLS CBendData 
{
public:
	int m_SetMidAngleFlag;//中点角度标志标志:1表示默认中点角度为角度1;2表示角度2;3表示角度3;0表示不设中点角度标志
	CBendData();
	virtual ~CBendData(){};
	int m_nBendUnitType; //(1:热压;2:弯制;3:焊制)
	double m_dbBendRadius;
	void operator=(CBendData unit2);
	double m_dbAngle;
	Coordinates m_CenterPoint;//中心点坐标
	Coordinates m_StartPoint;
	Coordinates m_EndPoint;
	
	Coordinates m_ResultCenterPoint; //管道产生位移时的坐标(中心点坐标)
	Coordinates m_ResultStartPoint; //管道产生位移时的坐标(弯单元起点真实坐标)
	Coordinates m_ResultEndPoint; //管道产生位移时的坐标(弯单元末点真实坐标)
	double m_dbResultAngle; //运行工况产生位移后弯管角度
	double m_dbResultBendRadius; //运行工况产生位移后弯管半径
	
	double m_dbAngle1; //节点位置#1的角度
	Coordinates m_Point1;//节点位置#1的坐标
	int m_dbNode1; //节点1号
	double m_dbAngle2; //节点位置#2的角度
	Coordinates m_Point2;//节点位置#2的坐标
	int m_dbNode2; //节点2号
	double m_dbAngle3; //节点位置#3的角度
	Coordinates m_Point3;//节点位置#3的坐标
	int m_dbNode3; //节点3号
	int m_iMiterPoints; //斜接切割数
	double m_dbFactor; //柔性系数
	double m_dbPipeWallThickness; //弯头的壁厚(和直管不同时有数据)
	
};

//风
class AFX_EXT_CLS CWind  
{
public:
	CWind();
	virtual ~CWind(){};
	BOOL m_bWindTag;
	double	m_dbBasicWindPressure;//基本风压
	double	m_dbEWHorizontalLoadDirectionCoefX;
	double	m_dbEWHorizontalLoadDirectionCoefY;
	double	m_dbEWHorizontalLoadDirectionCoefZ;
	double	m_dbPipeToWindDiameter;//迎风直径
	double	m_dbWindLoadFigureCoef;//风荷载体形系数
	double  m_dbWindPressureChangeCoef; //风压变化系数
	
};
//节点名称
class AFX_EXT_CLS CNodeName
{
public:
	CNodeName();
	virtual ~CNodeName(){};
	BOOL m_bNodeNameTag;
	CString m_strFromName;
	CString m_strToName;
};
//约束结构
class AFX_EXT_CLS RestrantData
{
public:
	RestrantData();
	virtual~RestrantData(){};
	int m_iNode;
	int m_iCNode;
	int m_iType;		//约束类型:1表示全部被约束;2,3,4分别表示x,y,z方向可以有线位移;5,6,7则表示三个方向可以角位移
	double m_dbStif; //约束刚度
	double m_dbGap; //约束间隔
	double m_dbMu; //约束摩擦系数
	double m_dbCosX;
	double m_dbCosY;
	double m_dbCosZ;
	int m_iBitTag;
};
//约束选择
class AFX_EXT_CLS RestrantSel
{
public:
	RestrantSel();
	virtual ~RestrantSel(){};
	RestrantData m_Restrant[4];
	BOOL m_bRestrantTag; 
	
};
//单元的辅助数据的标志
class AFX_EXT_CLS AssistPosition
{
public:
	AssistPosition();
	virtual ~AssistPosition(){};
	int m_iBend; //弯单元
	int m_iRigit; //刚性件
	int m_iExpjt; //补偿器
	int m_iRestrant; //约束
	int m_iDisp; //位移
	int m_iForcmnt; //力和力矩
	int m_iUniform; //均布荷载
	int m_iWind; //风荷载
	int m_iOffsets; //单元偏移
	int m_iAllowbls; //许用应力
	int m_iSifTees; //三通
	int m_iNodeNume; //节点名称
	int m_iReducer; //异径管
	int m_iSoid; //土壤数据
};

//力和力矩的结构
////////////////////////////////////////
class AFX_EXT_CLS CForcmntData
{
public:
	CForcmntData();
	virtual ~CForcmntData(){};
	double m_dbFX;
	double m_dbFY;
	double m_dbFZ;
	double m_dbMX;
	double m_dbMY;
	double m_dbMZ;
};
class AFX_EXT_CLS CForcmntNodeData
{
public:
	CForcmntNodeData();
	virtual ~CForcmntNodeData(){};
	int m_ForcmntNodeNumer;
	CForcmntData m_ForcmntData[11];//有11种工况(GLif算中10种和一种偶然荷载)
};
class AFX_EXT_CLS CForcmnt
{
public:
	CForcmnt();
	virtual ~CForcmnt(){};
	BOOL m_bForcmnt;
	CForcmntNodeData m_ForcmntNodeData[2];
};
//////////////////////////////////////////////////
class AFX_EXT_CLS COffSetsData //单元偏移数据
{
public:
	COffSetsData();
	virtual ~COffSetsData(){};
	bool m_bOffsets;
	double m_dbFromX;
	double m_dbFromY;
	double m_dbFromZ;
	double m_dbToX;
	double m_dbToY;
	double m_dbToZ;
};

/////////////////////////////////////////////////

//////////////////////位移结构
class AFX_EXT_CLS CDispData
{
public:
	CDispData();
	virtual ~CDispData(){};
	double m_dbLX;
	double m_dbLY;
	double m_dbLZ;
	double m_dbRX;
	double m_dbRY;
	double m_dbRZ;
};
class AFX_EXT_CLS CDispNodeData
{
public:
	CDispNodeData();
	virtual ~CDispNodeData(){};
	int	m_nGivenDisplacementNodeNum;
	CDispData m_DispData[9]; //最多只有9种工况
};

class AFX_EXT_CLS Displacement  
{
public:
	Displacement();
	virtual ~Displacement(){};
	BOOL m_bGivenDisplacement;
	CDispNodeData m_DispNodeData[2]; //每个单元可以描述两个点的位移
};
//////////////////////////////////////////////////////
//均布力
class AFX_EXT_CLS CAverageForceData
{
public:
	CAverageForceData();
	virtual ~CAverageForceData(){};
	double m_dbX;
	double m_dbY;
	double m_dbZ;
};

class AFX_EXT_CLS CAverageForce  
{
public:
	CAverageForce();
	virtual ~CAverageForce(){};
	BOOL m_bAverageForce;
	CAverageForceData m_AverageForceData[3];
};
//////////////////////////////////////////////////
class AFX_EXT_CLS CHanger
{
public:
	CHanger();
	virtual ~CHanger(){};
	BOOL m_bHangerTag; //是否有弹簧
	int m_iHangerNodeNumer;
	int m_iInitWork; // 冷/热态吊零
	int m_iMidLineSel;//是否用中线选择方法
	double m_dbHangerRig;//弹簧刚度
	double m_dbHangerVarSitf;//允许变化系数
	double m_dbDispCriteria;//刚性支吊架的位移标准
	double m_dbAvSpace;//弹簧允许空间
	double m_dbInitLoad;//冷荷载
	double m_dbWorkLoad;//热荷载
	double m_dbDefineLoad;//用户定义运行工况f/变力弹簧
	double m_dbAllowLimit;//最大允许行程极限
	double m_iCaseSel;//工况选择
	double m_dbConstantHanger; //弹簧恒力支吊荷载
	int m_iFreeResNode1; //要自由的固定节点1
	int m_iFreeResNode2; //要自由的固定节点2
	int m_iFreeCode1;//自由的方向
	int m_iFreeCode2;
	int m_iHangerNum;//弹簧并联数
	int m_iHangerTableID;//弹簧表号
	int m_iHangerShort;//是否优先使用短弹簧
	int m_iHangerCNode;//连接点号
};

//////////////////////////////////////////////////
//默认弹簧属性数据
class AFX_EXT_CLS CInitHangerAttribute
{
public:
	CInitHangerAttribute();
	virtual ~CInitHangerAttribute(){};
	int m_iInitHangerTableID;//弹簧表号
	double m_dbInitHangerVarSitf;//允许变化系数
	double m_dbInitDispCriteria;//刚性支吊架的位移标准
	double m_dbInitAllowLimit;//最大允许行程极限
	int m_iInitHangerShort;//是否可以用短弹簧
	int m_iInitDefMul; //是默认多荷载工况设计选项.
	int m_iInitIdfoper; //是默认弹簧设计运行工况号
	int m_iInitIactcld;  //是默认冷荷载计算开关
	int m_iInitIhgrlds; //是弹簧运行荷载数
	int m_iInitIactual; //是定义实际冷荷载的工况.
	int m_iInitImultiopts;//是多工况设计选项
};

//材料特性
class AFX_EXT_CLS MaterialFeature  
{
public:
	MaterialFeature();
	virtual ~MaterialFeature(){};
	
	BOOL m_bMaterialFeatureTag;
	double	m_dbBendLimitIn20Degree; //环境温度下的屈服极限
	double	m_dbElasticModulusIn20Degree;//环境温度下弹性模量值
	double	m_dbAllowStressIn20Degree;//环境温度下许用应力值
	double m_dbEff;//纵向焊接系数
	double m_dbSy;
	double m_dbFac;
	double m_dbPmax;
	
	//AutoPSA7.0中的参数
	double m_dbOperatingCycles[9];//管道交变次数
	double m_dbMaterialAllowStressValue[9]; //	许用应力值
	double m_dbMaterialLinearExpansionCoefValue[9]; //线胀系数值
	double m_dbMaterialElasticModulusValue[9]; //弹性模量值
	double m_dbBendLimitInWaterValue[9]; //屈服极限
	void operator =(MaterialFeature unit2);
	
};

//介质特性
class AFX_EXT_CLS MediumFeature  
{
public:
	MediumFeature();
	virtual ~MediumFeature(){};
	BOOL m_bMediumFeatureTag; //AutoPSA7.0 中的工作温度,压力标志
	double m_dbWaterTestConditionPressure1;//水压强度试验压力或液压试验压力
	double m_dbWaterTestConditionPressure2;//水压严密性试验压力或气压试验压力
	double m_dbWorkTemp[9]; //AuotPSA7.0中的工作温度,9种可以组合
	double m_dbWorkPressure[9]; //AuotPSA7.0中的工作压力,9种可以组合
	void operator=(MediumFeature unit2);
	MediumFeature operator / (double dbValue);
	MediumFeature operator + (const MediumFeature& unit2);
};

//补偿器
class AFX_EXT_CLS NoPipeStrongUnit  
{
public:
	NoPipeStrongUnit();
	virtual ~NoPipeStrongUnit(){};
	double	m_dbNoPipeStrongUnitOverallWeight;
	//cbl 2005.11.15
	double  m_dbAxialStif;  //轴向刚度
	double  m_dbTransStif;  //横向刚度
	double  m_dbBendingStif; //弯曲刚度
	double  m_dbTorsionStif;  //扭转刚度
	double  m_dbArea; //有效面积
	// luow  2006.8.24
	double  m_dbStrongFK[9];  //不同工作温度下,刚度的修正系数
};

//投影长
class AFX_EXT_CLS ProjectionLength  
{
public:
	ProjectionLength();
	virtual ~ProjectionLength(){};
	double m_dbProjectionLengthZ;
	double m_dbProjectionLengthY;
	double m_dbProjectionLengthX;
public:
	double GetProjectionLength()
	{
		double temp = m_dbProjectionLengthX*m_dbProjectionLengthX + m_dbProjectionLengthY*m_dbProjectionLengthY + m_dbProjectionLengthZ*m_dbProjectionLengthZ ;
		return sqrt( temp )  ;

	}
	
};

//大小头
class AFX_EXT_CLS ReducerUnit  
{
public:
	ReducerUnit();
	virtual ~ReducerUnit(){};
	double m_dbDiameter2;
	double m_dbThickness2;
	double m_dbReduceAngle;
	double m_dbR1;
	double m_dbR2;
	double m_dDn; //直径 PDMS->PSA接口增加
};

//刚性件
class AFX_EXT_CLS RigidUnit  
{
public:
	RigidUnit();
	virtual ~RigidUnit(){};
	double	m_dbRigidUnitOverallWeight;
	double m_dbRigidUnitLinearExpansionCoef;
	
};
//截面特性
class AFX_EXT_CLS SectionFeature  
{
public:
	SectionFeature();
	virtual ~SectionFeature(){};
	double m_dbPipeOuterDiameter;
	double m_dbPipeWallThickness;
	
};

class AFX_EXT_CLS SoilModels
{
public:
	SoilModels();
	virtual ~SoilModels(){};
	BOOL m_bSoilModelsTag;		// 埋地单元数据设置标志, 为"TRUE"表示已经被用户设置
	double m_dbSu;//摩擦系数
	double m_dbDensity;//密度
	double m_dbDepth;//埋地深度
	double m_dbFrictAngle; //摩擦角
	double m_dbShearStrength;//无排水剪切强度
	double m_dbOverburden;//过载压实乘子
	double m_dbYieldDispFactor;//屈服位移系数
	double m_dbExpansionCoef; //温度膨胀系数
	double m_dbTemperatureChange;//温度变化
	
};
/////////////////////////////////////////////////

class AFX_EXT_CLS CUnits
{
public:
	CUnits();
	virtual ~CUnits(){}
	CString m_strUnitName; //单位名称
	CString m_strNom;//"开" 或 "关" 并且告诉PREPIP 是否允许公称直径
	CString m_strLenTip;//长度
	CString m_strForTip;//力
	CString m_strWeightTip; //重量
	CString m_strMomentTip; //输入力矩
	CString m_strOMomentTip; //输出力矩
	CString m_strStressTip; //应力
	CString m_strScTip; //温度
	CString m_strLineDispTip; //位移
	CString m_strPreTip; //压力
	CString m_strEModulusTip;//弹性模量
	CString m_strExpansionCoefTip;//线胀系数
	CString m_strPipeDensityTip; //管道密度
	CString m_strInsulationDensityTip; //保温密度
	CString m_strFluidDensityTip;//流体密度
	CString m_strTSFTip;//平动刚度
	CString m_strRSFTip;//转动刚度
	CString m_strAverageForceTip;//均布荷载
	CString m_strLoadWeightRatioTip; //均布荷载使用荷重比g's标签
	CString m_strForceTip;//重力荷载
	CString m_strWindForceTip; //风荷载
	CString m_strEleTip;//标高
	CString m_strCLenTip;//合成长度
	CString m_strDiaTip;//直径
	CString m_strThkTip;//壁厚
	CString m_strAangleTip; //角度 
	CString m_strAangleDispTip; //角位移
	CString m_strMasTip;//质量
	CString m_strArea; //补偿器有效面积
};

class AFX_EXT_CLS CFastUnitsScale
{
public:
	CFastUnitsScale();
	virtual ~CFastUnitsScale(){}
	static double m_dbCmpCoefDia; //计算应力增强系数所用外径单位之间比例(内存标准m->mm)
	static double m_dbWriteCmpAngle; //写入计算临时文件所用"角度"单位(rad)
	
	static double m_dbMaterialMdbWallThickness; //材料数据库中的单位到内存单位的比例(壁厚mm->m) 
	static double m_dbMaterialMdbBendLimitIn; //材料数据库中的单位到内存单位的比例(屈服极限MPA->pa)
	static double m_dbMaterialMdbStress;//材料数据库中的单位到内存单位的比例(应力MPA->pa)
	static double m_dbMaterialMdbElasticModulus; //弹性模量(KN/mm2->N/m2)
	static double m_dbMaterialMdbALPHAt; //线胀系数
	
	static double m_dbSoidMdbShearStrength; //埋地数据库单位到内存单位比例
	
	static double m_dbAX; //补偿器数据库轴向刚度到内存单位比例
	static double m_dbTorS; //补偿器数据库扭转刚度到内存单位比例
	static double m_dbTraS; //补偿器数据库横向刚度到内存单位比例
	static double m_dbBendS; //补偿器数据库弯曲刚度到内存单位比例
	static double m_dbArea; //补偿器数据库有效面积到内存单位比例
};

class AFX_EXT_CLS CUnitsScale
{
public:
	CUnitsScale();
	virtual ~CUnitsScale(){};
	double m_dbLen; //标准单位到当前单位的长度比例 (M-->当前)
	double m_dbFor; //标准单位到当前单位的力比例 (N-->当前)
	double m_dbWeight; //标准单位到当前单位的重量比例 (KG-->当前)
	double m_dbMoment; //标准单位到当前单位的力矩比例 (N/M-->当前)
	double m_dbStress; //标准单位到当前单位应力比例 (Pa-->当前)
	double m_dbSc;     //标准单位到当前单位温度比例 (C-->当前)
	double m_dbScDev;  //标准单位到当前单位温度偏差 (C-->当前)
	double m_dbNewScDev;
	double m_dbLineDisp; //标准单位到当前单位位移比例 (M-->当前)
	double m_dbPre;      //标准单位到当前单位压力比例 (pa-->当前)
	double m_dbEModulus; //标准单位到当前单位弹性模量比例 (pa(N/m2) -->当前)
	double m_dbExpansionCoef; //标准单位到当前膨胀系数比例 (1E-6/C -->当前)
	double m_dbPipeDensity;   //标准单位到当前管道密度比例 (kg/m3 -->当前)
	double m_dbInsulationDensity; //标准单位到当前保温密度比例 (kg/m3 -->当前)
	double m_dbFluidDensity; //标准单位到当前流体密度比例 (kg/m3 -->当前)
	double m_dbTSF;  //标准单位到当前平动刚度比例 (N/M -->当前)
	double m_dbRSF; //标准单位到当前转动刚度比例 (N.M/Rad-->当前)
	double m_dbAverageForce; //标准单位到当前均布荷载比例 (N/M-->当前)
	double m_dbLodeWeightRatio;//标准单位到当前均布荷载比率(g's的情况下使用,其值为1.0)
	double m_dbForce; //标准单位到当前荷载比例 (N-->当前)
	double m_dbWindForce; //标准单位到当前风压比例 (Pa-->当前)
	double m_dbEle; //标准单位到当前标高比例 (M-->当前)
	double m_dbCLen; //标准单位到当前合成长度比例 (M-->当前)
	double m_dbDia; //标准单位到当前直径比例 (M-->当前)
	double m_dbThk; //标准单位到当前壁厚比例 (M-->当前)
	double m_dbAngle; //标准单位到当前角度比例 (度-->当前)
	double m_dbAngleDisp; //标准单位到当前角位移比例 (rad-->当前)
	double m_dbArea; //标准单位到当前补偿器面积比例 (M2-->当前)
};

class AFX_EXT_CLS OutUnitsScale
{
public:
	OutUnitsScale();
	virtual ~OutUnitsScale(){};
	double m_dbWritePHSFor;//写入PHS所用"力"单位(N)到输出文件的比例
	double m_dbWritePHSMoment;//写入PHS所用"力矩"单位(N.M)到输出文件的比例
	double m_dbWritePHSDisp; //写入PHS所用"线位移"单位(M)到输出文件的比例
	double m_dbWritePHSAngleDisp; //写入PHS所用"角位移"单位(RAD)到输出文件的比例
};

/////////////// PSA1.1中所有特证点和特证点类型
class AFX_EXT_CLS OldNodeType
{
public:
	OldNodeType();
	virtual ~OldNodeType(){};
	int m_iNodeNumer;
	int m_iNodeType;
};

////////////// 7.0中的约束类型转换1.1中的类型用
class AFX_EXT_CLS ResTypeWay
{
public:
	ResTypeWay();
	virtual ~ResTypeWay(){};
	int m_iNodeNumer;
	bool m_bResX;
	bool m_bResY;
	bool m_bResZ;
	bool m_bResRX;
	bool m_bResRY;
	bool m_bResRZ;
	int m_iNodeType;
	int m_iBitTag;  //读取GLif文件百位标志( 1:总体坐标系  2:单元坐标系 3:在规定工况起作用)

};

class AFX_EXT_CLS OldDisplacement // PSA1.1中的位移结构(给定,热,超温,地震位移)
{
public:
	OldDisplacement();
	virtual ~OldDisplacement(){};
	int m_nDisplacementNodeNum;
	double m_dbAngleDisplacementX;
	double m_dbAngleDisplacementY;
	double m_dbAngleDisplacementZ;
	double m_dbLinearDisplacementX;
	double m_dbLinearDisplacementY;
	double m_dbLinearDisplacementZ;
};

//冷紧口,保存PSA1.1中有用的数据
class AFX_EXT_CLS OldColdTightenToMouth 
{
public:
	OldColdTightenToMouth();
	virtual ~OldColdTightenToMouth(){};
	double	m_dbBMColdTightenValue;
	double m_dbX;
	double m_dbY;
	double m_dbZ;
	int	m_nColdTightenNodeNum;
};

//保存应力加强系数
class AFX_EXT_CLS OldStressStrongthenCoef
{
public:
	OldStressStrongthenCoef();
	virtual ~OldStressStrongthenCoef(){};
	int m_nStressCoefNodeNum;
	double m_dbStrongthenCoef;
};

//保存PSA1.1中三点有用的数据结构
class AFX_EXT_CLS OldThreeOpenPoint  
{
public:
	OldThreeOpenPoint();
	virtual ~OldThreeOpenPoint(){};
	int	m_nThreeOpenPointNodeNum;
	int m_nThreeOpenType;
	double m_dbCrotch;//热挤压三通过渡区外半径
	double m_dbPadThk;//鞍板厚度
	double m_dbFtgRo;//管接座加强段的外半径	
	double m_dbSif; //给定点应力增强系数
};

class AFX_EXT_CLS FilepesData
{
public:
	FilepesData();
	virtual ~FilepesData(){};
	int m_nFormNode;
	int m_mToNode;
	double dbPipeDensity;
	double dbFluidDensity;
	double dbInsulationDensity;
	double dbInsulationDensity2;
	double dbInsulationDensity3; 
	double dbPipeInsulationThickness;
	double dbPipeInsulationThickness2;
	double dbPipeInsulationThickness3;
	float m_fSlopeMeetSectNodeCount;//焊接弯管斜接段节数
};

class AFX_EXT_CLS PipeFeature
{
public:
	PipeFeature();
	virtual ~PipeFeature(){};
	CString m_strMaterialName;//材料名
	int m_iCodeType; //应规种类
	double m_dbPipeOuterDiameter;//管道外径
	double m_dbPipeWallThickness; //壁厚
	double m_dbPipeWallThicknessReduce;//管子壁厚负公差
	double m_dbPipeWallThicknessAdd;//管子壁厚正公差
	double m_dbPipeCorrosionAllowance;//管子的腐蚀允许值
	double m_dbPipeInsulationThickness; //保温厚度1
	double m_dbPipeInsulationThickness2; //保温厚度2
	double m_dbPipeInsulationThickness3; //保温厚度3
	double m_dbPipeDensity;//管道密度
	double m_dbFluidDensity;//流体密度
	double m_dbInsulationDensity;//保温密度1
	double m_dbInsulationDensity2;//保温密度2
	double m_dbInsulationDensity3;//保温密度3
	double m_dbPoissonRation; //泊松比
	BOOL m_bIsGap; //是否有焊缝
	double m_dDn; //直径 PDMS->PSA接口中增加
	double m_dInsWeight; //保温重量 C5.3中用到
};

//给定弹簧荷载(AutoPSA1.1中的)
class AFX_EXT_CLS OldGivenSpringLoad  
{
public:
	OldGivenSpringLoad();
	virtual ~OldGivenSpringLoad(){};
	int	m_nGivenSpringLoadNodeNum;
	double	m_dbGivenSpringVerticalLoad;
};

//给定偶然荷载(AutoPSA1.1中的)
class AFX_EXT_CLS OldAccidentalLoad  
{
public:
	OldAccidentalLoad();
	virtual ~OldAccidentalLoad(){};
	double m_dbAccidentalLoadMomentZ;
	double m_dbAccidentalLoadMomentY;
	double m_dbAccidentalLoadMomentX;
	double m_dbAccidentalLoadForceZ;
	double m_dbAccidentalLoadForceY;
	double m_dbAccidentalLoadForceX;
	int m_nAccidentalLoadNodeNum;
};

//附加荷载(AutoPSA1.1中的)
class AFX_EXT_CLS OldAdditionalLoad  
{
public:
	OldAdditionalLoad();
	virtual ~OldAdditionalLoad(){};
	int m_nAdditionalLoadNodeNum;
	int m_nAdditionalLoadConditionNum; //工况号
	double m_dbAdditionalLoadMomentZ;
	double m_dbAdditionalLoadMomentY;
	double m_dbAdditionalLoadMomentX;
	double m_dbAdditionalLoadForceZ;
	double m_dbAdditionalLoadForceY;
	double m_dbAdditionalLoadForceX;
};


//直单元
class AFX_EXT_CLS StraightUnit  
{
public:
	StraightUnit();
	virtual ~StraightUnit(){}
	BOOL m_bBend;
	CBendData m_BendData;
};
//三通数据
class AFX_EXT_CLS ThreeOrSifsOpenData
{
public:
	ThreeOrSifsOpenData();
	virtual ~ThreeOrSifsOpenData(){};
	int m_iThreeNodeNumer;//三通点号
	int m_iThreeType;//三通类型(数据中的ID号=但是界面索引值+1)
	double m_dbSifi;//平面内的系数
	double m_dbSifo;//平面外的系数
	double m_dbWeld;//焊接标志
	double m_dbFillet;//修剪
	double m_dbPadThk;//鞍板厚度
	double m_dbFtgRo;//管接座加强段的外半径
	double m_dbCrotch;//热挤压三通过渡区外半径
	double m_dbWeldId;//焊接标志
	double m_dbB1;
	double m_dbB2;
};
//三通
class AFX_EXT_CLS ThreeOrSifsOpen
{
public:
	ThreeOrSifsOpen();
	virtual ~ThreeOrSifsOpen(){};
	BOOL m_bThreeOrSifsOpen;
	ThreeOrSifsOpenData m_ThreeOrSifsOpenData[2];
};

//存放三通点呀用户设置点号的应力增强系数(在获得点的应力增强系数用)
class AFX_EXT_CLS ThreeOrSifsCoef
{
public:
	ThreeOrSifsCoef();
	virtual ~ThreeOrSifsCoef(){}
	int m_iNodeNumer;
	double m_dbStressCoefi; //平面内
	double m_dbStressCoefo; //平面外
};
class  HighDisplay
{
public:
	CForcmntData  m_forcemnt;
	CDispData m_displacement;
	float m_allowstress;
	float m_factstress;
	int id[2];//corlor
	HighDisplay(){id[0]=id[1]=-1;}
};



class AFX_EXT_CLS ResTypeSever  
{
public:
	ResTypeSever(){};
	virtual ~ResTypeSever(){};
public:
	bool IsAngleRes(int iResId);
	bool IsNotLineRes( int iResId );
	bool IsXRes( int iResId );
	bool IsYRes( int iResId );
	bool IsZRes( int iResId );
};	
#endif