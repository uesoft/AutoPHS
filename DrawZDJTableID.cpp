// DrawZDJTableID.cpp: implementation of the CDrawZDJTableID class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DrawZDJTableID.h"
#include "edibacad.h"
#include "edibgbl.h"
#include "edibdb.h"
#include "Frmtxsr.h"
#include "modphscal.h"
#include "user.h"
#include "cphs.h"
#include "math.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDrawZDJTableID::CDrawZDJTableID()
{

}

CDrawZDJTableID::~CDrawZDJTableID()
{

}
//绘制表格
void CDrawZDJTableID::DrawzdjTable(int index,bool bIsCom)
{
	try
	{
		bool Found=false;
		CCadPoint p0, p1;
		_RecordsetPtr rs;
		rs.CreateInstance(__uuidof(Recordset));

		_RecordsetPtr rs1;
		rs1.CreateInstance(__uuidof(Recordset));

		int i=0;
		int k=0;
		static int n=0;
		if(index!=iZDJDRAWEXECL && index!=iZDJDRAWCatalogueExcel)
		{
			EDIBAcad::StartAcad();
			EDIBAcad::DisplayAcadTop();
			EDIBAcad::SetAcadTop();
			EDIBAcad::GetActiveAcadDoc();
		}
		//如果当前打开的图形不是(PhsBlkDir & _T("phs.dwg")),需要重新打开
		//因为后面的操作需要大量的图形块准备好
		CString sTmp,sTmp1;
		_variant_t vTmp;
		CMObject objTmp;
		CCadPoint pTmp;
		CString SQLx;
		COleVariant varTmp;
		_variant_t varNull;
		varNull.ChangeType(VT_NULL);
		if(index!=iZDJDRAWTag && index!=iZDJDRAWbom)
		{
			//判断是否所有支吊架计算完成。
			try
			{
				if(!FrmTxsr.m_pViewTxsr->m_ActiveRs->adoEOF && !FrmTxsr.m_pViewTxsr->m_ActiveRs->BOF)
					FrmTxsr.m_pViewTxsr->m_ActiveRs->Update();
			}
			catch(CException *e)
			{
				e->Delete();
			}
			CString SQLx = _T("SELECT count(*) FROM [") + EDIBgbl::Btype[EDIBgbl::TZA] + _T("] WHERE ( bCalSuccess=0  OR bCalSuccess IS NULL )");
			
			if(modPHScal::gbSumAllVolume )
			{
				SQLx +=_T(" AND VolumeID IN ( SELECT VolumeID FROM Volume WHERE EnginID=\'") + EDIBgbl::SelPrjID + _T("\' AND ZYID = ") + ltos(EDIBgbl::SelSpecID) + _T(" AND SJJDID = ") + ltos(EDIBgbl::SelDsgnID) + _T(" AND SJHYID = ") + ltos(EDIBgbl::SelHyID) +_T(") ");
			}
			else
			{
				SQLx +=_T(" AND VolumeID =") + ltos(EDIBgbl::SelVlmID);
			}

			long i1,i2;

			rs1->Open(_variant_t(SQLx),(IDispatch*)::conPRJDB,adOpenKeyset, adLockOptimistic,adCmdText);
			i1=vtoi(rs1->GetCollect(_variant_t((long)0)));
			rs1->Close();

			SQLx = _T("SELECT count(*) FROM [") + EDIBgbl::Btype[EDIBgbl::TZA] + _T("] WHERE ");
			if(modPHScal::gbSumAllVolume )
			{
				SQLx +=_T(" VolumeID IN ( SELECT VolumeID FROM Volume WHERE EnginID=\'") + EDIBgbl::SelPrjID + _T("\' AND ZYID = ") + ltos(EDIBgbl::SelSpecID) + _T(" AND SJJDID = ") + ltos(EDIBgbl::SelDsgnID) + _T(" AND SJHYID = ") + ltos(EDIBgbl::SelHyID) +_T(") ");
			}
			else
			{
				SQLx +=_T(" VolumeID =") + ltos(EDIBgbl::SelVlmID);
			}
			rs1->Open(_variant_t(SQLx),(IDispatch*)::conPRJDB,adOpenKeyset, adLockOptimistic,adCmdText);
			i2=vtoi(rs1->GetCollect(_variant_t((long)0)));
			rs1->Close();
			rs1=NULL;
			CString strTmp;
			strTmp.Format(GetResStr(IDS_SomePhsNotCalSuccess),ltos(i2),ltos(i2-i1),ltos(i1));
			if( IDNO == ShowMessage(strTmp,MB_DEFBUTTON1|MB_ICONQUESTION|MB_YESNO) )
				return;
		}

		switch(index)
		{
		case iZDJDRAWTag:
			break;
		case iZDJDRAWbom:
			modPHScal::SetBasePoint();
			Cavphs->GetphsSEQ(FrmTxsr.m_pViewTxsr->m_ActiveRs);
			EDIBDB::MakeTmp2ToBom();
			p0.SetPoint(modPHScal::pt2x,modPHScal::pt2y);
			EDIBAcad::DeleteAllEntitiesInLayers(1,_T("bom"));
// 			rs.m_pDatabase=&EDIBgbl::dbPRJ;
// 			rs.Open(dbOpenSnapshot,_T("SELECT * FROM TMP2"));
			rs->Open((_bstr_t)_T("SELECT * FROM TMP2"), _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
				adOpenKeyset, adLockOptimistic, adCmdText); 

			EDIBAcad::DrawTableACAD(p0, EDIBgbl::TLJ, atan(1.0) * 0.0, rs,0,1.0f,_T("Bom"),_T("Bom"),_T("%g"),modPHScal::iAlignLjmx);
			sTmp.Format(GetResStr(IDS_xDrawingFinished),EDIBgbl::Cbtype[EDIBgbl::TCL].MnuCaption);

			ShowMessage(sTmp);
			break;
		case iZDJDRAWbomBJBW:
			modPHScal::SetBasePoint();
			Cavphs->GetphsSEQ(FrmTxsr.m_pViewTxsr->m_ActiveRs);
			EDIBDB::MakeTmp2ToBom();
			p0.SetPoint(modPHScal::pt2x,modPHScal::pt2y);
			EDIBAcad::DeleteAllEntitiesInLayers(1,_T("bom"));
// 			rs.m_pDatabase=&EDIBgbl::dbPRJ;
// 			rs.Open(dbOpenSnapshot,_T("SELECT * FROM TMP2"));
			rs->Open((_bstr_t)_T("SELECT * FROM TMP2"), _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
				adOpenKeyset, adLockOptimistic, adCmdText); 
			
			EDIBAcad::DrawTableACAD(p0, EDIBgbl::TLJ_BJBW, atan(1.0) * 0.0, rs,0,1.0f,_T("Bom"),_T("Bom"),_T("%g"),modPHScal::iAlignLjmx);
			sTmp.Format(GetResStr(IDS_xDrawingFinished),EDIBgbl::Cbtype[EDIBgbl::TCL].MnuCaption);

			ShowMessage(sTmp);
			break;
		case iZDJDRAWload:
			modPHScal::SetBasePoint();
			modPHScal::PhsDisplacementLoadINFOMake();
			break;
		case iZDJDRAW270mm:
				//每次计算一个支吊架时生成当前当前支吊架一览表，计算完成后总的一览表就生成了。
				//每次计算一个支吊架时生成当前当前支吊架明细一览表，计算完成后总的明细一览表就生成了。
			SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZD] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" ORDER BY zdjh");
// 			rs.m_pDatabase=&EDIBgbl::dbPRJDB;
// 			rs.Open(dbOpenSnapshot,SQLx);
			rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
				adOpenKeyset, adLockOptimistic, adCmdText); 
			if(!rs->BOF && !rs->adoEOF)
			{//zsy 12/17      改为用ARX画图
					ShowMessage(GetResStr(IDS_PleasePickApointAsZDJYLBInsertPointINAutoCAD));
					if( EDIBAcad::gbACADR14 || !bIsCom ) //使用动态库画图
					{
						rs->Close();
					}else
					{
						_variant_t var;
						var=EDIBAcad::objAcadDoc.GetPropertyByName(_T("Utility"));
						objTmp = var;
						p1=objTmp.Invoke(_T("GetPoint"),2,(LPVARIANT)p0,&_variant_t(GetResStr(IDS_PleasePickApointAsBOMInsertPointINAutoCAD)));
						p0 = p1;
					}
					EDIBAcad::DrawTableACAD(p0, EDIBgbl::TZD, atan(1.0) * 0.0, rs, 0,1.0f, _T("YLB"));
			}
			break;
		case iZDJDRAW180mm:
				//每次计算一个支吊架时生成当前支吊架一览表，计算完成后总的一览表就生成了。
				//每次计算一个支吊架时生成当前支吊架明细一览表，计算完成后总的明细一览表就生成了。
			SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZD] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" ORDER BY zdjh");
