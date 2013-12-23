// FrmFolderLocation.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "FrmFolderLocation.h"
#include "modRegistry.h"
#include "user.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFrmFolderLocation dialog

CFrmFolderLocation frmFolderLocation;
CFrmFolderLocation::CFrmFolderLocation(CWnd* pParent /*=NULL*/)
	: CDialog(CFrmFolderLocation::IDD, pParent)
{
	//m_strFolderLocation = _T("");
	//{{AFX_DATA_INIT(CFrmFolderLocation)
	//}}AFX_DATA_INIT
}


void CFrmFolderLocation::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFrmFolderLocation)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFrmFolderLocation, CDialog)
	//{{AFX_MSG_MAP(CFrmFolderLocation)
	ON_WM_KEYUP()
	ON_BN_CLICKED(IDC_SF1, OnSf1)
	ON_BN_CLICKED(IDC_SF2, OnSf2)
	ON_BN_CLICKED(IDC_SF3, OnSf3)
	ON_BN_CLICKED(IDC_SF4, OnSf4)
	ON_BN_CLICKED(IDC_SF5, OnSf5)
	ON_BN_CLICKED(IDC_SF6, OnSf6)
	ON_BN_CLICKED(IDC_SF7, OnSf7)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFrmFolderLocation message handlers

CString CFrmFolderLocation::GetDir(CString key)
{
	CString sDir("");
	sDir=::GetRegKey(_T("directory"),key,CString(_T("")));
	sDir.MakeLower();
	return sDir;
}

CFrmFolderLocation::SetDir(CString key,CString setval)
{
	::SetRegValue(_T("directory"),key,setval);
}

//DEL int CFrmFolderLocation::OnCreate(LPCREATESTRUCT lpCreateStruct) 
//DEL {
//DEL 	if (CDialog::OnCreate(lpCreateStruct) == -1)
//DEL 		return -1;
//DEL 	return 0;
//DEL 	// TODO: Add your specialized creation code here
//DEL 
//DEL 	
//DEL 	
//DEL }

void CFrmFolderLocation::LoadCaption()
{
	SetWindowText(GetResStr(IDS_frmFolderLocation_frmFolderLocation));
	SetDlgItemText(IDC_CAUTION,GetResStr(IDS_WarnDontRandomEDITthisDirectoryString));
}

void CFrmFolderLocation::OnSf(UINT uID) 
{
	// TODO: Add your control notification handler code here
	CString sStart,sSelected;
	CString szTmp;
	CFrmFolderLocation::GetDlgItemText(uID,szTmp);
	sStart=szTmp;
	//remove the back slash if user add
	while (sStart.Right(1)=="\\") sStart.Delete(sStart.GetLength()-1);
	BrowseForFolerModule::BrowseForFoldersFromPathStart(this->GetSafeHwnd(),sStart,sSelected);
	if (!sSelected.IsEmpty())
		CFrmFolderLocation::SetDlgItemText(uID,sSelected);
}

