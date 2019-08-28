# Microsoft Developer Studio Project File - Name="DRegZ" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=DRegZ - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DRegZ.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DRegZ.mak" CFG="DRegZ - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DRegZ - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "DRegZ - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DRegZ - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /I../../src/QRegZ /I../../src/DRegZ /I../../src/DRegZ/SDDecoder /I../../src/common /c
# ADD BASE RSC /l 0x410 /d "NDEBUG"
# ADD RSC /l 0x410 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "DRegZ - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FD /GZ /I../../src/QRegZ /I../../src/DRegZ /I../../src/DRegZ/SDDecoder /I../../src/common /c
# ADD BASE RSC /l 0x410 /d "_DEBUG"
# ADD RSC /l 0x410 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "DRegZ - Win32 Release"
# Name "DRegZ - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\common\arc4.c
# End Source File
# Begin Source File

SOURCE=..\..\src\DRegZ\Blowfish.c
# End Source File
# Begin Source File

SOURCE=..\..\src\DRegZ\crc32.c
# End Source File
# Begin Source File

SOURCE=..\..\src\DRegZ\dzmain.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\DRegZ\FastKey.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\ffpoly.c
# End Source File
# Begin Source File

SOURCE=..\..\src\common\fred103.c
# End Source File
# Begin Source File

SOURCE=..\..\src\DRegZ\Globals.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\karatsuba.c
# End Source File
# Begin Source File

SOURCE=..\..\src\DRegZ\KeyPair.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\DRegZ\KeyRing.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\lintra.c
# End Source File
# Begin Source File

SOURCE=..\..\src\DRegZ\LNMatrix.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\DRegZ\LNPoly.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\DRegZ\mstub.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\osdepn.c
# End Source File
# Begin Source File

SOURCE=..\..\src\DRegZ\RandPool.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\DRegZ\Remapper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\DRegZ\SDCluster.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\DRegZ\SDMatrix.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\DRegZ\SDVect.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\DRegZ\SHA.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\DRegZ\StringGen.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\userio.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\src\common\arc4.h
# End Source File
# Begin Source File

SOURCE=..\..\src\DRegZ\Blowfish.h
# End Source File
# Begin Source File

SOURCE=..\..\src\DRegZ\callbacks.h
# End Source File
# Begin Source File

SOURCE=..\..\src\DRegZ\crc32.h
# End Source File
# Begin Source File

SOURCE=..\..\src\DRegZ\FastKey.h
# End Source File
# Begin Source File

SOURCE=..\..\src\common\ffpoly.h
# End Source File
# Begin Source File

SOURCE=..\..\src\common\fred103.h
# End Source File
# Begin Source File

SOURCE=..\..\src\DRegZ\Globals.h
# End Source File
# Begin Source File

SOURCE=..\..\src\common\karatsuba.h
# End Source File
# Begin Source File

SOURCE=..\..\src\DRegZ\KeyPair.h
# End Source File
# Begin Source File

SOURCE=..\..\src\DRegZ\KeyRing.h
# End Source File
# Begin Source File

SOURCE=..\..\src\common\lintra.h
# End Source File
# Begin Source File

SOURCE=..\..\src\DRegZ\LNMatrix.h
# End Source File
# Begin Source File

SOURCE=..\..\src\DRegZ\LNPoly.h
# End Source File
# Begin Source File

SOURCE=..\..\src\DRegZ\mstub.h
# End Source File
# Begin Source File

SOURCE=..\..\src\common\mytypes.h
# End Source File
# Begin Source File

SOURCE=..\..\src\common\osdepn.h
# End Source File
# Begin Source File

SOURCE=..\..\src\DRegZ\RandPool.h
# End Source File
# Begin Source File

SOURCE=..\..\src\DRegZ\Remapper.h
# End Source File
# Begin Source File

SOURCE=..\..\src\DRegZ\SDCluster.h
# End Source File
# Begin Source File

SOURCE=..\..\src\DRegZ\SDMatrix.h
# End Source File
# Begin Source File

SOURCE=..\..\src\DRegZ\SDVect.h
# End Source File
# Begin Source File

SOURCE=..\..\src\DRegZ\SHA.h
# End Source File
# Begin Source File

SOURCE=..\..\src\DRegZ\StringGen.h
# End Source File
# Begin Source File

SOURCE=..\..\src\common\userio.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
