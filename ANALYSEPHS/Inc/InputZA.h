// InputZA.h: interface for the InputZA class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPUTZA_H__584D1F3B_1243_4F04_BEB0_1A78AC288A43__INCLUDED_)
#define AFX_INPUTZA_H__584D1F3B_1243_4F04_BEB0_1A78AC288A43__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//ZA表中部分数据
class InputZA  
{
public:
	InputZA();
	virtual ~InputZA();
	bool GetifLongVertPipe();
	void SetifLongVertPipe(bool ifLongVertPipe);
	
private:
	bool m_ifLongVertPipe;//rsza->GetCollect(_T("ifLongVertPipe")

};

#endif // !defined(AFX_INPUTZA_H__584D1F3B_1243_4F04_BEB0_1A78AC288A43__INCLUDED_)
