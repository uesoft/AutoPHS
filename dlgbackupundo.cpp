// DlgBackupUndo.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "DlgBackupUndo.h"
#include "EDIBgbl.h"
#include "modregistry.h"
#include "user.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgBackupUndo dialog


CDlgBackupUndo::CDlgBackupUndo(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgBackupUndo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgBackupUndo)
	//}}AFX_DATA_INIT
	m_strAppInstallPath=_T("");
}


void CDlgBackupUndo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBackupUndo)
	DDX_Control(pDX, IDC_COMBO_HAD_BACKUP, m_comboHadBackup);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBackupUndo, CDialog)
	//{{AFX_MSG_MAP(CDlgBackupUndo)
	ON_CBN_SELCHANGE(IDC_COMBO_HAD_BACKUP, OnSelchangeComboHadBackup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBackupUndo message handlers

void CDlgBackupUndo::OnSelchangeComboHadBackup() 
{
   	CString str;
	m_comboHadBackup.GetLBText(m_comboHadBackup.GetCurSel(),str);
	//m_strDes = m_map[str];
    UpdateData(FALSE);	
}

void CDlgBackupUndo::OnOK() 
{
	UpdateData(FALSE);
    if( AfxMessageBox(IDS_RESTOREDB_YESNO,MB_ICONINFORMATION|MB_YESNO) == IDYES )	
	{
		//在还原以前首先对当前的数据库作备份以防万一
		CopyFile(m_strPrjDBPath+"AllPrjDB.mdb",m_strPrjDBPath+"AllPrjDB.bak",FALSE);
		CopyFile(m_strPrjDBPath+"Sort.mdb",m_strPrjDBPath+"Sort.bak",FALSE);

		EDIBgbl::dbPRJDB.Close();
		EDIBgbl::dbSORT.Close();

		CString strDir;
		m_comboHadBackup.GetLBText(m_comboHadBackup.GetCurSel(),strDir);
		CopyFile(m_strAppInstallPath+"BackupDB\\"+strDir+"\\AllPrjDB.mdb",m_strPrjDBPath+"AllPrjDB.mdb",FALSE);
		CopyFile(m_strAppInstallPath+"BackupDB\\"+strDir+"\\Sort.mdb",m_strPrjDBPath+"Sort.mdb",FALSE);

		AfxMessageBox(IDS_RESTART_AUTOPHS);

		CDialog::OnOK();
	}

	return;	
}

BOOL CDlgBackupUndo::OnInitDialog() 
{
	CDialog::OnInitDialog();
	

	m_strPrjDBPath = GetRegKey(_T("Directory"), _T("EDInBox_PrjDBDir"), CString(_T("")));
	m_strAppInstallPath = GetRegKey(_T("Directory"), _T("EDInBox_InsDir"), CString(_T("")));

	CFileFind filefinder;
	BOOL bWorking=FALSE;
	filefinder.FindFile(m_strAppInstallPath+"BackupDB\\*.*");
	
	do
	{
		bWorking = filefinder.FindNextFile();
		if( filefinder.IsDirectory() )
		{
			if( !filefinder.IsDots() )
				m_comboHadBackup.AddString(filefinder.GetFileName());
		}
	}while( bWorking );

	if( m_comboHadBackup.GetCount()>0 )
	{
		m_comboHadBackup.SetCurSel(0);
		//m_comboHadBackup.GetLBText(0,strFirst);
		//m_strDes = m_map[strFirst];
	}
	else
	{
		m_comboHadBackup.AddString(_T("暂无备份"));
		m_comboHadBackup.SetCurSel(0);
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}

	UpdateData(FALSE);	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
