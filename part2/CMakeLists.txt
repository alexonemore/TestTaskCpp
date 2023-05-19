# Source code for Test task
# Licensed after GNU GPL v3

cmake_minimum_required(VERSION 3.5)

project(unique_finder LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCL REQUIRED)

add_executable(${PROJECT_NAME}
    main.cpp
    oclworker.h oclworker.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE OpenCL::OpenCL)

install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
