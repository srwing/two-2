# Microsoft Developer Studio Project File - Name="lib_glvu_lightcam" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=lib_glvu_lightcam - Win32 Debug Single Threaded
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "lib_glvu_lightcam.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lib_glvu_lightcam.mak" CFG="lib_glvu_lightcam - Win32 Debug Single Threaded"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lib_glvu_lightcam - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "lib_glvu_lightcam - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "lib_glvu_lightcam - Win32 Release Single Threaded" (based on "Win32 (x86) Static Library")
!MESSAGE "lib_glvu_lightcam - Win32 Debug Single Threaded" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "lib_glvu_lightcam - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "Release/lib_glvu_lightcam"
# PROP Target_Dir ""
LINK32=link.exe -lib
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\glutils\\" /I "..\camera\\" /I "..\math\\" /I "..\math_objs\\" /I "..\object\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Release/lib_glvu_lightcam.bsc"
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\glvu_lightcam.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Installing header files...
PostBuild_Cmds=call ..\MSVC6\insthdrs /i ..\include /e .hpp /d ..\lightcam
# End Special Build Tool

!ELSEIF  "$(CFG)" == "lib_glvu_lightcam - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "lib_glvu"
# PROP BASE Intermediate_Dir "lib_glvu"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "Debug/lib_glvu_lightcam"
# PROP Target_Dir ""
LINK32=link.exe -lib
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /I "..\glutils\\" /I "..\camera\\" /I "..\math\\" /I "..\math_objs\\" /I "..\object\\" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Debug/lib_glvu_lightcam.bsc"
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\glvu_lightcam_dbg.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Installing header files...
PostBuild_Cmds=call ..\MSVC6\insthdrs /i ..\include /e .hpp /d ..\lightcam
# End Special Build Tool

!ELSEIF  "$(CFG)" == "lib_glvu_lightcam - Win32 Release Single Threaded"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "lib_glvu_lightcam___Win32_Release_Single_Threaded"
# PROP BASE Intermediate_Dir "lib_glvu_lightcam___Win32_Release_Single_Threaded"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_Single_Threaded\lib_glvu_lightcam"
# PROP Intermediate_Dir "Release_Single_Threaded\lib_glvu_lightcam"
# PROP Target_Dir ""
LINK32=link.exe -lib
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\glutils\\" /I "..\camera\\" /I "..\math\\" /I "..\math_objs\\" /I "..\object\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\glutils\\" /I "..\camera\\" /I "..\math\\" /I "..\math_objs\\" /I "..\object\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"Release/lib_glvu_lightcam.bsc"
# ADD BSC32 /nologo /o"Release/lib_glvu_lightcam.bsc"
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\glvu_lightcam.lib"
# ADD LIB32 /nologo /out:"..\lib\glvu_lightcam_st.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Installing header files...
PostBuild_Cmds=call ..\MSVC6\insthdrs /i ..\include /e .hpp /d ..\lightcam
# End Special Build Tool

!ELSEIF  "$(CFG)" == "lib_glvu_lightcam - Win32 Debug Single Threaded"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "lib_glvu_lightcam___Win32_Debug_Single_Threaded"
# PROP BASE Intermediate_Dir "lib_glvu_lightcam___Win32_Debug_Single_Threaded"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_Single_Threaded\lib_glvu_lightcam"
# PROP Intermediate_Dir "Debug_Single_Threaded\lib_glvu_lightcam"
# PROP Target_Dir ""
LINK32=link.exe -lib
# ADD BASE CPP /nologo /MDd /W3 /GX /Z7 /Od /I "..\glutils\\" /I "..\camera\\" /I "..\math\\" /I "..\math_objs\\" /I "..\object\\" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Z7 /Od /I "..\glutils\\" /I "..\camera\\" /I "..\math\\" /I "..\math_objs\\" /I "..\object\\" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"Debug/lib_glvu_lightcam.bsc"
# ADD BSC32 /nologo /o"Debug/lib_glvu_lightcam.bsc"
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\glvu_lightcam_dbg.lib"
# ADD LIB32 /nologo /out:"..\lib\glvu_lightcam_dbg_st.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Installing header files...
PostBuild_Cmds=call ..\MSVC6\insthdrs /i ..\include /e .hpp /d ..\lightcam
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "lib_glvu_lightcam - Win32 Release"
# Name "lib_glvu_lightcam - Win32 Debug"
# Name "lib_glvu_lightcam - Win32 Release Single Threaded"
# Name "lib_glvu_lightcam - Win32 Debug Single Threaded"
# Begin Group "lightcam"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\lightcam\lightcam.cpp
# End Source File
# Begin Source File

SOURCE=..\lightcam\lightcam.hpp
# End Source File
# Begin Source File

SOURCE=..\lightcam\lightcamobj.cpp
# End Source File
# Begin Source File

SOURCE=..\lightcam\lightcamobj.hpp
# End Source File
# Begin Source File

SOURCE=..\lightcam\lightcamprojector.cpp
# End Source File
# Begin Source File

SOURCE=..\lightcam\lightcamprojector.hpp
# End Source File
# End Group
# End Target
# End Project
