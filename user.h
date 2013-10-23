// user.h


#if !defined(AFX_USER_H__66EB6429_712C_11D5_8752_00D009619765__INCLUDED_)
#define AFX_USER_H__66EB6429_712C_11D5_8752_00D009619765__INCLUDED_


#include "resource.h"

CString inline GetFileName(LPCTSTR lpszPathName)
{
	CString strPathName(lpszPathName);
	return strPathName.Mid(strPathName.ReverseFind(_T('\\'))+1);
}
void inline ReportError(LPCTSTR lpszErrMsg,LPCTSTR lpszFile,int iLine)
{
	CString strMsg;
	strMsg.Format(IDS_EXCEPTION,lpszErrMsg,GetFileName(lpszFile),iLine);
	AfxMessageBox(strMsg);
}
CString FormatVBString(LPCTSTR lpszFmt,double dVal);
class Form;
class PictureBox;

class user
{
public:
	static int s_iFilterIndex;
	static CString ChangePathSlash(CString Path);
	static CString SelFile(CString DlgTitle,CString filter,int FilterInd,CString InitPath, bool CreatePrompt=false);
	static CString SelFileDwg(LPCTSTR filter, CString& initDir,int& iFilterIndex=s_iFilterIndex);
	static DWORD GetVersion();
	static CString GetAppFileName();
	static CString GetAppPath();

    static void GetSysDir();
    /*
     * -----------------------------------------------------------
     * -----------------------------------------------------------
     *  SUB: AddDirSep
     *  Add a trailing directory path separator (back slash) to the
     *  end of a pathname unless one already exists
     * 
     *  IN/OUT: [strPathName] - path to add separator to
     * -----------------------------------------------------------
     */
    static void AddDirSep(CString &strPathName);
    /*
     * -----------------------------------------------------------
     *  SUB: RemoveDirSep
     *  Remove a trailing directory path separator (back slash) from the
     *  end of a pathname unless one doesn't exists
     * 
     *  IN/OUT: [strPathName] - path to remove separator from
     * -----------------------------------------------------------
     */
    static void RemoveDirSep(CString& strPathName);
    static bool GetPos(CString frmName,CRect& wndRect);
    static void SavePos(CWnd* frm,CString frmName);
    static void EditTextBox(CEdit* editBox);
    /*
     * 设置文件日期时间
     */
    /*
     * 各种应用程序的About窗体资源常数
     */
    /*static const int	resSTRabout;
    static const int	resSTRBASE;
    static const int	resSTRCompanyTel;
    static const int	resSTRCompanyMTel;
    static const int	resSTRCompanyEmail;
    static const int	resSTR_EDInBox;
    static const int	resBMP_EDInBox;
    static const int	resSTR_WATER;
    static const int	resBMP_WATER;
    static const int	resSTR_RC;
    static const int	resBMP_RC;
    static const int	resSTR_YLJSJK;
    static const int	resBMP_YLJSJK;
    static const int	resSTR_VENTPIPE;
    static const int	resBMP_VENTPIPE;
    static const int	resSTR_STRESSANA;
    static const int	resBMP_STRESSANA;
    static const int	resSTR_PHS;
    static const int	resBMP_PHS;
    static const int	resSTR_eManual;
    static const int	resBMP_eManual;*/
    /*
     * public Constants
     */
    static  const char*	gstrNULL;
    static const char*	gstrSEP_DIR;
    static const char*	gstrSEP_DIRALT;
    static const char*	gstrSEP_EXT;
    static const char*	gstrCOLON;
    static const char*	gstrSwitchPrefix1;
    static const char*	gstrSwitchPrefix2;
    static const char*	gstrCOMMA;
    static const char*	gstrDECIMAL;
    static const char*	gstrINI_PROTOCOL;
    static const int	gintMAX_SIZE;
    static const int	gintMIN_BUTTONWIDTH;
    static const float	gsngBUTTON_BORDER;
    static const int	intDRIVE_REMOVABLE;
    static const int	intDRIVE_FIXED;
    static const int	intDRIVE_REMOTE;
    static const int	gintNOVERINFO;
    static CString	gstrWinDir;
    static CString	gstrWinSysDir;
    static CString	gstrAppName;
    static CString	gstrChnAppName;
    /*
     * 
     * public Variables
     */
    static CString	LF;
    static CString	LS;
    /*
     * 主窗体对象
     */
//    static CWnd*	gObjMainFrm;

protected:


private:


};
CString GetShareDbPath();//得到PSA所用单位数据库的路径
extern int  vtoi(_variant_t & v);
extern int  vtoi2(COleVariant & v);
extern float vtof(_variant_t &v);
extern int  vtoi(COleVariant & v);
extern float vtof(COleVariant &v);
extern double vtod(_variant_t &v);
extern double vtod(COleVariant &v);
extern bool vtob(VARIANT &v);
extern CString inline ftos1(float v);
extern CString GetStrAtEx(CString strText,int i); //返回字符串中子串，(格式：动态库AutoPSA 生成文件中的格式);
extern int GetStrAtCountEx(CString strText); //返回字符串中子串，(以空格隔开的字符串);

