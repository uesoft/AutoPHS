# Microsoft Developer Studio Project File - Name="AutoPHS" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=AutoPHS - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AutoPHS.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AutoPHS.mak" CFG="AutoPHS - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AutoPHS - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "AutoPHS - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/AutoPHS50", QQFAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AutoPHS - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "D:\shareDLL\lib"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W2 /GX /Zi /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Fr /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 DataFormatDlg.lib version.lib SelEngineVolume.lib FillExcelDll.lib PSAPointElevation.lib interfaceAndStruct.lib /nologo /subsystem:windows /debug /machine:I386

!ELSEIF  "$(CFG)" == "AutoPHS - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "D:\shareDLL"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\SelEngineVolume\inc" /I "..\PSADomain\Inc" /I "..\InterfaceAndStruct\INC" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 htmlhelp.lib FillExcelDlld.lib DataFormatDlgd.lib version.lib SelEngineVolumed.lib InterfaceAndStructd.lib PSAPointElevationd.lib /nologo /subsystem:windows /debug /machine:I386 /out:"D:\shareDLL\AutoPHS7.0.exe" /implib:"Debug\AutoPHS7.0.lib" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "AutoPHS - Win32 Release"
# Name "AutoPHS - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Grid Control Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\dlgbackupundo.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgexportvolume.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgoutdescopt.cpp
# End Source File
# Begin Source File

SOURCE=.\gridctrl_src\gridcell.cpp
# End Source File
# Begin Source File

SOURCE=.\gridctrl_src\gridcellbase.cpp
# End Source File
# Begin Source File

SOURCE=.\gridcomboboxcell.cpp
# End Source File
# Begin Source File

SOURCE=.\gridctrl_src\gridctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\gridctrl_src\griddroptarget.cpp
# End Source File
# Begin Source File

SOURCE=.\gridctrl_src\inplaceedit.cpp
# End Source File
# Begin Source File

SOURCE=.\gridctrl_src\inplacelist.cpp
# End Source File
# Begin Source File

SOURCE=.\TBFormatDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\gridctrl_src\titletip.cpp
# End Source File
# End Group
# Begin Group "PipeStruct"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\PipeStruct\otherstruct.h
# End Source File
# Begin Source File

SOURCE=.\PipeStruct\outfunction.h
# End Source File
# Begin Source File

SOURCE=.\PipeStruct\pipeunitdata.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\_recordset.cpp
# End Source File
# Begin Source File

SOURCE=.\adodc.cpp
# End Source File
# Begin Source File

SOURCE=.\adorecordset.cpp
# End Source File
# Begin Source File

SOURCE=.\AutoPHS.cpp
# End Source File
# Begin Source File

SOURCE=.\autophs2002.rc
# End Source File
# Begin Source File

SOURCE=.\AutoPHSDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\AutoPHSView.cpp
# End Source File
# Begin Source File

SOURCE=.\basdirectory.cpp
# End Source File
# Begin Source File

SOURCE=.\BestrowJSJDH.cpp
# End Source File
# Begin Source File

SOURCE=.\bmpsewnd.cpp
# End Source File
# Begin Source File

SOURCE=.\browseforfolermodule.cpp
# End Source File
# Begin Source File

SOURCE=.\cadfontpag.cpp
# End Source File
# Begin Source File

SOURCE=.\cadpoint.cpp
# End Source File
# Begin Source File

SOURCE=.\CAESARIIToPHS.cpp
# End Source File
# Begin Source File

SOURCE=.\Calculate.cpp
# End Source File
# Begin Source File

SOURCE=.\calstructdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\calstructpag.cpp
# End Source File
# Begin Source File

SOURCE=.\ChangMaterilName.cpp
# End Source File
# Begin Source File

SOURCE=.\ClipCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\column.cpp
# End Source File
# Begin Source File

SOURCE=.\columns.cpp
# End Source File
# Begin Source File

SOURCE=.\ComboGenDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CommDBOper.cpp
# End Source File
# Begin Source File

SOURCE=.\Cphs.cpp
# End Source File
# Begin Source File

SOURCE=.\Cphs1.cpp
# End Source File
# Begin Source File

SOURCE=.\dataformatdisp.cpp
# End Source File
# Begin Source File

SOURCE=.\datagrid.cpp
# End Source File
# Begin Source File

