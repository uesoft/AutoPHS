// CSPRSel.cpp: implementation of the CSPRSel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CSPRSel.h"
#include "modPHScal.h"
#include "EDIBgbl.h"
//#include "TmpCSLenTable.h"
#include "ConstantSpring.h"
//#include "ConstantSpringTable.h"
#include "HDForceDist.h"
#include "HDCrude.h"
#include "LugDiaOfCSPR.h"
#include "Connect.h"
#include "TmpCSLen.h"
#include "ResultZB.h"
#include "PictureClipData.h"
#include "CrudeTurnbuckleHDCz.h"
#include "user.h"
#include "EDIBAcad.h"
#include <math.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSPRSel::CSPRSel()
{
	
}

CSPRSel::~CSPRSel()
{

}

BOOL CSPRSel::SelCSPRToTable(float& TmpPaz,float& tmpSelPJG4CSPRFiJ,long &mlfx,long &mlfx0)
{
	if(m_iSEQofSPR < modPHScal::giWholeSprNum)
			modPHScal::giCSPRindex[m_iSEQofSPR] = m_iSEQofPart;
    //复位特殊设计标志
//	TmpCSLenTable::SetSpecialDesign("");
	double TmpPgz = ConstantSpring::GetPGZ(m_dbSelPJG4CSPRPgz,m_strPartID,m_iFirstCal);
	TmpPaz = TmpPgz;

	//得到用来选型的位移
	double Tmpyr =  ConstantSpring::Getyr(modPHScal::yr);       //yr 单位:mm
	double tmpDist = Tmpyr / modPHScal::giWholeSprNum;

	//查位移系列值，按就近上靠原则确定位移系列。

//	tmpDist=ConstantSpringTable::GetDispSerial(tmpDist);

	//载荷容量单位为kN*mm,即N*m,位移单位为mm,荷载单位为kgf(或daN=10N)	
	double	tmpCapacity = TmpPgz * tmpDist * EDIBgbl::kgf2N / 1000;

	HDForceDist mDist;
	GetHDForceDist(&mDist,tmpCapacity,tmpDist,m_iSumSerialNum); //在弹簧特性表中找到合适的弹簧
	
	//给恒吊赋值
	SetCSPRValue(mDist.GetDH(),tmpDist);

	double tmpSelOpLoad = 0; //选择的工作荷载，单位为kgf
	tmpSelOpLoad = mDist.GetCapacity()/ tmpDist * 1000 / EDIBgbl::kgf2N;

	//在弹簧安装数据表中得到合适的恒力弹簧
	HDCrude mHDCrude;
	GetHDCrude(&mHDCrude); 
	
	int iFjmin = mHDCrude.GetFjmin();
	if( m_iFirstCal >= 2 )
	{
		//查恒吊松紧螺母直径FiJ、FiK、S、I、T。
		//常州电力机械厂样本p20 C型恒吊的T值应该该为I值，才与前面的LHE、PHE(1<=DH<=83)的尺寸吻合。
		//编程才方便。
		//按常州电力机械厂样本，拉杆直径既要满足荷载条件，也要满足最小直径条件（安装条件）。
		//第二次计算,记录螺纹直径,以便找出最大值，所有的恒吊松紧螺母约束到这个直径
		if( m_strPartID == _T("LHG") )
		{
			//下方恒力弹簧，需要循环3次才能找出最大拉杆直径,//下方恒吊松紧螺母直径=其后的拉杆直径
			modPHScal::PtypeDiameter[m_i] = modPHScal::PtypeDiameter[m_i + 1];
		}
		else
		{
			////其它恒力弹簧，循环2次才能找出最大拉杆直径,非下方恒吊松紧螺母直径=其前的拉杆直径
			modPHScal::PtypeDiameter[m_i] = modPHScal::PtypeDiameter[m_i - 1];
		}
		iFjmin = modPHScal::PtypeDiameter[m_i];
	}
	
	//得到合适拉杆
	LugDiaOfCSPR mLugFig;
	GetLugDiaOfCSPR( &mLugFig,mHDCrude.GetWeight(),mHDCrude.GetCustomID(),iFjmin );
	
	//设置恒力弹簧之前的部件选型直径
	SetAforeCompDia( mLugFig.GetFiJ() );

	double tmpSelPJG4CSPRPgzNext = 0;
	if( modPHScal::gbAddPartWeight2CSPRPgz )
	{
		//累加重量到恒吊工作荷载   //累加重量到弹簧工作荷载
		//忽略附件、螺栓螺母重量，粗略计算拉杆重量（按1m考虑）
		//恒吊规格计算载荷
		tmpSelPJG4CSPRPgzNext = m_dbSelPJG4CSPRPgz + mHDCrude.GetWeight() * m_iNumPart;
	}
	if( modPHScal::gbAddPartWeight2PMax)
	{
		//累加重量到计算荷载
		//忽略附件、螺栓螺母重量，粗略计算拉杆重量（按1m考虑）
		//恒吊松紧螺母直径计算载荷
		m_dbSelPJG4CSPRFiK = m_dbSelPJG4CSPRFiJ + mHDCrude.GetWeight() * m_iNumPart;
		//其它零件计算载荷
		m_dbtmpSelPJG = m_dbtmpSelPJG + mHDCrude.GetWeight() * m_iNumPart;
	}
	else
	{
		//恒吊松紧螺母直径计算载荷
		m_dbSelPJG4CSPRFiK = m_dbSelPJG4CSPRFiJ;
	}
	tmpSelPJG4CSPRFiJ = m_dbSelPJG4CSPRFiK;

	//得到合适的拉杆(有最小拉杆限制，所以再得一次)
	LugDiaOfCSPR mLugFik;
	GetLugDiaOfCSPR( &mLugFik,mHDCrude.GetWeight(),mHDCrude.GetCustomID(),modPHScal::PtypeDiameter[m_i] );
	modPHScal::sSprInfo[m_iSEQofSPR].FiJ = mLugFik.GetFiJ();
	modPHScal::sSprInfo[m_iSEQofSPR].FiK = mLugFik.GetFiK();

	ReplaceCNTB(&mLugFik);

	//还有插入长度
	
	SetTempCSLen(&mHDCrude,tmpDist,tmpSelOpLoad); //写TempCSLen表格

	SetResultZB(&mHDCrude,&mLugFik,mlfx,mlfx0);
	return TRUE;
}

