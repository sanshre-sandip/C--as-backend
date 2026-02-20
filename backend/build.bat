@echo off
set VCVARS="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
set MYSQL_INC="C:\Program Files\MySQL\MySQL Server 8.4\include"
set MYSQL_LIB="C:\Program Files\MySQL\MySQL Server 8.4\lib"

call %VCVARS%

echo Compiling...
cl server.c ^
    /I %MYSQL_INC% ^
    /link /LIBPATH:%MYSQL_LIB% libmysql.lib ws2_32.lib ^
    /out:server.exe

if %errorlevel% neq 0 (
    echo Compilation failed!
    exit /b %errorlevel%
)

echo Compilation successful! Run server.exe to start.
