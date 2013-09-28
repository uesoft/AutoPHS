// PSAResult.h: interface for the PSAResult class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PSARESULT_H__9E822B28_CD4F_4C74_8F0B_7C0AA28C63AF__INCLUDED_)
#define AFX_PSARESULT_H__9E822B28_CD4F_4C74_8F0B_7C0AA28C63AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//用来保存应力分析的结果数据中的荷载、位移、管径。
class PSAResultPara  
{
public:
	//固定支架计算模型坐标系的冷热态承受的力,kgf 已经进行坐标轴的转换(start)
	float pxlc;//modPHScal::pxlc;
	float pylc;//modPHScal::pylc;
	float pzlc;//modPHScal::pzlc;
	float pxrc;//modPHScal::pxrc;
	float pyrc;//modPHScal::pyrc;
	float pzrc;//modPHScal::pzrc;
	float mxlc;//modPHScal::mxlc;
	float mylc;//modPHScal::mylc;
	float mzlc;//modPHScal::mzlc;
	float mxrc;//modPHScal::mxrc;
	float myrc;//modPHScal::myrc;
	float mzrc;//modPHScal::mzrc;
	//固定支架计算模型坐标系的冷热态承受的力,kgf 已经进行坐标轴的转换(end)
	float	t0;;//温度
	
	PSAResultPara();
	virtual ~PSAResultPara();

};

#endif // !defined(AFX_PSARESULT_H__9E822B28_CD4F_4C74_8F0B_7C0AA28C63AF__INCLUDED_)