//得到弹簧特性数据表满足要求的数据集
void CSPRSel::GetHDForceDist(HDForceDist* pDist,float tmpCapacity,float tmpDist,int iSumSerialNum) 
{
	CString SQLx = _T("SELECT * FROM [") + modPHScal::tbnHDproperty + _T("] ORDER BY Capacity");
	pDist->Initialize(SQLx);
	if( pDist->IsNull()  )
	{
		throw "暂时开发";
		//恒吊载荷容量表为空，不可能
	}
	
	float Cmin,Cmax;
	Cmin = pDist->GetMinCapacity();
	Cmax =  pDist->GetMaxCapacity();

	CString sTmp,sTmp2;
	if( tmpCapacity < Cmin ) //荷载容量比最小的还小
	{
		//荷载容量比最小的还小
		sTmp.Format(GetResStr(IDS_CapacityLitterCmin),_T("\%g"),_T("\%g"));
		sTmp2.Format(sTmp,tmpCapacity, Cmin);
		throw sTmp2;
	}
	if( tmpCapacity > Cmax ) //荷载容量比最大的还大
	{
		throw GetResStr(IDS_NotFoundConstantSPRInTBNHDproperty);
	}

	sTmp.Format(_T("%s>=%g"),pDist->GetCapacityField(),tmpCapacity);
	pDist->FindFirst(sTmp);

	if( tmpDist < pDist->GetMinDist() )//比最小位移小
	{
		sTmp.Format(GetResStr(IDS_ShouldUseSpring),_T("\%g"),_T("\%g"));
		sTmp2.Format(sTmp,tmpDist,pDist->GetMinDist() );
		throw sTmp2;
	}

	if( tmpDist > pDist->GetMaxDist() ) //比最大位移大
	{
		if( iSumSerialNum >= modPHScal::giWholeSprNum )
		{
			//超过模板中的串联数,恒吊需要特殊设计（即恒吊不允许串联使用）,
			//只要一个恒吊就可以了,就用这种位移作为订货依据
			modPHScal::gbCSPRneedSpecialDesign = true;
			EDIBgbl::dbPRJ.Execute(GetResStr(IDS_ESPECIAL_DESIGN_SQL));
		}
	}
	
	//比较前一个是否更接近荷载
	double C1=0,C2=0;
	C1 = pDist->GetCapacity();
	pDist->MovePrev();
	if( !pDist->IsBOF() )
	{
		C2 = pDist->GetCapacity();
		if( fabs(C1 - tmpCapacity) < fabs(C2 - tmpCapacity) )
		{
			//载荷容量更接近C1,回到容量接近的记录
			pDist->MoveNext();
		}
	}
	else
	{
		pDist->MoveNext();
	}
}

