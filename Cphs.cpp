// Cphs.cpp
//

#include "stdafx.h"
#include "Cphs.h" 
#include "modPHScal.h" 
#include "EDIBgbl.h"
#include "EDIBDB.h" 
#include "user.h"
#include "frmStatus.h"
#include "PhsStart.h"
#include "PhsData.h"//
#include <math.h>
#include "FrmPhsSamp.h"
#include "basDirectory.h"
#include "FrmTxsr.h"
#include "FrmPhsSamp.h"
#include "ModEncrypt.h"
#include "ModRegistry.h" 
#include "EDibacad.h"
#include "resource.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//using namespace std;

Cphs	* Cavphs;
int vRCount;
//下两行不可取消，因为函数声明中不能直接使用枚举型变量
int Cphs::MODEoverwrite=-1;
int Cphs::AttachI=iAttachIncluded;

CString Cphs::GetBH(const CString& sBHFormat)
{
	int iPos;
	CString strRet;
	iPos = sBHFormat.Find(")");
	if(iPos > 0)
	{
		strRet = sBHFormat.Left(iPos + 1);	
	}
	else
	{
		strRet = sBHFormat;	
	}
	return strRet;
}

float Cphs::SATotalWeight()
{
	return this->mvarSATotalWeight;
}

bool Cphs::bFirstPartIsPA()
{
	CString sTmp;
	sTmp.Format(_T("%d"),iPA);
	CString temp=CString(_T("Index=")) + sTmp + _T(" AND trim(CustomID)=\'") + Trim(Cntb) + _T("\'");
	
	try
	{
		if(!rsID.IsOpen())
		{
			EDIBgbl::SQLx = _T("SELECT * FROM PictureClipData");
			rsID.m_pDatabase=&EDIBgbl::dbPRJ;
			rsID.Open(dbOpenDynaset,EDIBgbl::SQLx);
			brsIDStatus=TRUE;
		}
		if (!rsID.FindFirst(temp))
			return false;
		else
			return true;
	}
	catch(::CDaoException * e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}
}

long Cphs::SetPhsPATypeToListBox()
{
	//目的:将满足条件的所有管部或根部或连接件型式加入列表框控件ResultObj,以便选择
	//Input:iSAPAConnect(iSA/iPA/iConnectPART)
	//Output:满足条件的所有管部或根部或连接件型式加入列表框控件ResultObj
	//返回:可用零件个数
	CString sSQL;
	long ret;
	CDaoRecordset rs(&EDIBgbl::dbPRJ);
	try
	{
		int	i;
		if(!rsID.IsOpen())
		{
			EDIBgbl::SQLx = _T("SELECT * FROM PictureClipData");
			rsID.m_pDatabase=&EDIBgbl::dbPRJ;
			rsID.Open(dbOpenDynaset,EDIBgbl::SQLx);
			brsIDStatus=TRUE;
		}
		//默认为管部
		if (iSAPAConnect==-1) iSAPAConnect=iPA;
		//只能列出管部与根部
		if ((iSAPAConnect!=iPA)&&(iSAPAConnect!=iSA))
		{
			ShowMessage(GetResStr(IDS_CannotSelectConnectAtThisStage));
			return -1;
			//throw;
		}
		else
		{			
			if (iSAPAConnect==iPA) 				
				//管部
				if((iChkCondition & iOnlyPAh)!=0)
					//吊架
					sSQL.Format(_T("SELECT DISTINCT CustomID FROM PictureClipData WHERE CustomID IN (SELECT DISTINCT CustomID FROM tmpCustomIDPA) AND Index=%d AND (ClassID=%d OR ClassID=%d OR ClassID=%d OR ClassID=%d OR ClassID=%d OR ClassID=%d ) "),
					iSAPAConnect,
					iPAh,
					iPAD4LA,
					iPAD4LQ,
					iPAD11LD,
					iPALX,
					iPADLR
					);
				else
					//支架
					sSQL.Format(_T("SELECT DISTINCT CustomID FROM PictureClipData WHERE CustomID IN (SELECT DISTINCT CustomID FROM tmpCustomIDPA) AND Index=%d AND NOT (ClassID=%d OR ClassID=%d OR ClassID=%d OR ClassID=%d OR ClassID=%d OR ClassID=%d ) "),
					iSAPAConnect,
					iPAh,
					iPAD4LA,
					iPAD4LQ,
					iPAD11LD,
					iPALX,
					iPADLR
					);
				else
					//根部,暂时未处理
					;
		}
		if(sSQL==_T("")) return 0;
		if(rs.IsOpen())
			rs.Close();		
		rs.Open(dbOpenSnapshot,sSQL);
		
        if (rs.IsBOF() && rs.IsEOF())
		{
			ShowMessage(GetResStr(IDS_NoAnyRecordInPictureClipData));
			return 0;
		}
		else
		{
			try{ COleVariant vTmp; 
			//COleVariant vTmp;
			ResultObj->ResetContent();
			i=0;
			RsDeleteAll(rsUnCheckedType);
			while(!rs.IsEOF())
			{
				i++;
				rs.GetFieldValue(_T("CustomID"),vTmp);
				//	ResultObj->AddString(vtos(vTmp));
				//	rs.MoveNext();
				// try{
				/*if (EDIBgbl::tdfExists(EDIBgbl::dbPRJ,_T("rsUnCheckedType")))
				EDIBgbl::dbPRJ.Execute(_T("DELETE * FROM rsUnCheckedType"));
				else 
				EDIBgbl::dbPRJ.Execute(_T("CREATE TABLE rsUnCheckedType (CustomID TEXT (20))"));*/
				
				//	   for(i=0;i<ResultObj->GetCount();i++)
				//	   {
				//		   ResultObj->GetText(i,temp);
				rsUnCheckedType.AddNew();
				//		   vTmp.TrimLeft();Vtmp.TrimRight();
				rsUnCheckedType.SetFieldValue(_T("CustomID"),vTmp);//STR_VAR(vTmp));
				rsUnCheckedType.Update();
				//EDIBgbl::dbPRJ.Execute(_T("INSERT INTO rsUnCheckedType (CustomID) VALUES ('")+Trim(temp)+_T("')"));
				// }
				//  return i;
				rs.MoveNext();
			}
			}
			catch(::CDaoException * e)
			{
				e->ReportError();
				e->Delete();
				return 0;
			}
			
			//			}
			ret= i;
			//把初步获得的可用零件写到临时表
			//			i=SaveRsUnCheckedTypeFromResultObj();
			return ret;
		}
	}
	catch(::CDaoException * e)
	{
		e->ReportError();
		e->Delete();
		return (long)0;
	}
}

long Cphs::SetPhsTypeToListBox()
{
	//目的:可用支吊架零件加入到列表框控件ResultObj(显示的值要->CustomID字段值)
	//     根据选择的连接件型式Cntb和CntbP(CustomID值),确定与之相连的组件,
	//     并将可供选择的(过滤掉不符合当前标准的)型式加入列表框控件ResultObj,以便选择
	//Input:零件Cntb
	//      CntbP(CntbP代表Cntb的前一个零件,可为空)
	//Output:所有可与Cntb(及CntbP)相连的零件都加入到列表框控件ResultObj(显示的值要->CustomID字段值)
	//返回:可用零件个数
	//注意:connect表中Cntb、Cnte、Cntn字段内容为ID,不是CustomID;
	//     PhsStructureREF表中CustomID字段内容为CustomID.
	//CDaoRecordset rs((CDaoDatabase *)&EDIBgbl::dbPRJ);
	//CDaoRecordset rsUnCheckedType((CDaoDatabase *)&EDIBgbl::dbPRJ);
	
	CDaoRecordset rs(&EDIBgbl::dbPRJ);
	CString FldName;
	CString tmp,sSQL;
	long ret;
	long i,mvClassID;
	
	try
	{
		//假设3个零件要同时匹配
		if(!rsID.IsOpen())
		{
			EDIBgbl::SQLx = _T("SELECT * FROM PictureClipData");
			rsID.m_pDatabase=&EDIBgbl::dbPRJ;
			rsID.Open(dbOpenDynaset,EDIBgbl::SQLx);
			brsIDStatus=TRUE;
		}
		if (Cntb==_T(""))
		{
			//管部置为空，说明正在选择管部
			return SetPhsPATypeToListBox();
		}
		//管部不置为空，说明不在选择管部，选择连接件或根部
		if (CntbP==_T(""))
		{
			//连续2个零件连接匹配
			//sSQL=CString(_T("SELECT * FROM connect WHERE trim(cntb)=\'"))
			//	+modPHScal::sFindID(Trim(Cntb)+_T("\'"));
			sSQL=CString(_T(" trim(cntb)=\'"))+modPHScal::sFindID(Trim(Cntb))+_T("\'");
			//rsConnect.m_strFilter=sSQL;
			//rsConnect.Requery();
			//rs.Open(dbOpenSnapshot,sSQL,0);
			FldName = _T("cnte");
		}
		else
		{
			sSQL=CString(_T(" trim(cntb)=\'"))
				+modPHScal::sFindID(Trim(CntbP))
				+_T("\' AND Trim(cnte)=\'")
				+modPHScal::sFindID(Trim(Cntb))
				+_T("\' AND Trim(Cntn)<>\'\'");
			if (!rsConnect.FindFirst(sSQL))
			{            
			/*sSQL=CString(_T("SELECT * FROM connect WHERE trim(cntb)=\'"))
			+modPHScal::sFindID(Trim(Cntb))+_T("\'");
				rs.Open(dbOpenSnapshot,sSQL,0); */           
				sSQL=CString(_T(" trim(cntb)=\'"))
					+modPHScal::sFindID(Trim(Cntb))+_T("\'");
				//rsConnect.m_strFilter=sSQL;
				//rsConnect.Requery();
				FldName = _T("cnte");
			}
			else
			{				
				FldName = _T("cntn");//连续3个零件连接匹配
			}
		}
		//是否需要弹簧?
		if (!rsConnect.FindFirst(sSQL))
		{
			//ShowMessage(GetResStr(IDS_NoCustomIDinConnect));
			return 0;
			//throw;
		}
		
		//保存结果到m
		CString tbn;
		bool bAdd;
		COleVariant vTmp;
		ResultObj->ResetContent();
		i=0;
		modPHScal::ZdjType=vtos(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("TYPE")));
		do
		{
			i++;
			rsConnect.GetFieldValue(FldName,vTmp);
			//CString str = rsConnect.GetSQL();
			tmp=vtos(vTmp);
			tmp.TrimLeft();tmp.TrimRight();tmp.MakeUpper();
			
			if (rsID.FindFirst(_T("trim(ID)=\'")+tmp+_T("\'")))
			{
				//找到相应的ID
				rsID.GetFieldValue(_T("CustomID"),vTmp);
				tmp=vtos(vTmp);
				tmp.TrimLeft();tmp.TrimRight();tmp.MakeUpper();
				i=ResultObj->FindStringExact(-1,tmp);
				//i=SendMessage(ResultObj->m_hWnd,LB_FINDSTRINGEXACT,0,(long)vTmp.lVal);
				//该部件是否可加入_T("可用框"),bAdd=true可加入
				rsID.GetFieldValue(_T("ClassID"),vTmp);
				mvClassID=vtoi(vTmp);
				
				if (mvClassID==iCSPR)
				{
					//恒力弹簧是否加入,
					if (modPHScal::ZdjType.GetLength()==4)
					{
						if (modPHScal::ZdjType==_T("9999") || modPHScal::ZdjType==_T("5000"))
							bAdd=true;
						else
							bAdd=false;
					}
					else
						bAdd=false;
				}
				else if (mvClassID==iSPR)
				{
					if (modPHScal::ZdjType.GetLength()==4)
					{
						if (modPHScal::ZdjType!=_T("9999") && modPHScal::ZdjType!=_T("5000")) 
							bAdd=true;
						else
							bAdd=false;
					}
					else
						bAdd=false;
				}
				else
				{
					//not spring and not Constant spring
					//即使没找到i=-1,还要判断它是否属于当前支吊架规范
					CString SQLx;
					int iIndex;
					rsID.GetFieldValue(_T("Index"),vTmp);
					iIndex=vtoi(vTmp);
					if(iIndex== iPA)
						SQLx.Format(_T("SELECT CustomID FROM tmpCustomIDPA WHERE CustomID=\'%s\'"),tmp);						
					else if(iIndex== iConnectPART)
						SQLx.Format(_T("SELECT CustomID FROM tmpCustomIDPART WHERE CustomID=\'%s\'"),tmp);						
					else if(iIndex== iSA)
						SQLx.Format(_T("SELECT CustomID FROM tmpCustomIDSA WHERE CustomID=\'%s\'"),tmp);
					if(rs.IsOpen()) rs.Close();
					rs.Open(dbOpenSnapshot,SQLx);
					if(rs.IsBOF() && rs.IsEOF())
						//该零部件不存在于当前规范,不能加入列表框,防止误选
						bAdd=false;
					else
						bAdd=true;
				}
				
				if ((i==-1)&&bAdd) ResultObj->AddString(tmp);
			}//if block
		}
		while(rsConnect.FindNext(sSQL));
		//把初步获得的可用零件写到临时表RsUnCheckedPart
		ret=i;
		i=SaveRsUnCheckedTypeFromResultObj();		  
   }//try block
   catch(::CDaoException * e)
   {
	   e->ReportError();
	   e->Delete();
   }
   return ret;
   return 0;
}

long Cphs::SetPhsPreviousTypeToListBox()
{
	//根据选择的连接件型式Cntb和CntbP,确定在其前面与之相连的组件,
	//并将可供选择的型式加入列表框控件ResultObj,以便选择
	//Input:零件Cntb,CntbP(CntbP代表Cntb的前一个零件,可为空)
	//Output:所有可与Cntb(及CntbP)相连的前一个零件都加入到列表框控件ResultObj
	//返回:可用零件个数
	
	
	/*   	CDaoRecordset rs((CDaoDatabase *)&EDIBgbl::dbPRJ);
	CDaoRecordset rsUnCheckedType((CDaoDatabase *)&EDIBgbl::dbPRJ);
	bool bNullCntn=false;
	CString tmp,ret;
	int i;
	try{
	if ( (IsNull(_variant_t(mvarCntb))) || (mvarCntb.IsEmpty()) )
	throw;//Err.Raise iUE_NoSetCntbToCphs, , ResolveResString(iUE_NoSetCntbToCphs)
	else{
	if ( (IsNull(_variant_t(mvarCntbP))) || (mvarCntbP.IsEmpty())) {
				EDIBgbl::SQLx=_T("SELECT * FROM connect WHERE trim(cnte)='");
				EDIBgbl::SQLx+=modPHScal::sFindID(Trim(mvarCntb))+_T("'");
				bNullCntn=true;
				}
				else{
				EDIBgbl::SQLx=_T("SELECT * FROM connect WHERE trim(cnte)='");
				EDIBgbl::SQLx+=modPHScal::sFindID(Trim((LPCTSTR)CntbP))+_T("\' AND trim(cntn)=\'");
				EDIBgbl::SQLx+=modPHScal::sFindID(Trim((LPCTSTR)mvarCntb))+_T("\' AND Trim(Cntn)<>\'\'");
				rs.Open(AFX_DAO_USE_DEFAULT_TYPE,EDIBgbl::SQLx,0);
				if ((rs.IsEOF())&&(rs.IsBOF())){
				bNullCntn=true;
				EDIBgbl::SQLx=_T("SELECT * FROM connect WHERE trim(cnte)='")
				+modPHScal::sFindID(Trim(mvarCntb))+_T("\'");
				}
				}
				}//else block
				rs.Open(AFX_DAO_USE_DEFAULT_TYPE,EDIBgbl::SQLx);
				if ( (rs.IsEOF())&&(rs.IsBOF()))
				throw;//Err.Raise iUE_NoCustomIDinConnect, , ResolveResString(iUE_NoCustomIDinConnect)
				else if (!rs.IsEOF()){
				rs.MoveLast();
				rs.MoveFirst();
				ret=rs.GetRecordCount();
				}
				ResultObj->ResetContent();
				COleVariant vTmp;
				while(!rs.IsEOF()){
				if (bNullCntn){
				
				  rs.GetFieldValue(_T("cntb"),vTmp);
				  tmp=modPHScal::sFindCustomID(Trim((char *)vTmp.pbVal));
				  i=ResultObj->FindStringExact(-1,tmp);
				  if (i==LB_ERR) ResultObj->AddString(tmp);
				  }
				  else{
				  rs.GetFieldValue(_T("cntn"),vTmp);
				  if ((vTmp.vt!=VT_NULL) && (Trim((char *)vTmp.pbVal)!=_T(""))){
				  rs.GetFieldValue(_T("cntb"),vTmp);
				  tmp=modPHScal::sFindCustomID(Trim((char *)vTmp.pbVal));
				  i=ResultObj->FindStringExact(-1,tmp);
				  if (i==-1) ResultObj->AddString(tmp);
				  }//if block
				  }
				  rs.MoveNext();
				  }//while block
				  rs.Close();
				  //把初步获得的可用零件写到临时表
				  i=SaveRsUnCheckedTypeFromResultObj();
				  }//try block
				  catch(::CDaoException * e)
				  {
				  e->ReportError();
				  e->Delete();
				  }
	return ret;*/
	return 0;
}

long Cphs::SaveRsUnCheckedTypeFromResultObj()
{
	int i;
	CString temp;
	try{
	   /*if (EDIBgbl::tdfExists(EDIBgbl::dbPRJ,_T("rsUnCheckedType")))
	   EDIBgbl::dbPRJ.Execute(_T("DELETE * FROM rsUnCheckedType"));
	   else 
		EDIBgbl::dbPRJ.Execute(_T("CREATE TABLE rsUnCheckedType (CustomID TEXT (20))"));*/
		RsDeleteAll(rsUnCheckedType);
		for(i=0;i<ResultObj->GetCount();i++)
		{
			ResultObj->GetText(i,temp);
			rsUnCheckedType.AddNew();
			temp.TrimLeft();temp.TrimRight();
			rsUnCheckedType.SetFieldValue(_T("CustomID"),STR_VAR(temp));
			rsUnCheckedType.Update();
			//EDIBgbl::dbPRJ.Execute(_T("INSERT INTO rsUnCheckedType (CustomID) VALUES ('")+Trim(temp)+_T("')"));
		}
		return i;
		
	}
	catch(::CDaoException * e)
	{
		//e->ReportError();
		e->Delete();
		return 0;
	}
}

long Cphs::SetPhsCheckedTypeToListBox()
{
	//目的：在列表框放入可用的零件(CustomID值)
	//根据SetphsTypeToListBox初步选择的支吊架,确定确实存在的可与之相连的组件,
	//并将可供选择的型式加入列表框控件ResultObj,以便选择
	//Input:'SourcObj列表框
	//      'ResultObj列表框
	//      'rsUnCheckedType临时表
	//'处理:1.SourceObj列表框listindex及其之前的零件与rsUnCheckedType中的某个零件可构成一个临时支吊架组合(但是其后部可能是不完整的),将其写入到rsTmpREF临时表
	//     '2.调用SavephsStructRsTmpREFToTZB,将临时表rsTmpREF写到mvarRsTZB
	//     '3.删除临时表rsTmpREF
	//     '4.调用GetphsBHandSizesTest,确定是否能找到满足条件的零件
	//     '5.如果成功,则将可用零件放到临时表rsTmpCheckedType
	//     '6.转到1循环直到rsUnCheckedType尾
	//     '7.将rsTmpCheckedType内容覆盖ResultObj列表框,其中就是满足条件的可用零件
	//'Output:可用零件都加入到列表框控件ResultObj
	//'返回:可用零件个数
	//'rsTmpCheckedType及rsTmpREF--表中内容为CustomID
	//CDaoRecordset rs((CDaoDatabase *)&EDIBgbl::dbPRJ);
	//CDaoRecordset rsUnCheckedType((CDaoDatabase *)&EDIBgbl::dbPRJ);
	CDaoRecordset rsTmpCheckedType((CDaoDatabase *)&EDIBgbl::dbPRJ);
	
	//CDaoRecordset rsTZB,rsTZC,rsTZF;
	CString temp;
	int j=0,m=0;
	long boolk=0 ;
	try{
		//EDIBgbl::SQLx=_T("SELECT * FROM rsUnCheckedType");
		//rsUnCheckedType.Open(dbOpenSnapshot,EDIBgbl::SQLx);
		InitListRs();
		if ((rsUnCheckedType.IsEOF())&&(rsUnCheckedType.IsBOF()))
		{
			::ShowMessage(GetResStr(IDS_NullRsUnCheckedTypeIndbPRJ));
			goto errH;
		}
		//throw;//Err.Raise iUE_NullRsUnCheckedTypeIndbPRJ, , ResolveResString(iUE_NullRsUnCheckedTypeIndbPRJ)
		else if (!rsUnCheckedType.IsEOF()){
			rsUnCheckedType.MoveLast();
			rsUnCheckedType.MoveFirst();
			//'清除临时表rsTmpCheckedType,准备存放检查的结果
			if (EDIBgbl::tdfExists(EDIBgbl::dbPRJ,_T("rsTmpCheckedType")))
				EDIBgbl::dbPRJ.Execute(_T("DELETE * FROM rsTmpCheckedType"));
			else
				EDIBgbl::dbPRJ.Execute(_T("CREATE TABLE rsTmpCheckedType (CustomID char(20))"));
			MakeRsTZB(idbPRJ, _T("rsTmp"));
			for(m=0;m<rsUnCheckedType.GetRecordCount();m++){
				//清除临时表trsTmpREF,准备存放选择的组合结构链
				if (EDIBgbl::tdfExists(EDIBgbl::dbPRJ,_T("rsTmpREF")))
					EDIBgbl::dbPRJ.Execute(_T("DELETE * FROM rsTmpREF"));
				else
					EDIBgbl::dbPRJ.Execute(_T("CREATE TABLE rsTmpREF (CustomID char(20))"));
				if (iSAPAConnect!=iPA)//把零件链保存到trsTmpREF表
					for(j=0;j<=SourceObj->GetCurSel();j++){
						SourceObj->GetText(j,temp);
						EDIBgbl::SQLx=CString(_T("INSERT INTO rsTmpREF (CustomID) VALUES (\'"))+Trim(temp)+_T("\')");
						EDIBgbl::dbPRJ.Execute(EDIBgbl::SQLx);
					}
					
					//零件链最后一个元素保存到trsTmpREF表
					COleVariant vTmp;
					rsUnCheckedType.GetFieldValue(_T("CustomID"),vTmp);
					EDIBgbl::SQLx=CString(_T("INSERT INTO rsTmpREF (CustomID) VALUES (\'"))+vtos(vTmp)+_T("\')");
					EDIBgbl::dbPRJ.Execute(EDIBgbl::SQLx);
					
					//'开始步骤2:
					//每次修改AllPrjdb.mdb中ZB的数据表结构之后，莫忘了同时修改F4511s.mdb的ZB表结构及workprj.mdb中的rsTmpZB表的结构。
					//MakeRsTZB(idbPRJ, _T("rsTmp"));
					//开始步骤3:
					EDIBgbl::SQLx = _T("SELECT * FROM rsTmpREF");
					if(rsTmpREF.IsOpen())
						rsTmpREF.Close();
					rsTmpREF.m_pDatabase=&EDIBgbl::dbPRJ;
					rsTmpREF.Open(dbOpenDynaset,EDIBgbl::SQLx);
					//'开始步骤4和5
					bool k=GetphsBHandSizesTest();
					if (k){
						rsUnCheckedType.GetFieldValue(_T("CustomID"),vTmp);
						EDIBgbl::SQLx=_T("INSERT INTO rsTmpCheckedType (CustomID) VALUES ('")+vtos(vTmp)+_T("')");
						EDIBgbl::dbPRJ.Execute(EDIBgbl::SQLx);
					}
					rsUnCheckedType.MoveNext();
			}//for block
			//开始步骤7:重新填充ResultObj列表框
			
			rsTmpCheckedType.Open(AFX_DAO_USE_DEFAULT_TYPE,_T("SELECT CustomID FROM rsTmpCheckedType"));
			if ((rsTmpCheckedType.IsEOF())&&(rsTmpCheckedType.IsBOF()));
			else if (!rsTmpCheckedType.IsEOF()){
				rsTmpCheckedType.MoveLast();
				rsTmpCheckedType.MoveFirst();
			}
			//清除列表框以便重新装入
			ResultObj->ResetContent();
			COleVariant vTmp;
			for(j=0;j<rsTmpCheckedType.GetRecordCount();j++)
			{
				rsTmpCheckedType.GetFieldValue(_T("CustomID"),vTmp);
				ResultObj->AddString(vtos(vTmp));
				rsTmpCheckedType.MoveNext();
			}
			return rsTmpCheckedType.GetRecordCount();
			//Set rsTmpCheckedType = Nothing
		}
	}
	catch(::CDaoException * e)
	{
		e->ReportError();
		e->Delete();
	}
errH:
/*	if(rsTmpCheckedType.IsOpen())
rsTmpCheckedType.Close();
if(rsUnCheckedType.IsOpen())
	rsUnCheckedType.Close();*/
	return 0;
}

CString Cphs::GetPhsAssembleName(long /*Optional*/ SampleID)
{
	//注意：本过程的速度很重要，因为在初始化过程中被循环调用。
	//目的:根据支吊架样本索引号,确定样本图的名称
	//Input:
	//SampleID,
	//如果缺省则从SourceObj对象(ListBox)中提取支吊架组装零件集合
	//返回:支吊架组装样本名称
	CString * Ptype=NULL;
	CString ret;
	long IC;
	long iNumCSPR , iNumSPR, iNumRod ;
	CString sPA, sSA, sSPR ;
	long i=0, j=0;
	CString strTmp,sTmp;
	CString SQLx;
	CDaoFieldInfo fi;
	
	try
	{
		CDaoRecordset rs1(&EDIBgbl::dbPHScode), rs(&EDIBgbl::dbPRJ);//20071101 "dbSORT" 改为 "dbPHScode"
		if(!rsID.IsOpen())
		{
			EDIBgbl::SQLx = _T("SELECT * FROM PictureClipData");
			rsID.m_pDatabase=&EDIBgbl::dbPRJ;
			rsID.Open(dbOpenDynaset,EDIBgbl::SQLx);
			brsIDStatus=TRUE;
		}
		
		SQLx = _T("SELECT * FROM phsStructureREF WHERE SampleID=") + ltos(SampleID) + _T(" ORDER BY SEQ ");
		rs1.Open(dbOpenDynaset,SQLx);
		
		if(rs1.IsEOF() && rs1.IsBOF())
		{
			sTmp.Format(GetResStr(IDS_NoSampleIDinphsStructureREF),ltos(SampleID));
			throw sTmp;
		}
		rs1.MoveLast();
		IC = rs1.GetRecordCount();
		Ptype=new CString[IC];
		COleVariant vTmp;
		iNumSPR = 0;
		iNumCSPR = 0;
		iNumRod = 0;
		sSPR = _T(" ");
		for(i=IC-1;i>=0;i--)
		{
			//注释掉判断句，可加快速度
			//If Not IsNull(rs1.Fields(_T("ID"))) Then
			rs1.GetFieldValue(_T("ID"),vTmp);
			Ptype[i] = vtos(vTmp);
			//End If
			rs1.MovePrev();
		}
		rs.Open(dbOpenSnapshot,_T("SELECT * FROM PictureClipData"));
		rs1.MoveFirst();
		for( i = 0 ;i<IC;i++)
		{
			//从第一个开始查找匹配
			if(i == IC - 1)
				//根部
				SQLx = _T("Trim(ID)=\'") + Ptype[i] + _T("\' AND index=0");
			else
				SQLx = _T("Trim(ID)=\'") + Ptype[i] + _T("\'");
			if(!rs.FindFirst(SQLx))
			{
				sTmp.Format(GetResStr(IDS_NoRecordFLDEqvThisValueInPictureClipData),_T("ID"),Ptype[i]);
				throw sTmp;
			}
			//第一个是管部
			rs.GetFieldValue(_T("Description"),vTmp);
			if( i == 0)
				ret=vtos(vTmp);
			else if( i == IC - 1)
			{
				if( iNumCSPR + iNumSPR == 0 )
				{
					if (ret.Find(modPHScal::gsPhsTypeRigid) == -1) 
						ret+=modPHScal::gsPhsTypeRigid + vtos(vTmp);
				}
				else if( iNumCSPR > 0 )
				{
					if(ret.Find(modPHScal::gsPhsTypeCSPR) == -1)
						ret+=modPHScal::gsPhsTypeCSPR +vtos(vTmp);
				}
				else if( iNumSPR > 0)
				{
					if(ret.Find(modPHScal::gsPhsTypeSPR) ==-1)
						ret+=modPHScal::gsPhsTypeSPR +vtos(vTmp);
				}
			}
			else
			{
				rs.GetFieldValue(_T("ClassID"),vTmp);
				if(vtoi(vTmp)==iCSPR)
				{
					iNumCSPR++;
					sSPR = Ptype[i];
				}
				else if(vtoi(vTmp)== iSPR )
				{
					iNumSPR ++;
					sSPR = Ptype[i];
				}
				else if(vtoi(vTmp)== iROD)
					iNumRod ++;
			}
			//对CustomID初始化,防止和新选择的规范矛盾。
			//使用下段程序加快速度。
			rs.GetFieldValue(_T("ID"),vTmp);
			if(rsID.FindFirst(_T("trim(ID)=\'") + vtos(vTmp)+_T("\'")))
			{
				rsID.GetFieldValue(_T("CustomID"),vTmp);
				strTmp = vtos(vTmp);
				rs1.Edit();
				rs1.SetFieldValue(_T("CustomID"),STR_VAR(strTmp));
				rs1.Update();
			}
			rs1.MoveNext();
		}
		rs1.Close();
		
		//开始改样本名称
		//这会清除模板管理表中的使用频度信息
		
		SQLx = _T("SELECT * FROM phsStructureName WHERE SampleID=") + ltos(SampleID);
		rs1.Open(dbOpenDynaset,SQLx);
		sTmp.Format(_T("%05d"),SampleID);
		//如果不使用条件，则名称为空
		rs1.GetFieldInfo(_T("SampleName"),fi);
		strTmp=ret;
		//此处要注意字符串的字节数
		int n=ret.GetLength();
		CString ss=ret.Left(2);
		if(n > fi.m_lSize)
			strTmp=ret.Left(fi.m_lSize);
		if(rs1.IsEOF() && rs1.IsBOF())
		{
			//下面的开关可在使用SQL和数据记录集对象之间切换。
			//使用SQL时保存新模板名称总是“简支刚性单吊”，而使用对象则不会出现这问题，因此不能以为SQL可能加快速度而放弃使用rs1。
			SQLx.Format(_T("INSERT INTO phsStructureName (SampleID,BmpName,SampleName,PA,SA,SPR,iNumOfPart,iNumSPR,iNumCSPR,iNumRod,FreQuence,bAllowUse) ")
				_T("VALUES (%s,\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',%s,%s,%s,%s,1,-1)"),
				ltos(SampleID),sTmp,strTmp,Ptype[0],Ptype[IC - 1],sSPR,ltos(IC), ltos(iNumSPR),ltos(iNumCSPR),ltos(iNumRod));
			EDIBgbl::dbPHScode.Execute(SQLx);//20071101 "dbSORT" 改为 "dbPHScode"
			EDIBgbl::dbPRJ.Execute(SQLx);
		}
		else
		{
			SQLx.Format(_T("UPDATE phsStructureName SET BmpName=\'%s\',SampleName=\'%s\',PA=\'%s\',SA=\'%s\',SPR=\'%s\',iNumOfPart=%s,iNumSPR=%s,iNumCSPR=%s,iNumRod=%s,bAllowUse=-1 WHERE SampleID=%s"),
				sTmp,strTmp,Ptype[0],Ptype[IC - 1],sSPR,ltos(IC), ltos(iNumSPR),ltos(iNumCSPR),ltos(iNumRod),ltos(SampleID));
			EDIBgbl::dbPHScode.Execute(SQLx);//20071101 "dbSORT" 改为 "dbPHScode"
			EDIBgbl::dbPRJ.Execute(SQLx);
		}
		delete [] Ptype;
	}
	catch(::CDaoException * e)
	{
		e->ReportError();
		e->Delete();
		if(Ptype!=NULL)
			delete [] Ptype;
	}
	catch(CException *e)
	{
		e->Delete();
		if(Ptype!=NULL)
			delete [] Ptype;
	}
	catch(...)
	{
		if(Ptype!=NULL)
			delete [] Ptype;
	}
	return ret;
}

long Cphs::GetPhsOneClassPartNum(long iPtype, long /*ByVal*/ SampleID)
{
	return 0;
}

long Cphs::GetPhsOneClassPartNumAndPartInfo(long iPtype, long /*ByVal*/ SampleID, CString * & Ptype, long * & mlPartClassID, long * &mlPartIndex,long & Count)
{
	//目的:根据支吊架样本索引号指明的组合结构ID,确定其中的拉杆数,返回零件的类别号和索引号
	//Input:SampleID,如果缺省则从默认为1，提取支吊架组装零件集合
	//Output:long,由长整数iPtype规定类别的零件数;如果iPtype=iSPR or iCSPR,则弹簧的CustomID返回到sSprInfo()数组中，下表是0，上标是GetPhsOneClassPartNumAndPartInfo - 1。
	//此函数与GetPhsOneClassPartNumAndPartType函数只有几处赋值语句不同
	//数组下标为0。
	   long ret=0;
	   try
	   {
		   CDaoRecordset rs1(&EDIBgbl::dbPHScode), rs(&EDIBgbl::dbPRJ);//20071019 "dbSORT" 改为 "dbPHScode"
		   long  i =0, iNumTmp=0;
		   CString tmpStr;
		   CString SQLx = _T("SELECT * FROM PictureClipData");
		   rs.Open(dbOpenDynaset,SQLx );
		   if( rs.IsEOF() && rs.IsBOF())
		   {
			   throw GetResStr(IDS_NoAnyRecordInPictureClipData);
		   }
		   SQLx = _T("SELECT * FROM phsStructureREF WHERE SampleID=") + ltos(SampleID) + _T(" ORDER BY SEQ ");
		   rs1.Open(dbOpenDynaset,SQLx );
		   if( rs1.IsBOF() && rs1.IsEOF())
		   {
			   tmpStr.Format(GetResStr(IDS_NoSampleIDinphsStructureREF),ltos(SampleID));
			   throw tmpStr;
		   }
		   COleVariant vTmp;
		   rs1.MoveLast();
		   rs1.MoveFirst();
		   int C=rs1.GetRecordCount();
		   if(Ptype!=NULL)
			   delete [] Ptype;
		   Ptype=new CString [C];
		   if(mlPartClassID!=NULL)
			   delete [] mlPartClassID;
		   mlPartClassID=new long [C];
		   if(mlPartIndex!=NULL)
			   delete [] mlPartIndex;
		   mlPartIndex=new long[C];
		   Count=C;
		   for(i=0;i<C;i++)
		   {
			   rs1.GetFieldValue(_T("ID"),vTmp);
			   if(vTmp.vt!=VT_NULL)
			   {
				   Ptype[i]=vtos(vTmp);
				   
				   if(rs.FindFirst(_T("Trim(ID)=\'") + Ptype[i] + _T("\'")))
				   {
					   rs.GetFieldValue(_T("ClassID"),vTmp);
					   mlPartClassID[i]=vtoi(vTmp);
					   rs.GetFieldValue(_T("Index"),vTmp);
					   mlPartIndex[i]=vtoi(vTmp);
					   if(mlPartClassID[i]== iPtype)
					   {
						   if(iPtype == iSPR)
						   {
							   rs.GetFieldValue(_T("CustomID"),vTmp);
							   modPHScal::sSprInfo[iNumTmp].CustomID = vtos(vTmp);
							   iNumTmp ++;
						   }
						   ret++;
					   }
				   }
			   }
			   rs1.MoveNext();
		   }
	   }
	   catch(CString e)
	   {
		   ShowMessage(e);
	   }
	   catch(CDaoException * e)
	   {
		   e->ReportError();
		   e->Delete();
	   }
	   catch(CException *e)
	   {
		   e->Delete();
	   }
	   catch(...)
	   {
	   }
	   return ret;
}

long Cphs::GetPhsIsCSPR(long iPtype, long SampleID, int& RCount)
{
	//目的:根据支吊架样本索引号指明的组合结构ID,确定其中是否含有某类零件,返回零件的数量
	//Input:SampleID,如果缺省则从默认为1，提取支吊架组装零件集合
	//Output:long,由长整数iPtype规定类别的零件数;如果iPtype=iSPR or iCSPR,则弹簧的CustomID返回到modPHScal::sSprInfo()数组中，下表是0，上标是GetPhsIsCSPR - 1。
	//返回:RCount,记录数
	//此函数与GetPhsOneClassPartNumAndPartType函数只有几处赋值语句不同
	//数组下标为0。
	CDaoRecordset rs1;
	long iNumTmp=0;
	CString SQLx;
	COleVariant v;
	long ret=0;
	try
	{
		rs1.m_pDatabase=&EDIBgbl::dbPHScode;//20071018 "dbSORT" 改为 "dbPHScode"
		CString sTmp;
		char sID[64];
		ltoa(SampleID,sID,10);
		if(!rsID.IsOpen())
		{
			SQLx=_T("SELECT * FROM PictureClipData");
			rsID.m_pDatabase=&EDIBgbl::dbPRJ;
			rsID.Open(dbOpenDynaset,SQLx);
		}
		else
			rsID.Requery();
		brsIDStatus=TRUE;
		if( rsID.IsEOF() && rsID.IsBOF() )
		{
			ShowMessage(GetResStr(IDS_NoAnyRecordInPictureClipData));
			rsID.Close();
			brsIDStatus=FALSE;
		}
		else
		{
			SQLx = _T("SELECT * FROM phsStructureREF WHERE SampleID=");
			sTmp= sID;
			SQLx=SQLx + sTmp;
			sTmp=_T(" ORDER BY SEQ ");
			SQLx=SQLx + sTmp;
			rs1.Open(dbOpenDynaset,SQLx);
			if( rs1.IsBOF() && rs1.IsEOF() )
			{
				sTmp.Format(GetResStr(IDS_NoSampleIDinphsStructureREF),sID);
				ShowMessage(sTmp,MB_TOPMOST|MB_OK);
			}
			else
			{
				while(!rs1.IsEOF())
				{
					rs1.GetFieldValue(_T("ID"),v);
					if( v.vt!=VT_NULL)
					{
						if( rsID.FindFirst(CString(_T("Trim(ID)=\'")) + vtos(v) + _T("\'")))
						{
							rsID.GetFieldValue(_T("ClassID"),v);
							if( vtoi(v) == iPtype )
							{
								if( iPtype == iSPR )
								{
									rsID.GetFieldValue(_T("CustomID"),v);
									modPHScal::sSprInfo[iNumTmp].CustomID = vtos(v);
									iNumTmp = iNumTmp + 1;
								}
								ret++;
							}
						}
					}
					rs1.MoveNext();
				}
				RCount = rs1.GetRecordCount();
			}
		}
	}
	catch(::CDaoException * e)
	{
		e->ReportError();
		e->Delete();
		RCount=rs1.GetRecordCount();
	}
	rs1.Close();
	return ret;
}
#pragma warning( disable : 4129 )  //Added by Shuli Luo

long Cphs::GetphsStructIDsemiAuto()
{
	//半自动选择支吊架结构
	//根据管部mvarPA、根部mvarSA和弹簧或恒力弹簧的数量giWholeSprNum，
	//从phsStructureName中自动选择使用频度最大、零件最少的组合，
	//智能化地完成支吊架组装过程.
	//input:
	//mvarPA--属性PA,管部,可为空
	//mvarSA--属性SA,根部,可为空
	//output:
	//GetphsStructIDsemiAuto=SampleID或-1(失败)
	long ret=-1;
	try
	{
		//long IC , iSampID , iNum ;
		//CString tmpSQLPA , tmpSQLSA , tmpSQLSpr;
		CDaoRecordset rs1(&EDIBgbl::dbPHScode);//20071101 "dbSORT" 改为 "dbPHScode"
		_Recordset* rsza=FrmTxsr.m_pViewTxsr->m_ActiveRs;
		//假定支吊架最多可能有100个零件
		CString strSprNum;
		strSprNum.Format(_T("%d"),modPHScal::giWholeSprNum);
		CString SQL1;
		CString SQLx;		
		CString strPA,strSA;
		CString strPAexists,strSAexists;
		CString strFilter,strSAFilter;
		//CString strOrderBy=_T(" ORDER BY FREQUENCE DESC,iNumOfPart");//排序字段
		
		SQL1.Format((modPHScal::ZdjType == _T("9999") || modPHScal::ZdjType == _T("5000") ? _T("iNumCSPR=%d") : _T("iNumSPR=%d")),modPHScal::giWholeSprNum);
		PA = Trim(PA);
		SA = Trim(SA);
		CString sTmp,sTmp2;
		COleVariant vTmp;
		//首先限制管部根部为指定类型，选择恒力弹簧模板
		//有
		//选择一个模板,exit
		//没有
		//限制管部类型，
		//有
		//选择一个模板,exit
		//没有
		//限制根部类型，
		//有
								//选择一个模板,exit
		//没有
								//不限制管部/根部类型，
		//有
		//选择一个模板,exit
		//没有
		//报警，没有恒力弹簧模板
		
		sTmp=vtos(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("Unit_Force")));
		sTmp.MakeUpper();
		modPHScal::pjg=vtof(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("pjg1")));
		if(sTmp==_T("N"))
			modPHScal::pjg /= EDIBgbl::kgf2N;
		float tmppjg=fabs(modPHScal::pjg);
		modPHScal::t0=vtof(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("T01")));
		modPHScal::dj=vtof(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("dj1")));
		
		strPA.Format(_T(" (PA=\'%s\') "),PA);
		strPAexists = _T("( PA IN (Select ID FROM PictureClipData ")
			_T(" WHERE CustomID IN ( Select CustomID FROM [") + modPHScal::tbnPA + _T("] IN \"\" [\; DATABASE=") 
			+ modPHScal::dbZDJcrude.GetName() + _T(" ;PWD=") + ModEncrypt::gstrDBZdjCrudePassWord + _T("]WHERE Pmax >=") 
			+ ftos(tmppjg) + _T(" AND (Dw >= ") + ftos(modPHScal::dj* (1 - modPHScal::gnDW_delta * 0.01))
			+ _T(" AND Dw <= ") + ftos(modPHScal::dj* (1 + modPHScal::gnDW_delta * 0.01)) 
			+ _T(" AND Dw>0 OR Dw Is NULL OR Dw=0) AND tj >= ") + ftos(modPHScal::t0) + _T("))) ");
		
		strSA.Format(_T(" (SA=\'%s\') "),SA);
		int Gnum;			
		Gnum= (modPHScal::glNumSA!=0 ? modPHScal::glNumSA : 1);
		strSAexists = _T(" ( SA IN ( Select ID FROM PictureClipData ")
			_T(" WHERE EXISTS ( Select CustomID FROM [") + modPHScal::tbnSA + _T("] IN \"\" [; DATABASE=") 
			+ modPHScal::dbZDJcrude.GetName() + _T(" ;PWD=") + ModEncrypt::gstrDBZdjCrudePassWord + _T("] WHERE PictureClipData.CustomID = CustomID AND ((PictureClipData.ClassID= ") + ltos(iG100) + _T(") OR PMAXH >=") 
			+ ftos(tmppjg / Gnum * (vtob(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("ifLongVertPipe"))) ? Gnum : 1))
			+_T(" AND ((( PictureClipData.ClassID = ")
			+ ltos(iSACantilever) + _T(" OR PictureClipData.ClassID = ") + ltos(iSALbrace) + _T(" OR PictureClipData.ClassID = ") + ltos(iSALbraceFixG47) + _T(" OR PictureClipData.ClassID = ") + ltos(iSALbraceFixG48) + _T(" OR PictureClipData.ClassID = ") + ltos(iG51) + _T(" OR PictureClipData.ClassID = ") + ltos(iG56) + _T(" OR PictureClipData.ClassID = ") + ltos(iG57)  + _T(") AND GDW1 >=")
			+ ftos(modPHScal::gdw1) + _T(") OR (( PictureClipData.ClassID =") + ltos(iSAbeam) +_T(" OR PictureClipData.ClassID =") + ltos(iG52_55) + _T(" ) AND GDW1 >=")
			+ _T(" IIF(")+ ftos(modPHScal::gdw1+(modPHScal::glNumSA==1 ? modPHScal::DblHangerRodDist : 0)/2) +_T(" > L1/2 , L1 - ") + ftos(modPHScal::gdw1+(modPHScal::glNumSA == 1 ? modPHScal::DblHangerRodDist : 0) / 2)
			+ _T(" , ")+ ftos(modPHScal::gdw1+(modPHScal::glNumSA == 1 ? modPHScal::DblHangerRodDist : 0) / 2) + _T(") AND L1> ") + ftos(modPHScal::Lspan)
			+_T(" ) OR (PictureClipData.ClassID= ") + ltos(iGCement) + _T(")))))) ");
		
		SQLx.Format(_T("SELECT * FROM phsStructureName WHERE %s ORDER BY Frequence DESC,iNumOfPart"),
			SQL1 + _T(" AND ") + strPA + _T(" AND ") + strPAexists + _T(" AND ") + strSA + _T(" AND ") + strSAexists );
		rs1.Open(dbOpenSnapshot,SQLx);
		if( !(rs1.IsEOF() && rs1.IsBOF()) )
		{
			//限制管部,限制管部必须存在，限制根部/根部必须存在，有
		}
		else
		{
			//限制管部,限制管部必须存在，限制根部/根部必须存在，没有
			SQLx.Format(_T("SELECT * FROM phsStructureName WHERE %s ORDER BY Frequence DESC,iNumOfPart"),
				SQL1 + _T(" AND ") + strPAexists + _T(" AND ")  + strSA + _T(" AND ") + strSAexists );
			if(rs1.IsOpen()) rs1.Close();
			rs1.Open(dbOpenSnapshot,SQLx);
			if( !(rs1.IsEOF() && rs1.IsBOF()) )
			{
				//不限制管部,限制管部必须存在，限制根部/根部必须存在，有
			}
			else
			{
				//不限制管部,限制管部必须存在，限制根部/根部必须存在，没有
				SQLx.Format(_T("SELECT * FROM phsStructureName WHERE %s ORDER BY Frequence DESC,iNumOfPart"),
					SQL1 + _T(" AND ") + strSA + _T(" AND ") + strSAexists );
				if(rs1.IsOpen()) rs1.Close();
				rs1.Open(dbOpenSnapshot,SQLx);
				if( !(rs1.IsEOF() && rs1.IsBOF()) )
				{
					//不限制管部,不限制管部必须存在，限制根部/根部必须存在，有
				}
				else
				{
					//不限制管部,不限制管部必须存在，限制根部/根部必须存在，没有
					SQLx.Format(_T("SELECT * FROM phsStructureName WHERE %s ORDER BY Frequence DESC,iNumOfPart"),
						SQL1 + _T(" AND ") + strSAexists );
					if(rs1.IsOpen()) rs1.Close();
					rs1.Open(dbOpenSnapshot,SQLx);
					if( !(rs1.IsEOF() && rs1.IsBOF()) )
					{
						//不限制管部,不限制管部必须存在，不限制根部，限制根部必须存在，有
					}
					else
					{
						//不限制管部,不限制管部必须存在，不限制根部，限制根部必须存在，没有
						SQLx.Format(_T("SELECT * FROM phsStructureName WHERE %s ORDER BY Frequence DESC,iNumOfPart"),
							SQL1 );
						if(rs1.IsOpen()) rs1.Close();
						rs1.Open(dbOpenSnapshot,SQLx);
						if( !(rs1.IsEOF() && rs1.IsBOF()) )
						{
							//不限制管部,不限制管部必须存在，不限制根部，不限制根部必须存在，有
						}
						else
						{
							//不限制管部,不限制管部必须存在，不限制根部，不限制根部必须存在，没有
							SQL1=(modPHScal::ZdjType == _T("9999") || modPHScal::ZdjType == _T("5000") ? _T("iNumCSPR>=1") : _T("iNumSPR>=1"));
							SQLx.Format(_T("SELECT * FROM phsStructureName WHERE %s ORDER BY Frequence DESC,iNumOfPart"),
								SQL1 );
							if(rs1.IsOpen()) rs1.Close();
							rs1.Open(dbOpenSnapshot,SQLx);
							if( !(rs1.IsEOF() && rs1.IsBOF()) )
							{
								//不限制管部,不限制管部必须存在，不限制根部，不限制根部必须存在，不限制弹性件数量，有
							}
							else
							{
								//不限制管部,不限制管部必须存在，不限制根部，不限制根部必须存在，不限制弹性件数量，没有
								sTmp.Format(GetResStr(IDS_NotFoundThisSampleIDSprNum),_T(">=1"));
								throw sTmp;
							}
						}
					}
				}
			}
		}
		
		/*
		if (iLIST == mnuiPASALimited)
		{
		//限制管部根部的类型为用户指定型式
		if(PA == _T("") )
		{
		//管部为空
		if( SA == _T("") )
		{
		//管部为空，根部为空
		SQLx=_T("SELECT * FROM phsStructureName WHERE ")+SQL1+strSprNum+ _T("ORDER BY Frequence DESC,iNumOfPart");
		
		  rs1.Open(dbOpenSnapshot,SQLx);
		  if( rs1.IsEOF() && rs1.IsBOF() )
		  {
		  sTmp.Format(GetResStr(IDS_NotFoundThisSampleIDSprNum),strSprNum);
		  throw sTmp;
		  }
		  }
		  else
		  {
		  //管部为空，根部不为空
		  SQLx = _T("SELECT * FROM phsStructureName WHERE  trim(SA)=\'") + SA + _T("\' AND ")+SQL1+strSprNum+ _T(" ORDER BY Frequence DESC,iNumOfPart");
		  //rs1.m_pDatabase=&EDIBgbl::dbSORT;
		  rs1.Open(dbOpenSnapshot,SQLx);
		  if (rs1.IsEOF() && rs1.IsBOF())
		  {
		  sTmp.Format(GetResStr(IDS_NotFoundThisSampleIDSASprNum),strSprNum,SA);
		  ShowMessage(sTmp);
		  if(!modPHScal::gbAutoPASA)
		  return -1;
		  else
		  {
		  //管部为空，根部为空，重新选择
		  //管部为空，根部为空
		  SQLx = _T("SELECT * FROM phsStructureName WHERE ") +SQL1+strSprNum+  _T(" ORDER BY Frequence DESC,iNumOfPart");
		  rs1.Close();
		  rs1.Open(dbOpenSnapshot,SQLx);
		  if( rs1.IsEOF() && rs1.IsBOF())
		  {
		  sTmp.Format(GetResStr(IDS_NotFoundThisSampleIDSprNum),strSprNum);
		  throw sTmp;
		  }
		  }
		  }
		  }
		  }
		  else
		  {
		  //管部不为空
		  if( SA == _T(""))
		  {
		  //管部不为空，根部为空
		  SQLx = _T("SELECT * FROM phsStructureName WHERE  trim(PA)=\'")+PA +_T("\' AND ") +SQL1+strSprNum+ _T(" ORDER BY Frequence DESC,iNumOfPart");
		  rs1.Open(dbOpenSnapshot,SQLx);
		  if(rs1.IsEOF()&& rs1.IsBOF())
		  {
		  sTmp.Format(GetResStr(IDS_NotFoundThisSampleIDPASprNum),strSprNum,PA);
		  ShowMessage(sTmp);
		  if(!modPHScal::gbAutoPASA)
		  return -1;
		  else
		  {
		  //不限制管部，根部为空，重新选择
		  //管部为空，根部为空
		  SQLx = _T("SELECT * FROM phsStructureName WHERE ")+SQL1+strSprNum+ _T(" ORDER BY Frequence DESC,iNumOfPart");
		  rs1.Close();
		  rs1.Open(dbOpenSnapshot,SQLx);
		  if(rs1.IsEOF()&& rs1.IsBOF())
		  {
		  sTmp.Format(GetResStr(IDS_NotFoundThisSampleIDSprNum),strSprNum);
		  throw sTmp;
		  }
		  }
		  }
		  }
		  else
		  {
		  //管部不为空，根部不为空
		  SQLx = _T("SELECT * FROM phsStructureName WHERE  trim(PA)=\'") +PA + _T("\' AND  trim(SA)=\'") +SA +_T("\' AND ") +SQL1+strSprNum+  _T(" ORDER BY Frequence DESC,iNumOfPart");
		  rs1.Open(dbOpenSnapshot,SQLx);
		  if( rs1.IsEOF()&& rs1.IsBOF())
		  {
		  sTmp.Format(GetResStr(IDS_NotFoundThisSampleIDPASASprNum),strSprNum,PA,SA);
		  ShowMessage(sTmp);
		  if(!modPHScal::gbAutoPASA)
		  return -1;
		  else
		  {
		  //不限制管部，重新选择
		  //管部为空，根部不为空
		  SQLx = _T("SELECT * FROM phsStructureName WHERE  trim(SA)=\'")+SA +_T("\' AND ")+SQL1+strSprNum+ _T(" ORDER BY Frequence DESC,iNumOfPart");
		  rs1.Close();
		  rs1.Open(dbOpenSnapshot,SQLx);
		  if( rs1.IsEOF()&& rs1.IsBOF())
		  {
		  sTmp.Format(GetResStr(IDS_NotFoundThisSampleIDSASprNum),strSprNum,SA);
		  ShowMessage(sTmp);
		  //不限制管部，不限制根部，重新选择
		  //管部为空，根部为空
		  SQLx = _T("SELECT * FROM phsStructureName WHERE ")+SQL1+strSprNum+ _T(" ORDER BY Frequence DESC,iNumOfPart");
		  rs1.Close();
		  rs1.Open(dbOpenSnapshot,SQLx);
		  if( rs1.IsEOF()&& rs1.IsBOF())
		  {
		  sTmp.Format(GetResStr(IDS_NotFoundThisSampleIDSprNum),strSprNum);
		  ShowMessage(sTmp);
		  return -1;
		  }
		  }
		  }
		  }
		  }
		  }
		  }
*/
		if(!rs1.IsOpen() || rs1.IsEOF()||rs1.IsBOF())
			return -1;
		rs1.MoveFirst();
		rs1.GetFieldValue(_T("SampleID"),vTmp);
		ret=vtoi(vTmp);
		rs1.GetFieldValue(_T("PA"),vTmp);
		PA = vtos(vTmp);
		rs1.GetFieldValue(_T("SA"),vTmp);
		SA = vtos(vTmp);

		rsza->PutCollect(_T("dn1"),_variant_t(modPHScal::sFindCustomID (PA)));
		rsza->PutCollect(_T("gn1"),_variant_t(modPHScal::sFindCustomID (SA)));
		rsza->PutCollect(_T("iSelSampleID"),_variant_t((long)ret));
		return ret;
	   }
	   catch(CString e)
	   {
		   ShowMessage(e);
		   return -1;
	   }
	   catch(CDaoException * e)
	   {
		   e->ReportError();
		   e->Delete();
		   return -1;
	   }
	   catch(...)
	   {
		   return -1;
	   }
	   
}

long Cphs::GetPhsStructSampleID()
{
	//目的:自动选择支吊架结构模板号
	//1:首先从原始数据表rsZA的字段iSelSampleID中取得样本图号
	//2:如果字段iSelSampleID=null,则根据管部dn、根部gn和弹簧或恒力弹簧的数量，
	//从phsStructureREF中自动搜索使用频度最大的组合，
	//智能化地完成支吊架组装过程.
	//3:如果字段iSelSampleID=null,并且没有提供管部dn、根部gn和弹簧或恒力弹簧的数量，
	//则选择使用频度最大的支吊架组合
	//4:如果步骤3还不成功，则返回-1
	//input:
	//iSelSampleID--选择的支吊架模板号
	//output:
	//if it//s successiful, return SampleID,otherwise -1
	int ret=-1;
	try
	{
		//MsgBox ResolveResString(iUE_NotFoundSampleIDSprNum, _T("|1"), iSelSampleID, _T("|2"), giWholeSprNum)
		ret = GetphsStructIDsemiAuto();
	}
	catch(CException *e)
	{
		e->Delete();
	}
	catch(...)
	{
	}
	return ret;
}

long Cphs::SavephsStructTorsTmpREF()
{
	//根据支吊架号和由SampleID号指出的支吊架组合结构,
	//保存支吊架组合结构到rsTmpREF表,
	//Input:iSelSampleID,zdjh
	//iAttachIncluded=iAttachIncluded/iNotAttachInclude(包括附件/不包括附件)
	//Output:long,成功=-1;否则=错误码
	try{
		CDaoRecordset rs(&EDIBgbl::dbPRJ);
		//	int i;
		CString sTmp,sTmp2,SQLx;
		sTmp.Format(_T("%d"),modPHScal::iSelSampleID);
		SQLx = _T("SELECT *  FROM phsStructureREF WHERE SampleID=") + sTmp + _T(" ORDER BY seq");
		rs.Open(dbOpenSnapshot,SQLx);
		if(rs.IsBOF() && rs.IsEOF())
		{
			sTmp2.Format(GetResStr(IDS_NoSampleIDinphsStructureREF),sTmp);
			ShowMessage(sTmp2);
			return IDS_NoSampleIDinphsStructureREF;
		}
		else
		{
			rs.MoveLast();
			rs.MoveFirst();
			if(rsTmpREF.IsOpen())
				rsTmpREF.Close();
			if( EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("rsTmpREF")))
				EDIBgbl::dbPRJ.Execute(_T("DELETE * FROM rsTmpREF"));
			else
				EDIBgbl::dbPRJ.Execute(_T("CREATE TABLE rsTmpREF (CustomID char (20))"));
			COleVariant vTmp;
			while(!rs.IsEOF())
			{
				rs.GetFieldValue(_T("CustomID"),vTmp);
				SQLx = _T("INSERT INTO rsTmpREF (CustomID) VALUES (\'") + vtos(vTmp) + _T("\')");
				EDIBgbl::dbPRJ.Execute(SQLx);
				rs.MoveNext();
			}
			
			SQLx = _T("SELECT CustomID FROM rsTmpREF");
			
			this->rsTmpREF.m_pDatabase=&EDIBgbl::dbPRJ;
			rsTmpREF.Open(dbOpenDynaset,SQLx);
			return -1;
		}
	}
	catch(::CDaoException * e)
	{
		e->ReportError();
		e->Delete();
		return 0;
	}
}

long Cphs::SavephsAllStructToTZB(long& zdjh, int nth, int& /*Optional*/ AttachIncluded)
{
	//保存支吊架组合结构到JZB表,同时根据可选参数AttachIncluded决定是否
	//保存支吊架组合结构的附件数据(螺母/垫片等)到JZF表
	//根据支吊架号和SampleID号指出的支吊架组合结构,以及从connect表查找到的所有相关附件写到表中
	//Input:
	//SampleID
	//zdjh
	//AttachIncluded=iAttachIncluded/iNotAttachInclude(包括附件/不包括附件)
	//iMODEoverwrite=-1(重写)/0(不重写)
	//Output:long,成功=-1;否则=错误码
	CString sTmp,sTmp2;
	int j,k;
	int ret=0;
	
	_Recordset* rsza=::FrmTxsr.m_pViewTxsr->m_ActiveRs;
	CDaoRecordset rsTmpZB(&EDIBgbl::dbPRJDB); 
	//rsTmpZB.m_pDatabase=&EDIBgbl::dbPRJDB;
	CString SQLx;
	_variant_t vTmp;

	CString * SampleArray = NULL;
	try
	{
		//初始零件号
		k = 1;
		//删除表中所有字段zdjh值=zdjh的记录
		//modPHScal::MakeZDJ_ZBZG();
		CDaoRecordset rsX;
		//连接表，保存的是ID值
		if(!rsConnect.IsOpen())
		{
			SQLx = _T("SELECT *  FROM connect");
			rsConnect.m_pDatabase=&EDIBgbl::dbPRJ;
			rsConnect.Open(dbOpenDynaset,SQLx);
		}
		//模板表，保存了ID和CustomID值
		
		sTmp.Format(_T("%d"),modPHScal::iSelSampleID);
		SQLx = CString(_T("SELECT *  FROM phsStructureREF WHERE SampleID=")) + sTmp + _T(" ORDER BY seq");
		rsX.m_pDatabase=&EDIBgbl::dbPHScode;//20071018 "dbSORT" 改为 "dbPHScode"
		rsX.Open(dbOpenDynaset,SQLx);
		if(rsX.IsEOF() && rsX.IsBOF())
		{
			sTmp2.Format(_T("%d"),modPHScal::iSelSampleID);
			sTmp.Format(GetResStr(IDS_NoSampleIDinphsStructureREF),sTmp2);
			ShowMessage(sTmp);
			ret = IDS_NoSampleIDinphsStructureREF;
		
			return ret;
		}
		else
			rsX.MoveLast();
		long iCount = rsX.GetRecordCount();
		COleVariant rsXv;

		SampleArray = new CString[iCount];
		for(int i =iCount-1;i >= 0 ;i--)
		{
			rsX.GetFieldValue("ID",rsXv);
			SampleArray[i] = vtos(rsXv);
			rsX.MovePrev();
		}

		
		SQLx = CString(_T("SELECT *  FROM [")) + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] WHERE zdjh=") +ltos(zdjh) + _T(" AND VolumeID=")+ ltos(EDIBgbl::SelVlmID) + _T(" ORDER BY RECNO");
		rsTmpZB.Open(dbOpenDynaset,SQLx);
		//附件表
		
		
		CString sTmp2;
		COleVariant v,v1,v2,mvClassID,mvCustomID;
		_variant_t vTmp;
		
		for( i = 0 ;i<= iCount-1;i++)// - 1
		{
			mvClassID = modPHScal::sFindFLD(_T("ID"), _T("ClassID"), SampleArray[i]);
			mvCustomID = modPHScal::sFindFLD(_T("ID"), _T("CustomID"), SampleArray[i]);
			//更新支吊架原始数据表JZA的管部和根部
			if (i == 0 )
				rsza->PutCollect(_T("dn1"),mvCustomID);
			else if( i == rsX.GetRecordCount() - 1)
				rsza->PutCollect(_T("gn1"),mvCustomID);
			_variant_t tv;
			rsTmpZB.AddNew();
			rsTmpZB.SetFieldValue(_T("VolumeID"),COleVariant(EDIBgbl::SelVlmID));
			tv.ChangeType(VT_I4);
			tv.intVal=zdjh;
			rsTmpZB.SetFieldValue(_T("zdjh"),tv);
			tv.ChangeType(VT_I2);
			tv.iVal=k;
			rsTmpZB.SetFieldValue(_T("recno"),tv);
			tv.iVal = nth;
			rsTmpZB.SetFieldValue(_T("nth"),tv);

			rsTmpZB.SetFieldValue(_T("index"),variant_t((short)vtoi(modPHScal::sFindFLD(_T("ID"), _T("Index"),SampleArray[i]))));
			
			//不是附件
			tv.iVal=(short)vtoi(mvClassID);
			rsTmpZB.SetFieldValue(_T("ClassID"),tv);
			rsTmpZB.SetFieldValue(_T("ID"),STR_VAR(SampleArray[i]));
			rsTmpZB.SetFieldValue(_T("CustomID"),STR_VAR(modPHScal::sFindCustomID(SampleArray[i])));
			rsTmpZB.Update();
			if (i< iCount-1)// Then
			{
				//取得下一次的开头零件
				if(!rsConnect.FindFirst (CString(_T("trim(cntb)=\'")) + (SampleArray[i]) + _T("\' AND trim(cnte)=\'") + (SampleArray[i+1]) + _T("\'")))
				{
					sTmp.Format(GetResStr(IDS_PartCanNotMatchOrNotExistInConnect),SampleArray[i],SampleArray[i+1]);
					ShowMessage(sTmp);
					return ret;
				}
				else
				{
					//包括附件
					if (AttachIncluded == iAttachIncluded)
					{
						//最多8个附件,eg. F1/F2/...,etc.
						for( j = 1 ;j<= 8;j++)
						{
							sTmp.Format(_T("CntbF%d"),j);
							rsConnect.GetFieldValue( sTmp,v1);
							if (vtos(v1) != _T("") )
							{
								k = k + 1;
								rsTmpZB.AddNew();
								rsTmpZB.SetFieldValue(_T("VolumeID"),COleVariant(EDIBgbl::SelVlmID));
								rsTmpZB.SetFieldValue(_T("zdjh"),_variant_t((long)zdjh));
								rsTmpZB.SetFieldValue(_T("recno"),_variant_t((short)k));
								rsTmpZB.SetFieldValue(_T("nth"),_variant_t((short)nth));

								//是附件
								rsTmpZB.SetFieldValue(_T("ClassID"),_variant_t((short)vtoi(modPHScal::sFindFLD(_T("ID"), _T("ClassID"),vtos(v1)))));
								//函数sFindCustomID可以取出数字值
								rsConnect.GetFieldValue(_T("CntbF")+ltos(j)+_T("num"),v2);
								rsTmpZB.SetFieldValue(_T("CLnum"),v2);

								rsTmpZB.SetFieldValue(_T("ID"),v1);
								rsTmpZB.SetFieldValue(_T("CustomID"),STR_VAR(modPHScal::sFindCustomID(vtos(v1))));
								rsTmpZB.Update();
							}
						}
					}
				}
			}
			k = k + 1;
		}
		ret = -1;
		rsTmpZB.Close();
	}
	catch(::CDaoException * e)
	{
		e->ReportError();
		e->Delete();
	}
	catch(_com_error )
	{
		//ShowMessage(e.Description());
	}
	
	if( SampleArray != NULL )
		delete [] SampleArray;

	return ret;
}

void Cphs::GetPhsStructFromTZB(int zdjh)
{
	//读取支吊架结构from ZB表 to mvarResultObj(ListBox)
	//Input:zdjh,
	//Output:支吊架组装零件集合(CustomID值)填充到SourceObj对象(ListBox)
	try{
		if(TZBResultObj==NULL || !IsWindow(TZBResultObj->m_hWnd))
			return; 
		CDaoRecordset rs(&EDIBgbl::dbPRJDB);
		TZBResultObj->ResetContent();
		
		CString SQLx;
		SQLx.Format(CString(_T("SELECT * FROM [")) + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] WHERE zdjh=%d AND VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" AND IsSAPart<>-1 ORDER BY recno"),modPHScal::zdjh);
		rs.Open(dbOpenSnapshot,SQLx);
		if(rs.IsEOF() && rs.IsBOF())
		{
			throw GetResStr(IDS_NoComponentForZDJHInTZB);
		}
		rs.MoveLast();
		COleVariant vTmp;
		rs.GetFieldValue(_T("CustomID"),vTmp);
		modPHScal::UpdateTZB4zdjh(_T("gn1"), _variant_t(vtos(vTmp)));
		rs.MoveFirst();
		rs.GetFieldValue(_T("CustomID"),vTmp);
		modPHScal::UpdateTZB4zdjh(_T("dn1"), _variant_t(vtos(vTmp)));
		while(!rs.IsEOF())
		{
			rs.GetFieldValue(_T("CustomID"),vTmp);
			TZBResultObj->AddString(vtos(vTmp));
			rs.MoveNext();
		}
		rs.Close();
	}
	catch(CString )
	{
	}
	catch(CException *e)
	{
		e->Delete();
	}
	catch(...)
	{
	}
}

void Cphs::SetPhsPASAtoTZA(int zdjh, long /*ByVal*/ SampleID)
{
}

void Cphs::MakeRsTZB(long /*ByVal*/ iDbID, CString /*ByVal*/ rsPrefixName,CString strDeleteUserMaterial)
{
	//功能:为GetphsBHandSizes函数准备mvarrsTZB,mvarrsTZC,mvarrsTZF记录集//
	//On Error GoTo errH
	// tbn As String
	CString tbn;
	//结果结构数据表
	bool tbE=false;
	CString SQLx;
	try{
		CString sTmp;
		sTmp.Format(_T("%d"),modPHScal::zdjh);
		
		if( iDbID == idbPRJDB )
		{
			SQLx = CString(_T("DELETE  FROM [")) +EDIBgbl::Btype[EDIBgbl::TZB]+ _T("] WHERE zdjh=") + sTmp + _T(" AND VolumeID=") + ltos(EDIBgbl::SelVlmID) + strDeleteUserMaterial;
			EDIBgbl::dbPRJDB.Execute(SQLx);
		}
		else
		{
			SQLx = CString(_T("DELETE  FROM [rstmp")) +EDIBgbl::Btype[EDIBgbl::TZB]+ _T("] WHERE zdjh=") + sTmp + _T(" AND VolumeID=") + ltos(EDIBgbl::SelVlmID);
			EDIBgbl::dbPRJ.Execute(SQLx);
		}
		if(rsTZB.IsOpen())
			rsTZB.Close();
		if( iDbID == idbPRJDB )
		{
			SQLx = CString(_T("SELECT * FROM [")) + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] WHERE zdjh=") + sTmp + _T(" AND VolumeID=") + ltos(EDIBgbl::SelVlmID);
			rsTZB.m_pDatabase=&EDIBgbl::dbPRJDB;
			rsTZB.Open(dbOpenDynaset,SQLx);
		}
		else
		{
			SQLx = CString(_T("SELECT * FROM [rstmp")) + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] WHERE zdjh=") + sTmp + _T(" AND VolumeID=") + ltos(EDIBgbl::SelVlmID);
			rsTZB.m_pDatabase=&EDIBgbl::dbPRJ;
			rsTZB.Open(dbOpenDynaset,SQLx);
		}
	}
	catch(::CDaoException * e)
	{
		e->ReportError();
		e->Delete();
	}
	catch(_com_error & e)
	{
		ShowMessage(e.Description());
	}
}

bool Cphs::GetphsBHandSizesTest()
{
	//目的:从原始数据表中检索每个满足条件的零件，不包括
	//输入:ZB,zdjh
	//返回:连接成功,true;否则False
	//On Error GoTo errH
	try{
		CString LogicalOpr = _T("=<>");   //逻辑操作定界符
		int iLOPnum = 3;          //逻辑操作定界符长度
		CString msOpr,sTmp,sTmp2;
		//   long n=0,m=0;
		//   bool bFound;
		
		//以下一组变量用于计算固定支架的当量力sngSAfixP
		float sngSAfixP  =0; //固定支架的当量力sngSAfixP
		float sngSAfixPl=0;  //冷态固定支架的当量力sngSAfixPl
		float sngSAfixPr=0;  //热态固定支架的当量力sngSAfixPr*/
		//float PAfixH =0;//固定支架管部高度H,cm
		float F1ToWx1 =0;   //与根部型钢号有关的特性数据，储存在zdjcrudeXBDLY的槽钢特性表SSteelPropertyCS的F1ToWx1字段
		float PAfixC1 =0;   //固定支架C1值,mm
		float mvBWidth=0 ;    //固定支架根部主槽钢间距,cm,G47=根部表chdist字段值，G48=TZA表A01字段值
		float pnl =0 , pnr=0 ; //固定支架Z1管部螺栓冷/热态承受的力或Z2合成力,kgf
		float ml =0 , mr =0;   //固定支架Z2的合成力矩,kgf.m
		float pxl=0  , pzl=0  , pxr =0 , pzr=0;   //固定支架冷热态承受的力,kgf
		float mxl =0 , myl=0  , mzl =0 , mxr=0  , myr =0 , mzr=0;  
		float sngCoef=0 ;   //固定支架的系数
		_variant_t vTmp;
		float tmpPJG=0,tmpT0=0,tmpGDW1=0,tmpPmax4PA=0,tmpPmax4Part=0,tmpSelPJG=0; //累加零件重量后的计算荷载
		//如果条件丢失,则默认为荷载、温度、根部定位条件应该全部满足
		CString stmpT0,stmpPJG;
		if( iChkCondition==0 )
			iChkCondition = iPJG + iTJ + iGDW1 + iCNT + iDW;
		//较为适宜的做法：荷载应该累加零部件重量。
		//根部采用tmpPJG作为荷载计算标准。
		tmpPJG = fabs(modPHScal::pjg);
		stmpPJG.Format(_T("%g"),tmpPJG);
		//管部、零部件选择荷载tmpPmax4Part也使用tmpPmax4PA，较为合理。
		if( modPHScal::gbCalPARTbyMaxLoad )
		{
			tmpPmax4Part = (fabs(modPHScal::pgz) >= fabs(modPHScal::paz) ? fabs(modPHScal::pgz) : fabs(modPHScal::paz));
		}else {
			tmpPmax4Part = fabs(modPHScal::pjg);
		}
		//设置计算荷载初始值，以便累加
		tmpSelPJG = tmpPmax4Part;
		//设置弹簧计算荷载初始值，以便累加
		tmpSelPJG = tmpPmax4Part;
		tmpT0 = modPHScal::t0;
		stmpT0.Format(_T("%g"),tmpT0);
		tmpGDW1 =modPHScal::gdw1;
		
		CString strOrderBy ;      //排序字符串
		CString tmpSQLlugLen ;    //拉杆长度修正值
		CString  tmpSQL ;         //当前的连接匹配查询字段信息
		CString  tmpSQL0 ;        //前一次的连接匹配查询字段信息
		//   long lngErrCalPAfix ;   //计算固定支架时产生的错误号
		/* rsza As Recordset, rsX As Recordset, rsxx As Recordset
		Set rsza = Crs.rsDS*/
		_RecordsetPtr rsza=FrmTxsr.m_pViewTxsr->m_ActiveRs;
		CDaoRecordset rsTmp,rsX;
		rsTmp.m_pDatabase=&EDIBgbl::dbSORT;
		rsX.m_pDatabase=&modPHScal::dbZDJcrude;
		
		CString tbn1,SAfDPmax;
		long i=0,k=0;
		CString sBHFormat,sBH;
		CString sPartID,sRodPASA;
		CString tmpCustomID0,tmpCustomID1;
		CString tmpID0,tmpID1;
		CString tmpFD0;
		CString tmpExtLenFD;
		//	float tmpExtLenValue;//上一个零件（除拉杆外）插入当前零件L8的深度
		CString tmpFD1Previous,tmpRelationPrevious;
		
		CString tmpFD0value, tmpFD0valuePrevious;   //保存前一个零件的连接匹配字段值
		float sngDim=0,mvSumHeight=0,sngCSLen=0,sngWeight=0,sngW=0;
		long iCntbNum=0,iCnteNum=0,iNum=0,iNumPA=0;
		long mvSA=0,mvPA=0;
		
		//    long mviClassID=0;  //根部型钢的类型
		//    long mviPreviousClassID=0;  //前一个部件的类别号
		
		CString mvL1CustomID;  //保存拉杆L1的CustomID值，用于弹簧或恒吊的拉杆直径选择。当有弹簧时可加快运行速度。
		//用于根部附件查找螺栓螺母用
		CString tmpCustomID;
		float tmpSize2=0,tmpSizeH=0;
		
		//    long iMODE=0 ;       //计算模式，可用于加快运算速度。
		CString mvSAattachedCustomID;  //根部附件的CustomID
		///>>>>
		CString *Ptype=NULL;   //不含附件的组件结构CustomID数组
		//    CString *PtypeClassID;   //不含附件的组件结构ClassID数组
		//    long *PtypeIndex;   //不含附件的组件结构Index数组
		///>>>>>
		//测试计算状态，数据放在临时表中；
		//iNum=(iCnteNum/iCntbNum)
		//每个部件的数量iNumPart=其前一个部件的数量iNumPart*iNum*iCntbNum
		//iNumPart=iNumPart*iNum*iCntbNum
		//iSA=0,iCSPR=7
		//mviClassID=根部附件型钢类型
		//mvSA=最后一个零件的索引号,用于判别是否是根部,
		//因为在组装尚未完成时,最后一个零件不一定是根部类型
		//mvPA=管部类型,用于统计拉杆的总长度
		//mvariSeqofSPR=第n-1个串联弹簧(0开始)
		//iSEQofPart=第n个支吊架零件（包括螺母）
		//mvSumHeight=组装总高度(mm)
		mvSumHeight = 0;
		iSEQofSPR = 0;
		iSEQofPart = 1;
		CString SQLx,SQLx2;
		//SQLx = _T("SELECT * FROM connect");
		//rsConnect.m_pDatabase=&EDIBgbl::dbPRJ;
		//rsConnect.Open(dbOpenSnapshot,SQLx);
		
		
		//Set mvarrsTmpREF1 = rsTmpREF.Clone
		/*if(rsTmpREF.IsOpen())
		rsTmpREF.Requery();
		else
		{
		rsTmpREF.m_pDatabase=&EDIBgbl::dbPRJ;*/
		COleVariant v,v1,v2;
		if( !rsTmpREF.IsOpen() || (rsTmpREF.IsEOF() && rsTmpREF.IsBOF() )){
			throw (GetResStr(IDS_Null_rsTmpREF));
			return false;
		}
		//上面Ptype数组保留了mvarrsTmpREF的拷贝
		//以便取得支吊架组合的任意2个相邻零件,进行查找
		if(!rsID.IsOpen())
		{
			EDIBgbl::SQLx = _T("SELECT * FROM PictureClipData");
			rsID.m_pDatabase=&EDIBgbl::dbPRJ;
			rsID.Open(dbOpenDynaset,EDIBgbl::SQLx);
			brsIDStatus=TRUE;
		}
		if( ! rsTmpREF.IsEOF() )
		{
			rsTmpREF.MoveLast();
			//展开sFindTBN，加快速度。
			//tbn1 = sFindTBN(Trim(rsTmpREF.Fields(_T("CustomID"))))
			rsTmpREF.GetFieldValue(_T("CustomID"),v);
			if( !rsID.FindFirst(_T("trim(CustomID)=\'") + vtos(v) + _T("\'")))
			{
				//出错
				sTmp.Format(GetResStr(IDS_NoRecordFLDEqvThisValueInPictureClipData),_T("CustomID"),vtos(v));
				throw (sTmp);
				return false;
				//Err.Number = iUE_NoRecordFLDEqvThisValueInPictureClipData
				//Err.Description = ResolveResString(iUE_NoRecordFLDEqvThisValueInPictureClipData, _T("|1"), _T("CustomID"), _T("|2"), Trim(rsTmpREF.Fields(_T("CustomID"))))
				//Err.Raise iUE_NoRecordFLDEqvThisValueInPictureClipData
			}
			
			//假设最后一个零件是根部，保存根部类别码。
			rsID.GetFieldValue(_T("Index"),v);
			mvSA = vtoi(v);
			
			if( rsTmpREF.GetRecordCount() >= 2 )
			{
				//零件数量多于或等于2个，则可以取出头尾零件。
				rsTmpREF.MoveFirst();
				//展开sFindTBN，加快速度。
				//tbn1 = sFindTBN(Trim(rsTmpREF.Fields(_T("CustomID"))))
				rsTmpREF.GetFieldValue(_T("CustomID"),v);
				if( !rsID.FindFirst(CString(_T("trim(CustomID)=\'")) + vtos(v) + _T("\'")))
				{
					//没找到CustomID,出错
					sTmp.Format(GetResStr(IDS_NoRecordFLDEqvThisValueInPictureClipData),_T("CustomID"),vtos(v));
					throw (sTmp);
					return false;
				}
				//假设第一个零件是管部，保存管部类别码。
				rsID.GetFieldValue(_T("Index"),v);
				mvPA = vtoi(v);
				
			}
			CString sv,sv1;
			//根部总重量初始值=0
			mvarSATotalWeight = 0;
			//开始计算
			for (i = 0 ;i< rsTmpREF.GetRecordCount();i++)
			{
				//获得零件所在的原始数据表tbn1,以便查找
				//展开sFindTBN，加快速度。
				//tbn1 = sFindTBN(Trim(rsTmpREF.Fields(_T("CustomID"))))
				rsTmpREF.GetFieldValue(_T("CustomID"),v);
				sv=vtos(v);
				
				if(!rsID.FindFirst(CString(_T("trim(CustomID)=\'")) + sv + _T("\'")))
				{
					//没有找到表名
					sTmp.Format(GetResStr(IDS_NoRecordFLDEqvThisValueInPictureClipData),_T("CustomID"),vtos(v));
					throw (sTmp);
					return false;
				}
				else 
				{
					rsID.GetFieldValue(_T("ClassID"),v);
					modPHScal::glClassID = vtoi(v);
					rsID.GetFieldValue(_T("Index"),v);
					modPHScal::glIDIndex =vtoi(v);
					rsID.GetFieldValue(_T("ID"),v);
					sPartID = vtos(v);
					tmpID0 = sPartID;
				}
				if( modPHScal::glIDIndex == iSA )
				{
					//当前零件是根部或最后一个零件
					if( modPHScal::gbCalSAbyPJG )
					{
						//如果按结构荷载计算根部
						tmpSelPJG = tmpPJG;
						//否则按前一零件的计算荷载选择根部
					}
				}
				else 
				{
					//显示给用户进度条
					//如果不是根部或最后一个零件,则要根据连接的下一个零件,查出其尺寸字段和数量
					//前后紧接的零件ID-->tmpCustomID0,tmpCustomID1
					tmpCustomID0 = sv;
					if( i < rsTmpREF.GetRecordCount() - 1 )
					{
						rsID.GetFieldValue(_T("ID"),v);
						tmpID1=vtos(v);
						/*rsID.FindFirst(CString(_T("trim(CustomID)=\'")) + sv + _T("\'"))*/
						if( rsID.FindFirst(CString(_T("trim(CustomID)=\'")) + sv + _T("\'")))
						{
							rsID.GetFieldValue(_T("ID"),v);
							tmpID1 = vtos(v);
						}
						//不是最后一个零件\
						//从Connect表中检索其是否存在及其匹配的尺寸信息
						//tmpCustomID1 = Ptype(i + 1);
						rsTmpREF.MoveNext();
						if(!rsTmpREF.IsEOF())
						{
							rsTmpREF.GetFieldValue(_T("CustomID"),v);
							sv1=vtos(v);
							sv1.TrimLeft();sv1.TrimRight();
							tmpCustomID1=sv1;
						}
						rsTmpREF.MovePrev();
						if( !rsConnect.FindFirst(CString(_T("trim(cntb)=\'")) + tmpID0 + _T("\' AND trim(cnte)=\'") + tmpID1 + _T("\'")) )
						{
							//Connect表中不存在这种组合.一般不会发生这情况.
							sTmp.Format(GetResStr(IDS_PartCanNotMatchOrNotExistInConnect),tmpCustomID0,tmpCustomID1);
							throw (sTmp);
							return false;
						}
						else 
						{
						/*if( 0 ){
						//改用ConnectCondition字段的复杂SQL条件进行连接尺寸检查，这样的适应性更广。速度慢一点。
						//其语法类似：Cntb.Size2=Cnte.Size2 AND Cntb.size4<=Cnte.Size5
						if( IsNull(rsConnect.Fields(_T("ConnectCondition"))) ){
						tmpSQL0 = _T("")
						}else {
						tmpSQL0 = UCase(Trim(rsConnect.Fields(_T("ConnectCondition"))))
						}
							/*}*/
							rsConnect.GetFieldValue(_T("CntbNum"),v);                
							if( vtoi(v)<= 0 )
							{
								iCntbNum = 1;
							}else 
							{
								iCntbNum = vtoi(v);//rsConnect.Fields(_T("CntbNum"))
							}
							rsConnect.GetFieldValue(_T("CnteNum"),v);                
							if( vtoi(v)<= 0 )
							{
								iCnteNum = 1;
							}else 
							{
								iCnteNum = vtoi(v);//rsConnect.Fields(_T("CnteNum"))
							}
						}
					}
					//else 
					//{
					//是最后一个零件
					//}
				}
				if( i == 0 )
				{
					//第一个零件必定是管部
					//?
					modPHScal::glPAid = modPHScal::glClassID;
					//当前零件数量，用于确定是否能够承受荷载。
					//iNum = iCnteNum / iCntbNum
					//iNumPart = iCntbNum
					//计算模式:选择支吊架模板
					modPHScal::CalSpringParallelNum(sv);
					iNumPart = 1;
				}
				else if( i == rsTmpREF.GetRecordCount() - 1 && modPHScal::glIDIndex == iSA )
				{
					//最后一个零件
					//只有根部可能出现共用情况,glNumSA=rsza(_T("Gnum"))
					iNumPart = modPHScal::glNumSA;
					//MsgBox glNumSA
				}
				else 
				{
					//其它中间连接件
					if( -1 ){
						//每个部件的数量iNumPart=其前一个部件的数量iNumPart*iNum*iCntbNum
						iNumPart = iNumPart * iNum * iCntbNum;
						iNum = iCnteNum / iCntbNum;
						if( iNumPart <= 0 ) 
							iNumPart = modPHScal::gintParallelNum;
					}
					else 
					{
						//当前计算模式：选择支吊架模板。
						//默认零件数量为并联数，以便简化编程。
						//其实在计算横担管部或共用根部时这是有问题的。
						if( modPHScal::glIDIndex == iPA )
						{
							//管部的数量一般=1，只有连结L4时=2。
							iNumPart = 1;
						}else if( modPHScal::glIDIndex == iSA )
							
						{
							//根部的数量=glNumSA
							//此种情况已经在计算根部时列出，本来无需在此处罗列。
							iNumPart = modPHScal::glNumSA;
						}
						else 
						{
							iNumPart = modPHScal::gintParallelNum;
						}
					}
				}
				//显示给用户进度条
				//下面的字段变化只适合于根部,非根部的荷载字段名称均为SAfDPmax=_T("Pmax")
				if( modPHScal::glPAid == iPAh )
				{
					//吊架
					SAfDPmax = _T("PmaxH");
				}
				else if( modPHScal::glPAid == iPAs || modPHScal::glPAid == iPASS || modPHScal::glPAid == iPAGS )
				{
					//滑动支架,水平方向位移小于100mm时,允许承受荷载较大
					if(giUPxyz==1)
					{
						if( fabs(modPHScal::yr1) < 100 && fabs(modPHScal::zr) < 100 )
							SAfDPmax = _T("PmaxSS100");
						else
							SAfDPmax = _T("PmaxSS150");
					}
					else if(giUPxyz==2)
					{
						if( fabs(modPHScal::zr) < 100 && fabs(modPHScal::xr) < 100 )
							SAfDPmax = _T("PmaxSS100");
						else
							SAfDPmax = _T("PmaxSS150");
					}
					else if(giUPxyz==3)
					{
						if( fabs(modPHScal::xr) < 100 && fabs(modPHScal::yr1) < 100 )
							SAfDPmax = _T("PmaxSS100");
						else
							SAfDPmax = _T("PmaxSS150");
					}
				}
				else if( modPHScal::glPAid == iPAfixZ1 || modPHScal::glPAid == iPAfixZ2 )
				{
					//固定支架
					SAfDPmax = _T("PmaxSF");
				}
				//临时条件用于非管部的查找
				if( (iChkCondition & iCNT) != 0 )
				{
					//要求孔尺寸匹配
					if( tmpSQL != _T("") )
					{
						//tmpSQL = tmpSQL + _T(" AND ")
					}
					else 
					{
						tmpSQL = _T("");
					}
				}
				else 
				{
					//如果不需要进行连接孔、杆尺寸自动检查
					if( ! modPHScal::gbConnectHoleDimMatch )
						tmpSQL = _T("");
					else 
					{
						//要求孔尺寸匹配
						if( tmpSQL != _T("") )
						{
							//tmpSQL = tmpSQL + _T(" AND ")
						}
						else 
							tmpSQL = _T("");
					}
				}
				if( i == 0 )
				{
					//第一个零件是管部，无需连接匹配字段信息。它的信息仅仅用于其后的零件使用。
					tmpSQL = _T("");
					tmpFD0valuePrevious = _T("");
				}
				//MsgBox tmpSQL
				//glClassID已经从sFindTBN函数中获得
				switch(modPHScal::glClassID)
				{
				case iPAh:
					for (k = 0 ;k<= (int)modPHScal::gnDW_delta;k++)
					{
						//SQLx = _T("SELECT * FROM ") & tbnPA & _T(" WHERE Dw>= ") & modPHScal::dj * (1 - k * 0.01) & _T(" AND Dw<= ") & modPHScal::dj * (1 + k * 0.01) & _T(" AND Pmax>=") & tmpSelPJG / iNumPart & _T(" AND Tj>=") & tmpT0 & _T(" AND trim(CustomID)='") & Ptype(i) & _T("' ORDER BY Pmax,Dw,tj,Weight")
						sTmp.Format(_T("%g"),modPHScal::dj* (1 - k * 0.01));
						SQLx = _T("SELECT * FROM ") + modPHScal::tbnPA + _T(" WHERE Dw>= ") + sTmp;
						sTmp.Format(_T("%g"),modPHScal::dj* (1 + k * 0.01));
						SQLx+=_T(" AND Dw<= ") + sTmp + _T(" AND Pmax>=");
						sTmp.Format(_T("%g"),(float)(tmpSelPJG / iNumPart));
						SQLx+=sTmp;
						SQLx+= _T(" AND Tj>=") + stmpT0 + _T(" AND trim(CustomID)=\'") + sv + _T("\' ORDER BY tj,Dw,Pmax,Weight");
						//Debug.Print SQLx
						if(rsX.IsOpen())
							rsX.Close();
						rsX.Open(dbOpenSnapshot,SQLx);
						if( rsX.IsEOF() && rsX.IsBOF() )
						{
						}else {
							//找到，退出循环
							goto commonHandle;
						}
					}
					break;
				case iPAfixZ1:
				case iPAfixZ2:
					modPHScal::CalPAfixZ1Z2(Ptype[i], tmpSelPJG, tmpT0, iNumPart, rsX);
					goto spZ1Z2;
					break;
				case iPAs:
				case iPASS:
				case iPAGS:
					for (k = 0 ;k<=(int) modPHScal::gnDW_delta;k++)
					{
						//SQLx = _T("SELECT * FROM ") & tbnPA & _T(" WHERE Dw>= ") & modPHScal::dj * (1 - k * 0.01) & _T(" AND Dw<= ") & modPHScal::dj * (1 + k * 0.01) & _T(" AND Pmax>=") & tmpSelPJG / iNumPart & _T(" AND Tj>=") & tmpT0 & _T(" AND trim(CustomID)='") & Ptype(i) & _T("' ORDER BY Dw,tj,Weight")
						
						sTmp.Format(_T("%g"),modPHScal::dj * (1 - k * 0.01));
						SQLx = _T("SELECT * FROM ") + modPHScal::tbnPA + _T(" WHERE Dw>= ") + sTmp;
						sTmp.Format(_T("%g"),modPHScal::dj * (1 + k * 0.01));
						SQLx+=(_T(" AND Dw<= ") + sTmp + _T(" AND Pmax>=")) ;
						sTmp.Format(_T("%g"),tmpSelPJG / iNumPart);
						SQLx+=sTmp;
						SQLx+= (_T(" AND Tj>=") + stmpT0 + _T(" AND trim(CustomID)=\'") + sv + _T("\' ORDER BY tj,Dw,Weight"));
						if(rsX.IsOpen())
							rsX.Close();
						rsX.Open(dbOpenSnapshot,SQLx);
						if( rsX.IsEOF() && rsX.IsBOF() ){
						}else {
							//找到，退出循环
							goto commonHandle;
						}
						
					}

					//2007.09.27(start)
					if(modPHScal::gbPAForceZero && (rsX.IsEOF() && rsX.IsBOF()))
					{
						for (k = 0 ;k<=(int) modPHScal::gnDW_delta;k++)
						{
							//SQLx = _T("SELECT * FROM ") & tbnPA & _T(" WHERE Dw>= ") & modPHScal::dj * (1 - k * 0.01) & _T(" AND Dw<= ") & modPHScal::dj * (1 + k * 0.01) & _T(" AND Pmax>=") & tmpSelPJG / iNumPart & _T(" AND Tj>=") & tmpT0 & _T(" AND trim(CustomID)='") & Ptype(i) & _T("' ORDER BY Dw,tj,Weight")
							
							sTmp.Format(_T("%g"),modPHScal::dj * (1 - k * 0.01));
							SQLx = _T("SELECT * FROM ") + modPHScal::tbnPA + _T(" WHERE Dw>= ") + sTmp;
							sTmp.Format(_T("%g"),modPHScal::dj * (1 + k * 0.01));
							SQLx+=(_T(" AND Dw<= ") + sTmp + _T(" AND Pmax IS NULL ")) ;
							SQLx+= (_T(" AND Tj>=") + stmpT0 + _T(" AND trim(CustomID)=\'") + sv + _T("\' ORDER BY tj,Dw,Weight"));
							if(rsX.IsOpen())
								rsX.Close();
							rsX.Open(dbOpenSnapshot,SQLx);
							if( rsX.IsEOF() && rsX.IsBOF() ){
							}else {
								//找到，退出循环
								goto commonHandle;
							}
						}
					}
					//2007.09.27(end)

					break;
					
				case iPAD4LA:
				case iPAD11LD:
				case iPADLR:
				case iPALX:
					
					//这些(烟风煤粉管道管部)与管径无关
					//SQLx = _T("SELECT * FROM ") & tbnPA & _T(" WHERE Pmax>=") & tmpSelPJG / iNumPart & _T(" AND Tj>=") & tmpT0 & _T(" AND trim(CustomID)='") & Ptype(i) & _T("' ORDER BY Pmax,tj,Weight")
					
					sTmp.Format(_T("%g"),tmpSelPJG / iNumPart);
					SQLx = CString(_T("SELECT * FROM ")) + modPHScal::tbnPA + _T(" WHERE Pmax>=") + sTmp + _T(" AND Tj>=") + stmpT0 + _T(" AND trim(CustomID)=\'") + sv + _T("\' ORDER BY tj,Pmax,Weight");
					
					/*  if(rsX.IsOpen())
					rsX.Close();
					rsX.Open(dbOpenSnapshot,SQLx);*/
					break;
				case iROD:
					//《管道支吊技术》p202 12.1.2(4)与《火力发电厂汽水管道设计技术规定(DL/T 5054-1996)》p67 7.5.4均指出：
					//拉杆直径DN<=50,d>=10mm;DN>=65,d>=12mm。这些规定与暂行的支吊架标准矛盾。
					//如西北电力院汽水管道支吊架手册 焊接单板D4.76S孔径13mm,只能与L8.10(d=10mm)连接。这就与规定矛盾。
					//因此，这里不能按《技术规定》限制直径。否则，连接尺寸检查将失败。
					//if( modPHScal::dj <= 57 ){
					sTmp.Format(_T("%g"),tmpSelPJG / iNumPart * (rsza->GetCollect(_T("ifLongVertPipe")).boolVal ? iNumPart : 1));
					SQLx = CString(_T("SELECT * FROM ")) + modPHScal::tbnPART + _T(" WHERE ") + tmpSQL + _T(" Pmax>=") +sTmp + _T(" ORDER BY dh,Weight");
					//}else {
					//  SQLx = _T("SELECT * FROM ") + tbnPART + _T(" WHERE ") + tmpSQL + _T(" Pmax>=") + tmpSelPJG / iNumPart * Iif((rsza(_T("ifLongVertPipe")), iNumPart, 1) + _T(" AND trim(CustomID)=//") + Ptype(i) + _T("// ORDER BY dh,Weight")
					//}
					break;
				case iConnected:
				case iAttached:
					sTmp.Format(_T("%g"),tmpSelPJG / iNumPart * (rsza->GetCollect(_T("ifLongVertPipe")).boolVal ? iNumPart : 1));
					SQLx =CString( _T("SELECT * FROM ")) + modPHScal::tbnPART + _T(" WHERE ") + tmpSQL + _T(" Pmax>=") + sTmp + _T(" AND trim(CustomID)=\'") + sv + _T("\' ORDER BY dh,Weight");
					break;
				case iSPR:
					//弹簧的吊杆直径孔自动加大时，需要取消直径孔检查
					SQLx = CString(_T("SELECT * FROM [")) + modPHScal::tbnSPRINGCrude + _T("] WHERE ") + (tmpSQL != _T("") ? (modPHScal::gbSPRAutoLugDia ? _T("") : tmpSQL) : _T("")) + _T(" dh>=0 AND trim(CustomID)=\'") + sv + _T("\' ORDER BY tj,Weight");
					break;
					//SQLx = _T("SELECT * FROM [") + tbnSPRINGCrude + _T("] WHERE ") + Iif((tmpSQL <> _T(""), Iif((gbSPRAutoLugDia, _T(""), tmpSQL), _T("")) + _T(" dh=") + modPHScal::sSprInfo[iSEQofSPR].DH + _T(" AND trim(CustomID)=//") + Ptype(i) + _T("// ORDER BY dh,Weight")
				case iCSPR:
					SQLx =CString( _T("SELECT * FROM [")) + modPHScal::tbnHDCrude + _T("] WHERE ") + tmpSQL  + _T(" dh>=0 AND trim(CustomID)=\'") + sv + _T("\' ORDER BY dh,Weight");
					break;
					//SQLx = _T("SELECT * FROM [") + tbnHDCrude + _T("] WHERE ") + Iif((tmpSQL <> _T(""), tmpSQL, _T("")) + _T(" dh=") + modPHScal::sSprInfo[iSEQofSPR].DH + _T(" AND trim(CustomID)=//") + Ptype(i) + _T("// ORDER BY dh,Weight")
				case iSAh:
					sTmp.Format(_T("%g"),tmpSelPJG / iNumPart * (rsza->GetCollect(_T("ifLongVertPipe")).boolVal ? iNumPart : 1)); 
					SQLx = CString(_T("SELECT * FROM ")) + modPHScal::tbnSA + _T(" WHERE ") + tmpSQL + SAfDPmax + _T(">=") + sTmp + _T(" AND trim(CustomID)=\'") + sv + _T("\' ORDER BY GDW1,") + SAfDPmax;
					break;
				case iSACantilever:
				case iSALbrace:
					sTmp.Format(_T("%g"),modPHScal::gdw1);
					SQLx = CString(_T("SELECT * FROM ")) + modPHScal::tbnSA + _T(" WHERE GDW1>= ") + sTmp + _T(" AND ") + SAfDPmax + _T(">=");
					sTmp.Format(_T("%g"),tmpSelPJG / iNumPart * (rsza->GetCollect(_T("ifLongVertPipe")).boolVal ? iNumPart : 1));
					SQLx+=sTmp  + _T(" AND trim(CustomID)=\'") + sv + _T("\' ORDER BY GDW1,") + SAfDPmax;
					break;
				case iG51:
				case iG56:
				case iG57:
					sTmp.Format(_T("%g"),modPHScal::gdw1);
					SQLx = _T("SELECT * FROM ") + modPHScal::tbnSA + _T(" WHERE GDW1>= ") + sTmp + _T(" AND size2>= ");
					sTmp.Format(_T("%g"),modPHScal::gdw1 / modPHScal::WidthB);
					SQLx +=sTmp  + _T(" AND ") + SAfDPmax + _T(">=");
					sTmp.Format(_T("%g"),tmpSelPJG / iNumPart* (rsza->GetCollect(_T("ifLongVertPipe")).boolVal ? iNumPart : 1));
					SQLx+=sTmp   + _T(" AND trim(CustomID)=\'") + sv + _T("\' ORDER BY GDW1,size2,") + SAfDPmax;
					break;
				case iSAbeam:			
				case iG52_55:
					sTmp.Format(_T("%g"),modPHScal::gdw1);
					if( iNumPart == 2 )
					{
						//两个根部梁承担所有荷载
						SQLx = CString(_T("SELECT * FROM ")) + modPHScal::tbnSA + _T(" WHERE GDW1>=IIF(") + sTmp + _T(">L1/2,L1-") + sTmp + _T(",") + sTmp + _T(") AND L1>= ") ;
						sTmp2.Format(_T("%g"),modPHScal::Lspan);
						SQLx+= sTmp + _T(" AND ") + SAfDPmax + _T(">=");
						sTmp2.Format( _T("%g"), tmpSelPJG / iNumPart * (rsza->GetCollect(_T("ifLongVertPipe")) ? iNumPart : 1));
						SQLx+=sTmp2+ _T(" AND trim(CustomID)=\'") + sv + _T("\' ORDER BY GDW1,L1,") + SAfDPmax;
					}
					else
					{
						//一个根部梁承担所有荷载
						//假定荷载作用在一点，这样选择出来的根部偏于安全。
						SQLx =CString( _T("SELECT * FROM ")) + modPHScal::tbnSA + _T(" WHERE GDW1>=IIF(") + sTmp + _T(">L1/2,L1-") + sTmp + _T(",") + sTmp + _T(") AND L1>= ");
						sTmp2.Format(_T("%g"),modPHScal::Lspan);
						SQLx+= sTmp2 + _T(" AND ") + SAfDPmax + _T(">=");
						sTmp2.Format( _T("%g"), tmpSelPJG / iNumPart * (rsza->GetCollect(_T("ifLongVertPipe")) ? iNumPart : 1));
						SQLx+= sTmp2 + _T(" AND trim(CustomID)=\'") + sv + _T("\' ORDER BY GDW1,L1,") + SAfDPmax;
					}
					break;
				case iSALbraceFixG47:
				case iSALbraceFixG48:
					//公式中变量的单位:mvBWidth,cm;PAfixH,mm->cm; modPHScal::gdw1,mm->cm; 所有力矩,kgf.m->kgf.cm; 所有力,kgf->kgf.
					if(sv!=_T("SJ8"))
					{
						//强度计算:西北院固定支架根部G47(ClassID=iSALbraceFixG47=4),G48(ClassID=iSALbraceFixG48=10)
						if( modPHScal::glClassID == iSALbraceFixG47 )
						{
							mvBWidth = 30;
						}
						else 
						{
							vTmp=rsza->GetCollect(_T("A01"));
							mvBWidth = vtof(vTmp)/ 10;
						}
						bool bLongVertPipe;//是长垂直管道上的刚性吊架
						bLongVertPipe=vtob(rsza->GetCollect(_T("ifLongVertPipe")));
						sngSAfixPl = 1.3 * modPHScal::pxlc / iNumPart * (bLongVertPipe ? iNumPart : 1) - 1 / mvBWidth * (2.6 * modPHScal::PAfixH / 10 - 1.25 * modPHScal::gdw / 10) * modPHScal::pxlc + (1.95 * modPHScal::PAfixH / modPHScal::gdw + 0.5 + 1 / 4 * F1ToWx1 * modPHScal::PAfixH / 10) * modPHScal::pzlc + (1.95 * modPHScal::PAfixH / modPHScal::gdw + 1 / 4 * F1ToWx1) * modPHScal::mxlc * 100 + modPHScal::mylc * 100 / mvBWidth + 2.6 / mvBWidth * modPHScal::mzlc * 100;
						sngSAfixPr = 1.3 * modPHScal::pyrc / iNumPart * (bLongVertPipe ? iNumPart : 1) - 1 / mvBWidth * (2.6 * modPHScal::PAfixH / 10 - 1.25 * modPHScal::gdw / 10) * modPHScal::pxrc + (1.95 * modPHScal::PAfixH / modPHScal::gdw + 0.5 + 1 / 4 * F1ToWx1 * modPHScal::PAfixH / 10) * modPHScal::pzrc + (1.95 * modPHScal::PAfixH / modPHScal::gdw + 1 / 4 * F1ToWx1) * modPHScal::mxrc * 100 + modPHScal::myrc * 100 / mvBWidth + 2.6 / mvBWidth * modPHScal::mzrc * 100;
						//按照西北院支吊架手册1983年版116页解释，各力矩、力的方向可任意。
						if(1)
						{

							sngSAfixPl = 1.3 * fabs(modPHScal::pylc) / iNumPart * (bLongVertPipe ? iNumPart : 1) - 1 / mvBWidth * (2.6 * modPHScal::PAfixH / 10 - 1.25 * modPHScal::gdw / 10) * fabs(modPHScal::pxlc) + (1.95 * modPHScal::PAfixH / modPHScal::gdw + 0.5 + 1 / 4 * F1ToWx1 * modPHScal::PAfixH / 10) * fabs(modPHScal::pzlc) + (1.95 / (modPHScal::gdw/10 )+ 1 / 4 * F1ToWx1) * fabs(modPHScal::mxlc) * 100 + fabs(modPHScal::mylc) * 100 / mvBWidth + 2.6 / mvBWidth * fabs(modPHScal::mzlc) * 100;
							sngSAfixPr = 1.3 * fabs(modPHScal::pyrc) / iNumPart * (bLongVertPipe ? iNumPart : 1) - 1 / mvBWidth * (2.6 * modPHScal::PAfixH / 10 - 1.25 * modPHScal::gdw / 10) * fabs(modPHScal::pxrc) + (1.95 * modPHScal::PAfixH / modPHScal::gdw + 0.5 + 1 / 4 * F1ToWx1 * modPHScal::PAfixH / 10) * fabs(modPHScal::pzrc) + (1.95 / (modPHScal::gdw/10 ) + 1 / 4 * F1ToWx1) * fabs(modPHScal::mxrc) * 100 + fabs(modPHScal::myrc) * 100 / mvBWidth + 2.6 / mvBWidth * fabs(modPHScal::mzrc) * 100;							
						}
						//如果当量力<0,取其绝对值作为当量力 by ligb on 2004.09.11
						if(sngSAfixPl<0) sngSAfixPl=fabs(sngSAfixPl);
						if(sngSAfixPr<0) sngSAfixPr=fabs(sngSAfixPr);
						if( sngSAfixPl > sngSAfixP )
						{
							sngSAfixP = sngSAfixPl;
						}
						else
						{
							sngSAfixP = sngSAfixPr;
						}
						sTmp.Format(_T("%g"),modPHScal::gdw1);
						SQLx = CString(_T("SELECT * FROM ")) + modPHScal::tbnSA + _T(" WHERE GDW1>= ") + sTmp + _T(" AND PmaxSF>=")
							+ ftos(sngSAfixP)+ _T(" AND trim(CustomID)=\'") + sv + _T("\' ORDER BY GDW1,PmaxSF");
					}
					else
					{
						
						//结束强度计算:华东院固定支架根部双臂三角架SJ8(ClassID=iSALbraceFixG47=4)
					}
					break;
					/*case iGCement
					//混凝土结构不必处理
					SQLx = _T("SELECT * FROM ") + tbnSA + _T(" WHERE trim(CustomID)=//") + Ptype(i) + _T("//");
					*/			   
				default:
					sTmp.Format(GetResStr(IDS_CanNotHandlePART),sPartID);
					throw (sTmp);
					return false;
         }
         //从原始数据库中查找满足条件的零件,
         //MsgBox _T("Sqlx=") + SQLx
commonHandle:
         if( modPHScal::glClassID == iPAfixZ1 || modPHScal::glClassID == iPAfixZ2 );
         else 
		 {
			 if(rsX.IsOpen())
				 rsX.Close();
			 rsX.Open(dbOpenSnapshot,SQLx);
         }
spZ1Z2:
         if( rsX.IsEOF() && rsX.IsBOF() )
		 {
			 //Err.Raise iUE_NotFoundPartInZDJCrude, , ResolveResString(iUE_NotFoundPartInZDJCrude, _T("|1"), Ptype(i), _T("|2"), SQLx, _T("|3"), Iif((i > 0, Ptype[i-1], _T("")), _T("|4"), Iif((i < rsTmpREF.RecordCount - 1, Ptype(i + 1), _T("")))
			 if( i > 0 )
			 {
				 CString sv2;
				 rsTmpREF.MovePrev();
				 if(!rsTmpREF.IsBOF())
				 {
					 rsTmpREF.GetFieldValue(_T("CustomID"),v);
					 sv2=vtos(v);
				 }
				 rsTmpREF.MoveNext();
				 if( i < rsTmpREF.GetRecordCount() - 1 ){
					 sTmp.Format(GetResStr(IDS_NotFoundPartInZDJCrude),SQLx,sv,sv2,sv1);
					 throw (sTmp);
					 return false;
				 }else {
					 sTmp.Format(GetResStr(IDS_NotFoundPartInZDJCrude),SQLx,sv,sv2,_T("NULL"));
					 throw (sTmp);
					 return false;
				 }
			 }
			 else 
			 {
				 CString sv2;
				 rsTmpREF.MovePrev();
				 if(!rsTmpREF.IsBOF())
				 {
					 rsTmpREF.GetFieldValue(_T("CustomID"),v);
					 sv2=vtos(v);
				 }
				 rsTmpREF.MoveNext();
				 sTmp.Format(GetResStr(IDS_NotFoundPartInZDJCrude),SQLx,sv,_T("NULL"),sv1);
				 throw (sTmp);
				 return false;
			 }
         }
		 else 
		 {//1
			 rsX.MoveFirst();
			 if(phsAvailableTypeRs.IsOpen())
				 phsAvailableTypeRs.Close();
			 phsAvailableTypeRs.m_pDatabase=rsX.m_pDatabase;
			 phsAvailableTypeRs.Open(dbOpenDynaset,rsX.GetSQL());
			 //phsAvailableTypeRs=CDaoRecordset::GetSQL()
			 //Set phsAvailableTypeRs = rsX
			 //管部测试计算，此时可以退出
			 if( modPHScal::gbAddPartWeight2PMax && modPHScal::glIDIndex != iSA )
			 {
				 //累加重量到计算荷载
				 //忽略附件、螺栓螺母重量，粗略计算拉杆重量（按1m考虑）
				 rsX.GetFieldValue(_T("Weight"),v);
				 tmpSelPJG = tmpSelPJG + vtof(v) * iNumPart;
			 }
			 
			 if( modPHScal::glClassID == iCSPR || modPHScal::glClassID == iSPR )
			 {
				 iSEQofSPR ++;
				 if( iSEQofSPR > modPHScal::giWholeSprNum )
				 {
					 sTmp2.Format(GetResStr(IDS_iSEQofSPROvergiWholeSprNum),_T("\%d"),_T("\%d"));
					 sTmp.Format(sTmp2,iSEQofSPR,modPHScal::giWholeSprNum);
					 throw (sTmp);
					 return false;
				 }
			 }
         }//1
         //保存前一次的连接匹配查询字段信息
         tmpSQL = tmpSQL0;
         //零件数递增
         iSEQofPart ++;
         rsTmpREF.MoveNext();
      }//2
      return true;
   }
   return false;
   }
   catch(::CDaoException * e)
   {
	   e->ReportError();
	   e->Delete();
	   return false;
   }
   catch(_com_error & e)
   {
	   ShowMessage(e.Description());
	   return false;
   }
   catch(CString e)
   {
	   return false;
   }
	catch(CException *e)
	{
		e->Delete();
	}
   catch(...)
   {
	   return false;
   }
}

void Cphs::simplify2(CDaoRecordset &rsPartBoltNuts,int nth)
{
	CString SQLx;
	try
	{
		
        SQLx.Format("SELECT * FROM [ZB] WHERE [VolumeID]=%d AND [ZDJH]=%d AND [nth]=%d AND ( [ClassID]=%d OR [ClassID]=%d OR [ClassID]=%d ) AND [IsSAPart]<>-1 ORDER BY [recno]",
			EDIBgbl::SelVlmID,modPHScal::zdjh ,nth,iBolts,iNuts,iAttached);
		rsPartBoltNuts.Open(dbOpenDynaset,SQLx);				
	}
	catch(::CDaoException *e)
	{
		e->ReportError();
		e->Delete();
	}
	
	
}
#pragma warning( disable : 4129 )  //Added by Shuli Luo
void Cphs::simplify(CDaoRecordset &rsSAPart,int nth)
{
	try{
		CString SQLx;
		SQLx.Format("SELECT * FROM [ZB] WHERE [VolumeID]=%d AND [ZDJH]=%d AND [nth]=%d AND [IsSAPart]<>0 AND IsSAPart IS NOT NULL ORDER BY recno ",
			EDIBgbl::SelVlmID,modPHScal::zdjh,nth);
		rsSAPart.Open(dbOpenDynaset,SQLx);
		if( rsSAPart.IsBOF() && rsSAPart.IsEOF() )
			;
		else
		{
			//删除ZB表中本路支吊架的根部附件以免重复添加根部的附件	
			if(!rsSAPart.IsBOF()) 
				rsSAPart.MoveFirst();
			for(; !rsSAPart.IsEOF(); rsSAPart.MoveFirst())
			{		
				rsSAPart.Delete();
				if( rsSAPart.IsEOF() )
					break;	
			}
		}
	}
	catch(::CDaoException *e)
	{
		e->ReportError();
		e->Delete();
	}
}
#pragma warning( disable : 4129 )  //Added by Shuli Luo
bool Cphs::GetphsBHandSizes(CFrmStatus& frmStatus,int  FirstCal, int  MaxCalCount,int nth,float& TmpPaz,float& TmpPgz)
{
	//	return true;
	//目的:从原始数据表中查找每个零件的编号,尺寸,长度,单重,数量,根部配件及其数量
	//输入:CFrmStatus& frmStatus,显示进度的窗口;
	//int  FirstCal,对某个支吊架某路的计算次数;
	//第一遍计算,FirstCal=1;第二遍计算，FirstCal=2;第三遍计算，FirstCal=3;
	// int  MaxCalCount,最大计算次数，刚性吊架只计算一遍，弹性吊架计算2遍；
	// int nth,支吊架路数，可以按某路计算限位支吊架。
	//返回:连接成功,true;否则False
	//gdw经过了热位移偏装修正，在GetPhsBHandSizes中根据偏装后的根部定位值查表
//	AfxMessageBox("begin");
	CString dbstr = "\n" ;
	bool ret=false;
	CString strSelNumCS;//选择根部主槽钢数量的过滤字符串
	CString *PtypeID=NULL;   //不含附件的组件结构ID数组
	CString *Ptype=NULL;   //不含附件的组件结构CustomID数组
	long *PtypeClassID=NULL;   //不含附件的组件结构ClassID数组
	long *PtypeIndex=NULL;  //不含附件的组件结构Index数组
	
	float m_fDiaM1 = 0.0f;//保存管夹螺栓M1的值；
	CString strDiaM1FieldName;//管夹螺栓M1的尺寸字段，一般为size3或size2
	try
	{
		CString strPipeClampBoltDiameterChanged;//保存改变了螺栓直径的管夹ID标志字符
		_variant_t vnil;
		CString SQL1;
		CString  strClbz;     //add by LFX 2005.5.20
		
		vnil.ChangeType(VT_NULL);
		CString LogicalOpr = _T("=<>") ;  //逻辑操作定界符
		int iLOPnum = 3 ;         //逻辑操作定界符长度，用于尺寸连接匹配比较
		float msOpr=0;long n=0,m=0;
		bool bFound=0;
		CString sTmpSQL,SQLx;   //暂时保存SQLx
		
		//以下一组变量用于计算固定支架的当量力sngSAfixP
		float sngSAfixP=0;   //固定支架的当量力sngSAfixP
		float sngSAfixPl=0 ; //冷态固定支架的当量力sngSAfixPl
		float sngSAfixPr=0 ; //热态固定支架的当量力sngSAfixPr
		//float PAfixH=0 ;  //固定支架管部高度H,cm
		float F1ToWx1=0 ;    //与根部型钢号有关的特性数据，储存在zdjcrudeXBDLY的槽钢特性表SSteelPropertyCS的F1ToWx1字段
		float PAfixC1=0 ;    //固定支架C1值,mm
		float mvBWidth=0 ;     //固定支架根部主槽钢间距,cm,G47=根部表chdist字段值，G48=TZA表A01字段值
		float pnl =0 , pnr=0 ;  //固定支架Z1管部螺栓冷/热态承受的力或Z2合成力,kgf
		float ml =0 , mr=0  ;   //固定支架Z2的合成力矩,kgf.m
		float pxl =0 , pzl =0 , pxr  =0, pzr =0 ; //固定支架冷热态承受的力,kgf
		float mxl =0 , myl  =0, mzl =0 , mxr =0 , myr  =0, mzr=0 ; //固定支架冷热态承受的力矩,kgf.m
		//	float sngCoef;     //固定支架的系数
		
		//modPHScal::gdw=经过偏装修正的根部定位值,mm
		//modPHScal::gdw1=用户输入的未经过偏装修正的根部定位值,mm
		float Height_SPRINGsL5 =0;   //弹簧的花篮螺丝长度
		//float modPHScal::DblHangerRodDist=0 ;  //双吊的间距,移到modPHScal模块，改为全局变量。
		float p1 =0, p2=0   ; //用于自动选择双吊点简支梁槽钢型号的荷载,kgf
		float tmpPJG=0 , tmpT0 =0, tmpGDW1 =0, tmpPmax4PA =0, tmpPmax4Part =0;
		float tmpSelPJG =0 ;//累加零件重量后的计算荷载
		float tmpSelPJG4SPRPaz =0; //累加零件重量后的弹簧安装荷载，用于计算弹簧规格，也即弹簧安装荷载（不包括当前弹簧重量），=支吊点安装荷载+零部件重量
		float tmpSelPJG4SPRPgz =0; //累加零件重量后的弹簧计算荷载，用于计算弹簧规格，也即弹簧工作荷载（不包括当前弹簧重量），=支吊点工作荷载+零部件重量
		float tmpSelPJG4SPRPgzNext=0;  //累加零件重量后的计算荷载，用于计算下一个弹簧规格，也即下一个弹簧规格计算荷载（包括前一个弹簧重量）tmpSelPJG4SPRPgzNext=tmpSelPJG4SPRPgz+前一个弹簧重量
		float tmpSelPJG4SPRPazNext=0;  //累加零件重量后的计算荷载，用于计算下一个弹簧规格，也即下一个弹簧规格计算荷载（包括前一个弹簧重量）tmpSelPJG4SPRPgzNext=tmpSelPJG4SPRPgz+前一个弹簧重量
		float tmpSelPJG4CSPRPgz =0; //累加零件重量后的计算荷载，用于计算恒吊规格，也即恒吊工作荷载（不包括当前恒吊重量），=支吊点工作荷载+零部件重量
		float tmpSelPJG4CSPRPgzNext=0;  //累加零件重量后的计算荷载，用于计算下一个恒吊规格，也即下一个恒吊规格计算荷载（包括前一个恒吊重量）tmpSelPJG4CSPRPgzNext=tmpSelPJG4CSPRPgz+前一个恒吊重量
		float tmpSelPJG4CSPRFiJ =0 ;//临时累加零件重量后的计算荷载，用于计算恒吊与拉杆连接时的荷载，也即恒吊松紧螺母直径计算荷载
		float tmpSelPJG4CSPRFiK=0 ; //临时累加零件重量后的计算荷载，用于计算恒吊与耳子等连接时的荷载，也即恒吊下方荷载为tmpSelPJG,上方荷载为tmpSelPJG4CSPRFiJ+Weight
		float tmpSelOpLoad =0  ; //考虑位移裕量之后求得的工作荷载载荷值tmpSelOpLoad
		//如果条件丢失,则默认为荷载、温度、根部定位条件应该全部满足
		if( iChkCondition==0 ) iChkCondition = iPJG + iTJ + iGDW1 + iCNT + iDW;
		//较为适宜的做法：荷载应该累加零部件重量。
		//根部采用tmpPJG作为荷载计算标准。
		tmpPJG = fabs(modPHScal::pjg);
		//管部、零部件选择荷载tmpPmax4Part也使用tmpPmax4PA，较为合理。
		if( modPHScal::gbCalPARTbyMaxLoad )
			tmpPmax4Part = (fabs(modPHScal::pgz) >= fabs(modPHScal::paz) ? fabs(modPHScal::pgz) : fabs(modPHScal::paz));
		else
			tmpPmax4Part = fabs(modPHScal::pjg);
		
		//设置计算荷载初始值，以便累加
		tmpSelPJG = tmpPmax4Part;
		//设置恒吊松紧螺母直径计算载荷初始值，以便累加
		if( modPHScal::gbCalCSPRLugDiaByPjg )
			tmpSelPJG4CSPRFiJ = tmpPmax4Part;
		else
			tmpSelPJG4CSPRFiJ = fabs(modPHScal::pgz);
		
		//恒吊规格计算载荷初始值=支吊点工作荷载，以便累加
		tmpSelPJG4CSPRPgz = fabs(modPHScal::pgz);
		//弹簧规格计算载荷初始值=支吊点工作荷载，以便累加
		tmpSelPJG4SPRPgz = fabs(modPHScal::pgz) ;
		tmpSelPJG4SPRPaz = fabs(modPHScal::paz);
		tmpT0 =modPHScal::t0;

		CString strOrderBy ;      //排序字符串
		CString strTmp ;          //临时字符串变量
		CString tmpSQL ;          //当前的连接匹配查询字段信息
		CString tmpSQL0 ;         //前一次的连接匹配查询字段信息
		long mlfx =0, mlfx0 =0; //零件/前一个零件左视图的方向
		long mlRot =0, mlRot0 =0; //零件/前一个零件Y轴的方向：与块中方向相同，=0；相反，=180。
		//long lngErrCalPAfix;   //计算固定支架时产生的错误号
		_Recordset* rsza ;
		CDaoRecordset rsX(&modPHScal::dbZDJcrude) ;
		rsza=FrmTxsr.m_pViewTxsr->m_ActiveRs;
		
		CDaoRecordset rsPartBoltNuts(&EDIBgbl::dbPRJDB); //恒吊螺栓螺母
		CDaoRecordset rsSAPart(&EDIBgbl::dbPRJDB); //组成根部的附件表

		CDaoRecordset rsTZB;//当前支吊架当前路数包含所有零部件的表，包括螺栓螺母，按记录号排序
		
		CDaoRecordset rs1(&modPHScal::dbZDJcrude);
		CDaoRecordset rs3(&EDIBgbl::dbPHScode);//材料规范选择记录集//20071018 "dbSORT" 改为 "dbPHScode"
		CString m_strMaterial;//管部保温层以外的零部件选择的材料
		CDaoRecordset rs(&modPHScal::dbZDJcrude);
		CDaoRecordset rsTmp,rsTmp1,rsTmp2;
		CDaoRecordset rsCal;	//根部强度计算公式表CalFormulaOfFixPASA，位于sort.mdb，用户可修改
		CDaoRecordset rsDiaOfCSPRFiJ(&modPHScal::dbZDJcrude);//恒力弹簧拉杆直径表记录集，该集来源于表tbnLugDiaOfCSPR，上方恒吊松紧螺母荷载不要考虑恒吊自重
		CDaoRecordset rsDiaOfCSPRFiK;//恒力弹簧连接耳子直径表记录集，该集来源于表tbnLugDiaOfCSPR，连接耳子荷载要考虑恒吊自重
		rsDiaOfCSPRFiK.m_pDatabase=&modPHScal::dbZDJcrude;
		CString tbn1,SAfDPmax;
		bool m_bFoundL3=false;//发现与管夹连接的双孔吊板(这时可不必换小管夹螺栓)
		//第一次计算，直径初始化
		if( FirstCal == 1 )
		{
			modPHScal::gmiDiameter = modPHScal::giDiameter = 0;
		}
		long i , j, k ;
		CString sBHFormat, sBH ;
		CString sPartID, sRodPASA;
		CString tmpCustomID0 , tmpCustomID1 ;
		CString tmpID0, tmpID1 ;
		CString tmpFD0 ;
		CString tmpExtLenFD;
		float tmpExtLenValue=0 ;  //上一个零件（除拉杆外）插入当前零件L8的深度
		float tmpFD0value=0, tmpFD0valuePrevious=0 ;  //保存前一个零件的连接匹配字段值
		float sngDim=0 , mvSumHeight =0, sngCSLen=0 , sngWeight =0, sngW=0 ;
		long iCntbNum=0 , iCnteNum =0, iNum =0, iNumPA=0; 
		long mvSA  =0; //最后一个零件（根部）的索引号index，用于判断最后一个零件是否为根部
		long mvPA =0;  //第一个零件（管部）的索引号index，用于判断第一个零件是否为管部
		long mviSASSClassID   =0;//根部型钢的类别号ClassID
		long mviSASSIndex   =0;//根部附件的组件类别号Index
		CString mvsSASSMaterial ;  //根部型钢的材料
		long mviPreviousClassID=0;   //前一个部件的类别号ClassID
		// mviSAClassID As Long  //根部的类别号ClassID
		bool bWithBoltsNuts;   //厂家自带螺栓螺母
		int iSumSerialNum =0;  //到当前弹簧止，总的弹簧串联数，为各弹簧串联片数的累加值。
		bool bSPRphs =0;  //当前支吊架是弹(蝶)簧支吊架
		float sngH1xmax=0 ;  //恒吊尺寸H1值，一般H1
		
		//float Tmpyr=0 , TmpPgz=0 ,TmpPaz=0 , tmpCapacity=0;    //pfg2007.08.15原代码 单边恒吊热位移，恒吊/弹簧工作荷载，弹簧安装荷载，载荷容量
		float Tmpyr=0 ,  tmpCapacity=0;    //pfg2007.08.15单边恒吊热位移，恒吊/弹簧工作荷载，弹簧安装荷载，载荷容量
		float Cmin =0, Cmax=0;   //第一个/最后一个规格的载荷容量
		float C1 =0, C2 =0 ;  //当前/前一档恒吊载荷容量
		float tmpDist=0 ;   //临时计算热位移
		short tmpDistID=0 ;   //临时计算热位移代号
		bool m_bFoundCSPR=false ;   //找到合适恒吊
		
		CString mvL1CustomID ;  //保存拉杆L1的CustomID值，用于弹簧或恒吊的拉杆直径选择。当有弹簧时可加快运行速度。
		//用于根部附件查找螺栓螺母用
		CString tmpCustomID ;
		float tmpSize2=0 , tmpSizeH=0 ;
		// iMODE As Long       //计算模式，可用于加快运算速度。
		CString mvSAattachedCustomID ;  //根部附件的CustomID
		
		//如果条件丢失,则默认为第一遍计算状态；
		//否则，为第二遍计算状态。
		if( FirstCal==0 ) FirstCal = 1;
		//iNum=(iCnteNum/iCntbNum)
		//每个部件的数量iNumPart=其前一个部件的数量iNumPart*iNum*iCntbNum
		//iNumPart=iNumPart*iNum*iCntbNum
		//iSA=0,iCSPR=7
		//mviSASSClassID=根部附件型钢类型
		//mvSA=最后一个零件的索引号,用于判别是否是根部,
		//因为在组装尚未完成时,最后一个零件不一定是根部类型
		//mvPA=管部类型,用于统计拉杆的总长度
		//mvariSeqofSPR=第n-1个串联弹簧(0开始)
		//iSEQofPart=第n个支吊架零件（包括螺母,1开始）
		//mvSumHeight=组装总高度(mm)
		mvSumHeight = 0;
		iSEQofSPR = 0;
		iSEQofPart = 1;
		InitListRs();
		
		//Set mvarrsTmpREF1 = rsTmpREF.Clone
		COleVariant vTmp1,vTmp2,vTmp3,vTmp;
		COleVariant vBH;
		CString sTmp,sTmp2;
		CString strSQL;

		simplify(rsSAPart,nth);
		simplify2(rsPartBoltNuts,nth);

		strSQL.Format("SELECT * FROM %s WHERE zdjh=%d AND VolumeID=%d AND nth =%d ORDER BY recno",
			EDIBgbl::Btype[EDIBgbl::TZB],modPHScal::zdjh,EDIBgbl::SelVlmID,nth);
		rsTZB.m_pDatabase=&EDIBgbl::dbPRJDB;
		rsTZB.Open(dbOpenDynaset,strSQL);

		CDaoRecordset rsTmpZB(&EDIBgbl::dbPRJDB);//当前支吊架当前路数零部件表，不含螺栓螺母、根部附件
		strSQL.Format("SELECT CustomID FROM ZB WHERE [VolumeID]=%d AND [ZDJH]=%d AND [nth]=%d AND [ClassID]<>%d AND [ClassID]<>%d AND [ClassID]<>%d AND [IsSAPart]<>-1 Order By recno",
				EDIBgbl::SelVlmID,modPHScal::zdjh,nth,iBolts,iNuts,iAttached);
		rsTmpZB.Open(dbOpenDynaset,strSQL);
		int C;
 		if( rsTmpZB.IsEOF()&& rsTmpZB.IsBOF() )
			throw GetResStr(IDS_Null_rsTmpREF);
		else
		{
			rsTmpZB.MoveLast();
			rsTmpZB.MoveFirst();
			Ptype=new CString[rsTmpZB.GetRecordCount()];
			PtypeID=new CString[rsTmpZB.GetRecordCount()];
			PtypeIndex=new long[rsTmpZB.GetRecordCount()];
			PtypeClassID=new long[rsTmpZB.GetRecordCount()];
			C=rsTmpZB.GetRecordCount();
			CString strBH = sBHFormat;
			for( i = 0 ;i < C;i++)
			{
				rsTmpZB.GetFieldValue(_T("CustomID"),vTmp1);
				Ptype[i] = vtos(vTmp1);
				if( !rsID.FindFirst(_T("trim(CustomID)=\'") + vtos(vTmp1) + _T("\'")))
				{
					//出错
					sTmp.Format(GetResStr(IDS_NoRecordFLDEqvThisValueInPictureClipData),_T("CustomID"),vtos(vTmp1));
					throw sTmp;
				}
				else
				{
					rsID.GetFieldValue(_T("Index"),vTmp1);
					PtypeIndex[i] = vtoi(vTmp1);
					rsID.GetFieldValue(_T("ClassID"),vTmp1);
					PtypeClassID[i] = vtoi(vTmp1);
					rsID.GetFieldValue(_T("ID"),vTmp1);
					PtypeID[i] = vtos(vTmp1);
					if(i>=1 && PtypeID[i]=="L3" && (PtypeID[i-1]=="D1" ||PtypeID[i-1]=="D1A" || PtypeID[i-1]=="D2" || PtypeID[i-1]=="D2A" || PtypeID[i-1]=="D3" || PtypeID[i-1]=="D3A" || PtypeID[i-1]=="D9" || PtypeID[i-1]=="D9A" || PtypeID[i-1]=="D10")) 
						m_bFoundL3=true;//如果存在L3吊板，管夹螺栓是不需要换小的
				}
				rsTmpZB.MoveNext();
			}
			rsTmpZB.MoveFirst();
		}
		//上面Ptype数组保留了mvarrsTmpZB的拷贝
		//以便取得支吊架组合的任意2个相邻零件,进行查找
		
		rsTmpZB.MoveFirst();
		if( ! rsTmpZB.IsEOF() )
		{
			rsTmpZB.MoveLast();
			//展开sFindTBN，加快速度。
			rsTmpZB.GetFieldValue(_T("CustomID"),vTmp1);
			
			if( !rsID.FindFirst(_T("trim(CustomID)=\'") + vtos(vTmp1) + _T("\'")))
			{
				//出错
				sTmp.Format(GetResStr(IDS_NoRecordFLDEqvThisValueInPictureClipData),_T("CustomID"),vtos(vTmp1));
				throw sTmp;
			}
			//假设最后一个零件是根部，保存根部索引码。
			rsID.GetFieldValue(_T("Index"),vTmp1);
			mvSA = vtoi(vTmp1);
			//假设最后一个零件是根部，保存根部索引码。用于统计螺栓螺母时，判断当前零件是恒吊时下一个零件是否为根部。
			PtypeIndex[rsTmpZB.GetRecordCount()- 1] = mvSA;
			//以下确定采用单双槽钢
			rsTmp.m_pDatabase=&modPHScal::dbZDJcrude;
			rsTmp.Open(dbOpenSnapshot,CString(_T("SELECT DISTINCT PNum1 From ")) + modPHScal::tbnSA + 
				_T(" WHERE CustomID=\'") + Ptype[C-1] + _T("\' AND PNum1 is not null"));
			if(rsTmp.GetRecordCount()==1)
			{
				//只有一个记录,它的Pnum1字段=1，只能采用单槽钢；Pnum2=2，只能采用双槽钢。
				rsTmp.GetFieldValue(0,vTmp1);
				if(vtoi(vTmp1) ==1)
					strSelNumCS=_T(" AND PNUM1=1 ");
				if(vtoi(vTmp1) ==2)
					strSelNumCS=_T(" AND PNUM1=2 ");
			}
			else
			{
				switch(modPHScal::iNumCSVal)
				{
				case 1:
					strSelNumCS=_T(" AND PNum1=1 ");
					break;
				case 2:
					strSelNumCS=_T(" AND PNum1=2 ");
					break;
				case 3:
					strSelNumCS=_T(" AND  ");
					break;
				case 0:
					strSelNumCS=_T("");
					break;
				}
			}
			rsTmp.Close();
			if( rsTmpZB.GetRecordCount() >= 2 )
			{
				//零件数量多于或等于2个，则可以取出头尾零件。
				rsTmpZB.MoveFirst();
				//展开sFindTBN，加快速度。
				rsTmpZB.GetFieldValue(_T("CustomID"),vTmp1);
				
				if( !rsID.FindFirst(_T("trim(CustomID)=\'") + vtos(vTmp1) + _T("\'")))
				{
					//出错
					sTmp.Format(GetResStr(IDS_NoRecordFLDEqvThisValueInPictureClipData),_T("CustomID"),vtos(vTmp1));
					throw sTmp;
				}
				//假设第一个零件是管部，保存管部类别码。
				rsID.GetFieldValue(_T("Index"),vTmp1);
				mvPA = vtoi(vTmp1);
				
				//第一个零件是管部,并且最后一个零件是根部。
				//说明目前处于单个支吊架正式计算模式,而不是选择支吊架模板模式。
				//因为后者的最后一个零件绝大多数情况下mvSA<>iSA。
				//以上并不完全正确。
				
				
				//为了加快模板测试计算时的速度，测试计算时要跳出此段程序。
				
				//预先查出拉杆的CustomID，供后面计算弹簧或恒吊时选择拉杆直径使用。
				//因为拉杆可能在弹簧之后出现，故需要先查出与弹簧或恒吊配合的拉杆直径modPHScal::giDiameter。
				//常州电力机械厂引进美国ITT技术生产的恒吊，拉杆直径大于国内标准。
				//为了保证规范的统一，AutoPHS采用tbnPART表内的拉杆直径作为选择依据。
				
				//下段程序功能已经放到GetBoltsNutsAndAttachmentsCLgg过程中完成。
				
				
				//根部总重量初始值=0
				mvarSATotalWeight = 0;
				//开始计算
				C=rsTmpZB.GetRecordCount();
				CString strBH = sBHFormat;
				for (i = 0 ;i<C;i++)
				{
					if(i==0) rsTZB.MoveFirst();
					frmStatus.UpdateStatus((float)(i+1)/C,true);
					frmStatus.UpdateWindow();
					
					//获得零件所在的原始数据表tbn1,以便查找
					//展开sFindTBN，加快速度。
					
					if(! rsID.FindFirst (_T("trim(CustomID)=\'") + Ptype[i] + _T("\'")) )
					{
						//没有找到表名
						sTmp.Format(GetResStr(IDS_NoRecordFLDEqvThisValueInPictureClipData),_T("CustomID"),Ptype[i]);
						throw sTmp;
					}
					rsID.GetFieldValue(_T("ClassID"),vTmp1);
					modPHScal::glClassID =vtoi(vTmp1);
					rsID.GetFieldValue(_T("Index"),vTmp1);
					modPHScal::glIDIndex = vtoi(vTmp1);
					rsID.GetFieldValue(_T("ID"),vTmp1);
					sPartID = vtos(vTmp1);
					tmpID0 = sPartID;
					if( modPHScal::glIDIndex == iSA )
					{
						//给进程显示一点时间
						//当前零件是根部或最后一个零件
						if( modPHScal::gbCalSAbyPJG )
							//如果按结构荷载计算根部
							tmpSelPJG = tmpPJG;
						//否则按前一零件的计算荷载选择根部
						
					}
					else
					{
						//显示给用户进度条
						//如果不是根部或最后一个零件,则要根据连接的下一个零件,查出其尺寸字段和数量
						//前后紧接的零件ID-->tmpCustomID0,tmpCustomID1
						tmpCustomID0 = Ptype[i];
						if( i <C - 1 )
						{
							if( rsID.FindFirst( _T("trim(CustomID)=\'") + Ptype[i + 1] + _T("\'")))
							{
								rsID.GetFieldValue(_T("ID"),vTmp1);
								tmpID1 = vtos(vTmp1);
							}
							//不是最后一个零件
							tmpCustomID1 = Ptype[i + 1];
							//从Connect表中检索其是否存在及其匹配的尺寸信息
							
							if( !rsConnect.FindFirst( _T("trim(cntb)=\'") + tmpID0 + _T("\' AND trim(cnte)=\'") + tmpID1 + _T("\'")))
							{
								//Connect表中不存在这种组合.一般不会发生这情况.
								sTmp.Format(GetResStr(IDS_PartCanNotMatchOrNotExistInConnect),tmpCustomID0,tmpCustomID1);
								throw sTmp;
							}
							//改用ConnectCondition字段的复杂SQL条件进行连接尺寸检查，这样的适应性更广。速度慢一点。
							//其语法类似：Cntb.Size2=Cnte.Size2 AND Cntb.size4<=Cnte.Size5
							rsConnect.GetFieldValue(_T("ConnectCondition"),vTmp1);
							if( vTmp1.vt==VT_NULL)
								tmpSQL0 = _T("");
							else
							{
								tmpSQL0 = vtos(vTmp1);
								tmpSQL0.MakeUpper();
								//如果存在L3吊板，管夹螺栓是不需要换小的
								if(modPHScal::gbPipeClampBolt && !m_bFoundL3)
								{
									//如果要求管夹螺栓可以换小，那么管夹管部的连接尺寸检查就取消
									if( modPHScal::glIDIndex ==iPA &&( tmpID0 == "D1" || tmpID0 == "D1A" ||tmpID0 == "D2" ||   \
										tmpID0 == "D2A" || tmpID0 == "D3" ||tmpID0 == "D3A" ||tmpID0 == "D9" ||tmpID0 == "D9A" ||tmpID0 == "D10" ))
									{
										modPHScal::g_bPipeClampBoltDiameterChanged=TRUE;
										//找出管夹螺栓M1的尺寸字段，一般存在于连接字符串cntb.之后=之前
										int i1=tmpSQL0.Find("<",5);
										if(i1==-1) 
										{
											i1=tmpSQL0.Find(">",5);
											if(i1==-1) 
											{
												i1=tmpSQL0.Find("=",5);
											}
											{
												//没找到任何逻辑操作符号，一般由于数据库出了错误
											}
										}
										strDiaM1FieldName=tmpSQL0.Mid(5,i1-5);
										//连接字符串置空，则不检查连接螺栓尺寸，这样管夹螺栓即换小
										tmpSQL0 = _T("");
									}
									else
									{
										
									}
									
								}
								else
								{
									
								}
								
							}
							
							rsConnect.GetFieldValue(_T("CnteXfx"),vTmp1);
							mlfx0 =vtoi(vTmp1);
							rsConnect.GetFieldValue(_T("CnteRotation"),vTmp1);
							mlRot0 = vtoi(vTmp1);
							rsConnect.GetFieldValue(_T("CntbNum"),vTmp1);
							iCntbNum=(vtoi(vTmp1)<=0 ? 1 : vtoi(vTmp1));
							rsConnect.GetFieldValue(_T("CnteNum"),vTmp1);
							iCnteNum=(vtoi(vTmp1)<=0 ? 1 : vtoi(vTmp1));
						}
						
						//是最后一个零件
					}
					if( i == 0 )
					{
						//第一个零件必定是管部
						mlfx = modPHScal::GetPhsSAfx(_T("P") + modPHScal::df);
						modPHScal::glPAid = modPHScal::glClassID;
						//当前零件数量，用于确定是否能够承受荷载。
						iNum = iCnteNum / iCntbNum;
						iNumPart = iCntbNum;
					}
					else if( i == (C - 1) )
					{
						//最后一个零件
						//只有根部可能出现共用情况,glNumSA=rsza(_T("Gnum"))
						iNumPart = modPHScal::glNumSA;
					}
					else
					{
						//其它中间连接件
						//每个部件的数量iNumPart=其前一个部件的数量iNumPart*iNum*iCntbNum
						
						iNumPart = iNumPart * iNum * iCntbNum;
						iNum = iCnteNum / iCntbNum;
						if( iNumPart <= 0 ) iNumPart = modPHScal::gintParallelNum;
						//管部以外的零件的材料，较细致的做法是按照材料规范选择表按不同类别零件分别选择；此处简单地取默认值；
						SQLx.Format(_T("SELECT * FROM SpecificationOfMaterial WHERE ClassIndex=%d AND ID=\'default\' AND tmin<=%g AND %g<tmax ORDER BY tmin,tmax,SEQ"),modPHScal::giClassIndex,modPHScal::gnConnectTJ,modPHScal::gnConnectTJ);
						if(rs3.IsOpen())
							rs3.Close();
						rs3.Open(dbOpenSnapshot,SQLx);
						if(rs3.IsEOF() && rs3.IsBOF())
						{
							//在%s库%s材料选择规范表没有%s字段值为默认值%s的记录(非管部零件材料选择规则)
							sTmp.Format(IDS_NoDefaultInCustomIDInSpecificationOfMaterial,rs3.m_pDatabase->GetName(),_T("SpecificationOfMaterial"),_T("ID"),_T("default"));
							throw sTmp;
						}
						else
						{
							rs3.GetFieldValue(_T("Material"),vTmp);
							m_strMaterial=vtos(vTmp);
						}
					}
					//显示给用户进度条
					//下面的字段变化只适合于根部,非根部的荷载字段名称均为SAfDPmax=_T("Pmax")
					if( modPHScal::glPAid == iPAh || modPHScal::glPAid == iPAD4LA || modPHScal::glPAid == iPAD4LQ || modPHScal::glPAid == iPAD11LD || modPHScal::glPAid == iPADLR  || modPHScal::glPAid == iPALX )
					{
						//吊架
						SAfDPmax = _T("PmaxH");
					}
					else if( modPHScal::glPAid == iPAs || modPHScal::glPAid == iPASS|| modPHScal::glPAid == iPAGS  || modPHScal::glPAid == iPAXZ1_XZ3 )
					{
						//滑动支架
						if(giUPxyz==1)
						{
							if( fabs(modPHScal::yr1) < 100 && fabs(modPHScal::zr) < 100 )
								SAfDPmax = _T("PmaxSS100");
							else
								SAfDPmax = _T("PmaxSS150");
						}
						else if(giUPxyz==2)
						{
							if( fabs(modPHScal::zr) < 100 && fabs(modPHScal::xr) < 100 )
								SAfDPmax = _T("PmaxSS100");
							else
								SAfDPmax = _T("PmaxSS150");
						}
						else if(giUPxyz==3)
						{
							if( fabs(modPHScal::xr) < 100 && fabs(modPHScal::yr1) < 100 )
								SAfDPmax = _T("PmaxSS100");
							else
								SAfDPmax = _T("PmaxSS150");
						}
						
					}
					else if( modPHScal::glPAid == iPAfixZ1 || modPHScal::glPAid == iPAfixZ2 )
					{
						//固定支架
						SAfDPmax = _T("PmaxSF");
					}
					if( tmpSQL != _T("") )
						tmpSQL = tmpSQL + _T(" AND ");
					
					//
					//如果不需要进行连接孔、杆尺寸自动检查
					if( ! modPHScal::gbConnectHoleDimMatch ) 
						tmpSQL = _T("");
					else
					{
						//要求孔尺寸匹配
						if( tmpSQL != _T("") )
						{
						}
						else 
							tmpSQL = _T("");
					}
					
					//第一个零件是管部，无需连接匹配字段信息。它的信息仅仅用于其后的零件使用。
					switch(modPHScal::glClassID)
					{
					case iPAXZ1_XZ3:
					case iPAh:						
						for (k = 0 ;k<= modPHScal::gnDW_delta;k++)
						{//从零部件数据表中找一个与当前支吊架管部外径modPHScal::dj最接近的外径
						 //最终外径误差不会超过modPHScal::gnDW_delta/100。
							sTmp.Format(_T("%g"),modPHScal::dj*(1-k*0.01));
							SQLx = _T("SELECT * FROM ") + modPHScal::tbnPA + _T(" WHERE Dw>= ") + sTmp + _T(" AND Dw<= ") ;
							sTmp.Format(_T("%g"),modPHScal::dj * (1 + k * 0.01));
							SQLx+=sTmp  + _T(" AND Pmax>=");
							sTmp.Format(_T("%g"),tmpSelPJG / iNumPart);
							SQLx+=sTmp  + _T(" AND Tj>=");
							sTmp.Format(_T("%g"),tmpT0);
							SQLx+= sTmp + _T(" AND trim(CustomID)=\'") + Ptype[i] + _T("\' ORDER BY tj,Dw,Pmax,Weight");
							if(rsX.IsOpen())
								rsX.Close();
							rsX.Open(dbOpenSnapshot,SQLx);
							if( rsX.IsEOF() && rsX.IsBOF() )
								;
							else
								//没找到，每次外径递增1%，继续找
								k=modPHScal::gnDW_delta+1;		  
						}
						break;
					case iPAfixZ1://管夹固定支架Z1
					case iPAfixZ2://焊接固定支架Z2
						modPHScal::CalPAfixZ1Z2(Ptype[i], tmpSelPJG, tmpT0, iNumPart, rsX);
						goto spZ1Z2;
						break;
					case iPAs://普通支座
					case iPASS://滑动支座
					case iPAGS://导向支座
						for( k = 0 ;k<= modPHScal::gnDW_delta;k++)
						{
							sTmp.Format(_T("%g"),modPHScal::dj * (1 - k * 0.01));
							SQLx = _T("SELECT * FROM ") + modPHScal::tbnPA + _T(" WHERE Dw>= ") +sTmp+ _T(" AND Dw<= ") ;
							sTmp.Format(_T("%g"),modPHScal::dj * (1 + k * 0.01));
							SQLx+= sTmp + _T(" AND Pmax>=");
							sTmp.Format(_T("%g"),tmpSelPJG/iNumPart);
							SQLx+=sTmp + _T(" AND Tj>=");
							sTmp.Format(_T("%g"),tmpT0);
							SQLx+= sTmp + _T(" AND trim(CustomID)=\'") + Ptype[i] + _T("\' ORDER BY tj,Dw,Weight");
							if(rsX.IsOpen())
								rsX.Close();
							rsX.Open(dbOpenSnapshot,SQLx);
							if( rsX.IsEOF() && rsX.IsBOF() );
							else
								//没找到，每次外径递增1%，继续找
								k=modPHScal::gnDW_delta+1;
							
						}
						//2007.09.26(start)
						if(modPHScal::gbPAForceZero && (rsX.IsEOF() && rsX.IsBOF()))
						{
							for( k = 0 ;k<= modPHScal::gnDW_delta;k++)
							{
								sTmp.Format(_T("%g"),modPHScal::dj * (1 - k * 0.01));
								SQLx = _T("SELECT * FROM ") + modPHScal::tbnPA + _T(" WHERE Dw>= ") +sTmp+ _T(" AND Dw<= ") ;
								sTmp.Format(_T("%g"),modPHScal::dj * (1 + k * 0.01));
								SQLx+= sTmp + _T(" AND Pmax IS NULL");
								SQLx+= _T(" AND Tj>=");
								sTmp.Format(_T("%g"),tmpT0);
								SQLx+= sTmp + _T(" AND trim(CustomID)=\'") + Ptype[i] + _T("\' ORDER BY tj,Dw,Weight");
								if(rsX.IsOpen())
									rsX.Close();
								rsX.Open(dbOpenSnapshot,SQLx);
								if( rsX.IsEOF() && rsX.IsBOF() )
								{
									;
								}
								else
								{
									k=modPHScal::gnDW_delta+1;
									AfxMessageBox("警告:\n\n管部允许荷载没有,\n程序将不验算管部荷载!");
								}

							}
						}
						//2007.09.26(end)
						break;
					case iPAD4LA:
					case iPAD11LD:
					case iPADLR:
					case iPALX:
						//这些(烟风煤粉管道管部)与管径无关
						SQLx = _T("SELECT * FROM ") + modPHScal::tbnPA + _T(" WHERE Pmax>=") ;
						sTmp.Format(_T("%g"),tmpSelPJG/iNumPart);
						SQLx+=sTmp+ _T(" AND Tj>=");
						sTmp.Format(_T("%g"),tmpT0);
						SQLx+=sTmp+_T(" AND trim(CustomID)=\'") + Ptype[i] + _T("\' ORDER BY tj,Pmax,Weight");
						if(rsX.IsOpen())
							rsX.Close();
						rsX.Open(dbOpenSnapshot,SQLx);
						break;
					case iROD:
						//《管道支吊技术》p202 12.1.2(4)与《火力发电厂汽水管道设计技术规定(DL/T 5054-1996)》p67 7.5.4均指出：
						//拉杆直径DN<=50,d>=10mm;DN>=65,d>=12mm。这些规定与暂行的支吊架标准矛盾。
						//如西北电力院汽水管道支吊架手册 焊接单板D4.76S孔径13mm,只能与L8.10(d=10mm)连接。这就与规定矛盾。
						//因此，这里不能按《技术规定》限制直径。否则，连接尺寸检查将失败。
						
						SQLx = _T("SELECT * FROM ") + modPHScal::tbnPART + _T(" WHERE ") + tmpSQL + _T(" Pmax>=");
						sTmp.Format(_T("%g"),(tmpSelPJG / iNumPart) * (vtob(rsza->GetCollect(_T("ifLongVertPipe"))) ? iNumPart : 1 ));
						SQLx+=sTmp ;
						if(modPHScal::gbAutoApplyCSPRLugDia 
							&& (FirstCal == 2) 
							&& modPHScal::gbPhsIsCSPR 
							&& (i<modPHScal::PtypeIndexForDiameter )
							&& (modPHScal::PtypeDiameter[i] > 0)
							)
							//恒力弹簧拉杆直径
							sTmp.Format(_T(" AND size2>=%d"),modPHScal::PtypeDiameter[i]);
						else if(!modPHScal::gbAutoSPRRodDiaDecrease 
							&& (FirstCal == 2) 
							&& modPHScal::gbPhsIsSPR 
							&& (i<modPHScal::PtypeIndexForDiameter )
							&& (modPHScal::PtypeDiameter[i] > 0)
							)
							//弹簧拉杆直径
							sTmp.Format(_T(" AND size2>=%d"),modPHScal::PtypeDiameter[i]);
						else
							sTmp=_T("");
						SQLx+=sTmp;
						
						//AfxMessageBox("Rod diameter="+ltos(modPHScal::PtypeDiameter[i])); //HS->add 
						//modPHScal::PtypeDiameter[i] 
						
						sTmp=(modPHScal::gbLimitLugMinDia ? (modPHScal::dj >= 76 ? _T(" AND size2>=12") : (modPHScal::dj >= 57 ? _T(" AND size2>=10") : _T(" AND size2>=10"))) : _T(" "));
						SQLx+= sTmp + _T(" AND trim(CustomID)=\'") + Ptype[i] + _T("\' ORDER BY dh,tj,Weight");
						
						break; 
						
					case iConnected:
					case iXL1:
					case iAttached: 
						if( PtypeID[i] == _T("L5") || PtypeID[i] == _T("L6") || PtypeID[i] == _T("L7") || PtypeID[i] == _T("L8")  || PtypeID[i] == _T("L7Dd") || PtypeID[i] == _T("L8Dd") )
						{
							//拉杆直径
							if(PtypeClassID[i-1]==iROD)
							{
								//前一个是拉杆			  
								modPHScal::PtypeDiameter[i]=modPHScal::PtypeDiameter[i-1];
								if(modPHScal::iMaxRodDia<modPHScal::PtypeDiameter[i])
								{
									
								}
								else
								{
									
								}
								
								//added by ywh 2003/07/19	
								if(modPHScal::gbRodDiaByMax)
								{
									if(modPHScal::iMaxRodDia>modPHScal::PtypeDiameter[i])
									{
										modPHScal::PtypeDiameter[i]=modPHScal::iMaxRodDia;
										modPHScal::PtypeDiameter[i-1]=modPHScal::iMaxRodDia;
									}
									else
										modPHScal::iMaxRodDia=modPHScal::PtypeDiameter[i];
								}
								
								
							}
							else
							{
								//前一个不是拉杆
								if(PtypeClassID[i+1]==iROD)
								{
									//后一个是拉杆
									modPHScal::PtypeDiameter[i]=modPHScal::PtypeDiameter[i+1];
									
									if(modPHScal::gbRodDiaByMax)
									{
										if(modPHScal::iMaxRodDia>modPHScal::PtypeDiameter[i])
										{
											modPHScal::PtypeDiameter[i]=modPHScal::iMaxRodDia;
											modPHScal::PtypeDiameter[i+1]=modPHScal::iMaxRodDia;
										}
										else
											modPHScal::iMaxRodDia=modPHScal::PtypeDiameter[i];
									}
									
								}
								else
								{
									//后一个不是拉杆
									if(i+2<=C-1 && PtypeClassID[i+2]==iROD)
									{
										//后第二个是拉杆
										modPHScal::PtypeDiameter[i]=modPHScal::PtypeDiameter[i+2];
										
										if(modPHScal::gbRodDiaByMax)
										{
											if(modPHScal::iMaxRodDia>modPHScal::PtypeDiameter[i])
											{
												modPHScal::PtypeDiameter[i]=modPHScal::iMaxRodDia;
												modPHScal::PtypeDiameter[i+2]=modPHScal::iMaxRodDia;
											}
											else
												modPHScal::iMaxRodDia=modPHScal::PtypeDiameter[i];
										}
										
									}
									else
									{
										//后第二个不是拉杆
										if(i>=2 && PtypeClassID[i-2]==iROD)
										{
											//前第二个是拉杆
											modPHScal::PtypeDiameter[i]=modPHScal::PtypeDiameter[i-2];
											
											if(modPHScal::gbRodDiaByMax)
											{
												if(modPHScal::iMaxRodDia>modPHScal::PtypeDiameter[i])
												{
													modPHScal::PtypeDiameter[i]=modPHScal::iMaxRodDia;
													modPHScal::PtypeDiameter[i-2]=modPHScal::iMaxRodDia;
												}
												else
													modPHScal::iMaxRodDia=modPHScal::PtypeDiameter[i];
											}
											
										}
										else
										{
											//前第二个不是拉杆
										}
									}
								}
							}
							
							SQLx = _T("SELECT * FROM ") + modPHScal::tbnPART + _T(" WHERE ") + tmpSQL + _T(" Pmax>=");
							sTmp.Format(_T("%g"),tmpSelPJG / iNumPart * (vtob(rsza->GetCollect(_T("ifLongVertPipe")))? iNumPart : 1));
							SQLx+=sTmp;
							if(modPHScal::gbAutoApplyCSPRLugDia 
								&& (FirstCal == 2) 
								&& modPHScal::gbPhsIsCSPR 
								&& (i<modPHScal::PtypeIndexForDiameter )
								&& (modPHScal::PtypeDiameter[i] > 0)
								)
								//恒力弹簧拉杆直径
								sTmp.Format(_T(" AND size2>=%d"),modPHScal::PtypeDiameter[i]);
							else if(!modPHScal::gbAutoSPRRodDiaDecrease 
								&& (FirstCal == 2) 
								&& modPHScal::gbPhsIsSPR 
								&& (i<modPHScal::PtypeIndexForDiameter )			  
								&& (modPHScal::PtypeDiameter[i] > 0)
								)
							{
								sTmp.Format(_T(" AND size2>=%d"),modPHScal::PtypeDiameter[i]);
							}
							else
							{
								sTmp=_T("");
								if(modPHScal::gbRodDiaByMax)//如果拉杆上下一致，那么修改选择条件
								{
									CString str;
									str.Format ("AND size2>=%d",modPHScal::iMaxRodDia);
									sTmp = str;	
								}		  
							}
							SQLx+=sTmp;
							sTmp=(modPHScal::gbLimitLugMinDia ? (modPHScal::dj >= 76 ? _T(" AND size2>=12") : (modPHScal::dj >= 57 ? _T(" AND size2>=10") : _T(""))): _T(""));
							
							SQLx+=sTmp  + _T(" AND trim(CustomID)=\'") + Ptype[i] + _T("\' ORDER BY dh,tj,Weight");
							
						}
						else if(PtypeID[i]==_T("L15"))
						{	//连接件L15实际上是一块钢板
							//验证钢板大小
							if(modPHScal::gfPSWidth<=(modPHScal::dj+20))
								modPHScal::gfPSWidth=int((modPHScal::dj+20)/10)*10;
							if(modPHScal::gfPSLength<=(modPHScal::dj+20))
								modPHScal::gfPSLength=int((modPHScal::dj+20)/10)*10;
							//钢板规格
							sTmp.Format(_T("%sx%sx%s"),ftos(modPHScal::gfPSThickness),ftos(modPHScal::gfPSWidth),ftos(modPHScal::gfPSLength));
							//钢板重量kg
							float fPSWeight=modPHScal::gfPSThickness * modPHScal::gfPSWidth * modPHScal::gfPSLength * 7850e-9;
							SQLx = _T("SELECT * FROM ") + modPHScal::tbnAttachment + _T(" WHERE CustomID=\'PS\' AND BH=\'") + sTmp + _T("\' ORDER BY BH,Weight");
							if(rsTmp.IsOpen())
								rsTmp.Close();
							rsTmp.m_pDatabase=&modPHScal::dbZDJcrude;
							rsTmp.Open(dbOpenDynaset,SQLx);
							if(rsTmp.IsBOF() && rsTmp.IsEOF())
							{
								//如果不存在这块钢板,加入它。
								/*rsTmp.AddNew();
								rsTmp.SetFieldValue(_T("CustomID"),COleVariant(_T("PS")));
								rsTmp.SetFieldValue(_T("BH"),COleVariant(sTmp));
								rsTmp.SetFieldValue(_T("size2"),COleVariant(modPHScal::gfPSThickness));
								rsTmp.SetFieldValue(_T("Weight"),COleVariant(fPSWeight));
								rsTmp.Update();*/
								modPHScal::dbZDJcrude.Execute(_T("INSERT INTO [")+ modPHScal::tbnAttachment + _T("] (CustomID,BH,size2,Weight) VALUES (\'PS\',\'") + sTmp + _T("\',") + ftos(modPHScal::gfPSThickness) + _T(",") + ftos(fPSWeight) + _T(")"));
							}		  
						}
						else if(PtypeID[i]==_T("L3"))
						{
							//下孔径size3按升序排列，即可首先选择到小孔的双孔吊板
							SQLx = _T("SELECT * FROM ") + modPHScal::tbnPART + _T(" WHERE ") + tmpSQL + _T(" Pmax>=");
							sTmp.Format(_T("%g"),tmpSelPJG / iNumPart * (vtob(rsza->GetCollect(_T("ifLongVertPipe")))? iNumPart : 1));
							SQLx+=sTmp + _T(" AND trim(CustomID)=\'") + Ptype[i] + _T("\' ORDER BY tj,size3,size2,Weight");
						}
						else
						{
							SQLx = _T("SELECT * FROM ") + modPHScal::tbnPART + _T(" WHERE ") + tmpSQL + _T(" Pmax>=");
							sTmp.Format(_T("%g"),tmpSelPJG / iNumPart * (vtob(rsza->GetCollect(_T("ifLongVertPipe")))? iNumPart : 1));
							SQLx+=sTmp + _T(" AND trim(CustomID)=\'") + Ptype[i] + _T("\' ORDER BY dh,tj,Weight");
						}
			   break;
	case iSPR:
		//弹簧的吊杆直径孔自动加大时，需要取消直径孔检查
		//取该类弹簧的最后一个规格
		//SQLx = _T("SELECT TOP 1 * FROM [") + tbnSPRINGCrude + _T("] WHERE ") + IIf(tmpSQL <> _T(""), IIf(gbSPRAutoLugDia, _T(""), tmpSQL), _T("")) + _T(" (dh mod 100)=") + modPHScal::sSprInfo[iSEQofSPR].SprNo + _T(" AND int(dh/100)<=") + (giSumSerialNum - iSumSerialNum) + _T(" AND trim(CustomID)=//") + Ptype[i] + _T("// ORDER BY dh DESC,Weight")
		SQLx = _T("SELECT * FROM [") + modPHScal::tbnSPRINGCrude + _T("] WHERE ") + (tmpSQL != _T("") ? (modPHScal::gbSPRAutoLugDia ? _T("") : tmpSQL) : _T(""));
		if(iSEQofSPR < modPHScal::SprInfoIndex)
			sTmp.Format(_T(" dh=%d "),modPHScal::sSprInfo[iSEQofSPR].DH);
		else
			sTmp=_T(" ");
		SQLx+=sTmp + _T(" AND trim(CustomID)=\'") + Ptype[i] + _T("\' ORDER BY dh,Weight");
		if ( sPartID == _T("T5") && FirstCal >=2 )
		{
			if(!modPHScal::gbAddPartWeight2SPRPgz) //pfg2007.08.21
			{
				modPHScal::g_fLHGWeight=0;	
			}
			TmpPgz = (fabs(tmpSelPJG4SPRPgz)+modPHScal::g_fLHGWeight) / modPHScal::gintParallelNum;
			TmpPaz = (fabs(tmpSelPJG4SPRPaz)+modPHScal::g_fLHGWeight) / modPHScal::gintParallelNum;
		}
		else
		{
			TmpPgz = fabs(tmpSelPJG4SPRPgz) / modPHScal::gintParallelNum;
			TmpPaz = fabs(tmpSelPJG4SPRPaz) / modPHScal::gintParallelNum;
		}
		break;
	case iCSPR:
		if(iSEQofSPR < modPHScal::giWholeSprNum)
			modPHScal::giCSPRindex[iSEQofSPR] = i;
		//复位特殊设计标志
		EDIBgbl::dbPRJ.Execute(_T("UPDATE tmpCSLen SET SpecialDesign=\'\'"));
		if ( sPartID == _T("LHG") && FirstCal >=2 )
		{
			TmpPgz = (fabs(tmpSelPJG4CSPRPgz)+modPHScal::g_fLHGWeight) / modPHScal::gintParallelNum;
			TmpPaz= TmpPgz;
		}
		else
		{
			TmpPgz = fabs(tmpSelPJG4CSPRPgz) / modPHScal::gintParallelNum;
			TmpPaz= TmpPgz;
		}
		Tmpyr = fabs(modPHScal::yr);         //yr 单位:mm
		if( fabs(Tmpyr) * modPHScal::gUseConstantSpringPercentSurplusDist < modPHScal::gUseConstantSpringMinSurplusDist )
		{
			Tmpyr = fabs(Tmpyr) + modPHScal::gUseConstantSpringMinSurplusDist;
		}
		else
		{
			Tmpyr = fabs(Tmpyr) * (1 + modPHScal::gUseConstantSpringPercentSurplusDist);
		}
		SQLx = _T("SELECT * FROM [") + modPHScal::tbnHDproperty + _T("] ORDER BY Capacity");
		if(rs.IsOpen())
			rs.Close();
		rs.Open(dbOpenSnapshot,SQLx);
		if( rs.IsEOF() && rs.IsBOF() )
		{
			//恒吊载荷容量表为空，不可能
			sTmp.Format(GetResStr(IDS_NullTableInXMdb),modPHScal::dbZDJcrude.GetName(), modPHScal::tbnHDproperty);
			throw sTmp;
		}
		else
		{
			rs.GetFieldValue(_T("Capacity"),vTmp1);
			Cmin = vtof(vTmp1);
			rs.MoveLast();
			rs.GetFieldValue(_T("Capacity"),vTmp1);
			Cmax = vtof(vTmp1);
			rs.MoveFirst();
			//复位：找到合适恒吊标志
			m_bFoundCSPR = false;
			//初始化时串联数iSumSerialNum=0
			iSumSerialNum = iSumSerialNum + 1;
			tmpDist = Tmpyr / modPHScal::giWholeSprNum;
			//查位移系列值，按就近上靠原则确定位移系列。
			if(rs1.IsOpen())
				rs1.Close();
			sTmp.Format(_T("%g"),tmpDist);
			rs1.Open(dbOpenSnapshot,_T("SELECT * FROM [") + modPHScal::tbnDisplacementSerial + _T("] WHERE DisplacementSerial>=") + sTmp + _T(" ORDER BY DisplacementSerial"));
			
			if( rs1.IsEOF() && rs1.IsBOF() )
				//位移超出系列最大值
				;
			else
			{
				rs1.GetFieldValue(_T("DisplacementSerial"),vTmp1);
				tmpDist =vtof(vTmp1);
				//by ligb on 2008.05.09 
				try
				{
					rs1.GetFieldValue(_T("MovementID"),vTmp1);//这个字段是新加的，只有2007版江苏苏源恒力弹簧才有这个字段值。因此其它库可能出错，需要捕获错误。
					tmpDistID =vtoi(vTmp1);
				}
				catch(...)
				{
					tmpDistID=0;//其它恒力弹簧不需要这个位移代号值，设置为0即可。
				}
				
				//载荷容量单位为kN*mm,即N*m,位移单位为mm,荷载单位为kgf(或daN=10N)
				tmpCapacity = TmpPgz * tmpDist * EDIBgbl::kgf2N / 1000;
				if( tmpCapacity < Cmin )
				{
					//荷载容量比最小的还小
					sTmp.Format(GetResStr(IDS_CapacityLitterCmin),_T("\%g"),_T("\%g"));
					sTmp2.Format(sTmp,tmpCapacity, Cmin);
					throw sTmp2;
				}
				else
				{
					if( tmpCapacity > Cmax )
					{
						//荷载容量比最大的还大
						//串联数加1，继续循环
					}
					else
					{
						//荷载容量合适
						sTmp.Format(_T("%g"),tmpCapacity);
						
						if(!rs.FindFirst(_T("Capacity>=") + sTmp) );
						//没找到，不可能
						
						else
						{
							//找到了
							rs.GetFieldValue(_T("minDist"),vTmp1);
							if( tmpDist < vtof(vTmp1) )
							{
								//比最小位移小，应该用弹簧
								sTmp.Format(GetResStr(IDS_ShouldUseSpring),_T("\%g"),_T("\%g"));
								sTmp2.Format(sTmp,tmpDist,vtof(vTmp1));
								throw sTmp2;
								
							}
							else
							{
								rs.GetFieldValue(_T("maxDist"),vTmp1);
								if( tmpDist > vtof(vTmp1) )
								{
									//比最大位移还大
									if( iSumSerialNum >= modPHScal::giWholeSprNum )
									{
										//超过模板中的串联数
										//if( gbCSPRCanSpecialDesign ){
										//恒吊需要特殊设计（即恒吊不允许串联使用）,
										//只要一个恒吊就可以了
										//就用这种位移作为订货依据
										modPHScal::gbCSPRneedSpecialDesign = true;
										EDIBgbl::dbPRJ.Execute(GetResStr(IDS_ESPECIAL_DESIGN_SQL));
										m_bFoundCSPR = true;
										//}else{
										//恒吊不需要特殊设计（即恒吊允许串联使用）
										//继续循环
										//}
									}
									//	 else{
									//小于模板中的串联数
									//继续循环
									//}
								}
								else
								{
									//合适
									//这位移可以作为订货依据
									modPHScal::gbCSPRneedSpecialDesign = false;
									EDIBgbl::dbPRJ.Execute(_T("UPDATE tmpCSLen SET SpecialDesign=\'\'"));
									m_bFoundCSPR = true;
								}
							}
						}
					}
				}
			}
			if( ! m_bFoundCSPR )
				//串联数达到最大值，还没有合适的恒力弹簧
				throw GetResStr(IDS_NotFoundConstantSPRInTBNHDproperty);
			
			else
			{
				//找到合适的恒吊
				//按载荷容量就近原则选择恒吊
				//记录当前载荷容量
				rs.GetFieldValue(_T("Capacity"),vTmp1);
				C1 = vtof(vTmp1);
				//记录前一个载荷容量
				rs.MovePrev();
				rs.GetFieldValue(_T("Capacity"),vTmp1);
				C2 = vtof(vTmp1);
				if( fabs(C1 - tmpCapacity) >= fabs(C2 - tmpCapacity) );
				//载荷容量更接近C2
				else
				{
					//载荷容量更接近C1
					//回到容量接近的记录
					rs.MoveNext();
				}
				//给恒吊赋值
				if(iSEQofSPR<modPHScal::SprInfoIndex)
				{
					modPHScal::sSprInfo[iSEQofSPR].SerialNum = 1;
					rs.GetFieldValue(_T("dh"),vTmp1);
					modPHScal::sSprInfo[iSEQofSPR].DH =vtoi(vTmp1);
					modPHScal::sSprInfo[iSEQofSPR].haz = fabs(modPHScal::yr) / modPHScal::giWholeSprNum * modPHScal::sSprInfo[iSEQofSPR].SerialNum;
					modPHScal::sSprInfo[iSEQofSPR].HeatDisp = modPHScal::sSprInfo[iSEQofSPR].haz;
					modPHScal::sSprInfo[iSEQofSPR].CheckDisp = (modPHScal::gbMinMaxDispByActualDisp?modPHScal::sSprInfo[iSEQofSPR].HeatDisp:tmpDist);
					modPHScal::sSprInfo[iSEQofSPR].SumDisp = tmpDist;
					modPHScal::sSprInfo[iSEQofSPR].SumDistID = tmpDistID;
				}
				//选择的工作荷载，单位为kgf
				rs.GetFieldValue(_T("Capacity"),vTmp1);
				tmpSelOpLoad = vtof(vTmp1)/ tmpDist * 1000 / EDIBgbl::kgf2N;
			}
	   }
	   
	   SQLx = _T("SELECT * FROM [") + modPHScal::tbnHDCrude + _T("] WHERE ") + tmpSQL ;
	   if(iSEQofSPR < modPHScal::SprInfoIndex)
		   sTmp.Format(_T(" dh=%d"),modPHScal::sSprInfo[iSEQofSPR].DH);
	   else sTmp = _T(" ");
	   SQLx+=sTmp;
	   if(iSEQofSPR < modPHScal::SprInfoIndex)
	   {
		   sTmp.Format(_T(" AND fBmin<=%g AND fBmax>=%g"),modPHScal::sSprInfo[iSEQofSPR].CheckDisp,modPHScal::sSprInfo[iSEQofSPR].CheckDisp);
	   }
	   else
	   {
		   sTmp=_T(" ");
	   }
	   SQLx+=(modPHScal::gbCSPRneedSpecialDesign ? _T(" ") : sTmp) + _T(" AND trim(CustomID)=\'") + Ptype[i] + _T("\' ORDER BY dh,Weight");
	   break;
	case iSAh:
		SQLx.Format(_T("SELECT * FROM  %s  WHERE  + %s %s >=  %g  AND trim(CustomID)=\'%s\' ORDER BY GDW1,%s"),
			modPHScal::tbnSA,
			tmpSQL,
			SAfDPmax,
			tmpSelPJG / iNumPart * (vtob(rsza->GetCollect(_T("ifLongVertPipe"))) ? iNumPart : 1),
			Ptype[i],
			SAfDPmax);
		break;
	case iSACantilever:
	case iSALbrace:
		SQLx.Format(_T("SELECT * FROM  %s  WHERE GDW1>= %g AND %s >=%g AND trim(CustomID)=\'%s\' %s ORDER BY GDW1,%s"),
			modPHScal::tbnSA,
			modPHScal::gdw,
			SAfDPmax ,
			tmpSelPJG / iNumPart * (vtob(rsza->GetCollect(_T("ifLongVertPipe"))) ? iNumPart : 1),
			Ptype[i],
			strSelNumCS,
			SAfDPmax);
		break;
	case iG51:
	case iG56:
	case iG57:
		SQLx.Format(_T("SELECT * FROM %s WHERE GDW1>= %g AND size2>=%g AND %s >=%g AND trim(CustomID)=\'%s\' %s ORDER BY GDW1,size2,%s"),
			modPHScal::tbnSA,
			modPHScal::gdw,
			modPHScal::gdw/modPHScal::WidthB,
			SAfDPmax,
			tmpSelPJG / iNumPart * (vtob(rsza->GetCollect(_T("ifLongVertPipe"))) ? iNumPart : 1),
			Ptype[i],
			strSelNumCS,
			SAfDPmax);
		break;
	case iSAbeam:
	case iG52_55:
		//2006.06.09 by pfg,ligb
		//简支梁G31~G39和G52~G55排序方式应为ORDER BY L1,GDW1,%s或者ORDER BY DH
		//其它根部排序方式应为ORDER BY GDW1,L1,%s或者ORDER BY DH
		
		//在2006.06.09彭范庚发现根部选型有部分型号偏大以前，
		//根部选型排序方式为ORDER BY GDW1,L1,%s
		if( iNumPart == 2 )
		{
			//两个根部梁承担所有荷载
			SQLx.Format(_T("SELECT * FROM %s WHERE GDW1 >= IIF(%g > L1/2 , L1-%g , %g) AND L1>= %g AND %s >=%g AND trim(CustomID)=\'%s\' %s ORDER BY DH"),
				modPHScal::tbnSA,
				modPHScal::gdw+(modPHScal::glNumSA==1 ? modPHScal::DblHangerRodDist : 0)/2,
				modPHScal::gdw+(modPHScal::glNumSA==1 ? modPHScal::DblHangerRodDist : 0) / 2,
				modPHScal::gdw+(modPHScal::glNumSA==1 ? modPHScal::DblHangerRodDist : 0) / 2 ,
				modPHScal::Lspan,
				SAfDPmax,
				tmpSelPJG / iNumPart * (vtob(rsza->GetCollect(_T("ifLongVertPipe"))) ? iNumPart : 1 ),
				Ptype[i],
				strSelNumCS);
		}
		else
		{
			//一个根部梁承担所有荷载
			//假定荷载作用在一点，这样选择出来的根部偏于安全。
			SQLx.Format(_T("SELECT * FROM %s WHERE GDW1>=IIF(%g > L1/2 , L1-%g , %g) AND L1>=%g AND %s>=%g AND trim(CustomID)=\'%s\' %s ORDER BY DH"),
				modPHScal::tbnSA,
				modPHScal::gdw+(modPHScal::glNumSA==1 ? modPHScal::DblHangerRodDist : 0)/2,
				modPHScal::gdw+(modPHScal::glNumSA==1 ? modPHScal::DblHangerRodDist : 0)/2,
				modPHScal::gdw+(modPHScal::glNumSA==1 ? modPHScal::DblHangerRodDist : 0)/2,
				modPHScal::Lspan,
				SAfDPmax,
				tmpSelPJG / iNumPart * (vtob(rsza->GetCollect(_T("ifLongVertPipe"))) ? iNumPart : 1),
				Ptype[i],
				strSelNumCS);
		}
		break;
	case iSALbraceFixG47:
	case iSALbraceFixG48:               
		if((Ptype[i]!=_T("SJ8")))
		{
			//公式中变量的单位:mvBWidth,cm;PAfixH,mm->cm; modPHScal::gdw,mm->cm; 所有力矩,kgf.m->kgf.cm; 所有力,kgf->kgf.
			//强度计算:西北院固定支架根部G47(ClassID=iSALbraceFixG47=4),G48(ClassID=iSALbraceFixG48=10)
			if( modPHScal::glClassID == iSALbraceFixG47 )
			{
				mvBWidth = 30;
			}
			else 
			{
				vTmp=rsza->GetCollect(_T("A01"));
				mvBWidth = vtof(vTmp)/ 10;
			}
			bool bLongVertPipe;//是长垂直管道上的刚性吊架
			bLongVertPipe=vtob(rsza->GetCollect(_T("ifLongVertPipe")));
			//开始循环查找合适的固定支架G47、G48
			//查槽钢特性表，找F1ToWx1
			SQLx = _T("SELECT * FROM SSteelPropertyCS WHERE F1toWx1 is not null ORDER BY ID");
			if(rsTmp.IsOpen())
				rsTmp.Close();
			rsTmp.m_pDatabase=&modPHScal::dbZDJcrude;	
			rsTmp.Open(dbOpenSnapshot,SQLx);
			//按照西北电力设计院支吊架手册1983年版117页解释,把"GDW1>="改为"GDW1=". lgb and pfg20050927
			SQLx = CString(_T("SELECT * FROM ")) + modPHScal::tbnSA + _T(" WHERE GDW1= ") + ftos(int((modPHScal::gdw+99)/100)*100) + _T(" AND trim(CustomID)=\'") + Ptype[i] + _T("\' ORDER BY GDW1,PmaxSF");
			if(rsX.IsOpen())
				rsX.Close();
			rsX.Open(dbOpenSnapshot,SQLx);
			if( rsX.IsEOF() && rsX.IsBOF() )
				;
			else
			{
				while (!rsX.IsEOF())
				{
					rsX.GetFieldValue(_T("P1"),vTmp1);
					rsX.GetFieldValue(_T("PmaxSF"),vTmp3);
					sngSAfixP=vtof(vTmp3);
					if(rsTmp.FindFirst(_T("BH=\'")+vtos(vTmp1)+_T("\'")))
					{
						rsTmp.GetFieldValue(_T("F1ToWx1"),vTmp2);
						F1ToWx1=vtof(vTmp2);
						sngSAfixPl = 1.3 * modPHScal::pylc / iNumPart * (bLongVertPipe ? iNumPart : 1) - 1 / mvBWidth * (2.6 * modPHScal::PAfixH / 10 - 1.25 * modPHScal::gdw / 10) * modPHScal::pxlc + (1.95 * modPHScal::PAfixH / modPHScal::gdw + 0.5 + 1 / 4 * F1ToWx1 * modPHScal::PAfixH / 10) * modPHScal::pzlc + (1.95 * modPHScal::PAfixH / modPHScal::gdw + 1 / 4 * F1ToWx1) * modPHScal::mxlc * 100 + modPHScal::mylc * 100 / mvBWidth + 2.6 / mvBWidth * modPHScal::mzlc * 100;
						sngSAfixPr = 1.3 * modPHScal::pyrc / iNumPart * (bLongVertPipe ? iNumPart : 1) - 1 / mvBWidth * (2.6 * modPHScal::PAfixH / 10 - 1.25 * modPHScal::gdw / 10) * modPHScal::pxrc + (1.95 * modPHScal::PAfixH / modPHScal::gdw + 0.5 + 1 / 4 * F1ToWx1 * modPHScal::PAfixH / 10) * modPHScal::pzrc + (1.95 * modPHScal::PAfixH / modPHScal::gdw + 1 / 4 * F1ToWx1) * modPHScal::mxrc * 100 + modPHScal::myrc * 100 / mvBWidth + 2.6 / mvBWidth * modPHScal::mzrc * 100;
						//按照西北院支吊架手册1983年版116页解释，各力矩、力的方向可任意。
						if(1)
						{
							
							sngSAfixPl = 1.3 * fabs(modPHScal::pylc) / iNumPart * (bLongVertPipe ? iNumPart : 1) - 1 / mvBWidth * (2.6 * modPHScal::PAfixH / 10 - 1.25 * modPHScal::gdw / 10) * fabs(modPHScal::pxlc) + (1.95 * modPHScal::PAfixH / modPHScal::gdw + 0.5 + 1 / 4 * F1ToWx1 * modPHScal::PAfixH / 10) * fabs(modPHScal::pzlc) + (1.95/ (modPHScal::gdw/10) + 1 / 4 * F1ToWx1) * fabs(modPHScal::mxlc) * 100 + fabs(modPHScal::mylc) * 100 / mvBWidth + 2.6 / mvBWidth * fabs(modPHScal::mzlc) * 100;
							sngSAfixPr = 1.3 * fabs(modPHScal::pyrc) / iNumPart * (bLongVertPipe ? iNumPart : 1) - 1 / mvBWidth * (2.6 * modPHScal::PAfixH / 10 - 1.25 * modPHScal::gdw / 10) * fabs(modPHScal::pxrc) + (1.95 * modPHScal::PAfixH / modPHScal::gdw + 0.5 + 1 / 4 * F1ToWx1 * modPHScal::PAfixH / 10) * fabs(modPHScal::pzrc) + (1.95/ (modPHScal::gdw/10) + 1 / 4 * F1ToWx1) * fabs(modPHScal::mxrc) * 100 + fabs(modPHScal::myrc) * 100 / mvBWidth + 2.6 / mvBWidth * fabs(modPHScal::mzrc) * 100;							
						}
						//如果当量力<0,取其绝对值作为当量力 by ligb on 2004.09.11
						if(sngSAfixPl<0) sngSAfixPl=fabs(sngSAfixPl);
						if(sngSAfixPr<0) sngSAfixPr=fabs(sngSAfixPr);

						if( (sngSAfixP > sngSAfixPl) && (sngSAfixP > sngSAfixPr) )
						{
							//冷态、热态当量力均满足了
							SQLx = CString(_T("SELECT * FROM ")) + modPHScal::tbnSA + _T(" WHERE GDW1>= ") + ftos(modPHScal::gdw) + _T(" AND PmaxSF>=")
								+ ftos(sngSAfixP)+ _T(" AND trim(CustomID)=\'") + Ptype[i] + _T("\' ORDER BY GDW1,PmaxSF");
							break;
						}
						else
						{
							//继续找
							rsX.MoveNext();
						}
					}
				}
				if(rsX.IsEOF())
				{
					//按照西北电力设计院支吊架手册1983年版117页解释,把"GDW1>="改为"GDW1=".lgb and pfg20050927
					SQLx = CString(_T("SELECT * FROM ")) + modPHScal::tbnSA + _T(" WHERE GDW1= ") + ftos(int((modPHScal::gdw+99)/100)*100) + _T(" AND PmaxSF>=")
						+ ftos((sngSAfixPl>sngSAfixPr)?sngSAfixPl:sngSAfixPr)+ _T(" AND trim(CustomID)=\'") + Ptype[i] + _T("\' ORDER BY GDW1,PmaxSF");
				}
			}
		}
		else
		{
			//开始强度计算:华东院固定支架根部双臂三角架SJ8(ClassID=iSALbraceFixG47=4)
			
			//管部以外的零件的材料，较细致的做法是按照材料规范选择表按不同类别零件分别选择；此处简单地取默认值；
			SQLx.Format(_T("SELECT * FROM SpecificationOfMaterial WHERE ClassIndex=%d AND ID=\'default\' AND tmin<=%g AND %g<tmax ORDER BY tmin,tmax,SEQ"),modPHScal::giClassIndex,modPHScal::gnConnectTJ,modPHScal::gnConnectTJ);
			if(rs3.IsOpen())
				rs3.Close();
			rs3.Open(dbOpenSnapshot,SQLx);
			if(rs3.IsEOF() && rs3.IsBOF())
			{
				//在%s库%s材料选择规范表没有%s字段值为默认值%s的记录(非管部零件材料选择规则)
				sTmp.Format(IDS_NoDefaultInCustomIDInSpecificationOfMaterial,rs3.m_pDatabase->GetName(),_T("SpecificationOfMaterial"),_T("ID"),_T("default"));
				throw sTmp;
			}
			else
			{
				rs3.GetFieldValue(_T("Material"),vTmp);
				m_strMaterial=vtos(vTmp);
			}
			
			//查找应力
			float Sigmat=0.0,Sigmat100=0.0,Sigma0=0.0;
			modPHScal::StressOfMaterial(m_strMaterial,modPHScal::t0,Sigmat);
			modPHScal::StressOfMaterial(m_strMaterial,100,Sigmat100);
			modPHScal::StressOfMaterial(m_strMaterial,20,Sigma0);
			//打开固定支架计算公式表，备计算
			SQLx = _T("SELECT * FROM CalFormulaOfFixPASA WHERE CustomID=\'") + Ptype[i] + _T("\' ORDER BY CustomID,SEQ");
			if(rsCal.IsOpen())
				rsCal.Close();
			rsCal.m_pDatabase=&EDIBgbl::dbSACal;	//20071103 "dbSORT" 改为 "dbSACal"
			rsCal.Open(dbOpenSnapshot,SQLx);
			if(rsCal.IsEOF() && rsCal.IsBOF())
			{
				//在数据库%s中的根部强度计算公式表%s没有任何记录。
				sTmp.Format(IDS_NullTableCalFormulaOfFixPASA,EDIBgbl::dbSACal.GetName(),_T("CalFormulaOfFixPASA"));//20071103 "dbSORT" 改为 "dbSACal"
				throw sTmp;
			}
			//打开根部强度计算公式表，备计算
			SQLx = _T("SELECT * FROM tmpCalFixPhs");
			if(rsTmp1.IsOpen())
				rsTmp1.Close();
			rsTmp1.m_pDatabase=&EDIBgbl::dbPRJ;	
			//rsTmp1需要修改功能
			rsTmp1.Open(dbOpenDynaset,SQLx);
			//打开槽钢特性表，备查
			SQLx = _T("SELECT * FROM SSteelPropertyCS ORDER BY ID");
			if(rsTmp.IsOpen())
				rsTmp.Close();
			rsTmp.m_pDatabase=&modPHScal::dbZDJcrude;	
			rsTmp.Open(dbOpenSnapshot,SQLx);
			//打开角钢特性表，备查
			SQLx = _T("SELECT * FROM SSteelPropertyLS ORDER BY ID");
			if(rs1.IsOpen())
				rs1.Close();
			rs1.m_pDatabase=&modPHScal::dbZDJcrude;	
			rs1.Open(dbOpenSnapshot,SQLx);
			//取根部表中的满足定位条件的所有记录,按PmaxSF排序，逐个查找
			SQLx = CString(_T("SELECT * FROM ")) + modPHScal::tbnSA + _T(" WHERE GDW1>= ") + ftos(modPHScal::gdw) + _T(" AND trim(CustomID)=\'") + Ptype[i] + _T("\' ORDER BY GDW1,PmaxSF");
			if(rsX.IsOpen())
				rsX.Close();
			rsX.Open(dbOpenSnapshot,SQLx);
			if( rsX.IsEOF() && rsX.IsBOF())
				;
			else
			{				
				CString strP1,strP2,strP3;
				int n,dh;
				float bb,PL3,K;
				int i4,i5;
				float mfpx,mfpy,mfpz;//保存冷或热态力，kgf
				float mfmx,mfmy,mfmz;//保存冷或热态力矩，kgf.m
				CString F1sa,Wyh1sa,Wzh1sa;
				CString F2sa,Wyh2sa,Wzh2sa;
				CString SIGMA1sa,SIGMA1saMax,TAO1sa,TAO1saMax,TAOx1sa,TAOy1sa,TAOz1sa,lamda1;
				CString SIGMA2sa,SIGMA2saMax,TAO2sa,TAO2saMax,TAOx2sa,TAOy2sa,TAOz2sa,lamda2;
				float mfSIGMA1sa,mfSIGMA1saMax,mfTAO1sa,mfTAO1saMax,mfLamda1,mfFi1;
				float mfSIGMA2sa,mfSIGMA2saMax,mfTAO2sa,mfTAO2saMax,mfLamda2,mfFi2;
				CDaoRecordset rs2;
				
				//modPHScal::PAfixH=320.0;//本句可测试华东院例题
				
				rsTmp1.Edit();
				//rsTmp1.SetFieldValue(_T("CustomIDsa"),COleVariant(Ptype(i)));
				rsTmp1.SetFieldValue(_T("CustomIDsa"),COleVariant(sPartID));
				rsTmp1.SetFieldValue(_T("H"),COleVariant(modPHScal::PAfixH));
				rsTmp1.SetFieldValue(_T("gdw2"),COleVariant(modPHScal::gdw));
				rsTmp1.SetFieldValue(_T("tj"),COleVariant(modPHScal::t0));
				//许用应力单位kgf/cm2->kgf/mm2
				rsTmp1.SetFieldValue(_T("SIGMAjt"),COleVariant(Sigmat/100.0));
				rsTmp1.SetFieldValue(_T("SIGMAj20"),COleVariant(Sigma0/100.0));
				rsTmp1.SetFieldValue(_T("SIGMAj100"),COleVariant(Sigmat100/100.0));
				rsTmp1.Update();
				//以上一句加快更新，尽量防止锁定。
				
				//获得构件3
				rsX.GetFieldValue(_T("P3"),vTmp);
				strP3=vtos(vTmp);	
				rsTmp1.Edit();
				rsTmp1.SetFieldValue(_T("P3"),vTmp);
				//此处只适应中国标准，国际化时要修改代码
				n=strP3.Find((_T("x")));
				//构件3腿宽bb,unit:mm
				bb=_tcstod(strP3.Mid(1,n-1),NULL);
				rsTmp1.SetFieldValue(_T("bb"),COleVariant(bb));
				//获得主型钢P1
				rsX.GetFieldValue(_T("P1"),vTmp);
				strP1=vtos(vTmp);
				rsTmp1.SetFieldValue(_T("P1"),vTmp);
				//获得构件2型钢P2
				rsX.GetFieldValue(_T("P2"),vTmp);
				strP2=vtos(vTmp);
				rsTmp1.SetFieldValue(_T("P2"),vTmp);
				//双槽钢外缘最大距离PL3,mm
				rsX.GetFieldValue(_T("PL3"),vTmp);
				PL3=vtof(vTmp);
				//rsX中的PL3是CHAR(20),而rsTmp1中的PL3是REAL,可见变体能够自动转换。
				rsTmp1.SetFieldValue(_T("PL3"),vTmp);
				//槽钢焊缝高度K,mm
				rsX.GetFieldValue(_T("size2"),vTmp);
				K=vtof(vTmp);
				rsTmp1.SetFieldValue(_T("K"),vTmp);
				rsTmp1.Update();
				//序号DH,mm
				rsX.GetFieldValue(_T("dh"),vTmp);
				dh=vtoi(vTmp);				
				
				//先计算热态
				
				bool bEOF=false;
				bool bErr=false;//任意一个条件不满足，置为true
				do
				{
					if(!rsTmp.FindFirst(_T("BH=\'")+strP1+_T("\'")))
					{
						//在槽钢特性表%s中没找到槽钢型号%s
						sTmp.Format(GetResStr(IDS_NotFoundThisCSNo),_T("SSteelPropertyCS"),strP1);
						throw sTmp;
					}
					//构件1槽钢惯性矩Ix,cm4->mm4
					rsTmp.GetFieldValue(_T("Ix"),vTmp);
					float J1y=vtof(vTmp)*10000.0;
					rsTmp1.Edit();
					rsTmp1.SetFieldValue(_T("J1y"),COleVariant(J1y));
					//构件1槽钢惯性矩Iy,cm4->mm4
					rsTmp.GetFieldValue(_T("Iy"),vTmp);
					float J1z=vtof(vTmp)*10000.0;
					rsTmp1.SetFieldValue(_T("J1z"),COleVariant(J1z));
					//构件1槽钢最小惯性矩,cm4->mm4
					rsTmp1.SetFieldValue(_T("Imin1"),COleVariant(J1z>J1y?J1y:J1z));
					//构件1槽钢抗弯矩Wx,cm3->mm3
					rsTmp.GetFieldValue(_T("Wx"),vTmp);
					float W1y=vtof(vTmp)*1000.0;
					rsTmp1.SetFieldValue(_T("W1y"),COleVariant(W1y));
					//构件1槽钢抗弯矩Wy,cm3->mm3
					rsTmp.GetFieldValue(_T("Wy"),vTmp);
					float W1z=vtof(vTmp)*1000.0;
					rsTmp1.SetFieldValue(_T("W1z"),COleVariant(W1z));
					//构件1槽钢截面积S,cm2->mm2
					rsTmp.GetFieldValue(_T("S"),vTmp);
					float F1=vtof(vTmp)*100.0;
					rsTmp1.SetFieldValue(_T("F1"),COleVariant(F1));
					//构件1槽钢高度h,mm
					rsTmp.GetFieldValue(_T("h"),vTmp);
					float h1=vtof(vTmp);
					rsTmp1.SetFieldValue(_T("h1"),vTmp);
					//构件1槽钢腿宽b,mm
					rsTmp.GetFieldValue(_T("b"),vTmp);
					float b1=vtof(vTmp);
					rsTmp1.SetFieldValue(_T("b1"),vTmp);
					rsTmp1.Update();
					
					if(!rsTmp.FindFirst(_T("BH=\'")+strP2+_T("\'")))
					{
						//在槽钢特性表%s中没找到槽钢型号%s
						sTmp.Format(GetResStr(IDS_NotFoundThisCSNo),_T("SSteelPropertyCS"),strP2);
						throw sTmp;
					}
					//构件2槽钢惯性矩Ix,cm4->mm4
					rsTmp.GetFieldValue(_T("Ix"),vTmp);
					float J2y=vtof(vTmp)*10000.0;
					rsTmp1.Edit();
					rsTmp1.SetFieldValue(_T("J2y"),COleVariant(J2y));
					//构件2槽钢惯性矩Iy,cm4->mm4
					rsTmp.GetFieldValue(_T("Iy"),vTmp);
					float J2z=vtof(vTmp)*10000.0;
					rsTmp1.SetFieldValue(_T("J2z"),COleVariant(J2z));
					//构件1槽钢最小惯性矩,cm4->mm4
					rsTmp1.SetFieldValue(_T("Imin2"),COleVariant(J2z>J2y?J2y:J2z));
					//构件2槽钢抗弯矩Wx,cm3->mm3
					rsTmp.GetFieldValue(_T("Wx"),vTmp);
					float W2y=vtof(vTmp)*1000.0;
					rsTmp1.SetFieldValue(_T("W2y"),COleVariant(W2y));
					//构件2槽钢抗弯矩Wy,cm3->mm3
					rsTmp.GetFieldValue(_T("Wy"),vTmp);
					float W2z=vtof(vTmp)*1000.0;
					rsTmp1.SetFieldValue(_T("W2z"),COleVariant(W2z));
					//构件2槽钢截面积S,cm2->mm2
					rsTmp.GetFieldValue(_T("S"),vTmp);
					float F2=vtof(vTmp)*100.0;
					rsTmp1.SetFieldValue(_T("F2"),COleVariant(F2));
					//构件2槽钢高度h,mm
					rsTmp.GetFieldValue(_T("h"),vTmp);
					float h2=vtof(vTmp);
					rsTmp1.SetFieldValue(_T("h2"),COleVariant(h2));
					//构件2槽钢腿宽b,mm
					rsTmp.GetFieldValue(_T("b"),vTmp);
					float b2=vtof(vTmp);
					rsTmp1.SetFieldValue(_T("b2"),COleVariant(b2));
					
					//双槽钢净空距离g,mm
					float g=PL3-2*h1;
					rsTmp1.SetFieldValue(_T("g"),COleVariant(g));
					//双槽钢中心距B1,mm
					float B1=PL3-h1;
					//因为rsTmp1中存在字段b1,而Jet数据库字段名是不区分大小写的，故换B1->Bx,表中相应的公式B1已经替换。
					rsTmp1.SetFieldValue(_T("Bx"),COleVariant(B1));
					//更新数据准备计算
					rsTmp1.Update();					
					
					//热态/冷态计算
					for(i4=1;i4<=2;i4++)
					{
						if(i4==2)
						{
							mfpx=modPHScal::pxrc;
							mfpy=modPHScal::pyrc;
							mfpz=modPHScal::pzrc;
							mfmx=modPHScal::mxrc;
							mfmy=modPHScal::myrc;
							mfmz=modPHScal::mzrc;
						}
						else
						{
							mfpx=modPHScal::pxlc;
							mfpy=modPHScal::pylc;
							mfpz=modPHScal::pzlc;
							mfmx=modPHScal::mxlc;
							mfmy=modPHScal::mylc;
							mfmz=modPHScal::mzlc;
						}
						
						rsTmp1.Edit();
						rsTmp1.SetFieldValue(_T("Px"),COleVariant(mfpx));
						rsTmp1.SetFieldValue(_T("Py"),COleVariant(mfpy));
						rsTmp1.SetFieldValue(_T("Pz"),COleVariant(mfpz));
						rsTmp1.SetFieldValue(_T("Mx"),COleVariant(mfmx));
						rsTmp1.SetFieldValue(_T("My"),COleVariant(mfmy));
						rsTmp1.SetFieldValue(_T("Mz"),COleVariant(mfmz));
						rsTmp1.Update();
						
						//构件1和构件2焊缝的抗弯矩,mm3
						//float Wyh1=0.233*K*(2*h1+g)*(2*h1+g)+1.4*K*(bb+b1)*(h1+0.5*g);
						//float Wzh1=0.233*K*((b1+bb)*(b1+bb)+b1*b1)+0.35*K*(2*h1+g)*(bb+b1);
						//float Wyh2=(1.4*K*h2*h2*h2+4.2*K*h2*B1*B1)/3/(B1+h2)+0.7*K*b2*2*(B1*B1+h2*h2)/(B1+h2);
						//float Wzh2=K*b2*(0.467*b2+1.4*h2);
						
						if(!rsCal.FindFirst(_T("SEQ=1")))
						{
							//根部%s第%s种条件的计算公式没找到。
							sTmp.Format(IDS_SACalFormulaXNotFound,Ptype[i],_T("1"));
							throw sTmp;
						}
						else
						{
							if(i4==2)
							{//几何数据，只计算一次，加快速度
								//rsCal.GetFieldValue(_T("F1saFormula"),vTmp);
								//F1sa=vtos(vTmp);
								//rsCal.GetFieldValue(_T("F2saFormula"),vTmp);
								//F2sa=vtos(vTmp);
								rsCal.GetFieldValue(_T("Wyh1saFormula"),vTmp);
								Wyh1sa=vtos(vTmp);
								rsCal.GetFieldValue(_T("Wzh1saFormula"),vTmp);
								Wzh1sa=vtos(vTmp);
								rsCal.GetFieldValue(_T("Wyh2saFormula"),vTmp);
								Wyh2sa=vtos(vTmp);
								rsCal.GetFieldValue(_T("Wzh2saFormula"),vTmp);
								Wzh2sa=vtos(vTmp);
								
								//应力判据可先获得，方便以后计算
								rsCal.GetFieldValue(_T("SIGMA1saMaxFormula"),vTmp);
								SIGMA1saMax=vtos(vTmp);
								rsCal.GetFieldValue(_T("TAO1saMaxFormula"),vTmp);
								TAO1saMax=vtos(vTmp);
								rsCal.GetFieldValue(_T("SIGMA2saMaxFormula"),vTmp);
								SIGMA2saMax=vtos(vTmp);
								rsCal.GetFieldValue(_T("TAO2saMaxFormula"),vTmp);
								TAO2saMax=vtos(vTmp);
								rsCal.GetFieldValue(_T("lamda1Formula"),vTmp);
								lamda1=vtos(vTmp);
								rsCal.GetFieldValue(_T("lamda2Formula"),vTmp);
								lamda2=vtos(vTmp);
								
								//开始计算几何、截面特性
								SQLx=_T("SELECT ")+ Wyh1sa+_T(" AS Wyh1sac, ")+ Wyh2sa+_T(" AS Wyh2sac, ") + Wzh1sa+_T(" AS Wzh1sac, ")+ Wzh2sa+_T(" AS Wzh2sac, ")+SIGMA1saMax+_T(" AS SIGMA1saMaxc, ")+TAO1saMax+_T(" AS TAO1saMaxc, ")+SIGMA2saMax+_T(" AS SIGMA2saMaxc, ")+TAO2saMax+_T(" AS TAO2saMaxc, ")+lamda1+_T(" AS lamda1c, ")+lamda2+_T(" AS lamda2c FROM tmpCalFixPhs");
								if(rsTmp2.IsOpen())
									rsTmp2.Close();
								rsTmp2.m_pDatabase=&EDIBgbl::dbPRJ;
								rsTmp2.Open(dbOpenDynaset,SQLx);
								
								rsTmp1.Edit();
								rsTmp2.GetFieldValue(_T("Wyh1sac"),vTmp);
								rsTmp1.SetFieldValue(_T("Wyh1"),vTmp);
								
								rsTmp2.GetFieldValue(_T("Wzh1sac"),vTmp);
								rsTmp1.SetFieldValue(_T("Wzh1"),vTmp);
								
								rsTmp2.GetFieldValue(_T("Wyh2sac"),vTmp);
								rsTmp1.SetFieldValue(_T("Wyh2"),vTmp);
								
								rsTmp2.GetFieldValue(_T("Wzh2sac"),vTmp);
								rsTmp1.SetFieldValue(_T("Wzh2"),vTmp);							
								
								rsTmp2.GetFieldValue(_T("TAO1saMaxc"),vTmp);
								rsTmp1.SetFieldValue(_T("TAO1saMax"),vTmp);
								mfTAO1saMax=vtof(vTmp);
								rsTmp2.GetFieldValue(_T("SIGMA1saMaxc"),vTmp);
								rsTmp1.SetFieldValue(_T("SIGMA1saMax"),vTmp);
								mfSIGMA1saMax=vtof(vTmp);
								
								rsTmp2.GetFieldValue(_T("TAO2saMaxc"),vTmp);
								rsTmp1.SetFieldValue(_T("TAO2saMax"),vTmp);
								mfTAO2saMax=vtof(vTmp);
								rsTmp2.GetFieldValue(_T("SIGMA2saMaxc"),vTmp);
								rsTmp1.SetFieldValue(_T("SIGMA2saMax"),vTmp);
								mfSIGMA2saMax=vtof(vTmp);
								
								rsTmp2.GetFieldValue(_T("lamda1c"),vTmp);
								rsTmp1.SetFieldValue(_T("lamda1"),vTmp);
								mfLamda1=vtof(vTmp);
								rsTmp2.GetFieldValue(_T("lamda2c"),vTmp);
								rsTmp1.SetFieldValue(_T("lamda2"),vTmp);
								mfLamda2=vtof(vTmp);
								
								rsTmp1.Update();
								float mf1=0.0,mf2=0,mfl1=0,mfl2=0;
								SQLx=_T("SELECT Lamda,Fi FROM SteadyDecreaseCoef WHERE Material=\'") + m_strMaterial + _T("\'");
								if(rs2.IsOpen())
									rs2.Close();
								rs2.m_pDatabase=&EDIBgbl::dbSACal;//20071103 "dbSORT" 改为 "dbSACal"
								rs2.Open(dbOpenSnapshot,SQLx);
								if(rs2.IsEOF() && rs2.IsBOF())
								{
									//在%s库稳定折减系数表%s中没有%s这种材料。
									sTmp.Format(IDS_NoMaterialInSteadyDecreaseCoef,rs2.m_pDatabase->GetName(),_T("SteadyDecreaseCoef"),m_strMaterial);
									throw sTmp;
								}
								else
								{
									if(!rs2.FindFirst(_T("Lamda>")+ftos(mfLamda1)))
									{
										//计算的细长比%s超过%s库%s表中最大值。
										sTmp.Format(IDS_LamdaOverMaxValue,ftos(mfLamda1),modPHScal::dbZDJcrude.GetName(),_T("SteadyDecreaseCoef"));
										throw sTmp;
									}
									else
									{
										rs2.GetFieldValue(_T("Lamda"),vTmp);
										mfl2=vtof(vTmp);
										rs2.GetFieldValue(_T("Fi"),vTmp);
										mf2=vtof(vTmp);
										rs2.MovePrev();
										if(rs2.IsBOF())
										{
											//一般不可能,因为表中数值从0开始
										}
										else
										{
											//插值求Fi1
											rs2.GetFieldValue(_T("Lamda"),vTmp);
											mfl1=vtof(vTmp);
											rs2.GetFieldValue(_T("Fi"),vTmp);
											mf1=vtof(vTmp);
											mfFi1=(mf2-mf1)/(mfl2-mfl1)*(mfLamda1-mfl1)+mf1;
											rsTmp1.Edit();
											rsTmp1.SetFieldValue(_T("Fi1"),COleVariant(mfFi1));																					
											rsTmp1.Update();
										}
									}
									//求构件2稳定折减系数Fi2
									if(!rs2.FindFirst(_T("Lamda>")+ftos(mfLamda2)))
									{
										//计算的细长比%s超过%s库%s表中最大值。
										sTmp.Format(IDS_LamdaOverMaxValue,ftos(mfLamda2),modPHScal::dbZDJcrude.GetName(),_T("SteadyDecreaseCoef"));
										throw sTmp;
									}
									else
									{
										rs2.GetFieldValue(_T("Lamda"),vTmp);
										mfl2=vtof(vTmp);
										rs2.GetFieldValue(_T("Fi"),vTmp);
										mf2=vtof(vTmp);
										rs2.MovePrev();
										if(rs2.IsBOF())
										{
											//一般不可能,因为表中数值从0开始
										}
										else
										{
											//插值求Fi1
											rs2.GetFieldValue(_T("Lamda"),vTmp);
											mfl1=vtof(vTmp);
											rs2.GetFieldValue(_T("Fi"),vTmp);
											mf1=vtof(vTmp);
											mfFi2=(mf2-mf1)/(mfl2-mfl1)*(mfLamda2-mfl1)+mf1;
											rsTmp1.Edit();
											rsTmp1.SetFieldValue(_T("Fi2"),COleVariant(mfFi2));																					
											rsTmp1.Update();
										}
									}
								}
							}
						}
						
						//取Py1=1.5*(Py+/-fabs(1000*mx+pz*modPHScal::PAfixH)/B1),以绝对值较大者作为校核工况。unit,mm
						//开始热态计算
						float Py1r1=1.5*(mfpy+fabs(1000*mfmx+mfpz*modPHScal::PAfixH)/B1);
						float Py1r2=1.5*(mfpy-fabs(1000*mfmx+mfpz*modPHScal::PAfixH)/B1);
						float Py1r=((fabs(Py1r1)>fabs(Py1r2))?Py1r1:Py1r2);
						
						rsTmp1.Edit();
						rsTmp1.SetFieldValue(_T("Py1"),COleVariant(Py1r1));
						rsTmp1.SetFieldValue(_T("Py2"),COleVariant(Py1r2));
						rsTmp1.Update();
						
						if( (mfpx>=0 && 0>=Py1r) || (0>=mfpx && mfpx>=1.732*Py1r))
						{
							i5=1;
						}
						else if( (mfpx>=1.732*Py1r) && (Py1r>0))
						{
							i5=2;
						}
						if( (Py1r>=0 && 0>mfpx) || (1.732*Py1r>=mfpx && mfpx>=0))
						{
							i5=3;
						}
						else if( (mfpx<=1.732*Py1r) && (Py1r<0))
						{
							i5=4;
						}
						
						if(!rsCal.FindFirst(_T("SEQ=")+ltos(i5)))
						{
							//根部%s第%s种条件的计算公式没找到。
							sTmp.Format(IDS_SACalFormulaXNotFound,Ptype[i],_T("1"));
							throw sTmp;
						}
						else
						{
							rsCal.GetFieldValue(_T("SIGMA1saFormula"),vTmp);
							SIGMA1sa=vtos(vTmp);
							rsCal.GetFieldValue(_T("TAOx1saFormula"),vTmp);
							TAOx1sa=vtos(vTmp);
							rsCal.GetFieldValue(_T("TAOy1saFormula"),vTmp);
							TAOy1sa=vtos(vTmp);
							rsCal.GetFieldValue(_T("TAOz1saFormula"),vTmp);
							TAOz1sa=vtos(vTmp);
							rsCal.GetFieldValue(_T("TAO1saFormula"),vTmp);
							TAO1sa=vtos(vTmp);
							
							rsCal.GetFieldValue(_T("SIGMA2saFormula"),vTmp);
							SIGMA2sa=vtos(vTmp);
							rsCal.GetFieldValue(_T("TAOx2saFormula"),vTmp);
							TAOx2sa=vtos(vTmp);
							rsCal.GetFieldValue(_T("TAOy2saFormula"),vTmp);
							TAOy2sa=vtos(vTmp);
							rsCal.GetFieldValue(_T("TAOz2saFormula"),vTmp);
							TAOz2sa=vtos(vTmp);
							rsCal.GetFieldValue(_T("TAO2saFormula"),vTmp);
							TAO2sa=vtos(vTmp);
							
							//压杆的长度系数u1,u2:悬臂=2,简支=0.5							
							rsCal.GetFieldValue(_T("u1"),vTmp);
							rsTmp1.Edit();
							rsTmp1.SetFieldValue(_T("u1"),vTmp);
							rsCal.GetFieldValue(_T("u2"),vTmp);
							rsTmp1.SetFieldValue(_T("u2"),vTmp);
							rsTmp1.Update();
							
							//开始计算正应力sigma、剪应力tao
							SQLx=_T("SELECT ")+ TAOx1sa+_T(" AS TAOx1sac, ")+ TAOy1sa+_T(" AS TAOy1sac, ")+ TAOz1sa+_T(" AS TAOz1sac,")
								+ TAOx2sa+_T(" AS TAOx2sac, ")+ TAOy2sa+_T(" AS TAOy2sac, ")+ TAOz2sa+_T(" AS TAOz2sac FROM tmpCalFixPhs");
							if(rsTmp2.IsOpen())
								rsTmp2.Close();
							rsTmp2.m_pDatabase=&EDIBgbl::dbPRJ;
							rsTmp2.Open(dbOpenDynaset,SQLx);
							
							rsTmp1.Edit();
							rsTmp2.GetFieldValue(_T("TAOx1sac"),vTmp);
							rsTmp1.SetFieldValue(_T("TAOx1sa"),vTmp);
							rsTmp2.GetFieldValue(_T("TAOy1sac"),vTmp);
							rsTmp1.SetFieldValue(_T("TAOy1sa"),vTmp);
							rsTmp2.GetFieldValue(_T("TAOz1sac"),vTmp);
							rsTmp1.SetFieldValue(_T("TAOz1sa"),vTmp);
							
							rsTmp2.GetFieldValue(_T("TAOx2sac"),vTmp);
							rsTmp1.SetFieldValue(_T("TAOx2sa"),vTmp);
							rsTmp2.GetFieldValue(_T("TAOy2sac"),vTmp);
							rsTmp1.SetFieldValue(_T("TAOy2sa"),vTmp);
							rsTmp2.GetFieldValue(_T("TAOz2sac"),vTmp);
							rsTmp1.SetFieldValue(_T("TAOz2sa"),vTmp);
							rsTmp1.Update();
							
							SQLx=_T("SELECT ")+ TAO1sa+_T(" AS TAO1sac,")+ TAO2sa+_T(" AS TAO2sac,")+SIGMA1sa+_T(" AS SIGMA1sac,")+SIGMA2sa+_T(" AS SIGMA2sac FROM tmpCalFixPhs");
							if(rsTmp2.IsOpen())
								rsTmp2.Close();
							rsTmp2.m_pDatabase=&EDIBgbl::dbPRJ;
							rsTmp2.Open(dbOpenDynaset,SQLx);
							
							rsTmp2.GetFieldValue(_T("SIGMA1sac"),vTmp);
							mfSIGMA1sa=vtof(vTmp);
							rsTmp1.Edit();
							rsTmp1.SetFieldValue(_T("SIGMA1sa"),vTmp);
							
							rsTmp2.GetFieldValue(_T("SIGMA2sac"),vTmp);
							mfSIGMA2sa=vtof(vTmp);
							rsTmp1.SetFieldValue(_T("SIGMA2sa"),vTmp);
							
							rsTmp2.GetFieldValue(_T("TAO1sac"),vTmp);
							mfTAO1sa=vtof(vTmp);							
							rsTmp1.SetFieldValue(_T("TAO1sa"),vTmp);
							
							rsTmp2.GetFieldValue(_T("TAO2sac"),vTmp);
							mfTAO2sa=vtof(vTmp);
							rsTmp1.SetFieldValue(_T("TAO2sa"),vTmp);
							rsTmp1.Update();
							
							//构件1(主型钢P1)强度校核(槽钢部分)
							//unit,kgf/mm2
							if(mfSIGMA1sa>mfSIGMA1saMax)
							{
								if(bEOF)
								{
									//%s号构件%s槽钢%s热态正应力值%s超过允许值%s
									sTmp.Format(GetResStr(IDS_SSteelStressOver),ltos(modPHScal::zdjh),_T("1"),strP1,GetResStr(IDS_frmtxsr_OptHeatStatus),ftos(mfSIGMA1sa),ftos(mfSIGMA1saMax));
									throw sTmp;
								}
								else
									bErr=true;							
							}
							//构件1(主型钢P1)强度校核(焊缝部分)
							//构件1焊缝合成应力
							if(mfTAO1sa>mfTAO1saMax)
							{
								if(bEOF)
								{
									//%s号构件%s槽钢%s热态正应力值%s超过允许值%s
									sTmp.Format(GetResStr(IDS_SSteelStressOver1),ltos(modPHScal::zdjh),_T("1"),strP1,GetResStr(IDS_frmtxsr_OptHeatStatus),ftos(mfTAO1sa),ftos(mfTAO1saMax));
									throw sTmp;
								}
								else
									bErr=true;							
							}
							
							//构件2(斜撑槽钢P2)强度校核(槽钢部分)
							//unit,kgf/mm2
							if(mfSIGMA2sa>mfSIGMA2saMax)
							{
								if(bEOF)
								{
									//%s号构件%s槽钢%s热态正应力值%s超过允许值%s
									sTmp.Format(GetResStr(IDS_SSteelStressOver),ltos(modPHScal::zdjh),_T("2"),strP2,GetResStr(IDS_frmtxsr_OptHeatStatus),ftos(mfSIGMA2sa),ftos(mfSIGMA2saMax));
									throw sTmp;
								}
								else
									bErr=true;							
							}
							//构件2(斜撑槽钢P2)强度校核(焊缝部分)
							//构件2焊缝合成应力
							if(mfTAO1sa>mfTAO1saMax)
							{
								if(bEOF)
								{
									//%s号构件%s槽钢%s热态正应力值%s超过允许值%s
									sTmp.Format(GetResStr(IDS_SSteelStressOver1),ltos(modPHScal::zdjh),_T("2"),strP2,GetResStr(IDS_frmtxsr_OptHeatStatus),ftos(mfTAO2sa),ftos(mfTAO2saMax));
									throw sTmp;
								}
								else
									bErr=true;							
							}
						}						
					}
					
					if(!bErr /*&& (dh>=36)*/) //注释掉的部分可测试华东院计算手册例题
					{
						//没超限，找到
						break;
					}
					else
					{
						rsX.MoveNext();
						if(rsX.IsEOF())
						{
							//已经到记录尾，重新循环一次，以便报警
							bEOF=true;
							continue;
						}
						else
							//重新开始之前，设置超限标志
							bErr=false;
						
						//获得构件3
						rsX.GetFieldValue(_T("P3"),vTmp);
						strP3=vtos(vTmp);	
						rsTmp1.Edit();
						rsTmp1.SetFieldValue(_T("P3"),vTmp);
						//此处只适应中国标准，国际化时要修改代码
						n=strP3.Find((_T("x")));
						//构件3腿宽bb,unit:mm
						bb=_tcstod(strP3.Mid(1,n-1),NULL);
						rsTmp1.SetFieldValue(_T("bb"),COleVariant(bb));
						//获得主型钢P1
						rsX.GetFieldValue(_T("P1"),vTmp);
						strP1=vtos(vTmp);
						rsTmp1.SetFieldValue(_T("P1"),vTmp);
						//获得构件2型钢P2
						rsX.GetFieldValue(_T("P2"),vTmp);
						strP2=vtos(vTmp);
						rsTmp1.SetFieldValue(_T("P2"),vTmp);
						//双槽钢外缘最大距离PL3,mm
						rsX.GetFieldValue(_T("PL3"),vTmp);
						PL3=vtof(vTmp);
						//rsX中的PL3是CHAR(20),而rsTmp1中的PL3是REAL,可见变体能够自动转换。
						rsTmp1.SetFieldValue(_T("PL3"),vTmp);
						//槽钢焊缝高度K,mm
						rsX.GetFieldValue(_T("size2"),vTmp);
						K=vtof(vTmp);
						rsTmp1.SetFieldValue(_T("K"),vTmp);
						rsTmp1.Update();
						//序号DH,mm
						rsX.GetFieldValue(_T("dh"),vTmp);
						dh=vtoi(vTmp);				
					}
					
					/*
					//构件1和构件2焊缝的抗弯矩,mm3
					float Wyh1=0.233*K*(2*h1+g)*(2*h1+g)+1.4*K*(bb+b1)*(h1+0.5*g);
					float Wzh1=0.233*K*((b1+bb)*(b1+bb)+b1*b1)+0.35*K*(2*h1+g)*(bb+b1);
					float Wyh2=(1.4*K*h2*h2*h2+4.2*K*h2*B1*B1)/3/(B1+h2)+0.7*K*b2*2*(B1*B1+h2*h2)/(B1+h2);
					float Wzh2=K*b2*(0.467*b2+1.4*h2);
					
					  //取Py1=1.5*(Py+/-fabs(1000*mx+pz*modPHScal::PAfixH)/B1),以绝对值较大者作为校核工况。unit,mm
					  //开始热态计算
					  float Py1r1=1.5*(modPHScal::pyrc+fabs(1000*modPHScal::mxrc+modPHScal::pzrc*modPHScal::PAfixH)/B1);
					  float Py1r2=1.5*(modPHScal::pyrc-fabs(1000*modPHScal::mxrc+modPHScal::pzrc*modPHScal::PAfixH)/B1);
					  float Py1r=((fabs(Py1r1)>fabs(Py1r2))?Py1r1:Py1r2);
					  
						if( (modPHScal::pxrc>=0 && 0>=Py1r) || (0>=modPHScal::pxrc && modPHScal::pxrc>=1.732*Py1r))
						{
						//构件1(主型钢P1)强度校核(槽钢部分)
						float sigma1r=(modPHScal::pxrc-1.732*Py1r)/2/F1+fabs(1000*modPHScal::myrc-modPHScal::pzrc*modPHScal::gdw)*(B1+h1)*J1y/4/(J1y+0.25*F1*B1*B1)/(J1y+0.65*J2y)+fabs(1000.0*modPHScal::mzrc-modPHScal::pxrc*modPHScal::PAfixH)*J1z/4/W1z/(J1z+0.65*J2z);//unit,kgf/mm2
						//float sigma1r=(px-1.732*Py1)/2/F1+abs(1000*my-pz*gdw)*(B1+h1)*J1y/4/(J1y+0.25*F1*B1*B1)/(J1y+0.65*J2y)+fabs(1000.0*mz-px*H)*J1z/4/W1z/(J1z+0.65*J2z);//unit,kgf/mm2
						if(sigma1r>Sigma0)
						{
						if(bEOF)
						{
						//%s号构件%s槽钢%s热态正应力值%s超过允许值%s
						sTmp.Format(GetResStr(IDS_SSteelStressOver),ltos(modPHScal::zdjh),_T("1"),strP1,GetResStr(IDS_frmtxsr_OptHeatStatus),ftos(sigma1r),ftos(Sigma0));
						throw sTmp;
						}
						else
						bErr=true;							
						}
						//构件1(主型钢P1)强度校核(焊缝部分)
						//float TAOx1r=(px-1.732*Py1)/1.4/K/(PL3+2*b1+bb)+abs(1000*my-pz*gdw)*J1y/Wyh1/(J1y+0.65*J2y)+(1000*mz-px*H)*J1z/2/Wzh1/(J1z+0.65*J2z);//unit,kgf/mm2
						//float TAOy1r=3*abs(1000.0*mz-px*H)*J1z/2.8/gdw/(J1z+0.65*J2z)/(bb+2*b1)/K;//unit,kgf/mm2
						//float TAOz1r=pz/2.8/K/(PL3-h1);//unit,kgf/mm2
						float TAOx1r=(modPHScal::pxrc-1.732*Py1r)/1.4/K/(PL3+2*b1+bb)+fabs(1000*modPHScal::myrc-modPHScal::pzrc*modPHScal::gdw)*J1y/Wyh1/(J1y+0.65*J2y)+(1000*modPHScal::mzrc-modPHScal::pxrc*modPHScal::PAfixH)*J1z/2/Wzh1/(J1z+0.65*J2z);//unit,kgf/mm2
						float TAOy1r=3*fabs(1000.0*modPHScal::mzrc-modPHScal::pxrc*modPHScal::PAfixH)*J1z/2.8/modPHScal::gdw/(J1z+0.65*J2z)/(bb+2*b1)/K;//unit,kgf/mm2
						float TAOz1r=modPHScal::pzrc/2.8/K/(PL3-h1);//unit,kgf/mm2
						//构件1焊缝合成应力
						float TAO1r=sqrt(TAOx1r*TAOx1r+TAOy1r*TAOy1r+TAOz1r*TAOz1r);//unit,kgf/mm2
						//准则
						if(TAO1r>0.6*Sigma0)
						{							
						if(bEOF)
						{
						//%s号支吊架构件%s槽钢%s焊缝热态剪应力值%s超过允许值%s
						sTmp.Format(GetResStr(IDS_SSteelStressOver1),ltos(modPHScal::zdjh),_T("1"),strP1,GetResStr(IDS_frmtxsr_OptHeatStatus),ftos(TAO1r),ftos(0.6*Sigma0));
						throw sTmp;
						}
						else
						bErr=true;							
						}
						}
						
						  float Py1l1=1.5*(modPHScal::pylc+fabs(1000*modPHScal::mxlc+modPHScal::pzlc*modPHScal::PAfixH)/B1);
						  float Py1l2=1.5*(modPHScal::pylc-fabs(1000*modPHScal::mxlc+modPHScal::pzlc*modPHScal::PAfixH)/B1);
						  float Py1l=((fabs(Py1l1)>fabs(Py1l2))?Py1l1:Py1l2);
						  
							if( (modPHScal::pxlc>=0 && 0>=Py1l) || (0>=modPHScal::pxlc && modPHScal::pxlc>=1.732*Py1l))
							{
							//构件1(主型钢P1)强度校核(槽钢部分)
							float sigma1l=(modPHScal::pxlc-1.732*Py1l)/2/F1+fabs(1000*modPHScal::mylc-modPHScal::pzlc*modPHScal::gdw)*(B1+h1)*J1y/4/(J1y+0.25*F1*B1*B1)/(J1y+0.65*J2y)+fabs(1000.0*modPHScal::mzlc-modPHScal::pxlc*modPHScal::PAfixH)*J1z/4/W1z/(J1z+0.65*J2z);//unit,kgf/mm2
							if(sigma1l>Sigma0)
							{
							if(bEOF)
							{
							//%s号构件%s槽钢%s冷态正应力值%s超过允许值%s
							sTmp.Format(GetResStr(IDS_SSteelStressOver),ltos(modPHScal::zdjh),_T("1"),strP1,GetResStr(IDS_frmtxsr_OptHeatStatus),ftos(sigma1l),ftos(Sigma0));
							throw sTmp;
							}
							else
							bErr=true;							
							}
							//构件1(主型钢P1)强度校核(焊缝部分)
							float TAOx1l=(modPHScal::pxlc-1.732*Py1l)/1.4/K/(PL3+2*b1+bb)+fabs(1000*modPHScal::mylc-modPHScal::pzlc*modPHScal::gdw)*J1y/Wyh1/(J1y+0.65*J2y)+(1000*modPHScal::mzlc-modPHScal::pxlc*modPHScal::PAfixH)*J1z/2/Wzh1/(J1z+0.65*J2z);//unit,kgf/mm2
							float TAOy1l=3*fabs(1000.0*modPHScal::mzlc-modPHScal::pxlc*modPHScal::PAfixH)*J1z/2.8/modPHScal::gdw/(J1z+0.65*J2z)/(bb+2*b1)/K;//unit,kgf/mm2
							float TAOz1l=modPHScal::pzlc/2.8/K/(PL3-h1);//unit,kgf/mm2
							//构件1焊缝合成应力
							float TAO1l=sqrt(TAOx1l*TAOx1l+TAOy1l*TAOy1l+TAOz1l*TAOz1l);//unit,kgf/mm2
							//准则
							if(TAO1l>0.6*Sigma0)
							{							
							if(bEOF)
							{
							//%s号支吊架构件%s槽钢%s焊缝冷态剪应力值%s超过允许值%s
							sTmp.Format(GetResStr(IDS_SSteelStressOver1),ltos(modPHScal::zdjh),_T("1"),strP1,GetResStr(IDS_frmtxsr_OptCoolStatus),ftos(TAO1l),ftos(0.6*Sigma0));
							throw sTmp;
							}
							else
							bErr=true;							
							}
							}
							
							  if(!bErr && (dh>36))
							  {
							  //没超限，找到
							  break;
							  }
							  else
							  {
							  rsX.MoveNext();
							  if(rsX.IsEOF())
							  {
							  //已经到记录尾，重新循环一次，以便报警
							  bEOF=true;
							  continue;
							  }
							  else
							  //重新开始之前，设置超限标志
							  bErr=false;
							  
								//获得构件3
								rsX.GetFieldValue(_T("P3"),vTmp);
								strP3=vtos(vTmp);
								//此处只适应中国标准，国际化时要修改代码
								n=strP3.Find((_T("x")));
								//构件3腿宽bb,unit:mm
								bb=atof(strP3.Mid(1,n-1));
								//获得主型钢P1
								rsX.GetFieldValue(_T("P1"),vTmp);
								strP1=vtos(vTmp);
								//获得构件2型钢P2
								rsX.GetFieldValue(_T("P2"),vTmp);
								strP2=vtos(vTmp);
								//双槽钢外缘最大距离PL3,mm
								rsX.GetFieldValue(_T("PL3"),vTmp);
								PL3=vtof(vTmp);
								//槽钢焊缝高度K,mm
								rsX.GetFieldValue(_T("size2"),vTmp);
								K=vtof(vTmp);
								//序号DH,mm
								rsX.GetFieldValue(_T("dh"),vTmp);
								dh=vtoi(vTmp);
								}
*/
				}while(true);//记录尾再循环一次,以便报警
				
				//生成一个便于校核的数据库文件和相应的Excel文件
				EDIBgbl::ChangeColumnsToRows(EDIBgbl::dbPRJ,_T("tmpCalFixPhs"),_T("CheckCalFixPhs"));
				EDIBDB::OutPutTable( _T("CheckCalFixPhs"), basDirectory::ProjectDBDir+_T("CheckCalculation.xls"), _T("Excel 5.0;"), EDIBgbl::dbPRJ,  _T("CheckCalFixPhs"), _T("Excel 5.0;"));
				
				SQLx = CString(_T("SELECT * FROM ")) + modPHScal::tbnSA + _T(" WHERE GDW1>= ") + ftos(modPHScal::gdw) + _T(" AND trim(CustomID)=\'") + Ptype[i] + _T("\' AND dh=") + ltos(dh) + _T(" ORDER BY GDW1,PmaxSF");
			}
			//结束强度计算:华东院固定支架根部双臂三角架SJ8(ClassID=iSALbraceFixG47=4)
		}			
		break;
	case iG100:
		//用户自定义根部按用户输入的规格号选择
		modPHScal::giSAG100No=_ttoi(FrmTxsr.m_pViewTxsr->m_PagOption->m_csSAG100No);
		if(modPHScal::giSAG100No<=0)
		{
			modPHScal::giSAG100No=1;
		}
		SQLx.Format(_T("SELECT * FROM %s WHERE trim(CustomID)=\'%s\' AND dh=%d"),
			modPHScal::tbnSA,
			Ptype[i],
			modPHScal::giSAG100No);
		//vtoi(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("G100No"))));
		break;
	case iGCement:
		//混凝土结构不必处理
		SQLx.Format(_T("SELECT * FROM %s WHERE trim(CustomID)=\'%s\'"),
			modPHScal::tbnSA,
			Ptype[i]);
		break;
	default:
		sTmp.Format(GetResStr(IDS_CanNotHandlePART),sPartID);
		throw sTmp;
	}
	
	//从原始数据库中查找满足条件的零件,
	//if( FirstCal = MaxCalCount ){ MsgBox _T("Sqlx=") + SQLx
	//commonHandle:
	
	//判断双吊点简支梁能否承受荷载
	if( modPHScal::glClassID == iSAbeam || modPHScal::glClassID == iG52_55)
	{
		//保存SQLx以便作为错误提示信息,要求随后的一段程序中不要使用sTmpSQL
		//不必要,因为GetBHforDoubleCSBeam函数没有使用SQLx，不会破坏SQLx值
		//sTmpSQL = SQLx
		if( ! modPHScal::gbCalSAbyConservativenessMethod )
		{
			//如果不按保守方式选择根部，则按华东院烟风煤粉管道支吊架设计手册之双吊点计算公式计算根部型钢（仅按强度公式，未考虑水平力，型钢偏小）
			//是根部，可自动选择槽钢简支梁根部
			if( modPHScal::gintParallelNum == 2 )
			{
				if( modPHScal::glNumSA == 1 )
				{
					//双吊点，单根部
					p1 = tmpSelPJG / 2;
					p2 = p1;
					//如果前一个是恒力弹簧,并且与根部直接用螺栓连接，需要双槽钢
					if( PtypeID[i - 1] == _T("PHD") || PtypeID[i - 1] == _T("PHD1") || PtypeID[i-1] == _T("LHD") || PtypeID[i-1] == _T("LHD1") || PtypeID[i-1] == _T("ZHA") || PtypeID[i-1] == _T("ZHB") )
					{
						//直接用双槽钢
						k = 2;
					}
					else
					{
						//首先找单槽钢
						k = 1;
					}
					strTmp = GetBHforDoubleCSBeam(modPHScal::gdw / 10, modPHScal::DblHangerRodDist / 10, modPHScal::Lspan / 10, p1, p2, tmpCustomID1, k, FirstCal);
					if( strTmp == _T("") )
					{
						sTmp.Format(GetResStr(IDS_NotFoundAnyBHinXSteelProperty),_T("SSteelPropertyCS"));
						throw sTmp;
					}
					else
					{
						SQLx.Format(_T("SELECT * FROM %s WHERE GDW1>=IIF( %g > L1/2 , L1 - %g , %g ) AND L1>= %g AND Pnum1=%d AND Val(Mid(P1,2))>=%g AND Mid(P1,2)>=\'%s\' AND trim(CustomID)=\'%s\' ORDER BY GDW1,L1,%s"),
							modPHScal::tbnSA,
							modPHScal::gdw + (modPHScal::glNumSA == 1 ? modPHScal::DblHangerRodDist : 0) / 2,
							modPHScal::gdw + (modPHScal::glNumSA == 1 ? modPHScal::DblHangerRodDist : 0) / 2,
							modPHScal::gdw + (modPHScal::glNumSA == 1 ? modPHScal::DblHangerRodDist : 0) / 2,
							modPHScal::Lspan,
							k,
							_tcstod(strTmp,NULL),
							Trim(strTmp),
							Ptype[i],
							SAfDPmax);
						if(rsX.IsOpen())
							rsX.Close();
						rsX.Open(dbOpenSnapshot,SQLx);
						if( rsX.IsEOF() && rsX.IsBOF() )
						{//1
							if( k == 2 )
							{
								//已经找了双槽钢，退出
								if( i > 0 )
								{
									if( i < C - 1 )
									{
										sTmp.Format(GetResStr(IDS_NotFoundPartInZDJCrude),SQLx, Ptype[i],  Ptype[i - 1],Ptype[i + 1]);
										throw sTmp;
									}
									else
									{
										sTmp.Format(GetResStr(IDS_NotFoundPartInZDJCrude) ,SQLx, Ptype[i], Ptype[i - 1], _T("NULL"));
										throw sTmp;
									}
								}
								else
								{
									sTmp.Format(GetResStr(IDS_NotFoundPartInZDJCrude),SQLx,  Ptype[i], _T("NULL"), Ptype[i + 1]);
									throw sTmp;
								}
							}
							else
							{
								//没有找到单槽钢
								//再找双槽钢
								k = 2;
							}
							strTmp = GetBHforDoubleCSBeam(modPHScal::gdw / 10, modPHScal::DblHangerRodDist / 10, modPHScal::Lspan / 10, p1, p2, tmpCustomID1, k, FirstCal);
							if( strTmp == _T("") )
							{
								sTmp.Format(GetResStr(IDS_NotFoundAnyBHinXSteelProperty),_T("SSteelPropertyCS"));
								throw sTmp;
							}
							else
							{
								SQLx.Format(_T("SELECT * FROM %s WHERE GDW1>=IIF( %g > L1/2 , L1 - %g , %g ) AND L1>= %g AND Pnum1=%d AND Val(Mid(P1,2))>=%g AND Mid(P1,2)>=\'%s\' AND trim(CustomID)=\'%s\' ORDER BY GDW1,L1,%s"),
									modPHScal::tbnSA,
									modPHScal::gdw + (modPHScal::glNumSA == 1 ? modPHScal::DblHangerRodDist : 0) / 2,
									modPHScal::gdw + (modPHScal::glNumSA == 1 ? modPHScal::DblHangerRodDist : 0) / 2,
									modPHScal::gdw + (modPHScal::glNumSA == 1 ? modPHScal::DblHangerRodDist : 0) / 2,
									modPHScal::Lspan,
									k,
									_tcstod(strTmp,NULL),
									Trim(strTmp),
									Ptype[i],
									SAfDPmax);
								//SQLx = _T("SELECT * FROM ") + tbnSA + _T(" WHERE GDW1>=IIF(") + (modPHScal::gdw + IIf(glNumSA = 1, modPHScal::DblHangerRodDist, 0) / 2) + _T(">L1/2,L1-") + (modPHScal::gdw + IIf(glNumSA = 1, modPHScal::DblHangerRodDist, 0) / 2) + _T(",") + (modPHScal::gdw + IIf(glNumSA = 1, modPHScal::DblHangerRodDist, 0) / 2) + _T(") AND L1>= ") + modPHScal::Lspan + _T(" AND Pnum1=") + k + _T(" AND Val(Mid(P1,2))>=") + Val(strTmp) + _T(" AND Mid(P1,2)>=//") + Trim(strTmp) + _T("// AND trim(CustomID)=//") + Ptype[i] + _T("// ORDER BY GDW1,L1,") + SAfDPmax
								if(rsX.IsOpen())
									rsX.Close();
								rsX.Open(dbOpenSnapshot,SQLx);
								if( rsX.IsEOF() && rsX.IsBOF() )
								{//
									if( k == 2 )
									{
										if( i > 0 )
										{
											if( i < C - 1 )
											{
												sTmp.Format(GetResStr(IDS_NotFoundPartInZDJCrude),SQLx,Ptype[i],   Ptype[i - 1],Ptype[i + 1]);
												throw sTmp;
											}
											else
											{
												sTmp.Format(GetResStr(IDS_NotFoundPartInZDJCrude),SQLx, Ptype[i],  Ptype[i - 1], _T("NULL"));
												throw sTmp;
											}
										}
										else
										{
											sTmp.Format(GetResStr(IDS_NotFoundPartInZDJCrude),SQLx, Ptype[i],  _T("NULL"), Ptype[i + 1]);
											throw sTmp;
										}
									}
								}//
							}
						}//1
					}
			  }
			  else
			  {
				  //双吊点，双根部
				  if(rsX.IsOpen())
					  rsX.Close();
				  rsX.Open(dbOpenSnapshot,SQLx);
				  p1 = tmpSelPJG / 2;
				  p2 = 0;
				  strTmp = GetBHforDoubleCSBeam(modPHScal::gdw / 10, modPHScal::DblHangerRodDist / 10, modPHScal::Lspan / 10, p1, p2, tmpCustomID1, 2, FirstCal);
			  }
		   }
		   
		   else
		   {
			   //单吊点，单根部
			   if(rsX.IsOpen())
				   rsX.Close();
			   rsX.Open(dbOpenSnapshot,SQLx);
			   p1 = tmpSelPJG;
			   p2 = 0;
			   strTmp = GetBHforDoubleCSBeam(modPHScal::gdw / 10, modPHScal::DblHangerRodDist / 10, modPHScal::Lspan / 10, p1, p2, tmpCustomID1, 2, FirstCal);
		   }
		}
		else
		{
			//按保守方式计算根部
			if(rsX.IsOpen())
				rsX.Close();
			rsX.Open(dbOpenSnapshot,SQLx);
		}
	}
	else
	{
		if(rsX.IsOpen())
			rsX.Close();
		rsX.Open(dbOpenSnapshot,SQLx);
	}
spZ1Z2:
	//如果需要进行连接孔、杆尺寸自动检查, 要求孔尺寸匹配进行查找时
	//找不到匹配的应去除此项条件再查找 add by luorijin 2009.05.12
	if( modPHScal::gbConnectHoleDimMatch && tmpSQL != _T("") && rsX.IsEOF() && rsX.IsBOF() )
	{
		if(rsX.IsOpen())
			rsX.Close();
		SQLx.Replace(tmpSQL," ");
			rsX.Open(dbOpenSnapshot,SQLx);
	}
	if( rsX.IsEOF() && rsX.IsBOF() )
	{
		switch(modPHScal::glClassID)
		{
		case iSAbeam:
			//是根部，可自动选择槽钢简支梁根部
			if( i > 0 )
			{
				if( i < C - 1 )
				{
					sTmp.Format(GetResStr(IDS_NotFoundPartInZDJCrude),SQLx, Ptype[i],  Ptype[i - 1],Ptype[i + 1]);
					throw sTmp;
				}
				else
				{
					sTmp.Format(GetResStr(IDS_NotFoundPartInZDJCrude), SQLx,Ptype[i],  Ptype[i - 1], _T("NULL"));
					throw sTmp;
				}
			}
			else
			{
				sTmp.Format(GetResStr(IDS_NotFoundPartInZDJCrude), SQLx,Ptype[i],  _T("NULL"), Ptype[i + 1]);
				throw sTmp;
			}
			break;
		case iROD:
		case iConnected:
			//case iAttached:
			//如果L5，L7，L6，L8，L8Dd，L7Dd等连接件找不到，则仅按荷载条件选择零件，取该零件的直径为最大直径
			tmpSQL = _T("SELECT * FROM ") + modPHScal::tbnPART + _T(" WHERE ") + _T(" Pmax>=");
			sTmp.Format(_T("%g"),tmpSelPJG / iNumPart * (vtob(rsza->GetCollect(_T("ifLongVertPipe")))? iNumPart : 1));
			tmpSQL+=sTmp;
			tmpSQL=tmpSQL  + _T(" AND trim(CustomID)=\'") + Ptype[i] + _T("\' ORDER BY dh,tj,Weight");
			if(rsX.IsOpen())
				rsX.Close();
			rsX.Open(dbOpenSnapshot,tmpSQL);
			if( !rsX.IsEOF() )
			{
				goto spZ1Z2;
			}
			else
			{
				if( modPHScal::gbAutoApplyCSPRLugDia && (FirstCal == 2) && modPHScal::gbPhsIsCSPR && modPHScal::PtypeDiameter[i] > 0 )
				{
					sTmp.Format(GetResStr(IDS_NotClearCheckAutoApplyCSPRLugDia),modPHScal::dbZDJcrude.GetName(),modPHScal::tbnPART, SQLx);
					throw sTmp;
				}
				else
				{
					if( i > 0 )
					{
						if( i < C - 1 )
						{
							sTmp.Format(GetResStr(IDS_NotFoundPartInZDJCrude),SQLx,Ptype[i],   Ptype[i - 1],Ptype[i + 1]);
							throw sTmp;
						}
						else
						{
							sTmp.Format(GetResStr(IDS_NotFoundPartInZDJCrude),SQLx, Ptype[i],  Ptype[i - 1], _T("NULL"));
							throw sTmp;
						}
					}
					else
					{
						sTmp.Format(GetResStr(IDS_NotFoundPartInZDJCrude),SQLx, Ptype[i],  _T("NULL"), Ptype[i + 1]);
						throw sTmp;
					}
				}
			}
			rsX.Close();
			break;
		default:
			if( i > 0 )
			{
				if( i < C - 1 )
				{
					sTmp.Format(GetResStr(IDS_NotFoundPartInZDJCrude),SQLx,Ptype[i],   Ptype[i - 1],Ptype[i + 1]);
					throw sTmp;
				}
				else
				{
					sTmp.Format(GetResStr(IDS_NotFoundPartInZDJCrude), SQLx,Ptype[i],  Ptype[i - 1], _T("NULL"));
					throw sTmp;
				}
			}
			else
			{
				sTmp.Format(GetResStr(IDS_NotFoundPartInZDJCrude), SQLx,Ptype[i],  _T("NULL"), Ptype[i + 1]);
				throw sTmp;
			}
			break;
		}
	}
	else
	{
		rsX.MoveFirst();
		if(phsAvailableTypeRs.IsOpen())
			phsAvailableTypeRs.Close();
		phsAvailableTypeRs.m_pDatabase=rsX.m_pDatabase;
		phsAvailableTypeRs.Open(dbOpenDynaset,rsX.GetSQL());
		//注意下面几句的位置，不要移动到别处，以免荷载计算不正确
		if( modPHScal::glClassID == iCSPR )
		{
			//查恒吊松紧螺母直径FiJ、FiK、S、I、T。
			//常州电力机械厂样本p20 C型恒吊的T值应该该为I值，才与前面的LHE、PHE(1<=DH<=83)的尺寸吻合。
			//编程才方便。
			//按常州电力机械厂样本，拉杆直径既要满足荷载条件，也要满足最小直径条件（安装条件）。
			//第二次计算,记录螺纹直径,以便找出最大值，所有的恒吊松紧螺母约束到这个直径	   
			if( sPartID == _T("LHG") )
			{
				//下方恒力弹簧，需要循环3次才能找出最大拉杆直径
				if( FirstCal >= 2 )
				{
					//下方恒吊松紧螺母直径=其后的拉杆直径
					modPHScal::PtypeDiameter[i] = modPHScal::PtypeDiameter[i + 1];
					if(modPHScal::gbByForce)
					{
						rsX.GetFieldValue(_T("Weight"),vTmp1);
						sTmp.Format(_T(" PmaxKgf>=%g "),
							(sPartID == _T("LHG") ? tmpSelPJG4CSPRFiJ + vtof(vTmp1) * modPHScal::gintParallelNum : tmpSelPJG4CSPRFiJ) / modPHScal::gintParallelNum);
					}
					else
					{
						sTmp.Format(_T(" fBmin<=%g AND fBmax>=%g"),
							modPHScal::sSprInfo[iSEQofSPR].CheckDisp,
							modPHScal::sSprInfo[iSEQofSPR].CheckDisp);
					}
					rsX.GetFieldValue(_T("CustomID"),vTmp2);
					SQLx.Format( _T("SELECT * FROM [%s] WHERE %s  AND FiJ>=%d AND minDH<=%d AND maxDH>=%d AND trim(CustomID)=\'%s\' %s"),
						modPHScal::tbnLugDiaOfCSPR,
						sTmp,
						(modPHScal::PtypeDiameter[i] <= 0 ? 10 : modPHScal::PtypeDiameter[i]),
						modPHScal::sSprInfo[iSEQofSPR].DH,
						modPHScal::sSprInfo[iSEQofSPR].DH,
						vtos(vTmp2),
						(modPHScal::gbByForce ? _T(" ORDER BY Pmax,FiJ") : _T(" ORDER BY FiJ")));
				}
				else
				{
					if(modPHScal::gbByForce)
					{
						rsX.GetFieldValue(_T("Weight"),vTmp1);
						sTmp.Format(_T(" PmaxKgf>=%g "),
							(sPartID == _T("LHG") ? tmpSelPJG4CSPRFiJ +vtof(vTmp1) * modPHScal::gintParallelNum : tmpSelPJG4CSPRFiJ) / modPHScal::gintParallelNum);
					}
					else
					{
						sTmp.Format(_T(" fBmin<=%g AND fBmax>=%g"),
							modPHScal::sSprInfo[iSEQofSPR].CheckDisp,
							modPHScal::sSprInfo[iSEQofSPR].CheckDisp);
					}
					rsX.GetFieldValue(_T("CustomID"),vTmp2);
					rsX.GetFieldValue(_T("Fjmin"),vTmp3);
					SQLx.Format( _T("SELECT * FROM [%s] WHERE %s  AND FiJ>=%d AND minDH<=%d AND maxDH>=%d AND trim(CustomID)=\'%s\' %s"),
						modPHScal::tbnLugDiaOfCSPR,
						sTmp,
						(vtoi(vTmp3)<=0 ? 10 : vtoi(vTmp3)),
						modPHScal::sSprInfo[iSEQofSPR].DH,
						modPHScal::sSprInfo[iSEQofSPR].DH,
						vtos(vTmp2),
						(modPHScal::gbByForce ? _T(" ORDER BY Pmax,FiJ") : _T(" ORDER BY FiJ")));
				}
			}
			else
			{
				//其它恒力弹簧，循环2次才能找出最大拉杆直径
				if( FirstCal >= 2 )
				{
					//非下方恒吊松紧螺母直径=其前的拉杆直径
					modPHScal::PtypeDiameter[i] = modPHScal::PtypeDiameter[i - 1];
					if(modPHScal::gbByForce)
					{
						rsX.GetFieldValue(_T("Weight"),vTmp1);
						sTmp.Format(_T(" PmaxKgf>=%g "),
							(sPartID == _T("LHG") ? tmpSelPJG4CSPRFiJ +vtof(vTmp1) * modPHScal::gintParallelNum : tmpSelPJG4CSPRFiJ) / modPHScal::gintParallelNum);
					}
					else
					{
						sTmp.Format(_T(" fBmin<=%g AND fBmax>=%g"),
							modPHScal::sSprInfo[iSEQofSPR].CheckDisp,
							modPHScal::sSprInfo[iSEQofSPR].CheckDisp);
					}
					rsX.GetFieldValue(_T("CustomID"),vTmp2);
					SQLx.Format( _T("SELECT * FROM [%s] WHERE %s AND FiJ>=%d AND minDH<=%d AND maxDH>=%d  AND trim(CustomID)=\'%s\' %s"),
						modPHScal::tbnLugDiaOfCSPR,					
						sTmp,
						(modPHScal::PtypeDiameter[i] <= 0 ? 10 : modPHScal::PtypeDiameter[i]),
						modPHScal::sSprInfo[iSEQofSPR].DH,
						modPHScal::sSprInfo[iSEQofSPR].DH,
						vtos(vTmp2),
						(modPHScal::gbByForce ? _T(" ORDER BY Pmax,FiJ") : _T(" ORDER BY FiJ")));
				}
				else
				{
					if(modPHScal::gbByForce)
					{
						rsX.GetFieldValue(_T("Weight"),vTmp1);
						sTmp.Format(_T(" PmaxKgf>=%g "),
							(sPartID == _T("LHG") ? tmpSelPJG4CSPRFiJ +vtof(vTmp1) * modPHScal::gintParallelNum : tmpSelPJG4CSPRFiJ) / modPHScal::gintParallelNum);
					}
					else
					{
						sTmp.Format(_T(" fBmin<=%g AND fBmax>=%g"),
							modPHScal::sSprInfo[iSEQofSPR].CheckDisp,
							modPHScal::sSprInfo[iSEQofSPR].CheckDisp);
					}
					rsX.GetFieldValue(_T("CustomID"),vTmp2);
					rsX.GetFieldValue(_T("Fjmin"),vTmp3);
					SQLx.Format( _T("SELECT * FROM [%s] WHERE %s  AND FiJ>=%d AND minDH<=%d AND maxDH>=%d AND trim(CustomID)=\'%s\' %s"),
						modPHScal::tbnLugDiaOfCSPR,
						sTmp,
						(vtoi(vTmp3)<=0 ? 10 : vtoi(vTmp3)),
						modPHScal::sSprInfo[iSEQofSPR].DH,
						modPHScal::sSprInfo[iSEQofSPR].DH,
						vtos(vTmp2),
						(modPHScal::gbByForce ? _T(" ORDER BY Pmax,FiJ") : _T(" ORDER BY FiJ")));
				}
			}
			if(rsDiaOfCSPRFiJ.IsOpen())
				rsDiaOfCSPRFiJ.Close();
			rsDiaOfCSPRFiJ.Open(dbOpenSnapshot,SQLx);
			if( rsDiaOfCSPRFiJ.IsEOF() && rsDiaOfCSPRFiJ.IsBOF() )
			{
				sTmp.Format(GetResStr(IDS_NotFoundRecordInLugDiaOfCSPR),modPHScal::tbnLugDiaOfCSPR,SQLx);
				throw sTmp;
			}
			if( iSEQofSPR + 1 <= modPHScal::giWholeSprNum )
			{
				if( iSEQofSPR == 0 )
				{
					//第一个恒力弹簧
					for(k = 0 ;k<= modPHScal::giCSPRindex[iSEQofSPR];k++)
					{
						//第一次计算，之前的所有拉杆直径约束值=当前恒吊的松紧螺母直径
						//第二次计算，根据拉杆直径选择恒吊的松紧螺母直径，这时拉杆直径必然满足了恒吊松紧螺母的最小直径。
						if( FirstCal == 1 )
						{ 
							rsDiaOfCSPRFiJ.GetFieldValue(_T("FiJ"),vTmp1);
							modPHScal::PtypeDiameter[k] = vtoi(vTmp1);
							if(modPHScal::gbRodDiaByMax)
							{
								if(modPHScal::iMaxRodDia>modPHScal::PtypeDiameter[k])
									modPHScal::PtypeDiameter[k]=modPHScal::iMaxRodDia;
								else
									modPHScal::iMaxRodDia=modPHScal::PtypeDiameter[k];
							}
						}
					}
				}
				else
				{
					//第二个以上恒力弹簧
					//bool asc=true;
					//modPHScal::giCSPRindex[iSEQofSPR - 1] + 1 > modPHScal::giCSPRindex[iSEQofSPR] ? asc=false : asc=true;
					for(k = modPHScal::giCSPRindex[iSEQofSPR - 1] + 1 ;k<=modPHScal::giCSPRindex[iSEQofSPR] ;k++)
					{
						//第一次计算，之前的所有拉杆直径约束值=当前恒吊的松紧螺母直径
						//第二次计算，根据拉杆直径选择恒吊的松紧螺母直径，这时拉杆直径必然满足了恒吊松紧螺母的最小直径。
						if( FirstCal == 1 )
						{ 
							rsDiaOfCSPRFiJ.GetFieldValue(_T("FiJ"),vTmp1);
							modPHScal::PtypeDiameter[k] = vtoi(vTmp1);
							if(modPHScal::gbRodDiaByMax)
							{
								if(modPHScal::iMaxRodDia>modPHScal::PtypeDiameter[k])
									modPHScal::PtypeDiameter[k]=modPHScal::iMaxRodDia;
								else
									modPHScal::iMaxRodDia=modPHScal::PtypeDiameter[k];
							}
						}
					}
				}
			}
	}
	else if( modPHScal::glClassID == iSPR )
	{
		//设置弹簧标志
		bSPRphs = true;
		//弹簧,临时获得其单个弹簧的最大串联数
		rsX.GetFieldValue(_T("dh"),vTmp1);
		iSumSerialNum +=vtoi(vTmp1) / 100;
		if( iSumSerialNum > modPHScal::giSumSerialNum )
		{
			sTmp.Format(GetResStr(IDS_SumSerialNumOverNeedSerialNum),_T("\%d"),_T("\%d"));
			sTmp2.Format(sTmp,iSumSerialNum,  modPHScal::giSumSerialNum);
			throw sTmp2;
		}
		else
		{
			//弹簧串联数
			modPHScal::sSprInfo[iSEQofSPR].SerialNum =vtoi(vTmp1) / 100;
			//modPHScal::sSprInfo[iSEQofSPR].SprNo = rsX.Fields(_T("dh")) Mod 100
			//modPHScal::sSprInfo[iSEQofSPR].DH = rsX.Fields(_T("dh"))
			modPHScal::sSprInfo[iSEQofSPR].haz = modPHScal::gsngTotalInstallCompressValue / modPHScal::giSumSerialNum * modPHScal::sSprInfo[iSEQofSPR].SerialNum;
			//modPHScal::sSprInfo[iSEQofSPR].HeatDisp = yr / giSumSerialNum * modPHScal::sSprInfo[iSEQofSPR].SerialNum
			rsX.GetFieldValue(_T("BH"),vTmp1);
			//弹簧规格号
			modPHScal::sSprInfo[iSEQofSPR].BH=vtos(vTmp1);
			//获得弹簧自身松紧螺母直径
			rsX.GetFieldValue(_T("size2"),vTmp1);
			//第二次计算,记录螺纹直径,以便找出最大值，所有的弹簧/恒吊松紧螺母约束到这个直径	   
			if( sPartID == _T("T5") || sPartID == _T("T3A") )
			{
				//下方弹簧/恒力弹簧，需要循环3次才能找出最大拉杆直径
				//下方/横担弹簧松紧螺母直径=其后的拉杆直径或本身直径
				if(vtoi(vTmp1)>=modPHScal::PtypeDiameter[i + 1] )
				{
					//如果弹簧松紧螺母直径>=拉杆直径
					if(modPHScal::gbAutoSPRRodDiaDecrease)
					{
						//弹簧松紧螺母直径自动缩小
						//modPHScal::giDiameter=modPHScal::PtypeDiameter[i + 1];
						modPHScal::PtypeDiameter[i] = modPHScal::PtypeDiameter[i + 1];
						modPHScal::sSprInfo[iSEQofSPR].FiJ = modPHScal::PtypeDiameter[i + 1];
					}
					else
					{
						//弹簧松紧螺母直径不自动缩小，则
						//拉杆直径取较大者
						modPHScal::giDiameter=vtoi(vTmp1);
						if(modPHScal::gbRodDiaByMax)
						{
							if(modPHScal::iMaxRodDia>modPHScal::giDiameter)
							{
							}
							else
							{
								modPHScal::iMaxRodDia=modPHScal::giDiameter;
							}
							modPHScal::PtypeDiameter[i]=modPHScal::iMaxRodDia;
							modPHScal::PtypeDiameter[i+1]=modPHScal::iMaxRodDia;
							modPHScal::sSprInfo[iSEQofSPR].FiJ=modPHScal::iMaxRodDia;
						}
						else
						{
							modPHScal::PtypeDiameter[i] = modPHScal::giDiameter;
							modPHScal::PtypeDiameter[i + 1] = modPHScal::giDiameter;
							modPHScal::sSprInfo[iSEQofSPR].FiJ = modPHScal::giDiameter;
						}
					}
				}
				else
				{
					//如果弹簧松紧螺母<拉杆直径
					//modPHScal::giDiameter=modPHScal::PtypeDiameter[i + 1];
					modPHScal::PtypeDiameter[i] = modPHScal::PtypeDiameter[i + 1];
					modPHScal::sSprInfo[iSEQofSPR].FiJ = modPHScal::PtypeDiameter[i + 1];
				}
			}
			else
			{
				//其它弹簧/恒力弹簧，循环2次才能找出最大拉杆直径
				//非下方弹簧/恒吊松紧螺母直径=其前的拉杆直径
				if(vtoi(vTmp1)>=modPHScal::PtypeDiameter[i - 1] )
				{
					//如果弹簧松紧螺母直径>=拉杆直径
					if(modPHScal::gbAutoSPRRodDiaDecrease)
					{
						//弹簧松紧螺母直径自动缩小
						//modPHScal::giDiameter=modPHScal::PtypeDiameter[i - 1];
						modPHScal::PtypeDiameter[i] = modPHScal::PtypeDiameter[i - 1];
						modPHScal::sSprInfo[iSEQofSPR].FiJ = modPHScal::PtypeDiameter[i - 1];
					}
					else
					{
						//弹簧松紧螺母直径不自动缩小，则
						//拉杆直径取较大者
						modPHScal::giDiameter=vtoi(vTmp1);
						if(modPHScal::gbRodDiaByMax)
						{
							if(modPHScal::iMaxRodDia>modPHScal::giDiameter)
							{
							}
							else
							{
								modPHScal::iMaxRodDia=modPHScal::giDiameter;
							}
							modPHScal::PtypeDiameter[i]=modPHScal::iMaxRodDia;
							modPHScal::PtypeDiameter[i-1]=modPHScal::iMaxRodDia;
							modPHScal::sSprInfo[iSEQofSPR].FiJ=modPHScal::iMaxRodDia;
						}
						else
						{
							modPHScal::PtypeDiameter[i] = modPHScal::giDiameter;
							modPHScal::PtypeDiameter[i - 1] = modPHScal::giDiameter;
							modPHScal::sSprInfo[iSEQofSPR].FiJ = modPHScal::giDiameter;
						}
					}
				}
				else
				{
					//如果弹簧松紧螺母<拉杆直径
					//modPHScal::giDiameter=modPHScal::PtypeDiameter[i - 1];
					modPHScal::PtypeDiameter[i] = modPHScal::PtypeDiameter[i - 1];
					modPHScal::sSprInfo[iSEQofSPR].FiJ = modPHScal::PtypeDiameter[i - 1];
				}
			}
	   }
	}
	
	
	if( modPHScal::gbAddPartWeight2CSPRPgz )
	{
		//累加重量到恒吊工作荷载   //累加重量到弹簧工作荷载
		//忽略附件、螺栓螺母重量，粗略计算拉杆重量（按1m考虑）
		rsX.GetFieldValue(_T("Weight"),vTmp1);
		//恒吊规格计算载荷
		tmpSelPJG4CSPRPgzNext = tmpSelPJG4CSPRPgz + vtof(vTmp1) * iNumPart;
	}
	if(modPHScal::gbAddPartWeight2SPRPgz) //pfg2007.08.16
	{
		//弹簧规格计算载荷
		tmpSelPJG4SPRPgzNext = tmpSelPJG4SPRPgz + vtof(vTmp1) * iNumPart;
		tmpSelPJG4SPRPazNext = tmpSelPJG4SPRPaz + vtof(vTmp1) * iNumPart;
	}
	
	if(  modPHScal::glIDIndex != iSA)
	{
		if( modPHScal::gbAddPartWeight2PMax)
		{
			//累加重量到计算荷载
			//忽略附件、螺栓螺母重量，粗略计算拉杆重量（按1m考虑）
			rsX.GetFieldValue(_T("Weight"),vTmp1);
			//恒吊松紧螺母直径计算载荷
			tmpSelPJG4CSPRFiK = tmpSelPJG4CSPRFiJ + vtof(vTmp1) * iNumPart;
			//其它零件计算载荷
			tmpSelPJG = tmpSelPJG + vtof(vTmp1) * iNumPart;
		}
		else
		{
			//恒吊松紧螺母直径计算载荷
			tmpSelPJG4CSPRFiK = tmpSelPJG4CSPRFiJ;
		}
	}
	//注意下面几句的位置，不要移动到别处，以免荷载计算不正确
	if( modPHScal::glClassID == iCSPR )
	{
		//查恒吊松紧螺母直径FiJ、FiK、S、I、T。
		//常州电力机械厂样本p20 C型恒吊的T值应该该为I值，才与前面的LHE、PHE(1<=DH<=83)的尺寸吻合。
		//编程才方便。样本中并未提到要加自重在计算连接螺栓直径，从数值看似乎已经考虑了自重。
		//而且加自重后有时找不到与FiJ对应的FiK，故此这里荷载不加自重。
		//_T("WHERE ") + IIf(gbByForce, _T(" PmaxKgf>=") + IIf(sPartID = _T("LHG"), tmpSelPJG4CSPRFiJ + IIf(IsNull(rsX.Fields(_T("Weight"))), 0, rsX.Fields(_T("Weight"))) * gintParallelNum, tmpSelPJG4CSPRFiJ) / gintParallelNum, _T(" fBmin<=") + modPHScal::sSprInfo[iSEQofSPR].HeatDisp + _T(" AND fBmax>=") + modPHScal::sSprInfo[iSEQofSPR].HeatDisp) + _T(" AND FiJ>=") + IIf(modPHScal::PtypeDiameter[i] <= 0, 10, modPHScal::PtypeDiameter[i]) + _T(" AND minDH<=") + modPHScal::sSprInfo[iSEQofSPR].DH + _T(" AND maxDH>=") + modPHScal::sSprInfo[iSEQofSPR].DH + _T(" AND trim(CustomID)=//") + Trim(rsX.Fields(_T("CustomID"))) + _T("// ") + IIf(gbByForce, _T(" ORDER BY Pmax,FiJ"), _T(""))
		if(modPHScal::gbByForce)
		{
			rsX.GetFieldValue(_T("Weight"),vTmp1);
			sTmp.Format(_T(" PmaxKgf>=%g "),
				(sPartID == _T("LHG") ? tmpSelPJG4CSPRFiJ +vtof(vTmp1) * modPHScal::gintParallelNum : tmpSelPJG4CSPRFiJ) / modPHScal::gintParallelNum);
		}
		else
		{
			sTmp.Format(_T(" fBmin<=%g AND fBmax>=%g"),
				modPHScal::sSprInfo[iSEQofSPR].CheckDisp,
				modPHScal::sSprInfo[iSEQofSPR].CheckDisp);
		}
		rsX.GetFieldValue(_T("CustomID"),vTmp2);
		SQLx.Format( _T("SELECT * FROM [%s] WHERE %s  AND FiJ>=%d AND minDH<=%d AND maxDH>=%d  AND trim(CustomID)=\'%s\' %s"),
			modPHScal::tbnLugDiaOfCSPR,
			sTmp,
			(modPHScal::PtypeDiameter[i] <= 0 ? 10 : modPHScal::PtypeDiameter[i]),
			modPHScal::sSprInfo[iSEQofSPR].DH,
			modPHScal::sSprInfo[iSEQofSPR].DH,
			vtos(vTmp2),
			(modPHScal::gbByForce ? _T(" ORDER BY Pmax,FiJ") : _T(" ORDER BY FiJ")));
		
		if(rsDiaOfCSPRFiK.IsOpen())
			rsDiaOfCSPRFiK.Close();
		rsDiaOfCSPRFiK.Open(dbOpenSnapshot,SQLx);
		if( rsDiaOfCSPRFiK.IsEOF() && rsDiaOfCSPRFiK.IsBOF() )
		{
			sTmp.Format(GetResStr(IDS_NotFoundRecordInLugDiaOfCSPR), modPHScal::tbnLugDiaOfCSPR, SQLx);
			throw sTmp;
		}
		rsDiaOfCSPRFiJ.GetFieldValue(_T("FiJ"),vTmp1);
		modPHScal::sSprInfo[iSEQofSPR].FiJ = vtof(vTmp1);
		rsDiaOfCSPRFiK.GetFieldValue(_T("FiK"),vTmp1);
		modPHScal::sSprInfo[iSEQofSPR].FiK = vtof(vTmp1);
	}
	rsX.GetFieldValue(_T("CustomID"),vTmp1);
	
	if(rsID.FindFirst(_T("trim(CustomID)=\'") + vtos(vTmp1) + _T("\'")))
	{
		rsID.GetFieldValue(_T("BHFormat"),vTmp1);
		sBHFormat = vtos(vTmp1);
	}
	if( sBHFormat.GetLength()<=0 )
	{
		sBHFormat = _T("trim(BH)");
	}
	rsX.GetFieldValue(_T("BH"),vTmp1);
	sBH=vtos(vTmp1);
	
	if( tmpSQL0 != _T("") )
	{
		//查找连接表达式
		//首先去掉其中的Cnte.字符
		tmpSQL0.MakeUpper();
		n=tmpSQL0.Find((_T("CNTE.")));
		while(n!=-1)
		{
			tmpSQL0 =tmpSQL0.Left(n)+tmpSQL0.Mid(n+5);
			n=tmpSQL0.Find((_T("CNTE.")));
		}
		
		n=tmpSQL0.Find((_T("CNTB.")));
		while(n!=-1)
		{
			bFound = false;
			for( m = n + 5 ;m< tmpSQL0.GetLength();m++)
			{
				for( k = 0;k<LogicalOpr.GetLength();k++)
				{
					//看是否存在任何逻辑操作符
					if( LogicalOpr.Mid(k, 1) == tmpSQL0.Mid(m, 1) )
					{
						tmpFD0 = tmpSQL0.Mid( n + 5, m - n - 5);
						if( modPHScal::glClassID == iCSPR )
						{
							//恒吊的连接信息在另外一个记录集rsDiaOfCSPRFiK，不在rsX里面。
							rsDiaOfCSPRFiK.GetFieldValue(tmpFD0,vTmp1);
							tmpSQL0 =tmpSQL0.Left(n) + (vTmp1.vt==VT_NULL || vTmp1.vt==VT_EMPTY ? _T("0") : vtos(vTmp1)) + tmpSQL0.Mid( m);
						}
						else
						{
							rsX.GetFieldValue(tmpFD0,vTmp1);
							tmpSQL0 =tmpSQL0.Left(n) + (vTmp1.vt==VT_NULL || vTmp1.vt==VT_EMPTY ? _T("0") : vtos(vTmp1)) + tmpSQL0.Mid( m);
						}
						bFound = true;
						break;
					}
				}
				
				//找完一个，退出，再找下一个_T("Cntb.")
				if( bFound ) break;
			}
			
			n=tmpSQL0.Find((_T("CNTB.")),n+1);
		}
	}
	
	
	//初步调整拉杆长度
	//保存零件数据到对象rsTZB(结构数据表JZB)中
	//及对象mvarRsTZC(材料数据表JZC)中
	//	rsTZB.AddNew();
	int iTmpRecno;
	rsTZB.GetFieldValue(_T("recno"),vTmp);
	iTmpRecno=vtoi(vTmp);
	dbstr = dbstr +"iNnerRecno = " + ltos(iTmpRecno);
	rsTZB.GetFieldValue(_T("ID"),vTmp);
	dbstr=dbstr+"  ID= "+vtos(vTmp);
	//rsTZB.FindFirst("recno="+ltos(iTmpRecno));
	rsTZB.Edit();
	rsTZB.SetFieldValue(_T("nth"),COleVariant((short)nth));//标准支吊架路数=1，包括对称双吊和共用根部双吊

	dbstr = dbstr + "  FirstCal: "+ltos(FirstCal) + " nth: " + ltos(nth);
	
	//当前零件插入下一个零件(L8)的的长度尺寸字段
	rsConnect.GetFieldValue(_T("CNTBExtLenFLD"),vTmp1);
	if( vTmp1.vt==VT_NULL || vtos(vTmp1) == _T("") )
	{
		if( sPartID==_T("L7") || sPartID==_T("G12") || sPartID==_T("L7Dd") || sPartID==_T("G12Lc") )
		{
			//这两种耳子可连接在L8上面,因此要特殊处理，才能查到正确的插入L8的尺寸。
			if( PtypeID[i - 1] == _T("L8") || PtypeID[i - 1] == _T("L8Dd") )
			{
				//前一个是L8
				//先查数据
				//以后在计算拉杆长度函数CalLugLength中只要处理这种特殊情况即可。
				rsX.GetFieldValue(tmpExtLenFD,vTmp2);
				rsTZB.SetFieldValue(_T("PreviousPartInsertLen"),COleVariant((long)vtoi(vTmp2)));
			}
			////////////////////////////////////////////
			if( PtypeID[i + 1] == _T("L8") || PtypeID[i + 1] == _T("L8Dd"))
			{
				rsX.GetFieldValue (tmpExtLenFD,vTmp2);
				rsTZB.SetFieldValue (_T("PreviousPartInsertLen"),COleVariant((long)vtoi(vTmp2)));
				rsConnect.GetFieldValue(_T("CNTBExtLenFLD"),vTmp2);
				tmpExtLenFD = vtos(vTmp2);
			}
			////////////////////////////////////////
		}
	}
	else
	{
		if(sPartID.Left(2) == _T("L7")||sPartID.Left(2) == _T("G12")||sPartID == _T("L3")||sPartID == _T("L4")||sPartID == _T("T1")||sPartID == _T("D4")||sPartID == _T("D5")||sPartID == _T("D11"))
		{
			rsConnect.GetFieldValue(_T("CNTBExtLenFLD"),vTmp2);
			tmpExtLenFD = vtos(vTmp2);
			
			//因为管部不可能有前一个，所以只需要判断它的后一个
			if(sPartID != _T("D4") && sPartID != _T("D5") && sPartID != _T("D11"))
			{
				//这两种耳子可连接在L8上面,因此要特殊处理，才能查到正确的插入L8的尺寸。
				if( PtypeID[i - 1] == _T("L8") || PtypeID[i - 1] == _T("L8Dd") )
				{
					//前一个是L8
					//先查数据
					//以后在计算拉杆长度函数CalLugLength中只要处理这种特殊情况即可。
					rsX.GetFieldValue(tmpExtLenFD,vTmp2);
					rsTZB.SetFieldValue(_T("PreviousPartInsertLen"),COleVariant((long)vtoi(vTmp2)));
					//再查字段名称
					rsConnect.GetFieldValue(_T("CNTBExtLenFLD"),vTmp2);
					tmpExtLenFD = vtos(vTmp2);
				}
				else
				{
					//前一个不是L8
					if( i > 0 && mviPreviousClassID == iCSPR )
					{
						//前一个是恒吊
						//需要查找另外一个表（恒吊松紧螺母直径表）去查找连接处一个直径(字段R)
						//这几种恒力弹簧都采用耳子连接
						if( PtypeID[i - 1] == _T("PHE1") || PtypeID[i - 1] == _T("PHE") || PtypeID[i - 1] == _T("LHE") || PtypeID[i - 1] == _T("LHE1") )
						{
							rsConnect.GetFieldValue(_T("CNTBExtLenFLD"),vTmp2);
							tmpExtLenFD = vtos(vTmp2);
							rsDiaOfCSPRFiK.GetFieldValue(tmpExtLenFD,vTmp2);
							rsTZB.SetFieldValue(_T("PreviousPartInsertLen"),COleVariant((long)vtoi(vTmp2)));
						}
					}
					else
					{
						rsConnect.GetFieldValue(_T("CNTBExtLenFLD"),vTmp2);
						tmpExtLenFD = vtos(vTmp2);
						rsTZB.SetFieldValue(_T("PreviousPartInsertLen"),COleVariant((long)tmpExtLenValue));
					}
				}
			}
			//////////////////////////////////////////////////////////////////////
			if( PtypeID[i + 1] == _T("L8") || PtypeID[i + 1] == _T("L8Dd") )
			{
				rsX.GetFieldValue(tmpExtLenFD,vTmp2);
				rsTZB.SetFieldValue(_T("PreviousPartInsertLen"),COleVariant((long)vtoi(vTmp2)));
			}
			//////////////////////////////////////////////////////////////////////////
		}
		else
		{
			if( sPartID == _T("L8") || sPartID == _T("L8Dd") )
			{
				//当前不是L7、G12,是L8
				if( PtypeID[i + 1] == _T("L7") || PtypeID[i + 1] == _T("G12") || PtypeID[i + 1] == _T("L7Dd") || PtypeID[i + 1] == _T("G12Lc") )
				{
					//保留查出的字段值，下次查表
					rsConnect.GetFieldValue(_T("CNTBExtLenFLD"),vTmp1);
					tmpExtLenFD = vtos(vTmp1);
				}
				else
				{
					//根据查出的字段值直接查表
					rsConnect.GetFieldValue(_T("CNTBExtLenFLD"),vTmp1);
					tmpExtLenFD = vtos(vTmp1);
					rsTZB.SetFieldValue(_T("PreviousPartInsertLen"),COleVariant((long)tmpExtLenValue));
				}
				///////////////////////////////////////////////////////
				//			 if( PtypeID[i - 1] == _T("L7") || PtypeID[i - 1] == _T("L7Dd"))
				//			 {
				//				 rsConnect.GetFieldValue(_T("CNTBExtLenFLD"),vTmp1);
				//				 tmpExtLenFD = vtos(vTmp1);
				//			 }
				//			 else
				//			 {
				//				 rsConnect.GetFieldValue(_T("CNTBExtLenFLD"),vTmp1);
				//			 	 tmpExtLenFD = vtos(vTmp1);
				//				 rsTZB.SetFieldValue(_T("PreviousPartInsertLen"),COleVariant((long)tmpExtLenValue));
				//			 }
				////////////////////////////////////////////////////////////
			}
			else
			{
				//当前不是L7、G12,不是L8
				if( sPartID == _T("L5") || sPartID == _T("L6") )
				{
					//当前不是L7、G12,不是L8,是L5、L6
				}
				else
				{
					//当前不是L7、G12,不是L8,不是L5、L6
					if( mviPreviousClassID == iCSPR )
					{
						//当前是恒吊
						//需要查找另外一个表（恒吊松紧螺母直径表）去查找连接处一个直径(字段R)
						if( PtypeID[i-1] == _T("PHE1") || PtypeID[i-1] == _T("PHE") || PtypeID[i-1] == _T("LHE") || PtypeID[i-1] == _T("LHE1") )
						{
							rsConnect.GetFieldValue(_T("CNTBExtLenFLD"),vTmp1);
							tmpExtLenFD = vtos(vTmp1);
							rsDiaOfCSPRFiK.GetFieldValue(tmpExtLenFD,vTmp1);
							rsTZB.SetFieldValue(_T("PreviousPartInsertLen"),vTmp1);
						}
					}
					else
					{
						rsConnect.GetFieldValue(_T("CNTBExtLenFLD"),vTmp1);
						tmpExtLenFD = vtos(vTmp1);
						rsTZB.SetFieldValue(_T("PreviousPartInsertLen"),COleVariant((long)tmpExtLenValue));
					}
				}
			}
		}
	}
	
	
	if(i==0)
	{
		//双吊管部的拉杆间距
		if( modPHScal::gintParallelNum == 2 )
		{
			if( modPHScal::glClassID == iPAh )
			{
				//双吊
				rsX.GetFieldValue(_T("sizeC"),vTmp1);
				modPHScal::DblHangerRodDist =vtof(vTmp1);
				rsTZB.SetFieldValue(_T("sizeC"),COleVariant((float)modPHScal::DblHangerRodDist));
			}
			else
			{
				if( modPHScal::glClassID == iPAs )
				{
					//双支
					rsX.GetFieldValue(_T("size3"),vTmp1);
					modPHScal::DblHangerRodDist =vtof(vTmp1);
					rsTZB.SetFieldValue(_T("sizeC"),COleVariant((float)modPHScal::DblHangerRodDist));
				}
				else
				{
					if( modPHScal::glClassID == iPAD11LD || modPHScal::glClassID == iPADLR || modPHScal::glPAid == iPALX )
					{
						//与管径有关的双吊间距
						rsX.GetFieldValue(_T("sizeC"),vTmp1);
						modPHScal::DblHangerRodDist =vtof(vTmp1) + modPHScal::dj;
						rsTZB.SetFieldValue(_T("sizeC"),COleVariant((float)modPHScal::DblHangerRodDist));
					}
				}
			}
		}
	}
	
	   //由于恒吊或弹簧不便用SQL操作的方法获得编号，故去掉其中的连接操作符。
	   EDIBgbl::dbPRJ.Execute(_T("UPDATE tmpCSLen SET BH=\'") + sBH + _T("\'"));
	   rsTZB.SetFieldValue(_T("BH"),STR_VAR(sBH));
	   //对于横担弹簧T5或横担横吊LHG，必须根据管部D13/D14找出其中心距。
	   //对于下方弹簧T3A或横担弹簧T5或横担横吊LHG，必须先查找拉杆直径。
	   //所有螺栓螺母的规格将通过GetBoltsNutsAndAttachmentsCLgg函数处理。
	   sTmp.Format(_T("%d"),modPHScal::PtypeDiameter[i]);
	   EDIBgbl::dbPRJ.Execute (_T("UPDATE tmpCSLen SET LugDiameter=") + sTmp);
	   if( modPHScal::glClassID == iCSPR )
	   {
		   //确定恒吊方向
		   if( modPHScal::gbAutoAlignCSPRtoSA && sPartID !=_T("LHG") )
		   {
			   //恒吊，自动调整方向
			   mlfx = modPHScal::GetPhsSAfx(modPHScal::fx + modPHScal::gf);
			   mlfx0 = 0;
		   }
		   
		   //载荷容量单位为kN*mm,即N*m,位移单位为mm,荷载单位为kgf(或daN=10N)
		   //tmpCapacity = TmpPgz * Tmpyr *EDIBgbl::kgf2N / 10 / iSumSerialNum
		   //SQLx = _T("SELECT * FROM [") + tbnHDproperty + _T("] WHERE Capacity>=") + tmpCapacity + _T(" AND MaxDist>=") + Tmpyr / iSumSerialNum + _T(" ORDER BY dh,Capacity,MaxDist")
		   //rs.Open(dbOpenSnapshot,SQLx);
		   //if( rs.IsEOF() And rs.IsBOF() ){
		   sTmp.Format(_T("%d"),modPHScal::PtypeDiameter[i]);
		   EDIBgbl::dbPRJ.Execute(_T("UPDATE tmpCSLen SET LugDiameter=") + sTmp);
		   //EDIBgbl::dbPRJ.Execute _T("UPDATE tmpCSLen SET SumDist=") + IIf(tmpDist > IIf(IsNull(rsX(_T("fBmax"))), 0, rsX(_T("fBmax"))), IIf(IsNull(rsX(_T("fBmax"))), 0, rsX(_T("fBmax"))), tmpDist)
		   sTmp.Format(_T("%g"),tmpDist);
		   EDIBgbl::dbPRJ.Execute(_T("UPDATE tmpCSLen SET SumDist=") +sTmp);
		   sTmp.Format(_T("%d"),tmpDistID);
		   EDIBgbl::dbPRJ.Execute(_T("UPDATE tmpCSLen SET SumDistID=") +sTmp);//对于火电厂汽水管道支吊架2007版送审稿恒力弹簧，设置位移代号
		   rsX.GetFieldValue(_T("sizeC"),vTmp1);
		   sTmp.Format(_T("%g"),(sPartID == _T("LHG") ? modPHScal::DblHangerRodDist : vtof(vTmp1)));
		   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET sizeC=") + sTmp);
		   //保存中心距，用于T5/LHG查出拉杆直径后再次生成规格CLgg
		   modPHScal::sSprInfo[iSEQofSPR].sizeC=_ttoi(sTmp);
		   sTmp.Format(_T("%g"),modPHScal::sSprInfo[iSEQofSPR].HeatDisp);
		   EDIBgbl::dbPRJ.Execute(_T("UPDATE tmpCSLen SET HeatDist=") + sTmp);
		   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET UnitForce=\'") +modPHScal::gsCSPRUnitOfLoad + _T("\'"));
		   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET DistFX=\'") + (modPHScal::yr > 0 ? modPHScal::gSprDirectionChar.CSPR_UP : modPHScal::gSprDirectionChar.CSPR_Down) + _T("\'"));
		   if(modPHScal::gsCSPRUnitOfLoad == _T("daN") )
		   {
			   //SelOpLoad=选择的工作荷载,考虑位移裕量之后求得的载荷值tmpSelOpLoad
			   sTmp.Format(_T("%d"),(long)(tmpSelOpLoad * EDIBgbl::kgf2N / 10));
			   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET SelOpLoad=") + sTmp);  //常州电力机械厂引进ITT技术，单位为daN，其它厂家为N。
			   
			   sTmp.Format(_T("%d"),(long)((sPartID==_T("LHG")?tmpSelPJG4CSPRPgz + modPHScal::g_fLHGWeight : tmpSelPJG4CSPRPgz) /modPHScal::gintParallelNum * EDIBgbl::kgf2N / 10 ));
			   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET OpLoad=") + sTmp); //常州电力机械厂引进ITT技术，单位为daN，其它厂家为N。
			   sTmp.Format(_T("%d"),(long)((sPartID==_T("LHG")?tmpSelPJG4CSPRPgz + modPHScal::g_fLHGWeight : tmpSelPJG4CSPRPgz) /modPHScal::gintParallelNum * EDIBgbl::kgf2N / 10));
			   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET InstallLoad=") + sTmp); //常州电力机械厂引进ITT技术，单位为daN，其它厂家为N。
		   }
		   else if(modPHScal::gsCSPRUnitOfLoad == _T("N") )
		   {
			   //SelOpLoad=选择的工作荷载,考虑位移裕量之后求得的载荷值tmpSelOpLoad
			   sTmp.Format(_T("%d"),(long)(tmpSelOpLoad * EDIBgbl::kgf2N));
			   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET SelOpLoad=") +sTmp);
			   sTmp.Format(_T("%d"),(long)((sPartID==_T("LHG")? tmpSelPJG4CSPRPgz + modPHScal::g_fLHGWeight : tmpSelPJG4CSPRPgz) /modPHScal::gintParallelNum * EDIBgbl::kgf2N));
			   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET OpLoad=") + sTmp);
			   sTmp.Format(_T("%d"),(long)((sPartID==_T("LHG")?tmpSelPJG4CSPRPgz + modPHScal::g_fLHGWeight : tmpSelPJG4CSPRPgz) /modPHScal::gintParallelNum * EDIBgbl::kgf2N));
			   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET InstallLoad=") + sTmp);
		   }
		   else
		   {
			   //SelOpLoad=选择的工作荷载,考虑位移裕量之后求得的载荷值tmpSelOpLoad
			   sTmp.Format(_T("%d"),(long)(tmpSelOpLoad));
			   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET SelOpLoad=") +sTmp);
			   sTmp.Format(_T("%d"),(long)((sPartID==_T("LHG")?tmpSelPJG4CSPRPgz + modPHScal::g_fLHGWeight : tmpSelPJG4CSPRPgz )/modPHScal::gintParallelNum));
			   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET OpLoad=") + sTmp);
			   sTmp.Format(_T("%d"),(long)((sPartID==_T("LHG")?tmpSelPJG4CSPRPgz + modPHScal::g_fLHGWeight : tmpSelPJG4CSPRPgz) /modPHScal::gintParallelNum));
			   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET InstallLoad=") + sTmp);
		   }
	   }
	   else if(modPHScal::glClassID == iSPR )
	   {
		   //弹簧编号中荷载用安装荷载表示 
		   if(modPHScal::gbSPRBHLOADBYInstallLoad)
		   {
			   sBHFormat.Replace("OpLoad","InstallLoad");
		   }
		   
		   sTmp.Format(_T("%d"),modPHScal::sSprInfo[iSEQofSPR].SerialNum);
		   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET SerialNum=") + sTmp);
		   sTmp.Format(_T("%d"),modPHScal::sSprInfo[iSEQofSPR].SprNo);
		   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET SprNo=") + sTmp);
		   sTmp.Format(_T("%d"),modPHScal::gSumDistOfSpring);
		   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET SumDist=") +sTmp);
		   sTmp.Format(_T("%d"),modPHScal::sSprInfo[iSEQofSPR].SumDistID);
		   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET SumDistID=") +sTmp);
		   if( (PtypeID[i-1] == _T("D13") || PtypeID[i-1] == _T("D14")) && (sPartID == _T("T5") || sPartID == _T("TDG") || sPartID == _T("LHG")) )
		   {
			   //下方弹簧或下方恒力弹簧的中心距是管部中心距C决定的，
			   //这个C值要写入弹簧规格中，作为订货依据
			   sTmp.Format(_T("%g"),modPHScal::DblHangerRodDist);
			   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET sizeC=") + sTmp);
			   //保存中心距，用于T5/LHG查出拉杆直径后再次生成规格CLgg
			   modPHScal::sSprInfo[iSEQofSPR].sizeC=_ttoi(sTmp);
		   }		  
		   sTmp.Format(_T("%g"),modPHScal::sSprInfo[iSEQofSPR].HeatDisp);
		   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET HeatDist=") + sTmp);
		   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET UnitForce=\'") +modPHScal::gsSPRUnitOfLoad + _T("\'"));
		   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET DistFX=\'") + (modPHScal::yr > 0 ? modPHScal::gSprDirectionChar.SPR_UP : modPHScal::gSprDirectionChar.SPR_Down) + _T("\'"));
		   if(modPHScal::gsSPRUnitOfLoad == _T("daN") )
		   {
			   //荷载的单位是daN(10N)
			   sTmp.Format(_T("%d"),(long)(fabs(sPartID==_T("T5")?tmpSelPJG4SPRPgz + modPHScal::g_fLHGWeight : tmpSelPJG4SPRPgz) / iNumPart * EDIBgbl::kgf2N / 10));
			   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET OpLoad=") + sTmp); //常州电力机械厂引进ITT技术，单位为daN，其它厂家为N。
			   sTmp.Format(_T("%d"),(long)(fabs(sPartID==_T("T5")?tmpSelPJG4SPRPaz + modPHScal::g_fLHGWeight : tmpSelPJG4SPRPaz) / iNumPart * EDIBgbl::kgf2N / 10));
			   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET InstallLoad=") + sTmp); //常州电力机械厂引进ITT技术，单位为daN，其它厂家为N。
		   }
		   else if(modPHScal::gsSPRUnitOfLoad == _T("N") )
		   {
			   //荷载的单位是N
			   //工作荷载四舍五入，以免出现和输入值不一致的情况,此bug最先被中南院钟毅发现
			   //(例如,pgz=9000N,yr=14mm,下两句不加0.5会出现"T2.110,14向上/8999,M20")
			   sTmp.Format(_T("%d"),(long)(fabs(sPartID==_T("T5")?tmpSelPJG4SPRPgz + modPHScal::g_fLHGWeight : tmpSelPJG4SPRPgz) / iNumPart * EDIBgbl::kgf2N+0.5));
			   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET OpLoad=") + sTmp);
			   sTmp.Format(_T("%d"),(long)(fabs(sPartID==_T("T5")?tmpSelPJG4SPRPaz + modPHScal::g_fLHGWeight : tmpSelPJG4SPRPaz) / iNumPart * EDIBgbl::kgf2N+0.5));
			   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET InstallLoad=") + sTmp);
		   }
		   else
		   {
			   //荷载的单位是kgf
			   sTmp.Format(_T("%d"),(long)(fabs(sPartID==_T("T5")?tmpSelPJG4SPRPgz + modPHScal::g_fLHGWeight : tmpSelPJG4SPRPgz) / iNumPart));
			   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET OpLoad=") + sTmp);
			   sTmp.Format(_T("%d"),(long)(fabs(sPartID==_T("T5")?tmpSelPJG4SPRPaz + modPHScal::g_fLHGWeight : tmpSelPJG4SPRPaz) / iNumPart));
			   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET InstallLoad=") + sTmp);
		   }
	   }	   
	   if(modPHScal::glIDIndex != iSA )
	   {
		   if(modPHScal::glIDIndex == iPA )
		   {
			   //温度可能作为标志管部的信息
			   sTmp.Format(_T("%g"),modPHScal::t0);
			   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET t0=") + sTmp);
			   rsX.GetFieldValue(_T("Material"),vTmp1);
			   rsTZB.SetFieldValue(_T("CLcl"),vTmp1);
			   if(modPHScal::glClassID==iPAfixZ2)
			   {
				   //固定支架Z2A/Z2/SZ5管部根部焊缝高度k1(size4),k2(size5)--mm
				   rsX.GetFieldValue(_T("size4"),vTmp1);				 
				   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET K=") + vtos(vTmp1));
				   rsX.GetFieldValue(_T("size5"),vTmp1);				 
				   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET K2=") + vtos(vTmp1));
				   //固定支架Z2A/Z2/SZ5管部支管管径DW1xS(sizeC x size3),主管外径DW--mm,华东院手册需要它们作标记。
				   rsX.GetFieldValue(_T("DW"),vTmp1);				 
				   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET DW=") + vtos(vTmp1));
				   rsX.GetFieldValue(_T("sizeC"),vTmp1);				 
				   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET dw1=") + vtos(vTmp1));
				   rsX.GetFieldValue(_T("size3"),vTmp1);				 
				   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET S=") + vtos(vTmp1));
				   //SZ5标号中需要温度t0/管径dj			 
				   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET temperature=") + ftos(modPHScal::t0));
			   }
		   }
		   
		   SQLx = _T("SELECT (") + sBHFormat + _T(") AS sBHFormat  FROM tmpCSLen");
		   if(rsTmp.IsOpen())
			   rsTmp.Close();
		   rsTmp.m_pDatabase=&EDIBgbl::dbPRJ;
		   rsTmp.Open(dbOpenDynaset,SQLx);
		   rsTmp.GetFieldValue(_T("sBHFormat"),vTmp1);
		   sBHFormat =vtos(vTmp1);
		   rsTZB.SetFieldValue(_T("BHformat"),STR_VAR(sBHFormat));
		   if(modPHScal::glClassID == iSPR || modPHScal::glClassID == iCSPR )
		   {
			   rsTZB.SetFieldValue(_T("CLgg"),STR_VAR(sBHFormat));
			   //保存弹簧/恒力弹簧信息用于T5/LHG重新生成编号，及输出弹簧订货表等
			   modPHScal::sSprInfo[iSEQofSPR].CustomID=Ptype[i];
			   modPHScal::sSprInfo[iSEQofSPR].BHFormat=sBHFormat;
			   modPHScal::sSprInfo[iSEQofSPR].FiJ=modPHScal::PtypeDiameter[i];
		   }
		   else if(modPHScal::glClassID == iROD || modPHScal::glClassID == iXL1 )
			   //拉杆的规格需要在计算长度后确定，此处必须赋予CLgg字段正确的BH。
			   //在CalLugLength函数中需要正确的赋予BH值的CLgg值。
		   {
			   rsTZB.SetFieldValue(_T("CLgg"),STR_VAR(sBH));
		   }
		   else if( modPHScal::glIDIndex ==iPA && modPHScal::g_bPipeClampBoltDiameterChanged)
			   //小荷载大管径管夹按荷载选择，螺栓的直径M1变小，管夹规格中必须注明此M1值
		   {
			   //取出管夹螺栓M1的值
			   rsX.GetFieldValue(strDiaM1FieldName,vTmp1);
			   m_fDiaM1 = vtof(vTmp1);
			   //管夹螺栓M1与U形耳子L8的螺栓M功能相似
			   CString strSQLTmp ;
				strSQLTmp.Format("SELECT * FROM [%s] WHERE [CUSTOMID]=\'%s\' AND [SIZE2]=%d ORDER BY SIZE2",
					modPHScal::tbnPART,modPHScal::sFindCustomID (_T("L8")),int(modPHScal::gmiDiameter));			   
			   CDaoRecordset m_rs;
			   COleVariant vT1;
			   if(m_rs.IsOpen())
				   m_rs.Close();
			   
			   m_rs.m_pDatabase=&modPHScal::dbZDJcrude;
			   m_rs.Open(dbOpenSnapshot,strSQLTmp);
			   if((!m_rs.IsBOF()) && !(m_rs.IsEOF()))
			   {
				   m_rs.GetFieldValue(_T("size3"),vT1);
				   int x = vtoi(vT1);
				   //如果管夹螺栓的M1的值比需要的L8的螺栓M大，			   
				   if(m_fDiaM1 > x)
				   {
						strSQLTmp.Format("SELECT * FROM [%s] WHERE [CUSTOMID]=\'%s\' AND [SIZE2]=%d ORDER BY SIZE2",
							modPHScal::tbnPART,modPHScal::sFindCustomID (_T("L7")),int(modPHScal::gmiDiameter));
						m_rs.Close();
						m_rs.Open(dbOpenSnapshot,strSQLTmp);
						m_rs.GetFieldValue(_T("size3"),vT1);
						int y = vtoi(vT1);
						if(m_fDiaM1 > y)
						{
							//同时也比L7的内环直径D大,
							//则将管夹螺栓换小
							strSQLTmp.Format(", M1=%d ",x);
							sBH = sBH + strSQLTmp;
						}
						else
							modPHScal::g_bPipeClampBoltDiameterChanged = FALSE;
				   }
				   else
				   {
					   modPHScal::g_bPipeClampBoltDiameterChanged = FALSE;
				   }
			   }
			   m_rs.Close ();
			   strPipeClampBoltDiameterChanged = PtypeID[i];//保存ID值
			   rsTZB.SetFieldValue(_T("CLgg"),STR_VAR(sBH));
			   
		   }
		   else
		   {
			   rsTZB.SetFieldValue(_T("CLgg"),STR_VAR(sBHFormat));
		   }
	   }
	}
	rsTZB.SetFieldValue(_T("recno"),COleVariant((long)iSEQofPart));
	dbstr = dbstr + " recno= " + ltos(iSEQofPart);
	rsTZB.SetFieldValue(_T("VolumeID"),COleVariant(EDIBgbl::SelVlmID));
	dbstr = dbstr + " VolumeID= " + ltos(EDIBgbl::SelVlmID);
	rsTZB.SetFieldValue(_T("zdjh"),COleVariant((long)modPHScal::zdjh));
	dbstr = dbstr + " zdjh= " + ltos(modPHScal::zdjh);
	dbstr = dbstr + "\n";
//	AfxMessageBox(dbstr);
	rsTZB.SetFieldValue(_T("CustomID"),STR_VAR(Ptype[i]));
	rsTZB.SetFieldValue(_T("ClassID"),COleVariant((long)modPHScal::glClassID));
	//计算荷载写入结果表，便于校核计算
	rsTZB.SetFieldValue(_T("Pjs"),COleVariant((float)tmpSelPJG));
	//零件左视图第一路方向
	rsTZB.SetFieldValue(_T("fx"),COleVariant((long)mlfx));
	//零件y坐标方向
	rsTZB.SetFieldValue(_T("Rotation"),COleVariant((long)mlRot));
	if( sPartID == _T("LS") || sPartID == _T("LT") )
	{
		//矩形管道，高度值要加modPHScal::dwB/2
		//零件标注尺寸，先将sizeDIM也暂时置为零件实际尺寸
		rsX.GetFieldValue(_T("sizeH"),vTmp1);
		rsTZB.SetFieldValue(_T("sizeDIM"),COleVariant((long)-(vtof(vTmp1) + modPHScal::dwB / 2)));
		//零件实际尺寸
		rsTZB.SetFieldValue(_T("sizeH"),COleVariant((double)-(vtof(vTmp1) + modPHScal::dwB / 2)));
	}
	else if( sPartID == _T("D4LQ") )
	{
		//矩形管道，高度值要加modPHScal::dwB/2
		//零件标注尺寸，先将sizeDIM也暂时置为零件实际尺寸
		rsX.GetFieldValue(_T("sizeH"),vTmp1);
		rsTZB.SetFieldValue(_T("sizeDIM"),COleVariant((long)(vtof(vTmp1) + modPHScal::dwB / 2)));
		//零件实际尺寸
		rsTZB.SetFieldValue(_T("sizeH"),COleVariant((double)(vtof(vTmp1) + modPHScal::dwB / 2)));
	}
	else if( sPartID == _T("DLR") )
	{
		//矩形管道，高度值要加modPHScal::dwB/2
		//零件标注尺寸，先将sizeDIM也暂时置为零件实际尺寸
		rsX.GetFieldValue(_T("sizeH"),vTmp1);
		rsX.GetFieldValue(_T("size4"),vTmp2);
		rsTZB.SetFieldValue(_T("sizeDIM"),COleVariant((long)(vtof(vTmp1)+ modPHScal::dwB / 2 - vtof(vTmp2))));
		//零件实际尺寸
		rsTZB.SetFieldValue(_T("sizeH"),COleVariant((double)(long)(vtof(vTmp1)+ modPHScal::dwB / 2 - vtof(vTmp2))));
	}
	else if( sPartID == _T("LM2") || sPartID == _T("LM3") || sPartID == _T("LM4") )
	{
		//水平圆形管道连环吊架，高度值要加槽钢横担高度
		//零件标注尺寸，先将sizeDIM也暂时置为零件实际尺寸
		//rsTZB.SetFieldValue(_T("sizeDIM")) = -(IIf(IsNull(rsX.Fields(_T("size5"))), 0, rsX.Fields(_T("size5"))) + rsX.Fields(_T("size7")) * 10)
		rsX.GetFieldValue(_T("sizeH"),vTmp1);
		rsTZB.SetFieldValue(_T("sizeDIM"),COleVariant((long)vtof(vTmp1)));
		//零件实际尺寸
		rsTZB.SetFieldValue(_T("sizeH"),COleVariant((double)vtoi(vTmp1)));
	}
	else if( sPartID == _T("LN2") || sPartID == _T("LN3") )
	{
		//水平圆形管道槽钢焊接连环吊架，高度值要加
		//零件标注尺寸，先将sizeDIM也暂时置为零件实际尺寸
		rsX.GetFieldValue(_T("sizeH"),vTmp1);
		rsTZB.SetFieldValue(_T("sizeDIM"),COleVariant((long)vtof(vTmp1)));
		//零件实际尺寸
		rsTZB.SetFieldValue(_T("sizeH"),COleVariant((double)vtoi(vTmp1)));
	}
	else if(sPartID==_T("L15"))
	{
		//西北院固定支架Z2/Z2A下垫一块钢板L15,其厚度值保存在tbnAttachement表字段size2中或modPHScal::gfPSThickness
		//零件标注尺寸，先将sizeDIM也暂时置为零件实际尺寸
		if(PtypeID[i+1] == _T("GZD"))
		{
			//混凝土支墩，钢板埋入其中
			rsTZB.SetFieldValue(_T("sizeDIM"),COleVariant((long)0));
			//零件实际尺寸
			rsTZB.SetFieldValue(_T("sizeH"),COleVariant((double)0));
		}
		else
		{
			rsTZB.SetFieldValue(_T("sizeDIM"),COleVariant((long)modPHScal::gfPSThickness));
			//零件实际尺寸
			rsTZB.SetFieldValue(_T("sizeH"),COleVariant((double)modPHScal::gfPSThickness));
		}
	}
	else
	{
		if(modPHScal::glIDIndex==iSA)
		{
			//根部高度置为0,以便正确统计拉杆长度
			rsTZB.SetFieldValue(_T("sizeDIM"),COleVariant((long)0));
			//零件实际尺寸
			rsTZB.SetFieldValue(_T("sizeH"),COleVariant((double)0));
		}
		else
		{
			//零件标注尺寸，先将sizeDIM也暂时置为零件实际尺寸
			rsX.GetFieldValue(_T("sizeH"),vTmp1);
			rsTZB.SetFieldValue(_T("sizeDIM"),COleVariant((long)vtof(vTmp1)));
			//零件实际尺寸
			rsTZB.SetFieldValue(_T("sizeH"),COleVariant((double)vtoi(vTmp1)));
		}
	}
	
	rsTZB.SetFieldValue(_T("CLnum"),COleVariant((long)iNumPart));
	rsTZB.SetFieldValue(_T("SEQ"),COleVariant((long)iSEQofPart));
	rsTZB.SetFieldValue(_T("CLbz"),vnil);
	
	if(rsID.FindFirst(_T("trim(ID)=\'") + sPartID + _T("\'")))
	{
		rsID.GetFieldValue(_T("Description"),vTmp1);
		rsTZB.SetFieldValue(_T("CLmc"),vTmp1);
	}
	//rsTZB.SetFieldValue(_T("CLmc")) = sFindFLD(_T("ID"), _T("Description"), sPartID)
	rsX.GetFieldValue(_T("Weight"),vTmp1);
	if( sPartID == _T("LHG") )
		modPHScal::g_fLHGWeight = vtof(vTmp1);
	if( sPartID == _T("T5") )//T5和LHG不会同时出现，因为弹簧和恒力弹簧不会同时出现
		modPHScal::g_fLHGWeight = vtof(vTmp1);
	rsTZB.SetFieldValue(_T("CLdz"),COleVariant(vtof(vTmp1)));
	rsTZB.GetFieldValue(_T("CLnum"),vTmp2);
	rsTZB.SetFieldValue(_T("CLzz"),COleVariant(vtof(vTmp1)* vtof(vTmp2)));
	
	if(modPHScal::glIDIndex != iSA )
	{
		if(i==0)
		{
			//如果不是根部
			if(modPHScal::gintParallelNum == 2 )
			{
				if(modPHScal::glClassID == iPAh )
				{
					//双吊
					rsX.GetFieldValue(_T("sizeC"),vTmp1);
					modPHScal::DblHangerRodDist =vtof(vTmp1);
					rsTZB.SetFieldValue(_T("sizeC"),COleVariant(modPHScal::DblHangerRodDist));
				}
				else
				{
					if(modPHScal::glClassID == iPAs )
					{
						//双支
						rsX.GetFieldValue(_T("size3"),vTmp1);
						modPHScal::DblHangerRodDist = vtof(vTmp1);
						rsTZB.SetFieldValue(_T("sizeC"),COleVariant(modPHScal::DblHangerRodDist));
					}
					else
					{
						if(modPHScal::glClassID == iPAD11LD ||modPHScal::glClassID == iPADLR || modPHScal::glClassID == iPALX )
						{
							//与管径有关的双吊间距
							rsX.GetFieldValue(_T("sizeC"),vTmp1);
							modPHScal::DblHangerRodDist = vtof(vTmp1) + modPHScal::dj;
							rsTZB.SetFieldValue(_T("sizeC"),COleVariant(modPHScal::DblHangerRodDist));
						}
						else
						{
							//其它类零件保存中心距，如T5,LHG等
							rsX.GetFieldValue(_T("sizeC"),vTmp1);
							rsTZB.SetFieldValue(_T("sizeC"),COleVariant(vtof(vTmp1)));
						}
					}
				}
			}
		}
	}
	else
	{
		//如果是根部，保存双槽钢的间距。
		if(modPHScal::glClassID==iSALbraceFixG48)
		{
			//G48的柱子宽度保存在原始数据库中，它=双槽钢的间距
			vTmp1=rsza->GetCollect(_T("A01"));
			rsTZB.SetFieldValue(_T("C"),vtof(vTmp1));
		}
		else if(modPHScal::glClassID==iG56 || modPHScal::glClassID==iG57)
		{
			//G56,57的柱子宽度保存在原始数据库中，它=双槽钢的间距
			vTmp1=rsza->GetCollect(_T("A01"));
			COleVariant vTmp2;
			//柱子与槽钢的间隙值m,一般=5
			rsX.GetFieldValue(_T("m"),vTmp2);
			rsTZB.SetFieldValue(_T("C"),vtof(vTmp1)+2*vtof(vTmp2));
		}
		else
		{
			rsX.GetFieldValue(_T("CHDIST"),vTmp1);
			rsTZB.SetFieldValue(_T("C"),vtof(vTmp1));
		}
		
		//下段代码容易出错，经常将正确的跨度值改成更大的值。还是注释掉，由用户控制跨度值的准确性。
		if ( (modPHScal::gintParallelNum==2) && (modPHScal::glNumSA==1) /*&& modPHScal::bPAIsHanger()*/)
		{
			//双吊、单根部，校核根部跨度
			if( (modPHScal::glClassID==iSAbeam) || (modPHScal::glClassID==iG51) || (modPHScal::glClassID==iG52_55) || (modPHScal::glClassID==iG100) || (modPHScal::glClassID==iSACantilever) && sPartID!=_T("G23") && sPartID!=_T("G24"))
			{
				//只检查简支梁
				if( modPHScal::DblHangerRodDist > 0 && modPHScal::Lspan <modPHScal::gdw + modPHScal::DblHangerRodDist + 100 )
				{
					modPHScal::Lspan =modPHScal::gdw + modPHScal::DblHangerRodDist + 100;
					rsza->PutCollect(_T("LC1"),_variant_t((float)modPHScal::Lspan));
				}
			}
		}
	}
	//保存索引号，以便在汇总材料时能够迅速识别根部或其他类别。
	rsTZB.SetFieldValue(_T("Index"),COleVariant((long)modPHScal::glIDIndex));
	rsTmpZB.GetFieldValue(_T("CustomID"),vTmp1);
	
	if( rsID.FindFirst (_T("trim(CustomID)=\'") + vtos(vTmp1) + _T("\'")) )
	{
		rsID.GetFieldValue(_T("ID"),vTmp1);
		rsTZB.SetFieldValue(_T("ID"),vTmp1);
	}
	   CString strBH,strID;
	   CDaoRecordset rsTmp2;
	   switch(modPHScal::glClassID)
	   {
	   case iConnected:
		   //非拉杆的连接件
		   if( sPartID.Left(2)== _T("L5") ||sPartID.Left(2)== _T("L6") ||sPartID.Left(2)== _T("L7") ||sPartID.Left(2)== _T("L8"))
		   {
			   rsX.GetFieldValue(_T("size4"),vTmp1);
			   rsTZB.SetFieldValue(_T("AllowedMinLugExt"),vtof(vTmp1));
			   rsX.GetFieldValue(_T("size5"),vTmp1);
			   rsTZB.SetFieldValue(_T("AllowedMaxLugExt"),vtof(vTmp1));
			   //第二次计算,记录螺纹直径,以便找出最大值，所有的弹簧/恒吊松紧螺母约束到这个直径
			   if( FirstCal == 1 )
			   {
				   rsX.GetFieldValue(_T("size2"),vTmp1);
				   modPHScal::PtypeDiameter[i] =vtoi(vTmp1);
				   if(modPHScal::gbRodDiaByMax)
				   {
					   if(modPHScal::iMaxRodDia>modPHScal::PtypeDiameter[i])
						   modPHScal::PtypeDiameter[i]=modPHScal::iMaxRodDia;
					   else
						   modPHScal::iMaxRodDia=modPHScal::PtypeDiameter[i];
					   modPHScal::gmiDiameter=modPHScal::iMaxRodDia;
				   }
				   else
				   {
					   //记录最靠近管部的第一根拉杆的直径，
					   //以便当管夹螺栓可换小时能根据拉杆直径确定换小后螺栓的直径。
					   //added by ligb on 2003.08.17
					   if(modPHScal::giDiameter==0)	
						   modPHScal::gmiDiameter=modPHScal::PtypeDiameter[i];
				   }				   
			   }
		   }
		   
		   rsTZB.Update();  //?,每个case语句后都有这个更新语句
		   break;
	   case iROD:
		   //拉杆
		   //保存拉杆两端的螺纹长度
		   //L1的右螺纹，L2的左螺纹
		   rsX.GetFieldValue(_T("size4"),vTmp1);
		   rsTZB.SetFieldValue(_T("AllowedMinLugExt"),vtof(vTmp1));
		   //L1的右螺纹，L2的右螺纹
		   rsX.GetFieldValue(_T("size3"),vTmp1);
		   rsTZB.SetFieldValue(_T("AllowedMaxLugExt"),vtof(vTmp1));
		   //拉杆，获得直径
		   rsX.GetFieldValue(_T("size2"),vTmp1);
		   modPHScal::giDiameter = vtof(vTmp1);
		   //如果直径<10mm,自动启动限制拉杆直径
		   if(modPHScal::giDiameter<10)
		   {
			   modPHScal::gbLimitLugMinDia=true;
			   SetRegValue(_T("Settings"), _T("LimitLugMinDia"), modPHScal::gbLimitLugMinDia);
		   }
		   //第二次计算,记录螺纹直径,以便找出最大值，所有的弹簧/恒吊松紧螺母约束到这个直径		  
		   //第一次计算，保存这些直径
		   if( FirstCal ==1 /*|| FirstCal ==2*/ ) 
		   {		   
			   modPHScal::PtypeDiameter[i] = modPHScal::giDiameter;		  
			   if(modPHScal::gbRodDiaByMax)
			   {
				   if(modPHScal::iMaxRodDia>modPHScal::PtypeDiameter[i])
					   modPHScal::PtypeDiameter[i]=modPHScal::iMaxRodDia;
				   else
					   modPHScal::iMaxRodDia=modPHScal::PtypeDiameter[i];
				   modPHScal::gmiDiameter=modPHScal::iMaxRodDia;
			   }
			   else
			   {
				   //记录最靠近管部的第一根拉杆的直径，
				   //以便当管夹螺栓可换小时能根据拉杆直径确定换小后螺栓的直径。
				   //added by ligb on 2003.08.17
				   if(modPHScal::giDiameter==0)	   modPHScal::gmiDiameter=modPHScal::giDiameter;
			   }
		   }
		   //暂时借用sizeC字段保存拉杆直径，用于以后调整拉杆长度。
		   rsTZB.SetFieldValue(_T("sizeC"),COleVariant(modPHScal::giDiameter));
		   //拉杆直径保存到数组，以便过程末尾查找附件时使用。主要处理附件记录位于拉杆之前的这种情况，所有拉杆始端的附件都是这种情况。
		   //sizeH置为0便于后面统计总长
		   rsTZB.SetFieldValue(_T("sizeH"),COleVariant((long)0));
		   //暂时借用GDW1字段保留拉杆单重(kg/米),用于以后将拉杆作为零件计算整体单重。
		   rsX.GetFieldValue(_T("Weight"),vTmp1);
		   rsTZB.SetFieldValue(_T("GDW1"),vTmp1);
		   rsTZB.Update();  //?,每个case语句后都有这个更新语句
		   break;
	   case iSAh:
	   case iSAbeam:
	   case iSACantilever:
	   case iSALbrace:
	   case iSALbraceFixG47:
	   case iSALbraceFixG48:
	   case iG51:
	   case iG52_55:
	   case iG56:
	   case iG57:
	   case iG100://用户自定义根部不计算荷载，而要统计材料
		   {
			   rsTZB.SetFieldValue(_T("CLcl"),STR_VAR(GetResStr(IDS_GROUPWARE)));
			   j = 1; //主型钢
			   CString sj;
			   sj.Format(_T("%d"),j);
			   rsX.GetFieldValue(_T("P")+sj,vTmp1);
			   CString sTmp3=vtos(vTmp1);
			   sTmp3=sTmp3.Mid(1);
			   sTmp3.Format(_T("%g"),_tcstod(sTmp3,NULL)*10);
			   rsX.GetFieldValue(_T("Pnum")+sj,vTmp2);
			   sTmp=vtos(vTmp1);
			   //Pnum1=NULL或0，根部主型钢数量错误
			   if( sTmp!=_T("") &&  vtoi(vTmp2) > 0 )
				   //if( sTmp!=_T("") )			
			   {
				   if(modPHScal::glClassID == iSAh )
				   {
					   //直接吊G11
					   rsX.GetFieldValue(_T("PL")+sj,vTmp1);
					   sTmp=vtos(vTmp1);
					   if( sTmp!= _T("") )
					   {
						   rsX.GetFieldValue(_T("BH"),vTmp1);
						   EDIBgbl::dbPRJ.Execute( CString(_T("UPDATE tmpCSLen SET BH=\'")) + vtos(vTmp1) + _T("\'"));
						   rsTZB.SetFieldValue(_T("BH"),vTmp1);
						   rsX.GetFieldValue(_T("A"),vTmp1);
						   sTmp2.Format(_T("%g"),vtof(vTmp1));						
						   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET a=") +sTmp2);
						   //G11的公式为sizeH+A,其中sizeH=螺杆长度,长度储存在A01字段。
						   sTmp2.Format(_T("%g"),modPHScal::WidthA);
						   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET sizeH=") + sTmp2);
						   //暂时只要计算主零件的长度
						   sngCSLen = modPHScal::CSLength(sBHFormat, sTmp, sBH);
					   }
					   rsTZB.SetFieldValue(_T("sizeH"),COleVariant((float)modPHScal::WidthA));
					   rsTZB.SetFieldValue(_T("sizeDIM"),COleVariant((float)modPHScal::WidthA));
				   }
				   else
				   {
					   //非直接吊G11,即槽钢
					   rsX.GetFieldValue(_T("PL")+sj,vTmp1);
					   sTmp=vtos(vTmp1);
					   if( sTmp!= _T("") )
					   {
						   rsX.GetFieldValue(_T("BH"),vTmp1);
						   EDIBgbl::dbPRJ.Execute( CString(_T("UPDATE tmpCSLen SET BH=\'")) + vtos(vTmp1) + _T("\'"));                           
						   rsTZB.SetFieldValue(_T("BH"),vTmp1);
						   //G100自定义根部的主型钢型号P1写入tmpCSLen，以便作为材料规格号.
						   rsX.GetFieldValue(_T("P1"),vTmp1);
						   EDIBgbl::dbPRJ.Execute( CString(_T("UPDATE tmpCSLen SET P1=\'")) + vtos(vTmp1) + _T("\'"));                           
						   
						   rsX.GetFieldValue(_T("A"),vTmp1);
						   double fLenA=0;//记录悬臂端部到拉杆受力点之间的距离值A  Add by luorijin 2008.10.30
						   fLenA = vtof(vTmp1);
						   sTmp2.Format(_T("%g"),vtof(vTmp1));
						   if(modPHScal::glClassID==iG52_55)
						   {
							   //G52_55的公式为H+h+100,其中H=板肋高度,长度储存在A01字段。
							   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET CHLegHeight=")+sTmp3 );
							   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET CHheight=") +sTmp3);					   
						   }
						   else
						   {
							   //G47的双槽钢间距chdist=300,G48的双槽钢间距=Bwidth(柱子宽度)
							   
						   }
						   
						   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET a=") +sTmp2);
						   rsX.GetFieldValue(_T("c"),vTmp1);
						   sTmp2.Format(_T("%g"),vtof(vTmp1));
						   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET c=") +sTmp2);
						   sTmp2.Format(_T("%g"),vtof(rsza->GetCollect(_T("B01"))));
						   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET B=") +sTmp2);
						   sTmp2.Format(_T("%g"),vtof(rsza->GetCollect(_T("A01"))));
						   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET BWidth=") +sTmp2);
						   rsTZB.SetFieldValue(_T("B"),rsza->GetCollect(_T("B01")));
						   rsTZB.SetFieldValue(_T("A"),rsza->GetCollect(_T("A01")));
						   
						   //H=生根梁的高度
						   sTmp2.Format(_T("%g"),vtof(rsza->GetCollect(_T("A01"))));
						   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET H=") +sTmp2);
						   
						   
						   rsX.GetFieldValue(_T("m"),vTmp1);
						   sTmp2.Format(_T("%g"),vtof(vTmp1));
						   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET m=") +sTmp2);
						   rsX.GetFieldValue(_T("CHdist"),vTmp1);
						   sTmp2.Format(_T("%g"),vtof(vTmp1));
						   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET CHDist=") +sTmp2);
						   sTmp2.Format(_T("%g"),modPHScal::Lspan);
						   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET Lspan=") +sTmp2);
						   sTmp2.Format(_T("%g"),modPHScal::gdw);
						   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET GDW1=") +sTmp2);

						   sTmp2.Format(_T("%g"),modPHScal::Lspan);
						   EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET L1=") +sTmp2);
						   
						   //把简支梁跨度l1写到ZB 表LSpan字段，以方便三维使用，并且便于记录非标根部
						   rsTZB.SetFieldValue(_T("LSpan"),COleVariant((float)modPHScal::Lspan));
						   rsTZB.SetFieldValue(_T("XBL1"),COleVariant((float)(modPHScal::gdw+fLenA)));//XBL1记录悬臂长度  Add by luorijin 2008.10.30
						   //暂时只要计算主型钢的长度
						   sngCSLen = modPHScal::CSLength(sBHFormat, sTmp, sBH);
					   }
					   //槽刚的a值保存到字段T,该值在绘制根部定位尺寸时很重要。如G22~G24,G3x,G6x,G7x类
					   //if( (sPartID.Left(2) == _T("G3")) || (sPartID.Left(2) ==  _T("G6") ) || (sPartID.Left(2) == _T("G7")) || (sPartID.Left(2) == _T("G5")) && (sPartID.Left(3) != _T("G51"))  && (sPartID.Left(3) != _T("G56"))  && (sPartID.Left(3) != _T("G57")) )
					   if( (modPHScal::glClassID==iSAbeam) || (modPHScal::glClassID==iG52_55 ) )
						   rsTZB.SetFieldValue(_T("T"),COleVariant((float)(modPHScal::Lspan - modPHScal::gdw)));
					   else
					   {
						   rsX.GetFieldValue(_T("A"),vTmp1);
						   rsTZB.SetFieldValue(_T("T"),vtof(vTmp1));
					   }
					   
					   rsTZB.SetFieldValue(_T("L1"),COleVariant((float)sngCSLen));
					   rsTZB.SetFieldValue(_T("GDW1"),COleVariant(modPHScal::gdw));
					   //更新原始数据表TZA中的根部主槽钢数量
					   rsX.GetFieldValue(_T("Pnum1"),vTmp1);
					   if( vTmp1.vt==VT_NULL)
					   {
						   modPHScal::iCSnum = 1;
					   }
					   else
					   {
						   modPHScal::iCSnum = vtoi(vTmp1);
					   }
					   rsza->PutCollect(_T("iCSnum"),_variant_t((long)modPHScal::iCSnum));
					   
				   }
				   rsTZB.SetFieldValue(_T("CLgg"),STR_VAR(sBH));
				   //rsTZB.SetFieldValue(_T("BH"),STR_VAR(sBH));
				   //更新根部主槽钢总长度
				   rsza->PutCollect(_T("SATotalLength"),_variant_t(sngCSLen));
				   if(modPHScal::glClassID == iSAbeam )
				   {
				   }
				   else if(modPHScal::glClassID == iSALbrace || modPHScal::glClassID == iSALbraceFixG47 || modPHScal::glClassID == iSALbraceFixG48 || modPHScal::glClassID == iSACantilever )
				   {
					   //rsza->PutCollect(_T("SATotalLength"),_variant_t(sngCSLen));
					   rsza->PutCollect(_T("LC1"),_variant_t(sngCSLen));
				   }
				   
		  }
		  else
		  {
			  //碰到为空的附件，退出，加快速度。
			  //输入原始数据库时应该注意所有的附件从1开始填写，不要间断。
			  //由于以前输入的数据并不完全符合上面要求，故要取消退出，否则结果中有些附件不会被统计。
			  //Exit For
			  if( j = 1 )
			  {
				  if(modPHScal::glClassID == iSAh ||modPHScal::glClassID == iGCement )
				  {
					  //直接吊或混凝土梁或支墩
				  }
				  else
				  {
					  //型钢根部，P1字段必需有值
					  rsX.GetFieldValue(_T("BH"),vTmp1);
					  sTmp2.Format(GetResStr(IDS_SAP1MustBeChannelSteelNo),modPHScal::tbnSA,vtos(vTmp1));
					  //frmStatus.MessageBox(sTmp2);
					  ShowMessage(sTmp2);
				  }
			  }
		  }
		  
		  rsTZB.Update();
		  //根部附件,j=1是主型钢，统计时,如果在支吊架组装图中
		  //绘制零件明细表,则只标出根部整体结构;
		  //而统计材料时,应该拆成型钢.
		  //首先我们要做一个假定，以便简化此处的编程。
		  //即：所有附件要么在附件表中，要么在根部附件表中，要么在螺栓螺母表中。
		  //且根部表中的所有附件规格即Pi(i=1~12)字段的内容即是根部附件表中的BH字段。
		  //因此，制订支吊架根部规范时： 一定要使用相同标准的根部表和附件表。
		  //选择规范时则不必考虑，因为根部附件与根部数据表的信息都存放在根部信息管理表PhsManuSA中。
		  CString strBH;
		  
		  CString tmpID;
		  for (j = 1 ;j<= 12;j++)
		  {
			  sj.Format(_T("%d"),j);
			  rsX.GetFieldValue(_T("P")+sj,vTmp1);
			  rsX.GetFieldValue(_T("Pnum")+sj,vTmp2);
			  sTmp=vtos(vTmp1);
			  if( sTmp!=_T("") && vtoi(vTmp2)>0 )
			  {
				  mvSAattachedCustomID =sTmp;
				  //看是否是型钢:槽钢、角钢、工字钢、圆钢、扁钢？（钢板作为附件处理）
				  //扁钢(在根部表和型钢表中)的存放格式应为：-宽x厚，如-90x16。
				  SQL1 = _T("SELECT * FROM [") + modPHScal::tbnSectionSteel + _T("] WHERE trim(BH)=\'") + mvSAattachedCustomID + _T("\' ORDER BY bh");
				  if(rsTmp.IsOpen())
					  rsTmp.Close();
				  rsTmp.m_pDatabase=&modPHScal::dbZDJcrude;
				  rsTmp.Open(dbOpenSnapshot,SQL1);
				  if( rsTmp.IsEOF() && rsTmp.IsBOF() )
				  {
					  //不是型钢
					  //****************************
					  //找不到,则是附件,再查附件表
					  rsTmp.Close();
					  SQL1 = _T("SELECT * FROM [") + modPHScal::tbnAttachment + _T("] WHERE trim(BH)=\'") + mvSAattachedCustomID + _T("\' ORDER BY bh");
					  /*if(rsTmp.IsOpen())
					  rsTmp.Close();
					  rsTmp.m_pDatabase=&modPHScal::dbZDJcrude;*/
					  rsTmp.Open(dbOpenSnapshot,SQL1);
					  if( rsTmp.IsEOF() && rsTmp.IsBOF() )
					  {
						  //不是附件,则是螺栓螺母,再查螺栓螺母表
						  //螺栓螺母：根部使用的可能是特定标准的螺栓螺母，而配件可能使用其它标准的螺栓螺母，如国家标准螺栓螺母。
						  //其编号方式为M36x200,或M20
						  //因此，首先查找根部螺栓螺母表tbnSABoltsnuts，再根据查出的规格从通用螺栓螺母表tbnBoltsNutsID中查找相应的规格。
						  rsTmp.Close();
						  SQL1 = _T("SELECT * FROM [") + modPHScal::tbnSABoltsNuts + _T("] WHERE trim(BH)=\'") + mvSAattachedCustomID + _T("\' ORDER BY bh");
						  /*if(rsTmp.IsOpen())
						  rsTmp.Close();
						  rsTmp.m_pDatabase=&modPHScal::dbZDJcrude;*/
						  rsTmp.Open(dbOpenSnapshot,SQL1);
						  if( rsTmp.IsEOF() && rsTmp.IsBOF() )
						  {
							  //没找到螺栓螺母记录
							  sBH = _T("");
							  strBH = _T("");
							  sngWeight = 0;
							  mviSASSClassID = iSABoltsNutsUnknown;
							  //rsTmp.Close();
							  sTmp2.Format(GetResStr(IDS_NotFoundThisSAAttached),mvSAattachedCustomID);
							  //frmStatus.MessageBox(sTmp2);
							  ShowMessage(sTmp2);
							  //Err.Number = iUE_NotFoundThisSAAttached
							  //Err.Description = ResolveResString(iUE_NotFoundThisSAAttached, _T("|1"), mvSAattachedCustomID)
							  //Err.Raise iUE_NotFoundThisSAAttached
							  //报告一个错误，但不要退出。
							  //MsgBox ResolveResString(iUE_NotFoundThisSAAttached, _T("|1"), mvSAattachedCustomID), vbMsgBoxSetForeground
						  }
						  else
						  {
							  //找到相应的螺栓或螺母
							  strBH = _T("");
							  rsTmp.MoveFirst();
							  rsTmp.GetFieldValue(_T("BH"),vTmp1);
							  sBH = vtos(vTmp1);
							  strBH=sBH;
							  sTmp=CString(_T("UPDATE tmpCSLen SET BH=\'")) + sBH + _T("\'");
							  EDIBgbl::dbPRJ.Execute( sTmp);
							  rsTmp.GetFieldValue(_T("CustomID"),vTmp1);
							  tmpCustomID = vtos(vTmp1);
							  sTmp= CString(_T("UPDATE tmpCSLen SET CustomID=\'")) + tmpCustomID + _T("\'");
							  EDIBgbl::dbPRJ.Execute( sTmp);
							  
							  rsTmp.GetFieldValue(_T("size2"),vTmp1);
							  tmpSize2 =vtof(vTmp1);
							  rsTmp.GetFieldValue(_T("sizeH"),vTmp1);
							  tmpSizeH =vtof(vTmp1);
							  tmpID=modPHScal::sFindID(tmpCustomID);
							  if(tmpID==_T("F9") || tmpID==_T("F10"))
							  {
								  if(modPHScal::glClassID==iG56 || modPHScal::glClassID==iG57 || modPHScal::glClassID==iG52_55 || modPHScal::glClassID==iG51)
								  {
									  rsX.GetFieldValue(_T("PL")+sj,vTmp1);
									  sTmp=vtos(vTmp1);
									  if(rsID.FindFirst(_T("trim(CustomID)=\'") + tmpCustomID + _T("\'")))
									  {
										  rsID.GetFieldValue(_T("BHFormat"),vTmp1);
										  sBHFormat = vtos(vTmp1);
									  }
									  else
									  {
									  }
									  sTmp2.Format(_T("%g"),vtof(rsza->GetCollect(_T("A01"))));
									  EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET BWidth=") +sTmp2);								
									  sTmp2.Format(_T("%g"),vtof(rsza->GetCollect(_T("A01"))));
									  EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET H=") +sTmp2);
									  sTmp2.Format(_T("%g"),vtof(rsza->GetCollect(_T("B01"))));
									  EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET B=") +sTmp2);								
									  //计算螺栓的长度
									  sngCSLen = modPHScal::CSLength(sBHFormat, sTmp, sBH);
									  SQL1.Format(_T("SELECT * FROM [%s] WHERE trim(CustomID)=\'%s\' AND SIZE2=%g  ORDER BY bh"),
										  modPHScal::tbnBoltsNuts,
										  tmpCustomID,
										  tmpSize2);
									  
								  }
							  }
							  else if(tmpID==_T("F14"))
							  {
								  //六角头螺栓F14
								  //再根据查出的规格从通用螺栓螺母表tbnBoltsNuts中查找相应的规格。
								  SQL1.Format(_T("SELECT * FROM [%s] WHERE trim(CustomID)=\'%s\' AND SIZE2=%g AND sizeH=%g ORDER BY bh"),
									  modPHScal::tbnBoltsNuts,
									  tmpCustomID,
									  tmpSize2 ,
									  tmpSizeH);
								  sngCSLen=1000;
							  }
							  else
							  {
								  //六角螺母F1、六角扁螺母F2、球面垫圈F4等
								  //再根据查出的规格从通用螺栓螺母表tbnBoltsNuts中查找相应的规格。
								  SQL1.Format(_T("SELECT * FROM [%s] WHERE trim(CustomID)=\'%s\' AND SIZE2=%g ORDER BY bh"),
									  modPHScal::tbnBoltsNuts,
									  tmpCustomID,
									  tmpSize2);
							  }
							  rsTmp.Close();
							  rsTmp.Open(dbOpenSnapshot,SQL1);
							  if( rsTmp.IsEOF() && rsTmp.IsBOF() )
							  {
								  //没有记录
								  sBH = _T("");
								  sngWeight = 0;
								  mviSASSClassID = iBoltsNutsUnknown;
								  sTmp2.Format(GetResStr(IDS_NotFoundThisSAAttached),tmpCustomID);
								  //frmStatus.MessageBox(sTmp2);
								  ShowMessage(sTmp2);
							  }
							  else
							  {
								  //在通用螺栓螺母表中找到相应的螺栓或螺母
								  rsTmp.MoveFirst();
								  if(tmpID==_T("F9") || tmpID==_T("F10"))
								  {
									  //F9、F10已经查出了直径和长度,只要在通用螺栓螺母表中查出它们的CustomID和BH,再生成CLgg，就可以了。
									  rsTmp.GetFieldValue(_T("BH"),vTmp1);
									  sBH = vtos(vTmp1);
									  sTmp= CString(_T("UPDATE tmpCSLen SET BH=\'")) + sBH + _T("\'");
									  EDIBgbl::dbPRJ.Execute(sTmp);
									  
									  rsID.GetFieldValue(_T("BHFormat"),vTmp1);
									  sBHFormat = vtos(vTmp1);															    
									  sngCSLen = modPHScal::CSLength(sBHFormat, ftos(sngCSLen), sBH);
								  }
								  else if(tmpID==_T("F14"))
								  {
									  rsTmp.GetFieldValue(_T("BH"),vTmp1);
									  sBH = vtos(vTmp1);
									  sngCSLen = 1000;  //设置为1000,便于编程.
								  }						
								  else
								  {
									  rsTmp.GetFieldValue(_T("BH"),vTmp1);
									  sBH = vtos(vTmp1);
									  sngCSLen = 1000;  //设置为1000,便于编程.
								  }
								  rsTmp.GetFieldValue(_T("Weight"),vTmp1);
								  sngWeight =vtof(vTmp1)*sngCSLen/1000;
								  sngW = sngWeight;
								  rsTmp.GetFieldValue(_T("CustomID"),vTmp1);
								  tmpCustomID = vtos(vTmp1);                    
								  if(rsID.FindFirst( _T("trim(CustomID)=\'") + tmpCustomID + _T("\'")))
								  {
									  rsID.GetFieldValue(_T("ClassID"),vTmp1);
									  mviSASSClassID =vtoi(vTmp1);
									  rsID.GetFieldValue(_T("Index"),vTmp1);
									  mviSASSIndex =vtoi(vTmp1);
								  }
							  }
					  }
				   }
				   else
				   {
					   //查附件表找到附件,不是螺栓螺母,
					   strBH=_T("");
					   rsTmp.MoveFirst();
					   rsTmp.GetFieldValue(_T("BH"),vTmp1);
					   sBH = vtos(vTmp1);
					   rsTmp.GetFieldValue(_T("Weight"),vTmp1);
					   sngWeight =vtof(vTmp1);
					   sngW = sngWeight;
					   rsTmp.GetFieldValue(_T("CustomID"),vTmp1);
					   tmpCustomID = vtos(vTmp1);
					   tmpID=modPHScal::sFindID(tmpCustomID);
					   if(rsID.FindFirst( _T("trim(CustomID)=\'") + tmpCustomID + _T("\'")))
					   {
						   rsID.GetFieldValue(_T("ClassID"),vTmp1);
						   mviSASSClassID =vtoi(vTmp1);
						   rsID.GetFieldValue(_T("Index"),vTmp1);
						   mviSASSIndex =vtoi(vTmp1);
					   }
					   sngCSLen = 1000;  //设置为1000,便于编程.
				   }
				   //sngCSLen = 1000;  //设置为1000,便于编程.
				   //****************************
				   //结束不是型钢
				}
				else
				{
					//是型钢
					//if(modPHScal::glClassID = iSACantilever OrmodPHScal::glClassID = iSAbeam OrmodPHScal::glClassID = iSALbrace )
					if(modPHScal::glIDIndex == iSA )
					{
						//是根部附件中的型钢
						//查型钢单重
						if( rsTmp.IsEOF() && rsTmp.IsBOF() )
						{
							//没有这种型钢规格
							sTmp.Format(GetResStr(IDS_NothisSectionSteelInZDJcrudeMdb),modPHScal::dbZDJcrude.GetName(),mvSAattachedCustomID);
							throw sTmp;
						}
						else
						{
							//有这种型钢规格
							rsTmp.MoveFirst();
							rsTmp.GetFieldValue(_T("CustomID"),vTmp1);
							tmpCustomID =vtos(vTmp1);
							rsTmp.GetFieldValue(_T("Material"),vTmp1);
							mvsSASSMaterial = vtos(vTmp1);
							
							if( rsID.FindFirst(_T("trim(CustomID)=\'") + tmpCustomID + _T("\'")))
							{
								rsID.GetFieldValue(_T("ClassID"),vTmp1);
								mviSASSClassID = vtoi(vTmp1);
								rsID.GetFieldValue(_T("Index"),vTmp1);
								mviSASSIndex =vtoi(vTmp1);
								//暂时保存根部主型钢高度，用于以后确定恒吊与根部连接的螺栓的高度。
								if( j == 1 )
								{
									rsTmp.GetFieldValue(_T("sizeH"),vTmp1);
									SACSHeight =vtof(vTmp1);
								}
							}
						}
						rsX.GetFieldValue(_T("PL")+sj,vTmp1);
						sTmp=vtos(vTmp1);
						if(sTmp!=_T("") )
						{
							rsTmp.GetFieldValue(_T("BH"),vTmp1);
							EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET BH=\'") + vtos(vTmp1) + _T("\'"));
							vBH = vTmp1;			
							rsTmp.GetFieldValue(_T("CustomID"),vTmp1);
							EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET CustomID=\'")+vtos(vTmp1)+_T("\'"));
							rsX.GetFieldValue(_T("A"),vTmp1);
							sTmp2.Format(_T("%g"),vtof(vTmp1));
							EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET a=") +sTmp2);
							rsX.GetFieldValue(_T("c"),vTmp1);
							sTmp2.Format(_T("%g"),vtof(vTmp1));
							EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET c=") +sTmp2);
							if( modPHScal::glClassID==iG52_55 || modPHScal::glClassID==iG51)
							{
								sTmp2.Format(_T("%g"),vtof(rsza->GetCollect(_T("A01"))));
								EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET H=") +sTmp2);
							}
							else
							{
								sTmp2.Format(_T("%g"),vtof(rsza->GetCollect(_T("A01"))));
								EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET BWidth=") +sTmp2);
							}
							sTmp2.Format(_T("%g"),vtof(rsza->GetCollect(_T("B01"))));
							EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET B=") +sTmp2);
							rsX.GetFieldValue(_T("m"),vTmp1);
							sTmp2.Format(_T("%g"),vtof(vTmp1));
							EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET m=") +sTmp2);
							rsX.GetFieldValue(_T("CHdist"),vTmp1);
							sTmp2.Format(_T("%g"),vtof(vTmp1));
							EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET CHDist=") +sTmp2);
							rsTmp.GetFieldValue(_T("sizeH"),vTmp1);
							EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET CHHeight=")+(vtos(vTmp1)==_T("") ? _T("0") : vtos(vTmp1)) );
							EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET CHLegHeight=")+(vtos(vTmp1)==_T("") ? _T("0") : vtos(vTmp1)) );
							sTmp2.Format(_T("%g"),modPHScal::gdw);
							EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET GDW1=") +sTmp2);
							sTmp2.Format(_T("%g"),modPHScal::Lspan);
							EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET Lspan=") +sTmp2);
							sTmp2.Format(_T("%g"),modPHScal::Lspan);
							EDIBgbl::dbPRJ.Execute( _T("UPDATE tmpCSLen SET [L1]= ") +sTmp2);
							sngCSLen = modPHScal::CSLength(sBHFormat, sTmp, sBH);
						}
						//sBH中含有无效值,对于型钢不如将其置为规格更有意义
						sBH = mvSAattachedCustomID;
						//型钢重量=每米重量x长度(mm)/1000
						rsTmp.GetFieldValue(_T("Weight"),vTmp1);
						sngWeight =vtof(vTmp1);
						sngW = sngWeight * sngCSLen / 1000;
					}
				}
				
				rsX.GetFieldValue(_T("Pnum")+sj,vTmp1);
				//根部整体重量=根部每个附件的重量之和kg
				mvarSATotalWeight = mvarSATotalWeight + sngW * vtof(vTmp1);
				rsSAPart.AddNew();
				
				rsSAPart.SetFieldValue(_T("VolumeID"),COleVariant(EDIBgbl::SelVlmID));
				rsSAPart.SetFieldValue(_T("zdjh"),COleVariant((long)modPHScal::zdjh));
				//2002.05.26以后改写此段代码，以与phs.arx思路一致。
				//是根部附件,则写记录号,
				//以便区分根部附件与部件.也便于对零件编号。
				iSEQofPart++;
				rsSAPart.SetFieldValue(_T("recno"),COleVariant((long)iSEQofPart));
				//因为根部附件与根部的区分标志是IsSAPart字段是否=-1。
				rsSAPart.SetFieldValue(_T("IsSAPart"),COleVariant((long)-1));
				//rsSAPart.SetFieldValue(_T("Index")) = iSA
				rsSAPart.SetFieldValue(_T("nth"),COleVariant((short)nth));//标准支吊架路数=1，包括对称双吊和共用根部双吊
				rsSAPart.SetFieldValue(_T("ClassID"),COleVariant((long)mviSASSClassID));
				rsSAPart.SetFieldValue(_T("Index"),COleVariant((long)mviSASSIndex));
				rsTmp.GetFieldValue(_T("CustomID"),vTmp1);
				rsSAPart.SetFieldValue(_T("CustomID"),STR_VAR(vtos(vTmp1)));
				//sBH				
				rsSAPart.SetFieldValue(_T("CLgg"),STR_VAR(sBH));
				if(tmpID=="F9" || tmpID=="F10" || tmpID=="F14")
				{				
					rsSAPart.SetFieldValue(_T("BH"),STR_VAR(strBH));
					//根部螺栓或螺杆的长度，
					rsSAPart.SetFieldValue(_T("L1"),COleVariant( sngCSLen));
				}
				else
				{
					
					if(mviSASSIndex==iSectionSteel )
					{
						//根部型钢的长度，
						rsSAPart.SetFieldValue(_T("L1"),COleVariant( sngCSLen));
						//rsSAPart.SetFieldValue(_T("GDW1"),COleVariant(modPHScal::gdw));
						rsSAPart.SetFieldValue(_T("BH"),STR_VAR(sBH));
					}
					else
						rsSAPart.SetFieldValue(_T("BH"),STR_VAR(sBH));
				}
				
				rsTmp.GetFieldValue(_T("Material"),vTmp1);
				
				rsSAPart.SetFieldValue(_T("CLcl"),STR_VAR(vtos(vTmp1)));
				rsX.GetFieldValue(_T("Pnum")+sj,vTmp1);
				rsSAPart.SetFieldValue(_T("CLnum"),vTmp1);
				rsSAPart.SetFieldValue(_T("CLbz"),vnil);
				rsSAPart.SetFieldValue(_T("CLdz"),COleVariant(sngWeight));
				rsX.GetFieldValue(_T("Pnum")+sj,vTmp1);
				rsSAPart.SetFieldValue(_T("CLzz"),COleVariant(sngW * vtof(vTmp1)));
				rsID.GetFieldValue(_T("Description"),vTmp1);
				rsSAPart.SetFieldValue(_T("CLmc"),vTmp1);
				rsSAPart.Update();
			 }
			 else
			 {
				 //碰到为空的附件，退出，加快速度。
				 //输入原始数据库时应该注意所有的附件从1开始填写，不要间断。
				 //由于以前输入的数据并不完全符合上面要求，故要取消退出，否则结果中有些附件不会被统计。
				 //Exit For
				 //下句造成死循环
				 //continue;
			 }
		  }
		  }
		  break;
	   case iSPR:
		   {
			   rsTZB.SetFieldValue(_T("CLcl"),STR_VAR(GetResStr(IDS_GROUPWARE)));
			   //Height_SPRINGsL5 = IIf(IsNull(rsX(_T("size3"))), 0, rsX(_T("size3")))
			   if(sPartID==_T("T4"))
			   {
				   rsX.GetFieldValue(_T("sizeH"),vTmp1);
				   sngDim = vtof(vTmp1) - /*Sgn(modPHScal::yr) */ modPHScal::sSprInfo[iSEQofSPR].haz;//z.jia add
				   //rsTZB.SetFieldValue(_T("Size")) = Height_SPRINGsL5 / 2
			   }
			   else if(sPartID==_T("T3") || sPartID==_T("T3A"))
			   {
				   rsX.GetFieldValue(_T("sizeH"),vTmp1);
				   sngDim = fabs(vtof(vTmp1)) - modPHScal::sSprInfo[iSEQofSPR].haz;
				   //上/下方弹簧，其值在计算拉杆时应该使拉杆加长
				   sngDim = -sngDim;
			   }
			   //rsTZB.SetFieldValue(_T("Size")) = Height_SPRINGsL5 / 2
			   else
			   {
				   rsX.GetFieldValue(_T("sizeH"),vTmp1);
				   sngDim = vtof(vTmp1) + modPHScal::sSprInfo[iSEQofSPR].haz;
			   }
			   //弹簧的尺寸都是按松紧螺母外缘给的。且按最小位移时给出高度。
			   //配套不同的松紧螺母时应该换算不同的高度。此种处理程序暂不提供。
			   //弹簧的松紧螺母在CalLugLength中查表
			   rsTZB.SetFieldValue(_T("SizeH"),COleVariant(sngDim));
			   rsTZB.SetFieldValue(_T("sizeDIM"),COleVariant(sngDim));
			   //rsTZB.SetFieldValue(_T("CLgg")) = sBHformat
			   if( EDIBAcad::g_bBomSprFactory )//newHS->只有选择了标注弹簧厂家才写入数据库
			   {
				   rsTZB.SetFieldValue(_T("CLbz"),STR_VAR(modPHScal::gsSPRmanufactory));
			   }
			   rsTZB.SetFieldValue(_T("SpringNo"), COleVariant( (long)modPHScal::sSprInfo[iSEQofSPR].DH) );
			   rsTZB.SetFieldValue(_T("LugDia"), COleVariant(modPHScal::sSprInfo[iSEQofSPR].FiJ));
			   rsTZB.SetFieldValue(_T("HeatDisp"), COleVariant(modPHScal::sSprInfo[iSEQofSPR].HeatDisp));
			   rsTZB.SetFieldValue(_T("HAZ"), COleVariant(modPHScal::sSprInfo[iSEQofSPR].haz));
			   
			   rsTZB.Update();
		   }
		   break;
	   case iCSPR:
		   {
			   rsTZB.SetFieldValue(_T("CLcl"),STR_VAR(GetResStr(IDS_GROUPWARE)));
			   //记录原始尺寸T，便于过程GetBoltsNutsAndAttachmentsCLgg准确计算与根部连接的螺栓长度
			   rsX.GetFieldValue(_T("T"),vTmp1);
			   rsTZB.SetFieldValue(_T("T"),COleVariant((long)vtoi(vTmp1)));
			   //荷载连接件尺寸x,y,需要的螺纹长度GL保存在恒力弹簧数据表中。
			   //这是对常州电力机械厂样本而言。该厂sizeH=E +or- fB/2。
			   //Height_SPRINGsL5 = IIf(IsNull(rsX(_T("x"))), 0, rsX(_T("x"))) * 2
			   //查表tbnCSPRINGL5Crude，找出松紧螺母的长度、可插入深度
			   //2005.11.01北京巴威公司金滔高级工程师指出，国标恒吊长度计算不对。
			   //经ligb检查，是由于恒力弹簧松紧螺母长度计算被简单地处理为随拉杆直径而变化。
			   //实际上根据样本，无论常州电力还是大连弹簧，恒力弹簧松紧螺母长度都是因恒力弹簧型号、位移值不同而变化，
			   //现在已经由ligb修改正确。2005.11.01 20:00-23:00。
			   float tmpMovement=0;//恒力弹簧查表位移tmpMovement，程序使用这个值tmpMovement去查恒力弹簧数据表。这个值一般认为是计算位移或称选型位移，但是UESoft认为它也可以是实际热位移
			   if(modPHScal::gbCalcCSPRHeight_BySelDisp)
				   tmpMovement=modPHScal::sSprInfo[iSEQofSPR].SumDisp;
			   else
				   tmpMovement=modPHScal::sSprInfo[iSEQofSPR].haz;
			   SQLx.Format(_T("SELECT * FROM [%s] WHERE trim(CustomID)=\'%s\' AND minDH<=%d AND %d<=maxDH AND fBmin<=%g AND fBmax>=%g "),
				   modPHScal::tbnCSPRINGL5Crude,modPHScal::sFindCustomID(sPartID),modPHScal::sSprInfo[iSEQofSPR].DH,modPHScal::sSprInfo[iSEQofSPR].DH,
				   tmpMovement,tmpMovement);
			   
			   rsDiaOfCSPRFiJ.GetFieldValue(_T("FiJ"),vTmp1);
			   //SQLx.Format(_T("SELECT * FROM [%s] WHERE size2=%g"),modPHScal::tbnCSPRINGL5Crude,vtof(vTmp1));
			   if(rsTmp.IsOpen())
				   rsTmp.Close();
			   rsTmp.m_pDatabase=&modPHScal::dbZDJcrude;
			   rsTmp.Open(dbOpenSnapshot,SQLx);
			   if( rsTmp.IsEOF() && rsTmp.IsBOF() )
			   {
				   //没有找到直径值
				   sTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb),modPHScal::dbZDJcrude.GetName,modPHScal::tbnCSPRINGL5Crude,SQLx);
				   throw sTmp;
			   }
			   else
			   {
				   rsTmp.GetFieldValue(_T("sizeH"),vTmp1);
				   Height_SPRINGsL5 =vtof(vTmp1);
				   //获取拉杆可插入的最大长度和最小长度
				   rsTmp.GetFieldValue(_T("size4"),vTmp1);
				   rsTZB.SetFieldValue(_T("AllowedMinLugExt"),COleVariant(vtof(vTmp1)));//保存到数据库，以便CalLugLength函数使用
				   rsTmp.GetFieldValue(_T("size5"),vTmp1);
				   rsTZB.SetFieldValue(_T("AllowedMaxLugExt"),COleVariant(vtof(vTmp1)));
			   }
			   //恒力弹簧上方连接拉杆的下端插入值，保存到ZB表的AllowedMaxLugExtUp、AllowedMinLugExtUp字段里
			   if( sPartID == _T("LHA") || sPartID == _T("LHB") || sPartID == _T("PHA")|| sPartID == _T("PHB"))
			   {
				   rsX.GetFieldValue (_T("PP"),vTmp1);//pfg2005.12.23 上端最小插入值应该为PP
				   rsTZB.SetFieldValue(_T("AllowedMaxLugExtUp"),COleVariant(vtof(vTmp1)));
				   rsTZB.SetFieldValue(_T("AllowedMinLugExtUp"),COleVariant(vtof(vTmp1)));
			   }
			   
			   //如果恒力弹簧的选型位移modPHScal::sSprInfo[iSEQofSPR].SumDisp正好等于下限位移字段fBmin的一个值，
			   //则恒力弹簧的安装高度H1(sngH1xmax)=直接查得这个下限位移字段fBmin的值。
			   //否则，按插值法计算位移。add by ligb on 2004.11.24
			   CString tmpSQL;
			   CDaoRecordset rsX1;			   
			   tmpSQL = _T("SELECT * FROM [") + modPHScal::tbnHDCrude + _T("] WHERE ") + tmpSQL ;
			   if(iSEQofSPR < modPHScal::SprInfoIndex)
				   sTmp.Format(_T(" dh=%d"),modPHScal::sSprInfo[iSEQofSPR].DH);
			   else sTmp = _T(" ");
			   tmpSQL+=sTmp;
			   if(iSEQofSPR < modPHScal::SprInfoIndex)
			   {
				   //sTmp.Format(_T(" AND fBmin<=%g AND fBmax>=%g "),modPHScal::sSprInfo[iSEQofSPR].CheckDisp,modPHScal::sSprInfo[iSEQofSPR].CheckDisp);
				   sTmp.Format(_T(" AND fBmin=%g "),modPHScal::sSprInfo[iSEQofSPR].CheckDisp);
			   }
			   else
			   {
				   sTmp=_T(" ");
			   }
			   tmpSQL+=(modPHScal::gbCSPRneedSpecialDesign ? _T(" ") : sTmp) + _T(" AND trim(CustomID)=\'") + Ptype[i] + _T("\' ORDER BY dh,Weight");
			   rsX1.m_pDatabase=&modPHScal::dbZDJcrude;
			   rsX1.Open(dbOpenSnapshot,tmpSQL);
			   if(rsX1.IsBOF() && rsX1.IsEOF()||1)//当处在临界值时出错(sizeH的高度没有加上实际位移)pfg and lgb 2005.12.15
			   {
				   //常州电力机械厂样本数据库字段sizeH存储了正确的高度值。
				   rsX.GetFieldValue(_T("sizeH"),vTmp1);
				   //rsX.GetFieldValue(_T("E"),vTmp1);
				   sngH1xmax =vtof(vTmp1);
				   if(modPHScal::gCH1distPos == 1 )
				   {
					   //E字段给出了中间位置的高度
					   if(modPHScal::gbCalcCSPRHeight_BySelDisp)
						   sngDim = sngH1xmax + Sgn(modPHScal::yr) * modPHScal::sSprInfo[iSEQofSPR].haz /(modPHScal::bE_FormulaPlusHalf?2:1);
					   else
						   sngDim = sngH1xmax + Sgn(modPHScal::yr) * modPHScal::sSprInfo[iSEQofSPR].SumDisp /(modPHScal::bE_FormulaPlusHalf?2:1);
				   }
				   else if(modPHScal::gCH1distPos == 0 )
				   {
					   //E字段给出了最小位移位置的高度
					   rsX.GetFieldValue(_T("fBmin"),vTmp1);
					   if( modPHScal::yr < 0 )
					   {
						   //热位移<0
						   //sngDim = sngH1xmax + (Sgn(modPHScal::yr) * modPHScal::sSprInfo[iSEQofSPR].haz + vtof(vTmp1)) /(modPHScal::bE_FormulaPlusHalf?2:1);
						   //研究表明，国标恒吊位移计算公式与美国恒吊计算公式相同，无论bE_FormulaPlusHalf为何，都要除2
						   //
						   sngDim = sngH1xmax + (Sgn(modPHScal::yr) * (modPHScal::gbCalcCSPRHeight_BySelDisp?modPHScal::sSprInfo[iSEQofSPR].SumDisp:modPHScal::sSprInfo[iSEQofSPR].haz) + vtof(vTmp1)) /(modPHScal::bE_FormulaPlusHalf?2:2);
					   }
					   else
					   {
						   //热位移>=0，国标恒吊H1s=H1x+热位移T
						   sngDim = sngH1xmax + (Sgn(modPHScal::yr) * (modPHScal::gbCalcCSPRHeight_BySelDisp?modPHScal::sSprInfo[iSEQofSPR].SumDisp:modPHScal::sSprInfo[iSEQofSPR].haz) + vtof(vTmp1)) /(modPHScal::bE_FormulaPlusHalf?2:2) + (sPartID == _T("ZHB") ? -1 : 0) * modPHScal::sSprInfo[iSEQofSPR].haz;
					   }
					   //按国标恒吊或华东石油局扬州装备制造总厂管架分厂的解释，以上公式可废弃,应该按下列公式
					   // sngDim = sngH1xmax + Sgn(modPHScal::yr) * modPHScal::sSprInfo[iSEQofSPR].haz /(modPHScal::bE_FormulaPlusHalf?2:2);
				   }
				   else if(modPHScal::gCH1distPos == 2 )
				   {
					   //E字段给出了最大位移位置的高度
					   rsX.GetFieldValue(_T("fBmax"),vTmp1);
					   rsX.GetFieldValue(_T("fBmin"),vTmp2);
					   sngDim = sngH1xmax + (vtof(vTmp1) - vtof(vTmp2))/2;
					   if( modPHScal::yr < 0 )
					   {
						   //热位移<0
						   sngDim = sngDim + (Sgn(modPHScal::yr) * (modPHScal::gbCalcCSPRHeight_BySelDisp?modPHScal::sSprInfo[iSEQofSPR].SumDisp:modPHScal::sSprInfo[iSEQofSPR].haz) + vtof(vTmp2)) /(modPHScal::bE_FormulaPlusHalf?2:2);
					   }else
					   {
						   //热位移>=0
						   sngDim = sngDim + (Sgn(modPHScal::yr) * (modPHScal::gbCalcCSPRHeight_BySelDisp?modPHScal::sSprInfo[iSEQofSPR].SumDisp:modPHScal::sSprInfo[iSEQofSPR].haz) + vtof(vTmp2)) /(modPHScal::bE_FormulaPlusHalf?2:2) + (sPartID == _T("ZHB")? -1:0) * modPHScal::sSprInfo[iSEQofSPR].haz;
					   }
					   //按国标恒吊或华东石油局扬州装备制造总厂管架分厂的解释，以上公式可废弃,应该按下列公式
					   //sngDim = sngH1xmax + Sgn(modPHScal::yr) * modPHScal::sSprInfo[iSEQofSPR].haz /(modPHScal::bE_FormulaPlusHalf?2:0);
				   }
				   //常州的可用下列公式
				   //rsTZB.SetFieldValue(_T("AllowedMinLugExt")) = IIf(IsNull(rsX.Fields(_T("x"))), 0, rsX.Fields(_T("x")))
				   //rsTZB.SetFieldValue(_T("AllowedMaxLugExt")) = IIf(IsNull(rsX.Fields(_T("y"))), 0, rsX.Fields(_T("y")))+rsTZB.SetFieldValue(_T("AllowedMinLugExt"))
				   if(modPHScal::gCH1Pos == 1 )
				   {
					   //H1在花篮螺丝中间位置的高度
					   sngDim = sngDim + Height_SPRINGsL5 / 2;
				   }
				   else if(modPHScal::gCH1Pos == 2 )
				   {
					   //H1在花篮螺丝最高位置的高度
					   sngDim = sngDim + Height_SPRINGsL5;
				   }
				   else if(modPHScal::gCH1Pos == 0 )
				   {
					   //H1在花篮螺丝最低位置的高度
					   
				   }
				   //如果没包括花篮螺丝外缘，加上它
				   //常州的可用下列公式
				   if( !modPHScal::gbCH1IncludeB )
				   {
					   rsTmp.GetFieldValue(_T("size4"),vTmp1);
					   sngDim = sngDim + vtof(vTmp1);
				   }
				   if(sPartID == _T("LHD") || sPartID==_T("PHD"))
				   {
				   }
				   else if(sPartID==_T("LHA") ||sPartID==_T("PHA") || sPartID==_T("LHB") || sPartID==_T("PHB"))
				   {
					   //立式吊杆连接
					   //字段A保存拉杆插入长度
					   //千万不可增加下面这句，否则拉杆尺寸计算错误，并且导致整个尺寸链错误。
					   //rsTZB.SetFieldValue(_T("A")) = IIf(IsNull(rsX(_T("PP"))), 0, rsX(_T("PP")))
				   }
				   else if(sPartID==_T("LHE") || sPartID==_T("LHE1") || sPartID==_T("PHE") || sPartID==_T("PHE1") || sPartID==_T("PHC") || sPartID==_T("LHC"))
				   {
					   //立式吊杆连接，要加上耳板高度T。
					   if( !modPHScal::gbCH1IncludeT )
					   {
						   rsDiaOfCSPRFiK.GetFieldValue(_T("T"),vTmp1);
						   sngDim = sngDim +vtof(vTmp1);
					   }
				   }
				   else if(sPartID==_T("ZHB"))
				   {
					   //支座式恒力弹簧
				   }
				   else
				   {
				   }
			   }
			  /* else
			   {
				   rsX1.GetFieldValue(_T("sizeH"),vTmp1);
				   sngH1xmax =vtof(vTmp1);
				   if(modPHScal::gCH1distPos == 1 )
				   {
					   //E字段给出了中间位置的高度
					   if(modPHScal::gbCalcCSPRHeight_BySelDisp)
						   sngDim = sngH1xmax;
					   else
						   sngDim = sngH1xmax;
				   }
				   else if(modPHScal::gCH1distPos == 0 )
				   {
					   //E字段给出了最小位移位置的高度
					   sngDim = sngH1xmax;
					   //按国标恒吊或华东石油局扬州装备制造总厂管架分厂的解释，以上公式可废弃,应该按下列公式
					   // sngDim = sngH1xmax + Sgn(modPHScal::yr) * modPHScal::sSprInfo[iSEQofSPR].haz /(modPHScal::bE_FormulaPlusHalf?2:2);
				   }
				   else if(modPHScal::gCH1distPos == 2 )
				   {
					   //E字段给出了最大位移位置的高度
					   rsX.GetFieldValue(_T("fBmax"),vTmp1);
					   rsX.GetFieldValue(_T("fBmin"),vTmp2);
					   //sngDim = sngH1xmax + (vtof(vTmp1) - vtof(vTmp2))/2;
					   sngDim = sngH1xmax;
				   }
				   //常州的可用下列公式
				   //rsTZB.SetFieldValue(_T("AllowedMinLugExt")) = IIf(IsNull(rsX.Fields(_T("x"))), 0, rsX.Fields(_T("x")))
				   //rsTZB.SetFieldValue(_T("AllowedMaxLugExt")) = IIf(IsNull(rsX.Fields(_T("y"))), 0, rsX.Fields(_T("y")))+rsTZB.SetFieldValue(_T("AllowedMinLugExt"))
				   if(modPHScal::gCH1Pos == 1 )
				   {
					   //H1在花篮螺丝中间位置的高度
					   sngDim = sngDim + Height_SPRINGsL5 / 2;
				   }
				   else if(modPHScal::gCH1Pos == 2 )
				   {
					   //H1在花篮螺丝最高位置的高度
					   sngDim = sngDim + Height_SPRINGsL5;
				   }
				   else if(modPHScal::gCH1Pos == 0 )
				   {
					   //H1在花篮螺丝最低位置的高度
					   
				   }
				   //如果没包括花篮螺丝外缘，加上它
				   //常州的可用下列公式
				   //if( !modPHScal::gbH1IncludeB ){ sngDim = sngDim + IIf(IsNull(rsX.Fields(_T("y"))), 0, rsX.Fields(_T("y")))
				   if( !modPHScal::gbCH1IncludeB )
				   {
					   rsTmp.GetFieldValue(_T("size4"),vTmp1);
					   sngDim = sngDim + vtof(vTmp1);
				   }
				   if(sPartID == _T("LHD") || sPartID==_T("PHD"))
				   {
				   }
				   else if(sPartID==_T("LHA") ||sPartID==_T("PHA") || sPartID==_T("LHB") || sPartID==_T("PHB"))
				   {
					   //立式吊杆连接
					   //字段A保存拉杆插入长度
					   //千万不可增加下面这句，否则拉杆尺寸计算错误，并且导致整个尺寸链错误。
					   //rsTZB.SetFieldValue(_T("A")) = IIf(IsNull(rsX(_T("PP"))), 0, rsX(_T("PP")))
				   }
				   else if(sPartID==_T("LHE") || sPartID==_T("LHE1") || sPartID==_T("PHE") || sPartID==_T("PHE1") || sPartID==_T("PHC") || sPartID==_T("LHC"))
				   {
					   //立式吊杆连接
					   if( !modPHScal::gbCH1IncludeT )
					   {
						   rsDiaOfCSPRFiK.GetFieldValue(_T("T"),vTmp1);
						   sngDim = sngDim +vtof(vTmp1);
					   }
				   }
				   else if(sPartID==_T("ZHB"))
				   {
					   //支座式恒力弹簧
				   }
				   else
				   {
				   }
			   }*/
			   rsX1.Close();

			   //恒吊松紧螺母长度调节值
			   rsTZB.SetFieldValue(_T("SizeH"),COleVariant(sngDim));
			   rsTZB.SetFieldValue(_T("sizeDIM"),COleVariant(sngDim));
			   if( EDIBAcad::g_bBomSprFactory )//newHS->只有选择了标注恒力弹簧厂家才写入数据库
			   {
				   rsTZB.SetFieldValue(_T("CLbz"),STR_VAR(modPHScal::gsCSPRmanufactory));
			   }
			   //暂时借用gdw1字段保存计算荷载，供后续过程GetBoltsNutsAndAttachmentsCLgg使用
			   rsTZB.SetFieldValue(_T("GDW1"),COleVariant(tmpSelPJG4CSPRFiK / iNumPart));
			   rsTZB.SetFieldValue(_T("SpringNo"), COleVariant( (long)modPHScal::sSprInfo[iSEQofSPR].DH) );
			   rsTZB.SetFieldValue(_T("LugDia"), COleVariant(modPHScal::sSprInfo[iSEQofSPR].FiJ));
			   rsTZB.SetFieldValue(_T("HeatDisp"), COleVariant(modPHScal::sSprInfo[iSEQofSPR].HeatDisp));
			   
			   rsTZB.Update();
		  }
		  break;
	default:
		//其它的零部件类型
		rsTZB.Update();
	}
	//必须在处理组件后才能处理连接附件，因为要查找附件表
	//最多8个连接附件,eg. F1/F2/...,etc.
	//最后一个组件是根部,不可能还有连接配件,
	//故附加条件i < rsTmpZB.RecordCount - 1
	//连接表储存的螺栓螺母是西北院的CustomID,或者说是ID，因此在使用其它标准查找连接螺栓时需要先进行变换。
	//变换过程在Cphs.GetphsBHandSizes中实现。
	//连接附件（螺栓螺母垫片垫板）的简单信息写入TZB表，规格编号则等计算完成后
	//在一个单独的过程CalLugLength中处理。在其中还要处理下方弹簧的编号、螺栓螺母的直径等。
	if( iAttachIncd == iAttachIncluded && i < C - 1 )
	{
		//带螺栓螺母标志为bWithBoltsNuts=False
		bWithBoltsNuts = false;
		if(modPHScal::glClassID == iCSPR )
		{
			//当前零件是恒力弹簧
			if( PtypeIndex[i + 1] == iSA )
			{
				//下一个零件是根部
				if(modPHScal::gbWorksWithSABoltsNuts )
					//厂家自带与根部连接的螺栓螺母
					bWithBoltsNuts = true;
			}
			else
			{
				//下一个零件不是根部，是连接件
				if(modPHScal::gbWorksWithBoltsNuts )
					//厂家自带与连接件连接的螺栓螺母
					bWithBoltsNuts = true;
			}
		}
		
		//如果厂家不自带螺栓螺母
		
		for (j = 1;j<= 8;j++)
		{
			CString sj;
			strClbz.Empty();   //ADD BY LFX 2005.5.20
			sj.Format(_T("%d"),j);
			rsConnect.GetFieldValue(_T("CntbF")+sj,vTmp1);
			if(vtos(vTmp1)!=_T("") )
			{
				
				
				if( rsPartBoltNuts.IsEOF() )
					continue;
				rsPartBoltNuts.Edit();
				iSEQofPart = iSEQofPart + 1;
				rsPartBoltNuts.SetFieldValue(_T("BHformat"),STR_VAR(sBHFormat));
				rsPartBoltNuts.SetFieldValue(_T("VolumeID"),COleVariant(EDIBgbl::SelVlmID));
				rsPartBoltNuts.SetFieldValue(_T("zdjh"),COleVariant((long)modPHScal::zdjh));
				rsPartBoltNuts.SetFieldValue(_T("recno"),COleVariant((long)iSEQofPart));

				//rsPartBoltNuts.SetFieldValue(_T("index"),COleVariant((long)iBoltsNuts));
				rsPartBoltNuts.SetFieldValue(_T("nth"),COleVariant((short)nth));//标准支吊架路数=1，包括对称双吊和共用根部双吊
				rsConnect.GetFieldValue(_T("CntbF")+sj+_T("num"),vTmp1);

				if( vTmp1.vt==VT_NULL)
				{
					//螺栓螺母数量=其后零件数量*1///
					rsPartBoltNuts.SetFieldValue(_T("CLnum"),COleVariant(iNumPart * iNum * iCntbNum));
				}
				else
				{
					//附件数量=实际用量,包含vtmp1值 为零的情况
					rsPartBoltNuts.SetFieldValue(_T("CLnum"),COleVariant(vtoi(vTmp1) * iNumPart * iNum * iCntbNum));
										//lfx 2005.5.20改 
					//华东院拉杆已自带螺母,因此在组装图绘制过程中的材料统计中
					//相关螺母数目为零,需要加上相应的说明
					//华东院拉杆已自带螺母,因此绘制组装图时需注"拉杆自带"字样
					if (0 == vtoi(vTmp1))
					{
						strClbz.Format("拉杆自带");
					}
					//end lfx 2005.5.20改
				}
				
				
				//必须记录ID,否则绘图不正确。
				rsConnect.GetFieldValue(_T("CntbF")+sj,vTmp1);
				rsPartBoltNuts.SetFieldValue(_T("ID"),vTmp1);
				
				if(rsID.FindFirst (_T("trim(ID)=\'") + vtos(vTmp1) + _T("\'")))
				{
					rsID.GetFieldValue(_T("Description"),vTmp1);
					rsPartBoltNuts.SetFieldValue(_T("CLmc"),vTmp1);
					rsID.GetFieldValue(_T("CustomID"),vTmp1);
					rsPartBoltNuts.SetFieldValue(_T("CustomID"),vTmp1);
					rsID.GetFieldValue(_T("ClassID"),vTmp1);
					rsPartBoltNuts.SetFieldValue(_T("ClassID"),vTmp1);
				}
				//rsPartBoltNuts.SetFieldValue(_T("CustomID")) = IIf(gbFDid, sFindCustomID(rsc.Fields(_T("CntbF") + j)), rsc.Fields(_T("CntbF") + j))
				//rsPartBoltNuts.SetFieldValue(_T("ID")) = IIf(gbFDid, rsc.Fields(_T("CntbF") + j), sFindID(rsc.Fields(_T("CntbF") + j)))
				//rsPartBoltNuts.SetFieldValue(_T("CLmc")) = sFindFLD(IIf(gbFDid, _T("ID"), _T("CustomID")), _T("Description"), IIf(gbFDid, rsc.Fields(_T("CntbF") + j), sFindCustomID(rsc.Fields(_T("CntbF") + j))))
				if( ! bWithBoltsNuts )
				{
					//厂家不提供的螺栓螺母
					//赋给任意非空的数字，便于编号
					rsPartBoltNuts.SetFieldValue(_T("SEQ"),COleVariant((long) i));

					//lfx 2005.5.20改 
					//说明见上面//lfx 2005.5.20改 注释
					//原rsPartBoltNuts.SetFieldValue(_T("CLbz"),vnil);
					rsPartBoltNuts.SetFieldValue(_T("CLbz"),_bstr_t(strClbz));  
					//End 2005.5.20改
				}
				else
				{
					rsPartBoltNuts.SetFieldValue(_T("SEQ"),vnil);
					rsPartBoltNuts.SetFieldValue(_T("CLnum"),COleVariant((long)0));
					CString str;
					str.LoadString(IDS_PRODUCEFAC_OFFERGOODS);
					rsPartBoltNuts.SetFieldValue(_T("CLbz"),STR_VAR(str));
				}
				rsPartBoltNuts.Update();
				rsPartBoltNuts.MoveNext();
				rsTZB.MoveNext();
			}
			else
			{
				//碰到为空的附件，退出，加快速度。
				//输入原始数据库时应该注意所有的附件从1开始填写，不要间断。
				break;
			}
		}
	}
	
	//NotEditData:
	if(modPHScal::glClassID == iCSPR )
	{
		iSEQofSPR = iSEQofSPR + 1;
		if( iSEQofSPR >modPHScal::giWholeSprNum )
		{
			sTmp.Format(GetResStr(IDS_iSEQofSPROvergiWholeSprNum),_T("\%d"),_T("\%d"));
			sTmp2.Format(sTmp,iSEQofSPR,modPHScal::giWholeSprNum);
			throw sTmp2;
		}
	}
	else if(modPHScal::glClassID == iSPR )
	{
		iSEQofSPR = iSEQofSPR + 1;
		if( iSEQofSPR >modPHScal::giWholeSprNum )
		{
			sTmp.Format(GetResStr(IDS_iSEQofSPROvergiWholeSprNum),_T("\%d"),_T("\%d"));
			sTmp2.Format(sTmp,iSEQofSPR,modPHScal::giWholeSprNum);
			throw sTmp2;
		}
		else
		{
			if( iSumSerialNum ==modPHScal::giSumSerialNum )
			{
				//Err.Raise iUE_SumSerialNumEQVNeedSerialNum, , ResolveResString(iUE_SumSerialNumEQVNeedSerialNum, _T("|1"), iSEQofSPR, _T("|2"), iSumSerialNum)
			}
			else
			{
				if( iSumSerialNum >modPHScal::giSumSerialNum )
				{
					sTmp.Format(GetResStr(IDS_SumSerialNumOverNeedSerialNum),_T("\%d"),_T("\%d"));
					sTmp2.Format(sTmp,iSumSerialNum,modPHScal::giSumSerialNum);
					throw sTmp2;
				}
			}
		}
	}
	//保存前一次的连接匹配查询字段信息
	//tmpFD0valuePrevious = tmpFD0value
	//tmpRelationPrevious = tmpExtLenValue
	//tmpFD1Previous = tmpExtLenFD
	mlfx = mlfx + mlfx0;
	mlRot = mlRot0;
	tmpSQL = tmpSQL0;
	//下一个恒吊计算载荷
	tmpSelPJG4CSPRFiJ = tmpSelPJG4CSPRFiK;
	if( tmpSelPJG4CSPRPgzNext != 0 ) tmpSelPJG4CSPRPgz = tmpSelPJG4CSPRPgzNext;
	//下一个弹簧计算载荷
	if( tmpSelPJG4SPRPgzNext != 0 )  tmpSelPJG4SPRPgz  = tmpSelPJG4SPRPgzNext;
	if( tmpSelPJG4SPRPazNext != 0 )  tmpSelPJG4SPRPaz  = tmpSelPJG4SPRPazNext;
	//零件数递增
	iSEQofPart = iSEQofPart + 1;
	//保存前一个零件的类型号，方便判别
	mviPreviousClassID =modPHScal::glClassID;
	rsTmpZB.MoveNext();
	rsTZB.MoveNext();
	}//外循环结束
	
	//判断弹簧是否计算成功
	if(modPHScal::giWholeSprNum > 0 && bSPRphs )
	{
		if( iSumSerialNum >modPHScal::giSumSerialNum )
		{
			sTmp.Format(GetResStr(IDS_SumSerialNumOverNeedSerialNum),_T("\%d"),_T("\%d"));
			sTmp2.Format(sTmp,iSumSerialNum,modPHScal::giSumSerialNum);
			throw sTmp2;
		}
		else
		{
			if( iSumSerialNum <modPHScal::giSumSerialNum ){
				sTmp.Format(GetResStr(IDS_SumSerialNumLitterNeedSerialNum),_T("\%d"),_T("\%d"));
				sTmp2.Format(sTmp,iSumSerialNum,modPHScal::giSumSerialNum);
				throw sTmp2;
			}
		}
	}
	ret = true;
	
	
	//螺栓螺母规格查找已经改到ModPhsCal模块的CalBoltsNutsAndAttachmentsCLgg过程处理。
	//根部附件的规格已经在处理根部附件1~9时完成。
	//更新根部整体重量,最后一个recno不为空的记录就是根部记录,
	//但是,要排除根部是混凝土梁柱的情况
	
	//if(rsTZB.FindFirst(_T("IsNull(recno) AND (bUserAdd is null OR bUserAdd=0)")))
	//2002.05.25以后所有零件都写记录号，故改写此段程序。LEE Guobin
	if(rsTZB.FindFirst(_T(" (bUserAdd is null OR bUserAdd=0) AND Index=") + ltos(iSA)))
	{	
		rsTZB.Edit();
		if( rsTZB.IsBOF() )			
		{
			sTmp.Format(GetResStr(IDS_BOForEOFofmvarRsTZB),  _T("IsBOF()"));
			throw sTmp;
		}
		if( rsTZB.IsEOF() )			
		{
			sTmp.Format(GetResStr(IDS_BOForEOFofmvarRsTZB),  _T("IsEOF()"));
			throw sTmp;
		}
	}
	//写入seq字段,方便以后给零件编号,
	//这样处理之后,在GetphsBOMandCrd中给有效零件编号时,
	//只要获得所有零件seq<>Null的记录,
	//即可获得所有要输出到AutoCAD零件明细表的记录,
	//这就完全排除了对混凝土梁柱的无效材料统计
	rsTZB.GetFieldValue(_T("CustomID"),vTmp1);
	
	if( !rsID.FindFirst(_T("trim(CustomID)=\'") + vtos(vTmp1) + _T("\'"))){
		sTmp.Format(GetResStr(IDS_NoRecordFLDEqvThisValueInPictureClipData),  _T("CustomID"), vtos(vTmp1));
		throw sTmp;
	}
	else
	{
		rsTZB.Edit ();
		rsID.GetFieldValue(_T("ClassID"),vTmp1);
		if(  vtoi(vTmp1)== iGCement )
		{
			rsTZB.SetFieldValue(_T("SEQ"),vnil);
			rsTZB.SetFieldValue(_T("CLdz"),COleVariant((long)0));
			rsTZB.SetFieldValue(_T("CLzz"),COleVariant((long)0));
			mvarSATotalWeight = 0;
		}
		else
		{
			//型钢根部,赋给一非空的任意值
			rsTZB.SetFieldValue(_T("sizeDIM"),COleVariant(SACSHeight));
			rsTZB.GetFieldValue(_T("index"),vTmp1);
			vTmp2=rsza->GetCollect(_T("bNotSumSA"));
			if( vtoi(vTmp1)== iSA && !vtob(vTmp2)){
				//根部材料要统计
				rsTZB.SetFieldValue(_T("SEQ"),COleVariant((long)i));
				rsTZB.SetFieldValue(_T("CLdz"),COleVariant(mvarSATotalWeight));
				rsTZB.GetFieldValue(_T("CLNum"),vTmp1);
				rsTZB.SetFieldValue(_T("CLzz"),COleVariant(mvarSATotalWeight * vtof(vTmp1)));
			}
			else
			{
				//根部材料不要统计
				//不赋给根部编号，防止其写入零件明细表
				rsTZB.SetFieldValue(_T("SEQ"),vnil);
				rsTZB.SetFieldValue(_T("CLNum"),COleVariant((long)0));
				rsTZB.SetFieldValue(_T("CLdz"),COleVariant((long)0));
				rsTZB.SetFieldValue(_T("CLzz"),COleVariant((long)0));
			}
		}
	}
	rsTZB.Update();
	
	}
	
	}
	if( modPHScal::g_bPipeClampBoltDiameterChanged && FirstCal ==1 && modPHScal::giWholeSprNum ==0 )
		//小荷载大管径管夹按荷载选择，螺栓的直径M1变小，管夹规格中必须注明此M1值
	{
		CString strSQLTmp ;
		//管夹螺栓M1与U形耳子L8的螺栓M功能相似
		strSQLTmp.Format("SELECT * FROM [%s] WHERE [CUSTOMID]=\'%s\' AND [SIZE2]=%d ORDER BY SIZE2",
			modPHScal::tbnPART,modPHScal::sFindCustomID (_T("L8")),int(modPHScal::gmiDiameter));
		CDaoRecordset m_rs;
		COleVariant vT1;
		if(m_rs.IsOpen())
			m_rs.Close();
		
		m_rs.m_pDatabase=&modPHScal::dbZDJcrude;
		m_rs.Open(dbOpenDynaset,strSQLTmp);
		CString str;
		int x=0;
		if((!m_rs.IsBOF()) && !(m_rs.IsEOF()))
		{
			m_rs.GetFieldValue(_T("size3"),vT1);
			x = vtoi(vT1);
		}
		m_rs.Close ();
		
		strSQLTmp.Format(" [NTH]=1 AND recno=1 ");
		rsTZB.MoveFirst ();
		if(rsTZB.FindFirst(strSQLTmp))
		{
			rsTZB.Edit();
			rsTZB.GetFieldValue(_T("CLgg"),vT1);
			CString strTmp=vtos(vT1);			
		   //如果管夹螺栓的M1的值比需要的L8的螺栓M大，			   
		   if(m_fDiaM1 > x)
		   {
				strSQLTmp.Format("SELECT * FROM [%s] WHERE [CUSTOMID]=\'%s\' AND [SIZE2]=%d ORDER BY SIZE2",
					modPHScal::tbnPART,modPHScal::sFindCustomID (_T("L7")),int(modPHScal::gmiDiameter));
				m_rs.Close();
				m_rs.Open(dbOpenSnapshot,strSQLTmp);
				m_rs.GetFieldValue(_T("size3"),vT1);
				int y = vtoi(vT1);
				if(m_fDiaM1 > y)
				{
					//同时也比L7的内环直径D大,
					//则将管夹螺栓换小
					strSQLTmp.Format(", M1=%d ",x);
					strTmp = strTmp + strSQLTmp;
				}
				else
					modPHScal::g_bPipeClampBoltDiameterChanged = FALSE;
		   }
		   else
			   modPHScal::g_bPipeClampBoltDiameterChanged = FALSE;
			
			rsTZB.SetFieldValue(_T("CLgg"),STR_VAR(strTmp));
			rsTZB.Update();
		}
		
	}
	
	}
	
	catch(CString e)
	{
		ret=false;
		
		//如果下面的部分判断语句不注释掉，模块编译后超过64K而拒绝编译
		//仅在第二次（正式）计算时报告错误
		if(FirstCal == MaxCalCount)
			ShowMessage(e);
		
	}
	catch(::CDaoException * e)
	{
		ret=false;
		if(FirstCal == MaxCalCount)
			e->ReportError();
		e->Delete();
	}
	catch(_com_error e)
	{
		ret=false;
		if(FirstCal == MaxCalCount)
			//frmStatus.MessageBox(e.Description());
			ShowMessage(e.Description());
	}
	catch(CException *e)
	{
		e->Delete();
		ret=false;
	}
	catch(...)
	{
		ret=false;
	}

	if(rsTZB.IsOpen())
		rsTZB.Close();

	if(Ptype)
		delete [] Ptype;   //不含附件的组件结构CustomID数组
	if(PtypeID)
		delete [] PtypeID;   //不含附件的组件结构ID数组
	if(PtypeClassID)
		delete [] PtypeClassID;   //不含附件的组件结构ClassID数组
	if(PtypeIndex)
		delete [] PtypeIndex;
	FrmTxsr.UpdateData(false);
	return ret;
}
#pragma warning( disable : 4129 )  //Added by Shuli Luo
void Cphs::GetBoltsNutsAndAttachmentsCLgg(int nth )
{
	//目的：查找附件如螺栓螺母的编号，对于下方弹簧等的规格编号重新整理，使其螺母直径正确。
	//输入：RsTZB，零部件结构表。\
	
	CString *Ptype=NULL;   //不含附件的组件结构ID数组
	CString *PtypeCustomID=NULL;   //不含附件的组件结构CustomID数组
	long *PtypeClassID=NULL;   //不含附件的组件结构ClassID数组
	long *PtypeIndex=NULL;  //不含附件的组件结构Index数组
	bool *AttachmentGroupHavingF4=NULL;   //附件组中含有垫片F4，用于确定螺栓长度裕量。最大元素与NewPtype()数组相同。
	bool bFoundF4;        //附件组中找到F4。
	long *iRecNo=NULL;    //不含附件的组件结构记录号数组，保存了组件的序号。
	CString* NewPtypeCustomID=NULL;   //包含附件的组件结构CustomID数组
	CString* NewPtypeID=NULL;   //包含附件的组件结构ID数组
	long *NewPtypeClassID=NULL;   //包含附件的组件结构ClassID数组
	long *NewPtypeIndex=NULL;   //包含附件的组件结构Index数组
	long *iRecNoOfLugDia=NULL;    //数组：保存拉杆直径的记录号。用于查找连接附件的规格。
	long *iLugDia=NULL;           //数组：保存拉杆直径。用于查找连接附件的规格。
	//	 long iLugNo ;               //保存当前拉杆序号：第一根拉杆为iLugNo=1，第二根拉杆为iLugNo=2。
	long *iBoltsNutsDia=0;      //数组：保存螺栓螺母直径，索引号=当前记录号，因此是一个稀疏数组。用于查找连接附件的规格。
	CString tbn1;
	CDaoRecordset rsX(&modPHScal::dbZDJcrude), rsTmp, rs(&modPHScal::dbZDJcrude);
	CDaoRecordset rsTmpZB(&EDIBgbl::dbPRJDB);
	CString sBH ; //编号规格
	long miSEQofCSPR=0;    //第n个恒吊
	bool bCalBoltsDiaOfCSPR=0;  //发现需要重新计算螺栓螺母直径的恒吊
	float tmpSelPJG=0;    //临时计算荷载，由GetphsBHandSizes过程保存在恒吊的GDW1字段
	long LenBolts=0;  //螺栓长度
	bool bFoundG12G13=0;   //是否找到G12或G13连接件。为了使得标高都标注在槽钢梁的上方，这两种连接件必须计算根部主槽钢梁的高度，才能正确计算拉杆长度。
	long tmpT=0;  //恒吊原始数据表中的尺寸T,用于精确计算与根部连接的螺栓长度，由函数GetphsBHandSizes写入表TZB之T字段。
	COleVariant vTmp;
	//保存当前支吊架组合结构内容（不含附件，便于绘图）
	long PtypeCount=0;
	try
	{
		long i=0, k=0, n=0, IC=0;
		long lngLastPartNo=0;  //记录整个支吊架的组件数（不含附件）
		long lngCurrentPartNo=0 ; //记录当前组件（不含附件）的序号,起始号为0
		long lngAttachedPartNo=0 ;   //记录当前组件与前一个组件之间的附件数,起始号为0
		long tmpll=iROD;
		modPHScal::giNumRod = GetPhsOneClassPartNumAndPartInfo(tmpll, modPHScal::iSelSampleID, Ptype, PtypeClassID, PtypeIndex,PtypeCount);
		lngLastPartNo = PtypeCount-1;
		AttachmentGroupHavingF4=new bool[lngLastPartNo+1];
		CString SQLx;
		COleVariant vTmp1,vTmp2;
		CString strTmp;
		//此处不可使用mvarRsTZB对象代替rsTmpZB,否则零件明细表出现双份组件错误。
		SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] WHERE zdjh=") + ltos(modPHScal::zdjh) + _T(" AND VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" AND nth = ") + ltos(nth) + _T(" AND IsSAPart<>-1 ORDER BY recno");
		rsTmpZB.m_pDatabase=&EDIBgbl::dbPRJDB;
		rsTmpZB.Open(dbOpenDynaset,SQLx);
		if( rsTmpZB.IsEOF() && rsTmpZB.IsBOF() ){
			strTmp.Format(GetResStr(IDS_NoThisZDJHResultInTBNSelPrjspecTZB),  EDIBgbl::dbPRJDB.GetName(), EDIBgbl::Btype[EDIBgbl::TZB], SQLx);
			throw strTmp;
		}
		//获得所有零件（包括附件）
		rsTmpZB.MoveLast();
		rsTmpZB.MoveFirst();
		lngCurrentPartNo = -1;
		IC = rsTmpZB.GetRecordCount();
		NewPtypeCustomID=new CString[IC];
		PtypeCustomID=new CString[IC];
		NewPtypeID=new CString[IC];
		NewPtypeClassID=new long[IC];
		NewPtypeIndex=new long[IC];
		iBoltsNutsDia=new long[IC];
		iRecNo=new long[IC];
		iLugDia=new long[IC];
		iRecNoOfLugDia=new long[IC];
		for( i = 0;i< IC;i++)
		{
			//this->rsID.FindFirst _T("trim(CustomID)=//") + RsTZB.Fields(_T("CustomID")) + _T("//")
			rsTmpZB.GetFieldValue(_T("CustomID"),vTmp1);
			NewPtypeCustomID[i] = vtos(vTmp1);
			rsTmpZB.GetFieldValue(_T("ID"),vTmp1);
			NewPtypeID[i] = vtos(vTmp1);
			rsTmpZB.GetFieldValue(_T("ClassID"),vTmp1);
			NewPtypeClassID[i] = vtoi(vTmp1);
			rsTmpZB.GetFieldValue(_T("index"),vTmp1);
			NewPtypeIndex[i] = vtoi(vTmp1);
			if( NewPtypeClassID[i] == iROD ){
				//拉杆
				rsTmpZB.GetFieldValue(_T("recno"),vTmp1);
				rsTmpZB.GetFieldValue(_T("sizeC"),vTmp2);
				iLugDia[vtoi(vTmp1)] =vtoi(vTmp2); //拉杆直径保存在sizeC字段
				//iRecNoOfLugDia(iLugNo) = RsTZB.Fields(_T("recno"))   //起始号为1
				lngCurrentPartNo++;
				//ReDim Preserve iRecNo(lngCurrentPartNo)
				//ReDim Preserve PtypeCustomID(lngCurrentPartNo)
				PtypeCustomID[lngCurrentPartNo] =NewPtypeCustomID[i];
				rsTmpZB.GetFieldValue(_T("recno"),vTmp1);
				iRecNo[lngCurrentPartNo] =vtoi(vTmp1);
				//Debug.Print rsTmpZB.Fields(_T("recno")), iLugDia(i + 1), lngCurrentPartNo, iRecNo(lngCurrentPartNo)
			}
			else
			{
				//不是拉杆
				if( NewPtypeClassID[i] == iBolts || NewPtypeClassID[i] == iNuts || NewPtypeClassID[i] == iAttached )
				{
					//不是拉杆，是螺栓螺母或附件
					lngAttachedPartNo ++;
					if( NewPtypeID[i] == _T("F4") ){
						//发现球面垫圈
						bFoundF4 = true;
						AttachmentGroupHavingF4[lngCurrentPartNo] = true;
					}
				}
				else
				{
					//不是拉杆，不是螺栓螺母或附件
					//是恒吊，记录其T字段值，用于后面准确计算与根部连接的螺栓长度
					if( NewPtypeClassID[i] == iCSPR )
					{
						rsTmpZB.GetFieldValue(_T("T"),vTmp);
						tmpT = vtoi(vTmp);
					}
					lngCurrentPartNo ++;
					//ReDim Preserve iRecNo(lngCurrentPartNo)
					//ReDim Preserve PtypeCustomID(lngCurrentPartNo)
					PtypeCustomID[lngCurrentPartNo]= NewPtypeCustomID[i];
					rsTmpZB.GetFieldValue(_T("recno"),vTmp);
					iRecNo[lngCurrentPartNo] = vtoi(vTmp);
					//复位附件序号和球面垫圈发现标志
					lngAttachedPartNo = -1;
					bFoundF4 = false;
					//发现G12或G13
					if( NewPtypeID[i] == _T("G12") || NewPtypeID[i] == _T("G13") )
						bFoundG12G13 = true;
				}
			}
			rsTmpZB.MoveNext();
		}
		
		//正式开始
		rsTmpZB.MoveFirst();
		lngCurrentPartNo = -1;
		n = 0;
		miSEQofCSPR = -1;
		for (i = 0 ;i< IC;i++)
		{
			//找出恒力弹簧的CustomID,便于查表
			
			rsID.FindFirst (_T("trim(ID)=\'") + NewPtypeID[i] + _T("\'"));
			//初始设置重新计算恒吊直径为假：bCalBoltsDiaOfCSPR=False
			
			bCalBoltsDiaOfCSPR = false;
			if( NewPtypeClassID[i] == iROD )
			{
				//拉杆
				lngCurrentPartNo++;
				lngAttachedPartNo = -1;
				//找出拉杆两端的长度裕量
				if( PtypeIndex[lngCurrentPartNo - 1] == iPA || Ptype[lngCurrentPartNo - 1] == _T("T3A") )
				{
					//前一个组件是管部或下方弹簧
					//按照华东院支吊架手册的螺栓长度裕量计算准则计算拉杆长度裕量。
					rsTmpZB.GetFieldValue(_T("sizeC"),vTmp);
					SQLx = _T("SELECT * FROM [") + modPHScal::tbnBoltsSurplusLength + _T("] WHERE Diameter=") + vtos(vTmp);
					rsTmp.m_pDatabase=&modPHScal::dbZDJcrude;
					rsTmp.Open(dbOpenSnapshot,SQLx );
					if( rsTmp.IsEOF() && rsTmp.IsBOF() )
					{
						//没找到螺栓直径，可能数据表数据不全。
						strTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb),modPHScal::dbZDJcrude.GetName(),modPHScal::tbnBoltsSurplusLength,SQLx);
						throw strTmp;
					}
					rsTmpZB.Edit();
					if( AttachmentGroupHavingF4[lngCurrentPartNo] )
					{
						//附件组含球面垫圈(F4)
						rsTmp.GetFieldValue(_T("SurplusLengthWithF1F2F4"),vTmp);
					}
					else
					{
						rsTmp.GetFieldValue(_T("SurplusLengthWithF1F2"),vTmp);
					}
					rsTmpZB.SetFieldValue(_T("A"),COleVariant(vtof(vTmp)));
					//同时将拉杆伸出段最小长度保存在字段C，便于计算拉杆长度函数CalLugLength处理
					rsTmpZB.SetFieldValue(_T("C"),COleVariant(vtof(vTmp)));
					rsTmpZB.Update();
					rsTmp.Close();
				}
				/*
				else
				{
				//前一个组件不是管部和下方弹簧
				//暂时不处理，放到函数CalLugLength中处理
				//如果是T5
				//LHG不必处理，因为函数getphsBHandSizes已经将LHG处理得很完善了            
				}
				*/
				if( PtypeIndex[lngCurrentPartNo + 1] == iSA || Ptype[lngCurrentPartNo + 1] == _T("T3") )
				{
					//后一个组件是根部或上方弹簧
					//按照华东院支吊架手册的螺栓长度裕量计算准则计算拉杆长度裕量。
					rsTmpZB.GetFieldValue(_T("sizeC"),vTmp);
					SQLx = _T("SELECT * FROM [") + modPHScal::tbnBoltsSurplusLength + _T("] WHERE Diameter=") +vtos(vTmp);
					rsTmp.m_pDatabase=&modPHScal::dbZDJcrude;
					rsTmp.Open(dbOpenSnapshot,SQLx);
					if( rsTmp.IsEOF() && rsTmp.IsBOF() ){
						//没找到螺栓直径，可能数据表数据不全。
						strTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb),modPHScal::dbZDJcrude.GetName(),modPHScal::tbnBoltsSurplusLength,SQLx);
						throw strTmp;
					}
					rsTmpZB.Edit();
					if( AttachmentGroupHavingF4[lngCurrentPartNo]){
						//附件组含球面垫圈(F4)
						rsTmp.GetFieldValue(_T("SurplusLengthWithF1F2F4"),vTmp);
					}
					else{
						rsTmp.GetFieldValue(_T("SurplusLengthWithF1F2"),vTmp);
					}
					rsTmpZB.SetFieldValue(_T("B"),COleVariant(vtof(vTmp)));
					//同时将拉杆伸出段最小长度保存在字段C，便于计算拉杆长度函数CalLugLength处理
					rsTmpZB.SetFieldValue(_T("C"),COleVariant(vtof(vTmp)));
					rsTmpZB.Update();
					rsTmp.Close();
				}
				/*
				else
				{
				//前一个组件不是根部和上方弹簧
				//暂时不处理，放到函数CalLugLength中处理
				}
				*/
			}
			else
			{
				//不是拉杆
				if( NewPtypeClassID[i] == iBolts || NewPtypeClassID[i] == iNuts || NewPtypeClassID[i] == iAttached ){
					//当前零件不是拉杆，是螺栓螺母或附件
					lngAttachedPartNo ++;
					if( PtypeIndex[lngCurrentPartNo + 1] == iSA )
					{
						//当前零件不是拉杆，是螺栓螺母或附件,后一个组件是根部
						if( PtypeClassID[lngCurrentPartNo] == iROD )
						{
							//当前零件不是拉杆，是螺栓螺母或附件,后一个组件是根部，前一个组件是拉杆
							//如根部G11,G21~G56
							iBoltsNutsDia[i] = iLugDia[iRecNo[lngCurrentPartNo]];
						}
						else
						{
							//当前零件不是拉杆，是螺栓螺母或附件,后一个组件是根部，前一个组件不是拉杆
							//某些直接生根于根部的恒吊属于这种情况
							if( PtypeClassID[lngCurrentPartNo] == iCSPR )
							{  //miSEQofCSPR
								//当前零件不是拉杆，是螺栓螺母或附件,后一个组件是根部，前一个组件不是拉杆,前一个组件是恒吊
								//螺栓螺母直径取前一个拉杆的直径
								iBoltsNutsDia[i] = iLugDia[iRecNo[lngCurrentPartNo]];
								//后一个组件是根部，前一个组件不是拉杆,前一个组件是恒吊,当前附件是六角头螺栓或双头螺杆
								//Case _T("LHA"), _T("PHA"), _T("LHB"), _T("PHB")
								//这两种恒吊用螺纹拉杆向上连接到根部梁上
								rsTmp.m_pDatabase=&modPHScal::dbZDJcrude;
								if(Ptype[lngCurrentPartNo]==_T("LHE")
									|| Ptype[lngCurrentPartNo]==_T("LHE1")
									|| Ptype[lngCurrentPartNo]==_T("PHE")
									|| Ptype[lngCurrentPartNo]==_T("PHE1")
									|| Ptype[lngCurrentPartNo]==_T("PHC")
									|| Ptype[lngCurrentPartNo]==_T("LHC"))
								{
									//这两种恒吊需要六角头螺栓或双头螺栓连接到单眼吊板上
									bCalBoltsDiaOfCSPR = true;
									//if( NewPtypeID[i] = _T("F14") Or NewPtypeID[i] = _T("F10") ){
									//查恒吊松紧螺母直径FiJ、FiK、S、I、T。
									//常州电力机械厂样本p20 C型恒吊的T值应该该为I值，才与前面的LHE、PHE(1<=DH<=83)的尺寸吻合。
									//编程才方便。
									//不再使用荷载查找，因为经过3次GetphsBHandSizes函数，FiJ、FiK已经获得
									//SQLx = _T("SELECT * FROM [") + tbnLugDiaOfCSPR + _T("] WHERE  PmaxKgf>=") + tmpSelPJG + _T(" AND FiJ>=") + PtypeDiameter(lngCurrentPartNo) + _T(" AND minDH<=") + sSprInfo(miSEQofCSPR).DH + _T(" AND maxDH>=") + sSprInfo(miSEQofCSPR).DH + _T(" AND trim(CustomID)=//") + PtypeCustomID(lngCurrentPartNo) + _T("// ORDER BY Pmax")
									SQLx = _T("SELECT * FROM [") + modPHScal::tbnLugDiaOfCSPR + _T("] WHERE FiJ=") + ltos(modPHScal::PtypeDiameter[lngCurrentPartNo]) + _T(" AND minDH<=") + ltos(modPHScal::sSprInfo[miSEQofCSPR].DH) + _T(" AND maxDH>=") + ltos(modPHScal::sSprInfo[miSEQofCSPR].DH) + _T(" AND trim(CustomID)=\'") + PtypeCustomID[lngCurrentPartNo] + _T("\' ");
									rsTmp.Open(dbOpenSnapshot,SQLx);
									if( rsTmp.IsEOF() && rsTmp.IsBOF() )
									{
										//没找到螺栓直径，可能数据表数据不全。
										rsTmp.Close();
										if( !modPHScal::gbAutoApplyCSPRLugDia )
										{
											//自动调整恒力弹簧拉杆直径
											SQLx = _T("SELECT * FROM [") + modPHScal::tbnLugDiaOfCSPR + _T("] WHERE minDH<=") + ltos(modPHScal::sSprInfo[miSEQofCSPR].DH) + _T(" AND maxDH>=") + ltos(modPHScal::sSprInfo[miSEQofCSPR].DH) + _T(" AND trim(CustomID)=\'") + PtypeCustomID[lngCurrentPartNo] + _T("\' ");
											rsTmp.Open(dbOpenSnapshot,SQLx);
											if( rsTmp.IsEOF() && rsTmp.IsBOF() ){
												//没找到螺栓直径，可能数据表数据不全。
												strTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb), modPHScal::dbZDJcrude.GetName(), modPHScal::tbnBoltsSurplusLength, SQLx);
												throw strTmp;
											}
										}
										else
										{
											strTmp.Format(GetResStr(IDS_NotClearCheckAutoApplyCSPRLugDia),modPHScal::dbZDJcrude.GetName(),modPHScal::tbnBoltsSurplusLength,SQLx);
											throw strTmp;
										}
									}
									//if( True ){
									//获得螺栓孔径FiK
									rsTmp.GetFieldValue(_T("FiK"),vTmp);
									iBoltsNutsDia[i] =vtoi(vTmp);
									//查找螺栓直径系列表tbnCSPRDiameterSerial,获得准确直径
									SQLx = _T("SELECT * FROM [") + modPHScal::tbnCSPRDiameterSerial + _T("] WHERE Diameter<") + ltos(iBoltsNutsDia[i]) + _T(" ORDER BY Diameter DESC");
									rs.Open(dbOpenSnapshot,SQLx );
									if( rs.IsEOF() && rs.IsBOF() ){
										//没有找到匹配直径系列值
										strTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb), modPHScal::dbZDJcrude.GetName(),modPHScal::tbnBoltsSurplusLength, SQLx);
										throw strTmp;
									}
									rs.GetFieldValue(_T("Diameter"),vTmp);
									iBoltsNutsDia[i] =vtoi(vTmp);
									rsTmp.GetFieldValue(_T("S"),vTmp1);
									rsTmp.GetFieldValue(_T("I"),vTmp2);
									LenBolts = vtoi(vTmp1) + 2 * vtoi(vTmp2);
									rsTmp.Close();
									//按照华东院支吊架手册的螺栓长度裕量计算准则计算螺栓长度。
									SQLx = _T("SELECT * FROM [") + modPHScal::tbnBoltsSurplusLength + _T("] WHERE Diameter=") + ltos(iBoltsNutsDia[i]) + _T(" ORDER BY Diameter");
									rsTmp.Open(dbOpenSnapshot,SQLx);
									if( rsTmp.IsEOF() && rsTmp.IsBOF() )
									{
										//没找到螺栓直径，可能数据表数据不全。
										strTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb),modPHScal::dbZDJcrude.GetName(),modPHScal::tbnBoltsSurplusLength, SQLx);
										throw strTmp;
									}
									if( AttachmentGroupHavingF4[lngCurrentPartNo] )
									{
										//附件组含垫片(F4)
										rsTmp.GetFieldValue(_T("SurplusLengthWithF1F2F4"),vTmp);
										if( NewPtypeID[i] == _T("F10") )
										{    //双头螺栓
											LenBolts+=2 * vtof(vTmp);
										}
										else if( NewPtypeID[i] == _T("F9") || NewPtypeID[i] == _T("F14") )
										{//单头螺栓/六角头螺栓
											LenBolts +=vtof(vTmp);
										}
									}
									else
									{
										rsTmp.GetFieldValue(_T("SurplusLengthWithF1F2"),vTmp);
										if( NewPtypeID[i] == _T("F10") )
										{    //双头螺栓
											LenBolts += 2 * vtof(vTmp);
										}
										else if( NewPtypeID[i] == _T("F9") || NewPtypeID[i] == _T("F14") )
										{     //单头螺栓/六角头螺栓
											LenBolts +=vtof(vTmp);
										}
									}
									rsTmp.Close();
									rs.Close();
								}
								//}else{
								//不是六角头螺栓或双头螺杆
								//}
								else if(Ptype[lngCurrentPartNo]==_T("ZHA") 
									|| Ptype[lngCurrentPartNo]== _T("LHD") 
									|| Ptype[lngCurrentPartNo]== _T("LHD1") 
									|| Ptype[lngCurrentPartNo]== _T("ZHB") 
									|| Ptype[lngCurrentPartNo]== _T("PHD") 
									|| Ptype[lngCurrentPartNo]== _T("PHD1"))
								{
									//这些恒吊都直接连接在根部梁上
									bCalBoltsDiaOfCSPR = true;
									//if( NewPtypeID[i] = _T("F14") Or NewPtypeID[i] = _T("F10") ){
									SQLx = _T("SELECT * FROM [") + modPHScal::tbnLugDiaOfCSPR + _T("] WHERE FiJ=") + ltos(modPHScal::PtypeDiameter[lngCurrentPartNo])  + _T(" AND minDH<=") + ltos(modPHScal::sSprInfo[miSEQofCSPR].DH) + _T(" AND maxDH>=") + ltos(modPHScal::sSprInfo[miSEQofCSPR].DH)+ _T(" AND trim(CustomID)=\'") + PtypeCustomID[lngCurrentPartNo] + _T("\' ");
									rsTmp.Open(dbOpenSnapshot,SQLx);
									if( rsTmp.IsEOF() && rsTmp.IsBOF() ){
										//没找到螺栓直径，可能数据表数据不全。
										rsTmp.Close();
										if( ! modPHScal::gbAutoApplyCSPRLugDia )
										{
											//自动调整恒力弹簧拉杆直径
											SQLx = _T("SELECT * FROM [") + modPHScal::tbnLugDiaOfCSPR + _T("] WHERE minDH<=") + ltos(modPHScal::sSprInfo[miSEQofCSPR].DH) + _T(" AND maxDH>=") + ltos(modPHScal::sSprInfo[miSEQofCSPR].DH) + _T(" AND trim(CustomID)=\'") + PtypeCustomID[lngCurrentPartNo] + _T("\' ");
											rsTmp.Open(dbOpenSnapshot,SQLx);
											if( rsTmp.IsEOF() && rsTmp.IsBOF() )
											{
												//没找到螺栓直径，可能数据表数据不全。
												strTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb),modPHScal::dbZDJcrude.GetName(), modPHScal::tbnBoltsSurplusLength,  SQLx);
												throw strTmp;
											}
										}
										else
										{
											strTmp.Format(GetResStr(IDS_NotClearCheckAutoApplyCSPRLugDia),modPHScal::dbZDJcrude.GetName(),modPHScal::tbnBoltsSurplusLength, SQLx);
											throw strTmp;
										}
									}
									//获得螺栓孔径FiK
									rsTmp.GetFieldValue(_T("FiK"),vTmp);
									rsTmp.Close();
									iBoltsNutsDia[i] = vtoi(vTmp);
									//查找螺栓直径系列表tbnCSPRDiameterSerial,获得准确直径
									SQLx = _T("SELECT * FROM [") + modPHScal::tbnCSPRDiameterSerial + _T("] WHERE Diameter<") + ltos(iBoltsNutsDia[i]) + _T(" ORDER BY Diameter DESC");
									rs.Open(dbOpenSnapshot,SQLx);
									if( rs.IsEOF() && rs.IsBOF() )
									{
										//没有找到匹配直径系列值
										strTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb), modPHScal::dbZDJcrude.GetName(), modPHScal::tbnBoltsSurplusLength, SQLx);
										throw strTmp;
									}
									rs.GetFieldValue(_T("Diameter"),vTmp);
									iBoltsNutsDia[i] = vtoi(vTmp);
									LenBolts =SACSHeight + tmpT;
									//按照华东院支吊架手册的螺栓长度裕量计算准则计算螺栓长度。
									SQLx = _T("SELECT * FROM [") + modPHScal::tbnBoltsSurplusLength + _T("] WHERE Diameter=") + ltos(iBoltsNutsDia[i]) + _T(" ORDER BY Diameter");
									rsTmp.Open(dbOpenSnapshot,SQLx);
									if( rsTmp.IsEOF() && rsTmp.IsBOF() )
									{
										//没找到螺栓直径，可能数据表数据不全。
										strTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb), modPHScal::dbZDJcrude.GetName(),modPHScal:: tbnBoltsSurplusLength, SQLx); 
										throw strTmp;
									}
									if( AttachmentGroupHavingF4[lngCurrentPartNo] ){
										//附件组含垫片(F4)
										rsTmp.GetFieldValue(_T("SurplusLengthWithF1F2F4"),vTmp);
										if( NewPtypeID[i] == _T("F10") ){    //双头螺栓
											LenBolts += 2 * vtof(vTmp);
										}else if( NewPtypeID[i] == _T("F9") || NewPtypeID[i] == _T("F14") ){     //单头螺栓/六角头螺栓
											LenBolts +=vtof(vTmp);
										}
									}
									else{
										rsTmp.GetFieldValue(_T("SurplusLengthWithF1F2"),vTmp);
										if( NewPtypeID[i] == _T("F10") ){    //双头螺栓
											LenBolts +=2*vtof(vTmp);
										}else if( NewPtypeID[i] == _T("F9") || NewPtypeID[i] == _T("F14") ){     //单头螺栓/六角头螺栓
											LenBolts +=vtof(vTmp);
										}
									}
									rsTmp.Close();
									rs.Close();
								}
								//}else{
								//不是六角头螺栓或双头螺杆
								//}
								else if(Ptype[lngCurrentPartNo]== _T("LHA")||
									Ptype[lngCurrentPartNo]==_T("PHA")||
									Ptype[lngCurrentPartNo]==_T("LHB")||
									Ptype[lngCurrentPartNo]==_T("PHB"))
								{
									//这些恒吊都直接连接在根部梁上
									bCalBoltsDiaOfCSPR = true;
									//if( NewPtypeID[i] = _T("F14") Or NewPtypeID[i] = _T("F10") ){
									SQLx = _T("SELECT * FROM [") + modPHScal::tbnLugDiaOfCSPR + _T("] WHERE FiJ=") + ltos(modPHScal::PtypeDiameter[lngCurrentPartNo]) + _T(" AND minDH<=") + ltos(modPHScal::sSprInfo[miSEQofCSPR].DH) + _T(" AND maxDH>=") + ltos(modPHScal::sSprInfo[miSEQofCSPR].DH) + _T(" AND trim(CustomID)=\'") + PtypeCustomID[lngCurrentPartNo] + _T("\' ");
									rsTmp.Open(dbOpenSnapshot,SQLx);
									if( rsTmp.IsEOF() && rsTmp.IsBOF() )
									{
										//没找到螺栓直径，可能数据表数据不全。
										strTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb), modPHScal::dbZDJcrude.GetName(),modPHScal:: tbnBoltsSurplusLength, SQLx); 
										throw strTmp;
									}
									//拉杆直径是标准的，不需要查找系列表
									rsTmp.GetFieldValue(_T("FiJ"),vTmp);iBoltsNutsDia[i]=vtoi(vTmp);
									LenBolts = Cavphs->SACSHeight + 15;
									//按照华东院支吊架手册的螺栓长度裕量计算准则计算螺栓长度。
									SQLx = _T("SELECT * FROM [") + modPHScal::tbnBoltsSurplusLength + _T("] WHERE Diameter=") + ltos(iBoltsNutsDia[i]);
									rsTmp.Close();
									rsTmp.Open(dbOpenSnapshot,SQLx);
									if( rsTmp.IsEOF() && rsTmp.IsBOF() ){
										//没找到螺栓直径，可能数据表数据不全。
										strTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb), modPHScal::dbZDJcrude.GetName(),modPHScal:: tbnBoltsSurplusLength, SQLx); 
										throw strTmp;
									}
									if( AttachmentGroupHavingF4[lngCurrentPartNo] ){
										//附件组含垫片(F4)
										rsTmp.GetFieldValue(_T("SurplusLengthWithF1F2F4"),vTmp);
										if( NewPtypeID[i] == _T("F10") ){    //双头螺栓
											LenBolts+= 2 * vtof(vTmp);
										}else if( NewPtypeID[i] == _T("F9") || NewPtypeID[i] == _T("F14") ){     //单头螺栓/六角头螺栓
											LenBolts +=vtof(vTmp);
										}
									}else{
										rsTmp.GetFieldValue(_T("SurplusLengthWithF1F2"),vTmp);
										if( NewPtypeID[i] == _T("F10") ){    //双头螺栓
											LenBolts+= 2 * vtof(vTmp);
										}else if( NewPtypeID[i] == _T("F9") || NewPtypeID[i] == _T("F14") ){     //单头螺栓/六角头螺栓
											LenBolts +=vtof(vTmp);
										}
									}
									rsTmp.Close();
								}
								
								//首先，在螺栓螺母表里查找附件或螺栓螺母
								
								tbn1 = modPHScal::tbnBoltsNuts;
								if( bCalBoltsDiaOfCSPR )
								{
									CString sID;
									rsTmpZB.GetFieldValue(_T("ID"),vTmp);
									sID=vtos(vTmp);
									if( NewPtypeID[i] == _T("F14") ){
										//六角头（标准）螺栓，按直径、长度排序，取最小的一个
										SQLx = _T("SELECT * FROM [") + tbn1 + _T("] WHERE Trim(CustomID)=\'") + modPHScal::sFindCustomID(sID) + _T("\' AND size2=") + ltos(iBoltsNutsDia[i]) + _T(" AND sizeH>=") + ltos(LenBolts) + _T(" ORDER BY size2,sizeH");
									}
									else if( NewPtypeID[i] == _T("F10") || NewPtypeID[i] == _T("F9") ){
										//双/单头螺栓
										SQLx = _T("SELECT * FROM [") + tbn1 + _T("] WHERE Trim(CustomID)=\'") + modPHScal::sFindCustomID(sID) + _T("\' AND size2=") + ltos(iBoltsNutsDia[i]) + _T(" ORDER BY size2");
									}
									else{
										//螺母垫片
										SQLx = _T("SELECT * FROM [") + tbn1 + _T("] WHERE Trim(CustomID)=\'") + modPHScal::sFindCustomID(sID) + _T("\' AND size2=") + ltos(iBoltsNutsDia[i]) + _T(" ORDER BY size2");
									}
									rsX.Open(dbOpenSnapshot,SQLx);
									if( rsX.IsEOF() && rsX.IsBOF() )
									{
										//如果，在螺栓螺母表里没找到附件或螺栓螺母
										rsTmpZB.GetFieldValue(_T("CustomID"),vTmp);
										strTmp.Format(GetResStr(IDS_NoFoundAttachInZDJCrude),modPHScal::dbZDJcrude.GetName(),SQLx,vtos(vTmp));
										ShowMessage(strTmp);
									}
									else
									{
										//在螺栓螺母表里找到附件或螺栓螺母
										rsTmpZB.Edit();
										//不可以写入seq字段,因为计算函数GetphsBHandSizes已经确定了那些零件是可以编号，那些不可以编号。不需要编号的都赋给空。
										//rsTmpZB.Fields(_T("SEQ")) = i
										if( NewPtypeID[i] == _T("F10") || NewPtypeID[i] == _T("F9") )
										{
											//双头螺栓或者单头螺栓
											rsX.GetFieldValue(_T("BH"),vTmp);
											EDIBgbl::dbPRJ.Execute(CString(_T("UPDATE tmpCSLen SET BH=\'")) + vtos(vTmp)+_T("\'"));
											rsTmpZB.SetFieldValue(_T("BH"),vTmp);
											EDIBgbl::dbPRJ.Execute(CString( _T("UPDATE tmpCSLen SET L1=")) + ltos(LenBolts));
											SQLx = _T("SELECT ") + vtos(modPHScal::sFindFLD(_T("ID"), _T("BHFormat"), NewPtypeID[i])) + _T(" AS sBH FROM tmpCSLen");
											rsTmp.m_pDatabase=&EDIBgbl::dbPRJ;
											rsTmp.Open(dbOpenSnapshot,SQLx);
											if( rsTmp.IsEOF() && rsTmp.IsBOF() ){
												
											}
											else
											{
												rsTmp.GetFieldValue(_T("sBH"),vTmp);
												rsTmpZB.SetFieldValue(_T("CLgg"),vTmp);
												//												rsTmpZB.SetFieldValue(_T("BH"),vTmp);
												//双头螺栓单重按1000mm计算保存在原始表的质量字段
												rsX.GetFieldValue(_T("Weight"),vTmp1);
												rsTmpZB.GetFieldValue(_T("CLnum"),vTmp2);
												rsTmpZB.SetFieldValue(_T("CLdz"),COleVariant(vtof(vTmp1) / 1000 * LenBolts));
												rsTmpZB.SetFieldValue(_T("CLzz"),COleVariant(vtof(vTmp1) / 1000 * LenBolts * vtof(vTmp2)));
												if(NewPtypeID[i] == _T("F4"))
												{
													//一般可能使用45钢
													rsX.GetFieldValue(_T("Material"),vTmp1);
													rsTmpZB.SetFieldValue(_T("CLcl"),vTmp1);
												}
											}
											rsTmp.Close();
										}
										else
										{
											//六角头（标准）螺栓，按直径、长度排序，取最小的一个
											//螺母垫片
											rsX.GetFieldValue(_T("BH"),vTmp);
											rsTmpZB.SetFieldValue(_T("CLgg"),vTmp);
											rsTmpZB.SetFieldValue(_T("BH"),vTmp);
											rsX.GetFieldValue(_T("Weight"),vTmp1);
											rsTmpZB.GetFieldValue(_T("CLnum"),vTmp2);
											rsTmpZB.SetFieldValue(_T("CLdz"),vTmp1);
											rsTmpZB.SetFieldValue(_T("CLzz"),COleVariant(vtof(vTmp1)* vtof(vTmp2)));
											if(NewPtypeID[i] == _T("F4"))
											{
												//一般可能使用45钢
												rsX.GetFieldValue(_T("Material"),vTmp1);
												rsTmpZB.SetFieldValue(_T("CLcl"),vTmp1);
											}
										}
										rsTmpZB.Update();
									}
									rsX.Close();
								}
								//else{
								//不需重新计算螺栓螺母尺寸
								//}
							}
							//else{
							//当前零件不是拉杆，是螺栓螺母或附件,后一个组件是根部，前一个组件不是拉杆,前一个组件不是恒吊
							//}
						}
					}
					else
					{
						//当前零件不是拉杆，是螺栓螺母或附件,后一个组件不是根部
						if( PtypeClassID[lngCurrentPartNo + 1] == iROD ){
							//当前零件不是拉杆，是螺栓螺母或附件,后一个组件不是根部,是拉杆
							//螺栓螺母直径取后一个拉杆的直径
							iBoltsNutsDia[i] = iLugDia[iRecNo[lngCurrentPartNo + 1]];
						}
						else
						{
							//当前零件不是拉杆，是螺栓螺母或附件,后一个组件不是根部,不是拉杆
							if( PtypeClassID[lngCurrentPartNo] == iROD ){
								//当前零件不是拉杆，是螺栓螺母或附件,后一个组件不是根部,不是拉杆,前一个组件是拉杆
								//螺栓螺母直径取前一个拉杆的直径
								iBoltsNutsDia[i] = iLugDia[iRecNo[lngCurrentPartNo]];
							}
							else
							{
								//当前零件不是拉杆，是螺栓螺母或附件,后一个组件不是根部,不是拉杆,前一个组件不是拉杆
								//某些与G12/G13/L7/L8连接的恒吊属于这种情况
								if( PtypeClassID[lngCurrentPartNo] == iCSPR )
								{  //miSEQofCSPR
									//当前零件不是拉杆，是螺栓螺母或附件,后一个组件不是根部,不是拉杆,前一个组件不是拉杆,前一个组件是恒吊
									//螺栓螺母直径取前一个拉杆的直径
									iBoltsNutsDia[i] = iLugDia[iRecNo[lngCurrentPartNo]];
									//当前零件不是拉杆，是螺栓螺母或附件,后一个组件不是根部,不是拉杆,前一个组件不是拉杆,前一个组件是恒吊,当前附件是六角头螺栓或双头螺杆
									//Case _T("LHA"), _T("PHA"), _T("LHB"), _T("PHB")
									//这两种恒吊用螺纹拉杆向上连接到根部梁上
									rsTmp.m_pDatabase=&modPHScal::dbZDJcrude;
									if(Ptype[lngCurrentPartNo]==_T("LHE")  ||
										Ptype[lngCurrentPartNo]==_T("LHE1") ||
										Ptype[lngCurrentPartNo]==_T("PHE")  ||
										Ptype[lngCurrentPartNo]==_T("PHE1") ||
										Ptype[lngCurrentPartNo]==_T("PHC")  ||
										Ptype[lngCurrentPartNo]==_T("LHC"))
									{
										//_T("LHE"), _T("PHE")这两种恒吊需要六角头螺栓或双头螺栓连接到单眼吊板上
										bCalBoltsDiaOfCSPR = true;
										//if( NewPtypeID[i] = _T("F14") Or NewPtypeID[i] = _T("F10") ){
										//查恒吊松紧螺母直径FiJ、FiK、S、I、T。
										//常州电力机械厂样本p20 C型恒吊的T值应该为I值，才与前面的LHE、PHE(1<=DH<=83)的尺寸吻合。
										//编程才方便。
										SQLx = _T("SELECT * FROM [") + modPHScal::tbnLugDiaOfCSPR + _T("] WHERE FiJ=") + ltos(modPHScal::PtypeDiameter[lngCurrentPartNo]) + _T(" AND minDH<=") + ltos(modPHScal::sSprInfo[miSEQofCSPR].DH) + _T(" AND maxDH>=") + ltos(modPHScal::sSprInfo[miSEQofCSPR].DH) + _T(" AND trim(CustomID)=\'") + PtypeCustomID[lngCurrentPartNo] + _T("\' ");
										rsTmp.Open(dbOpenSnapshot,SQLx);
										if( rsTmp.IsEOF() && rsTmp.IsBOF() )
										{
											//没找到螺栓直径，可能数据表数据不全。
											rsTmp.Close();
											if( !modPHScal::gbAutoApplyCSPRLugDia )
											{
												//自动调整恒力弹簧拉杆直径
												SQLx = _T("SELECT * FROM [") + modPHScal::tbnLugDiaOfCSPR + _T("] WHERE minDH<=") + ltos(modPHScal::sSprInfo[miSEQofCSPR].DH) + _T(" AND maxDH>=") + ltos(modPHScal::sSprInfo[miSEQofCSPR].DH) + _T(" AND trim(CustomID)=\'") + PtypeCustomID[lngCurrentPartNo] + _T("\' ");
												rsTmp.Open(dbOpenSnapshot,SQLx);
												if( rsTmp.IsEOF() && rsTmp.IsBOF() )
												{
													//没找到螺栓直径，可能数据表数据不全。
													strTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb), modPHScal::dbZDJcrude.GetName(),modPHScal:: tbnBoltsSurplusLength, SQLx); 
													throw strTmp;
												}
											}
											else
											{
												strTmp.Format(GetResStr(IDS_NotClearCheckAutoApplyCSPRLugDia), modPHScal::dbZDJcrude.GetName(),modPHScal:: tbnBoltsSurplusLength, SQLx);
												throw strTmp;
											}
										}
										//获得螺栓孔径FiK
										rsTmp.GetFieldValue(_T("FiK"),vTmp);
										iBoltsNutsDia[i]=vtoi(vTmp);
										//查找螺栓直径系列表tbnCSPRDiameterSerial,获得准确直径
										SQLx = _T("SELECT * FROM [") + modPHScal::tbnCSPRDiameterSerial + _T("] WHERE Diameter<") + ltos(iBoltsNutsDia[i]) + _T(" ORDER BY Diameter DESC");
										rs.Open(dbOpenSnapshot,SQLx);
										if( rs.IsEOF() && rs.IsBOF() ){
											//没有找到匹配直径系列值
											strTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb), modPHScal::dbZDJcrude.GetName(),modPHScal:: tbnBoltsSurplusLength, SQLx); 
											throw strTmp;
										}
										rs.GetFieldValue(_T("Diameter"),vTmp);iBoltsNutsDia[i]=vtoi(vTmp);
										rsTmp.GetFieldValue(_T("S"),vTmp1);rsTmp.GetFieldValue(_T("I"),vTmp2);
										LenBolts=vtof(vTmp1)+2*vtof(vTmp2);
										//按照华东院支吊架手册的螺栓长度裕量计算准则计算螺栓长度。
										SQLx = _T("SELECT * FROM [") + modPHScal::tbnBoltsSurplusLength + _T("] WHERE Diameter=") + ltos(iBoltsNutsDia[i]) + _T(" ORDER BY Diameter");
										rsTmp.Close();
										rsTmp.Open(dbOpenSnapshot,SQLx);
										if( rsTmp.IsEOF() && rsTmp.IsBOF() ){
											//没找到螺栓直径，可能数据表数据不全。
											strTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb), modPHScal::dbZDJcrude.GetName(),modPHScal:: tbnBoltsSurplusLength, SQLx); 
											throw strTmp;
										}
										if( AttachmentGroupHavingF4[lngCurrentPartNo] ){
											//附件组含垫片(F4)
											rsTmp.GetFieldValue(_T("SurplusLengthWithF1F2F4"),vTmp);
											if( NewPtypeID[i] == _T("F10") ){    //双头螺栓
												LenBolts += 2 * vtof(vTmp);
											}else if( NewPtypeID[i] == _T("F9") || NewPtypeID[i] == _T("F14") ){     //单头螺栓/六角头螺栓
												LenBolts +=vtof(vTmp);
											}
										}
										else
										{
											rsTmp.GetFieldValue(_T("SurplusLengthWithF1F2"),vTmp);
											if( NewPtypeID[i] == _T("F10") ){    //双头螺栓
												LenBolts += 2 * vtof(vTmp);
											}else if( NewPtypeID[i] == _T("F9") || NewPtypeID[i] == _T("F14") ){     //单头螺栓/六角头螺栓
												LenBolts +=vtof(vTmp);
											}
										}
										rs.Close();
										rsTmp.Close();
									}
									//}else{
									//不是六角头螺栓或双头螺杆
									//}
									else if(Ptype[lngCurrentPartNo]==_T("ZHA") ||
										Ptype[lngCurrentPartNo]==_T("LHD") ||
										Ptype[lngCurrentPartNo]==_T("LHD1") ||
										Ptype[lngCurrentPartNo]==_T("ZHB") ||
										Ptype[lngCurrentPartNo]==_T("PHD") ||
										Ptype[lngCurrentPartNo]==_T("PHD1"))
									{
										//这些恒吊都直接连接在根部梁上
										bCalBoltsDiaOfCSPR = true ;   
										//if( NewPtypeID[i] = _T("F14") Or NewPtypeID[i] = _T("F10") ){
										SQLx = _T("SELECT * FROM [") + modPHScal::tbnLugDiaOfCSPR + _T("] WHERE FiJ=") + ltos(modPHScal::PtypeDiameter[lngCurrentPartNo]) + _T(" AND minDH<=") + ltos(modPHScal::sSprInfo[miSEQofCSPR].DH) + _T(" AND maxDH>=") + ltos(modPHScal::sSprInfo[miSEQofCSPR].DH) + _T(" AND trim(CustomID)=\'") + PtypeCustomID[lngCurrentPartNo] + _T("\' ");
										rsTmp.Open(dbOpenSnapshot,SQLx);
										if( rsTmp.IsEOF() && rsTmp.IsBOF() ){
											rsTmp.Close();
											//没找到螺栓直径，可能数据表数据不全。
											if( !modPHScal::gbAutoApplyCSPRLugDia )
											{
												//自动调整恒力弹簧拉杆直径
												SQLx = _T("SELECT * FROM [") + modPHScal::tbnLugDiaOfCSPR + _T("] WHERE minDH<=") + ltos(modPHScal::sSprInfo[miSEQofCSPR].DH) + _T(" AND maxDH>=") + ltos(modPHScal::sSprInfo[miSEQofCSPR].DH) + _T(" AND trim(CustomID)=\'") + PtypeCustomID[lngCurrentPartNo] + _T("\' ");
												rsTmp.Open(dbOpenSnapshot,SQLx);
												if( rsTmp.IsEOF() && rsTmp.IsBOF() ){
													//没找到螺栓直径，可能数据表数据不全。
													strTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb), modPHScal::dbZDJcrude.GetName(),modPHScal:: tbnBoltsSurplusLength, SQLx); 
													throw strTmp;
												}
											}else{
												strTmp.Format(GetResStr(IDS_NotClearCheckAutoApplyCSPRLugDia), modPHScal::dbZDJcrude.GetName(),modPHScal:: tbnBoltsSurplusLength, SQLx);
												throw strTmp;
											}
										}
										//获得螺栓孔径FiK
										rsTmp.GetFieldValue(_T("FiK"),vTmp);
										iBoltsNutsDia[i]=vtoi(vTmp);
										//查找螺栓直径系列表tbnCSPRDiameterSerial,获得准确直径
										SQLx = _T("SELECT * FROM [") + modPHScal::tbnCSPRDiameterSerial + _T("] WHERE Diameter<") + ltos(iBoltsNutsDia[i]) + _T(" ORDER BY Diameter DESC");
										rs.Open(dbOpenSnapshot,SQLx);
										if( rs.IsEOF() && rs.IsBOF() ){
											//没有找到匹配直径系列值
											strTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb), modPHScal::dbZDJcrude.GetName(),modPHScal:: tbnBoltsSurplusLength, SQLx); 
											throw strTmp;
										}
										rs.GetFieldValue(_T("Diameter"),vTmp);
										iBoltsNutsDia[i]=vtoi(vTmp);
										LenBolts = SACSHeight + tmpT;
										//按照华东院支吊架手册的螺栓长度裕量计算准则计算螺栓长度。
										SQLx = _T("SELECT * FROM [") + modPHScal::tbnBoltsSurplusLength + _T("] WHERE Diameter=") + ltos(iBoltsNutsDia[i]);
										rsTmp.Close();
										rsTmp.Open(dbOpenSnapshot,SQLx);
										if( rsTmp.IsEOF() && rsTmp.IsBOF() ){
											//没找到螺栓直径，可能数据表数据不全。
											strTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb), modPHScal::dbZDJcrude.GetName(),modPHScal:: tbnBoltsSurplusLength, SQLx); 
											throw strTmp;
										}
										if( AttachmentGroupHavingF4[lngCurrentPartNo] ){
											//附件组含垫片(F4)
											rsTmp.GetFieldValue(_T("SurplusLengthWithF1F2F4"),vTmp);
											if( NewPtypeID[i] == _T("F10") ){    //双头螺栓
												LenBolts +=2*vtof(vTmp);
											}else if( NewPtypeID[i] == _T("F9") || NewPtypeID[i] == _T("F14") ){     //单头螺栓/六角头螺栓
												LenBolts +=vtof(vTmp);
											}
										}else{
											rsTmp.GetFieldValue(_T("SurplusLengthWithF1F2"),vTmp);
											if( NewPtypeID[i] == _T("F10") ){    //双头螺栓
												LenBolts +=2*vtof(vTmp);
											}else if( NewPtypeID[i] == _T("F9") || NewPtypeID[i] == _T("F14") ){     //单头螺栓/六角头螺栓
												LenBolts +=vtof(vTmp);
											}
										}
										rs.Close();
										rsTmp.Close();
									}
									//}else{
									//不是六角头螺栓或双头螺杆
									//}
									else if(Ptype[lngCurrentPartNo]== _T("LHA")||
										Ptype[lngCurrentPartNo]==_T("PHA")||
										Ptype[lngCurrentPartNo]==_T("LHB")||
										Ptype[lngCurrentPartNo]==_T("PHB"))
									{
										//这些恒吊都直接连接在根部梁上
										bCalBoltsDiaOfCSPR = true;
										//if( NewPtypeID[i] = _T("F14") Or NewPtypeID[i] = _T("F10") ){
										SQLx = _T("SELECT * FROM [") + modPHScal::tbnLugDiaOfCSPR + _T("] WHERE FiJ=") + ltos(modPHScal::PtypeDiameter[lngCurrentPartNo]) + _T(" AND minDH<=") + ltos(modPHScal::sSprInfo[miSEQofCSPR].DH) + _T(" AND maxDH>=") + ltos(modPHScal::sSprInfo[miSEQofCSPR].DH) + _T(" AND trim(CustomID)=\'") + PtypeCustomID[lngCurrentPartNo] + _T("\' ");
										rsTmp.Open(dbOpenSnapshot,SQLx);
										if( rsTmp.IsEOF() && rsTmp.IsBOF() ){
											//没找到螺栓直径，可能数据表数据不全。
											strTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb), modPHScal::dbZDJcrude.GetName(),modPHScal:: tbnBoltsSurplusLength, SQLx); 
											throw strTmp;
										}
										rsTmp.GetFieldValue(_T("FiJ"),vTmp);
										iBoltsNutsDia[i]=vtoi(vTmp);
										LenBolts = SACSHeight + 15;
										rsTmp.Close();
										//按照华东院支吊架手册的螺栓长度裕量计算准则计算螺栓长度。
										SQLx = _T("SELECT * FROM [") + modPHScal::tbnBoltsSurplusLength + _T("] WHERE Diameter=") + ltos(iBoltsNutsDia[i]);
										rsTmp.Open(dbOpenSnapshot,SQLx);
										if( rsTmp.IsEOF() && rsTmp.IsBOF() ){
											//没找到螺栓直径，可能数据表数据不全。
											strTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb), modPHScal::dbZDJcrude.GetName(),modPHScal:: tbnBoltsSurplusLength, SQLx); 
											throw strTmp;
										}
										if( AttachmentGroupHavingF4[lngCurrentPartNo] ){
											//附件组含垫片(F4)
											rsTmp.GetFieldValue(_T("SurplusLengthWithF1F2F4"),vTmp);
											if( NewPtypeID[i] == _T("F10") ){    //双头螺栓
												LenBolts +=2*vtof(vTmp);
											}else if( NewPtypeID[i] == _T("F9") || NewPtypeID[i] == _T("F14") ){     //单头螺栓/六角头螺栓
												LenBolts +=vtof(vTmp);
											}
										}else{
											rsTmp.GetFieldValue(_T("SurplusLengthWithF1F2"),vTmp);
											if( NewPtypeID[i] == _T("F10") ){    //双头螺栓
												LenBolts +=2*vtof(vTmp);
											}else if( NewPtypeID[i] == _T("F9") || NewPtypeID[i] == _T("F14") ){     //单头螺栓/六角头螺栓
												LenBolts +=vtof(vTmp);
											}
										}
									}
									
									//首先，在螺栓螺母表里查找附件或螺栓螺母
									tbn1 = modPHScal::tbnBoltsNuts;
									if( bCalBoltsDiaOfCSPR )
									{
										//需要重新计算恒吊直径
										rsTmpZB.GetFieldValue(_T("ID"),vTmp);
										if( NewPtypeID[i] == _T("F14") ){
											//六角头（标准）螺栓，按直径、长度排序，取最小的一个
											SQLx = _T("SELECT * FROM [") + tbn1 + _T("] WHERE Trim(CustomID)=\'") + modPHScal::sFindCustomID(vtos(vTmp)) + _T("\' AND size2=") + ltos(iBoltsNutsDia[i]) + _T(" AND sizeH>=") + ltos(LenBolts) + _T(" ORDER BY size2,sizeH");
										}else if( NewPtypeID[i] == _T("F10") || NewPtypeID[i] == _T("F9") ){
											//双/单头螺栓
											SQLx = _T("SELECT * FROM [") + tbn1 + _T("] WHERE Trim(CustomID)=\'") + modPHScal::sFindCustomID(vtos(vTmp)) + _T("\' AND size2=") + ltos(iBoltsNutsDia[i]) + _T(" ORDER BY size2");
										}else{
											//螺母垫片
											SQLx = _T("SELECT * FROM [") + tbn1 + _T("] WHERE Trim(CustomID)=\'") + modPHScal::sFindCustomID(vtos(vTmp)) + _T("\' AND size2=") + ltos(iBoltsNutsDia[i]) + _T(" ORDER BY size2");
										}
										rsX.Open(dbOpenSnapshot,SQLx);
										if( rsX.IsEOF() && rsX.IsBOF() ){
											//如果，在螺栓螺母表里没找到附件或螺栓螺母
											//Err.Number = iUE_NoFoundAttachInZDJCrude
											//Err.Description = ResolveResString(iUE_NoFoundAttachInZDJCrude, _T("|1"), dbZDJcrude.Name, _T("|2"), SQLx, _T("|3"), rsTmpZB.Fields(_T("CustomID")))
											//Err.Raise iUE_NoFoundAttachInZDJCrude
											rsTmpZB.GetFieldValue(_T("CustomID"),vTmp);
											strTmp.Format(GetResStr(IDS_NoFoundAttachInZDJCrude),modPHScal::dbZDJcrude.GetName(),SQLx,vtos(vTmp));
											ShowMessage(strTmp);
										}
										else
										{
											//在螺栓螺母表里找到附件或螺栓螺母
											rsTmpZB.Edit();
											//不可以写入seq字段,因为计算函数GetphsBHandSizes已经确定了那些零件是可以编号，那些不可以编号。不需要编号的都赋给空。
											//rsTmpZB.Fields(_T("SEQ")) = i
											if( NewPtypeID[i] == _T("F10") || NewPtypeID[i] == _T("F9") )
											{
												//双头螺栓
												rsX.GetFieldValue(_T("BH"),vTmp);
												EDIBgbl::dbPRJ.Execute(_T("UPDATE tmpCSLen SET BH=\'") + vtos(vTmp) + _T("\'"));
												rsTmpZB.SetFieldValue(_T("BH"),vTmp);
												EDIBgbl::dbPRJ.Execute(_T("UPDATE tmpCSLen SET L1=") + ltos(LenBolts));
												SQLx = _T("SELECT ") + vtos(modPHScal::sFindFLD(_T("ID"), _T("BHFormat"), NewPtypeID[i])) + _T(" AS sBH FROM tmpCSLen");
												rsTmp.m_pDatabase=&EDIBgbl::dbPRJ;
												rsTmp.Open(dbOpenSnapshot,SQLx);
												if( rsTmp.IsEOF() && rsTmp.IsBOF() )
												{
													strTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb),EDIBgbl::dbPRJ.GetName(),_T("tmpCSLen"),SQLx);
													throw strTmp;
												}
												rsTmp.GetFieldValue(_T("sBH"),vTmp);
												rsTmpZB.SetFieldValue(_T("CLgg"),vTmp);
												rsTmpZB.SetFieldValue(_T("BH"),vTmp);
												//双头螺栓单重按1000mm计算保存在原始表的质量字段
												rsX.GetFieldValue(_T("Weight"),vTmp1);
												rsTmpZB.GetFieldValue(_T("CLnum"),vTmp2);
												rsTmpZB.SetFieldValue(_T("CLdz"),COleVariant(vtof(vTmp1) / 1000 * LenBolts));
												rsTmpZB.SetFieldValue(_T("CLzz"),COleVariant(vtof(vTmp1)/ 1000 * LenBolts * vtof(vTmp2)));
												rsTmp.Close();
												if(NewPtypeID[i] == _T("F4"))
												{
													//一般可能使用45钢
													rsX.GetFieldValue(_T("Material"),vTmp1);
													rsTmpZB.SetFieldValue(_T("CLcl"),vTmp1);
												}
											}
											else
											{
												//六角头（标准）螺栓，按直径、长度排序，取最小的一个
												//螺母垫片
												rsX.GetFieldValue(_T("BH"),vTmp);
												rsTmpZB.SetFieldValue(_T("CLgg"),vTmp);
												rsTmpZB.SetFieldValue(_T("BH"),vTmp);
												rsX.GetFieldValue(_T("Weight"),vTmp1);
												rsTmpZB.GetFieldValue(_T("CLnum"),vTmp2);
												rsTmpZB.SetFieldValue(_T("CLdz"),vTmp1);
												rsTmpZB.SetFieldValue(_T("CLzz"),COleVariant(vtof(vTmp1)*vtof(vTmp2)));
												if(NewPtypeID[i] == _T("F4"))
												{
													//一般可能使用45钢
													rsX.GetFieldValue(_T("Material"),vTmp1);
													rsTmpZB.SetFieldValue(_T("CLcl"),vTmp1);
												}
											}
											rsTmpZB.Update();
										}
										rsX.Close();
									}
								}
							}
						}
					}
					
					//已经获得螺栓螺母直径
					//在此开始查找螺栓螺母表
					//写附件(螺栓、螺母)数据
					rsTmpZB.GetFieldValue(_T("ClassID"),vTmp);
					if( ! bCalBoltsDiaOfCSPR && (vtoi(vTmp) == iAttached || vtoi(vTmp) == iBolts || vtoi(vTmp) == iNuts) )
					{
						//所有恒吊螺栓螺母的计算均不会进入这段程序，因为此时bCalBoltsDiaOfCSPR=true
						//首先，在螺栓螺母表里查找附件或螺栓螺母
						tbn1 = modPHScal::tbnBoltsNuts;
						rsTmpZB.GetFieldValue(_T("CustomID"),vTmp);
						SQLx = _T("SELECT * FROM [") + tbn1 + _T("] WHERE Trim(CustomID)=\'") + vtos(vTmp) + _T("\' AND size2=") + ltos(iBoltsNutsDia[i]);
						
						rsX.Open(dbOpenSnapshot,SQLx);
						if( rsX.IsEOF() && rsX.IsBOF() ){
							//如果，在螺栓螺母表里没找到附件或螺栓螺母
							//那么，在根部附件表里查找附件或螺栓螺母
							rsX.Close();
							tbn1 = modPHScal::tbnAttachment;
							SQLx = _T("SELECT * FROM [") + tbn1 + _T("] WHERE Trim(CustomID)=\'") + vtos(vTmp) + _T("\' AND size2=") + ltos(iBoltsNutsDia[i]) + _T(" ORDER BY size2");
							rsX.Open(dbOpenSnapshot,SQLx);
							if( rsX.IsEOF() && rsX.IsBOF() ){
								//在根部附件表里没找到附件或螺栓螺母
								//Err.Number = iUE_NoFoundAttachInZDJCrude
								//Err.Description = ResolveResString(iUE_NoFoundAttachInZDJCrude, _T("|1"), dbZDJcrude.Name, _T("|2"), SQLx, _T("|3"), rsTmpZB.Fields(_T("CustomID")))
								//Err.Raise iUE_NoFoundAttachInZDJCrude
								rsTmpZB.GetFieldValue(_T("CustomID"),vTmp);
								strTmp.Format(GetResStr(IDS_NoFoundAttachInZDJCrude),modPHScal::dbZDJcrude.GetName(),SQLx,vtos(vTmp));
								ShowMessage(strTmp);
							}
							else{
								//在根部附件表里找到附件或螺栓螺母
								rsTmpZB.Edit();
								//不可以写入seq字段,因为计算函数GetphsBHandSizes已经确定了那些零件是可以编号，那些不可以编号。不需要编号的都赋给空。
								//rsTmpZB.Fields(_T("SEQ")) = i
								rsX.GetFieldValue(_T("BH"),vTmp);
								rsTmpZB.SetFieldValue(_T("CLgg"),vTmp);
								rsTmpZB.SetFieldValue(_T("BH"),vTmp);
								rsX.GetFieldValue(_T("Weight"),vTmp1);
								rsTmpZB.GetFieldValue(_T("CLnum"),vTmp2);
								rsTmpZB.SetFieldValue(_T("CLdz"),vTmp1);
								rsTmpZB.SetFieldValue(_T("CLzz"),COleVariant(vtof(vTmp1)*vtof(vTmp2)));
								if(NewPtypeID[i] == _T("F4"))
								{
									//一般可能使用45钢
									rsX.GetFieldValue(_T("Material"),vTmp1);
									rsTmpZB.SetFieldValue(_T("CLcl"),vTmp1);
								}
								rsTmpZB.Update();
							}
						}
						else{
							//在螺栓螺母表里找到附件或螺栓螺母
							rsTmpZB.Edit();
							//不可以写入seq字段,因为计算函数GetphsBHandSizes已经确定了那些零件是可以编号，那些不可以编号。不需要编号的都赋给空。
							//rsTmpZB.Fields(_T("SEQ")) = i
							rsX.GetFieldValue(_T("BH"),vTmp);
							rsTmpZB.SetFieldValue(_T("CLgg"),vTmp);
							rsTmpZB.SetFieldValue(_T("BH"),vTmp);
							rsX.GetFieldValue(_T("Weight"),vTmp1);
							rsTmpZB.GetFieldValue(_T("CLnum"),vTmp2);
							rsTmpZB.SetFieldValue(_T("CLdz"),vTmp1);
							rsTmpZB.SetFieldValue(_T("CLzz"),COleVariant(vtof(vTmp1)*vtof(vTmp2)));
							if(NewPtypeID[i] == _T("F4"))
							{
								//一般可能使用45钢
								rsX.GetFieldValue(_T("Material"),vTmp1);
								rsTmpZB.SetFieldValue(_T("CLcl"),vTmp1);
							}
							rsTmpZB.Update();
						}
						rsX.Close();
					}
				}
				else{
					//不是拉杆，不是螺栓螺母或附件
					lngCurrentPartNo ++;
					lngAttachedPartNo = -1;
					//恒吊计数
					if( NewPtypeClassID[i] == iCSPR ){
						miSEQofCSPR ++;
						rsTmpZB.GetFieldValue(_T("gdw1"),vTmp);
						tmpSelPJG = vtof(vTmp);
					}
					if( NewPtypeClassID[i] == iSPR ){
						//当前零件不是拉杆，不是螺栓螺母或附件,是弹性件
						//恒吊已经在getphsBHandSizes中处理完毕。只要处理T5即可。
						if( PtypeClassID[lngCurrentPartNo + 1] == iROD ){
							//当前零件不是拉杆，不是螺栓螺母或附件,是弹性件,后一个组件是拉杆
							//与弹性件相配的拉杆直径取后一个拉杆的直径
							iBoltsNutsDia[i] = iLugDia[iRecNo[lngCurrentPartNo + 1]];
						}else{
							//当前零件不是拉杆，不是螺栓螺母或附件,是弹性件,后一个组件不是拉杆
							if( PtypeClassID[lngCurrentPartNo]== iROD ){
								//当前零件不是拉杆，不是螺栓螺母或附件,是弹性件,后一个组件不是拉杆,前一个组件是拉杆
								//与弹性件相配的拉杆直径取前一个拉杆的直径
								iBoltsNutsDia[i] = iLugDia[iRecNo[lngCurrentPartNo]];
							}else{
								//当前零件不是拉杆，不是螺栓螺母或附件,是弹性件,后一个组件不是拉杆,前一个组件不是拉杆
							}
						}
						//重新设置弹簧/恒力弹簧规格（连接螺杆直径）
						//如果是弹簧前或后有拉杆，则附加拉杆直径到编号规格中
						//恒吊已经在getphsBHandSizes中处理完毕。只要处理T5即可。
						if( NewPtypeID[i] == _T("T5") )
						{
							rsTmpZB.GetFieldValue(_T("CLgg"),vTmp);
							sBH =vtos(vTmp);
							//注意此处是以M作为T5松紧螺母直径标志的，在适应其它标准时可能遇到问题!
							k = sBH.Find(_T("M"));
							if( k !=-1 ){
								sBH = sBH.Left(k+1) + ltos(iBoltsNutsDia[i]);
								rsTmpZB.Edit();
								rsTmpZB.SetFieldValue(_T("CLgg"),STR_VAR(sBH));
								//								rsTmpZB.SetFieldValue(_T("BH"),STR_VAR(sBH));
								rsTmpZB.Update();
							}
						}
					}
					else
					{
						//当前零件不是拉杆，不是螺栓螺母或附件,不是弹性件
						if( NewPtypeClassID[i] == iGCement || NewPtypeClassID[i] == iSAh )
						{
							//混凝土梁和直接吊除外
						}
						else
						{
							if( bFoundG12G13 && NewPtypeIndex[i] == iSA ){
								//标高风格统一：都标注槽钢上表面标高
								//if( bFoundG12G13 And NewPtypeIndex[i] = iSA And NewPtypeID[i] <> _T("G43") And NewPtypeID[i] <> _T("G44") ){
								//标高风格不统一：G43/G44标注槽钢下表面，其余都标注槽钢上表面标高。
								//G43/G44无需计算高度
								//设置根部主槽钢梁高度，以便正确计算拉杆长度
								rsTmpZB.Edit();
								rsTmpZB.SetFieldValue(_T("sizeH"),COleVariant(SACSHeight));
								rsTmpZB.Update();
							}
						}
					}
					//结束当前零件不是拉杆，不是螺栓螺母或附件
				}
			}
			rsTmpZB.MoveNext();
		}
		
		//更新根部整体重量,最后一个recno不为空的记录就是根部记录,
		//但是,要排除根部是混凝土梁柱的情况
		
		SQLx.Format("SELECT * FROM [ZB] WHERE [VolumeID]=%d AND [ZDJH]=%d AND [nth]=%d AND [Index]=%d ORDER BY recno",
			EDIBgbl::SelVlmID,modPHScal::zdjh,nth,iSA);
		if( rsTmpZB.IsOpen() )
			rsTmpZB.Close();
		rsTmpZB.Open(dbOpenDynaset,SQLx);

		if(rsTmpZB.IsEOF() && rsTmpZB.IsBOF())
		{
			return ;
		}
		else 
		{
			rsTmpZB.Edit();
			if( rsTmpZB.IsBOF() )
			{
				strTmp.Format(GetResStr(IDS_BOForEOFofmvarRsTZB), _T("BOF"));
				throw strTmp;
			}
			if( rsTmpZB.IsEOF() )
			{
				strTmp.Format(GetResStr(IDS_BOForEOFofmvarRsTZB), _T("EOF"));
				throw strTmp;
			}
		}
		
		//写入seq字段,方便以后给零件编号,
		//这样处理之后,在GetphsBOMandCrd中给有效零件编号时,
		//只要获得所有零件seq<>Null的记录,
		//即可获得所有要输出到AutoCAD零件明细表的记录,
		//这就完全排除了对混凝土梁柱的无效材料统计
		rsTmpZB.GetFieldValue(_T("CustomID"),vTmp);
		
		if( !rsID.FindFirst(_T("trim(CustomID)=\'") + vtos(vTmp) + _T("\'")))
		{
			strTmp.Format(GetResStr(IDS_NoRecordFLDEqvThisValueInPictureClipData),_T("CustomID"),vtos(vTmp));
			ShowMessage(strTmp);
		}
		else
		{
			rsID.GetFieldValue(_T("ClassID"),vTmp);
			if( vtoi(vTmp)== iGCement )
			{
				COleVariant nul;
				nul.ChangeType(VT_NULL);
				rsTmpZB.SetFieldValue(_T("SEQ"),nul);
			}
			else
			{
				//型钢根部,赋给一非空的任意值
				//不可，GetphsBHandSizes函数已经确定那些根部可以编号
				//rsTmpZB.Fields(_T("SEQ")) = i
			}
		}
		rsTmpZB.SetFieldValue(_T("CLdz"),COleVariant(mvarSATotalWeight));
		rsTmpZB.GetFieldValue(_T("CLnum"),vTmp);
		rsTmpZB.SetFieldValue(_T("CLzz"),COleVariant(mvarSATotalWeight * vtof(vTmp)));
		rsTmpZB.Update();
		
		//最后一次循环，在螺栓螺母直径都确定时
	}
	catch(CString e)
	{
		ShowMessage(e);
	}
	catch(CDaoException * e)
	{
		e->ReportError();
		e->Delete();
	}
	catch(CException *e)
	{
		e->Delete();
	}
	catch(...)
	{
	}

	if(rsTmpZB.IsOpen())
		rsTmpZB.Close();

	if(NewPtypeClassID!=NULL)
		delete [] NewPtypeClassID;
	if(NewPtypeIndex!=NULL)
		delete [] NewPtypeIndex;
	if(Ptype!=NULL)
		delete [] Ptype;
	if(PtypeCustomID!=NULL)
		delete [] PtypeCustomID;
	if(PtypeClassID!=NULL)
		delete [] PtypeClassID;
	if(PtypeIndex!=NULL)
		delete [] PtypeIndex;
	if(AttachmentGroupHavingF4!=NULL)
		delete [] AttachmentGroupHavingF4;
	if(iRecNo!=NULL)
		delete [] iRecNo;
	if(NewPtypeCustomID!=NULL)
		delete [] NewPtypeCustomID;
	if(NewPtypeID!=NULL)
		delete [] NewPtypeID;
	if(iRecNoOfLugDia!=NULL)
		delete [] iRecNoOfLugDia;
	if(iLugDia!=NULL)
		delete [] iLugDia;
	if(iBoltsNutsDia!=NULL)
		delete [] iBoltsNutsDia;
}

void Cphs::GetphsBlkRotation()
{
}
#pragma warning( disable : 4129 )  //Added by Shuli Luo
long Cphs::CheckMatchPhs()
{
	//根据对象SourceObj(ListBox or Recordset)中包含的支吊架零件组合,确定它们是否能成功相连
	//Input:SourceObj中包含的支吊架零件组合
	//Output:mvarResultObj列表框中装入了可连接的零件
	//按华东电力设计院支吊架手册设计规则判断没有花篮螺丝的组合。
	//返回:成功为-1,否则返回出错位置索引
	long ret=0;
	CString* Ptype=NULL;
	try
	{
		CDaoRecordset rs, rs1;
		long IC=0, i=0, j=0 , k=0;bool  FlgMatch=0;
		long iNumL5 ;CString tbn1 ;long iClassPA=0, iNumRod =0;
		bool bLastPartIsSA=false ; //最后一个零件应当是根部
		if(SourceObj==NULL || !IsWindow(SourceObj->m_hWnd))
			return 0;
		IC=SourceObj->GetCount();
		Ptype=new CString [IC];
		if( IC > 1 ){
			//如果组装零部件个数大于1
			//假定最后一个零件不是根部
			bLastPartIsSA = false;
			CString lstText;
			for (i = 0 ;i<IC;i++)
			{
				SourceObj->GetText(i,lstText);
				Ptype[i] = modPHScal::sFindID(Trim(lstText));
				tbn1 = modPHScal::sFindTBN(Trim(lstText));
				//加装花篮螺丝的规则并未在华东院手册中贯彻一致，故最好留给设计者判断何处需要加设花篮螺丝更方便。
				//计算花篮螺丝数量：华东电力设计院支吊架手册规定“当吊架的根部与管部之间无拉杆长度调节措施时应该装设花篮螺丝”
				if( i == 0 ){
					iClassPA = modPHScal::glClassID;
				}else if( i == IC - 1 ){
					if( modPHScal::glIDIndex == iSA ){
						bLastPartIsSA = true;
					}
				}
				if( modPHScal::glClassID == iROD ) iNumRod = iNumRod + 1;
				//如果是吊架，则花篮螺丝数量
				if( iClassPA == iPAh ){
					if( modPHScal::sFindID(Ptype[i]) == _T("L5") ){
						iNumL5 = iNumL5 + 1;
					}
				}
			}
			CString SQLx,sTmp;
			COleVariant vTmp;
			if( bLastPartIsSA ){
				rs.m_pDatabase=&EDIBgbl::dbPRJ;
				for (i = 0;i<= IC - 2;i++)
				{
					//假设每一个都不匹配
					FlgMatch = false;
					//从第一个开始查找匹配
					SQLx = _T("SELECT * FROM connect WHERE trim(cntb)=\'") + Ptype[i] + _T("\' AND  trim(cnte)=\'") + Ptype[i + 1] + _T("\'");
					rs.Open(dbOpenSnapshot,SQLx);
					if( rs.IsEOF() && rs.IsBOF() ){
						FlgMatch = false;
						break;
					}
					else if( ! rs.IsEOF() )
					{
						rs.MoveLast();
						rs.MoveFirst();
						for( j = 0 ;j<rs.GetRecordCount();j++)
						{
							//3个或3个以上的零件,匹配条件更严格
							rs.GetFieldValue(_T("cntn"),vTmp);
							if( vtos(vTmp)==_T("")){
								FlgMatch = true;
								break;
							}else {
								if( i + 2 <= IC - 2 ){
									if( vtos(vTmp) == Ptype[i + 2] ){
										FlgMatch = true;
										break;
									}
								}else {
									FlgMatch = true;
									break;
								}
							}
							rs.MoveNext();
						}
						if( j >= rs.GetRecordCount() ){
							FlgMatch = false;
							break;
						}
					}
					
					//如果真地没有匹配,则退出
					if( ! FlgMatch ) break;
					rs.Close();
				}
				
				if( i == IC - 1 ){
					if( iClassPA == iPAh && iNumL5 > 0 && iNumRod > 0 ){
						//“当吊架的根部与管部之间无拉杆长度调节措施时应该装设花篮螺丝”
						ret = -1;
						AfxMessageBox(GetResStr(IDS_PhsAssembleSuccess),MB_OK,0);
					}else {
						//加装花篮螺丝的规则并未在华东院手册中贯彻一致，故最好留给设计者判断何处需要加设花篮螺丝更方便。
						ret = -1;
						//MsgBox ResolveResString(iUE_LossL5)
						AfxMessageBox(GetResStr(IDS_PhsAssembleSuccess),MB_OK,0);
					}
				}else {
					ret = i;
					//MsgBox _T("支吊架组装失败!不能连接位置在") & Ptype(i) & _T("和") & Ptype(i + 1) & _T("之间。")
					sTmp.Format(GetResStr(IDS_PhsAssembleFailed),modPHScal::sFindCustomID(Ptype[i]), modPHScal::sFindCustomID(Ptype[i + 1]));
					AfxMessageBox(sTmp,MB_OK,0);
				}
			}else {
				//如果最后一个零件不是根部
				//人为设置以便报错
				//MsgBox _T("支吊架组装失败!(最后一个零件不是根部)")
				ret= 0;
				AfxMessageBox(GetResStr(IDS_PhsAssembleFailedLastPartIsNotSA),MB_OK,0);
			}
		}else {
			//如果组装零部件个数不大于1
			//人为设置以便报错
			//MsgBox _T("支吊架组装失败!(零件数太少)")
			ret = 0;
			AfxMessageBox(GetResStr(IDS_PhsAssembleFailedToolittlePart),MB_OK,0);
		}
		
	}
	catch(::CDaoException* e)
	{
		e->ReportError();
		e->Delete();
		ret=0;
	}
	if(Ptype!=NULL)
		delete [] Ptype;
	return ret;
}
#pragma warning( disable : 4129 )  //Added by Shuli Luo
void Cphs::CheckDuplicateIndex()
{
	//检查sort.mdb中的表phsStructureName,
	//保证其SampleID值唯一.
	//由于程序的逻辑严密,可保证SampleID值从1依次递增,中间不会缺号
	//Screen.MousePointer = vbHourglass
	AfxGetApp()->BeginWaitCursor();
	CDaoRecordset rs1,rs;
	CString sTmp;
	try{
		EDIBgbl::SQLx = _T("SELECT * FROM phsStructureName");
		//使用rs1以免和GetphsAssembleName中的模块级对象rs冲突
		rs1.m_pDatabase=&EDIBgbl::dbPHScode; //20071101 "dbSORT" 改为 "dbPHScode"
		rs1.Open(dbOpenDynaset,EDIBgbl::SQLx);
		EDIBgbl::SQLx = _T("SELECT * FROM phsStructureREF");
		rs.m_pDatabase=&EDIBgbl::dbPHScode; //20071101 "dbSORT" 改为 "dbPHScode"
		rs.Open(dbOpenSnapshot,EDIBgbl::SQLx);
		
		COleVariant v;
		if(rs1.IsBOF()&& rs1.IsEOF())
		{
			sTmp=GetResStr(IDS_NoAnyRecordInphsStructureName)+GetResStr(IDS_NoAnyRecordInphsStructureName);
			ShowMessage(sTmp);
			goto errH;
		}
		if( !rs1.IsEOF())
		{
			rs1.MoveLast();
			rs1.MoveFirst();
			while(!rs1.IsEOF())
				//for (int i = 0 ;i<rs1.GetRecordCount();i++)
			{
				rs1.GetFieldValue(_T("SampleID"),v);
				sTmp.Format(_T("%d"),vtoi(v));
				sTmp=CString(_T("SampleID="))+sTmp+_T("");
				//在phsStructureREF中没有发现phsStructureName中
				//存在的SampleID的数据,则删除phsStructureName中的这个SampleID记录
				if(! rs.FindFirst(sTmp))
				{
					rs1.Delete();
				}
				rs1.MoveNext();
			}
		}
	}
	catch(::CDaoException * e)
	{
		e->ReportError();
		e->Delete();
	}
errH:
	AfxGetApp()->EndWaitCursor();
	if(rs.IsOpen())
		rs.Close();
	if(rs1.IsOpen())
		rs1.Close();
}

long Cphs::CheckDuplicateREFRecord()
{
    return (long)0;
}

long Cphs::CheckInitRecord()
{
    return (long)0;
}
#pragma warning( disable : 4129 )  //Added by Shuli Luo
long Cphs::CheckDuplicateREFRecordWhenAppend(int *ipCheckButton)   //LFX  2005.3.24  加
{
	//目的：检查SampleID的唯一性。
	//检查sort.mdb中的表phsStructureREF,保证要加入的支吊架组合与库中组合不同
	//也就是说,任何不同的SampleID都有不同的支吊架组合
	//input:SourceObj对象(listBox)中含有待加入库中的支吊架组合
	//output:成功,返回加入库中的支吊架组合的新索引号;出错,返回-1
	//本函数可以处理phsStructureName中为空记录的情况
	long ret=-1;
	// CString *sSampleID=NULL;
	// CString *DupRec=NULL;
	try
	{
		CDaoRecordset rs1;
		
		long IC =0;
		long k=0,m=0,n=0;
		long i=0;
		
		//找出phsStructureREF中所有seq=0的记录,按SampleID排序
		CString SQLx,sTmp;
		COleVariant vTmp;
		IC=SourceObj->GetCount();
		if(IC<1)
			throw;
		long Ci=0;
		for(i=0;i<IC;i++)
		{
			SourceObj->GetText(i,sTmp);
			sTmp=modPHScal::sFindID(Trim(sTmp));
			if(i==0)
			{
				//这两句SQL语句十分精彩，免去了逐个记录比较的循环过程，节约了时间。李国斌注2001.12.20
				SQLx=(_T("Select SampleID  From phsStructureREF phsStructureREFX Where SEQ = 0 AND ID = \'") + sTmp + _T("\'"));
			}
			else 
			{
				SQLx+=CString((_T(" AND SampleID IN \n( ")))+(_T("Select SampleID From phsStructureREF Where SEQ = ") + ltos(i) +_T(" AND ID = \'") + sTmp + _T("\'"));
				Ci++;
			}
		}
		sTmp=_T("");
		for(i=0;i<Ci;i++)
			sTmp+=(_T(")"));
		SQLx+=sTmp;
		SQLx+=(_T(" AND NOT EXISTS ( Select * From phsStructureREF Where  SEQ >= ")+ ltos(IC) + _T(" AND SampleID = phsStructureREFX.SampleID )"));
		rs1.m_pDatabase=&EDIBgbl::dbPHScode;//20071101 "dbSORT" 改为 "dbPHScode"
		rs1.Open(dbOpenDynaset,SQLx);
		CString DupRec;
		if(!rs1.IsEOF() && !rs1.IsBOF())
		{
			m=1;
			rs1.GetFieldValue(_T("SampleID"),vTmp);
			DupRec=vtos(vTmp);
		}
		else
			m=0;
		rs1.Close();
		
		//若m=0,则不是重复记录
		//rs2.m_pDatabase=&EDIBgbl::dbSORT;
		if( m == 0 )
		{
			//获取尚未使用的SampleID号,其值作为最新的SampleID
			SQLx = _T("SELECT * FROM phsStructureREF WHERE Seq=0 ORDER BY SampleID");
			rs1.Open(dbOpenDynaset,SQLx);
			if( rs1.IsBOF() && rs1.IsEOF() )
			{
				//MsgBox ResolveResString(iUE_NoAnyRecordInphsStructureName)
				//Exit Function
				IC = 0;
			}
			else if( ! rs1.IsEOF() )
			{
				rs1.MoveLast();
				rs1.MoveFirst();
				for (k = 1 ;k<= rs1.GetRecordCount();k++)
				{
					rs1.GetFieldValue(_T("SampleID"),vTmp);
					IC =vtoi(vTmp);
					//如果记录次序与SampleID不等，说明不连续，
					//则空缺的SampleID号可用
					if( k != IC ) 
						break;
					rs1.MoveNext();
				}
				//如果k > rs2.RecordCount,说明SampleID连续，
				//最后一个SampleID号+1可用
				if( k > rs1.GetRecordCount() )
				{
					IC = k - 1;
				}
				else
				{
					//可用的SampleID-1=IC
					IC = IC - 2;
				}
			}
			rs1.Close();
			modPHScal::iSelSampleID = IC + 1;
			sTmp.Format(_T("%04d"),modPHScal::iSelSampleID);
			SQLx = _T("INSERT INTO phsStructureName (FREQUENCE,SampleID,BmpName,bAllowUse) VALUES (1,") + ltos(modPHScal::iSelSampleID) + _T(",\'") + sTmp + _T("\',-1)");
			//必须加入共享库，以便集中管理
			EDIBgbl::dbPHScode.Execute(SQLx);//20071101 "dbSORT" 改为 "dbPHScode"
			//同时加入本地库中，以便检索更快
			EDIBgbl::dbPRJ.Execute(SQLx);
			for( k = 0 ;k<SourceObj->GetCount();k++)
			{
				SourceObj->GetText(k,sTmp);
				SQLx = _T("INSERT INTO phsStructureREF (SampleID,SEQ,ID,CustomID,FREQUENCE) VALUES (")+
					ltos(modPHScal::iSelSampleID)+_T(",")+ltos(k)+_T(", \'") +modPHScal::sFindID(Trim(sTmp)) +_T("\' , \'") +Trim(sTmp) + _T("\' , 1)");
				//必须加入共享库，以便集中管理
				EDIBgbl::dbPHScode.Execute(SQLx);//20071101 "dbSORT" 改为 "dbPHScode"
				//同时加入本地库中，以便检索更快
				EDIBgbl::dbPRJ.Execute(SQLx);
			}
			ret = modPHScal::iSelSampleID;
			sTmp.Format(GetResStr(IDS_NewPhsSampleID), ltos(modPHScal::iSelSampleID));

 			// LFX 2005.3.24 加 用于用户选择了非“全部模块” 单选框时，提示用户是否将
			//当前模块置为常用模块或默认常用模块
			SQLx.Empty();
			if (ipCheckButton != NULL && *ipCheckButton == 0) 
			{
				//如果选中了 默认常用模块 单选框
				sTmp = sTmp + "\n真的要将新建模块置为 默认常用模块 吗?";
				if (AfxMessageBox(sTmp,MB_YESNO) == IDYES)
				{
					SQLx.Format("UPDATE phsStructureName SET Favorite = TRUE,DefaultFavoriteTemplate = TRUE WHERE SampleID = %d",modPHScal::iSelSampleID);
				}
				else
				{
					*ipCheckButton = 2;      //设置单选框为 全部模板
				}
			}
			else if (ipCheckButton != NULL && *ipCheckButton == 1)
			{
				//如果选中了 默认模块 单选框
				sTmp = sTmp + "\n真的要将新建模块置为 常用模块 吗?";
				if (AfxMessageBox(sTmp,MB_YESNO) == IDYES)
				{
					SQLx.Format("UPDATE phsStructureName SET Favorite = TRUE WHERE SampleID = %d",modPHScal::iSelSampleID);
				}
				else
				{
					*ipCheckButton = 2;      //设置单选框为 全部模板
				}
			}
			else
			{
				//如果选中了 全部模板 单选框
				ShowMessage(sTmp);  				
			}

			if (ipCheckButton != NULL && (*ipCheckButton == 0 || * ipCheckButton == 1))
			{
				//必须加入共享库，以便集中管理
				EDIBgbl::dbPHScode.Execute(SQLx);//20071101 "dbSORT" 改为 "dbPHScode"
				//同时加入本地库中，以便检索更快
				EDIBgbl::dbPRJ.Execute(SQLx);
			}
 


			//ShowMessage(sTmp);  LFX 去掉

			//END  LFX 2005.3.24 加 用于用户选择了非“全部模块” 单选框时，提示用户是否将

		}
		else 
		{
			//MsgBox _T("支吊架组合在样本库中已经存在. 模板号为") & sSampleID(i)
			// LFX 2005.3.25 加 如果模板已存在，提示用户是否立即查看该模板
			bool bDefFavTem;   //是否为默认常用模板
			bool bFavTem;      //是否为常用模板

			sTmp.Format(GetResStr(IDS_ExistsThisSampleIDinSampleREF), EDIBgbl::dbSORT.GetName(), DupRec);  //原

			SQLx.Format(_T("SELECT * FROM phsStructureName WHERE SampleID = %d"),_ttoi(DupRec));  
			rs1.Open(dbOpenDynaset,SQLx);			
			rs1.GetFieldValue(_T("DefaultFavoriteTemplate"),vTmp);
			bDefFavTem = vtob(vTmp);
			rs1.GetFieldValue(_T("Favorite"),vTmp);
			bFavTem = vtob(vTmp);

			if (bDefFavTem)
			{
				sTmp = sTmp + _T("\n当前模板在 默认常用模板 类中，要立即查看该模板内容吗？");
			}
			else if (bFavTem)
			{
				sTmp = sTmp + _T("\n当前模板在 常用模板 类中，要立即查看该模板内容吗？");
			}
			else
			{
				sTmp = sTmp + _T("\n当前模板在 所有模板 类中，要立即查看该模板内容吗？");
			}

			//ShowMessage(sTmp);  LFX 去掉
			if (AfxMessageBox(sTmp,MB_YESNO) == IDYES)
			{
				modPHScal::iSelSampleID = _ttoi(DupRec);
				if (bDefFavTem)
				{
					*ipCheckButton = 0;
				}
				else if (bFavTem)
				{
					*ipCheckButton = 1;
				}
				else
				{
					*ipCheckButton = 2;
				}
			}
			ret = _ttoi(DupRec);
		}
	}
	catch(::CDaoException* e)
	{
		ret=-1;
		e->ReportError();
		e->Delete();
	}
	catch(CString e)
	{
		ShowMessage(e);
		ret=-1;
	}
	catch(CException *e)
	{
		e->Delete();
		ret = -1;
	}
	catch(...)
	{
		ret=-1;
	}
	return ret;
}
#pragma warning( disable : 4129 )  //Added by Shuli Luo
void Cphs::CheckAllCustomIDandIDinPhsStructureREF()
{
	//目的：检查所有的CustomID字段，将其转换为ID字段。设置ID字段是为了保持PhsStructureREF表中模板号的唯一性/通用性。
	//所有的查找表现均以ID为依据.
	
	
	//打开这个快照，查找速度比调用modPHScal::sFindID和sFindCustomID快。
	CDaoRecordset rs,rsX;
	CString sTmp;
	COleVariant v,vTmp;
	CString sf1,sf2,sf3,sTmp1,stmp2;
	frmStatus.ShowWindow(SW_SHOW);
	//下面的算法以phsStructureREF为中心，依次代换，由于零件成千上万，速度很慢。
	/*
	try
	{
	//CString SQLx = _T("SELECT CustomID,ID  FROM PictureClipData");
	//rsID.m_pDatabase=&EDIBgbl::dbPRJ;
	//rsID.Open(dbOpenSnapshot,SQLx);
	if(!rsID.IsOpen())
	{
	EDIBgbl::SQLx = _T("SELECT * FROM PictureClipData");
	rsID.m_pDatabase=&EDIBgbl::dbPRJ;
	rsID.Open(dbOpenDynaset,EDIBgbl::SQLx);
	brsIDStatus=TRUE;
	}
	
	  EDIBgbl::SQLx = _T("SELECT SampleID,seq,CustomID,ID  FROM phsStructureREF ORDER BY SampleID,seq");
	  rsX.m_pDatabase=&EDIBgbl::dbSORT;
	  rsX.Open( dbOpenDynaset ,EDIBgbl::SQLx);   
	  if( rsX.IsEOF() && rsX.IsBOF() ) 
	  {
	  sTmp=GetResStr(IDS_NoAnySEQeqZeroRecordInphsStructureREF);
	  ShowMessage(sTmp);
	  goto errH;
	  }
	  rsX.MoveLast();
	  int iC2 = rsX.GetRecordCount();
	  rsX.GetFieldValue(_T("SampleID"),v);
	  int iCSampleID =vtoi(v);
	  rsX.MoveFirst();
	  frmStatus.m_Label1=GetResStr(IDS_CheckAllCustomIDandIDinPhsStructureREF);
	  CString CustomID,ID;
	  for(int j = 0 ;j< iC2 ;j++)
	  {
	  AfxGetApp()->BeginWaitCursor();
	  //Screen.MousePointer = vbHourglass
	  //给窗体显示一点时间
	  rsX.GetFieldValue(_T("CustomID"),v);
	  CustomID=vtos(v);
	  if(rsID.FindFirst (CString(_T("Trim(CustomID)=\'")) + CustomID + _T("\'")))
	  {
	  rsID.GetFieldValue(_T("ID"),v);
	  ID=vtos(v);
	  if (ID== _T("") )
	  {
	  rsX.GetFieldValue(_T("ID"),v);
	  ID=vtos(v);
	  if(rsID.FindFirst(CString(_T("Trim(ID)=\'")) + ID + _T("\'")))
	  //如果为空，则显然不能从CustomID转换为ID,因而进行逆变换。
	  {
						rsID.GetFieldValue(_T("CustomID"),v);
						CustomID=vtos(v);
						if( CustomID != _T("") )
						//仍然为空，报告一个错误
						//msgbox
						{
						rsX.Edit();
						rsX.SetFieldValue(_T("CustomID"),v);
						rsX.Update();
						}
						}
						
						  //在ID-CustomID对照表中没有发现这种零部件
						  //msgbox
						  }
						  else
						  {
						  rsID.GetFieldValue(_T("ID"),v);
						  rsX.Edit();
						  rsX.SetFieldValue(_T("ID"),v);
						  rsX.Update();
						  }
						  }
						  else
						  {
						  //如果为空，则显然不能从CustomID转换为ID,因而进行逆变换。
						  rsX.GetFieldValue(_T("ID"),v);
						  ID=vtos(v);
						  if(rsID.FindFirst( CString(_T("Trim(ID)=\'")) + ID + _T("\'")))
						  {
						  //如果为空，则显然不能从CustomID转换为ID,因而进行逆变换。
						  rsID.GetFieldValue(_T("CustomID"),v);
						  CustomID=vtos(v);
						  if(CustomID!=_T(""))
						  {
						  rsX.Edit();
						  rsX.SetFieldValue(_T("CustomID"),v);
						  rsX.Update();
						  }
						  else
						  {
						  }
						  }
						  else
						  {
						  //在ID-CustomID对照表中没有发现这种零部件
						  }
						  }
						  rsX.GetFieldValue(_T("SampleID"),v);
						  sf1.Format(_T("%d"),vtoi(v));
						  rsX.GetFieldValue(_T("seq"),v);
						  sf2.Format(_T("%d"),vtoi(v));
						  sTmp1.Format(_T("%d"),iCSampleID);
						  stmp2.Format(_T("%d"),iC2);
						  //共%s个模板%s个零件,第%s号模板第%s号零件CustomID=%s
						  frmStatus.m_Label2.Format(GetResStr(IDS_CheckNoXCustomIDandIDinPhsStructureREF),sTmp1,stmp2,sf1,sf2,CustomID);
						  frmStatus.UpdateStatus((float)(j+1)/(float)iC2,true);
						  frmStatus.UpdateData(false);
						  
							rsX.MoveNext();
							}
							}
*/

//改变算法，提高速度
//下面的算法以PictureClipData为中心，依次代换，由于零件只有138个，速度非常快。
#pragma warning( disable : 4129 )  //Added by Shuli Luo
try
{
	AfxGetApp()->BeginWaitCursor();
	
	CString SQLx = _T("SELECT CustomID,ID  FROM PictureClipData WHERE index=") + ltos(iPA) + _T(" OR index=") + ltos(iSA)+ _T(" OR index=") +ltos(iConnectPART);
	rsX.m_pDatabase=&EDIBgbl::dbPRJ;
	rsX.Open(dbOpenSnapshot,SQLx);
	if( rsX.IsEOF() && rsX.IsBOF() ) 
	{
		sTmp=GetResStr(IDS_NoAnySEQeqZeroRecordInphsStructureREF);
		ShowMessage(sTmp);
		goto errH;
	}
	else
	{
		frmStatus.m_Label1.Format(GetResStr(IDS_UpdateCustomIDinPhsStructureREF));
		
		rsX.MoveLast();
		int iC2=rsX.GetRecordCount();						
		for(int j=1;j<=iC2;j++)
		{
			rsX.GetFieldValue(_T("CustomID"),v);
			rsX.GetFieldValue(_T("ID"),vTmp);
			SQLx = _T("UPDATE phsStructureREF SET CustomID=\'") + vtos(v)+ _T("\' WHERE ID=\'") + vtos(vTmp)+_T("\'");
			EDIBgbl::dbPRJ.Execute(SQLx);
			EDIBgbl::dbPHScode.Execute(SQLx);//20071018 "dbSORT" 改为 "dbPHScode"
			//已经替换
			frmStatus.m_Label2=_T("");
			frmStatus.UpdateStatus((float)j/(float)iC2,true);
			frmStatus.UpdateData(false);
			
			
			rsX.MovePrev();
		}		
	}
}
catch(::CDaoException * e)
{
	e->ReportError();
	e->Delete();
}
errH:
frmStatus.ShowWindow(SW_HIDE);
//AfxGetApp()->EndWaitCursor();

}
#pragma warning( disable : 4129 )  //Added by Shuli Luo
void Cphs::ChangeNameInphsStructureName()
{
	//目的：重新整理模板管理表
	//（先清空表中所有记录，再根据模板结构表生成模板管理表，这保存了使用频度值）	
	
	//下面的算法逐个记录查找，速度已经优化，比后面注释掉的大段程序快得多。
	AfxGetApp()->BeginWaitCursor();
	long iNumOfSPR=0,SampleID=0,iNumCSPR=0,iNumSPR=0,iNumRod=0,IC=0;
	CString sPA,sSA,sSPR;	
	CString Ptype0,PtypeC,Ptype;
	CString sTmp1;
	CString strTmpName,msNamePA,msNameSA,msNameType;
	CString strTmp;
	int IC2=0;
	int j=0, iPos;
	CString sFl;	
	//删除表phsStructureName所有记录,这把使用频度也删除了。FREQUENCE应该保存。
	
	CString sTmp,strSQL;
	CDaoRecordset rs1;//rs1与rs2取自同一个表phsStructureREF
	CDaoRecordset rs2;
	CDaoRecordset rs3;
	CDaoRecordset rs4;
	rs1.m_pDatabase=&EDIBgbl::dbPRJ;
	rs2.m_pDatabase=&EDIBgbl::dbPRJ;
	rs3.m_pDatabase=&EDIBgbl::dbPRJ;
	rs4.m_pDatabase=&EDIBgbl::dbPHScode;//20071018 "dbSORT" 改为 "dbPHScode"
	//打开表备查
	CDaoRecordset rsPA(&EDIBgbl::dbPRJ);
	CDaoRecordset rsSA(&EDIBgbl::dbPRJ);
	CDaoRecordset rsPR(&EDIBgbl::dbPRJ);
	CDaoRecordset rsSP(&EDIBgbl::dbPRJ);
	CDaoRecordset rsCS(&EDIBgbl::dbPRJ);
	rsPA.Open(dbOpenTable,_T("tmpCustomIDPA"));
	rsSA.Open(dbOpenTable,_T("tmpCustomIDSA"));
	rsPR.Open(dbOpenTable,_T("tmpCustomIDPART"));
	rsSP.Open(dbOpenTable,_T("tmpCustomIDSPR"));
	rsCS.Open(dbOpenTable,_T("tmpCustomIDCSPR"));
	rsPA.SetCurrentIndex(_T("Index1"));
	rsSA.SetCurrentIndex(_T("Index1"));
	rsPR.SetCurrentIndex(_T("Index1"));
	rsSP.SetCurrentIndex(_T("Index1"));
	rsCS.SetCurrentIndex(_T("Index1"));

	std::vector<COleVariant> vecID;
	std::vector<CString> vecDescription;
	std::vector<COleVariant> vecCustomID;
	std::vector<int> vecClassID;
	std::vector<COleVariant>::iterator iter;
	vecID.reserve(200);
	vecDescription.reserve(200);
	vecCustomID.reserve(200);
	vecClassID.reserve(200);

	
	CString sFind;
	COleVariant vTmp;
	int oldSampleID;
	
	long t1;
	long t2;
	t1=::GetTickCount();		
	//2002.01.03测试时间31084ms<12413+55730ms,故综合在一起查询较快
	
	//预先设置当前规范可用的模板为全部不可用，只要有任意一个零件的bAllowUse=0，即为不可用
	strSQL=_T("UPDATE PhsStructureName SET bAllowUse=0");
	EDIBgbl::dbPHScode.Execute(strSQL);//20071018 "dbSORT" 改为 "dbPHScode"



	if(!rsID.IsOpen())
	{
		EDIBgbl::SQLx = _T("SELECT * FROM PictureClipData");
		//if(Cavphs->rsID==NULL)
		//Cavphs->rsID.CreateInstance(__uuidof(Recordset));
		//Cavphs->rsID->CursorLocation=adUseClient;
		//Cavphs->rsID->Open(_variant_t(EDIBgbl::SQLx),(IDispatch*)EDIBgbl::dbPRJ,adOpenDynamic,adLockOptimistic,adCmdText);
		rsID.m_pDatabase=&EDIBgbl::dbPRJ;
		rsID.Open(dbOpenDynaset,EDIBgbl::SQLx);
		brsIDStatus=TRUE;
	}	
	try
	{
		while(!rsID.IsEOF())
		{
			rsID.GetFieldValue(_T("ID"), vTmp);
			vecID.push_back(vTmp);
			rsID.GetFieldValue(_T("CustomID"), vTmp);
			vecCustomID.push_back(vTmp);
			rsID.GetFieldValue(_T("Description"), vTmp);
			vecDescription.push_back(vtos2(vTmp));
			rsID.GetFieldValue(_T("ClassID"), vTmp);
			vecClassID.push_back(vtoi2(vTmp));
			rsID.MoveNext();	
		}


		EDIBgbl::SQLx=_T("SELECT SampleID FROM phsStructureREF WHERE SEQ=0");
		rs2.Open(dbOpenSnapshot,EDIBgbl::SQLx);
		rs2.MoveLast();
		rs2.MoveFirst();
		IC2=rs2.GetRecordCount();
		rs2.Close();

		EDIBgbl::SQLx = _T("SELECT * FROM phsStructureREF ORDER BY SampleID ASC, SEQ ASC");
		rs2.Open(dbOpenDynaset ,EDIBgbl::SQLx);
		CString strSQL = _T("select * from phsStructureNAME");
		rs4.Open(dbOpenDynaset ,strSQL);
		bool stop=false;
		int iNum=0;
		int iTimes=-1;//各零部件可用标志的连乘积
		if( rs2.IsEOF() && rs2.IsBOF() )
		{
			//空的phsStructureREF
			ShowMessage(GetResStr(IDS_NoAnySEQeqZeroRecordInphsStructureREF));
			//goto errH;
		}
		else
		{
			if(!frmStatus.IsWindowVisible())
				frmStatus.ShowWindow(SW_SHOW);
			frmStatus.SetWindowText(GetResStr(IDS_InitializeAutoPHS));
			frmStatus.m_Label1 = GetResStr(IDS_ChangeNameInphsStructureName);
			EDIBgbl::SQLx = _T("SELECT * FROM phsStructureName");
			rs3.Open(dbOpenDynaset ,EDIBgbl::SQLx);
			int fdL;
			CDaoFieldInfo info;
			rs3.GetFieldInfo(_T("SampleName"),info);
			fdL=info.m_lSize;	
			sSPR = _T(" ");
			//移动到结构表头
			rs2.MoveFirst();
			while(!rs2.IsEOF())
			{
				AfxGetApp()->BeginWaitCursor();
				rs2.GetFieldValue( _T("SampleID"),vTmp);
				SampleID =vtoi( vTmp);
				oldSampleID=SampleID;
				sTmp1.Format(_T("%d"),SampleID);
				EDIBgbl::SQLx = CString(_T("SampleID=")) + sTmp1;
				iNumSPR=0;
				iNumCSPR=0;
				iNumRod=0;
				stop=false;
				iNum=0;
				do
				{
					rs2.MoveNext();
					if(rs2.IsEOF())
					{
						stop=true;
					}
					else
					{
						rs2.GetFieldValue( _T("SampleID"),vTmp);
						SampleID =vtoi( vTmp);
						if(SampleID!=oldSampleID)
							stop=true;
					}
					rs2.MovePrev();

					rs2.GetFieldValue( _T("SampleID"),vTmp);
					SampleID =vtoi( vTmp);
					
					rs2.GetFieldValue(_T("ID"),vTmp);
					Ptype=vtos(vTmp);
					if(iNum==0)
					{
						//管部
						Ptype0=Ptype;
						iter = std::find (vecID.begin(),vecID.end(),vTmp);
						if(iter!=vecID.end())
						{
							iPos = (iter - vecID.begin());

							msNamePA = vecDescription[iPos];

							if(rsPA.Seek(_T("="),&vecCustomID[iPos]))
								//当前标准中有这个管部
								iTimes=-1;
							else
								//当前标准中没有这个管部
								iTimes=0;
						}
						else
						{
							//sTmp.Format(GetResStr(IDS_NoRecordFLDEqvThisValueInPictureClipData), _T("ID"), Ptype);
							msNamePA=_T("");
							iTimes=0;
						}
						iNum++;
						rs2.MoveNext();
						continue;
					}
					else 
					{
						if(stop)
						{
							//根部
							PtypeC=Ptype;
							iter = std::find(vecID.begin(), vecID.end(), vTmp);
							if(iter!=vecID.end())
							{
								iPos = (iter - vecID.begin());
								msNameSA = vecDescription[iPos];
								if(rsSA.Seek(_T("="),&vecCustomID[iPos]))
									//当前标准中有这个管部
									iTimes=iTimes*(-1);
								else
									//当前标准中没有这个管部
									iTimes=0;
							}
							else
							{
								//sTmp.Format(GetResStr(IDS_NoRecordFLDEqvThisValueInPictureClipData), _T("ID"), Ptype);
								msNameSA=_T("");
								iTimes =0;
							}
							
							if( iNumSPR > 0 )
							{
								strTmpName = msNamePA + modPHScal::gsPhsTypeSPR + msNameSA;
							}
							else
							{
								if( iNumCSPR == 0 )
								{
									strTmpName = msNamePA + modPHScal::gsPhsTypeRigid + msNameSA;
								}
								else
								{
									strTmpName = msNamePA + modPHScal::gsPhsTypeCSPR + msNameSA;
								}
							}
							
							iNum++;
							break;
						}
						else
						{
							//其它中间连接零件
							iter = std::find(vecID.begin(), vecID.end(), vTmp);
							if(iter!=vecID.end())
							{
								iPos = (iter - vecID.begin());
								int ix = vecClassID[iPos];
								if(ix==iSPR)
								{
									iNumSPR++;
									if(rsSP.Seek(_T("="),&vecCustomID[iPos])) //当前标准中有这个弹簧 
										iTimes =iTimes*(-1);
									else
										//当前标准中没有这个弹簧
										iTimes =0;
								}
								else if(ix==iCSPR)
								{
									iNumCSPR++;
									if(rsCS.Seek(_T("="),&vecCustomID[iPos]))
										//当前标准中有这个恒力弹簧
										iTimes =iTimes*(-1);
									else
										//当前标准中没有这个恒力弹簧
										iTimes =0;
								}
								else if(ix==iROD)
								{
									iNumRod++;
								}
								else
								{
									if(rsPR.Seek(_T("="),&vecCustomID[iPos]))										//当前标准中有这个连接件
										iTimes =iTimes*(-1);
									else
										//当前标准中没有这个连接件
										iTimes =0;
								}
								
							}
							else
							{
								iTimes =0;
							}
							iNum++;
							rs2.MoveNext();
							continue;
						}
					}

				}while(!stop);

				
				//开始改模板名称
				//这会清除模板管理表中的使用频度信息

				EDIBgbl::SQLx=_T("SampleID=")+sTmp1;
				
				if(!rs3.FindFirst(EDIBgbl::SQLx))
				{
					rs3.AddNew();

					rs3.SetFieldValue(_T("SampleID"),_variant_t(SampleID));
					sTmp1.Format(_T("%05d"),SampleID);
					rs3.SetFieldValue(_T("BmpName"),STR_VAR(sTmp1));					

					if(strTmpName.GetLength() > fdL)						
						strTmp=strTmpName.Left(fdL);
					else
						strTmp=strTmpName;

					rs4.AddNew();
					rs4.SetFieldValue(_T("SampleID"), COleVariant(SampleID));
					rs4.SetFieldValue(_T("BmpName"), COleVariant(sTmp1, VT_BSTRT));
					rs4.SetFieldValue(_T("SampleName"), COleVariant(strTmp, VT_BSTRT));
					rs4.SetFieldValue(_T("PA"), COleVariant(Ptype0, VT_BSTRT));
					rs4.SetFieldValue(_T("SA"), COleVariant(PtypeC, VT_BSTRT));
					rs4.SetFieldValue(_T("SPR"), COleVariant(sSPR, VT_BSTRT));
					rs4.SetFieldValue(_T("iNumOfPart"), COleVariant((long)iNum));
					rs4.SetFieldValue(_T("iNumSPR"), COleVariant(iNumSPR));
					rs4.SetFieldValue(_T("iNumCSPR"), COleVariant(iNumCSPR));
					rs4.SetFieldValue(_T("iNumRod"), COleVariant(iNumRod));
					rs4.SetFieldValue(_T("bAllowUse"), COleVariant((long)iTimes));
					rs4.Update();

				}
				else
				{
					rs3.Edit();

					rs4.FindFirst(EDIBgbl::SQLx);

					if(strTmpName.GetLength() > fdL)
						strTmp=strTmpName.Left(fdL);
					else
						strTmp=strTmpName;
					rs4.Edit();
					rs4.SetFieldValue(_T("SampleName"), COleVariant(strTmp, VT_BSTRT));
					rs4.SetFieldValue(_T("PA"), COleVariant(Ptype0, VT_BSTRT));
					rs4.SetFieldValue(_T("SA"), COleVariant(PtypeC, VT_BSTRT));
					rs4.SetFieldValue(_T("SPR"), COleVariant(sSPR, VT_BSTRT));
					rs4.SetFieldValue(_T("iNumOfPart"), COleVariant((long)iNum));
					rs4.SetFieldValue(_T("iNumSPR"), COleVariant(iNumSPR));
					rs4.SetFieldValue(_T("iNumCSPR"), COleVariant(iNumCSPR));
					rs4.SetFieldValue(_T("iNumRod"), COleVariant(iNumRod));
					rs4.SetFieldValue(_T("bAllowUse"), COleVariant((long)iTimes));
					rs4.Update();

				}
				{
					//如果不使用条件，则名称为空
					if(strTmpName.GetLength()>0 )
					{
						//rs3->Fields->Item[_variant_t(_T("SampleName"))]->get_DefinedSize(&fdL);
						if(strTmpName.GetLength() > fdL)
							strTmp=strTmpName.Left(fdL);
						else
							strTmp=strTmpName;
						rs3.SetFieldValue(_T("SampleName"),STR_VAR(strTmp)); 
					}
					if(Ptype0.GetLength()>0)
						rs3.SetFieldValue(_T("PA"), STR_VAR(Ptype0));
					if(PtypeC.GetLength()>0)
						rs3.SetFieldValue(_T("SA"),STR_VAR(PtypeC));
					if(sSPR.GetLength()>0)
						rs3.SetFieldValue(_T("SPR"),STR_VAR( sSPR));
					rs3.SetFieldValue(_T("iNumOfPart"),_variant_t((long)iNum));
					rs3.SetFieldValue(_T("iNumSPR"),_variant_t(iNumSPR));
					rs3.SetFieldValue(_T("iNumCSPR"),_variant_t(iNumCSPR));
					rs3.SetFieldValue(_T("iNumRod"),_variant_t(iNumRod));
					//模板是否允许使用
					rs3.SetFieldValue(_T("bAllowUse"),_variant_t((bool) iTimes));
					rs3.Update();
//					EXECUTE_TIME_END
				}     
				sTmp1.Format(GetResStr(IDS_ChangeNoXNameInphsStructureName),(_T("\%d")),(_T("\%d")),strTmpName);
				frmStatus.m_Label2.Format(sTmp1, IC2, oldSampleID);
				frmStatus.UpdateData(false);
				frmStatus.UpdateStatus( (float)oldSampleID/(float)IC2, TRUE);
				frmStatus.UpdateWindow();
				rs2.MoveNext();
			}
			rs2.Close();
			rs3.Close();
			rs4.Close();
		}
	}
catch(CDaoException * e)
{
	e->ReportError();
	e->Delete();
}
//errH:
AfxGetApp()->EndWaitCursor();
if(rs2.IsOpen())
rs2.Close();
if(rs3.IsOpen())
rs3.Close();
//	timee=CTime::GetCurrentTime();
//CTimeSpan t=timee-timeb;
//CString s=t.Format(_T("%M分%S秒"));
//s=_T("共用时:")+s;
//ShowMessage(s);
frmStatus.ShowWindow(SW_HIDE);
}


#pragma warning( disable : 4129 )  //Added by Shuli Luo
CDaoRecordset* Cphs::AvailableSampleIDrsForphsSamp(CString strOrderByDesc,CString strFilter)
{
	//可用于frmphsSamp窗体的数据控件选择数据
	CString tmpSQL;
	static CDaoRecordset ret;
	if(ret.IsOpen())
		ret.Close();
	ret.m_strFilter=_T("");
	ret.m_strSort=_T("");
	ret.m_pDatabase=&EDIBgbl::dbPRJ;
	tmpSQL=CString((_T("SELECT * FROM phsStructureName ")));
	if(Trim(strFilter)!=_T(""))
		tmpSQL +=_T("WHERE ") + strFilter ;
	if(Trim(SortFieldName) !=_T(""))
		tmpSQL+=CString((_T(" ORDER BY "))) + (_T("  [")) + Trim(SortFieldName) + (_T("] ")) + strOrderByDesc;
	ret.Open(dbOpenDynaset,tmpSQL);
	return & ret;
}
#pragma warning( disable : 4129 )  //Added by Shuli Luo
void Cphs::GetMaterial()
{
	//材料选择规范表SpecificationOfMaterial位于sort.mdb。
	//该表含有类别、上下限温度、上下限压力(可用于汽水管道材料选择)、序号等，
	//根据该表及小序号优先原则，以及默认材料，和环境计算温度te(C,即冬季空气调节室外计算温度GBJ19,火力发电厂汽水管道设计技术规定DL/T5054-1996,P76,第7.5.2.2条)，可唯一确定支吊架管道
	try
	{
		CString sTmp;
		COleVariant vTmp;
		CDaoRecordset rs(&EDIBgbl::dbPHScode);//20071018 "dbSORT" 改为 "dbPHScode"
		//材料规范选择表SpecificationOfMaterial
		CString SQLx ;
		SQLx.Format(_T("SELECT * FROM SpecificationOfMaterial WHERE ClassIndex=%d AND ID=\'default\' AND tmin<=%g AND %g<tmax ORDER BY tmin,tmax,SEQ"),modPHScal::giClassIndex,modPHScal::gnConnectTJ,modPHScal::gnConnectTJ);
		rs.Open(dbOpenSnapshot,SQLx);
		if(rs.IsEOF() && rs.IsBOF())
		{
			//在%s库%s材料选择规范表没有%s字段值为默认值%s的记录(非管部零件材料选择规则)
			sTmp.Format(IDS_NoDefaultInCustomIDInSpecificationOfMaterial,rs.m_pDatabase->GetName(),_T("SpecificationOfMaterial"),_T("ID"),_T("default"));
			throw sTmp;
		}
		rs.MoveFirst();
		//所有不是管部、编号不为null、且材料尚无值的零件,其材料都可由环境计算温度查出
		//用户添加的材料bUserAdd!=0，编号为null
		SQLx=CString(_T("UPDATE ["))  
			+ EDIBgbl::Btype[EDIBgbl::TZB] 
			+ _T("] SET CLcl=\'");
		rs.GetFieldValue(_T("Material"),vTmp);
		SQLx+=vtos(vTmp);
		SQLx+=_T("\' WHERE zdjh=") ;
		sTmp.Format(_T("%d"),modPHScal::zdjh);
		SQLx+=sTmp 
			+ _T(" AND VolumeID=")
			+ ltos(EDIBgbl::SelVlmID) 
			+ _T("  AND ( ClassID<>");
		sTmp.Format(_T("%d"),iPAfixZ1);
		SQLx+=sTmp
			+ _T(" AND ClassID<>");
		sTmp.Format(_T("%d"),iPAfixZ2);
		SQLx+=sTmp
			+_T(" AND ClassID<>");
		sTmp.Format(_T("%d"),iPAs);
		SQLx+=sTmp
			+_T(" AND ClassID<>");
		sTmp.Format(_T("%d"),iPAh);
		SQLx+=sTmp		  
			+_T(") AND (ISNULL(CLcl) or trim(CLcl)=\'\') ");//AND NOT ISNULL(seq)";
		EDIBgbl::dbPRJDB.Execute(SQLx); 
	}
	catch(CDaoException *e)
	{
		e->ReportError();
		e->Delete();
	}
	
}
#pragma warning( disable : 4129 )  //Added by Shuli Luo
void Cphs::GetphsSEQ(_RecordsetPtr /*ByVal*/ rsza)
{
	//目的：生成零件编号,同类零件具有相同编号
	//input:  dbsort中的表CLtemperature,
	//        dbprjdb中的F####?-JZB,
	//output:
	//        dbprj中的TmpCLgroup,
	try
	{
		CDaoRecordset rs, rsTmp2,rsTmpZB;
		CDaoRecordset rsCL, rsTmpCLgroup;
		long i;
		//		bool bFoundSA;// As Boolean
		long mvIndex =0; //索引号
		CString tbn , tmpBlkID , tmpID ;
		CString mvCL, sGROUPBY ,tmpStr;
		CString *Ptype=NULL;
		//		float sTmp=0, sngSumWeight=0;
		
		if( EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("TmpCLgroup")))
			//表TmpCLgroup用于保存分组结果
			EDIBgbl::dbPRJ.Execute( _T("DROP TABLE TmpCLgroup"));
		//生成一个空表
		sGROUPBY = _T("CLgg,CLcl,CLmc,CLID,CLdw,CLdz");
		EDIBgbl::dbPRJDB.Execute(_T("SELECT ") + sGROUPBY + _T(" INTO TmpCLgroup IN \'") + EDIBgbl::dbPRJ.GetName() + _T("\' FROM [") + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] WHERE zdjh=-1"));
		//GROUP BY子句的字段必须包含SELECT字句的全部字段,
		//但是seq,CLnum不能被分组,故要另外加入seq,CLnum字段到临时表
		EDIBgbl::dbPRJ.Execute(_T("ALTER TABLE TmpCLgroup ADD COLUMN seq short"));
		EDIBgbl::dbPRJ.Execute(_T("ALTER TABLE TmpCLgroup ADD COLUMN CLnum SINGLE"));
		//排序、分组统计零件，
		EDIBgbl::dbPRJDB.Execute (_T("INSERT INTO TmpCLgroup IN \'") + EDIBgbl::dbPRJ.GetName() + _T("\' SELECT DISTINCT ") + sGROUPBY + _T(" FROM [") + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" AND zdjh=") + ltos(modPHScal::zdjh) + _T(" AND NOT ISNULL(seq)  GROUP BY ") + sGROUPBY);
		
		sGROUPBY = _T("seq,CLgg,CLcl,CLmc,CLID,CLdw,CLdz,CLnum,CLzz,CLbz");
		
		if( EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("Tmp2")))
			EDIBgbl::dbPRJ.Execute(_T("DROP TABLE tmp2"));
		CString SQLx;
		//建立空表tmp2
		SQLx = _T("SELECT ") + sGROUPBY + _T(" INTO tmp2 IN \'") + EDIBgbl::dbPRJ.GetName() + _T("\' FROM [") + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] WHERE zdjh=-1");
		EDIBgbl::dbPRJDB.Execute( SQLx);
		SQLx = _T("DELETE FROM tmp2");
		EDIBgbl::dbPRJ.Execute(SQLx);
		rsTmp2.m_pDatabase=&EDIBgbl::dbPRJ;
		rsTmp2.Open(dbOpenDynaset,_T("SELECT * FROM tmp2") );
		
		//将支吊架材料汇总功能转移到单独的过程中GetPhsBOM，以便材料统计的选项改变之后，能够正确生成材料汇总表
		//首先删除本支吊架材料数据
		SQLx = _T("DELETE * FROM [") +EDIBgbl:: Btype[EDIBgbl::TCL] + _T("] WHERE zdjh=") + ltos(modPHScal::zdjh) + _T(" AND VolumeID=") + ltos(EDIBgbl::SelVlmID);
		EDIBgbl::dbPRJDB.Execute(SQLx);
		SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TCL] + _T("] WHERE zdjh=") + ltos(modPHScal::zdjh) + _T(" AND VolumeID=") + ltos(EDIBgbl::SelVlmID);
		rsCL.m_pDatabase=&EDIBgbl::dbPRJDB;
		rsCL.Open(dbOpenDynaset,SQLx);
		
		//并且从管部到根部依次给零件编号
		//同时生成零件明细表到tmp2和材料汇总表到F????S-JCL
		SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] WHERE zdjh=") + ltos(modPHScal::zdjh) + _T(" AND VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" AND NOT ISNULL(seq) ORDER BY recno");
		if(rsTmpZB.IsOpen())
			rsTmpZB.Close();
		rsTmpZB.m_pDatabase=&EDIBgbl::dbPRJDB;
		rsTmpZB.Open(dbOpenDynaset,SQLx);
		if(rsTmpZB.IsEOF() && rsTmpZB.IsBOF())
		{
			//如果结构表没有任何零件
			tmpStr.Format(GetResStr(IDS_NoRecordInTZB),EDIBgbl::dbPRJDB.GetName(),EDIBgbl::Btype[EDIBgbl::TZB],
				EDIBgbl::SelJcdm,ltos(modPHScal::zdjh));
			throw tmpStr;
		}
		i = 1;
		COleVariant vTmp;
		rsTmpCLgroup.m_pDatabase=&EDIBgbl::dbPRJ;
		COleVariant nil;
		nil.ChangeType(VT_NULL);
		while(!rsTmpZB.IsEOF())
		{
			CString CLgg,CLcl,CLmc;
			rsTmpZB.GetFieldValue(_T("CLgg"),vTmp);
			CLgg=vtos(vTmp);
			rsTmpZB.GetFieldValue(_T("CLcl"),vTmp);
			CLcl=vtos(vTmp);
			rsTmpZB.GetFieldValue(_T("CLmc"),vTmp);
			CLmc=vtos(vTmp);
			//空的seq字段，写入编号给它
			SQLx=_T("UPDATE tmpCLgroup SET seq=") + ltos(i) + _T(" WHERE ISNULL(seq) AND trim(CLgg)=\'");
			SQLx+=CLgg+_T("\' AND trim(CLcl)=\'");
			SQLx+=CLcl+_T("\' AND (trim(CLmc))=\'") ;
			SQLx+=CLmc+_T("\'");
			EDIBgbl::dbPRJ.Execute(SQLx);
			//Debug.Print dbPRJ.RecordsAffected, i, rsTmpZB(_T("CLmc")), rsTmpZB(_T("CLgg")), rsTmpZB(_T("CLcl"))
			//SQLx = _T("SELECT seq FROM tmpCLgroup INNER JOIN [") + EDIBgbl::TBNSelPrjSpec + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] AS tbn ON  WHERE ISNULL(seq) AND trim(CLgg)=\'")
			//	 +CLgg + _T("\' AND trim(CLcl)=\'") +CLcl + _T("\' AND (trim(CLmc))=\'") +CLmc+ _T("\' ");
			// SQLx = _T("SELECT * FROM tmpCLgroup WHERE NOT ISNULL(seq) AND trim(CLgg)='") & Trim(rsTmpZB.Fields(_T("CLgg"))) & _T("' AND trim(CLcl)='") & Trim(rsTmpZB.Fields(_T("CLcl"))) & _T("' AND (trim(CLmc))='") & Trim(rsTmpZB.Fields(_T("CLmc"))) & _T("' ")
			SQLx = _T("SELECT seq FROM tmpCLgroup WHERE NOT ISNULL(seq) AND trim(CLgg)=\'")
				+ CLgg+ _T("\' AND trim(CLcl)=\'") +CLcl+ _T("\' AND (trim(CLmc))=\'") +CLmc+ _T("\' ");
			rsTmpCLgroup.Open(dbOpenSnapshot,SQLx);
			if( rsTmpCLgroup.IsBOF() && rsTmpCLgroup.IsEOF())
			{
				tmpStr.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb),EDIBgbl::dbPRJ.GetName(),_T("tmpCLgroup"),SQLx);
				throw tmpStr;
			}
			rsTmpCLgroup.MoveLast();
			//有记录受到影响，编号递增。
			if(EDIBgbl::dbPRJ.GetRecordsAffected()>0)//返回最后一次更新、编辑、增加操作或Execute调用所影响的记录数
				i++;
			rsTmpZB.GetFieldValue(_T("CustomID"),vTmp);
			if(vtoi(vTmp)!= iGCement)
			{
				//不是混凝土梁
				//不是所有的零件都写入了索引号,对于索引号为空的零件，需要查表
				mvIndex = vtoi(modPHScal::sFindFLD(_T("CustomID"), _T("Index"), vtos(vTmp)));
				rsTmpZB.Edit();
				rsTmpCLgroup.GetFieldValue(_T("seq"),vTmp);
				if(mvIndex != iSA)
				{
					//不是根部
					rsTmpZB.SetFieldValue(_T("seq"),vTmp);
				}
				else
				{
					if(!vtob(rsza->GetCollect(_T("bNotSumSA"))))
					{
						//是根部,要统计根部材料
						rsTmpZB.SetFieldValue(_T("seq"),vTmp);
					}
					else
						//是根部,不要统计根部材料
						rsTmpZB.SetFieldValue(_T("seq"),nil);
				}
				rsTmpZB.Update();
				
				//写零件明细表Tmp2
				rsTmp2.AddNew();
				rsTmpZB.GetFieldValue(_T("seq"),vTmp);
				rsTmp2.SetFieldValue(_T("seq"),vTmp);
				rsTmpZB.GetFieldValue(_T("CLID"),vTmp);
				rsTmp2.SetFieldValue(_T("CLID"),vTmp);
				rsTmpZB.GetFieldValue(_T("CLmc"),vTmp);
				rsTmp2.SetFieldValue(_T("CLmc"),vTmp);
				rsTmpZB.GetFieldValue(_T("CLgg"),vTmp);
				rsTmp2.SetFieldValue(_T("CLgg"),vTmp);
				rsTmpZB.GetFieldValue(_T("CLcl"),vTmp);
				rsTmp2.SetFieldValue(_T("CLcl"),vTmp);
				rsTmpZB.GetFieldValue(_T("CLdw"),vTmp);
				rsTmp2.SetFieldValue(_T("CLdw"),vTmp);
				rsTmpZB.GetFieldValue(_T("CLdz"),vTmp);
				rsTmp2.SetFieldValue(_T("CLdz"),vTmp);
				rsTmpZB.GetFieldValue(_T("CLnum"),vTmp);
				rsTmp2.SetFieldValue(_T("CLnum"),vTmp);
				rsTmpZB.GetFieldValue(_T("CLzz"),vTmp);
				rsTmp2.SetFieldValue(_T("CLzz"),vTmp);
				rsTmpZB.GetFieldValue(_T("CLbz"),vTmp);
				rsTmp2.SetFieldValue(_T("CLbz"),vTmp);
				rsTmp2.Update();
			}
			rsTmpZB.MoveNext();
			rsTmpCLgroup.Close();
		}
		}
		catch(CDaoException * e)
		{
			e->ReportError();
			e->Delete();
		}
		catch(_com_error e)
		{
			ShowMessage(e.Description());
		}
		catch(CString e)
		{
			ShowMessage(e);
		}
}
#pragma warning( disable : 4129 )  //Added by Shuli Luo
void Cphs::GetphsSumBom()
{
	//目的：生成包括用户添加的材料在内的材料汇总表（根据变量gbSumAllVolume决定是单个卷册或所有卷册）
	//处理：按拉杆、根部、根部附件、管部或其它连接件分类计算。
	//input:  rzTZB
	//        dbPrjDB中的ZB,
	//   ---生成所有或某个卷册支吊架的材料汇总表:
	//   ---直接从ZB表中提取材料数据,其数量*表ZA中的制作套数=材料汇总表中的数量,写入AllPrjDb.mdb中的当前卷册的CL表.
	//	  ---材料数量的计算原则如下：
	//	如果是recno=null,
	//		如果是用户添加的材料，材料数量=用户输入的数量；
	//		否则，
	//			如果支吊架计算已成功
	//			否则支吊架计算未成功，
	//				如果根部材料要统计
	//					材料数量=根部数量Gnum字段*本支吊架数量Num字段；
	//				否则根部材料不统计
	//					材料数量=0。
	//	否则不是recno=null,
	//   ---
	//output:结果写到dbprjdb中的TCL表CL
	//   ---同时，分类汇总排序后的结果写入Tmp2，以便其它过程输出
	//
	try
	{
		CDaoRecordset rs , rsza, rsCL ;
		COleVariant v;
		float sTmp=0, sngSumWeight;
		long m_iNum=0,m_iCLNum=0,m_iSANum=0,m_iL1=0;
		bool m_bNotSumSA=false;
		bool m_bSuccess=false;
		bool m_bUserAdd=false;
		CString strTmp;
		CString mvCL ;
		CString sGROUPBY;
		long iSEQ=0;  //编号
		long iSEQsub=0; //子编号
		CString SQLx;
		SQLx.Format(_T("DELETE FROM [%s]  tb1 WHERE NOT EXISTES ( SELECT ZDJH FROM [%s]  tb2 WHERE tb1.VolumeID=tb2.VolumeID AND tb1.ZDJH=tb2.ZDJH )"),
			EDIBgbl::Btype[EDIBgbl::TZB],EDIBgbl::Btype[EDIBgbl::TZA]);
		EDIBgbl::dbPRJDB.Execute(SQLx);
		//确定要输出的字段:输出字段信息可以从TableINFO表和t??表获得,并且可以用户化
		sGROUPBY = _T("seq,CLgg,CLcl,CLmc,CLID,CLdw,CLdz,CLbz");
		//在dbprj中建立临时分类材料汇总表
		if( EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("TmpTCL")) )
		{
			//WorkPrj.mdb中的表TmpTCL用于保存分类汇总的中间结果
			EDIBgbl::dbPRJ.Execute( _T("DROP TABLE TmpTCL"));
		}
		//拷贝TCL到TmpTCL，以便可以删除其中的选择过的材料。
		SQLx = _T("SELECT * INTO TmpTCL IN \'") + EDIBgbl::dbPRJ.GetName() + _T("\' FROM [") + EDIBgbl::Btype[EDIBgbl::TCL] + _T("] WHERE VolumeID=-1");
		EDIBgbl::dbPRJDB.Execute( SQLx);
		if(!rsID.IsOpen())
		{
			EDIBgbl::SQLx = _T("SELECT * FROM PictureClipData");
			rsID.m_pDatabase=&EDIBgbl::dbPRJ;
			rsID.Open(dbOpenDynaset,EDIBgbl::SQLx);
			brsIDStatus=TRUE;
		}
		//在dbprj中生成空的Tmp2表
		if( EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("Tmp2")) )
		{
			//表TmpCLgroup用于保存分组结果
			EDIBgbl::dbPRJ.Execute( _T("DROP TABLE Tmp2"));
		}
		CString strOrderBy;
		strOrderBy = sGROUPBY;
		//生成空的Tmp2表
		EDIBgbl::dbPRJDB.Execute( CString(_T("SELECT ")) + sGROUPBY + _T(",CLnum,CLnum*CLdz AS CLzz INTO Tmp2 IN \'") + EDIBgbl::dbPRJ.GetName() + _T("\' FROM [") + EDIBgbl::Btype[EDIBgbl::TCL] + _T("] WHERE zdjh=0 AND VolumeID=-1"));
		rs.m_pDatabase=&EDIBgbl::dbPHScode;//20071101 "dbSORT" 改为 "dbPHScode"
		rs.Open(dbOpenSnapshot,_T("SELECT * FROM BomName ORDER BY BomIndex"));
		COleVariant vTmp,vTmp1,vTmp2;
		while(!rs.IsEOF())
		{
			rs.GetFieldValue(_T("BomTable"),vTmp);
			if(vTmp.vt!=VT_NULL)
			{
				strTmp =(_T("TMP") + vtos(vTmp));
				strTmp.MakeUpper();
				if( strTmp == _T("TMP2") )
				{
					//防止删除Tmp2,因为它是材料汇总表
				}
				else
				{
					//建立所有的部件分类汇总表
					if( EDIBgbl::tdfExists(EDIBgbl::dbPRJ, strTmp) )
					{
						//表TmpCLgroup用于保存分组结果
						EDIBgbl::dbPRJ.Execute( _T("DROP TABLE [") + strTmp + _T("]"));
					}
					//复制tmp2的结构到各个表
					EDIBgbl::dbPRJ.Execute( _T("SELECT * INTO [") + strTmp + _T("] FROM Tmp2"));
				}
			}
			rs.MoveNext();
		}
		rs.Close();   
		
		//将支吊架材料汇总功能转移到单独的过程中GetPhsSumBOM，以便材料统计的选项改变之后，能够正确生成材料汇总表
		//首先删除本支吊架材料数据
		//strSelectVolume =(modPHScal::gbSumAllVolume ? (_T("")) : (_T(" VolumeID=\'") + EDIBgbl::SelVlmID + _T("\'")));
		SQLx = _T("DELETE FROM [") + EDIBgbl::Btype[EDIBgbl::TCL] + _T("] ") ;
		if(modPHScal::gbSumAllVolume)
		{
			SQLx +=_T(" WHERE VolumeID IN ( SELECT VolumeID FROM Volume WHERE EnginID=\'") + EDIBgbl::SelPrjID + _T("\' AND ZYID = ") + ltos(EDIBgbl::SelSpecID) + _T(" AND SJJDID = ") + ltos(EDIBgbl::SelDsgnID) + _T(" AND SJHYID = ") + ltos(EDIBgbl::SelHyID) +_T(") ");
		}
		else
		{
			SQLx +=_T(" WHERE VolumeID =") + ltos(EDIBgbl::SelVlmID);
		}
		EDIBgbl::dbPRJDB.Execute( SQLx);
		
		//统计材料之前，自动将未计算成功的支吊架且不是用户加的材料剔除
		//首先，选择所有没有计算成功的支吊架
		SQLx=_T("DELETE FROM [") + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] WHERE (bUserAdd IS NULL OR bUserAdd =0) AND NOT EXISTS ( SELECT VolumeID,Zdjh FROM ") +
			EDIBgbl::Btype[EDIBgbl::TZA] + _T(" WHERE [") + EDIBgbl::Btype[EDIBgbl::TZA] + _T("].VolumeID=[")
			+ EDIBgbl::Btype[EDIBgbl::TZB] + _T("].VolumeID AND [")+ EDIBgbl::Btype[EDIBgbl::TZA] + _T("].Zdjh = [")
			+ EDIBgbl::Btype[EDIBgbl::TZB] + _T("].Zdjh ) OR EXISTS  ( SELECT VolumeID,Zdjh FROM [") 
			+ EDIBgbl::Btype[EDIBgbl::TZA] +_T("] ")
			+ _T(" WHERE [") + EDIBgbl::Btype[EDIBgbl::TZB] + _T("].VolumeID=[") + EDIBgbl::Btype[EDIBgbl::TZA] + _T("].VolumeID ")
			+ _T(" AND  [") + EDIBgbl::Btype[EDIBgbl::TZB] + _T("].zdjh=[") + EDIBgbl::Btype[EDIBgbl::TZA] + _T("].zdjh AND [")
			+ EDIBgbl::Btype[EDIBgbl::TZA] + _T("].bCalSuccess=0 ");
		if(modPHScal::gbSumAllVolume )
		{
			SQLx +=_T(" AND [") + EDIBgbl::Btype[EDIBgbl::TZA] + _T("].VolumeID IN ( SELECT VolumeID FROM Volume WHERE EnginID=\'") + EDIBgbl::SelPrjID + _T("\' AND ZYID = ") + ltos(EDIBgbl::SelSpecID) + _T(" AND SJJDID = ") + ltos(EDIBgbl::SelDsgnID) + _T(" AND SJHYID = ") + ltos(EDIBgbl::SelHyID) +_T(")) ");
		}
		else
		{
			SQLx +=_T(" AND [") + EDIBgbl::Btype[EDIBgbl::TZA] + _T("].VolumeID =") + ltos(EDIBgbl::SelVlmID) + _T(")");
		}
		SQLx+=_T(" AND ( bUserAdd is  null OR bUserAdd =0 )");
		EDIBgbl::dbPRJDB.Execute( SQLx);
		
		//正式开始汇总材料
		SQLx = _T("SELECT * FROM TmpTCL");
		rsCL.m_pDatabase=&EDIBgbl::dbPRJ; 
		rsCL.Open(dbOpenDynaset,_T("SELECT * FROM TmpTCL") );
		
		//首先计算总的要处理的记录数
		//总的要处理的记录数量，用于进程条显示。
		long iSum=0,iTmp=0;
		
		//恒力弹簧
		SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] ");
		if(modPHScal::gbSumAllVolume )
		{
			SQLx +=_T(" WHERE VolumeID IN ( SELECT VolumeID FROM Volume WHERE EnginID=\'") + EDIBgbl::SelPrjID + _T("\' AND ZYID = ") + ltos(EDIBgbl::SelSpecID) + _T(" AND SJJDID = ") + ltos(EDIBgbl::SelDsgnID) + _T(" AND SJHYID = ") + ltos(EDIBgbl::SelHyID) +_T(") AND ClassID=") + ltos(iCSPR);
		}
		else
		{
			SQLx +=_T(" WHERE VolumeID =") + ltos(EDIBgbl::SelVlmID) +_T(" AND ClassID=") + ltos(iCSPR);
		}
		rs.m_pDatabase=&EDIBgbl::dbPRJDB;
		rs.Open(dbOpenDynaset,SQLx );
		if( rs.IsEOF() && rs.IsBOF() )
		{
			
		}
		else
		{
			rs.MoveLast();
			iSum+=rs.GetRecordCount();
		}
		rs.Close();
		
		//现在处理弹簧
		SQLx = _T("SELECT * FROM [") +  EDIBgbl::Btype[EDIBgbl::TZB] + _T("] ");
		if(modPHScal::gbSumAllVolume)
		{
			SQLx +=_T(" WHERE VolumeID IN ( SELECT VolumeID FROM Volume WHERE EnginID=\'") + EDIBgbl::SelPrjID + _T("\' AND ZYID = ") + ltos(EDIBgbl::SelSpecID) + _T(" AND SJJDID = ") + ltos(EDIBgbl::SelDsgnID) + _T(" AND SJHYID = ") + ltos(EDIBgbl::SelHyID) +_T(") AND ClassID=") + ltos(iSPR);
		}
		else
		{
			SQLx +=_T(" WHERE VolumeID =") + ltos(EDIBgbl::SelVlmID) + _T(" AND ClassID=") + ltos(iSPR);
		}
		rs.m_pDatabase = &EDIBgbl::dbPRJDB;
		rs.Open(dbOpenDynaset,SQLx );
		if( rs.IsEOF() && rs.IsBOF() ) 
		{	}
		else
		{
			rs.MoveLast();
			iSum+=rs.GetRecordCount();
		}
		rs.Close();
		
		//现在处理管部
		SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] ");
		if(modPHScal::gbSumAllVolume)
		{
			SQLx +=_T(" WHERE VolumeID IN ( SELECT VolumeID FROM Volume WHERE EnginID=\'") + EDIBgbl::SelPrjID + _T("\' AND ZYID = ") + ltos(EDIBgbl::SelSpecID) + _T(" AND SJJDID = ") + ltos(EDIBgbl::SelDsgnID) + _T(" AND SJHYID = ") + ltos(EDIBgbl::SelHyID) +_T(") ");
		}
		else
		{
			SQLx +=_T(" WHERE VolumeID =") + ltos(EDIBgbl::SelVlmID) ;
		}
		SQLx += _T(" AND ( Index=") + ltos(iPA) + _T(" )");
		rs.m_pDatabase = &EDIBgbl::dbPRJDB;rs.Open(dbOpenDynaset,SQLx);
		if( rs.IsEOF() && rs.IsBOF() )
		{	}
		else
		{
			rs.MoveLast();
			iSum+=rs.GetRecordCount();
		}
		rs.Close();
		
		//现在处理非弹性连接件（不包括拉杆）
		SQLx = CString(_T("SELECT * FROM [")) + EDIBgbl::Btype[EDIBgbl::TZB] + (_T("] ")) ;
		if(modPHScal::gbSumAllVolume)
		{
			SQLx +=_T(" WHERE VolumeID IN ( SELECT VolumeID FROM Volume WHERE EnginID=\'") + EDIBgbl::SelPrjID + _T("\' AND ZYID = ") + ltos(EDIBgbl::SelSpecID) + _T(" AND SJJDID = ") + ltos(EDIBgbl::SelDsgnID) + _T(" AND SJHYID = ") + ltos(EDIBgbl::SelHyID) +_T(")");
		}
		else
		{
			SQLx +=_T(" WHERE VolumeID =") + ltos(EDIBgbl::SelVlmID);
		}
		SQLx+=_T(" AND ( Index=") + ltos(iConnectPART) + _T(" AND ClassID<>") + ltos(iROD) + _T(" AND ClassID<>") + ltos(iCSPR) + _T(" AND ClassID<>") + ltos(iSPR) + _T(" AND IsSAPart<>-1)");
		rs.m_pDatabase = &EDIBgbl::dbPRJDB;rs.Open(dbOpenDynaset,SQLx);
		if( rs.IsEOF() && rs.IsBOF() ) {	}
		else
		{
			rs.MoveLast();
			iSum+=rs.GetRecordCount();
		}
		rs.Close();
		
		//统计拉杆零件数量
		SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] ");
		if(modPHScal::gbSumAllVolume)
		{
			SQLx +=_T(" WHERE VolumeID IN ( SELECT VolumeID FROM Volume WHERE EnginID=\'") + EDIBgbl::SelPrjID + _T("\' AND ZYID = ") + ltos(EDIBgbl::SelSpecID) + _T(" AND SJJDID = ") + ltos(EDIBgbl::SelDsgnID) + _T(" AND SJHYID = ") + ltos(EDIBgbl::SelHyID) +_T(")");
		}
		else
		{
			SQLx +=_T(" WHERE VolumeID =") + ltos(EDIBgbl::SelVlmID) ;
		}
		SQLx+= _T(" AND ClassID=") + ltos(iROD);
		rs.m_pDatabase = &EDIBgbl::dbPRJDB;rs.Open(dbOpenDynaset,SQLx);
		if( rs.IsEOF() && rs.IsBOF() )
		{	}
		else
		{
			rs.MoveLast();
			//拉杆要处理2遍
			iSum+=2*rs.GetRecordCount();
		}
		rs.Close();
		
		//选择根部附件中的型钢、非螺母螺栓的零件
		SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] ") ;
		if(modPHScal::gbSumAllVolume)
		{
			SQLx +=_T(" WHERE VolumeID IN ( SELECT VolumeID FROM Volume WHERE EnginID=\'") + EDIBgbl::SelPrjID + _T("\' AND ZYID = ") + ltos(EDIBgbl::SelSpecID) + _T(" AND SJJDID = ") + ltos(EDIBgbl::SelDsgnID) + _T(" AND SJHYID = ") + ltos(EDIBgbl::SelHyID) +_T(")");
		}
		else
		{
			SQLx +=_T(" WHERE VolumeID =") + ltos(EDIBgbl::SelVlmID) ;
		}
		SQLx+=_T(" AND  IsSAPart=-1 AND ClassID<>") + ltos(iBolts) + _T(" AND ClassID<>") + ltos(iNuts);		
		rs.m_pDatabase = &EDIBgbl::dbPRJDB;rs.Open(dbOpenDynaset,SQLx);
		if( rs.IsEOF() && rs.IsBOF() )
		{	}
		else
		{
			rs.MoveLast();
			//型钢要处理3遍
			iSum+=3*rs.GetRecordCount();
		}
		rs.Close();
		
		//统计螺栓螺母、附件：编号不为空（为空的属于厂家供货，不要统计），或根部使用的螺栓螺母（当根部需要统计时）
		SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] ") ;
		if(modPHScal::gbSumAllVolume)
		{
			SQLx +=_T(" WHERE VolumeID IN ( SELECT VolumeID FROM Volume WHERE EnginID=\'") + EDIBgbl::SelPrjID + _T("\' AND ZYID = ") + ltos(EDIBgbl::SelSpecID) + _T(" AND SJJDID = ") + ltos(EDIBgbl::SelDsgnID) + _T(" AND SJHYID = ") + ltos(EDIBgbl::SelHyID) +_T(")");
		}
		else
		{
			SQLx +=_T(" WHERE VolumeID =") + ltos(EDIBgbl::SelVlmID) ;
		}
		//AND后的条件必须打括号
		SQLx+=_T(" AND  ((ClassID=") + ltos(iBolts) + _T(" OR ClassID=") + ltos(iNuts) + _T(" OR ClassID=") + ltos(iAttached) + _T(") AND NOT IsNull(SEQ) OR (ClassID=") + ltos(iBolts) + _T(" OR ClassID=") + ltos(iNuts) + _T(") AND IsSAPart=-1) ");
		rs.m_pDatabase = &EDIBgbl::dbPRJDB;rs.Open(dbOpenDynaset,SQLx);
		if( rs.IsEOF() && rs.IsBOF() ) 
		{	}
		else
		{
			rs.MoveLast();
			iSum+=rs.GetRecordCount();
		}
		rs.Close();
		
		//如果要输出根部整体零件表
		if( modPHScal::gbOutputSA )
		{
			//输出根部整体零件表
			SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] ");
			if(modPHScal::gbSumAllVolume)
			{
				SQLx +=_T(" WHERE VolumeID IN ( SELECT VolumeID FROM Volume WHERE EnginID=\'") + EDIBgbl::SelPrjID + _T("\' AND ZYID = ") + ltos(EDIBgbl::SelSpecID) + _T(" AND SJJDID = ") + ltos(EDIBgbl::SelDsgnID) + _T(" AND SJHYID = ") + ltos(EDIBgbl::SelHyID) +_T(")");
			}
			else
			{
				SQLx +=_T(" WHERE VolumeID =") + ltos(EDIBgbl::SelVlmID) ;
			}
			SQLx += _T(" AND  Index=") + ltos(iSA) + _T(" AND ClassID<>") + ltos(iGCement);
			rs.m_pDatabase = &EDIBgbl::dbPRJDB;rs.Open(dbOpenDynaset,SQLx);
			if( rs.IsEOF() && rs.IsBOF() )
			{	}
			else
			{
				rs.MoveLast();
				iSum+=rs.GetRecordCount();
			}
			rs.Close();
		}
		
		//显示进程条
		if(!IsWindow(frmStatus.m_hWnd))
			frmStatus.Create(IDD_STATUS,AfxGetMainWnd());
		//frmStatus.ShowWindow(SW_SHOW);
		//frmStatus.UpdateWindow();
		frmStatus.ShowWindow(SW_SHOW);
		frmStatus.m_Label1 = GetResStr(IDS_SumPhsBom);
		frmStatus.m_Label2 = _T("");
		strTmp.Format(GetResStr(IDS_SumPhsBomTitle),(modPHScal::gbSumAllVolume ? GetResStr(IDS_CURRENT_PROJECT):GetResStr(IDS_CURRENT_VOLUME)));
		frmStatus.SetWindowText(strTmp);
		//frmStatus.Show
		frmStatus.UpdateData(false);
		frmStatus.UpdateStatus(0.0 / iSum,true);
		frmStatus.UpdateWindow();
		iTmp+=1;
		
		//************************
		//统计管部、连接件、弹性件
		//除了根部及根部附件、拉杆之外的其它材料
		//现在处理恒力弹簧
		COleVariant nil;
		nil.ChangeType(VT_NULL);
		SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] ");
		if(modPHScal::gbSumAllVolume )
		{
			SQLx +=_T(" WHERE VolumeID IN ( SELECT VolumeID FROM Volume WHERE EnginID=\'") + EDIBgbl::SelPrjID + _T("\' AND ZYID = ") + ltos(EDIBgbl::SelSpecID) + _T(" AND SJJDID = ") + ltos(EDIBgbl::SelDsgnID) + _T(" AND SJHYID = ") + ltos(EDIBgbl::SelHyID) +_T(") AND ClassID=") + ltos(iCSPR);
		}
		else
		{
			SQLx +=_T(" WHERE VolumeID =") + ltos(EDIBgbl::SelVlmID) +_T(" AND ClassID=") + ltos(iCSPR);
		}
		rs.m_pDatabase=&EDIBgbl::dbPRJDB;
		rs.Open(dbOpenDynaset,SQLx );
		if( rs.IsEOF() && rs.IsBOF() )
		{
		}
		else
		{
			while(!rs.IsEOF())
			{
				frmStatus.m_Label1= GetResStr(IDS_SumPhsBomCSPR);
				strTmp.Format(GetResStr(IDS_SumPhsBomRec),ltos(iSum),ltos(iTmp));
				frmStatus.m_Label2=strTmp;
				frmStatus.UpdateStatus ( (float)iTmp/iSum, true);
				frmStatus.UpdateData(false);
				iTmp+=1;
				
				//SQLx = _T("SELECT * FROM [") +  EDIBgbl::Btype[EDIBgbl::TZA] + _T("] WHERE VolumeID=") + vtos(vTmp1) + _T(" AND zdjh=") + vtos(vTmp2);
				SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZA] + _T("] WHERE VolumeID=") + vtos(GetFields(rs,_T("VolumeID"))) + _T(" AND zdjh=") + vtos(GetFields(rs,_T("zdjh")));
				rsza.m_pDatabase = &EDIBgbl::dbPRJDB;rsza.Open(dbOpenDynaset,SQLx,dbReadOnly);
				if( rsza.IsEOF() && rsza.IsBOF() )
				{
				}
				else
				{
					//iSEQ = iSEQ + 1
					rsCL.AddNew();
					rsCL.SetFieldValue(_T("seq"),nil);
					rs.GetFieldValue(_T("zdjh"),v);
					rsCL.SetFieldValue(_T("zdjh"),v);
					rs.GetFieldValue(_T("VolumeID"),v);
					rsCL.SetFieldValue(_T("VolumeID"),v);
					rs.GetFieldValue(_T("CLID"),v);
					rsCL.SetFieldValue(_T("CLID"),v);
					rs.GetFieldValue(_T("CLmc"),v);
					rsCL.SetFieldValue(_T("CLmc"),v);
					rs.GetFieldValue(_T("CLgg"),v);
					rsCL.SetFieldValue(_T("CLgg"),v);
					rs.GetFieldValue(_T("CLcl"),v);
					rsCL.SetFieldValue(_T("CLcl"),v);
					rs.GetFieldValue(_T("CLdw"),v);
					CString str;
					str.LoadString(IDS_PIECE);
					rsCL.SetFieldValue(_T("CLdw"),(v.vt==VT_NULL) ? STR_VAR(str) : v);
					rs.GetFieldValue(_T("CLdz"),v);
					rsCL.SetFieldValue(_T("CLdz"),v);					
					rs.GetFieldValue(_T("CLzz"),v);
					rsCL.SetFieldValue(_T("CLzz"),v);
					if( EDIBAcad::g_bSumBomSprFactory )//newHS->是否在材料汇总表中标注弹簧/恒力弹簧厂家
					{
						rs.GetFieldValue(_T("CLbz"),v);
						rsCL.SetFieldValue(_T("CLbz"),v);
					}
					rs.GetFieldValue(_T("CLnum"),v);
					rsza.GetFieldValue(_T("Num"),vTmp1);
					rsCL.SetFieldValue(_T("CLnum"),COleVariant(vtof(v)*vtof(vTmp1)));
					rsCL.Update();
				}
				rsza.Close();
				rs.MoveNext();
			}
		}
		rs.Close();
		//立刻将恒力弹簧从TCL分类汇总到Tmp2
		if( EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("TmpCLgroup")) )
		{
			//表TmpCLgroup用于保存分组结果
			EDIBgbl::dbPRJ.Execute( _T("DROP TABLE TmpCLgroup"));
		}
		//rsCL.Close();
		//从tmp2生成一个分组结果TmpCLgroup
		//GROUP BY子句的字段必须包含SELECT字句的全部字段(除了聚合函数的字段)
		EDIBgbl::dbPRJ.Execute( _T("SELECT ") + sGROUPBY + _T(",sum(CLnum) as CLnums,CLnums*CLdz as CLzz INTO TmpCLgroup IN \'") + EDIBgbl::dbPRJ.GetName() + _T("\' FROM TmpTCL GROUP BY ") + sGROUPBY);
		//拷贝tmpCLgroup到tmp2,以便保存分组数据
		EDIBgbl::dbPRJ.Execute( _T("INSERT INTO Tmp2 SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy);
		//将临时材料表的内容拷贝到正式材料表TCL
		EDIBgbl::dbPRJ.Execute( _T("INSERT INTO [") + EDIBgbl::Btype[EDIBgbl::TCL] + _T("] IN \'") + EDIBgbl::dbPRJDB.GetName() + _T("\' SELECT * FROM TmpTCL"));
		//将恒力弹簧的内容拷贝到TmpCSPR表
		EDIBgbl::dbPRJ.Execute( _T("INSERT INTO TmpCSPR SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy);
		//删除临时表
		RsDeleteAll(rsCL);
		//更新临时材料表数据记对象
		//rsCL.Open(dbOpenDynaset,_T("SELECT * FROM TmpTCL") );
		
		//************
		//现在处理弹簧
		SQLx = _T("SELECT * FROM [") +  EDIBgbl::Btype[EDIBgbl::TZB] + _T("] ");
		if(modPHScal::gbSumAllVolume)
		{
			SQLx +=_T(" WHERE VolumeID IN ( SELECT VolumeID FROM Volume WHERE EnginID=\'") + EDIBgbl::SelPrjID + _T("\' AND ZYID = ") + ltos(EDIBgbl::SelSpecID) + _T(" AND SJJDID = ") + ltos(EDIBgbl::SelDsgnID) + _T(" AND SJHYID = ") + ltos(EDIBgbl::SelHyID) +_T(") AND ClassID=") + ltos(iSPR);
		}
		else
		{
			SQLx +=_T(" WHERE VolumeID =") + ltos(EDIBgbl::SelVlmID) + _T(" AND ClassID=") + ltos(iSPR);
		}
		rs.m_pDatabase = &EDIBgbl::dbPRJDB;rs.Open(dbOpenDynaset,SQLx);
		if( rs.IsEOF() && rs.IsBOF() )
		{
		}
		else
		{
			while(!rs.IsEOF())
			{
				frmStatus.m_Label1= GetResStr(IDS_SumPhsBomSPR);
				strTmp.Format(GetResStr(IDS_SumPhsBomRec),ltos(iSum),ltos(iTmp));
				frmStatus.m_Label2=strTmp;
				frmStatus.UpdateStatus ( (float)iTmp/iSum, true);
				frmStatus.UpdateData(false);
				iTmp+=1;
				
				SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZA] + _T("] WHERE VolumeID=") + vtos(GetFields(rs,_T("VolumeID"))) + _T(" AND zdjh=") + vtos(GetFields(rs,_T("zdjh")));
				rsza.m_pDatabase = &EDIBgbl::dbPRJDB;rsza.Open(dbOpenDynaset,SQLx,dbReadOnly);
				if( rsza.IsEOF() && rsza.IsBOF() )
				{
				}
				else
				{
					rsza.MoveFirst();
					rsCL.AddNew();
					rsCL.SetFieldValue(_T("seq"),nil);
					rs.GetFieldValue(_T("zdjh"),v);
					rsCL.SetFieldValue(_T("zdjh"),v);
					rs.GetFieldValue(_T("VolumeID"),v);
					rsCL.SetFieldValue(_T("VolumeID"),v);
					rs.GetFieldValue(_T("CLID"),v);
					rsCL.SetFieldValue(_T("CLID"),v);
					rs.GetFieldValue(_T("CLmc"),v);
					rsCL.SetFieldValue(_T("CLmc"),v);
					rs.GetFieldValue(_T("CLgg"),v);
					rsCL.SetFieldValue(_T("CLgg"),v);
					rs.GetFieldValue(_T("CLcl"),v);
					rsCL.SetFieldValue(_T("CLcl"),v);
					rs.GetFieldValue(_T("CLdw"),v);
					CString str;
					str.LoadString(IDS_PIECE);
					rsCL.SetFieldValue(_T("CLdw"),(v.vt==VT_NULL) ? STR_VAR(str) : v);
					rs.GetFieldValue(_T("CLdz"),v);
					rsCL.SetFieldValue(_T("CLdz"),v);					
					rs.GetFieldValue(_T("CLzz"),v);
					rsCL.SetFieldValue(_T("CLzz"),v);
					if( EDIBAcad::g_bSumBomSprFactory )//newhs->是否在材料汇总表中标注弹簧/恒力弹簧厂家
					{
						rs.GetFieldValue(_T("CLbz"),v);
						rsCL.SetFieldValue(_T("CLbz"),v);
					}
					rs.GetFieldValue(_T("CLnum"),v);
					rsza.GetFieldValue(_T("Num"),vTmp1);
					rsCL.SetFieldValue(_T("CLnum"),COleVariant(vtof(v)*vtof(vTmp1)));
					rsCL.Update();
				}
				rsza.Close();
				rs.MoveNext();
			}
		}
		rs.Close();
		//立刻将弹簧从TCL分类汇总到Tmp2
		if( EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("TmpCLgroup")) )
		{
			//表TmpCLgroup用于保存分组结果
			EDIBgbl::dbPRJ.Execute( _T("DROP TABLE TmpCLgroup"));
		}
		//rsCL.Close();
		//从tmp2生成一个分组结果TmpCLgroup
		//GROUP BY子句的字段必须包含SELECT字句的全部字段(除了聚合函数的字段)
		SQLx =_T("SELECT ") + sGROUPBY + _T(",sum(CLnum) as CLnums,CLnums*CLdz as CLzz INTO TmpCLgroup   FROM TmpTCL GROUP BY ") + sGROUPBY;
		EDIBgbl::dbPRJ.Execute(SQLx);
		//拷贝tmpCLgroup到tmp2,以便保存分组数据
		EDIBgbl::dbPRJ.Execute( _T("INSERT INTO Tmp2 SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy);
		//将弹簧的内容拷贝到TmpSPR表
		EDIBgbl::dbPRJ.Execute( _T("INSERT INTO TmpSPR SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy);
		//将临时材料表的内容拷贝到正式材料表TCL
		EDIBgbl::dbPRJ.Execute( _T("INSERT INTO [") +  EDIBgbl::Btype[EDIBgbl::TCL] + _T("] IN \'") + EDIBgbl::dbPRJDB.GetName() + _T("\' SELECT * FROM TmpTCL"));
		//删除临时表
		RsDeleteAll(rsCL);
		//更新临时材料表数据记对象
		
		
		//************
		//现在处理管部
		SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] ");
		if(modPHScal::gbSumAllVolume)
		{
			SQLx +=_T(" WHERE VolumeID IN ( SELECT VolumeID FROM Volume WHERE EnginID=\'") + EDIBgbl::SelPrjID + _T("\' AND ZYID = ") + ltos(EDIBgbl::SelSpecID) + _T(" AND SJJDID = ") + ltos(EDIBgbl::SelDsgnID) + _T(" AND SJHYID = ") + ltos(EDIBgbl::SelHyID) +_T(") ");
		}
		else
		{
			SQLx +=_T(" WHERE VolumeID =") + ltos(EDIBgbl::SelVlmID) ;
		}
		SQLx += _T(" AND ( Index=") + ltos(iPA) + _T(" )");
		rs.m_pDatabase = &EDIBgbl::dbPRJDB;rs.Open(dbOpenDynaset,SQLx);
		if( rs.IsEOF() && rs.IsBOF() )
		{
		}
		else
		{
			while(!rs.IsEOF())
			{
				frmStatus.m_Label1= GetResStr(IDS_SumPhsBomPA);
				strTmp.Format(GetResStr(IDS_SumPhsBomRec),ltos(iSum),ltos(iTmp));
				frmStatus.m_Label2=strTmp;
				frmStatus.UpdateStatus ( (float)iTmp/iSum, true);
				frmStatus.UpdateData(false);
				iTmp+=1;
				
				SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZA] + _T("] WHERE VolumeID=") + vtos(GetFields(rs,_T("VolumeID"))) + _T(" AND zdjh=") + vtos(GetFields(rs,_T("zdjh")));
				rsza.m_pDatabase = &EDIBgbl::dbPRJDB;rsza.Open(dbOpenDynaset,SQLx,dbReadOnly);
				if( rsza.IsEOF() && rsza.IsBOF() )
				{
				}
				else
				{
					rsza.MoveFirst();
					rsCL.AddNew();
					rsCL.SetFieldValue(_T("seq"),nil);
					rs.GetFieldValue(_T("zdjh"),v);
					rsCL.SetFieldValue(_T("zdjh"),v);
					rs.GetFieldValue(_T("VolumeID"),v);
					rsCL.SetFieldValue(_T("VolumeID"),v);
					rs.GetFieldValue(_T("CLID"),v);
					rsCL.SetFieldValue(_T("CLID"),v);
					rs.GetFieldValue(_T("CLmc"),v);
					rsCL.SetFieldValue(_T("CLmc"),v);
					rs.GetFieldValue(_T("CLgg"),v);
					rsCL.SetFieldValue(_T("CLgg"),v);
					rs.GetFieldValue(_T("CLcl"),v);
					rsCL.SetFieldValue(_T("CLcl"),v);
					rs.GetFieldValue(_T("CLdw"),v);
					CString str;
					str.LoadString(IDS_PIECE);
					rsCL.SetFieldValue(_T("CLdw"),(v.vt==VT_NULL) ? STR_VAR(str) : v);
					rs.GetFieldValue(_T("CLdz"),v);
					rsCL.SetFieldValue(_T("CLdz"),v);					
					rs.GetFieldValue(_T("CLzz"),v);
					rsCL.SetFieldValue(_T("CLzz"),v);
					rs.GetFieldValue(_T("CLbz"),v);
					rsCL.SetFieldValue(_T("CLbz"),v);
					rs.GetFieldValue(_T("CLnum"),v);
					rsza.GetFieldValue(_T("Num"),vTmp1);
					rsCL.SetFieldValue(_T("CLnum"),COleVariant(vtof(v)*vtof(vTmp1)));
					rsCL.Update();
				}
				rsza.Close();
				rs.MoveNext();
			}
		}
		rs.Close();
		//立刻将管部从TCL分类汇总到Tmp2
		if( EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("TmpCLgroup")) )
		{
			//表TmpCLgroup用于保存分组结果
			EDIBgbl::dbPRJ.Execute( _T("DROP TABLE TmpCLgroup"));
		}
		//rsCL.Close();
		//从tmp2生成一个分组结果TmpCLgroup
		//GROUP BY子句的字段必须包含SELECT字句的全部字段(除了聚合函数的字段)
		SQLx=_T("SELECT ") + sGROUPBY + _T(",sum(CLnum) as CLnums,CLnums*CLdz as CLzz INTO TmpCLgroup    FROM TmpTCL ");
		SQLx +=_T(" GROUP BY ") + sGROUPBY;
		EDIBgbl::dbPRJ.Execute(SQLx);
		//拷贝tmpCLgroup到tmp2,以便保存分组数据
		EDIBgbl::dbPRJ.Execute( _T("INSERT INTO Tmp2 SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy);
		//将管部内容拷贝到TmpPA表
		EDIBgbl::dbPRJ.Execute( _T("INSERT INTO TmpPA SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy);
		//将临时材料表的内容拷贝到正式材料表TCL
		EDIBgbl::dbPRJ.Execute( _T("INSERT INTO [") + EDIBgbl::Btype[EDIBgbl::TCL] + _T("] IN \'") + EDIBgbl::dbPRJDB.GetName() + _T("\' SELECT * FROM TmpTCL"));
		//删除临时表
		RsDeleteAll(rsCL);
		//更新临时材料表数据记对象
		
		//*********************************
		//现在处理非弹性连接件（不包括拉杆）
		SQLx = CString(_T("SELECT * FROM [")) + EDIBgbl::Btype[EDIBgbl::TZB] + (_T("] ")) ;
		if(modPHScal::gbSumAllVolume)
		{
			SQLx +=_T(" WHERE VolumeID IN ( SELECT VolumeID FROM Volume WHERE EnginID=\'") + EDIBgbl::SelPrjID + _T("\' AND ZYID = ") + ltos(EDIBgbl::SelSpecID) + _T(" AND SJJDID = ") + ltos(EDIBgbl::SelDsgnID) + _T(" AND SJHYID = ") + ltos(EDIBgbl::SelHyID) +_T(")");
		}
		else
		{
			SQLx +=_T(" WHERE VolumeID =") + ltos(EDIBgbl::SelVlmID);
		}
		SQLx+=_T(" AND ( Index=") + ltos(iConnectPART) + _T(" AND ClassID<>") + ltos(iROD) + _T(" AND ClassID<>") + ltos(iCSPR) + _T(" AND ClassID<>") + ltos(iSPR) + _T(" AND IsSAPart<>-1)");
		rs.m_pDatabase = &EDIBgbl::dbPRJDB;rs.Open(dbOpenDynaset,SQLx);
		if( rs.IsEOF() && rs.IsBOF() )
		{
		}
		else
		{
			while(!rs.IsEOF())
			{
				frmStatus.m_Label1= GetResStr(IDS_SumPhsBomNotSPRConnect);
				strTmp.Format(GetResStr(IDS_SumPhsBomRec),ltos(iSum),ltos(iTmp));
				frmStatus.m_Label2=strTmp;
				frmStatus.UpdateStatus ( (float)iTmp/iSum, true);
				frmStatus.UpdateData(false);
				iTmp+=1;
				
				SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZA] + _T("] WHERE VolumeID=") + vtos(GetFields(rs,_T("VolumeID"))) + _T(" AND zdjh=") + vtos(GetFields(rs,_T("zdjh")));
				rsza.m_pDatabase = &EDIBgbl::dbPRJDB;rsza.Open(dbOpenDynaset,SQLx,dbReadOnly);
				if( rsza.IsEOF() && rsza.IsBOF() )
				{
				}
				else
				{
					rsza.MoveFirst();
					rsCL.AddNew();
					rsCL.SetFieldValue(_T("seq"),nil);
					rs.GetFieldValue(_T("zdjh"),v);
					rsCL.SetFieldValue(_T("zdjh"),v);
					rs.GetFieldValue(_T("VolumeID"),v);
					rsCL.SetFieldValue(_T("VolumeID"),v);
					rs.GetFieldValue(_T("CLID"),v);
					rsCL.SetFieldValue(_T("CLID"),v);
					rs.GetFieldValue(_T("CLmc"),v);
					rsCL.SetFieldValue(_T("CLmc"),v);
					rs.GetFieldValue(_T("CLgg"),v);
					rsCL.SetFieldValue(_T("CLgg"),v);
					rs.GetFieldValue(_T("CLcl"),v);
					rsCL.SetFieldValue(_T("CLcl"),v);
					rs.GetFieldValue(_T("CLdw"),v);
					CString str;
					str.LoadString(IDS_PIECE);
					rsCL.SetFieldValue(_T("CLdw"),(v.vt==VT_NULL) ? STR_VAR(str) : v);
					rs.GetFieldValue(_T("CLdz"),v);
					rsCL.SetFieldValue(_T("CLdz"),v);					
					rs.GetFieldValue(_T("CLzz"),v);
					rsCL.SetFieldValue(_T("CLzz"),v);
					rs.GetFieldValue(_T("CLbz"),v);
					rsCL.SetFieldValue(_T("CLbz"),v);
					rs.GetFieldValue(_T("CLnum"),v);
					rsza.GetFieldValue(_T("Num"),vTmp1);
					rsCL.SetFieldValue(_T("CLnum"),COleVariant(vtof(v)*vtof(vTmp1)));
					rsCL.Update();
				}
				rsza.Close();
				rs.MoveNext();
			}
		}
		rs.Close();
		//立刻将非弹性连接件从TCL分类汇总到Tmp2
		if( EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("TmpCLgroup")) )
		{
			//表TmpCLgroup用于保存分组结果
			EDIBgbl::dbPRJ.Execute( _T("DROP TABLE TmpCLgroup"));
		}
		//rsCL.Close();
		//从tmp2生成一个分组结果TmpCLgroup
		//GROUP BY子句的字段必须包含SELECT字句的全部字段(除了聚合函数的字段)
		SQLx=_T("SELECT ") + sGROUPBY + _T(",sum(CLnum) as CLnums,CLnums*CLdz as CLzz INTO TmpCLgroup   FROM TmpTCL ");
		SQLx +=_T(" GROUP BY ") + sGROUPBY;
		EDIBgbl::dbPRJ.Execute(SQLx);
		//拷贝tmpCLgroup到tmp2,以便保存分组数据
		EDIBgbl::dbPRJ.Execute( _T("INSERT INTO Tmp2 SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy);
		//将非弹性连接件(不包括拉杆)的内容拷贝到TmpConnection表
		EDIBgbl::dbPRJ.Execute( _T("INSERT INTO TmpConnection SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy);
		//将临时材料表的内容拷贝到正式材料表TCL
		EDIBgbl::dbPRJ.Execute( _T("INSERT INTO [") +EDIBgbl::Btype[EDIBgbl::TCL] + _T("] IN \'") + EDIBgbl::dbPRJDB.GetName() + _T("\' SELECT * FROM TmpTCL"));
		//删除临时表
		RsDeleteAll(rsCL);
		//更新临时材料表数据记对象
		
		
		//************
		//统计拉杆零件数量
		SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] ");
		if(modPHScal::gbSumAllVolume)
		{
			SQLx +=_T(" WHERE VolumeID IN ( SELECT VolumeID FROM Volume WHERE EnginID=\'") + EDIBgbl::SelPrjID + _T("\' AND ZYID = ") + ltos(EDIBgbl::SelSpecID) + _T(" AND SJJDID = ") + ltos(EDIBgbl::SelDsgnID) + _T(" AND SJHYID = ") + ltos(EDIBgbl::SelHyID) +_T(")");
		}
		else
		{
			SQLx +=_T(" WHERE VolumeID =") + ltos(EDIBgbl::SelVlmID) ;
		}
		SQLx+= _T(" AND   ClassID=") + ltos(iROD);
		rs.m_pDatabase = &EDIBgbl::dbPRJDB;rs.Open(dbOpenDynaset,SQLx);
		if( rs.IsEOF() && rs.IsBOF() )
		{
			//没有任何拉杆
		}
		else
		{
			//有任何拉杆,RS-圆钢
			this->rsID.FindFirst( _T("trim(ID)=\'RS\'"));
			while(!rs.IsEOF())
			{
				frmStatus.m_Label1= GetResStr(IDS_SumPhsBomRod);
				strTmp.Format(GetResStr(IDS_SumPhsBomRec),ltos(iSum),ltos(iTmp));
				frmStatus.m_Label2=strTmp;
				frmStatus.UpdateStatus ( (float)iTmp/iSum, true);
				frmStatus.UpdateData(false);
				iTmp+=1;
				
				SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZA] + _T("] WHERE VolumeID=") + vtos(GetFields(rs,_T("VolumeID"))) + _T(" AND zdjh=") + vtos(GetFields(rs,_T("zdjh")));
				rsza.m_pDatabase=&EDIBgbl::dbPRJDB; rsza.Open(dbOpenSnapshot,SQLx,dbReadOnly);
				if( rsza.IsEOF() && rsza.IsBOF() )
				{
				}
				else
				{
					rsza.MoveFirst();
					
					sTmp=((vTmp=GetFields(rsza,_T("num"))).vt==VT_NULL ? 1.0 : vtof(vTmp)) * ((vTmp=GetFields(rs,_T("CLnum"))).vt==VT_NULL ? 1.0 : vtof(vTmp));					
					sngSumWeight=sTmp * vtof(GetFields(rs,_T("GDW1"))) * vtof(GetFields(rs,_T("L1")) ) / 1000.0; //拉杆1m单重保存在GDW1字段
					rsCL.AddNew();
					rsCL.SetFieldValue(_T("seq"),nil);
					rs.GetFieldValue(_T("zdjh"),v);
					rsCL.SetFieldValue(_T("zdjh"),v);
					rs.GetFieldValue(_T("VolumeID"),v);
					rsCL.SetFieldValue(_T("VolumeID"),v);
					rs.GetFieldValue(_T("CLID"),v);
					rsCL.SetFieldValue(_T("CLID"),v);
					rs.GetFieldValue(_T("CLmc"),v);
					rsCL.SetFieldValue(_T("CLmc"),v);
					rs.GetFieldValue(_T("CLgg"),v);
					rsCL.SetFieldValue(_T("CLgg"),v);
					rs.GetFieldValue(_T("CLcl"),v);
					rsCL.SetFieldValue(_T("CLcl"),v);
					rs.GetFieldValue(_T("CLdw"),v);
					CString str;
					str.LoadString(IDS_PIECE);
					rsCL.SetFieldValue(_T("CLdw"),(v.vt==VT_NULL) ? STR_VAR(str) : v);
					rs.GetFieldValue(_T("CLdz"),v);
					rsCL.SetFieldValue(_T("CLdz"),v);					
					rs.GetFieldValue(_T("CLbz"),v);
					rsCL.SetFieldValue(_T("CLbz"),v);
					rs.GetFieldValue(_T("GDW1"),v);
					rs.GetFieldValue(_T("L1"),vTmp1);
					
					rsCL.SetFieldValue(_T("CLdz"),COleVariant(vtof(v) * vtof(vTmp1)/1000.0)); //拉杆1m单重保存在GDW1字段
					rsCL.SetFieldValue(_T("CLnum"),COleVariant(sTmp));
					rsCL.SetFieldValue(_T("CLzz"),COleVariant(sngSumWeight));
					rsCL.Update();
				}
				rsza.Close();
				rs.MoveNext();
			}
		}
		//立刻将拉杆零件从TCL分类汇总到Tmp2
		if( EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("TmpCLgroup")) )
		{
			//表TmpCLgroup用于保存分组结果
			EDIBgbl::dbPRJ.Execute( _T("DROP TABLE TmpCLgroup"));
		}
		//rsCL.Close();
		//从tmp2生成一个分组结果TmpCLgroup
		//GROUP BY子句的字段必须包含SELECT字句的全部字段(除了聚合函数的字段)
		SQLx=_T("SELECT ") + sGROUPBY + _T(",sum(CLnum) as CLnums,CLnums*CLdz as CLzz INTO TmpCLgroup   FROM TmpTCL ");
		SQLx +=_T(" GROUP BY ") + sGROUPBY;
		EDIBgbl::dbPRJ.Execute(SQLx);
		if(modPHScal::gbSumRodByRoundSteel)
			;
		else
		{
			//拷贝tmpCLgroup到tmp2,以便保存分组数据
			EDIBgbl::dbPRJ.Execute( _T("INSERT INTO Tmp2 SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy);
			//将拉杆的内容拷贝到TmpConnection表
			EDIBgbl::dbPRJ.Execute( _T("INSERT INTO TmpConnection SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy);
		}
		//将临时材料表的内容拷贝到正式材料表TCL
		EDIBgbl::dbPRJ.Execute( _T("INSERT INTO [") + EDIBgbl::Btype[EDIBgbl::TCL] + _T("] IN \'") + EDIBgbl::dbPRJDB.GetName() + _T("\' SELECT * FROM TmpTCL"));
		//删除临时表
		RsDeleteAll(rsCL);
		//更新临时材料表数据记对象
		
		//************
		//统计拉杆长度（按圆钢直径分类）
		//回到第一条记录
		if( rs.IsEOF() && rs.IsBOF() )
		{
			//没有任何拉杆
		}
		else
		{
			rs.MoveFirst();
			//有任何拉杆
			while(!rs.IsEOF())
			{
				frmStatus.m_Label1= GetResStr(IDS_SumPhsBomRodLen);
				strTmp.Format(GetResStr(IDS_SumPhsBomRec),ltos(iSum),ltos(iTmp));
				frmStatus.m_Label2=strTmp;
				frmStatus.UpdateStatus ( (float)iTmp/iSum, true);
				frmStatus.UpdateData(false);
				iTmp+=1;
				
				SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZA] + _T("] WHERE VolumeID=") + vtos(GetFields(rs,_T("VolumeID"))) + _T(" AND zdjh=") + vtos(GetFields(rs,_T("zdjh")));
				rsza.m_pDatabase=&EDIBgbl::dbPRJDB; rsza.Open(dbOpenSnapshot,SQLx,dbReadOnly);
				if( rsza.IsEOF() && rsza.IsBOF() )
				{
				}
				else
				{
					rsza.MoveFirst();
					
					sTmp=((vTmp=GetFields(rsza,_T("num"))).vt==VT_NULL ? 1.0 : vtof(vTmp)) * vtof(GetFields(rs,_T("CLnum"))) * vtof(GetFields(rs,_T("L1"))) / 1000.0;
					sngSumWeight = sTmp * vtof(GetFields(rs,_T("GDW1")));  //拉杆1m单重保存在GDW1字段
					//if( sTmp <> 0 ){ Debug.Print sTmp
					rsCL.AddNew();
					rsCL.SetFieldValue(_T("seq"),nil);
					rs.GetFieldValue(_T("zdjh"),v);
					rsCL.SetFieldValue(_T("zdjh"),v);
					rs.GetFieldValue(_T("VolumeID"),v);
					rsCL.SetFieldValue(_T("VolumeID"),v);
					rs.GetFieldValue(_T("CLID"),v);
					rsCL.SetFieldValue(_T("CLID"),v);
					rsCL.SetFieldValue(_T("CLmc"),GetFields(this->rsID,_T("Description")));
					rsCL.SetFieldValue(_T("CLgg"),STR_VAR(CString(_T("d=")) +vtos(GetFields(rs,_T("sizeC")))));
					rsCL.SetFieldValue(_T("CLcl"),GetFields(rs,_T("CLcl")));
					CString str;
					str.LoadString(IDS_METER);
					rsCL.SetFieldValue(_T("CLdw"),(GetFields(rs,_T("Cldw")).vt==VT_NULL ?  STR_VAR(str) : GetFields(rs,_T("CLdw"))));
					//rs.GetFieldValue(_T("GDW1"),vTmp); vTmp.vt==VT_NULL ? strTmp=_T("0") : strTmp.Format(_T("%.3f"),vtof(vTmp));rsCL.SetFieldValue(_T("CLdz"),STR_VAR(strTmp));
					
					rsCL.SetFieldValue(_T("CLdz"),GetFields(rs,_T("GDW1")));
					//rsCL.SetFieldValue(_T("CLdz")) = Format(IIf(IsNull(rsCL.SetFieldValue(_T("CLdz"))), 0, rsCL.SetFieldValue(_T("CLdz"))), _T("0.##"))
					rsCL.SetFieldValue(_T("CLnum"),COleVariant(sTmp));
					rsCL.SetFieldValue(_T("CLzz"),COleVariant(sngSumWeight));
					rsCL.SetFieldValue(_T("CLbz"),GetFields(rs,_T("CLbz")));
					rsCL.Update();
				}
				rsza.Close();
				rs.MoveNext();
			}
		}
		rs.Close();
		//立刻将拉杆材料从TCL分类汇总到Tmp2
		if( EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("TmpCLgroup")) )
		{
			//表TmpCLgroup用于保存分组结果
			EDIBgbl::dbPRJ.Execute( _T("DROP TABLE TmpCLgroup"));
		}
		//rsCL.Close();
		//从tmp2生成一个分组结果TmpCLgroup
		//GROUP BY子句的字段必须包含SELECT字句的全部字段(除了聚合函数的字段)
		SQLx=_T("SELECT ") + sGROUPBY + _T(",sum(CLnum) as CLnums,CLnums*CLdz as CLzz INTO TmpCLgroup   FROM TmpTCL ");
		SQLx +=_T(" GROUP BY ") + sGROUPBY;
		EDIBgbl::dbPRJ.Execute(SQLx);
		//拷贝tmpCLgroup到tmp2,以便保存分组数据
		//EDIBgbl::dbPRJ.Execute( _T("INSERT INTO Tmp2 SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy
		if(modPHScal::gbSumRodByRoundSteel)
		{
			//拷贝tmpCLgroup到tmp2,以便保存分组数据,tmp2的数据记录将输出
			EDIBgbl::dbPRJ.Execute( _T("INSERT INTO Tmp2 SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy);
			//将临时材料表的内容拷贝到正式材料表TCL
			EDIBgbl::dbPRJ.Execute( _T("INSERT INTO [") + EDIBgbl::Btype[EDIBgbl::TCL] + _T("] IN \'") + EDIBgbl::dbPRJDB.GetName() + _T("\' SELECT * FROM TmpTCL"));
		}
		
		//将拉杆的内容拷贝到TmpLugBom表
		EDIBgbl::dbPRJ.Execute( _T("INSERT INTO TmpLugBom SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy);
		//将临时材料表的内容拷贝到正式材料表TCL
		//EDIBgbl::dbPRJ.Execute( _T("INSERT INTO [") + EDIBgbl::TBNSelPrjSpec + EDIBgbl::Btype[EDIBgbl::TCL] + _T("] IN \'") + EDIBgbl::dbPRJDB.GetName() + _T("\' SELECT * FROM TmpTCL")
		//删除临时表
		RsDeleteAll(rsCL);
		//更新临时材料表数据记对象
		
		
		//******************
		//统计根部和根部附件
		//选择根部附件中的非螺母,螺栓的零件
		SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] ") ;
		if(modPHScal::gbSumAllVolume)
		{
			SQLx +=_T(" WHERE VolumeID IN ( SELECT VolumeID FROM Volume WHERE EnginID=\'") + EDIBgbl::SelPrjID + _T("\' AND ZYID = ") + ltos(EDIBgbl::SelSpecID) + _T(" AND SJJDID = ") + ltos(EDIBgbl::SelDsgnID) + _T(" AND SJHYID = ") + ltos(EDIBgbl::SelHyID) +_T(")");
		}
		else
		{
			SQLx +=_T(" WHERE VolumeID =") + ltos(EDIBgbl::SelVlmID) ;
		}
		SQLx+=_T(" AND IsSAPart=-1 AND ClassID<>") + ltos(iBolts) + _T(" AND ClassID<>") + ltos(iNuts);
		rs.m_pDatabase = &EDIBgbl::dbPRJDB;rs.Open(dbOpenDynaset,SQLx);
		if( rs.IsEOF() && rs.IsBOF() )
		{
			//没有任何非型钢,非螺栓螺母等的附件
		}
		else
		{
			//有任何非型钢,非螺栓螺母等的零件
			while(!rs.IsEOF())
			{
				frmStatus.m_Label1= GetResStr(IDS_SumPhsBomAttachements);
				strTmp.Format(GetResStr(IDS_SumPhsBomRec),ltos(iSum),ltos(iTmp));
				frmStatus.m_Label2=strTmp;
				frmStatus.UpdateStatus ( (float)iTmp/iSum, true);
				frmStatus.UpdateData(false);
				iTmp+=1;
				
				//只选择根部材料需要统计的支吊架
				SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZA] + _T("] WHERE VolumeID=") + vtos(GetFields(rs,_T("VolumeID"))) + _T(" AND zdjh=") + vtos(GetFields(rs,_T("zdjh")));
				rsza.m_pDatabase = &EDIBgbl::dbPRJDB;rsza.Open(dbOpenDynaset,SQLx,dbReadOnly);
				if( rsza.IsEOF() && rsza.IsBOF() )
				{
				}
				else
				{
					rsza.MoveFirst();
					
					if( this->rsID.FindFirst( _T("trim(CustomID)=\'") + vtos(GetFields(rs,_T("CustomID"))) + _T("\'")))
					{
						int intIndex=vtoi(GetFields(this->rsID,_T("Index")));
						if( intIndex==iSectionSteel )
						{
						}
						else
						{
							rsza.GetFieldValue(_T("num"),v);
							m_iNum=vtoi(v);
							rsza.GetFieldValue(_T("Gnum"),v);
							m_iSANum=vtoi(v);
							rsza.GetFieldValue(_T("bNotSumSA"),v);
							m_bNotSumSA=vtob(v);
							rs.GetFieldValue(_T("CLnum"),v);
							m_iCLNum=vtoi(v);
							rs.GetFieldValue(_T("bUserAdd"),v);
							m_bUserAdd=vtob(v);
							if(m_bUserAdd)
								sTmp=m_iNum*m_iSANum*m_iCLNum;
							else
							{
								if (!m_bNotSumSA)
									sTmp=m_iNum*m_iSANum*m_iCLNum;
								else
									sTmp=0;
							}
							sngSumWeight = sTmp * vtof( GetFields(rs,_T("CLdz")));
							rsCL.AddNew();
							rsCL.SetFieldValue(_T("seq"),nil);
							rs.GetFieldValue(_T("zdjh"),v);
							rsCL.SetFieldValue(_T("zdjh"),v);
							rs.GetFieldValue(_T("VolumeID"),v);
							rsCL.SetFieldValue(_T("VolumeID"),v);
							rs.GetFieldValue(_T("CLID"),v);
							rsCL.SetFieldValue(_T("CLID"),v);
							rs.GetFieldValue(_T("CLmc"),v);
							rsCL.SetFieldValue(_T("CLmc"),v);
							rsCL.SetFieldValue(_T("CLgg"),GetFields(rs,_T("CLgg")));
							rsCL.SetFieldValue(_T("CLcl"),GetFields(rs,_T("CLcl")));
							//rsCL.SetFieldValue(_T("CLdw"),((vTmp=GetFields(rs,_T("Cldw"))).vt==VT_NULL ? STR_VAR(_T("件")) : vTmp));
							CString str;
							str.LoadString(IDS_PIECE);
							rsCL.SetFieldValue(_T("CLdw"),((vTmp=GetFields(rs,_T("Cldw"))).vt==VT_NULL ? STR_VAR(str) : vTmp));
							rsCL.SetFieldValue(_T("CLdz"),GetFields(rs,_T("CLdz")));
							//rsCL.SetFieldValue(_T("CLdz")) = Format(IIf(IsNull(rsCL.SetFieldValue(_T("CLdz"))), 0, rsCL.SetFieldValue(_T("CLdz"))), _T("0.##"))
							rsCL.SetFieldValue(_T("CLnum"),COleVariant(sTmp));
							rsCL.SetFieldValue(_T("CLzz"),COleVariant(sngSumWeight));
							rsCL.SetFieldValue(_T("CLbz"),GetFields(rs,_T("CLbz")));
							rsCL.Update();
						}
					}
				}
				rsza.Close();
				rs.MoveNext();
			}
			//立刻将非螺母,螺栓根部附件从TCL分类汇总到Tmp2
			if( EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("TmpCLgroup")) )
			{
				//表TmpCLgroup用于保存分组结果
				EDIBgbl::dbPRJ.Execute( _T("DROP TABLE TmpCLgroup"));
			}
			//rsCL.Close();
			//从tmp2生成一个分组结果TmpCLgroup
			//GROUP BY子句的字段必须包含SELECT字句的全部字段(除了聚合函数的字段)
			SQLx=_T("SELECT ") + sGROUPBY + _T(",sum(CLnum) as CLnums,CLnums*CLdz as CLzz INTO TmpCLgroup   FROM TmpTCL ");
			SQLx +=_T(" GROUP BY ") + sGROUPBY;
			EDIBgbl::dbPRJ.Execute(SQLx);
			//拷贝tmpCLgroup到tmp2,以便保存分组数据
			EDIBgbl::dbPRJ.Execute( _T("INSERT INTO Tmp2 SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy);
			//将非螺母,螺栓根部附件的内容拷贝到TmpAttachment表
			EDIBgbl::dbPRJ.Execute( _T("INSERT INTO TmpAttachment SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy);
			//将临时表的内容拷贝到正式材料表TCL
			EDIBgbl::dbPRJ.Execute( _T("INSERT INTO [") + EDIBgbl::Btype[EDIBgbl::TCL] + _T("] IN \'") + EDIBgbl::dbPRJDB.GetName() + _T("\' SELECT * FROM TmpTCL"));
			//删除临时表
			RsDeleteAll(rsCL);
			//更新临时材料表数据记对象
			//rsCL.Open(dbOpenDynaset,_T("SELECT * FROM TmpTCL") );
		}
		
		
		//型钢按长度统计单独写一个表TmpSSBom
		if( rs.IsEOF() && rs.IsBOF() )
		{
			//没有型钢
		}else
		{
			rs.MoveFirst();
			while(!rs.IsEOF())
			{
				frmStatus.m_Label1= GetResStr(IDS_SumPhsBomSSLen);
				strTmp.Format(GetResStr(IDS_SumPhsBomRec),ltos(iSum),ltos(iTmp));
				frmStatus.m_Label2=strTmp;
				frmStatus.UpdateStatus ( (float)iTmp/iSum, true);
				frmStatus.UpdateData(false);
				iTmp+=1;
				
				SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZA] + _T("] WHERE VolumeID=") + vtos(GetFields(rs,_T("VolumeID"))) + _T(" AND zdjh=") + vtos(GetFields(rs,_T("zdjh"))) + _T(" AND  NOT bNotSumSA ");
				rsza.m_pDatabase = &EDIBgbl::dbPRJDB;rsza.Open(dbOpenDynaset,SQLx,dbReadOnly);
				if( rsza.IsEOF() && rsza.IsBOF() )
				{
				}
				else
				{
					rsza.MoveFirst();
					
					if(this->rsID.FindFirst( _T("trim(CustomID)=\'") + vtos(GetFields(rs,_T("CustomID"))) + _T("\'")))
					{					
						if( vtoi(GetFields(this->rsID,_T("Index")))==iSectionSteel )
						{                  
							rsza.GetFieldValue(_T("num"),v);
							m_iNum=vtoi(v);
							rsza.GetFieldValue(_T("Gnum"),v);
							m_iSANum=vtoi(v);
							rsza.GetFieldValue(_T("bNotSumSA"),v);
							m_bNotSumSA=vtob(v);
							rs.GetFieldValue(_T("CLnum"),v);
							m_iCLNum=vtoi(v);
							rs.GetFieldValue(_T("bUserAdd"),v);
							m_bUserAdd=vtob(v);
							rs.GetFieldValue(_T("L1"),v);
							m_iL1=vtoi(v);
							if(m_bUserAdd)
								sTmp=m_iNum*m_iSANum*m_iCLNum*m_iL1/1000.0;
							else
							{
								if (!m_bNotSumSA)
									sTmp=m_iNum*m_iSANum*m_iCLNum*m_iL1/1000.0;
								else
									sTmp=0;
							}
							sngSumWeight = sTmp * vtof( GetFields(rs,_T("CLdz")));
							rsCL.AddNew();
							rsCL.SetFieldValue(_T("seq"),nil);
							rs.GetFieldValue(_T("zdjh"),v);
							rsCL.SetFieldValue(_T("zdjh"),v);
							rs.GetFieldValue(_T("VolumeID"),v);
							rsCL.SetFieldValue(_T("VolumeID"),v);
							rs.GetFieldValue(_T("CLID"),v);
							rsCL.SetFieldValue(_T("CLID"),v);
							rs.GetFieldValue(_T("CLmc"),v);
							rsCL.SetFieldValue(_T("CLmc"),v);
							rs.GetFieldValue(_T("CLgg"),v);
							rsCL.SetFieldValue(_T("CLgg"),v);
							rsCL.SetFieldValue(_T("CLcl"),GetFields(rs,_T("CLcl")));
							CString str;
							str.LoadString(IDS_METER);
							rsCL.SetFieldValue(_T("CLdw"),(GetFields(rs,_T("Cldw")).vt==VT_NULL ?  STR_VAR(str) : GetFields(rs,_T("CLdw"))));
							rsCL.SetFieldValue(_T("CLdz"),GetFields(rs,_T("CLdz")));
							//rsCL.SetFieldValue(_T("CLdz")) = Format(IIf(IsNull(rsCL.SetFieldValue(_T("CLdz"))), 0, rsCL.SetFieldValue(_T("CLdz"))), _T("0.##"))
							rsCL.SetFieldValue(_T("CLnum"),COleVariant(sTmp));
							rsCL.SetFieldValue(_T("CLzz"),COleVariant(sngSumWeight));
							rsCL.SetFieldValue(_T("CLbz"),GetFields(rs,_T("CLbz")));
							rsCL.Update();
						}
					}
				}
				rsza.Close();
				rs.MoveNext();
			}
			//立刻将型钢从TCL分类汇总到Tmp2
			if( EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("TmpCLgroup")) )
			{
				//表TmpCLgroup用于保存分组结果
				EDIBgbl::dbPRJ.Execute( _T("DROP TABLE TmpCLgroup"));
			}
			//rsCL.Close();
			//从tmp2生成一个分组结果TmpCLgroup
			//GROUP BY子句的字段必须包含SELECT字句的全部字段(除了聚合函数的字段)
			SQLx=_T("SELECT ") + sGROUPBY + _T(",sum(CLnum) as CLnums,CLnums*CLdz as CLzz INTO TmpCLgroup   FROM TmpTCL ");
			SQLx +=_T(" GROUP BY ") + sGROUPBY;
			EDIBgbl::dbPRJ.Execute(SQLx);
			//拷贝tmpCLgroup到tmp2,以便保存分组数据
			//下句不可少，因为AutoCAD输出材料表时首先要调用GetphsSumBom获得所有材料的集合，且ACAD中该集合来源为tmp2表。
			EDIBgbl::dbPRJ.Execute( _T("INSERT INTO Tmp2 SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy);
			//将型钢的内容拷贝到TmpSSBom表
			EDIBgbl::dbPRJ.Execute( _T("INSERT INTO TmpSSBom SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy);
			//将临时表的内容拷贝到正式材料表TCL
			//EDIBgbl::dbPRJ.Execute( _T("INSERT INTO [") + EDIBgbl::TBNSelPrjSpec + EDIBgbl::Btype[EDIBgbl::TCL] + _T("] IN \'") + EDIBgbl::dbPRJDB.GetName() + _T("\' SELECT * FROM TmpTCL")
			//删除临时表
			RsDeleteAll(rsCL);
			//更新临时材料表数据记对象
		}
		
		
		//型钢按零件统计单独写一个表TmpSS
		if( rs.IsEOF() && rs.IsBOF() )
		{
			//没有型钢
		}
		else
		{
			rs.MoveFirst();
			while(!rs.IsEOF())
			{
				frmStatus.m_Label1= GetResStr(IDS_SumPhsBomSS);
				strTmp.Format(GetResStr(IDS_SumPhsBomRec),ltos(iSum),ltos(iTmp));
				frmStatus.m_Label2=strTmp;
				frmStatus.UpdateStatus ( (float)iTmp/iSum, true);
				frmStatus.UpdateData(false);
				iTmp+=1;
				
				SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZA] + _T("] WHERE VolumeID=") + vtos(GetFields(rs,_T("VolumeID"))) + _T(" AND zdjh=") + vtos(GetFields(rs,_T("zdjh"))) + _T(" AND NOT bNotSumSA");
				rsza.m_pDatabase = &EDIBgbl::dbPRJDB;rsza.Open(dbOpenDynaset,SQLx,dbReadOnly);
				if( rsza.IsEOF() && rsza.IsBOF() )
				{
				}
				else
				{
					rsza.MoveFirst();
					
					if(this->rsID.FindFirst( _T("trim(CustomID)=\'") + vtos(GetFields(rs,_T("CustomID"))) + _T("\'")))
					{
						//型钢(自2001.12.31起钢板PS类别从型钢(index=3)改为附件(index=4)
						if( (vtoi(GetFields(this->rsID,_T("Index")))==iSectionSteel) )
						{
							rsza.GetFieldValue(_T("Num"),v);
							m_iNum=vtoi(v);
							rsza.GetFieldValue(_T("Gnum"),v);
							m_iSANum=vtoi(v);
							rsza.GetFieldValue(_T("bNotSumSA"),v);
							m_bNotSumSA=vtob(v);
							rs.GetFieldValue(_T("CLnum"),v);
							m_iCLNum=vtoi(v);
							rs.GetFieldValue(_T("bUserAdd"),v);
							m_bUserAdd=vtob(v);
							rs.GetFieldValue(_T("L1"),v);
							m_iL1=vtoi(v);
							if(m_bUserAdd)
								sTmp=m_iNum*m_iSANum*m_iCLNum;
							else
							{
								if (!m_bNotSumSA)
									sTmp=m_iNum*m_iSANum*m_iCLNum;
								else
									sTmp=0;
							}
							sngSumWeight = sTmp * m_iL1/1000.0 * vtof( GetFields(rs,_T("CLdz"))) ;
							rsCL.AddNew();
							rsCL.SetFieldValue(_T("seq"),nil);
							rs.GetFieldValue(_T("zdjh"),v);
							rsCL.SetFieldValue(_T("zdjh"),v);
							rs.GetFieldValue(_T("VolumeID"),v);
							rsCL.SetFieldValue(_T("VolumeID"),v);
							rs.GetFieldValue(_T("CLID"),v);
							rsCL.SetFieldValue(_T("CLID"),v);
							rs.GetFieldValue(_T("CLmc"),v);
							rsCL.SetFieldValue(_T("CLmc"),v);
							
							rsCL.SetFieldValue(_T("CLgg"),STR_VAR(vtos(GetFields(rs,_T("CLgg"))) + _T(" L=") + ltos(m_iL1)));
							rsCL.SetFieldValue(_T("CLcl"),GetFields(rs,_T("CLcl")));
							rsCL.SetFieldValue(_T("CLdw"),((vTmp=GetFields(rs,_T("Cldw"))).vt==VT_NULL ? STR_VAR(_T("件")) : vTmp));
							
							rsCL.SetFieldValue(_T("CLdz"),COleVariant(vtof(GetFields(rs,_T("CLdz"))) * m_iL1/1000.0));
							rsCL.SetFieldValue(_T("CLnum"),COleVariant(sTmp));
							rsCL.SetFieldValue(_T("CLzz"),COleVariant(sngSumWeight));
							rsCL.SetFieldValue(_T("CLbz"),GetFields(rs,_T("CLbz")));
							rsCL.Update();
							
						}
					}
					
				}
				rsza.Close();
				rs.MoveNext();
			}
			//立刻将型钢从TCL分类汇总到Tmp2
			if( EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("TmpCLgroup")) )
			{
				//表TmpCLgroup用于保存分组结果
				EDIBgbl::dbPRJ.Execute( _T("DROP TABLE TmpCLgroup"));
			}
			//rsCL.Close();
			//从tmp2生成一个分组结果TmpCLgroup
			//GROUP BY子句的字段必须包含SELECT字句的全部字段(除了聚合函数的字段)
			SQLx=_T("SELECT ") + sGROUPBY + _T(",sum(CLnum) as CLnums,CLnums*CLdz as CLzz INTO TmpCLgroup   FROM TmpTCL ");
			SQLx +=_T(" GROUP BY ") + sGROUPBY;
			EDIBgbl::dbPRJ.Execute(SQLx);
			//拷贝tmpCLgroup到tmp2,以便保存分组数据
			//EDIBgbl::dbPRJ.Execute( _T("INSERT INTO Tmp2 SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy
			//将型钢的内容拷贝到TmpSS表
			EDIBgbl::dbPRJ.Execute( _T("INSERT INTO TmpSS SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy);
			//将临时表的内容拷贝到正式材料表TCL
			//EDIBgbl::dbPRJ.Execute( _T("INSERT INTO [") + EDIBgbl::TBNSelPrjSpec + EDIBgbl::Btype[EDIBgbl::TCL] + _T("] IN \'") + EDIBgbl::dbPRJDB.GetName() + _T("\' SELECT * FROM TmpTCL")
			//删除临时表
			RsDeleteAll(rsCL);
			//更新临时材料表数据记对象
		}
		rs.Close();
		//如果要输出根部整体零件表
		if( modPHScal::gbOutputSA )
		{
			//输出根部整体零件表
			SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] ");
			if(modPHScal::gbSumAllVolume)
			{
				SQLx +=_T(" WHERE VolumeID IN ( SELECT VolumeID FROM Volume WHERE EnginID=\'") + EDIBgbl::SelPrjID + _T("\' AND ZYID = ") + ltos(EDIBgbl::SelSpecID) + _T(" AND SJJDID = ") + ltos(EDIBgbl::SelDsgnID) + _T(" AND SJHYID = ") + ltos(EDIBgbl::SelHyID) +_T(")");
			}
			else
			{
				SQLx +=_T(" WHERE VolumeID =") + ltos(EDIBgbl::SelVlmID) ;
			}
			//不包括混凝土根部和自定义根部
			SQLx += _T(" AND  Index=") + ltos(iSA) + _T(" AND ClassID<>") + ltos(iGCement) + _T(" AND ClassID<>") + ltos(iG100);
			rs.m_pDatabase = &EDIBgbl::dbPRJDB;rs.Open(dbOpenDynaset,SQLx);
			if( rs.IsEOF() && rs.IsBOF() )
			{
			}
			else
			{
				while(!rs.IsEOF())
				{
					frmStatus.m_Label1= GetResStr(IDS_SumPhsBomSA);
					strTmp.Format(GetResStr(IDS_SumPhsBomRec),ltos(iSum),ltos(iTmp));
					frmStatus.m_Label2=strTmp;
					frmStatus.UpdateStatus ( (float)iTmp/iSum, true);
					frmStatus.UpdateData(false);
					iTmp+=1;
					
					SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZA] + _T("] WHERE VolumeID=") + vtos(GetFields(rs,_T("VolumeID"))) + _T(" AND zdjh=") + vtos(GetFields(rs,_T("zdjh"))) + _T(" AND NOT bNotSumSA");
					rsza.m_pDatabase = &EDIBgbl::dbPRJDB;rsza.Open(dbOpenDynaset,SQLx,dbReadOnly);
					if( rsza.IsEOF() && rsza.IsBOF() )
					{
					}
					else
					{
						rsza.MoveFirst();
						rsCL.AddNew();
						rsCL.SetFieldValue(_T("seq"),nil);
						rs.GetFieldValue(_T("zdjh"),v);
						rsCL.SetFieldValue(_T("zdjh"),v);
						rs.GetFieldValue(_T("VolumeID"),v);
						rsCL.SetFieldValue(_T("VolumeID"),v);
						rs.GetFieldValue(_T("CLID"),v);
						rsCL.SetFieldValue(_T("CLID"),v);
						rs.GetFieldValue(_T("CLmc"),v);
						rsCL.SetFieldValue(_T("CLmc"),v);
						rs.GetFieldValue(_T("CLgg"),v);
						rsCL.SetFieldValue(_T("CLgg"),v);
						rs.GetFieldValue(_T("CLcl"),v);
						rsCL.SetFieldValue(_T("CLcl"),v);
						CString str;
						str.LoadString(IDS_PIECE);
						rsCL.SetFieldValue(_T("CLdw"),((vTmp=GetFields(rs,_T("Cldw"))).vt==VT_NULL ? STR_VAR(str) : vTmp));
						rsCL.SetFieldValue(_T("CLdz"),GetFields(rs,_T("CLdz")));//, 0, rs.Fields(_T("CLdz"))), _T("0.##"))
						rsCL.SetFieldValue(_T("CLnum"),COleVariant(vtof(GetFields(rsza,_T("Gnum"))) * vtof(GetFields(rsza,_T("num")))));
						rsCL.SetFieldValue(_T("CLzz"),GetFields(rs,_T("CLzz")));
						rsCL.SetFieldValue(_T("CLbz"),GetFields(rs,_T("CLbz")));
						rsCL.Update();
					}
					rsza.Close();
					rs.MoveNext();
				}
				//立刻将根部汇总从TCL分类汇总到TmpSA
				if( EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("TmpCLgroup")) )
				{
					//表TmpCLgroup用于保存分组结果
					EDIBgbl::dbPRJ.Execute( _T("DROP TABLE TmpCLgroup"));
				}
				//rsCL.Close();
				//从tmpCL生成一个分组结果TmpCLgroup
				//GROUP BY子句的字段必须包含SELECT字句的全部字段(除了聚合函数的字段)
				SQLx=_T("SELECT ") + sGROUPBY + _T(",sum(CLnum) as CLnums,CLnums*CLdz as CLzz INTO TmpCLgroup   FROM TmpTCL ");
				SQLx +=_T(" GROUP BY ") + sGROUPBY;
				EDIBgbl::dbPRJ.Execute(SQLx);
				//将根部汇总的内容拷贝到TmpSA表
				EDIBgbl::dbPRJ.Execute( _T("INSERT INTO TmpSA SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy);
				//将临时表的内容拷贝到正式材料表TCL
				//根部整体零件表不能汇总到材料表中
				//删除临时表
				RsDeleteAll(rsCL);
				//更新临时材料表数据记对象
				//rsCL.Open(dbOpenDynaset,_T("SELECT * FROM TmpTCL") );
			}
		}
		rs.Close();
		
		//************
		//现在处理螺栓螺母、非根部的附件(如F3)
		//统计螺栓螺母：编号不为空（为空的属于厂家供货，不要统计），或根部使用的螺栓螺母（当根部需要统计时），或用户添加的螺栓螺母(bUserAdd=-1)
		SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] ") ;
		if(modPHScal::gbSumAllVolume)
		{
			SQLx +=_T(" WHERE VolumeID IN ( SELECT VolumeID FROM Volume WHERE EnginID=\'") + EDIBgbl::SelPrjID + _T("\' AND ZYID = ") + ltos(EDIBgbl::SelSpecID) + _T(" AND SJJDID = ") + ltos(EDIBgbl::SelDsgnID) + _T(" AND SJHYID = ") + ltos(EDIBgbl::SelHyID) +_T(")");
		}
		else
		{
			SQLx +=_T(" WHERE VolumeID =") + ltos(EDIBgbl::SelVlmID) ;
		}
		//AND后的条件必须打括号
		SQLx+=_T(" AND  ((ClassID=") + ltos(iBolts) + _T(" OR ClassID=") + ltos(iNuts) + _T(" OR ClassID=") + ltos(iAttached) + _T(") AND NOT IsNull(SEQ) AND IsSAPart<>-1  OR (ClassID=") + ltos(iBolts) + _T(" OR ClassID=") + ltos(iNuts) + _T(") AND IsSAPart=-1 ) ");		
		rs.m_pDatabase = &EDIBgbl::dbPRJDB;rs.Open(dbOpenDynaset,SQLx);
		if( rs.IsEOF() && rs.IsBOF() )
		{
		}
		else
		{
			while(!rs.IsEOF())
			{
				frmStatus.m_Label1= GetResStr(IDS_SumPhsBomBoltsNuts);
				strTmp.Format(GetResStr(IDS_SumPhsBomRec),ltos(iSum),ltos(iTmp));
				frmStatus.m_Label2=strTmp;
				frmStatus.UpdateStatus ( (float)iTmp/iSum, true);
				frmStatus.UpdateData(false);
				iTmp+=1;
				
				SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZA] + _T("] WHERE VolumeID=") + vtos(GetFields(rs,_T("VolumeID"))) + _T(" AND zdjh=") + vtos(GetFields(rs,_T("zdjh")));
				rsza.m_pDatabase = &EDIBgbl::dbPRJDB;rsza.Open(dbOpenDynaset,SQLx,dbReadOnly);
				if( rsza.IsEOF() && rsza.IsBOF() )
				{
				}
				else
				{
					rsza.MoveFirst();
					rsza.GetFieldValue(_T("bNotSumSA"),v);
					m_bNotSumSA=vtob(v);
					rs.GetFieldValue(_T("bUserAdd"),v);
					m_bUserAdd=vtob(v);					
					rs.GetFieldValue(_T("recno"),v);
					/*
					rs.GetFieldValue(_T("SEQ"),vSEQ);
					if(v.vt==VT_NULL)
					if(m_bUserAdd)
					;
					else
					if(!m_bNotSumSA)
					;
					else
					//只有此时不统计数量;
					else
					;
					*/
					
					if(!( !(v.vt==VT_NULL) ||  (v.vt==VT_NULL) && (m_bUserAdd || !m_bUserAdd && !m_bUserAdd) ))
					{
						//空的记录号，且不是用户添加的材料，不统计根部
						//不需要统计的根部螺栓螺母
					}else
					{
						rsza.GetFieldValue(_T("Num"),v);
						m_iNum=vtoi(v);
						rsza.GetFieldValue(_T("Gnum"),v);
						m_iSANum=vtoi(v);
						rsza.GetFieldValue(_T("bNotSumSA"),v);
						m_bNotSumSA=vtob(v);
						rs.GetFieldValue(_T("CLnum"),v);
						m_iCLNum=vtoi(v);
						rs.GetFieldValue(_T("bUserAdd"),v);
						m_bUserAdd=vtob(v);
						rs.GetFieldValue(_T("L1"),v);
						m_iL1=vtoi(v);
						sTmp=m_iNum*m_iCLNum;
						rsCL.AddNew();
						rsCL.SetFieldValue(_T("seq"),nil);
						rs.GetFieldValue(_T("zdjh"),v);
						rsCL.SetFieldValue(_T("zdjh"),v);
						rs.GetFieldValue(_T("VolumeID"),v);
						rsCL.SetFieldValue(_T("VolumeID"),v);
						rs.GetFieldValue(_T("CLID"),v);
						rsCL.SetFieldValue(_T("CLID"),v);
						rs.GetFieldValue(_T("CLmc"),v);
						rsCL.SetFieldValue(_T("CLmc"),v);
						rs.GetFieldValue(_T("CLgg"),v);
						rsCL.SetFieldValue(_T("CLgg"),v);
						rs.GetFieldValue(_T("CLcl"),v);
						rsCL.SetFieldValue(_T("CLcl"),v);
						CString str;
						str.LoadString(IDS_PIECE);
						rsCL.SetFieldValue(_T("CLdw"),((vTmp=GetFields(rs,_T("Cldw"))).vt==VT_NULL ? STR_VAR(str) : vTmp));
						rsCL.SetFieldValue(_T("CLdz"),GetFields(rs,_T("CLdz")));
						rsCL.SetFieldValue(_T("CLnum"),COleVariant(sTmp));
						rsCL.SetFieldValue(_T("CLzz"),GetFields(rs,_T("CLzz")));
						rsCL.SetFieldValue(_T("CLbz"),GetFields(rs,_T("CLbz")));
						rsCL.Update();
					}
				}
				rsza.Close();
				rs.MoveNext();
			}
		}
		rs.Close();
		//立刻将螺栓螺母从TCL分类汇总到Tmp2
		if( EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("TmpCLgroup")) )
		{
			//表TmpCLgroup用于保存分组结果
			EDIBgbl::dbPRJ.Execute( _T("DROP TABLE TmpCLgroup"));
		}
		//rsCL.Close();
		//从tmp2生成一个分组结果TmpCLgroup
		//GROUP BY子句的字段必须包含SELECT字句的全部字段(除了聚合函数的字段)
		SQLx=_T("SELECT ") + sGROUPBY + _T(",sum(CLnum) as CLnums,CLnums*CLdz as CLzz INTO TmpCLgroup   FROM TmpTCL ");
		SQLx +=_T(" GROUP BY ") + sGROUPBY;
		EDIBgbl::dbPRJ.Execute(SQLx);
		//拷贝tmpCLgroup到tmp2,以便保存分组数据
		EDIBgbl::dbPRJ.Execute( _T("INSERT INTO Tmp2 SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy);
		//将螺栓螺母的内容拷贝到TmpBoltsNuts表
		EDIBgbl::dbPRJ.Execute( _T("INSERT INTO TmpBoltsNuts SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy);
		//将临时材料表的内容拷贝到正式材料表TCL
		EDIBgbl::dbPRJ.Execute( _T("INSERT INTO [") + EDIBgbl::Btype[EDIBgbl::TCL] + _T("] IN \'") + EDIBgbl::dbPRJDB.GetName() + _T("\' SELECT * FROM TmpTCL"));
		//删除临时表
		EDIBgbl::dbPRJ.Execute( _T("DELETE * FROM TmpTCL"));
		//更新临时材料表数据记对象
		//rsCL.Open(dbOpenDynaset,_T("SELECT * FROM TmpTCL") );
	}
	catch(::CDaoException* e)
	{
		e->ReportError();
		e->Delete();
	}
	catch(CException *e)
	{
		e->Delete();
	}
	catch(...)
	{
	}
	frmStatus.UpdateStatus(1,true);
	frmStatus.ShowWindow(SW_HIDE);
}

void Cphs::GetPhsBlkIDandCrd(_RecordsetPtr /*ByVal*/ rsza)
{
	//目的：获取图块名称ID和绘制高度（Y坐标位移）
	//input:
	//        dbprjdb中的F####?-JZB,
	//output:
	//        dbprj中的TmpCLgroup,
	CDaoRecordset rs(&EDIBgbl::dbPHScode),rsTmpZB(&EDIBgbl::dbPRJDB);//20071101 "dbSORT" 改为 "dbPHScode"
	//   long i;
	CString tbn,tmpID,tmpBlkID,SQLx;
	CString tmpStr;
	try
	{
		SQLx = _T("SELECT * FROM phsBlkDimPos");
		rs.Open(dbOpenSnapshot,SQLx);
		//所有可绘制零件recno<>Null
		SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] WHERE zdjh=") + ltos(modPHScal::zdjh) + _T(" AND VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" AND IsSAPart<>-1 ORDER BY recno");
		rsTmpZB.Open(dbOpenDynaset,SQLx);
		if(rsTmpZB.IsEOF() && rsTmpZB.IsBOF())
		{
			tmpStr.Format(GetResStr(IDS_NoRecordInTZB),EDIBgbl::dbPRJDB.GetName(),EDIBgbl::Btype[EDIBgbl::TZB], EDIBgbl::SelJcdm,ltos(modPHScal::zdjh));
			throw tmpStr;
		}
		COleVariant vTmp;
		while(!rsTmpZB.IsEOF())
		{
			//展开sFindTBN以加快速度
			//tbn = sFindTBN(rsTmpZB.Fields(_T("CustomID")))
			rsTmpZB.GetFieldValue(_T("CustomID"),vTmp);
			
			if(rsID.FindFirst(_T("trim(CustomID)=\'") +vtos(vTmp) + _T("\'")))
			{
				rsID.GetFieldValue(_T("Index"),vTmp);
				modPHScal::glIDIndex = vtoi(vTmp);
				rsID.GetFieldValue(_T("ClassID"),vTmp);
				modPHScal::glClassID=vtoi(vTmp);
			}
			if(modPHScal::glIDIndex == iPA)
				modPHScal::glPAid = modPHScal::glClassID;
			rsTmpZB.GetFieldValue(_T("ID"),vTmp);
			if(vtos(vTmp)==_T(""))
			{
				//对于象华东院标准的附件，如果查不到ID,则查CustomID,然后查得其ID.
				rsTmpZB.GetFieldValue(_T("CustomID"),vTmp);
				tmpID = modPHScal::sFindID(vtos(vTmp));
				rsTmpZB.Edit();
				rsTmpZB.SetFieldValue(_T("ID"),STR_VAR(tmpID));
				rsTmpZB.Update();
			}
			else
			{
				rsTmpZB.GetFieldValue(_T("ID"),vTmp);
				tmpID = vtos(vTmp);
			}
			tmpBlkID =tmpID;
			//首先看表中是否存在ID名称代表的块
			bool bm=false;
			bm=rs.FindFirst( _T("trim(blkID)=\'") + tmpBlkID + _T("\'"));
			if( modPHScal::glIDIndex == iSA)
			{
				//是根部
				if(!bm)
				{
					//如果没有,则肯定是槽钢组成的根部，加上槽钢数量构成新的块,继续查找
					if( modPHScal::glClassID != iGCement)
						tmpBlkID = tmpBlkID +vtos( rsza->GetCollect(_T("iCSnum")));
					bm=rs.FindFirst( _T("trim(blkID)=\'") + tmpBlkID + _T("\'"));
				}
			}
			rsTmpZB.Edit();
			if(!bm)
			{
				//注释掉，防止一些杂乱信息干扰用户。
				tmpStr.Format(GetResStr(IDS_NoThisBlockInphsBlkDimPos),tmpBlkID);
				ShowMessage(tmpStr);
			}
			else
			{
				if( modPHScal::glIDIndex == iSA && modPHScal::glPAid != iPAh)
				{
					rs.GetFieldValue(_T("Zh"),vTmp);
					rsTmpZB.SetFieldValue(_T("Crd"),vTmp);
				}
				else
				{
					rs.GetFieldValue(_T("Dh"),vTmp);
					rsTmpZB.SetFieldValue(_T("Crd"),vTmp);
				}
			}
			rsTmpZB.SetFieldValue(_T("blkID"),STR_VAR(tmpBlkID));
			rsTmpZB.Update();
			rsTmpZB.MoveNext();
		}
	}
	catch(CDaoException * e)
	{
		e->ReportError();
		e->Delete();
	}
	catch(_com_error e)
	{
		ShowMessage(e.Description());
	}
	catch(CString e)
	{
		ShowMessage(e);
	}
}

void Cphs::GetPhsSAELandPAdxdydz()
{
	//目的:计算支吊架定位尺寸，计算支架根部标高。
	try
	{
		CDaoRecordset rs(&EDIBgbl::dbPRJDB);
		CString SQLx;
		COleVariant vTmp;
		modPHScal::glPAid = vtoi(modPHScal::sFindFLD(_T("CustomID"), _T("ClassID"), modPHScal::dn));
		if(! modPHScal::bPAIsHanger())
		{
			//支架
			//If glPAid = iPAs Or glPAid = iPAfixZ1 Or glPAid = iPAfixZ2 Then
			//排除根部(index=iSA)及其附件(记录号为空)的所有零件的高度和
			SQLx = _T("SELECT sum(sizeH) as sumH FROM [") + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] WHERE zdjh=") + ltos(modPHScal::zdjh) + _T(" AND VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" AND IsSAPart<>-1 AND index<>") + ltos(iSA);
			rs.Open(dbOpenSnapshot,SQLx );
			if(!rs.IsEOF() && ! rs.IsBOF())
			{
				//MsgBox rs.RecordCount
				rs.GetFieldValue(_T("sumH"),vTmp);
				modPHScal::sngSEL =modPHScal::sngPEL- (vtof(vTmp)/ 1000);
				modPHScal::UpdateTZB4zdjh(_T("gh1"), _variant_t(modPHScal::sngSEL));
			}
		}
		
		//管部,根部偏装位置值的计算
		if( giUPxyz == 1)
		{
			//X轴向上
			//左视图定位
// 			modPHScal::dxl = modPHScal::dxa + modPHScal::yl;
// 			modPHScal::dxr = modPHScal::dxl + modPHScal::yr1;

            //方向弄反,Modify by luorijin 2009.02.24  将"+"改成"-"
			modPHScal::dxl = modPHScal::dxa - modPHScal::yl;
 			modPHScal::dxr = modPHScal::dxl - modPHScal::yr1;


			//垂直定位
			modPHScal::dyl = modPHScal::sngPEL * 1000 + modPHScal::xl;
			modPHScal::dyr = modPHScal::dyl + modPHScal::xr;
			//右视图定位
			modPHScal::dzl = modPHScal::dza + modPHScal::zl;
			modPHScal::dzr = modPHScal::dzl + modPHScal::zr;
		}
		else if( giUPxyz == 2 )
		{
			//y轴向上
			//左视图定位
			modPHScal::dxl = modPHScal::dxa + modPHScal::xl;
			modPHScal::dxr = modPHScal::dxl + modPHScal::xr;
			//垂直定位
			modPHScal::dyl = modPHScal::sngPEL * 1000 + modPHScal::yl;
			modPHScal::dyr = modPHScal::dyl + modPHScal::yr;
			//右视图定位
			modPHScal::dzl = modPHScal::dza + modPHScal::zl;
			modPHScal::dzr = modPHScal::dzl + modPHScal::zr;
		}
		else if( giUPxyz == 3)
		{
			//z轴向上
			//左视图定位
			modPHScal::dxl = modPHScal::dxa + modPHScal::xl;
			modPHScal::dxr = modPHScal::dxl + modPHScal::xr;
			//垂直定位
			modPHScal::dyl = modPHScal::sngPEL * 1000 + modPHScal::zl;
			modPHScal::dyr = modPHScal::dyl + modPHScal::zr;
			//右视图定位
 			modPHScal::dzl = modPHScal::dza - modPHScal::yl;
// 			modPHScal::dzr = modPHScal::dzl + modPHScal::yr1; 

            //右视图方向弄反,Modify by luorijin 2009.02.24  将"+"改成"-"
			modPHScal::dzr = modPHScal::dzl - modPHScal::yr1;
		}
		modPHScal::UpdatePipeDimHeight(); // 管部标高
	}
	catch(CDaoException *e)
	{
		e->ReportError();
		e->Delete();
	}
}

void Cphs::Class_Initialize()
{
}

void Cphs::Class_Terminate()
{
}

_variant_t Cphs::ClassDebugID()
{
	return _variant_t(_T(""));
}

CString Cphs::GetBHforDoubleCSBeam(float /*ByVal*/ GDW1, float /*ByVal*/ OffsetOfP1toP2, float /*ByVal*/ Span, float /*ByVal*/ p1, float /*ByVal*/ p2, CString /*ByVal*/ SACustomID, int /*Optional*/ iDCS, int /*Optional*/ FirstCal)
{
	//目的:槽钢双吊点简支吊计算。
	//来源:<火力发电厂烟风煤粉管道支吊架设计手册(试行本)>华东电力设计院1977年本
	//输入:GDW1--荷载P1(kgf)距简支梁起点的距离,cm.
	//        OffsetOfP1toP2--荷载P1(kgf)与荷载P2(kgf)的间距,cm.
	//        C--荷载P2距简支梁另外一端的距离,cm,C=Span-GDW1-OffsetOfP1toP2
	//        Span--简支梁跨度,cm
	//        iDCS--槽钢数量，双槽钢iDCS=2,单槽钢iDCS=1，默认为双槽钢
	//        FirstCal--显示提示信息，1=不显示；2=显示
	//  返回:槽钢型号，纯数字,不带前面的标志[。
	
	//	int i;
	float C=0,Wx1=0,Wx=0;
	CString sSQLx,sTmp,sTmp2;
	CString ret;
	COleVariant v;
	C = Span - GDW1 - OffsetOfP1toP2;
	try
	{
		Wx = GDW1 * (p1 * (OffsetOfP1toP2 + C) + p2 * C) / 3200 / Span * (2 / iDCS);
		Wx1 = C * (p2 * (OffsetOfP1toP2 + GDW1) + p1 * GDW1) / 3200 / Span * (2 / iDCS);
		CDaoRecordset rs;
		rs.m_pDatabase=&modPHScal::dbZDJcrude;
		sTmp.Format(_T("%d"),(Wx1>Wx ? Wx1 : Wx));
		sSQLx = _T("SELECT * FROM [SSteelPropertyCS] WHERE Wx>=");
		sSQLx+=sTmp;
		sSQLx+= _T(" ORDER BY ID");
		rs.Open(dbOpenSnapshot,sSQLx);
		if(rs.IsEOF() && rs.IsBOF())
		{
			sTmp.Format(GetResStr(IDS_NotFoundAnyBHinXSteelProperty),_T("SSteelPropertyCS"));
			throw sTmp;		
		}
		else
		{
			rs.GetFieldValue(_T("BH"),v);
			ret=vtos(v);
			sTmp2.Format(GetResStr(IDS_SAMadeBySSteel),_T("\%d"),ret,SACustomID);
			sTmp.Format(sTmp2,iDCS);
			throw sTmp;
		}
		rs.Close();
		return ret;
	}
	catch(::CDaoException * e)
	{
		if(FirstCal==2)
			e->ReportError();
		e->Delete();
	}
	catch(CString e)
	{
		if(FirstCal==2)
		{
			ShowMessage(e);
		}
	}
	return ret;
}

int Cphs::GetPhsStructFromSampleID(long /*ByVal*/ SampleID)
{
	int ret=-1;
	try
	{
		CDaoRecordset rs(&EDIBgbl::dbPHScode); //20071018 "dbSORT" 改为 "dbPHScode"
		_Recordset* rsza = FrmTxsr.m_pViewTxsr->m_ActiveRs;
		CString SQLx,sTmp,sTmp2;
		COleVariant v;
		if(modPHScal::iSelSampleID > 0)
		{
			sTmp.Format(_T("%d"),SampleID);
			SQLx = _T("SELECT * FROM phsStructureName WHERE SampleID=") + sTmp;
			rs.Open(dbOpenDynaset,SQLx);
			if (rs.IsEOF() && rs.IsBOF())
			{
				sTmp2.Format(GetResStr(IDS_NotFoundSampleIDSprNum),_T("\%d"),_T("\%d"));
				sTmp.Format(sTmp2,modPHScal::iSelSampleID,modPHScal::giWholeSprNum);
				ShowMessage(sTmp);
				ret = -1;
			}
			else if(!rs.IsEOF())
			{
				rs.MoveLast();
				rs.GetFieldValue(_T("SampleID"),v);
				ret=vtoi(v);
				rs.GetFieldValue(_T("PA"),v);
				//管部属性赋值
				PA=vtos(v);
				rsza->PutCollect(_T("IDdn1"),stov(PA));
				rs.GetFieldValue(_T("SA"),v);
				//根部属性赋值
				SA=vtos(v);
				rsza->PutCollect(_T("IDgn1"),stov(SA));
				rsza->PutCollect(_T("iSelSampleID"),_variant_t((long)ret));
				//rsza->Update();
			}
		}
		else
			ret = -1;
	}
	catch(_com_error )
	{
	}
	catch(::CDaoException * e)
	{
		e->ReportError();
		e->Delete();
	}
	return ret;
}



Cphs::Cphs()
{
	iAttachIncd=iAttachIncluded;
	iChkCondition=0;
    iSAPAConnect=-1;
	SACSHeight=0;
	rsTZC=NULL;
	rsphsStructureREF.m_pDatabase=&EDIBgbl::dbSORT;
	TZBResultObj=NULL;
	m_rsObj=NULL;
}

Cphs::~Cphs()
{
	if(rsID.IsOpen())
		rsID.Close();
}

void Cphs::InitListRs()
{
	//打开连接表rsConnect和rsUnCheckedType
	try
	{
		CString sSQL;
		if(!rsConnect.IsOpen())
		{
			rsConnect.m_pDatabase=&EDIBgbl::dbPRJ;
			sSQL=(_T("SELECT * FROM connect "));
			rsConnect.Open(dbOpenDynaset,sSQL);
		}
		else
			rsConnect.Requery();
			/*if(rsPictureClipData.IsOpen())
			rsPictureClipData.Close();
			rsPictureClipData.m_pDatabase=&EDIBgbl::dbPRJ;
			sSQL=(_T("SELECT * FROM PictureClipData "));
		rsPictureClipData.Open(dbOpenSnapshot,sSQL);*/
		if(!rsUnCheckedType.IsOpen())
		{
			rsUnCheckedType.m_pDatabase=&EDIBgbl::dbPRJ;
			sSQL=(_T("SELECT * FROM rsUnCheckedType "));
			rsUnCheckedType.Open(dbOpenDynaset,sSQL);
		}
		else
			rsUnCheckedType.Requery();
	}
	catch(::CDaoException * e)
	{
		e->ReportError();
		e->Delete();
	}
	
}

void Cphs::SelectItemRsObj()
{
/*	try
{
CString tbn1, tmpID, _Recordset* rsza;
rsza =FrmTxsr.m_pViewTxsr->m_ActiveRs;
if(m_rsObj==NULL || m_rsObj->BOF || m_rsObj->adoEOF)
{
return;
}
if(rsza==NULL || rsza->BOF || rsza->adoBOF)
return;
modPHScal::gsPartType = vtos(m_rsObj->GetCollect(_T("CustomID")));
tmpID = modPHScal::sFindID(gsPartType);
//MsgBox gsPartType
if(tmpID.Left(1)==_T("G"))
{
modPHScal::iCSnum = vtoi(m_rsObj->GetCollect(_T("P1num")));
rsza->PutCollect(_T("iCSnum"),_variant_t((long)iCSnum));
rsza->PutCollect(_T("gn1"),stov(modPHScal::gsPartType));
rsza->Update();
}
else if(tmpID.Left(1)==_T("D") || tmpID.Left(1)==_T("Z"))
{
rsza->PutCollect(_T("dn1"),stov(modPHScal::gsPartType));
rsza->Update();
}
}
catch(_com_error e)
{
ShowMessage(e.Description());
}*/
}

void Cphs::CloseRecordsets()
{
	//关闭rsConnect和rsUnCheckedType
	try
	{
		if(rsConnect.IsOpen())
			rsConnect.Close();
		if(rsUnCheckedType.IsOpen())
			rsUnCheckedType.Close();
	}
	catch(CException *e)
	{
		e->Delete();
	}
	catch(...)
	{
	}
}

