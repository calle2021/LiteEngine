set(GAME "Game")
add_executable(GAME
    Main.cpp
)

target_sources(GAME PRIVATE
    game.cpp
    game.h
)

target_link_libraries(GAME PRIVATE ENGINE)

set_target_properties(GAME PROPERTIES
    LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE"
    LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
)