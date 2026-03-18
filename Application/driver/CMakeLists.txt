# # ============================================================================
# # 驱动层 - 硬件驱动相关源文件
# # ============================================================================

# # 打印信息
# message(STATUS "Configuring driver layer...")

# # 顶层CMakeLists已经直接glob了源文件
# # 如果需要为driver层单独设置编译选项，可以在这里添加

# # 例如：为driver目录添加特定的编译选项
# target_compile_options(${PROJECT_NAME} PRIVATE
#     $<$<COMPILE_LANGUAGE:C>:${CMAKE_C_FLAGS}>
# )