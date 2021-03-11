@echo off

SET SwitchFlags=/O2 /D INTERNAL=0
SET SourceFile=%CD%\code\main.cpp
SET CompilerFlags= /nologo /fp:fast /fp:except- /EHa- /GR- /GS- /Gs9999999 /Fe:"VIMG.exe"
SET LinkerFlags=  /INCREMENTAL:NO /SUBSYSTEM:WINDOWS,5.02
SET Libraries=user32.lib gdi32.lib shell32.lib kernel32.lib winmm.lib ws2_32.lib
SET MAINLOCATION=%CD%


rc.exe "%MAINLOCATION%\code\resources.rc" > nul
MKDIR %MAINLOCATION%\build
PUSHD %MAINLOCATION%\build
CD
cl.exe %SwitchFlags% %CompilerFlags% %SourceFile% "%MAINLOCATION%\code\resources.res" -I %MAINLOCATION%\include\stb /link %LinkerFlags% %Libraries% 
POPD

