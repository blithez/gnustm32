@echo off
%~d0
cd %~dp0

if not exist .\out (
    mkdir out
)

make ARCH=arm OS=win %1 -j4

if "%errorlevel%" == "0" (
    echo build successfully
    rem ftp -n -s:"ftppushscript.txt"
) else (
    echo build failed
)
:pause