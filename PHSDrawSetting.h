// PHSDrawSetting.h: interface for the PHSDrawSetting class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PHSDRAWSETTING_H__05888D89_1268_4AF4_BD69_90424CD88136__INCLUDED_)
#define AFX_PHSDRAWSETTING_H__05888D89_1268_4AF4_BD69_90424CD88136__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PHSSetting.h"

class __declspec( dllexport ) PHSDrawSetting : public PHSSetting  
{
public:
	PHSDrawSetting();
	virtual ~PHSDrawSetting();

public:
	virtual BOOL InitSetting();
	virtual BOOL SaveSetting();

public:
	//运行平台代号
	INT GetOperatingStation();
	void SetOperationgStation(INT nStation);

	//图幅尺寸
	INT GetFrameSize();
	void SetFrameSize(INT nSize);

	//绘制颜色
	CString GetDrawColor();
	void SetDrawColor(const CString& strColor);

	//前缀模式
	LONG GetGraphicFilePrefixMode();
	void SetGraphicFilePrefixMode(LONG lMode);

	//前缀字符串
	CString GetGraphicFilePrefix();
	void SetGraphicFilePrefix(const CString& strPrefix);

	//编号之间的距离
	INT GetMinDistanceBetweenSerialNum();
	void SetMinDistanceBetweenSerialNum(INT nDistance);

	//抑制提示-AutoCAD切换
	BOOL IsNotClewSwitchAutoCAD();
	void SetNotClewSwitchAutoCAD(BOOL bState);

	//抑制提示-目录覆盖
	BOOL IsNotClewCoverCatalog();
	void SetNotClewCoverCatalog(BOOL bState);

	//自动保存当前支吊架图纸
	BOOL IsAutoSave();
	void SetIsAutoSave(BOOL bState);

	//支吊架输出信息类型
	INT GetPHSOutputInfoStyle();
	void SetPHSOutputInfoStyle(INT nStyle);

	//支吊架号带工程和卷册号
	BOOL HasEnginIDAndVolumeID();
	void SetHasEnginIDAndVolumeID(BOOL bState);

	//支吊架起始图号
	LONG GetPHSStartDrawNum();
	void SetPHSStartDrawNum(LONG nNum);

	//图形版本号
	CString GetDWGVersion();
	void SetDWGVersion(const CString& strVersion);

	//不画会签栏
	BOOL IsNoDrawHuiQian();
	void SetNoDrawHuiQian(BOOL bState);

	//不画图框分区
	BOOL IsNoDrawFrameSubeara();
	void SetNoDrawFrameSubeara(BOOL bState);

	//不画图框外框
	BOOL IsNoDrawOutFrame();
	void SetNoDrawOutFrame(BOOL bState);

	//图框带有CAD标志
	BOOL HasCADSign();
	void SetHasCADSign(BOOL bState);

	//需要零件明细表
	BOOL HasPartList();
	void SetHasPartList(BOOL bState);

	//自动覆盖已有图形
	BOOL IsCoverDraw();
	void SetCoverDraw(BOOL bState);

	//清晰标注弹性件
	BOOL IsClearLabelSpring();
	void SetClearLabelSpring(BOOL bState);

	//标注管部定位尺寸
	BOOL IsLabelOrientationOfPA();
	void SetLabelOrientationOfPA(BOOL bState);
	
	//标注根部定位尺寸
	BOOL IsLabelOrientationOfSA();
	void SetLabelOrientationOfSA(BOOL bState);

	//图号手工输入
	BOOL IsManuInputDrawNum();
	void SetManuInputDrawNum(BOOL bState);

	//标注插入长度
	BOOL IsLabelInsertLength();
	void SetLabelInsertLength(BOOL bState);

	//标注"冷态"与"热态"标高
	BOOL IsLabelElevationOfColdAndHot();
	void SetLabelElevationOfColdAndHot(BOOL bState);

	//载荷位移表中载荷用绝对值表示
	BOOL IsAbsValue();
	void SetAbsValue(BOOL bState);

