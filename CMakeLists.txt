cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

set(PROGRAM_NAME ECS)
project(ECS)

###################################################################################################

set(CMAKE_CXX_FLAGS "-Wall -g -pedantic -pthread")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_COLOR_MAKEFILE True)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

###################################################################################################

include_directories(${PROJECT_SOURCE_DIR}/include)
message(STATUS "[INFO] Searching for header directories...")
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
	message(STATUS "[INFO] Found ${dir}")
endforeach()

file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(${PROGRAM_NAME} ${SOURCES} ${PROJECT_SOURCE_DIR}/main.cpp)
target_link_libraries(${PROGRAM_NAME} PUBLIC m)

###################################################################################################
