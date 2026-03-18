@echo off

set TOOL_DIR=%~dp0tools\xpack-openocd-0.12.0-7-win32-x64\xpack-openocd-0.12.0-7

set OPENOCD=%TOOL_DIR%\bin\openocd.exe
set SCRIPTS=%TOOL_DIR%\scripts

%OPENOCD% -s %SCRIPTS% ^
-f interface/stlink.cfg ^
-f target/stm32f1x.cfg ^
-c "program Build_out/SimpleSched.hex verify reset exit"

pause