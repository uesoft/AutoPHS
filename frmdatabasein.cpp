// FrmDatabaseIn.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "FrmDatabaseIn.h"
#include "user.h"
#include "EDIBgbl.h"
#include "FrmTxsr.h"
#include "phsdata.h"
#include "basDirectory.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFrmDatabaseIn dialog


CFrmDatabaseIn::CFrmDatabaseIn(CWnd* pParent /*=NULL*/)
	: CDialog(CFrmDatabaseIn::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFrmDatabaseIn)
	m_sVlmID = _T("");
	m_sGc = _T("");
	m_sList1 = _T("");
	//}}AFX_DATA_INIT
	m_bIsCurDB=false;
	m_bIsNewDB=false;
}


void CFrmDatabaseIn::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFrmDatabaseIn)
	DDX_Control(pDX, IDC_PID, m_staticPID);
	DDX_Control(pDX, IDC_COMBO_ENGIN, m_comboEngin);
	DDX_Control(pDX, IDC_LIST1, m_List1);
	DDX_Text(pDX, IDC_VID, m_sVlmID);
	DDX_Text(pDX, IDC_PID, m_sGc);
	DDX_LBString(pDX, IDC_LIST1, m_sList1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFrmDatabaseIn, CDialog)
	//{{AFX_MSG_MAP(CFrmDatabaseIn)
	ON_BN_CLICKED(IDC_BUTTON1, OnDataIn)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	ON_CBN_SELCHANGE(IDC_COMBO_ENGIN, OnSelchangeComboEngin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFrmDatabaseIn message handlers

void CFrmDatabaseIn::getDatabase()
{
	try
	{
		_ConnectionPtr db;
		db.CreateInstance(__uuidof(Connection));
		CString mytablenameA,mytablenameB;
		m_bDataSelected=false;
		CString sqlA;
		_RecordsetPtr rsA;
		rsA.CreateInstance(__uuidof(Recordset));
   
//		FrmTxsr.m_pViewTxsr->m_ActiveRs->Close();
   
		mytablenameA = EDIBgbl::TBNSelPrjSpec + EDIBgbl::Btype[EDIBgbl::TZA];
		mytablenameB = EDIBgbl::TBNSelPrjSpec + EDIBgbl::Btype[EDIBgbl::TZB];
		CString sTitle;


		sTitle.Format(GetResStr(IDS_SelectPrjdbMdb),_T("AllPrjdb.mdb"));
		//sFilter=_T("*.*\0*.*\0\0");//GetResStr(IDS_DatabaseMdbAllFiles);
		CFileDialog fd(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,GetResStr(IDS_DatabaseMdbAllFiles));

		fd.m_ofn.lpstrTitle=sTitle;
		fd.m_ofn.lpstrInitialDir=basDirectory::ProjectDBDir;
		fd.m_ofn.nFilterIndex=1;
		CString sTmp;
		if(fd.DoModal()==IDOK)
		{
			m_List1.ResetContent();
			m_strFileName=fd.GetPathName();
			sTmp= EDIBgbl::GetDBName(EDIBgbl::dbPRJDB);
			sTmp.MakeUpper();
			sTmp.TrimLeft();
			sTmp.TrimRight();
			m_strFileName.MakeUpper();
			m_strFileName.TrimLeft();
			m_strFileName.TrimRight();
			db->Open((_bstr_t)m_strFileName, "", "", adConnectUnspecified);
			if(EDIBgbl::tdfExists(db,EDIBgbl::Btype[EDIBgbl::TZA]) && EDIBgbl::tdfExists(db,EDIBgbl::Btype[EDIBgbl::TZB]) )
			{		//新版本数据库
				m_bIsNewDB=true;
				if(m_strFileName == sTmp)
					m_bIsCurDB=true;
				m_db->Open((_bstr_t)m_strFileName, "", "", adConnectUnspecified);
				m_staticPID.ShowWindow(SW_HIDE);
				m_comboEngin.ShowWindow(SW_SHOW);
				LoadListEngin();
				if(m_comboEngin.GetCount()>0)
				{
					m_comboEngin.SetCurSel(0);
					OnSelchangeComboEngin();
				}
				m_bDataSelected=true;
			}
			else if(EDIBgbl::tdfExists(db, mytablenameA) && EDIBgbl::tdfExists(db, mytablenameB) )
			{		//老版本数据库
				if(m_strFileName == sTmp)
				{
					AfxMessageBox(IDS_SEL_DDATABASE_ALIKE_ORIGINAL);
					return;
				}
				m_staticPID.ShowWindow(SW_SHOW);
				m_comboEngin.ShowWindow(SW_HIDE);
				m_bIsNewDB=false;
				sqlA=_T("SELECT DISTINCT VolumeID FROM [") + mytablenameA + _T("] WHERE NOT IsNull(VolumeID) AND (VolumeID)<>\'\' AND (VolumeID)<>") + EDIBgbl::SelJcdm;
				//sqlB = _T("SELECT DISTINCT VolumeID FROM[") & mytablenameB & _T("] WHERE NOT IsNull(VolumeID) AND (VolumeID)<>'' AND (VolumeID)<>'") & SelVlmID & _T("'")
				rsA->Open(_variant_t(sqlA),(IDispatch*)db,adOpenKeyset, adLockOptimistic,adCmdText);
				_variant_t vTmp;
				while(!rsA->adoEOF)
				{
					rsA->get_Collect((_variant_t)_T("VolumeID"), &vTmp);
					m_List1.AddString(vtos(vTmp));
					rsA->MoveNext();
				}
				m_bDataSelected=true;
			}
			else
			{
				AfxMessageBox(IDS_SEL_DATABASE_ERROR);
				m_bDataSelected = false;
			}

		}
		else
		{
			m_bDataSelected=false;
			this->SendMessage(WM_CLOSE);
		}
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
		m_bDataSelected=false;
		this->SendMessage(WM_CLOSE);
	}	
}

BOOL CFrmDatabaseIn::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_bDataSelected=true;
	m_sGc=EDIBgbl::SelPrjID;
	m_sVlmID=EDIBgbl::SelJcdm;
	this->getDatabase();
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFrmDatabaseIn::OnDataIn() 
{
	// TODO: Add your control notification handler code here
	AfxGetApp()->BeginWaitCursor();
	try
	{
		UpdateData();
		long i=0;
		bool bInsert=false;  //需要导入记录
		_ConnectionPtr db;
		db.CreateInstance(__uuidof(Recordset));
		_RecordsetPtr rs1;
		rs1.CreateInstance(__uuidof(Recordset));
		_RecordsetPtr rs2;
		rs2.CreateInstance(__uuidof(Recordset));
		_RecordsetPtr rsVlm;
		rsVlm.CreateInstance(__uuidof(Recordset));
		long VlmID=0;
		long VlmID2=0;
		_variant_t v1,v2;
		CString strSQL;
		long count;
		_variant_t varTmp;
		CString SQL1;
		CString tblName;
		CString strJcdm1;
		CString strJcmc1;
		int ix=m_List1.GetCurSel();
		m_List1.GetText(ix,strJcdm1);
		if(m_sList1 == EDIBgbl::SelVlmID)
		{
			//MsgBox _T("您选择的卷册与当前卷册相同，请选择其他卷册！")
			//MsgBox ResolveResString(iUE_SelectedVolumeIDSameAsCurrentVolumeID), vbMsgBoxSetForeground
		}
		else
		{
			if(m_bDataSelected)
			{
				if(EDIBgbl::SelPrjID !=m_strEnginID )
				{
					strSQL.Format(_T("DELETE FROM [%s] WHERE VolumeID IN (SELECT VOlumeID FROM Volume WHERE jcdm =\'%s\' AND Enginid=\'%s\')"),
						EDIBgbl::Btype[EDIBgbl::TZA],strJcdm1,EDIBgbl::SelPrjID);
					try
					{
						EDIBgbl::dbPRJDB->Execute((_bstr_t)strSQL, NULL, adCmdText);
					}
					catch (_com_error &e)
					{
						CString strMsg;
						strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
						AfxMessageBox(strMsg);
					}
					int VlmID2;
					ix=m_List1.GetCurSel();
					VlmID2=m_List1.GetItemData(ix);
					strSQL.Format(_T("SELECT jcmc FROM Volume WHERE VolumeID=%d"),VlmID2);
					rs1->Open(_variant_t(strSQL),(IDispatch*)m_db,adOpenKeyset, adLockOptimistic,adCmdText);
					if(!rs1->BOF && !rs1->adoEOF)
					{
						rs1->get_Collect((_variant_t)0L, &varTmp);
						strJcmc1=vtos(varTmp);
					}
					rs1->Close();
					strSQL.Format(_T("SELECT * FROM Volume WHERE Enginid=\'%s\' AND jcdm=\'%s\' AND SJHYID=%d AND SJJDID=%d AND ZYID=%d"),
						EDIBgbl::SelPrjID,strJcdm1,EDIBgbl::SelHyID,EDIBgbl::SelDsgnID,EDIBgbl::SelSpecID);
					rs1->Open(_variant_t(strSQL),(IDispatch*)EDIBgbl::dbPRJDB,adOpenKeyset, adLockOptimistic,adCmdText);
					if(rs1->adoEOF && rs1->BOF)
					{
						VlmID=GetMaxVlmID(EDIBgbl::dbPRJDB)+1;
						rs1->AddNew();
						rs1->put_Collect((_variant_t)_T("VolumeID"),_variant_t((long)VlmID));
						rs1->put_Collect((_variant_t)_T("EnginID"),STR_VAR(EDIBgbl::SelPrjID));
						rs1->put_Collect((_variant_t)_T("jcdm"),STR_VAR(strJcdm1));
						rs1->put_Collect((_variant_t)_T("jcmc"),STR_VAR(strJcmc1));
						rs1->put_Collect((_variant_t)_T("SJJDID"),_variant_t(EDIBgbl::SelDsgnID));
						rs1->put_Collect((_variant_t)_T("SJHYID"),_variant_t(EDIBgbl::SelHyID));
						rs1->put_Collect((_variant_t)_T("ZYID"),_variant_t(EDIBgbl::SelSpecID));
						rs1->Update();
					}
					else
					{
						rs1->get_Collect((_variant_t)_T("VolumeID"), &varTmp);
						VlmID=vtoi(varTmp);
					}
					rs1->Close();
					tblName=CString(_T("[")) + EDIBgbl::Btype[EDIBgbl::TZA] + _T("]");
					strSQL.Format(_T("INSERT INTO [%s] SELECT %d AS VolumeID,%s FROM [%s] IN \'%s\' WHERE VOlumeID=%d"),
							EDIBgbl::Btype[EDIBgbl::TZA],VlmID,SQL1,EDIBgbl::Btype[EDIBgbl::TZA],EDIBgbl::GetDBName(m_db),VlmID2);
					try
					{
						EDIBgbl::dbPRJDB->Execute((_bstr_t)strSQL, NULL, adCmdText);
					}
					catch(...)
					{
					}
				}
				else
				{
					bInsert = false;
					db->Open((_bstr_t)m_strFileName, "", "", adConnectUnspecified);
					CString sTmp;
					//先检查当前库中的数据记录是否与导入源
					CString SQLx = _T("SELECT count(*) as C1 FROM [") + EDIBgbl::Btype[EDIBgbl::TZA] + _T("] WHERE VolumeID IN ") 
						+ _T(" ( SELECT VolumeID FROM Volume WHERE EnginID=\'") + EDIBgbl::SelPrjID+ _T("\' AND jcdm=\'") + m_sList1 
						+ _T("\' AND SJJDID = ") + ltos(EDIBgbl::SelDsgnID) + _T(" AND SJHYID = ") + ltos(EDIBgbl::SelHyID) 
						+ _T(" AND ZYID=") + ltos(EDIBgbl::SelSpecID) + _T(" )");
					rs2->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
						adOpenKeyset, adLockOptimistic, adCmdText); 
					if(!m_bIsNewDB)
					{
						SQLx=_T("SELECT count(*) as C1 FROM [") + EDIBgbl::TBNSelPrjSpec + EDIBgbl::Btype[EDIBgbl::TZA] + _T("] WHERE (VolumeID) =\'") + m_sList1 + _T("\'");
					}
					rs1->Open((_bstr_t)SQLx, _variant_t((IDispatch*)db,true), 
						adOpenKeyset, adLockOptimistic, adCmdText); 
					if(!rs2->adoEOF && !rs2->BOF && !rs1->adoEOF && !rs1->BOF)
					{
						rs2->get_Collect((_variant_t)_T("C1"), &v2);
						rs1->get_Collect((_variant_t)_T("C1"), &v1);
						count=vtoi(v1);
						if(vtoi(v2) >= vtoi(v1))
						{
							//当前库中记录比源库中同一卷册记录多，提示是否导入
							sTmp.Format(GetResStr(IDS_CurrentMdbRCsMoreThanSourceMdb),EDIBgbl::GetDBName(EDIBgbl::dbPRJDB),m_sList1,vtos(v2),EDIBgbl::GetDBName(db),vtos(v1));
							if(MessageBox(sTmp,NULL,MB_DEFBUTTON2|MB_ICONQUESTION|MB_YESNO)==IDYES)
								bInsert=true;
						}
							//当前库中记录比源库中同一卷册记录少，要导入
						else 
						{
							bInsert = true;
						}
					}
					rs1->Close();
					rs2->Close();
					if(bInsert)
					{
						SQLx =_T("SELECT * FROM Volume WHERE EnginID=\'") + EDIBgbl::SelPrjID+ _T("\' AND jcdm=\'") + m_sList1 
								+ _T("\' AND SJJDID = ") + ltos(EDIBgbl::SelDsgnID) + _T(" AND SJHYID = ") + ltos(EDIBgbl::SelHyID) 
								+ _T(" AND ZYID=") + ltos(EDIBgbl::SelSpecID);
						rsVlm->Open((_bstr_t)SQLx, _variant_t((IDispatch*)EDIBgbl::dbPRJDB,true), 
							adOpenKeyset, adLockOptimistic, adCmdText); 
						if(rsVlm->BOF && rsVlm->adoEOF)
						{
							rsVlm->AddNew();
							VlmID=GetMaxVlmID(EDIBgbl::dbPRJDB)+1;
							rsVlm->put_Collect((_variant_t)_T("VolumeID"),_variant_t(VlmID));
							rsVlm->put_Collect((_variant_t)_T("EnginID"),STR_VAR(EDIBgbl::SelPrjID));
							rsVlm->put_Collect((_variant_t)_T("jcdm"),STR_VAR(m_sList1));
							rsVlm->put_Collect((_variant_t)_T("SJJDID"),_variant_t(EDIBgbl::SelDsgnID));
							rsVlm->put_Collect((_variant_t)_T("SJHYID"),_variant_t(EDIBgbl::SelHyID));
							rsVlm->put_Collect((_variant_t)_T("ZYID"),_variant_t(EDIBgbl::SelSpecID));
							rsVlm->Update();
						}
						else
						{
							rsVlm->get_Collect((_variant_t)_T("VolumeID"), &v1);
							VlmID=vtoi(v1);
						}
						rsVlm->Close();
						if(m_bIsNewDB)
						{
							VlmID2=m_List1.GetItemData(m_List1.GetCurSel());
							//先删除
							SQLx = _T("DELETE * FROM [") + EDIBgbl::Btype[EDIBgbl::TZA] + _T("] WHERE VolumeID =") + ltos(VlmID);
							EDIBgbl::dbPRJDB->Execute((_bstr_t)SQLx, NULL, adCmdText);
							//再插入
							tblName=CString(_T("[")) + EDIBgbl::Btype[EDIBgbl::TZA] + _T("]");
							SQLx= _T("INSERT INTO [") + EDIBgbl::Btype[EDIBgbl::TZA] + _T("] SELECT ") + ltos(VlmID) + _T(" AS VolumeID,") + 
								SQL1 + _T(" FROM ") + tblName + _T(" IN \'") + EDIBgbl::GetDBName(db) + _T("\' WHERE VolumeID =") + ltos(VlmID2);
							try
							{
								EDIBgbl::dbPRJDB->Execute((_bstr_t)SQLx, NULL, adCmdText);
							}
							catch (_com_error &e)
							{
								CString strMsg;
								strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
								AfxMessageBox(strMsg);
							}

							//先删除
							SQLx = _T("DELETE * FROM [") + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] WHERE VolumeID =") + ltos(VlmID);
							EDIBgbl::dbPRJDB->Execute((_bstr_t)SQLx, NULL, adCmdText);
							//再插入
							tblName=CString(_T("[")) + EDIBgbl::Btype[EDIBgbl::TZB] + _T("]");
							SQLx= _T("INSERT INTO [") + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] SELECT ") + ltos(VlmID) + _T(" AS VolumeID,") + 
								SQL1 + _T(" FROM ") + tblName + _T(" IN \'") + EDIBgbl::GetDBName(db) + _T("\' WHERE VolumeID =") + ltos(VlmID2);
							try
							{
								EDIBgbl::dbPRJDB->Execute((_bstr_t)SQLx, NULL, adCmdText);
							}
							catch (_com_error &e)
							{
								CString strMsg;
								strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
								AfxMessageBox(strMsg);
							}

							//先删除
							SQLx = _T("DELETE * FROM [") + EDIBgbl::Btype[EDIBgbl::TZD] + _T("] WHERE VolumeID =") + ltos(VlmID);
							EDIBgbl::dbPRJDB->Execute((_bstr_t)SQLx, NULL, adCmdText);
							//再插入
							tblName=CString(_T("[")) + EDIBgbl::Btype[EDIBgbl::TZD] + _T("]");
							SQLx= _T("INSERT INTO [") + EDIBgbl::Btype[EDIBgbl::TZD] + _T("] SELECT ") + ltos(VlmID) + _T(" AS VolumeID,") + 
								SQL1 + _T(" FROM ") + tblName + _T(" IN \'") + EDIBgbl::GetDBName(db) + _T("\' WHERE VolumeID =") + ltos(VlmID2);
							try
							{
								EDIBgbl::dbPRJDB->Execute((_bstr_t)SQLx, NULL, adCmdText);
							}
							catch (_com_error &e)
							{
								CString strMsg;
								strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
								AfxMessageBox(strMsg);
							}

							//先删除
							SQLx = _T("DELETE * FROM [") + EDIBgbl::Btype[EDIBgbl::TML] + _T("] WHERE VolumeID =") + ltos(VlmID);
							EDIBgbl::dbPRJDB->Execute((_bstr_t)SQLx, NULL, adCmdText);
							//再插入
							tblName=CString(_T("[")) + EDIBgbl::Btype[EDIBgbl::TML] + _T("]");
							SQLx= _T("INSERT INTO [") + EDIBgbl::Btype[EDIBgbl::TML] + _T("] SELECT ") + ltos(VlmID) + _T(" AS VolumeID,") + 
								SQL1 + _T(" FROM ") + tblName + _T(" IN \'") + EDIBgbl::GetDBName(db) + _T("\' WHERE VolumeID =") + ltos(VlmID2);
							try
							{
								EDIBgbl::dbPRJDB->Execute((_bstr_t)SQLx, NULL, adCmdText);
							}
							catch (_com_error &e)
							{
								CString strMsg;
								strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
								AfxMessageBox(strMsg);
							}

						}
						else
						{
							//先删除
							SQLx = _T("DELETE * FROM [") + EDIBgbl::Btype[EDIBgbl::TZA] + _T("] WHERE VolumeID =") + ltos(VlmID);
							EDIBgbl::dbPRJDB->Execute((_bstr_t)SQLx, NULL, adCmdText);
							//再插入
							tblName=CString(_T("[")) + EDIBgbl::TBNSelPrjSpec + EDIBgbl::Btype[EDIBgbl::TZA] + _T("]");
							SQLx= _T("INSERT INTO [") + EDIBgbl::Btype[EDIBgbl::TZA] + _T("] SELECT ") + ltos(VlmID) + _T(" AS VolumeID,") + 
								SQL1 + _T(" FROM [") +EDIBgbl::TBNSelPrjSpec+ EDIBgbl::Btype[EDIBgbl::TZA] + _T("] IN \'")+ 
								EDIBgbl::GetDBName(db) + _T("\' WHERE (VolumeID) =\'") + m_sList1 + _T("\'");
							try
							{
								EDIBgbl::dbPRJDB->Execute((_bstr_t)SQLx, NULL, adCmdText);
							}
							catch (_com_error &e)
							{
								CString strMsg;
								strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
								AfxMessageBox(strMsg);
							}

							//先删除
							SQLx = _T("DELETE * FROM [") + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] WHERE VolumeID =") + ltos(VlmID);
							EDIBgbl::dbPRJDB->Execute((_bstr_t)SQLx, NULL, adCmdText);
							//再插入
							tblName=CString(_T("[")) + EDIBgbl::TBNSelPrjSpec + EDIBgbl::Btype[EDIBgbl::TZB] + _T("]");
							SQLx= _T("INSERT INTO [") + EDIBgbl::Btype[EDIBgbl::TZB] + _T("] SELECT ") + ltos(VlmID) + _T(" AS VolumeID,") + 
								SQL1 + _T(" FROM [") +EDIBgbl::TBNSelPrjSpec+ EDIBgbl::Btype[EDIBgbl::TZB] + _T("] IN \'")+ 
								EDIBgbl::GetDBName(db) + _T("\' WHERE (VolumeID) =\'") + m_sList1 + _T("\'");
							//MessageBox(SQLx);
							try
							{
								EDIBgbl::dbPRJDB->Execute((_bstr_t)SQLx, NULL, adCmdText);
							}
							catch (_com_error &e)
							{
								CString strMsg;
								strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
								AfxMessageBox(strMsg);
							}

							//先删除
							SQLx = _T("DELETE * FROM [") + EDIBgbl::Btype[EDIBgbl::TZD] + _T("] WHERE VolumeID =") + ltos(VlmID);
							EDIBgbl::dbPRJDB->Execute((_bstr_t)SQLx, NULL, adCmdText);
							//再插入
							tblName=CString(_T("[")) + EDIBgbl::TBNSelPrjSpec + EDIBgbl::Btype[EDIBgbl::TZD] + _T("]");
							SQLx= _T("INSERT INTO [") + EDIBgbl::Btype[EDIBgbl::TZD] + _T("] SELECT ") + ltos(VlmID) + _T(" AS VolumeID,") + 
								SQL1 + _T(" FROM [") +EDIBgbl::TBNSelPrjSpec+ EDIBgbl::Btype[EDIBgbl::TZD] + _T("] IN \'")+ 
								EDIBgbl::GetDBName(db) + _T("\' WHERE (VolumeID) =\'") + m_sList1 + _T("\'");
							try
							{
								EDIBgbl::dbPRJDB->Execute((_bstr_t)SQLx, NULL, adCmdText);
							}
							catch (_com_error &e)
							{
								CString strMsg;
								strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
								AfxMessageBox(strMsg);
							}

							//先删除
							SQLx = _T("DELETE * FROM [") + EDIBgbl::Btype[EDIBgbl::TML] + _T("] WHERE VolumeID =") + ltos(VlmID);
							EDIBgbl::dbPRJDB->Execute((_bstr_t)SQLx, NULL, adCmdText);
							//再插入
							tblName=CString(_T("[")) + EDIBgbl::TBNSelPrjSpec + EDIBgbl::Btype[EDIBgbl::TML] + _T("]");
							SQLx= _T("INSERT INTO [") + EDIBgbl::Btype[EDIBgbl::TML] + _T("] SELECT ") + ltos(VlmID) + _T(" AS VolumeID,") + 
								SQL1 + _T(" FROM [") +EDIBgbl::TBNSelPrjSpec+ EDIBgbl::Btype[EDIBgbl::TML] + _T("] IN \'")+ 
								EDIBgbl::GetDBName(db) + _T("\' WHERE (VolumeID) =\'") + m_sList1 + _T("\'");
							try
							{
								EDIBgbl::dbPRJDB->Execute((_bstr_t)SQLx, NULL, adCmdText);
							}
							catch (_com_error &e)
							{
								CString strMsg;
								strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
								AfxMessageBox(strMsg);
							}
						}
					}
					sTmp.Format(GetResStr(IDS_VlmIDDataImported),m_sList1,ltos(count));
					MessageBox(sTmp); //_T("数据导入成功！")
				}
			}
		}
		try
		{
			FrmTxsr.m_pViewTxsr->m_ActiveRs->Update();
		}
		catch (_com_error &e)
		{
			CString strMsg;
			strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
			AfxMessageBox(strMsg);
			FrmTxsr.m_pViewTxsr->m_ActiveRs->CancelUpdate();
		}
		FrmTxsr.m_pViewTxsr->m_ActiveRs->Requery(-1);
		FrmPhsData.InitDBbill();
		this->SendMessage(WM_CLOSE);
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}
	AfxGetApp()->EndWaitCursor();
}

