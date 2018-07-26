@echo off
set PATH=C:\MinGW\bin;
if "%~1"=="h" del /q "out\CPP"
mingw32-make.exe -f MakefileWIN > log.txt
if %ERRORLEVEL% == 0 "Sonic 3.exe"
pause
