#if !defined(AFX_IMPORTFROMXLSDLG_H__B50DF418_09BE_4479_9AD3_1EB20C186C24__INCLUDED_)
#define AFX_IMPORTFROMXLSDLG_H__B50DF418_09BE_4479_9AD3_1EB20C186C24__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImportFromXLSDlg.h : header file
//
#define CONNECTSTRING_EXCEL "Provider=Microsoft.Jet.OLEDB.4.0;Extended properties=Excel 5.0;Data Source="

/////////////////////////////////////////////////////////////////////////////
// 
// 从外部导入原始数据对话框的基类
//
// 调用次对话框前需要调用SetProjectID，SetProjectDbConnect
// 设置工程的编号与连接
//
/////////////////////////////////////////////////////////////////////////
class _Application;

#include "MyButton.h"
#include "PropertyWnd.h"	// 属性窗口类
#include "excel9.h"
#include "resource.h"

class CImportFromXLSDlg : public CDialog
{
public:
	// 从EXCEL文件导入数据的子结构
	struct ImportFromXLSElement		
	{
		CString SourceFieldName;	// Excel的起始列号
		int		ExcelColNum;		// Excel的列数.
		CString DestinationName;	// 对应到目的数据库的字段名
	};

	// 描述EXCEL文件导如数据的结构
	struct ImportFromXLSStruct
	{
		CString XLSFilePath;		// EXCEL文件的路径
		CString	XLSTableName;		// EXCEL的工作薄名
		int		BeginRow;			// 开始导入的行号
		int		RowCount;			// 需要的导入的记录数

		CString ProjectDBTableName;	// 导入到的目的工程表名
		CString ProjectID;			// 导入到的目的工程名

		ImportFromXLSElement *pElement;	// 指向子单元的结构
		int ElementNum;	//子单元的结构数目
	};

public:
	BOOL ImportExcelToAccess(ImportFromXLSStruct *pImportStruct);

// Construction
public:
	CImportFromXLSDlg(CWnd* pParent = NULL);   // standard constructor
	~CImportFromXLSDlg();

public:
	CPropertyWnd* GetPropertyWnd();
	CString GetHintInformation();
	void SetHintInformation(LPCTSTR szHint);

	CString GetRegSubKey();
	void SetRegSubKey(LPCTSTR szRegSubkey);

	_ConnectionPtr GetProjectDbConnect();
	void SetProjectDbConnect(_ConnectionPtr IConnection);

	CString GetProjectID();
	void SetProjectID(LPCTSTR szProjectID);


// Dialog Data
	//{{AFX_DATA(CImportFromXLSDlg)
	enum { IDD = IDD_IMPORT_FROM_XLS };
	CMyButton	m_OpenFileDlgButton;
	CString	m_XLSFilePath;
	CString	m_HintInformation;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImportFromXLSDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	// 将信息保存到注册表内
	virtual void SaveSetToReg();
	// 从注册表获得数据初始化
	virtual void InitFromReg();
	
	// 初始化属性窗口
	virtual BOOL InitPropertyWnd();

	// 在导入时验证输入数据的有效性
	virtual BOOL ValidateData();

	// 执行导入数据的操作
	virtual void BeginImport();

protected:
	CPropertyWnd m_PropertyWnd;		// 属性窗口控件

private:
	void StringToNumber(CString strNO, long & nNO);

	CString m_strProjectID;				// 工程的编号
	_ConnectionPtr m_ProjectDbConnect;	// 工程相关的数据库
	_ConnectionPtr m_pConExcel;			// 用ADO方式联接EXCEL
	
	CString	m_RegSubKey;	// 用于存放信息的子键名称
	CString m_strPrecFilePath;

protected:
	BOOL OpenExcelTable(_RecordsetPtr pRsTbl, CString& strSheetName, CString strExcelFileName);
	BOOL ConnectExcelFile(const CString strExcelName, _ConnectionPtr & pConExcel);

	// Generated message map functions
	//{{AFX_MSG(CImportFromXLSDlg)
	virtual BOOL OnInitDialog();
	virtual afx_msg void OnOpenFiledlg();
	afx_msg void OnDestroy();
	virtual afx_msg void OnBeginImport();
	//}}AFX_MSG
	virtual afx_msg	void OnSelectChange(NMHDR *pNMHDR,LRESULT *pResult);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMPORTFROMXLSDLG_H__B50DF418_09BE_4479_9AD3_1EB20C186C24__INCLUDED_)
