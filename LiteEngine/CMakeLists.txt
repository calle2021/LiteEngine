add_library(LiteEngine STATIC)

target_include_directories(LiteEngine
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

add_subdirectory(Core)
add_subdirectory(Assets)
add_subdirectory(Include)

set_target_properties(LiteEngine PROPERTIES
    LITE_ENGINE_ASSETS_DIR "${CMAKE_CURRENT_BINARY_DIR}/Assets"
)

find_package (Vulkan REQUIRED)
target_include_directories(LiteEngine PRIVATE "${Vulkan_INCLUDE_DIR}")
target_link_libraries(LiteEngine PUBLIC Vulkan::Vulkan)
target_compile_definitions(LiteEngine
        PUBLIC VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1 VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
)

include(FetchContent)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.x
)
FetchContent_MakeAvailable(spdlog)
target_link_libraries(LiteEngine PRIVATE spdlog::spdlog_header_only)

FetchContent_Declare(
	glfw
	GIT_REPOSITORY	git@github.com:glfw/glfw.git
    GIT_TAG master
)
FetchContent_MakeAvailable(glfw)
target_link_libraries(LiteEngine PUBLIC glfw)

FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
    GIT_TAG master
)
FetchContent_MakeAvailable(glm)
target_link_libraries(LiteEngine PUBLIC glm::glm)
