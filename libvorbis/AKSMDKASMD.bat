@echo off
set PATH=C:\MinGW\bin;%PATH%
configure --prefix=/mingw --target=x86_64-mingw64
mingw32-make.exe -f Makefile.in
pause