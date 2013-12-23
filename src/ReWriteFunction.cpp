
#include "stdafx.h"
#include "ReWriteFunction.h"
#include "edibgbl.h"
#include "modPHScal.h"

extern CString vtos(COleVariant& v);
extern int vtoi(COleVariant & v);
extern float vtof(COleVariant &v);
extern float Sgn(float x);
/*
开发人:陈百炼
时间: 2007.11.08
输入参数 TableName:当前支吊架当前路数零部件表名 Zdjh:支吊加号;SelVlmID：卷册号; nth:路数
输出参数 rsTZB:当前支吊架当前路数零部件  rsTmpZB:当前支吊架当前路数零部件表，不含螺栓螺母、根部附件
*/
BOOL GetPHSCompRs(CString TableName ,long zdjh,long SelVlmID, int nth,CDaoRecordset &rsTZB,CDaoRecordset &rsTmpZB)
{
	
	CString strSQL ;
	
	strSQL.Format(_T("SELECT * FROM %s WHERE zdjh=%d AND VolumeID=%d AND nth =%d ORDER BY recno"),
		TableName,zdjh,SelVlmID,nth);
	rsTZB.m_pDatabase=&EDIBgbl::dbPRJDB;
	rsTZB.Open(dbOpenDynaset,strSQL);
	
	strSQL.Format(_T("SELECT CustomID FROM ZB WHERE [VolumeID]=%d AND [ZDJH]=%d AND [nth]=%d AND [ClassID]<>%d AND [ClassID]<>%d AND [ClassID]<>%d AND [IsSAPart]<>-1 Order By recno"),
		SelVlmID,zdjh,nth,iBolts,iNuts,iAttached);
	rsTmpZB.Open(dbOpenDynaset,strSQL);
	if( rsTmpZB.IsEOF()&& rsTmpZB.IsBOF() )
		return FALSE;
	return TRUE;
}

