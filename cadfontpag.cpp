// CadFontPag.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "CadFontPag.h"
#include "user.h"
#include "basDirectory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCadFontPag dialog


CCadFontPag::CCadFontPag(CString* pstrSHXFont,CString* pstrBigFont,float * pfWidthFactor,int* pAlign,CWnd* pParent /*=NULL*/)
	: CDialog(CCadFontPag::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCadFontPag)
	//}}AFX_DATA_INIT
	m_piAlign = pAlign;
	m_pstrSHXFont = pstrSHXFont;
	m_pstrBigFont = pstrBigFont;
	m_pfWidthFactor=pfWidthFactor;
}


void CCadFontPag::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCadFontPag)
	DDX_Control(pDX, IDC_TEXTALIGN, m_comboAlign);
	DDX_Control(pDX, IDC_SHXFONT, m_comboSHXFont);
	DDX_Control(pDX, IDC_BIGFONT, m_comboBigFont);
	//}}AFX_DATA_MAP
	DDX_CBIndex(pDX, IDC_TEXTALIGN, *m_piAlign);
	DDX_CBString(pDX, IDC_SHXFONT, *m_pstrSHXFont);
	DDX_CBString(pDX, IDC_BIGFONT, *m_pstrBigFont);
	DDX_Text(pDX, IDC_EDIT_WIDTHFACTOR, (float)*m_pfWidthFactor);
}


BEGIN_MESSAGE_MAP(CCadFontPag, CDialog)
	//{{AFX_MSG_MAP(CCadFontPag)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCadFontPag message handlers

void CCadFontPag::LoadCombo()
{
    //取消这项功能
    return;

    CString strFontDir=GetCADFontDir();
    if(strFontDir.IsEmpty())
        return;
    WIN32_FIND_DATA fd;
    memset(&fd,0,sizeof(fd));
    HANDLE fh;
    //_tcscpy(fd.cFileName,(_T("*.shx")));
    CString sDir=strFontDir+(_T("\\*.shx"));
    
    fh=FindFirstFile(sDir,&fd);
    if(fh!=INVALID_HANDLE_VALUE)
    {
        do
        {
            CString strFileName=strFontDir + (_T("\\")) + fd.cFileName;
            strFileName.MakeLower();
            if(strFileName.GetLength()>3 && strFileName.Right(4)==_T(".shx"))
            {
                try
                {
                    CFile file(strFileName,CFile::modeRead);
                    CString strVal;
                    char data[30];
                    int iCount=file.Read(data,29);
                    data[iCount]=0;
                    strVal=data;
                    strVal.MakeLower();
                    if(strVal.Find((_T("bigfont")))!=-1)
                    {
                        strFileName=strFileName.Mid(strFileName.ReverseFind((_T('\\')))+1);
                        strFileName=strFileName.Left(strFileName.ReverseFind((_T('.'))));
                        m_comboBigFont.AddString(strFileName);
                    }
                    else if(strVal.Find((_T("unifont")))!=-1)
                    {
                        strFileName=strFileName.Mid(strFileName.ReverseFind((_T('\\')))+1);
                        strFileName=strFileName.Left(strFileName.ReverseFind((_T('.'))));
                        m_comboSHXFont.AddString(strFileName);
                    }
                    file.Close();
                }
                catch(CFileException * e)
                {
                    if(e->m_cause==11)
                    {
                        //由于 AutoCAD 当前使用该字体，而发生共享违例
                        strFileName=strFileName.Mid(strFileName.ReverseFind((_T('\\')))+1);
                        if(::CopyFile(e->m_strFileName,basDirectory::TemplateDir + strFileName,FALSE))
                        {
                            try
                            {
                                CFile file(basDirectory::TemplateDir + strFileName,CFile::modeRead);
                                CString strVal;
                                char data[30];
                                int iCount=file.Read(data,29);
                                data[iCount]=0;
                                strVal=data;
                                strVal.MakeLower();
                                if(strVal.Find(_T("bigfont"))!=-1)
                                {
                                    strFileName=strFileName.Left(strFileName.ReverseFind((_T('.'))));
                                    m_comboBigFont.AddString(strFileName);
                                }
                                else if(strVal.Find((_T("unifont")))!=-1)
                                {
                                    strFileName=strFileName.Left(strFileName.ReverseFind((_T('.'))));
                                    m_comboSHXFont.AddString(strFileName);
                                }
                                file.Close();
                            }
                            catch(CFileException * e)
                            {
#ifdef _DEBUG
                                e->ReportError();
#endif
                                e->Delete();
                            }
                        }
                    }
                    else
                    {
#ifdef _DEBUG
                        e->ReportError();
#endif
                    }
                    e->Delete();
                }
            }
        }while(FindNextFile(fh,&fd));
        FindClose(fh);
    }
}

CString CCadFontPag::GetCADFontDir()
{
	HKEY hKey;
	CString strSubKey=(_T("Software\\Autodesk\\AutoCAD\\R14.0"));
	if(ERROR_SUCCESS!=RegOpenKey(HKEY_LOCAL_MACHINE,strSubKey,&hKey))
	{
		strSubKey=(_T("Software\\Autodesk\\AutoCAD\\R14.01"));
		if(ERROR_SUCCESS!=RegOpenKey(HKEY_LOCAL_MACHINE,strSubKey,&hKey))
		{
			strSubKey=(_T("Software\\Autodesk\\AutoCAD\\R15.0"));
			if(ERROR_SUCCESS!=RegOpenKey(HKEY_LOCAL_MACHINE,strSubKey,&hKey))
			{
				return (_T(""));
			}
		}
	}
	_TCHAR szValue[256];
	long len=256*sizeof(_TCHAR);
	ULONG type=REG_SZ;
	if(ERROR_SUCCESS!=RegQueryValueEx(hKey,(_T("CurVer")),NULL,&type,(unsigned char*)szValue,(unsigned long*)&len))
	{
		RegCloseKey(hKey);
		return (_T(""));
	}
	HKEY hKey2;
	if(ERROR_SUCCESS!=RegOpenKey(hKey,szValue,&hKey2))
	{
		RegCloseKey(hKey);
		return (_T(""));
	}
	RegCloseKey(hKey);
	len=256*sizeof(_TCHAR);
	if(ERROR_SUCCESS!=RegQueryValueEx(hKey2,(_T("AcadLocation")),NULL,&type,(unsigned char*)szValue,(unsigned long*)&len))
	{
		RegCloseKey(hKey2);
		return (_T(""));
	}
	RegCloseKey(hKey2);
	CString strRet=szValue;
	user::AddDirSep(strRet);
	strRet+=(_T("FONTS"));
	return strRet;
}

BOOL CCadFontPag::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	LoadCombo();
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCadFontPag::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

void CCadFontPag::OnOK() 
{
	// TODO: Add extra validation here
	
	//CDialog::OnOK();
}

void CCadFontPag::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	//CDialog::OnCancel();
}

void CCadFontPag::DoDefaultSetting()
{
    //取消这项功能
    return ;

	*m_piAlign = 0;
	*m_pstrSHXFont = "txt";
	*m_pstrBigFont = "hzfs";
	*m_pfWidthFactor = 1.0;

	UpdateData(FALSE);
}
