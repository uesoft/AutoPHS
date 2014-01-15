#ifndef OUTFUNCTION_H
#define OUTFUNCTION_H

#include <windef.h>
#include "PipeUnitData.h"
//CStringW 只有在VS2002以上的环境中才可以用。
#if _MSC_VER > 1200
typedef CList<CStringW,CStringW> strMatNameList;
#endif // _MSC_VER > 1200

typedef CList<CPipeUnitData *,CPipeUnitData *> mPipeUnitList;
struct MaxTable
{
  CString m_Value;
  INT m_Node;
  CString m_Name; 
  MaxTable(){m_Name="";m_Node=0;m_Value="";}
};



class __declspec(dllexport) CaseTable// 数字+字符串
{
public:
 CaseTable()
 {m_tableName=m_returnField=m_dependField="";
  m_NO=NULL;m_str=NULL;m_library=NULL;
  m_stat=0;
 }
 CaseTable(_ConnectionPtr,CString,CString,CString);
 VOID SetDataLibrary(_ConnectionPtr);
 VOID DesignTable(CString,CString,CString);
 VOID refresh();
 int GetNumericField(int);
 CString GetStringField(int);
 ~ CaseTable ();
protected:
	VOID SetCaseTable();
private:
	 _ConnectionPtr m_dataLibrary;
	 CString m_dependField;
	 CString m_tableName;
     CString m_returnField;
	 CString *m_str;
	 int *m_NO;
	 int m_stat;
	 BOOL m_library;
};




 extern "C" __declspec(dllexport) int GetAllowStressCase(_ConnectionPtr&,int**);///返回有许用应力的工况
 extern "C" __declspec(dllexport) BOOL GetStressForCase (_ConnectionPtr&,mPipeUnitList *,int);///按工况返回管系的对应节点的应力和许用应力
 __declspec(dllexport) MaxTable GetMaxWithTable(_ConnectionPtr&,CString ,int,CString ="LCASE_NUM",
														  CString ="OUTPUT_STRESSES");//按工况返回Table的最大值

 //__declspec(dllexport)VOID OperateCaseTable(CaseTable**,INT symbol);

 //0 上Z  1 下-Z  2东X  3西-X  4北Y  5南-Y
 typedef enum
 {
	 CsUp = 0,
		 CsDown = 1,
		 CsEast = 2,
		 CsWest = 3,
		 CsNorth = 4,
		CsSouth = 5
}CSType;

 int GetCaseTable(_ConnectionPtr&,CString**,int **,CString="CASE",CString ="LCASE_NUM",
														  CString ="OUTPUT_RESTRAINTS");//返回工况表格
extern "C" __declspec(dllexport) int ReadPSAFileToListNoTransfer(mPipeUnitList *m_pPipeUnitList,CPipeGlobalData *m_PipeGlobalData,CString cFileName,_ConnectionPtr pRefInfoCon);//没有将单位转化为标准单位
extern "C" __declspec(dllexport) int ReadPSAFileToList(mPipeUnitList *m_pPipeUnitList,CPipeGlobalData *m_PipeGlobalData,CString cFileName,_ConnectionPtr pRefInfoCon);
extern "C" __declspec(dllexport) void freeList(mPipeUnitList *pPipeUnitList);

extern "C" __declspec(dllexport) int WriteDataToPSAFile(mPipeUnitList *pPipeUnitList,CPipeGlobalData *pPipeGlobalData,CString strFileName,CString strmainVersion,CString strsubVersion,_ConnectionPtr pMaterialCon);//2007.04.26 lyf

//进行条件编译，只有在VS2002以上的编译环境中才用此接口
#if _MSC_VER > 1200
//支持宽字符的接口，为PDMS8.0调用
extern "C" __declspec(dllexport) void WriteDataToPSAFileW(mPipeUnitList *pPipeUnitList,CPipeGlobalData *pPipeGlobalData,CStringW strFileName,CStringW strmainVersion,CStringW strsubVersion,_ConnectionPtr pMaterialCon , strMatNameList &MatNameList);//2009.04.27 liuc
#endif // _MSC_VER > 1200

