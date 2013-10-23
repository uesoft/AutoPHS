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
		if(rsID->State != adOpenStatic)
		{
			EDIBgbl::SQLx = _T("SELECT * FROM PictureClipData");
			rsID->Open((_bstr_t)EDIBgbl::SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 
			brsIDStatus=TRUE;
		}
		VARIANT vTmp;
		if(!rsID->Find((_bstr_t)temp, 0, adSearchForward, vTmp))
			return false;
		else
			return true;
	}
	catch(CException *e)
	{
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
	_RecordsetPtr rs;
	HRESULT hr = S_OK;
	hr = rs.CreateInstance(__uuidof(Recordset));
	try
	{
		int	i;
		if(rsID->State != adOpenStatic)
		{
			EDIBgbl::SQLx = _T("SELECT * FROM PictureClipData");
//			rsID.m_pDatabase=&EDIBgbl::dbPRJ;
			rsID->Open((_bstr_t)EDIBgbl::SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
				adOpenForwardOnly, adLockReadOnly, adCmdText); 
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
		if(rs->State == adStateOpen)
		{
			rs->Close();
		}
		rsID->Open((_bstr_t)sSQL,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
			adOpenForwardOnly, adLockReadOnly, adCmdText); 
		
        if (rs->BOF && rs->BOF)
		{
			ShowMessage(GetResStr(IDS_NoAnyRecordInPictureClipData));
			return 0;
		}
		else
		{
			try{ 
				COleVariant vTmp; 
				//COleVariant vTmp;
				ResultObj->ResetContent();
				i=0;
				RsDeleteAll(rsUnCheckedType);
				while(!rs->BOF)
				{
					i++;
					rs->get_Collect((_variant_t)_T("CustomID"), &vTmp);
					rsUnCheckedType->AddNew();
					rsUnCheckedType->put_Collect((_variant_t)_T("CustomID"),vTmp);//STR_VAR(vTmp));
					rsUnCheckedType->Update();
					rs->MoveNext();
				}
			}
			catch(CException *e)
			{
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
	catch(CException *e)
	{
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
	
	_RecordsetPtr rs;
	HRESULT hr = S_OK;
	hr = rs.CreateInstance(__uuidof(Recordset));
	CString FldName;
	CString tmp,sSQL;
	long ret;
	long i,mvClassID;
	
	try
	{
		//假设3个零件要同时匹配
		if(rsID->State != adOpenStatic)
		{
			EDIBgbl::SQLx = _T("SELECT * FROM PictureClipData");
//			rsID.m_pDatabase=&EDIBgbl::dbPRJ;
			rsID->Open((_bstr_t)EDIBgbl::SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 
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
			//rsConnect->Requery(adExecuteRecord);
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

			VARIANT vTmp;
			if(!rsConnect->Find((_bstr_t)sSQL, 0, adSearchForward, vTmp))
			{            
			/*sSQL=CString(_T("SELECT * FROM connect WHERE trim(cntb)=\'"))
			+modPHScal::sFindID(Trim(Cntb))+_T("\'");
				rs.Open(dbOpenSnapshot,sSQL,0); */           
				sSQL=CString(_T(" trim(cntb)=\'"))
					+modPHScal::sFindID(Trim(Cntb))+_T("\'");
				//rsConnect.m_strFilter=sSQL;
				//rsConnect->Requery(adExecuteRecord);
				FldName = _T("cnte");
			}
			else
			{				
				FldName = _T("cntn");//连续3个零件连接匹配
			}
		}
		//是否需要弹簧?
		_variant_t vTmp;
		if(!rsConnect->Find((_bstr_t)sSQL, 0, adSearchForward, vTmp))
		{
			//ShowMessage(GetResStr(IDS_NoCustomIDinConnect));
			return 0;
			//throw;
		}
		
		//保存结果到m
		CString tbn;
		bool bAdd;
//		COleVariant vTmp;
		ResultObj->ResetContent();
		i=0;
		modPHScal::ZdjType=vtos(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("TYPE")));
		do
		{
			i++;
			rsConnect->get_Collect((_variant_t)FldName, &vTmp);
			//CString str = rsConnect.GetSQL();
			tmp=vtos(vTmp);
			tmp.TrimLeft();tmp.TrimRight();tmp.MakeUpper();
			
			if(rsID->Find((_bstr_t)(_T("trim(ID)=\'")+tmp+_T("\'")), 0, adSearchForward, vTmp))
			{
				//找到相应的ID
				rsID->get_Collect((_variant_t)_T("CustomID"), &vTmp);
				tmp=vtos(vTmp);
				tmp.TrimLeft();tmp.TrimRight();tmp.MakeUpper();
				i=ResultObj->FindStringExact(-1,tmp);
				//i=SendMessage(ResultObj->m_hWnd,LB_FINDSTRINGEXACT,0,(long)vTmp.lVal);
				//该部件是否可加入_T("可用框"),bAdd=true可加入
				rsID->get_Collect((_variant_t)_T("ClassID"), &vTmp);
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
					rsID->get_Collect((_variant_t)_T("Index"), &vTmp);
					iIndex=vtoi(vTmp);
					if(iIndex== iPA)
						SQLx.Format(_T("SELECT CustomID FROM tmpCustomIDPA WHERE CustomID=\'%s\'"),tmp);						
					else if(iIndex== iConnectPART)
						SQLx.Format(_T("SELECT CustomID FROM tmpCustomIDPART WHERE CustomID=\'%s\'"),tmp);						
					else if(iIndex== iSA)
						SQLx.Format(_T("SELECT CustomID FROM tmpCustomIDSA WHERE CustomID=\'%s\'"),tmp);
					if(rs->State == adStateOpen) rs->Close();
					rs->Open((_bstr_t)EDIBgbl::SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
						adOpenDynamic, adLockReadOnly, adCmdText); 
					if(rs->BOF && rs->BOF)
						//该零部件不存在于当前规范,不能加入列表框,防止误选
						bAdd=false;
					else
						bAdd=true;
				}
				
				if ((i==-1)&&bAdd) ResultObj->AddString(tmp);
			}//if block
		}
		while(rsConnect->NextRecordset(NULL));
		//把初步获得的可用零件写到临时表RsUnCheckedPart
		ret=i;
		i=SaveRsUnCheckedTypeFromResultObj();		  
   }//try block
   catch(CException *e)
   {
	   e->Delete();
   }
   return ret;
}

long Cphs::SetPhsPreviousTypeToListBox()
{
	//根据选择的连接件型式Cntb和CntbP,确定在其前面与之相连的组件,
	//并将可供选择的型式加入列表框控件ResultObj,以便选择
	//Input:零件Cntb,CntbP(CntbP代表Cntb的前一个零件,可为空)
	//Output:所有可与Cntb(及CntbP)相连的前一个零件都加入到列表框控件ResultObj
	//返回:可用零件个数
	return 0;
}

long Cphs::SaveRsUnCheckedTypeFromResultObj()
{
	int i;
	CString temp;
	try{
		RsDeleteAll(rsUnCheckedType);
		for(i=0;i<ResultObj->GetCount();i++)
		{
			ResultObj->GetText(i,temp);
			rsUnCheckedType->AddNew();
			temp.TrimLeft();temp.TrimRight();
			rsUnCheckedType->put_Collect((_variant_t)_T("CustomID"),STR_VAR(temp));
			rsUnCheckedType->Update();
		}
		return i;
		
	}
	catch(CException *e)
	{
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
	_RecordsetPtr rsTmpCheckedType;
	HRESULT hr = S_OK;
	hr = rsTmpCheckedType.CreateInstance(__uuidof(Recordset));

	CString temp;
	int j=0,m=0;
	long boolk=0 ;
	try{
		InitListRs();
		if ((rsUnCheckedType->adoEOF)&&(rsUnCheckedType->BOF))
		{
			::ShowMessage(GetResStr(IDS_NullRsUnCheckedTypeIndbPRJ));
			goto errH;
		}
		//throw;//Err.Raise iUE_NullRsUnCheckedTypeIndbPRJ, , ResolveResString(iUE_NullRsUnCheckedTypeIndbPRJ)
		else if (!rsUnCheckedType->adoEOF){
			rsUnCheckedType->MoveLast();
			rsUnCheckedType->MoveFirst();
			//'清除临时表rsTmpCheckedType,准备存放检查的结果
			if (EDIBgbl::tdfExists(EDIBgbl::dbPRJ,_T("rsTmpCheckedType")))
				EDIBgbl::dbPRJ->Execute((_bstr_t)_T("DELETE * FROM rsTmpCheckedType"), NULL, adCmdText);
			else
				EDIBgbl::dbPRJ->Execute((_bstr_t)_T("CREATE TABLE rsTmpCheckedType (CustomID char(20))"), NULL, adCmdText);
			MakeRsTZB(idbPRJ, _T("rsTmp"));
			for(m=0;m<rsUnCheckedType->GetRecordCount();m++){
				//清除临时表trsTmpREF,准备存放选择的组合结构链
				if (EDIBgbl::tdfExists(EDIBgbl::dbPRJ,_T("rsTmpREF")))
					EDIBgbl::dbPRJ->Execute((_bstr_t)_T("DELETE * FROM rsTmpREF"), NULL, adCmdText);
				else
					EDIBgbl::dbPRJ->Execute((_bstr_t)_T("CREATE TABLE rsTmpREF (CustomID char(20))"), NULL, adCmdText);
				if (iSAPAConnect!=iPA)//把零件链保存到trsTmpREF表
					for(j=0;j<=SourceObj->GetCurSel();j++){
						SourceObj->GetText(j,temp);
						EDIBgbl::SQLx=CString(_T("INSERT INTO rsTmpREF (CustomID) VALUES (\'"))+Trim(temp)+_T("\')");
						EDIBgbl::dbPRJ->Execute((_bstr_t)EDIBgbl::SQLx, NULL, adCmdText);
					}
					
					//零件链最后一个元素保存到trsTmpREF表
					COleVariant vTmp;
					rsUnCheckedType->get_Collect((_variant_t)_T("CustomID"), &vTmp);
					EDIBgbl::SQLx=CString(_T("INSERT INTO rsTmpREF (CustomID) VALUES (\'"))+vtos(vTmp)+_T("\')");
					EDIBgbl::dbPRJ->Execute((_bstr_t)EDIBgbl::SQLx, NULL, adCmdText);
					
					//'开始步骤2:
					//每次修改AllPrjdb.mdb中ZB的数据表结构之后，莫忘了同时修改F4511s.mdb的ZB表结构及workprj.mdb中的rsTmpZB表的结构。
					//MakeRsTZB(idbPRJ, _T("rsTmp"));
					//开始步骤3:
					EDIBgbl::SQLx = _T("SELECT * FROM rsTmpREF");
					if(rsTmpREF->State == adOpenStatic)
						rsTmpREF->Close();
//					rsTmpREF.m_pDatabase=&EDIBgbl::dbPRJ;
//					rsTmpREF.Open(dbOpenDynaset,EDIBgbl::SQLx);
					rsTmpREF->Open((_bstr_t)EDIBgbl::SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
						adOpenDynamic, adLockReadOnly, adCmdText); 
					//'开始步骤4和5
					bool k=GetphsBHandSizesTest();
					if (k){
						rsUnCheckedType->get_Collect((_variant_t)_T("CustomID"), &vTmp);
						EDIBgbl::SQLx=_T("INSERT INTO rsTmpCheckedType (CustomID) VALUES ('")+vtos(vTmp)+_T("')");
						EDIBgbl::dbPRJ->Execute((_bstr_t)EDIBgbl::SQLx, NULL, adCmdText);
					}
					rsUnCheckedType->MoveNext();
			}//for block
			//开始步骤7:重新填充ResultObj列表框
			
			CString strSQL = _T("SELECT CustomID FROM rsTmpCheckedType");
			rsTmpCheckedType->Open((_bstr_t)strSQL,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 
			if ((rsTmpCheckedType->adoEOF)&&(rsTmpCheckedType->BOF));
			else if (!rsTmpCheckedType->adoEOF){
				rsTmpCheckedType->MoveLast();
				rsTmpCheckedType->MoveFirst();
			}
			//清除列表框以便重新装入
			ResultObj->ResetContent();
			COleVariant vTmp;
			for(j=0;j<rsTmpCheckedType->GetRecordCount();j++)
			{
				rsTmpCheckedType->get_Collect((_variant_t)_T("CustomID"), &vTmp);
				ResultObj->AddString(vtos(vTmp));
				rsTmpCheckedType->MoveNext();
			}
			return rsTmpCheckedType->GetRecordCount();
			//Set rsTmpCheckedType = Nothing
		}
	}
	catch(CException *e)
	{
		e->Delete();
	}
errH:
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
	
	try
	{
//		rs1(&EDIBgbl::dbPHScode), rs(&EDIBgbl::dbPRJ);//20071101 "dbSORT" 改为 "dbPHScode"
		_RecordsetPtr rs1;
		HRESULT hr = S_OK;
		hr = rs1.CreateInstance(__uuidof(Recordset));

		_RecordsetPtr rs;
		hr = rs.CreateInstance(__uuidof(Recordset));
		if(rsID->State != adOpenStatic)
		{
			EDIBgbl::SQLx = _T("SELECT * FROM PictureClipData");
//			rsID.m_pDatabase=&EDIBgbl::dbPRJ;
//			rsID.Open(dbOpenDynaset,EDIBgbl::SQLx);
			rsID->Open((_bstr_t)EDIBgbl::SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 
			brsIDStatus=TRUE;
		}
		
		SQLx = _T("SELECT * FROM phsStructureREF WHERE SampleID=") + ltos(SampleID) + _T(" ORDER BY SEQ ");
//		rs1.Open(dbOpenDynaset,SQLx);
		rs1->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPHScode,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		
		if(rs1->adoEOF && rs1->BOF)
		{
			sTmp.Format(GetResStr(IDS_NoSampleIDinphsStructureREF),ltos(SampleID));
			throw sTmp;
		}
		rs1->MoveLast();
		IC = rs1->GetRecordCount();
		Ptype=new CString[IC];
		COleVariant vTmp;
		iNumSPR = 0;
		iNumCSPR = 0;
		iNumRod = 0;
		sSPR = _T(" ");
		for(i=IC-1;i>=0;i--)
		{
			//注释掉判断句，可加快速度
			rs1->get_Collect((_variant_t)_T("ID"), &vTmp);
			Ptype[i] = vtos(vTmp);
			rs1->MovePrevious();
		}
//		rs.Open(dbOpenSnapshot,_T("SELECT * FROM PictureClipData"));
		SQLx = _T("SELECT * FROM PictureClipData");
		rs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPHScode,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		rs1->MoveFirst();
		for( i = 0 ;i<IC;i++)
		{
			//从第一个开始查找匹配
			if(i == IC - 1)
				//根部
				SQLx = _T("Trim(ID)=\'") + Ptype[i] + _T("\' AND index=0");
			else
				SQLx = _T("Trim(ID)=\'") + Ptype[i] + _T("\'");

			_variant_t vTmp;
			if(!rs->Find((_bstr_t)SQLx, 0, adSearchForward, vTmp))
			{
				sTmp.Format(GetResStr(IDS_NoRecordFLDEqvThisValueInPictureClipData),_T("ID"),Ptype[i]);
				throw sTmp;
			}
			//第一个是管部
			rs->get_Collect((_variant_t)_T("Description"), &vTmp);
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
				rs->get_Collect((_variant_t)_T("ClassID"), &vTmp);
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
			rs->get_Collect((_variant_t)_T("ID"), &vTmp);
			if(rsID->Find((_bstr_t)(_T("trim(ID)=\'") + vtos(vTmp)+_T("\'")), 0, adSearchForward, vTmp))
			{
				rsID->get_Collect((_variant_t)_T("CustomID"), &vTmp);
				strTmp = vtos(vTmp);
//				rs1.Edit();
				rs1->put_Collect((_variant_t)_T("CustomID"),STR_VAR(strTmp));
				rs1->Update();
			}
			rs1->MoveNext();
		}
		rs1->Close();
		
		//开始改样本名称
		//这会清除模板管理表中的使用频度信息
		
		SQLx = _T("SELECT * FROM phsStructureName WHERE SampleID=") + ltos(SampleID);
//		rs1.Open(dbOpenDynaset,SQLx);
		rs1->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPHScode,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		sTmp.Format(_T("%05d"),SampleID);
/*		//如果不使用条件，则名称为空
		rs1->get_Collect((_variant_t)_T("SampleName"), &fi);
		strTmp=ret;
		//此处要注意字符串的字节数
		int n=ret.GetLength();
		CString ss=ret.Left(2);
		if(n > fi.m_lSize)
			strTmp=ret.Left(fi.m_lSize);
*/
		if(rs1->adoEOF && rs1->BOF)
		{
			//下面的开关可在使用SQL和数据记录集对象之间切换。
			//使用SQL时保存新模板名称总是“简支刚性单吊”，而使用对象则不会出现这问题，因此不能以为SQL可能加快速度而放弃使用rs1。
			SQLx.Format(_T("INSERT INTO phsStructureName (SampleID,BmpName,SampleName,PA,SA,SPR,iNumOfPart,iNumSPR,iNumCSPR,iNumRod,FreQuence,bAllowUse) ")
				_T("VALUES (%s,\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',%s,%s,%s,%s,1,-1)"),
				ltos(SampleID),sTmp,strTmp,Ptype[0],Ptype[IC - 1],sSPR,ltos(IC), ltos(iNumSPR),ltos(iNumCSPR),ltos(iNumRod));
			EDIBgbl::dbPHScode->Execute((_bstr_t)SQLx, NULL, adCmdText);//20071101 "dbSORT" 改为 "dbPHScode"
			EDIBgbl::dbPRJ->Execute((_bstr_t)SQLx, NULL, adCmdText);
		}
		else
		{
			SQLx.Format(_T("UPDATE phsStructureName SET BmpName=\'%s\',SampleName=\'%s\',PA=\'%s\',SA=\'%s\',SPR=\'%s\',iNumOfPart=%s,iNumSPR=%s,iNumCSPR=%s,iNumRod=%s,bAllowUse=-1 WHERE SampleID=%s"),
				sTmp,strTmp,Ptype[0],Ptype[IC - 1],sSPR,ltos(IC), ltos(iNumSPR),ltos(iNumCSPR),ltos(iNumRod),ltos(SampleID));
			EDIBgbl::dbPHScode->Execute((_bstr_t)SQLx, NULL, adCmdText);//20071101 "dbSORT" 改为 "dbPHScode"
			EDIBgbl::dbPRJ->Execute((_bstr_t)SQLx, NULL, adCmdText);
		}
		delete [] Ptype;
	}
	catch(CException *e)
	{
		e->Delete();
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
//		   rs1(&EDIBgbl::dbPHScode), rs(&EDIBgbl::dbPRJ);//20071019 "dbSORT" 改为 "dbPHScode"
		   _RecordsetPtr rs1;
		   HRESULT hr = S_OK;
		   hr = rs1.CreateInstance(__uuidof(Recordset));

		   _RecordsetPtr rs;
		   hr = rs.CreateInstance(__uuidof(Recordset));

		   long  i =0, iNumTmp=0;
		   CString tmpStr;
		   CString SQLx = _T("SELECT * FROM PictureClipData");
//		   rs.Open(dbOpenDynaset,SQLx );
		   rs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
			   adOpenDynamic, adLockReadOnly, adCmdText); 
		   if( rs->adoEOF && rs->BOF)
		   {
			   throw GetResStr(IDS_NoAnyRecordInPictureClipData);
		   }
		   SQLx = _T("SELECT * FROM phsStructureREF WHERE SampleID=") + ltos(SampleID) + _T(" ORDER BY SEQ ");
//		   rs1.Open(dbOpenDynaset,SQLx );
		   rs1->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPHScode,true), 
			   adOpenDynamic, adLockReadOnly, adCmdText); 
		   if( rs1->BOF && rs1->adoEOF)
		   {
			   tmpStr.Format(GetResStr(IDS_NoSampleIDinphsStructureREF),ltos(SampleID));
			   throw tmpStr;
		   }
		   COleVariant vTmp;
		   rs1->MoveLast();
		   rs1->MoveFirst();
		   int C=rs1->GetRecordCount();
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
			   rs1->get_Collect((_variant_t)_T("ID"), &vTmp);
			   if(vTmp.vt!=VT_NULL)
			   {
				   Ptype[i]=vtos(vTmp);
				   
				   if(rs->Find((_bstr_t)(_T("Trim(ID)=\'") + Ptype[i] + _T("\'")), 0, adSearchForward, vTmp))
				   {
					   rs->get_Collect((_variant_t)_T("ClassID"), &vTmp);
					   mlPartClassID[i]=vtoi(vTmp);
					   rs->get_Collect((_variant_t)_T("Index"), &vTmp);
					   mlPartIndex[i]=vtoi(vTmp);
					   if(mlPartClassID[i]== iPtype)
					   {
						   if(iPtype == iSPR)
						   {
							   rs->get_Collect((_variant_t)_T("CustomID"), &vTmp);
							   modPHScal::sSprInfo[iNumTmp].CustomID = vtos(vTmp);
							   iNumTmp ++;
						   }
						   ret++;
					   }
				   }
			   }
			   rs1->MoveNext();
		   }
	   }
	   catch(CString e)
	   {
		   ShowMessage(e);
	   }
	   catch(CException *e)
	   {
		   e->Delete();
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
	_RecordsetPtr rs1;
	HRESULT hr = S_OK;
	hr = rs1.CreateInstance(__uuidof(Recordset));
	long iNumTmp=0;
	CString SQLx;
	COleVariant v;
	long ret=0;
	try
	{
//		rs1.m_pDatabase=&EDIBgbl::dbPHScode;//20071018 "dbSORT" 改为 "dbPHScode"
		CString sTmp;
		char sID[64];
		ltoa(SampleID,sID,10);
		if(rsID->State != adOpenStatic)
		{
			SQLx=_T("SELECT * FROM PictureClipData");
//			rsID.m_pDatabase=&EDIBgbl::dbPRJ;
			rsID->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 
		}
		else
			rsID->Requery(adExecuteRecord);
		brsIDStatus=TRUE;
		if( rsID->adoEOF && rsID->BOF )
		{
			ShowMessage(GetResStr(IDS_NoAnyRecordInPictureClipData));
			rsID->Close();
			brsIDStatus=FALSE;
		}
		else
		{
			SQLx = _T("SELECT * FROM phsStructureREF WHERE SampleID=");
			sTmp= sID;
			SQLx=SQLx + sTmp;
			sTmp=_T(" ORDER BY SEQ ");
			SQLx=SQLx + sTmp;
//			rs1.Open(dbOpenDynaset,SQLx);
			rs1->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPHScode,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 
			if( rs1->BOF && rs1->adoEOF )
			{
				sTmp.Format(GetResStr(IDS_NoSampleIDinphsStructureREF),sID);
				ShowMessage(sTmp,MB_TOPMOST|MB_OK);
			}
			else
			{
				while(!rs1->adoEOF)
				{
					rs1->get_Collect((_variant_t)_T("ID"),v);
					if( v.vt!=VT_NULL)
					{
						_variant_t vTmp;
						CString strSql = (_T("Trim(ID)=\'") + vtos(v) + _T("\'"));
						if( rsID->Find((_bstr_t)strSql, 0, adSearchForward, vTmp))
						{
							rsID->get_Collect((_variant_t)_T("ClassID"),v);
							if( vtoi(v) == iPtype )
							{
								if( iPtype == iSPR )
								{
									rsID->get_Collect((_variant_t)_T("CustomID"),v);
									modPHScal::sSprInfo[iNumTmp].CustomID = vtos(v);
									iNumTmp = iNumTmp + 1;
								}
								ret++;
							}
						}
					}
					rs1->MoveNext();
				}
				RCount = rs1->GetRecordCount();
			}
		}
	}
	catch(CException *e)
	{
		e->Delete();
		RCount=rs1->GetRecordCount();
	}
	rs1->Close();
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
		_RecordsetPtr rs1;
		HRESULT hr = S_OK;
		hr = rs1.CreateInstance(__uuidof(Recordset));
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
			+ (LPTSTR)(LPCTSTR)modPHScal::dbZDJcrude->DefaultDatabase + _T(" ;PWD=") + ModEncrypt::gstrDBZdjCrudePassWord + _T("]WHERE Pmax >=") 
			+ ftos(tmppjg) + _T(" AND (Dw >= ") + ftos(modPHScal::dj* (1 - modPHScal::gnDW_delta * 0.01))
			+ _T(" AND Dw <= ") + ftos(modPHScal::dj* (1 + modPHScal::gnDW_delta * 0.01)) 
			+ _T(" AND Dw>0 OR Dw Is NULL OR Dw=0) AND tj >= ") + ftos(modPHScal::t0) + _T("))) ");
		
		strSA.Format(_T(" (SA=\'%s\') "),SA);
		int Gnum;			
		Gnum= (modPHScal::glNumSA!=0 ? modPHScal::glNumSA : 1);
		strSAexists = _T(" ( SA IN ( Select ID FROM PictureClipData ")
			_T(" WHERE EXISTS ( Select CustomID FROM [") + modPHScal::tbnSA + _T("] IN \"\" [; DATABASE=") 
			+ (LPTSTR)(LPCTSTR)modPHScal::dbZDJcrude->DefaultDatabase + _T(" ;PWD=") + ModEncrypt::gstrDBZdjCrudePassWord + _T("] WHERE PictureClipData.CustomID = CustomID AND ((PictureClipData.ClassID= ") + ltos(iG100) + _T(") OR PMAXH >=") 
			+ ftos(tmppjg / Gnum * (vtob(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetCollect(_T("ifLongVertPipe"))) ? Gnum : 1))
			+_T(" AND ((( PictureClipData.ClassID = ")
			+ ltos(iSACantilever) + _T(" OR PictureClipData.ClassID = ") + ltos(iSALbrace) + _T(" OR PictureClipData.ClassID = ") + ltos(iSALbraceFixG47) + _T(" OR PictureClipData.ClassID = ") + ltos(iSALbraceFixG48) + _T(" OR PictureClipData.ClassID = ") + ltos(iG51) + _T(" OR PictureClipData.ClassID = ") + ltos(iG56) + _T(" OR PictureClipData.ClassID = ") + ltos(iG57)  + _T(") AND GDW1 >=")
			+ ftos(modPHScal::gdw1) + _T(") OR (( PictureClipData.ClassID =") + ltos(iSAbeam) +_T(" OR PictureClipData.ClassID =") + ltos(iG52_55) + _T(" ) AND GDW1 >=")
			+ _T(" IIF(")+ ftos(modPHScal::gdw1+(modPHScal::glNumSA==1 ? modPHScal::DblHangerRodDist : 0)/2) +_T(" > L1/2 , L1 - ") + ftos(modPHScal::gdw1+(modPHScal::glNumSA == 1 ? modPHScal::DblHangerRodDist : 0) / 2)
			+ _T(" , ")+ ftos(modPHScal::gdw1+(modPHScal::glNumSA == 1 ? modPHScal::DblHangerRodDist : 0) / 2) + _T(") AND L1> ") + ftos(modPHScal::Lspan)
			+_T(" ) OR (PictureClipData.ClassID= ") + ltos(iGCement) + _T(")))))) ");
		
		SQLx.Format(_T("SELECT * FROM phsStructureName WHERE %s ORDER BY Frequence DESC,iNumOfPart"),
			SQL1 + _T(" AND ") + strPA + _T(" AND ") + strPAexists + _T(" AND ") + strSA + _T(" AND ") + strSAexists );
//		rs1.Open(dbOpenSnapshot,SQLx);
		rs1->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPHScode,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		if( !(rs1->adoEOF && rs1->BOF) )
		{
			//限制管部,限制管部必须存在，限制根部/根部必须存在，有
		}
		else
		{
			//限制管部,限制管部必须存在，限制根部/根部必须存在，没有
			SQLx.Format(_T("SELECT * FROM phsStructureName WHERE %s ORDER BY Frequence DESC,iNumOfPart"),
				SQL1 + _T(" AND ") + strPAexists + _T(" AND ")  + strSA + _T(" AND ") + strSAexists );
			if(rs1->State == adOpenStatic) rs1->Close();
//			rs1.Open(dbOpenSnapshot,SQLx);
			rs1->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPHScode,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 
			if( !(rs1->adoEOF && rs1->BOF) )
			{
				//不限制管部,限制管部必须存在，限制根部/根部必须存在，有
			}
			else
			{
				//不限制管部,限制管部必须存在，限制根部/根部必须存在，没有
				SQLx.Format(_T("SELECT * FROM phsStructureName WHERE %s ORDER BY Frequence DESC,iNumOfPart"),
					SQL1 + _T(" AND ") + strSA + _T(" AND ") + strSAexists );
				if(rs1->State == adOpenStatic) rs1->Close();
				rs1->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPHScode,true), 
					adOpenDynamic, adLockReadOnly, adCmdText); 
				if( !(rs1->adoEOF && rs1->BOF) )
				{
					//不限制管部,不限制管部必须存在，限制根部/根部必须存在，有
				}
				else
				{
					//不限制管部,不限制管部必须存在，限制根部/根部必须存在，没有
					SQLx.Format(_T("SELECT * FROM phsStructureName WHERE %s ORDER BY Frequence DESC,iNumOfPart"),
						SQL1 + _T(" AND ") + strSAexists );
					if(rs1->State == adOpenStatic) rs1->Close();
					rs1->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPHScode,true), 
						adOpenDynamic, adLockReadOnly, adCmdText); 
					if( !(rs1->adoEOF && rs1->BOF) )
					{
						//不限制管部,不限制管部必须存在，不限制根部，限制根部必须存在，有
					}
					else
					{
						//不限制管部,不限制管部必须存在，不限制根部，限制根部必须存在，没有
						SQLx.Format(_T("SELECT * FROM phsStructureName WHERE %s ORDER BY Frequence DESC,iNumOfPart"),
							SQL1 );
						if(rs1->State == adOpenStatic) rs1->Close();
						rs1->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPHScode,true), 
							adOpenDynamic, adLockReadOnly, adCmdText); 
						if( !(rs1->adoEOF && rs1->BOF) )
						{
							//不限制管部,不限制管部必须存在，不限制根部，不限制根部必须存在，有
						}
						else
						{
							//不限制管部,不限制管部必须存在，不限制根部，不限制根部必须存在，没有
							SQL1=(modPHScal::ZdjType == _T("9999") || modPHScal::ZdjType == _T("5000") ? _T("iNumCSPR>=1") : _T("iNumSPR>=1"));
							SQLx.Format(_T("SELECT * FROM phsStructureName WHERE %s ORDER BY Frequence DESC,iNumOfPart"),
								SQL1 );
							if(rs1->State == adOpenStatic) rs1->Close();
							rs1->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPHScode,true), 
								adOpenDynamic, adLockReadOnly, adCmdText); 
							if( !(rs1->adoEOF && rs1->BOF) )
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
		
		if(rs1->State != adOpenStatic || rs1->adoEOF||rs1->BOF)
			return -1;
		rs1->MoveFirst();
		rs1->get_Collect((_variant_t)_T("SampleID"), &vTmp);
		ret=vtoi(vTmp);
		rs1->get_Collect((_variant_t)_T("PA"), &vTmp);
		PA = vtos(vTmp);
		rs1->get_Collect((_variant_t)_T("SA"), &vTmp);
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
	   catch(CException *e)
	   {
		   e->Delete();
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
		_RecordsetPtr rs;
		HRESULT hr = S_OK;
		hr = rs.CreateInstance(__uuidof(Recordset));
		//	int i;
		CString sTmp,sTmp2,SQLx;
		sTmp.Format(_T("%d"),modPHScal::iSelSampleID);
		SQLx = _T("SELECT *  FROM phsStructureREF WHERE SampleID=") + sTmp + _T(" ORDER BY seq");
//		rs.Open(dbOpenSnapshot,SQLx);
		rs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		if(rs->BOF && rs->adoEOF)
		{
			sTmp2.Format(GetResStr(IDS_NoSampleIDinphsStructureREF),sTmp);
			ShowMessage(sTmp2);
			return IDS_NoSampleIDinphsStructureREF;
		}
		else
		{
			rs->MoveLast();
			rs->MoveFirst();
			if(rsTmpREF->State == adOpenStatic)
				rsTmpREF->Close();
			if( EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("rsTmpREF")))
				EDIBgbl::dbPRJ->Execute((_bstr_t)_T("DELETE * FROM rsTmpREF"), NULL, adCmdText);
			else
				EDIBgbl::dbPRJ->Execute((_bstr_t)_T("CREATE TABLE rsTmpREF (CustomID char (20))"), NULL, adCmdText);
			COleVariant vTmp;
			while(!rs->adoEOF)
			{
				rs->get_Collect((_variant_t)_T("CustomID"), &vTmp);
				SQLx = _T("INSERT INTO rsTmpREF (CustomID) VALUES (\'") + vtos(vTmp) + _T("\')");
				EDIBgbl::dbPRJ->Execute((_bstr_t)SQLx, NULL, adCmdText);
				rs->MoveNext();
			}
			
			SQLx = _T("SELECT CustomID FROM rsTmpREF");
			
//			this->rsTmpREF.m_pDatabase=&EDIBgbl::dbPRJ;
//			rsTmpREF.Open(dbOpenDynaset,SQLx);
			rsTmpREF->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 
			return -1;
		}
	}
	catch(CException *e)
	{
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
	_RecordsetPtr rsTmpZB;
	HRESULT hr = S_OK;
	hr = rsTmpZB.CreateInstance(__uuidof(Recordset));
	CString SQLx;
	_variant_t vTmp;

	CString * SampleArray = NULL;
	try
	{
		//初始零件号
		k = 1;
		//删除表中所有字段zdjh值=zdjh的记录
		//modPHScal::MakeZDJ_ZBZG();
		_RecordsetPtr rsX;
		hr = rsX.CreateInstance(__uuidof(Recordset));
		//连接表，保存的是ID值
		if(rsConnect->State != adOpenStatic)
		{
			SQLx = _T("SELECT *  FROM connect");
//			rsConnect.m_pDatabase=&EDIBgbl::dbPRJ;
//			rsConnect.Open(dbOpenDynaset,SQLx);
			rsConnect->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 
		}
		//模板表，保存了ID和CustomID值
		
		sTmp.Format(_T("%d"),modPHScal::iSelSampleID);
		SQLx = CString(_T("SELECT *  FROM phsStructureREF WHERE SampleID=")) + sTmp + _T(" ORDER BY seq");
//		rsX.m_pDatabase=&EDIBgbl::dbPHScode;//20071018 "dbSORT" 改为 "dbPHScode"
		rsX->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPHScode,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		if(rsX->adoEOF && rsX->BOF)
		{
			sTmp2.Format(_T("%d"),modPHScal::iSelSampleID);
			sTmp.Format(GetResStr(IDS_NoSampleIDinphsStructureREF),sTmp2);
			ShowMessage(sTmp);
			ret = IDS_NoSampleIDinphsStructureREF;
		
			return ret;
		}
		else
			rsX->MoveLast();
		long iCount = rsX->GetRecordCount();
		COleVariant rsXv;

		SampleArray = new CString[iCount];

		int i;
		for(i =iCount-1;i >= 0 ;i--)
		{
			rsX->get_Collect((_variant_t)"ID",rsXv);
			SampleArray[i] = vtos(rsXv);
			rsX->MovePrevious();
		}

		
		SQLx = CString(_T("SELECT *  FROM [")) + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] WHERE zdjh=") +ltos(zdjh) + _T(" AND VolumeID=")+ ltos(EDIBgbl::SelVlmID) + _T(" ORDER BY RECNO");
		rsTmpZB->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
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
			else if( i == rsX->GetRecordCount() - 1)
				rsza->PutCollect(_T("gn1"),mvCustomID);
			_variant_t tv;
			rsTmpZB->AddNew();
			rsTmpZB->put_Collect((_variant_t)_T("VolumeID"),COleVariant(EDIBgbl::SelVlmID));
			tv.ChangeType(VT_I4);
			tv.intVal=zdjh;
			rsTmpZB->put_Collect((_variant_t)_T("zdjh"),tv);
			tv.ChangeType(VT_I2);
			tv.iVal=k;
			rsTmpZB->put_Collect((_variant_t)_T("recno"),tv);
			tv.iVal = nth;
			rsTmpZB->put_Collect((_variant_t)_T("nth"),tv);

			rsTmpZB->put_Collect((_variant_t)_T("index"),variant_t((short)vtoi(modPHScal::sFindFLD(_T("ID"), _T("Index"),SampleArray[i]))));
			
			//不是附件
			tv.iVal=(short)vtoi(mvClassID);
			rsTmpZB->put_Collect((_variant_t)_T("ClassID"),tv);
			rsTmpZB->put_Collect((_variant_t)_T("ID"),STR_VAR(SampleArray[i]));
			rsTmpZB->put_Collect((_variant_t)_T("CustomID"),STR_VAR(modPHScal::sFindCustomID(SampleArray[i])));
			rsTmpZB->Update();
			if (i< iCount-1)// Then
			{
				//取得下一次的开头零件
				CString strSql = (_T("trim(cntb)=\'") + (SampleArray[i]) + _T("\' AND trim(cnte)=\'") + (SampleArray[i+1]) + _T("\'"));
				_variant_t vTmp;
				if(!rsConnect->Find((_bstr_t)strSql, 0, adSearchForward, vTmp))
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
							rsConnect->get_Collect((_variant_t) sTmp,v1);
							if (vtos(v1) != _T("") )
							{
								k = k + 1;
								rsTmpZB->AddNew();
								rsTmpZB->put_Collect((_variant_t)_T("VolumeID"),COleVariant(EDIBgbl::SelVlmID));
								rsTmpZB->put_Collect((_variant_t)_T("zdjh"),_variant_t((long)zdjh));
								rsTmpZB->put_Collect((_variant_t)_T("recno"),_variant_t((short)k));
								rsTmpZB->put_Collect((_variant_t)_T("nth"),_variant_t((short)nth));

								//是附件
								rsTmpZB->put_Collect((_variant_t)_T("ClassID"),_variant_t((short)vtoi(modPHScal::sFindFLD(_T("ID"), _T("ClassID"),vtos(v1)))));
								//函数sFindCustomID可以取出数字值
								rsConnect->get_Collect((_variant_t)(_T("CntbF")+ltos(j)+_T("num")), &v2);
								rsTmpZB->put_Collect((_variant_t)_T("CLnum"),v2);

								rsTmpZB->put_Collect((_variant_t)_T("ID"),v1);
								rsTmpZB->put_Collect((_variant_t)_T("CustomID"),STR_VAR(modPHScal::sFindCustomID(vtos(v1))));
								rsTmpZB->Update();
							}
						}
					}
				}
			}
			k = k + 1;
		}
		ret = -1;
		rsTmpZB->Close();
	}
	catch(_com_error )
	{
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

		_RecordsetPtr rs;
		HRESULT hr = S_OK;
		hr = rs.CreateInstance(__uuidof(Recordset));
		TZBResultObj->ResetContent();
		
		CString SQLx;
		SQLx.Format(CString(_T("SELECT * FROM [")) + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] WHERE zdjh=%d AND VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" AND IsSAPart<>-1 ORDER BY recno"),modPHScal::zdjh);
		rs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
