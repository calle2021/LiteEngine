add_library(ENGINE STATIC)

target_include_directories(ENGINE 
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

target_precompile_headers(ENGINE PRIVATE pch.h)

add_subdirectory(Core)
add_subdirectory(Assets)
add_subdirectory(Include)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")

find_package (Vulkan REQUIRED)

# set up Vulkan C++ module
add_library(VulkanCppModule)
add_library(Vulkan::cppm ALIAS VulkanCppModule)

target_compile_definitions(VulkanCppModule
        PUBLIC VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1 VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
)

target_include_directories(VulkanCppModule
        PRIVATE
        "${Vulkan_INCLUDE_DIR}"
)
target_link_libraries(VulkanCppModule
        PUBLIC
        Vulkan::Vulkan
)

set_target_properties(VulkanCppModule PROPERTIES CXX_STANDARD 20)

target_sources(VulkanCppModule
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        BASE_DIRS
        "${Vulkan_INCLUDE_DIR}"
        FILES
        "${Vulkan_INCLUDE_DIR}/vulkan/vulkan.cppm"
)
target_link_libraries(ENGINE PUBLIC Vulkan::cppm)

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
