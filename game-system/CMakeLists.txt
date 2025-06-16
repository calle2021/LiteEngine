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

find_package(Vulkan REQUIRED)
target_link_libraries(GameSystem PRIVATE Vulkan::Vulkan)

include(FetchContent)
FetchContent_Declare(
	spdlog
	GIT_REPOSITORY	git@github.com:gabime/spdlog.git
    GIT_TAG v1.x
)
FetchContent_MakeAvailable(spdlog)
target_link_libraries(GameSystem PUBLIC spdlog::spdlog)


FetchContent_Declare(
	glfw
	GIT_REPOSITORY	git@github.com:glfw/glfw.git
    GIT_TAG master
)
FetchContent_MakeAvailable(glfw)
target_link_libraries(GameSystem PRIVATE glfw)


FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
    GIT_TAG master
)
FetchContent_MakeAvailable(glm)
target_link_libraries(GameSystem PRIVATE glm::glm)
