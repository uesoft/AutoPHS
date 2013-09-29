# Microsoft Developer Studio Project File - Name="AnalysePHS" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=AnalysePHS - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AnalysePHS.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AnalysePHS.mak" CFG="AnalysePHS - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AnalysePHS - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AnalysePHS - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/AutoPHS50/AnalysePHS", KEZBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AnalysePHS - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386

!ELSEIF  "$(CFG)" == "AnalysePHS - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "AnalysePHS - Win32 Release"
# Name "AnalysePHS - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Src\AnalyseHang.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\AnalyseHangPara.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\AnalysePAPara.cpp
# End Source File
# Begin Source File

SOURCE=.\AnalysePHS.cpp
# End Source File
# Begin Source File

SOURCE=.\AnalysePHS.def
# End Source File
# Begin Source File

SOURCE=.\AnalysePHS.rc
# End Source File
# Begin Source File

SOURCE=.\Src\AnalysePHSPara.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\AnalyseSAPara.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\BuildSeleSQLPA.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\BuildSeleSQLSA.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\CalcConfigPara.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\CalFormulaOfFixPASATb.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\DataBasePara.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\InputZA.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\PHSDefine.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\PHSUser.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\PSAResult.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\SAbeamSel.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\SAG51Sel.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\SAiG100Sel.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\SALbraceFixG47Sel.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\SALbraceSel.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\SCLASel.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\SelectPA.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\SelectPartPHS.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\SelectPartSA.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\SSteelPropertyCSTb.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Src\StressOfMaterialUser.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\tmpCalFixPhsTb.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Inc\AnalyseHang.h
# End Source File
# Begin Source File

SOURCE=.\Inc\AnalyseHangPara.h
# End Source File
# Begin Source File

SOURCE=.\Inc\AnalysePAPara.h
# End Source File
# Begin Source File

SOURCE=.\Inc\AnalysePHSPara.h
# End Source File
# Begin Source File

SOURCE=.\Inc\AnalyseSAPara.h
# End Source File
# Begin Source File

SOURCE=.\Inc\BuildSeleSQLPA.h
# End Source File
# Begin Source File

SOURCE=.\Inc\BuildSeleSQLSA.h
# End Source File
# Begin Source File

SOURCE=.\Inc\CalcConfigPara.h
# End Source File
# Begin Source File

SOURCE=.\Inc\CalFormulaOfFixPASATb.h
# End Source File
# Begin Source File

SOURCE=.\Inc\DataBasePara.h
# End Source File
# Begin Source File

SOURCE=.\Inc\InputZA.h
# End Source File
# Begin Source File

SOURCE=.\Inc\PHSDefine.h
# End Source File
# Begin Source File

SOURCE=.\Inc\PHSUser.h
# End Source File
# Begin Source File

SOURCE=.\Inc\PSAResult.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\Inc\SAbeamSel.h
# End Source File
# Begin Source File

SOURCE=.\Inc\SAG51Sel.h
# End Source File
# Begin Source File

SOURCE=.\Inc\SAiG100Sel.h
# End Source File
# Begin Source File

SOURCE=.\Inc\SALbraceFixG47Sel.h
# End Source File
# Begin Source File

SOURCE=.\Inc\SALbraceSel.h
# End Source File
# Begin Source File

SOURCE=.\Inc\SCLASel.h
# End Source File
# Begin Source File

SOURCE=.\Inc\SelectPA.h
# End Source File
# Begin Source File

SOURCE=.\Inc\SelectPartPHS.h
# End Source File
# Begin Source File

SOURCE=.\Inc\SelectPartSA.h
# End Source File
# Begin Source File

SOURCE=.\Inc\SSteelPropertyCSTb.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Inc\StressOfMaterialUser.h
# End Source File
# Begin Source File

SOURCE=.\Inc\tmpCalFixPhsTb.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\AnalysePHS.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
