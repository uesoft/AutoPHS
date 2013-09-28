// iSAhSel.h: interface for the CiSAhSel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISAHSEL_H__3D630240_6328_4B74_9026_92220E97E468__INCLUDED_)
#define AFX_ISAHSEL_H__3D630240_6328_4B74_9026_92220E97E468__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//iSAh,直接吊,0

class CiSAhSel  
{
public:
	CString GetiSAhSQL(CString &Ptype);
	CiSAhSel();
	virtual ~CiSAhSel();
protected:
	CString m_tmpSQL ;//Cphs.cpp(2496)tmpSQL当前的连接匹配查询字段信息
	CString m_SAfDPmax;//Cphs.cpp(2519)SAfDPmax
	float m_tmpSelPJG ;//Cphs.cpp(2459)tmpSelPJG累加零件重量后的计算荷载
	int	m_iNumPart;//Cphs.h(93)iNumPart

};

#endif // !defined(AFX_ISAHSEL_H__3D630240_6328_4B74_9026_92220E97E468__INCLUDED_)
