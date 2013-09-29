// PHSFactory.h: interface for the CPHSFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PHSFACTORY_H__6DE77BD1_4D4F_4A2B_A226_3C15E8353562__INCLUDED_)
#define AFX_PHSFACTORY_H__6DE77BD1_4D4F_4A2B_A226_3C15E8353562__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4290)

#include "PHSApp.h"
#include "..\Part.h"
#include "..\PartPA.h"
#include "..\PartRod.h"
#include "..\SupportHanger.h"
#include "..\PartSpring.h"
#include "..\PartConstSpring.h"
#include "..\PartSA.h"
#include "..\PartConnect.h"
#include "..\PartAttach.h"
#include "..\PartNut.h"
#include "..\PartJoin.h"
#include "..\PartProfiledBar.h"
#include "..\Standardofboltnut.h"
#include "..\Standardofclamp.h"
#include "..\standardofconnect.h"
#include "..\standardofconstspring.h"
#include "..\standardofsa.h"
#include "..\standardofspring.h"
#include "..\PartPAofSupport.h"

/*class Part;
class PartPA;
class PartRod;
class SupportHanger;
class PartSpring;
class PartConstSpring;
class PartSA;
class PartConnect;
class PartAttach;
class PartNut;
class PartJoin;
class PartProfiledBar;
class StandardOfBoltNut;
class StandardOfClamp;
class StandardOfConnect;
class StandardOfConstSpring;
class StandardOfSA;
class StandardOfSpring;
class PartPAOfSupport;*/
class CPartPAView;
class CPartRodView;
class CPartConnectView;
class CPartNutView;
class CPartAttachView;
class CPartSAView;
class CPartSpringView;
class CPartConstSpringView;
class CPartSAView;

class DllExport CPHSFactory  
{
public:
	BOOL CreateSupport(SupportHanger& support);
	BOOL CreateSupportString(SupportHanger& support);
	Part* CreatePart(CString strCustomID);
	Part* CreateSAAttach(const CString& strSpec);
	
	BOOL Save(SupportHanger& support);
	BOOL Save(PartString& string);
	BOOL Save(PartAttach& attach);
	BOOL Save(PartConnect& connect);
	BOOL Save(PartConstSpring& constSpring);
	BOOL Save(PartNut& nut);
	BOOL Save(PartPA& pa);
	BOOL Save(PartSA& sa);
	BOOL Save(PartSAAttach& saAttach);
	BOOL Save(PartSpring& spring);
	BOOL Save(PartRod& rod);
	BOOL Save(PartProfiledBar& part);


	BOOL InitPart(Part& part);

	BOOL InitStandard(StandardOfBoltNut& standard);
	BOOL InitStandard(StandardOfClamp& standard);
	BOOL InitStandard(StandardOfConnect& standard);
	BOOL InitStandard(StandardOfConstSpring& standard);
	BOOL InitStandard(StandardOfSA& standard);
	BOOL InitStandard(StandardOfSpring& standard);

	CString GetExpressionValue(const CString& strExpression, PartString& part);
	CString GetExpressionValue(const CString& strExpression, PartAttach& part);
	CString GetExpressionValue(const CString& strExpression, PartConnect& part);
	CString GetExpressionValue(const CString& strExpression, PartConstSpring& part);
	CString GetExpressionValue(const CString& strExpression, PartNut& part);
	CString GetExpressionValue(const CString& strExpression, PartPA& part);
	CString GetExpressionValue(const CString& strExpression, PartSA& part);
	CString GetExpressionValue(const CString& strExpression, PartProfiledBar& part);
	CString GetExpressionValue(const CString& strExpression, PartSpring& part);
	CString GetExpressionValue(const CString& strExpression, PartRod& part);
	CString GetExpressionValue(const CString& strExpression, PartPAOfFixSupport& part);

	BOOL ReadJoin(PartJoin* pJoin);
	CString LoadSpec(Part& part);

	CPHSFactory();
	virtual ~CPHSFactory();

