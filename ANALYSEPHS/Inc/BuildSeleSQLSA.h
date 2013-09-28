// BuildSeleSQLSA.h: interface for the BuildSeleSQLSA class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUILDSELESQLSA_H__05893DE5_BF95_4FC8_A546_C71DF3BCC99D__INCLUDED_)
#define AFX_BUILDSELESQLSA_H__05893DE5_BF95_4FC8_A546_C71DF3BCC99D__INCLUDED_

#include "AnalyseHangPara.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "AnalyseHangPara.h"
#include "AnalyseSAPara.h"
#include "AnalysePHSPara.h"
#include "PSAResult.h"
#include "DataBasePara.h"
#include "AnalysePAPara.h"

namespace PHS
{
	namespace PHSCalc
	{
		class BuildSeleSQLSA  
		{
		public:
//			CString GetiG100SQL(CString &Ptype);
//			CString GetiSALbraceFixG48SQL(CString &Ptype);
//			CString GetiGCementSQL(CString &Ptype);
//			CString GetiG52_55SQL(CString &Ptype);
//			CString GetiG51SQL(CString &Ptype);
//			CString GetiSACantileverSQL(CString &Ptype);
//			CString GetiSAhSQL(CString &Ptype);
			
		public:
			AnalyseHangPara *m_Para;
			AnalyseSAPara *m_SAPara;
			AnalysePHSPara *m_PHSPara;
			PSAResultPara *m_PSAPara;
			DataBasePara *m_DBPara;
			AnalysePAPara *m_PAPara;

			BuildSeleSQLSA();
			virtual ~BuildSeleSQLSA();
		private:
//			CString GetFixG48SQL(CString &Ptype);
//			CString GetSJ8SQL(CString &Ptype);
		};

	}
}

//modPHScal::giSAG100No=_ttoi(FrmTxsr.m_pViewTxsr->m_PagOption->m_csSAG100No);//CPhs4394д­ДњТы

#endif // !defined(AFX_BUILDSELESQLSA_H__05893DE5_BF95_4FC8_A546_C71DF3BCC99D__INCLUDED_)