//		rs.Open(dbOpenSnapshot,SQLx);
		if(rs->adoEOF && rs->BOF)
		{
			throw GetResStr(IDS_NoComponentForZDJHInTZB);
		}
		rs->MoveLast();
		COleVariant vTmp;
		rs->get_Collect((_variant_t)_T("CustomID"), &vTmp);
		modPHScal::UpdateTZB4zdjh(_T("gn1"), _variant_t(vtos(vTmp)));
		rs->MoveFirst();
		rs->get_Collect((_variant_t)_T("CustomID"), &vTmp);
		modPHScal::UpdateTZB4zdjh(_T("dn1"), _variant_t(vtos(vTmp)));
		while(!rs->adoEOF)
		{
			rs->get_Collect((_variant_t)_T("CustomID"), &vTmp);
			TZBResultObj->AddString(vtos(vTmp));
			rs->MoveNext();
		}
		rs->Close();
	}
	catch(CString )
	{
	}
	catch(CException *e)
	{
		e->Delete();
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
			EDIBgbl::dbPRJDB->Execute((_bstr_t)SQLx, NULL, adCmdText);
		}
		else
		{
			SQLx = CString(_T("DELETE  FROM [rstmp")) +EDIBgbl::Btype[EDIBgbl::TZB]+ _T("] WHERE zdjh=") + sTmp + _T(" AND VolumeID=") + ltos(EDIBgbl::SelVlmID);
			EDIBgbl::dbPRJ->Execute((_bstr_t)SQLx, NULL, adCmdText);
		}
		if(rsTZB->State == adOpenStatic)
			rsTZB->Close();
		if( iDbID == idbPRJDB )
		{
			SQLx = CString(_T("SELECT * FROM [")) + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] WHERE zdjh=") + sTmp + _T(" AND VolumeID=") + ltos(EDIBgbl::SelVlmID);
// 			rsTZB.m_pDatabase=&EDIBgbl::dbPRJDB;
// 			rsTZB.Open(dbOpenDynaset,SQLx);
			rsTZB->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 
		}
		else
		{
			SQLx = CString(_T("SELECT * FROM [rstmp")) + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] WHERE zdjh=") + sTmp + _T(" AND VolumeID=") + ltos(EDIBgbl::SelVlmID);
// 			rsTZB.m_pDatabase=&EDIBgbl::dbPRJ;
// 			rsTZB.Open(dbOpenDynaset,SQLx);
			rsTZB->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 
		}
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

		_RecordsetPtr rsTmp;
		rsTmp.CreateInstance(__uuidof(_Recordset));
		_RecordsetPtr rsX;
		rsX.CreateInstance(__uuidof(_Recordset));
//		rsTmp.m_pDatabase=&EDIBgbl::dbSORT;
//		rsX.m_pDatabase=&modPHScal::dbZDJcrude;
		
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
		COleVariant v,v1,v2;
		if( rsTmpREF->State != adOpenStatic || (rsTmpREF->adoEOF && rsTmpREF->BOF )){
			throw (GetResStr(IDS_Null_rsTmpREF));
			return false;
		}
		//上面Ptype数组保留了mvarrsTmpREF的拷贝
		//以便取得支吊架组合的任意2个相邻零件,进行查找
		if(rsID->State != adOpenStatic)
		{
			EDIBgbl::SQLx = _T("SELECT * FROM PictureClipData");
//			rsID.m_pDatabase=&EDIBgbl::dbPRJ;
//			rsID.Open(dbOpenDynaset,EDIBgbl::SQLx);
			rsID->Open((_bstr_t)EDIBgbl::SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 
			brsIDStatus=TRUE;
		}
		if( ! rsTmpREF->adoEOF )
		{
			rsTmpREF->MoveLast();
			//展开sFindTBN，加快速度。
			//tbn1 = sFindTBN(Trim(rsTmpREF.Fields(_T("CustomID"))))
			rsTmpREF->get_Collect((_variant_t)_T("CustomID"),v);
			if( !rsID->Find((_bstr_t)(_T("trim(CustomID)=\'") + vtos(v) + _T("\'")), 0, adSearchForward, vTmp))
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
			rsID->get_Collect((_variant_t)_T("Index"),v);
			mvSA = vtoi(v);
			
			if( rsTmpREF->GetRecordCount() >= 2 )
			{
				//零件数量多于或等于2个，则可以取出头尾零件。
				rsTmpREF->MoveFirst();
				//展开sFindTBN，加快速度。
				//tbn1 = sFindTBN(Trim(rsTmpREF.Fields(_T("CustomID"))))
				rsTmpREF->get_Collect((_variant_t)_T("CustomID"),v);
				CString strSql = (_T("trim(CustomID)=\'")) + vtos(v) + _T("\'");
				_variant_t vTmp;
				if( !rsID->Find((_bstr_t)strSql, 0, adSearchForward, vTmp))
				{
					//没找到CustomID,出错
					sTmp.Format(GetResStr(IDS_NoRecordFLDEqvThisValueInPictureClipData),_T("CustomID"),vtos(v));
					throw (sTmp);
					return false;
				}
				//假设第一个零件是管部，保存管部类别码。
				rsID->get_Collect((_variant_t)_T("Index"),v);
				mvPA = vtoi(v);
				
			}
			CString sv,sv1;
			//根部总重量初始值=0
			mvarSATotalWeight = 0;
			//开始计算
			for (i = 0 ;i< rsTmpREF->GetRecordCount();i++)
			{
				//获得零件所在的原始数据表tbn1,以便查找
				//展开sFindTBN，加快速度。
				//tbn1 = sFindTBN(Trim(rsTmpREF.Fields(_T("CustomID"))))
				rsTmpREF->get_Collect((_variant_t)_T("CustomID"),v);
				sv=vtos(v);
				
				if(!rsID->Find((_bstr_t)CString(_T("trim(CustomID)=\'") + sv + _T("\'")), 0, adSearchForward, vTmp))
				{
					//没有找到表名
					sTmp.Format(GetResStr(IDS_NoRecordFLDEqvThisValueInPictureClipData),_T("CustomID"),vtos(v));
					throw (sTmp);
					return false;
				}
				else 
				{
					rsID->get_Collect((_variant_t)_T("ClassID"),v);
					modPHScal::glClassID = vtoi(v);
					rsID->get_Collect((_variant_t)_T("Index"),v);
					modPHScal::glIDIndex =vtoi(v);
					rsID->get_Collect((_variant_t)_T("ID"),v);
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
					if( i < rsTmpREF->GetRecordCount() - 1 )
					{
						rsID->get_Collect((_variant_t)_T("ID"),v);
						tmpID1=vtos(v);
						/*rsID->Find((_bstr_t)CString(_T("trim(CustomID)=\'")) + sv + _T("\'"))*/
						if( rsID->Find((_bstr_t)CString(_T("trim(CustomID)=\'") + sv + _T("\'")), 0, adSearchForward, vTmp))
						{
							rsID->get_Collect((_variant_t)_T("ID"),v);
							tmpID1 = vtos(v);
						}
						//不是最后一个零件\
						//从Connect表中检索其是否存在及其匹配的尺寸信息
						//tmpCustomID1 = Ptype(i + 1);
						rsTmpREF->MoveNext();
						if(!rsTmpREF->adoEOF)
						{
							rsTmpREF->get_Collect((_variant_t)_T("CustomID"),v);
							sv1=vtos(v);
							sv1.TrimLeft();sv1.TrimRight();
							tmpCustomID1=sv1;
						}
						rsTmpREF->MovePrevious();
						if( !rsConnect->Find((_bstr_t)CString(_T("trim(cntb)=\'") + tmpID0 + _T("\' AND trim(cnte)=\'") + tmpID1 + _T("\'")) , 0, adSearchForward, vTmp))
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
							/*}*/
							rsConnect->get_Collect((_variant_t)_T("CntbNum"),v);                
							if( vtoi(v)<= 0 )
							{
								iCntbNum = 1;
							}else 
							{
								iCntbNum = vtoi(v);//rsConnect.Fields(_T("CntbNum"))
							}
							rsConnect->get_Collect((_variant_t)_T("CnteNum"),v);                
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
				else if( i == rsTmpREF->GetRecordCount() - 1 && modPHScal::glIDIndex == iSA )
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
						if(rsX->State == adOpenStatic)
							rsX->Close();
//						rsX.Open(dbOpenSnapshot,SQLx);
						rsX->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
							adOpenDynamic, adLockReadOnly, adCmdText); 
						if( rsX->adoEOF && rsX->BOF )
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
						if(rsX->State == adOpenStatic)
							rsX->Close();
//						rsX.Open(dbOpenSnapshot,SQLx);
						rsX->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
							adOpenDynamic, adLockReadOnly, adCmdText); 
						if( rsX->adoEOF && rsX->BOF ){
						}else {
							//找到，退出循环
							goto commonHandle;
						}
						
					}

					//2007.09.27(start)
					if(modPHScal::gbPAForceZero && (rsX->adoEOF && rsX->BOF))
					{
						for (k = 0 ;k<=(int) modPHScal::gnDW_delta;k++)
						{
							//SQLx = _T("SELECT * FROM ") & tbnPA & _T(" WHERE Dw>= ") & modPHScal::dj * (1 - k * 0.01) & _T(" AND Dw<= ") & modPHScal::dj * (1 + k * 0.01) & _T(" AND Pmax>=") & tmpSelPJG / iNumPart & _T(" AND Tj>=") & tmpT0 & _T(" AND trim(CustomID)='") & Ptype(i) & _T("' ORDER BY Dw,tj,Weight")
							
							sTmp.Format(_T("%g"),modPHScal::dj * (1 - k * 0.01));
							SQLx = _T("SELECT * FROM ") + modPHScal::tbnPA + _T(" WHERE Dw>= ") + sTmp;
							sTmp.Format(_T("%g"),modPHScal::dj * (1 + k * 0.01));
							SQLx+=(_T(" AND Dw<= ") + sTmp + _T(" AND Pmax IS NULL ")) ;
							SQLx+= (_T(" AND Tj>=") + stmpT0 + _T(" AND trim(CustomID)=\'") + sv + _T("\' ORDER BY tj,Dw,Weight"));
							if(rsX->State == adOpenStatic)
								rsX->Close();
//							rsX.Open(dbOpenSnapshot,SQLx);
							rsX->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
								adOpenDynamic, adLockReadOnly, adCmdText); 
							if( rsX->adoEOF && rsX->BOF ){
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
			 if(rsX->State == adOpenStatic)
				 rsX->Close();
//			 rsX.Open(dbOpenSnapshot,SQLx);
			 rsX->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
				 adOpenDynamic, adLockReadOnly, adCmdText); 
         }
spZ1Z2:
         if( rsX->adoEOF && rsX->BOF )
		 {
			 //Err.Raise iUE_NotFoundPartInZDJCrude, , ResolveResString(iUE_NotFoundPartInZDJCrude, _T("|1"), Ptype(i), _T("|2"), SQLx, _T("|3"), Iif((i > 0, Ptype[i-1], _T("")), _T("|4"), Iif((i < rsTmpREF.RecordCount - 1, Ptype(i + 1), _T("")))
			 if( i > 0 )
			 {
				 CString sv2;
				 rsTmpREF->MovePrevious();
				 if(!rsTmpREF->BOF)
				 {
					 rsTmpREF->get_Collect((_variant_t)_T("CustomID"),v);
					 sv2=vtos(v);
				 }
				 rsTmpREF->MoveNext();
				 if( i < rsTmpREF->GetRecordCount() - 1 ){
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
				 rsTmpREF->MovePrevious();
				 if(!rsTmpREF->BOF)
				 {
					 rsTmpREF->get_Collect((_variant_t)_T("CustomID"),v);
					 sv2=vtos(v);
				 }
				 rsTmpREF->MoveNext();
				 sTmp.Format(GetResStr(IDS_NotFoundPartInZDJCrude),SQLx,sv,_T("NULL"),sv1);
				 throw (sTmp);
				 return false;
			 }
         }
		 else 
		 {//1
			 rsX->MoveFirst();
			 if(phsAvailableTypeRs->State == adOpenStatic)
				 phsAvailableTypeRs->Close();
//			 phsAvailableTypeRs.m_pDatabase=rsX.m_pDatabase;
//			 phsAvailableTypeRs.Open(dbOpenDynaset,rsX.GetSQL());
			 phsAvailableTypeRs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
				 adOpenDynamic, adLockReadOnly, adCmdText); 
			 //管部测试计算，此时可以退出
			 if( modPHScal::gbAddPartWeight2PMax && modPHScal::glIDIndex != iSA )
			 {
				 //累加重量到计算荷载
				 //忽略附件、螺栓螺母重量，粗略计算拉杆重量（按1m考虑）
				 rsX->get_Collect((_variant_t)_T("Weight"),v);
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
         rsTmpREF->MoveNext();
      }//2
      return true;
   }
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
	   return false;
}

void Cphs::simplify2(_RecordsetPtr rsPartBoltNuts,int nth)
{
	CString SQLx;
	try
	{
		
        SQLx.Format("SELECT * FROM [ZB] WHERE [VolumeID]=%d AND [ZDJH]=%d AND [nth]=%d AND ( [ClassID]=%d OR [ClassID]=%d OR [ClassID]=%d ) AND [IsSAPart]<>-1 ORDER BY [recno]",
			EDIBgbl::SelVlmID,modPHScal::zdjh ,nth,iBolts,iNuts,iAttached);
//		rsPartBoltNuts->Open(dbOpenDynaset,SQLx);				
		rsPartBoltNuts->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
	}
	catch(CException *e)
	{
		e->Delete();
	}
	
	
}
#pragma warning( disable : 4129 )  //Added by Shuli Luo
void Cphs::simplify(_RecordsetPtr rsSAPart,int nth)
{
	try{
		CString SQLx;
		SQLx.Format("SELECT * FROM [ZB] WHERE [VolumeID]=%d AND [ZDJH]=%d AND [nth]=%d AND [IsSAPart]<>0 AND IsSAPart IS NOT NULL ORDER BY recno ",
			EDIBgbl::SelVlmID,modPHScal::zdjh,nth);
//		rsSAPart.Open(dbOpenDynaset,SQLx);
		rsSAPart->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		if( rsSAPart->BOF && rsSAPart->adoEOF )
			;
		else
		{
			//删除ZB表中本路支吊架的根部附件以免重复添加根部的附件	
			if(!rsSAPart->BOF) 
				rsSAPart->MoveFirst();
			for(; !rsSAPart->adoEOF; rsSAPart->MoveFirst())
			{		
				rsSAPart->Delete(adAffectCurrent);
				if( rsSAPart->adoEOF )
					break;	
			}
		}
	}
	catch(CException *e)
	{
		e->Delete();
	}
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
//	 rsX(&modPHScal::dbZDJcrude), rsTmp, rs(&modPHScal::dbZDJcrude);
//	 rsTmpZB(&EDIBgbl::dbPRJDB);
	_RecordsetPtr rsX, rsTmp, rs, rsTmpZB;
	rsX.CreateInstance(__uuidof(Recordset));
	rsTmp.CreateInstance(__uuidof(Recordset));
	rs.CreateInstance(__uuidof(Recordset));
	rsTmpZB.CreateInstance(__uuidof(Recordset));

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
//		rsTmpZB.m_pDatabase=&EDIBgbl::dbPRJDB;
//		rsTmpZB.Open(dbOpenDynaset,SQLx);
		rsTmpZB->Open((_bstr_t)SQLx, EDIBgbl::dbPRJDB.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);
		if( rsTmpZB->adoEOF && rsTmpZB->BOF ){
			strTmp.Format(GetResStr(IDS_NoThisZDJHResultInTBNSelPrjspecTZB),  EDIBgbl::dbPRJDB->DefaultDatabase, EDIBgbl::Btype[EDIBgbl::TZB], SQLx);
			throw strTmp;
		}
		//获得所有零件（包括附件）
		rsTmpZB->MoveLast();
		rsTmpZB->MoveFirst();
		lngCurrentPartNo = -1;
		IC = rsTmpZB->GetRecordCount();
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
			rsTmpZB->get_Collect((_variant_t)_T("CustomID"), &vTmp1);
			NewPtypeCustomID[i] = vtos(vTmp1);
			rsTmpZB->get_Collect((_variant_t)_T("ID"), &vTmp1);
			NewPtypeID[i] = vtos(vTmp1);
			rsTmpZB->get_Collect((_variant_t)_T("ClassID"), &vTmp1);
			NewPtypeClassID[i] = vtoi(vTmp1);
			rsTmpZB->get_Collect((_variant_t)_T("index"), &vTmp1);
			NewPtypeIndex[i] = vtoi(vTmp1);
			if( NewPtypeClassID[i] == iROD ){
				//拉杆
				rsTmpZB->get_Collect((_variant_t)_T("recno"), &vTmp1);
				rsTmpZB->get_Collect((_variant_t)_T("sizeC"), &vTmp2);
				iLugDia[vtoi(vTmp1)] =vtoi(vTmp2); //拉杆直径保存在sizeC字段
				//iRecNoOfLugDia(iLugNo) = RsTZB.Fields(_T("recno"))   //起始号为1
				lngCurrentPartNo++;
				//ReDim Preserve iRecNo(lngCurrentPartNo)
				//ReDim Preserve PtypeCustomID(lngCurrentPartNo)
				PtypeCustomID[lngCurrentPartNo] =NewPtypeCustomID[i];
				rsTmpZB->get_Collect((_variant_t)_T("recno"), &vTmp1);
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
						rsTmpZB->get_Collect((_variant_t)_T("T"), &vTmp);
						tmpT = vtoi(vTmp);
					}
					lngCurrentPartNo ++;
					//ReDim Preserve iRecNo(lngCurrentPartNo)
					//ReDim Preserve PtypeCustomID(lngCurrentPartNo)
					PtypeCustomID[lngCurrentPartNo]= NewPtypeCustomID[i];
					rsTmpZB->get_Collect((_variant_t)_T("recno"), &vTmp);
					iRecNo[lngCurrentPartNo] = vtoi(vTmp);
					//复位附件序号和球面垫圈发现标志
					lngAttachedPartNo = -1;
					bFoundF4 = false;
					//发现G12或G13
					if( NewPtypeID[i] == _T("G12") || NewPtypeID[i] == _T("G13") )
						bFoundG12G13 = true;
				}
			}
			rsTmpZB->MoveNext();
		}
		
		//正式开始
		rsTmpZB->MoveFirst();
		lngCurrentPartNo = -1;
		n = 0;
		miSEQofCSPR = -1;
		for (i = 0 ;i< IC;i++)
		{
			//找出恒力弹簧的CustomID,便于查表
			
			rsID->Find((_bstr_t)(_T("trim(ID)=\'") + NewPtypeID[i] + _T("\'")), 0, adSearchForward, vTmp);
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
					rsTmpZB->get_Collect((_variant_t)_T("sizeC"), &vTmp);
					SQLx = _T("SELECT * FROM [") + modPHScal::tbnBoltsSurplusLength + _T("] WHERE Diameter=") + vtos(vTmp);
//					rsTmp.m_pDatabase=&modPHScal::dbZDJcrude;
//					rsTmp.Open(dbOpenSnapshot,SQLx );
					rsTmp->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
						adOpenDynamic, adLockReadOnly, adCmdText); 
					if( rsTmp->adoEOF && rsTmp->BOF )
					{
						//没找到螺栓直径，可能数据表数据不全。
						strTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb),modPHScal::dbZDJcrude->DefaultDatabase,modPHScal::tbnBoltsSurplusLength,SQLx);
						throw strTmp;
					}
