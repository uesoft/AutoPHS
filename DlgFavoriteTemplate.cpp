// DlgFavoriteTemplate.cpp : implementation file
//

#include "stdafx.h"
#include "autophs.h"
#include "DlgFavoriteTemplate.h"
#include "edibgbl.h"
#include "user.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgFavoriteTemplate dialog


CDlgFavoriteTemplate::CDlgFavoriteTemplate(CWnd* pParent ,long lSampleID)
	: CDialog(CDlgFavoriteTemplate::IDD, pParent)
{
	m_lSampleID = lSampleID;
}

CDlgFavoriteTemplate::CDlgFavoriteTemplate(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFavoriteTemplate::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgFavoriteTemplate)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_lCurSelSampleID = 0;
}


void CDlgFavoriteTemplate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgFavoriteTemplate)
		// NOTE: the ClassWizard will add DDX and DDV calls here
 	DDX_Control(pDX, IDC_LIST_FAVORITE, m_lstFavorite);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgFavoriteTemplate, CDialog)
	//{{AFX_MSG_MAP(CDlgFavoriteTemplateemplate)
	ON_BN_CLICKED(IDC_BTN_DELETE, OnDelete)
	ON_LBN_SELCHANGE(IDC_LIST_FAVORITE, OnSelchangeListFavorite)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgFavoriteTemplate message handlers

BOOL CDlgFavoriteTemplate::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//选出所有常用模板
	m_rsFavorite.m_pDatabase=&EDIBgbl::dbPRJ;
	if( m_rsFavorite.IsOpen() )
		m_rsFavorite.Close();
	m_rsFavorite.Open(
		dbOpenDynaset,
		"SELECT * FROM PhsStructureName Where [Favorite]=-1 ORDER By [SampleID]");

	InitFavoriteList();
	return TRUE;
}

void CDlgFavoriteTemplate::InitFavoriteList()
{
	m_lstFavorite.ResetContent();
	CString strTemplateName;
	int iWillSelIndex=0;
	long iSampleID=1;
	COleVariant varTmp;

	if(m_rsFavorite.IsBOF() || m_rsFavorite.IsEOF() )
	{
		return;
	}

	for(int iIndex=0; !m_rsFavorite.IsEOF() ; m_rsFavorite.MoveNext())
	{
		m_rsFavorite.GetFieldValue("SampleID",varTmp);
		iSampleID = vtoi(varTmp);
		m_rsFavorite.GetFieldValue("SampleName",varTmp);
		if( vtos(varTmp) != "" )
		{
			iIndex = m_lstFavorite.AddString(vtos(varTmp));
			m_lstFavorite.SetItemData(iIndex,(DWORD)iSampleID);
		}
		if(m_lSampleID == iSampleID)
		{
			iWillSelIndex = iIndex;
		}
	}
	m_iCurSel = iWillSelIndex;
	m_lstFavorite.SetCurSel(iWillSelIndex);
}

void CDlgFavoriteTemplate::OnDelete() 
{
	Delete(m_iCurSel);
}

void CDlgFavoriteTemplate::Delete(int index)
{
	CString strSQL;

	strSQL.Format("UPDATE PhsStructureName SET [Favorite]=0 WHERE [SampleID]=%d",
		m_lstFavorite.GetItemData(index));
	EDIBgbl::dbPRJ.Execute(strSQL);

	m_lstFavorite.DeleteString(index);
}

void CDlgFavoriteTemplate::OnOK() 
{
	m_lCurSelSampleID = m_lstFavorite.GetItemData(m_iCurSel);
	CDialog::OnOK();
}

void CDlgFavoriteTemplate::OnSelchangeListFavorite() 
{
    m_iCurSel = m_lstFavorite.GetCurSel();
}
