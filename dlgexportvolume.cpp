// DlgExportVolume.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "DlgExportVolume.h"
#include "EDIBgbl.h"
#include "modregistry.h"
#include "user.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgExportVolume dialog


CDlgExportVolume::CDlgExportVolume(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgExportVolume::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgExportVolume)
	m_strDes = _T("");
	m_strCurBackupDes = _T("");
	m_strCurBackupTablename = _T("");
	//}}AFX_DATA_INIT
}


void CDlgExportVolume::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgExportVolume)
	DDX_Control(pDX, IDC_COMBO_HAD_BACKUP, m_comboHadBackup);
	DDX_Text(pDX, IDC_EDIT_DESCRIPTION, m_strDes);
	DDX_Text(pDX, IDC_EDIT_BACKUP_DESCRIPTION, m_strCurBackupDes);
	DDX_Text(pDX, IDC_EDIT_BACKUP_TABLENAME, m_strCurBackupTablename);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgExportVolume, CDialog)
	//{{AFX_MSG_MAP(CDlgExportVolume)
	ON_CBN_SELCHANGE(IDC_COMBO_HAD_BACKUP, OnSelchangeComboHadBackup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgExportVolume message handlers

void CDlgExportVolume::OnSelchangeComboHadBackup() 
{
	CString str;
	m_comboHadBackup.GetLBText(m_comboHadBackup.GetCurSel(),str);
	m_strDes = m_map[str];
    UpdateData(FALSE);	
}

BOOL CDlgExportVolume::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	_RecordsetPtr rsbackup;
	rsbackup.CreateInstance(__uuidof(Recordset));
	CString strSQL;
	strSQL = "SELECT * FROM BackupDBTbl";
	rsbackup->Open((_bstr_t)strSQL, _variant_t((IDispatch*)EDIBgbl::dbPRJ,true), 
		adOpenKeyset, adLockOptimistic, adCmdText); 
	_variant_t varField;
	CString str1,str2,strFirst;	
	while( !rsbackup->adoEOF )
	{
		rsbackup->get_Collect((_variant_t)1L, &varField);//BackupDB_Tablename
		str1 = vtos(varField);
		rsbackup->get_Collect((_variant_t)2L, &varField);//Description
		str2 = vtos(varField);
		m_comboHadBackup.AddString(str1);
		m_map[str1]=str2;
		rsbackup->MoveNext();
	}
	rsbackup->Close();



	if( m_comboHadBackup.GetCount()>0 )
	{
		m_comboHadBackup.SetCurSel(0);
		m_comboHadBackup.GetLBText(0,strFirst);
		m_strDes = m_map[strFirst];
	}
	else
	{
		m_comboHadBackup.AddString(_T("暂无备份"));
		m_comboHadBackup.SetCurSel(0);
	}
		

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgExportVolume::OnOK() 
{
	UpdateData();
	m_strCurBackupTablename.TrimLeft();
	m_strCurBackupTablename.TrimRight();
	if( m_strCurBackupTablename.IsEmpty() )
	{
		AfxMessageBox(_T("表名不能为空"));
		return;
	}

	CString strSQL;
	if( EDIBgbl::tdfExists(EDIBgbl::dbPRJ, _T(m_strCurBackupTablename)) )
	{
		strSQL.Format(_T("DROP TABLE %s"),m_strCurBackupTablename);
		EDIBgbl::dbPRJ->Execute((_bstr_t)strSQL, NULL, adCmdText);
	}
	strSQL.Format(_T("SELECT * INTO %s IN \'C:\\prj4.0\\workprj.mdb\' FROM ZA WHERE VolumeID=%d"),
		m_strCurBackupTablename,EDIBgbl::SelVlmID);
	EDIBgbl::dbPRJDB->Execute((_bstr_t) strSQL , NULL, adCmdText);
	strSQL.Format(_T("INSERT INTO BackupDBTbl(BackupDB_Tablename,Description) VALUES(\'%s\',\'%s\')"),m_strCurBackupTablename,m_strCurBackupDes);
	EDIBgbl::dbPRJ->Execute((_bstr_t) strSQL , NULL, adCmdText);

	CDialog::OnOK();
}
