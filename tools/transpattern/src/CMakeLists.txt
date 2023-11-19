cmake_minimum_required(VERSION 3.20)

target_sources(
    ${PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/400Bits/Bits.cpp"
)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/400Bits"
)
