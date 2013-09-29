// ModEncrypt.h
//

#if !defined(ModEncrypt_h)
#define ModEncrypt_h

#include "resource.h"


class ModEncrypt
{
public:
	static DWORD GetSaveDogNO();
	static DWORD GetNHDogNo();
	static void EncryptLogout();
	//static long gLngSavedMicroDogID;
	//static long gIngVersionID;
    static bool CombineEncryptedFile();
    static bool SplitEncryptedFile();
    static void MakeEncrypt();
    static long MakeEncryptFile(CString  p, CString  CompanyName);
    static void EncryptCompanyName(LPCSTR  strPassWord, bool  bGetPassWord);
    static void VerifyEncryptFile(/*CString &strPassWord*/);
    static void VerifyID();
    static bool CheckLegalUser();
    static void CheckInstallDate();
    /*
     * 加密文件名称//
     */
    static const char*	FilejmBASE0;
    static const char*	FilejmBASE1;
    static const char*	FilejmCompanyName0;
    static const char*	FilejmCompanyName1;
    static const char*	FilejmCombinedXYZ;
    static const char*	FilePsw;
    static const char*	FileIns;
    static const char*	FileOutDate;
    static const char*	FileFlag;
    static const char*	FileWinCom;
    static const char*	FileWinDir;
    /*
     * 合法用户
     */
    static bool	gbLegalUser;
    static bool	gbLegalStandardUser;//合法标准版用户
    static bool	gbLegalLearningUser;//合法学习版用户
    static bool	gbLegalDemoUser;//合法演示版用户
    static CString	gstrLegalCompanyName;
    static long	glngUseDemoVersionDays;//演示版实际使用天数
    static long	glngLimitedDemoVersionDays;//演示版使用天数上限

    static DWORD	gLngVersionID;//版本标志(演示版/学习版/标准版=1234/1235/1236)
    static long	glngMicroDogID;//加密锁流水号标志(每个加密锁都是唯一的)

    static long	gLngSavedMicroDogID;//保存的加密锁流水号标志(在加密文件中，每个加密锁都是唯一的)
    
	static CString	gstrDBZdjCrudePassWord;//合并的支吊架标准原始数据库密码
    static CString	gstrDBProductPassWord;//支吊架厂家提供的产品原始数据库密码
    static const char*	conStrDBPasswordSuffix;

protected:


private:


};

#endif /* ModEncrypt_h */
