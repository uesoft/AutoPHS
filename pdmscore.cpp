#include "stdafx.h"
#include "PDMSCore.h"
#include "user.h"
#include <fstream.h>
#include <math.h>
#include "frmtxsr.h"
#include "phsdata.h"
#include "EDIBgbl.h"
#include "frmStatus.h"
#include "modphscal.h"
#include "FileDialogEx.h"
#include <AFXCONV.h>

struct CPDMSINFO{
	CPDMSINFO()
	{
		bIsDLG=TRUE;
		strPName.Empty();
		iZdjh=0;
		fPDW=0.0f;
		strPFX.Empty();
		strZdjType.Empty();
		strSAType.Empty();
		fSH=0.0f;
		fSHA=0.0f;
		fSHB=0.0f;
		fPH=0.0f;
		strMJFX.Empty();
		strXBFX.Empty();
		fXBL=0.0f;
		fJZl=0.0f;
		fJZL=0.0f;
		fJZl1=0.0f;
		fXBLA=0.0f;
		fXBLB=0.0f;
		fLJMA=0.0f;
		fLJMB=0.0f;
		fPAFX=0.0f;
	}
	BOOL	bIsDLG;
	CString strPName;
	int		iZdjh;
	float	fPDW;
	CString strPFX;
	float   fPAFX;
	CString	strPABZFX;
	CString strZdjType;
	CString	strSAType;
	CString strSATypeA;
	CString strSATypeB;
	CString strSAN;
	CString strSANA;
	CString strSANB;
	CString strZDJINFO;
	CString strSAFX;
	CString strSAAFX;
	CString strSABFX;
	float	fJZl;
	float	fJZL;
	float	fJZl1;
	float	fSH;
	float   fSHA,fSHB;
	float	fPH;
	CString	strMJFX;
	CString	strXBFX;
	CString strXBFXA;
	CString strXBFXB;
	float	fXBL;
	float	fXBLA;
	float	fXBLB;
	float	fLJMA;
	float	fLJMB;
};

CString GetPDMSVal(LPCSTR lpszData)
{
	const char szColonEn[]	=	":";
	const char szColonCHS[]	=	"：";
	const char szBlank[]	=	" ";
	char* pchar;
	pchar=strstr(lpszData,szColonCHS);
	if(!pchar)
	{
		pchar=strstr(lpszData,szColonEn);
		if(!pchar)
		{
			pchar=strstr(lpszData,szBlank);
			if(!pchar)
				return CString(_T(""));
		}
	}
	else pchar++;
	CString strRet=pchar+1;
	strRet.TrimLeft();
	strRet.TrimRight();
	return strRet;
}


void ReadPDMS()
{
	try
	{
   
		long X=0;
		CString YLJSVer;

		CString DataFileName;
		CString strTemp;
		bool  bExit=false; //退出标志
   
		ifstream f;
		//"您要替换" + EDIBgbl::strSelSpec + EDIBgbl::SelJcdm + "册中的支吊架原始数据吗？"
		strTemp.Format(GetResStr(IDS_ReplacePSADataInSpecVolume),EDIBgbl::SelSpecName,EDIBgbl::SelJcdm);
		if(ShowMessage(strTemp, MB_YESNO|MB_ICONQUESTION, GetResStr(IDS_AutoImportPSAData)) ==IDYES)
		{
			CFileDialogEx fileDlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,GetResStr(IDS_PDMSPIPINGDATEFILEALLFILE));
		

			int ii = fileDlg.DoModal();
			if(ii == IDOK)
			{
				//********注意下句不能移动到函数开头,否则死锁
				CString strSQL;
			
				ii = FrmTxsr.m_pViewTxsr->m_ActiveRs->State==adStateOpen;
				if(FrmTxsr.m_pViewTxsr->m_ActiveRs!=NULL && FrmTxsr.m_pViewTxsr->m_ActiveRs->State==adStateOpen)
				{
					
					strSQL=(char*)_bstr_t(FrmTxsr.m_pViewTxsr->m_ActiveRs->GetSource());
					FrmTxsr.m_pViewTxsr->m_bAllowUpd=false;
					if(!FrmTxsr.m_pViewTxsr->m_ActiveRs->adoEOF && !FrmTxsr.m_pViewTxsr->m_ActiveRs->BOF)
					{
						try
						{
							FrmTxsr.m_pViewTxsr->m_ActiveRs->Update();
						}
						catch(_com_error)
						{
							FrmTxsr.m_pViewTxsr->m_ActiveRs->CancelUpdate();
						}
					}
					//FrmTxsr.m_pViewTxsr->m_Databill.SetRefRecordset(NULL);
				
					FrmTxsr.m_pViewTxsr->m_Databill.SetRecordSource(NULL);
					FrmPhsData.m_DBGbill.SetRefDataSource(NULL);
					FrmTxsr.m_pViewTxsr->m_ActiveRs->Close();
					_RecordsetPtr rs(__uuidof(Recordset));
					rs->Open(_variant_t(strSQL),(IDispatch*)::conPRJDB,adOpenDynamic,adLockOptimistic,adCmdText);
					::PDMSToPHS(rs,fileDlg.GetPathName());
					
					if(rs->State == adStateOpen)
						rs->Close();
					FrmTxsr.m_pViewTxsr->m_bAllowUpd=true;
					FrmTxsr.m_pViewTxsr->m_SQL=strSQL;
					FrmTxsr.m_pViewTxsr->InitRs();
				}
			}
		}
	}
	catch(_com_error e)
	{
		ShowMessage(e.Description());
		FrmTxsr.m_pViewTxsr->m_bAllowUpd=true;
	}
}

