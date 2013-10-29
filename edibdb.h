// EDIBDB.h
//

#if !defined(EDIBDB_h)
#define EDIBDB_h

#include "resource.h"
#include "datagrid.h"
#include "adodc.h"
#include "MObject.h"
#include "excel9.h"
class EDIBDB
{
public:
	static void CreateTableToAutoIPED(CString& strFileName,CString& strTblName,CString remark,double CLzz);
	static bool OutPutTable(CString OutputTableName, CString DestFileName, CString  sCnnType, _ConnectionPtr  db, CString sSrcTableName, CString  Table_Format);
	static void CloseExcel();
	static _variant_t GridCol(CDataGrid& grid,LPCSTR ColName);
	static void RefreshGrid(CDataGrid& grid,_RecordsetPtr rs);
	//static void RefreshGrid(CDataGrid &grid, CAdodc& data);
    static void StartEXCEL(CString  FileName);
    static bool bExcelStart();
    static bool DrawInfoIsNull(COleVariant);
    static void DrawInfoMakeFile(CString PrjName, CString DrawName, CString DrawID);
    static void DrawInfoGet();
    static void MakeTmp2ToBom();
    static void GetTBsize();
    static void BackupDb(CString mdb);
    static void MakeTbn(CString /*ByVal*/ tbn);
    static void AnalysisFieldsMAT(CString B, COleVariant fld());
    static void TxtMat2Mdb(CString FN);
    static void MDB2Txt(COleVariant MyData);
    static void TBNTMP2Txt(CString DestFN);
    static void SumMaterial(CString  VlmID="");
    static void SumNumbers();
    static void UpdTotalWeightAndMaterial(COleVariant tbn);
    static void Txt2Mdb(CString FN);
    static void AnalysisEngID(CString B);
    static void AnalysisFields(CString B, COleVariant fld());
    static void SetColumnsProperty(CDataGrid& DBGrid1, int  BILL);
    static CString GetOutputFields(int /*ByVal*/ BILL);
    static int	Qt;
    static int	FldSep;
    static int	IDSep;
    static int	DensityOfSteel;
    static int	pi;
    static CString	SJHY;
    static int	SJHYIndex;
    static int	TB0Len;
    static int	TB2Len;
    static int	TB0Hei;
    static int	TB2Hei;
    static long	A0H;
    static long	A0W;
    static CString	DrawIDMask;
    static CString	DrawIDinitNo;
    static CString	PrjName;
    static CString	DrawName;
    static CString	DrawID;
    static CString	DsgnName;
    static long DwgScale;
    static int	drawinfoFN;
    static CString	SelFileName;
    static CMObject	ObjExcelApp;
    static CMObject	ExcWorkBook;

protected:


private:

	static _ConnectionPtr	db;
//     static COleVariant	*tb;
//     static COleVariant	*Idx;
//     static COleVariant	*FD;
//     static COleVariant	*Qd;
//     static _RecordsetPtr	*rs;
//     static _RecordsetPtr	*rs1;

};

#endif /* EDIBDB_h */