/*
开发人:陈百炼
时间: 2007.11.09
用实际的数替换匹配式
tmpSQL0:需要替换的字符;rsDiaOfCSPRFiK:??;rsX:??
*/
void ReplaceCNTB(CString &tmpSQL0,CDaoRecordset &rsDiaOfCSPRFiK,CDaoRecordset &rsX )
{

	CString tmpFD0;
	bool bFound = false;  
	int n = 0,m = 0,k = 0;
	COleVariant vTmp1;
	CString LogicalOpr = _T("=<>") ;  //逻辑操作定界符
	if( tmpSQL0 != _T("") )
	{//查找连接表达式,首先去掉其中的Cnte.字符
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
}


//设置前一个零件的插入长度
void SetPrePartInsertLen(int i,CString* pstrPartID,CString mviPreviousClassID,CDaoRecordset &rsConnect,CDaoRecordset &rsX,CDaoRecordset &rsTZB,CDaoRecordset &rsDiaOfCSPRFiK,CString &tmpExtLenFD)
{
	COleVariant vTmp2,vTmp1;
	rsConnect.GetFieldValue(_T("CNTBExtLenFLD"),vTmp1);
	if( vTmp1.vt==VT_NULL || vtos(vTmp1) == _T("") )
	{
		if( pstrPartID[i]==_T("L7") || pstrPartID[i]==_T("G12") || pstrPartID[i]==_T("L7Dd") || pstrPartID[i]==_T("G12Lc") )
		{
			//这两种耳子可连接在L8上面,因此要特殊处理，才能查到正确的插入L8的尺寸。
			if( pstrPartID[i-1] == _T("L8") || pstrPartID[i-1] == _T("L8Dd") )
			{
				//前一个是L8
				//先查数据
				//以后在计算拉杆长度函数CalLugLength中只要处理这种特殊情况即可。
				rsX.GetFieldValue(tmpExtLenFD,vTmp2);
				rsTZB.SetFieldValue(_T("PreviousPartInsertLen"),COleVariant((long)vtoi(vTmp2)));
			}
			////////////////////////////////////////////
			if( pstrPartID[i+1] == _T("L8") || pstrPartID[i+1] == _T("L8Dd"))
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
		if( pstrPartID[i].Left(2) == _T("L7")||pstrPartID[i].Left(2) == _T("G12")||pstrPartID[i] == _T("L3")||pstrPartID[i] == _T("L4")||pstrPartID[i] == _T("T1")||pstrPartID[i] == _T("D4")||pstrPartID[i] == _T("D5")||pstrPartID[i] == _T("D11"))
		{
			rsConnect.GetFieldValue(_T("CNTBExtLenFLD"),vTmp2);
			tmpExtLenFD = vtos(vTmp2);
			
			//因为管部不可能有前一个，所以只需要判断它的后一个
			if(pstrPartID[i] != _T("D4") && pstrPartID[i] != _T("D5") && pstrPartID[i] != _T("D11"))
			{
				//这两种耳子可连接在L8上面,因此要特殊处理，才能查到正确的插入L8的尺寸。
				if( pstrPartID[i-1] == _T("L8") || pstrPartID[i-1] == _T("L8Dd") )
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
						if( pstrPartID[i-1] == _T("PHE1") || pstrPartID[i-1] == _T("PHE") || pstrPartID[i-1] == _T("LHE") || pstrPartID[i-1] == _T("LHE1") )
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
						rsTZB.SetFieldValue(_T("PreviousPartInsertLen"),COleVariant((long)0));
					}
				}
			}
			//////////////////////////////////////////////////////////////////////
			if( pstrPartID[i+1] == _T("L8") || pstrPartID[i+1] == _T("L8Dd") )
			{
				rsX.GetFieldValue(tmpExtLenFD,vTmp2);
				rsTZB.SetFieldValue(_T("PreviousPartInsertLen"),COleVariant((long)vtoi(vTmp2)));
			}
			//////////////////////////////////////////////////////////////////////////
		}
		else
		{
			if( pstrPartID[i] == _T("L8") || pstrPartID[i] == _T("L8Dd") )
			{
				//当前不是L7、G12,是L8
				if( pstrPartID[i+1] == _T("L7") || pstrPartID[i+1] == _T("G12") || pstrPartID[i+1] == _T("L7Dd") || pstrPartID[i+1] == _T("G12Lc") )
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
					rsTZB.SetFieldValue(_T("PreviousPartInsertLen"),COleVariant((long)0));
				}
			}
			else
			{
				//当前不是L7、G12,不是L8
				if( pstrPartID[i] == _T("L5") || pstrPartID[i] == _T("L6") )
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
						if( pstrPartID[i-1] == _T("PHE1") || pstrPartID[i-1] == _T("PHE") || pstrPartID[i-1] == _T("LHE") || pstrPartID[i-1] == _T("LHE1") )
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
						rsTZB.SetFieldValue(_T("PreviousPartInsertLen"),COleVariant((long)0));
					}
				}
			}
		}
	}
}

