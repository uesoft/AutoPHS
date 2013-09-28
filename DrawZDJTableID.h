// DrawZDJTableID.h: interface for the CDrawZDJTableID class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWZDJTABLEID_H__F65C49B4_8504_43D1_900A_9F9453921935__INCLUDED_)
#define AFX_DRAWZDJTABLEID_H__F65C49B4_8504_43D1_900A_9F9453921935__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDrawZDJTableID  
{
public:
	void DrawzdjTable(int index,bool bIsCom);
	CDrawZDJTableID();
	virtual ~CDrawZDJTableID();
	enum mnuZDJ_DRAWindex
	{
	   iZDJDRAWTag=0,
	   iZDJDRAWphs,
	   iZDJDRAWbom,
	   iZDJDRAWload,
	   iZDJDRAW180mm,
	   iZDJDRAW270mm,
	   iZDJDRAW705mm ,   //支吊架明细一览表
	   iZDJDRAW270pc,    //支吊架明细一览表(石化)
	   iZDJDRAW270L,//支吊架明细一览表(轻工)
	   iZDJDRAWEXECL,//
	   iZDJDRAWCatalogue,//目录
	   iZDJDRAWm_phs, //开始手工修改，实际上切换到m_phs层
	   iZDJDRAWCatalogueExcel,
	   iZDJDRAWXN180mm,    //西南院支吊架明细一览表(180mm,A4)
	   iZDJDRAWbomBJBW,//北京巴威零件明细表
	   iZDJDRAWloadBJBW,   //北京巴威支吊荷载位移表(202mm)
	   iZDJDRAW705mmHSY, //核四院支吊架明细一览表
	   iZDJDRAW705mmQGY,	 //青工院支吊架明细一览表
	   iZDJDRAW705mmHBYZ,   //华北冶建
	   iZDJDRAWBJHY01,//北京华宇格式<一>
	   iZDJDRAWBJHY02 //北京华宇格式<二>
	};
};

#endif // !defined(AFX_DRAWZDJTABLEID_H__F65C49B4_8504_43D1_900A_9F9453921935__INCLUDED_)
