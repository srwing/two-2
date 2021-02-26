# Microsoft Developer Studio Project File - Name="lib_qglvu" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=lib_qglvu - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "lib_qglvu.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lib_qglvu.mak" CFG="lib_qglvu - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lib_qglvu - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "lib_qglvu - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "lib_qglvu - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "lib_qglvu___Win32_Release"
# PROP BASE Intermediate_Dir "lib_qglvu___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "Release\lib_qglvu"
# PROP Target_Dir ""
LINK32=link.exe -lib
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "..\math" /I "..\math_objs" /I "..\camera" /I "..\glvu" /I "$(QTDIR)\include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "QT_DLL" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Release/lib_qglvu.bsc"
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\qglvu.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=call ..\MSVC6\insthdrs /e .hpp /i ..\include /d ..\camera ..\math ..\math_objs ..\timer	call ..\MSVC6\insthdrs /e .h /i ..\include /d ..\glvu ..\qglvu
# End Special Build Tool

!ELSEIF  "$(CFG)" == "lib_qglvu - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "lib_qglvu___Win32_Debug"
# PROP BASE Intermediate_Dir "lib_qglvu___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "Debug\lib_qglvu"
# PROP Target_Dir ""
LINK32=link.exe -lib
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\math" /I "..\math_objs" /I "..\camera" /I "..\glvu" /I "$(QTDIR)\include" /D "_DEBUG" /D "QT_DLL" /D "WIN32" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Debug/lib_qglvu.bsc"
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\qglvu_dbg.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=call ..\MSVC6\insthdrs /e .hpp /i ..\include /d ..\camera ..\math ..\math_objs ..\timer	call ..\MSVC6\insthdrs /e .h /i ..\include /d ..\glvu ..\qglvu
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "lib_qglvu - Win32 Release"
# Name "lib_qglvu - Win32 Debug"
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
# Begin Group "qglvu"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\qglvu\moc_qglvuwidget.cpp
# End Source File
# Begin Source File

SOURCE=..\qglvu\moc_qglvuwindow.cpp
# End Source File
# Begin Source File

SOURCE=..\qglvu\qglvucontext.h
# End Source File
# Begin Source File

SOURCE=..\qglvu\qglvupixmap.h
# End Source File
# Begin Source File

SOURCE=..\qglvu\qglvuwidget.cpp
# End Source File
# Begin Source File

SOURCE=..\qglvu\qglvuwidget.h

!IF  "$(CFG)" == "lib_qglvu - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\src\glvu\qglvu
InputPath=..\qglvu\qglvuwidget.h
InputName=qglvuwidget

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "lib_qglvu - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).hpp ...
InputDir=\src\glvu\qglvu
InputPath=..\qglvu\qglvuwidget.h
InputName=qglvuwidget

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\qglvu\qglvuwindow.cpp
# End Source File
# Begin Source File

SOURCE=..\qglvu\qglvuwindow.h

!IF  "$(CFG)" == "lib_qglvu - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\src\glvu\qglvu
InputPath=..\qglvu\qglvuwindow.h
InputName=qglvuwindow

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "lib_qglvu - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\src\glvu\qglvu
InputPath=..\qglvu\qglvuwindow.h
InputName=qglvuwindow

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\glvu\trackball.c
# End Source File
# Begin Source File

SOURCE=..\glvu\trackball.h
# End Source File
# End Group
# End Target
# End Project