//单位转换
extern "C" __declspec(dllexport) void TransferData(mPipeUnitList *pPipeUnitList,CPipeGlobalData *pPipeGlobalData,_ConnectionPtr &m_UnitsCon);

//得到单位的比例,标签提示
extern "C" __declspec(dllexport) void GetUnitScaleTip(CPipeGlobalData * pGlobalData,_ConnectionPtr &m_UnitsCon,int iUnitsCode);

//计算点坐标
extern "C" __declspec(dllexport) void SetUnitCoordinate(mPipeUnitList *pPipeUnitList, CPipeGlobalData *PipeGlobalData);

extern "C" __declspec(dllexport) int WriteListToCmpFile(mPipeUnitList *pPipeUnitList,CPipeGlobalData *pPipeGlobalData,CString strFileName,_ConnectionPtr m_RefInfoCon,_ConnectionPtr pMaterialConn);
//坐标转换
extern "C" __declspec(dllexport) void ChangeCoordinate(_ConnectionPtr &RefInfoCon,mPipeUnitList *pPipeUnitList,CPipeGlobalData *PipeGlobalData,int axisFrom,int axisTo);

extern "C" __declspec(dllexport) void ExChangeCoordinate(_ConnectionPtr &RefInfoCon,mPipeUnitList *pPipeUnitList,CPipeGlobalData *PipeGlobalData,CSType XCs,CSType YCs,CSType ZCs);

//得到管系中出现过的所有工况荷载(包括:H弹簧;冷紧,水压,风,温度,压力,力/力矩,均布力,位移)
extern "C" __declspec(dllexport) void GetAllCaseLoadCode(CArray<CString,CString> &CaseLoadArray,mPipeUnitList *pPipeUnitList);

//查找链表中的错误数据
extern "C" __declspec(dllexport) void CheckDataError(CArray<CString,CString> &pErrArray,CArray<CString,CString> &pWarningArray,mPipeUnitList *pPipeList,CPipeGlobalData *pPipeGlobalData,_ConnectionPtr pRefInfoCon,_ConnectionPtr pMaterialCon,CString strFileName);

//写静力计算结果文件到表格中
extern "C" __declspec(dllexport) int WritStaOutFileToTable(CString strFileName,_ConnectionPtr &pRefInfoCon,_ConnectionPtr &pSpringCon);

//读PSA1.1的文件数据
extern "C" __declspec(dllexport) int ReadInFileToList(mPipeUnitList *pPipeList,CPipeGlobalData *pGlobalData,CString strFileName,_ConnectionPtr pRefInfoCon,_ConnectionPtr pMaterialCon,CArray<CString,CString> &pArray);
//写CII文件
extern "C" __declspec(dllexport) int WriteListToCiiFile(mPipeUnitList *pPipeUnitList,CPipeGlobalData const *pGlobalData,CString strFileName,_ConnectionPtr pRefInfoCon,_ConnectionPtr pMaterialCon,_ConnectionPtr pSpringCon);
extern "C" __declspec(dllexport) int WriteListToCiiFileEx(mPipeUnitList *pPipeUnitList,CPipeGlobalData const *pGlobalData,CString strFileName,_ConnectionPtr pRefInfoCon,_ConnectionPtr pMaterialCon,_ConnectionPtr pSpringCon);

//写.in文件
extern "C" __declspec(dllexport) int WriteListToInFile(int nOutUnitsType,int nSpringType,int nCodeType,int nCoefProofS,int nCoefProofB,
							 float fStressReduceRatio,float fAllowStressRatio5,float fAllowStressRatio6,
							 int nMaxSeriesNum,CString	strLoadChangeRatio,double dbRigidityType,double dbRigidityTimes,
							 float fEAffectRatio,int nOutPutStress,int nAddWaterWeight,
							 bool bIGK5,bool bIGK6,bool bIGK7,bool bIGK8,bool bIGK9,bool bIGK10,int nBendFlag,double dbWeldFactor,
							 mPipeUnitList *pPipeUnitList,CPipeGlobalData *PipeGlobalData,CString strFileName,_ConnectionPtr pRefInfoCon);

