# Microsoft Developer Studio Project File - Name="lib_glvu" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=lib_glvu - Win32 Debug Single Threaded
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "lib_glvu.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lib_glvu.mak" CFG="lib_glvu - Win32 Debug Single Threaded"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lib_glvu - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "lib_glvu - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "lib_glvu - Win32 Debug Single Threaded" (based on "Win32 (x86) Static Library")
!MESSAGE "lib_glvu - Win32 Release Single Threaded" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "lib_glvu - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "Release\lib_glvu"
# PROP Target_Dir ""
LINK32=link.exe -lib
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".." /I "..\glvu" /I "..\math" /I "..\camera" /I "..\math_objs" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Release/lib_glvu.bsc"
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\glvu.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Installing header files...
PostBuild_Cmds=call ..\MSVC6\insthdrs /e .hpp /i ..\include /d ..\camera ..\glvu ..\math ..\math_objs ..\timer	call ..\MSVC6\insthdrs /e .h /i ..\include /d ..\glvu
# End Special Build Tool

!ELSEIF  "$(CFG)" == "lib_glvu - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "Debug\lib_glvu"
# PROP Target_Dir ""
LINK32=link.exe -lib
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /I ".." /I "..\glvu" /I "..\math" /I "..\camera" /I "..\math_objs" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Debug/lib_glvu.bsc"
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\glvu_dbg.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Installing header files...
PostBuild_Cmds=call ..\MSVC6\insthdrs /e .hpp /i ..\include /d ..\camera ..\glvu ..\math ..\math_objs ..\timer	call ..\MSVC6\insthdrs /e .h /i ..\include /d ..\glvu
# End Special Build Tool

!ELSEIF  "$(CFG)" == "lib_glvu - Win32 Debug Single Threaded"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "lib_glvu___Win32_Debug_Single_Threaded"
# PROP BASE Intermediate_Dir "lib_glvu___Win32_Debug_Single_Threaded"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_Single_Threaded\lib_glvu"
# PROP Intermediate_Dir "Debug_Single_Threaded\lib_glvu"
# PROP Target_Dir ""
LINK32=link.exe -lib
# ADD BASE CPP /nologo /MDd /W3 /GX /Z7 /Od /I ".." /I "..\glvu" /I "..\math" /I "..\camera" /I "..\math_objs" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Z7 /Od /I ".." /I "..\glvu" /I "..\math" /I "..\camera" /I "..\math_objs" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"Debug/lib_glvu.bsc"
# ADD BSC32 /nologo /o"Debug/lib_glvu.bsc"
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\glvu_dbg.lib"
# ADD LIB32 /nologo /out:"..\lib\glvu_dbg_st.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Installing header files...
PostBuild_Cmds=call ..\MSVC6\insthdrs /e .hpp /i ..\include /d ..\camera ..\glvu ..\math ..\math_objs ..\timer	call ..\MSVC6\insthdrs /e .h /i ..\include /d ..\glvu
# End Special Build Tool

!ELSEIF  "$(CFG)" == "lib_glvu - Win32 Release Single Threaded"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "lib_glvu___Win32_Release_Single_Threaded"
# PROP BASE Intermediate_Dir "lib_glvu___Win32_Release_Single_Threaded"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_Single_Threaded\lib_glvu"
# PROP Intermediate_Dir "Release_Single_Threaded\lib_glvu"
# PROP Target_Dir ""
LINK32=link.exe -lib
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I ".." /I "..\glvu" /I "..\math" /I "..\camera" /I "..\math_objs" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I ".." /I "..\glvu" /I "..\math" /I "..\camera" /I "..\math_objs" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"Release/lib_glvu.bsc"
# ADD BSC32 /nologo /o"Release/lib_glvu.bsc"
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\glvu.lib"
# ADD LIB32 /nologo /out:"..\lib\glvu_st.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Installing header files...
PostBuild_Cmds=call ..\MSVC6\insthdrs /e .hpp /i ..\include /d ..\camera ..\glvu ..\math ..\math_objs ..\timer	call ..\MSVC6\insthdrs /e .h /i ..\include /d ..\glvu
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "lib_glvu - Win32 Release"
# Name "lib_glvu - Win32 Debug"
# Name "lib_glvu - Win32 Debug Single Threaded"
# Name "lib_glvu - Win32 Release Single Threaded"
# Begin Group "camera"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\camera\camdisplay.cpp
# End Source File
# Begin Source File

SOURCE=..\camera\camera.cpp
# End Source File
# Begin Source File

SOURCE=..\camera\camera.hpp
# End Source File
# Begin Source File

SOURCE=..\camera\camutils.cpp
# End Source File
# Begin Source File

SOURCE=..\camera\camutils.hpp
# End Source File
# End Group
# Begin Group "glvu"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\glvu\glvu.cpp
# End Source File
# Begin Source File

SOURCE=..\glvu\glvu.hpp
# End Source File
# Begin Source File

SOURCE=..\glvu\glvu_camview.cpp
# End Source File
# Begin Source File

SOURCE=..\glvu\glvu_keyinput.cpp
# End Source File
# Begin Source File

SOURCE=..\glvu\glvu_menu.cpp
# End Source File
# Begin Source File

SOURCE=..\glvu\glvu_mouse.cpp
# End Source File
# Begin Source File

SOURCE=..\glvu\snapshot.cpp
# End Source File
# Begin Source File

SOURCE=..\glvu\snapshot.hpp
# End Source File
# Begin Source File

SOURCE=..\glvu\text.cpp
# End Source File
# Begin Source File

SOURCE=..\glvu\text.hpp
# End Source File
# Begin Source File

SOURCE=..\glvu\trackball.c
# End Source File
# Begin Source File

SOURCE=..\glvu\trackball.h
# End Source File
# End Group
# Begin Group "math"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\math\mat16fv.cpp
# End Source File
# Begin Source File

SOURCE=..\math\mat16fv.hpp
# End Source File
# Begin Source File

SOURCE=..\math\mat33.hpp
# End Source File
# Begin Source File

SOURCE=..\math\mat33impl.hpp
# End Source File
# Begin Source File

SOURCE=..\math\mat44.hpp
# End Source File
# Begin Source File

SOURCE=..\math\mat44impl.hpp
# End Source File
# Begin Source File

SOURCE=..\math\quat.hpp
# End Source File
# Begin Source File

SOURCE=..\math\quatimpl.hpp
# End Source File
# Begin Source File

SOURCE=..\math\vec2f.hpp
# End Source File
# Begin Source File

SOURCE=..\math\vec3f.hpp
# End Source File
# Begin Source File

SOURCE=..\math\vec3fv.cpp
# End Source File
# Begin Source File

SOURCE=..\math\vec3fv.hpp
# End Source File
# Begin Source File

SOURCE=..\math\vec4f.hpp
# End Source File
# End Group
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
# Begin Group "timer"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\timer\stopwatch.cpp
# End Source File
# Begin Source File

SOURCE=..\timer\stopwatch.hpp
# End Source File
# End Group
# End Target
# End Project
