@ECHO OFF
SETLOCAL EnableDelayedExpansion
for /F "tokens=1,2 delims=#" %%a in ('"prompt #$H#$E# & echo on & for %%b in (1) do     rem"') do (
  set "DEL=%%a"
)
CALL :colorEcho 0C "Kosinski Decompressor"
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
FOR %%j IN ("%arg%/*.bin") DO (
   "C:/Program Files/KensSharp/KensSharp.exe" --decompress=kos "%%j" -s
)
GOTO end

:is_file
SET arg=%~1
CALL :colorEcho 0A "Current File"
ECHO : %arg%
FOR %%j IN ("%arg%") DO (
   "C:/Program Files/KensSharp/KensSharp.exe" --decompress=kos "%%j" -s
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