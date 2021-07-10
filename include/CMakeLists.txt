cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(ECS)

# include_directories(root)

message(STATUS "[INFO] Searching for header directories...")
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "[INFO] Found ${dir}")
endforeach()