//查找当前单元的材料特性
extern "C" __declspec(dllexport) void FindUnitMaterialFeature(CPipeUnitData *pPipeUnitData,CPipeGlobalData* pGlobalData,_ConnectionPtr pMaterialCon);

//将所有单元老材料转换成新材料
extern "C" __declspec(dllexport) void ConversionNewMaterial(mPipeUnitList *pUnitList,_ConnectionPtr pMaterialCon,CArray<CString,CString> *pInfoArray=NULL);

//设置所有单元的密度，泊松比等数据
extern "C" __declspec(dllexport) void SetUnitDensityPoissOther(mPipeUnitList *pUnitList,_ConnectionPtr pMaterialCon);

//得到材料密度
extern "C" __declspec(dllexport) void FindMaterialDensityPoissOther(CString strMaterialName,_ConnectionPtr pMaterialCon,double &dbPipeDensity,double &dbPoisson);

//找用来存放节点约束数据的单元(在一个单元中描述不完一个点的特证,如PSA1.1中的176类型)
//pPipeUnitData:当前单元
//pRtPipeUnitData:返回值存放的单元; iIndex:存放位置
extern "C" __declspec(dllexport) void GetSaveNodeResTypeUnit(mPipeUnitList *pUnitList,CPipeUnitData *pPipeUnitData,CPipeUnitData *&pRtPipeUnitData,int &iIndex);


//合并弯单元(用于.in到.psa文件或PDARX生成.psa文件) iIWD: 1:时直单元的长度不包含弯管半 iMaxNodeNumr:最大点
extern "C" __declspec(dllexport) void UniteBendUnit(mPipeUnitList *pUnitList,int iIWD,int &iMaxNodeNumr);

//得到单元0度处的点号(当前单元的末点以步长1找)
extern "C" __declspec(dllexport) int GetBend0DegreeNode(mPipeUnitList * m_pUnitList,CPipeUnitData *pPipeUnitData);

//写支吊架数据到PHS(支吊架软件表格)中
extern "C" __declspec(dllexport) int WritePHSDataToTable(_ConnectionPtr pRefInfoCon,_Recordset *rsResult,CString strDataFileName,long imaxZdjh,long iFileNameID,long iSelVlmID,double dbRatioOfPjg2Pgz );

//转换GLif1.2ToGlif3.x文件格式
extern "C" __declspec(dllexport) int GLIFv12ToGLIF3x(CString strOldFileName,CString strNewFileName);

//CII4.5文件转换成PSA文件格式
extern "C" __declspec(dllexport) int CIIToPSAFile(CString strCiiFileName,CString strPSAFileName,
						_ConnectionPtr pRefInfo,_ConnectionPtr pSpringInfo,_ConnectionPtr pMaterialInfo,
						CArray<CString,CString> *pArray,CString strMainVerSion,CString strsubVersion);

extern "C" __declspec(dllexport) int ResultDataDistill(mPipeUnitList *pPipeUnitList,CPipeGlobalData *pPipeGlobalData,int iCase,_ConnectionPtr pConResult,double dbDispScale);
//改变位移放大比例, 条件:内存结构中一定比例的结果位移已算出来
extern "C" __declspec(dllexport) int SetResultDispScale(mPipeUnitList *pPipeUnitList,CPipeGlobalData *pPipeGlobalData,int iCase,_ConnectionPtr pConResult,double dbNewDispScale,double dbOldDispScale);

//得到与单元相连的单元
//i=1:查找与起点相连的单元; 2:与末点相连的单元
__declspec( dllexport ) CPipeUnitData *GetConnectUnit(mPipeUnitList *pPipeList,CPipeUnitData *pPipeUnitData,int i);

//得到弯单元的角度
__declspec( dllexport ) double GetBendUnitAngle(mPipeUnitList *pPipeList,CPipeUnitData *pPipeUnitData);

#endif
