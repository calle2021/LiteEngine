cmake_minimum_required(VERSION 3.28)
project(LiteDev LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(/Zc:__cplusplus)

add_subdirectory(LiteEngine)
add_subdirectory(LiteBox)
