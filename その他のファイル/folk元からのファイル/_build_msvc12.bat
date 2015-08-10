@echo off

rem set MASM_PATH=d:\dosbin\
set NASM_PATH=d:\dosbin

set PATH=%PATH%;%NASM_PATH%

set EXTRA_CPPFLAGS=/FS

call "%VS120COMNTOOLS%vsvars32.bat" x86

jom /version >NUL 2>&1

if "%ERRORLEVEL%" == "0" (
	jom
) else (
	nmake
)

pause
