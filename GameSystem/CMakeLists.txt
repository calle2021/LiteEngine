add_library(GameSystem STATIC)

if(WIN32)
target_compile_definitions(GameSystem PRIVATE WINDOWS)
elseif(UNIX)
target_compile_definitions(GameSystem PRIVATE LINUX)
endif()

target_include_directories(GameSystem PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Core
)

target_precompile_headers(GameSystem PRIVATE pch.h)

add_subdirectory(Core)
add_subdirectory(External/spdlog)
add_subdirectory(External/glfw)

target_link_libraries(GameSystem PUBLIC glfw)
target_link_libraries(GameSystem PUBLIC spdlog::spdlog_header_only)