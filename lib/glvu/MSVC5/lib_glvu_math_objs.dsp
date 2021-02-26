# Microsoft Developer Studio Project File - Name="lib_glvu_math_objs" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=lib_glvu_math_objs - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "lib_glvu_math_objs.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lib_glvu_math_objs.mak"\
 CFG="lib_glvu_math_objs - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lib_glvu_math_objs - Win32 Release" (based on\
 "Win32 (x86) Static Library")
!MESSAGE "lib_glvu_math_objs - Win32 Debug" (based on\
 "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "lib_glvu_math_objs - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "lib_glvu"
# PROP BASE Intermediate_Dir "lib_glvu"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "Release\lib_glvu_math_objs"
# PROP Target_Dir ""
RSC=rc.exe
LINK32=link.exe -lib
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\glvu_math_objs.lib"

!ELSEIF  "$(CFG)" == "lib_glvu_math_objs - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "lib_glv0"
# PROP BASE Intermediate_Dir "lib_glv0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "Debug\lib_glvu_math_objs"
# PROP Target_Dir ""
RSC=rc.exe
LINK32=link.exe -lib
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\glvu_math_objs_dbg.lib"

!ENDIF 

# Begin Target

# Name "lib_glvu_math_objs - Win32 Release"
# Name "lib_glvu_math_objs - Win32 Debug"
# Begin Group "math_objs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\math_objs\minmaxbox.cpp
# End Source File
# Begin Source File

SOURCE=..\math_objs\minmaxbox.hpp
# End Source File
# Begin Source File

SOURCE=..\math_objs\plane.cpp
# End Source File
# Begin Source File

SOURCE=..\math_objs\plane.hpp
# End Source File
# Begin Source File

SOURCE=..\math_objs\tri.cpp
# End Source File
# Begin Source File

SOURCE=..\math_objs\tri.hpp
# End Source File
# End Group
# End Target
# End Project
