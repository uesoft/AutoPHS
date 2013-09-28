// PHSApp.h: interface for the CPHSApp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PHSAPP_H__2AEEAB99_3C46_4DBC_8422_B3FBBE5026FB__INCLUDED_)
#define AFX_PHSAPP_H__2AEEAB99_3C46_4DBC_8422_B3FBBE5026FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/*
#include "..\Standardofboltnut.h"
#include "..\Standardofclamp.h"
#include "..\standardofconnect.h"
#include "..\standardofconstspring.h"
#include "..\standardofsa.h"
#include "..\standardofspring.h"
#include "..\standardofAttach.h"
*/
#define DllExport __declspec( dllexport )

class DllExport CPHSApp  
{
public:
	void CloseConnection();
	void InitApplication();
	void SaveApplication();
	long GetCurProjectVolumeID();
	void SetCurProjectVolumeID(long iVlmID);
	const _ConnectionPtr& GetWorkPrjConnection();
	const _ConnectionPtr& GetZdjcrudeConnection();
	const _ConnectionPtr& GetSortConnection();
	const _ConnectionPtr& GetAllPrjDBConnection();

	CString GetWorkPrjDBPath();
	CString GetZdjcrudeDBPath();
	CString GetSortDBPath();
	CString GetAllPrjDBPath();

	INT GetCurZdjh();

	void SetCurZdjh(INT nZdjh);

	CString GetCurPATable();
	CString GetCurPAFixTable();

	CString GetCurSATable();

	CString GetCurSABoltNutTable();

	CString GetCurBoltNutTable();

	CString GetCurRodTable();

	CString GetCurConnectTable();

	CString GetCurProfiledBarTable();

	CString GetCurSpringTable();

	CString GetCurConstSpringTable();
	
	CString GetCurAttachTable();

	CString GetConstSpringRodDiameterTable();

	void SetCurStringNum(LONG nNum);
	LONG GetCurStringNum();

	//是否为安装后的第一次程序运行
	BOOL IsInstalledAndFirstRun(); 
	void SetInstalledAndFirstRun(BOOL bFirstRun);

	//程序是否已启动完毕
	bool IsStartedOnceAPHS();
	void SetStartedOnceAPHS(bool bStarted);

	//管部选择规范
	CString GetPhsPASel();
	void SetPhsPASel(CString strSel);

	//根部选择规范
	CString GetPhsSASel();
	void SetPhsSASel(CString strSel);

	//连接件选择规范
	CString GetPhsPARTSel();
	void SetPhsPARTSel(CString strSel);
	
	//弹簧选择规范
	CString GetPhsSPRINGSel();
	void SetPhsSPRINGSel(CString strSel);

	//恒力弹簧选择规范
	CString GetPhsConstSpringSel();
	void SetPhsConstSpringSel(CString strSel);
	
	//螺栓螺母选择规范
	CString GetPhsBoltsNutsSel();
	void SetPhsBoltsNutsSel(CString strSel);

	//读取最近工作支吊架规范
	void GetRecentWorkSPEC();
		
	//保存最近工作支吊架规范
	void SaveRecentWorkSPEC();

	//注册表中系统注册项名前缀
	//真是 shit,不知道描述
	CString GetRegAppPreName();
	void SetRegPreAppName(CString strAppName);

	CPHSApp();
	virtual ~CPHSApp();


private:
	_ConnectionPtr m_pConAllPrj;
	_ConnectionPtr m_pConWorkPrj;
	_ConnectionPtr m_pConSort;
	_ConnectionPtr m_pConZdjCrude;
	INT m_nCurStringNum;//当前路数
};

DllExport extern CPHSApp theAutoPhsApp;
DllExport CPHSApp* GetPHSApp();
DllExport BOOL IsNumber(CString& str);
#endif // !defined(AFX_PHSAPP_H__2AEEAB99_3C46_4DBC_8422_B3FBBE5026FB__INCLUDED_)
