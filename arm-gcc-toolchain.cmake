# 指定目标系统名称为通用类型（用于裸机嵌入式系统）
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# 指定编译器
set(CMAKE_C_COMPILER "arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "arm-none-eabi-g++")
set(CMAKE_ASM_COMPILER "arm-none-eabi-gcc")
set(CMAKE_AR "arm-none-eabi-ar")
set(CMAKE_OBJCOPY "arm-none-eabi-objcopy")
set(CMAKE_OBJDUMP "arm-none-eabi-objdump")
set(CMAKE_SIZE "arm-none-eabi-size")

# 【关键】设置CMake在尝试编译测试程序时只生成静态库，避免链接失败
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

# 禁用默认启动文件和标准库的链接（适合裸机开发）
set(CMAKE_EXE_LINKER_FLAGS_INIT "-nostartfiles -nodefaultlibs")

# 交叉编译搜索策略设置
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)