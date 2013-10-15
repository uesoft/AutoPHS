// BuildSeleSQLSA.cpp: implementation of the BuildSeleSQLSA class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\INC\BuildSeleSQLSA.h"
#include "PHSuser.h"
#include "PHSDefine.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace PHS
{
	namespace PHSCalc
	{
		BuildSeleSQLSA::BuildSeleSQLSA()
		{
			m_Para=NULL;
			m_PHSPara=NULL;
			m_PSAPara=NULL;
			m_SAPara=NULL;
			m_PAPara=NULL;
			m_DBPara=NULL;
		}

		BuildSeleSQLSA::~BuildSeleSQLSA()
		{

		}
	}
}





