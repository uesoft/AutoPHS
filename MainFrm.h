// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__10451401_4E56_4046_BCAC_3DD09B004748__INCLUDED_)
#define AFX_MAINFRM_H__10451401_4E56_4046_BCAC_3DD09B004748__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define WM_UPD_OPTION WM_USER + 101
#define WM_QUIT_CUS WM_USER + 103
class CMainFrame : public CFrameWnd
{
	
public: // create from serialization only
	CMainFrame();
protected:
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	enum mnuZDJ_DRAWindex
	{
	   iZDJDRAWTag=0,
	   iZDJDRAWphs,
	   iZDJDRAWbom,
	   iZDJDRAWload,
	   iZDJDRAW180mm,
	   iZDJDRAW270mm,
	   iZDJDRAW705mm ,   //支吊架明细一览表
	   iZDJDRAW270pc,    //支吊架明细一览表(石化)
	   iZDJDRAW270L,//支吊架明细一览表(轻工)
	   iZDJDRAWEXECL,//
	   iZDJDRAWCatalogue,//目录
	   iZDJDRAWm_phs, //开始手工修改，实际上切换到m_phs层
	   iZDJDRAWCatalogueExcel,
	   iZDJDRAWXN180mm,    //西南院支吊架明细一览表(180mm,A4)
	   iZDJDRAWbomBJBW,//北京巴威零件明细表
	   iZDJDRAWloadBJBW,   //北京巴威支吊荷载位移表(202mm)
	   iZDJDRAW705mmHSY, //核四院支吊架明细一览表
	   iZDJDRAW705mmQGY,	 //青工院支吊架明细一览表
	   iZDJDRAW705mmHBYZ,   //华北冶建
	   iZDJDRAWBJHY01,//北京华宇格式<一>
	   iZDJDRAWBJHY02 //北京华宇格式<二>
	};

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SumCL(int Index);
	void DDEFileCmd(DWORD cmd);
	void InitPos();
	long GetMenuHeight();
	void SetMenuEnabled();
	int Timer1Interval;
//	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	afx_msg LRESULT OnUpdOption(WPARAM wParam,LPARAM lParam);
	void mnuRecFile_Click(int index);
public:
	BOOL Start();
	bool m_bIsExitMsg;
	void AddRecentFiles(CString FileName);
	int m_nRecFileNum;
	void LoadRecentFiles();
	static void CalAllZdjh(LPVOID pParam);
	void DrawZdjTab(int index);
	//{{AFX_MSG(CMainFrame)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnIntTabint();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnFileNewDwg();
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnIntImgint();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnSelPDSV();
	afx_msg void OnSampleManage();
	afx_msg void OnInputSArec();
	afx_msg void OnCalcZdjh();
	afx_msg void OnDrawZdj();
	afx_msg void OnStartacad();
	afx_msg void OnSelPreferences();
	afx_msg void OnFileOpt();
	afx_msg void OnIntImpdata();
	afx_msg void OnSetTableFormat();
	afx_msg void OnSumAcad();
	afx_msg void OnSumExc();
	afx_msg void OnSumTxt();
	afx_msg void OnSumAcad2();
	afx_msg void OnIntPipeana();
	afx_msg void OnFileSelDir();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileCloseAcad();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileNewOpen(CCmdUI* pCmdUI);
	afx_msg void OnCalAllzdj();
	afx_msg void OnFileCloseExcel();
	afx_msg void OnDrawZdjTab180();
	afx_msg void OnDrawZdjTab270();
	afx_msg void OnZdjDrawEP();
	afx_msg void OnZdjDrawPC();
	afx_msg void OnDrawZdjTML();
	afx_msg void OnEraseAll();
	afx_msg void OnEraseLayer();
	afx_msg void OnCadCel();
	afx_msg void OnCadMenu();
	afx_msg void OnDeleteLayerTag();
	afx_msg void OnCouShowtab();
	afx_msg void OnDrawChangeDim();
	afx_msg void OnRecFile1();
	afx_msg void OnRecFile2();
	afx_msg void OnRecFile3();
	afx_msg void OnRecFile4();
	afx_msg void OnRecFile5();
	afx_msg void OnRecFile6();
	afx_msg void OnRecFile7();
	afx_msg void OnRecFile8();
	afx_msg void OnUpdateSpecPa(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSpecCspr(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSpecBolts(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSpecPart(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSpecSa(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSpecSpr(CCmdUI* pCmdUI);
	afx_msg void OnCalAddmaterial();
	afx_msg void OnCalAllowMaxSpan();
	afx_msg void OnDrawZdjtabl();
	afx_msg void OnDrawZdjtablExce();
	afx_msg void OnSumExcel();
	afx_msg void OnDrawZdjdirExecl();
	afx_msg void OnFileOpenDb();
	afx_msg void OnToolWblock();
	afx_msg void OnToolInsertUserBlock();
	afx_msg void OnUpdateToolWblock(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToolInsertuserblock(CCmdUI* pCmdUI);
	afx_msg void OnPdmstophs();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	afx_msg void OnPhstToPdms();
	afx_msg void OnUpdatePdmsToPhs(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePhsToPdms(CCmdUI* pCmdUI);
	afx_msg void OnCalStruct();
	afx_msg void OnClose();
	afx_msg void OnUpdateIntImpdata(CCmdUI* pCmdUI);
	afx_msg void OnModifyZdjh();
	afx_msg void OnGuideEnter();
	afx_msg void OnZdjXn();
	afx_msg void OnExportCurrentVolume();
	afx_msg void OnImportVolume();
	afx_msg void OnFileBackupDB();
	afx_msg void OnFileRestoreDB();
	afx_msg void OnAutoUpdate();
	afx_msg LRESULT OnQuitCus(WPARAM wParam,LPARAM lParam);
	afx_msg void OnDrawZdjArx();
	afx_msg void OnUpdateDrawZdj(CCmdUI* pCmdUI);
	afx_msg void OnZdjDrawepHsy();
	afx_msg void OnMenuitemChangmaterial();
	afx_msg void OnZdjDrawepQgy();
	afx_msg void OnZdjHbyj();
	afx_msg void OnPhsToAutoiped();
	afx_msg void OnCommdboper();
	afx_msg void OnZdjAdrawbjhy();
	afx_msg void OnZdjBdrawbjhy();
	afx_msg void OnMENUImportAutoPHS();
	afx_msg void OnPlantvisionToPhs();
	afx_msg void OnMenuitemOutputpds();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
LRESULT CALLBACK CalAllZdjhProc();

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__10451401_4E56_4046_BCAC_3DD09B004748__INCLUDED_)