//					rsTmpZB.Edit();
					if( AttachmentGroupHavingF4[lngCurrentPartNo] )
					{
						//附件组含球面垫圈(F4)
						rsTmp->get_Collect((_variant_t)_T("SurplusLengthWithF1F2F4"), &vTmp);
					}
					else
					{
						rsTmp->get_Collect((_variant_t)_T("SurplusLengthWithF1F2"), &vTmp);
					}
					rsTmpZB->put_Collect((_variant_t)_T("A"),COleVariant(vtof(vTmp)));
					//同时将拉杆伸出段最小长度保存在字段C，便于计算拉杆长度函数CalLugLength处理
					rsTmpZB->put_Collect((_variant_t)_T("C"),COleVariant(vtof(vTmp)));
					rsTmpZB->Update();
					rsTmp->Close();
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
					rsTmpZB->get_Collect((_variant_t)_T("sizeC"), &vTmp);
					SQLx = _T("SELECT * FROM [") + modPHScal::tbnBoltsSurplusLength + _T("] WHERE Diameter=") +vtos(vTmp);
//					rsTmp.m_pDatabase=&modPHScal::dbZDJcrude;
//					rsTmp.Open(dbOpenSnapshot,SQLx);
					rsTmp->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
						adOpenDynamic, adLockReadOnly, adCmdText); 
					if( rsTmp->adoEOF && rsTmp->BOF ){
						//没找到螺栓直径，可能数据表数据不全。
						strTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb),modPHScal::dbZDJcrude->DefaultDatabase,modPHScal::tbnBoltsSurplusLength,SQLx);
						throw strTmp;
					}
//					rsTmpZB.Edit();
					if( AttachmentGroupHavingF4[lngCurrentPartNo]){
						//附件组含球面垫圈(F4)
						rsTmp->get_Collect((_variant_t)_T("SurplusLengthWithF1F2F4"), &vTmp);
					}
					else{
						rsTmp->get_Collect((_variant_t)_T("SurplusLengthWithF1F2"), &vTmp);
					}
					rsTmpZB->put_Collect((_variant_t)_T("B"),COleVariant(vtof(vTmp)));
					//同时将拉杆伸出段最小长度保存在字段C，便于计算拉杆长度函数CalLugLength处理
					rsTmpZB->put_Collect((_variant_t)_T("C"),COleVariant(vtof(vTmp)));
					rsTmpZB->Update();
					rsTmp->Close();
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
//								rsTmp.m_pDatabase=&modPHScal::dbZDJcrude;
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
//									rsTmp.Open(dbOpenSnapshot,SQLx);
									rsTmp->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
										adOpenDynamic, adLockReadOnly, adCmdText); 
									if( rsTmp->adoEOF && rsTmp->BOF )
									{
										//没找到螺栓直径，可能数据表数据不全。
										rsTmp->Close();
										if( !modPHScal::gbAutoApplyCSPRLugDia )
										{
											//自动调整恒力弹簧拉杆直径
											SQLx = _T("SELECT * FROM [") + modPHScal::tbnLugDiaOfCSPR + _T("] WHERE minDH<=") + ltos(modPHScal::sSprInfo[miSEQofCSPR].DH) + _T(" AND maxDH>=") + ltos(modPHScal::sSprInfo[miSEQofCSPR].DH) + _T(" AND trim(CustomID)=\'") + PtypeCustomID[lngCurrentPartNo] + _T("\' ");
//											rsTmp.Open(dbOpenSnapshot,SQLx);
											rsTmp->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
												adOpenDynamic, adLockReadOnly, adCmdText); 
											if( rsTmp->adoEOF && rsTmp->BOF ){
												//没找到螺栓直径，可能数据表数据不全。
												strTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb), modPHScal::dbZDJcrude->DefaultDatabase, modPHScal::tbnBoltsSurplusLength, SQLx);
												throw strTmp;
											}
										}
										else
										{
											strTmp.Format(GetResStr(IDS_NotClearCheckAutoApplyCSPRLugDia),modPHScal::dbZDJcrude->DefaultDatabase,modPHScal::tbnBoltsSurplusLength,SQLx);
											throw strTmp;
										}
									}
									//if( True ){
									//获得螺栓孔径FiK
									rsTmp->get_Collect((_variant_t)_T("FiK"), &vTmp);
									iBoltsNutsDia[i] =vtoi(vTmp);
									//查找螺栓直径系列表tbnCSPRDiameterSerial,获得准确直径
									SQLx = _T("SELECT * FROM [") + modPHScal::tbnCSPRDiameterSerial + _T("] WHERE Diameter<") + ltos(iBoltsNutsDia[i]) + _T(" ORDER BY Diameter DESC");
//									rs.Open(dbOpenSnapshot,SQLx );
									rs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
										adOpenDynamic, adLockReadOnly, adCmdText); 
									if( rs->adoEOF && rs->BOF ){
										//没有找到匹配直径系列值
										strTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb), modPHScal::dbZDJcrude->DefaultDatabase,modPHScal::tbnBoltsSurplusLength, SQLx);
										throw strTmp;
									}
									rs->get_Collect((_variant_t)_T("Diameter"), &vTmp);
									iBoltsNutsDia[i] =vtoi(vTmp);
									rsTmp->get_Collect((_variant_t)_T("S"), &vTmp1);
									rsTmp->get_Collect((_variant_t)_T("I"), &vTmp2);
									LenBolts = vtoi(vTmp1) + 2 * vtoi(vTmp2);
									rsTmp->Close();
									//按照华东院支吊架手册的螺栓长度裕量计算准则计算螺栓长度。
									SQLx = _T("SELECT * FROM [") + modPHScal::tbnBoltsSurplusLength + _T("] WHERE Diameter=") + ltos(iBoltsNutsDia[i]) + _T(" ORDER BY Diameter");
//									rsTmp.Open(dbOpenSnapshot,SQLx);
									rsTmp->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
										adOpenDynamic, adLockReadOnly, adCmdText); 
									if( rsTmp->adoEOF && rsTmp->BOF )
									{
										//没找到螺栓直径，可能数据表数据不全。
										strTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb),modPHScal::dbZDJcrude->DefaultDatabase,modPHScal::tbnBoltsSurplusLength, SQLx);
										throw strTmp;
									}
									if( AttachmentGroupHavingF4[lngCurrentPartNo] )
									{
										//附件组含垫片(F4)
										rsTmp->get_Collect((_variant_t)_T("SurplusLengthWithF1F2F4"), &vTmp);
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
										rsTmp->get_Collect((_variant_t)_T("SurplusLengthWithF1F2"), &vTmp);
										if( NewPtypeID[i] == _T("F10") )
										{    //双头螺栓
											LenBolts += 2 * vtof(vTmp);
										}
										else if( NewPtypeID[i] == _T("F9") || NewPtypeID[i] == _T("F14") )
										{     //单头螺栓/六角头螺栓
											LenBolts +=vtof(vTmp);
										}
									}
									rsTmp->Close();
									rs->Close();
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
//									rsTmp.Open(dbOpenSnapshot,SQLx);
									rsTmp->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
										adOpenDynamic, adLockReadOnly, adCmdText); 
									if( rsTmp->adoEOF && rsTmp->BOF ){
										//没找到螺栓直径，可能数据表数据不全。
										rsTmp->Close();
										if( ! modPHScal::gbAutoApplyCSPRLugDia )
										{
											//自动调整恒力弹簧拉杆直径
											SQLx = _T("SELECT * FROM [") + modPHScal::tbnLugDiaOfCSPR + _T("] WHERE minDH<=") + ltos(modPHScal::sSprInfo[miSEQofCSPR].DH) + _T(" AND maxDH>=") + ltos(modPHScal::sSprInfo[miSEQofCSPR].DH) + _T(" AND trim(CustomID)=\'") + PtypeCustomID[lngCurrentPartNo] + _T("\' ");
//											rsTmp.Open(dbOpenSnapshot,SQLx);
											rsTmp->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
												adOpenDynamic, adLockReadOnly, adCmdText); 
											if( rsTmp->adoEOF && rsTmp->BOF )
											{
												//没找到螺栓直径，可能数据表数据不全。
												strTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb),modPHScal::dbZDJcrude->DefaultDatabase, modPHScal::tbnBoltsSurplusLength,  SQLx);
												throw strTmp;
											}
										}
										else
										{
											strTmp.Format(GetResStr(IDS_NotClearCheckAutoApplyCSPRLugDia),modPHScal::dbZDJcrude->DefaultDatabase,modPHScal::tbnBoltsSurplusLength, SQLx);
											throw strTmp;
										}
									}
									//获得螺栓孔径FiK
									rsTmp->get_Collect((_variant_t)_T("FiK"), &vTmp);
									rsTmp->Close();
									iBoltsNutsDia[i] = vtoi(vTmp);
									//查找螺栓直径系列表tbnCSPRDiameterSerial,获得准确直径
									SQLx = _T("SELECT * FROM [") + modPHScal::tbnCSPRDiameterSerial + _T("] WHERE Diameter<") + ltos(iBoltsNutsDia[i]) + _T(" ORDER BY Diameter DESC");
//									rs.Open(dbOpenSnapshot,SQLx);
									rs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
										adOpenDynamic, adLockReadOnly, adCmdText); 
									if( rs->adoEOF && rs->BOF )
									{
										//没有找到匹配直径系列值
										strTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb), modPHScal::dbZDJcrude->DefaultDatabase, modPHScal::tbnBoltsSurplusLength, SQLx);
										throw strTmp;
									}
									rs->get_Collect((_variant_t)_T("Diameter"), &vTmp);
									iBoltsNutsDia[i] = vtoi(vTmp);
									LenBolts =SACSHeight + tmpT;
									//按照华东院支吊架手册的螺栓长度裕量计算准则计算螺栓长度。
									SQLx = _T("SELECT * FROM [") + modPHScal::tbnBoltsSurplusLength + _T("] WHERE Diameter=") + ltos(iBoltsNutsDia[i]) + _T(" ORDER BY Diameter");
//									rsTmp.Open(dbOpenSnapshot,SQLx);
									rsTmp->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
										adOpenDynamic, adLockReadOnly, adCmdText); 
									if( rsTmp->adoEOF && rsTmp->BOF )
									{
										//没找到螺栓直径，可能数据表数据不全。
										strTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb), modPHScal::dbZDJcrude->DefaultDatabase,modPHScal:: tbnBoltsSurplusLength, SQLx); 
										throw strTmp;
									}
									if( AttachmentGroupHavingF4[lngCurrentPartNo] ){
										//附件组含垫片(F4)
										rsTmp->get_Collect((_variant_t)_T("SurplusLengthWithF1F2F4"), &vTmp);
										if( NewPtypeID[i] == _T("F10") ){    //双头螺栓
											LenBolts += 2 * vtof(vTmp);
										}else if( NewPtypeID[i] == _T("F9") || NewPtypeID[i] == _T("F14") ){     //单头螺栓/六角头螺栓
											LenBolts +=vtof(vTmp);
										}
									}
									else{
										rsTmp->get_Collect((_variant_t)_T("SurplusLengthWithF1F2"), &vTmp);
										if( NewPtypeID[i] == _T("F10") ){    //双头螺栓
											LenBolts +=2*vtof(vTmp);
										}else if( NewPtypeID[i] == _T("F9") || NewPtypeID[i] == _T("F14") ){     //单头螺栓/六角头螺栓
											LenBolts +=vtof(vTmp);
										}
									}
									rsTmp->Close();
									rs->Close();
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
//									rsTmp.Open(dbOpenSnapshot,SQLx);
									rsTmp->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
										adOpenDynamic, adLockReadOnly, adCmdText); 
									if( rsTmp->adoEOF && rsTmp->BOF )
									{
										//没找到螺栓直径，可能数据表数据不全。
										strTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb), modPHScal::dbZDJcrude->DefaultDatabase,modPHScal:: tbnBoltsSurplusLength, SQLx); 
										throw strTmp;
									}
									//拉杆直径是标准的，不需要查找系列表
									rsTmp->get_Collect((_variant_t)_T("FiJ"), &vTmp);iBoltsNutsDia[i]=vtoi(vTmp);
									LenBolts = Cavphs->SACSHeight + 15;
									//按照华东院支吊架手册的螺栓长度裕量计算准则计算螺栓长度。
									SQLx = _T("SELECT * FROM [") + modPHScal::tbnBoltsSurplusLength + _T("] WHERE Diameter=") + ltos(iBoltsNutsDia[i]);
									rsTmp->Close();
//									rsTmp.Open(dbOpenSnapshot,SQLx);
									rsTmp->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
										adOpenDynamic, adLockReadOnly, adCmdText); 
									if( rsTmp->adoEOF && rsTmp->BOF ){
										//没找到螺栓直径，可能数据表数据不全。
										strTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb), modPHScal::dbZDJcrude->DefaultDatabase,modPHScal:: tbnBoltsSurplusLength, SQLx); 
										throw strTmp;
									}
									if( AttachmentGroupHavingF4[lngCurrentPartNo] ){
										//附件组含垫片(F4)
										rsTmp->get_Collect((_variant_t)_T("SurplusLengthWithF1F2F4"), &vTmp);
										if( NewPtypeID[i] == _T("F10") ){    //双头螺栓
											LenBolts+= 2 * vtof(vTmp);
										}else if( NewPtypeID[i] == _T("F9") || NewPtypeID[i] == _T("F14") ){     //单头螺栓/六角头螺栓
											LenBolts +=vtof(vTmp);
										}
									}else{
										rsTmp->get_Collect((_variant_t)_T("SurplusLengthWithF1F2"), &vTmp);
										if( NewPtypeID[i] == _T("F10") ){    //双头螺栓
											LenBolts+= 2 * vtof(vTmp);
										}else if( NewPtypeID[i] == _T("F9") || NewPtypeID[i] == _T("F14") ){     //单头螺栓/六角头螺栓
											LenBolts +=vtof(vTmp);
										}
									}
									rsTmp->Close();
								}
								
								//首先，在螺栓螺母表里查找附件或螺栓螺母
								
								tbn1 = modPHScal::tbnBoltsNuts;
								if( bCalBoltsDiaOfCSPR )
								{
									CString sID;
									rsTmpZB->get_Collect((_variant_t)_T("ID"), &vTmp);
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
//									rsX.Open(dbOpenSnapshot,SQLx);
									rsX->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
										adOpenDynamic, adLockReadOnly, adCmdText); 
									if( rsX->adoEOF && rsX->BOF )
									{
										//如果，在螺栓螺母表里没找到附件或螺栓螺母
										rsTmpZB->get_Collect((_variant_t)_T("CustomID"), &vTmp);
										strTmp.Format(GetResStr(IDS_NoFoundAttachInZDJCrude),modPHScal::dbZDJcrude->DefaultDatabase,SQLx,vtos(vTmp));
										ShowMessage(strTmp);
									}
									else
									{
										//在螺栓螺母表里找到附件或螺栓螺母
//										rsTmpZB.Edit();
										//不可以写入seq字段,因为计算函数GetphsBHandSizes已经确定了那些零件是可以编号，那些不可以编号。不需要编号的都赋给空。
										//rsTmpZB.Fields(_T("SEQ")) = i
										if( NewPtypeID[i] == _T("F10") || NewPtypeID[i] == _T("F9") )
										{
											//双头螺栓或者单头螺栓
											rsX->get_Collect((_variant_t)_T("BH"), &vTmp);
											EDIBgbl::dbPRJ->Execute((_bstr_t)(CString(_T("UPDATE tmpCSLen SET BH=\'")) + vtos(vTmp)+_T("\'")), NULL, adCmdText);
											rsTmpZB->put_Collect((_variant_t)_T("BH"),vTmp);
											EDIBgbl::dbPRJ->Execute((_bstr_t)(CString( _T("UPDATE tmpCSLen SET L1=")) + ltos(LenBolts)), NULL, adCmdText);
											SQLx = _T("SELECT ") + vtos(modPHScal::sFindFLD(_T("ID"), _T("BHFormat"), NewPtypeID[i])) + _T(" AS sBH FROM tmpCSLen");
//											rsTmp.m_pDatabase=&EDIBgbl::dbPRJ;
//											rsTmp.Open(dbOpenSnapshot,SQLx);
											rsTmp->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
												adOpenDynamic, adLockReadOnly, adCmdText); 
											if( rsTmp->adoEOF && rsTmp->BOF ){
												
											}
											else
											{
												rsTmp->get_Collect((_variant_t)_T("sBH"), &vTmp);
												rsTmpZB->put_Collect((_variant_t)_T("CLgg"),vTmp);
												//												rsTmpZB->put_Collect((_variant_t)_T("BH"),vTmp);
												//双头螺栓单重按1000mm计算保存在原始表的质量字段
												rsX->get_Collect((_variant_t)_T("Weight"), &vTmp1);
												rsTmpZB->get_Collect((_variant_t)_T("CLnum"), &vTmp2);
												rsTmpZB->put_Collect((_variant_t)_T("CLdz"),COleVariant(vtof(vTmp1) / 1000 * LenBolts));
												rsTmpZB->put_Collect((_variant_t)_T("CLzz"),COleVariant(vtof(vTmp1) / 1000 * LenBolts * vtof(vTmp2)));
												if(NewPtypeID[i] == _T("F4"))
												{
													//一般可能使用45钢
													rsX->get_Collect((_variant_t)_T("Material"), &vTmp1);
													rsTmpZB->put_Collect((_variant_t)_T("CLcl"),vTmp1);
												}
											}
											rsTmp->Close();
										}
										else
										{
											//六角头（标准）螺栓，按直径、长度排序，取最小的一个
											//螺母垫片
											rsX->get_Collect((_variant_t)_T("BH"), &vTmp);
											rsTmpZB->put_Collect((_variant_t)_T("CLgg"),vTmp);
											rsTmpZB->put_Collect((_variant_t)_T("BH"),vTmp);
											rsX->get_Collect((_variant_t)_T("Weight"), &vTmp1);
											rsTmpZB->get_Collect((_variant_t)_T("CLnum"), &vTmp2);
											rsTmpZB->put_Collect((_variant_t)_T("CLdz"),vTmp1);
											rsTmpZB->put_Collect((_variant_t)_T("CLzz"),COleVariant(vtof(vTmp1)* vtof(vTmp2)));
											if(NewPtypeID[i] == _T("F4"))
											{
												//一般可能使用45钢
												rsX->get_Collect((_variant_t)_T("Material"), &vTmp1);
												rsTmpZB->put_Collect((_variant_t)_T("CLcl"),vTmp1);
											}
										}
										rsTmpZB->Update();
									}
									rsX->Close();
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
//									rsTmp.m_pDatabase=&modPHScal::dbZDJcrude;
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
//										rsTmp.Open(dbOpenSnapshot,SQLx);
										rsTmp->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
											adOpenDynamic, adLockReadOnly, adCmdText); 
										if( rsTmp->adoEOF && rsTmp->BOF )
										{
											//没找到螺栓直径，可能数据表数据不全。
											rsTmp->Close();
											if( !modPHScal::gbAutoApplyCSPRLugDia )
											{
												//自动调整恒力弹簧拉杆直径
												SQLx = _T("SELECT * FROM [") + modPHScal::tbnLugDiaOfCSPR + _T("] WHERE minDH<=") + ltos(modPHScal::sSprInfo[miSEQofCSPR].DH) + _T(" AND maxDH>=") + ltos(modPHScal::sSprInfo[miSEQofCSPR].DH) + _T(" AND trim(CustomID)=\'") + PtypeCustomID[lngCurrentPartNo] + _T("\' ");
//												rsTmp.Open(dbOpenSnapshot,SQLx);
												rsTmp->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
													adOpenDynamic, adLockReadOnly, adCmdText); 
												if( rsTmp->adoEOF && rsTmp->BOF )
												{
													//没找到螺栓直径，可能数据表数据不全。
													strTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb), modPHScal::dbZDJcrude->DefaultDatabase,modPHScal:: tbnBoltsSurplusLength, SQLx); 
													throw strTmp;
												}
											}
											else
											{
												strTmp.Format(GetResStr(IDS_NotClearCheckAutoApplyCSPRLugDia), modPHScal::dbZDJcrude->DefaultDatabase,modPHScal:: tbnBoltsSurplusLength, SQLx);
												throw strTmp;
											}
										}
										//获得螺栓孔径FiK
										rsTmp->get_Collect((_variant_t)_T("FiK"), &vTmp);
										iBoltsNutsDia[i]=vtoi(vTmp);
										//查找螺栓直径系列表tbnCSPRDiameterSerial,获得准确直径
										SQLx = _T("SELECT * FROM [") + modPHScal::tbnCSPRDiameterSerial + _T("] WHERE Diameter<") + ltos(iBoltsNutsDia[i]) + _T(" ORDER BY Diameter DESC");
//										rs.Open(dbOpenSnapshot,SQLx);
										rs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
											adOpenDynamic, adLockReadOnly, adCmdText); 
										if( rs->adoEOF && rs->BOF ){
											//没有找到匹配直径系列值
											strTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb), modPHScal::dbZDJcrude->DefaultDatabase,modPHScal:: tbnBoltsSurplusLength, SQLx); 
											throw strTmp;
										}
										rs->get_Collect((_variant_t)_T("Diameter"), &vTmp);iBoltsNutsDia[i]=vtoi(vTmp);
										rsTmp->get_Collect((_variant_t)_T("S"), &vTmp1);rsTmp->get_Collect((_variant_t)_T("I"), &vTmp2);
										LenBolts=vtof(vTmp1)+2*vtof(vTmp2);
										//按照华东院支吊架手册的螺栓长度裕量计算准则计算螺栓长度。
										SQLx = _T("SELECT * FROM [") + modPHScal::tbnBoltsSurplusLength + _T("] WHERE Diameter=") + ltos(iBoltsNutsDia[i]) + _T(" ORDER BY Diameter");
										rsTmp->Close();
//										rsTmp.Open(dbOpenSnapshot,SQLx);
										rsTmp->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
											adOpenDynamic, adLockReadOnly, adCmdText); 
										if( rsTmp->adoEOF && rsTmp->BOF ){
											//没找到螺栓直径，可能数据表数据不全。
											strTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb), modPHScal::dbZDJcrude->DefaultDatabase,modPHScal:: tbnBoltsSurplusLength, SQLx); 
											throw strTmp;
										}
										if( AttachmentGroupHavingF4[lngCurrentPartNo] ){
											//附件组含垫片(F4)
											rsTmp->get_Collect((_variant_t)_T("SurplusLengthWithF1F2F4"), &vTmp);
											if( NewPtypeID[i] == _T("F10") ){    //双头螺栓
												LenBolts += 2 * vtof(vTmp);
											}else if( NewPtypeID[i] == _T("F9") || NewPtypeID[i] == _T("F14") ){     //单头螺栓/六角头螺栓
												LenBolts +=vtof(vTmp);
											}
										}
										else
										{
											rsTmp->get_Collect((_variant_t)_T("SurplusLengthWithF1F2"), &vTmp);
											if( NewPtypeID[i] == _T("F10") ){    //双头螺栓
												LenBolts += 2 * vtof(vTmp);
											}else if( NewPtypeID[i] == _T("F9") || NewPtypeID[i] == _T("F14") ){     //单头螺栓/六角头螺栓
												LenBolts +=vtof(vTmp);
											}
										}
										rs->Close();
										rsTmp->Close();
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
//										rsTmp.Open(dbOpenSnapshot,SQLx);
										rsTmp->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
											adOpenDynamic, adLockReadOnly, adCmdText); 
										if( rsTmp->adoEOF && rsTmp->BOF ){
											rsTmp->Close();
											//没找到螺栓直径，可能数据表数据不全。
											if( !modPHScal::gbAutoApplyCSPRLugDia )
											{
												//自动调整恒力弹簧拉杆直径
												SQLx = _T("SELECT * FROM [") + modPHScal::tbnLugDiaOfCSPR + _T("] WHERE minDH<=") + ltos(modPHScal::sSprInfo[miSEQofCSPR].DH) + _T(" AND maxDH>=") + ltos(modPHScal::sSprInfo[miSEQofCSPR].DH) + _T(" AND trim(CustomID)=\'") + PtypeCustomID[lngCurrentPartNo] + _T("\' ");
//												rsTmp.Open(dbOpenSnapshot,SQLx);
												rsTmp->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
													adOpenDynamic, adLockReadOnly, adCmdText); 
												if( rsTmp->adoEOF && rsTmp->BOF ){
													//没找到螺栓直径，可能数据表数据不全。
													strTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb), modPHScal::dbZDJcrude->DefaultDatabase,modPHScal:: tbnBoltsSurplusLength, SQLx); 
													throw strTmp;
												}
											}else{
												strTmp.Format(GetResStr(IDS_NotClearCheckAutoApplyCSPRLugDia), modPHScal::dbZDJcrude->DefaultDatabase,modPHScal:: tbnBoltsSurplusLength, SQLx);
												throw strTmp;
											}
										}
										//获得螺栓孔径FiK
										rsTmp->get_Collect((_variant_t)_T("FiK"), &vTmp);
										iBoltsNutsDia[i]=vtoi(vTmp);
										//查找螺栓直径系列表tbnCSPRDiameterSerial,获得准确直径
										SQLx = _T("SELECT * FROM [") + modPHScal::tbnCSPRDiameterSerial + _T("] WHERE Diameter<") + ltos(iBoltsNutsDia[i]) + _T(" ORDER BY Diameter DESC");
