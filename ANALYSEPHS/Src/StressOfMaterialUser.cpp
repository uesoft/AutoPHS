// StressOfMaterialUser.cpp: implementation of the StressOfMaterialUser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StressOfMaterialUser.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

StressOfMaterialUser::StressOfMaterialUser()
{

}

StressOfMaterialUser::~StressOfMaterialUser()
{

}
void StressOfMaterial(CString Material,float temperature,float& Sigmat,int iMODE,float& Density)
{
//	CString SQLx,sTmp,strTbn,strFD;
//	COleVariant vTmp;
//	float t1,t2,Sigma1,Sigma2;
//	CDaoRecordset rsTmp;
//	Sigmat=0.0;
//	Density=0.0;
//	float Coef;
//	if(iMODE==0)
//	{
//		//查找材料在某温度temperature下的许用应力sigma,返回值单位unit:kgf/cm2,
//		//数据表中单位MPa(N/mm2)
//		//查找许用应力Sigmat,MPa->kgf/cm2
//		if(modPHScal::tbnAllowStress==_T("")) modPHScal::tbnAllowStress=_T("MechanicalOfMaterialSIGMAt");
//		strTbn=modPHScal::tbnAllowStress;
//		strFD=_T("SIGMA");
//		Coef=100.0/EDIBgbl::kgf2N;	
//	}
//	else if(iMODE==1)
//	{
//		//查找弹性模量Et,kN/mm,
//		strTbn=_T("MechanicalOfMaterialEt");
//		strFD=_T("Et");
//		Coef=1.0;	
//	}
//	else if(iMODE==2)
//	{
//		//查找线性膨胀系数Alphat,unit:10e-6/C
//		strTbn=_T("MechanicalOfMaterialALPHAt");
//		strFD=_T("ALPHAt");
//		Coef=1.0;	
//	}
//	else return;
//
//	try
//	{
//		rsTmp.m_pDatabase=&EDIBgbl::dbMaterial;//20071102 "dbSORT" 改为 "dbMaterial"
//		rsTmp.Open(dbOpenSnapshot,_T("SELECT * FROM [") + strTbn + _T("] WHERE Trim(material)=\'") + Trim(Material) + _T("\' ORDER BY t"));
//		if(rsTmp.IsBOF() && rsTmp.IsEOF())
//			;
//		else
//		{
//			//查找计算温度t0时的许用应力值Sigma1
//			SQLx.Format(_T("t>=%g"),temperature);
//			if(rsTmp.FindFirst(SQLx))
//			{
//				rsTmp.GetFieldValue(strFD,vTmp);
//				Sigma1 = vtof(vTmp);
//				rsTmp.GetFieldValue(_T("t"),vTmp);
//				t1 = vtof(vTmp);
//				if(iMODE==1)
//				{
//					rsTmp.GetFieldValue(_T("Density"),vTmp);
//					Density=vtof(vTmp);
//				}
//				//找到温度值,前移一个记录，其温度值肯定比计算温度低。
//				rsTmp.MovePrev();
//				if(rsTmp.IsBOF())
//				{
//					if(t1!=temperature)
//					{
//						//不可能出现这种情况
//						//在材料许用应力表%s中没找到>=%sC的温度值。
//						sTmp.Format(GetResStr(IDS_NotFoundTemperatureValueInTableAllowStress),_T("\%d"),modPHScal::tbnAllowStress,temperature);
//						throw sTmp;
//					}
//					else
//					{
//						Sigmat = Sigma1 *Coef;
//					}
//				}
//				else
//				{
//					rsTmp.GetFieldValue(strFD,vTmp);
//					Sigma2 = vtof(vTmp);
//					rsTmp.GetFieldValue(_T("t"),vTmp);
//					t2 = vtof(vTmp);
//					Sigmat = Sigma2 + (temperature - t2) * (Sigma1 - Sigma2) / (t1 - t2);
//					Sigmat = Sigmat *Coef;
//					if(iMODE==1)
//					{
//						rsTmp.GetFieldValue(_T("Density"),vTmp);
//						Density=vtof(vTmp);
//					}
//				}
//			}
//			else
//			{
//				//在材料许用应力表%s中没找到>=%sC的温度值。
//				//sTmp.Format(GetResStr(IDS_NotFoundTemperatureValueInTableAllowStress),_T("\%d"),modPHScal::tbnAllowStress,temperature);
//				//throw sTmp;
//				//取最大温度下的值作为查询值
//				rsTmp.MoveLast();
//				rsTmp.GetFieldValue(strFD,vTmp);
//				Sigmat = vtof(vTmp) *Coef;
//				if(iMODE==1)
//				{
//					rsTmp.GetFieldValue(_T("Density"),vTmp);
//					Density=vtof(vTmp);
//				}
//			}
//		}
//	}
//	catch(CDaoException * e)
//	{
//		e->ReportError();
//		e->Delete();
//	}
//	catch(CException *e)
//	{
//		e->Delete();
//	}
//	catch(...)
//	{
//	}
}