	//不标注冷热态柱子定位尺寸
	BOOL IsNoLabelOrientationOfPillar();
	void SetNoLabelOrientationOfPillar(BOOL bState);

	//标注重复件号
	BOOL IsLabelSamePartNum();
	void SetLabelSamePartNum(BOOL bState);

	//不画平面定位图
	BOOL IsNoDrawOrientationOfFlat();
	void SetNoDrawOrientationOfFlat(BOOL bState);

	//绘制焊缝符号
	BOOL IsDrawWelding();
	void SetDrawWelding(BOOL bState);

	//程序自动确定焊缝视图插入位置
	BOOL IsAutoFixPositionOfWelding();
	void SetAutoFixPositionOfWelding(BOOL bState);

	//绘制焊缝视图标注
	BOOL IsDrawViewOfWelding();
	void SetDrawViewOfWelding(BOOL bState);

	//自动确定焊缝符号方向和位置
	BOOL IsAutoFixPositionAndDirOfSignOfWelding();
	void SetAutoFixPositionAndDirOfSingOfWelding(BOOL bState);

	//巴威荷载位移表格式
	BOOL IsBaWeiTableFormate();
	void SetBaWeiTableFormate(BOOL bState);

	//图名不带支吊架号
	BOOL IsNoSupportIDInDraw();
	void SetNoSupportIDInDraw(BOOL bState);

	//零件数为1不绘单重
	BOOL IsNoDrawUnitWeightIfNumOne();
	void SetNoDrawUnitWeightIfNumOne(BOOL bState);

	//材料汇总表中标注弹簧厂家
	BOOL IsDrawManufactoryOfSpringInMaterial();
	void SetDrawManufactoryOfSpringInMaterial(BOOL bState);

	//巴威零件明细表
	BOOL IsBaWeiPartList();
	void setBaWeiPartList(BOOL bState);

	//编号用圆圈表示
	BOOL IsCircleDrawSerialNum();
	void SetCircleDrawSerialNum(BOOL bState);

	//巴威形式标高
	BOOL IsBaWeiElevation();
	void SetBaWeiElevation(BOOL bState);

	//编号圆圈直径(mm)
	DOUBLE GetDiameterOfCircleOfSerialNum();
	void SetDiameterOfCircleOfSerialNum(DOUBLE dbDiameter);

	//编号文字高度(mm)
	DOUBLE GetHeightOfCharacterofSerialNum();
	void SetHeightOfCharaterOfSerialNum(DOUBLE dbHeight);

	//零件明细表上方绘总重
	BOOL IsDrawWeightOnPartList();
	void SetDrawWeightOnPartList(BOOL bState);

	//零件明细表中标注弹簧厂家
	BOOL IsDrawManufactoryOfSpringInPartList();
	void SetDrawmanufactoryOfSpringInPartList(BOOL bState);

	//注明"安装"标高二字
	BOOL IsDrawAssembleCharactor();
	void SetDrawAssembleCharactor(BOOL bState);

	//材料表与零件明细表中的“组件”表示名
	CString GetNameOfComponent();
	void SetNameOfComponent(const CString& strName);

	//材料表与零件明细表中的“组件”表示名ID
	INT GetNameIDOfComponent();
	void SetNameIDOfcomponent(INT nID);

	//图框线型宽度设置
	DOUBLE GetLineWidthOfFrame();
	void SetLineWidthOfFrame(DOUBLE dbWidth);

	//表格线形宽度设置
	DOUBLE GetLineWidthOfTable();
	void SetLineWidthOfTable(DOUBLE dbWidth);

	//尺寸标注超出尺寸线范围
	DOUBLE GetBeyondScopeOfLabel();
	void SetBeyondScopeOfLabel(DOUBLE dbScope);

	////尺寸标注起点偏移量
	DOUBLE GetExcursionOfLabel();
	void SetExcursionOfLabel(DOUBLE dbExcursion);
};

_declspec(dllexport) PHSDrawSetting& GetCurDrawSetting();
#endif // !defined(AFX_PHSDRAWSETTING_H__05888D89_1268_4AF4_BD69_90424CD88136__INCLUDED_)
