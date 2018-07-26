@ECHO OFF
SETLOCAL EnableDelayedExpansion
for /F "tokens=1,2 delims=#" %%a in ('"prompt #$H#$E# & echo on & for %%b in (1) do     rem"') do (
  set "DEL=%%a"
)
CALL :colorEcho 0C "Folder Decompressor"
ECHO ~
ECHO ~
SET arg=%~dp0
IF "%~1"=="" (
   GOTO is_directory
) ELSE (
   SET arg=%~1\
)
FOR %%i IN ("%arg%") DO IF EXIST %%~si\NUL GOTO is_directory
GOTO is_file

:is_directory
CALL :colorEcho 0A "Current Directory"
ECHO : %arg% 
CALL "C:/Users/Justin/Dropbox/sonic3/tools/DecompressKosinski.bat" "%arg%/Blocks"
CALL "C:/Users/Justin/Dropbox/sonic3/tools/DecompressKosinski.bat" "%arg%/Chunks"
CALL "C:/Users/Justin/Dropbox/sonic3/tools/DecompressKosinskiM.bat" "%arg%/KosinskiM Art"
CALL "C:/Users/Justin/Dropbox/sonic3/tools/DecompressNemesis.bat" "%arg%/Nemesis Art"
CALL "C:/Users/Justin/Dropbox/sonic3/tools/DecompressKosinskiM.bat" "%arg%/Tiles"
GOTO end

:is_file
SET arg=%~1
CALL :colorEcho 0A "Current File"
ECHO : %arg%
FOR %%j IN ("%arg%") DO (
   "C:/Program Files/KensSharp/KensSharp.exe" --decompress=nem "%%j" -s
)
GOTO end

:colorEcho
echo off
<nul set /p ".=%DEL%" > "%~2"
findstr /v /a:%1 /R "^$" "%~2" nul
del "%~2" > nul 2>&1i
GOTO meme

:end
pause

:meme