SOURCE=.\DataGridEx.cpp
# End Source File
# Begin Source File

SOURCE=.\DataInput.cpp
# End Source File
# Begin Source File

SOURCE=.\dbcombobox.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgaddmaterial.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgcalstructres.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgDBOper.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgeditdb.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFavoriteTemplate.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFindAutoPSAFile.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgimglst.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgimgscroll.cpp
# End Source File
# Begin Source File

SOURCE=.\dlglistbox.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgmodifyzdjh.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgsarecord.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgspan.cpp
# End Source File
# Begin Source File

SOURCE=.\DrawZDJTable.cpp
# End Source File
# Begin Source File

SOURCE=.\DrawZDJTableID.cpp
# End Source File
# Begin Source File

SOURCE=.\edibacad.cpp
# End Source File
# Begin Source File

SOURCE=.\edibdb.cpp
# End Source File
# Begin Source File

SOURCE=.\edibgbl.cpp
# End Source File
# Begin Source File

SOURCE=.\excel9.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportPaint.cpp
# End Source File
# Begin Source File

SOURCE=.\field.cpp
# End Source File
# Begin Source File

SOURCE=.\fields.cpp
# End Source File
# Begin Source File

SOURCE=.\filedialogex.cpp
# End Source File
# Begin Source File

SOURCE=.\Files.cpp
# End Source File
# Begin Source File

SOURCE=.\font.cpp
# End Source File
# Begin Source File

SOURCE=.\Format.cpp
# End Source File
# Begin Source File

SOURCE=.\frmaddsarecord.cpp
# End Source File
# Begin Source File

SOURCE=.\frmbmpse.cpp
# End Source File
# Begin Source File

SOURCE=.\frmdatabasein.cpp
# End Source File
# Begin Source File

SOURCE=.\frmdataedit.cpp
# End Source File
# Begin Source File

SOURCE=.\frmdrawinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\frmfolderlocation.cpp
# End Source File
# Begin Source File

SOURCE=.\frmlistbox.cpp
# End Source File
# Begin Source File

SOURCE=.\frmphssamp.cpp
# End Source File
# Begin Source File

SOURCE=.\frmpreferences.cpp
# End Source File
# Begin Source File

SOURCE=.\frmselpdsv.cpp
# End Source File
# Begin Source File

SOURCE=.\frmselspecification.cpp
# End Source File
# Begin Source File

SOURCE=.\frmstatus.cpp
# End Source File
# Begin Source File

SOURCE=.\frmtk.cpp
# End Source File
# Begin Source File

SOURCE=.\frmtxsr.cpp
# End Source File
# Begin Source File

SOURCE=.\Guide.cpp
# End Source File
# Begin Source File

SOURCE=.\hyperlink.cpp
# End Source File
# Begin Source File

SOURCE=.\ImportAutoPHSData.cpp
# End Source File
# Begin Source File

SOURCE=.\ImportAutoPHSUserData.cpp
# End Source File
# Begin Source File

SOURCE=.\importfromxlsdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ImportPlantVision.cpp
# End Source File
# Begin Source File

SOURCE=.\inputbox.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\messageboxex.cpp
# End Source File
# Begin Source File

SOURCE=.\mimage.cpp
# End Source File
# Begin Source File

SOURCE=.\mobject.cpp
# End Source File
# Begin Source File

SOURCE=.\modencrypt.cpp
# End Source File
# Begin Source File

SOURCE=.\modPHScal.cpp
# End Source File
# Begin Source File

SOURCE=.\modregistry.cpp
# End Source File
# Begin Source File

SOURCE=.\modselpdsv.cpp
# End Source File
# Begin Source File

SOURCE=.\msmask.cpp
# End Source File
# Begin Source File

SOURCE=.\mybutton.cpp
# End Source File
# Begin Source File

SOURCE=.\mylistctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\mystatic.cpp
# End Source File
# Begin Source File

SOURCE=.\NewStyleDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OutputPDSFileTr.cpp
# End Source File
# Begin Source File

SOURCE=.\pagtxsroption.cpp
# End Source File
# Begin Source File

SOURCE=.\pdmscore.cpp
# End Source File
# Begin Source File

SOURCE=.\PHSApp.cpp
# End Source File
# Begin Source File

