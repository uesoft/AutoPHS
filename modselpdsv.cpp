// modSelPDSV.cpp
//

#include "stdafx.h"
#include "modSelPDSV.h"
#include "user.h"
#include "ReadPSAResult.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
const _TCHAR modSelPDSV::defPrjID[] =_T("F4511");
const _TCHAR * modSelPDSV::defPrjName;
const long modSelPDSV::defDsgnID =4;
const _TCHAR modSelPDSV::defDsgn[] =_T("S");
const _TCHAR * modSelPDSV::defDsgnName;
_TCHAR ssPrjName[64];
_TCHAR ssDsgnName[64];
_TCHAR ssSpecName[64];
_TCHAR ssVlmName[64];
const long modSelPDSV::defSpecID =3;
const _TCHAR modSelPDSV::defSpec[] =_T("J");
const _TCHAR * modSelPDSV::defSpecName;
const _TCHAR modSelPDSV::defJcdm[] =_T("0501");
const long modSelPDSV::defVlmID = 1;
const _TCHAR * modSelPDSV::defVlmName ;
const long modSelPDSV::defHyID =0;
const int   modSelPDSV::defSJHYIndex =0;

_TCHAR ssSprNWEPDI[128];
_TCHAR ssSprECEPDI[128];
_TCHAR ssSprGB10182[128];

void InitDefVar()
{
	LoadString(::AfxGetResourceHandle(),IDS_HNYYElectricityFactory,ssPrjName,sizeof(ssPrjName));
	modSelPDSV::defPrjName=ssPrjName;
	LoadString(::AfxGetResourceHandle(),IDS_SHOP_DRAWING,ssDsgnName,sizeof(ssDsgnName));
	modSelPDSV::defDsgnName=ssDsgnName;
	LoadString(::AfxGetResourceHandle(),IDS_HOTENGINE,ssSpecName,sizeof(ssSpecName));
	modSelPDSV::defSpecName=ssSpecName;
	LoadString(::AfxGetResourceHandle(),IDS_MainSteamPipingInstallChart,ssVlmName,sizeof(ssVlmName));
	modSelPDSV::defVlmName=ssVlmName;
	LoadString(::AfxGetResourceHandle(),IDS_NorthwestIns,ssSprNWEPDI,sizeof(ssSprNWEPDI));
	LoadString(::AfxGetResourceHandle(),IDS_ChinaeastIns,ssSprECEPDI,sizeof(ssSprECEPDI));
	LoadString(::AfxGetResourceHandle(),IDS_DaLianSpringFac,ssSprGB10182,sizeof(ssSprGB10182));
	
	strSprNWEPDI = ssSprNWEPDI;
	strSprECEPDI = ssSprECEPDI;
	strSprGB10182 =ssSprGB10182;
}

