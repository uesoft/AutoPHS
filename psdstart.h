// PSDstart.h
//

#if !defined(AFX_PSDSTART_H__66EB6429_712C_11D5_8752_00D009619765__INCLUDED_)
#define AFX_PSDSTART_H__66EB6429_712C_11D5_8752_00D009619765__INCLUDED_


/*
 * 鉴于此模块可能要将源代码提供给开发者，故其中的提示文字不放到资源文件中更方便。
 */

#include "resource.h"
#include "time.h"

class PSDstart
{
public:

    static void ConvertAllDb(_variant_t dVer, _variant_t Pwd);
    static void tmpT();
    static void tmpSub2DB();
    static void tmpTelmdb2BitWareDefaultDBF();
    static bool Start();
    static CTime	sTime;
    static CTime	eTime;
protected:


private:


};

#endif /* PSDstart_h */