SOURCE=.\PHSBasepointSetting.cpp
# End Source File
# Begin Source File

SOURCE=.\PHSCaculateSetting.cpp
# End Source File
# Begin Source File

SOURCE=.\phsdata.cpp
# End Source File
# Begin Source File

SOURCE=.\PHSDrawSetting.cpp
# End Source File
# Begin Source File

SOURCE=.\PHSSetting.cpp
# End Source File
# Begin Source File

SOURCE=.\phsstart.cpp
# End Source File
# Begin Source File

SOURCE=.\picture.cpp
# End Source File
# Begin Source File

SOURCE=.\PlantVisionToPHS.cpp
# End Source File
# Begin Source File

SOURCE=.\precalpag.cpp
# End Source File
# Begin Source File

SOURCE=.\predrawpag.cpp
# End Source File
# Begin Source File

SOURCE=.\prepointpag.cpp
# End Source File
# Begin Source File

SOURCE=.\PreStylePag.cpp
# End Source File
# Begin Source File

SOURCE=.\properties.cpp
# End Source File
# Begin Source File

SOURCE=.\property.cpp
# End Source File
# Begin Source File

SOURCE=.\propertywnd.cpp
# End Source File
# Begin Source File

SOURCE=.\psdstart.cpp
# End Source File
# Begin Source File

SOURCE=.\qsort.cpp
# End Source File
# Begin Source File

SOURCE=.\readpsaresult.cpp

!IF  "$(CFG)" == "AutoPHS - Win32 Release"

# ADD CPP /w /W0 /O2

!ELSEIF  "$(CFG)" == "AutoPHS - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rstevent.cpp
# End Source File
# Begin Source File

SOURCE=.\selbookmarks.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectXYZdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\selpsaprojectnamedlg.cpp
# End Source File
# Begin Source File

SOURCE=.\seltemplate.cpp
# End Source File
# Begin Source File

SOURCE=.\selzdjhdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SHDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\splashthread.cpp
# End Source File
# Begin Source File

SOURCE=.\split.cpp
# End Source File
# Begin Source File

SOURCE=.\splits.cpp
# End Source File
# Begin Source File

SOURCE=.\statusbarthread.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\stddataformatsdisp.cpp
# End Source File
# Begin Source File

SOURCE=.\SumStuff.cpp
# End Source File
# Begin Source File

SOURCE=.\SysVar.cpp
# End Source File
# Begin Source File

SOURCE=.\Tab.cpp
# End Source File
# Begin Source File

SOURCE=.\textprogressctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\txsrpag1.cpp
# End Source File
# Begin Source File

SOURCE=.\txsrpag2.cpp
# End Source File
# Begin Source File

SOURCE=.\txsrpag3.cpp
# End Source File
# Begin Source File

SOURCE=.\txsrpagitem.cpp
# End Source File
# Begin Source File

SOURCE=.\UnitsTransformDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\user.cpp
# End Source File
# Begin Source File

SOURCE=.\variantarray.cpp
# End Source File
# Begin Source File

SOURCE=.\viewtxsr.cpp

!IF  "$(CFG)" == "AutoPHS - Win32 Release"

# ADD CPP /w /W0
# SUBTRACT CPP /WX /O<none>

!ELSEIF  "$(CFG)" == "AutoPHS - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\VirtualListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\xdialog.cpp
# End Source File
# Begin Source File

SOURCE=.\xedit.cpp
# End Source File
# Begin Source File

SOURCE=.\XHeaderCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\ximglistbox.cpp
# End Source File
# Begin Source File

SOURCE=.\xlistbox.cpp
# End Source File
# Begin Source File

SOURCE=.\xtabctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\nhwin32c.obj
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Grid Control Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\gridctrl_src\cellrange.h
# End Source File
# Begin Source File

SOURCE=.\dlgbackupundo.h
# End Source File
# Begin Source File

SOURCE=.\dlgcalstructres.h
# End Source File
# Begin Source File

SOURCE=.\dlgexportvolume.h
# End Source File
# Begin Source File

SOURCE=.\dlgoutdescopt.h
# End Source File
# Begin Source File

SOURCE=.\gridctrl_src\gridcell.h
# End Source File
# Begin Source File

SOURCE=.\gridctrl_src\gridcellbase.h
# End Source File
# Begin Source File