// 			rs.m_pDatabase=&EDIBgbl::dbPRJDB;
// 			rs.Open(dbOpenSnapshot,SQLx);
			rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
				adOpenKeyset, adLockOptimistic, adCmdText); 
			if(!rs->BOF && !rs->adoEOF)
			{//zsy 12/17      改为用ARX画图
				ShowMessage(GetResStr(IDS_PleasePickApointAsZDJYLBInsertPointINAutoCAD));
				if( EDIBAcad::gbACADR14 || !bIsCom ) //使用动态库画图
				{
					rs->Close();
				}else
				{
					_variant_t v=EDIBAcad::objAcadDoc.GetPropertyByName(_T("Utility"));
					objTmp = v.pdispVal;
					p1=objTmp.Invoke(_T("GetPoint"),2,(LPVARIANT)p0,&_variant_t(GetResStr(IDS_PleasePickApointAsBOMInsertPointINAutoCAD)));
					p0 = p1;
				}
				EDIBAcad::DrawTableACAD(p0, EDIBgbl::TZD180, atan(1.0) * 0.0, rs, 0 , 1.0f, _T("YLB"));
			}
			break;	
		case iZDJDRAW270L:
			SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZD] + _T("] WHERE ZDJH IN ( SELECT ZDJH FROM [")
				+ EDIBgbl::Btype[EDIBgbl::TZA] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) 
				+ _T(" AND TYPE LIKE \'????\' ) AND VolumeID =")+ ltos(EDIBgbl::SelVlmID) + _T(" ORDER BY zdjh");
