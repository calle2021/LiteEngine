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

include(FetchContent)

target_link_libraries(GameSystem PRIVATE glfw)
target_link_libraries(GameSystem PUBLIC spdlog::spdlog_header_only)
find_package(Vulkan REQUIRED)
target_link_libraries(GameSystem PRIVATE Vulkan::Vulkan)

FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	2d4c4b4dd31fde06cfffad7915c2b3006402322f
)
FetchContent_MakeAvailable(glm)
target_link_libraries(GameSystem PRIVATE glm::glm)
