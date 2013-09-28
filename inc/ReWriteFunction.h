
//#if _MSC_VER > 1000
//#pragma once
//#endif

BOOL GetPHSCompRs(CString TableName ,long zdjh,long SelVlmID, int nth,CDaoRecordset &rsTZB,CDaoRecordset &rsTmpZB);
void ReplaceCNTB(CString &tmpSQL0,CDaoRecordset &rsDiaOfCSPRFiK,CDaoRecordset &rsX );
void SetPrePartInsertLen( int i,CString* pstrPartID,CString mviPreviousClassID,CDaoRecordset &rsConnect,CDaoRecordset &rsX,CDaoRecordset &rsTZB,CDaoRecordset &rsDiaOfCSPRFiK,CString &tmpExtLenFD );
double GetPartSizeH(int i,CDaoRecordset &rsX,const CString& strPartID,const CString*PtypeID);
double GetSngDim(float sngH1xmax,float Height_SPRINGsL5,int iSEQofSPR,CString PType,CString sPartID,CDaoRecordset &rsX,CDaoRecordset &rsTmp,CDaoRecordset &rsDiaOfCSPRFiK); //得到恒力弹簧的安装高度