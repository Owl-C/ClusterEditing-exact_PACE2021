cmake_minimum_required(VERSION 3.5.1)
set(CMAKE_CXX_STANDARD 11)
project (Main CXX)

add_executable(Main main.cpp graph.cpp reduction.cpp solver.cpp)

#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
#find_package(GLPK REQUIRED)
#include_directories(${GLPK_INCLUDE_DIRS})
#target_link_libraries(Main ${GLPK_LIBRARIES})
target_compile_options(Main PUBLIC -Wall)
target_compile_definitions(Main PUBLIC NDEBUG _USE_MATH_DEFINES)