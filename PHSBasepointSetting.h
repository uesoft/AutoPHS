// PHSBasepointSetting.h: interface for the PHSBasepointSetting class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PHSBASEPOINTSETTING_H__ADC24A88_1B25_4AAE_9FF5_C8FB950137B6__INCLUDED_)
#define AFX_PHSBASEPOINTSETTING_H__ADC24A88_1B25_4AAE_9FF5_C8FB950137B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PHSSetting.h"

class __declspec( dllexport ) PHSBasepointSetting : public PHSSetting  
{
public:
	enum{
		eSUPPORTLIST,
			eSUPPORTNOLIST,
			eSINGLEHANGERLIST,
			eSINGLEHANGERNOLIST,
			eDOUBLEHANGERLIST,
			eDOUBLEHANGERNOLIST};
public:		
	enum{
		eCURPOSITION,
			eDEFAULTPOSITION,
			eOLDPOSITION};
		
public:
	PHSBasepointSetting();
	virtual ~PHSBasepointSetting();

public:
	virtual BOOL InitSetting();
	virtual BOOL SaveSetting();

public:
	//图框尺寸名称
	CString GetFrameName();
	void SetFrameName(const CString& strName);

	//基点类型,0表示支架,1表示单吊,2表示双吊
	INT GetBasePointStyle();
	void SetBasePointStyle(INT nStyle);
	void SetBasePointStyle(INT nSupportStyle, BOOL HasPartList);

	//左视图X坐标
	LONG GetCenterXOfLeftView();
	void SetCenterXOfLeftView(LONG lX);

	//左视图Y坐标
	LONG GetCenterYOfLeftView();
	void SetCenterYOfLeftView(LONG lY);

	//右视图X坐标
	LONG GetCenterXOfRightView();
	void SetCenterXOfRightView(LONG lX);

	//右视图Y坐标
	LONG GetCenterYOfRightView();
	void SetCenterYOfRightView(LONG lY);

	//注释块右下脚X坐标
	LONG GetRightBottomXOfNote();
	void SetRightBottomXOfNote(LONG lX);

	//注释块右下脚Y坐标
	LONG GetRightBottomYOfNote();
	void SetRightBottomYOfNote(LONG lY);

	//零件清单右下脚X坐标
	LONG GetRightBottomXOfPartList();
	void SetRightBottomXOfPartList(LONG lX);

	//零件清单右下脚Y坐标
	LONG GetRightBottomYOfPartList();
	void SetRightBottomYOfPartList(LONG lY);

	//载荷位移表右上脚X坐标
	LONG GetRightTopXOfLoadAndDisp();
	void SetRightTopXOfLoadAndDisp(LONG lX);

	//载荷位移表右上脚Y坐标
	LONG GetRightTopYOfLoadAndDisp();
	void SetRightTopYOfLoadAndDisp(LONG lY);

	LONG GetDimOffset();
	void SetDimOffset(LONG lOffset);

	LONG GetAttDOffsetX();
	void SetAttDOffsetX(LONG lX);

	LONG GetAttDOffsetY();
	void SetAttDOffsetY(LONG lY);

	LONG GetPt0X();
	void SetPt0X(LONG lX);

private:
	INT m_nStyle;
};

#endif // !defined(AFX_PHSBASEPOINTSETTING_H__ADC24A88_1B25_4AAE_9FF5_C8FB950137B6__INCLUDED_)