//										rs.Open(dbOpenSnapshot,SQLx);
										rs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
											adOpenDynamic, adLockReadOnly, adCmdText); 
										if( rs->adoEOF && rs->BOF ){
											//没有找到匹配直径系列值
											strTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb), modPHScal::dbZDJcrude->DefaultDatabase,modPHScal:: tbnBoltsSurplusLength, SQLx); 
											throw strTmp;
										}
										rs->get_Collect((_variant_t)_T("Diameter"), &vTmp);
										iBoltsNutsDia[i]=vtoi(vTmp);
										LenBolts = SACSHeight + tmpT;
										//按照华东院支吊架手册的螺栓长度裕量计算准则计算螺栓长度。
										SQLx = _T("SELECT * FROM [") + modPHScal::tbnBoltsSurplusLength + _T("] WHERE Diameter=") + ltos(iBoltsNutsDia[i]);
										rsTmp->Close();
//										rsTmp.Open(dbOpenSnapshot,SQLx);
										rsTmp->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
											adOpenDynamic, adLockReadOnly, adCmdText); 
										if( rsTmp->adoEOF && rsTmp->BOF ){
											//没找到螺栓直径，可能数据表数据不全。
											strTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb), modPHScal::dbZDJcrude->DefaultDatabase,modPHScal:: tbnBoltsSurplusLength, SQLx); 
											throw strTmp;
										}
										if( AttachmentGroupHavingF4[lngCurrentPartNo] ){
											//附件组含垫片(F4)
											rsTmp->get_Collect((_variant_t)_T("SurplusLengthWithF1F2F4"), &vTmp);
											if( NewPtypeID[i] == _T("F10") ){    //双头螺栓
												LenBolts +=2*vtof(vTmp);
											}else if( NewPtypeID[i] == _T("F9") || NewPtypeID[i] == _T("F14") ){     //单头螺栓/六角头螺栓
												LenBolts +=vtof(vTmp);
											}
										}else{
											rsTmp->get_Collect((_variant_t)_T("SurplusLengthWithF1F2"), &vTmp);
											if( NewPtypeID[i] == _T("F10") ){    //双头螺栓
												LenBolts +=2*vtof(vTmp);
											}else if( NewPtypeID[i] == _T("F9") || NewPtypeID[i] == _T("F14") ){     //单头螺栓/六角头螺栓
												LenBolts +=vtof(vTmp);
											}
										}
										rs->Close();
										rsTmp->Close();
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
//										rsTmp.Open(dbOpenSnapshot,SQLx);
										rsTmp->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
											adOpenDynamic, adLockReadOnly, adCmdText); 
										if( rsTmp->adoEOF && rsTmp->BOF ){
											//没找到螺栓直径，可能数据表数据不全。
											strTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb), modPHScal::dbZDJcrude->DefaultDatabase,modPHScal:: tbnBoltsSurplusLength, SQLx); 
											throw strTmp;
										}
										rsTmp->get_Collect((_variant_t)_T("FiJ"), &vTmp);
										iBoltsNutsDia[i]=vtoi(vTmp);
										LenBolts = SACSHeight + 15;
										rsTmp->Close();
										//按照华东院支吊架手册的螺栓长度裕量计算准则计算螺栓长度。
										SQLx = _T("SELECT * FROM [") + modPHScal::tbnBoltsSurplusLength + _T("] WHERE Diameter=") + ltos(iBoltsNutsDia[i]);
//										rsTmp.Open(dbOpenSnapshot,SQLx);
										rsTmp->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
											adOpenDynamic, adLockReadOnly, adCmdText); 
										if( rsTmp->adoEOF && rsTmp->BOF ){
											//没找到螺栓直径，可能数据表数据不全。
											strTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb), modPHScal::dbZDJcrude->DefaultDatabase,modPHScal:: tbnBoltsSurplusLength, SQLx); 
											throw strTmp;
										}
										if( AttachmentGroupHavingF4[lngCurrentPartNo] ){
											//附件组含垫片(F4)
											rsTmp->get_Collect((_variant_t)_T("SurplusLengthWithF1F2F4"), &vTmp);
											if( NewPtypeID[i] == _T("F10") ){    //双头螺栓
												LenBolts +=2*vtof(vTmp);
											}else if( NewPtypeID[i] == _T("F9") || NewPtypeID[i] == _T("F14") ){     //单头螺栓/六角头螺栓
												LenBolts +=vtof(vTmp);
											}
										}else{
											rsTmp->get_Collect((_variant_t)_T("SurplusLengthWithF1F2"), &vTmp);
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
										rsTmpZB->get_Collect((_variant_t)_T("ID"), &vTmp);
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
//										rsX.Open(dbOpenSnapshot,SQLx);
										rsX->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
											adOpenDynamic, adLockReadOnly, adCmdText); 
										if( rsX->adoEOF && rsX->BOF ){
											//如果，在螺栓螺母表里没找到附件或螺栓螺母
											//Err.Number = iUE_NoFoundAttachInZDJCrude
											//Err.Description = ResolveResString(iUE_NoFoundAttachInZDJCrude, _T("|1"), dbZDJcrude.Name, _T("|2"), SQLx, _T("|3"), rsTmpZB.Fields(_T("CustomID")))
											//Err.Raise iUE_NoFoundAttachInZDJCrude
											rsTmpZB->get_Collect((_variant_t)_T("CustomID"), &vTmp);
											strTmp.Format(GetResStr(IDS_NoFoundAttachInZDJCrude),modPHScal::dbZDJcrude->DefaultDatabase,SQLx,vtos(vTmp));
											ShowMessage(strTmp);
										}
										else
										{
											//在螺栓螺母表里找到附件或螺栓螺母
//											rsTmpZB.Edit();
											//不可以写入seq字段,因为计算函数GetphsBHandSizes已经确定了那些零件是可以编号，那些不可以编号。不需要编号的都赋给空。
											//rsTmpZB.Fields(_T("SEQ")) = i
											if( NewPtypeID[i] == _T("F10") || NewPtypeID[i] == _T("F9") )
											{
												//双头螺栓
												rsX->get_Collect((_variant_t)_T("BH"), &vTmp);
												EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET BH=\'") + vtos(vTmp) + _T("\'")), NULL, adCmdText);
												rsTmpZB->put_Collect((_variant_t)_T("BH"),vTmp);
												EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET L1=") + ltos(LenBolts)), NULL, adCmdText);
												SQLx = _T("SELECT ") + vtos(modPHScal::sFindFLD(_T("ID"), _T("BHFormat"), NewPtypeID[i])) + _T(" AS sBH FROM tmpCSLen");
//												rsTmp.m_pDatabase=&EDIBgbl::dbPRJ;
//												rsTmp.Open(dbOpenSnapshot,SQLx);
												rsTmp->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
													adOpenDynamic, adLockReadOnly, adCmdText); 
												if( rsTmp->adoEOF && rsTmp->BOF )
												{
													strTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb),EDIBgbl::dbPRJ->DefaultDatabase,_T("tmpCSLen"),SQLx);
													throw strTmp;
												}
												rsTmp->get_Collect((_variant_t)_T("sBH"), &vTmp);
												rsTmpZB->put_Collect((_variant_t)_T("CLgg"),vTmp);
												rsTmpZB->put_Collect((_variant_t)_T("BH"),vTmp);
												//双头螺栓单重按1000mm计算保存在原始表的质量字段
												rsX->get_Collect((_variant_t)_T("Weight"), &vTmp1);
												rsTmpZB->get_Collect((_variant_t)_T("CLnum"), &vTmp2);
												rsTmpZB->put_Collect((_variant_t)_T("CLdz"),COleVariant(vtof(vTmp1) / 1000 * LenBolts));
												rsTmpZB->put_Collect((_variant_t)_T("CLzz"),COleVariant(vtof(vTmp1)/ 1000 * LenBolts * vtof(vTmp2)));
												rsTmp->Close();
												if(NewPtypeID[i] == _T("F4"))
												{
													//一般可能使用45钢
													rsX->get_Collect((_variant_t)_T("Material"), &vTmp1);
													rsTmpZB->put_Collect((_variant_t)_T("CLcl"),vTmp1);
												}
											}
											else
											{
												//六角头（标准）螺栓，按直径、长度排序，取最小的一个
												//螺母垫片
												rsX->get_Collect((_variant_t)_T("BH"), &vTmp);
												rsTmpZB->put_Collect((_variant_t)_T("CLgg"),vTmp);
												rsTmpZB->put_Collect((_variant_t)_T("BH"),vTmp);
												rsX->get_Collect((_variant_t)_T("Weight"), &vTmp1);
												rsTmpZB->get_Collect((_variant_t)_T("CLnum"), &vTmp2);
												rsTmpZB->put_Collect((_variant_t)_T("CLdz"),vTmp1);
												rsTmpZB->put_Collect((_variant_t)_T("CLzz"),COleVariant(vtof(vTmp1)*vtof(vTmp2)));
												if(NewPtypeID[i] == _T("F4"))
												{
													//一般可能使用45钢
													rsX->get_Collect((_variant_t)_T("Material"), &vTmp1);
													rsTmpZB->put_Collect((_variant_t)_T("CLcl"),vTmp1);
												}
											}
											rsTmpZB->Update();
										}
										rsX->Close();
									}
								}
							}
						}
					}
					
					//已经获得螺栓螺母直径
					//在此开始查找螺栓螺母表
					//写附件(螺栓、螺母)数据
					rsTmpZB->get_Collect((_variant_t)_T("ClassID"), &vTmp);
					if( ! bCalBoltsDiaOfCSPR && (vtoi(vTmp) == iAttached || vtoi(vTmp) == iBolts || vtoi(vTmp) == iNuts) )
					{
						//所有恒吊螺栓螺母的计算均不会进入这段程序，因为此时bCalBoltsDiaOfCSPR=true
						//首先，在螺栓螺母表里查找附件或螺栓螺母
						tbn1 = modPHScal::tbnBoltsNuts;
						rsTmpZB->get_Collect((_variant_t)_T("CustomID"), &vTmp);
						SQLx = _T("SELECT * FROM [") + tbn1 + _T("] WHERE Trim(CustomID)=\'") + vtos(vTmp) + _T("\' AND size2=") + ltos(iBoltsNutsDia[i]);
						
//						rsX.Open(dbOpenSnapshot,SQLx);
						rsX->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
							adOpenDynamic, adLockReadOnly, adCmdText); 
						if( rsX->adoEOF && rsX->BOF ){
							//如果，在螺栓螺母表里没找到附件或螺栓螺母
							//那么，在根部附件表里查找附件或螺栓螺母
							rsX->Close();
							tbn1 = modPHScal::tbnAttachment;
							SQLx = _T("SELECT * FROM [") + tbn1 + _T("] WHERE Trim(CustomID)=\'") + vtos(vTmp) + _T("\' AND size2=") + ltos(iBoltsNutsDia[i]) + _T(" ORDER BY size2");
//							rsX.Open(dbOpenSnapshot,SQLx);
							rsX->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
								adOpenDynamic, adLockReadOnly, adCmdText); 
							if( rsX->adoEOF && rsX->BOF ){
								//在根部附件表里没找到附件或螺栓螺母
								//Err.Number = iUE_NoFoundAttachInZDJCrude
								//Err.Description = ResolveResString(iUE_NoFoundAttachInZDJCrude, _T("|1"), dbZDJcrude.Name, _T("|2"), SQLx, _T("|3"), rsTmpZB.Fields(_T("CustomID")))
								//Err.Raise iUE_NoFoundAttachInZDJCrude
								rsTmpZB->get_Collect((_variant_t)_T("CustomID"), &vTmp);
								strTmp.Format(GetResStr(IDS_NoFoundAttachInZDJCrude),modPHScal::dbZDJcrude->DefaultDatabase,SQLx,vtos(vTmp));
								ShowMessage(strTmp);
							}
							else{
								//在根部附件表里找到附件或螺栓螺母
//								rsTmpZB.Edit();
								//不可以写入seq字段,因为计算函数GetphsBHandSizes已经确定了那些零件是可以编号，那些不可以编号。不需要编号的都赋给空。
								//rsTmpZB.Fields(_T("SEQ")) = i
								rsX->get_Collect((_variant_t)_T("BH"), &vTmp);
								rsTmpZB->put_Collect((_variant_t)_T("CLgg"),vTmp);
								rsTmpZB->put_Collect((_variant_t)_T("BH"),vTmp);
								rsX->get_Collect((_variant_t)_T("Weight"), &vTmp1);
								rsTmpZB->get_Collect((_variant_t)_T("CLnum"), &vTmp2);
								rsTmpZB->put_Collect((_variant_t)_T("CLdz"),vTmp1);
								rsTmpZB->put_Collect((_variant_t)_T("CLzz"),COleVariant(vtof(vTmp1)*vtof(vTmp2)));
								if(NewPtypeID[i] == _T("F4"))
								{
									//一般可能使用45钢
									rsX->get_Collect((_variant_t)_T("Material"), &vTmp1);
									rsTmpZB->put_Collect((_variant_t)_T("CLcl"),vTmp1);
								}
								rsTmpZB->Update();
							}
						}
						else{
							//在螺栓螺母表里找到附件或螺栓螺母
//							rsTmpZB.Edit();
							//不可以写入seq字段,因为计算函数GetphsBHandSizes已经确定了那些零件是可以编号，那些不可以编号。不需要编号的都赋给空。
							//rsTmpZB.Fields(_T("SEQ")) = i
							rsX->get_Collect((_variant_t)_T("BH"), &vTmp);
							rsTmpZB->put_Collect((_variant_t)_T("CLgg"),vTmp);
							rsTmpZB->put_Collect((_variant_t)_T("BH"),vTmp);
							rsX->get_Collect((_variant_t)_T("Weight"), &vTmp1);
							rsTmpZB->get_Collect((_variant_t)_T("CLnum"), &vTmp2);
							rsTmpZB->put_Collect((_variant_t)_T("CLdz"),vTmp1);
							rsTmpZB->put_Collect((_variant_t)_T("CLzz"),COleVariant(vtof(vTmp1)*vtof(vTmp2)));
							if(NewPtypeID[i] == _T("F4"))
							{
								//一般可能使用45钢
								rsX->get_Collect((_variant_t)_T("Material"), &vTmp1);
								rsTmpZB->put_Collect((_variant_t)_T("CLcl"),vTmp1);
							}
							rsTmpZB->Update();
						}
						rsX->Close();
					}
				}
				else{
					//不是拉杆，不是螺栓螺母或附件
					lngCurrentPartNo ++;
					lngAttachedPartNo = -1;
					//恒吊计数
					if( NewPtypeClassID[i] == iCSPR ){
						miSEQofCSPR ++;
						rsTmpZB->get_Collect((_variant_t)_T("gdw1"), &vTmp);
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
							rsTmpZB->get_Collect((_variant_t)_T("CLgg"), &vTmp);
							sBH =vtos(vTmp);
							//注意此处是以M作为T5松紧螺母直径标志的，在适应其它标准时可能遇到问题!
							k = sBH.Find(_T("M"));
							if( k !=-1 ){
								sBH = sBH.Left(k+1) + ltos(iBoltsNutsDia[i]);
//								rsTmpZB.Edit();
								rsTmpZB->put_Collect((_variant_t)_T("CLgg"),STR_VAR(sBH));
								//								rsTmpZB->put_Collect((_variant_t)_T("BH"),STR_VAR(sBH));
								rsTmpZB->Update();
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
//								rsTmpZB.Edit();
								rsTmpZB->put_Collect((_variant_t)_T("sizeH"),COleVariant(SACSHeight));
								rsTmpZB->Update();
							}
						}
					}
					//结束当前零件不是拉杆，不是螺栓螺母或附件
				}
			}
			rsTmpZB->MoveNext();
		}
		
		//更新根部整体重量,最后一个recno不为空的记录就是根部记录,
		//但是,要排除根部是混凝土梁柱的情况
		
		SQLx.Format("SELECT * FROM [ZB] WHERE [VolumeID]=%d AND [ZDJH]=%d AND [nth]=%d AND [Index]=%d ORDER BY recno",
			EDIBgbl::SelVlmID,modPHScal::zdjh,nth,iSA);
		if( rsTmpZB->State == adOpenStatic )
			rsTmpZB->Close();