//给恒吊赋值
void CSPRSel::SetCSPRValue(int iDH,double tmpDist)
{
	if(m_iSEQofSPR<modPHScal::SprInfoIndex)
	{
		modPHScal::sSprInfo[m_iSEQofSPR].SerialNum = 1;
		modPHScal::sSprInfo[m_iSEQofSPR].DH = iDH;
		modPHScal::sSprInfo[m_iSEQofSPR].haz = fabs(modPHScal::yr) / modPHScal::giWholeSprNum * modPHScal::sSprInfo[m_iSEQofSPR].SerialNum;
		modPHScal::sSprInfo[m_iSEQofSPR].HeatDisp = modPHScal::sSprInfo[m_iSEQofSPR].haz;
		modPHScal::sSprInfo[m_iSEQofSPR].CheckDisp = (modPHScal::gbMinMaxDispByActualDisp?modPHScal::sSprInfo[m_iSEQofSPR].HeatDisp:tmpDist);
		modPHScal::sSprInfo[m_iSEQofSPR].SumDisp = tmpDist;
	}
}

//得到弹簧数据表中满足要求的数据集
void CSPRSel::GetHDCrude(HDCrude *pHDCrude)
{
	CString sTmp,SQLx;
	
	SQLx = _T("SELECT * FROM [") + modPHScal::tbnHDCrude + _T("]")  ;
	if( m_iSEQofSPR < modPHScal::SprInfoIndex  )
	{
		SQLx = SQLx+_T(" WHERE "); 
	}
	if(m_iSEQofSPR < modPHScal::SprInfoIndex)
	{
		sTmp.Format(_T("%s=%d"),pHDCrude->GetDHField(),modPHScal::sSprInfo[m_iSEQofSPR].DH);
		SQLx+=sTmp;
	}
	if(m_iSEQofSPR < modPHScal::SprInfoIndex && modPHScal::gbCSPRneedSpecialDesign)
	{
		sTmp.Format(_T(" AND %s<=%g AND %s>=%g"),pHDCrude->GetfBminField(),pHDCrude->GetfBmaxField(),
			modPHScal::sSprInfo[m_iSEQofSPR].CheckDisp,modPHScal::sSprInfo[m_iSEQofSPR].CheckDisp);
		SQLx += sTmp;
	}
	sTmp.Format(_T(" AND trim(%s)=\'%s\' ORDER BY %s,%s"),pHDCrude->GetCustomIDField(),m_strPtype,
		pHDCrude->GetDHField(),pHDCrude->GetWeightField());
	SQLx += sTmp;
	pHDCrude->Initialize(SQLx);
}

