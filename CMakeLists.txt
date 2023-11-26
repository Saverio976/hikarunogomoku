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
