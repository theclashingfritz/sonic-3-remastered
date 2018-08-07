@echo off
set PATH=C:\MinGW\bin;
gdb.exe -batch -x gdbCMD --args "Sonic 3.exe" %*
pause