	//获得该部件后面可以连接的部件名称
	CStringArray* LoadAfterPart(Part* pPrev, Part* pPrevPrev = NULL);

	//获取路中第一个部件可用的类型
	CStringArray* LoadFirtPartsForString(INT nIndex);
	
	//根部相关参数获取函数
	CStringArray* LoadSABH(const CString& strTableName,
						   const CString& strCustomID,
						   const CString& strPressFieldName,
						   DOUBLE dbLoad);
	CStringArray* LoadSAMaterial(const CString& strCustomID,
								 const CString& strObservation,
								 DOUBLE dbTemperature);
	CStringArray* LoadSAPosition(const CString& strTableName,
								 const CString& strCustomID,
								 const CString& strSpec);
	CString LoadSAPressFieldName(PartSA& part);
	
	//弹簧相关参数获取函数
	CStringArray* LoadSpringNutDia(const CString& strTableName,
								   const CString& strCustomID);

	//恒力弹簧相关参数获取函数
	CStringArray* LoadConstSpringNutDia(const CString& strTableName,
										const CString& strCustomID,
										DOUBLE dbLoad);
	CStringArray* LoadSpringDisplacement(const CString& strTableName,
										 const CString& strCustomID,
										 const CString& strSpec,
										 CArray<DOUBLE, DOUBLE>& theMin,
										 CArray<DOUBLE, DOUBLE>& theMax);

	CStringArray* LoadMaterial(const CString& strCustomID,
							   const CString& strObservation,
							   DOUBLE dbTemperature);
	CStringArray* LoadMaterial(const CString& strTableName,
							   const CString& strCustomID,
						   	   const CString& strSpec,
							   DOUBLE dbLoad,
							   DOUBLE dbTemperature);

	CStringArray* LoadBH(const CString& strTableName,
						 const CString& strCustomID,
						 DOUBLE dbLoad,
						 DOUBLE dbTemperature);
	CStringArray* LoadBH(const CString& strTableName,
						 const CString& strCustomID,
						 DOUBLE dbLoad);
	CStringArray* LoadBH(const CString& strTableName,
						 const CString& strCustomID);

	CStringArray* LoadConstSpringBH(const CString& strTableName,
									const CString& strNutTableName,
									const CString& strCustomID,
									DOUBLE dbLoad);

	CStringArray* LoadDiameter(const CString& strTableName,
							   const CString& strCustomID,
							   const CString& strSpec);
	
	CStringArray* LoadPADiameter(const CString& strTableName,
							   const CString& strCustomID,
							   const CString& strSpec);

	CStringArray* LoadDH(const CString& strTableName,
						 const CString& strCustomID,
						 const CString& strSpec);

	CStringArray* LoadSupports();

	CStringArray* LoadSpringRuleName();
	CStringArray* LoadSpringRule(const CString& strRuleName);

	BOOL LoadOthers(Part& part);

//输出各个表的记录集
public:
	_RecordsetPtr GetEngineSet();
	_RecordsetPtr GetEngineVolumeSet(const CString& strEngineID = CString());

private:
	CArray<Part*, Part*> m_PartAttach;//在读取支吊架数据时,对两个支吊架部件之间的附件对象的缓冲

	CString m_strManuSpringTable;//弹簧数据管理信息数据表表名
	CString m_strManuConstSpringTable;//恒力弹簧数据管理信息数据表表名
	CString m_strManuClampTable;//管部数据管理信息数据表表名
	CString m_strManuConnectTable;//连接件数据管理信息数据表表名
	CString m_strManuBoltNutTable;//螺栓螺母数据管理信息数据表表名
	CString m_strManuSATable;//根部数据管理信息数据表表名

private:
	void DeletePartAttachArray();
	BOOL ClearTable(const CString& strTable, const _ConnectionPtr& pConn);
	void CopyData(Part& Destination, Part& Source);
	PartRod* CreatePartRod(Part& pPart, const _RecordsetPtr& rs);
	PartPA* CreatePartPA(Part& part, const _RecordsetPtr& rs);
	PartSpring* CreatePartSpring(Part& part, const _RecordsetPtr& rs);
	PartConstSpring* CreatePartConstSpring(Part& part, const _RecordsetPtr& rs);
	PartConnect* CreatePartConnect(Part& part, const _RecordsetPtr& rs);
	PartSA* CreatePartSA(Part& part, const _RecordsetPtr& rs);
	PartAttach* CreatePartAttach(Part& part, const _RecordsetPtr& rs);
	PartNut* CreatePartNut(Part& part, const _RecordsetPtr& rs);
	PartProfiledBar* CreatePartProfiledBar(Part& part, const _RecordsetPtr& rs);
	BOOL ReadPartForString(PartString& string);
	