CString  Trim(LPCTSTR pcs);

extern CString inline ltos(long v);
extern CString inline ftos(float v);
extern int inline Fix(double dv);
extern  CString inline GetResStr(UINT id);
extern bool FileExists(CString filename);
extern bool DirExists(CString Directory);
extern void SetFileTime(CString Sfile,CString Dfile);
extern CString GetFilePath(CString FileName);//不带 '\'
extern bool inline IsNull(_variant_t &v);
//#define IsNull ( V ) V.vt==VT_NULL
//#define IsNull (v)   v.vt==VT_NULL
#define MAJOR(X)   (WORD)(X>>16)
#define MINOR(X)   (WORD)X 
extern CString gObjMainFrm;
//extern int vtoi( VARIANT & v );
extern void RsDeleteAll(_Recordset& rs);
extern COleVariant inline STR_VAR( LPCTSTR X );
#define V_STR( X ) (X.vt!=VT_NULL ? X.pcVal : "") 
extern _ConnectionPtr conPRJDB;
extern _ConnectionPtr conPRJDB4;
extern _ConnectionPtr conPHSConfig;
extern _ConnectionPtr conZDJCrude;
extern CString vtos(COleVariant& v);
extern CString vtos(_variant_t& v);
extern CString vtos2(COleVariant& v);
extern float Sgn(float x);
extern _variant_t stov(LPCTSTR svar);
extern bool GetAllMenu(HMENU hMenu,long pos);
extern bool DisabledSystemMenu(HWND hWnd,HMENU hMenu);
extern CString inline btos(BOOL v);
extern int inline ShowMessage(LPCTSTR lpszText, UINT nType = MB_OK,LPCTSTR lpszTitle=AfxGetApp()->m_pszAppName, UINT nIDHelp = 0 );
extern COleVariant inline GetFields(_RecordsetPtr rs,CString FieldName);
extern void SetWindowCenter(HWND hWnd); 
extern void inline DoEvents();
extern BOOL MakePath(CString strDirName);
extern void ModifyControlStyle(HWND &hWnd,DWORD dwRemove=0,DWORD dwAdd=0);
extern const _TCHAR dbConnectionString[];
extern const _TCHAR dbConnectionString4[];

#define EXECUTE_TIME_BEGIN \
long first_execute_time,end_execute_time; \
CString str_execute_time; \
first_execute_time = GetTickCount();

#define EXECUTE_TIME_END end_execute_time = GetTickCount();\
end_execute_time -= first_execute_time;\
str_execute_time.Format("%d", end_execute_time);\
AfxMessageBox(str_execute_time);

#endif /* user_h */
