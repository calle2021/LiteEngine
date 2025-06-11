add_library(GameSystem STATIC)

target_include_directories(GameSystem PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Core
)

target_precompile_headers(GameSystem PRIVATE pch.h)

add_subdirectory(External/spdlog)
target_link_libraries(GameSystem PUBLIC spdlog::spdlog_header_only)

add_subdirectory(Core)