//得到合适拉杆
void CSPRSel::GetLugDiaOfCSPR(LugDiaOfCSPR *pLug,double dbWeight,CString strCustomID,int iFjmin)
{
	//其它恒力弹簧，循环2次才能找出最大拉杆直径
	CString sTmp,sTmp1,SQLx;
	
	if(modPHScal::gbByForce) //是否用荷载选拉杆
	{
		sTmp.Format(_T(" %s>=%g "),pLug->GetPmaxKgfField(),
			(m_strPartID == _T("LHG") ? m_dbSelPJG4CSPRFiJ +dbWeight * modPHScal::gintParallelNum : m_dbSelPJG4CSPRFiJ) / modPHScal::gintParallelNum);
		sTmp1.Format(_T(" ORDER BY %s,%s"),pLug->GetPmaxField(),pLug->GetFiJField() );
	}
	else
	{
		sTmp1.Format(_T(" ORDER BY %s"),pLug->GetFiJField() );
		
		//数据据库中根本就没有fBmin和fBmax字段
	/*	sTmp.Format(_T(" fBmin<=%g AND fBmax>=%g"),
			modPHScal::sSprInfo[iSEQofSPR].CheckDisp,
			modPHScal::sSprInfo[iSEQofSPR].CheckDisp);
		*/
	}
	SQLx.Format( _T("SELECT * FROM [%s] WHERE %s AND %s>=%d AND %s<=%d AND %s>=%d  AND trim(%s)=\'%s\' %s"),
		modPHScal::tbnLugDiaOfCSPR,					
		sTmp,pLug->GetFiJField(),
		(iFjmin <= 0 ? 10 : iFjmin),
		pLug->GetminDHField(),modPHScal::sSprInfo[m_iSEQofSPR].DH,
		pLug->GetmaxDHField(),modPHScal::sSprInfo[m_iSEQofSPR].DH,
		pLug->GetCustomIDField(),strCustomID,
		sTmp1);
	pLug->Initialize(SQLx);
}

//设置恒力弹簧之前的部件选型直径
void CSPRSel::SetAforeCompDia(int iFiJ)
{
	int k=0;
	if( m_iSEQofSPR + 1 > modPHScal::giWholeSprNum )
	{
		return ;
	}
	if( m_iFirstCal > 1 )
	{
		//第一次计算，之前的所有拉杆直径约束值=当前恒吊的松紧螺母直径
		//第二次计算，根据拉杆直径选择恒吊的松紧螺母直径，这时拉杆直径必然满足了恒吊松紧螺母的最小直径。
		return ;
	}
	if( m_iSEQofSPR > 0 )
	{
		k = modPHScal::giCSPRindex[m_iSEQofSPR - 1] + 1;
	}
	for(;k<= modPHScal::giCSPRindex[m_iSEQofSPR];k++)
	{
		modPHScal::PtypeDiameter[k] = iFiJ;
		if(modPHScal::gbRodDiaByMax) //拉杆直径最小限制
		{
			if(modPHScal::iMaxRodDia>modPHScal::PtypeDiameter[k])
				modPHScal::PtypeDiameter[k]=modPHScal::iMaxRodDia;
			else
				modPHScal::iMaxRodDia=modPHScal::PtypeDiameter[k];
		}
	}
}

/*
开发人:陈百炼
时间: 2007.11.16
用实际的数替换匹配式
tmpSQL0:需要替换的字符;pLugFik:??;
*/
void CSPRSel::ReplaceCNTB(LugDiaOfCSPR *pLugFik)
{
	CString tmpFD0;
	bool bFound = false;  
	int n = 0,m = 0,k = 0;
	COleVariant vTmp1;
	CString LogicalOpr = _T("=<>") ;  //逻辑操作定界符
	if( m_strtmpSQL0 != _T("") )
	{//查找连接表达式,首先去掉其中的Cnte.字符
		m_strtmpSQL0.MakeUpper();
		n=m_strtmpSQL0.Find((_T("CNTE.")));
		while(n!=-1)
		{
			m_strtmpSQL0 =m_strtmpSQL0.Left(n)+m_strtmpSQL0.Mid(n+5);
			n=m_strtmpSQL0.Find((_T("CNTE.")));
		}
		
		n=m_strtmpSQL0.Find((_T("CNTB.")));
		while(n!=-1)
		{
			bFound = false;
			for( m = n + 5 ;m< m_strtmpSQL0.GetLength();m++)
			{
				for( k = 0;k<LogicalOpr.GetLength();k++)
				{
					//看是否存在任何逻辑操作符
					if( LogicalOpr.Mid(k, 1) == m_strtmpSQL0.Mid(m, 1) )
					{
						tmpFD0 = m_strtmpSQL0.Mid( n + 5, m - n - 5);
						
						//恒吊的连接信息在另外一个记录集pLugFik。
						CString strValue;
						strValue = pLugFik->GetFieldValue(tmpFD0);
						m_strtmpSQL0 =m_strtmpSQL0.Left(n) + (strValue.IsEmpty() ? _T("0") : strValue) + m_strtmpSQL0.Mid( m);
						bFound = true;
						break;
					}
				}
				
				//找完一个，退出，再找下一个_T("Cntb.")
				if( bFound ) break;
			}
			
			n=m_strtmpSQL0.Find((_T("CNTB.")),n+1);
		}
	}
}

