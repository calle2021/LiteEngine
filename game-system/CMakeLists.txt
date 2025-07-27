add_library(ENGINE STATIC)

target_include_directories(ENGINE
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

target_precompile_headers(ENGINE PRIVATE pch.h)

add_subdirectory(Core)
add_subdirectory(Assets)
add_subdirectory(Include)

set_target_properties(ENGINE PROPERTIES
    ENGINE_ASSETS_DIR "${CMAKE_CURRENT_BINARY_DIR}/Assets"
)

find_package (Vulkan REQUIRED)
target_include_directories(ENGINE PRIVATE "${Vulkan_INCLUDE_DIR}")
target_link_libraries(ENGINE PUBLIC Vulkan::Vulkan)
target_compile_definitions(ENGINE
        PUBLIC VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1 VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
)

include(FetchContent)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.x
)
FetchContent_MakeAvailable(spdlog)
target_link_libraries(ENGINE PRIVATE spdlog::spdlog_header_only)

FetchContent_Declare(
	glfw
	GIT_REPOSITORY	git@github.com:glfw/glfw.git
    GIT_TAG master
)
FetchContent_MakeAvailable(glfw)
target_link_libraries(ENGINE PUBLIC glfw)

FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
    GIT_TAG master
)
FetchContent_MakeAvailable(glm)
target_link_libraries(ENGINE PUBLIC glm::glm)
