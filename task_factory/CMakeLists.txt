cmake_minimum_required(VERSION 3.25)
project(FactorySimulation)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

file(GLOB_RECURSE SOURCES "src/*.cpp" "main.cpp")
file(GLOB_RECURSE HEADERS "include/*.hpp")

add_executable(FactorySimulation ${SOURCES} ${HEADERS})