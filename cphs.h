// Cphs.h
//

#if !defined(AFX_CPHS_H__66EB6429_712C_11D5_8752_00D009619765__INCLUDED_)
#define AFX_CPHS_H__66EB6429_712C_11D5_8752_00D009619765__INCLUDED_


#include "resource.h"
#include "EDIBgbl.h"
#include "modPHScal.h"
extern int vRCount;
#include "DlgListBox.h"
#include "FrmStatus.h"
class Cphs
{
public:
	~Cphs();
	Cphs();

    float SATotalWeight();
    bool bFirstPartIsPA();
    long SetPhsPATypeToListBox();
    long SetPhsTypeToListBox();
    long SetPhsPreviousTypeToListBox();
    long SaveRsUnCheckedTypeFromResultObj();
    long SetPhsCheckedTypeToListBox();
    CString GetPhsAssembleName(long/*Optional*/ SampleID);
    long GetPhsOneClassPartNum(long iPtype, long /*ByVal*/ SampleID);
    long GetPhsOneClassPartNumAndPartInfo(long iPtype, long /*ByVal*/ SampleID, CString * & Ptype, long * & mlPartClassID, long * & mlPartIndex,long & Count);
    long GetPhsIsCSPR(long iPtype, long /*ByVal*/ SampleID, int& /*Optional*/ RCount=vRCount);
    long GetphsStructIDsemiAuto();
    long GetPhsStructSampleID();
    long SavephsStructTorsTmpREF();
	static int MODEoverwrite;
	static int AttachI;
    long SavephsAllStructToTZB(long& zdjh,int nth,  int& AttachIncluded=AttachI);
    void GetPhsStructFromTZB(int zdjh);
    void SetPhsPASAtoTZA(int zdjh, long SampleID);
    void MakeRsTZB(long iDbID, CString  rsPrefixName,CString strDeleteUserMaterial="");
    bool GetphsBHandSizesTest();
    bool GetphsBHandSizes(CFrmStatus& frmStatus, int FirstCal, int  MaxCalCount,int nth,float& ,float& );
    void GetBoltsNutsAndAttachmentsCLgg(int nth = 1);
    void GetphsBlkRotation();
    long CheckMatchPhs();
	CString GetBH(const CString& sBHFormat);
    void CheckDuplicateIndex();
    long CheckDuplicateREFRecord();
    //long CheckDuplicateREFRecordWhenAppend();   //原
	long CheckDuplicateREFRecordWhenAppend(int *ipCheckButton = NULL);//LFX  2005.3.24  加
    void CheckAllCustomIDandIDinPhsStructureREF();
    void ChangeNameInphsStructureName();
    _RecordsetPtr AvailableSampleIDrsForphsSamp(CString strOrderByDesc,CString strFilter);
    void GetMaterial();
    void GetphsSEQ(_RecordsetPtr /*ByVal*/ rsza);
    void GetphsSumBom();
    void GetPhsBlkIDandCrd(_RecordsetPtr /*ByVal*/ rsza);
    void GetPhsSAELandPAdxdydz();
	void simplify2(CComPtr<_Recordset>& rsPartBoltNuts,int nth);
	void simplify(CComPtr<_Recordset>& rsSAPart,int nth);
    _variant_t ClassDebugID();
    CString GetBHforDoubleCSBeam(float  GDW1, float  OffsetOfP1toP2, float  Span, float  p1, float  p2, CString  SACustomID, int  iDCS=2, int  FirstCal=2);
    int GetPhsStructFromSampleID(long /*ByVal*/ SampleID);
public:
	bool brsIDStatus;
    /*Object*/CListBox* ResultObj;
    /*Object*/CListBox* SourceObj;
	//_RecordsetPtr rsID;
	CComPtr<_Recordset> rsID;
	CString	PA;
    CString	SA;
protected:

public:
	void CloseRecordsets();
	_Recordset* m_rsObj;
	void SelectItemRsObj();
	void InitListRs();

    long CheckInitRecord();
    void Class_Initialize();
    void Class_Terminate();
    /*
     * 局部变量，保存在 Class_Initialize 中创建的连续的类 ID
     */
    long	mlClassDebugID;
    /*
     * 保持属性值的局部变量
     */
    long	iAttachIncd;
    int	iSEQofPart;
    int	iSEQofSPR;
    int	iNumPart;
    int	mvariNumE2B;
    CString	Cntb;
    CString	CntbP;
    long	mvarNumSpring;
    
    CString	SortFieldName;
    long	iLIST;
	
    /*Object*/int	mvarData1;
    /*Object*/int	mvarTmpphsCalObj;
    /*Object*/int	mvarfrmDisplayData;
    bool	mvarbGetphsBHandSizesBK;
    long	iChkCondition;
    long	iSAPAConnect;
    float	mvarSATotalWeight;
    float	SACSHeight;
    /*
     * 保持属性值的局部变量
     */
    CListBox * TZBResultObj;
    /*
     * 保持属性值的局部变量
     * 保持属性值的局部变量
     */
    bool	PhsSampOrder;
    /*
     * 保持属性值的局部变量
     * 保持属性值的局部变量
     */
    bool	mvarbrsIDStatus;
    _RecordsetPtr	phsAvailableTypeRs;
    _RecordsetPtr	rsTZB;
    _RecordsetPtr	rsTZC;
    _RecordsetPtr	rsTmpREF;
    _RecordsetPtr	mvarrsDiameterSerial;
    /*
     * 保持属性值的局部变量
     */

	_RecordsetPtr rsUnCheckedType;
	_RecordsetPtr rsConnect;
	_RecordsetPtr rsphsStructureREF;
	//_RecordsetPtr rsPictureClipData;

};
extern Cphs	* Cavphs;
#endif /* Cphs_h */