// 			rs.m_pDatabase=&EDIBgbl::dbPRJDB;
// 			rs.Open(dbOpenSnapshot,SQLx);
			rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
				adOpenKeyset, adLockOptimistic, adCmdText); 
			if(!rs->BOF && !rs->adoEOF)
			{//zsy 12/17      改为用ARX画图
				ShowMessage(GetResStr(IDS_PleasePickApointAsZDJYLBInsertPointINAutoCAD));
				if( EDIBAcad::gbACADR14 || !bIsCom ) //使用动态库画图
				{
					rs->Close();
				}else
				{
					_variant_t v=EDIBAcad::objAcadDoc.GetPropertyByName(_T("Utility"));
					objTmp = v.pdispVal;
					p1=objTmp.Invoke(_T("GetPoint"),2,(LPVARIANT)p0,&_variant_t(GetResStr(IDS_PleasePickApointAsBOMInsertPointINAutoCAD)));
					p0 = p1;
				}
				EDIBAcad::DrawTableACAD(p0, EDIBgbl::TZD270L, atan(1.0) * 0.0, rs, 0 , 1.0f, _T("YLB"));
			}
			break;
		case iZDJDRAWEXECL:
			SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZD] + _T("] WHERE ZDJH IN ( SELECT ZDJH FROM [")
				+ EDIBgbl::Btype[EDIBgbl::TZA] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) 
				+ _T(" AND TYPE LIKE \'????\' ) AND VolumeID =")+ ltos(EDIBgbl::SelVlmID) + _T(" ORDER BY zdjh");			
// 			rs.m_pDatabase=&EDIBgbl::dbPRJDB;
// 			rs.Open(dbOpenSnapshot,SQLx);
			rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
				adOpenKeyset, adLockOptimistic, adCmdText); 
			if(!rs->BOF && !rs->adoEOF)
			{
				EDIBAcad::DrawTableExcel( EDIBgbl::TZDExcel, rs);
			}
			sTmp.Format(GetResStr(IDS_xDrawingFinished),EDIBgbl::Cbtype[EDIBgbl::TZDExcel].MnuCaption);
			ShowMessage(sTmp);
			break;
		case iZDJDRAW705mm:
				//判断是否所有支吊架计算完成。
			SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZD] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" ORDER BY zdjh");
