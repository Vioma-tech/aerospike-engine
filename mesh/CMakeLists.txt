cmake_minimum_required(VERSION 3.11)

set(CMAKE_CXX_FLAGS "-std=c++11 -g")

project(gmshdemo)
add_executable(laval laval.cpp)


#include_directories(${GMSH_INCLUDE_DIR})

add_library(gmsh SHARED IMPORTED)
set_target_properties(gmsh PROPERTIES
    IMPORTED_LOCATION "/usr/lib/x86_64-linux-gnu/libgmsh.so"
    INTERFACE_INCLUDE_DIRECTORIES "/usr/include"
)
set(GMSH_LIBRARIES gmsh)


target_link_libraries(laval ${GMSH_LIBRARIES})

# копируем в корневую директорию .msh файл
add_custom_command(TARGET laval POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_SOURCE_DIR}/build/laval.msh"
        "${CMAKE_SOURCE_DIR}/../laval.msh"
    COMMENT "Copying laval.msh to root directory"
)