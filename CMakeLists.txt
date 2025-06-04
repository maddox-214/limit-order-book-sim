cmake_minimum_required(VERSION 3.10)
project(LimitOrderBook)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# All source files
file(GLOB SOURCES "src/*.cpp")

add_executable(LOB_Simulator ${SOURCES})

# (Optional) If you want to enable compiler warnings:
if (MSVC)
  target_compile_options(LOB_Simulator PRIVATE /W4 /permissive-)
else()
  target_compile_options(LOB_Simulator PRIVATE -Wall -Wextra -pedantic)
endif()