//得到部件的实际高度和长度
double GetPartSizeH(int i,CDaoRecordset &rsX,const CString& strPartID,const CString*PtypeID)
{
	double dbSizeH = 0;
	COleVariant vTmp1,vTmp2;
	if( strPartID == _T("LS") || strPartID == _T("LT") )
	{
		//矩形管道，高度值要加modPHScal::dwB/2
		//零件标注尺寸，先将sizeDIM也暂时置为零件实际尺寸
		rsX.GetFieldValue( _T("sizeH"),vTmp1 );
		dbSizeH = -(vtof(vTmp1) + modPHScal::dwB / 2);
	}
	else if( strPartID == _T("D4LQ") )
	{
		//矩形管道，高度值要加modPHScal::dwB/2
		//零件标注尺寸，先将sizeDIM也暂时置为零件实际尺寸
		rsX.GetFieldValue(_T("sizeH"),vTmp1);
		dbSizeH = vtof(vTmp1) + modPHScal::dwB / 2;
		}
	else if( strPartID == _T("DLR") )
	{
		//矩形管道，高度值要加modPHScal::dwB/2
		//零件标注尺寸，先将sizeDIM也暂时置为零件实际尺寸
		rsX.GetFieldValue(_T("sizeH"),vTmp1);
		rsX.GetFieldValue(_T("size4"),vTmp2);
		dbSizeH = vtof(vTmp1)+ modPHScal::dwB / 2 - vtof(vTmp2);
	}
	else if( strPartID == _T("LM2") || strPartID == _T("LM3") || strPartID == _T("LM4") )
	{
		//水平圆形管道连环吊架，高度值要加槽钢横担高度
		//零件标注尺寸，先将sizeDIM也暂时置为零件实际尺寸
		//rsTZB.SetFieldValue(_T("sizeDIM")) = -(IIf(IsNull(rsX.Fields(_T("size5"))), 0, rsX.Fields(_T("size5"))) + rsX.Fields(_T("size7")) * 10)
		rsX.GetFieldValue(_T("sizeH"),vTmp1);
		dbSizeH = vtof(vTmp1);
	}
	else if( strPartID == _T("LN2") || strPartID == _T("LN3") )
	{
		//水平圆形管道槽钢焊接连环吊架，高度值要加
		//零件标注尺寸，先将sizeDIM也暂时置为零件实际尺寸
		rsX.GetFieldValue(_T("sizeH"),vTmp1);
		dbSizeH = vtof(vTmp1);
	}
	else if(strPartID==_T("L15"))
	{
		//西北院固定支架Z2/Z2A下垫一块钢板L15,其厚度值保存在tbnAttachement表字段size2中或modPHScal::gfPSThickness
		//零件标注尺寸，先将sizeDIM也暂时置为零件实际尺寸
		if(PtypeID[i+1] == _T("GZD"))
		{
			dbSizeH = 0.0;
			//混凝土支墩，钢板埋入其中
			//零件实际尺寸
		}
		else
		{
			dbSizeH = modPHScal::gfPSThickness;
			//零件实际尺寸
		}
	}
	else
	{
		if(modPHScal::glIDIndex==iSA)
		{
			dbSizeH = 0;
			//根部高度置为0,以便正确统计拉杆长度
			//零件实际尺寸
		}
		else
		{
			//零件标注尺寸，先将sizeDIM也暂时置为零件实际尺寸
			rsX.GetFieldValue(_T("sizeH"),vTmp1);
			dbSizeH = vtof(vTmp1);
			//零件实际尺寸
		}
	}
	return dbSizeH;
}

//得到恒力弹簧的安装高度
double GetSngDim(float sngH1xmax,float Height_SPRINGsL5,int iSEQofSPR,CString PType,CString sPartID,CDaoRecordset &rsX,CDaoRecordset &rsTmp,CDaoRecordset &rsDiaOfCSPRFiK) 
{
	//如果恒力弹簧的选型位移modPHScal::sSprInfo[iSEQofSPR].SumDisp正好等于下限位移字段fBmin的一个值，
	//则恒力弹簧的安装高度H1(sngH1xmax)=直接查得这个下限位移字段fBmin的值。
	//否则，按插值法计算位移。add by ligb on 2004.11.24
	CString sTmp;
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
	tmpSQL+=(modPHScal::gbCSPRneedSpecialDesign ? _T(" ") : sTmp) + _T(" AND trim(CustomID)=\'") + PType + _T("\' ORDER BY dh,Weight");
	rsX1.m_pDatabase=&modPHScal::dbZDJcrude;
	rsX1.Open(dbOpenSnapshot,tmpSQL);
	double sngDim = 0;
	COleVariant vTmp1,vTmp2;
	if(rsX1.IsBOF() && rsX1.IsEOF()||1)//当处在临界值时出错(sizeH的高度没有加上实际位移)pfg and lgb 2005.12.15
	{
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
		if(sPartID==_T("LHE") || sPartID==_T("LHE1") || sPartID==_T("PHE") || sPartID==_T("PHE1") || sPartID==_T("PHC") || sPartID==_T("LHC"))
		{
			//立式吊杆连接，要加上耳板高度T。
			if( !modPHScal::gbCH1IncludeT )
			{
				rsDiaOfCSPRFiK.GetFieldValue(_T("T"),vTmp1);
				sngDim = sngDim +vtof(vTmp1);
			}
		}
		else
		{
		}
	}
	rsX1.Close();
	return sngDim;
}
