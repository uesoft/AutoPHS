// ComStart.h
//

#if !defined(ComStart_h)
#define ComStart_h

#include "resource.h"


class ComStart
{
public:
	static int MMkDir(CString strDirName);

    //static Integer GetOpenFileName(OPENFILENAME lpofn);
    static void CreatePostMail();
    static bool GetComDir();
    static void UpdateAutoexec();
    static void CreateStressAnaBatch();
    static void CheckCmdLine();
    static bool GetEDinBoxDir();
    static void GetRcDir();
    static const char*	YLJSBAT;

protected:


private:


};

#endif /* ComStart_h */
