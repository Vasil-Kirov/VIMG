@echo off
CLS

REM SET SwitchFlags=/O2 /D INTERNAL=0
SET SourceFile=%CD%\main.cpp
SET CompilerFlags= /nologo /fp:fast /fp:except- /EHa- /GR- /GS- /Gs9999999 /Fe:"VIMG.exe" /O2
SET LinkerFlags=  /INCREMENTAL:NO /SUBSYSTEM:WINDOWS,5.02
SET Libraries=user32.lib gdi32.lib shell32.lib kernel32.lib winmm.lib ws2_32.lib

PUSHD ..\VIMG
rc.exe "resources.rc" > nul
REM %SwitchFlags%
cl.exe -Zi %CompilerFlags% %SourceFile% "resources.res" -I ..\include /link %LinkerFlags% %Libraries% 
POPD