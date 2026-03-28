cmake_minimum_required(VERSION 3.10)
project(SimpleSched)

# 让CMake生成compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 屏蔽错误选项
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

# ARM核心类型
set(ARM_CORE_TYPE cortex-m3)

add_definitions(
    -DSTM32F10X_MD
    -DUSE_STDPERIPH_DRIVER
)

# 编译选项
set(MCU_FLAGS "-mcpu=${ARM_CORE_TYPE} -mthumb")
set(CMAKE_C_FLAGS "${MCU_FLAGS} -Wall -Wno-unknown-pragmas -ffunction-sections -fdata-sections -fno-strict-aliasing -g")
set(CMAKE_C_FLAGS_RELEASE "-O2")
set(CMAKE_BUILD_TYPE "Release")

# 启用汇编语言支持
ENABLE_LANGUAGE(ASM)

# 链接脚本（可选，如果不提供则使用工具链默认的）
set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/linker/stm32f103.ld)
set(CMAKE_EXE_LINKER_FLAGS
    "-T${LINKER_SCRIPT} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map -Wl,--gc-sections -specs=nosys.specs"
)
# 【关键】链接选项 - 使用 nosys.specs 提供系统调用
# set(CMAKE_EXE_LINKER_FLAGS 
#     "-specs=nosys.specs -lc -lm -lnosys -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map,--cref -Wl,--gc-sections"
# )

# 头文件路径
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/driver
    ${CMAKE_CURRENT_SOURCE_DIR}/library/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/user/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/start
    ${CMAKE_CURRENT_SOURCE_DIR}/system
    ${CMAKE_CURRENT_SOURCE_DIR}/common/inc
)

# 收集所有源文件
file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/driver/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/library/src/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/user/src/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/start/*.s
    ${CMAKE_CURRENT_SOURCE_DIR}/start/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/system/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/*.c
)

# 创建可执行文件
add_executable(${PROJECT_NAME}.elf ${SOURCES})

# 生成hex、bin和lst文件
set(ELF_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.elf)
set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)
set(LST_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.lst)

add_custom_command(TARGET "${PROJECT_NAME}.elf" POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -Obinary ${ELF_FILE} ${BIN_FILE}
    COMMAND ${CMAKE_OBJCOPY} -Oihex ${ELF_FILE} ${HEX_FILE}
    COMMAND ${CMAKE_OBJDUMP} -S ${ELF_FILE} > ${LST_FILE}
    COMMAND ${CMAKE_SIZE} --format=berkeley ${PROJECT_NAME}.elf
    COMMENT "Generating binary, hex, and list files"
)