SOURCE=.\gridcomboboxcell.h
# End Source File
# Begin Source File

SOURCE=.\gridctrl_src\gridctrl.h
# End Source File
# Begin Source File

SOURCE=.\gridctrl_src\griddroptarget.h
# End Source File
# Begin Source File

SOURCE=.\gridctrl_src\inplaceedit.h
# End Source File
# Begin Source File

SOURCE=.\gridctrl_src\inplacelist.h
# End Source File
# Begin Source File

SOURCE=.\gridctrl_src\memdc.h
# End Source File
# Begin Source File

SOURCE=.\Tab.h
# End Source File
# Begin Source File

SOURCE=.\gridctrl_src\titletip.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\_recordset.h
# End Source File
# Begin Source File

SOURCE=.\adodc.h
# End Source File
# Begin Source File

SOURCE=.\adorecordset.h
# End Source File
# Begin Source File

SOURCE=.\AutoPHS.h
# End Source File
# Begin Source File

SOURCE=.\AutoPHS_h.h
# End Source File
# Begin Source File

SOURCE=.\AutoPHSDoc.h
# End Source File
# Begin Source File

SOURCE=.\AutoPHSView.h
# End Source File
# Begin Source File

SOURCE=.\basdirectory.h
# End Source File
# Begin Source File

SOURCE=.\BestrowJSJDH.h
# End Source File
# Begin Source File

SOURCE=.\bmpsewnd.h
# End Source File
# Begin Source File

SOURCE=.\browseforfolermodule.h
# End Source File
# Begin Source File

SOURCE=.\cadfontpag.h
# End Source File
# Begin Source File

SOURCE=.\cadpoint.h
# End Source File
# Begin Source File

SOURCE=.\CAESARIIToPHS.h
# End Source File
# Begin Source File

SOURCE=.\Calculate.h
# End Source File
# Begin Source File

SOURCE=.\calstructdlg.h
# End Source File
# Begin Source File

SOURCE=.\calstructpag.h
# End Source File
# Begin Source File

SOURCE=.\ChangMaterilName.h
# End Source File
# Begin Source File

SOURCE=.\ClipCtrl.h
# End Source File
# Begin Source File

SOURCE=.\column.h
# End Source File
# Begin Source File

SOURCE=.\columns.h
# End Source File
# Begin Source File

SOURCE=.\ComboGenDlg.h
# End Source File
# Begin Source File

SOURCE=.\CommDBOper.h
# End Source File
# Begin Source File

SOURCE=.\comstart.h
# End Source File
# Begin Source File

SOURCE=.\cphs.h
# End Source File
# Begin Source File

SOURCE=.\dataformatdisp.h
# End Source File
# Begin Source File

SOURCE=.\datagrid.h
# End Source File
# Begin Source File

SOURCE=.\DataGridEx.h
# End Source File
# Begin Source File

SOURCE=.\DataInput.h
# End Source File
# Begin Source File

SOURCE=.\dbcombobox.h
# End Source File
# Begin Source File

SOURCE=.\dlgaddmaterial.h
# End Source File
# Begin Source File

SOURCE=.\dlgdataformatdll.h
# End Source File
# Begin Source File

SOURCE=.\DlgDBOper.h
# End Source File
# Begin Source File

SOURCE=.\dlgeditdb.h
# End Source File
# Begin Source File

SOURCE=.\DlgFavoriteTemplate.h
# End Source File
# Begin Source File

SOURCE=.\DlgFindAutoPSAFile.h
# End Source File
# Begin Source File

SOURCE=.\dlgimglst.h
# End Source File
# Begin Source File

SOURCE=.\dlgimgscroll.h
# End Source File
# Begin Source File

SOURCE=.\dlglistbox.h
# End Source File
# Begin Source File

SOURCE=.\dlgmodifyzdjh.h
# End Source File
# Begin Source File

SOURCE=.\dlgsarecord.h
# End Source File
# Begin Source File

SOURCE=.\dlgspan.h
# End Source File
# Begin Source File

SOURCE=.\DrawZDJTable.h
# End Source File
# Begin Source File

SOURCE=.\DrawZDJTableID.h
# End Source File
# Begin Source File

SOURCE=.\edibacad.h
# End Source File
# Begin Source File

SOURCE=.\edibdb.h
# End Source File
# Begin Source File