// 			rs.m_pDatabase=&EDIBgbl::dbPRJDB;
// 			rs.Open(dbOpenSnapshot,SQLx);
			rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
				adOpenKeyset, adLockOptimistic, adCmdText); 
			if(!rs->BOF && !rs->adoEOF)
			{//zsy 12/17      改为用ARX画图
					ShowMessage(GetResStr(IDS_PleasePickApointAsZDJYLBInsertPointINAutoCAD));
					if( EDIBAcad::gbACADR14 || !bIsCom ) //使用动态库画图
					{
						rs->Close();
					}else
					{

						_variant_t v=EDIBAcad::objAcadDoc.GetPropertyByName(_T("Utility"));
						objTmp =v.pdispVal;
						p1=objTmp.Invoke(_T("GetPoint"),2,(LPVARIANT)p0,&_variant_t(GetResStr(IDS_PleasePickApointAsBOMInsertPointINAutoCAD)));
						p0 = p1;
					}
					EDIBAcad::DrawTableACAD(p0, EDIBgbl::TZD800, atan(1.0) * 0.0, rs, 0 , 1.0f, _T("YLB"));
			}
			break;

		case iZDJDRAW705mmHSY:			////核四院支吊架明细一览表
				//判断是否所有支吊架计算完成。
			SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZD] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" ORDER BY zdjh");
// 			rs.m_pDatabase=&EDIBgbl::dbPRJDB;
// 			rs.Open(dbOpenSnapshot,SQLx);
			rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
				adOpenKeyset, adLockOptimistic, adCmdText); 
			if(!rs->BOF && !rs->adoEOF)
			{//zsy 12/17      改为用ARX画图
					ShowMessage(GetResStr(IDS_PleasePickApointAsZDJYLBInsertPointINAutoCAD));
					if( EDIBAcad::gbACADR14 || !bIsCom ) //使用动态库画图
					{
						rs->Close();
					}else
					{
						_variant_t v=EDIBAcad::objAcadDoc.GetPropertyByName(_T("Utility"));
						objTmp =v.pdispVal;
						p1=objTmp.Invoke(_T("GetPoint"),2,(LPVARIANT)p0,&_variant_t(GetResStr(IDS_PleasePickApointAsBOMInsertPointINAutoCAD)));
						p0 = p1;
					}
					EDIBAcad::DrawTableACAD(p0, EDIBgbl::TZD800HSY, atan(1.0) * 0.0, rs, 0 , 1.0f, _T("YLB"));
			}
			break;

		case iZDJDRAW705mmQGY:			////支吊架明细一览表
				//判断是否所有支吊架计算完成。
			SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZD] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" ORDER BY zdjh");
// 			rs.m_pDatabase=&EDIBgbl::dbPRJDB;
// 			rs.Open(dbOpenSnapshot,SQLx);
			rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
				adOpenKeyset, adLockOptimistic, adCmdText); 
			if(!rs->BOF && !rs->adoEOF)
			{//zsy 12/17      改为用ARX画图
				ShowMessage(GetResStr(IDS_PleasePickApointAsZDJYLBInsertPointINAutoCAD));
				if( EDIBAcad::gbACADR14 || !bIsCom ) //使用动态库画图
				{
					rs->Close();
				}else
				{
					_variant_t v=EDIBAcad::objAcadDoc.GetPropertyByName(_T("Utility"));
					objTmp =v.pdispVal;
					p1=objTmp.Invoke(_T("GetPoint"),2,(LPVARIANT)p0,&_variant_t(GetResStr(IDS_PleasePickApointAsBOMInsertPointINAutoCAD)));
					p0 = p1;
				}
				EDIBAcad::DrawTableACAD(p0, EDIBgbl::TZD800QGY, atan(1.0) * 0.0, rs, 0 , 1.0f, _T("YLB"));
			}
			break;

		case iZDJDRAW270pc:   //巴陵石化设计院支吊架表
				//每次计算一个支吊架时生成当前当前支吊架一览表，计算完成后总的一览表就生成了。
				//每次计算一个支吊架时生成当前当前支吊架明细一览表，计算完成后总的明细一览表就生成了。
			SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZD] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" ORDER BY zdjh");
