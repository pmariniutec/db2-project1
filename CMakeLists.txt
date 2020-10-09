cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(dbms VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SRC_DIR "src/")

file(GLOB_RECURSE HEADER_SRC CONFIGURE_DEPENDS "${SRC_DIR}/*.h")
file(GLOB_RECURSE SOURCE_SRC CONFIGURE_DEPENDS "${SRC_DIR}/*.cpp")

add_executable(dbms ${HEADER_SRC} ${SOURCE_SRC})
set_target_properties(dbms PROPERTIES CXX_STANDARD 17)
target_include_directories(dbms PUBLIC ${SRC_DIR})
