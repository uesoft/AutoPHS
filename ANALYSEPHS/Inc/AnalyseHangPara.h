// InputSeleParaSA.h: interface for the InputSeleParaSA class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPUTSELEPARASA_H__E713D7FC_9671_4A69_90CF_C4CEF2AC850C__INCLUDED_)
#define AFX_INPUTSELEPARASA_H__E713D7FC_9671_4A69_90CF_C4CEF2AC850C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class AnalyseHangPara  
{
public:

//函数bool Cphs::GetphsBHandSizes(CFrmStatus& frmStatus,int  FirstCal, 
//int  MaxCalCount,int nth,float& TmpPaz,float& TmpPgz)内的局部变量.
 	CString m_strSelNumCS;//选择根部主槽钢数量的过滤字符串
	CString *m_Ptype;   //不含附件的组件结构CustomID数组
	float m_mvBWidth;     //固定支架根部主槽钢间距,cm,G47=根部表chdist字段值，G48=TZA表A01字段值
	float m_tmpSelPJG;//累加零件重量后的计算荷载
	CString m_tmpSQL ;          //当前的连接匹配查询字段信息
	
	CString m_tbn1;
	CString m_SAfDPmax;

	//类Cphs.cpp的数据成员(start)
	int m_iNumPart;//iNumPart 
	//类Cphs.cpp的数据成员(end)
public:
	CString GetstrSelNumCS();
	CString *GetPtype();
	float GetmvBWidth();
	float GettmpSelPJG();
	CString GettmpSQL();

	CString Gettbn1();
	CString GetSAfDPmax();
	int GetiNumPart();//iNumPart 
	
	void SetstrSelNumCS(const CString &strSelNumCS);
	void SetPtype(CString* Ptype);
	void SetmvBWidth(float mvBWidth);
	
	void SettmpSelPJG(float tmpSelPJG);
	void SettmpSQL(const CString &tmpSQL);
	void Settbn1(const CString &tbn1);
	void SetSAfDPmax(const CString &SAfDPmax);
	void SetiNumPart(int iNumPart);//iNumPart 

public:
	AnalyseHangPara();
	virtual ~AnalyseHangPara();

};

#endif // !defined(AFX_INPUTSELEPARASA_H__E713D7FC_9671_4A69_90CF_C4CEF2AC850C__INCLUDED_)
