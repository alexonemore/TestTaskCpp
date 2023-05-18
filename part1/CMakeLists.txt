# Source code for Test task
# Licensed after GNU GPL v3

cmake_minimum_required(VERSION 3.5)

project(data_compressor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
    main.cpp
    compressor.h compressor.cpp
    huffmantree.h huffmantree.cpp
)

install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
