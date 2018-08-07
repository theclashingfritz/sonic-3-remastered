@echo off
echo Performing Make.
set PATH=C:\MinGW\bin;
if "%~1"=="h" del /q "out\CPP"
mingw32-make.exe -f MakefileWIN_FINAL > log.txt 2>&1
if %ERRORLEVEL% == 0 "Sonic 3.exe"
echo Finished Make.
pause
