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

#pragma warning( disable : 4129 )  //Added by Shuli Luo
bool Cphs::GetphsBHandSizes(CFrmStatus& frmStatus,int  FirstCal, int MaxCalCount,int nth,
							float& TmpPaz,float& TmpPgz)
{
	float mvSumHeight =0;
	long iSEQofSPR, iSEQofPart, iChkCondition;
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

//		 rsSAPart(&EDIBgbl::dbPRJDB); //组成根部的附件表
	_RecordsetPtr rsSAPart;
//		 rsPartBoltNuts(&EDIBgbl::dbPRJDB); //恒吊螺栓螺母
	_RecordsetPtr rsPartBoltNuts;
	
	simplify(rsSAPart,nth);
	simplify2(rsPartBoltNuts,nth);
	
	//如果条件丢失,则默认为荷载、温度、根部定位条件应该全部满足
	if( iChkCondition==0 ) iChkCondition = iPJG + iTJ + iGDW1 + iCNT + iDW;
	
	//第一次计算，直径初始化
	if( FirstCal == 1 )
	{
		modPHScal::gmiDiameter = modPHScal::giDiameter = 0;
	}
	
	//如果条件丢失,则默认为第一遍计算状态；
	//否则，为第二遍计算状态。
	if( FirstCal==0 ) FirstCal = 1;
	return GetphsBHandSizes1(rsSAPart, rsPartBoltNuts, FirstCal, MaxCalCount, nth, TmpPaz, TmpPgz);
}

