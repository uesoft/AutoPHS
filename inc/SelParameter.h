// SelParameter.h: interface for the SelParameter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SELPARAMETER_H__C763413F_1000_48B9_9D0D_3BE7FE437BCB__INCLUDED_)
#define AFX_SELPARAMETER_H__C763413F_1000_48B9_9D0D_3BE7FE437BCB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



//管道参数
class PipePara
{
public:
	PipePara();
	virtual ~PipePara();
public:
	double GetDj()
	{
		return m_dj;
	}
	double GetT0()
	{
		return m_tmpT0;
	}
private:
	double m_dj; //管道外径  对应modPHScal::dj
	double m_tmpT0; //温度   对应modPHScal::t0
};

/*
参数基类
*/
class ParaBase
{

};

//选型用的参数
class IPAHParaC : public ParaBase
{
public:
	IPAHParaC();
public:
	double GetDj() //外径
	{
		return m_dj;
	}
	double GetT0() //外径
	{
		return m_tmpT0;
	}
	double GetSelPJG()
	{
		return m_dbtmpSelPJG;
	}
	int GetiNumPart()
	{
		return m_iNumPart;
	}
	double GetDWdelta()
	{
		return m_dbDWdelta;
	}
	const CString& GetCustomID()
	{
		return m_strCustomID;
	}
	double SetDj(double dbValue) //外径
	{
		m_dj = dbValue;
	}
	double SetT0(double dbValue) //外径
	{
		m_tmpT0 = dbValue;
	}
	void SetSelPJG(double dbValue)
	{
		m_dbtmpSelPJG = dbValue;
	}
	void SetNumPart(int iValue)
	{
		m_iNumPart = iValue;
	}
	void SetDWdelta( double dbValue )
	{
		m_dbDWdelta = dbValue;
	}

private:
	double m_dj; //管道外径  对应modPHScal::dj
	double m_tmpT0; //温度   对应modPHScal::t0
	double m_dbtmpSelPJG; //选型时用的荷载值
	int m_iNumPart; //部件的数量
	double m_dbDWdelta;//查找管径范围扩大,0~7(%)
	CString m_strCustomID; //对应Ptype(i)
};


#endif // !defined(AFX_SELPARAMETER_H__C763413F_1000_48B9_9D0D_3BE7FE437BCB__INCLUDED_)
