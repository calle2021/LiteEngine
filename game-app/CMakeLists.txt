add_executable(Game
    main.cpp
)

target_sources(GameSystem PRIVATE
    game.cpp
    game.h
)

target_link_libraries(Game PRIVATE GameSystem)

if(WIN32)
set_target_properties(Game PROPERTIES
    LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE"
    LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
)
endif()