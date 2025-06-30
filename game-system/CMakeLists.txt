add_library(ENGINE STATIC)

target_include_directories(ENGINE 
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

target_precompile_headers(ENGINE PRIVATE pch.h)

add_subdirectory(core)
add_subdirectory(assets)
add_subdirectory(Include)

find_package(Vulkan REQUIRED)
target_link_libraries(ENGINE PRIVATE Vulkan::Vulkan)

include(FetchContent)
FetchContent_Declare(
	spdlog
	GIT_REPOSITORY	git@github.com:gabime/spdlog.git
    GIT_TAG v1.x
)
FetchContent_MakeAvailable(spdlog)
target_link_libraries(ENGINE PUBLIC spdlog::spdlog)


FetchContent_Declare(
	glfw
	GIT_REPOSITORY	git@github.com:glfw/glfw.git
    GIT_TAG master
)
FetchContent_MakeAvailable(glfw)
target_link_libraries(ENGINE PRIVATE glfw)


FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
    GIT_TAG master
)
FetchContent_MakeAvailable(glm)
target_link_libraries(ENGINE PRIVATE glm::glm)