void CFrmDatabaseIn::OnButton3() 
{
	// TODO: Add your control notification handler code here
	this->getDatabase();
}

long inline CFrmDatabaseIn::GetMaxVlmID(_ConnectionPtr & db)
{
	long ret=0;
	_variant_t vTmp;
	_RecordsetPtr rs;
	rs.CreateInstance(__uuidof(Recordset));

	try
	{
		CString SQLx;
		SQLx = _T("Select MAX(VolumeID) From Volume");
		rs->Open((_bstr_t)SQLx, _variant_t((IDispatch*)db,true), 
			adOpenKeyset, adLockOptimistic, adCmdText); 
		rs->get_Collect((_variant_t)0L, &vTmp);
		ret=vtoi(vTmp);
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}
	return ret;
}

void CFrmDatabaseIn::LoadListEngin()
{
	_RecordsetPtr rs;
	rs.CreateInstance(__uuidof(Recordset));
	try
	{
		CString strSQL;
		m_comboEngin.ResetContent();
		if(m_bIsCurDB)
			strSQL.Format(_T("SELECT EnginID FROM Engin WHERE EnginID<>\'%s\'"),EDIBgbl::SelPrjID);
		else
			strSQL=_T("SELECT EnginID FROM Engin");
		rs->Open((_bstr_t)strSQL, _variant_t((IDispatch*)m_db,true), 
			adOpenKeyset, adLockOptimistic, adCmdText); 
		_variant_t vartmp;
		while(!rs->adoEOF)
		{
			rs->get_Collect((_variant_t)0L, &vartmp);
			m_comboEngin.AddString(vtos(vartmp));
			rs->MoveNext();
		}
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}
}

