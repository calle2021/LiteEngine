cmake_minimum_required(VERSION 3.20)
project(MyWorkspace LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(game-system)
add_subdirectory(game-app)