BOOL PDMSToPHS(_RecordsetPtr rsZA,CString strFileName)
{
	USES_CONVERSION;
	CWaitCursor waitCursor;
	ifstream file(T2A((LPTSTR)(LPCTSTR)strFileName));
	
	if(!file.is_open())
		return FALSE;

	char szPName[64];
	strcpy(szPName,T2A((LPTSTR)(LPCTSTR)GetResStr(IDS_PIPINGNAME)));
	char szZDJH[64];
	strcpy(szZDJH,T2A((LPTSTR)(LPCTSTR)GetResStr(IDS_PHSMARK)));
	char szPDW[64];
	strcpy(szPDW,T2A((LPTSTR)(LPCTSTR)GetResStr(IDS_PIPINGOUTERPATH)));
	char szPFX[64];
	strcpy(szPFX,T2A((LPTSTR)(LPCTSTR)GetResStr(IDS_PIPINGWAY)));
	char szPAFX[64];
	strcpy(szPAFX,T2A((LPTSTR)(LPCTSTR)GetResStr(IDS_PIPINGFIX)));
	char szPH[64];
	strcpy(szPH,T2A((LPTSTR)(LPCTSTR)GetResStr(IDS_PIPINGCENTER_LEVEL)));
	char szZDJTYPE[64];
	strcpy(szZDJTYPE,T2A((LPTSTR)(LPCTSTR)GetResStr(IDS_PHSBASICTYPE)));
	char szSATYPE[64];
	strcpy(szSATYPE,T2A((LPTSTR)(LPCTSTR)GetResStr(IDS_BASEPARTFORM)));
	char szSATYPEA[64];
	strcpy(szSATYPEA,T2A((LPTSTR)(LPCTSTR)GetResStr(IDS_BASEPARTa_BASICFORM)));
	char szSATYPEB[64];
	strcpy(szSATYPEB,T2A((LPTSTR)(LPCTSTR)GetResStr(IDS_BASEPARTb_BASICFORM)));
	char szSH[64];
	strcpy(szSH,T2A((LPTSTR)(LPCTSTR)GetResStr(IDS_BASEPARTLEVEL)));
	char szSHA[64];
	strcpy(szSHA,T2A((LPTSTR)(LPCTSTR)GetResStr(IDS_BASEPARTaLEVEL)));
	char szSHB[64];
	strcpy(szSHB,T2A((LPTSTR)(LPCTSTR)GetResStr(IDS_BASEPARTbLEVEL)));
	char szSAJZFX[64];
	strcpy(szSAJZFX,T2A((LPTSTR)(LPCTSTR)GetResStr(IDS_BASEPART_SIMPLE_GIRDERWAY)));
	char szJZl[64];
	strcpy(szJZl,T2A((LPTSTR)(LPCTSTR)GetResStr(IDS_SIMPLE_GIRDERWAYLVAL1)));
	char szJZL[64];
	strcpy(szJZL,T2A((LPTSTR)(LPCTSTR)GetResStr(IDS_SIMPLE_GIRDERWAYLVAL)));
	char szJZl1[64];
	strcpy(szJZl1,T2A((LPTSTR)(LPCTSTR)GetResStr(IDS_SIMPLE_GIRDERWAYl1VAL)));
	char szMJLHA1[64];
	strcpy(szMJLHA1,T2A((LPTSTR)(LPCTSTR)GetResStr(IDS_BASEPART_INTER_PIECE_A1_LEVEL)));
	char szMJLHA2[64];
	strcpy(szMJLHA2,T2A((LPTSTR)(LPCTSTR)GetResStr(IDS_BASEPART_INTER_PIECE_A2_LEVEL)));
	char szMJLFXA1[64];
	strcpy(szMJLFXA1,T2A((LPTSTR)(LPCTSTR)GetResStr(IDS_BASEPART_INTER_PIECE_A1_WAY)));
	char szMJLFXA2[64];
	strcpy(szMJLFXA2,T2A((LPTSTR)(LPCTSTR)GetResStr(IDS_BASEPART_INTER_PIECE_A2_WAY)));
	char szMJFX[64];
	strcpy(szMJFX,T2A((LPTSTR)(LPCTSTR)GetResStr(IDS_INTER_PIECE_WAY)));
	char szMJFXA[64];
	strcpy(szMJFXA,T2A((LPTSTR)(LPCTSTR)GetResStr(IDS_BASEPARTA_INTER_PIECE_WAY)));
	char szMJFXB[64];
	strcpy(szMJFXB,T2A((LPTSTR)(LPCTSTR)GetResStr(IDS_BASEPARTB_INTER_PIECE_WAY)));
	char szXBFX[64];
	strcpy(szXBFX,T2A((LPTSTR)(LPCTSTR)GetResStr(IDS_CANTALEVEL_WAY)));
	char szXBFXA[64];
	strcpy(szXBFXA,T2A((LPTSTR)(LPCTSTR)GetResStr(IDS_BASEPARTa_CANTALEVELWAY)));
	char szXBFXB[64];
	strcpy(szXBFXB,T2A((LPTSTR)(LPCTSTR)GetResStr(IDS_BASEPARTb_CANTALEVELWAY)));
	char szXBL[64];
	strcpy(szXBL,T2A((LPTSTR)(LPCTSTR)GetResStr(IDS_CANTALEVEL_LENGTH)));
	char szXBLA[64];
	strcpy(szXBLA,T2A((LPTSTR)(LPCTSTR)GetResStr(IDS_BASEPARTa_CANTALEVELLENGTH)));
	char szXBLB[64];
	strcpy(szXBLB,T2A((LPTSTR)(LPCTSTR)GetResStr(IDS_BASEPARTa_CANTALEVELLENGTH)));
	char szLJMA[64];
	strcpy(szLJMA,T2A((LPTSTR)(LPCTSTR)GetResStr(IDS_GIRDER_SECTIONSIZEa)));
	char szLJMB[64];
	strcpy(szLJMB,T2A((LPTSTR)(LPCTSTR)GetResStr(IDS_GIRDER_SECTIONSIZEb)));
	char szLJMAA[64];
	strcpy(szLJMAA,T2A((LPTSTR)(LPCTSTR)GetResStr(IDS_BASEPARTaGROW_BASEGIRDERSECTIONSIZEa)));
	char szLJMAB[64];
	strcpy(szLJMAB,T2A((LPTSTR)(LPCTSTR)GetResStr(IDS_BASEPARTaGROW_BASEGIRDERSECTIONSIZEb)));
	char szLJMBA[64];
	strcpy(szLJMBA,T2A((LPTSTR)(LPCTSTR)GetResStr(IDS_BASEPARTbGROW_BASEGIRDERSECTIONSIZEa)));
	char szLJMBB[64];
	strcpy(szLJMBB,T2A((LPTSTR)(LPCTSTR)GetResStr(IDS_BASEPARTbGROW_BASEGIRDERSECTIONSIZEb)));
	char szPABZFX[64];
	strcpy(szPABZFX,T2A((LPTSTR)(LPCTSTR)GetResStr(IDS_PIPINGPART_LAYFIX)));
	
	const char szColonEn[]	=	":";
	const char szColonCHS[]	=	"：";
	const char szBlank[]	=	" ";
	
	const char szEnd[]		=	"End Of Data";
	const char szCpt[]		=	"--";

	CMapStringToString mapSAType;
	mapSAType[GetResStr(IDS_POLESIDE_CANTALEVER)]=_T("G25");
	mapSAType[GetResStr(IDS_POLEFACE_CANTALEVER)]=_T("G56");
	mapSAType[GetResStr(IDS_GIRDERBOTTOMCENTER_NOCANTALEVER)]=_T("GCBEAM");
	mapSAType[GetResStr(IDS_GIRDERSIDECENTER_CANTALEVER)]=_T("G21");
	mapSAType[GetResStr(IDS_SIMPLEBRANCHGIRDER)]=_T("G72");
	mapSAType[GetResStr(IDS_GIRDERBOTTOMCENTER_CANTALEVER)]=_T("G22");
	mapSAType[GetResStr(IDS_GIRDERTIPCENTER_CANTALEVER)]=_T("");

	CMapStringToString mapZdjType;
	mapZdjType[GetResStr(IDS_ODDPULLPOLE)]=_T("DLGDJ");
	mapZdjType[GetResStr(IDS_BOTHPULLPOLE)]=_T("SLGDJ");
	mapZdjType[GetResStr(IDS_BRACKET)]=_T("ZJ");
	mapZdjType[_T("ZJ")]=_T("ZJ");
	mapZdjType[_T("DLGDJ")]=_T("DLGDJ");
	mapZdjType[_T("SLGDJ")]=_T("SLGDJ");
	mapZdjType[_T("LGZJ")]=_T("LGZJ");
	
	CMapStringToString mapPAFX;//管道方向映射
	mapPAFX[_T("E")]=_T("X");
	mapPAFX[_T("N")]=_T("Y");
	mapPAFX[_T("UP")]=_T("Z");

	char szData[256];
	CString strFilter;
	CString strData;
	int i=0;

	CString strStatusTitle;
	strStatusTitle.LoadString(IDS_INPUTPDMS);
	frmStatus.SetWindowText(strStatusTitle);
	frmStatus.m_Label1.Format(IDS_INPUTPDMS);

	frmStatus.UpdateStatus(0.0f);
	frmStatus.ShowWindow(SW_SHOW);
	frmStatus.UpdateWindow();

	_RecordsetPtr rs1(__uuidof(Recordset));
	CString strSQL;
	while(!file.eof())
	{
		CPDMSINFO PDMSINFO;
		BOOL bOK=FALSE;
		bool bReadData = false;  //是否读到管道数据
		while(!file.eof())
		{
			file.getline(szData,sizeof(szData));
			
			//空行隔离不同的管道数据
			if(strstr(szData,szEnd) || (bReadData && (strcmp(szData,"") == 0)))
			{
				bOK=TRUE;
				break;
			}
			if(strcmp(szData,"") == 0 || strstr(szData,szCpt))
				continue;
			if(strstr(szData,szPName))
			{
				//管道名称
				PDMSINFO.strPName=GetPDMSVal(szData);
				bReadData = true;
			}
			else if(strstr(szData,szZDJH))
			{
				//支吊架号
				PDMSINFO.iZdjh=_ttoi(GetPDMSVal(szData));
			}
			else if(strstr(szData,szPDW))
			{
				//管道外径
				PDMSINFO.fPDW=_tcstod(GetPDMSVal(szData),NULL);
			}
			else if(strstr(szData,szPFX))
			{
				//管道方向
				strData=GetPDMSVal(szData);
				strData.MakeUpper();
				PDMSINFO.strPFX=mapPAFX[strData];
			}
			else if(strstr(szData,szPABZFX))
			{
				//管部布置方位
				strData=GetPDMSVal(szData);
			}
			else if(strstr(szData,szPAFX))
			{
				//管部布置方位
				strData=GetPDMSVal(szData);
				PDMSINFO.fPAFX=_tcstod(strData,NULL);
			}
			else if(strstr(szData,szPH))
			{
				//管道中心标高
				PDMSINFO.fPH=_tcstod(GetPDMSVal(szData),NULL);
			}
			else if(strstr(szData,szZDJTYPE))
			{
				//支吊架基本类型
				PDMSINFO.strZDJINFO=mapZdjType[GetPDMSVal(szData)];
			}
			else if(strstr(szData,szSATYPE))
			{
				//根部形式
				strData=GetPDMSVal(szData);
				PDMSINFO.strSAType=mapSAType[strData];
				PDMSINFO.strSAN=strData;
			}
			else if(strstr(szData,szSATYPEA))
			{
				//根部A基本形式
				strData=GetPDMSVal(szData);
				PDMSINFO.bIsDLG=FALSE;
				PDMSINFO.strSATypeA=mapSAType[strData];
				PDMSINFO.strSANA=strData;
			}
			else if(strstr(szData,szSATYPEB))
			{
				//根部B基本形式
				strData=GetPDMSVal(szData);
				PDMSINFO.bIsDLG=FALSE;
				PDMSINFO.strSATypeB=mapSAType[strData];
				PDMSINFO.strSANB=strData;
			}
			else if(strstr(szData,szSHA))
			{
				//根部A根部标高
				strData=GetPDMSVal(szData);
				PDMSINFO.fSHA=_tcstod(strData,NULL);
				PDMSINFO.fSH=PDMSINFO.fSHA;
			}
			else if(strstr(szData,szSHB))
			{
				//根部B根部标高
				strData=GetPDMSVal(szData);
				PDMSINFO.fSHB=_tcstod(strData,NULL);
			}
			else if(strstr(szData,szMJLHA1))
			{
				//根部埋件A1(l侧)标高
				strData=GetPDMSVal(szData);
				PDMSINFO.fSHA=_tcstod(strData,NULL);
				PDMSINFO.fSH=PDMSINFO.fSHA;
			}
			else if(strstr(szData,szMJLHA2))
			{
				//根部埋件A2(l侧)标高
				strData=GetPDMSVal(szData);
				PDMSINFO.fSHB=_tcstod(strData,NULL);
			}
			else if(strstr(szData,szSH))
			{
				//根部标高
				PDMSINFO.fSH=_tcstod(GetPDMSVal(szData),NULL);
			}
			else if(strstr(szData,szMJFX))
			{
				//埋件方向
				strData=GetPDMSVal(szData);
			}
			else if(strstr(szData,szXBFXA))
			{
				//根部A悬臂方向
				strData=GetPDMSVal(szData);
				PDMSINFO.strSAAFX=strData;
			}
			else if(strstr(szData,szXBFXB))
			{
				//根部B悬臂方向
				strData=GetPDMSVal(szData);
				PDMSINFO.strSABFX=strData;
			}
			else if(strstr(szData,szXBFX))
			{
				//悬臂方向
				strData=GetPDMSVal(szData);
				PDMSINFO.strSAFX=strData;
			}
			else if(strstr(szData,szXBL))
			{
				//悬臂长度
				PDMSINFO.fXBL=_tcstod(GetPDMSVal(szData),NULL);
			}
			else if(strstr(szData,szXBLA))
			{
				//根部A悬臂长度
			} 
			else if(strstr(szData,szXBLB))
			{
				//根部B悬臂长度
			}
			else if(strstr(szData,szJZl))
			{
				//简支梁l值
				PDMSINFO.fJZl=_tcstod(GetPDMSVal(szData),NULL);
				PDMSINFO.fXBL=PDMSINFO.fJZl;
			}
			else if(strstr(szData,szJZL))
			{
				//简支梁L值
				PDMSINFO.fJZL=_tcstod(GetPDMSVal(szData),NULL);
			}
			else if(strstr(szData,szJZl1))
			{
				//简支梁l1值
				PDMSINFO.fJZl1=_tcstod(GetPDMSVal(szData),NULL);
			}
			else if(strstr(szData,szLJMA))
			{
				//梁截面尺寸A
				PDMSINFO.fLJMA=_tcstod(GetPDMSVal(szData),NULL);
			}
			else if(strstr(szData,szLJMB))
			{
				//梁截面尺寸B
				PDMSINFO.fLJMB=_tcstod(GetPDMSVal(szData),NULL);
			}
			else
			{
				//无匹配串
			}
		}
		if(bOK && PDMSINFO.iZdjh != 0)
		{
			try
			{
				strFilter.Format(_T("ZDJH=%d"),PDMSINFO.iZdjh);
				rsZA->Filter=_bstr_t(strFilter);
				if(rsZA->adoEOF )
				{
					rsZA->AddNew();
					rsZA->PutCollect("ZDJH",(long)PDMSINFO.iZdjh);
					rsZA->PutCollect("VolumeID",(long)EDIBgbl::SelVlmID);
				}
				try
				{
				rsZA->PutCollect(_T("IsPDMSOut"),_variant_t(true));
				rsZA->PutCollect("DJ1",PDMSINFO.fPDW);
				rsZA->PutCollect("DF1",_variant_t(PDMSINFO.strPFX));
				rsZA->PutCollect("TYPE",_variant_t(PDMSINFO.strZdjType));
				rsZA->PutCollect("GN1",_variant_t(PDMSINFO.strSAType));
				rsZA->PutCollect("GH1",PDMSINFO.fSH/1000.0f);
				rsZA->PutCollect("DH1",PDMSINFO.fPH/1000.0f);
				rsZA->PutCollect("GDW1",PDMSINFO.fXBL);
				rsZA->PutCollect("LC1",PDMSINFO.fJZl1);
				rsZA->PutCollect("IsDLG",(bool)PDMSINFO.bIsDLG);
				rsZA->PutCollect("SAN1",(LPCTSTR)PDMSINFO.strSAN);
				rsZA->PutCollect("GDMC",(LPCTSTR)PDMSINFO.strPName);
				rsZA->PutCollect("ZDJINFO",(LPCTSTR)PDMSINFO.strZDJINFO);
				rsZA->PutCollect("PAAngle",PDMSINFO.fPAFX);
				rsZA->PutCollect("SAFX",(LPCTSTR)PDMSINFO.strSAFX);
				rsZA->PutCollect("SAAFX",(LPCTSTR)PDMSINFO.strSAAFX);
				rsZA->PutCollect("SABFX",(LPCTSTR)PDMSINFO.strSABFX);

				rsZA->Update();
				}
				catch(_com_error e)
				{
				
					rsZA->CancelUpdate();
					
					if(rsZA->State == adStateOpen)
						rsZA->Close();

					file.close();
					frmStatus.UpdateStatus(1.0f);
					frmStatus.UpdateWindow();
					frmStatus.ShowWindow(SW_HIDE);
					
					AfxMessageBox(e.Description());
					return false;
				}
				if(!PDMSINFO.bIsDLG)
				{
					//是双吊
					strSQL.Format(_T("SELECT * FROM [Z1] WHERE VolumeID=%d AND Zdjh=%d ORDER BY nth"),EDIBgbl::SelVlmID,PDMSINFO.iZdjh);
					rs1->Open(_variant_t(strSQL),(IDispatch*)::conPRJDB ,adOpenStatic,adLockOptimistic,adCmdText);
					rs1->Find(_T("nth=1"),0,adSearchForward);
					if(rs1->adoEOF)
					{
						rs1->AddNew();
						rs1->PutCollect(_T("ZDJH"),(long)PDMSINFO.iZdjh);
						rs1->PutCollect(_T("VolumeID"),(long)EDIBgbl::SelVlmID);
						rs1->PutCollect(_T("nth"),1L);
					}
					rs1->PutCollect(_T("IDGN1"),(LPCTSTR)PDMSINFO.strSATypeA);
					rs1->PutCollect(_T("SAN1"),(LPCTSTR)PDMSINFO.strSANA);
					rs1->PutCollect(_T("SAFX"),(LPCTSTR)PDMSINFO.strSAAFX);
					rs1->Update();
					rs1->MoveFirst();

					rs1->Find(_T("nth=2"),0,adSearchForward);
					if(rs1->adoEOF)
					{
						rs1->AddNew();
						rs1->PutCollect(_T("ZDJH"),(long)PDMSINFO.iZdjh);
						rs1->PutCollect(_T("VolumeID"),(long)EDIBgbl::SelVlmID);
						rs1->PutCollect(_T("nth"),2L);
					}
					rs1->PutCollect(_T("IDGN1"),(LPCTSTR)PDMSINFO.strSATypeB);
					rs1->PutCollect(_T("SAN1"),(LPCTSTR)PDMSINFO.strSANB);
					rs1->PutCollect(_T("SAFX"),(LPCTSTR)PDMSINFO.strSABFX);
					rs1->Update();
					rs1->Close();
				}
				i++;
				frmStatus.UpdateStatus(i/100.0f);
				frmStatus.UpdateWindow();
			}
			catch(_com_error e)
			{
				if(rs1->State == adStateOpen)
					rs1->Close();

				if(rsZA->State == adStateOpen)
						rsZA->Close();

				file.close();
				frmStatus.UpdateStatus(1.0f);
				frmStatus.UpdateWindow();
				frmStatus.ShowWindow(SW_HIDE);
				
				AfxMessageBox(e.Description());
				return false;
			}
		}
	}
	file.close();
	frmStatus.UpdateStatus(1.0f);
	frmStatus.UpdateWindow();
	frmStatus.ShowWindow(SW_HIDE);
	return TRUE;
}

