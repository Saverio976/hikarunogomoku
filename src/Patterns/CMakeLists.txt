cmake_minimum_required(VERSION 3.20)

# Transpil patterns from text file to cpp used by the brain
if (WIN32)
    add_custom_command(
        OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/Pattern20.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/Pattern20.hpp"
        COMMAND ${CMAKE_COMMAND} -S "${CMAKE_SOURCE_DIR}/tools/transpattern" -B "${CMAKE_SOURCE_DIR}/tools/transpattern/build"
        COMMAND ${CMAKE_COMMAND} --build "${CMAKE_SOURCE_DIR}/tools/transpattern/build"
        COMMAND "${CMAKE_SOURCE_DIR}/tools/transpattern/transpattern.exe" "20" "${CMAKE_SOURCE_DIR}/patterns" "${CMAKE_SOURCE_DIR}/src/Patterns"
    )
else()
    add_custom_command(
        OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/Pattern20.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/Pattern20.hpp"
        COMMAND ${CMAKE_COMMAND} -S "${CMAKE_SOURCE_DIR}/tools/transpattern" -B "${CMAKE_SOURCE_DIR}/tools/transpattern/build"
        COMMAND ${CMAKE_COMMAND} --build "${CMAKE_SOURCE_DIR}/tools/transpattern/build"
        COMMAND "${CMAKE_SOURCE_DIR}/tools/transpattern/transpattern" "20" "${CMAKE_SOURCE_DIR}/patterns" "${CMAKE_SOURCE_DIR}/src/Patterns"
    )
endif()

add_library(
    transpattern_gen_cpp
    STATIC
)
target_compile_features(
    transpattern_gen_cpp
    PUBLIC
    cxx_std_20
)

target_sources(
    transpattern_gen_cpp
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/Pattern20.cpp"
)

target_include_directories(
    transpattern_gen_cpp
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_include_directories(
    transpattern_gen_cpp
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../400Bits"
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    transpattern_gen_cpp
)