SOURCE=.\edibgbl.h
# End Source File
# Begin Source File

SOURCE=.\excel9.h
# End Source File
# Begin Source File

SOURCE=.\ExportPaint.h
# End Source File
# Begin Source File

SOURCE=.\field.h
# End Source File
# Begin Source File

SOURCE=.\fields.h
# End Source File
# Begin Source File

SOURCE=.\filedialogex.h
# End Source File
# Begin Source File

SOURCE=.\Files.h
# End Source File
# Begin Source File

SOURCE=.\font.h
# End Source File
# Begin Source File

SOURCE=.\Format.h
# End Source File
# Begin Source File

SOURCE=.\frmaddsarecord.h
# End Source File
# Begin Source File

SOURCE=.\frmbmpse.h
# End Source File
# Begin Source File

SOURCE=.\frmdatabasein.h
# End Source File
# Begin Source File

SOURCE=.\frmdataedit.h
# End Source File
# Begin Source File

SOURCE=.\frmdrawinfo.h
# End Source File
# Begin Source File

SOURCE=.\frmfolderlocation.h
# End Source File
# Begin Source File

SOURCE=.\frmlistbox.h
# End Source File
# Begin Source File

SOURCE=.\frmphssamp.h
# End Source File
# Begin Source File

SOURCE=.\frmpreferences.h
# End Source File
# Begin Source File

SOURCE=.\frmselpdsv.h
# End Source File
# Begin Source File

SOURCE=.\frmselspecification.h
# End Source File
# Begin Source File

SOURCE=.\frmstatus.h
# End Source File
# Begin Source File

SOURCE=.\frmtk.h
# End Source File
# Begin Source File

SOURCE=.\frmtxsr.h
# End Source File
# Begin Source File

SOURCE=.\Guide.h
# End Source File
# Begin Source File

SOURCE=.\htmlhelp.h
# End Source File
# Begin Source File

SOURCE=.\hyperlink.h
# End Source File
# Begin Source File

SOURCE=.\ImportAutoPHSData.h
# End Source File
# Begin Source File

SOURCE=.\ImportAutoPHSUserData.h
# End Source File
# Begin Source File

SOURCE=.\importfromxlsdlg.h
# End Source File
# Begin Source File

SOURCE=.\ImportPlantVision.h
# End Source File
# Begin Source File

SOURCE=.\inputbox.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\messageboxex.h
# End Source File
# Begin Source File

SOURCE=.\mimage.h
# End Source File
# Begin Source File

SOURCE=.\mobject.h
# End Source File
# Begin Source File

SOURCE=.\modencrypt.h
# End Source File
# Begin Source File

SOURCE=.\modphscal.h
# End Source File
# Begin Source File

SOURCE=.\modregistry.h
# End Source File
# Begin Source File

SOURCE=.\modselpdsv.h
# End Source File
# Begin Source File

SOURCE=.\msmask.h
# End Source File
# Begin Source File

SOURCE=.\mybutton.h
# End Source File
# Begin Source File

SOURCE=.\mylistctrl.h
# End Source File
# Begin Source File

SOURCE=.\mystatic.h
# End Source File
# Begin Source File

SOURCE=.\netdog.h
# End Source File
# Begin Source File

SOURCE=.\NewStyleDlg.h
# End Source File
# Begin Source File

SOURCE=.\OutputPDSFileTr.h
# End Source File
# Begin Source File

SOURCE=.\pagtxsroption.h
# End Source File
# Begin Source File

SOURCE=.\pdmscore.h
# End Source File
# Begin Source File

SOURCE=.\PHSApp.h
# End Source File
# Begin Source File

SOURCE=.\PHSBasepointSetting.h
# End Source File
# Begin Source File

SOURCE=.\PHSCaculateSetting.h
# End Source File
# Begin Source File

SOURCE=.\phsdata.h
# End Source File
# Begin Source File

SOURCE=.\PHSDrawSetting.h
# End Source File
# Begin Source File

SOURCE=.\PHSSetting.h
# End Source File
# Begin Source File

SOURCE=.\phsstart.h
# End Source File
# Begin Source File

SOURCE=.\picture.h
# End Source File
# Begin Source File

SOURCE=.\PlantVisionToPHS.h
# End Source File
# Begin Source File