//		rsTmpZB.Open(dbOpenDynaset,SQLx);
		rsTmpZB->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 

		if(rsTmpZB->adoEOF && rsTmpZB->BOF)
		{
			return ;
		}
		else 
		{
//			rsTmpZB.Edit();
			if( rsTmpZB->BOF )
			{
				strTmp.Format(GetResStr(IDS_BOForEOFofmvarRsTZB), _T("BOF"));
				throw strTmp;
			}
			if( rsTmpZB->adoEOF )
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
		rsTmpZB->get_Collect((_variant_t)_T("CustomID"), &vTmp);
		
		if( !rsID->Find((_bstr_t)(_T("trim(CustomID)=\'") + vtos(vTmp) + _T("\'")), 0, adSearchForward, vTmp))
		{
			strTmp.Format(GetResStr(IDS_NoRecordFLDEqvThisValueInPictureClipData),_T("CustomID"),vtos(vTmp));
			ShowMessage(strTmp);
		}
		else
		{
			rsID->get_Collect((_variant_t)_T("ClassID"), &vTmp);
			if( vtoi(vTmp)== iGCement )
			{
				COleVariant nul;
				nul.ChangeType(VT_NULL);
				rsTmpZB->put_Collect((_variant_t)_T("SEQ"),nul);
			}
			else
			{
				//型钢根部,赋给一非空的任意值
				//不可，GetphsBHandSizes函数已经确定那些根部可以编号
				//rsTmpZB.Fields(_T("SEQ")) = i
			}
		}
		rsTmpZB->put_Collect((_variant_t)_T("CLdz"),COleVariant(mvarSATotalWeight));
		rsTmpZB->get_Collect((_variant_t)_T("CLnum"), &vTmp);
		rsTmpZB->put_Collect((_variant_t)_T("CLzz"),COleVariant(mvarSATotalWeight * vtof(vTmp)));
		rsTmpZB->Update();
		
		//最后一次循环，在螺栓螺母直径都确定时
	}
	catch(CString e)
	{
		ShowMessage(e);
	}
	catch(CException *e)
	{
		e->Delete();
	}

	if(rsTmpZB->State == adOpenStatic)
		rsTmpZB->Close();

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
		_RecordsetPtr rs, rs1;
		rs.CreateInstance(__uuidof(Recordset));
		rs1.CreateInstance(__uuidof(Recordset));
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
//				rs.m_pDatabase=&EDIBgbl::dbPRJ;
				for (i = 0;i<= IC - 2;i++)
				{
					//假设每一个都不匹配
					FlgMatch = false;
					//从第一个开始查找匹配
					SQLx = _T("SELECT * FROM connect WHERE trim(cntb)=\'") + Ptype[i] + _T("\' AND  trim(cnte)=\'") + Ptype[i + 1] + _T("\'");
//					rs.Open(dbOpenSnapshot,SQLx);
					rs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
						adOpenDynamic, adLockReadOnly, adCmdText); 
					if( rs->adoEOF && rs->BOF ){
						FlgMatch = false;
						break;
					}
					else if( ! rs->adoEOF )
					{
						rs->MoveLast();
						rs->MoveFirst();
						for( j = 0 ;j<rs->GetRecordCount();j++)
						{
							//3个或3个以上的零件,匹配条件更严格
							rs->get_Collect((_variant_t)_T("cntn"), &vTmp);
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
							rs->MoveNext();
						}
						if( j >= rs->GetRecordCount() ){
							FlgMatch = false;
							break;
						}
					}
					
					//如果真地没有匹配,则退出
					if( ! FlgMatch ) break;
					rs->Close();
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
	catch(CException *e)
	{
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
	_RecordsetPtr rs, rs1;
	rs.CreateInstance(__uuidof(Recordset));
	rs1.CreateInstance(__uuidof(Recordset));

	CString sTmp;
	try{
		EDIBgbl::SQLx = _T("SELECT * FROM phsStructureName");
		//使用rs1以免和GetphsAssembleName中的模块级对象rs冲突
//		rs1.m_pDatabase=&EDIBgbl::dbPHScode; //20071101 "dbSORT" 改为 "dbPHScode"
//		rs1.Open(dbOpenDynaset,EDIBgbl::SQLx);
		rs1->Open((_bstr_t)EDIBgbl::SQLx,_variant_t((IDispatch*)EDIBgbl::dbPHScode,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		EDIBgbl::SQLx = _T("SELECT * FROM phsStructureREF");
//		rs.m_pDatabase=&EDIBgbl::dbPHScode; //20071101 "dbSORT" 改为 "dbPHScode"
//		rs.Open(dbOpenSnapshot,EDIBgbl::SQLx);
		rs->Open((_bstr_t)EDIBgbl::SQLx,_variant_t((IDispatch*)EDIBgbl::dbPHScode,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		
		COleVariant v;
		if(rs1->BOF&& rs1->adoEOF)
		{
			sTmp=GetResStr(IDS_NoAnyRecordInphsStructureName)+GetResStr(IDS_NoAnyRecordInphsStructureName);
			ShowMessage(sTmp);
			goto errH;
		}
		if( !rs1->adoEOF)
		{
			rs1->MoveLast();
			rs1->MoveFirst();
			while(!rs1->adoEOF)
				//for (int i = 0 ;i<rs1->GetRecordCount();i++)
			{
				rs1->get_Collect((_variant_t)_T("SampleID"),v);
				sTmp.Format(_T("%d"),vtoi(v));
				sTmp=CString(_T("SampleID="))+sTmp+_T("");
				//在phsStructureREF中没有发现phsStructureName中
				//存在的SampleID的数据,则删除phsStructureName中的这个SampleID记录
				_variant_t vTmp;
				if(! rs->Find((_bstr_t)sTmp, 0, adSearchForward, vTmp))
				{
					rs1->Delete(adAffectCurrent);
				}
				rs1->MoveNext();
			}
		}
	}
	catch(CException *e)
	{
		e->Delete();
	}
errH:
	AfxGetApp()->EndWaitCursor();
	if(rs->State == adOpenStatic)
		rs->Close();
	if(rs1->State == adOpenStatic)
		rs1->Close();
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
		_RecordsetPtr rs1;
		rs1.CreateInstance(__uuidof(Recordset));

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
//		rs1.m_pDatabase=&EDIBgbl::dbPHScode;//20071101 "dbSORT" 改为 "dbPHScode"
//		rs1.Open(dbOpenDynaset,SQLx);
		rs1->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPHScode,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		CString DupRec;
		if(!rs1->adoEOF && !rs1->BOF)
		{
			m=1;
			rs1->get_Collect((_variant_t)_T("SampleID"), &vTmp);
			DupRec=vtos(vTmp);
		}
		else
			m=0;
		rs1->Close();
		
		//若m=0,则不是重复记录
		//rs2.m_pDatabase=&EDIBgbl::dbSORT;
		if( m == 0 )
		{
			//获取尚未使用的SampleID号,其值作为最新的SampleID
			SQLx = _T("SELECT * FROM phsStructureREF WHERE Seq=0 ORDER BY SampleID");
//			rs1.Open(dbOpenDynaset,SQLx);
			rs1->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPHScode,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 
			if( rs1->BOF && rs1->adoEOF )
			{
				//MsgBox ResolveResString(iUE_NoAnyRecordInphsStructureName)
				//Exit Function
				IC = 0;
			}
			else if( ! rs1->adoEOF )
			{
				rs1->MoveLast();
				rs1->MoveFirst();
				for (k = 1 ;k<= rs1->GetRecordCount();k++)
				{
					rs1->get_Collect((_variant_t)_T("SampleID"), &vTmp);
					IC =vtoi(vTmp);
					//如果记录次序与SampleID不等，说明不连续，
					//则空缺的SampleID号可用
					if( k != IC ) 
						break;
					rs1->MoveNext();
				}
				//如果k > rs2.RecordCount,说明SampleID连续，
				//最后一个SampleID号+1可用
				if( k > rs1->GetRecordCount() )
				{
					IC = k - 1;
				}
				else
				{
					//可用的SampleID-1=IC
					IC = IC - 2;
				}
			}
			rs1->Close();
			modPHScal::iSelSampleID = IC + 1;
			sTmp.Format(_T("%04d"),modPHScal::iSelSampleID);
			SQLx = _T("INSERT INTO phsStructureName (FREQUENCE,SampleID,BmpName,bAllowUse) VALUES (1,") + ltos(modPHScal::iSelSampleID) + _T(",\'") + sTmp + _T("\',-1)");
			//必须加入共享库，以便集中管理
			EDIBgbl::dbPHScode->Execute((_bstr_t)(SQLx), NULL, adCmdText);//20071101 "dbSORT" 改为 "dbPHScode"
			//同时加入本地库中，以便检索更快
			EDIBgbl::dbPRJ->Execute((_bstr_t)(SQLx), NULL, adCmdText);
			for( k = 0 ;k<SourceObj->GetCount();k++)
			{
				SourceObj->GetText(k,sTmp);
				SQLx = _T("INSERT INTO phsStructureREF (SampleID,SEQ,ID,CustomID,FREQUENCE) VALUES (")+
					ltos(modPHScal::iSelSampleID)+_T(",")+ltos(k)+_T(", \'") +modPHScal::sFindID(Trim(sTmp)) +_T("\' , \'") +Trim(sTmp) + _T("\' , 1)");
				//必须加入共享库，以便集中管理
				EDIBgbl::dbPHScode->Execute((_bstr_t)(SQLx), NULL, adCmdText);//20071101 "dbSORT" 改为 "dbPHScode"
				//同时加入本地库中，以便检索更快
				EDIBgbl::dbPRJ->Execute((_bstr_t)SQLx, NULL, adCmdText);
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
				EDIBgbl::dbPHScode->Execute((_bstr_t)SQLx, NULL, adCmdText);//20071101 "dbSORT" 改为 "dbPHScode"
				//同时加入本地库中，以便检索更快
				EDIBgbl::dbPRJ->Execute((_bstr_t)SQLx, NULL, adCmdText);
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

			sTmp.Format(GetResStr(IDS_ExistsThisSampleIDinSampleREF), EDIBgbl::dbSORT->DefaultDatabase, DupRec);  //原

			SQLx.Format(_T("SELECT * FROM phsStructureName WHERE SampleID = %d"),_ttoi(DupRec));  
//			rs1.Open(dbOpenDynaset,SQLx);			
			rs1->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPHScode,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 
			rs1->get_Collect((_variant_t)_T("DefaultFavoriteTemplate"), &vTmp);
			bDefFavTem = vtob(vTmp);
			rs1->get_Collect((_variant_t)_T("Favorite"), &vTmp);
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
	return ret;
}
#pragma warning( disable : 4129 )  //Added by Shuli Luo
void Cphs::CheckAllCustomIDandIDinPhsStructureREF()
{
	//目的：检查所有的CustomID字段，将其转换为ID字段。设置ID字段是为了保持PhsStructureREF表中模板号的唯一性/通用性。
	//所有的查找表现均以ID为依据.
	
	
	//打开这个快照，查找速度比调用modPHScal::sFindID和sFindCustomID快。
	_RecordsetPtr rs, rsX;
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
	if(!rsID->State == adOpenStatic)
	{
	EDIBgbl::SQLx = _T("SELECT * FROM PictureClipData");
	rsID.m_pDatabase=&EDIBgbl::dbPRJ;
	rsID.Open(dbOpenDynaset,EDIBgbl::SQLx);
	brsIDStatus=TRUE;
	}
	
	  EDIBgbl::SQLx = _T("SELECT SampleID,seq,CustomID,ID  FROM phsStructureREF ORDER BY SampleID,seq");
	  rsX.m_pDatabase=&EDIBgbl::dbSORT;
	  rsX.Open( dbOpenDynaset ,EDIBgbl::SQLx);   
	  if( rsX->adoEOF && rsX->BOF ) 
	  {
	  sTmp=GetResStr(IDS_NoAnySEQeqZeroRecordInphsStructureREF);
	  ShowMessage(sTmp);
	  goto errH;
	  }
	  rsX->MoveLast();
	  int iC2 = rsX->GetRecordCount();
	  rsX->get_Collect((_variant_t)_T("SampleID"),v);
	  int iCSampleID =vtoi(v);
	  rsX->MoveFirst();
	  frmStatus.m_Label1=GetResStr(IDS_CheckAllCustomIDandIDinPhsStructureREF);
	  CString CustomID,ID;
	  for(int j = 0 ;j< iC2 ;j++)
	  {
	  AfxGetApp()->BeginWaitCursor();
	  //Screen.MousePointer = vbHourglass
	  //给窗体显示一点时间
	  rsX->get_Collect((_variant_t)_T("CustomID"),v);
	  CustomID=vtos(v);
	  if(rsID->Find((_bstr_t)CString(_T("Trim(CustomID)=\'")) + CustomID + _T("\'")))
	  {
	  rsID->get_Collect((_variant_t)_T("ID"),v);
	  ID=vtos(v);
	  if (ID== _T("") )
	  {
	  rsX->get_Collect((_variant_t)_T("ID"),v);
	  ID=vtos(v);
	  if(rsID->Find((_bstr_t)CString(_T("Trim(ID)=\'")) + ID + _T("\'")))
	  //如果为空，则显然不能从CustomID转换为ID,因而进行逆变换。
	  {
						rsID->get_Collect((_variant_t)_T("CustomID"),v);
						CustomID=vtos(v);
						if( CustomID != _T("") )
						//仍然为空，报告一个错误
						//msgbox
						{
						rsX.Edit();
						rsX->put_Collect((_variant_t)_T("CustomID"),v);
						rsX->Update();
						}
						}
						
						  //在ID-CustomID对照表中没有发现这种零部件
						  //msgbox
						  }
						  else
						  {
						  rsID->get_Collect((_variant_t)_T("ID"),v);
						  rsX.Edit();
						  rsX->put_Collect((_variant_t)_T("ID"),v);
						  rsX->Update();
						  }
						  }
						  else
						  {
						  //如果为空，则显然不能从CustomID转换为ID,因而进行逆变换。
						  rsX->get_Collect((_variant_t)_T("ID"),v);
						  ID=vtos(v);
						  if(rsID->Find((_bstr_t) CString(_T("Trim(ID)=\'")) + ID + _T("\'")))
						  {
						  //如果为空，则显然不能从CustomID转换为ID,因而进行逆变换。
						  rsID->get_Collect((_variant_t)_T("CustomID"),v);
						  CustomID=vtos(v);
						  if(CustomID!=_T(""))
						  {
						  rsX.Edit();
						  rsX->put_Collect((_variant_t)_T("CustomID"),v);
						  rsX->Update();
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
						  rsX->get_Collect((_variant_t)_T("SampleID"),v);
						  sf1.Format(_T("%d"),vtoi(v));
						  rsX->get_Collect((_variant_t)_T("seq"),v);
						  sf2.Format(_T("%d"),vtoi(v));
						  sTmp1.Format(_T("%d"),iCSampleID);
						  stmp2.Format(_T("%d"),iC2);
						  //共%s个模板%s个零件,第%s号模板第%s号零件CustomID=%s
						  frmStatus.m_Label2.Format(GetResStr(IDS_CheckNoXCustomIDandIDinPhsStructureREF),sTmp1,stmp2,sf1,sf2,CustomID);
						  frmStatus.UpdateStatus((float)(j+1)/(float)iC2,true);
						  frmStatus.UpdateData(false);
						  
							rsX->MoveNext();
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
//	rsX.m_pDatabase=&EDIBgbl::dbPRJ;
//	rsX.Open(dbOpenSnapshot,SQLx);
	rsX->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
		adOpenDynamic, adLockReadOnly, adCmdText); 
	if( rsX->adoEOF && rsX->BOF ) 
	{
		sTmp=GetResStr(IDS_NoAnySEQeqZeroRecordInphsStructureREF);
		ShowMessage(sTmp);
		goto errH;
	}
	else
	{
		frmStatus.m_Label1.Format(GetResStr(IDS_UpdateCustomIDinPhsStructureREF));
		
		rsX->MoveLast();
		int iC2=rsX->GetRecordCount();						
		for(int j=1;j<=iC2;j++)
		{
			rsX->get_Collect((_variant_t)_T("CustomID"),v);
			rsX->get_Collect((_variant_t)_T("ID"), &vTmp);
			SQLx = _T("UPDATE phsStructureREF SET CustomID=\'") + vtos(v)+ _T("\' WHERE ID=\'") + vtos(vTmp)+_T("\'");
			EDIBgbl::dbPRJ->Execute((_bstr_t)SQLx, NULL, adCmdText);
			EDIBgbl::dbPHScode->Execute((_bstr_t)SQLx, NULL, adCmdText);//20071018 "dbSORT" 改为 "dbPHScode"
			//已经替换
			frmStatus.m_Label2=_T("");
			frmStatus.UpdateStatus((float)j/(float)iC2,true);
			frmStatus.UpdateData(false);
			
			
			rsX->MovePrevious();
		}		
	}
}
catch(CException *e)
{
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
	_RecordsetPtr rs1, rs2, rs3, rs4;//rs1与rs2取自同一个表phsStructureREF
	rs1.CreateInstance(__uuidof(Recordset));
	rs2.CreateInstance(__uuidof(Recordset));
	rs3.CreateInstance(__uuidof(Recordset));
	rs4.CreateInstance(__uuidof(Recordset));
// 	rs1.m_pDatabase=&EDIBgbl::dbPRJ;
// 	rs2.m_pDatabase=&EDIBgbl::dbPRJ;
// 	rs3.m_pDatabase=&EDIBgbl::dbPRJ;
// 	rs4.m_pDatabase=&EDIBgbl::dbPHScode;//20071018 "dbSORT" 改为 "dbPHScode"
	//打开表备查
// 	 rsPA(&EDIBgbl::dbPRJ);
// 	 rsSA(&EDIBgbl::dbPRJ);
// 	 rsPR(&EDIBgbl::dbPRJ);
// 	 rsSP(&EDIBgbl::dbPRJ);
// 	 rsCS(&EDIBgbl::dbPRJ);
	_RecordsetPtr rsPA, rsSA, rsPR, rsSP, rsCS;//rs1与rs2取自同一个表phsStructureREF
	rsPA.CreateInstance(__uuidof(Recordset));
	rsSA.CreateInstance(__uuidof(Recordset));
	rsPR.CreateInstance(__uuidof(Recordset));
	rsSP.CreateInstance(__uuidof(Recordset));
	rsCS.CreateInstance(__uuidof(Recordset));

// 	rsPA.Open(dbOpenTable,_T("tmpCustomIDPA"));
// 	rsSA.Open(dbOpenTable,_T("tmpCustomIDSA"));
// 	rsPR.Open(dbOpenTable,_T("tmpCustomIDPART"));
// 	rsSP.Open(dbOpenTable,_T("tmpCustomIDSPR"));
// 	rsCS.Open(dbOpenTable,_T("tmpCustomIDCSPR"));
	rsPA->Open((_bstr_t)_T("tmpCustomIDPA"),_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
		adOpenDynamic, adLockReadOnly, adCmdTable); 

	rsSA->Open((_bstr_t)_T("tmpCustomIDSA"),_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
		adOpenDynamic, adLockReadOnly, adCmdTable); 
	
	rsPR->Open((_bstr_t)_T("tmpCustomIDPART"),_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
		adOpenDynamic, adLockReadOnly, adCmdTable); 
	
	rsSP->Open((_bstr_t)_T("tmpCustomIDSPR"),_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
		adOpenDynamic, adLockReadOnly, adCmdTable); 
	
	rsCS->Open((_bstr_t)_T("tmpCustomIDCSPR"),_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
		adOpenDynamic, adLockReadOnly, adCmdTable); 
	
// 	rsPA.SetCurrentIndex(_T("Index1"));
// 	rsSA.SetCurrentIndex(_T("Index1"));
// 	rsPR.SetCurrentIndex(_T("Index1"));
// 	rsSP.SetCurrentIndex(_T("Index1"));
// 	rsCS.SetCurrentIndex(_T("Index1"));

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
//	long t2;
	t1=::GetTickCount();		
	//2002.01.03测试时间31084ms<12413+55730ms,故综合在一起查询较快
	
	//预先设置当前规范可用的模板为全部不可用，只要有任意一个零件的bAllowUse=0，即为不可用
	strSQL=_T("UPDATE PhsStructureName SET bAllowUse=0");
	EDIBgbl::dbPHScode->Execute((_bstr_t)strSQL, NULL, adCmdText);//20071018 "dbSORT" 改为 "dbPHScode"



	if(rsID->State != adOpenStatic)
	{
		EDIBgbl::SQLx = _T("SELECT * FROM PictureClipData");
		//if(Cavphs->rsID==NULL)
		//Cavphs->rsID.CreateInstance(__uuidof(Recordset));
		//Cavphs->rsID->CursorLocation=adUseClient;
		//Cavphs->rsID->Open(_variant_t(EDIBgbl::SQLx),(IDispatch*)EDIBgbl::dbPRJ,adOpenDynamic,adLockOptimistic,adCmdText);
//		rsID.m_pDatabase=&EDIBgbl::dbPRJ;
//		rsID.Open(dbOpenDynaset,EDIBgbl::SQLx);
		rsID->Open((_bstr_t)EDIBgbl::SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		brsIDStatus=TRUE;
	}	
	try
	{
		while(!rsID->adoEOF)
		{
			rsID->get_Collect((_variant_t)_T("ID"), vTmp);
			vecID.push_back(vTmp);
			rsID->get_Collect((_variant_t)_T("CustomID"), vTmp);
			vecCustomID.push_back(vTmp);
			rsID->get_Collect((_variant_t)_T("Description"), vTmp);
			vecDescription.push_back(vtos2(vTmp));
			rsID->get_Collect((_variant_t)_T("ClassID"), vTmp);
			vecClassID.push_back(vtoi2(vTmp));
			rsID->MoveNext();	
		}


		EDIBgbl::SQLx=_T("SELECT SampleID FROM phsStructureREF WHERE SEQ=0");
//		rs2.Open(dbOpenSnapshot,EDIBgbl::SQLx);
		rs2->Open((_bstr_t)EDIBgbl::SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		rs2->MoveLast();
		rs2->MoveFirst();
		IC2=rs2->GetRecordCount();
		rs2->Close();

		EDIBgbl::SQLx = _T("SELECT * FROM phsStructureREF ORDER BY SampleID ASC, SEQ ASC");
//		rs2.Open(dbOpenDynaset ,EDIBgbl::SQLx);
		rs2->Open((_bstr_t)EDIBgbl::SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		CString strSQL = _T("select * from phsStructureNAME");
//		rs4.Open(dbOpenDynaset ,strSQL);
		rs4->Open((_bstr_t)strSQL,_variant_t((IDispatch*)EDIBgbl::dbPHScode,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		bool stop=false;
		int iNum=0;
		long iTimes=-1;//各零部件可用标志的连乘积
		if( rs2->adoEOF && rs2->BOF )
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
//			rs3.Open(dbOpenDynaset ,EDIBgbl::SQLx);
			rs2->Open((_bstr_t)EDIBgbl::SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 
			int fdL;
			FieldsPtr info;
//			rs3->get_Collect((_variant_t)_T("SampleName"),info);
			rs3->get_Fields(&info);
			fdL=info->GetCount();	
			sSPR = _T(" ");
			//移动到结构表头
			rs2->MoveFirst();
			while(!rs2->adoEOF)
			{
				AfxGetApp()->BeginWaitCursor();
				rs2->get_Collect((_variant_t) _T("SampleID"), &vTmp);
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
					rs2->MoveNext();
					if(rs2->adoEOF)
					{
						stop=true;
					}
					else
					{
						rs2->get_Collect((_variant_t) _T("SampleID"), &vTmp);
						SampleID =vtoi( vTmp);
						if(SampleID!=oldSampleID)
							stop=true;
					}
					rs2->MovePrevious();

					rs2->get_Collect((_variant_t) _T("SampleID"), &vTmp);
					SampleID =vtoi( vTmp);
					
					rs2->get_Collect((_variant_t)_T("ID"), &vTmp);
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

//							if(rsPA->Seek(_T("="),&vecCustomID[iPos]))

							rsPA->Seek(vecCustomID[iPos], adSeekAfter);
							if(!rsPA->adoEOF)
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
						rs2->MoveNext();
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
//								if(rsSA.Seek(_T("="),&vecCustomID[iPos]))
								rsPA->Seek(vecCustomID[iPos], adSeekAfter);
								if(!rsPA->adoEOF)
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
//									if(rsSP.Seek(_T("="),&vecCustomID[iPos])) //当前标准中有这个弹簧 
									rsSP->Seek(vecCustomID[iPos], adSeekAfter);
									if(!rsPA->adoEOF)
										iTimes =iTimes*(-1);
									else
										//当前标准中没有这个弹簧
										iTimes =0;
								}
								else if(ix==iCSPR)
								{
									iNumCSPR++;
//									if(rsCS.Seek(_T("="),&vecCustomID[iPos]))
									rsCS->Seek(vecCustomID[iPos], adSeekAfter);
									if(rsPA->adoEOF)
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
									rsPR->Seek(vecCustomID[iPos], adSeekAfter);
									if(rsPR->adoEOF)										//当前标准中有这个连接件
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
							rs2->MoveNext();
							continue;
						}
					}

				}while(!stop);

				
				//开始改模板名称
				//这会清除模板管理表中的使用频度信息

				EDIBgbl::SQLx=_T("SampleID=")+sTmp1;
				
				if(!rs3->Find((_bstr_t)EDIBgbl::SQLx, 0, adSearchForward, vTmp))
				{
					rs3->AddNew();

					rs3->put_Collect((_variant_t)_T("SampleID"),_variant_t(SampleID));
					sTmp1.Format(_T("%05d"),SampleID);
					rs3->put_Collect((_variant_t)_T("BmpName"),STR_VAR(sTmp1));					

					if(strTmpName.GetLength() > fdL)						
						strTmp=strTmpName.Left(fdL);
					else
						strTmp=strTmpName;

					rs4->AddNew();
					rs4->put_Collect((_variant_t)_T("SampleID"), COleVariant(SampleID));
					rs4->put_Collect((_variant_t)_T("BmpName"), COleVariant(sTmp1, VT_BSTRT));
					rs4->put_Collect((_variant_t)_T("SampleName"), COleVariant(strTmp, VT_BSTRT));
					rs4->put_Collect((_variant_t)_T("PA"), COleVariant(Ptype0, VT_BSTRT));
					rs4->put_Collect((_variant_t)_T("SA"), COleVariant(PtypeC, VT_BSTRT));
					rs4->put_Collect((_variant_t)_T("SPR"), COleVariant(sSPR, VT_BSTRT));
					rs4->put_Collect((_variant_t)_T("iNumOfPart"), COleVariant((long)iNum));
					rs4->put_Collect((_variant_t)_T("iNumSPR"), COleVariant(iNumSPR));
					rs4->put_Collect((_variant_t)_T("iNumCSPR"), COleVariant(iNumCSPR));
					rs4->put_Collect((_variant_t)_T("iNumRod"), COleVariant(iNumRod));
					rs4->put_Collect((_variant_t)_T("bAllowUse"), COleVariant((long)iTimes));
					rs4->Update();

				}
				else
				{
//					rs3.Edit();

					rs4->Find((_bstr_t)EDIBgbl::SQLx, 0, adSearchForward, vTmp);

					if(strTmpName.GetLength() > fdL)
						strTmp=strTmpName.Left(fdL);
					else
						strTmp=strTmpName;
//					rs4.Edit();
					rs4->put_Collect((_variant_t)_T("SampleName"), COleVariant(strTmp, VT_BSTRT));
					rs4->put_Collect((_variant_t)_T("PA"), COleVariant(Ptype0, VT_BSTRT));
					rs4->put_Collect((_variant_t)_T("SA"), COleVariant(PtypeC, VT_BSTRT));
					rs4->put_Collect((_variant_t)_T("SPR"), COleVariant(sSPR, VT_BSTRT));
					rs4->put_Collect((_variant_t)_T("iNumOfPart"), COleVariant((long)iNum));
					rs4->put_Collect((_variant_t)_T("iNumSPR"), COleVariant(iNumSPR));
					rs4->put_Collect((_variant_t)_T("iNumCSPR"), COleVariant(iNumCSPR));
					rs4->put_Collect((_variant_t)_T("iNumRod"), COleVariant(iNumRod));
					rs4->put_Collect((_variant_t)_T("bAllowUse"), COleVariant((long)iTimes));
					rs4->Update();

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
						rs3->put_Collect((_variant_t)_T("SampleName"),STR_VAR(strTmp)); 
					}
					if(Ptype0.GetLength()>0)
						rs3->put_Collect((_variant_t)_T("PA"), STR_VAR(Ptype0));
					if(PtypeC.GetLength()>0)
						rs3->put_Collect((_variant_t)_T("SA"),STR_VAR(PtypeC));
					if(sSPR.GetLength()>0)
						rs3->put_Collect((_variant_t)_T("SPR"),STR_VAR( sSPR));
					rs3->put_Collect((_variant_t)_T("iNumOfPart"),_variant_t((long)iNum));
					rs3->put_Collect((_variant_t)_T("iNumSPR"),_variant_t(iNumSPR));
					rs3->put_Collect((_variant_t)_T("iNumCSPR"),_variant_t(iNumCSPR));
					rs3->put_Collect((_variant_t)_T("iNumRod"),_variant_t(iNumRod));
					//模板是否允许使用
					rs3->put_Collect((_variant_t)_T("bAllowUse"),_variant_t( iTimes));
					rs3->Update();
//					EXECUTE_TIME_END
				}     
				sTmp1.Format(GetResStr(IDS_ChangeNoXNameInphsStructureName),(_T("\%d")),(_T("\%d")),strTmpName);
				frmStatus.m_Label2.Format(sTmp1, IC2, oldSampleID);
				frmStatus.UpdateData(false);
				frmStatus.UpdateStatus( (float)oldSampleID/(float)IC2, TRUE);
				frmStatus.UpdateWindow();
				rs2->MoveNext();
			}
			rs2->Close();
			rs3->Close();
			rs4->Close();
		}
	}
	catch(CException *e)
	{
		e->Delete();
	}
	//errH:
	AfxGetApp()->EndWaitCursor();
	if(rs2->State == adOpenStatic)
	rs2->Close();
	if(rs3->State == adOpenStatic)
	rs3->Close();
	//	timee=CTime::GetCurrentTime();
	//CTimeSpan t=timee-timeb;
	//CString s=t.Format(_T("%M分%S秒"));
	//s=_T("共用时:")+s;
	//ShowMessage(s);
	frmStatus.ShowWindow(SW_HIDE);
}


#pragma warning( disable : 4129 )  //Added by Shuli Luo
_RecordsetPtr Cphs::AvailableSampleIDrsForphsSamp(CString strOrderByDesc,CString strFilter)
{
	//可用于frmphsSamp窗体的数据控件选择数据
	CString tmpSQL;
	_RecordsetPtr ret;
	ret.CreateInstance(__uuidof(Recordset));
	if(ret->State == adOpenStatic)
		ret->Close();
// 	ret->m_strFilter=_T("");
// 	ret->m_strSort=_T("");
// 	ret->m_pDatabase=&EDIBgbl::dbPRJ;
	tmpSQL=CString((_T("SELECT * FROM phsStructureName ")));
	if(Trim(strFilter)!=_T(""))
		tmpSQL +=_T("WHERE ") + strFilter ;
	if(Trim(SortFieldName) !=_T(""))
		tmpSQL+=CString((_T(" ORDER BY "))) + (_T("  [")) + Trim(SortFieldName) + (_T("] ")) + strOrderByDesc;
//	ret->Open(dbOpenDynaset,tmpSQL);
	ret->Open((_bstr_t)tmpSQL,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
		adOpenDynamic, adLockReadOnly, adCmdText); 
	return ret;
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
//		 rs(&EDIBgbl::dbPHScode);//20071018 "dbSORT" 改为 "dbPHScode"
		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(_Recordset));
		//材料规范选择表SpecificationOfMaterial
		CString SQLx ;
		SQLx.Format(_T("SELECT * FROM SpecificationOfMaterial WHERE ClassIndex=%d AND ID=\'default\' AND tmin<=%g AND %g<tmax ORDER BY tmin,tmax,SEQ"),modPHScal::giClassIndex,modPHScal::gnConnectTJ,modPHScal::gnConnectTJ);
//		rs.Open(dbOpenSnapshot,SQLx);
		rs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPHScode,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		if(rs->adoEOF && rs->BOF)
		{
			//在%s库%s材料选择规范表没有%s字段值为默认值%s的记录(非管部零件材料选择规则)
			sTmp.Format(IDS_NoDefaultInCustomIDInSpecificationOfMaterial,EDIBgbl::dbPHScode->DefaultDatabase,_T("SpecificationOfMaterial"),_T("ID"),_T("default"));
			throw sTmp;
		}
		rs->MoveFirst();
		//所有不是管部、编号不为null、且材料尚无值的零件,其材料都可由环境计算温度查出
		//用户添加的材料bUserAdd!=0，编号为null
		SQLx=CString(_T("UPDATE ["))  
			+ EDIBgbl::Btype[EDIBgbl::TZB] 
			+ _T("] SET CLcl=\'");
		rs->get_Collect((_variant_t)_T("Material"), &vTmp);
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
		EDIBgbl::dbPRJDB->Execute((_bstr_t)SQLx, NULL, adCmdText); 
	}
	catch(CException *e)
	{
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
		_RecordsetPtr rs, rsTmp2,rsTmpZB;
		_RecordsetPtr rsCL, rsTmpCLgroup;
		rs.CreateInstance(__uuidof(Recordset));
		rsTmp2.CreateInstance(__uuidof(Recordset));
		rsTmpZB.CreateInstance(__uuidof(Recordset));
		rsCL.CreateInstance(__uuidof(Recordset));
		rsTmpCLgroup.CreateInstance(__uuidof(Recordset));

		long i;
		//		bool bFoundSA;// As Boolean
		long mvIndex =0; //索引号
		CString tbn , tmpBlkID , tmpID ;
		CString mvCL, sGROUPBY ,tmpStr;
		CString *Ptype=NULL;
		//		float sTmp=0, sngSumWeight=0;
		
		if( EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("TmpCLgroup")))
			//表TmpCLgroup用于保存分组结果
			EDIBgbl::dbPRJ->Execute((_bstr_t)_T("DROP TABLE TmpCLgroup"), NULL, adCmdText);
		//生成一个空表
		sGROUPBY = _T("CLgg,CLcl,CLmc,CLID,CLdw,CLdz");
		EDIBgbl::dbPRJDB->Execute((_bstr_t)(_T("SELECT ") + sGROUPBY + _T(" INTO TmpCLgroup IN \'") + (LPTSTR)(LPCTSTR)EDIBgbl::dbPRJ->DefaultDatabase + _T("\' FROM [") + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] WHERE zdjh=-1")), NULL, adCmdText);
		//GROUP BY子句的字段必须包含SELECT字句的全部字段,
		//但是seq,CLnum不能被分组,故要另外加入seq,CLnum字段到临时表
		EDIBgbl::dbPRJ->Execute((_bstr_t)_T("ALTER TABLE TmpCLgroup ADD COLUMN seq short"), NULL, adCmdText);
		EDIBgbl::dbPRJ->Execute((_bstr_t)_T("ALTER TABLE TmpCLgroup ADD COLUMN CLnum SINGLE"), NULL, adCmdText);
		//排序、分组统计零件，
		EDIBgbl::dbPRJDB->Execute((_bstr_t)(_T("INSERT INTO TmpCLgroup IN \'") + EDIBgbl::dbPRJ->DefaultDatabase + _T("\' SELECT DISTINCT ") + sGROUPBY + _T(" FROM [") + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" AND zdjh=") + ltos(modPHScal::zdjh) + _T(" AND NOT ISNULL(seq)  GROUP BY ") + sGROUPBY), NULL, adCmdText);
		
		sGROUPBY = _T("seq,CLgg,CLcl,CLmc,CLID,CLdw,CLdz,CLnum,CLzz,CLbz");
		
		if( EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("Tmp2")))
			EDIBgbl::dbPRJ->Execute((_bstr_t)_T("DROP TABLE tmp2"), NULL, adCmdText);
		CString SQLx;
		//建立空表tmp2
		SQLx = _T("SELECT ") + sGROUPBY + _T(" INTO tmp2 IN \'") + (LPTSTR)(LPCTSTR)EDIBgbl::dbPRJ->DefaultDatabase + _T("\' FROM [") + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] WHERE zdjh=-1");
		EDIBgbl::dbPRJDB->Execute((_bstr_t)SQLx, NULL, adCmdText);
		SQLx = _T("DELETE FROM tmp2");
		EDIBgbl::dbPRJ->Execute((_bstr_t)SQLx, NULL, adCmdText);
//		rsTmp2.m_pDatabase=&EDIBgbl::dbPRJ;
//		rsTmp2.Open(dbOpenDynaset,_T("SELECT * FROM tmp2") );
		rsTmp2->Open((_bstr_t)_T("SELECT * FROM tmp2"),_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		
		//将支吊架材料汇总功能转移到单独的过程中GetPhsBOM，以便材料统计的选项改变之后，能够正确生成材料汇总表
		//首先删除本支吊架材料数据
		SQLx = _T("DELETE * FROM [") +EDIBgbl:: Btype[EDIBgbl::TCL] + _T("] WHERE zdjh=") + ltos(modPHScal::zdjh) + _T(" AND VolumeID=") + ltos(EDIBgbl::SelVlmID);
		EDIBgbl::dbPRJDB->Execute((_bstr_t)SQLx, NULL, adCmdText);
		SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TCL] + _T("] WHERE zdjh=") + ltos(modPHScal::zdjh) + _T(" AND VolumeID=") + ltos(EDIBgbl::SelVlmID);
//		rsCL.m_pDatabase=&EDIBgbl::dbPRJDB;
//		rsCL.Open(dbOpenDynaset,SQLx);
		rsCL->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		
		//并且从管部到根部依次给零件编号
		//同时生成零件明细表到tmp2和材料汇总表到F????S-JCL
		SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] WHERE zdjh=") + ltos(modPHScal::zdjh) + _T(" AND VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" AND NOT ISNULL(seq) ORDER BY recno");
		if(rsTmpZB->State == adOpenStatic)
			rsTmpZB->Close();
//		rsTmpZB.m_pDatabase=&EDIBgbl::dbPRJDB;
//		rsTmpZB.Open(dbOpenDynaset,SQLx);
		rsCL->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 

		if(rsTmpZB->adoEOF && rsTmpZB->BOF)
		{
			//如果结构表没有任何零件
			tmpStr.Format(GetResStr(IDS_NoRecordInTZB),EDIBgbl::dbPRJDB->DefaultDatabase,EDIBgbl::Btype[EDIBgbl::TZB],
				EDIBgbl::SelJcdm,ltos(modPHScal::zdjh));
			throw tmpStr;
		}
		i = 1;
		COleVariant vTmp;
//		rsTmpCLgroup.m_pDatabase=&EDIBgbl::dbPRJ;
		COleVariant nil;
		nil.ChangeType(VT_NULL);
		while(!rsTmpZB->adoEOF)
		{
			CString CLgg,CLcl,CLmc;
			rsTmpZB->get_Collect((_variant_t)_T("CLgg"), &vTmp);
			CLgg=vtos(vTmp);
			rsTmpZB->get_Collect((_variant_t)_T("CLcl"), &vTmp);
			CLcl=vtos(vTmp);
			rsTmpZB->get_Collect((_variant_t)_T("CLmc"), &vTmp);
			CLmc=vtos(vTmp);
			//空的seq字段，写入编号给它
			SQLx=_T("UPDATE tmpCLgroup SET seq=") + ltos(i) + _T(" WHERE ISNULL(seq) AND trim(CLgg)=\'");
			SQLx+=CLgg+_T("\' AND trim(CLcl)=\'");
			SQLx+=CLcl+_T("\' AND (trim(CLmc))=\'") ;
			SQLx+=CLmc+_T("\'");
			EDIBgbl::dbPRJ->Execute((_bstr_t)SQLx, NULL, adCmdText);
			//Debug.Print dbPRJ.RecordsAffected, i, rsTmpZB(_T("CLmc")), rsTmpZB(_T("CLgg")), rsTmpZB(_T("CLcl"))
			//SQLx = _T("SELECT seq FROM tmpCLgroup INNER JOIN [") + EDIBgbl::TBNSelPrjSpec + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] AS tbn ON  WHERE ISNULL(seq) AND trim(CLgg)=\'")
			//	 +CLgg + _T("\' AND trim(CLcl)=\'") +CLcl + _T("\' AND (trim(CLmc))=\'") +CLmc+ _T("\' ");
			// SQLx = _T("SELECT * FROM tmpCLgroup WHERE NOT ISNULL(seq) AND trim(CLgg)='") & Trim(rsTmpZB.Fields(_T("CLgg"))) & _T("' AND trim(CLcl)='") & Trim(rsTmpZB.Fields(_T("CLcl"))) & _T("' AND (trim(CLmc))='") & Trim(rsTmpZB.Fields(_T("CLmc"))) & _T("' ")
			SQLx = _T("SELECT seq FROM tmpCLgroup WHERE NOT ISNULL(seq) AND trim(CLgg)=\'")
				+ CLgg+ _T("\' AND trim(CLcl)=\'") +CLcl+ _T("\' AND (trim(CLmc))=\'") +CLmc+ _T("\' ");
//			rsTmpCLgroup.Open(dbOpenSnapshot,SQLx);
			rsTmpCLgroup->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 
			if( rsTmpCLgroup->BOF && rsTmpCLgroup->adoEOF)
			{
				tmpStr.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb),EDIBgbl::dbPRJ->DefaultDatabase,_T("tmpCLgroup"),SQLx);
				throw tmpStr;
			}
			rsTmpCLgroup->MoveLast();
			//有记录受到影响，编号递增。
//			if(EDIBgbl::dbPRJ.GetRecordsAffected()>0)//返回最后一次更新、编辑、增加操作或Execute调用所影响的记录数
//			if(EDIBgbl::dbPRJ->()>0)//返回最后一次更新、编辑、增加操作或Execute调用所影响的记录数
//				i++;
			rsTmpZB->get_Collect((_variant_t)_T("CustomID"), &vTmp);
			if(vtoi(vTmp)!= iGCement)
			{
				//不是混凝土梁
				//不是所有的零件都写入了索引号,对于索引号为空的零件，需要查表
				mvIndex = vtoi(modPHScal::sFindFLD(_T("CustomID"), _T("Index"), vtos(vTmp)));
//				rsTmpZB.Edit();
				rsTmpCLgroup->get_Collect((_variant_t)_T("seq"), &vTmp);
				if(mvIndex != iSA)
				{
					//不是根部
					rsTmpZB->put_Collect((_variant_t)_T("seq"),vTmp);
				}
				else
				{
					if(!vtob(rsza->GetCollect(_T("bNotSumSA"))))
					{
						//是根部,要统计根部材料
						rsTmpZB->put_Collect((_variant_t)_T("seq"),vTmp);
					}
					else
						//是根部,不要统计根部材料
						rsTmpZB->put_Collect((_variant_t)_T("seq"),nil);
				}
				rsTmpZB->Update();
				
				//写零件明细表Tmp2
				rsTmp2->AddNew();
				rsTmpZB->get_Collect((_variant_t)_T("seq"), &vTmp);
				rsTmp2->put_Collect((_variant_t)_T("seq"),vTmp);
				rsTmpZB->get_Collect((_variant_t)_T("CLID"), &vTmp);
				rsTmp2->put_Collect((_variant_t)_T("CLID"),vTmp);
				rsTmpZB->get_Collect((_variant_t)_T("CLmc"), &vTmp);
				rsTmp2->put_Collect((_variant_t)_T("CLmc"),vTmp);
				rsTmpZB->get_Collect((_variant_t)_T("CLgg"), &vTmp);
				rsTmp2->put_Collect((_variant_t)_T("CLgg"),vTmp);
				rsTmpZB->get_Collect((_variant_t)_T("CLcl"), &vTmp);
				rsTmp2->put_Collect((_variant_t)_T("CLcl"),vTmp);
				rsTmpZB->get_Collect((_variant_t)_T("CLdw"), &vTmp);
				rsTmp2->put_Collect((_variant_t)_T("CLdw"),vTmp);
				rsTmpZB->get_Collect((_variant_t)_T("CLdz"), &vTmp);
				rsTmp2->put_Collect((_variant_t)_T("CLdz"),vTmp);
				rsTmpZB->get_Collect((_variant_t)_T("CLnum"), &vTmp);
				rsTmp2->put_Collect((_variant_t)_T("CLnum"),vTmp);
				rsTmpZB->get_Collect((_variant_t)_T("CLzz"), &vTmp);
				rsTmp2->put_Collect((_variant_t)_T("CLzz"),vTmp);
				rsTmpZB->get_Collect((_variant_t)_T("CLbz"), &vTmp);
				rsTmp2->put_Collect((_variant_t)_T("CLbz"),vTmp);
				rsTmp2->Update();
			}
			rsTmpZB->MoveNext();
			rsTmpCLgroup->Close();
		}
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
		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(Recordset));

		_RecordsetPtr rsza;
		rsza.CreateInstance(__uuidof(Recordset));

		_RecordsetPtr rsCL;
		rsCL.CreateInstance(__uuidof(Recordset));

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
		EDIBgbl::dbPRJDB->Execute((_bstr_t)SQLx, NULL, adCmdText);
		//确定要输出的字段:输出字段信息可以从TableINFO表和t??表获得,并且可以用户化
		sGROUPBY = _T("seq,CLgg,CLcl,CLmc,CLID,CLdw,CLdz,CLbz");
		//在dbprj中建立临时分类材料汇总表
		if( EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("TmpTCL")) )
		{
			//WorkPrj.mdb中的表TmpTCL用于保存分类汇总的中间结果
			EDIBgbl::dbPRJ->Execute((_bstr_t)_T("DROP TABLE TmpTCL"), NULL, adCmdText);
		}
		//拷贝TCL到TmpTCL，以便可以删除其中的选择过的材料。
		SQLx = _T("SELECT * INTO TmpTCL IN \'") + EDIBgbl::dbPRJ->DefaultDatabase + _T("\' FROM [") + EDIBgbl::Btype[EDIBgbl::TCL] + _T("] WHERE VolumeID=-1");
		EDIBgbl::dbPRJDB->Execute((_bstr_t)SQLx, NULL, adCmdText);
		if(rsID->State != adOpenStatic)
		{
			EDIBgbl::SQLx = _T("SELECT * FROM PictureClipData");
//			rsID.m_pDatabase=&EDIBgbl::dbPRJ;
//			rsID.Open(dbOpenDynaset,EDIBgbl::SQLx);
			rsID->Open((_bstr_t)EDIBgbl::SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 
			brsIDStatus=TRUE;
		}
		//在dbprj中生成空的Tmp2表
		if( EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("Tmp2")) )
		{
			//表TmpCLgroup用于保存分组结果
			EDIBgbl::dbPRJ->Execute((_bstr_t)_T("DROP TABLE Tmp2"), NULL, adCmdText);
		}
		CString strOrderBy;
		strOrderBy = sGROUPBY;
		//生成空的Tmp2表
		EDIBgbl::dbPRJDB->Execute((_bstr_t)CString(_T("SELECT ") + sGROUPBY + _T(",CLnum,CLnum*CLdz AS CLzz INTO Tmp2 IN \'") + (LPTSTR)(LPCTSTR)EDIBgbl::dbPRJ->DefaultDatabase + _T("\' FROM [") + EDIBgbl::Btype[EDIBgbl::TCL] + _T("] WHERE zdjh=0 AND VolumeID=-1")), NULL, adCmdText);
// 		rs.m_pDatabase=&EDIBgbl::dbPHScode;//20071101 "dbSORT" 改为 "dbPHScode"
// 		rs.Open(dbOpenSnapshot,_T("SELECT * FROM BomName ORDER BY BomIndex"));
		rs->Open((_bstr_t)_T("SELECT * FROM BomName ORDER BY BomIndex"),_variant_t((IDispatch*)EDIBgbl::dbPHScode,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		COleVariant vTmp,vTmp1,vTmp2;
		while(!rs->adoEOF)
		{
			rs->get_Collect((_variant_t)_T("BomTable"),vTmp);
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
						EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("DROP TABLE [") + strTmp + _T("]")), NULL, adCmdText);
					}
					//复制tmp2的结构到各个表
					EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("SELECT * INTO [") + strTmp + _T("] FROM Tmp2")), NULL, adCmdText);
				}
			}
			rs->MoveNext();
		}
		rs->Close();   
		
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
		EDIBgbl::dbPRJDB->Execute((_bstr_t)SQLx, NULL, adCmdText);
		
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
		EDIBgbl::dbPRJDB->Execute((_bstr_t)SQLx, NULL, adCmdText);
		
		//正式开始汇总材料
		SQLx = _T("SELECT * FROM TmpTCL");
