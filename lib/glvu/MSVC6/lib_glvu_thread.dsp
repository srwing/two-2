# Microsoft Developer Studio Project File - Name="lib_glvu_thread" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=lib_glvu_thread - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "lib_glvu_thread.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lib_glvu_thread.mak" CFG="lib_glvu_thread - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lib_glvu_thread - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "lib_glvu_thread - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "lib_glvu_thread - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "lib_glvu_thread___Win32_Release"
# PROP BASE Intermediate_Dir "lib_glvu_thread___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "Release\lib_glvu_thread"
# PROP Target_Dir ""
LINK32=link.exe -lib
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\images\ppm" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\glvu_thread.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Installing header files...
PostBuild_Cmds=call ..\MSVC6\insthdrs /i ..\include /e .hpp /d ..\thread
# End Special Build Tool

!ELSEIF  "$(CFG)" == "lib_glvu_thread - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "lib_glvu_thread___Win32_Debug"
# PROP BASE Intermediate_Dir "lib_glvu_thread___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "Debug\lib_glvu_thread"
# PROP Target_Dir ""
LINK32=link.exe -lib
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\images\ppm" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\glvu_thread_dbg.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Installing header files...
PostBuild_Cmds=call ..\MSVC6\insthdrs /i ..\include /e .hpp /d ..\thread
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "lib_glvu_thread - Win32 Release"
# Name "lib_glvu_thread - Win32 Debug"
# Begin Group "thread"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\thread\thread.cpp
# End Source File
# Begin Source File

SOURCE=..\thread\thread.hpp
# End Source File
# Begin Source File

SOURCE=..\thread\threadutil.cpp
# End Source File
# Begin Source File

SOURCE=..\thread\threadutil.h
# End Source File
# End Group
# End Target
# End Project