SOURCE=.\precalpag.h
# End Source File
# Begin Source File

SOURCE=.\predrawpag.h
# End Source File
# Begin Source File

SOURCE=.\prepointpag.h
# End Source File
# Begin Source File

SOURCE=.\PreStylePag.h
# End Source File
# Begin Source File

SOURCE=.\properties.h
# End Source File
# Begin Source File

SOURCE=.\property.h
# End Source File
# Begin Source File

SOURCE=.\propertywnd.h
# End Source File
# Begin Source File

SOURCE=.\PSAPointElevation.h
# End Source File
# Begin Source File

SOURCE=.\psdstart.h
# End Source File
# Begin Source File

SOURCE=.\qsort.h
# End Source File
# Begin Source File

SOURCE=.\readpsaresult.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\rstevent.h
# End Source File
# Begin Source File

SOURCE=.\selbookmarks.h
# End Source File
# Begin Source File

SOURCE=.\SelectXYZdlg.h
# End Source File
# Begin Source File

SOURCE=.\selpsaprojectnamedlg.h
# End Source File
# Begin Source File

SOURCE=.\seltemplate.h
# End Source File
# Begin Source File

SOURCE=.\selzdjhdlg.h
# End Source File
# Begin Source File

SOURCE=.\SHDlg.h
# End Source File
# Begin Source File

SOURCE=.\splashthread.h
# End Source File
# Begin Source File

SOURCE=.\split.h
# End Source File
# Begin Source File

SOURCE=.\splits.h
# End Source File
# Begin Source File

SOURCE=.\statusbarthread.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\stddataformatsdisp.h
# End Source File
# Begin Source File

SOURCE=.\SumStuff.h
# End Source File
# Begin Source File

SOURCE=.\SysVar.h
# End Source File
# Begin Source File

SOURCE=.\TBFormatDlg.h
# End Source File
# Begin Source File

SOURCE=.\textprogressctrl.h
# End Source File
# Begin Source File

SOURCE=.\txsrpag1.h
# End Source File
# Begin Source File

SOURCE=.\txsrpag2.h
# End Source File
# Begin Source File

SOURCE=.\txsrpag3.h
# End Source File
# Begin Source File

SOURCE=.\txsrpagitem.h
# End Source File
# Begin Source File

SOURCE=.\UnitsTransformDlg.h
# End Source File
# Begin Source File

SOURCE=.\user.h
# End Source File
# Begin Source File

SOURCE=.\variantarray.h
# End Source File
# Begin Source File

SOURCE=.\viewtxsr.h
# End Source File
# Begin Source File

SOURCE=.\VirtualListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\xdialog.h
# End Source File
# Begin Source File

SOURCE=.\xedit.h
# End Source File
# Begin Source File

SOURCE=.\XHeaderCtrl.h
# End Source File
# Begin Source File

SOURCE=.\ximglistbox.h
# End Source File
# Begin Source File

SOURCE=.\xlistbox.h
# End Source File
# Begin Source File

SOURCE=.\xtabctrl.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\AutoPHS.ico
# End Source File
# Begin Source File

SOURCE=.\res\AutoPHS.rc2
# End Source File
# Begin Source File

SOURCE=.\res\autophs2002.bmp
# End Source File
# Begin Source File

SOURCE=.\res\AutoPHSDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\bitmap10.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bom.ico
# End Source File
# Begin Source File

SOURCE=.\res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\res\cursor2.cur
# End Source File
# Begin Source File

SOURCE=.\res\figinpug1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gdox.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gdoz.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gdpx.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gdpz.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gsox.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gsoz.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gspx.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gspz.bmp
# End Source File
# Begin Source File

SOURCE=.\res\hpyelink.cur
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00002.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon3.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon4.ico
# End Source File
# Begin Source File

SOURCE=.\res\upxry.bmp
# End Source File
# Begin Source File

SOURCE=.\res\upxrz.bmp
# End Source File
# Begin Source File

SOURCE=.\res\upyrx.bmp
# End Source File
# Begin Source File

SOURCE=.\res\upyrz.bmp
# End Source File
# Begin Source File

SOURCE=.\res\upzrx.bmp
# End Source File
# Begin Source File

SOURCE=.\res\upzry.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\HTMLHELP.LIB
# End Source File
# End Target
# End Project