// 			rs.m_pDatabase=&EDIBgbl::dbPRJDB;
// 			rs.Open(dbOpenSnapshot,SQLx);
			rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
				adOpenKeyset, adLockOptimistic, adCmdText); 
			if(!rs->BOF && !rs->adoEOF)
			{//zsy 12/17      改为用ARX画图
					ShowMessage(GetResStr(IDS_PleasePickApointAsZDJYLBInsertPointINAutoCAD));
					if( EDIBAcad::gbACADR14 || !bIsCom ) //使用动态库画图
					{
						rs->Close();
					}else
					{
						_variant_t v=EDIBAcad::objAcadDoc.GetPropertyByName(_T("Utility"));
						objTmp = v.pdispVal;
						p1=objTmp.Invoke(_T("GetPoint"),2,(LPVARIANT)p0,&_variant_t(GetResStr(IDS_PleasePickApointAsBOMInsertPointINAutoCAD)));
						p0 = p1;
					}
					EDIBAcad::DrawTableACAD(p0, EDIBgbl::TZP, atan(1.0) * 0.0, rs, 0 , 1.0f, _T("YLB"));
			}
			break;	
		case iZDJDRAWCatalogue:
			EDIBDB::DrawID = EDIBAcad::GetDrawIDAndName(1, EDIBDB::DrawName);

				//绘制目录
				//每次计算一个支吊架时生成当前当前支吊架图纸目录信息，计算完成后总的目录就生成了。
			SQLx=_T("DELETE FROM [ML] WHERE VolumeID IS NULL AND ZDJH IS NULL");
			EDIBgbl::dbPRJDB->Execute((_bstr_t)SQLx, NULL, adCmdText);
			SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TML] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" ORDER BY SEQ");
// 			rs.m_pDatabase=&EDIBgbl::dbPRJDB;
// 			rs.Open(dbOpenDynaset,SQLx);
			rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
				adOpenKeyset, adLockOptimistic, adCmdText); 
			if(!rs->BOF && !rs->adoEOF)
			{
					//如果第一个图纸的序号>=1，在前面添加空的图纸目录,以便用户手工修改
				VARIANT vTmp;
				if(!rs->Find((_bstr_t)(_T("ZDJH <> NULL")), 0, adSearchForward, vTmp))
					return;
				rs->get_Collect((_variant_t)_T("SEQ"),varTmp);
				k = vtoi(varTmp);

				for( i = 1 ;i< k ;i++)
				{
					if(!rs->Find((_bstr_t)(_T("SEQ=") + ltos(i)), 0, adSearchForward, vTmp))
					{
						rs->AddNew();
						rs->put_Collect((_variant_t)_T("SEQ"),COleVariant((long)i));
						rs->put_Collect((_variant_t)_T("Pages"),COleVariant((long)1));
						rs->put_Collect((_variant_t)_T("DrawNo"),STR_VAR(_T("1")));
						rs->put_Collect((_variant_t)_T("DrawNa"),STR_VAR(_T("1")));
						rs->put_Collect((_variant_t)_T("Zdjh"),varNull);
						rs->put_Collect((_variant_t)_T("VolumeID"),varNull);
						rs->Update();
					}
				}
				rs->Close();
				//zsy 12/17      改为用ARX画图
				SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TML] + _T("] WHERE VolumeID IS NULL OR VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T("  ORDER BY SEQ");
//				rs.Open(dbOpenDynaset,SQLx);
				rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
					adOpenKeyset, adLockOptimistic, adCmdText); 
				ShowMessage(GetResStr(IDS_PleasePickApointAsZDJYLBInsertPointINAutoCAD));
				if( EDIBAcad::gbACADR14 || !bIsCom ) //使用动态库画图
				{
					rs->Close();
				}else
				{
					_variant_t v=EDIBAcad::objAcadDoc.GetPropertyByName(_T("Utility"));
					objTmp = v.pdispVal;
					p1=objTmp.Invoke(_T("GetPoint"),2,(LPVARIANT)p0,&_variant_t(GetResStr(IDS_PleasePickApointAsBOMInsertPointINAutoCAD)));
					p0 = p1;
				}
				EDIBAcad::DrawTableACAD(p0, EDIBgbl::TML, atan(1.0) * 0.0, rs, 0 , 1.0f, _T("ML"));
			}
			break;
		case iZDJDRAWCatalogueExcel:
			EDIBDB::DrawID = EDIBAcad::GetDrawIDAndName(1, EDIBDB::DrawName);
				//绘制目录
				//每次计算一个支吊架时生成当前当前支吊架图纸目录信息，计算完成后总的目录就生成了。
			SQLx=_T("DELETE FROM [ML] WHERE VolumeID IS NULL AND ZDJH IS NULL");
			EDIBgbl::dbPRJDB->Execute((_bstr_t)SQLx, NULL, adCmdText);

			SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TML] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" ORDER BY SEQ");
