set(SHADER_SLANG_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Shaders/shader.slang
)

find_program(SLANGC_EXECUTABLE slangc HINTS $ENV{VULKAN_SDK}/bin)

function (add_slang_shader_target TARGET)
    cmake_parse_arguments(SHADER "" "" "SOURCES" ${ARGN})
    set (SHADERS_DIR ${CMAKE_CURRENT_BINARY_DIR}/Shaders)
    set (ENTRY_POINTS -entry vertMain -entry fragMain)
    set(OUTPUT_FILE ${SHADERS_DIR}/slang.spv)
    add_custom_command (
        OUTPUT  ${OUTPUT_FILE}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADERS_DIR}
        COMMAND ${SLANGC_EXECUTABLE} ${SHADER_SOURCES} -target spirv -profile spirv_1_4 -emit-spirv-directly -fvk-use-entrypoint-name ${ENTRY_POINTS} -o ${OUTPUT_FILE}
        DEPENDS ${SHADER_SOURCES}
        COMMENT "Compiling Slang Shaders"
        VERBATIM
    )
    add_custom_target (${TARGET} DEPENDS ${OUTPUT_FILE})
endfunction()

add_slang_shader_target(SHADERS SOURCES ${SHADER_SLANG_SOURCES})
add_dependencies(ENGINE SHADERS)