// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__234CA37F_18CC_4419_AEE8_96D7FFDDB1E6__INCLUDED_)
#define AFX_STDAFX_H__234CA37F_18CC_4419_AEE8_96D7FFDDB1E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _CRT_SECURE_NO_WARNINGS
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxtempl.h>
#include <afxdlgs.h>

#include <vector>
#include <string>
#include <algorithm>
#include <atlbase.h>

//#import "C:\\Program Files (x86)\\Common Files\\System\\ado\\msado60_backcompat_i386.tlb" no_namespace 
//#import "C:\Program Files\Common Files\System\ado\msado60.tlb" no_namespace  
#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace  \
rename("EOF","adoEOF") rename("EditModeEnum","adoEditModeEnum")\
rename("LockTypeEnum","adoLockTypeEnum") rename("FieldAttributeEnum","adoFieldAttributeEnum")\
rename("DataTypeEnum","adoDataTypeEnum") rename("ParameterDirectionEnum","adoParameterDirectionEnum")\
rename("RecordStatusEnum","adoRecordStatusEnum") 



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__234CA37F_18CC_4419_AEE8_96D7FFDDB1E6__INCLUDED_)
