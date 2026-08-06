@echo off

rem --- 1) vswhere: documented fixed location, handles custom VS install dirs ---
for /f "usebackq delims=" %%i in (`"C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath 2^>nul`) do set "VSROOT=%%i"

rem --- 2) fallback: scan the two standard roots, take the newest vcvarsall.bat ---
if defined VSROOT goto :haveit

set "TMPFILE=%TEMP%\vsroot_tmp.txt"
if exist "%TMPFILE%" del "%TMPFILE%"
for %%R in ("C:\Program Files\Microsoft Visual Studio" "C:\Program Files (x86)\Microsoft Visual Studio") do if exist "%%~R" for /f "delims=" %%P in ('dir /b /s "%%~R\vcvarsall.bat" 2^>nul') do echo(%%P>>"%TMPFILE%"
for /f "usebackq delims=" %%B in (`sort /r "%TMPFILE%" 2^>nul`) do if not defined VSROOT set "VSROOT=%%B"
if exist "%TMPFILE%" del "%TMPFILE%"

if not defined VSROOT goto :fail
set "VSROOT=%VSROOT:\VC\Auxiliary\Build\vcvarsall.bat=%"

:haveit
call "%VSROOT%\VC\Auxiliary\Build\vcvarsall.bat" x64
cmd

:fail
echo ERROR: Could not locate vcvarsall.bat. Install the "Desktop development with C++" workload.
exit /b 1