//设置前一个零件的插入长度
double CSPRSel::GetPrePartInsertLen()
{
	if( m_strPartID.IsEmpty() || m_strPrePartID.IsEmpty() )
	{
		return 0.0;
	}
	Connect mConnect;
	CString strSql;
	strSql.Format("SELECT * FROM %s WHERE %s = \'%s\' AND %s = \'%s\'",
		mConnect.GetTableName(),mConnect.GetCNTBField(),m_strPrePartID,
		mConnect.GetCNTEField(),m_strPartID);
	mConnect.Initialize(strSql);
	if( mConnect.IsNull() )
		return 0.0;
	return mConnect.GetCNTBextlenFLD();
}

//设置数据到TempCSLen表格中
void CSPRSel::SetTempCSLen(HDCrude *pHDCrude,double tmpDist,double tmpSelOpLoad)
{
	TmpCSLen mCSLen;
	mCSLen.SetAllBH( pHDCrude->GetBH() );
	mCSLen.SetAllLugDiameter( modPHScal::PtypeDiameter[m_i] );
	mCSLen.SetAllSumDist( tmpDist );
	double dbSizeC = 0;
	if( m_strPartID == _T("LHG") )
		dbSizeC = modPHScal::DblHangerRodDist;
	else
		dbSizeC = pHDCrude->GetSizeC();
	modPHScal::sSprInfo[m_iSEQofSPR].sizeC = dbSizeC ;
	mCSLen.SetAllSizeC( dbSizeC );
	
	mCSLen.SetAllHeatDist( modPHScal::sSprInfo[m_iSEQofSPR].HeatDisp );

	CString strText;
	strText.Format( _T("%s"),modPHScal::gsCSPRUnitOfLoad );
	mCSLen.SetAllUnitForce(strText);

	if( modPHScal::yr > 0 )
	{
		strText.Format( _T("%s"),modPHScal::gSprDirectionChar.CSPR_UP );
	}
	else
	{
		strText.Format( _T("%s"),modPHScal::gSprDirectionChar.CSPR_Down );
		
	}
	mCSLen.SetAllDistFX( strText );

	double dbScale = 1.0;
	if(modPHScal::gsCSPRUnitOfLoad == _T("daN") )
	{
		dbScale = EDIBgbl::kgf2N / 10.0;
	}
	else
	{
		dbScale = EDIBgbl::kgf2N;
	}
	mCSLen.SetAllSelOpLoad( tmpSelOpLoad * dbScale );
	
	double dbOpLoad = 0.0;
	if( m_strPartID == _T("LHG") )
	{
		dbOpLoad = (m_dbSelPJG4CSPRPgz + modPHScal::g_fLHGWeight)/modPHScal::gintParallelNum;
	}
	else
	{
		dbOpLoad = m_dbSelPJG4CSPRPgz / modPHScal::gintParallelNum;
	}
	mCSLen.SetAllOpLoad( long(dbOpLoad * dbScale) );
	mCSLen.SetAllInstallLoad( long(dbOpLoad * dbScale) );
}

