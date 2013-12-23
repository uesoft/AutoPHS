// BuildSeleSQLPA.cpp: implementation of the BuildSeleSQLPA class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BuildSeleSQLPA.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BuildSeleSQLPA::BuildSeleSQLPA()
{
	m_PAPara=NULL;
	m_Para=NULL;
	m_PHSPara=NULL;
}

BuildSeleSQLPA::~BuildSeleSQLPA()
{

}
//iPAh,Cphs.cpp(2942,2961)
//iPAXZ1_XZ3:
//CString BuildSeleSQLPA::GetiPAhSQL(CString &Ptype)
//{
//	CString strSQL=_T("");
//	CString sTmp=_T("");
//	int k=0;
//	CDaoRecordset rsX;//cg?
//	try
//	{
//		for (;k<= m_PAPara->gnDW_delta;k++)
//		{//从零部件数据表中找一个与当前支吊架管部外径m_PAPara->dj最接近的外径
//		 //最终外径误差不会超过m_PAPara->gnDW_delta/100。
//			sTmp.Format(_T("%g"),m_PAPara->dj*(1-k*0.01));
//			strSQL = _T("SELECT * FROM ") + m_PAPara->tbnPA + _T(" WHERE Dw>= ") + sTmp + _T(" AND Dw<= ") ;
//			sTmp.Format(_T("%g"),m_PAPara->dj * (1 + k * 0.01));
//			strSQL+=sTmp  + _T(" AND Pmax>=");
//			sTmp.Format(_T("%g"),m_Para->tmpSelPJG / m_Para->iNumPart);
//			strSQL+=sTmp  + _T(" AND Tj>=");
//			sTmp.Format(_T("%g"),m_PHSPara->t0);
//			strSQL+= sTmp + _T(" AND trim(CustomID)=\'") + Ptype + _T("\' ORDER BY tj,Dw,Pmax,Weight");
//			if(rsX.IsOpen())
//				rsX.Close();
//			rsX.Open(dbOpenSnapshot,strSQL);
//			if( rsX.IsEOF() && rsX.IsBOF() )
//				;
//			else
//				//没找到，每次外径递增1%，继续找
//				k=m_PAPara->gnDW_delta+1;		  
//		}
//	}
//	catch(_com_error e)
//	{
//	}
//	return strSQL;
//}
//
////iPAfixZ1 Cphs.cpp(2965,2966)管夹固定支架
////iPAfixZ2://焊接固定支架Z2
//CString BuildSeleSQLPA::GetiPAfixZ1(CString &Ptype)
//{
//	CString strSQL=_T("");
//	try
//	{
//	}
//	catch(_com_error e)
//	{
//	}
//	return strSQL;
//}
//
////iPAs://普通支座 Cphs.cpp(2971,3016)
////iPASS://滑动支座
////iPAGS://导向支座
//CString BuildSeleSQLPA::GetiPAsSQL(CString &Ptype)
//{
//	CString strSQL=_T("");
//	CString sTmp=_T("");
//	int k=0;
//	CDaoRecordset rsX;//cg?
//	try
//	{
//		for(;k<= m_PAPara->gnDW_delta;k++)
//		{
//			sTmp.Format(_T("%g"),m_PAPara->dj * (1 - k * 0.01));
//			strSQL = _T("SELECT * FROM ") + m_PAPara->tbnPA + _T(" WHERE Dw>= ") +sTmp+ _T(" AND Dw<= ") ;
//			sTmp.Format(_T("%g"),m_PAPara->dj * (1 + k * 0.01));
//			strSQL+= sTmp + _T(" AND Pmax>=");
//			sTmp.Format(_T("%g"),m_Para->tmpSelPJG/m_Para->iNumPart);
//			strSQL+=sTmp + _T(" AND Tj>=");
//			sTmp.Format(_T("%g"),m_PHSPara->t0);
//			strSQL+= sTmp + _T(" AND trim(CustomID)=\'") + Ptype + _T("\' ORDER BY tj,Dw,Weight");
//			if(rsX.IsOpen())
//				rsX.Close();
//			rsX.Open(dbOpenSnapshot,strSQL);
//			if( rsX.IsEOF() && rsX.IsBOF() );
//			else
//				//没找到，每次外径递增1%，继续找
//				k=m_PAPara->gnDW_delta+1;
//			
//		}
//		if(m_PAPara->gbPAForceZero && (rsX.IsEOF() && rsX.IsBOF()))
//		{
//			for( k = 0 ;k<= m_PAPara->gnDW_delta;k++)
//			{
//				sTmp.Format(_T("%g"),m_PAPara->dj * (1 - k * 0.01));
//				strSQL = _T("SELECT * FROM ") + m_PAPara->tbnPA + _T(" WHERE Dw>= ") +sTmp+ _T(" AND Dw<= ") ;
//				sTmp.Format(_T("%g"),m_PAPara->dj * (1 + k * 0.01));
//				strSQL+= sTmp + _T(" AND Pmax IS NULL");
//				strSQL+= _T(" AND Tj>=");
//				sTmp.Format(_T("%g"),m_PHSPara->t0);
//				strSQL+= sTmp + _T(" AND trim(CustomID)=\'") + Ptype + _T("\' ORDER BY tj,Dw,Weight");
//				if(rsX.IsOpen())
//					rsX.Close();
//				rsX.Open(dbOpenSnapshot,strSQL);
//				if( rsX.IsEOF() && rsX.IsBOF() )
//				{
//					;
//				}
//				else
//				{
//					k=m_PAPara->gnDW_delta+1;
//					AfxMessageBox("警告:\n\n管部允许荷载没有,\n程序将不验算管部荷载!");
//				}
//
//			}
//		}
//	}
//	catch(_com_error e)
//	{
//	}
//	return strSQL;
//}
//
////iPAD4LA:
////iPAD11LD:
////iPADLR:
////iPALX: Cphs.cpp(3023,3031)
//CString BuildSeleSQLPA::GetiPAD4LA(CString &Ptype)
//{
//	CString strSQL=_T("");
//	CString sTmp=_T("");
//	CDaoRecordset rsX;//cg?
//	try
//	{
//		//这些(烟风煤粉管道管部)与管径无关
//		strSQL = _T("SELECT * FROM ") + m_PAPara->tbnPA + _T(" WHERE Pmax>=") ;
//		sTmp.Format(_T("%g"),m_Para->tmpSelPJG/m_Para->iNumPart);
//		strSQL+=sTmp+ _T(" AND Tj>=");
//		sTmp.Format(_T("%g"),m_PHSPara->t0);
//		strSQL+=sTmp+_T(" AND trim(CustomID)=\'") + Ptype + _T("\' ORDER BY tj,Pmax,Weight");
//		if(rsX.IsOpen())
//			rsX.Close();
//		rsX.Open(dbOpenSnapshot,strSQL);
//	}
//	catch(_com_error e)
//	{
//	}
//	return strSQL;
//}