void CFrmFolderLocation::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	//if user press 'Escape' then exit the dialog
	if (nChar==VK_ESCAPE)
			CFrmFolderLocation::OnCancel();
	CDialog::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CFrmFolderLocation::OnOK() 
{
		CString X;
		CString sTmp,sMessage;
		bool PrjDBDirChanged = false;//项目数据库目录被改变，需要升级
		bool PrjDirChanged = false;//项目临时数据库目录被改变，需要升级
	for(int i=0;i<7;i++){
		GetDlgItem(IDC_FL1+i)->GetWindowText(X);
		sTmp=Trim(X);
		sTmp.MakeLower();
		SetDir(basDirectory::gstrEDIBdir[i],sTmp);
		user::AddDirSep(sTmp);
		if(!::DirExists(sTmp))
		{
			sMessage.Format(IDS_RESETPath,sTmp);
			MessageBox(sMessage);
			return;
		}
		switch(i){
			case iEDIBprjDwgDir:
				basDirectory::ProjectDWGDir.MakeLower();
				if (sTmp!=basDirectory::ProjectDWGDir){
							PrjXDirChanged=true;
							basDirectory::ProjectDWGDir=sTmp;
				}
				break;
			case iEDIBprjDbDir:
				basDirectory::ProjectDBDir.MakeLower();
				if (sTmp!= basDirectory::ProjectDBDir){
							PrjXDirChanged=true;
							PrjDBDirChanged=true;
							basDirectory::ProjectDBDir=sTmp;
				}
				break;
			case iEDIBprjDir:
				if (sTmp!= basDirectory::ProjectDir){
							PrjXDirChanged=true;
							PrjDirChanged=true;
							basDirectory::ProjectDir=sTmp;
				}
				break;
			case iEDIBBlkDir:
				if (sTmp!=basDirectory::BlkDir){
					PrjXDirChanged=true;
						 basDirectory::BlkDir=sTmp;
				}
				break;
			case iEDIBphsBlkDir:
				if (sTmp!=basDirectory::PhsBlkDir) {
					PrjXDirChanged = true;
						 basDirectory::PhsBlkDir = sTmp;
				}
				break;
			case iEDIBinstallDir:
						basDirectory::InstallDir = sTmp;
					break;
			case iEDIBTemplateDir:
				if (sTmp != basDirectory::TemplateDir){
							PrjXDirChanged = true;
							basDirectory::TemplateDir = sTmp;
				}
		}//switch end
	}
	if (PrjXDirChanged){
		if (IDYES==(::MessageBox(this->m_hWnd,GetResStr(IDS_NeedReStartBecaueEditDirectoryString),
			_T("AutoPHS"),MB_YESNO|MB_ICONEXCLAMATION|MB_SYSTEMMODAL)))
		{
			((CMainFrame*)AfxGetApp()->m_pMainWnd)->m_bIsExitMsg=false;
			//新路径下的数据库可能需要升级。
			if (PrjDBDirChanged || PrjDirChanged)
				::SetRegValue(_T("Status"), _T("Install"), CString(_T("0")));
			::PostMessage(AfxGetMainWnd()->m_hWnd,WM_CLOSE,0,0);

		}
	}
	PrjXDirChanged=false;
	CDialog::OnOK();
}

//DEL void CFrmFolderLocation::OnCancel() 
//DEL {
//DEL 	CDialog::OnCancel();
//DEL }

BOOL CFrmFolderLocation::OnInitDialog() 
{
	CDialog::OnInitDialog();
	::SetWindowCenter(this->m_hWnd);
	// TODO: Add extra initialization here
	LoadCaption();
	basDirectory::InitgstrEDIBdir();
	//GetPos Me
	//this->SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	//Load Lable and Editbox's caption
	for (int i=0;i<7;i++){
      SetDlgItemText(IDC_DIR1+i,GetResStr(IDS_EDIBDir0+i));
	  SetDlgItemText(IDC_FL1+i,GetDir(basDirectory::gstrEDIBdir[i]));
	}

   
	GetDlgItem(IDC_FL1+iEDIBinstallDir)->EnableWindow(false);
	GetDlgItem(IDC_SF1+iEDIBinstallDir)->EnableWindow(false);
   
    INIchanged=false;//Assume Dir isn't changed
    PrjXDirChanged=false;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFrmFolderLocation::OnSf1() 
{
	// TODO: Add your control notification handler code here
	OnSf(IDC_FL1);
}

void CFrmFolderLocation::OnSf2() 
{
	// TODO: Add your control notification handler code here
	OnSf(IDC_FL2);
}

void CFrmFolderLocation::OnSf3() 
{
	// TODO: Add your control notification handler code here
	OnSf(IDC_FL3);
}

void CFrmFolderLocation::OnSf4() 
{
	// TODO: Add your control notification handler code here
	OnSf(IDC_FL4);
}

void CFrmFolderLocation::OnSf5() 
{
	// TODO: Add your control notification handler code here
	OnSf(IDC_FL5);
}

void CFrmFolderLocation::OnSf6() 
{
	// TODO: Add your control notification handler code here
	OnSf(IDC_FL6);
}

void CFrmFolderLocation::OnSf7() 
{
	// TODO: Add your control notification handler code here
	OnSf(IDC_FL7);
}