//设置数据到ZB表格中
/*
mlfx:当前部件的旋转方向; mlfx0:前一部件的旋转方向
*/
void CSPRSel::SetResultZB(HDCrude *pCrude,LugDiaOfCSPR *pLugFik,long& mlfx,long& mlfx0)
{
	CString strSql;
	ResultZB mZB;
	strSql.Format(_T("SELECT * FROM %s WHERE %s=%d AND %s=%d AND %s =%d AND %s = %d ORDER BY %s"),
		EDIBgbl::Btype[EDIBgbl::TZB],mZB.GetZdjhField(),modPHScal::zdjh,
		mZB.GetCustomIDField(),EDIBgbl::SelVlmID,mZB.GetNthField(),m_inth,
		mZB.GetRecnoField(),m_iSEQofPart,
		mZB.GetRecnoField() );
	mZB.Initialize(strSql);
	if( mZB.IsNull() )
		return;

	mZB.SetNth( m_inth ); //标准支吊架路数=1
	double dbPreInsertLen = 0.0;
	dbPreInsertLen = GetPrePartInsertLen();
	mZB.SetPreviousPartInsertLen( dbPreInsertLen ); //前一部件插入长度
	

	//得到部件输出名字
	PictureClipData mClipData;
	strSql.Format( _T("SELECT * FROM %s WHERE trim(%s)=\'%s\'"),mClipData.GetTableName(),mClipData.GetCustomIDField(),m_strPtype );
	mClipData.Initialize(strSql);
	CString strBHFormat;
	strBHFormat = mClipData.GetBHFormat();
	if( !strBHFormat.IsEmpty() )
	{
		TmpCSLen mCSLen;
		strSql.Format(_T("SELECT (%s) AS sBHFormat FROM %s"),strBHFormat,mCSLen.GetTableName() );
		mCSLen.Initialize(strSql);
		CString strBHFormat;
		strBHFormat = mCSLen.GetFieldValue( _T("sBHFormat") );
		mZB.SetCLgg(strBHFormat);
		mZB.SetBHformat( strBHFormat ); //部件输出名字
		
		modPHScal::sSprInfo[m_iSEQofSPR].BHFormat=strBHFormat;
	}
	modPHScal::sSprInfo[m_iSEQofSPR].CustomID=m_strPartID;
	modPHScal::sSprInfo[m_iSEQofSPR].FiJ=modPHScal::PtypeDiameter[m_i];
	
	mZB.SetRecno(m_iSEQofPart);
	mZB.SetVolumeID(EDIBgbl::SelVlmID);
	mZB.SetZdjh(modPHScal::zdjh);
	mZB.SetCustomID(m_strPtype);
	mZB.SetClassID(modPHScal::glClassID);
	mZB.SetPjs(m_dbtmpSelPJG);

	//确定恒吊方向
	if( modPHScal::gbAutoAlignCSPRtoSA && m_strPartID !=_T("LHG") )
	{
		//恒吊，自动调整方向
		mlfx = modPHScal::GetPhsSAfx(modPHScal::fx + modPHScal::gf);
		mlfx0 = 0;
	}
	mZB.SetFx(mlfx);
	mZB.SetRotation(m_dbmlRot);//零件y坐标方向

	mZB.SetCLnum(m_iNumPart);
	mZB.SetSEQ(m_iSEQofPart); //当前恒力弹簧在模板中的部件索引号
	mZB.SetCLmc( mClipData.GetDescription() );

	double dbWeight = 0.0;
	//T5和LHG不会同时出现，因为弹簧和恒力弹簧不会同时出现
	if( m_strPartID == _T("LHG") || m_strPartID == _T("T5") )
		modPHScal::g_fLHGWeight = pCrude->GetWeight() ;
	mZB.SetCldz( dbWeight*m_iNumPart );
	mZB.SetIndex( modPHScal::glIDIndex ); //保存索引号，以便在汇总材料时能够迅速识别根部或其他类别。
	mZB.SetID( mClipData.GetID() );
	
	mZB.SetCLcl( GetResStr(IDS_GROUPWARE) );
	mZB.SetT( (int)pCrude->GetT() );
	
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
		tmpMovement=modPHScal::sSprInfo[m_iSEQofSPR].SumDisp;
	else
		tmpMovement=modPHScal::sSprInfo[m_iSEQofSPR].haz;
	CrudeTurnbuckleHDCz mCrudeT;
	strSql.Format(_T("SELECT * FROM [%s] WHERE trim(%s)=\'%s\' AND %s<=%d AND %d<=%s AND %s<=%g AND %s>=%g "),
				   modPHScal::tbnCSPRINGL5Crude,mCrudeT.GetCustomIDField(),
				   m_strPtype,mCrudeT.GetminDHField(),modPHScal::sSprInfo[m_iSEQofSPR].DH,
				   modPHScal::sSprInfo[m_iSEQofSPR].DH,mCrudeT.GetmaxDHField(),
				   mCrudeT.GetfBminField(),tmpMovement,
				   mCrudeT.GetfBmaxDHField(),tmpMovement);
	mCrudeT.Initialize( strSql );
	
	double dbMinLug = 0.0, dbMaxLug = 0.0;
	if( m_strPartID == _T("LHA") || m_strPartID == _T("LHB") || m_strPartID == _T("PHA")|| m_strPartID == _T("PHB") )
	{
		dbMinLug = pCrude->GetPP();
		dbMaxLug = dbMinLug;
	}
	else
	{
		dbMinLug = mCrudeT.GetSize4();
		dbMaxLug = mCrudeT.GetSize5();
	}
	mZB.SetAllowedMinLugExt( dbMinLug );
	mZB.SetAllowedMaxLugExt( dbMaxLug );

	double dbSigDim = GetSigDim(pCrude,&mCrudeT,pLugFik);
	mZB.SetSizeDim(dbSigDim); //
	mZB.SetSizeH(dbSigDim); //实际高度和长度
	if( EDIBAcad::g_bBomSprFactory )//newHS->只有选择了标注恒力弹簧厂家才写入数据库
	{
		mZB.SetClbz( modPHScal::gsCSPRmanufactory );
	}
	//暂时借用gdw1字段保存计算荷载，供后续过程GetBoltsNutsAndAttachmentsCLgg使用
	mZB.SetGDW1( m_dbSelPJG4CSPRFiK/m_iNumPart ); 
	mZB.SetSpringNo( modPHScal::sSprInfo[m_iSEQofSPR].DH );
	mZB.SetLugDia( modPHScal::sSprInfo[m_iSEQofSPR].FiJ );
	mZB.SetHeatDisp( modPHScal::sSprInfo[m_iSEQofSPR].HeatDisp );
	mZB.Update();
}