BOOL PHSToPDMS(CString strFileName)
{
	CWaitCursor waitCursor;
	USES_CONVERSION;
	ofstream file(T2A((LPTSTR)(LPCTSTR)strFileName),ios::out);
	if(!file.is_open())
		return FALSE;
	const char	szGDMC[]	="GDMC";
	const char	szZDJINFO[]	="ZDJINFO";
	const char	szZDJELE[]	="ZDJELE";
	const char	szDef[]		="";
	const char  szEn[]		="\n";
	CString strTemp;
	CString strClgg;
	int iTemp;
	int iIndex;
	CString astrNut[10];// 保存附件,螺栓螺母的数组
	int iNutCount=0;//附件,螺栓螺母数组数量
	BOOL bSAT3=FALSE;//判断在吊第一路前面是否碰到了SA or T3 
	BOOL bROD=FALSE;//判断在吊第一路前面是否碰到了ROD
	int iOldClassID=0;
	int iClassID=0;
	CString strID;
	int i;
	float fA,fB;
	int inth;
	int iOldnth;
	try
	{
		_RecordsetPtr rsZA,rsZ1,rsZB,rsGDMC;
		CString strSQL;
		strSQL.Format(_T("SELECT DISTINCT GDMC FROM [ZA] WHERE [VolumeID]=%d AND ISPDMSOUT IS NOT NULL AND ISPDMSOUT <> 0 AND GDMC IS NOT NULL AND GDMC <> \'\' ORDER BY GDMC"),
			EDIBgbl::SelVlmID);
		rsGDMC=::conPRJDB->Execute(_bstr_t(strSQL),NULL,adCmdText);
		for(;!rsGDMC->adoEOF;rsGDMC->MoveNext())
		{
			CString strGDMC=vtos(rsGDMC->GetCollect(_T("GDMC")));
			file<<szGDMC<<","<<strGDMC<<szEn;
			strSQL.Format(_T("SELECT * FROM [ZA] WHERE [VolumeID]=%d AND ISPDMSOUT IS NOT NULL AND ISPDMSOUT <> 0 AND GDMC=\'%s\' AND bCalSuccess <> 0 AND bCalSuccess Is not null"),EDIBgbl::SelVlmID,strGDMC);
			rsZA=::conPRJDB->Execute((LPCTSTR)strSQL,NULL,adCmdText);
			for(;!rsZA->adoEOF;rsZA->MoveNext())
			{
				int iZdjh=vtoi(rsZA->GetCollect("ZDJH"));
				file<<szZDJINFO<<","<<iZdjh<<","<<vtos(rsZA->GetCollect("ZDJINFO"))<<szDef<<szEn;
				if(vtob(rsZA->GetCollect("ISDLG")))
				{
					//是单拉杆
					strSQL.Format(_T("SELECT * FROM [ZB] WHERE [VolumeID]=%d AND [ZDJH]=%d AND [nth] =1 AND SEQ >0 ORDER BY [recno]"),
						EDIBgbl::SelVlmID,iZdjh);
					rsZB=::conPRJDB->Execute((LPCTSTR)strSQL,NULL,adCmdText);
					for(iNutCount=0;!rsZB->adoEOF;rsZB->MoveNext())
					{
						iIndex=vtoi(rsZB->GetCollect(_T("Index")));
						strID=vtos(rsZB->GetCollect(_T("ID")));
						strID.MakeUpper();
						strClgg=vtos(rsZB->GetCollect(_T("CLGG")));
						iClassID=vtoi(rsZB->GetCollect(_T("ClassID")));
						fA=vtof(rsZB->GetCollect(_T("A")));
						fB=vtof(rsZB->GetCollect(_T("B")));
						if(iIndex==iSA)
						{
							if(strID=="GIBEAM" || strID=="GCBEAM")
								continue;
						}
						if(rsZB->GetCollect("Index").vt==VT_NULL)
							iIndex=-1;
						if(iIndex==iSA || (iClassID==iSPR && strID=="T3"))
						{
							for(i=iNutCount-1;i>=0;i--)
								file<<astrNut[i];
							iNutCount=0;
						} 
						else if(iClassID!=iAttached && iClassID!=iNuts && iClassID!=iBolts)
						{
							for(i=0;i<iNutCount;i++)
								file<<astrNut[i];
							iNutCount=0;
						}
						
						
						switch(iIndex)
						{
						case iPA:
							file<<szZDJELE<<","<<iZdjh<<",";
							file<<T2A((LPTSTR)(LPCTSTR)strClgg)<<","<<vtof(rsZA->GetCollect("PAAngle"));
							file<<szEn;
							break;
						case iSA:
							file<<szZDJELE<<","<<iZdjh<<",";
							file<<T2A((LPTSTR)(LPCTSTR)strClgg)<<","<<vtos(rsZA->GetCollect("SAFX"));
							file<<szEn;
							break;
						default:
							switch(iClassID)
							{
							case iROD:
								file<<szZDJELE<<","<<iZdjh<<",";
								file<<T2A((LPTSTR)(LPCTSTR)strClgg)<<","<<fA<<","<<fB;
								file<<szEn;
								break;
							case iSPR:
								file<<szZDJELE<<","<<iZdjh<<",";
								iTemp=strClgg.Find(_T(","));
								if(iTemp>=0)
									strClgg=strClgg.Left(iTemp);
								/*strTemp=vtos(rsZB->GetCollect("ID"));
								strTemp.MakeUpper();
								if(strTemp=="T5" || strTemp=="LHG")	
								{
									strTemp.Format("(C=%g)",vtof(rsZB->GetCollect("sizeC")));
									strClgg+=strTemp;
								}*/
								file<<T2A((LPTSTR)(LPCTSTR)strClgg)<<","<<vtof(rsZB->GetCollect(_T("HAZ")));
								file<<szEn;
								break;
							case iCSPR:
								file<<szZDJELE<<","<<iZdjh<<",";
								file<<T2A((LPTSTR)(LPCTSTR)strClgg)<<",";			//规格串
								file<<vtos(rsZB->GetCollect(_T("SpringNo")))<<",";//规格号
								file<<vtos(rsZB->GetCollect(_T("LugDia")))<<",";//拉杆直径
								file<<vtos(rsZB->GetCollect(_T("SumDist")))<<",";//总位移
								file<<vtos(rsZB->GetCollect(_T("HeatDisp")));//实际位移
								//file<<vtos(rsZB->GetCollect(_T("Pgz")));//荷载
								file<<szEn;
								break;
							case iAttached:
							case iNuts:
							case iBolts:
								astrNut[iNutCount].Format(_T("%s,%d,%s%s"),szZDJELE,iZdjh,strClgg,szEn);
								iNutCount++;
								break;
							default:
								file<<szZDJELE<<","<<iZdjh<<",";
								file<<T2A((LPTSTR)(LPCTSTR)strClgg);
								file<<szEn;
							}
						}
						
					}
					rsZB->Close();
				}
				else
				{
					//双拉杆

					if(!vtob(rsZA->GetCollect(_T("Iscal"))))
					{
						//不是由 AutoPHS 程序计算的支吊架

						//输出不是管部且 nth 最小的一路
						strSQL.Format(_T("SELECT * FROM [Z1] WHERE [VolumeID]=%d AND [ZDJH]=%d AND [nth] IN ")
							_T("(SELECT DISTINCT [nth] FROM [ZB] WHERE VolumeID=%d AND ZDJH=%d AND INDEX <> %d) ORDER BY [nth]"),
							EDIBgbl::SelVlmID,iZdjh,EDIBgbl::SelVlmID,iZdjh,iPA);
						rsZ1=::conPRJDB->Execute((LPCTSTR)strSQL,NULL,adCmdText);
						inth=vtoi(rsZ1->GetCollect("nth"));
						iOldnth=inth;
						strSQL.Format(_T("SELECT * FROM [ZB] WHERE [VolumeID]=%d AND [ZDJH]=%d AND [nth]=%d AND SEQ >0 ORDER BY [recno] DESC"),
							EDIBgbl::SelVlmID,iZdjh,inth);
						rsZB=::conPRJDB->Execute((LPCTSTR)strSQL,NULL,adCmdText);
						for(iNutCount=0,bSAT3=FALSE,bROD=FALSE;!rsZB->adoEOF;rsZB->MoveNext())
						{
							iIndex=vtoi(rsZB->GetCollect("Index"));
							strID=vtos(rsZB->GetCollect("ID"));
							strID.MakeUpper();
							iClassID=vtoi(rsZB->GetCollect("ClassID"));
							strClgg=vtos(rsZB->GetCollect("CLGG"));
							fA=vtof(rsZB->GetCollect("A"));
							fB=vtof(rsZB->GetCollect("B"));
							if(iIndex==iSA)
							{
								if(strID=="GIBEAM" || strID=="GCBEAM")
									continue;
							}
							if(rsZB->GetCollect("Index").vt==VT_NULL)
								iIndex=-1;
							if((iClassID==iROD && bSAT3) /*|| ((iIndex==iPA || strID=="T3A") && bROD)*/)
							{
								for(i=iNutCount-1;i>=0;i--)
									file<<astrNut[i];
								iNutCount=0;
								if(iClassID==iROD) bSAT3=FALSE;
								if((iIndex==iPA || strID=="T3A")) bROD=FALSE;
							} 
							else if(iClassID!=iAttached && iClassID!=iNuts && iClassID!=iBolts)
							{
								for(i=0;i<iNutCount;i++)
									file<<astrNut[i];
								iNutCount=0;
							}
							if(iIndex==iSA || strID=="T3")
								bSAT3=TRUE; 
							else if(iClassID!=iAttached && iClassID!=iNuts && iClassID!=iBolts)
								bSAT3=FALSE;
							if(iClassID==iROD)
								bROD=TRUE;
							else if(iClassID!=iAttached && iClassID!=iNuts && iClassID!=iBolts)
								bROD=FALSE;
							switch(iIndex)
							{
							case iPA:
								file<<szZDJELE<<","<<iZdjh<<",";
								file<<T2A((LPTSTR)(LPCTSTR)strClgg)<<","<<vtof(rsZA->GetCollect("PAAngle"));
								file<<szEn;
								break;
							case iSA:
								file<<szZDJELE<<","<<iZdjh<<",";
								file<<T2A((LPTSTR)(LPCTSTR)strClgg)<<","<<vtos(rsZ1->GetCollect("SAFX"));
								file<<szEn;
								break;
							default:
								switch(vtoi(rsZB->GetCollect(_T("ClassID"))))
								{
								case iROD:
									file<<szZDJELE<<","<<iZdjh<<",";
									file<<T2A((LPTSTR)(LPCTSTR)strClgg)<<","<<fB<<","<<fA;
									file<<szEn;
									break;
								case iSPR:
									file<<szZDJELE<<","<<iZdjh<<",";
									iTemp=strClgg.Find(_T(","));
									if(iTemp>=0)
										strClgg=strClgg.Left(iTemp);
									/*strTemp=vtos(rsZB->GetCollect("ID"));
									strTemp.MakeUpper();
									if(strTemp=="T5" || strTemp=="LHG")	
									{
										strTemp.Format("(C=%g)",vtof(rsZB->GetCollect("sizeC")));
										strClgg+=strTemp;
									}*/
									file<<T2A((LPTSTR)(LPCTSTR)strClgg)<<","<<vtof(rsZB->GetCollect(_T("HAZ")));
									file<<szEn;
									break;
								case iCSPR:
									file<<szZDJELE<<","<<iZdjh<<",";
									file<<T2A((LPTSTR)(LPCTSTR)strClgg)<<",";			//规格串
									file<<vtos(rsZB->GetCollect(_T("SpringNo")))<<",";//规格号
									file<<vtos(rsZB->GetCollect(_T("LugDia")))<<",";//拉杆直径
									file<<vtos(rsZB->GetCollect(_T("SumDist")))<<",";//总位移
									file<<vtos(rsZB->GetCollect(_T("HeatDisp")));//实际位移
									//file<<vtos(rsZB->GetCollect(_T("Pgz")));//荷载
									file<<szEn;
									break;
								case iAttached:
								case iNuts:
								case iBolts:
									astrNut[iNutCount].Format(_T("%s,%d,%s%s"),szZDJELE,iZdjh,strClgg,szEn);
									iNutCount++;
									break;
								default:
									file<<szZDJELE<<","<<iZdjh<<",";
									file<<T2A((LPTSTR)(LPCTSTR)strClgg);
									file<<szEn;
								}
							}
						}
						rsZB->Close();
						rsZ1->Close();
						
						//输出是管部的一路
						strSQL.Format(_T("SELECT * FROM [Z1] WHERE [VolumeID]=%d AND [ZDJH]=%d AND [nth] IN ")
							_T("(SELECT DISTINCT [nth] FROM [ZB] WHERE VolumeID=%d AND ZDJH=%d AND INDEX = %d) ORDER BY [nth]"),
							EDIBgbl::SelVlmID,iZdjh,EDIBgbl::SelVlmID,iZdjh,iPA);
						rsZ1=::conPRJDB->Execute((LPCTSTR)strSQL,NULL,adCmdText);
						inth=vtoi(rsZ1->GetCollect("nth"));
						strSQL.Format(_T("SELECT * FROM [ZB] WHERE [VolumeID]=%d AND [ZDJH]=%d AND [nth]=%d AND SEQ >0 ORDER BY [recno] "),
							EDIBgbl::SelVlmID,iZdjh,inth);
						rsZB=::conPRJDB->Execute((LPCTSTR)strSQL,NULL,adCmdText);

						for(iNutCount=0;!rsZB->adoEOF;rsZB->MoveNext())
						{

							strID=vtos(rsZB->GetCollect("ID"));
							strID.MakeUpper();
							iIndex=vtoi(rsZB->GetCollect("Index"));
							iClassID=vtoi(rsZB->GetCollect("ClassID"));
							strClgg=vtos(rsZB->GetCollect("CLGG"));
							fA=vtof(rsZB->GetCollect("A"));
							fB=vtof(rsZB->GetCollect("B"));
							if(strID=="T5" || strID=="LHG")	
								continue;
							if(iIndex==iSA)
							{
								if(strID=="GIBEAM" || strID=="GCBEAM")
									continue;
							}
							if(rsZB->GetCollect("Index").vt==VT_NULL)
								iIndex=-1;
							if(iIndex==iSA || (iClassID==iSPR && strID=="T3"))
							{
								for(i=iNutCount-1;i>=0;i--)
									file<<astrNut[i];
								iNutCount=0;
							} 
							else if(iClassID!=iAttached && iClassID!=iNuts && iClassID!=iBolts)
							{
								for(i=0;i<iNutCount;i++)
									file<<astrNut[i];
								iNutCount=0;
							}
							
							switch(iIndex)
							{
							case iPA:
								file<<szZDJELE<<","<<iZdjh<<",";
								file<<T2A((LPTSTR)(LPCTSTR)strClgg)<<","<<vtof(rsZA->GetCollect("PAAngle"));
								file<<szEn;
								break;
							case iSA:
								file<<szZDJELE<<","<<iZdjh<<",";
								file<<T2A((LPTSTR)(LPCTSTR)strClgg)<<","<<vtos(rsZ1->GetCollect("SAFX"));
								file<<szEn;
								break;
							default:
								switch(iClassID)
								{
								case iROD:
									file<<szZDJELE<<","<<iZdjh<<",";
									file<<T2A((LPTSTR)(LPCTSTR)strClgg)<<","<<fA<<","<<fB;
									file<<szEn;
									break;
								case iSPR:
									file<<szZDJELE<<","<<iZdjh<<",";
									iTemp=strClgg.Find(_T(","));
									if(iTemp>=0)
										strClgg=strClgg.Left(iTemp);
									/*strTemp=vtos(rsZB->GetCollect("ID"));
									strTemp.MakeUpper();
									if(strTemp=="T5" || strTemp=="LHG")	
									{
										strTemp.Format("(C=%g)",vtof(rsZB->GetCollect("sizeC")));
										strClgg+=strTemp;
									}*/
									file<<T2A((LPTSTR)(LPCTSTR)strClgg)<<","<<vtof(rsZB->GetCollect(_T("HAZ")));
									file<<szEn;
									break;
								case iCSPR:
									file<<szZDJELE<<","<<iZdjh<<",";
									file<<T2A((LPTSTR)(LPCTSTR)strClgg)<<",";			//规格串
									file<<vtos(rsZB->GetCollect(_T("SpringNo")))<<",";//规格号
									file<<vtos(rsZB->GetCollect(_T("LugDia")))<<",";//拉杆直径
									file<<vtos(rsZB->GetCollect(_T("SumDist")))<<",";//总位移
									file<<vtos(rsZB->GetCollect(_T("HeatDisp")));//实际位移
									//file<<vtos(rsZB->GetCollect(_T("Pgz")));//荷载
									file<<szEn;
									break;
								case iAttached:
								case iNuts:
								case iBolts:
									astrNut[iNutCount].Format(_T("%s,%d,%s%s"),szZDJELE,iZdjh,strClgg,szEn);
									iNutCount++;
									break;
								default:
									file<<szZDJELE<<","<<iZdjh<<",";
									file<<T2A((LPTSTR)(LPCTSTR)strClgg);
									file<<szEn;
								}
							}
							
						}
						rsZB->Close();
						rsZ1->Close();

						//输出 (不是管部 AND 前面没有输出的 AND nth 最小) 的一路
						strSQL.Format(_T("SELECT * FROM [Z1] WHERE [VolumeID]=%d AND [ZDJH]=%d AND [nth]<>%d AND [nth] IN ")
							_T("(SELECT DISTINCT [nth] FROM [ZB] WHERE VolumeID=%d AND ZDJH=%d AND INDEX = %d) ORDER BY [nth]"),
							EDIBgbl::SelVlmID,iZdjh,iOldnth,EDIBgbl::SelVlmID,iZdjh,iPA);
						rsZ1=::conPRJDB->Execute((LPCTSTR)strSQL,NULL,adCmdText);
						inth=vtoi(rsZ1->GetCollect("nth"));
						strSQL.Format(_T("SELECT * FROM [ZB] WHERE [VolumeID]=%d AND [ZDJH]=%d AND [nth]=%d AND SEQ >0 ORDER BY [recno] "),
							EDIBgbl::SelVlmID,iZdjh,inth);
						rsZB=::conPRJDB->Execute((LPCTSTR)strSQL,NULL,adCmdText);
						for(iNutCount=0;!rsZB->adoEOF;rsZB->MoveNext())
						{

							strID=vtos(rsZB->GetCollect("ID"));
							strID.MakeUpper();
							iIndex=vtoi(rsZB->GetCollect("Index"));
							iClassID=vtoi(rsZB->GetCollect("ClassID"));
							strClgg=vtos(rsZB->GetCollect("CLGG"));
							fA=vtof(rsZB->GetCollect("A"));
							fB=vtof(rsZB->GetCollect("B"));
							if(strID=="T5" || strID=="LHG")	
								continue;
							if(iIndex==iSA)
							{
								if(strID=="GIBEAM" || strID=="GCBEAM")
									continue;
							}
							if(rsZB->GetCollect("Index").vt==VT_NULL)
								iIndex=-1;
							if(iIndex==iSA || (iClassID==iSPR && strID=="T3"))
							{
								for(i=iNutCount-1;i>=0;i--)
									file<<astrNut[i];
								iNutCount=0;
							} 
							else if(iClassID!=iAttached && iClassID!=iNuts && iClassID!=iBolts)
							{
								for(i=0;i<iNutCount;i++)
									file<<astrNut[i];
								iNutCount=0;
							}
							
							switch(iIndex)
							{
							case iPA:
								file<<szZDJELE<<","<<iZdjh<<",";
								file<<T2A((LPTSTR)(LPCTSTR)strClgg)<<","<<vtof(rsZA->GetCollect("PAAngle"));
								file<<szEn;
								break;
							case iSA:
								file<<szZDJELE<<","<<iZdjh<<",";
								file<<T2A((LPTSTR)(LPCTSTR)strClgg)<<","<<vtos(rsZ1->GetCollect("SAFX"));
								file<<szEn;
								break;
							default:
								switch(iClassID)
								{
								case iROD:
									file<<szZDJELE<<","<<iZdjh<<",";
									file<<T2A((LPTSTR)(LPCTSTR)strClgg)<<","<<fA<<","<<fB;
									file<<szEn;
									break;
								case iSPR:
									file<<szZDJELE<<","<<iZdjh<<",";
									iTemp=strClgg.Find(_T(","));
									if(iTemp>=0)
										strClgg=strClgg.Left(iTemp);
									/*strTemp=vtos(rsZB->GetCollect("ID"));
									strTemp.MakeUpper();
									if(strTemp=="T5" || strTemp=="LHG")	
									{
										strTemp.Format("(C=%g)",vtof(rsZB->GetCollect("sizeC")));
										strClgg+=strTemp;
									}*/
									file<<T2A((LPTSTR)(LPCTSTR)strClgg)<<","<<vtof(rsZB->GetCollect(_T("HAZ")));
									file<<szEn;
									break;
								case iCSPR:
									file<<szZDJELE<<","<<iZdjh<<",";
									file<<T2A((LPTSTR)(LPCTSTR)strClgg)<<",";			//规格串
									file<<vtos(rsZB->GetCollect(_T("SpringNo")))<<",";//规格号
									file<<vtos(rsZB->GetCollect(_T("LugDia")))<<",";//拉杆直径
									file<<vtos(rsZB->GetCollect(_T("SumDist")))<<",";//总位移
									file<<vtos(rsZB->GetCollect(_T("HeatDisp")));//实际位移
									//file<<vtos(rsZB->GetCollect(_T("Pgz")));//荷载
									file<<szEn;
									break;
								case iAttached:
								case iNuts:
								case iBolts:
									astrNut[iNutCount].Format(_T("%s,%d,%s%s"),szZDJELE,iZdjh,strClgg,szEn);
									iNutCount++;
									break;
								default:
									file<<szZDJELE<<","<<iZdjh<<",";
									file<<T2A((LPTSTR)(LPCTSTR)strClgg);
									file<<szEn;
								}
							}
							
						}
						rsZB->Close();
						rsZ1->Close();
					}
					else
					{
						//是由 AutoPHS 程序计算的支吊架
						strSQL.Format(_T("SELECT * FROM [ZB] WHERE [VolumeID]=%d AND [ZDJH]=%d AND [nth]=1 AND SEQ >0 ORDER BY [recno] DESC"),
						EDIBgbl::SelVlmID,iZdjh);
						rsZB=::conPRJDB->Execute((LPCTSTR)strSQL,NULL,adCmdText);
						strSQL.Format(_T("SELECT * FROM [Z1] WHERE [VolumeID]=%d AND [ZDJH]=%d AND [nth]=1 "),EDIBgbl::SelVlmID,iZdjh);
						rsZ1=::conPRJDB->Execute((LPCTSTR)strSQL,NULL,adCmdText);
						for(iNutCount=0,bSAT3=FALSE,bROD=FALSE;!rsZB->adoEOF;rsZB->MoveNext())
						{
							iIndex=vtoi(rsZB->GetCollect("Index"));
							strID=vtos(rsZB->GetCollect("ID"));
							strID.MakeUpper();
							iClassID=vtoi(rsZB->GetCollect("ClassID"));
							strClgg=vtos(rsZB->GetCollect("CLGG"));
							fA=vtof(rsZB->GetCollect("A"));
							fB=vtof(rsZB->GetCollect("B"));
							if(iIndex==iSA)
							{
								if(strID=="GIBEAM" || strID=="GCBEAM")
									continue;
							}
							if(rsZB->GetCollect("Index").vt==VT_NULL)
								iIndex=-1;
							if((iClassID==iROD && bSAT3) /*|| ((iIndex==iPA || strID=="T3A") && bROD)*/)
							{
								for(i=iNutCount-1;i>=0;i--)
									file<<astrNut[i];
								iNutCount=0;
								if(iClassID==iROD) bSAT3=FALSE;
								if((iIndex==iPA || strID=="T3A")) bROD=FALSE;
							} 
							else if(iClassID!=iAttached && iClassID!=iNuts && iClassID!=iBolts)
							{
								for(i=0;i<iNutCount;i++)
									file<<astrNut[i];
								iNutCount=0;
							}
							if(iIndex==iSA || strID=="T3")
								bSAT3=TRUE; 
							else if(iClassID!=iAttached && iClassID!=iNuts && iClassID!=iBolts)
								bSAT3=FALSE;
							if(iClassID==iROD)
								bROD=TRUE;
							else if(iClassID!=iAttached && iClassID!=iNuts && iClassID!=iBolts)
								bROD=FALSE;
							switch(iIndex)
							{
							case iPA:
								file<<szZDJELE<<","<<iZdjh<<",";
								file<<T2A((LPTSTR)(LPCTSTR)strClgg)<<","<<vtof(rsZA->GetCollect("PAAngle"));
								file<<szEn;
								break;
							case iSA:
								file<<szZDJELE<<","<<iZdjh<<",";
								file<<T2A((LPTSTR)(LPCTSTR)strClgg)<<","<<vtos(rsZA->GetCollect("SAAFX"));
								file<<szEn;
								break;
							default:
								switch(vtoi(rsZB->GetCollect(_T("ClassID"))))
								{
								case iROD:
									file<<szZDJELE<<","<<iZdjh<<",";
									file<<T2A((LPTSTR)(LPCTSTR)strClgg)<<","<<fB<<","<<fA;
									file<<szEn;
									break;
								case iSPR:
									file<<szZDJELE<<","<<iZdjh<<",";
									iTemp=strClgg.Find(_T(","));
									if(iTemp>=0)
										strClgg=strClgg.Left(iTemp);
									/*strTemp=vtos(rsZB->GetCollect("ID"));
									strTemp.MakeUpper();
									if(strTemp=="T5" || strTemp=="LHG")	
									{
										strTemp.Format("(C=%g)",vtof(rsZB->GetCollect("sizeC")));
										strClgg+=strTemp;
									}*/
									file<<T2A((LPTSTR)(LPCTSTR)strClgg)<<","<<vtof(rsZB->GetCollect(_T("HAZ")));
									file<<szEn;
									break;
								case iCSPR:
									file<<szZDJELE<<","<<iZdjh<<",";
									file<<T2A((LPTSTR)(LPCTSTR)strClgg)<<",";			//规格串
									file<<vtos(rsZB->GetCollect(_T("SpringNo")))<<",";//规格号
									file<<vtos(rsZB->GetCollect(_T("LugDia")))<<",";//拉杆直径
									file<<vtos(rsZB->GetCollect(_T("SumDist")))<<",";//总位移
									file<<vtos(rsZB->GetCollect(_T("HeatDisp")));//实际位移
									//file<<vtos(rsZB->GetCollect(_T("Pgz")));//荷载
									file<<szEn;
									break;
								case iAttached:
								case iNuts:
								case iBolts:
									astrNut[iNutCount].Format(_T("%s,%d,%s%s"),szZDJELE,iZdjh,strClgg,szEn);
									iNutCount++;
									break;
								default:
									file<<szZDJELE<<","<<iZdjh<<",";
									file<<T2A((LPTSTR)(LPCTSTR)strClgg);
									file<<szEn;
								}
							}
						}
						rsZB->Close();
						rsZ1->Close();

						strSQL.Format(_T("SELECT * FROM [ZB] WHERE [VolumeID]=%d AND [ZDJH]=%d AND [nth]=1 AND [recno]<>1 AND  SEQ >0 ORDER BY [recno]"),
							EDIBgbl::SelVlmID,iZdjh);
						rsZB=::conPRJDB->Execute((LPCTSTR)strSQL,NULL,adCmdText);
						strSQL.Format(_T("SELECT * FROM [Z1] WHERE [VolumeID]=%d AND [ZDJH]=%d AND [nth]=1 "),EDIBgbl::SelVlmID,iZdjh);
						rsZ1=::conPRJDB->Execute((LPCTSTR)strSQL,NULL,adCmdText);
						for(iNutCount=0;!rsZB->adoEOF;rsZB->MoveNext())
						{
							strID=vtos(rsZB->GetCollect("ID"));
							strID.MakeUpper();
							iIndex=vtoi(rsZB->GetCollect("Index"));
							iClassID=vtoi(rsZB->GetCollect("ClassID"));
							strClgg=vtos(rsZB->GetCollect("CLGG"));
							fA=vtof(rsZB->GetCollect("A"));
							fB=vtof(rsZB->GetCollect("B"));
							if(strID=="T5" || strID=="LHG")	
								continue;
							if(iIndex==iSA)
							{
								if(strID=="GIBEAM" || strID=="GCBEAM")
									continue;
							}
							strClgg=vtos(rsZB->GetCollect("CLGG"));
							if(rsZB->GetCollect("Index").vt==VT_NULL)
								iIndex=-1;
							if(iIndex==iSA || (iClassID==iSPR && strID=="T3"))
							{
								for(i=iNutCount-1;i>=0;i--)
									file<<astrNut[i];
								iNutCount=0;
							} 
							else if(iClassID!=iAttached && iClassID!=iNuts && iClassID!=iBolts)
							{
								for(i=0;i<iNutCount;i++)
									file<<astrNut[i];
								iNutCount=0;
							}
							
							switch(iIndex)
							{
							case iPA:
								file<<szZDJELE<<","<<iZdjh<<",";
								file<<T2A((LPTSTR)(LPCTSTR)strClgg)<<","<<vtof(rsZA->GetCollect("PAAngle"));
								file<<szEn;
								break;

							case iSA:
								file<<szZDJELE<<","<<iZdjh<<",";
								file<<T2A((LPTSTR)(LPCTSTR)strClgg)<<","<<vtos(rsZA->GetCollect("SABFX"));
								file<<szEn;
								break;
							default:
								switch(iClassID)
								{
								case iROD:
									file<<szZDJELE<<","<<iZdjh<<",";
									file<<T2A((LPTSTR)(LPCTSTR)strClgg)<<","<<fA<<","<<fB;
									file<<szEn;
									break;
								case iSPR:
									file<<szZDJELE<<","<<iZdjh<<",";
									iTemp=strClgg.Find(_T(","));
									if(iTemp>=0)
										strClgg=strClgg.Left(iTemp);
									/*strTemp=vtos(rsZB->GetCollect("ID"));
									strTemp.MakeUpper();
									if(strTemp=="T5" || strTemp=="LHG")	
									{
										strTemp.Format("(C=%g)",vtof(rsZB->GetCollect("sizeC")));
										strClgg+=strTemp;
									}*/
									file<<T2A((LPTSTR)(LPCTSTR)strClgg)<<","<<vtof(rsZB->GetCollect(_T("HAZ")));
									file<<szEn;
									break;
								case iCSPR:
									file<<szZDJELE<<","<<iZdjh<<",";
									file<<T2A((LPTSTR)(LPCTSTR)strClgg)<<",";			//规格串
									file<<vtos(rsZB->GetCollect(_T("SpringNo")))<<",";//规格号
									file<<vtos(rsZB->GetCollect(_T("LugDia")))<<",";//拉杆直径
									file<<vtos(rsZB->GetCollect(_T("SumDist")))<<",";//总位移
									file<<vtos(rsZB->GetCollect(_T("HeatDisp")));//实际位移
									//file<<vtos(rsZB->GetCollect(_T("Pgz")));//荷载
									file<<szEn;
									break;
								case iAttached:
								case iNuts:
								case iBolts:
									astrNut[iNutCount].Format(_T("%s,%d,%s%s"),szZDJELE,iZdjh,strClgg,szEn);
									iNutCount++;
									break;
								default:
									file<<szZDJELE<<","<<iZdjh<<",";
									file<<T2A((LPTSTR)(LPCTSTR)strClgg);
									file<<szEn;
								}
							}
						}
						rsZB->Close();
						rsZ1->Close();
					}
				}
			}
			rsZA->Close();
		}
		rsGDMC->Close();
		file.close();
		return TRUE;
	}
	catch(_com_error & e)
	{
		AfxMessageBox(e.Description());
		file.close();
		return FALSE;
	}
}