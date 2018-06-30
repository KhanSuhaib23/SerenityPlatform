@echo off


set CommonCompilerFlags=/nologo /MD /EHsc /Od /Zi /W4 /wd4100 /wd4099 /wd4204 /wd4996 /we4702
set Defines=/DSERENITY_MAX_WINDOW=5 /D_CRT_SECURE_NO_WARNINGS
set FilesToCompile=../code/Main.cpp
set LinkLibraries=opengl32.lib kernel32.lib user32.lib gdi32.lib shell32.lib ole32.lib

pushd helper

cl %CommonCompilerFlags% PerformanceParser.c

PerformanceParser ..\code\ ..\parsed_code\

popd

IF EXIST build GOTO DIRECTORY_EXISTS
mkdir build

:DIRECTORY_EXISTS
pushd build
del /Q *.*


cl %CommonCompilerFlags% %Defines% %FilesToCompile% /link %AdditionalLibDirectories% %LinkLibraries%
popd
