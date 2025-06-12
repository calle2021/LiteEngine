add_executable(GameApp
    Main.cpp
)

target_sources(GameSystem PRIVATE
    Game.cpp
    Game.h
)

target_link_libraries(GameApp PRIVATE GameSystem)

if(WIN32)
set_target_properties(GameApp PROPERTIES
    LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE"
    LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
)
endif()