bool Cphs::GetphsBHandSizes1(_RecordsetPtr rsSAPart, _RecordsetPtr rsPartBoltNuts, int FirstCal,int MaxCalCount,int nth,float& TmpPaz,float& TmpPgz)
{
	CString dbstr = "\n" ;
	bool ret=false;
	CString strSelNumCS;//选择根部主槽钢数量的过滤字符串
	CString *PtypeID=NULL;   //不含附件的组件结构ID数组
	CString *Ptype=NULL;   //不含附件的组件结构CustomID数组
	long *PtypeClassID=NULL;   //不含附件的组件结构ClassID数组
	long *PtypeIndex=NULL;  //不含附件的组件结构Index数组
	
	float m_fDiaM1 = 0.0f;//保存管夹螺栓M1的值；
	CString strDiaM1FieldName;//管夹螺栓M1的尺寸字段，一般为size3或size2
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
	_Recordset* rsza ;
	rsza=FrmTxsr.m_pViewTxsr->m_ActiveRs;
	long i , j, k ;
	CString sBHFormat, sBH ;
	CString sPartID, sRodPASA;
	CString tmpCustomID0 , tmpCustomID1 ;
	CString tmpID0, tmpID1 ;
	CString tmpFD0 ;
	CString tmpExtLenFD;
	float tmpExtLenValue=0 ;  //上一个零件（除拉杆外）插入当前零件L8的深度
	float tmpFD0value=0, tmpFD0valuePrevious=0 ;  //保存前一个零件的连接匹配字段值
	float sngDim=0 , sngCSLen=0 , sngWeight =0, sngW=0 ;
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
	//Set mvarrsTmpREF1 = rsTmpREF.Clone
	COleVariant vTmp1,vTmp2,vTmp3,vTmp;
	COleVariant vBH;
	CString sTmp,sTmp2;

//		 rs1(&modPHScal::dbZDJcrude);
//		 rs3(&EDIBgbl::dbPHScode);//材料规范选择记录集//20071018 "dbSORT" 改为 "dbPHScode"
	_RecordsetPtr rs1;
	_RecordsetPtr rs3;
	CString m_strMaterial;//管部保温层以外的零部件选择的材料
// 		 rs(&modPHScal::dbZDJcrude);
// 		 rsTmp,rsTmp1,rsTmp2;
// 		 rsCal;	//根部强度计算公式表CalFormulaOfFixPASA，位于sort.mdb，用户可修改
// 		 rsDiaOfCSPRFiJ(&modPHScal::dbZDJcrude);//恒力弹簧拉杆直径表记录集，该集来源于表tbnLugDiaOfCSPR，上方恒吊松紧螺母荷载不要考虑恒吊自重
// 		 rsDiaOfCSPRFiK;//恒力弹簧连接耳子直径表记录集，该集来源于表tbnLugDiaOfCSPR，连接耳子荷载要考虑恒吊自重
//		rsDiaOfCSPRFiK.m_pDatabase=&modPHScal::dbZDJcrude;
	_RecordsetPtr rs;
	_RecordsetPtr rsTmp;
	_RecordsetPtr rsTmp1;
	_RecordsetPtr rsTmp2;
	_RecordsetPtr rsCal;
	_RecordsetPtr rsDiaOfCSPRFiJ;
	_RecordsetPtr rsDiaOfCSPRFiK;
	CString tbn1,SAfDPmax;
	bool m_bFoundL3=false;//发现与管夹连接的双孔吊板(这时可不必换小管夹螺栓)

	CString strOrderBy ;      //排序字符串
	CString strTmp ;          //临时字符串变量
	CString tmpSQL ;          //当前的连接匹配查询字段信息
	CString tmpSQL0 ;         //前一次的连接匹配查询字段信息
	long mlfx =0, mlfx0 =0; //零件/前一个零件左视图的方向
	long mlRot =0, mlRot0 =0; //零件/前一个零件Y轴的方向：与块中方向相同，=0；相反，=180。
	//long lngErrCalPAfix;   //计算固定支架时产生的错误号
// 		 rsX(&modPHScal::dbZDJcrude) ;
	_RecordsetPtr rsX;

	CString strSQL;
	strSQL.Format("SELECT * FROM %s WHERE zdjh=%d AND VolumeID=%d AND nth =%d ORDER BY recno",
		EDIBgbl::Btype[EDIBgbl::TZB],modPHScal::zdjh,EDIBgbl::SelVlmID,nth);

	_RecordsetPtr rsTZB;//当前支吊架当前路数包含所有零部件的表，包括螺栓螺母，按记录号排序
	try
	{
		
//		rsTZB.m_pDatabase=&EDIBgbl::dbPRJDB;
//		rsTZB.Open(dbOpenDynaset,strSQL);
		rsTZB->Open((_bstr_t)strSQL,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
	}
	catch(_com_error e)
	{
		ret=false;
	}

//		 rsTmpZB(&EDIBgbl::dbPRJDB);//当前支吊架当前路数零部件表，不含螺栓螺母、根部附件
		_RecordsetPtr rsTmpZB;
		strSQL.Format("SELECT CustomID FROM ZB WHERE [VolumeID]=%d AND [ZDJH]=%d AND [nth]=%d AND [ClassID]<>%d AND [ClassID]<>%d AND [ClassID]<>%d AND [IsSAPart]<>-1 Order By recno",
				EDIBgbl::SelVlmID,modPHScal::zdjh,nth,iBolts,iNuts,iAttached);
	try
	{
//		rsTmpZB.Open(dbOpenDynaset,strSQL);
		rsTmpZB->Open((_bstr_t)strSQL,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
			adOpenDynamic, adLockReadOnly, adCmdText); 
	}
	catch(_com_error e)
	{
		ret=false;
	}
		int C;
 		if( rsTmpZB->adoEOF&& rsTmpZB->BOF )
			throw GetResStr(IDS_Null_rsTmpREF);
		else
		{
			rsTmpZB->MoveLast();
			rsTmpZB->MoveFirst();
			Ptype=new CString[rsTmpZB->GetRecordCount()];
			PtypeID=new CString[rsTmpZB->GetRecordCount()];
			PtypeIndex=new long[rsTmpZB->GetRecordCount()];
			PtypeClassID=new long[rsTmpZB->GetRecordCount()];
			C=rsTmpZB->GetRecordCount();
			CString strBH = sBHFormat;
			for( i = 0 ;i < C;i++)
			{
				rsTmpZB->get_Collect((_variant_t)_T("CustomID"), &vTmp1);
				Ptype[i] = vtos(vTmp1);
				if( !rsID->Find((_bstr_t)(_T("trim(CustomID)=\'") + vtos(vTmp1) + _T("\'")), 0, adSearchForward, vTmp))
				{
					//出错
					sTmp.Format(GetResStr(IDS_NoRecordFLDEqvThisValueInPictureClipData),_T("CustomID"),vtos(vTmp1));
					throw sTmp;
				}
				else
				{
					rsID->get_Collect((_variant_t)_T("Index"), &vTmp1);
					PtypeIndex[i] = vtoi(vTmp1);
					rsID->get_Collect((_variant_t)_T("ClassID"), &vTmp1);
					PtypeClassID[i] = vtoi(vTmp1);
					rsID->get_Collect((_variant_t)_T("ID"), &vTmp1);
					PtypeID[i] = vtos(vTmp1);
					if(i>=1 && PtypeID[i]=="L3" && (PtypeID[i-1]=="D1" ||PtypeID[i-1]=="D1A" || PtypeID[i-1]=="D2" || PtypeID[i-1]=="D2A" || PtypeID[i-1]=="D3" || PtypeID[i-1]=="D3A" || PtypeID[i-1]=="D9" || PtypeID[i-1]=="D9A" || PtypeID[i-1]=="D10")) 
						m_bFoundL3=true;//如果存在L3吊板，管夹螺栓是不需要换小的
				}
				rsTmpZB->MoveNext();
			}
			rsTmpZB->MoveFirst();
		}
		//上面Ptype数组保留了mvarrsTmpZB的拷贝
		//以便取得支吊架组合的任意2个相邻零件,进行查找
		
		rsTmpZB->MoveFirst();
		if( ! rsTmpZB->adoEOF )
		{
			rsTmpZB->MoveLast();
			//展开sFindTBN，加快速度。
			rsTmpZB->get_Collect((_variant_t)_T("CustomID"), &vTmp1);
			
			if( !rsID->Find((_bstr_t)(_T("trim(CustomID)=\'") + vtos(vTmp1) + _T("\'")), 0, adSearchForward, vTmp))
			{
				//出错
				sTmp.Format(GetResStr(IDS_NoRecordFLDEqvThisValueInPictureClipData),_T("CustomID"),vtos(vTmp1));
				throw sTmp;
			}
			//假设最后一个零件是根部，保存根部索引码。
			rsID->get_Collect((_variant_t)_T("Index"), &vTmp1);
			mvSA = vtoi(vTmp1);
			//假设最后一个零件是根部，保存根部索引码。用于统计螺栓螺母时，判断当前零件是恒吊时下一个零件是否为根部。
			PtypeIndex[rsTmpZB->GetRecordCount()- 1] = mvSA;
			//以下确定采用单双槽钢
//			rsTmp.m_pDatabase=&modPHScal::dbZDJcrude;
//			rsTmp.Open(dbOpenSnapshot);
			CString strSQL = (_T("SELECT DISTINCT PNum1 From ")) + modPHScal::tbnSA + 
				_T(" WHERE CustomID=\'") + Ptype[C-1] + _T("\' AND PNum1 is not null");
		try
		{
			rsTmp->Open((_bstr_t)strSQL,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 
		}
		catch(_com_error e)
		{
			ret=false;
		}
			if(rsTmp->GetRecordCount()==1)
			{
				//只有一个记录,它的Pnum1字段=1，只能采用单槽钢；Pnum2=2，只能采用双槽钢。
				rsTmp->get_Collect((_variant_t)0L, &vTmp1);
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
			rsTmp->Close();
			if( rsTmpZB->GetRecordCount() >= 2 )
			{
				//零件数量多于或等于2个，则可以取出头尾零件。
				rsTmpZB->MoveFirst();
				//展开sFindTBN，加快速度。
				rsTmpZB->get_Collect((_variant_t)_T("CustomID"), &vTmp1);
				
				if( !rsID->Find((_bstr_t)(_T("trim(CustomID)=\'") + vtos(vTmp1) + _T("\'")), 0, adSearchForward, vTmp))
				{
					//出错
					sTmp.Format(GetResStr(IDS_NoRecordFLDEqvThisValueInPictureClipData),_T("CustomID"),vtos(vTmp1));
					throw sTmp;
				}
				//假设第一个零件是管部，保存管部类别码。
				rsID->get_Collect((_variant_t)_T("Index"), &vTmp1);
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
				C=rsTmpZB->GetRecordCount();
				CString strBH = sBHFormat;

				for (i = 0 ;i<C;i++)
				{
					if(i==0) rsTZB->MoveFirst();
					frmStatus.UpdateStatus((float)(i+1)/C,true);
					frmStatus.UpdateWindow();
					
					//获得零件所在的原始数据表tbn1,以便查找
					//展开sFindTBN，加快速度。
					
					if(! rsID->Find((_bstr_t)(_T("trim(CustomID)=\'") + Ptype[i] + _T("\'")), 0, adSearchForward, vTmp) )
					{
						//没有找到表名
						sTmp.Format(GetResStr(IDS_NoRecordFLDEqvThisValueInPictureClipData),_T("CustomID"),Ptype[i]);
						throw sTmp;
					}
					rsID->get_Collect((_variant_t)_T("ClassID"), &vTmp1);
					modPHScal::glClassID =vtoi(vTmp1);
					rsID->get_Collect((_variant_t)_T("Index"), &vTmp1);
					modPHScal::glIDIndex = vtoi(vTmp1);
					rsID->get_Collect((_variant_t)_T("ID"), &vTmp1);
					sPartID = vtos(vTmp1);
					tmpID0 = sPartID;
					if( modPHScal::glIDIndex == iSA )
					{
						//给进程显示一点时间
						//当前零件是根部或最后一个零件
						if( modPHScal::gbCalSAbyPJG )
						{
							//如果按结构荷载计算根部
							tmpSelPJG = tmpPJG;
						}
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
							if( rsID->Find((_bstr_t) (_T("trim(CustomID)=\'") + Ptype[i + 1] + _T("\'")), 0, adSearchForward, vTmp))
							{
								rsID->get_Collect((_variant_t)_T("ID"), &vTmp1);
								tmpID1 = vtos(vTmp1);
							}
							//不是最后一个零件
							tmpCustomID1 = Ptype[i + 1];
							//从Connect表中检索其是否存在及其匹配的尺寸信息
							
							if( !rsConnect->Find((_bstr_t) (_T("trim(cntb)=\'") + tmpID0 + _T("\' AND trim(cnte)=\'") + tmpID1 + _T("\'")), 0, adSearchForward, vTmp))
							{
								//Connect表中不存在这种组合.一般不会发生这情况.
								sTmp.Format(GetResStr(IDS_PartCanNotMatchOrNotExistInConnect),tmpCustomID0,tmpCustomID1);
								throw sTmp;
							}
							//改用ConnectCondition字段的复杂SQL条件进行连接尺寸检查，这样的适应性更广。速度慢一点。
							//其语法类似：Cntb.Size2=Cnte.Size2 AND Cntb.size4<=Cnte.Size5
							rsConnect->get_Collect((_variant_t)_T("ConnectCondition"), &vTmp1);
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
											} else
											{
												//没找到任何逻辑操作符号，一般由于数据库出了错误
											}
										}
										strDiaM1FieldName=tmpSQL0.Mid(5,i1-5);
										//连接字符串置空，则不检查连接螺栓尺寸，这样管夹螺栓即换小
										tmpSQL0 = _T("");
									}
								}
							}
							
							rsConnect->get_Collect((_variant_t)_T("CnteXfx"), &vTmp1);
							mlfx0 =vtoi(vTmp1);
							rsConnect->get_Collect((_variant_t)_T("CnteRotation"), &vTmp1);
							mlRot0 = vtoi(vTmp1);
							rsConnect->get_Collect((_variant_t)_T("CntbNum"), &vTmp1);
							iCntbNum=(vtoi(vTmp1)<=0 ? 1 : vtoi(vTmp1));
							rsConnect->get_Collect((_variant_t)_T("CnteNum"), &vTmp1);
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
						if(rs3->State == adOpenStatic)
							rs3->Close();
					try {
//						rs3.Open(dbOpenSnapshot,SQLx);
						rs3->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPHScode,true), 
							adOpenDynamic, adLockReadOnly, adCmdText); 
					}
					catch(_com_error e)
					{
						ret=false;
					}
						if(rs3->adoEOF && rs3->BOF)
						{
							//在%s库%s材料选择规范表没有%s字段值为默认值%s的记录(非管部零件材料选择规则)
							sTmp.Format(IDS_NoDefaultInCustomIDInSpecificationOfMaterial,EDIBgbl::dbPHScode->DefaultDatabase,_T("SpecificationOfMaterial"),_T("ID"),_T("default"));
							throw sTmp;
						}
						else
						{
							rs3->get_Collect((_variant_t)_T("Material"), &vTmp);
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
		{
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
				if(rsX->State == adOpenStatic)
					rsX->Close();
			try {
	//							rsX.Open(dbOpenSnapshot,SQLx);
				rsX->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPHScode,true), 
					adOpenDynamic, adLockReadOnly, adCmdText); 
			}
			catch(_com_error e)
			{
				ret=false;
			}
				if( rsX->adoEOF && rsX->BOF )
					;
				else
					//没找到，每次外径递增1%，继续找
					k=modPHScal::gnDW_delta+1;		  
			}

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
		{
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
				if(rsX->State == adOpenStatic)
					rsX->Close();
			try {
	//							rsX.Open(dbOpenSnapshot,SQLx);
				rsX->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPHScode,true), 
					adOpenDynamic, adLockReadOnly, adCmdText); 
			}
			catch(_com_error e)
			{
				ret=false;
			}
				if( rsX->adoEOF && rsX->BOF );
				else
					//没找到，每次外径递增1%，继续找
					k=modPHScal::gnDW_delta+1;
			}
			//2007.09.26(start)
			if(modPHScal::gbPAForceZero && (rsX->adoEOF && rsX->BOF))
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
					if(rsX->State == adOpenStatic)
						rsX->Close();
		try
		{
	//								rsX.Open(dbOpenSnapshot,SQLx);
					rsX->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPHScode,true), 
						adOpenDynamic, adLockReadOnly, adCmdText); 
				}
				catch(_com_error e)
				{
					ret=false;
				}
					if( rsX->adoEOF && rsX->BOF )
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
		}

		break;
	case iPAD4LA:
	case iPAD11LD:
	case iPADLR:
	case iPALX:
		{
			//这些(烟风煤粉管道管部)与管径无关
			SQLx = _T("SELECT * FROM ") + modPHScal::tbnPA + _T(" WHERE Pmax>=") ;
			sTmp.Format(_T("%g"),tmpSelPJG/iNumPart);
			SQLx+=sTmp+ _T(" AND Tj>=");
			sTmp.Format(_T("%g"),tmpT0);
			SQLx+=sTmp+_T(" AND trim(CustomID)=\'") + Ptype[i] + _T("\' ORDER BY tj,Pmax,Weight");
			if(rsX->State == adOpenStatic)
				rsX->Close();
		try
		{
	//						rsX.Open(dbOpenSnapshot,SQLx);
			rsX->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPHScode,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 
		}
		catch(_com_error e)
		{
			ret=false;
		}
		}

		break;
	case iROD:
		{
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
			
		}

		break; 
		
	case iConnected:
	case iXL1:
	case iAttached: 
		{
			if( PtypeID[i] == _T("L5") || PtypeID[i] == _T("L6") || PtypeID[i] == _T("L7") || PtypeID[i] == _T("L8")  || PtypeID[i] == _T("L7Dd") || PtypeID[i] == _T("L8Dd") )
			{
				//拉杆直径
				if(PtypeClassID[i-1]==iROD)
				{
					//前一个是拉杆			  
					modPHScal::PtypeDiameter[i]=modPHScal::PtypeDiameter[i-1];
					
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
				if(rsTmp->State == adOpenStatic)
					rsTmp->Close();
		try
		{
	//							rsTmp.m_pDatabase=&modPHScal::dbZDJcrude;
	//							rsTmp.Open(dbOpenDynaset,SQLx);
				rsTmp->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
					adOpenDynamic, adLockReadOnly, adCmdText); 
			}
			catch(_com_error e)
			{
				ret=false;
			}
				if(rsTmp->BOF && rsTmp->adoEOF)
				{
					//如果不存在这块钢板,加入它。
					/*rsTmp->AddNew();
					rsTmp->put_Collect((_variant_t)_T("CustomID"),COleVariant(_T("PS")));
					rsTmp->put_Collect((_variant_t)_T("BH"),COleVariant(sTmp));
					rsTmp->put_Collect((_variant_t)_T("size2"),COleVariant(modPHScal::gfPSThickness));
					rsTmp->put_Collect((_variant_t)_T("Weight"),COleVariant(fPSWeight));
					rsTmp->Update();*/
					strSQL = _T("INSERT INTO [")+ modPHScal::tbnAttachment + _T("] (CustomID,BH,size2,Weight) VALUES (\'PS\',\'") + sTmp + _T("\',") + ftos(modPHScal::gfPSThickness) + _T(",") + ftos(fPSWeight) + _T(")");
					modPHScal::dbZDJcrude->Execute((_bstr_t)strSQL, NULL, adCmdText);
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
		{
			if(iSEQofSPR < modPHScal::giWholeSprNum)
				modPHScal::giCSPRindex[iSEQofSPR] = i;
			//复位特殊设计标志
			EDIBgbl::dbPRJ->Execute((_bstr_t)_T("UPDATE tmpCSLen SET SpecialDesign=\'\'"), NULL, adCmdText);
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
			if(rs->State == adOpenStatic)
				rs->Close();
		try
		{
	//		rs.Open(dbOpenSnapshot,SQLx);
			rs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 
		}
		catch(_com_error e)
		{
			ret=false;
		}
			if( rs->adoEOF && rs->BOF )
			{
				//恒吊载荷容量表为空，不可能
				sTmp.Format(GetResStr(IDS_NullTableInXMdb),modPHScal::dbZDJcrude->DefaultDatabase, modPHScal::tbnHDproperty);
				throw sTmp;
			}
			else
			{
				rs->get_Collect((_variant_t)_T("Capacity"), &vTmp1);
				Cmin = vtof(vTmp1);
				rs->MoveLast();
				rs->get_Collect((_variant_t)_T("Capacity"), &vTmp1);
				Cmax = vtof(vTmp1);
				rs->MoveFirst();
				//复位：找到合适恒吊标志
				m_bFoundCSPR = false;
				//初始化时串联数iSumSerialNum=0
				iSumSerialNum = iSumSerialNum + 1;
				tmpDist = Tmpyr / modPHScal::giWholeSprNum;
				//查位移系列值，按就近上靠原则确定位移系列。
				if(rs1->State == adOpenStatic)
					rs1->Close();
				sTmp.Format(_T("%g"),tmpDist);
		try
		{
	//			rs1.Open(dbOpenSnapshot,_T("SELECT * FROM [") + modPHScal::tbnDisplacementSerial + _T("] WHERE DisplacementSerial>=") + sTmp + _T(" ORDER BY DisplacementSerial"));
				rs1->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
					adOpenDynamic, adLockReadOnly, adCmdText); 
			}
			catch(_com_error e)
			{
				ret=false;
			}

				if( rs1->adoEOF && rs1->BOF )
					//位移超出系列最大值
					;
				else
				{
					rs1->get_Collect((_variant_t)_T("DisplacementSerial"), &vTmp1);
					tmpDist =vtof(vTmp1);
					//by ligb on 2008.05.09 
					try
					{
						rs1->get_Collect((_variant_t)_T("MovementID"), &vTmp1);//这个字段是新加的，只有2007版江苏苏源恒力弹簧才有这个字段值。因此其它库可能出错，需要捕获错误。
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
				
							if(!rs->Find((_bstr_t)(_T("Capacity>=") + sTmp), 0, adSearchForward, vTmp));
							//没找到，不可能
							else
							{
								//找到了
								rs->get_Collect((_variant_t)_T("minDist"), &vTmp1);
								if( tmpDist < vtof(vTmp1) )
								{
									//比最小位移小，应该用弹簧
									sTmp.Format(GetResStr(IDS_ShouldUseSpring),_T("\%g"),_T("\%g"));
									sTmp2.Format(sTmp,tmpDist,vtof(vTmp1));
									throw sTmp2;
						
								}
								else
								{
									rs->get_Collect((_variant_t)_T("maxDist"), &vTmp1);
									if( tmpDist > vtof(vTmp1) )
									{
										//比最大位移还大
										if( iSumSerialNum >= modPHScal::giWholeSprNum )
										{
											//超过模板中的串联数

											//恒吊需要特殊设计（即恒吊不允许串联使用）,
											//只要一个恒吊就可以了
											//就用这种位移作为订货依据
											modPHScal::gbCSPRneedSpecialDesign = true;
											EDIBgbl::dbPRJ->Execute((_bstr_t)GetResStr(IDS_ESPECIAL_DESIGN_SQL), NULL, adCmdText);
											m_bFoundCSPR = true;
										}
									}
									else
									{
										//合适
										//这位移可以作为订货依据
										modPHScal::gbCSPRneedSpecialDesign = false;
										EDIBgbl::dbPRJ->Execute((_bstr_t)_T("UPDATE tmpCSLen SET SpecialDesign=\'\'"), NULL, adCmdText);
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
					rs->get_Collect((_variant_t)_T("Capacity"), &vTmp1);
					C1 = vtof(vTmp1);
					//记录前一个载荷容量
					rs->MovePrevious();
					rs->get_Collect((_variant_t)_T("Capacity"), &vTmp1);
					C2 = vtof(vTmp1);
					if( fabs(C1 - tmpCapacity) >= fabs(C2 - tmpCapacity) );
					//载荷容量更接近C2
					else
					{
						//载荷容量更接近C1
						//回到容量接近的记录
						rs->MoveNext();
					}
					//给恒吊赋值
					if(iSEQofSPR<modPHScal::SprInfoIndex)
					{
						modPHScal::sSprInfo[iSEQofSPR].SerialNum = 1;
						rs->get_Collect((_variant_t)_T("dh"), &vTmp1);
						modPHScal::sSprInfo[iSEQofSPR].DH =vtoi(vTmp1);
						modPHScal::sSprInfo[iSEQofSPR].haz = fabs(modPHScal::yr) / modPHScal::giWholeSprNum * modPHScal::sSprInfo[iSEQofSPR].SerialNum;
						modPHScal::sSprInfo[iSEQofSPR].HeatDisp = modPHScal::sSprInfo[iSEQofSPR].haz;
						modPHScal::sSprInfo[iSEQofSPR].CheckDisp = (modPHScal::gbMinMaxDispByActualDisp?modPHScal::sSprInfo[iSEQofSPR].HeatDisp:tmpDist);
						modPHScal::sSprInfo[iSEQofSPR].SumDisp = tmpDist;
						modPHScal::sSprInfo[iSEQofSPR].SumDistID = tmpDistID;
					}
					//选择的工作荷载，单位为kgf
					rs->get_Collect((_variant_t)_T("Capacity"), &vTmp1);
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
		   SQLx += (modPHScal::gbCSPRneedSpecialDesign ? _T(" ") : sTmp);
		   SQLx += _T(" AND trim(CustomID)=\'");
		   SQLx +=  Ptype[i];
		   SQLx += _T("\' ORDER BY dh,Weight");
		}

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
		{
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
		}

		break;
	case iSALbraceFixG47:
	case iSALbraceFixG48:    
		{
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
				if(rsTmp->State == adOpenStatic)
					rsTmp->Close();
		try
		{
	//			rsTmp.m_pDatabase=&modPHScal::dbZDJcrude;	
	//			rsTmp.Open(dbOpenSnapshot,SQLx);
				rsTmp->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
					adOpenDynamic, adLockReadOnly, adCmdText); 
			}
			catch(_com_error e)
			{
				ret=false;
			}
				//按照西北电力设计院支吊架手册1983年版117页解释,把"GDW1>="改为"GDW1=". lgb and pfg20050927
				SQLx = CString(_T("SELECT * FROM ")) + modPHScal::tbnSA + _T(" WHERE GDW1= ") + ftos(int((modPHScal::gdw+99)/100)*100) + _T(" AND trim(CustomID)=\'") + Ptype[i] + _T("\' ORDER BY GDW1,PmaxSF");
				if(rsX->State == adOpenStatic)
					rsX->Close();
		try
		{
	//			rsX.Open(dbOpenSnapshot,SQLx);
				rsX->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
					adOpenDynamic, adLockReadOnly, adCmdText); 
		}
		catch(_com_error e)
		{
			ret=false;
		}
				if( rsX->adoEOF && rsX->BOF )
					;
				else
				{
					while (!rsX->adoEOF)
					{
						rsX->get_Collect((_variant_t)_T("P1"), &vTmp1);
						rsX->get_Collect((_variant_t)_T("PmaxSF"), &vTmp3);
						sngSAfixP=vtof(vTmp3);
						if(rsTmp->Find((_bstr_t)(_T("BH=\'")+vtos(vTmp1)+_T("\'")), 0, adSearchForward, vTmp))
						{
							rsTmp->get_Collect((_variant_t)_T("F1ToWx1"), &vTmp2);
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
								rsX->MoveNext();
							}
						}
					}
					if(rsX->adoEOF)
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
				if(rs3->State == adOpenStatic)
					rs3->Close();
		try
		{
	//			rs3.Open(dbOpenSnapshot,SQLx);
				rs3->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
					adOpenDynamic, adLockReadOnly, adCmdText); 
			}
			catch(_com_error e)
			{
				ret=false;
			}
				if(rs3->adoEOF && rs3->BOF)
				{
					//在%s库%s材料选择规范表没有%s字段值为默认值%s的记录(非管部零件材料选择规则)
					sTmp.Format(IDS_NoDefaultInCustomIDInSpecificationOfMaterial,modPHScal::dbZDJcrude->DefaultDatabase,_T("SpecificationOfMaterial"),_T("ID"),_T("default"));
					throw sTmp;
				}
				else
				{
					rs3->get_Collect((_variant_t)_T("Material"), &vTmp);
					m_strMaterial=vtos(vTmp);
				}

				//查找应力
				float Sigmat=0.0,Sigmat100=0.0,Sigma0=0.0;
				modPHScal::StressOfMaterial(m_strMaterial,modPHScal::t0,Sigmat);
				modPHScal::StressOfMaterial(m_strMaterial,100,Sigmat100);
				modPHScal::StressOfMaterial(m_strMaterial,20,Sigma0);
				//打开固定支架计算公式表，备计算
				SQLx = _T("SELECT * FROM CalFormulaOfFixPASA WHERE CustomID=\'") + Ptype[i] + _T("\' ORDER BY CustomID,SEQ");
				if(rsCal->State == adOpenStatic)
					rsCal->Close();
		try
		{
	//			rsCal.m_pDatabase=&EDIBgbl::dbSACal;	//20071103 "dbSORT" 改为 "dbSACal"
	//			rsCal.Open(dbOpenSnapshot,SQLx);
				rsCal->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbSACal,true), 
					adOpenDynamic, adLockReadOnly, adCmdText); 
		}
		catch(_com_error e)
		{
			ret=false;
		}
				if(rsCal->adoEOF && rsCal->BOF)
				{
					//在数据库%s中的根部强度计算公式表%s没有任何记录。
					sTmp.Format(IDS_NullTableCalFormulaOfFixPASA,EDIBgbl::dbSACal->DefaultDatabase,_T("CalFormulaOfFixPASA"));//20071103 "dbSORT" 改为 "dbSACal"
					throw sTmp;
				}
				//打开根部强度计算公式表，备计算
				SQLx = _T("SELECT * FROM tmpCalFixPhs");
				if(rsTmp1->State == adOpenStatic)
					rsTmp1->Close();
		try
		{
	//			rsTmp1.m_pDatabase=&EDIBgbl::dbPRJ;	
				//rsTmp1需要修改功能
	//			rsTmp1.Open(dbOpenDynaset,SQLx);
				rsTmp1->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
					adOpenDynamic, adLockReadOnly, adCmdText); 
		}
		catch(_com_error e)
		{
			ret=false;
		}
				//打开槽钢特性表，备查
				SQLx = _T("SELECT * FROM SSteelPropertyCS ORDER BY ID");
				if(rsTmp->State == adOpenStatic)
					rsTmp->Close();
		try
		{
	//			rsTmp.m_pDatabase=&modPHScal::dbZDJcrude;	
	//			rsTmp.Open(dbOpenSnapshot,SQLx);
				rsTmp->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
					adOpenDynamic, adLockReadOnly, adCmdText); 
		}
		catch(_com_error e)
		{
			ret=false;
		}
				//打开角钢特性表，备查
				SQLx = _T("SELECT * FROM SSteelPropertyLS ORDER BY ID");
				if(rs1->State == adOpenStatic)
					rs1->Close();
		try
		{
	//			rs1.m_pDatabase=&modPHScal::dbZDJcrude;	
	//			rs1.Open(dbOpenSnapshot,SQLx);
				rs1->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
					adOpenDynamic, adLockReadOnly, adCmdText); 
		}
		catch(_com_error e)
		{
			ret=false;
		}
				//取根部表中的满足定位条件的所有记录,按PmaxSF排序，逐个查找
				SQLx = CString(_T("SELECT * FROM ")) + modPHScal::tbnSA + _T(" WHERE GDW1>= ") + ftos(modPHScal::gdw) + _T(" AND trim(CustomID)=\'") + Ptype[i] + _T("\' ORDER BY GDW1,PmaxSF");
				if(rsX->State == adOpenStatic)
					rsX->Close();
		try
		{
	//			rsX.Open(dbOpenSnapshot,SQLx);
				rsX->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
					adOpenDynamic, adLockReadOnly, adCmdText); 
		}
		catch(_com_error e)
		{
			ret=false;
		}
				if( rsX->adoEOF && rsX->BOF)
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
					_RecordsetPtr rs2;
					//modPHScal::PAfixH=320.0;//本句可测试华东院例题
		
	//				rsTmp1.Edit();
					//rsTmp1->put_Collect((_variant_t)_T("CustomIDsa"),COleVariant(Ptype(i)));
					rsTmp1->put_Collect((_variant_t)_T("CustomIDsa"),COleVariant(sPartID));
					rsTmp1->put_Collect((_variant_t)_T("H"),COleVariant(modPHScal::PAfixH));
					rsTmp1->put_Collect((_variant_t)_T("gdw2"),COleVariant(modPHScal::gdw));
					rsTmp1->put_Collect((_variant_t)_T("tj"),COleVariant(modPHScal::t0));
					//许用应力单位kgf/cm2->kgf/mm2
					rsTmp1->put_Collect((_variant_t)_T("SIGMAjt"),COleVariant(Sigmat/100.0));
					rsTmp1->put_Collect((_variant_t)_T("SIGMAj20"),COleVariant(Sigma0/100.0));
					rsTmp1->put_Collect((_variant_t)_T("SIGMAj100"),COleVariant(Sigmat100/100.0));
					rsTmp1->Update();
					//以上一句加快更新，尽量防止锁定。
		
					//获得构件3
					rsX->get_Collect((_variant_t)_T("P3"), &vTmp);
					strP3=vtos(vTmp);	
	//				rsTmp1.Edit();
					rsTmp1->put_Collect((_variant_t)_T("P3"),vTmp);
					//此处只适应中国标准，国际化时要修改代码
					n=strP3.Find((_T("x")));
					//构件3腿宽bb,unit:mm
					bb=_tcstod(strP3.Mid(1,n-1),NULL);
					rsTmp1->put_Collect((_variant_t)_T("bb"),COleVariant(bb));
					//获得主型钢P1
					rsX->get_Collect((_variant_t)_T("P1"), &vTmp);
					strP1=vtos(vTmp);
					rsTmp1->put_Collect((_variant_t)_T("P1"),vTmp);
					//获得构件2型钢P2
					rsX->get_Collect((_variant_t)_T("P2"), &vTmp);
					strP2=vtos(vTmp);
					rsTmp1->put_Collect((_variant_t)_T("P2"),vTmp);
					//双槽钢外缘最大距离PL3,mm
					rsX->get_Collect((_variant_t)_T("PL3"), &vTmp);
					PL3=vtof(vTmp);
					//rsX中的PL3是CHAR(20),而rsTmp1中的PL3是REAL,可见变体能够自动转换。
					rsTmp1->put_Collect((_variant_t)_T("PL3"),vTmp);
					//槽钢焊缝高度K,mm
					rsX->get_Collect((_variant_t)_T("size2"), &vTmp);
					K=vtof(vTmp);
					rsTmp1->put_Collect((_variant_t)_T("K"),vTmp);
					rsTmp1->Update();
					//序号DH,mm
					rsX->get_Collect((_variant_t)_T("dh"), &vTmp);
					dh=vtoi(vTmp);				
		
					//先计算热态
		
					bool bEOF=false;
					bool bErr=false;//任意一个条件不满足，置为true
					do
					{
						if(!rsTmp->Find((_bstr_t)(_T("BH=\'")+strP1+_T("\'")), 0, adSearchForward, vTmp))
						{
							//在槽钢特性表%s中没找到槽钢型号%s
							sTmp.Format(GetResStr(IDS_NotFoundThisCSNo),_T("SSteelPropertyCS"),strP1);
							throw sTmp;
						}
						//构件1槽钢惯性矩Ix,cm4->mm4
						rsTmp->get_Collect((_variant_t)_T("Ix"), &vTmp);
						float J1y=vtof(vTmp)*10000.0;
	//					rsTmp1.Edit();
						rsTmp1->put_Collect((_variant_t)_T("J1y"),COleVariant(J1y));
						//构件1槽钢惯性矩Iy,cm4->mm4
						rsTmp->get_Collect((_variant_t)_T("Iy"), &vTmp);
						float J1z=vtof(vTmp)*10000.0;
						rsTmp1->put_Collect((_variant_t)_T("J1z"),COleVariant(J1z));
						//构件1槽钢最小惯性矩,cm4->mm4
						rsTmp1->put_Collect((_variant_t)_T("Imin1"),COleVariant(J1z>J1y?J1y:J1z));
						//构件1槽钢抗弯矩Wx,cm3->mm3
						rsTmp->get_Collect((_variant_t)_T("Wx"), &vTmp);
						float W1y=vtof(vTmp)*1000.0;
						rsTmp1->put_Collect((_variant_t)_T("W1y"),COleVariant(W1y));
						//构件1槽钢抗弯矩Wy,cm3->mm3
						rsTmp->get_Collect((_variant_t)_T("Wy"), &vTmp);
						float W1z=vtof(vTmp)*1000.0;
						rsTmp1->put_Collect((_variant_t)_T("W1z"),COleVariant(W1z));
						//构件1槽钢截面积S,cm2->mm2
						rsTmp->get_Collect((_variant_t)_T("S"), &vTmp);
						float F1=vtof(vTmp)*100.0;
						rsTmp1->put_Collect((_variant_t)_T("F1"),COleVariant(F1));
						//构件1槽钢高度h,mm
						rsTmp->get_Collect((_variant_t)_T("h"), &vTmp);
						float h1=vtof(vTmp);
						rsTmp1->put_Collect((_variant_t)_T("h1"),vTmp);
						//构件1槽钢腿宽b,mm
						rsTmp->get_Collect((_variant_t)_T("b"), &vTmp);
						float b1=vtof(vTmp);
						rsTmp1->put_Collect((_variant_t)_T("b1"),vTmp);
						rsTmp1->Update();
			
						if(!rsTmp->Find((_bstr_t)(_T("BH=\'")+strP2+_T("\'")), 0, adSearchForward, vTmp))
						{
							//在槽钢特性表%s中没找到槽钢型号%s
							sTmp.Format(GetResStr(IDS_NotFoundThisCSNo),_T("SSteelPropertyCS"),strP2);
							throw sTmp;
						}
						//构件2槽钢惯性矩Ix,cm4->mm4
						rsTmp->get_Collect((_variant_t)_T("Ix"), &vTmp);
						float J2y=vtof(vTmp)*10000.0;
	//					rsTmp1.Edit();
						rsTmp1->put_Collect((_variant_t)_T("J2y"),COleVariant(J2y));
						//构件2槽钢惯性矩Iy,cm4->mm4
						rsTmp->get_Collect((_variant_t)_T("Iy"), &vTmp);
						float J2z=vtof(vTmp)*10000.0;
						rsTmp1->put_Collect((_variant_t)_T("J2z"),COleVariant(J2z));
						//构件1槽钢最小惯性矩,cm4->mm4
						rsTmp1->put_Collect((_variant_t)_T("Imin2"),COleVariant(J2z>J2y?J2y:J2z));
						//构件2槽钢抗弯矩Wx,cm3->mm3
						rsTmp->get_Collect((_variant_t)_T("Wx"), &vTmp);
						float W2y=vtof(vTmp)*1000.0;
						rsTmp1->put_Collect((_variant_t)_T("W2y"),COleVariant(W2y));
						//构件2槽钢抗弯矩Wy,cm3->mm3
						rsTmp->get_Collect((_variant_t)_T("Wy"), &vTmp);
						float W2z=vtof(vTmp)*1000.0;
						rsTmp1->put_Collect((_variant_t)_T("W2z"),COleVariant(W2z));
						//构件2槽钢截面积S,cm2->mm2
						rsTmp->get_Collect((_variant_t)_T("S"), &vTmp);
						float F2=vtof(vTmp)*100.0;
						rsTmp1->put_Collect((_variant_t)_T("F2"),COleVariant(F2));
						//构件2槽钢高度h,mm
						rsTmp->get_Collect((_variant_t)_T("h"), &vTmp);
						float h2=vtof(vTmp);
						rsTmp1->put_Collect((_variant_t)_T("h2"),COleVariant(h2));
						//构件2槽钢腿宽b,mm
						rsTmp->get_Collect((_variant_t)_T("b"), &vTmp);
						float b2=vtof(vTmp);
						rsTmp1->put_Collect((_variant_t)_T("b2"),COleVariant(b2));
			
						//双槽钢净空距离g,mm
						float g=PL3-2*h1;
						rsTmp1->put_Collect((_variant_t)_T("g"),COleVariant(g));
						//双槽钢中心距B1,mm
						float B1=PL3-h1;
						//因为rsTmp1中存在字段b1,而Jet数据库字段名是不区分大小写的，故换B1->Bx,表中相应的公式B1已经替换。
						rsTmp1->put_Collect((_variant_t)_T("Bx"),COleVariant(B1));
						//更新数据准备计算
						rsTmp1->Update();					
			
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
				
	//						rsTmp1.Edit();
							rsTmp1->put_Collect((_variant_t)_T("Px"),COleVariant(mfpx));
							rsTmp1->put_Collect((_variant_t)_T("Py"),COleVariant(mfpy));
							rsTmp1->put_Collect((_variant_t)_T("Pz"),COleVariant(mfpz));
							rsTmp1->put_Collect((_variant_t)_T("Mx"),COleVariant(mfmx));
							rsTmp1->put_Collect((_variant_t)_T("My"),COleVariant(mfmy));
							rsTmp1->put_Collect((_variant_t)_T("Mz"),COleVariant(mfmz));
							rsTmp1->Update();
				
							//构件1和构件2焊缝的抗弯矩,mm3						
							if(!rsCal->Find((_bstr_t)(_T("SEQ=1")), 0, adSearchForward, vTmp))
							{
								//根部%s第%s种条件的计算公式没找到。
								sTmp.Format(IDS_SACalFormulaXNotFound,Ptype[i],_T("1"));
								throw sTmp;
							}
							else
							{
								if(i4==2)
								{//几何数据，只计算一次，加快速度
									rsCal->get_Collect((_variant_t)_T("Wyh1saFormula"), &vTmp);
									Wyh1sa=vtos(vTmp);
									rsCal->get_Collect((_variant_t)_T("Wzh1saFormula"), &vTmp);
									Wzh1sa=vtos(vTmp);
									rsCal->get_Collect((_variant_t)_T("Wyh2saFormula"), &vTmp);
									Wyh2sa=vtos(vTmp);
									rsCal->get_Collect((_variant_t)_T("Wzh2saFormula"), &vTmp);
									Wzh2sa=vtos(vTmp);
						
									//应力判据可先获得，方便以后计算
									rsCal->get_Collect((_variant_t)_T("SIGMA1saMaxFormula"), &vTmp);
									SIGMA1saMax=vtos(vTmp);
									rsCal->get_Collect((_variant_t)_T("TAO1saMaxFormula"), &vTmp);
									TAO1saMax=vtos(vTmp);
									rsCal->get_Collect((_variant_t)_T("SIGMA2saMaxFormula"), &vTmp);
									SIGMA2saMax=vtos(vTmp);
									rsCal->get_Collect((_variant_t)_T("TAO2saMaxFormula"), &vTmp);
									TAO2saMax=vtos(vTmp);
									rsCal->get_Collect((_variant_t)_T("lamda1Formula"), &vTmp);
									lamda1=vtos(vTmp);
									rsCal->get_Collect((_variant_t)_T("lamda2Formula"), &vTmp);
									lamda2=vtos(vTmp);
						
									//开始计算几何、截面特性
									SQLx=_T("SELECT ")+ Wyh1sa+_T(" AS Wyh1sac, ")+ Wyh2sa+_T(" AS Wyh2sac, ") + Wzh1sa+_T(" AS Wzh1sac, ")+ Wzh2sa+_T(" AS Wzh2sac, ")+SIGMA1saMax+_T(" AS SIGMA1saMaxc, ")+TAO1saMax+_T(" AS TAO1saMaxc, ")+SIGMA2saMax+_T(" AS SIGMA2saMaxc, ")+TAO2saMax+_T(" AS TAO2saMaxc, ")+lamda1+_T(" AS lamda1c, ")+lamda2+_T(" AS lamda2c FROM tmpCalFixPhs");
									if(rsTmp2->State == adOpenStatic)
										rsTmp2->Close();
		try
		{
	//								rsTmp2.m_pDatabase=&EDIBgbl::dbPRJ;
	//								rsTmp2.Open(dbOpenDynaset,SQLx);
									rsTmp2->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
										adOpenDynamic, adLockReadOnly, adCmdText); 
								}
								catch(_com_error e)
								{
									ret=false;
								}
						
	//								rsTmp1.Edit();
									rsTmp2->get_Collect((_variant_t)_T("Wyh1sac"), &vTmp);
									rsTmp1->put_Collect((_variant_t)_T("Wyh1"),vTmp);
						
									rsTmp2->get_Collect((_variant_t)_T("Wzh1sac"), &vTmp);
									rsTmp1->put_Collect((_variant_t)_T("Wzh1"),vTmp);
						
									rsTmp2->get_Collect((_variant_t)_T("Wyh2sac"), &vTmp);
									rsTmp1->put_Collect((_variant_t)_T("Wyh2"),vTmp);
						
									rsTmp2->get_Collect((_variant_t)_T("Wzh2sac"), &vTmp);
									rsTmp1->put_Collect((_variant_t)_T("Wzh2"),vTmp);							
						
									rsTmp2->get_Collect((_variant_t)_T("TAO1saMaxc"), &vTmp);
									rsTmp1->put_Collect((_variant_t)_T("TAO1saMax"),vTmp);
									mfTAO1saMax=vtof(vTmp);
									rsTmp2->get_Collect((_variant_t)_T("SIGMA1saMaxc"), &vTmp);
									rsTmp1->put_Collect((_variant_t)_T("SIGMA1saMax"),vTmp);
									mfSIGMA1saMax=vtof(vTmp);
						
									rsTmp2->get_Collect((_variant_t)_T("TAO2saMaxc"), &vTmp);
									rsTmp1->put_Collect((_variant_t)_T("TAO2saMax"),vTmp);
									mfTAO2saMax=vtof(vTmp);
									rsTmp2->get_Collect((_variant_t)_T("SIGMA2saMaxc"), &vTmp);
									rsTmp1->put_Collect((_variant_t)_T("SIGMA2saMax"),vTmp);
									mfSIGMA2saMax=vtof(vTmp);
						
									rsTmp2->get_Collect((_variant_t)_T("lamda1c"), &vTmp);
									rsTmp1->put_Collect((_variant_t)_T("lamda1"),vTmp);
									mfLamda1=vtof(vTmp);
									rsTmp2->get_Collect((_variant_t)_T("lamda2c"), &vTmp);
									rsTmp1->put_Collect((_variant_t)_T("lamda2"),vTmp);
									mfLamda2=vtof(vTmp);
						
									rsTmp1->Update();
									float mf1=0.0,mf2=0,mfl1=0,mfl2=0;
									SQLx=_T("SELECT Lamda,Fi FROM SteadyDecreaseCoef WHERE Material=\'") + m_strMaterial + _T("\'");
									if(rs2->State == adOpenStatic)
										rs2->Close();
		try
		{
	//								rs2.m_pDatabase=&EDIBgbl::dbSACal;//20071103 "dbSORT" 改为 "dbSACal"
	//								rs2.Open(dbOpenSnapshot,SQLx);
									rs2->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbSACal,true), 
										adOpenDynamic, adLockReadOnly, adCmdText); 
							}
							catch(_com_error e)
							{
								ret=false;
							}
									if(rs2->adoEOF && rs2->BOF)
									{
										//在%s库稳定折减系数表%s中没有%s这种材料。
										sTmp.Format(IDS_NoMaterialInSteadyDecreaseCoef, EDIBgbl::dbSACal->DefaultDatabase,_T("SteadyDecreaseCoef"),m_strMaterial);
										throw sTmp;
									}
									else
									{
										if(!rs2->Find((_bstr_t)(_T("Lamda>")+ftos(mfLamda1)), 0, adSearchForward, vTmp))
										{
											//计算的细长比%s超过%s库%s表中最大值。
											sTmp.Format(IDS_LamdaOverMaxValue,ftos(mfLamda1),modPHScal::dbZDJcrude->DefaultDatabase,_T("SteadyDecreaseCoef"));
											throw sTmp;
										}
										else
										{
											rs2->get_Collect((_variant_t)_T("Lamda"), &vTmp);
											mfl2=vtof(vTmp);
											rs2->get_Collect((_variant_t)_T("Fi"), &vTmp);
											mf2=vtof(vTmp);
											rs2->MovePrevious();
											if(rs2->BOF)
											{
												//一般不可能,因为表中数值从0开始
											}
											else
											{
												//插值求Fi1
												rs2->get_Collect((_variant_t)_T("Lamda"), &vTmp);
												mfl1=vtof(vTmp);
												rs2->get_Collect((_variant_t)_T("Fi"), &vTmp);
												mf1=vtof(vTmp);
												mfFi1=(mf2-mf1)/(mfl2-mfl1)*(mfLamda1-mfl1)+mf1;
	//											rsTmp1.Edit();
												rsTmp1->put_Collect((_variant_t)_T("Fi1"),COleVariant(mfFi1));																					
												rsTmp1->Update();
											}
										}
										//求构件2稳定折减系数Fi2
										if(!rs2->Find((_bstr_t)(_T("Lamda>")+ftos(mfLamda2)), 0, adSearchForward, vTmp))
										{
											//计算的细长比%s超过%s库%s表中最大值。
											sTmp.Format(IDS_LamdaOverMaxValue,ftos(mfLamda2),modPHScal::dbZDJcrude->DefaultDatabase,_T("SteadyDecreaseCoef"));
											throw sTmp;
										}
										else
										{
											rs2->get_Collect((_variant_t)_T("Lamda"), &vTmp);
											mfl2=vtof(vTmp);
											rs2->get_Collect((_variant_t)_T("Fi"), &vTmp);
											mf2=vtof(vTmp);
											rs2->MovePrevious();
											if(rs2->BOF)
											{
												//一般不可能,因为表中数值从0开始
											}
											else
											{
												//插值求Fi1
												rs2->get_Collect((_variant_t)_T("Lamda"), &vTmp);
												mfl1=vtof(vTmp);
												rs2->get_Collect((_variant_t)_T("Fi"), &vTmp);
												mf1=vtof(vTmp);
												mfFi2=(mf2-mf1)/(mfl2-mfl1)*(mfLamda2-mfl1)+mf1;
	//											rsTmp1.Edit();
												rsTmp1->put_Collect((_variant_t)_T("Fi2"),COleVariant(mfFi2));																					
												rsTmp1->Update();
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
				
	//						rsTmp1.Edit();
							rsTmp1->put_Collect((_variant_t)_T("Py1"),COleVariant(Py1r1));
							rsTmp1->put_Collect((_variant_t)_T("Py2"),COleVariant(Py1r2));
							rsTmp1->Update();
				
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
				
							if(!rsCal->Find((_bstr_t)(_T("SEQ=")+ltos(i5)), 0, adSearchForward, vTmp))
							{
								//根部%s第%s种条件的计算公式没找到。
								sTmp.Format(IDS_SACalFormulaXNotFound,Ptype[i],_T("1"));
								throw sTmp;
							}
							else
							{
								rsCal->get_Collect((_variant_t)_T("SIGMA1saFormula"), &vTmp);
								SIGMA1sa=vtos(vTmp);
								rsCal->get_Collect((_variant_t)_T("TAOx1saFormula"), &vTmp);
								TAOx1sa=vtos(vTmp);
								rsCal->get_Collect((_variant_t)_T("TAOy1saFormula"), &vTmp);
								TAOy1sa=vtos(vTmp);
								rsCal->get_Collect((_variant_t)_T("TAOz1saFormula"), &vTmp);
								TAOz1sa=vtos(vTmp);
								rsCal->get_Collect((_variant_t)_T("TAO1saFormula"), &vTmp);
								TAO1sa=vtos(vTmp);
					
								rsCal->get_Collect((_variant_t)_T("SIGMA2saFormula"), &vTmp);
								SIGMA2sa=vtos(vTmp);
								rsCal->get_Collect((_variant_t)_T("TAOx2saFormula"), &vTmp);
								TAOx2sa=vtos(vTmp);
								rsCal->get_Collect((_variant_t)_T("TAOy2saFormula"), &vTmp);
								TAOy2sa=vtos(vTmp);
								rsCal->get_Collect((_variant_t)_T("TAOz2saFormula"), &vTmp);
								TAOz2sa=vtos(vTmp);
								rsCal->get_Collect((_variant_t)_T("TAO2saFormula"), &vTmp);
								TAO2sa=vtos(vTmp);
					
								//压杆的长度系数u1,u2:悬臂=2,简支=0.5							
								rsCal->get_Collect((_variant_t)_T("u1"), &vTmp);
	//							rsTmp1.Edit();
								rsTmp1->put_Collect((_variant_t)_T("u1"),vTmp);
								rsCal->get_Collect((_variant_t)_T("u2"), &vTmp);
								rsTmp1->put_Collect((_variant_t)_T("u2"),vTmp);
								rsTmp1->Update();
					
								//开始计算正应力sigma、剪应力tao
								SQLx=_T("SELECT ")+ TAOx1sa+_T(" AS TAOx1sac, ")+ TAOy1sa+_T(" AS TAOy1sac, ")+ TAOz1sa+_T(" AS TAOz1sac,")
									+ TAOx2sa+_T(" AS TAOx2sac, ")+ TAOy2sa+_T(" AS TAOy2sac, ")+ TAOz2sa+_T(" AS TAOz2sac FROM tmpCalFixPhs");
								if(rsTmp2->State == adOpenStatic)
									rsTmp2->Close();
		try
		{
	//							rsTmp2.m_pDatabase=&EDIBgbl::dbPRJ;
	//							rsTmp2.Open(dbOpenDynaset,SQLx);
								rsTmp2->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
									adOpenDynamic, adLockReadOnly, adCmdText); 
							}
							catch(_com_error e)
							{
								ret=false;
							}
					
	//							rsTmp1.Edit();
								rsTmp2->get_Collect((_variant_t)_T("TAOx1sac"), &vTmp);
								rsTmp1->put_Collect((_variant_t)_T("TAOx1sa"),vTmp);
								rsTmp2->get_Collect((_variant_t)_T("TAOy1sac"), &vTmp);
								rsTmp1->put_Collect((_variant_t)_T("TAOy1sa"),vTmp);
								rsTmp2->get_Collect((_variant_t)_T("TAOz1sac"), &vTmp);
								rsTmp1->put_Collect((_variant_t)_T("TAOz1sa"),vTmp);
					
								rsTmp2->get_Collect((_variant_t)_T("TAOx2sac"), &vTmp);
								rsTmp1->put_Collect((_variant_t)_T("TAOx2sa"),vTmp);
								rsTmp2->get_Collect((_variant_t)_T("TAOy2sac"), &vTmp);
								rsTmp1->put_Collect((_variant_t)_T("TAOy2sa"),vTmp);
								rsTmp2->get_Collect((_variant_t)_T("TAOz2sac"), &vTmp);
								rsTmp1->put_Collect((_variant_t)_T("TAOz2sa"),vTmp);
								rsTmp1->Update();
					
								SQLx=_T("SELECT ")+ TAO1sa+_T(" AS TAO1sac,")+ TAO2sa+_T(" AS TAO2sac,")+SIGMA1sa+_T(" AS SIGMA1sac,")+SIGMA2sa+_T(" AS SIGMA2sac FROM tmpCalFixPhs");
								if(rsTmp2->State == adOpenStatic)
									rsTmp2->Close();
		try
		{
	//							rsTmp2.m_pDatabase=&EDIBgbl::dbPRJ;
	//							rsTmp2.Open(dbOpenDynaset,SQLx);
								rsTmp2->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
									adOpenDynamic, adLockReadOnly, adCmdText); 
				}
				catch(_com_error e)
				{
					ret=false;
				}
			
								rsTmp2->get_Collect((_variant_t)_T("SIGMA1sac"), &vTmp);
								mfSIGMA1sa=vtof(vTmp);
	//							rsTmp1.Edit();
								rsTmp1->put_Collect((_variant_t)_T("SIGMA1sa"),vTmp);
					
								rsTmp2->get_Collect((_variant_t)_T("SIGMA2sac"), &vTmp);
								mfSIGMA2sa=vtof(vTmp);
								rsTmp1->put_Collect((_variant_t)_T("SIGMA2sa"),vTmp);
					
								rsTmp2->get_Collect((_variant_t)_T("TAO1sac"), &vTmp);
								mfTAO1sa=vtof(vTmp);							
								rsTmp1->put_Collect((_variant_t)_T("TAO1sa"),vTmp);
					
								rsTmp2->get_Collect((_variant_t)_T("TAO2sac"), &vTmp);
								mfTAO2sa=vtof(vTmp);
								rsTmp1->put_Collect((_variant_t)_T("TAO2sa"),vTmp);
								rsTmp1->Update();
					
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
							rsX->MoveNext();
							if(rsX->adoEOF)
							{
								//已经到记录尾，重新循环一次，以便报警
								bEOF=true;
								continue;
							}
							else
								//重新开始之前，设置超限标志
								bErr=false;
				
							//获得构件3
							rsX->get_Collect((_variant_t)_T("P3"), &vTmp);
							strP3=vtos(vTmp);	
	//						rsTmp1.Edit();
							rsTmp1->put_Collect((_variant_t)_T("P3"),vTmp);
							//此处只适应中国标准，国际化时要修改代码
							n=strP3.Find((_T("x")));
							//构件3腿宽bb,unit:mm
							bb=_tcstod(strP3.Mid(1,n-1),NULL);
							rsTmp1->put_Collect((_variant_t)_T("bb"),COleVariant(bb));
							//获得主型钢P1
							rsX->get_Collect((_variant_t)_T("P1"), &vTmp);
							strP1=vtos(vTmp);
							rsTmp1->put_Collect((_variant_t)_T("P1"),vTmp);
							//获得构件2型钢P2
							rsX->get_Collect((_variant_t)_T("P2"), &vTmp);
							strP2=vtos(vTmp);
							rsTmp1->put_Collect((_variant_t)_T("P2"),vTmp);
							//双槽钢外缘最大距离PL3,mm
							rsX->get_Collect((_variant_t)_T("PL3"), &vTmp);
							PL3=vtof(vTmp);
							//rsX中的PL3是CHAR(20),而rsTmp1中的PL3是REAL,可见变体能够自动转换。
							rsTmp1->put_Collect((_variant_t)_T("PL3"),vTmp);
							//槽钢焊缝高度K,mm
							rsX->get_Collect((_variant_t)_T("size2"), &vTmp);
							K=vtof(vTmp);
							rsTmp1->put_Collect((_variant_t)_T("K"),vTmp);
							rsTmp1->Update();
							//序号DH,mm
							rsX->get_Collect((_variant_t)_T("dh"), &vTmp);
							dh=vtoi(vTmp);				
						}
					}while(true);//记录尾再循环一次,以便报警
		
					//生成一个便于校核的数据库文件和相应的Excel文件
					EDIBgbl::ChangeColumnsToRows(EDIBgbl::dbPRJ,_T("tmpCalFixPhs"),_T("CheckCalFixPhs"));
					EDIBDB::OutPutTable( _T("CheckCalFixPhs"), basDirectory::ProjectDBDir+_T("CheckCalculation.xls"), _T("Excel 5.0;"), EDIBgbl::dbPRJ,  _T("CheckCalFixPhs"), _T("Excel 5.0;"));
		
					SQLx = CString(_T("SELECT * FROM ")) + modPHScal::tbnSA + _T(" WHERE GDW1>= ") + ftos(modPHScal::gdw) + _T(" AND trim(CustomID)=\'") + Ptype[i] + _T("\' AND dh=") + ltos(dh) + _T(" ORDER BY GDW1,PmaxSF");
				}
				//结束强度计算:华东院固定支架根部双臂三角架SJ8(ClassID=iSALbraceFixG47=4)
			}	
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
						if(rsX->State == adOpenStatic)
							rsX->Close();
	try
	{
//						rsX.Open(dbOpenSnapshot,SQLx);
						rsX->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
							adOpenDynamic, adLockReadOnly, adCmdText); 
					}
					catch(_com_error e)
					{
						ret=false;
					}
						if( rsX->adoEOF && rsX->BOF )
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
								if(rsX->State == adOpenStatic)
									rsX->Close();
	try
	{
//								rsX.Open(dbOpenSnapshot,SQLx);
								rsX->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
									adOpenDynamic, adLockReadOnly, adCmdText); 
							}
							catch(_com_error e)
							{
								ret=false;
							}
								if( rsX->adoEOF && rsX->BOF )
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
				  if(rsX->State == adOpenStatic)
					  rsX->Close();
	try
	{
//				  rsX.Open(dbOpenSnapshot,SQLx);
				  rsX->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
					  adOpenDynamic, adLockReadOnly, adCmdText); 
			  }
			  catch(_com_error e)
			  {
				  ret=false;
			  }
				  p1 = tmpSelPJG / 2;
				  p2 = 0;
				  strTmp = GetBHforDoubleCSBeam(modPHScal::gdw / 10, modPHScal::DblHangerRodDist / 10, modPHScal::Lspan / 10, p1, p2, tmpCustomID1, 2, FirstCal);
			  }
		   }
		   else
		   {
			   //单吊点，单根部
			   if(rsX->State == adOpenStatic)
				   rsX->Close();
	try
	{
//			   rsX.Open(dbOpenSnapshot,SQLx);
			   rsX->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
				   adOpenDynamic, adLockReadOnly, adCmdText); 
		   }
		   catch(_com_error e)
		   {
			   ret=false;
		   }
			   p1 = tmpSelPJG;
			   p2 = 0;
			   strTmp = GetBHforDoubleCSBeam(modPHScal::gdw / 10, modPHScal::DblHangerRodDist / 10, modPHScal::Lspan / 10, p1, p2, tmpCustomID1, 2, FirstCal);
		   }
		}
		else
		{
			//按保守方式计算根部
			if(rsX->State == adOpenStatic)
				rsX->Close();
	try
	{
//			rsX.Open(dbOpenSnapshot,SQLx);
			rsX->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 
		}
		catch(_com_error e)
		{
			ret=false;
		}
		}
	}
	else
	{
		if(rsX->State == adOpenStatic)
			rsX->Close();
	try
	{
//		rsX.Open(dbOpenSnapshot,SQLx);
		rsX->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
									adOpenDynamic, adLockReadOnly, adCmdText); 
	}
	catch(_com_error e)
	{
		ret=false;
	}
	}