//得到实际尺寸
double CSPRSel::GetSigDim(HDCrude *pCrude,CrudeTurnbuckleHDCz *pCrudeTurn,LugDiaOfCSPR *pLugFik)
{
	//如果恒力弹簧的选型位移modPHScal::sSprInfo[iSEQofSPR].SumDisp正好等于下限位移字段fBmin的一个值，
	//则恒力弹簧的安装高度H1(sngH1xmax)=直接查得这个下限位移字段fBmin的值。
	//否则，按插值法计算位移。add by ligb on 2004.11.24
	CString sTmp;
	CString tmpSQL;
	double sngH1xmax = 0.0,sngDim = 0.0;

	sngH1xmax = pCrude->GetSizeH();
	if(modPHScal::gCH1distPos == 1 )
	{
		//E字段给出了中间位置的高度
		if(modPHScal::gbCalcCSPRHeight_BySelDisp)
			sngDim = sngH1xmax + Sgn(modPHScal::yr) * modPHScal::sSprInfo[m_iSEQofSPR].haz /(modPHScal::bE_FormulaPlusHalf?2:1);
		else
			sngDim = sngH1xmax + Sgn(modPHScal::yr) * modPHScal::sSprInfo[m_iSEQofSPR].SumDisp /(modPHScal::bE_FormulaPlusHalf?2:1);
	}
	else if(modPHScal::gCH1distPos == 0 )
	{
		double fBmin=0.0;
		fBmin = pCrude->GetfBmin();
		//E字段给出了最小位移位置的高度
		if( modPHScal::yr < 0 )
		{
			//热位移<0
			//sngDim = sngH1xmax + (Sgn(modPHScal::yr) * modPHScal::sSprInfo[iSEQofSPR].haz + vtof(vTmp1)) /(modPHScal::bE_FormulaPlusHalf?2:1);
			//研究表明，国标恒吊位移计算公式与美国恒吊计算公式相同，无论bE_FormulaPlusHalf为何，都要除2
			sngDim = sngH1xmax + (Sgn(modPHScal::yr) * (modPHScal::gbCalcCSPRHeight_BySelDisp?modPHScal::sSprInfo[m_iSEQofSPR].SumDisp:modPHScal::sSprInfo[m_iSEQofSPR].haz) + fBmin) /(modPHScal::bE_FormulaPlusHalf?2:2);
		}
		else
		{
			//热位移>=0，国标恒吊H1s=H1x+热位移T
			sngDim = sngH1xmax + (Sgn(modPHScal::yr) * (modPHScal::gbCalcCSPRHeight_BySelDisp?modPHScal::sSprInfo[m_iSEQofSPR].SumDisp:modPHScal::sSprInfo[m_iSEQofSPR].haz) + fBmin) /(modPHScal::bE_FormulaPlusHalf?2:2) + (m_strPartID == _T("ZHB") ? -1 : 0) * modPHScal::sSprInfo[m_iSEQofSPR].haz;
		}
	}
	else if(modPHScal::gCH1distPos == 2 )
	{
		//E字段给出了最大位移位置的高度
		double fBmin=0.0,fBmax=0.0;
		fBmin = pCrude->GetfBmin();
		fBmax = pCrude->GetfBmax();
		sngDim = sngH1xmax + (fBmin - fBmax)/2;
		if( modPHScal::yr < 0 )
		{
			//热位移<0
			sngDim = sngDim + (Sgn(modPHScal::yr) * (modPHScal::gbCalcCSPRHeight_BySelDisp?modPHScal::sSprInfo[m_iSEQofSPR].SumDisp:modPHScal::sSprInfo[m_iSEQofSPR].haz) + fBmax) /(modPHScal::bE_FormulaPlusHalf?2:2);
		}else
		{
			//热位移>=0
			sngDim = sngDim + (Sgn(modPHScal::yr) * (modPHScal::gbCalcCSPRHeight_BySelDisp?modPHScal::sSprInfo[m_iSEQofSPR].SumDisp:modPHScal::sSprInfo[m_iSEQofSPR].haz) + fBmax) /(modPHScal::bE_FormulaPlusHalf?2:2) + (m_strPartID == _T("ZHB")? -1:0) * modPHScal::sSprInfo[m_iSEQofSPR].haz;
		}
		//按国标恒吊或华东石油局扬州装备制造总厂管架分厂的解释，以上公式可废弃,应该按下列公式
		//sngDim = sngH1xmax + Sgn(modPHScal::yr) * modPHScal::sSprInfo[iSEQofSPR].haz /(modPHScal::bE_FormulaPlusHalf?2:0);
	}
	
	double Height_SPRINGsL5 = 0.0;
	Height_SPRINGsL5 = pCrudeTurn->GetSizeH();
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
		sngDim = sngDim + pCrudeTurn->GetSize4() ;
	}
	if(m_strPartID==_T("LHE") || m_strPartID==_T("LHE1") || m_strPartID==_T("PHE") 
		|| m_strPartID==_T("PHE1") || m_strPartID==_T("PHC") || m_strPartID==_T("LHC"))
	{
		//立式吊杆连接，要加上耳板高度T。
		if( !modPHScal::gbCH1IncludeT )
		{
			sngDim = sngDim + pLugFik->GetT();
		}
	}
	return sngDim;
}



