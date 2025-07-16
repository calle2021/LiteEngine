add_library(ENGINE STATIC)

target_include_directories(ENGINE 
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

target_precompile_headers(ENGINE PRIVATE pch.h)

add_subdirectory(Core)
add_subdirectory(Assets)
add_subdirectory(Include)

find_package (Vulkan REQUIRED)
target_include_directories(ENGINE PRIVATE "${Vulkan_INCLUDE_DIR}")
target_link_libraries(ENGINE PUBLIC Vulkan::Vulkan)

include(FetchContent)
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
