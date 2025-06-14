add_executable(GameApp
    main.cpp
)

target_sources(GameSystem PRIVATE
    game.cpp
    game.h
)

target_link_libraries(GameApp PRIVATE GameSystem)

if(WIN32)
set_target_properties(GameApp PROPERTIES
    LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE"
    LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
)
endif()