// 			rs.m_pDatabase=&EDIBgbl::dbPRJDB;
// 			rs.Open(dbOpenDynaset,SQLx);
			rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
				adOpenKeyset, adLockOptimistic, adCmdText); 
			VARIANT vTmp;
			if(!rs->BOF && !rs->adoEOF)
			{
					//如果第一个图纸的序号>=1，在前面添加空的图纸目录,以便用户手工修改
				if(!rs->Find((_bstr_t)(_T("ZDJH <> NULL")), 0, adSearchForward, vTmp))
					return;
				rs->get_Collect((_variant_t)_T("SEQ"),varTmp);
				k = vtoi(varTmp);
				VARIANT vTmp;
				for( i = 1 ;i< k ;i++)
				{
					if(!rs->Find((_bstr_t)(_T("SEQ=") + ltos(i)), 0, adSearchForward, vTmp))
					{
						rs->AddNew();
						rs->put_Collect((_variant_t)_T("SEQ"),COleVariant((long)i));
						rs->put_Collect((_variant_t)_T("Pages"),COleVariant((long)1));
						rs->put_Collect((_variant_t)_T("DrawNo"),STR_VAR(_T("1")));
						rs->put_Collect((_variant_t)_T("DrawNa"),STR_VAR(_T("1")));
						rs->put_Collect((_variant_t)_T("Zdjh"),varNull);
						rs->put_Collect((_variant_t)_T("VolumeID"),varNull);
						rs->Update();
					}
				}
				rs->Close();
				SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TML] + _T("] WHERE VolumeID IS NULL OR VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T("  ORDER BY SEQ");
//				rs.Open(dbOpenDynaset,SQLx);
				rs->Open((_bstr_t)SQLx,_variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
					adOpenKeyset, adLockOptimistic, adCmdText); 
				EDIBAcad::DrawTableExcel(EDIBgbl::TMLE, rs);
			}
			sTmp.Format(GetResStr(IDS_xDrawingFinished),EDIBgbl::Cbtype[EDIBgbl::TML].MnuCaption);
			ShowMessage(sTmp);
			break;
		case iZDJDRAWm_phs :  //开始手工修改
			EDIBAcad::MakeNewLayer(_T("M_phs0"), acRed);
			break;
		case iZDJDRAWXN180mm://绘制西南电力院的支吊架一览表
			//Z8表
			SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZD] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" ORDER BY zdjh");
// 			rs.m_pDatabase=&EDIBgbl::dbPRJDB;
// 			rs.Open(dbOpenSnapshot,SQLx);
			rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
				adOpenKeyset, adLockOptimistic, adCmdText); 
			if(!rs->BOF && !rs->adoEOF)
			{//zsy 12/17      改为用ARX画图
				ShowMessage(GetResStr(IDS_PleasePickApointAsZDJYLBInsertPointINAutoCAD));
				if( EDIBAcad::gbACADR14 || !bIsCom ) //使用动态库画图
				{
					rs->Close();
				}else
				{
					_variant_t v=EDIBAcad::objAcadDoc.GetPropertyByName(_T("Utility"));
					objTmp = v.pdispVal;
					p1=objTmp.Invoke(_T("GetPoint"),2,(LPVARIANT)p0,&_variant_t(GetResStr(IDS_PleasePickApointAsBOMInsertPointINAutoCAD)));
					p0 = p1;
				}
				EDIBAcad::DrawTableACAD(p0, EDIBgbl::TZDSWED180, atan(1.0) * 0.0, rs, 0 , 1.0f, _T("YLB"));
			}
			break;
		case iZDJDRAW705mmHBYZ: //华北冶建
			//z9
						SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZD] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" ORDER BY zdjh");