// 		rsCL.m_pDatabase=&EDIBgbl::dbPRJ; 
// 		rsCL.Open(dbOpenDynaset,_T("SELECT * FROM TmpTCL") );
		rsCL->Open((_bstr_t)_T("SELECT * FROM TmpTCL"),_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		
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
// 		rs.m_pDatabase=&EDIBgbl::dbPRJDB;
// 		rs.Open(dbOpenDynaset,SQLx );
		rs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		if( rs->adoEOF && rs->BOF )
		{
			
		}
		else
		{
			rs->MoveLast();
			iSum+=rs->GetRecordCount();
		}
		rs->Close();
		
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
// 		rs.m_pDatabase = &EDIBgbl::dbPRJDB;
// 		rs.Open(dbOpenDynaset,SQLx );
		rs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		if( rs->adoEOF && rs->BOF ) 
		{	
		}
		else
		{
			rs->MoveLast();
			iSum+=rs->GetRecordCount();
		}
		rs->Close();
		
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
// 		rs.m_pDatabase = &EDIBgbl::dbPRJDB;rs.Open(dbOpenDynaset,SQLx);
		rs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		if( rs->adoEOF && rs->BOF )
		{	}
		else
		{
			rs->MoveLast();
			iSum+=rs->GetRecordCount();
		}
		rs->Close();
		
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
// 		rs.m_pDatabase = &EDIBgbl::dbPRJDB;rs.Open(dbOpenDynaset,SQLx);
		rs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		if( rs->adoEOF && rs->BOF ) {	}
		else
		{
			rs->MoveLast();
			iSum+=rs->GetRecordCount();
		}
		rs->Close();
		
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
// 		rs.m_pDatabase = &EDIBgbl::dbPRJDB;rs.Open(dbOpenDynaset,SQLx);
		rs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		if( rs->adoEOF && rs->BOF )
		{	}
		else
		{
			rs->MoveLast();
			//拉杆要处理2遍
			iSum+=2*rs->GetRecordCount();
		}
		rs->Close();
		
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
// 		rs.m_pDatabase = &EDIBgbl::dbPRJDB;rs.Open(dbOpenDynaset,SQLx);
		rs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		if( rs->adoEOF && rs->BOF )
		{	}
		else
		{
			rs->MoveLast();
			//型钢要处理3遍
			iSum+=3*rs->GetRecordCount();
		}
		rs->Close();
		
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
// 		rs.m_pDatabase = &EDIBgbl::dbPRJDB;rs.Open(dbOpenDynaset,SQLx);
		rs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		if( rs->adoEOF && rs->BOF ) 
		{	}
		else
		{
			rs->MoveLast();
			iSum+=rs->GetRecordCount();
		}
		rs->Close();
		
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
// 			rs.m_pDatabase = &EDIBgbl::dbPRJDB;rs.Open(dbOpenDynaset,SQLx);
			rs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 
			if( rs->adoEOF && rs->BOF )
			{	}
			else
			{
				rs->MoveLast();
				iSum+=rs->GetRecordCount();
			}
			rs->Close();
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
// 		rs.m_pDatabase=&EDIBgbl::dbPRJDB;
// 		rs.Open(dbOpenDynaset,SQLx );
		rs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		if( rs->adoEOF && rs->BOF )
		{
		}
		else
		{
			while(!rs->adoEOF)
			{
				frmStatus.m_Label1= GetResStr(IDS_SumPhsBomCSPR);
				strTmp.Format(GetResStr(IDS_SumPhsBomRec),ltos(iSum),ltos(iTmp));
				frmStatus.m_Label2=strTmp;
				frmStatus.UpdateStatus ( (float)iTmp/iSum, true);
				frmStatus.UpdateData(false);
				iTmp+=1;
				
				//SQLx = _T("SELECT * FROM [") +  EDIBgbl::Btype[EDIBgbl::TZA] + _T("] WHERE VolumeID=") + vtos(vTmp1) + _T(" AND zdjh=") + vtos(vTmp2);
				SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZA] + _T("] WHERE VolumeID=") + vtos(GetFields(rs,_T("VolumeID"))) + _T(" AND zdjh=") + vtos(GetFields(rs,_T("zdjh")));
// 				rsza.m_pDatabase = &EDIBgbl::dbPRJDB;rsza.Open(dbOpenDynaset,SQLx,dbReadOnly);
				rsza->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
					adOpenDynamic, adLockReadOnly, adCmdText); 
				if( rsza->adoEOF && rsza->BOF )
				{
				}
				else
				{
					//iSEQ = iSEQ + 1
					rsCL->AddNew();
					rsCL->put_Collect((_variant_t)_T("seq"),nil);
					rs->get_Collect((_variant_t)_T("zdjh"),v);
					rsCL->put_Collect((_variant_t)_T("zdjh"),v);
					rs->get_Collect((_variant_t)_T("VolumeID"),v);
					rsCL->put_Collect((_variant_t)_T("VolumeID"),v);
					rs->get_Collect((_variant_t)_T("CLID"),v);
					rsCL->put_Collect((_variant_t)_T("CLID"),v);
					rs->get_Collect((_variant_t)_T("CLmc"),v);
					rsCL->put_Collect((_variant_t)_T("CLmc"),v);
					rs->get_Collect((_variant_t)_T("CLgg"),v);
					rsCL->put_Collect((_variant_t)_T("CLgg"),v);
					rs->get_Collect((_variant_t)_T("CLcl"),v);
					rsCL->put_Collect((_variant_t)_T("CLcl"),v);
					rs->get_Collect((_variant_t)_T("CLdw"),v);
					CString str;
					str.LoadString(IDS_PIECE);
					rsCL->put_Collect((_variant_t)_T("CLdw"),(v.vt==VT_NULL) ? STR_VAR(str) : v);
					rs->get_Collect((_variant_t)_T("CLdz"),v);
					rsCL->put_Collect((_variant_t)_T("CLdz"),v);					
					rs->get_Collect((_variant_t)_T("CLzz"),v);
					rsCL->put_Collect((_variant_t)_T("CLzz"),v);
					if( EDIBAcad::g_bSumBomSprFactory )//newHS->是否在材料汇总表中标注弹簧/恒力弹簧厂家
					{
						rs->get_Collect((_variant_t)_T("CLbz"),v);
						rsCL->put_Collect((_variant_t)_T("CLbz"),v);
					}
					rs->get_Collect((_variant_t)_T("CLnum"),v);
					rsza->get_Collect((_variant_t)_T("Num"), &vTmp1);
					rsCL->put_Collect((_variant_t)_T("CLnum"),COleVariant(vtof(v)*vtof(vTmp1)));
					rsCL->Update();
				}
				rsza->Close();
				rs->MoveNext();
			}
		}
		rs->Close();
		//立刻将恒力弹簧从TCL分类汇总到Tmp2
		if( EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("TmpCLgroup")) )
		{
			//表TmpCLgroup用于保存分组结果
			EDIBgbl::dbPRJ->Execute((_bstr_t)_T("DROP TABLE TmpCLgroup"), NULL, adCmdText);
		}
		//rsCL->Close();
		//从tmp2生成一个分组结果TmpCLgroup
		//GROUP BY子句的字段必须包含SELECT字句的全部字段(除了聚合函数的字段)
		EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("SELECT ") + sGROUPBY + _T(",sum(CLnum) as CLnums,CLnums*CLdz as CLzz INTO TmpCLgroup IN \'") + (LPTSTR)(LPCTSTR)EDIBgbl::dbPRJ->DefaultDatabase + _T("\' FROM TmpTCL GROUP BY ") + sGROUPBY), NULL, adCmdText);
		//拷贝tmpCLgroup到tmp2,以便保存分组数据
		EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("INSERT INTO Tmp2 SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy), NULL, adCmdText);
		//将临时材料表的内容拷贝到正式材料表TCL
		EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("INSERT INTO [") + EDIBgbl::Btype[EDIBgbl::TCL] + _T("] IN \'") + (LPTSTR)(LPCTSTR)EDIBgbl::dbPRJDB->DefaultDatabase + _T("\' SELECT * FROM TmpTCL")), NULL, adCmdText);
		//将恒力弹簧的内容拷贝到TmpCSPR表
		EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("INSERT INTO TmpCSPR SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy), NULL, adCmdText);
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
// 		rs.m_pDatabase = &EDIBgbl::dbPRJDB;rs.Open(dbOpenDynaset,SQLx);
		rs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		if( rs->adoEOF && rs->BOF )
		{
		}
		else
		{
			while(!rs->adoEOF)
			{
				frmStatus.m_Label1= GetResStr(IDS_SumPhsBomSPR);
				strTmp.Format(GetResStr(IDS_SumPhsBomRec),ltos(iSum),ltos(iTmp));
				frmStatus.m_Label2=strTmp;
				frmStatus.UpdateStatus ( (float)iTmp/iSum, true);
				frmStatus.UpdateData(false);
				iTmp+=1;
				
				SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZA] + _T("] WHERE VolumeID=") + vtos(GetFields(rs,_T("VolumeID"))) + _T(" AND zdjh=") + vtos(GetFields(rs,_T("zdjh")));
// 				rsza.m_pDatabase = &EDIBgbl::dbPRJDB;rsza.Open(dbOpenDynaset,SQLx,dbReadOnly);
				rsza->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
					adOpenDynamic, adLockReadOnly, adCmdText); 
				if( rsza->adoEOF && rsza->BOF )
				{
				}
				else
				{
					rsza->MoveFirst();
					rsCL->AddNew();
					rsCL->put_Collect((_variant_t)_T("seq"),nil);
					rs->get_Collect((_variant_t)_T("zdjh"),v);
					rsCL->put_Collect((_variant_t)_T("zdjh"),v);
					rs->get_Collect((_variant_t)_T("VolumeID"),v);
					rsCL->put_Collect((_variant_t)_T("VolumeID"),v);
					rs->get_Collect((_variant_t)_T("CLID"),v);
					rsCL->put_Collect((_variant_t)_T("CLID"),v);
					rs->get_Collect((_variant_t)_T("CLmc"),v);
					rsCL->put_Collect((_variant_t)_T("CLmc"),v);
					rs->get_Collect((_variant_t)_T("CLgg"),v);
					rsCL->put_Collect((_variant_t)_T("CLgg"),v);
					rs->get_Collect((_variant_t)_T("CLcl"),v);
					rsCL->put_Collect((_variant_t)_T("CLcl"),v);
					rs->get_Collect((_variant_t)_T("CLdw"),v);
					CString str;
					str.LoadString(IDS_PIECE);
					rsCL->put_Collect((_variant_t)_T("CLdw"),(v.vt==VT_NULL) ? STR_VAR(str) : v);
					rs->get_Collect((_variant_t)_T("CLdz"),v);
					rsCL->put_Collect((_variant_t)_T("CLdz"),v);					
					rs->get_Collect((_variant_t)_T("CLzz"),v);
					rsCL->put_Collect((_variant_t)_T("CLzz"),v);
					if( EDIBAcad::g_bSumBomSprFactory )//newhs->是否在材料汇总表中标注弹簧/恒力弹簧厂家
					{
						rs->get_Collect((_variant_t)_T("CLbz"),v);
						rsCL->put_Collect((_variant_t)_T("CLbz"),v);
					}
					rs->get_Collect((_variant_t)_T("CLnum"),v);
					rsza->get_Collect((_variant_t)_T("Num"), &vTmp1);
					rsCL->put_Collect((_variant_t)_T("CLnum"),COleVariant(vtof(v)*vtof(vTmp1)));
					rsCL->Update();
				}
				rsza->Close();
				rs->MoveNext();
			}
		}
		rs->Close();
		//立刻将弹簧从TCL分类汇总到Tmp2
		if( EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("TmpCLgroup")) )
		{
			//表TmpCLgroup用于保存分组结果
			EDIBgbl::dbPRJ->Execute((_bstr_t)_T("DROP TABLE TmpCLgroup"), NULL, adCmdText);
		}
		//rsCL->Close();
		//从tmp2生成一个分组结果TmpCLgroup
		//GROUP BY子句的字段必须包含SELECT字句的全部字段(除了聚合函数的字段)
		SQLx =_T("SELECT ") + sGROUPBY + _T(",sum(CLnum) as CLnums,CLnums*CLdz as CLzz INTO TmpCLgroup   FROM TmpTCL GROUP BY ") + sGROUPBY;
		EDIBgbl::dbPRJ->Execute((_bstr_t)SQLx, NULL, adCmdText);
		//拷贝tmpCLgroup到tmp2,以便保存分组数据
		EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("INSERT INTO Tmp2 SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy), NULL, adCmdText);
		//将弹簧的内容拷贝到TmpSPR表
		EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("INSERT INTO TmpSPR SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy), NULL, adCmdText);
		//将临时材料表的内容拷贝到正式材料表TCL
		EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("INSERT INTO [") +  EDIBgbl::Btype[EDIBgbl::TCL] + _T("] IN \'") + (LPTSTR)(LPCTSTR)EDIBgbl::dbPRJDB->DefaultDatabase + _T("\' SELECT * FROM TmpTCL")), NULL, adCmdText);
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
// 		rs.m_pDatabase = &EDIBgbl::dbPRJDB;rs.Open(dbOpenDynaset,SQLx);
		rs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		if( rs->adoEOF && rs->BOF )
		{
		}
		else
		{
			while(!rs->adoEOF)
			{
				frmStatus.m_Label1= GetResStr(IDS_SumPhsBomPA);
				strTmp.Format(GetResStr(IDS_SumPhsBomRec),ltos(iSum),ltos(iTmp));
				frmStatus.m_Label2=strTmp;
				frmStatus.UpdateStatus ( (float)iTmp/iSum, true);
				frmStatus.UpdateData(false);
				iTmp+=1;
				
				SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZA] + _T("] WHERE VolumeID=") + vtos(GetFields(rs,_T("VolumeID"))) + _T(" AND zdjh=") + vtos(GetFields(rs,_T("zdjh")));
// 				rsza.m_pDatabase = &EDIBgbl::dbPRJDB;rsza.Open(dbOpenDynaset,SQLx,dbReadOnly);
				rs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
					adOpenDynamic, adLockReadOnly, adCmdText); 
				if( rsza->adoEOF && rsza->BOF )
				{
				}
				else
				{
					rsza->MoveFirst();
					rsCL->AddNew();
					rsCL->put_Collect((_variant_t)_T("seq"),nil);
					rs->get_Collect((_variant_t)_T("zdjh"),v);
					rsCL->put_Collect((_variant_t)_T("zdjh"),v);
					rs->get_Collect((_variant_t)_T("VolumeID"),v);
					rsCL->put_Collect((_variant_t)_T("VolumeID"),v);
					rs->get_Collect((_variant_t)_T("CLID"),v);
					rsCL->put_Collect((_variant_t)_T("CLID"),v);
					rs->get_Collect((_variant_t)_T("CLmc"),v);
					rsCL->put_Collect((_variant_t)_T("CLmc"),v);
					rs->get_Collect((_variant_t)_T("CLgg"),v);
					rsCL->put_Collect((_variant_t)_T("CLgg"),v);
					rs->get_Collect((_variant_t)_T("CLcl"),v);
					rsCL->put_Collect((_variant_t)_T("CLcl"),v);
					rs->get_Collect((_variant_t)_T("CLdw"),v);
					CString str;
					str.LoadString(IDS_PIECE);
					rsCL->put_Collect((_variant_t)_T("CLdw"),(v.vt==VT_NULL) ? STR_VAR(str) : v);
					rs->get_Collect((_variant_t)_T("CLdz"),v);
					rsCL->put_Collect((_variant_t)_T("CLdz"),v);					
					rs->get_Collect((_variant_t)_T("CLzz"),v);
					rsCL->put_Collect((_variant_t)_T("CLzz"),v);
					rs->get_Collect((_variant_t)_T("CLbz"),v);
					rsCL->put_Collect((_variant_t)_T("CLbz"),v);
					rs->get_Collect((_variant_t)_T("CLnum"),v);
					rsza->get_Collect((_variant_t)_T("Num"), &vTmp1);
					rsCL->put_Collect((_variant_t)_T("CLnum"),COleVariant(vtof(v)*vtof(vTmp1)));
					rsCL->Update();
				}
				rsza->Close();
				rs->MoveNext();
			}
		}
		rs->Close();
		//立刻将管部从TCL分类汇总到Tmp2
		if( EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("TmpCLgroup")) )
		{
			//表TmpCLgroup用于保存分组结果
			EDIBgbl::dbPRJ->Execute((_bstr_t)_T("DROP TABLE TmpCLgroup"), NULL, adCmdText);
		}
		//rsCL->Close();
		//从tmp2生成一个分组结果TmpCLgroup
		//GROUP BY子句的字段必须包含SELECT字句的全部字段(除了聚合函数的字段)
		SQLx=_T("SELECT ") + sGROUPBY + _T(",sum(CLnum) as CLnums,CLnums*CLdz as CLzz INTO TmpCLgroup    FROM TmpTCL ");
		SQLx +=_T(" GROUP BY ") + sGROUPBY;
		EDIBgbl::dbPRJ->Execute((_bstr_t)SQLx, NULL, adCmdText);
		//拷贝tmpCLgroup到tmp2,以便保存分组数据
		EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("INSERT INTO Tmp2 SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy), NULL, adCmdText);
		//将管部内容拷贝到TmpPA表
		EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("INSERT INTO TmpPA SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy), NULL, adCmdText);
		//将临时材料表的内容拷贝到正式材料表TCL
		EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("INSERT INTO [") + EDIBgbl::Btype[EDIBgbl::TCL] + _T("] IN \'") + (LPTSTR)(LPCTSTR)EDIBgbl::dbPRJDB->DefaultDatabase + _T("\' SELECT * FROM TmpTCL")), NULL, adCmdText);
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
// 		rs.m_pDatabase = &EDIBgbl::dbPRJDB;rs.Open(dbOpenDynaset,SQLx);
		rs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		if( rs->adoEOF && rs->BOF )
		{
		}
		else
		{
			while(!rs->adoEOF)
			{
				frmStatus.m_Label1= GetResStr(IDS_SumPhsBomNotSPRConnect);
				strTmp.Format(GetResStr(IDS_SumPhsBomRec),ltos(iSum),ltos(iTmp));
				frmStatus.m_Label2=strTmp;
				frmStatus.UpdateStatus ( (float)iTmp/iSum, true);
				frmStatus.UpdateData(false);
				iTmp+=1;
				
				SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZA] + _T("] WHERE VolumeID=") + vtos(GetFields(rs,_T("VolumeID"))) + _T(" AND zdjh=") + vtos(GetFields(rs,_T("zdjh")));
// 				rsza.m_pDatabase = &EDIBgbl::dbPRJDB;rsza.Open(dbOpenDynaset,SQLx,dbReadOnly);
				rs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
					adOpenDynamic, adLockReadOnly, adCmdText); 
				if( rsza->adoEOF && rsza->BOF )
				{
				}
				else
				{
					rsza->MoveFirst();
					rsCL->AddNew();
					rsCL->put_Collect((_variant_t)_T("seq"),nil);
					rs->get_Collect((_variant_t)_T("zdjh"),v);
					rsCL->put_Collect((_variant_t)_T("zdjh"),v);
					rs->get_Collect((_variant_t)_T("VolumeID"),v);
					rsCL->put_Collect((_variant_t)_T("VolumeID"),v);
					rs->get_Collect((_variant_t)_T("CLID"),v);
					rsCL->put_Collect((_variant_t)_T("CLID"),v);
					rs->get_Collect((_variant_t)_T("CLmc"),v);
					rsCL->put_Collect((_variant_t)_T("CLmc"),v);
					rs->get_Collect((_variant_t)_T("CLgg"),v);
					rsCL->put_Collect((_variant_t)_T("CLgg"),v);
					rs->get_Collect((_variant_t)_T("CLcl"),v);
					rsCL->put_Collect((_variant_t)_T("CLcl"),v);
					rs->get_Collect((_variant_t)_T("CLdw"),v);
					CString str;
					str.LoadString(IDS_PIECE);
					rsCL->put_Collect((_variant_t)_T("CLdw"),(v.vt==VT_NULL) ? STR_VAR(str) : v);
					rs->get_Collect((_variant_t)_T("CLdz"),v);
					rsCL->put_Collect((_variant_t)_T("CLdz"),v);					
					rs->get_Collect((_variant_t)_T("CLzz"),v);
					rsCL->put_Collect((_variant_t)_T("CLzz"),v);
					rs->get_Collect((_variant_t)_T("CLbz"),v);
					rsCL->put_Collect((_variant_t)_T("CLbz"),v);
					rs->get_Collect((_variant_t)_T("CLnum"),v);
					rsza->get_Collect((_variant_t)_T("Num"), &vTmp1);
					rsCL->put_Collect((_variant_t)_T("CLnum"),COleVariant(vtof(v)*vtof(vTmp1)));
					rsCL->Update();
				}
				rsza->Close();
				rs->MoveNext();
			}
		}
		rs->Close();
		//立刻将非弹性连接件从TCL分类汇总到Tmp2
		if( EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("TmpCLgroup")) )
		{
			//表TmpCLgroup用于保存分组结果
			EDIBgbl::dbPRJ->Execute((_bstr_t)_T("DROP TABLE TmpCLgroup"), NULL, adCmdText);
		}
		//rsCL->Close();
		//从tmp2生成一个分组结果TmpCLgroup
		//GROUP BY子句的字段必须包含SELECT字句的全部字段(除了聚合函数的字段)
		SQLx=_T("SELECT ") + sGROUPBY + _T(",sum(CLnum) as CLnums,CLnums*CLdz as CLzz INTO TmpCLgroup   FROM TmpTCL ");
		SQLx +=_T(" GROUP BY ") + sGROUPBY;
		EDIBgbl::dbPRJ->Execute((_bstr_t)SQLx, NULL, adCmdText);
		//拷贝tmpCLgroup到tmp2,以便保存分组数据
		EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("INSERT INTO Tmp2 SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy), NULL, adCmdText);
		//将非弹性连接件(不包括拉杆)的内容拷贝到TmpConnection表
		EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("INSERT INTO TmpConnection SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy), NULL, adCmdText);
		//将临时材料表的内容拷贝到正式材料表TCL
		EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("INSERT INTO [") +EDIBgbl::Btype[EDIBgbl::TCL] + _T("] IN \'") + (LPTSTR)(LPCTSTR)EDIBgbl::dbPRJDB->DefaultDatabase + _T("\' SELECT * FROM TmpTCL")), NULL, adCmdText);
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
// 		rs.m_pDatabase = &EDIBgbl::dbPRJDB;rs.Open(dbOpenDynaset,SQLx);
		rs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		if( rs->adoEOF && rs->BOF )
		{
			//没有任何拉杆
		}
		else
		{
			//有任何拉杆,RS-圆钢
			this->rsID->Find((_bstr_t)(_T("trim(ID)=\'RS\'")), 0, adSearchForward, vTmp);
			while(!rs->adoEOF)
			{
				frmStatus.m_Label1= GetResStr(IDS_SumPhsBomRod);
				strTmp.Format(GetResStr(IDS_SumPhsBomRec),ltos(iSum),ltos(iTmp));
				frmStatus.m_Label2=strTmp;
				frmStatus.UpdateStatus ( (float)iTmp/iSum, true);
				frmStatus.UpdateData(false);
				iTmp+=1;
				
				SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZA] + _T("] WHERE VolumeID=") + vtos(GetFields(rs,_T("VolumeID"))) + _T(" AND zdjh=") + vtos(GetFields(rs,_T("zdjh")));
// 				rsza.m_pDatabase=&EDIBgbl::dbPRJDB; rsza.Open(dbOpenSnapshot,SQLx,dbReadOnly);
				rs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
					adOpenDynamic, adLockReadOnly, adCmdText); 
				if( rsza->adoEOF && rsza->BOF )
				{
				}
				else
				{
					rsza->MoveFirst();
					
					sTmp=((vTmp=GetFields(rsza,_T("num"))).vt==VT_NULL ? 1.0 : vtof(vTmp)) * ((vTmp=GetFields(rs,_T("CLnum"))).vt==VT_NULL ? 1.0 : vtof(vTmp));					
					sngSumWeight=sTmp * vtof(GetFields(rs,_T("GDW1"))) * vtof(GetFields(rs,_T("L1")) ) / 1000.0; //拉杆1m单重保存在GDW1字段
					rsCL->AddNew();
					rsCL->put_Collect((_variant_t)_T("seq"),nil);
					rs->get_Collect((_variant_t)_T("zdjh"),v);
					rsCL->put_Collect((_variant_t)_T("zdjh"),v);
					rs->get_Collect((_variant_t)_T("VolumeID"),v);
					rsCL->put_Collect((_variant_t)_T("VolumeID"),v);
					rs->get_Collect((_variant_t)_T("CLID"),v);
					rsCL->put_Collect((_variant_t)_T("CLID"),v);
					rs->get_Collect((_variant_t)_T("CLmc"),v);
					rsCL->put_Collect((_variant_t)_T("CLmc"),v);
					rs->get_Collect((_variant_t)_T("CLgg"),v);
					rsCL->put_Collect((_variant_t)_T("CLgg"),v);
					rs->get_Collect((_variant_t)_T("CLcl"),v);
					rsCL->put_Collect((_variant_t)_T("CLcl"),v);
					rs->get_Collect((_variant_t)_T("CLdw"),v);
					CString str;
					str.LoadString(IDS_PIECE);
					rsCL->put_Collect((_variant_t)_T("CLdw"),(v.vt==VT_NULL) ? STR_VAR(str) : v);
					rs->get_Collect((_variant_t)_T("CLdz"),v);
					rsCL->put_Collect((_variant_t)_T("CLdz"),v);					
					rs->get_Collect((_variant_t)_T("CLbz"),v);
					rsCL->put_Collect((_variant_t)_T("CLbz"),v);
					rs->get_Collect((_variant_t)_T("GDW1"),v);
					rs->get_Collect((_variant_t)_T("L1"), &vTmp1);
					
					rsCL->put_Collect((_variant_t)_T("CLdz"),COleVariant(vtof(v) * vtof(vTmp1)/1000.0)); //拉杆1m单重保存在GDW1字段
					rsCL->put_Collect((_variant_t)_T("CLnum"),COleVariant(sTmp));
					rsCL->put_Collect((_variant_t)_T("CLzz"),COleVariant(sngSumWeight));
					rsCL->Update();
				}
				rsza->Close();
				rs->MoveNext();
			}
		}
		//立刻将拉杆零件从TCL分类汇总到Tmp2
		if( EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("TmpCLgroup")) )
		{
			//表TmpCLgroup用于保存分组结果
			EDIBgbl::dbPRJ->Execute((_bstr_t)_T("DROP TABLE TmpCLgroup"), NULL, adCmdText);
		}
		//rsCL->Close();
		//从tmp2生成一个分组结果TmpCLgroup
		//GROUP BY子句的字段必须包含SELECT字句的全部字段(除了聚合函数的字段)
		SQLx=_T("SELECT ") + sGROUPBY + _T(",sum(CLnum) as CLnums,CLnums*CLdz as CLzz INTO TmpCLgroup   FROM TmpTCL ");
		SQLx +=_T(" GROUP BY ") + sGROUPBY;
		EDIBgbl::dbPRJ->Execute((_bstr_t)SQLx, NULL, adCmdText);
		if(modPHScal::gbSumRodByRoundSteel)
			;
		else
		{
			//拷贝tmpCLgroup到tmp2,以便保存分组数据
			EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("INSERT INTO Tmp2 SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy), NULL, adCmdText);
			//将拉杆的内容拷贝到TmpConnection表
			EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("INSERT INTO TmpConnection SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy), NULL, adCmdText);
		}
		//将临时材料表的内容拷贝到正式材料表TCL
		EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("INSERT INTO [") + EDIBgbl::Btype[EDIBgbl::TCL] + _T("] IN \'") + (LPTSTR)(LPCTSTR)EDIBgbl::dbPRJDB->DefaultDatabase + _T("\' SELECT * FROM TmpTCL")), NULL, adCmdText);
		//删除临时表
		RsDeleteAll(rsCL);
		//更新临时材料表数据记对象
		
		//************
		//统计拉杆长度（按圆钢直径分类）
		//回到第一条记录
		if( rs->adoEOF && rs->BOF )
		{
			//没有任何拉杆
		}
		else
		{
			rs->MoveFirst();
			//有任何拉杆
			while(!rs->adoEOF)
			{
				frmStatus.m_Label1= GetResStr(IDS_SumPhsBomRodLen);
				strTmp.Format(GetResStr(IDS_SumPhsBomRec),ltos(iSum),ltos(iTmp));
				frmStatus.m_Label2=strTmp;
				frmStatus.UpdateStatus ( (float)iTmp/iSum, true);
				frmStatus.UpdateData(false);
				iTmp+=1;
				
				SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZA] + _T("] WHERE VolumeID=") + vtos(GetFields(rs,_T("VolumeID"))) + _T(" AND zdjh=") + vtos(GetFields(rs,_T("zdjh")));
//				rsza.m_pDatabase=&EDIBgbl::dbPRJDB; rsza.Open(dbOpenSnapshot,SQLx,dbReadOnly);
				rsza->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
					adOpenDynamic, adLockReadOnly, adCmdText); 
				if( rsza->adoEOF && rsza->BOF )
				{
				}
				else
				{
					rsza->MoveFirst();
					
					sTmp=((vTmp=GetFields(rsza,_T("num"))).vt==VT_NULL ? 1.0 : vtof(vTmp)) * vtof(GetFields(rs,_T("CLnum"))) * vtof(GetFields(rs,_T("L1"))) / 1000.0;
					sngSumWeight = sTmp * vtof(GetFields(rs,_T("GDW1")));  //拉杆1m单重保存在GDW1字段
					//if( sTmp <> 0 ){ Debug.Print sTmp
					rsCL->AddNew();
					rsCL->put_Collect((_variant_t)_T("seq"),nil);
					rs->get_Collect((_variant_t)_T("zdjh"),v);
					rsCL->put_Collect((_variant_t)_T("zdjh"),v);
					rs->get_Collect((_variant_t)_T("VolumeID"),v);
					rsCL->put_Collect((_variant_t)_T("VolumeID"),v);
					rs->get_Collect((_variant_t)_T("CLID"),v);
					rsCL->put_Collect((_variant_t)_T("CLID"),v);
					rsCL->put_Collect((_variant_t)_T("CLmc"),GetFields(this->rsID,_T("Description")));
					rsCL->put_Collect((_variant_t)_T("CLgg"),STR_VAR(CString(_T("d=")) +vtos(GetFields(rs,_T("sizeC")))));
					rsCL->put_Collect((_variant_t)_T("CLcl"),GetFields(rs,_T("CLcl")));
					CString str;
					str.LoadString(IDS_METER);
					rsCL->put_Collect((_variant_t)_T("CLdw"),(GetFields(rs,_T("Cldw")).vt==VT_NULL ?  STR_VAR(str) : GetFields(rs,_T("CLdw"))));
					//rs->get_Collect((_variant_t)_T("GDW1"), &vTmp); vTmp.vt==VT_NULL ? strTmp=_T("0") : strTmp.Format(_T("%.3f"),vtof(vTmp));rsCL->put_Collect((_variant_t)_T("CLdz"),STR_VAR(strTmp));
					
					rsCL->put_Collect((_variant_t)_T("CLdz"),GetFields(rs,_T("GDW1")));
					//rsCL->put_Collect((_variant_t)_T("CLdz")) = Format(IIf(IsNull(rsCL->put_Collect((_variant_t)_T("CLdz"))), 0, rsCL->put_Collect((_variant_t)_T("CLdz"))), _T("0.##"))
					rsCL->put_Collect((_variant_t)_T("CLnum"),COleVariant(sTmp));
					rsCL->put_Collect((_variant_t)_T("CLzz"),COleVariant(sngSumWeight));
					rsCL->put_Collect((_variant_t)_T("CLbz"),GetFields(rs,_T("CLbz")));
					rsCL->Update();
				}
				rsza->Close();
				rs->MoveNext();
			}
		}
		rs->Close();
		//立刻将拉杆材料从TCL分类汇总到Tmp2
		if( EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("TmpCLgroup")) )
		{
			//表TmpCLgroup用于保存分组结果
			EDIBgbl::dbPRJ->Execute((_bstr_t)_T("DROP TABLE TmpCLgroup"), NULL, adCmdText);
		}
		//rsCL->Close();
		//从tmp2生成一个分组结果TmpCLgroup
		//GROUP BY子句的字段必须包含SELECT字句的全部字段(除了聚合函数的字段)
		SQLx=_T("SELECT ") + sGROUPBY + _T(",sum(CLnum) as CLnums,CLnums*CLdz as CLzz INTO TmpCLgroup   FROM TmpTCL ");
		SQLx +=_T(" GROUP BY ") + sGROUPBY;
		EDIBgbl::dbPRJ->Execute((_bstr_t)SQLx, NULL, adCmdText);
		//拷贝tmpCLgroup到tmp2,以便保存分组数据
		//EDIBgbl::dbPRJ->Execute((_bstr_t)_T("INSERT INTO Tmp2 SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy
		if(modPHScal::gbSumRodByRoundSteel)
		{
			//拷贝tmpCLgroup到tmp2,以便保存分组数据,tmp2的数据记录将输出
			EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("INSERT INTO Tmp2 SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy), NULL, adCmdText);
			//将临时材料表的内容拷贝到正式材料表TCL
			EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("INSERT INTO [") + EDIBgbl::Btype[EDIBgbl::TCL] + _T("] IN \'") + (LPTSTR)(LPCTSTR)EDIBgbl::dbPRJDB->DefaultDatabase + _T("\' SELECT * FROM TmpTCL")), NULL, adCmdText);
		}
		
		//将拉杆的内容拷贝到TmpLugBom表
		EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("INSERT INTO TmpLugBom SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy), NULL, adCmdText);
		//将临时材料表的内容拷贝到正式材料表TCL
		//EDIBgbl::dbPRJ->Execute((_bstr_t)_T("INSERT INTO [") + EDIBgbl::TBNSelPrjSpec + EDIBgbl::Btype[EDIBgbl::TCL] + _T("] IN \'") + EDIBgbl::dbPRJDB->DefaultDatabase + _T("\' SELECT * FROM TmpTCL")
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
//		rs.m_pDatabase = &EDIBgbl::dbPRJDB;rs.Open(dbOpenDynaset,SQLx);
		rs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		if( rs->adoEOF && rs->BOF )
		{
			//没有任何非型钢,非螺栓螺母等的附件
		}
		else
		{
			//有任何非型钢,非螺栓螺母等的零件
			while(!rs->adoEOF)
			{
				frmStatus.m_Label1= GetResStr(IDS_SumPhsBomAttachements);
				strTmp.Format(GetResStr(IDS_SumPhsBomRec),ltos(iSum),ltos(iTmp));
				frmStatus.m_Label2=strTmp;
				frmStatus.UpdateStatus ( (float)iTmp/iSum, true);
				frmStatus.UpdateData(false);
				iTmp+=1;
				
				//只选择根部材料需要统计的支吊架
				SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZA] + _T("] WHERE VolumeID=") + vtos(GetFields(rs,_T("VolumeID"))) + _T(" AND zdjh=") + vtos(GetFields(rs,_T("zdjh")));
//				rsza.m_pDatabase = &EDIBgbl::dbPRJDB;rsza.Open(dbOpenDynaset,SQLx,dbReadOnly);
				rsza->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
					adOpenDynamic, adLockReadOnly, adCmdText); 
				if( rsza->adoEOF && rsza->BOF )
				{
				}
				else
				{
					rsza->MoveFirst();
					
					if( this->rsID->Find((_bstr_t) (_T("trim(CustomID)=\'") + vtos(GetFields(rs,_T("CustomID"))) + _T("\'")), 0, adSearchForward, vTmp))
					{
						int intIndex=vtoi(GetFields(this->rsID,_T("Index")));
						if( intIndex==iSectionSteel )
						{
						}
						else
						{
							rsza->get_Collect((_variant_t)_T("num"),v);
							m_iNum=vtoi(v);
							rsza->get_Collect((_variant_t)_T("Gnum"),v);
							m_iSANum=vtoi(v);
							rsza->get_Collect((_variant_t)_T("bNotSumSA"),v);
							m_bNotSumSA=vtob(v);
							rs->get_Collect((_variant_t)_T("CLnum"),v);
							m_iCLNum=vtoi(v);
							rs->get_Collect((_variant_t)_T("bUserAdd"),v);
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
							rsCL->AddNew();
							rsCL->put_Collect((_variant_t)_T("seq"),nil);
							rs->get_Collect((_variant_t)_T("zdjh"),v);
							rsCL->put_Collect((_variant_t)_T("zdjh"),v);
							rs->get_Collect((_variant_t)_T("VolumeID"),v);
							rsCL->put_Collect((_variant_t)_T("VolumeID"),v);
							rs->get_Collect((_variant_t)_T("CLID"),v);
							rsCL->put_Collect((_variant_t)_T("CLID"),v);
							rs->get_Collect((_variant_t)_T("CLmc"),v);
							rsCL->put_Collect((_variant_t)_T("CLmc"),v);
							rsCL->put_Collect((_variant_t)_T("CLgg"),GetFields(rs,_T("CLgg")));
							rsCL->put_Collect((_variant_t)_T("CLcl"),GetFields(rs,_T("CLcl")));
							//rsCL->put_Collect((_variant_t)_T("CLdw"),((vTmp=GetFields(rs,_T("Cldw"))).vt==VT_NULL ? STR_VAR(_T("件")) : vTmp));
							CString str;
							str.LoadString(IDS_PIECE);
							rsCL->put_Collect((_variant_t)_T("CLdw"),((vTmp=GetFields(rs,_T("Cldw"))).vt==VT_NULL ? STR_VAR(str) : vTmp));
							rsCL->put_Collect((_variant_t)_T("CLdz"),GetFields(rs,_T("CLdz")));
							//rsCL->put_Collect((_variant_t)_T("CLdz")) = Format(IIf(IsNull(rsCL->put_Collect((_variant_t)_T("CLdz"))), 0, rsCL->put_Collect((_variant_t)_T("CLdz"))), _T("0.##"))
							rsCL->put_Collect((_variant_t)_T("CLnum"),COleVariant(sTmp));
							rsCL->put_Collect((_variant_t)_T("CLzz"),COleVariant(sngSumWeight));
							rsCL->put_Collect((_variant_t)_T("CLbz"),GetFields(rs,_T("CLbz")));
							rsCL->Update();
						}
					}
				}
				rsza->Close();
				rs->MoveNext();
			}
			//立刻将非螺母,螺栓根部附件从TCL分类汇总到Tmp2
			if( EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("TmpCLgroup")) )
			{
				//表TmpCLgroup用于保存分组结果
				EDIBgbl::dbPRJ->Execute((_bstr_t)_T("DROP TABLE TmpCLgroup"), NULL, adCmdText);
			}
			//rsCL->Close();
			//从tmp2生成一个分组结果TmpCLgroup
			//GROUP BY子句的字段必须包含SELECT字句的全部字段(除了聚合函数的字段)
			SQLx=_T("SELECT ") + sGROUPBY + _T(",sum(CLnum) as CLnums,CLnums*CLdz as CLzz INTO TmpCLgroup   FROM TmpTCL ");
			SQLx +=_T(" GROUP BY ") + sGROUPBY;
			EDIBgbl::dbPRJ->Execute((_bstr_t)SQLx, NULL, adCmdText);
			//拷贝tmpCLgroup到tmp2,以便保存分组数据
			EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("INSERT INTO Tmp2 SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy), NULL, adCmdText);
			//将非螺母,螺栓根部附件的内容拷贝到TmpAttachment表
			EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("INSERT INTO TmpAttachment SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy), NULL, adCmdText);
			//将临时表的内容拷贝到正式材料表TCL
			EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("INSERT INTO [") + EDIBgbl::Btype[EDIBgbl::TCL] + _T("] IN \'") + (LPTSTR)(LPCTSTR)EDIBgbl::dbPRJDB->DefaultDatabase + _T("\' SELECT * FROM TmpTCL")), NULL, adCmdText);
			//删除临时表
			RsDeleteAll(rsCL);
			//更新临时材料表数据记对象
			//rsCL.Open(dbOpenDynaset,_T("SELECT * FROM TmpTCL") );
		}
		
		
		//型钢按长度统计单独写一个表TmpSSBom
		if( rs->adoEOF && rs->BOF )
		{
			//没有型钢
		}else
		{
			rs->MoveFirst();
			while(!rs->adoEOF)
			{
				frmStatus.m_Label1= GetResStr(IDS_SumPhsBomSSLen);
				strTmp.Format(GetResStr(IDS_SumPhsBomRec),ltos(iSum),ltos(iTmp));
				frmStatus.m_Label2=strTmp;
				frmStatus.UpdateStatus ( (float)iTmp/iSum, true);
				frmStatus.UpdateData(false);
				iTmp+=1;
				
				SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZA] + _T("] WHERE VolumeID=") + vtos(GetFields(rs,_T("VolumeID"))) + _T(" AND zdjh=") + vtos(GetFields(rs,_T("zdjh"))) + _T(" AND  NOT bNotSumSA ");
//				rsza.m_pDatabase = &EDIBgbl::dbPRJDB;rsza.Open(dbOpenDynaset,SQLx,dbReadOnly);
				rsza->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
					adOpenDynamic, adLockReadOnly, adCmdText); 
				if( rsza->adoEOF && rsza->BOF )
				{
				}
				else
				{
					rsza->MoveFirst();
					
					if(this->rsID->Find((_bstr_t) (_T("trim(CustomID)=\'") + vtos(GetFields(rs,_T("CustomID"))) + _T("\'")), 0, adSearchForward, vTmp))
					{					
						if( vtoi(GetFields(this->rsID,_T("Index")))==iSectionSteel )
						{                  
							rsza->get_Collect((_variant_t)_T("num"),v);
							m_iNum=vtoi(v);
							rsza->get_Collect((_variant_t)_T("Gnum"),v);
							m_iSANum=vtoi(v);
							rsza->get_Collect((_variant_t)_T("bNotSumSA"),v);
							m_bNotSumSA=vtob(v);
							rs->get_Collect((_variant_t)_T("CLnum"),v);
							m_iCLNum=vtoi(v);
							rs->get_Collect((_variant_t)_T("bUserAdd"),v);
							m_bUserAdd=vtob(v);
							rs->get_Collect((_variant_t)_T("L1"),v);
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
							rsCL->AddNew();
							rsCL->put_Collect((_variant_t)_T("seq"),nil);
							rs->get_Collect((_variant_t)_T("zdjh"),v);
							rsCL->put_Collect((_variant_t)_T("zdjh"),v);
							rs->get_Collect((_variant_t)_T("VolumeID"),v);
							rsCL->put_Collect((_variant_t)_T("VolumeID"),v);
							rs->get_Collect((_variant_t)_T("CLID"),v);
							rsCL->put_Collect((_variant_t)_T("CLID"),v);
							rs->get_Collect((_variant_t)_T("CLmc"),v);
							rsCL->put_Collect((_variant_t)_T("CLmc"),v);
							rs->get_Collect((_variant_t)_T("CLgg"),v);
							rsCL->put_Collect((_variant_t)_T("CLgg"),v);
							rsCL->put_Collect((_variant_t)_T("CLcl"),GetFields(rs,_T("CLcl")));
							CString str;
							str.LoadString(IDS_METER);
							rsCL->put_Collect((_variant_t)_T("CLdw"),(GetFields(rs,_T("Cldw")).vt==VT_NULL ?  STR_VAR(str) : GetFields(rs,_T("CLdw"))));
							rsCL->put_Collect((_variant_t)_T("CLdz"),GetFields(rs,_T("CLdz")));
							//rsCL->put_Collect((_variant_t)_T("CLdz")) = Format(IIf(IsNull(rsCL->put_Collect((_variant_t)_T("CLdz"))), 0, rsCL->put_Collect((_variant_t)_T("CLdz"))), _T("0.##"))
							rsCL->put_Collect((_variant_t)_T("CLnum"),COleVariant(sTmp));
							rsCL->put_Collect((_variant_t)_T("CLzz"),COleVariant(sngSumWeight));
							rsCL->put_Collect((_variant_t)_T("CLbz"),GetFields(rs,_T("CLbz")));
							rsCL->Update();
						}
					}
				}
				rsza->Close();
				rs->MoveNext();
			}
			//立刻将型钢从TCL分类汇总到Tmp2
			if( EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("TmpCLgroup")) )
			{
				//表TmpCLgroup用于保存分组结果
				EDIBgbl::dbPRJ->Execute((_bstr_t)_T("DROP TABLE TmpCLgroup"), NULL, adCmdText);
			}
			//rsCL->Close();
			//从tmp2生成一个分组结果TmpCLgroup
			//GROUP BY子句的字段必须包含SELECT字句的全部字段(除了聚合函数的字段)
			SQLx=_T("SELECT ") + sGROUPBY + _T(",sum(CLnum) as CLnums,CLnums*CLdz as CLzz INTO TmpCLgroup   FROM TmpTCL ");
			SQLx +=_T(" GROUP BY ") + sGROUPBY;
			EDIBgbl::dbPRJ->Execute((_bstr_t)SQLx, NULL, adCmdText);
			//拷贝tmpCLgroup到tmp2,以便保存分组数据
			//下句不可少，因为AutoCAD输出材料表时首先要调用GetphsSumBom获得所有材料的集合，且ACAD中该集合来源为tmp2表。
			EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("INSERT INTO Tmp2 SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy), NULL, adCmdText);
			//将型钢的内容拷贝到TmpSSBom表
			EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("INSERT INTO TmpSSBom SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy), NULL, adCmdText);
			//将临时表的内容拷贝到正式材料表TCL
			//EDIBgbl::dbPRJ->Execute((_bstr_t)_T("INSERT INTO [") + EDIBgbl::TBNSelPrjSpec + EDIBgbl::Btype[EDIBgbl::TCL] + _T("] IN \'") + EDIBgbl::dbPRJDB->DefaultDatabase + _T("\' SELECT * FROM TmpTCL")
			//删除临时表
			RsDeleteAll(rsCL);
			//更新临时材料表数据记对象
		}
		
		
		//型钢按零件统计单独写一个表TmpSS
		if( rs->adoEOF && rs->BOF )
		{
			//没有型钢
		}
		else
		{
			rs->MoveFirst();
			while(!rs->adoEOF)
			{
				frmStatus.m_Label1= GetResStr(IDS_SumPhsBomSS);
				strTmp.Format(GetResStr(IDS_SumPhsBomRec),ltos(iSum),ltos(iTmp));
				frmStatus.m_Label2=strTmp;
				frmStatus.UpdateStatus ( (float)iTmp/iSum, true);
				frmStatus.UpdateData(false);
				iTmp+=1;
				
				SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZA] + _T("] WHERE VolumeID=") + vtos(GetFields(rs,_T("VolumeID"))) + _T(" AND zdjh=") + vtos(GetFields(rs,_T("zdjh"))) + _T(" AND NOT bNotSumSA");
//				rsza.m_pDatabase = &EDIBgbl::dbPRJDB;rsza.Open(dbOpenDynaset,SQLx,dbReadOnly);
				rsza->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
					adOpenDynamic, adLockReadOnly, adCmdText); 
				if( rsza->adoEOF && rsza->BOF )
				{
				}
				else
				{
					rsza->MoveFirst();
					
					if(this->rsID->Find((_bstr_t) (_T("trim(CustomID)=\'") + vtos(GetFields(rs,_T("CustomID"))) + _T("\'")), 0, adSearchForward, vTmp))
					{
						//型钢(自2001.12.31起钢板PS类别从型钢(index=3)改为附件(index=4)
						if( (vtoi(GetFields(this->rsID,_T("Index")))==iSectionSteel) )
						{
							rsza->get_Collect((_variant_t)_T("Num"),v);
							m_iNum=vtoi(v);
							rsza->get_Collect((_variant_t)_T("Gnum"),v);
							m_iSANum=vtoi(v);
							rsza->get_Collect((_variant_t)_T("bNotSumSA"),v);
							m_bNotSumSA=vtob(v);
							rs->get_Collect((_variant_t)_T("CLnum"),v);
							m_iCLNum=vtoi(v);
							rs->get_Collect((_variant_t)_T("bUserAdd"),v);
							m_bUserAdd=vtob(v);
							rs->get_Collect((_variant_t)_T("L1"),v);
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
							rsCL->AddNew();
							rsCL->put_Collect((_variant_t)_T("seq"),nil);
							rs->get_Collect((_variant_t)_T("zdjh"),v);
							rsCL->put_Collect((_variant_t)_T("zdjh"),v);
							rs->get_Collect((_variant_t)_T("VolumeID"),v);
							rsCL->put_Collect((_variant_t)_T("VolumeID"),v);
							rs->get_Collect((_variant_t)_T("CLID"),v);
							rsCL->put_Collect((_variant_t)_T("CLID"),v);
							rs->get_Collect((_variant_t)_T("CLmc"),v);
							rsCL->put_Collect((_variant_t)_T("CLmc"),v);
							
							rsCL->put_Collect((_variant_t)_T("CLgg"),STR_VAR(vtos(GetFields(rs,_T("CLgg"))) + _T(" L=") + ltos(m_iL1)));
							rsCL->put_Collect((_variant_t)_T("CLcl"),GetFields(rs,_T("CLcl")));
							rsCL->put_Collect((_variant_t)_T("CLdw"),((vTmp=GetFields(rs,_T("Cldw"))).vt==VT_NULL ? STR_VAR(_T("件")) : vTmp));
							
							rsCL->put_Collect((_variant_t)_T("CLdz"),COleVariant(vtof(GetFields(rs,_T("CLdz"))) * m_iL1/1000.0));
							rsCL->put_Collect((_variant_t)_T("CLnum"),COleVariant(sTmp));
							rsCL->put_Collect((_variant_t)_T("CLzz"),COleVariant(sngSumWeight));
							rsCL->put_Collect((_variant_t)_T("CLbz"),GetFields(rs,_T("CLbz")));
							rsCL->Update();
							
						}
					}
					
				}
				rsza->Close();
				rs->MoveNext();
			}
			//立刻将型钢从TCL分类汇总到Tmp2
			if( EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("TmpCLgroup")) )
			{
				//表TmpCLgroup用于保存分组结果
				EDIBgbl::dbPRJ->Execute((_bstr_t)_T("DROP TABLE TmpCLgroup"), NULL, adCmdText);
			}
			//rsCL->Close();
			//从tmp2生成一个分组结果TmpCLgroup
			//GROUP BY子句的字段必须包含SELECT字句的全部字段(除了聚合函数的字段)
			SQLx=_T("SELECT ") + sGROUPBY + _T(",sum(CLnum) as CLnums,CLnums*CLdz as CLzz INTO TmpCLgroup   FROM TmpTCL ");
			SQLx +=_T(" GROUP BY ") + sGROUPBY;
			EDIBgbl::dbPRJ->Execute((_bstr_t)SQLx, NULL, adCmdText);
			//拷贝tmpCLgroup到tmp2,以便保存分组数据
			//EDIBgbl::dbPRJ->Execute((_bstr_t)_T("INSERT INTO Tmp2 SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy
			//将型钢的内容拷贝到TmpSS表
			EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("INSERT INTO TmpSS SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy), NULL, adCmdText);
			//将临时表的内容拷贝到正式材料表TCL
			//EDIBgbl::dbPRJ->Execute((_bstr_t)_T("INSERT INTO [") + EDIBgbl::TBNSelPrjSpec + EDIBgbl::Btype[EDIBgbl::TCL] + _T("] IN \'") + EDIBgbl::dbPRJDB->DefaultDatabase + _T("\' SELECT * FROM TmpTCL")
			//删除临时表
			RsDeleteAll(rsCL);
			//更新临时材料表数据记对象
		}
		rs->Close();
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
//			rs.m_pDatabase = &EDIBgbl::dbPRJDB;rs.Open(dbOpenDynaset,SQLx);
			rs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 
			if( rs->adoEOF && rs->BOF )
			{
			}
			else
			{
				while(!rs->adoEOF)
				{
					frmStatus.m_Label1= GetResStr(IDS_SumPhsBomSA);
					strTmp.Format(GetResStr(IDS_SumPhsBomRec),ltos(iSum),ltos(iTmp));
					frmStatus.m_Label2=strTmp;
					frmStatus.UpdateStatus ( (float)iTmp/iSum, true);
					frmStatus.UpdateData(false);
					iTmp+=1;
					
					SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZA] + _T("] WHERE VolumeID=") + vtos(GetFields(rs,_T("VolumeID"))) + _T(" AND zdjh=") + vtos(GetFields(rs,_T("zdjh"))) + _T(" AND NOT bNotSumSA");
//					rsza.m_pDatabase = &EDIBgbl::dbPRJDB;rsza.Open(dbOpenDynaset,SQLx,dbReadOnly);
					rsza->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
						adOpenDynamic, adLockReadOnly, adCmdText); 
					if( rsza->adoEOF && rsza->BOF )
					{
					}
					else
					{
						rsza->MoveFirst();
						rsCL->AddNew();
						rsCL->put_Collect((_variant_t)_T("seq"),nil);
						rs->get_Collect((_variant_t)_T("zdjh"),v);
						rsCL->put_Collect((_variant_t)_T("zdjh"),v);
						rs->get_Collect((_variant_t)_T("VolumeID"),v);
						rsCL->put_Collect((_variant_t)_T("VolumeID"),v);
						rs->get_Collect((_variant_t)_T("CLID"),v);
						rsCL->put_Collect((_variant_t)_T("CLID"),v);
						rs->get_Collect((_variant_t)_T("CLmc"),v);
						rsCL->put_Collect((_variant_t)_T("CLmc"),v);
						rs->get_Collect((_variant_t)_T("CLgg"),v);
						rsCL->put_Collect((_variant_t)_T("CLgg"),v);
						rs->get_Collect((_variant_t)_T("CLcl"),v);
						rsCL->put_Collect((_variant_t)_T("CLcl"),v);
						CString str;
						str.LoadString(IDS_PIECE);
						rsCL->put_Collect((_variant_t)_T("CLdw"),((vTmp=GetFields(rs,_T("Cldw"))).vt==VT_NULL ? STR_VAR(str) : vTmp));
						rsCL->put_Collect((_variant_t)_T("CLdz"),GetFields(rs,_T("CLdz")));//, 0, rs.Fields(_T("CLdz"))), _T("0.##"))
						rsCL->put_Collect((_variant_t)_T("CLnum"),COleVariant(vtof(GetFields(rsza,_T("Gnum"))) * vtof(GetFields(rsza,_T("num")))));
						rsCL->put_Collect((_variant_t)_T("CLzz"),GetFields(rs,_T("CLzz")));
						rsCL->put_Collect((_variant_t)_T("CLbz"),GetFields(rs,_T("CLbz")));
						rsCL->Update();
					}
					rsza->Close();
					rs->MoveNext();
				}
				//立刻将根部汇总从TCL分类汇总到TmpSA
				if( EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("TmpCLgroup")) )
				{
					//表TmpCLgroup用于保存分组结果
					EDIBgbl::dbPRJ->Execute((_bstr_t)_T("DROP TABLE TmpCLgroup"), NULL, adCmdText);
				}
				//rsCL->Close();
				//从tmpCL生成一个分组结果TmpCLgroup
				//GROUP BY子句的字段必须包含SELECT字句的全部字段(除了聚合函数的字段)
				SQLx=_T("SELECT ") + sGROUPBY + _T(",sum(CLnum) as CLnums,CLnums*CLdz as CLzz INTO TmpCLgroup   FROM TmpTCL ");
				SQLx +=_T(" GROUP BY ") + sGROUPBY;
				EDIBgbl::dbPRJ->Execute((_bstr_t)SQLx, NULL, adCmdText);
				//将根部汇总的内容拷贝到TmpSA表
				EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("INSERT INTO TmpSA SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy), NULL, adCmdText);
				//将临时表的内容拷贝到正式材料表TCL
				//根部整体零件表不能汇总到材料表中
				//删除临时表
				RsDeleteAll(rsCL);
				//更新临时材料表数据记对象
				//rsCL.Open(dbOpenDynaset,_T("SELECT * FROM TmpTCL") );
			}
		}
		rs->Close();
		
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
//		rs.m_pDatabase = &EDIBgbl::dbPRJDB;rs.Open(dbOpenDynaset,SQLx);
		rs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		if( rs->adoEOF && rs->BOF )
		{
		}
		else
		{
			while(!rs->adoEOF)
			{
				frmStatus.m_Label1= GetResStr(IDS_SumPhsBomBoltsNuts);
				strTmp.Format(GetResStr(IDS_SumPhsBomRec),ltos(iSum),ltos(iTmp));
				frmStatus.m_Label2=strTmp;
				frmStatus.UpdateStatus ( (float)iTmp/iSum, true);
				frmStatus.UpdateData(false);
				iTmp+=1;
				
				SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZA] + _T("] WHERE VolumeID=") + vtos(GetFields(rs,_T("VolumeID"))) + _T(" AND zdjh=") + vtos(GetFields(rs,_T("zdjh")));
//				rsza.m_pDatabase = &EDIBgbl::dbPRJDB;rsza.Open(dbOpenDynaset,SQLx,dbReadOnly);
				rsza->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
					adOpenDynamic, adLockReadOnly, adCmdText); 
				if( rsza->adoEOF && rsza->BOF )
				{
				}
				else
				{
					rsza->MoveFirst();
					rsza->get_Collect((_variant_t)_T("bNotSumSA"),v);
					m_bNotSumSA=vtob(v);
					rs->get_Collect((_variant_t)_T("bUserAdd"),v);
					m_bUserAdd=vtob(v);					
					rs->get_Collect((_variant_t)_T("recno"),v);
					
					if(!( !(v.vt==VT_NULL) ||  (v.vt==VT_NULL) && (m_bUserAdd || !m_bUserAdd && !m_bUserAdd) ))
					{
						//空的记录号，且不是用户添加的材料，不统计根部
						//不需要统计的根部螺栓螺母
					}else
					{
						rsza->get_Collect((_variant_t)_T("Num"),v);
						m_iNum=vtoi(v);
						rsza->get_Collect((_variant_t)_T("Gnum"),v);
						m_iSANum=vtoi(v);
						rsza->get_Collect((_variant_t)_T("bNotSumSA"),v);
						m_bNotSumSA=vtob(v);
						rs->get_Collect((_variant_t)_T("CLnum"),v);
						m_iCLNum=vtoi(v);
						rs->get_Collect((_variant_t)_T("bUserAdd"),v);
						m_bUserAdd=vtob(v);
						rs->get_Collect((_variant_t)_T("L1"),v);
						m_iL1=vtoi(v);
						sTmp=m_iNum*m_iCLNum;
						rsCL->AddNew();
						rsCL->put_Collect((_variant_t)_T("seq"),nil);
						rs->get_Collect((_variant_t)_T("zdjh"),v);
						rsCL->put_Collect((_variant_t)_T("zdjh"),v);
						rs->get_Collect((_variant_t)_T("VolumeID"),v);
						rsCL->put_Collect((_variant_t)_T("VolumeID"),v);
						rs->get_Collect((_variant_t)_T("CLID"),v);
						rsCL->put_Collect((_variant_t)_T("CLID"),v);
						rs->get_Collect((_variant_t)_T("CLmc"),v);
						rsCL->put_Collect((_variant_t)_T("CLmc"),v);
						rs->get_Collect((_variant_t)_T("CLgg"),v);
						rsCL->put_Collect((_variant_t)_T("CLgg"),v);
						rs->get_Collect((_variant_t)_T("CLcl"),v);
						rsCL->put_Collect((_variant_t)_T("CLcl"),v);
						CString str;
						str.LoadString(IDS_PIECE);
						rsCL->put_Collect((_variant_t)_T("CLdw"),((vTmp=GetFields(rs,_T("Cldw"))).vt==VT_NULL ? STR_VAR(str) : vTmp));
						rsCL->put_Collect((_variant_t)_T("CLdz"),GetFields(rs,_T("CLdz")));
						rsCL->put_Collect((_variant_t)_T("CLnum"),COleVariant(sTmp));
						rsCL->put_Collect((_variant_t)_T("CLzz"),GetFields(rs,_T("CLzz")));
						rsCL->put_Collect((_variant_t)_T("CLbz"),GetFields(rs,_T("CLbz")));
						rsCL->Update();
					}
				}
				rsza->Close();
				rs->MoveNext();
			}
		}
		rs->Close();
		//立刻将螺栓螺母从TCL分类汇总到Tmp2
		if( EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T("TmpCLgroup")) )
		{
			//表TmpCLgroup用于保存分组结果
			EDIBgbl::dbPRJ->Execute((_bstr_t)_T("DROP TABLE TmpCLgroup"), NULL, adCmdText);
		}
		//rsCL->Close();
		//从tmp2生成一个分组结果TmpCLgroup
		//GROUP BY子句的字段必须包含SELECT字句的全部字段(除了聚合函数的字段)
		SQLx=_T("SELECT ") + sGROUPBY + _T(",sum(CLnum) as CLnums,CLnums*CLdz as CLzz INTO TmpCLgroup   FROM TmpTCL ");
		SQLx +=_T(" GROUP BY ") + sGROUPBY;
		EDIBgbl::dbPRJ->Execute((_bstr_t)SQLx, NULL, adCmdText);
		//拷贝tmpCLgroup到tmp2,以便保存分组数据
		EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("INSERT INTO Tmp2 SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy), NULL, adCmdText);
		//将螺栓螺母的内容拷贝到TmpBoltsNuts表
		EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("INSERT INTO TmpBoltsNuts SELECT ") + sGROUPBY + _T(", CLnums as CLnum, CLzz FROM tmpCLgroup ORDER BY ") + strOrderBy), NULL, adCmdText);
		//将临时材料表的内容拷贝到正式材料表TCL
		EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("INSERT INTO [") + EDIBgbl::Btype[EDIBgbl::TCL] + _T("] IN \'") + (LPTSTR)(LPCTSTR)EDIBgbl::dbPRJDB->DefaultDatabase + _T("\' SELECT * FROM TmpTCL")), NULL, adCmdText);
		//删除临时表
		EDIBgbl::dbPRJ->Execute((_bstr_t)_T("DELETE * FROM TmpTCL"), NULL, adCmdText);
		//更新临时材料表数据记对象
		//rsCL.Open(dbOpenDynaset,_T("SELECT * FROM TmpTCL") );
	}
	catch(CException *e)
	{
		e->Delete();
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
	_RecordsetPtr rs;
	rs.CreateInstance(__uuidof(Recordset));

	_RecordsetPtr rsTmpZB;
	rsTmpZB.CreateInstance(__uuidof(Recordset));
	CString tbn,tmpID,tmpBlkID,SQLx;
	CString tmpStr;
	try
	{
		SQLx = _T("SELECT * FROM phsBlkDimPos");
		rs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPHScode,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		//所有可绘制零件recno<>Null
		SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] WHERE zdjh=") + ltos(modPHScal::zdjh) + _T(" AND VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" AND IsSAPart<>-1 ORDER BY recno");
		rsTmpZB->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		if(rsTmpZB->adoEOF && rsTmpZB->BOF)
		{
			tmpStr.Format(GetResStr(IDS_NoRecordInTZB),EDIBgbl::dbPRJDB->DefaultDatabase,EDIBgbl::Btype[EDIBgbl::TZB], EDIBgbl::SelJcdm,ltos(modPHScal::zdjh));
			throw tmpStr;
		}
		COleVariant vTmp;
		while(!rsTmpZB->adoEOF)
		{
			//展开sFindTBN以加快速度
			//tbn = sFindTBN(rsTmpZB.Fields(_T("CustomID")))
			rsTmpZB->get_Collect((_variant_t)_T("CustomID"), &vTmp);
			
			if(rsID->Find((_bstr_t)(_T("trim(CustomID)=\'") +vtos(vTmp) + _T("\'")), 0, adSearchForward, vTmp))
			{
				rsID->get_Collect((_variant_t)_T("Index"), &vTmp);
				modPHScal::glIDIndex = vtoi(vTmp);
				rsID->get_Collect((_variant_t)_T("ClassID"), &vTmp);
				modPHScal::glClassID=vtoi(vTmp);
			}
			if(modPHScal::glIDIndex == iPA)
				modPHScal::glPAid = modPHScal::glClassID;
			rsTmpZB->get_Collect((_variant_t)_T("ID"), &vTmp);
			if(vtos(vTmp)==_T(""))
			{
				//对于象华东院标准的附件，如果查不到ID,则查CustomID,然后查得其ID.
				rsTmpZB->get_Collect((_variant_t)_T("CustomID"), &vTmp);
				tmpID = modPHScal::sFindID(vtos(vTmp));
//				rsTmpZB.Edit();
				rsTmpZB->put_Collect((_variant_t)_T("ID"),STR_VAR(tmpID));
				rsTmpZB->Update();
			}
			else
			{
				rsTmpZB->get_Collect((_variant_t)_T("ID"), &vTmp);
				tmpID = vtos(vTmp);
			}
			tmpBlkID =tmpID;
			//首先看表中是否存在ID名称代表的块
			rs->Find((_bstr_t) (_T("trim(blkID)=\'") + tmpBlkID + _T("\'")), 0, adSearchForward, vTmp);
			VARIANT_BOOL bm=VARIANT_FALSE;
			bm= rs->adoEOF;
			if( modPHScal::glIDIndex == iSA)
			{
				//是根部
				if(!bm)
				{
					//如果没有,则肯定是槽钢组成的根部，加上槽钢数量构成新的块,继续查找
					if( modPHScal::glClassID != iGCement)
						tmpBlkID = tmpBlkID +vtos( rsza->GetCollect(_T("iCSnum")));
					rs->Find((_bstr_t) (_T("trim(blkID)=\'") + tmpBlkID + _T("\'")), 0, adSearchForward, vTmp);
					bm=rs->adoEOF;
				}
			}
//			rsTmpZB.Edit();
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
					rs->get_Collect((_variant_t)_T("Zh"), &vTmp);
					rsTmpZB->put_Collect((_variant_t)_T("Crd"),vTmp);
				}
				else
				{
					rs->get_Collect((_variant_t)_T("Dh"), &vTmp);
					rsTmpZB->put_Collect((_variant_t)_T("Crd"),vTmp);
				}
			}
			rsTmpZB->put_Collect((_variant_t)_T("blkID"),STR_VAR(tmpBlkID));
			rsTmpZB->Update();
			rsTmpZB->MoveNext();
		}
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
		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(Recordset));
		CString SQLx;
		COleVariant vTmp;
		modPHScal::glPAid = vtoi(modPHScal::sFindFLD(_T("CustomID"), _T("ClassID"), modPHScal::dn));
		if(! modPHScal::bPAIsHanger())
		{
			//支架
			//If glPAid = iPAs Or glPAid = iPAfixZ1 Or glPAid = iPAfixZ2 Then
			//排除根部(index=iSA)及其附件(记录号为空)的所有零件的高度和
			SQLx = _T("SELECT sum(sizeH) as sumH FROM [") + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] WHERE zdjh=") + ltos(modPHScal::zdjh) + _T(" AND VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" AND IsSAPart<>-1 AND index<>") + ltos(iSA);
			rs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 
			if(!rs->adoEOF && ! rs->BOF)
			{
				//MsgBox rs.RecordCount
				rs->get_Collect((_variant_t)_T("sumH"), &vTmp);
				modPHScal::sngSEL =modPHScal::sngPEL- (vtof(vTmp)/ 1000);
				modPHScal::UpdateTZB4zdjh(_T("gh1"), _variant_t(modPHScal::sngSEL));
			}
		}
		
		//管部,根部偏装位置值的计算
		if( giUPxyz == 1)
		{
			//X轴向上

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
	catch(CException *e)
	{
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
		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(Recordset));
//		rs.m_pDatabase=&modPHScal::dbZDJcrude;
		sTmp.Format(_T("%d"),(Wx1>Wx ? Wx1 : Wx));
		sSQLx = _T("SELECT * FROM [SSteelPropertyCS] WHERE Wx>=");
		sSQLx+=sTmp;
		sSQLx+= _T(" ORDER BY ID");
		rs->Open((_bstr_t)sSQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
		if(rs->adoEOF && rs->BOF)
		{
			sTmp.Format(GetResStr(IDS_NotFoundAnyBHinXSteelProperty),_T("SSteelPropertyCS"));
			throw sTmp;		
		}
		else
		{
			rs->get_Collect((_variant_t)_T("BH"),v);
			ret=vtos(v);
			sTmp2.Format(GetResStr(IDS_SAMadeBySSteel),_T("\%d"),ret,SACustomID);
			sTmp.Format(sTmp2,iDCS);
			throw sTmp;
		}
		rs->Close();
		return ret;
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
		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(Recordset));
		_Recordset* rsza = FrmTxsr.m_pViewTxsr->m_ActiveRs;
		CString SQLx,sTmp,sTmp2;
		COleVariant v;
		if(modPHScal::iSelSampleID > 0)
		{
			sTmp.Format(_T("%d"),SampleID);
			SQLx = _T("SELECT * FROM phsStructureName WHERE SampleID=") + sTmp;
			rs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPHScode,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 
			if (rs->adoEOF && rs->BOF)
			{
				sTmp2.Format(GetResStr(IDS_NotFoundSampleIDSprNum),_T("\%d"),_T("\%d"));
				sTmp.Format(sTmp2,modPHScal::iSelSampleID,modPHScal::giWholeSprNum);
				ShowMessage(sTmp);
				ret = -1;
			}
			else if(!rs->adoEOF)
			{
				rs->MoveLast();
				rs->get_Collect((_variant_t)_T("SampleID"),v);
				ret=vtoi(v);
				rs->get_Collect((_variant_t)_T("PA"),v);
				//管部属性赋值
				PA=vtos(v);
				rsza->PutCollect(_T("IDdn1"),stov(PA));
				rs->get_Collect((_variant_t)_T("SA"),v);
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
	return ret;
}



Cphs::Cphs()
{
	iAttachIncd=iAttachIncluded;
	iChkCondition=0;
    iSAPAConnect=-1;
	SACSHeight=0;
	rsTZC=NULL;
//	rsphsStructureREF.m_pDatabase=&EDIBgbl::dbSORT;
	TZBResultObj=NULL;
	m_rsObj=NULL;
	rsID.CreateInstance(__uuidof(_Recordset));
	m_rsObj.CreateInstance(__uuidof(_Recordset));
}

Cphs::~Cphs()
{
	if(rsID->State == adOpenStatic)
		rsID->Close();
}

void Cphs::InitListRs()
{
	//打开连接表rsConnect和rsUnCheckedType
	try
	{
		CString sSQL;
		if(rsConnect->State != adOpenStatic)
		{
//			rsConnect.m_pDatabase=&EDIBgbl::dbPRJ;
			sSQL=(_T("SELECT * FROM connect "));
//			rsConnect.Open(dbOpenDynaset,sSQL);
			rsConnect->Open((_bstr_t)sSQL,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 
		}
		else
			rsConnect->Requery(adExecuteRecord);
		if(rsUnCheckedType->State != adOpenStatic)
		{
//			rsUnCheckedType.m_pDatabase=&EDIBgbl::dbPRJ;
			sSQL=(_T("SELECT * FROM rsUnCheckedType "));
//			rsUnCheckedType.Open(dbOpenDynaset,sSQL);
			rsUnCheckedType->Open((_bstr_t)sSQL,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 
		}
		else
			rsUnCheckedType->Requery(adExecuteRecord);
	}
	catch(CException *e)
	{
		e->Delete();
	}
	
}

void Cphs::SelectItemRsObj()
{

}

void Cphs::CloseRecordsets()
{
	//关闭rsConnect和rsUnCheckedType
	try
	{
		if(rsConnect->State == adOpenStatic)
			rsConnect->Close();
		if(rsUnCheckedType->State == adOpenStatic)
			rsUnCheckedType->Close();
	}
	catch(CException *e)
	{
		e->Delete();
	}
}