void CFrmDatabaseIn::OnSelchangeComboEngin() 
{
	// TODO: Add your control notification handler code here
	int ix=m_comboEngin.GetCurSel();
	if(ix==-1)
	{
		m_strEnginID=_T("");
	}
	else
		m_comboEngin.GetLBText(ix,m_strEnginID);
	LoadListVlm();
}

void CFrmDatabaseIn::LoadListVlm()
{
	_RecordsetPtr rs;
	rs.CreateInstance(__uuidof(Recordset));
	CString strSQL;
	m_List1.ResetContent();
	if(m_strEnginID==_T(""))return;
	try
	{
		_variant_t vtmp;
		strSQL.Format(_T("SELECT jcdm,VolumeID FROM Volume WHERE EnginID=\'%s\' AND SJHYID=%d AND ZYID=%d AND (jcdm<>\'%s\' OR EnginID<>\'%s\')"),
							m_strEnginID,
							EDIBgbl::SelHyID,
							EDIBgbl::SelSpecID,
							EDIBgbl::SelJcdm,
							EDIBgbl::SelPrjID);
		rs->Open((_bstr_t)strSQL, _variant_t((IDispatch*)m_db,true), 
			adOpenKeyset, adLockOptimistic, adCmdText); 
		int ix;
		while(!rs->adoEOF)
		{
			rs->get_Collect((_variant_t)0L, &vtmp);
			ix=m_List1.AddString(vtos(vtmp));
			rs->get_Collect((_variant_t)1L, &vtmp);
			m_List1.SetItemData(ix,vtoi(vtmp));
			rs->MoveNext();
		}
	}
	catch (_com_error &e)
	{
		CString strMsg;
		strMsg.Format("%s:%d %s", __FILE__, __LINE__, (LPSTR)e.Description());
		AfxMessageBox(strMsg);
	}
	
}

