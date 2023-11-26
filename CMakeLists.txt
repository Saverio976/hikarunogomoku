cmake_minimum_required(VERSION 3.20)

set(PROJECT_NAME "pbrain-hikarunogomoku")
option(WITH_STAT "add statistics for program run" OFF)
option(BUILD_STATIC "build static library" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "" OR CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_BUILD_TYPE Release)
    if (MSVC)
        set(MAKE_CONFIGURATION_TYPES Release)
    endif()
endif()

project(
    ${PROJECT_NAME}
    VERSION 1.0.0
    LANGUAGES CXX
)

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-funroll-loops HAS_UNROLL_LOOPS)
if (HAS_UNROLL_LOOPS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -funroll-loops")
    message(STATUS "unroll-loops")
endif()
check_cxx_compiler_flag(-march=native HAS_MARCH_NATIVE)
if (HAS_MARCH_NATIVE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
    message(STATUS "march=native")
endif()
check_cxx_compiler_flag(-mfpmath=sse HAS_FPMATH_SSE)
if (HAS_FPMATH_SSE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpmath=sse")
    message(STATUS "mfpmath=sse")
endif()
check_cxx_compiler_flag(-ftree-vectorize HAS_FTREE_VECTORIZE)
if (HAS_FTREE_VECTORIZE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftree-vectorize")
    message(STATUS "ftree-vectorize")
endif()
check_cxx_compiler_flag("/GL" HAS_GL)
if (HAS_GL)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GL")
    message(STATUS "/GL")
endif()
check_cxx_compiler_flag("/Ot" HAS_OT)
if (HAS_OT)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /OT")
    message(STATUS "/OT")
endif()
check_cxx_compiler_flag("-mllvm" HAS_MLLVM)
if (HAS_MLLVM)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mllvm")
    message(STATUS "mllvm")
endif()
check_cxx_compiler_flag("-polly" HAS_POLLY)
if (HAS_POLLY)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -polly")
    message(STATUS "polly")
endif()
check_cxx_compiler_flag("-O3" HAS_O3)
if (HAS_O3)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
    message(STATUS "O3")
endif()


if (BUILD_STATIC)
    if (MSVC)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    else()
        set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
        set(BUILD_SHARED_LIBS OFF)
        set(CMAKE_EXE_LINKER_FLAGS "-static")
    endif()
endif()


add_executable(${PROJECT_NAME})

if (WITH_STAT)
    target_compile_definitions(${PROJECT_NAME} PUBLIC WITH_STAT)
endif()

add_subdirectory(src)

if (WIN32)
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:${PROJECT_NAME}>
        ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}.exe
    )
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}.exe
        ${CMAKE_SOURCE_DIR}/pbrain-gomoku-ai.exe
    )
else()
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:${PROJECT_NAME}>
        ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}
    )
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}
        ${CMAKE_SOURCE_DIR}/pbrain-gomoku-ai
    )
endif()