	HRESULT ReadDataFromWorkPrjDB(Part* pPart);

	void ReadCrudeDataForPart(const _RecordsetPtr& rs, PartSA& part) throw(_com_error);
	void ReadCrudeDataForPart(const _RecordsetPtr& rs, PartPA& part) throw(_com_error);
	void ReadCrudeDataForPart(const _RecordsetPtr& rs, PartProfiledBar& part) throw(_com_error);
	void ReadCrudeDataForPart(const _RecordsetPtr& rs, PartConnect& part) throw(_com_error);
	void ReadCrudeDataForPart(const _RecordsetPtr& rs, PartRod& part) throw(_com_error);
	void ReadCrudeDataForPart(const _RecordsetPtr& rs, PartAttach& part) throw(_com_error);
	void ReadCrudeDataForPart(const _RecordsetPtr& rs, PartNut& part) throw(_com_error);
	void ReadCrudeDataForPart(const _RecordsetPtr& rs, PartSpring& part) throw(_com_error);
	void ReadCrudeDataForPart(const _RecordsetPtr& rs, PartConstSpring& part) throw(_com_error);
	void ReadCrudeDataForPart(const _RecordsetPtr& rs, PartPAOfFixSupport& part) throw(_com_error);

	void ReadDataForObject(const _RecordsetPtr& rs, StandardOfClamp& standard);
	void ReadDataForObject(const _RecordsetPtr& rs, StandardOfSA& standard);
	void ReadDataForObject(const _RecordsetPtr& rs, StandardOfConnect& standard);
	void ReadDataForObject(const _RecordsetPtr& rs, StandardOfConstSpring& standard);
	void ReadDataForObject(const _RecordsetPtr& rs, StandardOfSpring& standard);
	void ReadDataForObject(const _RecordsetPtr& rs, StandardOfBoltNut& standard);

	HRESULT GetClassNameForPart(Part* pPart);
	void ReadPartData(Part* pPart, const _RecordsetPtr& rs);
	Part* CreateSupportPart(const _RecordsetPtr& rs);
	CStringArray* LoadTemperature(const CString& strTable, const CString& strID);
//	CStringArray* LoadBH(const CString& strTable, const CString& strID);
	CStringArray* LoadMaterial(const CString& strTable, const CString& strID);
	CStringArray* LoadMaterial(const CString& strTable, const CString& strID, const CString& strBH);
	CStringArray* LoadFieldValue(const CString& strSql);
	CStringArray* LoadFieldValue(const CString& strSql, const _ConnectionPtr& pConn);
	CStringArray* LoadFieldValue(const CString& strTable, const CString& strID, const CString& strField);
	BOOL IsSAAttach(PartSA* pPartSA, const _RecordsetPtr& rs);

	BOOL UpdateDB(const CString& strSql, const _ConnectionPtr& pConn);
	CString GetExpressionValue(const CString& strExpression, const CString& strTable);

	void InitPartSpec(Part& part);

	_RecordsetPtr GetInitRecordset(Part& part);
};

DllExport CPHSFactory& GetCurDatabase();
#endif // !defined(AFX_PHSFACTORY_H__6DE77BD1_4D4F_4A2B_A226_3C15E8353562__INCLUDED_)
