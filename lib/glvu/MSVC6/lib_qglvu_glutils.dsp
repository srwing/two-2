# Microsoft Developer Studio Project File - Name="lib_qglvu_glutils" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=lib_qglvu_glutils - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "lib_qglvu_glutils.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lib_qglvu_glutils.mak" CFG="lib_qglvu_glutils - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lib_qglvu_glutils - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "lib_qglvu_glutils - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "lib_qglvu_glutils - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "lib_qglvu_glutils___Win32_Release"
# PROP BASE Intermediate_Dir "lib_qglvu_glutils___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "Release\lib_qglvu_glutils"
# PROP Target_Dir ""
LINK32=link.exe -lib
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "../../math" /I "../math" /I "../include" /I "$(NVSDK_DIR)/include/glh" /I "$(QTDIR)\include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "QT_DLL" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\qglvu_glutils.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Installing header files...
PostBuild_Cmds=call ..\MSVC6\insthdrs /e .hpp /i ..\include /d ..\qglvu\qglutils
# End Special Build Tool

!ELSEIF  "$(CFG)" == "lib_qglvu_glutils - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "lib_qglvu_glutils___Win32_Debug"
# PROP BASE Intermediate_Dir "lib_qglvu_glutils___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "Debug\lib_qglvu_glutils"
# PROP Target_Dir ""
LINK32=link.exe -lib
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../include" /I "$(NVSDK_DIR)/include/glh" /I "$(QTDIR)\include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "QT_DLL" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\qglvu_glutils_dbg.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Installing header files...
PostBuild_Cmds=call ..\MSVC6\insthdrs /e .h /i ..\include /d ..\qglvu\qglutils
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "lib_qglvu_glutils - Win32 Release"
# Name "lib_qglvu_glutils - Win32 Debug"
# Begin Source File

SOURCE=..\qglvu\qglutils\qglvupbuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\qglvu\qglutils\qglvupbuffer.h
# End Source File
# Begin Source File

SOURCE=..\qglvu\qglutils\qglvushapes.cpp
# End Source File
# Begin Source File

SOURCE=..\qglvu\qglutils\qglvushapes.h
# End Source File
# Begin Source File

SOURCE=..\qglvu\qglutils\qglvuTeapot.cpp
# End Source File
# Begin Source File

SOURCE=..\qglvu\qglutils\qglvuteapot.h
# End Source File
# Begin Source File

SOURCE=..\qglvu\qglutils\qglvutexturemanager.cpp
# End Source File
# Begin Source File

SOURCE=..\qglvu\qglutils\qglvutexturemanager.h
# End Source File
# Begin Source File

SOURCE=..\qglvu\qglutils\singleton.h
# End Source File
# End Target
# End Project
