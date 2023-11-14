cmake_minimum_required(VERSION 3.20)

target_sources(
    ${PROJECT_NAME}
    PRIVATE
    main.cpp
)

add_subdirectory(Board)
add_subdirectory(Protocol)
add_subdirectory(IA)
add_subdirectory(Patterns)
