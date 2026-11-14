cmake_minimum_required(VERSION 3.15)
project(CrossFactionResFix LANGUAGES C)

if(NOT WIN32 OR NOT MSVC)
    message(FATAL_ERROR "Use a Visual Studio MSVC toolchain targeting Windows.")
endif()
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "CrossFactionResFix requires x86. Configure with -A Win32.")
endif()

add_library(CrossFactionResFix SHARED src/CrossFactionResFix.c)
target_compile_options(CrossFactionResFix PRIVATE /W4)
target_link_libraries(CrossFactionResFix PRIVATE kernel32)
set_target_properties(CrossFactionResFix PROPERTIES
    PREFIX ""
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
