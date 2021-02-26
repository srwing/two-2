# Microsoft Developer Studio Project File - Name="lib_glvu_objs" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=lib_glvu_objs - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "lib_glvu_objs.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lib_glvu_objs.mak" CFG="lib_glvu_objs - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lib_glvu_objs - Win32 Release" (based on\
 "Win32 (x86) Static Library")
!MESSAGE "lib_glvu_objs - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "lib_glvu_objs - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "Release\lib_glvu_objs"
# PROP Target_Dir ""
LINK32=link.exe -lib
RSC=rc.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\timer" /I "..\glutils\\" /I "..\glvu\\" /I "..\camera\\" /I "..\math\\" /I "..\math_objs\\" /I "..\object\\" /I "..\object\objmodel\\" /I "..\object\trimodel\\" /I "..\object\vrmltrimodel\\" /I "..\object\bfftrimodel\\" /I "..\object\misc_objs\\" /I "..\images\ppm\\" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Release/lib_glvu_objs.bsc"
LIB32=link.exe -lib
# ADD LIB32 /out:"..\lib\glvu_objs.lib"

!ELSEIF  "$(CFG)" == "lib_glvu_objs - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "Debug\lib_glvu_objs"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LINK32=link.exe -lib
RSC=rc.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\object\plymodel\\" /I "..\timer" /I "..\glutils\\" /I "..\glvu\\" /I "..\camera\\" /I "..\math\\" /I "..\math_objs\\" /I "..\object\\" /I "..\object\objmodel\\" /I "..\object\trimodel\\" /I "..\object\vrmltrimodel\\" /I "..\object\bfftrimodel\\" /I "..\object\misc_objs\\" /I "..\images\ppm\\" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Debug/lib_glvu_objs.bsc"
LIB32=link.exe -lib
# ADD LIB32 /out:"..\lib\glvu_objs_dbg.lib"

!ENDIF 

# Begin Target

# Name "lib_glvu_objs - Win32 Release"
# Name "lib_glvu_objs - Win32 Debug"
# Begin Group "glutils"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\glutils\glbuffers.cpp
# End Source File
# Begin Source File

SOURCE=..\glutils\glbuffers.hpp
# End Source File
# Begin Source File

SOURCE=..\glutils\glutils.cpp
# End Source File
# Begin Source File

SOURCE=..\glutils\glutils.hpp
# End Source File
# Begin Source File

SOURCE=..\glutils\textureobj.cpp
# End Source File
# Begin Source File

SOURCE=..\glutils\textureobj.hpp
# End Source File
# End Group
# Begin Group "object"

# PROP Default_Filter ""
# Begin Group "objmodel"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\object\objmodel\glm.cpp
# End Source File
# Begin Source File

SOURCE=..\object\objmodel\glm.hpp
# End Source File
# Begin Source File

SOURCE=..\object\objmodel\objmodel.cpp
# End Source File
# Begin Source File

SOURCE=..\object\objmodel\objmodel.hpp
# End Source File
# End Group
# Begin Group "trimodel"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\object\trimodel\trimodel.cpp
# End Source File
# Begin Source File

SOURCE=..\object\trimodel\trimodel.hpp
# End Source File
# Begin Source File

SOURCE=..\object\trimodel\trimodel_disp.cpp
# End Source File
# End Group
# Begin Group "vrmltrimodel"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\object\vrmltrimodel\vrmlreadtools.cpp
# End Source File
# Begin Source File

SOURCE=..\object\vrmltrimodel\vrmlreadtools.hpp
# End Source File
# Begin Source File

SOURCE=..\object\vrmltrimodel\vrmltri.cpp
# End Source File
# Begin Source File

SOURCE=..\object\vrmltrimodel\vrmltri.hpp
# End Source File
# Begin Source File

SOURCE=..\object\vrmltrimodel\vrmltrimodel.cpp
# End Source File
# Begin Source File

SOURCE=..\object\vrmltrimodel\vrmltrimodel.hpp
# End Source File
# Begin Source File

SOURCE=..\object\vrmltrimodel\vrmltrimodel_disp.cpp
# End Source File
# End Group
# Begin Group "nurbsmodel"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\object\nurbsmodel\nurbs.cpp
# End Source File
# Begin Source File

SOURCE=..\object\nurbsmodel\nurbs.hpp
# End Source File
# Begin Source File

SOURCE=..\object\nurbsmodel\nurbsmodel.cpp
# End Source File
# Begin Source File

SOURCE=..\object\nurbsmodel\nurbsmodel.hpp
# End Source File
# End Group
# Begin Group "bfftrimodel"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\object\bfftrimodel\bfftri.cpp
# End Source File
# Begin Source File

SOURCE=..\object\bfftrimodel\bfftri.hpp
# End Source File
# Begin Source File

SOURCE=..\object\bfftrimodel\bfftrimodel.cpp
# End Source File
# Begin Source File

SOURCE=..\object\bfftrimodel\bfftrimodel.hpp
# End Source File
# Begin Source File

SOURCE=..\object\bfftrimodel\bfftrimodel_disp.cpp
# End Source File
# Begin Source File

SOURCE=..\object\bfftrimodel\txtr.cpp
# End Source File
# Begin Source File

SOURCE=..\object\bfftrimodel\txtr.hpp
# End Source File
# End Group
# Begin Group "misc_objs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\object\misc_objects\boundingbox.cpp
# End Source File
# Begin Source File

SOURCE=..\object\misc_objects\boundingbox.hpp
# End Source File
# Begin Source File

SOURCE=..\object\misc_objects\groundplane.cpp
# End Source File
# Begin Source File

SOURCE=..\object\misc_objects\groundplane.hpp
# End Source File
# Begin Source File

SOURCE=..\object\misc_objects\wavy.cpp
# End Source File
# Begin Source File

SOURCE=..\object\misc_objects\wavy.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\object\object.cpp
# End Source File
# Begin Source File

SOURCE=..\object\object.hpp
# End Source File
# Begin Source File

SOURCE=..\object\objframe.cpp
# End Source File
# Begin Source File

SOURCE=..\object\objframe.hpp
# End Source File
# Begin Source File

SOURCE=..\object\objmngr.cpp
# End Source File
# Begin Source File

SOURCE=..\object\objmngr.hpp
# End Source File
# End Group
# Begin Group "images"

# PROP Default_Filter ""
# Begin Group "ppm"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\images\ppm\ppm.cpp
# End Source File
# Begin Source File

SOURCE=..\images\ppm\ppm.hpp
# End Source File
# End Group
# End Group
# End Target
# End Project
