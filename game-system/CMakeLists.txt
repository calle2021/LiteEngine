add_library(GameSystem STATIC)

if(WIN32)
target_compile_definitions(GameSystem PRIVATE WINDOWS)
elseif(UNIX)
target_compile_definitions(GameSystem PRIVATE LINUX)
endif()

target_include_directories(GameSystem PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/core
)

target_precompile_headers(GameSystem PRIVATE pch.h)

add_subdirectory(core)
add_subdirectory(external/spdlog)
add_subdirectory(external/glfw)

target_link_libraries(GameSystem PUBLIC glfw)
target_link_libraries(GameSystem PUBLIC spdlog::spdlog_header_only)
find_package(Vulkan REQUIRED)
target_link_libraries(GameSystem PRIVATE Vulkan::Vulkan)