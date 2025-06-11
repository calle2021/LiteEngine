add_executable(GameApp WIN32
    Main.cpp
)

target_sources(GameSystem PRIVATE
    Game.cpp
    Game.h
)

target_link_libraries(GameApp PRIVATE GameSystem)