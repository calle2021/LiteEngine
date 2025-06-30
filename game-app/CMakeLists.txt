set(GAME "Game")
add_executable(GAME
    main.cpp
)

target_sources(GAME PRIVATE
    game.cpp
    game.h
)

target_link_libraries(GAME PRIVATE ENGINE)

if(WIN32)
set_target_properties(GAME PROPERTIES
    LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE"
    LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
)
endif()

add_custom_command(TARGET GAME POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_BINARY_DIR}/assets
    $<TARGET_FILE_DIR:GAME>/assets
)
