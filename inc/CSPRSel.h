// CSPRSel.h: interface for the CSPRSel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CSPRSEL_H__0440E273_C345_4D45_9FC7_4BA215DB05C3__INCLUDED_)
#define AFX_CSPRSEL_H__0440E273_C345_4D45_9FC7_4BA215DB05C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class HDForceDist;
class HDCrude;
class LugDiaOfCSPR;
class CrudeTurnbuckleHDCz;
class CSPRSel  
{
public:
	CSPRSel();
	virtual ~CSPRSel();
	BOOL SelCSPRToTable(float& TmpPaz,float& tmpSelPJG4CSPRFiJ,long &mlfx,long &mlfx0);
private:
	//得到弹簧特性数据表满足要求的数据集
	void GetHDForceDist(HDForceDist *pDist,float tmpCapacity,float tmpDist,int iSumSerialNum); 
	//给恒吊赋值
	void SetCSPRValue(int iDH,double tmpDist);
	//得到弹簧数据表中满足要求的数据集
	void GetHDCrude(HDCrude *pHDCrude);
	//得到合适拉杆
	void GetLugDiaOfCSPR(LugDiaOfCSPR *pLug,double dbWeight,CString strCustomID,int iFjmin);
	//设置恒力弹簧之前的部件选型直径
	void SetAforeCompDia(int iFiJ);
	//用实际的数替换匹配式
	void ReplaceCNTB(LugDiaOfCSPR *pLugFik);
	//得到前一个零件的插入长度
	double GetPrePartInsertLen();
	//设置数据到TempCSLen表格中
	void SetTempCSLen(HDCrude *pHDCrude,double tmpDist,double tmpSelOpLoad);
	//设置数据到ZB表格中
	void SetResultZB(HDCrude *pCrude,LugDiaOfCSPR *pLugFik,long& mlfx,long& mlfx0);
	//得到实际尺寸
	double GetSigDim(HDCrude *pCrude,CrudeTurnbuckleHDCz *pCrudeTurn,LugDiaOfCSPR *pLugFik);

public:
	const CString& GettrtmpSQL0()
	{
		return m_strtmpSQL0;
	}
public:
	int m_iSEQofSPR; //当前恒力弹簧在模板中的恒力弹簧索引号
	int m_iSEQofPart; //当前恒力弹簧在模板中的部件索引号
	int m_i;          //当前部件的索引号
	int m_iFirstCal; //选型次数
	CString m_strPartID; //选型部件的ID号     
	CString m_strPrePartID; //前一部件的ID号 
	double m_dbSelPJG4CSPRPgz; //选型的工作荷载 对应PHS的tmpSelPJG4CSPRPgz
	int m_iSumSerialNum; //串联数
	CString m_strPtype; //选型部件的CustomID
	double m_dbSelPJG4CSPRFiJ; //恒吊松紧螺母直径计算载荷值
	int m_iNumPart; //恒力弹簧个数
	double m_dbtmpSelPJG; //其它零件计算载荷,恒力弹簧选型时，重量可能会累加上去
	CString m_strtmpSQL0; //尺寸连接匹配条件
	int m_inth; //当前路数
	double m_dbmlRot; //旋转角度
private: //内部用的
	double m_dbSelPJG4CSPRFiK;

};

#endif // !defined(AFX_CSPRSEL_H__0440E273_C345_4D45_9FC7_4BA215DB05C3__INCLUDED_)
