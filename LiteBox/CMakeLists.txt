
add_executable("LiteBox" Main.cpp)

target_sources(LiteBox PRIVATE
    LiteBox.cpp
    LiteBox.h
)

target_link_libraries(LiteBox PRIVATE LiteEngine)

set_target_properties(LiteBox PROPERTIES
    LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE"
    LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
)

add_custom_command(
    TARGET LiteBox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/Assets
            $<TARGET_FILE_DIR:LiteBox>/Assets
)

get_target_property(LITE_ENGINE_SHADER_DIR LiteEngine LITE_ENGINE_SHADER_DIR)

add_custom_command(
  TARGET LiteBox POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${LITE_ENGINE_SHADER_DIR}
          $<TARGET_FILE_DIR:LiteBox>/Assets/Shaders
)