// 			rs.m_pDatabase=&EDIBgbl::dbPRJDB;
// 			rs.Open(dbOpenSnapshot,SQLx);
			rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
				adOpenKeyset, adLockOptimistic, adCmdText); 
			if(!rs->BOF && !rs->adoEOF)
			{//zsy 12/17      改为用ARX画图
				ShowMessage(GetResStr(IDS_PleasePickApointAsZDJYLBInsertPointINAutoCAD));
				if( EDIBAcad::gbACADR14 || !bIsCom ) //使用动态库画图
				{
					rs->Close();
				}else
				{
					_variant_t v=EDIBAcad::objAcadDoc.GetPropertyByName(_T("Utility"));
					objTmp = v;
					p1=objTmp.Invoke(_T("GetPoint"),2,(LPVARIANT)p0,&_variant_t(GetResStr(IDS_PleasePickApointAsBOMInsertPointINAutoCAD)));
					p0 = p1;
				}
				EDIBAcad::DrawTableACAD(p0, EDIBgbl::TZD800HBYJ, atan(1.0) * 0.0, rs, 0 , 1.0f, _T("YLB"));
			}
			break;
		case iZDJDRAWloadBJBW://北京巴威支吊荷载位移表
			modPHScal::SetBasePoint();
			modPHScal::PhsDisplacementLoadINFOMake("hz",0,EDIBgbl::TzDLOADNEW);
			break;
		//pfg20050601//北京华宇
		case iZDJDRAWBJHY01:
				//判断是否所有支吊架计算完成。
			SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZD] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" ORDER BY zdjh");
// 			rs.m_pDatabase=&EDIBgbl::dbPRJDB;
// 			rs.Open(dbOpenSnapshot,SQLx);
			rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
				adOpenKeyset, adLockOptimistic, adCmdText); 
			if(!rs->BOF && !rs->adoEOF)
			{//zsy 12/17      改为用ARX画图
					ShowMessage(GetResStr(IDS_PleasePickApointAsZDJYLBInsertPointINAutoCAD));
					if( EDIBAcad::gbACADR14 || !bIsCom ) //使用动态库画图
					{
						rs->Close();
					}else
					{

						_variant_t v=EDIBAcad::objAcadDoc.GetPropertyByName(_T("Utility"));
						objTmp =v.pdispVal;
						p1=objTmp.Invoke(_T("GetPoint"),2,(LPVARIANT)p0,&_variant_t(GetResStr(IDS_PleasePickApointAsBOMInsertPointINAutoCAD)));
						p0 = p1;
					}
					EDIBAcad::DrawTableACAD(p0, 36, atan(1.0) * 0.0, rs, 0 , 1.0f, _T("YLB"));
			}
			break;

		case iZDJDRAWBJHY02:
				//判断是否所有支吊架计算完成。
			SQLx = _T("SELECT * FROM [") + EDIBgbl::Btype[EDIBgbl::TZD] + _T("] WHERE VolumeID=") + ltos(EDIBgbl::SelVlmID) + _T(" ORDER BY zdjh");
// 			rs.m_pDatabase=&EDIBgbl::dbPRJDB;
// 			rs.Open(dbOpenSnapshot,SQLx);
			rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
				adOpenKeyset, adLockOptimistic, adCmdText); 
			if(!rs->BOF && !rs->adoEOF)
			{//zsy 12/17      改为用ARX画图
					ShowMessage(GetResStr(IDS_PleasePickApointAsZDJYLBInsertPointINAutoCAD));
					if( EDIBAcad::gbACADR14 || !bIsCom ) //使用动态库画图
					{
						rs->Close();
					}else
					{

						_variant_t v=EDIBAcad::objAcadDoc.GetPropertyByName(_T("Utility"));
						objTmp = v.pdispVal;
						p1=objTmp.Invoke(_T("GetPoint"),2,(LPVARIANT)p0,&_variant_t(GetResStr(IDS_PleasePickApointAsBOMInsertPointINAutoCAD)));
						p0 = p1;
					}
					EDIBAcad::DrawTableACAD(p0, 37, atan(1.0) * 0.0, rs, 0 , 1.0f, _T("YLB"));
			}
			break;
		//pfg20050601
		}
	}
	catch(_com_error e)
	{
		ShowMessage(e.Description());
	}
}
