cmake_minimum_required(VERSION 3.20)

set(PROJECT_NAME "pbrain-hikarunogomoku")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)
if (MSVC)
    set(MAKE_CONFIGURATION_TYPES Release)
endif()

project(
    ${PROJECT_NAME}
    VERSION 1.0.0
    LANGUAGES CXX
)

add_executable(${PROJECT_NAME})

add_subdirectory(src)

# Copy executable binary of the brain to the root repo
if (WIN32)
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:${PROJECT_NAME}>
        ${CMAKE_SOURCE_DIR}/${PROJECT_NAME_CLIENT}.exe
    )
else()
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:${PROJECT_NAME}>
        ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}
    )
endif()
