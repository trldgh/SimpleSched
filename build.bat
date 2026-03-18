@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION

::=========================Record compile start time============================
set T1=%time: =0%
echo Start time: %T1%
set /A hour_1 = %T1:~0,1%*10 + %T1:~1,1%
set /A min_1  = %T1:~3,1%*10 + %T1:~4,1%
set /A sec_1  = %T1:~6,1%*10 + %T1:~7,1%
set /A msec_1 = %T1:~9,1%*10 + %T1:~10,1%

::============================== 编译配置 ================================
set BUILD_DIR=Build_out
set PROJECT_NAME=SimpleSched

:: 【修改】使用项目内的工具链文件夹
:: 使用相对路径找到 tools
set TOOLCHAIN_FOLDER=%~dp0tools
set PATH=%TOOLCHAIN_FOLDER%\gcc-arm-none-eabi\bin;%TOOLCHAIN_FOLDER%\cmake\bin;%TOOLCHAIN_FOLDER%\ninja;%PATH%

:: 验证工具链
echo Checking ARM toolchain...
arm-none-eabi-gcc --version
if %errorlevel% neq 0 (
    echo ERROR: ARM toolchain not found in %TOOLCHAIN_FOLDER%
    echo Please copy ARM toolchain to %TOOLCHAIN_FOLDER%\gcc-arm-none-eabi\
    pause
    exit /b 1
)


::============================== 清理旧编译 ================================
ECHO.
ECHO ==========================================
ECHO         Building %PROJECT_NAME% Start...
ECHO ==========================================
ECHO.

::创建输出目录
if not exist %BUILD_DIR% (
    md %BUILD_DIR%
) else (
    echo Clean old build files...
    if exist %BUILD_DIR%\* del /Q %BUILD_DIR%\*
)

::进入编译目录
cd %BUILD_DIR%

::============================== 执行CMake ================================
echo Configuring with CMake...

:: 使用工具链文件配置
cmake -G "Ninja" ..\Application ^
    -DCMAKE_TOOLCHAIN_FILE=..\arm-gcc-toolchain.cmake ^
    -DPROJECT_NAME=%PROJECT_NAME% ^
    -DCMAKE_BUILD_TYPE=Release

::============================== 编译 ================================
echo Building...
cmake --build .

::============================== 检查结果 ================================
cd ..

if exist %BUILD_DIR%\%PROJECT_NAME%.bin (
    echo ==========================================
    echo         Build %PROJECT_NAME% Success!
    echo         Output: %BUILD_DIR%\%PROJECT_NAME%.bin
    echo ==========================================
    set compile_state=PASS
) else (
    echo ==========================================
    echo         Build %PROJECT_NAME% Failed!
    echo ==========================================
    set compile_state=FAILED
)

::============================== 计算编译时间 ================================
set T2=%time: =0%
echo End time: %T2%
set /A hour_2 = %T2:~0,1%*10 + %T2:~1,1%
set /A min_2  = %T2:~3,1%*10 + %T2:~4,1%
set /A sec_2  = %T2:~6,1%*10 + %T2:~7,1%
set /A msec_2 = %T2:~9,1%*10 + %T2:~10,1%

set /A T1_sec =%hour_1%*3600 + %min_1%*60 + %sec_1%
set /A T2_sec =%hour_2%*3600 + %min_2%*60 + %sec_2%
set /A all_sec =%T2_sec% - %T1_sec%
if %all_sec% LSS 0 set /A all_sec+=86400

set /A all_msec=%msec_2%-%msec_1%
if %all_msec% LSS 0 set /A all_msec+=100

::============================== 打印结果 ================================
ECHO.
ECHO ****************************************************
ECHO *  SimpleSched Build System
ECHO *  Project     : %PROJECT_NAME%
ECHO *  Build Time  : %all_sec%.%all_msec% s
ECHO *  Output      : %BUILD_DIR%\%PROJECT_NAME%.bin
ECHO *  Result      : %compile_state%
ECHO ****************************************************

pause