spZ1Z2:
	//如果需要进行连接孔、杆尺寸自动检查, 要求孔尺寸匹配进行查找时
	//找不到匹配的应去除此项条件再查找 add by luorijin 2009.05.12
	if( modPHScal::gbConnectHoleDimMatch && tmpSQL != _T("") && rsX->adoEOF && rsX->BOF )
	{
		if(rsX->State == adOpenStatic)
			rsX->Close();
		SQLx.Replace(tmpSQL," ");
	try
	{
//			rsX.Open(dbOpenSnapshot,SQLx);
			rsX->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 
	}
	catch(_com_error e)
	{
		ret=false;
	}
	}
	if( rsX->adoEOF && rsX->BOF )
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
			if(rsX->State == adOpenStatic)
				rsX->Close();
	try
	{
//			rsX.Open(dbOpenSnapshot,tmpSQL);
			rsX->Open((_bstr_t)tmpSQL,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 
		}
		catch(_com_error e)
		{
			ret=false;
		}
			if( !rsX->adoEOF )
			{
				goto spZ1Z2;
			}
			else
			{
				if( modPHScal::gbAutoApplyCSPRLugDia && (FirstCal == 2) && modPHScal::gbPhsIsCSPR && modPHScal::PtypeDiameter[i] > 0 )
				{
					sTmp.Format(GetResStr(IDS_NotClearCheckAutoApplyCSPRLugDia),modPHScal::dbZDJcrude->DefaultDatabase,modPHScal::tbnPART, SQLx);
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
			rsX->Close();
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
		rsX->MoveFirst();
		if(phsAvailableTypeRs->State == adOpenStatic)
			phsAvailableTypeRs->Close();
	try
	{
//		phsAvailableTypeRs.m_pDatabase=rsX.m_pDatabase;
//		phsAvailableTypeRs.Open(dbOpenDynaset,rsX.GetSQL());
		phsAvailableTypeRs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
									adOpenDynamic, adLockReadOnly, adCmdText); 
	}
	catch(_com_error e)
	{
		ret=false;
	}
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
						rsX->get_Collect((_variant_t)_T("Weight"), &vTmp1);
						sTmp.Format(_T(" PmaxKgf>=%g "),
							(sPartID == _T("LHG") ? tmpSelPJG4CSPRFiJ + vtof(vTmp1) * modPHScal::gintParallelNum : tmpSelPJG4CSPRFiJ) / modPHScal::gintParallelNum);
					}
					else
					{
						sTmp.Format(_T(" fBmin<=%g AND fBmax>=%g"),
							modPHScal::sSprInfo[iSEQofSPR].CheckDisp,
							modPHScal::sSprInfo[iSEQofSPR].CheckDisp);
					}
					rsX->get_Collect((_variant_t)_T("CustomID"), &vTmp2);
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
						rsX->get_Collect((_variant_t)_T("Weight"), &vTmp1);
						sTmp.Format(_T(" PmaxKgf>=%g "),
							(sPartID == _T("LHG") ? tmpSelPJG4CSPRFiJ +vtof(vTmp1) * modPHScal::gintParallelNum : tmpSelPJG4CSPRFiJ) / modPHScal::gintParallelNum);
					}
					else
					{
						sTmp.Format(_T(" fBmin<=%g AND fBmax>=%g"),
							modPHScal::sSprInfo[iSEQofSPR].CheckDisp,
							modPHScal::sSprInfo[iSEQofSPR].CheckDisp);
					}
					rsX->get_Collect((_variant_t)_T("CustomID"), &vTmp2);
					rsX->get_Collect((_variant_t)_T("Fjmin"), &vTmp3);
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
						rsX->get_Collect((_variant_t)_T("Weight"), &vTmp1);
						sTmp.Format(_T(" PmaxKgf>=%g "),
							(sPartID == _T("LHG") ? tmpSelPJG4CSPRFiJ +vtof(vTmp1) * modPHScal::gintParallelNum : tmpSelPJG4CSPRFiJ) / modPHScal::gintParallelNum);
					}
					else
					{
						sTmp.Format(_T(" fBmin<=%g AND fBmax>=%g"),
							modPHScal::sSprInfo[iSEQofSPR].CheckDisp,
							modPHScal::sSprInfo[iSEQofSPR].CheckDisp);
					}
					rsX->get_Collect((_variant_t)_T("CustomID"), &vTmp2);
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
						rsX->get_Collect((_variant_t)_T("Weight"), &vTmp1);
						sTmp.Format(_T(" PmaxKgf>=%g "),
							(sPartID == _T("LHG") ? tmpSelPJG4CSPRFiJ +vtof(vTmp1) * modPHScal::gintParallelNum : tmpSelPJG4CSPRFiJ) / modPHScal::gintParallelNum);
					}
					else
					{
						sTmp.Format(_T(" fBmin<=%g AND fBmax>=%g"),
							modPHScal::sSprInfo[iSEQofSPR].CheckDisp,
							modPHScal::sSprInfo[iSEQofSPR].CheckDisp);
					}
					rsX->get_Collect((_variant_t)_T("CustomID"), &vTmp2);
					rsX->get_Collect((_variant_t)_T("Fjmin"), &vTmp3);
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
			if(rsDiaOfCSPRFiJ->State == adOpenStatic)
				rsDiaOfCSPRFiJ->Close();
	try
	{
//			rsDiaOfCSPRFiJ.Open(dbOpenSnapshot,SQLx);
			rsDiaOfCSPRFiJ->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 
	}
	catch(_com_error e)
	{
		ret=false;
	}
			if( rsDiaOfCSPRFiJ->adoEOF && rsDiaOfCSPRFiJ->BOF )
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
							rsDiaOfCSPRFiJ->get_Collect((_variant_t)_T("FiJ"), &vTmp1);
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
							rsDiaOfCSPRFiJ->get_Collect((_variant_t)_T("FiJ"), &vTmp1);
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
			rsX->get_Collect((_variant_t)_T("dh"), &vTmp1);
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
				rsX->get_Collect((_variant_t)_T("BH"), &vTmp1);
				//弹簧规格号
				modPHScal::sSprInfo[iSEQofSPR].BH=vtos(vTmp1);
				//获得弹簧自身松紧螺母直径
				rsX->get_Collect((_variant_t)_T("size2"), &vTmp1);
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
			rsX->get_Collect((_variant_t)_T("Weight"), &vTmp1);
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
				rsX->get_Collect((_variant_t)_T("Weight"), &vTmp1);
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
				rsX->get_Collect((_variant_t)_T("Weight"), &vTmp1);
				sTmp.Format(_T(" PmaxKgf>=%g "),
					(sPartID == _T("LHG") ? tmpSelPJG4CSPRFiJ +vtof(vTmp1) * modPHScal::gintParallelNum : tmpSelPJG4CSPRFiJ) / modPHScal::gintParallelNum);
			}
			else
			{
				sTmp.Format(_T(" fBmin<=%g AND fBmax>=%g"),
					modPHScal::sSprInfo[iSEQofSPR].CheckDisp,
					modPHScal::sSprInfo[iSEQofSPR].CheckDisp);
			}
			rsX->get_Collect((_variant_t)_T("CustomID"), &vTmp2);
			SQLx.Format( _T("SELECT * FROM [%s] WHERE %s  AND FiJ>=%d AND minDH<=%d AND maxDH>=%d  AND trim(CustomID)=\'%s\' %s"),
				modPHScal::tbnLugDiaOfCSPR,
				sTmp,
				(modPHScal::PtypeDiameter[i] <= 0 ? 10 : modPHScal::PtypeDiameter[i]),
				modPHScal::sSprInfo[iSEQofSPR].DH,
				modPHScal::sSprInfo[iSEQofSPR].DH,
				vtos(vTmp2),
				(modPHScal::gbByForce ? _T(" ORDER BY Pmax,FiJ") : _T(" ORDER BY FiJ")));

			if(rsDiaOfCSPRFiK->State == adOpenStatic)
				rsDiaOfCSPRFiK->Close();
	try
	{
	//		rsDiaOfCSPRFiK.Open(dbOpenSnapshot,SQLx);
			rsDiaOfCSPRFiK->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbSACal,true), 
				adOpenDynamic, adLockReadOnly, adCmdText); 
		}
		catch(_com_error e)
		{
			ret=false;
		}
			if( rsDiaOfCSPRFiK->adoEOF && rsDiaOfCSPRFiK->BOF )
			{
				sTmp.Format(GetResStr(IDS_NotFoundRecordInLugDiaOfCSPR), modPHScal::tbnLugDiaOfCSPR, SQLx);
				throw sTmp;
			}
			rsDiaOfCSPRFiJ->get_Collect((_variant_t)_T("FiJ"), &vTmp1);
			modPHScal::sSprInfo[iSEQofSPR].FiJ = vtof(vTmp1);
			rsDiaOfCSPRFiK->get_Collect((_variant_t)_T("FiK"), &vTmp1);
			modPHScal::sSprInfo[iSEQofSPR].FiK = vtof(vTmp1);
		}
		rsX->get_Collect((_variant_t)_T("CustomID"), &vTmp1);

		if(rsID->Find((_bstr_t)(_T("trim(CustomID)=\'") + vtos(vTmp1) + _T("\'")), 0, adSearchForward, vTmp))
		{
			rsID->get_Collect((_variant_t)_T("BHFormat"), &vTmp1);
			sBHFormat = vtos(vTmp1);
		}
		if( sBHFormat.GetLength()<=0 )
		{
			sBHFormat = _T("trim(BH)");
		}
		rsX->get_Collect((_variant_t)_T("BH"), &vTmp1);
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
								rsDiaOfCSPRFiK->get_Collect((_variant_t)tmpFD0, &vTmp1);
								tmpSQL0 =tmpSQL0.Left(n) + (vTmp1.vt==VT_NULL || vTmp1.vt==VT_EMPTY ? _T("0") : vtos(vTmp1)) + tmpSQL0.Mid( m);
							}
							else
							{
								rsX->get_Collect((_variant_t)tmpFD0, &vTmp1);
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
		//	rsTZB->AddNew();
		int iTmpRecno;
		rsTZB->get_Collect((_variant_t)_T("recno"), &vTmp);
		iTmpRecno=vtoi(vTmp);
		dbstr = dbstr +"iNnerRecno = " + ltos(iTmpRecno);
		rsTZB->get_Collect((_variant_t)_T("ID"), &vTmp);
		dbstr=dbstr+"  ID= "+vtos(vTmp);
		//rsTZB->Find((_bstr_t)"recno="+ltos(iTmpRecno));
	//	rsTZB.Edit();
		rsTZB->put_Collect((_variant_t)_T("nth"),COleVariant((short)nth));//标准支吊架路数=1，包括对称双吊和共用根部双吊

		dbstr = dbstr + "  FirstCal: "+ltos(FirstCal) + " nth: " + ltos(nth);

		//当前零件插入下一个零件(L8)的的长度尺寸字段
		rsConnect->get_Collect((_variant_t)_T("CNTBExtLenFLD"), &vTmp1);
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
					rsX->get_Collect((_variant_t)tmpExtLenFD, &vTmp2);
					rsTZB->put_Collect((_variant_t)_T("PreviousPartInsertLen"),COleVariant((long)vtoi(vTmp2)));
				}
				////////////////////////////////////////////
				if( PtypeID[i + 1] == _T("L8") || PtypeID[i + 1] == _T("L8Dd"))
				{
					rsX->get_Collect((_variant_t)tmpExtLenFD, &vTmp2);
					rsTZB->put_Collect((_variant_t)_T("PreviousPartInsertLen"),COleVariant((long)vtoi(vTmp2)));
					rsConnect->get_Collect((_variant_t)_T("CNTBExtLenFLD"), &vTmp2);
					tmpExtLenFD = vtos(vTmp2);
				}
				////////////////////////////////////////
			}
		}
		else
		{
			if(sPartID.Left(2) == _T("L7")||sPartID.Left(2) == _T("G12")||sPartID == _T("L3")||sPartID == _T("L4")||sPartID == _T("T1")||sPartID == _T("D4")||sPartID == _T("D5")||sPartID == _T("D11"))
			{
				rsConnect->get_Collect((_variant_t)_T("CNTBExtLenFLD"), &vTmp2);
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
						rsX->get_Collect((_variant_t)tmpExtLenFD, &vTmp2);
						rsTZB->put_Collect((_variant_t)_T("PreviousPartInsertLen"),COleVariant((long)vtoi(vTmp2)));
						//再查字段名称
						rsConnect->get_Collect((_variant_t)_T("CNTBExtLenFLD"), &vTmp2);
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
								rsConnect->get_Collect((_variant_t)_T("CNTBExtLenFLD"), &vTmp2);
								tmpExtLenFD = vtos(vTmp2);
								rsDiaOfCSPRFiK->get_Collect((_variant_t)tmpExtLenFD, &vTmp2);
								rsTZB->put_Collect((_variant_t)_T("PreviousPartInsertLen"),COleVariant((long)vtoi(vTmp2)));
							}
						}
						else
						{
							rsConnect->get_Collect((_variant_t)_T("CNTBExtLenFLD"), &vTmp2);
							tmpExtLenFD = vtos(vTmp2);
							rsTZB->put_Collect((_variant_t)_T("PreviousPartInsertLen"),COleVariant((long)tmpExtLenValue));
						}
					}
				}
				//////////////////////////////////////////////////////////////////////
				if( PtypeID[i + 1] == _T("L8") || PtypeID[i + 1] == _T("L8Dd") )
				{
					rsX->get_Collect((_variant_t)tmpExtLenFD, &vTmp2);
					rsTZB->put_Collect((_variant_t)_T("PreviousPartInsertLen"),COleVariant((long)vtoi(vTmp2)));
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
						rsConnect->get_Collect((_variant_t)_T("CNTBExtLenFLD"), &vTmp1);
						tmpExtLenFD = vtos(vTmp1);
					}
					else
					{
						//根据查出的字段值直接查表
						rsConnect->get_Collect((_variant_t)_T("CNTBExtLenFLD"), &vTmp1);
						tmpExtLenFD = vtos(vTmp1);
						rsTZB->put_Collect((_variant_t)_T("PreviousPartInsertLen"),COleVariant((long)tmpExtLenValue));
					}
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
								rsConnect->get_Collect((_variant_t)_T("CNTBExtLenFLD"), &vTmp1);
								tmpExtLenFD = vtos(vTmp1);
								rsDiaOfCSPRFiK->get_Collect((_variant_t)tmpExtLenFD, &vTmp1);
								rsTZB->put_Collect((_variant_t)_T("PreviousPartInsertLen"),vTmp1);
							}
						}
						else
						{
							rsConnect->get_Collect((_variant_t)_T("CNTBExtLenFLD"), &vTmp1);
							tmpExtLenFD = vtos(vTmp1);
							rsTZB->put_Collect((_variant_t)_T("PreviousPartInsertLen"),COleVariant((long)tmpExtLenValue));
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
					rsX->get_Collect((_variant_t)_T("sizeC"), &vTmp1);
					modPHScal::DblHangerRodDist =vtof(vTmp1);
					rsTZB->put_Collect((_variant_t)_T("sizeC"),COleVariant((float)modPHScal::DblHangerRodDist));
				}
				else
				{
					if( modPHScal::glClassID == iPAs )
					{
						//双支
						rsX->get_Collect((_variant_t)_T("size3"), &vTmp1);
						modPHScal::DblHangerRodDist =vtof(vTmp1);
						rsTZB->put_Collect((_variant_t)_T("sizeC"),COleVariant((float)modPHScal::DblHangerRodDist));
					}
					else
					{
						if( modPHScal::glClassID == iPAD11LD || modPHScal::glClassID == iPADLR || modPHScal::glPAid == iPALX )
						{
							//与管径有关的双吊间距
							rsX->get_Collect((_variant_t)_T("sizeC"), &vTmp1);
							modPHScal::DblHangerRodDist =vtof(vTmp1) + modPHScal::dj;
							rsTZB->put_Collect((_variant_t)_T("sizeC"),COleVariant((float)modPHScal::DblHangerRodDist));
						}
					}
				}
			}
		}

	   //由于恒吊或弹簧不便用SQL操作的方法获得编号，故去掉其中的连接操作符。
	   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET BH=\'") + sBH + _T("\'")), NULL, adCmdText);
	   rsTZB->put_Collect((_variant_t)_T("BH"),STR_VAR(sBH));
	   //对于横担弹簧T5或横担横吊LHG，必须根据管部D13/D14找出其中心距。
	   //对于下方弹簧T3A或横担弹簧T5或横担横吊LHG，必须先查找拉杆直径。
	   //所有螺栓螺母的规格将通过GetBoltsNutsAndAttachmentsCLgg函数处理。
	   sTmp.Format(_T("%d"),modPHScal::PtypeDiameter[i]);
	   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET LugDiameter=") + sTmp), NULL, adCmdText);
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
		   //if( rs->adoEOF And rs->BOF ){
		   sTmp.Format(_T("%d"),modPHScal::PtypeDiameter[i]);
		   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET LugDiameter=") + sTmp), NULL, adCmdText);
		   //EDIBgbl::dbPRJ.Execute _T("UPDATE tmpCSLen SET SumDist=") + IIf(tmpDist > IIf(IsNull(rsX(_T("fBmax"))), 0, rsX(_T("fBmax"))), IIf(IsNull(rsX(_T("fBmax"))), 0, rsX(_T("fBmax"))), tmpDist)
		   sTmp.Format(_T("%g"),tmpDist);
		   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET SumDist=") +sTmp), NULL, adCmdText);
		   sTmp.Format(_T("%d"),tmpDistID);
		   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET SumDistID=") +sTmp), NULL, adCmdText);//对于火电厂汽水管道支吊架2007版送审稿恒力弹簧，设置位移代号
		   rsX->get_Collect((_variant_t)_T("sizeC"), &vTmp1);
		   sTmp.Format(_T("%g"),(sPartID == _T("LHG") ? modPHScal::DblHangerRodDist : vtof(vTmp1)));
		   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET sizeC=") + sTmp), NULL, adCmdText);
		   //保存中心距，用于T5/LHG查出拉杆直径后再次生成规格CLgg
		   modPHScal::sSprInfo[iSEQofSPR].sizeC=_ttoi(sTmp);
		   sTmp.Format(_T("%g"),modPHScal::sSprInfo[iSEQofSPR].HeatDisp);
		   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET HeatDist=") + sTmp), NULL, adCmdText);
		   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET UnitForce=\'") +modPHScal::gsCSPRUnitOfLoad + _T("\'")), NULL, adCmdText);
		   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET DistFX=\'") + (modPHScal::yr > 0 ? modPHScal::gSprDirectionChar.CSPR_UP : modPHScal::gSprDirectionChar.CSPR_Down) + _T("\'")), NULL, adCmdText);
		   if(modPHScal::gsCSPRUnitOfLoad == _T("daN") )
		   {
			   //SelOpLoad=选择的工作荷载,考虑位移裕量之后求得的载荷值tmpSelOpLoad
			   sTmp.Format(_T("%d"),(long)(tmpSelOpLoad * EDIBgbl::kgf2N / 10));
			   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET SelOpLoad=") + sTmp), NULL, adCmdText);  //常州电力机械厂引进ITT技术，单位为daN，其它厂家为N。
	   
			   sTmp.Format(_T("%d"),(long)((sPartID==_T("LHG")?tmpSelPJG4CSPRPgz + modPHScal::g_fLHGWeight : tmpSelPJG4CSPRPgz) /modPHScal::gintParallelNum * EDIBgbl::kgf2N / 10 ));
			   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET OpLoad=") + sTmp), NULL, adCmdText); //常州电力机械厂引进ITT技术，单位为daN，其它厂家为N。
			   sTmp.Format(_T("%d"),(long)((sPartID==_T("LHG")?tmpSelPJG4CSPRPgz + modPHScal::g_fLHGWeight : tmpSelPJG4CSPRPgz) /modPHScal::gintParallelNum * EDIBgbl::kgf2N / 10));
			   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET InstallLoad=") + sTmp), NULL, adCmdText); //常州电力机械厂引进ITT技术，单位为daN，其它厂家为N。
		   }
		   else if(modPHScal::gsCSPRUnitOfLoad == _T("N") )
		   {
			   //SelOpLoad=选择的工作荷载,考虑位移裕量之后求得的载荷值tmpSelOpLoad
			   sTmp.Format(_T("%d"),(long)(tmpSelOpLoad * EDIBgbl::kgf2N));
			   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET SelOpLoad=") +sTmp), NULL, adCmdText);
			   sTmp.Format(_T("%d"),(long)((sPartID==_T("LHG")? tmpSelPJG4CSPRPgz + modPHScal::g_fLHGWeight : tmpSelPJG4CSPRPgz) /modPHScal::gintParallelNum * EDIBgbl::kgf2N));
			   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET OpLoad=") + sTmp), NULL, adCmdText);
			   sTmp.Format(_T("%d"),(long)((sPartID==_T("LHG")?tmpSelPJG4CSPRPgz + modPHScal::g_fLHGWeight : tmpSelPJG4CSPRPgz) /modPHScal::gintParallelNum * EDIBgbl::kgf2N));
			   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET InstallLoad=") + sTmp), NULL, adCmdText);
		   }
		   else
		   {
			   //SelOpLoad=选择的工作荷载,考虑位移裕量之后求得的载荷值tmpSelOpLoad
			   sTmp.Format(_T("%d"),(long)(tmpSelOpLoad));
			   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET SelOpLoad=") +sTmp), NULL, adCmdText);
			   sTmp.Format(_T("%d"),(long)((sPartID==_T("LHG")?tmpSelPJG4CSPRPgz + modPHScal::g_fLHGWeight : tmpSelPJG4CSPRPgz )/modPHScal::gintParallelNum));
			   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET OpLoad=") + sTmp), NULL, adCmdText);
			   sTmp.Format(_T("%d"),(long)((sPartID==_T("LHG")?tmpSelPJG4CSPRPgz + modPHScal::g_fLHGWeight : tmpSelPJG4CSPRPgz) /modPHScal::gintParallelNum));
			   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET InstallLoad=") + sTmp), NULL, adCmdText);
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
		   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET SerialNum=") + sTmp), NULL, adCmdText);
		   sTmp.Format(_T("%d"),modPHScal::sSprInfo[iSEQofSPR].SprNo);
		   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET SprNo=") + sTmp), NULL, adCmdText);
		   sTmp.Format(_T("%d"),modPHScal::gSumDistOfSpring);
		   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET SumDist=") +sTmp), NULL, adCmdText);
		   sTmp.Format(_T("%d"),modPHScal::sSprInfo[iSEQofSPR].SumDistID);
		   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET SumDistID=") +sTmp), NULL, adCmdText);
		   if( (PtypeID[i-1] == _T("D13") || PtypeID[i-1] == _T("D14")) && (sPartID == _T("T5") || sPartID == _T("TDG") || sPartID == _T("LHG")) )
		   {
			   //下方弹簧或下方恒力弹簧的中心距是管部中心距C决定的，
			   //这个C值要写入弹簧规格中，作为订货依据
			   sTmp.Format(_T("%g"),modPHScal::DblHangerRodDist);
			   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET sizeC=") + sTmp), NULL, adCmdText);
			   //保存中心距，用于T5/LHG查出拉杆直径后再次生成规格CLgg
			   modPHScal::sSprInfo[iSEQofSPR].sizeC=_ttoi(sTmp);
		   }		  
		   sTmp.Format(_T("%g"),modPHScal::sSprInfo[iSEQofSPR].HeatDisp);
		   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET HeatDist=") + sTmp), NULL, adCmdText);
		   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET UnitForce=\'") +modPHScal::gsSPRUnitOfLoad + _T("\'")), NULL, adCmdText);
		   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET DistFX=\'") + (modPHScal::yr > 0 ? modPHScal::gSprDirectionChar.SPR_UP : modPHScal::gSprDirectionChar.SPR_Down) + _T("\'")), NULL, adCmdText);
		   if(modPHScal::gsSPRUnitOfLoad == _T("daN") )
		   {
			   //荷载的单位是daN(10N)
			   sTmp.Format(_T("%d"),(long)(fabs(sPartID==_T("T5")?tmpSelPJG4SPRPgz + modPHScal::g_fLHGWeight : tmpSelPJG4SPRPgz) / iNumPart * EDIBgbl::kgf2N / 10));
			   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET OpLoad=") + sTmp), NULL, adCmdText); //常州电力机械厂引进ITT技术，单位为daN，其它厂家为N。
			   sTmp.Format(_T("%d"),(long)(fabs(sPartID==_T("T5")?tmpSelPJG4SPRPaz + modPHScal::g_fLHGWeight : tmpSelPJG4SPRPaz) / iNumPart * EDIBgbl::kgf2N / 10));
			   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET InstallLoad=") + sTmp), NULL, adCmdText); //常州电力机械厂引进ITT技术，单位为daN，其它厂家为N。
		   }
		   else if(modPHScal::gsSPRUnitOfLoad == _T("N") )
		   {
			   //荷载的单位是N
			   //工作荷载四舍五入，以免出现和输入值不一致的情况,此bug最先被中南院钟毅发现
			   //(例如,pgz=9000N,yr=14mm,下两句不加0.5会出现"T2.110,14向上/8999,M20")
			   sTmp.Format(_T("%d"),(long)(fabs(sPartID==_T("T5")?tmpSelPJG4SPRPgz + modPHScal::g_fLHGWeight : tmpSelPJG4SPRPgz) / iNumPart * EDIBgbl::kgf2N+0.5));
			   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET OpLoad=") + sTmp), NULL, adCmdText);
			   sTmp.Format(_T("%d"),(long)(fabs(sPartID==_T("T5")?tmpSelPJG4SPRPaz + modPHScal::g_fLHGWeight : tmpSelPJG4SPRPaz) / iNumPart * EDIBgbl::kgf2N+0.5));
			   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET InstallLoad=") + sTmp), NULL, adCmdText);
		   }
		   else
		   {
			   //荷载的单位是kgf
			   sTmp.Format(_T("%d"),(long)(fabs(sPartID==_T("T5")?tmpSelPJG4SPRPgz + modPHScal::g_fLHGWeight : tmpSelPJG4SPRPgz) / iNumPart));
			   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET OpLoad=") + sTmp), NULL, adCmdText);
			   sTmp.Format(_T("%d"),(long)(fabs(sPartID==_T("T5")?tmpSelPJG4SPRPaz + modPHScal::g_fLHGWeight : tmpSelPJG4SPRPaz) / iNumPart));
			   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET InstallLoad=") + sTmp), NULL, adCmdText);
		   }
	   }	   
	   if(modPHScal::glIDIndex != iSA )
	   {
		   if(modPHScal::glIDIndex == iPA )
		   {
			   //温度可能作为标志管部的信息
			   sTmp.Format(_T("%g"),modPHScal::t0);
			   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET t0=") + sTmp), NULL, adCmdText);
			   rsX->get_Collect((_variant_t)_T("Material"), &vTmp1);
			   rsTZB->put_Collect((_variant_t)_T("CLcl"),vTmp1);
			   if(modPHScal::glClassID==iPAfixZ2)
			   {
				   //固定支架Z2A/Z2/SZ5管部根部焊缝高度k1(size4),k2(size5)--mm
				   rsX->get_Collect((_variant_t)_T("size4"), &vTmp1);				 
				   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET K=") + vtos(vTmp1)), NULL, adCmdText);
				   rsX->get_Collect((_variant_t)_T("size5"), &vTmp1);				 
				   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET K2=") + vtos(vTmp1)), NULL, adCmdText);
				   //固定支架Z2A/Z2/SZ5管部支管管径DW1xS(sizeC x size3),主管外径DW--mm,华东院手册需要它们作标记。
				   rsX->get_Collect((_variant_t)_T("DW"), &vTmp1);				 
				   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET DW=") + vtos(vTmp1)), NULL, adCmdText);
				   rsX->get_Collect((_variant_t)_T("sizeC"), &vTmp1);				 
				   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET dw1=") + vtos(vTmp1)), NULL, adCmdText);
				   rsX->get_Collect((_variant_t)_T("size3"), &vTmp1);				 
				   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET S=") + vtos(vTmp1)), NULL, adCmdText);
				   //SZ5标号中需要温度t0/管径dj			 
				   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET temperature=") + ftos(modPHScal::t0)), NULL, adCmdText);
			   }
		   }
   
		   SQLx = _T("SELECT (") + sBHFormat + _T(") AS sBHFormat  FROM tmpCSLen");
		   if(rsTmp->State == adOpenStatic)
			   rsTmp->Close();
	try
	{
//		   rsTmp.m_pDatabase=&EDIBgbl::dbPRJ;
//		   rsTmp.Open(dbOpenDynaset,SQLx);
		   rsTmp->Open((_bstr_t)strSQL,_variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
			   adOpenForwardOnly, adLockReadOnly, adCmdText); 
	   }
	   catch(_com_error e)
	   {
		   ret=false;
	   }
		   rsTmp->get_Collect((_variant_t)_T("sBHFormat"), &vTmp1);
		   sBHFormat =vtos(vTmp1);
		   rsTZB->put_Collect((_variant_t)_T("BHformat"),STR_VAR(sBHFormat));
		   if(modPHScal::glClassID == iSPR || modPHScal::glClassID == iCSPR )
		   {
			   rsTZB->put_Collect((_variant_t)_T("CLgg"),STR_VAR(sBHFormat));
			   //保存弹簧/恒力弹簧信息用于T5/LHG重新生成编号，及输出弹簧订货表等
			   modPHScal::sSprInfo[iSEQofSPR].CustomID=Ptype[i];
			   modPHScal::sSprInfo[iSEQofSPR].BHFormat=sBHFormat;
			   modPHScal::sSprInfo[iSEQofSPR].FiJ=modPHScal::PtypeDiameter[i];
		   }
		   else if(modPHScal::glClassID == iROD || modPHScal::glClassID == iXL1 )
			   //拉杆的规格需要在计算长度后确定，此处必须赋予CLgg字段正确的BH。
			   //在CalLugLength函数中需要正确的赋予BH值的CLgg值。
		   {
			   rsTZB->put_Collect((_variant_t)_T("CLgg"),STR_VAR(sBH));
		   }
		   else if( modPHScal::glIDIndex ==iPA && modPHScal::g_bPipeClampBoltDiameterChanged)
			   //小荷载大管径管夹按荷载选择，螺栓的直径M1变小，管夹规格中必须注明此M1值
		   {
			   //取出管夹螺栓M1的值
			   rsX->get_Collect((_variant_t)strDiaM1FieldName, &vTmp1);
			   m_fDiaM1 = vtof(vTmp1);
			   //管夹螺栓M1与U形耳子L8的螺栓M功能相似
			   CString strSQLTmp ;
				strSQLTmp.Format("SELECT * FROM [%s] WHERE [CUSTOMID]=\'%s\' AND [SIZE2]=%d ORDER BY SIZE2",
					modPHScal::tbnPART,modPHScal::sFindCustomID (_T("L8")),int(modPHScal::gmiDiameter));			   
				_RecordsetPtr m_rs;
			   COleVariant vT1;
			   if(m_rs->State == adOpenStatic)
				   m_rs->Close();
	   
	try
	{
//			   m_rs.m_pDatabase=&modPHScal::dbZDJcrude;
//			   m_rs.Open(dbOpenSnapshot,strSQLTmp);
			   m_rs->Open((_bstr_t)strSQL,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
				   adOpenForwardOnly, adLockReadOnly, adCmdText); 
		   }
		   catch(_com_error e)
		   {
			   ret=false;
		   }
			   if((!m_rs->BOF) && !(m_rs->adoEOF))
			   {
				   m_rs->get_Collect((_variant_t)_T("size3"),vT1);
				   int x = vtoi(vT1);
				   //如果管夹螺栓的M1的值比需要的L8的螺栓M大，			   
				   if(m_fDiaM1 > x)
				   {
						strSQLTmp.Format("SELECT * FROM [%s] WHERE [CUSTOMID]=\'%s\' AND [SIZE2]=%d ORDER BY SIZE2",
							modPHScal::tbnPART,modPHScal::sFindCustomID (_T("L7")),int(modPHScal::gmiDiameter));
						m_rs->Close();
	try
	{
//						m_rs.Open(dbOpenSnapshot,strSQLTmp);
						m_rs->Open((_bstr_t)strSQLTmp,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
							adOpenForwardOnly, adLockReadOnly, adCmdText); 
				   }
				   catch(_com_error e)
				   {
					   ret=false;
				   }
						m_rs->get_Collect((_variant_t)_T("size3"),vT1);
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
			   m_rs->Close();
			   strPipeClampBoltDiameterChanged = PtypeID[i];//保存ID值
			   rsTZB->put_Collect((_variant_t)_T("CLgg"),STR_VAR(sBH));
	   
		   }
		   else
		   {
			   rsTZB->put_Collect((_variant_t)_T("CLgg"),STR_VAR(sBHFormat));
		   }
	   }
	}
	rsTZB->put_Collect((_variant_t)_T("recno"),COleVariant((long)iSEQofPart));
	dbstr = dbstr + " recno= " + ltos(iSEQofPart);
	rsTZB->put_Collect((_variant_t)_T("VolumeID"),COleVariant(EDIBgbl::SelVlmID));
	dbstr = dbstr + " VolumeID= " + ltos(EDIBgbl::SelVlmID);
	rsTZB->put_Collect((_variant_t)_T("zdjh"),COleVariant((long)modPHScal::zdjh));
	dbstr = dbstr + " zdjh= " + ltos(modPHScal::zdjh);
	dbstr = dbstr + "\n";
//	AfxMessageBox(dbstr);
	rsTZB->put_Collect((_variant_t)_T("CustomID"),STR_VAR(Ptype[i]));
	rsTZB->put_Collect((_variant_t)_T("ClassID"),COleVariant((long)modPHScal::glClassID));
	//计算荷载写入结果表，便于校核计算
	rsTZB->put_Collect((_variant_t)_T("Pjs"),COleVariant((float)tmpSelPJG));
	//零件左视图第一路方向
	rsTZB->put_Collect((_variant_t)_T("fx"),COleVariant((long)mlfx));
	//零件y坐标方向
	rsTZB->put_Collect((_variant_t)_T("Rotation"),COleVariant((long)mlRot));
	if( sPartID == _T("LS") || sPartID == _T("LT") )
	{
		//矩形管道，高度值要加modPHScal::dwB/2
		//零件标注尺寸，先将sizeDIM也暂时置为零件实际尺寸
		rsX->get_Collect((_variant_t)_T("sizeH"), &vTmp1);
		rsTZB->put_Collect((_variant_t)_T("sizeDIM"),COleVariant((long)-(vtof(vTmp1) + modPHScal::dwB / 2)));
		//零件实际尺寸
		rsTZB->put_Collect((_variant_t)_T("sizeH"),COleVariant((double)-(vtof(vTmp1) + modPHScal::dwB / 2)));
	}
	else if( sPartID == _T("D4LQ") )
	{
		//矩形管道，高度值要加modPHScal::dwB/2
		//零件标注尺寸，先将sizeDIM也暂时置为零件实际尺寸
		rsX->get_Collect((_variant_t)_T("sizeH"), &vTmp1);
		rsTZB->put_Collect((_variant_t)_T("sizeDIM"),COleVariant((long)(vtof(vTmp1) + modPHScal::dwB / 2)));
		//零件实际尺寸
		rsTZB->put_Collect((_variant_t)_T("sizeH"),COleVariant((double)(vtof(vTmp1) + modPHScal::dwB / 2)));
	}
	else if( sPartID == _T("DLR") )
	{
		//矩形管道，高度值要加modPHScal::dwB/2
		//零件标注尺寸，先将sizeDIM也暂时置为零件实际尺寸
		rsX->get_Collect((_variant_t)_T("sizeH"), &vTmp1);
		rsX->get_Collect((_variant_t)_T("size4"), &vTmp2);
		rsTZB->put_Collect((_variant_t)_T("sizeDIM"),COleVariant((long)(vtof(vTmp1)+ modPHScal::dwB / 2 - vtof(vTmp2))));
		//零件实际尺寸
		rsTZB->put_Collect((_variant_t)_T("sizeH"),COleVariant((double)(long)(vtof(vTmp1)+ modPHScal::dwB / 2 - vtof(vTmp2))));
	}
	else if( sPartID == _T("LM2") || sPartID == _T("LM3") || sPartID == _T("LM4") )
	{
		//水平圆形管道连环吊架，高度值要加槽钢横担高度
		//零件标注尺寸，先将sizeDIM也暂时置为零件实际尺寸
		//rsTZB->put_Collect((_variant_t)_T("sizeDIM")) = -(IIf(IsNull(rsX.Fields(_T("size5"))), 0, rsX.Fields(_T("size5"))) + rsX.Fields(_T("size7")) * 10)
		rsX->get_Collect((_variant_t)_T("sizeH"), &vTmp1);
		rsTZB->put_Collect((_variant_t)_T("sizeDIM"),COleVariant((long)vtof(vTmp1)));
		//零件实际尺寸
		rsTZB->put_Collect((_variant_t)_T("sizeH"),COleVariant((double)vtoi(vTmp1)));
	}
	else if( sPartID == _T("LN2") || sPartID == _T("LN3") )
	{
		//水平圆形管道槽钢焊接连环吊架，高度值要加
		//零件标注尺寸，先将sizeDIM也暂时置为零件实际尺寸
		rsX->get_Collect((_variant_t)_T("sizeH"), &vTmp1);
		rsTZB->put_Collect((_variant_t)_T("sizeDIM"),COleVariant((long)vtof(vTmp1)));
		//零件实际尺寸
		rsTZB->put_Collect((_variant_t)_T("sizeH"),COleVariant((double)vtoi(vTmp1)));
	}
	else if(sPartID==_T("L15"))
	{
		//西北院固定支架Z2/Z2A下垫一块钢板L15,其厚度值保存在tbnAttachement表字段size2中或modPHScal::gfPSThickness
		//零件标注尺寸，先将sizeDIM也暂时置为零件实际尺寸
		if(PtypeID[i+1] == _T("GZD"))
		{
			//混凝土支墩，钢板埋入其中
			rsTZB->put_Collect((_variant_t)_T("sizeDIM"),COleVariant((long)0));
			//零件实际尺寸
			rsTZB->put_Collect((_variant_t)_T("sizeH"),COleVariant((double)0));
		}
		else
		{
			rsTZB->put_Collect((_variant_t)_T("sizeDIM"),COleVariant((long)modPHScal::gfPSThickness));
			//零件实际尺寸
			rsTZB->put_Collect((_variant_t)_T("sizeH"),COleVariant((double)modPHScal::gfPSThickness));
		}
	}
	else
	{
		if(modPHScal::glIDIndex==iSA)
		{
			//根部高度置为0,以便正确统计拉杆长度
			rsTZB->put_Collect((_variant_t)_T("sizeDIM"),COleVariant((long)0));
			//零件实际尺寸
			rsTZB->put_Collect((_variant_t)_T("sizeH"),COleVariant((double)0));
		}
		else
		{
			//零件标注尺寸，先将sizeDIM也暂时置为零件实际尺寸
			rsX->get_Collect((_variant_t)_T("sizeH"), &vTmp1);
			rsTZB->put_Collect((_variant_t)_T("sizeDIM"),COleVariant((long)vtof(vTmp1)));
			//零件实际尺寸
			rsTZB->put_Collect((_variant_t)_T("sizeH"),COleVariant((double)vtoi(vTmp1)));
		}
	}

	rsTZB->put_Collect((_variant_t)_T("CLnum"),COleVariant((long)iNumPart));
	rsTZB->put_Collect((_variant_t)_T("SEQ"),COleVariant((long)iSEQofPart));
	rsTZB->put_Collect((_variant_t)_T("CLbz"),vnil);

	_variant_t vTmp;
	if(rsID->Find((_bstr_t)(_T("trim(ID)=\'") + sPartID + _T("\'")), 0, adSearchForward, vTmp))
	{
		rsID->get_Collect((_variant_t)_T("Description"), &vTmp1);
		rsTZB->put_Collect((_variant_t)_T("CLmc"),vTmp1);
	}
	//rsTZB->put_Collect((_variant_t)_T("CLmc")) = sFindFLD(_T("ID"), _T("Description"), sPartID)
	rsX->get_Collect((_variant_t)_T("Weight"), &vTmp1);
	if( sPartID == _T("LHG") )
		modPHScal::g_fLHGWeight = vtof(vTmp1);
	if( sPartID == _T("T5") )//T5和LHG不会同时出现，因为弹簧和恒力弹簧不会同时出现
		modPHScal::g_fLHGWeight = vtof(vTmp1);
	rsTZB->put_Collect((_variant_t)_T("CLdz"),COleVariant(vtof(vTmp1)));
	rsTZB->get_Collect((_variant_t)_T("CLnum"), &vTmp2);
	rsTZB->put_Collect((_variant_t)_T("CLzz"),COleVariant(vtof(vTmp1)* vtof(vTmp2)));

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
					rsX->get_Collect((_variant_t)_T("sizeC"), &vTmp1);
					modPHScal::DblHangerRodDist =vtof(vTmp1);
					rsTZB->put_Collect((_variant_t)_T("sizeC"),COleVariant(modPHScal::DblHangerRodDist));
				}
				else
				{
					if(modPHScal::glClassID == iPAs )
					{
						//双支
						rsX->get_Collect((_variant_t)_T("size3"), &vTmp1);
						modPHScal::DblHangerRodDist = vtof(vTmp1);
						rsTZB->put_Collect((_variant_t)_T("sizeC"),COleVariant(modPHScal::DblHangerRodDist));
					}
					else
					{
						if(modPHScal::glClassID == iPAD11LD ||modPHScal::glClassID == iPADLR || modPHScal::glClassID == iPALX )
						{
							//与管径有关的双吊间距
							rsX->get_Collect((_variant_t)_T("sizeC"), &vTmp1);
							modPHScal::DblHangerRodDist = vtof(vTmp1) + modPHScal::dj;
							rsTZB->put_Collect((_variant_t)_T("sizeC"),COleVariant(modPHScal::DblHangerRodDist));
						}
						else
						{
							//其它类零件保存中心距，如T5,LHG等
							rsX->get_Collect((_variant_t)_T("sizeC"), &vTmp1);
							rsTZB->put_Collect((_variant_t)_T("sizeC"),COleVariant(vtof(vTmp1)));
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
			rsTZB->put_Collect((_variant_t)_T("C"),(vTmp1));
		}
		else if(modPHScal::glClassID==iG56 || modPHScal::glClassID==iG57)
		{
			//G56,57的柱子宽度保存在原始数据库中，它=双槽钢的间距
			vTmp1=rsza->GetCollect(_T("A01"));
			COleVariant vTmp2;
			//柱子与槽钢的间隙值m,一般=5
			rsX->get_Collect((_variant_t)_T("m"), &vTmp2);
			vTmp2.dblVal = vTmp2.dblVal * 2 + vTmp1.dblVal;
			rsTZB->put_Collect((_variant_t)_T("C"), vTmp2);
		}
		else
		{
			rsX->get_Collect((_variant_t)_T("CHDIST"), &vTmp1);
			rsTZB->put_Collect((_variant_t)_T("C"),(vTmp1));
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
	rsTZB->put_Collect((_variant_t)_T("Index"),COleVariant((long)modPHScal::glIDIndex));
	rsTmpZB->get_Collect((_variant_t)_T("CustomID"), &vTmp1);

	if( rsID->Find((_bstr_t)(_T("trim(CustomID)=\'") + vtos(vTmp1) + _T("\'")), 0, adSearchForward, vTmp) )
	{
		rsID->get_Collect((_variant_t)_T("ID"), &vTmp1);
		rsTZB->put_Collect((_variant_t)_T("ID"),vTmp1);
	}
	   CString strBH,strID;
	   _RecordsetPtr rsTmp2;
	   switch(modPHScal::glClassID)
	   {
	   case iConnected:
		   //非拉杆的连接件
		   if( sPartID.Left(2)== _T("L5") ||sPartID.Left(2)== _T("L6") ||sPartID.Left(2)== _T("L7") ||sPartID.Left(2)== _T("L8"))
		   {
			   rsX->get_Collect((_variant_t)_T("size4"), &vTmp1);
			   rsTZB->put_Collect((_variant_t)_T("AllowedMinLugExt"),(vTmp1));
			   rsX->get_Collect((_variant_t)_T("size5"), &vTmp1);
			   rsTZB->put_Collect((_variant_t)_T("AllowedMaxLugExt"), (vTmp1));
			   //第二次计算,记录螺纹直径,以便找出最大值，所有的弹簧/恒吊松紧螺母约束到这个直径
			   if( FirstCal == 1 )
			   {
				   rsX->get_Collect((_variant_t)_T("size2"), &vTmp1);
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
   
		   rsTZB->Update();  //?,每个case语句后都有这个更新语句
		   break;
	   case iROD:
		   //拉杆
		   //保存拉杆两端的螺纹长度
		   //L1的右螺纹，L2的左螺纹
		   rsX->get_Collect((_variant_t)_T("size4"), &vTmp1);
		   rsTZB->put_Collect((_variant_t)_T("AllowedMinLugExt"), (vTmp1));
		   //L1的右螺纹，L2的右螺纹
		   rsX->get_Collect((_variant_t)_T("size3"), &vTmp1);
		   rsTZB->put_Collect((_variant_t)_T("AllowedMaxLugExt"), (vTmp1));
		   //拉杆，获得直径
		   rsX->get_Collect((_variant_t)_T("size2"), &vTmp1);
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
		   rsTZB->put_Collect((_variant_t)_T("sizeC"),COleVariant(modPHScal::giDiameter));
		   //拉杆直径保存到数组，以便过程末尾查找附件时使用。主要处理附件记录位于拉杆之前的这种情况，所有拉杆始端的附件都是这种情况。
		   //sizeH置为0便于后面统计总长
		   rsTZB->put_Collect((_variant_t)_T("sizeH"),COleVariant((long)0));
		   //暂时借用GDW1字段保留拉杆单重(kg/米),用于以后将拉杆作为零件计算整体单重。
		   rsX->get_Collect((_variant_t)_T("Weight"), &vTmp1);
		   rsTZB->put_Collect((_variant_t)_T("GDW1"),vTmp1);
		   rsTZB->Update();  //?,每个case语句后都有这个更新语句
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
			   rsTZB->put_Collect((_variant_t)_T("CLcl"),STR_VAR(GetResStr(IDS_GROUPWARE)));
			   j = 1; //主型钢
			   CString sj;
			   sj.Format(_T("%d"),j);
			   rsX->get_Collect((_variant_t)(_T("P")+sj), &vTmp1);
			   CString sTmp3=vtos(vTmp1);
			   sTmp3=sTmp3.Mid(1);
			   sTmp3.Format(_T("%g"),_tcstod(sTmp3,NULL)*10);
			   rsX->get_Collect((_variant_t)(_T("Pnum")+sj), &vTmp2);
			   sTmp=vtos(vTmp1);
			   //Pnum1=NULL或0，根部主型钢数量错误
			   if( sTmp!=_T("") &&  vtoi(vTmp2) > 0 )
				   //if( sTmp!=_T("") )			
			   {
				   if(modPHScal::glClassID == iSAh )
				   {
					   //直接吊G11
					   rsX->get_Collect((_variant_t)(_T("PL")+sj), &vTmp1);
					   sTmp=vtos(vTmp1);
					   if( sTmp!= _T("") )
					   {
						   rsX->get_Collect((_variant_t)_T("BH"), &vTmp1);
						   EDIBgbl::dbPRJ->Execute((_bstr_t)(CString(_T("UPDATE tmpCSLen SET BH=\'")) + vtos(vTmp1) + _T("\'")), NULL, adCmdText);
						   rsTZB->put_Collect((_variant_t)_T("BH"),vTmp1);
						   rsX->get_Collect((_variant_t)_T("A"), &vTmp1);
						   sTmp2.Format(_T("%g"),vtof(vTmp1));						
						   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET a=") +sTmp2), NULL, adCmdText);
						   //G11的公式为sizeH+A,其中sizeH=螺杆长度,长度储存在A01字段。
						   sTmp2.Format(_T("%g"),modPHScal::WidthA);
						   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET sizeH=") + sTmp2), NULL, adCmdText);
						   //暂时只要计算主零件的长度
						   sngCSLen = modPHScal::CSLength(sBHFormat, sTmp, sBH);
					   }
					   rsTZB->put_Collect((_variant_t)_T("sizeH"),COleVariant((float)modPHScal::WidthA));
					   rsTZB->put_Collect((_variant_t)_T("sizeDIM"),COleVariant((float)modPHScal::WidthA));
				   }
				   else
				   {
					   //非直接吊G11,即槽钢
					   rsX->get_Collect((_variant_t)(_T("PL")+sj), &vTmp1);
					   sTmp=vtos(vTmp1);
					   if( sTmp!= _T("") )
					   {
						   rsX->get_Collect((_variant_t)_T("BH"), &vTmp1);
						   EDIBgbl::dbPRJ->Execute((_bstr_t)(CString(_T("UPDATE tmpCSLen SET BH=\'")) + vtos(vTmp1) + _T("\'")), NULL, adCmdText);                           
						   rsTZB->put_Collect((_variant_t)_T("BH"),vTmp1);
						   //G100自定义根部的主型钢型号P1写入tmpCSLen，以便作为材料规格号.
						   rsX->get_Collect((_variant_t)_T("P1"), &vTmp1);
						   EDIBgbl::dbPRJ->Execute((_bstr_t)(CString(_T("UPDATE tmpCSLen SET P1=\'")) + vtos(vTmp1) + _T("\'")), NULL, adCmdText);                           
				   
						   rsX->get_Collect((_variant_t)_T("A"), &vTmp1);
						   double fLenA=0;//记录悬臂端部到拉杆受力点之间的距离值A  Add by luorijin 2008.10.30
						   fLenA = vtof(vTmp1);
						   sTmp2.Format(_T("%g"),vtof(vTmp1));
						   if(modPHScal::glClassID==iG52_55)
						   {
							   //G52_55的公式为H+h+100,其中H=板肋高度,长度储存在A01字段。
							   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET CHLegHeight=")+sTmp3 ), NULL, adCmdText);
							   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET CHheight=") +sTmp3), NULL, adCmdText);					   
						   }
						   else
						   {
							   //G47的双槽钢间距chdist=300,G48的双槽钢间距=Bwidth(柱子宽度)
					   
						   }
				   
						   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET a=") +sTmp2), NULL, adCmdText);
						   rsX->get_Collect((_variant_t)_T("c"), &vTmp1);
						   sTmp2.Format(_T("%g"),vtof(vTmp1));
						   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET c=") +sTmp2), NULL, adCmdText);
						   sTmp2.Format(_T("%g"),vtof(rsza->GetCollect(_T("B01"))));
						   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET B=") +sTmp2), NULL, adCmdText);
						   sTmp2.Format(_T("%g"),vtof(rsza->GetCollect(_T("A01"))));
						   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET BWidth=") +sTmp2), NULL, adCmdText);
						   rsTZB->put_Collect((_variant_t)_T("B"),rsza->GetCollect(_T("B01")));
						   rsTZB->put_Collect((_variant_t)_T("A"),rsza->GetCollect(_T("A01")));
				   
						   //H=生根梁的高度
						   sTmp2.Format(_T("%g"),vtof(rsza->GetCollect(_T("A01"))));
						   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET H=") +sTmp2), NULL, adCmdText);
				   
				   
						   rsX->get_Collect((_variant_t)_T("m"), &vTmp1);
						   sTmp2.Format(_T("%g"),vtof(vTmp1));
						   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET m=") +sTmp2), NULL, adCmdText);
						   rsX->get_Collect((_variant_t)_T("CHdist"), &vTmp1);
						   sTmp2.Format(_T("%g"),vtof(vTmp1));
						   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET CHDist=") +sTmp2), NULL, adCmdText);
						   sTmp2.Format(_T("%g"),modPHScal::Lspan);
						   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET Lspan=") +sTmp2), NULL, adCmdText);
						   sTmp2.Format(_T("%g"),modPHScal::gdw);
						   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET GDW1=") +sTmp2), NULL, adCmdText);

						   sTmp2.Format(_T("%g"),modPHScal::Lspan);
						   EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET L1=") +sTmp2), NULL, adCmdText);
				   
						   //把简支梁跨度l1写到ZB 表LSpan字段，以方便三维使用，并且便于记录非标根部
						   rsTZB->put_Collect((_variant_t)_T("LSpan"),COleVariant((float)modPHScal::Lspan));
						   rsTZB->put_Collect((_variant_t)_T("XBL1"),COleVariant((float)(modPHScal::gdw+fLenA)));//XBL1记录悬臂长度  Add by luorijin 2008.10.30
						   //暂时只要计算主型钢的长度
						   sngCSLen = modPHScal::CSLength(sBHFormat, sTmp, sBH);
					   }
					   //槽刚的a值保存到字段T,该值在绘制根部定位尺寸时很重要。如G22~G24,G3x,G6x,G7x类
					   //if( (sPartID.Left(2) == _T("G3")) || (sPartID.Left(2) ==  _T("G6") ) || (sPartID.Left(2) == _T("G7")) || (sPartID.Left(2) == _T("G5")) && (sPartID.Left(3) != _T("G51"))  && (sPartID.Left(3) != _T("G56"))  && (sPartID.Left(3) != _T("G57")) )
					   if( (modPHScal::glClassID==iSAbeam) || (modPHScal::glClassID==iG52_55 ) )
						   rsTZB->put_Collect((_variant_t)_T("T"),COleVariant((float)(modPHScal::Lspan - modPHScal::gdw)));
					   else
					   {
						   rsX->get_Collect((_variant_t)_T("A"), &vTmp1);
						   rsTZB->put_Collect((_variant_t)_T("T"), (vTmp1));
					   }
			   
					   rsTZB->put_Collect((_variant_t)_T("L1"),COleVariant((float)sngCSLen));
					   rsTZB->put_Collect((_variant_t)_T("GDW1"),COleVariant(modPHScal::gdw));
					   //更新原始数据表TZA中的根部主槽钢数量
					   rsX->get_Collect((_variant_t)_T("Pnum1"), &vTmp1);
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
				   rsTZB->put_Collect((_variant_t)_T("CLgg"),STR_VAR(sBH));
				   //rsTZB->put_Collect((_variant_t)_T("BH"),STR_VAR(sBH));
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
					  rsX->get_Collect((_variant_t)_T("BH"), &vTmp1);
					  sTmp2.Format(GetResStr(IDS_SAP1MustBeChannelSteelNo),modPHScal::tbnSA,vtos(vTmp1));
					  //frmStatus.MessageBox(sTmp2);
					  ShowMessage(sTmp2);
				  }
			  }
		  }
  
		  rsTZB->Update();
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
			  rsX->get_Collect((_variant_t)(_T("P")+sj), &vTmp1);
			  rsX->get_Collect((_variant_t)(_T("Pnum")+sj), &vTmp2);
			  sTmp=vtos(vTmp1);
			  if( sTmp!=_T("") && vtoi(vTmp2)>0 )
			  {
				  mvSAattachedCustomID =sTmp;
				  //看是否是型钢:槽钢、角钢、工字钢、圆钢、扁钢？（钢板作为附件处理）
				  //扁钢(在根部表和型钢表中)的存放格式应为：-宽x厚，如-90x16。
				  SQL1 = _T("SELECT * FROM [") + modPHScal::tbnSectionSteel + _T("] WHERE trim(BH)=\'") + mvSAattachedCustomID + _T("\' ORDER BY bh");
				  if(rsTmp->State == adOpenStatic)
					  rsTmp->Close();
	try
	{
//				  rsTmp.m_pDatabase=&modPHScal::dbZDJcrude;
//				  rsTmp.Open(dbOpenSnapshot,SQL1);
				  rsTmp->Open((_bstr_t)SQL1,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
					  adOpenForwardOnly, adLockReadOnly, adCmdText); 
			  }
			  catch(_com_error e)
			  {
				  ret=false;
			  }
				  if( rsTmp->adoEOF && rsTmp->BOF )
				  {
					  //不是型钢
					  //****************************
					  //找不到,则是附件,再查附件表
					  rsTmp->Close();
					  SQL1 = _T("SELECT * FROM [") + modPHScal::tbnAttachment + _T("] WHERE trim(BH)=\'") + mvSAattachedCustomID + _T("\' ORDER BY bh");
					  /*if(rsTmp->State == adOpenStatic)
					  rsTmp->Close();
	try
	{
					  rsTmp.m_pDatabase=&modPHScal::dbZDJcrude;*/
//					  rsTmp.Open(dbOpenSnapshot,SQL1);
	try
	{
					  rsTmp->Open((_bstr_t)SQL1,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
						  adOpenForwardOnly, adLockReadOnly, adCmdText); 
				  }
				  catch(_com_error e)
				  {
					  ret=false;
				  }
					  if( rsTmp->adoEOF && rsTmp->BOF )
					  {
						  //不是附件,则是螺栓螺母,再查螺栓螺母表
						  //螺栓螺母：根部使用的可能是特定标准的螺栓螺母，而配件可能使用其它标准的螺栓螺母，如国家标准螺栓螺母。
						  //其编号方式为M36x200,或M20
						  //因此，首先查找根部螺栓螺母表tbnSABoltsnuts，再根据查出的规格从通用螺栓螺母表tbnBoltsNutsID中查找相应的规格。
						  rsTmp->Close();
						  SQL1 = _T("SELECT * FROM [") + modPHScal::tbnSABoltsNuts + _T("] WHERE trim(BH)=\'") + mvSAattachedCustomID + _T("\' ORDER BY bh");
						  /*if(rsTmp->State == adOpenStatic)
						  rsTmp->Close();
						  rsTmp.m_pDatabase=&modPHScal::dbZDJcrude;*/
//						  rsTmp.Open(dbOpenSnapshot,SQL1);
	try
	{
						  rsTmp->Open((_bstr_t)SQL1,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
							  adOpenForwardOnly, adLockReadOnly, adCmdText); 
					  }
					  catch(_com_error e)
					  {
						  ret=false;
					  }
						  if( rsTmp->adoEOF && rsTmp->BOF )
						  {
							  //没找到螺栓螺母记录
							  sBH = _T("");
							  strBH = _T("");
							  sngWeight = 0;
							  mviSASSClassID = iSABoltsNutsUnknown;
							  //rsTmp->Close();
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
							  rsTmp->MoveFirst();
							  rsTmp->get_Collect((_variant_t)_T("BH"), &vTmp1);
							  sBH = vtos(vTmp1);
							  strBH=sBH;
							  sTmp=CString(_T("UPDATE tmpCSLen SET BH=\'")) + sBH + _T("\'");
							  EDIBgbl::dbPRJ->Execute((_bstr_t)(sTmp), NULL, adCmdText);
							  rsTmp->get_Collect((_variant_t)_T("CustomID"), &vTmp1);
							  tmpCustomID = vtos(vTmp1);
							  sTmp= CString(_T("UPDATE tmpCSLen SET CustomID=\'")) + tmpCustomID + _T("\'");
							  EDIBgbl::dbPRJ->Execute((_bstr_t)(sTmp), NULL, adCmdText);
					  
							  rsTmp->get_Collect((_variant_t)_T("size2"), &vTmp1);
							  tmpSize2 =vtof(vTmp1);
							  rsTmp->get_Collect((_variant_t)_T("sizeH"), &vTmp1);
							  tmpSizeH =vtof(vTmp1);
							  tmpID=modPHScal::sFindID(tmpCustomID);
							  if(tmpID==_T("F9") || tmpID==_T("F10"))
							  {
								  if(modPHScal::glClassID==iG56 || modPHScal::glClassID==iG57 || modPHScal::glClassID==iG52_55 || modPHScal::glClassID==iG51)
								  {
									  rsX->get_Collect((_variant_t)(_T("PL")+sj), &vTmp1);
									  sTmp=vtos(vTmp1);
									  if(rsID->Find((_bstr_t)(_T("trim(CustomID)=\'") + tmpCustomID + _T("\'")), 0, adSearchForward, vTmp))
									  {
										  rsID->get_Collect((_variant_t)_T("BHFormat"), &vTmp1);
										  sBHFormat = vtos(vTmp1);
									  }
									  else
									  {
									  }
									  sTmp2.Format(_T("%g"),vtof(rsza->GetCollect(_T("A01"))));
									  EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET BWidth=") +sTmp2), NULL, adCmdText);								
									  sTmp2.Format(_T("%g"),vtof(rsza->GetCollect(_T("A01"))));
									  EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET H=") +sTmp2), NULL, adCmdText);
									  sTmp2.Format(_T("%g"),vtof(rsza->GetCollect(_T("B01"))));
									  EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET B=") +sTmp2), NULL, adCmdText);								
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
								  rsTmp->Close();
	try
	{
	//							  rsTmp.Open(dbOpenSnapshot,SQL1);
								rsTmp->Open((_bstr_t)SQL1,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
								  adOpenForwardOnly, adLockReadOnly, adCmdText); 
						  }
						  catch(_com_error e)
						  {
							  ret=false;
						  }
								  if( rsTmp->adoEOF && rsTmp->BOF )
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
									  rsTmp->MoveFirst();
									  if(tmpID==_T("F9") || tmpID==_T("F10"))
									  {
										  //F9、F10已经查出了直径和长度,只要在通用螺栓螺母表中查出它们的CustomID和BH,再生成CLgg，就可以了。
										  rsTmp->get_Collect((_variant_t)_T("BH"), &vTmp1);
										  sBH = vtos(vTmp1);
										  sTmp= CString(_T("UPDATE tmpCSLen SET BH=\'")) + sBH + _T("\'");
										  EDIBgbl::dbPRJ->Execute((_bstr_t)(sTmp), NULL, adCmdText);
								  
										  rsID->get_Collect((_variant_t)_T("BHFormat"), &vTmp1);
										  sBHFormat = vtos(vTmp1);															    
										  sngCSLen = modPHScal::CSLength(sBHFormat, ftos(sngCSLen), sBH);
									  }
									  else if(tmpID==_T("F14"))
									  {
										  rsTmp->get_Collect((_variant_t)_T("BH"), &vTmp1);
										  sBH = vtos(vTmp1);
										  sngCSLen = 1000;  //设置为1000,便于编程.
									  }						
									  else
									  {
										  rsTmp->get_Collect((_variant_t)_T("BH"), &vTmp1);
										  sBH = vtos(vTmp1);
										  sngCSLen = 1000;  //设置为1000,便于编程.
									  }
									  rsTmp->get_Collect((_variant_t)_T("Weight"), &vTmp1);
									  sngWeight =vtof(vTmp1)*sngCSLen/1000;
									  sngW = sngWeight;
									  rsTmp->get_Collect((_variant_t)_T("CustomID"), &vTmp1);
									  tmpCustomID = vtos(vTmp1);                    
									  if(rsID->Find((_bstr_t)(_T("trim(CustomID)=\'") + tmpCustomID + _T("\'")), 0, adSearchForward, vTmp))
									  {
										  rsID->get_Collect((_variant_t)_T("ClassID"), &vTmp1);
										  mviSASSClassID =vtoi(vTmp1);
										  rsID->get_Collect((_variant_t)_T("Index"), &vTmp1);
										  mviSASSIndex =vtoi(vTmp1);
									  }
								  }
						  }
					   }
					   else
					   {
						   //查附件表找到附件,不是螺栓螺母,
						   strBH=_T("");
						   rsTmp->MoveFirst();
						   rsTmp->get_Collect((_variant_t)_T("BH"), &vTmp1);
						   sBH = vtos(vTmp1);
						   rsTmp->get_Collect((_variant_t)_T("Weight"), &vTmp1);
						   sngWeight =vtof(vTmp1);
						   sngW = sngWeight;
						   rsTmp->get_Collect((_variant_t)_T("CustomID"), &vTmp1);
						   tmpCustomID = vtos(vTmp1);
						   tmpID=modPHScal::sFindID(tmpCustomID);
						   if(rsID->Find((_bstr_t)(_T("trim(CustomID)=\'") + tmpCustomID + _T("\'")), 0, adSearchForward, vTmp))
						   {
							   rsID->get_Collect((_variant_t)_T("ClassID"), &vTmp1);
							   mviSASSClassID =vtoi(vTmp1);
							   rsID->get_Collect((_variant_t)_T("Index"), &vTmp1);
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
							if( rsTmp->adoEOF && rsTmp->BOF )
							{
								//没有这种型钢规格
								sTmp.Format(GetResStr(IDS_NothisSectionSteelInZDJcrudeMdb),modPHScal::dbZDJcrude->DefaultDatabase,mvSAattachedCustomID);
								throw sTmp;
							}
							else
							{
								//有这种型钢规格
								rsTmp->MoveFirst();
								rsTmp->get_Collect((_variant_t)_T("CustomID"), &vTmp1);
								tmpCustomID =vtos(vTmp1);
								rsTmp->get_Collect((_variant_t)_T("Material"), &vTmp1);
								mvsSASSMaterial = vtos(vTmp1);
						
								if( rsID->Find((_bstr_t)(_T("trim(CustomID)=\'") + tmpCustomID + _T("\'")), 0, adSearchForward, vTmp))
								{
									rsID->get_Collect((_variant_t)_T("ClassID"), &vTmp1);
									mviSASSClassID = vtoi(vTmp1);
									rsID->get_Collect((_variant_t)_T("Index"), &vTmp1);
									mviSASSIndex =vtoi(vTmp1);
									//暂时保存根部主型钢高度，用于以后确定恒吊与根部连接的螺栓的高度。
									if( j == 1 )
									{
										rsTmp->get_Collect((_variant_t)_T("sizeH"), &vTmp1);
										SACSHeight =vtof(vTmp1);
									}
								}
							}
							rsX->get_Collect((_variant_t)(_T("PL")+sj), &vTmp1);
							sTmp=vtos(vTmp1);
							if(sTmp!=_T("") )
							{
								rsTmp->get_Collect((_variant_t)_T("BH"), &vTmp1);
								EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET BH=\'") + vtos(vTmp1) + _T("\'")), NULL, adCmdText);
								vBH = vTmp1;			
								rsTmp->get_Collect((_variant_t)_T("CustomID"), &vTmp1);
								EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET CustomID=\'")+vtos(vTmp1)+_T("\'")), NULL, adCmdText);
								rsX->get_Collect((_variant_t)_T("A"), &vTmp1);
								sTmp2.Format(_T("%g"),vtof(vTmp1));
								EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET a=") +sTmp2), NULL, adCmdText);
								rsX->get_Collect((_variant_t)_T("c"), &vTmp1);
								sTmp2.Format(_T("%g"),vtof(vTmp1));
								EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET c=") +sTmp2), NULL, adCmdText);
								if( modPHScal::glClassID==iG52_55 || modPHScal::glClassID==iG51)
								{
									sTmp2.Format(_T("%g"),vtof(rsza->GetCollect(_T("A01"))));
									EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET H=") +sTmp2), NULL, adCmdText);
								}
								else
								{
									sTmp2.Format(_T("%g"),vtof(rsza->GetCollect(_T("A01"))));
									EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET BWidth=") +sTmp2), NULL, adCmdText);
								}
								sTmp2.Format(_T("%g"),vtof(rsza->GetCollect(_T("B01"))));
								EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET B=") +sTmp2), NULL, adCmdText);
								rsX->get_Collect((_variant_t)_T("m"), &vTmp1);
								sTmp2.Format(_T("%g"),vtof(vTmp1));
								EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET m=") +sTmp2), NULL, adCmdText);
								rsX->get_Collect((_variant_t)_T("CHdist"), &vTmp1);
								sTmp2.Format(_T("%g"),vtof(vTmp1));
								EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET CHDist=") +sTmp2), NULL, adCmdText);

								rsTmp->get_Collect((_variant_t)_T("sizeH"), &vTmp1);
								if (vtos(vTmp1)==_T(""))
								{
									sTmp2 = _T("UPDATE tmpCSLen SET CHHeight=0");
								} else {
									sTmp2 = _T("UPDATE tmpCSLen SET CHHeight=")+vtos(vTmp1);
								}
								EDIBgbl::dbPRJ->Execute((_bstr_t)sTmp2, NULL, adCmdText);

								if (vtos(vTmp1)==_T(""))
								{
									sTmp2 = _T("UPDATE tmpCSLen SET CHLegHeight=0");
								} else {
									sTmp2 = _T("UPDATE tmpCSLen SET CHLegHeight=")+vtos(vTmp1);
								}
								EDIBgbl::dbPRJ->Execute((_bstr_t)sTmp2, NULL, adCmdText);
								sTmp2.Format(_T("%g"),modPHScal::gdw);
								EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET GDW1=") +sTmp2), NULL, adCmdText);
								sTmp2.Format(_T("%g"),modPHScal::Lspan);
								EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET Lspan=") +sTmp2), NULL, adCmdText);
								sTmp2.Format(_T("%g"),modPHScal::Lspan);
								EDIBgbl::dbPRJ->Execute((_bstr_t)(_T("UPDATE tmpCSLen SET [L1]= ") +sTmp2), NULL, adCmdText);
								sngCSLen = modPHScal::CSLength(sBHFormat, sTmp, sBH);
							}
							//sBH中含有无效值,对于型钢不如将其置为规格更有意义
							sBH = mvSAattachedCustomID;
							//型钢重量=每米重量x长度(mm)/1000
							rsTmp->get_Collect((_variant_t)_T("Weight"), &vTmp1);
							sngWeight =vtof(vTmp1);
							sngW = sngWeight * sngCSLen / 1000;
						}
					}
			
					rsX->get_Collect((_variant_t)(_T("Pnum")+sj), &vTmp1);
					//根部整体重量=根部每个附件的重量之和kg
					mvarSATotalWeight = mvarSATotalWeight + sngW * vtof(vTmp1);
					rsSAPart->AddNew();
			
					rsSAPart->put_Collect((_variant_t)_T("VolumeID"),COleVariant(EDIBgbl::SelVlmID));
					rsSAPart->put_Collect((_variant_t)_T("zdjh"),COleVariant((long)modPHScal::zdjh));
					//2002.05.26以后改写此段代码，以与phs.arx思路一致。
					//是根部附件,则写记录号,
					//以便区分根部附件与部件.也便于对零件编号。
					iSEQofPart++;
					rsSAPart->put_Collect((_variant_t)_T("recno"),COleVariant((long)iSEQofPart));
					//因为根部附件与根部的区分标志是IsSAPart字段是否=-1。
					rsSAPart->put_Collect((_variant_t)_T("IsSAPart"),COleVariant((long)-1));
					//rsSAPart->put_Collect((_variant_t)_T("Index")) = iSA
					rsSAPart->put_Collect((_variant_t)_T("nth"),COleVariant((short)nth));//标准支吊架路数=1，包括对称双吊和共用根部双吊
					rsSAPart->put_Collect((_variant_t)_T("ClassID"),COleVariant((long)mviSASSClassID));
					rsSAPart->put_Collect((_variant_t)_T("Index"),COleVariant((long)mviSASSIndex));
					rsTmp->get_Collect((_variant_t)_T("CustomID"), &vTmp1);
					rsSAPart->put_Collect((_variant_t)_T("CustomID"),STR_VAR(vtos(vTmp1)));
					//sBH				
					rsSAPart->put_Collect((_variant_t)_T("CLgg"),STR_VAR(sBH));
					if(tmpID=="F9" || tmpID=="F10" || tmpID=="F14")
					{				
						rsSAPart->put_Collect((_variant_t)_T("BH"),STR_VAR(strBH));
						//根部螺栓或螺杆的长度，
						rsSAPart->put_Collect((_variant_t)_T("L1"),COleVariant( sngCSLen));
					}
					else
					{
				
						if(mviSASSIndex==iSectionSteel )
						{
							//根部型钢的长度，
							rsSAPart->put_Collect((_variant_t)_T("L1"),COleVariant( sngCSLen));
							//rsSAPart->put_Collect((_variant_t)_T("GDW1"),COleVariant(modPHScal::gdw));
							rsSAPart->put_Collect((_variant_t)_T("BH"),STR_VAR(sBH));
						}
						else
							rsSAPart->put_Collect((_variant_t)_T("BH"),STR_VAR(sBH));
					}
			
					rsTmp->get_Collect((_variant_t)_T("Material"), &vTmp1);
			
					rsSAPart->put_Collect((_variant_t)_T("CLcl"),STR_VAR(vtos(vTmp1)));
					rsX->get_Collect((_variant_t)(_T("Pnum")+sj), &vTmp1);
					rsSAPart->put_Collect((_variant_t)_T("CLnum"),vTmp1);
					rsSAPart->put_Collect((_variant_t)_T("CLbz"),vnil);
					rsSAPart->put_Collect((_variant_t)_T("CLdz"),COleVariant(sngWeight));
					rsX->get_Collect((_variant_t)(_T("Pnum")+sj), &vTmp1);
					rsSAPart->put_Collect((_variant_t)_T("CLzz"),COleVariant(sngW * vtof(vTmp1)));
					rsID->get_Collect((_variant_t)_T("Description"), &vTmp1);
					rsSAPart->put_Collect((_variant_t)_T("CLmc"),vTmp1);
					rsSAPart->Update();
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
				   rsTZB->put_Collect((_variant_t)_T("CLcl"),STR_VAR(GetResStr(IDS_GROUPWARE)));
				   //Height_SPRINGsL5 = IIf(IsNull(rsX(_T("size3"))), 0, rsX(_T("size3")))
				   if(sPartID==_T("T4"))
				   {
					   rsX->get_Collect((_variant_t)_T("sizeH"), &vTmp1);
					   sngDim = vtof(vTmp1) - /*Sgn(modPHScal::yr) */ modPHScal::sSprInfo[iSEQofSPR].haz;//z.jia add
					   //rsTZB->put_Collect((_variant_t)_T("Size")) = Height_SPRINGsL5 / 2
				   }
				   else if(sPartID==_T("T3") || sPartID==_T("T3A"))
				   {
					   rsX->get_Collect((_variant_t)_T("sizeH"), &vTmp1);
					   sngDim = fabs(vtof(vTmp1)) - modPHScal::sSprInfo[iSEQofSPR].haz;
					   //上/下方弹簧，其值在计算拉杆时应该使拉杆加长
					   sngDim = -sngDim;
				   }
				   //rsTZB->put_Collect((_variant_t)_T("Size")) = Height_SPRINGsL5 / 2
				   else
				   {
					   rsX->get_Collect((_variant_t)_T("sizeH"), &vTmp1);
					   sngDim = vtof(vTmp1) + modPHScal::sSprInfo[iSEQofSPR].haz;
				   }
				   //弹簧的尺寸都是按松紧螺母外缘给的。且按最小位移时给出高度。
				   //配套不同的松紧螺母时应该换算不同的高度。此种处理程序暂不提供。
				   //弹簧的松紧螺母在CalLugLength中查表
				   rsTZB->put_Collect((_variant_t)_T("SizeH"),COleVariant(sngDim));
				   rsTZB->put_Collect((_variant_t)_T("sizeDIM"),COleVariant(sngDim));
				   //rsTZB->put_Collect((_variant_t)_T("CLgg")) = sBHformat
				   if( EDIBAcad::g_bBomSprFactory )//newHS->只有选择了标注弹簧厂家才写入数据库
				   {
					   rsTZB->put_Collect((_variant_t)_T("CLbz"),STR_VAR(modPHScal::gsSPRmanufactory));
				   }
				   rsTZB->put_Collect((_variant_t)_T("SpringNo"), COleVariant( (long)modPHScal::sSprInfo[iSEQofSPR].DH) );
				   rsTZB->put_Collect((_variant_t)_T("LugDia"), COleVariant(modPHScal::sSprInfo[iSEQofSPR].FiJ));
				   rsTZB->put_Collect((_variant_t)_T("HeatDisp"), COleVariant(modPHScal::sSprInfo[iSEQofSPR].HeatDisp));
				   rsTZB->put_Collect((_variant_t)_T("HAZ"), COleVariant(modPHScal::sSprInfo[iSEQofSPR].haz));
		   
				   rsTZB->Update();
			   }
			   break;
		   case iCSPR:
			   {
				   rsTZB->put_Collect((_variant_t)_T("CLcl"),STR_VAR(GetResStr(IDS_GROUPWARE)));
				   //记录原始尺寸T，便于过程GetBoltsNutsAndAttachmentsCLgg准确计算与根部连接的螺栓长度
				   rsX->get_Collect((_variant_t)_T("T"), &vTmp1);
				   rsTZB->put_Collect((_variant_t)_T("T"),COleVariant((long)vtoi(vTmp1)));
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
		   
				   rsDiaOfCSPRFiJ->get_Collect((_variant_t)_T("FiJ"), &vTmp1);
				   //SQLx.Format(_T("SELECT * FROM [%s] WHERE size2=%g"),modPHScal::tbnCSPRINGL5Crude,vtof(vTmp1));
				   if(rsTmp->State == adOpenStatic)
					   rsTmp->Close();
	try
	{
	//			   rsTmp.m_pDatabase=&modPHScal::dbZDJcrude;
	//			   rsTmp.Open(dbOpenSnapshot,SQLx);
				   rsTmp->Open((_bstr_t)SQLx,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
					   adOpenForwardOnly, adLockReadOnly, adCmdText); 
			   }
			   catch(_com_error e)
			   {
				   ret=false;
			   }
				   if( rsTmp->adoEOF && rsTmp->BOF )
				   {
					   //没有找到直径值
					   sTmp.Format(GetResStr(IDS_NotMatchDiameterValueInZdjcrudeMdb),modPHScal::dbZDJcrude->DefaultDatabase,modPHScal::tbnCSPRINGL5Crude,SQLx);
					   throw sTmp;
				   }
				   else
				   {
					   rsTmp->get_Collect((_variant_t)_T("sizeH"), &vTmp1);
					   Height_SPRINGsL5 =vtof(vTmp1);
					   //获取拉杆可插入的最大长度和最小长度
					   rsTmp->get_Collect((_variant_t)_T("size4"), &vTmp1);
					   rsTZB->put_Collect((_variant_t)_T("AllowedMinLugExt"),COleVariant(vtof(vTmp1)));//保存到数据库，以便CalLugLength函数使用
					   rsTmp->get_Collect((_variant_t)_T("size5"), &vTmp1);
					   rsTZB->put_Collect((_variant_t)_T("AllowedMaxLugExt"),COleVariant(vtof(vTmp1)));
				   }
				   //恒力弹簧上方连接拉杆的下端插入值，保存到ZB表的AllowedMaxLugExtUp、AllowedMinLugExtUp字段里
				   if( sPartID == _T("LHA") || sPartID == _T("LHB") || sPartID == _T("PHA")|| sPartID == _T("PHB"))
				   {
					   rsX->get_Collect((_variant_t)_T("PP"), &vTmp1);//pfg2005.12.23 上端最小插入值应该为PP
					   rsTZB->put_Collect((_variant_t)_T("AllowedMaxLugExtUp"),COleVariant(vtof(vTmp1)));
					   rsTZB->put_Collect((_variant_t)_T("AllowedMinLugExtUp"),COleVariant(vtof(vTmp1)));
				   }
		   
				   //如果恒力弹簧的选型位移modPHScal::sSprInfo[iSEQofSPR].SumDisp正好等于下限位移字段fBmin的一个值，
				   //则恒力弹簧的安装高度H1(sngH1xmax)=直接查得这个下限位移字段fBmin的值。
				   //否则，按插值法计算位移。add by ligb on 2004.11.24
				   CString tmpSQL;
				   _RecordsetPtr rsX1;
				   rsX1.CreateInstance(__uuidof(Recordset));
		   
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
				   tmpSQL += (modPHScal::gbCSPRneedSpecialDesign ? _T(" ") : sTmp);
				   tmpSQL += _T(" AND trim(CustomID)=\'");
				   tmpSQL += Ptype[i];
				   tmpSQL += _T("\' ORDER BY dh,Weight");
	try
	{
	//			   rsX1.m_pDatabase=&modPHScal::dbZDJcrude;
	//			   rsX1.Open(dbOpenSnapshot,tmpSQL);
				   rsX1->Open((_bstr_t)tmpSQL,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
					   adOpenForwardOnly, adLockReadOnly, adCmdText); 
	}
	catch(_com_error e)
	{
		ret=false;
	}
				   if(rsX1->BOF && rsX1->adoEOF||1)//当处在临界值时出错(sizeH的高度没有加上实际位移)pfg and lgb 2005.12.15
				   {
					   //常州电力机械厂样本数据库字段sizeH存储了正确的高度值。
					   rsX->get_Collect((_variant_t)_T("sizeH"), &vTmp1);
					   //rsX->get_Collect((_variant_t)_T("E"),vTmp1);
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
						   rsX->get_Collect((_variant_t)_T("fBmin"), &vTmp1);
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
						   rsX->get_Collect((_variant_t)_T("fBmax"), &vTmp1);
						   rsX->get_Collect((_variant_t)_T("fBmin"), &vTmp2);
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
					   //rsTZB->put_Collect((_variant_t)_T("AllowedMinLugExt")) = IIf(IsNull(rsX.Fields(_T("x"))), 0, rsX.Fields(_T("x")))
					   //rsTZB->put_Collect((_variant_t)_T("AllowedMaxLugExt")) = IIf(IsNull(rsX.Fields(_T("y"))), 0, rsX.Fields(_T("y")))+rsTZB->put_Collect((_variant_t)_T("AllowedMinLugExt"))
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
						   rsTmp->get_Collect((_variant_t)_T("size4"), &vTmp1);
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
						   //rsTZB->put_Collect((_variant_t)_T("A")) = IIf(IsNull(rsX(_T("PP"))), 0, rsX(_T("PP")))
					   }
					   else if(sPartID==_T("LHE") || sPartID==_T("LHE1") || sPartID==_T("PHE") || sPartID==_T("PHE1") || sPartID==_T("PHC") || sPartID==_T("LHC"))
					   {
						   //立式吊杆连接，要加上耳板高度T。
						   if( !modPHScal::gbCH1IncludeT )
						   {
							   rsDiaOfCSPRFiK->get_Collect((_variant_t)_T("T"), &vTmp1);
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

				   rsX1->Close();

				   //恒吊松紧螺母长度调节值
				   rsTZB->put_Collect((_variant_t)_T("SizeH"),COleVariant(sngDim));
				   rsTZB->put_Collect((_variant_t)_T("sizeDIM"),COleVariant(sngDim));
				   if( EDIBAcad::g_bBomSprFactory )//newHS->只有选择了标注恒力弹簧厂家才写入数据库
				   {
					   rsTZB->put_Collect((_variant_t)_T("CLbz"),STR_VAR(modPHScal::gsCSPRmanufactory));
				   }
				   //暂时借用gdw1字段保存计算荷载，供后续过程GetBoltsNutsAndAttachmentsCLgg使用
				   rsTZB->put_Collect((_variant_t)_T("GDW1"),COleVariant(tmpSelPJG4CSPRFiK / iNumPart));
				   rsTZB->put_Collect((_variant_t)_T("SpringNo"), COleVariant( (long)modPHScal::sSprInfo[iSEQofSPR].DH) );
				   rsTZB->put_Collect((_variant_t)_T("LugDia"), COleVariant(modPHScal::sSprInfo[iSEQofSPR].FiJ));
				   rsTZB->put_Collect((_variant_t)_T("HeatDisp"), COleVariant(modPHScal::sSprInfo[iSEQofSPR].HeatDisp));
		   
				   rsTZB->Update();
			  }
			  break;
		default:
			//其它的零部件类型
			rsTZB->Update();
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
				rsConnect->get_Collect((_variant_t)(_T("CntbF")+sj), &vTmp1);
				if(vtos(vTmp1)!=_T("") )
				{
			
			
					if( rsPartBoltNuts->adoEOF )
						continue;
	//				rsPartBoltNuts.Edit();
					iSEQofPart = iSEQofPart + 1;
					rsPartBoltNuts->put_Collect((_variant_t)_T("BHformat"),STR_VAR(sBHFormat));
					rsPartBoltNuts->put_Collect((_variant_t)_T("VolumeID"),COleVariant(EDIBgbl::SelVlmID));
					rsPartBoltNuts->put_Collect((_variant_t)_T("zdjh"),COleVariant((long)modPHScal::zdjh));
					rsPartBoltNuts->put_Collect((_variant_t)_T("recno"),COleVariant((long)iSEQofPart));

					//rsPartBoltNuts->put_Collect((_variant_t)_T("index"),COleVariant((long)iBoltsNuts));
					rsPartBoltNuts->put_Collect((_variant_t)_T("nth"),COleVariant((short)nth));//标准支吊架路数=1，包括对称双吊和共用根部双吊
					rsConnect->get_Collect((_variant_t)(_T("CntbF")+sj+_T("num")), &vTmp1);

					if( vTmp1.vt==VT_NULL)
					{
						//螺栓螺母数量=其后零件数量*1///
						rsPartBoltNuts->put_Collect((_variant_t)_T("CLnum"),COleVariant(iNumPart * iNum * iCntbNum));
					}
					else
					{
						//附件数量=实际用量,包含vtmp1值 为零的情况
						rsPartBoltNuts->put_Collect((_variant_t)_T("CLnum"),COleVariant(vtoi(vTmp1) * iNumPart * iNum * iCntbNum));
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
					rsConnect->get_Collect((_variant_t)(_T("CntbF")+sj), &vTmp1);
					rsPartBoltNuts->put_Collect((_variant_t)_T("ID"),vTmp1);
			
					if(rsID->Find((_bstr_t)(_T("trim(ID)=\'") + vtos(vTmp1) + _T("\'")), 0, adSearchForward, vTmp))
					{
						rsID->get_Collect((_variant_t)_T("Description"), &vTmp1);
						rsPartBoltNuts->put_Collect((_variant_t)_T("CLmc"),vTmp1);
						rsID->get_Collect((_variant_t)_T("CustomID"), &vTmp1);
						rsPartBoltNuts->put_Collect((_variant_t)_T("CustomID"),vTmp1);
						rsID->get_Collect((_variant_t)_T("ClassID"), &vTmp1);
						rsPartBoltNuts->put_Collect((_variant_t)_T("ClassID"),vTmp1);
					}
					//rsPartBoltNuts->put_Collect((_variant_t)_T("CustomID")) = IIf(gbFDid, sFindCustomID(rsc.Fields(_T("CntbF") + j)), rsc.Fields(_T("CntbF") + j))
					//rsPartBoltNuts->put_Collect((_variant_t)_T("ID")) = IIf(gbFDid, rsc.Fields(_T("CntbF") + j), sFindID(rsc.Fields(_T("CntbF") + j)))
					//rsPartBoltNuts->put_Collect((_variant_t)_T("CLmc")) = sFindFLD(IIf(gbFDid, _T("ID"), _T("CustomID")), _T("Description"), IIf(gbFDid, rsc.Fields(_T("CntbF") + j), sFindCustomID(rsc.Fields(_T("CntbF") + j))))
					if( ! bWithBoltsNuts )
					{
						//厂家不提供的螺栓螺母
						//赋给任意非空的数字，便于编号
						rsPartBoltNuts->put_Collect((_variant_t)_T("SEQ"),COleVariant((long) i));

						//lfx 2005.5.20改 
						//说明见上面//lfx 2005.5.20改 注释
						//原rsPartBoltNuts->put_Collect((_variant_t)_T("CLbz"),vnil);
						rsPartBoltNuts->put_Collect((_variant_t)_T("CLbz"),COleVariant(strClbz));  
						//End 2005.5.20改
					}
					else
					{
						rsPartBoltNuts->put_Collect((_variant_t)_T("SEQ"),vnil);
						rsPartBoltNuts->put_Collect((_variant_t)_T("CLnum"),COleVariant((long)0));
						CString str;
						str.LoadString(IDS_PRODUCEFAC_OFFERGOODS);
						rsPartBoltNuts->put_Collect((_variant_t)_T("CLbz"),STR_VAR(str));
					}
					rsPartBoltNuts->Update();
					rsPartBoltNuts->MoveNext();
					rsTZB->MoveNext();
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
		rsTmpZB->MoveNext();
		rsTZB->MoveNext();
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
			
				//if(rsTZB->Find((_bstr_t)_T("IsNull(recno) AND (bUserAdd is null OR bUserAdd=0)")))
				//2002.05.25以后所有零件都写记录号，故改写此段程序。LEE Guobin
				if(rsTZB->Find((_bstr_t)(_T(" (bUserAdd is null OR bUserAdd=0) AND Index=") + ltos(iSA)), 0, adSearchForward, vTmp))
				{	
			//		rsTZB.Edit();
					if( rsTZB->BOF )			
					{
						sTmp.Format(GetResStr(IDS_BOForEOFofmvarRsTZB),  _T("IsBOF()"));
						throw sTmp;
					}
					if( rsTZB->adoEOF )			
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
				rsTZB->get_Collect((_variant_t)_T("CustomID"), &vTmp1);
			
				if( !rsID->Find((_bstr_t)(_T("trim(CustomID)=\'") + vtos(vTmp1) + _T("\'")), 0, adSearchForward, vTmp)){
					sTmp.Format(GetResStr(IDS_NoRecordFLDEqvThisValueInPictureClipData),  _T("CustomID"), vtos(vTmp1));
					throw sTmp;
				}
				else
				{
			//		rsTZB.Edit ();
					rsID->get_Collect((_variant_t)_T("ClassID"), &vTmp1);
					if(  vtoi(vTmp1)== iGCement )
					{
						rsTZB->put_Collect((_variant_t)_T("SEQ"),vnil);
						rsTZB->put_Collect((_variant_t)_T("CLdz"),COleVariant((long)0));
						rsTZB->put_Collect((_variant_t)_T("CLzz"),COleVariant((long)0));
						mvarSATotalWeight = 0;
					}
					else
					{
						//型钢根部,赋给一非空的任意值
						rsTZB->put_Collect((_variant_t)_T("sizeDIM"),COleVariant(SACSHeight));
						rsTZB->get_Collect((_variant_t)_T("index"), &vTmp1);
						vTmp2=rsza->GetCollect(_T("bNotSumSA"));
						if( vtoi(vTmp1)== iSA && !vtob(vTmp2)){
							//根部材料要统计
							rsTZB->put_Collect((_variant_t)_T("SEQ"),COleVariant((long)i));
							rsTZB->put_Collect((_variant_t)_T("CLdz"),COleVariant(mvarSATotalWeight));
							rsTZB->get_Collect((_variant_t)_T("CLNum"), &vTmp1);
							rsTZB->put_Collect((_variant_t)_T("CLzz"),COleVariant(mvarSATotalWeight * vtof(vTmp1)));
						}
						else
						{
							//根部材料不要统计
							//不赋给根部编号，防止其写入零件明细表
							rsTZB->put_Collect((_variant_t)_T("SEQ"),vnil);
							rsTZB->put_Collect((_variant_t)_T("CLNum"),COleVariant((long)0));
							rsTZB->put_Collect((_variant_t)_T("CLdz"),COleVariant((long)0));
							rsTZB->put_Collect((_variant_t)_T("CLzz"),COleVariant((long)0));
						}
					}
				}
				rsTZB->Update();		
			}		
		}
		if( modPHScal::g_bPipeClampBoltDiameterChanged && FirstCal ==1 && modPHScal::giWholeSprNum ==0 )
			//小荷载大管径管夹按荷载选择，螺栓的直径M1变小，管夹规格中必须注明此M1值
		{
			CString strSQLTmp ;
			//管夹螺栓M1与U形耳子L8的螺栓M功能相似
			strSQLTmp.Format("SELECT * FROM [%s] WHERE [CUSTOMID]=\'%s\' AND [SIZE2]=%d ORDER BY SIZE2",
				modPHScal::tbnPART,modPHScal::sFindCustomID (_T("L8")),int(modPHScal::gmiDiameter));
			_RecordsetPtr m_rs;
			COleVariant vT1;
			if(m_rs->State == adOpenStatic)
				m_rs->Close();
			
	try
	{
	//		m_rs.m_pDatabase=&modPHScal::dbZDJcrude;
	//		m_rs.Open(dbOpenDynaset,strSQLTmp);
			m_rs->Open((_bstr_t)strSQLTmp,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
				adOpenForwardOnly, adLockReadOnly, adCmdText); 
	}
	catch(_com_error e)
	{
		ret=false;
	}
			CString str;
			int x=0;
			if((!m_rs->BOF) && !(m_rs->adoEOF))
			{
				m_rs->get_Collect((_variant_t)_T("size3"),vT1);
				x = vtoi(vT1);
			}
			m_rs->Close();
			
			strSQLTmp.Format(" [NTH]=1 AND recno=1 ");
			rsTZB->MoveFirst();
			rsTZB->Find((_bstr_t)strSQLTmp, 0, adSearchForward, vTmp);
			if(!rsTZB->adoEOF)
			{
	//			rsTZB.Edit();
				rsTZB->get_Collect((_variant_t)_T("CLgg"), &vT1);
				CString strTmp=vtos(vT1);			
			   //如果管夹螺栓的M1的值比需要的L8的螺栓M大，			   
			   if(m_fDiaM1 > x)
			   {
					strSQLTmp.Format("SELECT * FROM [%s] WHERE [CUSTOMID]=\'%s\' AND [SIZE2]=%d ORDER BY SIZE2",
						modPHScal::tbnPART,modPHScal::sFindCustomID (_T("L7")),int(modPHScal::gmiDiameter));
					m_rs->Close();
	try
	{
	//				m_rs.Open(dbOpenSnapshot,strSQLTmp);
					m_rs->Open((_bstr_t)strSQLTmp,_variant_t((IDispatch*)modPHScal::dbZDJcrude,true), 
						adOpenForwardOnly, adLockReadOnly, adCmdText); 
	}
	catch(_com_error e)
	{
		ret=false;
	}
					m_rs->get_Collect((_variant_t)_T("size3"),vT1);
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
				
				rsTZB->put_Collect((_variant_t)_T("CLgg"),STR_VAR(strTmp));
				rsTZB->Update();
			}
			
		}
/*		
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
*/
	if(rsTZB->State == adOpenStatic)
		rsTZB->Close();

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
