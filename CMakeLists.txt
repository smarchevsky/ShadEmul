cmake_minimum_required(VERSION 3.16)

project(ShaderEmul LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

FILE(GLOB_RECURSE ALL_HEADERS ${CMAKE_SOURCE_DIR}/*.h ${CMAKE_SOURCE_DIR}/*.hpp)
FILE(GLOB_RECURSE ALL_CPP  "experiments/*.cpp")

add_executable(ShaderEmul ${ALL_HEADERS} ${ALL_CPP} main.cpp)

include_directories(include/)

include(GNUInstallDirs)
install(TARGETS ShaderEmul
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
